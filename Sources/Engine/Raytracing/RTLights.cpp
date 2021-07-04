/* Copyright (c) 2020-2021 Sultim Tsyrendashiev
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

#include "StdH.h"

#include <Engine/Entities/Entity.h>
#include <Engine/Models/RenderModel.h>
#include <Engine/Models/ModelObject.h>
#include <Engine/Models/ModelData.h>
#include <Engine/Light/LightSource.h>
#include <Engine/World/World.h>
#include <Engine/World/WorldRayCasting.h>

#include <Engine/Base/ListIterator.inl>

#include "SSRT.h"
#include "RTProcessing.h"
#include "SSRTGlobals.h"



extern SSRT::SSRTGlobals _srtGlobals;



static CStaticStackArray<const CLightSource *> RT_IgnoredLights;
static CStaticStackArray<CEntity *> RT_PotentialLights;



template<class Type>
inline Type SmoothStep(const Type edge0, const Type edge1, const Type x)
{
  Type t = Clamp((x - edge0) / (edge1 - edge0), (Type)0.0, (Type)1.0);
  return t * t * (3.0 - 2.0 * t);
}


static bool RT_IsModelPlayer(CEntity *pEn)
{
  return IsOfClass(pEn, "Player");
}


static void RT_AdjustSaturation(FLOAT3D &color, float saturation)
{
  const float luminance = color % FLOAT3D(0.2125f, 0.7154f, 0.0721f);

  for (int i = 1; i <= 3; i++)
  {
    color(i) = Lerp(luminance, color(i), saturation);
  }
}


static void RT_AdjustPow(FLOAT3D &color, float power)
{
  for (int i = 1; i <= 3; i++)
  {
    color(i) = Min(powf(color(i), power), 1.0f);
  }
}


static FLOAT3D GetDirectionalLightColor(const CLightSource *plsLight)
{
  UBYTE r, g, b;
  plsLight->GetLightColor(r, g, b);
  FLOAT3D color = { r / 255.0f, g / 255.0f, b / 255.0f };

  RT_AdjustSaturation(color, _srtGlobals.srt_fLightDirectionalSaturation);
  RT_AdjustPow(color, _srtGlobals.srt_fLightDirectionalColorPow);
  color *= _srtGlobals.srt_fLightDirectionalIntensityMultiplier;

  return color;
}


static FLOAT3D GetSphericalLightColor(const CLightSource *plsLight)
{
  UBYTE r, g, b;
  plsLight->GetLightColor(r, g, b);
  FLOAT3D color = { r / 255.0f, g / 255.0f, b / 255.0f };

  RT_AdjustSaturation(color, _srtGlobals.srt_fLightSphericalSaturation);
  RT_AdjustPow(color, _srtGlobals.srt_fLightSphericalColorPow);
  color *= _srtGlobals.srt_fLightSphericalIntensityMultiplier;

  return color;
}


// Offset lights from the nearest polygon, otherwise 
// there will be no light's contribution on that polygon
static FLOAT3D RT_FixSphericalLightPosition(CEntity *pEn, const FLOAT3D &position, float offset, CWorld *pWorld)
{
  if (offset > 0.0f)
  {
    FLOAT3D point;
    FLOATplane3D plane;
    FLOAT distanceToEdge;
    if (pEn->GetNearestPolygon(point, plane, distanceToEdge) != nullptr)
    {
      FLOAT3D d = point - position;

      // if light is too close to the plane, adjust the position
      if (d % d < offset * offset)
      {
        FLOAT3D normal = (FLOAT3D &)plane;
        FLOAT3D targetPos = position + normal * offset;

        CCastRay crRay(pEn, position, targetPos);
        crRay.cr_ttHitModels = CCastRay::TT_NONE;     // only brushes block the damage
        crRay.cr_bHitTranslucentPortals = FALSE;
        crRay.cr_bPhysical = TRUE;
        pWorld->CastRay(crRay);

        if (crRay.cr_penHit != nullptr)
        {
          // if found intersection, use middle point
          return (crRay.cr_vHit + position) * 0.5f;
        }
        else
        {
          // if ray wasn't intersected, use target position
          return targetPos;
        }
      }
    }
  }

  return position;
}


static FLOAT3D RT_FindMuzzleFlashPosition(CEntity *pEn, 
                                          const FLOAT3D &basePosition, 
                                          const FLOAT3D &forward, 
                                          SSRT::Scene *pScene)
{
  float eps = 1.0f;
  FLOAT3D muzzlePos = basePosition - forward * _srtGlobals.srt_fLightMuzzleOffset;
  FLOAT3D rayDstPos = basePosition - forward * (_srtGlobals.srt_fLightMuzzleOffset + eps);

  CCastRay crRay(pEn, basePosition, rayDstPos);
  crRay.cr_ttHitModels = _srtGlobals.srt_bLightFixWithModels ? CCastRay::TT_COLLISIONBOX : CCastRay::TT_NONE;
  crRay.cr_bHitTranslucentPortals = _srtGlobals.srt_bLightFixWithTranslucent;
  crRay.cr_bPhysical = TRUE;
  pScene->GetWorld()->CastRay(crRay);

  // if found intersection
  if (crRay.cr_penHit != nullptr && crRay.cr_pbpoBrushPolygon != nullptr)
  {
    const FLOAT3D &normal = (const FLOAT3D &)crRay.cr_pbpoBrushPolygon->bpo_pbplPlane->bpl_plAbsolute;

    return crRay.cr_vHit + normal * _srtGlobals.srt_fLightSphericalPolygonOffset;
  }
  else
  {
    return muzzlePos;
  }
}


#define SPOTLIGHT_NEAR_FIX_DIST_MIN 2.0f
#define SPOTLIGHT_NEAR_FIX_DIST_MAX 10.0f

#define SPOTLIGHT_START_THRESHOLD 0.05f


static void RT_FixSpotlightPosition(CEntity *pEn,
                                    SSRT::Scene *pScene,
                                    const FLOAT3D &direction,
                                    FLOAT3D &refStartPosition,
                                    FLOAT3D &refEndPosition)
{
  {
    float len = (refStartPosition - pScene->GetCameraPosition()).Length();

    if (len < SPOTLIGHT_START_THRESHOLD)
    {
      refStartPosition = pScene->GetCameraPosition();
    }
    else
    {
      CCastRay crRay(pEn, pScene->GetCameraPosition(), refStartPosition);
      crRay.cr_ttHitModels = CCastRay::TT_NONE;
      crRay.cr_bHitTranslucentPortals = _srtGlobals.srt_bLightFixWithTranslucent;
      crRay.cr_bPhysical = TRUE;
      pScene->GetWorld()->CastRay(crRay);

      if (crRay.cr_penHit != nullptr && crRay.cr_pbpoBrushPolygon != nullptr)
      {
        refStartPosition = Lerp(pScene->GetCameraPosition(), refStartPosition, Clamp(crRay.cr_fHitDistance / len, SPOTLIGHT_START_THRESHOLD, 1.0f));
      }
    }
  }

  {
    CCastRay crRay(pEn, pScene->GetCameraPosition(), pScene->GetCameraPosition() + direction * SPOTLIGHT_NEAR_FIX_DIST_MAX);
    crRay.cr_ttHitModels = _srtGlobals.srt_bLightFixWithModels ? CCastRay::TT_COLLISIONBOX : CCastRay::TT_NONE;
    crRay.cr_bHitTranslucentPortals = _srtGlobals.srt_bLightFixWithTranslucent;
    crRay.cr_bPhysical = TRUE;
    pScene->GetWorld()->CastRay(crRay);

    FLOAT3D defaultEndPos = pScene->GetCameraPosition() + direction * _srtGlobals.srt_fSpotlightFalloffDistance;

    // if found intersection
    if (crRay.cr_penHit != nullptr)
    {
      refEndPosition = Lerp(crRay.cr_vHit,
                            defaultEndPos,
                            SmoothStep(SPOTLIGHT_NEAR_FIX_DIST_MIN, SPOTLIGHT_NEAR_FIX_DIST_MAX, crRay.cr_fHitDistance));
    }
    else
    {
      refEndPosition = defaultEndPos;
    }
  }
}


static FLOAT3D RT_FindMuzzleFlashPositionForFirstPerson(CEntity *pEn, SSRT::Scene *pScene)
{
  if (_srtGlobals.srt_fLightMuzzleOffset < 0.01f)
  {
    return pScene->GetCameraPosition();
  }

  // TODO: find muzzle flash from the weapon model, not from camera position
  const FLOAT3D &cameraPos = pScene->GetCameraPosition();

  const FLOATmatrix3D &rot = pScene->GetCameraRotation();
  FLOAT3D forward = { rot(1, 3), rot(2, 3), rot(3, 3) };

  return RT_FindMuzzleFlashPosition(pEn, cameraPos, forward, pScene);
}


static FLOAT3D RT_FindMuzzleFlashPositionForPlayerAABB(CEntity *pEn, SSRT::Scene *pScene)
{
  FLOATaabbox3D aabb;
  pEn->GetBoundingBox(aabb);

  const FLOATmatrix3D &rot = pEn->GetRotationMatrix();
  FLOAT3D forward = { rot(1, 3), rot(2, 3), rot(3, 3) };

  return RT_FindMuzzleFlashPosition(pEn, aabb.Center(), forward, pScene);
}


static void RT_AddModifiedSphereLightToScene(ULONG entityID,
                                             const FLOAT3D &position,
                                             const FLOAT3D &color,
                                             float hotspotDistance,
                                             float falloffDistance,
                                             bool isDynamic,
                                             bool isMuzzleFlash,
                                             SSRT::Scene *pScene)
{
  SSRT::CSphereLight light = {};
  light.entityID = entityID;
  light.absPosition = position;
  light.color = color;

  if (isDynamic)
  {
    light.radius = _srtGlobals.srt_fLightSphericalRadiusOfDynamic;

    if (isMuzzleFlash)
    {
      light.radius *= 0.1f;
    }

    float f = Clamp(hotspotDistance + falloffDistance,
                    _srtGlobals.srt_fLightSphericalFalloffOfDynamicMin,
                    _srtGlobals.srt_fLightSphericalFalloffOfDynamicMax);
    light.falloffDistance = f * _srtGlobals.srt_fLightSphericalFalloffOfDynamicMultiplier;
  }
  else
  {
    light.radius = hotspotDistance * _srtGlobals.srt_fLightSphericalRadiusMultiplier;

    float f = hotspotDistance + falloffDistance;
    light.falloffDistance = f * _srtGlobals.srt_fLightSphericalFalloffMultiplier;
  }

  pScene->AddLight(light);
}


static void RT_AddSpotlight(CEntity *pEn, SSRT::Scene *pScene)
{
  const FLOATmatrix3D &rot = pScene->GetCameraRotation();
  FLOAT3D right   = { rot(1, 1), rot(2, 1), rot(3, 1) };
  FLOAT3D forward = { rot(1, 3), rot(2, 3), rot(3, 3) };
  FLOAT3D up      = { rot(1, 2), rot(2, 2), rot(3, 2) };

  forward *= -1;


  FLOAT3D position, endPosition;

  if (pEn->en_ulID == pScene->GetViewerEntityID())
  {
    position = pScene->GetCameraPosition();
  }
  else if (RT_IsModelPlayer(pEn))
  {
    FLOATaabbox3D aabb;
    pEn->GetSize(aabb);

    position = pEn->GetLerpedPlacement().pl_PositionVector + aabb.Center();
  }
  else
  {
    return;
  }

  position += right   * _srtGlobals.srt_vSpotlightOffset(1) +
              up      * _srtGlobals.srt_vSpotlightOffset(2) +
              forward * _srtGlobals.srt_vSpotlightOffset(3);

  RT_FixSpotlightPosition(pEn, pScene, forward, position, endPosition);


  SSRT::CSpotLight light = {};
  light.absPosition = position;
  light.direction = (endPosition - position).Normalize();
  light.upVector = up;

  pScene->AddLight(light);
}


static void RT_AddLight(const CLightSource *plsLight, SSRT::Scene *pScene)
{
  ASSERT(plsLight != NULL);


  // don't consider lights that subtract light or have only lens flare
  if (plsLight->ls_ulFlags & (LSF_DARKLIGHT | LSF_LENSFLAREONLY))
  {
    return;
  }

  // and that have no contribution
  UBYTE ubR, ubG, ubB;
  plsLight->GetLightColor(ubR, ubG, ubB);
  if ((uint32_t)ubR + (uint32_t)ubG + (uint32_t)ubB == 0)
  {
    return;
  }


  CEntity *pEn = plsLight->ls_penEntity;
  const ULONG entityID = pEn->en_ulID;

  // directional light
  if (plsLight->ls_ulFlags & LSF_DIRECTIONAL)
  {
    if (pScene->GetCustomInfo()->IsDirectionalLightIgnored(plsLight))
    {
      return;
    }

    FLOAT3D direction;
    AnglesToDirectionVector(pEn->GetLerpedPlacement().pl_OrientationAngle, direction);

    SSRT::CDirectionalLight light = {};
    light.entityID = entityID;
    light.direction = direction;
    light.color = GetDirectionalLightColor(plsLight);

    pScene->AddLight(light);
  }
  else
  {
    if (pScene->GetCustomInfo()->IsSphericalLightIgnored(plsLight))
    {
      // add it to the list of ignored lights, it will be used
      // to match light sources from fire textures;
      // it is done because a good way for determination of
      // "real" light sources (like torches, rockets) still wasn't found
      RT_IgnoredLights.Push() = plsLight;

      return;
    }

    bool isDynamic = plsLight->ls_ulFlags & LSF_DYNAMIC;
    bool isMuzzleFlash = false;


    FLOAT3D position;

    if (entityID == pScene->GetViewerEntityID())
    {
      position = RT_FindMuzzleFlashPositionForFirstPerson(pEn, pScene);
      isMuzzleFlash = true;
    }
    else if (RT_IsModelPlayer(pEn))
    {
      position = RT_FindMuzzleFlashPositionForPlayerAABB(pEn, pScene);
      isMuzzleFlash = true;
    }
    else
    {
      position = pEn->GetLerpedPlacement().pl_PositionVector;

      if (isDynamic)
      {
        position = RT_FixSphericalLightPosition(pEn, position, _srtGlobals.srt_fLightSphericalPolygonOffset, pScene->GetWorld());
      }
    }


    RT_AddModifiedSphereLightToScene(entityID,
                                     position, GetSphericalLightColor(plsLight),
                                     plsLight->ls_rHotSpot, plsLight->ls_rFallOff,
                                     isDynamic, isMuzzleFlash,
                                     pScene);
  }
}


static void RT_TryAddPotentialLight(CEntity *pEn, SSRT::Scene *pScene)
{
  const float threshold = 1.0f;
  FLOAT3D originalPos = pEn->GetLerpedPlacement().pl_PositionVector;

  INDEX closestLtIndex = -1;
  float closestDist = threshold;
  FLOAT3D closestPos;
  FLOAT3D pos;

  // scan ignored lights, and try to find 
  for (INDEX i = 0; i < RT_IgnoredLights.Count(); i++)
  {
    ASSERT(RT_IgnoredLights[i] && RT_IgnoredLights[i]->ls_penEntity);

    pos = RT_IgnoredLights[i]->ls_penEntity->GetLerpedPlacement().pl_PositionVector;

    float d = (originalPos - pos).ManhattanNorm();

    if (d < closestDist)
    {
      closestLtIndex = i;
      closestPos = pos;
      closestDist = d;
    }
  }


  // light.isDynamic = plsLight->ls_ulFlags & LSF_DYNAMIC;
  // let it use dynamic radius/falloff adjustments to exaggurate light
  const bool isDynamic = true;


  // if found
  if (closestLtIndex != -1)
  {
    const CLightSource *plsLight = RT_IgnoredLights[closestLtIndex];

    RT_AddModifiedSphereLightToScene(pEn->en_ulID,
                                     closestPos, GetSphericalLightColor(plsLight),
                                     plsLight->ls_rHotSpot * _srtGlobals.srt_fLightSphericalRadiusOfPotentialMultiplier,
                                     plsLight->ls_rFallOff * _srtGlobals.srt_fLightSphericalFalloffOfPotentialMultiplier,
                                     isDynamic, false,
                                     pScene);
  }
  else
  {
    FLOAT3D color = { 1.0f, 0.7f, 0.35f };
    color *= _srtGlobals.srt_fLightSphericalIntensityMultiplier;

    float falloffDistance = 1.0f;

    if (pEn->en_pmoModelObject != nullptr)
    {
      falloffDistance *= pEn->en_pmoModelObject->mo_Stretch.Length();

      FLOATaabbox3D aabb;
      pEn->GetBoundingBox(aabb);

      originalPos = aabb.Center();
    }

    falloffDistance *= _srtGlobals.srt_fLightSphericalFalloffOfPotentialMultiplier;

    // if there is no nearest light, then just use any params
    float hotspotDistance = 1.0f;
    hotspotDistance *= _srtGlobals.srt_fLightSphericalRadiusOfPotentialMultiplier;

    RT_AddModifiedSphereLightToScene(pEn->en_ulID,
                                     originalPos, color,
                                     hotspotDistance, falloffDistance,
                                     isDynamic, false,
                                     pScene);
  }
}


void RT_ProcessModelLights(CEntity *penModel, SSRT::Scene *pScene)
{
  const CLightSource *pls = penModel->GetLightSource();

  if (pls != NULL)
  {
    RT_AddLight(pls, pScene);
  }
  // RT: add light if model has fire texture
  else if (pScene->GetCustomInfo()->HasModelFireTexture(penModel))
  {
    RT_PotentialLights.Push() = penModel;
  }
  
  if (_srtGlobals.srt_bSpotlightEnable)
  {
    RT_AddSpotlight(penModel, pScene);
  }
}


void RT_AddPotentialLightSources(SSRT::Scene *pScene)
{
  for (INDEX i = 0; i < RT_PotentialLights.Count(); i++)
  {
    RT_TryAddPotentialLight(RT_PotentialLights[i], pScene);
  }

  RT_IgnoredLights.PopAll();
  RT_PotentialLights.PopAll();
}
