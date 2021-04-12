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

#include <Engine/Templates/DynamicContainer.cpp>

#include "RTProcessing.h"
#include "Utils.h"
#define DUMP_GEOMETRY_TO_OBJ 0

#include "GeometryExporter.h"


extern FLOAT srt_fDefaultSpecularMetallic = 0.15f;
extern FLOAT srt_fDefaultSpecularRoughness = 0.05f;

extern FLOAT srt_fDefaultReflectiveMetallic = 1.0f;
extern FLOAT srt_fDefaultReflectiveRoughness = 0.0f;

extern INDEX srt_bEnableViewerShadows = 1;

extern FLOAT srt_fLightDirectionalIntensityMultiplier = 10.0f;
extern FLOAT srt_fLightDirectionalSaturation = 1.0f;
extern FLOAT srt_fLightDirectionalColorPow = 2.2f;
extern FLOAT srt_fLightDirectionalAngularDiameter = 0.5f;

extern INDEX srt_iLightSphericalMaxCount = 16;
extern INDEX srt_iLightSphericalHSVThresholdHLower = 20;
extern INDEX srt_iLightSphericalHSVThresholdHUpper = 45;
extern INDEX srt_iLightSphericalHSVThresholdVLower = 80;
extern INDEX srt_iLightSphericalHSVThresholdVUpper = 255;
extern FLOAT srt_fLightSphericalIntensityMultiplier = 1.0f;
extern FLOAT srt_fLightSphericalSaturation = 1.0f;
extern FLOAT srt_fLightSphericalColorPow = 2.2f;
extern FLOAT srt_fLightSphericalRadiusMultiplier = 0.1f;
extern FLOAT srt_fLightSphericalFalloffMultiplier = 1.0f;
extern INDEX srt_bLightSphericalIgnoreEditorModels = 0;

extern INDEX srt_bModelUseOriginalNormals = 1;

void SSRT::Scene::InitShellVariables()
{
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDefaultSpecularMetallic;", &srt_fDefaultSpecularMetallic);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDefaultSpecularRoughness;", &srt_fDefaultSpecularRoughness);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDefaultReflectiveMetallic;", &srt_fDefaultReflectiveMetallic);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDefaultReflectiveRoughness;", &srt_fDefaultReflectiveRoughness);

  _pShell->DeclareSymbol("persistent user INDEX srt_bEnableViewerShadows;", &srt_bEnableViewerShadows);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightDirectionalIntensityMultiplier;", &srt_fLightDirectionalIntensityMultiplier);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightDirectionalSaturation;", &srt_fLightDirectionalSaturation);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightDirectionalColorPow;", &srt_fLightDirectionalColorPow);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightDirectionalAngularDiameter;", &srt_fLightDirectionalAngularDiameter);

  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalMaxCount;", &srt_iLightSphericalMaxCount);
  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalHSVThresholdHLower;", &srt_iLightSphericalHSVThresholdHLower);
  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalHSVThresholdHUpper;", &srt_iLightSphericalHSVThresholdHUpper);
  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalHSVThresholdVLower;", &srt_iLightSphericalHSVThresholdVLower);
  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalHSVThresholdVUpper;", &srt_iLightSphericalHSVThresholdVUpper);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightSphericalIntensityMultiplier;", &srt_fLightSphericalIntensityMultiplier);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightSphericalSaturation;", &srt_fLightSphericalSaturation);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightSphericalColorPow;", &srt_fLightSphericalColorPow);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightSphericalRadiusMultiplier;", &srt_fLightSphericalRadiusMultiplier);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightSphericalFalloffMultiplier;", &srt_fLightSphericalFalloffMultiplier);
  _pShell->DeclareSymbol("persistent user INDEX srt_bLightSphericalIgnoreEditorModels;", &srt_bLightSphericalIgnoreEditorModels);

  _pShell->DeclareSymbol("persistent user INDEX srt_bModelUseOriginalNormals;", &srt_bModelUseOriginalNormals);
}

void SSRT::Scene::NormalizeShellVariables()
{
  srt_fDefaultSpecularMetallic    = Clamp(srt_fDefaultSpecularMetallic, 0.0f, 1.0f);
  srt_fDefaultSpecularRoughness   = Clamp(srt_fDefaultSpecularRoughness, 0.0f, 1.0f);
  srt_fDefaultReflectiveMetallic  = Clamp(srt_fDefaultReflectiveMetallic, 0.0f, 1.0f);
  srt_fDefaultReflectiveRoughness = Clamp(srt_fDefaultReflectiveRoughness, 0.0f, 1.0f);

  srt_bEnableViewerShadows = !!srt_bEnableViewerShadows;

  srt_fLightDirectionalIntensityMultiplier  = Max(srt_fLightDirectionalIntensityMultiplier, 0.0f);
  srt_fLightDirectionalSaturation           = Max(srt_fLightDirectionalSaturation, 0.0f);
  srt_fLightDirectionalColorPow             = Max(srt_fLightDirectionalColorPow, 0.0f);
  srt_fLightDirectionalAngularDiameter      = Max(srt_fLightDirectionalAngularDiameter, 0.0f);

  srt_iLightSphericalHSVThresholdHLower     = Clamp<INDEX>(srt_iLightSphericalHSVThresholdHLower, 0, 255);
  srt_iLightSphericalHSVThresholdHUpper     = Clamp<INDEX>(srt_iLightSphericalHSVThresholdHUpper, 0, 255);
  srt_iLightSphericalHSVThresholdVLower     = Clamp<INDEX>(srt_iLightSphericalHSVThresholdVLower, 0, 255);
  srt_iLightSphericalHSVThresholdVUpper     = Clamp<INDEX>(srt_iLightSphericalHSVThresholdVUpper, 0, 255);
  srt_fLightSphericalIntensityMultiplier    = Max(srt_fLightSphericalIntensityMultiplier, 0.0f);
  srt_fLightSphericalSaturation             = Max(srt_fLightSphericalSaturation, 0.0f);
  srt_fLightSphericalColorPow               = Max(srt_fLightSphericalColorPow, 0.0f);
  srt_fLightSphericalRadiusMultiplier       = Max(srt_fLightSphericalRadiusMultiplier, 0.0f);
  srt_fLightSphericalFalloffMultiplier      = Max(srt_fLightSphericalFalloffMultiplier, 0.0f);

  srt_bLightSphericalIgnoreEditorModels = !!srt_bLightSphericalIgnoreEditorModels;
  srt_bModelUseOriginalNormals = !!srt_bModelUseOriginalNormals;
}


SSRT::Scene::Scene(RgInstance _instance, CWorld *_pWorld, TextureUploader *_pTextureUploader)
:
  instance(_instance),
  pSceneBrushes(nullptr),
  pTextureUploader(_pTextureUploader),
  pWorld(_pWorld),
  viewerEntityID(UINT32_MAX),
  worldName(_pWorld->GetName())
{
  ASSERT(pWorld != nullptr);
  CPrintF("SSRT scene was created.\n");

  pSceneBrushes = new SceneBrushes(_instance, _pWorld, _pTextureUploader);

  // upload static geometry (brushes)
  ProcessBrushes();
}

SSRT::Scene::~Scene()
{
  CPrintF("SSRT scene was deleted.\n");

  // submit empty scene to destroy current
  RgResult r = rgStartNewScene(instance);
  RG_CHECKERROR(r);
  r = rgSubmitStaticGeometries(instance);
  RG_CHECKERROR(r);
}

void SSRT::Scene::Update(const CWorldRenderingInfo &info)
{
  this->viewerEntityID = info.viewerEntityID;
  this->viewerPosition = info.viewerPosition;
  this->viewerRotation = info.viewerRotation;

  NormalizeShellVariables();

  // clear previous data
  sphLights.clear();
  dirLights.clear();

  // upload dynamic geometry (models)
  // and scan for movable geometry
  ProcessDynamicGeometry();
}

const CTString &SSRT::Scene::GetWorldName() const
{
  return worldName;
}

const CWorld *SSRT::Scene::GetWorld() const
{
  return pWorld;
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
    roughness = srt_fDefaultSpecularRoughness;
    metallic = srt_fDefaultSpecularMetallic;
  }
  else if (model.isReflective)
  {
    roughness = srt_fDefaultReflectiveRoughness;
    metallic = srt_fDefaultReflectiveMetallic;
  }

  if (!model.isRasterized)
  {
    RgGeometryUploadInfo info = {};
    info.uniqueID = model.GetUniqueID();
    info.geomType = RG_GEOMETRY_TYPE_DYNAMIC;
    info.passThroughType = model.passThroughType;
    info.visibilityType = model.visibilityType;
    info.vertexCount = model.vertexCount;
    info.vertexData = model.vertices;
    info.normalData = model.normals;
    info.texCoordLayerData[0] = model.texCoordLayers[0];
    info.texCoordLayerData[1] = nullptr;
    info.texCoordLayerData[2] = nullptr;
    info.defaultMetallicity = metallic;
    info.defaultRoughness = roughness;
    info.indexCount = model.indexCount;
    info.indexData = model.indices;

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
    bool isSky = model.visibilityType == RG_GEOMETRY_VISIBILITY_TYPE_SKYBOX;

    RgRasterizedGeometryVertexArrays vertInfo = {};
    vertInfo.vertexData = model.vertices;
    vertInfo.texCoordData = model.texCoordLayers[0];
    vertInfo.colorData = nullptr;
    vertInfo.vertexStride = sizeof(GFXVertex);
    vertInfo.texCoordStride = sizeof(GFXTexCoord);
    vertInfo.colorStride = sizeof(GFXColor);

    RgRasterizedGeometryUploadInfo info = {};
    info.vertexCount = model.vertexCount;
    info.arrays = &vertInfo;
    info.indexCount = model.indexCount;
    info.indexData = model.indices;
    info.color = { model.color(1), model.color(2), model.color(3), model.color(4) };
    info.material = pTextureUploader->GetMaterial(model.textures[0], model.textureFrames[0]);
    info.blendEnable = model.blendEnable;
    info.blendFuncSrc = model.blendSrc;
    info.blendFuncDst = model.blendDst;
    info.depthTest = RG_TRUE;
    info.renderType = isSky ? RG_RASTERIZED_GEOMETRY_RENDER_TYPE_SKY : RG_RASTERIZED_GEOMETRY_RENDER_TYPE_DEFAULT;

    Utils::CopyTransform(info.transform, model);

    RgResult r = rgUploadRasterizedGeometry(instance, &info, nullptr, nullptr);
    RG_CHECKERROR(r);
  }

  GeometryExporter::ExportGeometry(model);
}

void SSRT::Scene::AddParticles(const CParticlesGeometry &particles)
{
  if (particles.vertices == nullptr || particles.vertexCount == 0 || particles.indices == nullptr || particles.indexCount == 0)
  {
    return;
  }

  RgRasterizedGeometryVertexArrays vertInfo = {};
  vertInfo.vertexData = particles.vertices;
  vertInfo.texCoordData = particles.texCoordLayers[0];
  vertInfo.colorData = particles.colorData;
  vertInfo.vertexStride = sizeof(GFXVertex);
  vertInfo.texCoordStride = sizeof(GFXTexCoord);
  vertInfo.colorStride = sizeof(GFXColor);

  RgRasterizedGeometryUploadInfo info = {};
  info.vertexCount = particles.vertexCount;
  info.arrays = &vertInfo;
  info.indexCount = particles.indexCount;
  info.indexData = particles.indices;
  info.color = { 1, 1, 1, 1 };
  info.material = pTextureUploader->GetMaterial(particles.textures[0], particles.textureFrames[0]);
  info.blendEnable = particles.blendEnable;
  info.blendFuncSrc = particles.blendSrc;
  info.blendFuncDst = particles.blendDst;
  info.depthTest = RG_TRUE;
  info.renderType = RG_RASTERIZED_GEOMETRY_RENDER_TYPE_DEFAULT;

  Utils::CopyTransform(info.transform, particles);

  RgResult r = rgUploadRasterizedGeometry(instance, &info, nullptr, nullptr);
  RG_CHECKERROR(r);
}

void SSRT::Scene::AddBrush(const CBrushGeometry &brush)
{
  pSceneBrushes->RegisterBrush(brush);

  GeometryExporter::ExportGeometry(brush);
}

void SSRT::Scene::AddLight(const CSphereLight &sphLt)
{
  sphLights.push_back(sphLt);
}

void SSRT::Scene::AddLight(const CDirectionalLight &dirLt)
{
  RgResult r;

  RgDirectionalLightUploadInfo info = {};
  info.uniqueID = dirLt.entityID;
  info.color = { dirLt.color(1), dirLt.color(2), dirLt.color(3) };
  info.direction = { dirLt.direction(1), dirLt.direction(2), dirLt.direction(3) };
  info.angularDiameterDegrees = srt_fLightDirectionalAngularDiameter;

  r = rgUploadDirectionalLight(instance, &info);

  dirLights.push_back(dirLt);
}

void SSRT::Scene::UpdateBrushNonStaticTexture(CTextureData *pTexture, uint32_t textureFrameIndex)
{
  pTextureUploader->GetMaterial(pTexture, textureFrameIndex);
}

void SSRT::Scene::UpdateBrushTexCoords(const CUpdateTexCoordsInfo &info)
{
  pSceneBrushes->UpdateBrushTexCoords(info);
}

void SSRT::Scene::ProcessFirstPersonModel(const CFirstPersonModelInfo &info, ULONG entityId)
{
  RT_AddFirstPersonModel(info.modelObject, info.renderModel, entityId, this);
}

void SSRT::Scene::ProcessBrushes()
{
  RgResult r = rgStartNewScene(instance);
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

  CPrintF("SSRT scene's brushes were uploaded.\n");
}

void SSRT::Scene::ProcessDynamicGeometry()
{
  CEntity *viewer = nullptr;

  // check all movable brushes, models, light sources
  // and brushes that have non-static textures
  FOREACHINDYNAMICCONTAINER(pWorld->wo_cenEntities, CEntity, iten)
  {
    if (iten->en_ulID == viewerEntityID)
    {
      viewer = (CEntity *)iten;

      if (!srt_bEnableViewerShadows)
      {
        continue;
      }
    }

    if (iten->en_RenderType == CEntity::RT_MODEL || iten->en_RenderType == CEntity::RT_EDITORMODEL)
    {
      // add it as a model and scan for light sources
      RT_AddModelEntity(&iten.Current(), this);
    }
    else if (iten->en_RenderType == CEntity::RT_BRUSH)
    {
      pSceneBrushes->Update(iten, this);
    }
  }

  if (viewer != nullptr)
  {
    RT_AddAllParticles(pWorld, viewer, this);
  }



  static FLOAT3D v = {};
  v = viewerPosition;

  std::sort(sphLights.begin(), sphLights.end(), 
            [] (const CSphereLight &l1,  CSphereLight &l2)
            {
              auto d1 = l1.absPosition - v;
              auto d2 = l2.absPosition - v;

              return d1 % d1 < d2 % d2;
            });

  for (int i = 0; i < srt_iLightSphericalMaxCount && i < sphLights.size(); i++)
  {
    const CSphereLight &sphLt = sphLights[i];

    RgSphericalLightUploadInfo info = {};
    info.uniqueID = sphLt.entityID;
    info.color = { sphLt.color(1), sphLt.color(2), sphLt.color(3) };
    info.position = { sphLt.absPosition(1), sphLt.absPosition(2), sphLt.absPosition(3) };
    info.radius = Sqrt(sphLt.hotspotDistance) * srt_fLightSphericalRadiusMultiplier;
    info.falloffDistance = sphLt.faloffDistance * srt_fLightSphericalFalloffMultiplier;

    RgResult r = rgUploadSphericalLight(instance, &info);    
    RG_CHECKERROR(r);
  }
}

ULONG SSRT::Scene::GetViewerEntityID() const
{
  return viewerEntityID;
}

const FLOAT3D &SSRT::Scene::GetViewerPosition() const
{
  return viewerPosition;
}

const FLOATmatrix3D &SSRT::Scene::GetViewerRotation() const
{
  return viewerRotation;
}