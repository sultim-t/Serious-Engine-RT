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
#include <Engine/Entities/EntityCollision.h>
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
#include "Engine/Graphics/DrawPort.h"
#include "Engine/Light/LensFlares.h"
#include "Engine/Rendering/Render.h"


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
  FLOAT3D muzzlePos = basePosition + forward * _srtGlobals.srt_fMuzzleLightOffset;
  FLOAT3D rayDstPos = basePosition + forward * (_srtGlobals.srt_fMuzzleLightOffset + eps);

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
#define SPOTLIGHT_NEAR_FIX_DIST_MAX 30.0f

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
        refStartPosition = Lerp(pScene->GetCameraPosition(), refStartPosition, Clamp((crRay.cr_fHitDistance - 0.2f) / len, SPOTLIGHT_START_THRESHOLD, 1.0f));
      }
    }
  }

  {
    CCastRay crRay(pEn, pScene->GetCameraPosition(), pScene->GetCameraPosition() + direction * SPOTLIGHT_NEAR_FIX_DIST_MAX);
    crRay.cr_ttHitModels = _srtGlobals.srt_bLightFixWithModels ? CCastRay::TT_COLLISIONBOX : CCastRay::TT_NONE;
    crRay.cr_bHitTranslucentPortals = _srtGlobals.srt_bLightFixWithTranslucent;
    crRay.cr_bPhysical = TRUE;
    pScene->GetWorld()->CastRay(crRay);

    FLOAT3D defaultEndPos = pScene->GetCameraPosition() + direction * SPOTLIGHT_NEAR_FIX_DIST_MAX;

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
  FLOAT3D forward = -FLOAT3D(rot(1, 3), rot(2, 3), rot(3, 3));
  FLOAT3D up = FLOAT3D(rot(1, 2), rot(2, 2), rot(3, 2));

  return RT_FindMuzzleFlashPosition(pEn, cameraPos + up * 0.2f, forward, pScene);
}


static FLOAT3D RT_FindMuzzleFlashPositionForPlayerAABB(CEntity *pEn, SSRT::Scene *pScene)
{
  FLOATaabbox3D aabb;
  pEn->GetBoundingBox(aabb);

  const FLOATmatrix3D &rot = pEn->GetRotationMatrix();
  FLOAT3D forward = -FLOAT3D(rot(1, 3), rot(2, 3), rot(3, 3));

  return RT_FindMuzzleFlashPosition(pEn, aabb.Center(), forward, pScene);
}


static void RT_AddModifiedSphereLightToScene(const CLightSource *plsLight,
                                             ULONG entityID,
                                             const FLOAT3D &position,
                                             const FLOAT3D &color,
                                             float hotspotDistance,
                                             float falloffDistance,
                                             bool isDynamic,
                                             bool isPotential,
                                             bool isMuzzleFlash,
                                             SSRT::Scene *pScene)
{
  SSRT::CSphereLight light = {};
  light.plsLight = plsLight;
  light.entityID = entityID;
  light.absPosition = position;
  light.color = color;

  float f = hotspotDistance + falloffDistance;

  if (isMuzzleFlash)
  {
    // make muzzle flash yellow
    float b = _srtGlobals.srt_fMuzzleLightIntensity;
    light.color = { 1.0f * b, 0.87f * b, 0.53f * b };

    light.radius = _srtGlobals.srt_fMuzzleLightRadius;
    light.falloffDistance = f * _srtGlobals.srt_fMuzzleLightFalloffMultiplier;
  }
  else if (isPotential)
  {
    light.radius = Max(_srtGlobals.srt_fPotentialLightSphRadiusMin, hotspotDistance * _srtGlobals.srt_fPotentialLightSphRadiusMultiplier);
    light.falloffDistance = Max(_srtGlobals.srt_fPotentialLightSphFalloffMin, f * _srtGlobals.srt_fPotentialLightSphFalloffMultiplier);
  }
  else if (isDynamic)
  {
    light.radius = _srtGlobals.srt_fDynamicLightSphRadius;
    f = Clamp(f, _srtGlobals.srt_fDynamicLightSphFalloffMin, _srtGlobals.srt_fDynamicLightSphFalloffMax);
    light.falloffDistance = f * _srtGlobals.srt_fDynamicLightSphFalloffMultiplier;
  }
  else
  {
    light.radius = Max(_srtGlobals.srt_fOriginalLightSphRadiusMin, hotspotDistance * _srtGlobals.srt_fOriginalLightSphRadiusMultiplier);
    light.falloffDistance = f * _srtGlobals.srt_fOriginalLightSphFalloffMultiplier;
  }

  pScene->AddLight(light);
}


// https://www.scratchapixel.com/code.php?id=10&origin=&src=1
static bool RT_RayBoxIntersection(const FLOATaabbox3D &box, const FLOAT3D &vRayOrigin, const FLOAT3D &vRayDir, float *pResult)
{
  struct vec3_t
  {
    float x, y, z;
  };

  struct ray_t
  {
    vec3_t orig, dir;
    vec3_t invdir;
    int sign[3];
  };

  ray_t r = {};
  r.orig = { vRayOrigin(1), vRayOrigin(2), vRayOrigin(3) };
  r.invdir = { 1.0f / vRayDir(1), 1.0f / vRayDir(2), 1.0f / vRayDir(3) };
  r.sign[0] = r.invdir.x < 0;
  r.sign[1] = r.invdir.y < 0;
  r.sign[2] = r.invdir.z < 0;

  vec3_t bounds[2];
  bounds[0] = { box.minvect(1), box.minvect(2), box.minvect(3) };
  bounds[1] = { box.maxvect(1), box.maxvect(2), box.maxvect(3) };

  float tmin, tmax, tymin, tymax, tzmin, tzmax;

  tmin = (bounds[r.sign[0]].x - r.orig.x) * r.invdir.x;
  tmax = (bounds[1 - r.sign[0]].x - r.orig.x) * r.invdir.x;
  tymin = (bounds[r.sign[1]].y - r.orig.y) * r.invdir.y;
  tymax = (bounds[1 - r.sign[1]].y - r.orig.y) * r.invdir.y;

  if ((tmin > tymax) || (tymin > tmax))
    return false;

  if (tymin > tmin)
    tmin = tymin;
  if (tymax < tmax)
    tmax = tymax;

  tzmin = (bounds[r.sign[2]].z - r.orig.z) * r.invdir.z;
  tzmax = (bounds[1 - r.sign[2]].z - r.orig.z) * r.invdir.z;

  if ((tmin > tzmax) || (tzmin > tmax))
    return false;

  if (tzmin > tmin)
    tmin = tzmin;
  if (tzmax < tmax)
    tmax = tzmax;

  float &t = *pResult;
  t = tmin;

  if (t < 0)
  {
    t = tmax;
    if (t < 0) return false;
  }

  return true;
}


static FLOAT3D RT_FixFlashlightThirdPerson(CEntity *pEn, const FLOAT3D &vCameraForward)
{
  CCollisionInfo *pci = pEn->en_pciCollisionInfo;

  if (pci == nullptr)
  {
    return pEn->GetLerpedPlacement().pl_PositionVector;
  }

  FLOATaabbox3D bbLocalBox = FLOATaabbox3D();

  // MakeBoxAtPlacement, but local
  {
    CMovingSphere &ms0 = pci->ci_absSpheres[0];
    CMovingSphere &ms1 = pci->ci_absSpheres[pci->ci_absSpheres.Count() - 1];
    bbLocalBox = FLOATaabbox3D(ms0.ms_vCenter, ms0.ms_fR);
    bbLocalBox |= FLOATaabbox3D(ms1.ms_vCenter, ms1.ms_fR);
  }

  // AABB center, but with Y a bit higher
  FLOAT3D vLocalOrigin;
  vLocalOrigin(1) = (bbLocalBox.minvect(1) + bbLocalBox.maxvect(1)) * 0.5f;
  vLocalOrigin(2) = (bbLocalBox.minvect(2) + bbLocalBox.maxvect(2)) * 0.8f;
  vLocalOrigin(3) = (bbLocalBox.minvect(3) + bbLocalBox.maxvect(3)) * 0.5f;

  float tFinal = 0.0f;
  float t;

  if (RT_RayBoxIntersection(bbLocalBox, vLocalOrigin, vCameraForward, &t))
  {
    tFinal = t + 0.2f;
  }

  return pEn->GetLerpedPlacement().pl_PositionVector + vLocalOrigin + vCameraForward * tFinal;
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
    /*FLOATmatrix3D playerRot;
    MakeRotationMatrix(playerRot, pEn->GetPlacement().pl_OrientationAngle);

    auto playerRight   = FLOAT3D(playerRot(1, 1), playerRot(2, 1), playerRot(3, 1));
    auto playerUp      = FLOAT3D(playerRot(1, 2), playerRot(2, 2), playerRot(3, 2));
    auto playerForward = FLOAT3D(playerRot(1, 3), playerRot(2, 3), playerRot(3, 3)) * -1;
   
    // default position
    auto playerPos = pEn->GetLerpedPlacement().pl_PositionVector + playerUp * 1.0f;*/

    FLOAT3D playerPos = RT_FixFlashlightThirdPerson(pEn, camForward);


    /*playerPos += 
      playerRight   * _srtGlobals.srt_vFlashlightOffsetThirdPerson(1) +
      playerUp      * _srtGlobals.srt_vFlashlightOffsetThirdPerson(2) +
      playerForward * _srtGlobals.srt_vFlashlightOffsetThirdPerson(3);*/


    SSRT::CSpotLight light = {};
    light.absPosition = playerPos;
    light.direction = camForward;
    light.upVector = camUp;
    light.isFirstPerson = false;

    pScene->AddLight(light);
  }
}


static FLOAT IntensityAtDistance(FLOAT fFallOff, FLOAT fHotSpot, FLOAT fDistance)
{
  // intensity is zero if further than fall-off range
  if (fDistance > fFallOff) return 0.0f;
  // intensity is maximum if closer than hot-spot range
  if (fDistance < fHotSpot) return 1.0f;
  // interpolate if between fall-off and hot-spot range
  return (fFallOff - fDistance) / (fFallOff - fHotSpot);
}


static void UploadLensFlare(const FLOAT3D pointToCheck,
                            CTextureObject *pto, 
                            FLOAT fI, FLOAT fJ, FLOAT fSizeI, FLOAT fSizeJ, 
                            ANGLE aRotation, COLOR colLight,
                            FLOAT fScreenSizeI, FLOAT fScreenSizeJ)
{
  static const INDEX pindices[6] = { 0,1,2,2,3,0 };

  GFXVertex pvtx[4];
  GFXTexCoord ptex[4];
  GFXColor pcol[4];

  // find lens location and dimension
  const FLOAT fRI = fSizeI * 0.5f;
  const FLOAT fRJ = fSizeJ * 0.5f;
  const FLOAT fSinA = SinFast(aRotation);
  const FLOAT fCosA = CosFast(aRotation);
  const FLOAT fRICosA = fRI * +fCosA;
  const FLOAT fRJSinA = fRJ * -fSinA;
  const FLOAT fRISinA = fRI * +fSinA;
  const FLOAT fRJCosA = fRJ * +fCosA;

  // get texture parameters for current frame and needed mip factor and upload texture
  CTextureData *ptd = (CTextureData *)pto->GetData();
  ptd->SetAsCurrent(pto->GetFrame());
  // set lens color
  colLight = AdjustColor(colLight, _slShdHueShift, _slShdSaturation);
  const GFXColor glcol(colLight);

  // prepare coordinates of the rectangle
  pvtx[0].x = fI - fRICosA + fRJSinA;  pvtx[0].y = fJ - fRISinA + fRJCosA;  pvtx[0].z = 0.01f;
  pvtx[1].x = fI - fRICosA - fRJSinA;  pvtx[1].y = fJ - fRISinA - fRJCosA;  pvtx[1].z = 0.01f;
  pvtx[2].x = fI + fRICosA - fRJSinA;  pvtx[2].y = fJ + fRISinA - fRJCosA;  pvtx[2].z = 0.01f;
  pvtx[3].x = fI + fRICosA + fRJSinA;  pvtx[3].y = fJ + fRISinA + fRJCosA;  pvtx[3].z = 0.01f;
  ptex[0].s = 0;  ptex[0].t = 0;
  ptex[1].s = 0;  ptex[1].t = 1;
  ptex[2].s = 1;  ptex[2].t = 1;
  ptex[3].s = 1;  ptex[3].t = 0;
  pcol[0] = glcol;
  pcol[1] = glcol;
  pcol[2] = glcol;
  pcol[3] = glcol;

  // from pixel coords to [-1,1]
  for (auto &v : pvtx)
  {
    v.x = (v.x / fScreenSizeI) * 2 - 1;
    v.y = (v.y / fScreenSizeJ) * 2 - 1;
  }

  SSRT::CHudElementInfo hudInfo = {};
  hudInfo.textureData = ptd;
  hudInfo.textureWrapU = hudInfo.textureWrapV = GfxWrap::GFX_CLAMP;

  hudInfo.pPositions = pvtx;
  hudInfo.pTexCoords = ptex;
  hudInfo.pColors = pcol;
  hudInfo.vertexCount = std::size(pvtx);
  hudInfo.pIndices = pindices;
  hudInfo.indexCount = std::size(pindices);
  
  _pGfx->gl_SSRT->ProcessLensFlare(hudInfo, pointToCheck);
}


// Based on CRenderer::AddLensFlare / CRenderer::RenderLensFlares
// Assume gfx_iLensFlareQuality=3
static void AddLensFlare(const CLightSource *plsLight, SSRT::Scene *pScene)
{
  if (!_srtGlobals.srt_bLensFlares)
  {
    return;
  }

  if (_wrpWorldRenderPrefs.wrp_lftLensFlares == CWorldRenderPrefs::LFT_NONE)
  {
    return;
  }

  CEntity *penModel = plsLight->ls_penEntity;

  CDrawPort *pdpDrawPort = pScene->GetDrawPort();
  if (pdpDrawPort == nullptr)
  {
    return;
  }

  FLOAT fScreenSizeI = (FLOAT)pdpDrawPort->GetWidth();
  FLOAT fScreenSizeJ = (FLOAT)pdpDrawPort->GetHeight();


  FLOAT lfi_fI, lfi_fJ, lfi_fOoK, lfi_fDistance;
  FLOAT3D pointToCheck;
  {
    FLOAT3D position = penModel->GetLerpedPlacement().pl_PositionVector;

    const CProjection3D *pprProjection = penModel->en_ulFlags & ENF_BACKGROUND ?
      pScene->GetSEProjectionBackground() :
      pScene->GetSEProjection();






    // TODO: need world space, as projection matrix is inside RTGL1,
    // and pprProjection doesn't match it, so we can see through walls!






    FLOAT3D vRotated;
    pprProjection->PreClip(position, vRotated);
    // if it is behind near clip plane
    if (-vRotated(3) < pprProjection->NearClipDistanceR())
    {
      return;
    }
    // project it to screen
    FLOAT3D vScreen;
    pprProjection->PostClip(vRotated, vScreen);

    lfi_fI = vScreen(1);
    lfi_fJ = vScreen(2);
    lfi_fOoK = (1 - pprProjection->pr_fDepthBufferFactor / vRotated(3))
      * pprProjection->pr_fDepthBufferMul + pprProjection->pr_fDepthBufferAdd;
    lfi_fDistance = -vRotated(3);
    
    pointToCheck = FLOAT3D(
      lfi_fI / fScreenSizeI,  // screen coords in [0..1]
      lfi_fJ / fScreenSizeJ,
      lfi_fOoK * 0.5f + 0.5f  // NDC depth in [0..1]
    );
  }
  
  // RT: flare fading should be done on RTGL1 side
  FLOAT lfi_fFadeFactor = 1.0f;


  FLOAT fScreenCenterI = fScreenSizeI * 0.5f;
  FLOAT fScreenCenterJ = fScreenSizeJ * 0.5f;
  FLOAT fLightI = lfi_fI;
  FLOAT fLightJ = lfi_fJ;
  FLOAT fIPositionFactor = (fLightI - fScreenCenterI) / fScreenSizeI;
  FLOAT fReflectionDirI = fScreenCenterI - fLightI;
  FLOAT fReflectionDirJ = fScreenCenterJ - fLightJ;
  UBYTE ubR, ubG, ubB;
  plsLight->GetLightColor(ubR, ubG, ubB);
  UBYTE ubI = (ULONG(ubR) + ULONG(ubG) + ULONG(ubB)) / 3;
  FLOAT fReflectionDistance = sqrt(fReflectionDirI * fReflectionDirI + fReflectionDirJ * fReflectionDirJ);
  FLOAT fOfCenterFadeFactor = 1.0f - 2.0f * fReflectionDistance / fScreenSizeI;
  fOfCenterFadeFactor = Max(fOfCenterFadeFactor, 0.0f);


  // RT: ignore LFF_HAZE / LFF_FOG
  FLOAT fFogHazeFade = 1.0f;


  CStaticArray<COneLensFlare> &aolf = plsLight->ls_plftLensFlare->lft_aolfFlares;
  INDEX ctReflections = aolf.Count();
  if (_wrpWorldRenderPrefs.wrp_lftLensFlares == CWorldRenderPrefs::LFT_SINGLE_FLARE)
  {
    ctReflections = 1;
  }

  for (INDEX iReflection = 0; iReflection < ctReflections; iReflection++)
  {
    COneLensFlare &olf = aolf[iReflection];
    // calculate its fading factors
    FLOAT fFadeFactor = lfi_fFadeFactor * IntensityAtDistance(
      plsLight->ls_rFallOff * olf.oft_fFallOffFactor,
      plsLight->ls_rHotSpot * olf.oft_fFallOffFactor,
      lfi_fDistance);
    fFadeFactor *= fFogHazeFade;
    if (olf.olf_ulFlags & OLF_FADEOFCENTER)
    {
      fFadeFactor *= fOfCenterFadeFactor;
    }

    FLOAT fSizeIFactor = fScreenSizeI;
    if (olf.olf_ulFlags & OLF_FADESIZE)
    {
      fSizeIFactor *= fFadeFactor;
    }
    FLOAT fIntensityFactor = olf.olf_fLightAmplification;
    if (olf.olf_ulFlags & OLF_FADEINTENSITY)
    {
      fIntensityFactor *= fFadeFactor;
    }

    // calculate flare size
    FLOAT fSizeI = olf.olf_fSizeIOverScreenSizeI * fSizeIFactor;
    FLOAT fSizeJ = olf.olf_fSizeJOverScreenSizeI * fSizeIFactor;

    // skip if this flare is invisible
    if (fIntensityFactor < 0.01f || fSizeI < 2.0f || fSizeJ < 2.0f) continue;

    // determine color
    FLOAT fThisR = (ubR + (FLOAT(ubI) - ubR) * olf.olf_fLightDesaturation) * fIntensityFactor;
    FLOAT fThisG = (ubG + (FLOAT(ubI) - ubG) * olf.olf_fLightDesaturation) * fIntensityFactor;
    FLOAT fThisB = (ubB + (FLOAT(ubI) - ubB) * olf.olf_fLightDesaturation) * fIntensityFactor;
    UBYTE ubThisR = Min(fThisR, 255.0f);
    UBYTE ubThisG = Min(fThisG, 255.0f);
    UBYTE ubThisB = Min(fThisB, 255.0f);
    COLOR colBlending = RGBToColor(ubThisR, ubThisG, ubThisB);

    // render the flare
    UploadLensFlare(
      pointToCheck,
      &olf.olf_toTexture,
      fLightI + olf.olf_fReflectionPosition * fReflectionDirI,
      fLightJ + olf.olf_fReflectionPosition * fReflectionDirJ,
      fSizeI, fSizeJ,
      olf.olf_aRotationFactor * fIPositionFactor,
      colBlending,
      fScreenSizeI, fScreenSizeJ);
  }


  // RT: ignore glare, doesn't look right
  return;


  // if screen glare is on
  CLensFlareType &lft = *plsLight->ls_plftLensFlare;
  FLOAT fGlearCompression = lft.lft_fGlareCompression;
  if (_wrpWorldRenderPrefs.wrp_lftLensFlares >= CWorldRenderPrefs::LFT_REFLECTIONS_AND_GLARE && lft.lft_fGlareIntensity > 0.01f)
  {
    // calculate glare factor for current position
    FLOAT fIntensity = IntensityAtDistance(plsLight->ls_rFallOff * lft.lft_fGlareFallOffFactor,
                                           plsLight->ls_rHotSpot * lft.lft_fGlareFallOffFactor,
                                           lfi_fDistance);
    FLOAT fCenterFactor = (1 - fOfCenterFadeFactor);
    FLOAT fGlare = lft.lft_fGlareIntensity * fIntensity
      * (exp(1.0f / (1.0f + fGlearCompression * fCenterFactor * fCenterFactor)) - 1.0f) / (exp(1.0f) - 1.0f);
    ULONG ulGlareA = ClampUp(NormFloatToByte(fGlare), 255UL);
    // if there is any relevant glare
    if (ulGlareA > 1)
    {
      // calculate glare color
      FLOAT fGlareR = (ubR + (FLOAT(ubI) - ubR) * lft.lft_fGlareDesaturation);
      FLOAT fGlareG = (ubG + (FLOAT(ubI) - ubG) * lft.lft_fGlareDesaturation);
      FLOAT fGlareB = (ubB + (FLOAT(ubI) - ubB) * lft.lft_fGlareDesaturation);
      FLOAT fMax = Max(fGlareR, Max(fGlareG, fGlareB));
      FLOAT fBrightFactor = 255.0f / fMax;
      fGlareR *= fBrightFactor;
      fGlareG *= fBrightFactor;
      fGlareB *= fBrightFactor;
      ULONG ulGlareR = ClampUp(FloatToInt(fGlareR), 255L);
      ULONG ulGlareG = ClampUp(FloatToInt(fGlareG), 255L);
      ULONG ulGlareB = ClampUp(FloatToInt(fGlareB), 255L);
      // add the glare to screen blending
      pdpDrawPort->dp_ulBlendingRA += ulGlareR * ulGlareA;
      pdpDrawPort->dp_ulBlendingGA += ulGlareG * ulGlareA;
      pdpDrawPort->dp_ulBlendingBA += ulGlareB * ulGlareA;
      pdpDrawPort->dp_ulBlendingA += ulGlareA;
    }
  }
}

static void RT_AddLight(const CLightSource *plsLight, SSRT::Scene *pScene)
{
  ASSERT(plsLight != NULL);


  if (plsLight->ls_plftLensFlare != NULL)
  {
    AddLensFlare(plsLight, pScene);
  }


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
    light.color = RT_GetDirectionalLightColor(plsLight) * 
                  _srtGlobals.srt_fSunIntensity * 
                  (1.0 - pScene->GetCustomInfo()->GetCloudsOpacity());

    pScene->AddLight(light);
  }
  else
  {
    bool isDynamic = (plsLight->ls_ulFlags & LSF_DYNAMIC) || pScene->GetCustomInfo()->IsLightForceDynamic(plsLight);

    if (isDynamic && pScene->GetCustomInfo()->IsDynamicSphericalLightIgnored(plsLight))
    {
      return;
    }

    if (!isDynamic && pScene->GetCustomInfo()->IsSphericalLightIgnored(plsLight, _srtGlobals.srt_fPotentialLightSphDistanceThreshold))
    {
      // add it to the list of ignored lights, it will be used
      // to match light sources from fire textures;
      // it is done because a good way for determination of
      // "real" light sources (like torches, rockets) still wasn't found
      RT_IgnoredLights.Push() = plsLight;

      return;
    }

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

      if (isDynamic && pScene->GetCustomInfo()->IsLightOffsetFixEnabled(plsLight))
      {
        position = RT_FixSphericalLightPosition(pEn, position, _srtGlobals.srt_fLightSphPolygonOffset, pScene->GetWorld());
      }
    }

    float intensity =
      isMuzzleFlash ? _srtGlobals.srt_fMuzzleLightIntensity :
      isDynamic ? _srtGlobals.srt_fDynamicLightSphIntensity :
      _srtGlobals.srt_fOriginalLightSphIntensity;

    RT_AddModifiedSphereLightToScene(plsLight,
                                     entityID,
                                     position, RT_GetSphericalLightColor(plsLight) * intensity,
                                     plsLight->ls_rHotSpot, plsLight->ls_rFallOff,
                                     isDynamic, false, isMuzzleFlash,
                                     pScene);
  }
}


static void RT_TryAddPotentialLight(CEntity *pEn, SSRT::Scene *pScene)
{
  FLOAT3D originalPos = pEn->GetLerpedPlacement().pl_PositionVector;

  INDEX closestLtIndex = -1;
  float closestDist = _srtGlobals.srt_fPotentialLightSphDistanceThreshold;
  FLOAT3D closestPos;
  FLOAT3D pos;


  bool bForced = pScene->GetCustomInfo()->IsPotentialForcedAdd(originalPos);

  if (bForced)
  {
    // just any
    const FLOAT3D color = { 1.0f, 0.7f, 0.35f };

    float falloffDistance = _srtGlobals.srt_fPotentialLightSphFalloffDefault;

    if (pEn->en_pmoModelObject != nullptr)
    {
      falloffDistance *= pEn->en_pmoModelObject->mo_Stretch.Length();

      FLOATaabbox3D aabb;
      pEn->GetBoundingBox(aabb);

      originalPos = aabb.Center();
    }

    // if there is no nearest light, then just use any params
    float hotspotDistance = 1.0f;

    RT_AddModifiedSphereLightToScene(nullptr,
                                     pEn->en_ulID,
                                     originalPos, color * _srtGlobals.srt_fPotentialLightSphIntensity,
                                     hotspotDistance, falloffDistance,
                                     false, true, false,
                                     pScene);

    return;
  }


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

  // if found
  if (closestLtIndex != -1 && !bForced)
  {
    const CLightSource *plsLight = RT_IgnoredLights[closestLtIndex];

    bool isDynamic = plsLight->ls_ulFlags & LSF_DYNAMIC;

    RT_AddModifiedSphereLightToScene(nullptr,
                                     pEn->en_ulID,
                                     closestPos, RT_GetSphericalLightColor(plsLight) * _srtGlobals.srt_fPotentialLightSphIntensity,
                                     plsLight->ls_rHotSpot,
                                     plsLight->ls_rFallOff,
                                     isDynamic, true, false,
                                     pScene);
  }
}


void RT_ProcessModelLights(CEntity *penModel, SSRT::Scene *pScene)
{
  const CLightSource *pls = penModel->GetLightSource();

  if (pls != NULL)
  {
    ASSERT(penModel == pls->ls_penEntity);
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
