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
#include <Engine/Math/Quaternion.h>
#include <Engine/Models/RenderModel.h>
#include <Engine/Models/ModelObject.h>
#include <Engine/Models/ModelData.h>
#include <Engine/Network/Network.h>
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


static FLOAT3D RT_GetDirectionalLightColor(const CLightSource *plsLight)
{
  UBYTE r, g, b;
  plsLight->GetLightColor(r, g, b);
  FLOAT3D color = { r / 255.0f, g / 255.0f, b / 255.0f };

  RT_AdjustSaturation(color, _srtGlobals.srt_fSunSaturation);
  RT_AdjustPow(color, _srtGlobals.srt_fSunColorPow);

  return color;
}


static FLOAT3D RT_GetSphericalLightColor(const CLightSource *plsLight)
{
  UBYTE r, g, b;
  plsLight->GetLightColor(r, g, b);
  FLOAT3D color = { r / 255.0f, g / 255.0f, b / 255.0f };

  RT_AdjustSaturation(color, _srtGlobals.srt_fLightSphSaturation);
  RT_AdjustPow(color, _srtGlobals.srt_fLightSphColorPow);

  return color;
}


static FLOAT3D RT_GetDirectionalLightDirection(const CLightSource *plsLight, SSRT::Scene *pScene)
{
  CPlacement3D pl = plsLight->ls_penEntity->GetLerpedPlacement();

  return pScene->GetCustomInfo()->GetAnimatedSunDirection(pl.pl_OrientationAngle);
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
  FLOAT3D muzzlePos = basePosition - forward * _srtGlobals.srt_fMuzzleLightOffset;
  FLOAT3D rayDstPos = basePosition - forward * (_srtGlobals.srt_fMuzzleLightOffset + eps);

  CCastRay crRay(pEn, basePosition, rayDstPos);
  crRay.cr_ttHitModels = _srtGlobals.srt_bLightFixWithModels ? CCastRay::TT_COLLISIONBOX : CCastRay::TT_NONE;
  crRay.cr_bHitTranslucentPortals = _srtGlobals.srt_bLightFixWithTranslucent;
  crRay.cr_bPhysical = TRUE;
  pScene->GetWorld()->CastRay(crRay);

  // if found intersection
  if (crRay.cr_penHit != nullptr && crRay.cr_pbpoBrushPolygon != nullptr)
  {
    const FLOAT3D &normal = (const FLOAT3D &)crRay.cr_pbpoBrushPolygon->bpo_pbplPlane->bpl_plAbsolute;

    return crRay.cr_vHit + normal * _srtGlobals.srt_fLightSphPolygonOffset;
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

    FLOAT3D defaultEndPos = pScene->GetCameraPosition() + direction * _srtGlobals.srt_fFlashlightFalloffDistance;

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
  if (_srtGlobals.srt_fMuzzleLightOffset < 0.01f)
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

  float f = hotspotDistance + falloffDistance;

  if (isMuzzleFlash)
  {
    light.radius = _srtGlobals.srt_fMuzzleLightRadius;
    light.falloffDistance = f * _srtGlobals.srt_fMuzzleLightFalloffMultiplier;
  }
  else if (isDynamic)
  {
    light.radius = _srtGlobals.srt_fDynamicLightSphRadius;

    f = Clamp(f, _srtGlobals.srt_fDynamicLightSphFalloffMin, _srtGlobals.srt_fDynamicLightSphFalloffMax);
    light.falloffDistance = f * _srtGlobals.srt_fDynamicLightSphFalloffMultiplier;
  }
  else
  {
    light.radius = hotspotDistance * _srtGlobals.srt_fOriginalLightSphRadiusMultiplier;
    light.falloffDistance = f * _srtGlobals.srt_fOriginalLightSphFalloffMultiplier;
  }

  pScene->AddLight(light);
}


// A dirty way to determine player's torso attachment to position a flashlight.
static void RT_FixFlashlightWithPlayerTorso(CEntity *pEn, FLOAT3D &playerPos, FLOAT3D &playerRight, FLOAT3D &playerUp, FLOAT3D &playerForward)
{
  // this define is from "Models/Player/SeriousSam/Player.h"
#define PLAYER_ATTACHMENT_TORSO 0

  if (CModelObject *pmoParent = pEn->GetModelObject())
  {
    if (CAttachmentModelObject *pamo = pmoParent->GetAttachmentModel(PLAYER_ATTACHMENT_TORSO))
    {
      // get the position
      const CAttachedModelPosition &amp = pmoParent->GetData()->md_aampAttachedPosition[pamo->amo_iAttachedPosition];

      // unpack the reference vertices
      FLOAT3D vCenter, vFront, vUp;
      const INDEX iCenter = amp.amp_iCenterVertex;
      const INDEX iFront = amp.amp_iFrontVertex;
      const INDEX iUp = amp.amp_iUpVertex;

      CRenderModel rmMain;
      rmMain.rm_pmdModelData = pmoParent->GetData();
      rmMain.rm_fDistanceFactor = 0;
      rmMain.SetObjectPlacement(pEn->GetLerpedPlacement());
      pmoParent->GetFrame(rmMain.rm_iFrame0, rmMain.rm_iFrame1, rmMain.rm_fRatio);
      const INDEX ctVertices = rmMain.rm_pmdModelData->md_VerticesCt;
      if (rmMain.rm_pmdModelData->md_Flags & MF_COMPRESSED_16BIT)
      {
        // set pFrame to point to last and next frames' vertices
        rmMain.rm_pFrame16_0 = &rmMain.rm_pmdModelData->md_FrameVertices16[rmMain.rm_iFrame0 * ctVertices];
        rmMain.rm_pFrame16_1 = &rmMain.rm_pmdModelData->md_FrameVertices16[rmMain.rm_iFrame1 * ctVertices];
      }
      else
      {
        // set pFrame to point to last and next frames' vertices
        rmMain.rm_pFrame8_0 = &rmMain.rm_pmdModelData->md_FrameVertices8[rmMain.rm_iFrame0 * ctVertices];
        rmMain.rm_pFrame8_1 = &rmMain.rm_pmdModelData->md_FrameVertices8[rmMain.rm_iFrame1 * ctVertices];
      }
      FLOAT3D &vDataStretch = rmMain.rm_pmdModelData->md_Stretch;
      rmMain.rm_vStretch(1) = vDataStretch(1) * pmoParent->mo_Stretch(1);
      rmMain.rm_vStretch(2) = vDataStretch(2) * pmoParent->mo_Stretch(2);
      rmMain.rm_vStretch(3) = vDataStretch(3) * pmoParent->mo_Stretch(3);
      rmMain.rm_vOffset = rmMain.rm_pmdModelData->md_vCompressedCenter;


      auto unpackVertex = [pmoParent, &rmMain] (INDEX iVertex, FLOAT3D &vVertex)
      {
        // GetData()
        const CModelData *data = pmoParent->GetData();

        if (data->md_Flags & MF_COMPRESSED_16BIT)
        {
          // get 16 bit packed vertices
          const SWPOINT3D &vsw0 = rmMain.rm_pFrame16_0[iVertex].mfv_SWPoint;
          const SWPOINT3D &vsw1 = rmMain.rm_pFrame16_1[iVertex].mfv_SWPoint;
          // convert them to float and lerp between them
          vVertex(1) = (Lerp((FLOAT)vsw0(1), (FLOAT)vsw1(1), rmMain.rm_fRatio) - rmMain.rm_vOffset(1)) * rmMain.rm_vStretch(1);
          vVertex(2) = (Lerp((FLOAT)vsw0(2), (FLOAT)vsw1(2), rmMain.rm_fRatio) - rmMain.rm_vOffset(2)) * rmMain.rm_vStretch(2);
          vVertex(3) = (Lerp((FLOAT)vsw0(3), (FLOAT)vsw1(3), rmMain.rm_fRatio) - rmMain.rm_vOffset(3)) * rmMain.rm_vStretch(3);
        }
        else
        {
          // get 8 bit packed vertices
          const SBPOINT3D &vsb0 = rmMain.rm_pFrame8_0[iVertex].mfv_SBPoint;
          const SBPOINT3D &vsb1 = rmMain.rm_pFrame8_1[iVertex].mfv_SBPoint;
          // convert them to float and lerp between them
          vVertex(1) = (Lerp((FLOAT)vsb0(1), (FLOAT)vsb1(1), rmMain.rm_fRatio) - rmMain.rm_vOffset(1)) * rmMain.rm_vStretch(1);
          vVertex(2) = (Lerp((FLOAT)vsb0(2), (FLOAT)vsb1(2), rmMain.rm_fRatio) - rmMain.rm_vOffset(2)) * rmMain.rm_vStretch(2);
          vVertex(3) = (Lerp((FLOAT)vsb0(3), (FLOAT)vsb1(3), rmMain.rm_fRatio) - rmMain.rm_vOffset(3)) * rmMain.rm_vStretch(3);
        }
      };

      unpackVertex(iCenter, vCenter);
      unpackVertex(iFront, vFront);
      unpackVertex(iUp, vUp);

      // create front and up direction vectors
      FLOAT3D vY = vUp - vCenter;
      FLOAT3D vZ = vCenter - vFront;
      // project center and directions from object to absolute space
      const FLOATmatrix3D &mO2A = rmMain.rm_mObjectRotation;
      const FLOAT3D &vO2A = rmMain.rm_vObjectPosition;
      vCenter = vCenter * mO2A + vO2A;
      vY = vY * mO2A;
      vZ = vZ * mO2A;

      // make a rotation matrix from the direction vectors
      FLOAT3D vX = vY * vZ;
      vY = vZ * vX;
      vX.Normalize();
      vY.Normalize();
      vZ.Normalize();
      FLOATmatrix3D mOrientation;
      mOrientation(1, 1) = vX(1);  mOrientation(1, 2) = vY(1);  mOrientation(1, 3) = vZ(1);
      mOrientation(2, 1) = vX(2);  mOrientation(2, 2) = vY(2);  mOrientation(2, 3) = vZ(2);
      mOrientation(3, 1) = vX(3);  mOrientation(3, 2) = vY(3);  mOrientation(3, 3) = vZ(3);

      FLOAT3D vOffset;
      FLOATmatrix3D mRelative;
      MakeRotationMatrix(mRelative, pamo->amo_plRelative.pl_OrientationAngle);
      vOffset(1) = pamo->amo_plRelative.pl_PositionVector(1) * pmoParent->mo_Stretch(1);
      vOffset(2) = pamo->amo_plRelative.pl_PositionVector(2) * pmoParent->mo_Stretch(2);
      vOffset(3) = pamo->amo_plRelative.pl_PositionVector(3) * pmoParent->mo_Stretch(3);
      FLOAT3D vO = vCenter + vOffset * mOrientation;
      mOrientation *= mRelative; 
      // rmAttached.SetObjectPlacement(vO, mOrientation);



      playerPos = vO;

      playerRight = FLOAT3D(mOrientation(1, 1), mOrientation(2, 1), mOrientation(3, 1));
      playerUp = FLOAT3D(mOrientation(1, 2), mOrientation(2, 2), mOrientation(3, 2));
      playerForward = FLOAT3D(mOrientation(1, 3), mOrientation(2, 3), mOrientation(3, 3)) * -1;
    }
  }
}


static void RT_AddSpotlight(CEntity *pEn, SSRT::Scene *pScene)
{
  const FLOATmatrix3D &camRot = pScene->GetCameraRotation();
  const auto camRight   = FLOAT3D(camRot(1, 1), camRot(2, 1), camRot(3, 1));
  const auto camUp      = FLOAT3D(camRot(1, 2), camRot(2, 2), camRot(3, 2));
  const auto camForward = FLOAT3D(camRot(1, 3), camRot(2, 3), camRot(3, 3)) * -1;

  // if first person
  if (pEn->en_ulID == pScene->GetViewerEntityID())
  {
    FLOAT3D position, endPosition;

    position = 
      pScene->GetCameraPosition() +
      camRight   * _srtGlobals.srt_vFlashlightOffset(1) +
      camUp      * _srtGlobals.srt_vFlashlightOffset(2) +
      camForward * _srtGlobals.srt_vFlashlightOffset(3);

    RT_FixSpotlightPosition(pEn, pScene, camForward, position, endPosition);


    SSRT::CSpotLight light = {};
    light.absPosition = position;
    light.direction = (endPosition - position).Normalize();
    light.upVector = camUp;
    light.isFirstPerson = true;

    pScene->AddLight(light);
  }
  // only for local player
  // NOTE: must be tested with splitscreen, there can be several local players
  else if (_pNetwork->IsPlayerLocal(pEn))
  {
    // default rotation
    FLOATmatrix3D playerRot;
    MakeRotationMatrix(playerRot, pEn->GetPlacement().pl_OrientationAngle);

    auto playerRight   = FLOAT3D(playerRot(1, 1), playerRot(2, 1), playerRot(3, 1));
    auto playerUp      = FLOAT3D(playerRot(1, 2), playerRot(2, 2), playerRot(3, 2));
    auto playerForward = FLOAT3D(playerRot(1, 3), playerRot(2, 3), playerRot(3, 3)) * -1;
   
    // default position
    auto playerPos = pEn->GetLerpedPlacement().pl_PositionVector + playerUp * 1.0f;


    RT_FixFlashlightWithPlayerTorso(pEn, playerPos, playerRight, playerUp, playerForward);


    playerPos += 
      playerRight   * _srtGlobals.srt_vFlashlightOffsetThirdPerson(1) +
      playerUp      * _srtGlobals.srt_vFlashlightOffsetThirdPerson(2) +
      playerForward * _srtGlobals.srt_vFlashlightOffsetThirdPerson(3);


    SSRT::CSpotLight light = {};
    light.absPosition = playerPos;
    light.direction = playerForward;
    light.upVector = playerUp;
    light.isFirstPerson = false;

    pScene->AddLight(light);
  }
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

    SSRT::CDirectionalLight light = {};
    light.entityID = entityID;
    light.direction = RT_GetDirectionalLightDirection(plsLight, pScene);
    light.color = RT_GetDirectionalLightColor(plsLight) * _srtGlobals.srt_fSunIntensity;

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
        position = RT_FixSphericalLightPosition(pEn, position, _srtGlobals.srt_fLightSphPolygonOffset, pScene->GetWorld());
      }
    }

    float intensity =
      isMuzzleFlash ? _srtGlobals.srt_fMuzzleLightIntensity :
      isDynamic ? _srtGlobals.srt_fDynamicLightSphIntensity :
      _srtGlobals.srt_fOriginalLightSphIntensity;

    RT_AddModifiedSphereLightToScene(entityID,
                                     position, RT_GetSphericalLightColor(plsLight) * intensity,
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
                                     closestPos, RT_GetSphericalLightColor(plsLight) * _srtGlobals.srt_fPotentialLightSphIntensity,
                                     plsLight->ls_rHotSpot * _srtGlobals.srt_fPotentialLightSphRadiusMultiplier,
                                     plsLight->ls_rFallOff * _srtGlobals.srt_fPotentialLightSphFalloffMultiplier,
                                     isDynamic, false,
                                     pScene);
  }
  else
  {
    // just any
    const FLOAT3D color = { 1.0f, 0.7f, 0.35f };

    float falloffDistance = 1.0f;

    if (pEn->en_pmoModelObject != nullptr)
    {
      falloffDistance *= pEn->en_pmoModelObject->mo_Stretch.Length();

      FLOATaabbox3D aabb;
      pEn->GetBoundingBox(aabb);

      originalPos = aabb.Center();
    }

    falloffDistance *= _srtGlobals.srt_fPotentialLightSphFalloffMultiplier;

    // if there is no nearest light, then just use any params
    float hotspotDistance = 1.0f;
    hotspotDistance *= _srtGlobals.srt_fPotentialLightSphRadiusMultiplier;

    RT_AddModifiedSphereLightToScene(pEn->en_ulID,
                                     originalPos, color * _srtGlobals.srt_fPotentialLightSphIntensity,
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
  
  if (_srtGlobals.srt_bFlashlightEnable)
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
