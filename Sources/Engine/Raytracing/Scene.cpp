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

#include <Engine/World/World.h>

#include <Engine/Templates/DynamicContainer.cpp>

#include "RTProcessing.h"
#include "Utils.h"
#define DUMP_GEOMETRY_TO_OBJ 0
#include "GeometryExporter.h"

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

  RgResult r = rgStartNewScene(instance);
  RG_CHECKERROR(r);

  // submit empty scene
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

  RgGeometryUploadInfo dnInfo = {};
  dnInfo.geomType = RG_GEOMETRY_TYPE_DYNAMIC;
  dnInfo.passThroughType = model.passThroughType;
  dnInfo.vertexCount = model.vertexCount;
  dnInfo.vertexData = (float *)model.vertices;
  dnInfo.normalData = (float *)model.normals;
  dnInfo.texCoordData = (float *)model.texCoords;
  dnInfo.colorData = nullptr;
  dnInfo.indexCount = model.indexCount;
  dnInfo.indexData = (uint32_t *)model.indices;
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
  stInfo.vertexData = (float *)brush.vertices;
  stInfo.normalData = (float *)brush.normals;
  stInfo.texCoordData = (float *)brush.texCoords;
  stInfo.colorData = nullptr;
  stInfo.indexCount = brush.indexCount;
  stInfo.indexData = (uint32_t *)brush.indices;
  stInfo.geomMaterial = 
  {
    textureUploader->GetMaterial(brush.textures[0], brush.textureFrames[0]),
    textureUploader->GetMaterial(brush.textures[1], brush.textureFrames[1]),
    textureUploader->GetMaterial(brush.textures[2], brush.textureFrames[2]),
  };

  Utils::CopyTransform(stInfo.transform, brush);

  RgResult r = rgUploadGeometry(instance, &stInfo, &geomIndex);
  RG_CHECKERROR(r);

  if (brush.isMovable)
  {
    auto it = entityToMovableBrush.find(brush.entityID);

    if (it == entityToMovableBrush.end())
    {
      entityToMovableBrush[brush.entityID] = {};
    }

    entityToMovableBrush[brush.entityID].push_back(geomIndex);
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
    RT_AddParticles(pWorld, viewer, this);
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