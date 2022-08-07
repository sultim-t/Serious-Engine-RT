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
#include "Scene.h"

#include <algorithm>

#include <Engine/Base/Shell.h>
#include <Engine/World/World.h>
#include <Engine/World/WorldSettings.h>
#include <Engine/Light/LightSource.h>

#include <Engine/Templates/DynamicContainer.cpp>

#include "RTProcessing.h"
#include "Utils.h"
#define DUMP_GEOMETRY_TO_OBJ 0

#include "GeometryExporter.h"

#include "SSRTGlobals.h"
extern SSRT::SSRTGlobals _srtGlobals;

SSRT::Scene::Scene(RgInstance _instance, CWorld *_pWorld, TextureUploader *_pTextureUploader)
:
  instance(_instance),
  pTextureUploader(_pTextureUploader),
  pSceneBrushes(new SceneBrushes(_instance, _pWorld, _pTextureUploader)),
  pCustomInfo(new CustomInfo(_pWorld)),
  pWorld(_pWorld),
  worldName(_pWorld->GetName()),
  pViewerEntity(nullptr)
{
  RT_SetCustomInfoForTextures(pCustomInfo);

  ASSERT(pWorld != nullptr);
#ifndef NDEBUG
  CPrintF("SSRT scene was created.\n");
#endif // !NDEBUG

  // upload static geometry (brushes)
  ProcessBrushes();
}

SSRT::Scene::~Scene()
{
#ifndef NDEBUG
  CPrintF("SSRT scene was deleted.\n");
#endif // !NDEBUG

  RT_SetCustomInfoForTextures(nullptr);

  delete pSceneBrushes;
  delete pCustomInfo;

  // submit empty scene to destroy current
  RgResult r = rgBeginStaticGeometries(instance);
  RG_CHECKERROR(r);
  r = rgSubmitStaticGeometries(instance);
  RG_CHECKERROR(r);
}

void SSRT::Scene::Update(const CWorldRenderingInfo &info)
{
  this->pViewerEntity = info.pViewerEntity;
  this->cameraPosition = info.cameraPosition;
  this->cameraRotation = info.cameraRotation;
  this->currentFOV = info.fovH;
  this->pdpDrawport = info.pDrawport;
  this->prSEProjection = info.prProjection;
  this->prSEProjectionBackground = info.prProjectionBackground;

  isCameraInHaze = false;

  // clear previous data
  sphLights.clear();
  dirLights.clear();
  firstPersonFlashlight.isAdded = false;
  thirdPersonFlashlight.isAdded = false;

  warpPortals.clear();

  pCustomInfo->Update(this->cameraPosition);

  // upload dynamic geometry (models)
  // and scan for movable geometry
  ProcessDynamicGeometry();
}

void SSRT::Scene::ResetOnGameStop()
{
  warpPortals.clear();
  pSceneBrushes->ResetOnGameStop();
}

const CTString &SSRT::Scene::GetWorldName() const
{
  return worldName;
}

const CWorld *SSRT::Scene::GetWorld() const
{
  return pWorld;
}

CDrawPort *SSRT::Scene::GetDrawPort()
{
  return pdpDrawport;
}

CAnyProjection3D &SSRT::Scene::GetSEProjection()
{
  return prSEProjection;
}

CAnyProjection3D &SSRT::Scene::GetSEProjectionBackground()
{
  return prSEProjectionBackground;
}

CWorld *SSRT::Scene::GetWorld()
{
  return pWorld;
}

FLOAT3D SSRT::Scene::GetBackgroundViewerPosition() const
{
  if (pWorld->GetBackgroundViewer() == nullptr)
  {
    return FLOAT3D(0, 0, 0);
  }

  return pWorld->GetBackgroundViewer()->GetLerpedPlacement().pl_PositionVector;
}

ANGLE3D SSRT::Scene::GetBackgroundViewerOrientationAngle() const
{
  if (pWorld->GetBackgroundViewer() == nullptr)
  {
    return ANGLE3D(0, 0, 0);
  }

  return pWorld->GetBackgroundViewer()->GetLerpedPlacement().pl_OrientationAngle;
}

void SSRT::Scene::AddNearestToCameraPortalInfo() const
{
  float fNearestSqDist = FLT_MAX;

  bool bFound = false;
  CMirrorParameters mFound = {};

  for (const auto &w : warpPortals)
  {
    CMirrorParameters m;
    if (w.penBrush->GetMirror(w.iMirrorType, m))
    {
      FLOAT3D vD = m.mp_plWarpIn.pl_PositionVector - cameraPosition;
      float fSqDist = vD % vD;

      if (fNearestSqDist > fSqDist)
      {
        fNearestSqDist = fSqDist;

        bFound = true;
        mFound = m;
      }
    }
  }

  if (bFound)
  {
    FLOATmatrix3D mOutRotation;
    MakeRotationMatrix(mOutRotation, mFound.mp_plWarpOut.pl_OrientationAngle);
    
    FLOAT3D vOutDir = FLOAT3D(0.0f, 0.0f, -1.0f) * mOutRotation;
    FLOAT3D vOutUp = FLOAT3D(0.0f, 1.0f, 0.0f) * mOutRotation;

    RgPortalUploadInfo info = {};
    info.portalIndex = 0;
    memcpy(info.inPosition.data, &mFound.mp_plWarpIn.pl_PositionVector, 3 * sizeof(float));
    memcpy(info.outPosition.data, &mFound.mp_plWarpOut.pl_PositionVector, 3 * sizeof(float));
    memcpy(info.outDirection.data, &vOutDir, 3 * sizeof(float));
    memcpy(info.outUp.data, &vOutUp, 3 * sizeof(float));

    RgResult r = rgUploadPortal(instance, &info);
    RG_CHECKERROR(r);
  }
}

bool SSRT::Scene::HasBrushWarpPortal(CEntity *penBrush) const
{
  if (penBrush == nullptr)
  {
    return false;
  }

  for (const auto &p : warpPortals)
  {
    if (p.penBrush->en_ulID == penBrush->en_ulID)
    {
      return true;
    }
  }

  return false;
}

bool SSRT::Scene::IsCameraInHaze() const
{
  return isCameraInHaze && pCustomInfo->CanHazeBeApplied();
}

bool SSRT::Scene::IsBrushMovable(CEntity *penBrush)
{
  ASSERT(penBrush->GetRenderType() == CEntity::RenderType::RT_BRUSH);

  // from Entities/Common/Flags.h
  constexpr ULONG EPF_BRUSH_MOVING =
    EPF_ONBLOCK_PUSH | EPF_RT_SYNCHRONIZED |
    EPF_ABSOLUTETRANSLATE | EPF_NOACCELERATION | EPF_MOVABLE;

  return 
    (penBrush->en_ulPhysicsFlags & EPF_BRUSH_MOVING) || 
    (penBrush->en_penParent != nullptr && penBrush->en_penParent->en_ulPhysicsFlags & EPF_BRUSH_MOVING) ||
    (penBrush->en_penParent != nullptr && penBrush->en_penParent->en_penParent != nullptr && penBrush->en_penParent->en_penParent->en_ulPhysicsFlags & EPF_BRUSH_MOVING);
}

const SSRT::CustomInfo *SSRT::Scene::GetCustomInfo() const
{
  ASSERT(pCustomInfo != nullptr);
  return pCustomInfo;
}

SSRT::CustomInfo *SSRT::Scene::GetCustomInfo()
{
  ASSERT(pCustomInfo != nullptr);
  return pCustomInfo;
}

void SSRT::Scene::AddModel(const CModelGeometry &model)
{
  if (model.vertices == nullptr || model.vertexCount == 0 || model.indices == nullptr || model.indexCount == 0)
  {
    return;
  }

  float metallic = 0.0f;
  float roughness = 1.0f;

  if (model.isSpecular)
  {
    roughness = _srtGlobals.srt_fModelSpecularRoughnessDefault;
    metallic = _srtGlobals.srt_fModelSpecularMetallicDefault;
  }
  else if (model.isReflective)
  {
    roughness = _srtGlobals.srt_fModelReflectiveRoughnessDefault;
    metallic = _srtGlobals.srt_fModelReflectiveMetallicDefault;
  }

  if (!model.isRasterized)
  {
    RgGeometryUploadInfo info = {};
    info.uniqueID = model.GetUniqueID();
    info.geomType = RG_GEOMETRY_TYPE_DYNAMIC;
    info.passThroughType = model.passThroughType;
    info.visibilityType = model.visibilityType;
    info.vertexCount = model.vertexCount;
    info.pVertices = model.vertices;
    info.indexCount = model.indexCount;
    info.pIndices = (const uint32_t*)model.indices;

    info.flags = 0;
    if (model.generateNormals)
    {
      info.flags |= RG_GEOMETRY_UPLOAD_GENERATE_NORMALS_BIT;
    }
    if (model.invertedNormals)
    {
      info.flags |= RG_GEOMETRY_UPLOAD_GENERATE_INVERTED_NORMALS_BIT;
    }

    info.defaultMetallicity = metallic;
    info.defaultRoughness = roughness;
    info.defaultEmission = model.isEmissive ? _srtGlobals.srt_fEmissionForFullbright : 0.0f;

    info.layerColors[0] = { model.color(1), model.color(2), model.color(3), model.color(4) };
    info.layerBlendingTypes[0] = RG_GEOMETRY_MATERIAL_BLEND_TYPE_OPAQUE;

    info.geomMaterial =
    {
      pTextureUploader->GetMaterial(model.textures[0], model.textureFrames[0]),
      RG_NO_MATERIAL,
      RG_NO_MATERIAL,
    };

    Utils::CopyTransform(info.transform, model);

    RgResult r = rgUploadGeometry(instance, &info);
    RG_CHECKERROR(r);
  }
  else
  {
    RgRasterizedGeometryUploadInfo info = {};
    info.vertexCount = model.vertexCount;
    info.pVertices = model.vertices;
    info.indexCount = model.indexCount;
    info.pIndices = model.indices;
    info.color = { model.color(1), model.color(2), model.color(3), model.color(4) };
    info.material = pTextureUploader->GetMaterial(model.textures[0], model.textureFrames[0]);
    info.pipelineState = model.blendEnable ? RG_RASTERIZED_GEOMETRY_STATE_BLEND_ENABLE : 0;
    info.pipelineState |= RG_RASTERIZED_GEOMETRY_STATE_DEPTH_TEST;
    info.blendFuncSrc = model.blendSrc;
    info.blendFuncDst = model.blendDst;
    info.renderType = model.isSky ? RG_RASTERIZED_GEOMETRY_RENDER_TYPE_SKY : RG_RASTERIZED_GEOMETRY_RENDER_TYPE_DEFAULT;

    Utils::CopyTransform(info.transform, model);

    RgResult r = rgUploadRasterizedGeometry(instance, &info, nullptr, nullptr);
    RG_CHECKERROR(r);
  }

  GeometryExporter::ExportGeometry(model);
}

void SSRT::Scene::AddParticles(const CParticlesGeometry &particles)
{
  if (particles.pVertexData == nullptr || particles.vertexCount == 0 || particles.pIndexData == nullptr || particles.indexCount == 0)
  {
    return;
  }

  RgRasterizedGeometryUploadInfo info = {};
  info.vertexCount = particles.vertexCount;
  info.pVertices = particles.pVertexData;
  info.indexCount = particles.indexCount;
  info.pIndices = particles.pIndexData;
  info.color = { 1, 1, 1, _srtGlobals.srt_fParticlesAlphaMultiplier };
  info.material = pTextureUploader->GetMaterial(particles.pTexture, particles.textureFrame);
  info.pipelineState = particles.blendEnable ? RG_RASTERIZED_GEOMETRY_STATE_BLEND_ENABLE : 0;
  info.pipelineState |= RG_RASTERIZED_GEOMETRY_STATE_DEPTH_TEST;
  info.blendFuncSrc = particles.blendSrc;
  info.blendFuncDst = particles.blendDst;
  info.renderType = particles.isSky ? RG_RASTERIZED_GEOMETRY_RENDER_TYPE_SKY : RG_RASTERIZED_GEOMETRY_RENDER_TYPE_DEFAULT;

  if (particles.isSky)
  {
    FLOATmatrix3D bv;
    MakeInverseRotationMatrix(bv, GetBackgroundViewerOrientationAngle());

    Utils::CopyTransform(info.transform, { 0, 0, 0 }, bv);
  }
  else
  {
    Utils::CopyTransform(info.transform, particles.absPosition, particles.absRotation);
  }

  RgResult r = rgUploadRasterizedGeometry(instance, &info, nullptr, nullptr);
  RG_CHECKERROR(r);
}

void SSRT::Scene::AddBrush(const CBrushGeometry &brush)
{
  pSceneBrushes->RegisterBrush(brush);

  if (brush.isSky)
  {
    pCustomInfo->OnSkyBrushAdd(brush);
  }

  GeometryExporter::ExportGeometry(brush);
}

void SSRT::Scene::AddLight(const CSphereLight &sphLt)
{
  if (sphLights.size() >= (uint64_t)_srtGlobals.srt_iLightSphMaxCount)
  {
    return;
  }

  FLOAT3D c = sphLt.color * _srtGlobals.srt_fLightSphGlobalIntensityMultiplier;

  {
    float t = Clamp(_srtGlobals.srt_fLightSphGlobalFalloffToIntensity, 0.0f, 1.0f);
    c *= Lerp(1.0f, Max(0.0f, sphLt.falloffDistance), t);
  }

  RgSphericalLightUploadInfo info = {};
  info.uniqueID = sphLt.entityID;
  info.color = { c(1), c(2), c(3) };
  info.position = { sphLt.absPosition(1), sphLt.absPosition(2), sphLt.absPosition(3) };
  info.radius = sphLt.radius;

  RgResult r = rgUploadSphericalLight(instance, &info);
  RG_CHECKERROR(r);

  sphLights.push_back(sphLt);

  pCustomInfo->OnSphereLightAdd(sphLt);
}

void SSRT::Scene::AddLight(const CDirectionalLight &dirLt)
{
  RgDirectionalLightUploadInfo info = {};
  info.uniqueID = dirLt.entityID;
  info.color = {
    dirLt.color(1) * _srtGlobals.srt_fFixSunIntensityGlobal,
    dirLt.color(2) * _srtGlobals.srt_fFixSunIntensityGlobal,
    dirLt.color(3) * _srtGlobals.srt_fFixSunIntensityGlobal };
  info.direction = { dirLt.direction(1), dirLt.direction(2), dirLt.direction(3) };
  info.angularDiameterDegrees = _srtGlobals.srt_fSunAngularDiameter;

  RgResult r = rgUploadDirectionalLight(instance, &info);
  RG_CHECKERROR(r);

  dirLights.push_back(dirLt);
}

void SSRT::Scene::AddLight(const CSpotLight &spotLt)
{
  firstPersonFlashlight.isAdded |= spotLt.isFirstPerson;
  thirdPersonFlashlight.isAdded |= !spotLt.isFirstPerson;

  FLOAT3D color = _srtGlobals.srt_vFlashlightColor * _srtGlobals.srt_fFlashlightIntensity;

  auto &sp = spotLt.isFirstPerson ? firstPersonFlashlight.spotlightInfo : thirdPersonFlashlight.spotlightInfo;

  sp.position = { spotLt.absPosition(1), spotLt.absPosition(2), spotLt.absPosition(3) };
  sp.direction = { spotLt.direction(1), spotLt.direction(2), spotLt.direction(3) };
  sp.color = { color(1), color(2), color(3) };

  sp.angleOuter = RadAngle(_srtGlobals.srt_fFlashlightAngleOuter);
  sp.angleInner = RadAngle(_srtGlobals.srt_fFlashlightAngleInner);
  sp.radius = _srtGlobals.srt_fFlashlightRadius;
}

void SSRT::Scene::UpdateBrush(CEntity *pEntity)
{
  ASSERT(pEntity->en_RenderType == CEntity::RT_BRUSH);
  pSceneBrushes->Update(pEntity, this);
}

void SSRT::Scene::UpdateBrushNonStaticTexture(CTextureData *pTexture, uint32_t textureFrameIndex)
{
  pTextureUploader->GetMaterial(pTexture, textureFrameIndex);
}

void SSRT::Scene::UpdateBrushTexCoords(const CUpdateTexCoordsInfo &info)
{
  pSceneBrushes->UpdateBrushTexCoords(info);
}

void SSRT::Scene::AddWarpPortal(CEntity *penBrush, INDEX iMirrorType)
{
  if (penBrush == nullptr)
  {
    return;
  }

  WarpPortalState st = {};
  st.penBrush = penBrush;
  st.iMirrorType = iMirrorType;

  warpPortals.insert(st);
}

void SSRT::Scene::SetCameraIsInHaze()
{
  isCameraInHaze = true;
}

void SSRT::Scene::ProcessFirstPersonModel(const CFirstPersonModelInfo &info, ULONG entityId)
{
  RT_AddFirstPersonModel(info.modelObject, info.renderModel, entityId, this);
}

void SSRT::Scene::ProcessBrushes()
{
  RgResult r = rgBeginStaticGeometries(instance);
  RG_CHECKERROR(r);

  // find all brushes, their geomtry won't change,
  // but movable brushes have dynamic transformation 
  // and they'll be updated on a frame start

  FOREACHINDYNAMICCONTAINER(pWorld->wo_cenEntities, CEntity, iten)
  {
    // if it is any brush
    if (iten->en_RenderType == CEntity::RT_BRUSH)
    {
      // add all of its sectors
      RT_AddBrushEntity(&iten.Current(), this);
    }
  }

  RT_BrushProcessingClear();

  // submit
  r = rgSubmitStaticGeometries(instance);
  RG_CHECKERROR(r);

#ifndef NDEBUG
  CPrintF("SSRT scene's brushes were uploaded.\n");
#endif // !NDEBUG
}

void SSRT::Scene::ProcessDynamicGeometry()
{
  if (pViewerEntity == nullptr)
  {
    return;
  }

  FOREACHINDYNAMICCONTAINER(pWorld->wo_cenEntities, CEntity, iten)
  {
    // always update background
    if (iten->en_ulFlags & ENF_BACKGROUND)
    {
      if (iten->en_RenderType == CEntity::RT_BRUSH)
      {
        UpdateBrush(iten);
      }
      else if (iten->en_RenderType == CEntity::RT_MODEL)
      {
        RT_AddModelEntity(iten, this);
        RT_AddParticlesForEntity(iten, this);
      }
      else if (iten->en_RenderType == CEntity::RT_EDITORMODEL)
      {
        RT_AddParticlesForEntity(iten, this);
      }
    }

    // always add directional light source
    if (iten->GetLightSource() != nullptr)
    {
      if (iten->GetLightSource()->ls_ulFlags & LSF_DIRECTIONAL)
      {
        RT_AddModelEntity(iten, this);
      }
    }
  }

  RT_AddModelEntitiesAroundViewer(this);
  RT_AddPotentialLightSources(this);

  RT_PrintBrushPolygonInfo(this);
}

void SSRT::Scene::OnFrameEnd(bool isCameraFirstPerson)
{
  // spotlights are processed, add one of them;
  // it's done here because a way to determine,
  // if the camera is first or third person, wasn't found
  RgSpotLightUploadInfo info = {};
  bool added = false;

  if (isCameraFirstPerson && firstPersonFlashlight.isAdded)
  {
    info = firstPersonFlashlight.spotlightInfo;
    added = true;
  }
  else if (thirdPersonFlashlight.isAdded)
  {
    info = thirdPersonFlashlight.spotlightInfo;
    added = true;
  }

  if (added)
  {
    float f = pCustomInfo->GetFlashlightIntensityForLevel(cameraPosition);
    info.color.data[0] *= f;
    info.color.data[1] *= f;
    info.color.data[2] *= f;

    float relativeFov = Clamp(currentFOV / 75.0f, 1.0f, 1.5f);
    info.angleOuter *= relativeFov;

    info.uniqueID = UINT64_MAX;

    RgResult r = rgUploadSpotLight(instance, &info);
    RG_CHECKERROR(r);
  }
}

CEntity *SSRT::Scene::GetViewerEntity() const
{
  return pViewerEntity;
}

ULONG SSRT::Scene::GetViewerEntityID() const
{
  if (pViewerEntity == nullptr)
  {
    return UINT32_MAX;
  }
    
  return pViewerEntity->en_ulID;
}

const FLOAT3D &SSRT::Scene::GetCameraPosition() const
{
  return cameraPosition;
}

const FLOATmatrix3D &SSRT::Scene::GetCameraRotation() const
{
  return cameraRotation;
}