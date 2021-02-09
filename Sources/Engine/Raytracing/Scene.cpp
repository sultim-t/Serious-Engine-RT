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

void SSRT::Scene::InitShellVariables()
{
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDefaultSpecularMetallic;", &srt_fDefaultSpecularMetallic);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDefaultSpecularRoughness;", &srt_fDefaultSpecularRoughness);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDefaultReflectiveMetallic;", &srt_fDefaultReflectiveMetallic);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDefaultReflectiveRoughness;", &srt_fDefaultReflectiveRoughness);
}


SSRT::Scene::Scene(RgInstance _instance, CWorld *_pWorld, TextureUploader *_textureUploader)
:
  instance(_instance),
  pWorld(_pWorld),
  viewerEntityID(UINT32_MAX),
  textureUploader(_textureUploader),
  worldName(_pWorld->GetName())
{
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

  // clear previous data
  sphLights.clear();
  dirLights.clear();

  // upload dynamic geometry (models)
  // and scan for movable geometry
  ProcessDynamicGeometry();

  // update textures for brushes
  ProcessNonStaticTextures();
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
  dnInfo.vertexCount = model.vertexCount;
  dnInfo.vertexData = model.vertices;
  dnInfo.normalData = model.normals;
  dnInfo.texCoordData = model.texCoords;
  dnInfo.defaultMetallicity = metallic;
  dnInfo.defaultRoughness = roughness;
  dnInfo.indexCount = model.indexCount;
  dnInfo.indexData = model.indices;

  memcpy(dnInfo.color, model.color.vector, sizeof(float) * 4);
  dnInfo.geomMaterial =
  {
    textureUploader->GetMaterial(model.textures[0], model.textureFrames[0]),
    textureUploader->GetMaterial(model.textures[1], model.textureFrames[1]),
    textureUploader->GetMaterial(model.textures[2], model.textureFrames[2]),
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
  paInfo.vertexCount = info.vertexCount;
  paInfo.vertexData = info.vertices;
  paInfo.normalData = info.normals;
  paInfo.texCoordData = info.texCoords;
  paInfo.defaultRoughness = 1.0f;
  paInfo.indexCount = info.indexCount;
  paInfo.indexData = info.indices;

  memcpy(paInfo.color, info.color.vector, sizeof(float) * 4);
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
  stInfo.vertexCount = brush.vertexCount;
  stInfo.vertexData = brush.vertices;
  stInfo.normalData = brush.normals;
  stInfo.texCoordData = brush.texCoords;
  stInfo.defaultRoughness = 1.0f;
  stInfo.indexCount = brush.indexCount;
  stInfo.indexData = brush.indices;

  memcpy(stInfo.color, brush.color.vector, sizeof(float) * 4);
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
    if (to && td && (td->td_ptegEffect != nullptr || td->td_ctFrames > 1))
    {
      brushNonStaticTextures.insert(brush.textureObjects[i]);
    }
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
  sphLights.push_back(sphLt);
}

void SSRT::Scene::AddLight(const CDirectionalLight &dirLt)
{
  dirLights.push_back(dirLt);
}

void SSRT::Scene::AddFirstPersonModel(const CFirstPersonModelInfo &info, ULONG entityId)
{
  if (entityId == viewerEntityID)
  {
    return;
  }

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

  // check all movable brushes, models and light sources
  FOREACHINDYNAMICCONTAINER(pWorld->wo_cenEntities, CEntity, iten)
  {
    if (iten->en_ulID == viewerEntityID)
    {
      viewer = (CEntity *)iten;
      continue;
    }

    if (iten->en_RenderType == CEntity::RT_MODEL)
    {
      // add it as a model and scan for light sources
      RT_AddModelEntity(&iten.Current(), this);
    }
    else if (iten->en_RenderType == CEntity::RT_BRUSH && (iten->en_ulPhysicsFlags & EPF_MOVABLE))
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

  if (viewer != nullptr)
  {
    RT_AddAllParticles(pWorld, viewer, this);
  }
}

void SSRT::Scene::ProcessNonStaticTextures()
{
  for (CTextureObject *to : brushNonStaticTextures)
  {
    CTextureData *td = (CTextureData*)to->GetData();
    uint32_t tdFrame = to->GetFrame();

    textureUploader->GetMaterial(td, tdFrame);
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