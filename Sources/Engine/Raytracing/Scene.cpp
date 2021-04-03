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
extern FLOAT srt_fLightDirectionalAngularDiameter = 0.5f;

extern INDEX srt_iLightSphericalMaxCount = 16;
extern INDEX srt_iLightSphericalHSVThresholdHLower = 20;
extern INDEX srt_iLightSphericalHSVThresholdHUpper = 45;
extern INDEX srt_iLightSphericalHSVThresholdVLower = 80;
extern INDEX srt_iLightSphericalHSVThresholdVUpper = 255;
extern FLOAT srt_fLightSphericalIntensityMultiplier = 1.0f;
extern FLOAT srt_fLightSphericalSaturation = 1.0f;
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
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightDirectionalAngularDiameter;", &srt_fLightDirectionalAngularDiameter);

  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalMaxCount;", &srt_iLightSphericalMaxCount);
  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalHSVThresholdHLower;", &srt_iLightSphericalHSVThresholdHLower);
  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalHSVThresholdHUpper;", &srt_iLightSphericalHSVThresholdHUpper);
  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalHSVThresholdVLower;", &srt_iLightSphericalHSVThresholdVLower);
  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalHSVThresholdVUpper;", &srt_iLightSphericalHSVThresholdVUpper);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightSphericalIntensityMultiplier;", &srt_fLightSphericalIntensityMultiplier);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightSphericalSaturation;", &srt_fLightSphericalSaturation);
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
  srt_fLightDirectionalAngularDiameter      = Max(srt_fLightDirectionalAngularDiameter, 0.0f);

  srt_iLightSphericalHSVThresholdHLower     = Clamp<INDEX>(srt_iLightSphericalHSVThresholdHLower, 0, 255);
  srt_iLightSphericalHSVThresholdHUpper     = Clamp<INDEX>(srt_iLightSphericalHSVThresholdHUpper, 0, 255);
  srt_iLightSphericalHSVThresholdVLower     = Clamp<INDEX>(srt_iLightSphericalHSVThresholdVLower, 0, 255);
  srt_iLightSphericalHSVThresholdVUpper     = Clamp<INDEX>(srt_iLightSphericalHSVThresholdVUpper, 0, 255);
  srt_fLightSphericalIntensityMultiplier    = Max(srt_fLightSphericalIntensityMultiplier, 0.0f);
  srt_fLightSphericalSaturation             = Max(srt_fLightSphericalSaturation, 0.0f);
  srt_fLightSphericalRadiusMultiplier       = Max(srt_fLightSphericalRadiusMultiplier, 0.0f);
  srt_fLightSphericalFalloffMultiplier      = Max(srt_fLightSphericalFalloffMultiplier, 0.0f);

  srt_bLightSphericalIgnoreEditorModels = !!srt_bLightSphericalIgnoreEditorModels;
  srt_bModelUseOriginalNormals = !!srt_bModelUseOriginalNormals;
}


SSRT::Scene::Scene(RgInstance _instance, CWorld *_pWorld, TextureUploader *_textureUploader)
:
  instance(_instance),
  pWorld(_pWorld),
  viewerEntityID(UINT32_MAX),
  textureUploader(_textureUploader),
  worldName(_pWorld->GetName())
{
  ASSERT(pWorld != nullptr);
  CPrintF("SSRT scene was created.\n");

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

  // background view matrix
  float backgroundView[16];
  {
    const FLOAT3D &v = GetBackgroundViewerPosition();

    FLOATmatrix3D m;
    MakeInverseRotationMatrix(m, GetBackgroundViewerOrientationAngle());

    // inverse Y axis for Vulkan
    m(2, 1) = -m(2, 1);
    m(2, 2) = -m(2, 2);
    m(2, 3) = -m(2, 3);

    // 4th column of (T*M)^-1 = M^(-1)*T^(-1),
    FLOAT3D t =
    {
      -v % FLOAT3D(m(1, 1), m(1, 2), m(1, 3)),
      -v % FLOAT3D(m(2, 1), m(2, 2), m(2, 3)),
      -v % FLOAT3D(m(3, 1), m(3, 2), m(3, 3))
    };

    float *cm = backgroundView;
    cm[0] = m(1, 1);  cm[4] = m(1, 2);  cm[8] = m(1, 3);   cm[12] = v(1);
    cm[1] = m(2, 1);  cm[5] = m(2, 2);  cm[9] = m(2, 3);   cm[13] = v(2);
    cm[2] = m(3, 1);  cm[6] = m(3, 2);  cm[10] = m(3, 3);  cm[14] = v(3);
    cm[3] = 0;        cm[7] = 0;        cm[11] = 0;        cm[15] = 1;
  }

  extern void Svk_MatMultiply(float *result, const float *a, const float *b);
  Svk_MatMultiply(backgroundViewProj, backgroundView, info.projectionMatrix);


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
      textureUploader->GetMaterial(model.textures[0], model.textureFrames[0]),
      RG_NO_MATERIAL,
      RG_NO_MATERIAL,
    };

    Utils::CopyTransform(info.transform, model);

    RgResult r = rgUploadGeometry(instance, &info);
    RG_CHECKERROR(r);
  }
  else
  {
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
    info.material = textureUploader->GetMaterial(model.textures[0], model.textureFrames[0]);
    info.blendEnable = model.blendEnable;
    info.blendFuncSrc = model.blendSrc;
    info.blendFuncDst = model.blendDst;
    info.depthTest = RG_TRUE;
    info.renderToSwapchain = RG_FALSE;

    Utils::CopyTransform(info.transform, model);

    float *viewProj = 
      model.visibilityType == RG_GEOMETRY_VISIBILITY_TYPE_SKYBOX ?
      backgroundViewProj : nullptr;

    RgResult r = rgUploadRasterizedGeometry(instance, &info, viewProj, nullptr);
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
  info.material = textureUploader->GetMaterial(particles.textures[0], particles.textureFrames[0]);
  info.blendEnable = particles.blendEnable;
  info.blendFuncSrc = particles.blendSrc;
  info.blendFuncDst = particles.blendDst;
  info.depthTest = RG_TRUE;
  info.renderToSwapchain = RG_FALSE;

  Utils::CopyTransform(info.transform, particles);

  RgResult r = rgUploadRasterizedGeometry(instance, &info, nullptr, nullptr);
  RG_CHECKERROR(r);
}

void SSRT::Scene::AddBrush(const CBrushGeometry &brush)
{
  if (brush.vertices == nullptr || brush.vertexCount == 0 || brush.indices == nullptr || brush.indexCount == 0)
  {
    return;
  }

  if (!brush.isRasterized)
  {
    RgGeometryUploadInfo stInfo = {};
    stInfo.uniqueID = brush.GetUniqueID();
    stInfo.geomType = brush.isMovable ?
      RG_GEOMETRY_TYPE_STATIC_MOVABLE :
      RG_GEOMETRY_TYPE_STATIC;
    stInfo.passThroughType = brush.passThroughType;
    stInfo.visibilityType = brush.visibilityType;
    stInfo.vertexCount = brush.vertexCount;
    stInfo.vertexData = brush.vertices;
    stInfo.normalData = brush.normals;
    stInfo.defaultRoughness = 1.0f;
    stInfo.indexCount = brush.indexCount;
    stInfo.indexData = brush.indices;

    for (uint32_t i = 0; i < 3; i++)
    {
      stInfo.texCoordLayerData[i] = brush.texCoordLayers[i];
      stInfo.layerBlendingTypes[i] = brush.layerBlendings[i];
      stInfo.layerColors[i] = { brush.layerColors[i](1), brush.layerColors[i](2), brush.layerColors[i](3), brush.layerColors[i](4) };
    }

    stInfo.geomMaterial =
    {
      textureUploader->GetMaterial(brush.textures[0], brush.textureFrames[0]),
      textureUploader->GetMaterial(brush.textures[1], brush.textureFrames[1]),
      textureUploader->GetMaterial(brush.textures[2], brush.textureFrames[2]),
    };

    Utils::CopyTransform(stInfo.transform, brush);

    RgResult r = rgUploadGeometry(instance, &stInfo);
    RG_CHECKERROR(r);
  }
  else
  {
    RgRasterizedGeometryVertexArrays vertInfo = {};
    vertInfo.vertexData = brush.vertices;
    vertInfo.texCoordData = brush.texCoordLayers[0];
    vertInfo.colorData = nullptr;
    vertInfo.vertexStride = sizeof(GFXVertex);
    vertInfo.texCoordStride = sizeof(GFXTexCoord);
    vertInfo.colorStride = sizeof(GFXColor);

    RgRasterizedGeometryUploadInfo info = {};
    info.vertexCount = brush.vertexCount;
    info.arrays = &vertInfo;
    info.indexCount = brush.indexCount;
    info.indexData = brush.indices;
    info.color = { brush.layerColors[0](1), brush.layerColors[0](2), brush.layerColors[0](3), brush.layerColors[0](4) };
    info.material = textureUploader->GetMaterial(brush.textures[0], brush.textureFrames[0]);
    info.blendEnable = brush.blendEnable;
    info.blendFuncSrc = brush.blendSrc;
    info.blendFuncDst = brush.blendDst;
    info.depthTest = RG_TRUE;
    info.renderToSwapchain = RG_FALSE;

    Utils::CopyTransform(info.transform, brush);

    float *viewProj = 
      brush.visibilityType == RG_GEOMETRY_VISIBILITY_TYPE_SKYBOX ?
      backgroundViewProj : nullptr;

    RgResult r = rgUploadRasterizedGeometry(instance, &info, viewProj, nullptr);
    RG_CHECKERROR(r);
  }

  if (!brush.isRasterized)
  {
    // save movable brush info for updating
    if (brush.isMovable)
    {
      // create vector if doesn't exist
      if (entityToMovableBrush.find(brush.entityID) == entityToMovableBrush.end())
      {
        entityToMovableBrush[brush.entityID] = {};
      }

      entityToMovableBrush[brush.entityID].push_back(brush.GetUniqueID());
    }

    // save effect and animated textures to update them each frame
    for (uint32_t i = 0; i < 3; i++)
    {
      CTextureObject *to = brush.textureObjects[i];
      CTextureData *td = brush.textures[i];

      // if effect or animated
      bool hasNonStaticTexture = to && td && (td->td_ptegEffect != nullptr || td->td_ctFrames > 1);

      if (entityHasNonStaticTexture.find(brush.entityID) == entityHasNonStaticTexture.end())
      {
        entityHasNonStaticTexture[brush.entityID] = false;
      }

      entityHasNonStaticTexture[brush.entityID] |= hasNonStaticTexture;
    }

    // save brush part geom index for updating dynamic texture coordinates
    if (brush.hasScrollingTextures)
    {
      // create vector if doesn't exist
      if (entitiesWithDynamicTexCoords.find(brush.entityID) == entitiesWithDynamicTexCoords.end())
      {
        entitiesWithDynamicTexCoords[brush.entityID] = {};
      }

      BrushPartGeometryIndex t = {};
      t.brushPartIndex = brush.brushPartIndex;
      t.vertexCount = brush.vertexCount;

      entitiesWithDynamicTexCoords[brush.entityID].push_back(t);
    }
  }

  GeometryExporter::ExportGeometry(brush);
}

void SSRT::Scene::UpdateMovableBrush(ULONG entityId, const CPlacement3D &placement)
{
  const auto it = entityToMovableBrush.find(entityId);

  if (it == entityToMovableBrush.end())
  {
    return;
  }

  RgResult r;

  RgUpdateTransformInfo updateInfo = {};
  Utils::CopyTransform(updateInfo.transform, placement);

  // for each part of this entity
  for (uint64_t uniqueId : it->second)
  {
    updateInfo.movableStaticUniqueID = uniqueId;

    r = rgUpdateGeometryTransform(instance, &updateInfo);
    RG_CHECKERROR(r);
  }
}

void SSRT::Scene::HideMovableBrush(ULONG entityId)
{
  const auto it = entityToMovableBrush.find(entityId);

  // ignore if it wasn't registered
  if (it == entityToMovableBrush.end())
  {
    return;
  }

  RgResult r;

  // to hide brush, let the trasform be null
  RgUpdateTransformInfo updateInfo = {};
  updateInfo.transform = {};

  for (uint64_t uniqueId : it->second)
  {
    updateInfo.movableStaticUniqueID = uniqueId;

    r = rgUpdateGeometryTransform(instance, &updateInfo);
    RG_CHECKERROR(r);
  }
}

bool SSRT::Scene::ArePlacementsSame(const CPlacement3D &a, const CPlacement3D &b)
{
  float posDiff = 0.0f;
  float angDiff = 0.0f;

  for (int i = 0; i < 3; i++)
  {
    posDiff += Abs(a.pl_PositionVector.vector[i] - b.pl_PositionVector.vector[i]);
    angDiff += Abs(a.pl_OrientationAngle.vector[i] - b.pl_OrientationAngle.vector[i]);
  }
  
  const float posEps = 0.005f;
  const float angEps = 0.005f;

  return posDiff < posEps && angDiff < angEps;
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
  textureUploader->GetMaterial(pTexture, textureFrameIndex);
}

void SSRT::Scene::UpdateBrushTexCoords(const CUpdateTexCoordsInfo &info)
{
  auto it = entitiesWithDynamicTexCoords.find(info.brushEntityID);
  ASSERT(it != entitiesWithDynamicTexCoords.end());

  for (const auto &part : it->second)
  {
    if (part.brushPartIndex == info.brushPartIndex)
    {
      ASSERT(part.vertexCount == info.vertexCount);

      // if found geomIndex for that part
      RgUpdateTexCoordsInfo texCoordsInfo = {};
      texCoordsInfo.staticUniqueID = CBrushGeometry::GetBrushUniqueID(info.brushEntityID, info.brushPartIndex);
      texCoordsInfo.texCoordLayerData[0] = info.texCoordLayers[0];
      texCoordsInfo.texCoordLayerData[1] = info.texCoordLayers[1];
      texCoordsInfo.texCoordLayerData[2] = info.texCoordLayers[2];
      texCoordsInfo.vertexOffset = 0;
      texCoordsInfo.vertexCount = info.vertexCount;

      RgResult r = rgUpdateGeometryTexCoords(instance, &texCoordsInfo);
      RG_CHECKERROR(r);

      return;
    }
  }
}

void SSRT::Scene::ProcessFirstPersonModel(const CFirstPersonModelInfo &info, ULONG entityId)
{
  RT_AddFirstPersonModel(info.modelObject, info.renderModel, entityId, this);
}

void SSRT::Scene::ProcessBrushes()
{
  entityToMovableBrush.clear();
  entityToMovableBrushPlacement.clear();
  entityHasNonStaticTexture.clear();
  entitiesWithDynamicTexCoords.clear();

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
    if (entityHasNonStaticTexture[iten->en_ulID])
    {
      RT_UpdateBrushNonStaticTexture(iten, this);
    }

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
      // if brush has dynamic texture coords
      if (entitiesWithDynamicTexCoords.find(iten->en_ulID) != entitiesWithDynamicTexCoords.end())
      {
        // this will call UpdateBrushTexCoords(..)
        RT_UpdateBrushTexCoords(iten, this);
      }


      // process movable
      if (iten->en_ulPhysicsFlags & EPF_MOVABLE)
      {
        const bool isHidden = iten->en_ulFlags & ENF_HIDDEN;
        const auto &pl = iten->GetLerpedPlacement();

        const auto f = entityToMovableBrushPlacement.find(iten->en_ulID);
        MovableState *movableState = nullptr;

        if (f != entityToMovableBrushPlacement.end())
        {
          movableState = &f->second;

          // if should be and was hidden
          if (isHidden && movableState->isHidden)
          {
            continue;
          }

          // if shouldn't be and wasn't hidden, check the placements
          if (!isHidden && !movableState->isHidden)
          {
            if (ArePlacementsSame(movableState->placement, pl))
            {
              // if it wasn't moving in prev frame, skip it
              if (!movableState->wasMoving)
              {
                continue;
              }

              // if it was, then mark it
              movableState->wasMoving = false;
            }
            else
            {
              movableState->wasMoving = true;
            }
          }
        }
        else
        {
          // create and get pointer
          movableState = &entityToMovableBrushPlacement[iten->en_ulID];
        }

        if (!isHidden)
        {
          // if it's a visible movable brush, update transform
          UpdateMovableBrush(iten->en_ulID, pl);

          movableState->isHidden = false;
          movableState->placement = pl;
        }
        else
        {
          HideMovableBrush(iten->en_ulID);

          movableState->isHidden = true;
        }
      }
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