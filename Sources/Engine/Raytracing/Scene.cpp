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

extern FLOAT srt_fLightDirectionalColorMultiplier = 10.0f;
extern FLOAT srt_fLightDirectionalAngularDiameter = 0.5f;

extern INDEX srt_iLightSphericalHSVThresholdHLower = 20;
extern INDEX srt_iLightSphericalHSVThresholdHUpper = 45;
extern INDEX srt_iLightSphericalHSVThresholdVLower = 80;
extern INDEX srt_iLightSphericalHSVThresholdVUpper = 255;

void SSRT::Scene::InitShellVariables()
{
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDefaultSpecularMetallic;", &srt_fDefaultSpecularMetallic);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDefaultSpecularRoughness;", &srt_fDefaultSpecularRoughness);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDefaultReflectiveMetallic;", &srt_fDefaultReflectiveMetallic);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDefaultReflectiveRoughness;", &srt_fDefaultReflectiveRoughness);

  _pShell->DeclareSymbol("persistent user INDEX srt_bEnableViewerShadows;", &srt_bEnableViewerShadows);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightDirectionalColorMultiplier;", &srt_fLightDirectionalColorMultiplier);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightDirectionalAngularDiameter;", &srt_fLightDirectionalAngularDiameter);

  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalHSVThresholdHLower;", &srt_iLightSphericalHSVThresholdHLower);
  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalHSVThresholdHUpper;", &srt_iLightSphericalHSVThresholdHUpper);
  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalHSVThresholdVLower;", &srt_iLightSphericalHSVThresholdVLower);
  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphericalHSVThresholdVUpper;", &srt_iLightSphericalHSVThresholdVUpper);
}

void SSRT::Scene::NormalizeShellVariables()
{
  srt_fDefaultSpecularMetallic = Clamp(srt_fDefaultSpecularMetallic, 0.0f, 1.0f);
  srt_fDefaultSpecularRoughness = Clamp(srt_fDefaultSpecularRoughness, 0.0f, 1.0f);
  srt_fDefaultReflectiveMetallic = Clamp(srt_fDefaultReflectiveMetallic, 0.0f, 1.0f);
  srt_fDefaultReflectiveRoughness = Clamp(srt_fDefaultReflectiveRoughness, 0.0f, 1.0f);
  srt_bEnableViewerShadows = !!srt_bEnableViewerShadows;
  srt_fLightDirectionalColorMultiplier = Clamp(srt_fLightDirectionalColorMultiplier, 0.0f, 10000.0f);
  srt_fLightDirectionalAngularDiameter = Clamp(srt_fLightDirectionalAngularDiameter, 0.0f, 10000.0f);

  srt_iLightSphericalHSVThresholdHLower = Clamp<INDEX>(srt_iLightSphericalHSVThresholdHLower, 0, 255);
  srt_iLightSphericalHSVThresholdHUpper = Clamp<INDEX>(srt_iLightSphericalHSVThresholdHUpper, 0, 255);
  srt_iLightSphericalHSVThresholdVLower = Clamp<INDEX>(srt_iLightSphericalHSVThresholdVLower, 0, 255);
  srt_iLightSphericalHSVThresholdVUpper = Clamp<INDEX>(srt_iLightSphericalHSVThresholdVUpper, 0, 255);
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

void SSRT::Scene::Update(const FLOAT3D &_viewerPosition, const FLOATmatrix3D &_viewerRotation, ULONG _viewerEntityID)
{
  this->viewerEntityID = _viewerEntityID;
  this->viewerPosition = _viewerPosition;
  this->viewerRotation = _viewerRotation;

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
  
  RgGeometryUploadInfo dnInfo = {};
  dnInfo.geomType = RG_GEOMETRY_TYPE_DYNAMIC;
  dnInfo.passThroughType = model.passThroughType;
  dnInfo.visibilityType = model.visibilityType;
  dnInfo.vertexCount = model.vertexCount;
  dnInfo.vertexData = model.vertices;
  dnInfo.normalData = model.normals;
  dnInfo.texCoordLayerData[0] = model.texCoordLayers[0];
  dnInfo.texCoordLayerData[1] = nullptr;
  dnInfo.texCoordLayerData[2] = nullptr;
  dnInfo.defaultMetallicity = metallic;
  dnInfo.defaultRoughness = roughness;
  dnInfo.indexCount = model.indexCount;
  dnInfo.indexData = model.indices;

  dnInfo.layerColors[0] = { model.color(1), model.color(2), model.color(3), model.color(4) };
  dnInfo.layerBlendingTypes[0] = RG_GEOMETRY_MATERIAL_BLEND_TYPE_OPAQUE;

  dnInfo.geomMaterial =
  {
    textureUploader->GetMaterial(model.textures[0], model.textureFrames[0]),
    RG_NO_MATERIAL,
    RG_NO_MATERIAL,
  };

  Utils::CopyTransform(dnInfo.transform, model);

  RgResult r = rgUploadGeometry(instance, &dnInfo, nullptr);
  RG_CHECKERROR(r);

  GeometryExporter::ExportGeometry(model);
}

void SSRT::Scene::AddParticles(const CParticlesGeometry &info)
{
  if (info.vertices == nullptr || info.vertexCount == 0 || info.indices == nullptr || info.indexCount == 0)
  {
    return;
  }

  RgGeometryUploadInfo paInfo = {};
  paInfo.geomType = RG_GEOMETRY_TYPE_DYNAMIC;
  paInfo.passThroughType = info.passThroughType;
  paInfo.visibilityType = info.visibilityType;
  paInfo.vertexCount = info.vertexCount;
  paInfo.vertexData = info.vertices;
  paInfo.normalData = info.normals;
  paInfo.texCoordLayerData[0] = info.texCoordLayers[0];
  paInfo.texCoordLayerData[1] = nullptr;
  paInfo.texCoordLayerData[2] = nullptr;
  paInfo.defaultRoughness = 1.0f;
  paInfo.indexCount = info.indexCount;
  paInfo.indexData = info.indices;

  paInfo.layerColors[0] = { info.color(1), info.color(2), info.color(3), info.color(4) };
  paInfo.layerBlendingTypes[0] = RG_GEOMETRY_MATERIAL_BLEND_TYPE_OPAQUE;
  
  paInfo.geomMaterial =
  {
    textureUploader->GetMaterial(info.textures[0], info.textureFrames[0]),
    RG_NO_MATERIAL,
    RG_NO_MATERIAL,
  };

  Utils::CopyTransform(paInfo.transform, info);

  RgResult r = rgUploadGeometry(instance, &paInfo, nullptr);
  RG_CHECKERROR(r);
}

void SSRT::Scene::AddBrush(const CBrushGeometry &brush)
{
  if (brush.vertices == nullptr || brush.vertexCount == 0 || brush.indices == nullptr || brush.indexCount == 0)
  {
    return;
  }

  RgGeometry geomIndex;

  RgGeometryUploadInfo stInfo = {};
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

  RgResult r = rgUploadGeometry(instance, &stInfo, &geomIndex);
  RG_CHECKERROR(r);

  // save movable brush info for updating
  if (brush.isMovable)
  {
    auto it = entityToMovableBrush.find(brush.entityID);

    if (it == entityToMovableBrush.end())
    {
      // create vector if doesn't exist
      entityToMovableBrush[brush.entityID] = {};
    }

    entityToMovableBrush[brush.entityID].push_back(geomIndex);
  }

  // save effect and animated textures to update them each frame
  for (uint32_t i = 0; i < 3; i++)
  {
    CTextureObject *to = brush.textureObjects[i];
    CTextureData *td = brush.textures[i];

    // if effect or animated
    bool hasNonStaticTexture = to && td && (td->td_ptegEffect != nullptr || td->td_ctFrames > 1);

    entityHasNonStaticTexture[brush.entityID] = hasNonStaticTexture;
  }

  // save brush part geom index for updating dynamic texture coordinates
  if (brush.hasScrollingTextures)
  {
    auto it = entitiesWithDynamicTexCoords.find(brush.entityID);

    if (it == entitiesWithDynamicTexCoords.end())
    {
      // create vector if doesn't exist
      entitiesWithDynamicTexCoords[brush.entityID] = {};
    }

    BrushPartGeometryIndex t = {};
    t.brushPartIndex = brush.brushPartIndex;
    t.geomIndex = geomIndex;

    entitiesWithDynamicTexCoords[brush.entityID].push_back(t);
  }

  GeometryExporter::ExportGeometry(brush);
}

void SSRT::Scene::UpdateMovableBrush(ULONG entityId, const CPlacement3D &placement)
{
  auto it = entityToMovableBrush.find(entityId);

  if (it == entityToMovableBrush.end())
  {
    return;
  }

  RgResult r;

  RgUpdateTransformInfo updateInfo = {};
  Utils::CopyTransform(updateInfo.transform, placement);

  // for each part of this entity
  for (RgGeometry geomIndex : it->second)
  {
    updateInfo.movableStaticGeom = geomIndex;

    r = rgUpdateGeometryTransform(instance, &updateInfo);
    RG_CHECKERROR(r);
  }
}

void SSRT::Scene::HideMovableBrush(ULONG entityId)
{
  auto it = entityToMovableBrush.find(entityId);

  // ignore if it wasn't registered
  if (it == entityToMovableBrush.end())
  {
    return;
  }

  RgResult r;

  // to hide brush, let the trasform be null
  RgUpdateTransformInfo updateInfo = {};
  updateInfo.transform = {};

  for (RgGeometry geomIndex : it->second)
  {
    updateInfo.movableStaticGeom = geomIndex;

    r = rgUpdateGeometryTransform(instance, &updateInfo);
    RG_CHECKERROR(r);
  }
}

void SSRT::Scene::AddLight(const CSphereLight &sphLt)
{
  RgResult r;

  FLOAT3D color = sphLt.color * sphLt.intensity;

  RgSphericalLightUploadInfo info = {};
  info.color = { color(1), color(2), color(3) };
  info.position = { sphLt.absPosition(1), sphLt.absPosition(2), sphLt.absPosition(3) };
  info.radius = sphLt.sphereRadius;

  r = rgUploadSphericalLight(instance, &info);

  sphLights.push_back(sphLt);
}

void SSRT::Scene::AddLight(const CDirectionalLight &dirLt)
{
  RgResult r;

  FLOAT3D color = dirLt.color * srt_fLightDirectionalColorMultiplier;

  RgDirectionalLightUploadInfo info = {};
  info.color = { color(1), color(2), color(3) };
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
    if (part.brushPartIndex == part.brushPartIndex)
    {
      // if found geomIndex for that part
      RgUpdateTexCoordsInfo texCoordsInfo = {};
      texCoordsInfo.staticGeom = part.geomIndex;
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

  RT_BrushClear();

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
        if (iten->en_ulFlags & ENF_HIDDEN)
        {
          // try to hide
          HideMovableBrush(iten->en_ulID);
        }
        else
        {
          // if it's a visible movable brush, update transform
          UpdateMovableBrush(iten->en_ulID, iten->GetLerpedPlacement());
        }
      }
    }
  }

  if (viewer != nullptr)
  {
    RT_AddAllParticles(pWorld, viewer, this);
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