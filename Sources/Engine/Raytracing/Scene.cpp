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

#include "Utils.h"

#define DUMP_GEOMETRY_TO_OBJ 0
#include "GeometryExporter.h"
#include "RTProcessing.h"

SSRT::Scene::Scene(RgInstance _instance, CWorld *_pWorld)
  : instance(_instance), pWorld(_pWorld), worldName(_pWorld->GetName())
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

void SSRT::Scene::Update(ULONG viewerEntityID)
{
  // clear previous data
  sphLights.clear();
  dirLights.clear();

  // upload dynamic geometry (models)
  // and scan for movable geometry
  ProcessDynamicGeometry(viewerEntityID);
}

const CTString &SSRT::Scene::GetWorldName() const
{
  return worldName;
}

void SSRT::Scene::AddModel(const CModelGeometry &model)
{
  if (model.vertices == nullptr || model.vertexCount == 0 || model.indices == nullptr || model.indexCount == 0)
  {
    return;
  }

  RgGeometryUploadInfo dnInfo = {};
  dnInfo.geomType = RG_GEOMETRY_TYPE_DYNAMIC;
  dnInfo.vertexCount = model.vertexCount;
  dnInfo.vertexData = (float *)model.vertices;
  dnInfo.normalData = (float *)model.normals;
  dnInfo.texCoordData = (float *)model.texCoords;
  dnInfo.colorData = nullptr;
  dnInfo.indexCount = model.indexCount;
  dnInfo.indexData = (uint32_t *)model.indices;
  dnInfo.geomMaterial = {
    RG_NO_MATERIAL,
    RG_NO_MATERIAL,
    RG_NO_MATERIAL
  };

  Utils::CopyTransform(dnInfo.transform, model);

  RgResult r = rgUploadGeometry(instance, &dnInfo, nullptr);
  RG_CHECKERROR(r);

  GeometryExporter::ExportGeometry(model);
}

void SSRT::Scene::AddBrush(const CBrushGeometry &brush, bool isMovable)
{
  if (brush.vertices == nullptr || brush.vertexCount == 0 || brush.indices == nullptr || brush.indexCount == 0)
  {
    return;
  }

  RgGeometry geomIndex;

  RgGeometryUploadInfo stInfo = {};
  stInfo.geomType = isMovable ? 
    RG_GEOMETRY_TYPE_STATIC_MOVABLE :
    RG_GEOMETRY_TYPE_STATIC;
  stInfo.vertexCount = brush.vertexCount;
  stInfo.vertexData = (float *)brush.vertices;
  stInfo.normalData = (float *)brush.normals;
  stInfo.texCoordData = (float *)brush.texCoords;
  stInfo.colorData = nullptr;
  stInfo.indexCount = brush.indexCount;
  stInfo.indexData = (uint32_t *)brush.indices;
  stInfo.geomMaterial = {
    RG_NO_MATERIAL,
    RG_NO_MATERIAL,
    RG_NO_MATERIAL
  };

  Utils::CopyTransform(stInfo.transform, brush);

  RgResult r = rgUploadGeometry(instance, &stInfo, &geomIndex);
  RG_CHECKERROR(r);

  if (isMovable)
  {
  #ifndef NDEBUG
    auto it = entityToMovableBrush.find(brush.entityID);
    ASSERTMSG(it == entityToMovableBrush.end(), "Movable brush with the same entity ID was already added");
  #endif

    entityToMovableBrush[brush.entityID] = geomIndex;
  }

  GeometryExporter::ExportGeometry(brush);
}

void SSRT::Scene::UpdateMovableBrush(ULONG entityId, const CPlacement3D &placement)
{
  auto it = entityToMovableBrush.find(entityId);

  if (it == entityToMovableBrush.end())
  {
    // ASSERTALWAYS("Movable brush wasn't uploaded");
    return;
  }

  RgGeometry geomIndex = it->second;

  RgUpdateTransformInfo updateInfo = {};
  updateInfo.movableStaticGeom = geomIndex;
  Utils::CopyTransform(updateInfo.transform, placement);

  RgResult r = rgUpdateGeometryTransform(instance, &updateInfo);
  RG_CHECKERROR(r);
}

void SSRT::Scene::AddLight(const CSphereLight &sphLt)
{
  sphLights.push_back(sphLt);
}

void SSRT::Scene::AddLight(const CDirectionalLight &dirLt)
{
  dirLights.push_back(dirLt);
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
      RT_AddNonZoningBrush(&iten.Current(), this);
    }
  }

  // submit
  r = rgSubmitStaticGeometries(instance);
  RG_CHECKERROR(r);
}

void SSRT::Scene::ProcessDynamicGeometry(ULONG viewerEntityID)
{
  // check all movable brushes, models and light sources
  FOREACHINDYNAMICCONTAINER(pWorld->wo_cenEntities, CEntity, iten)
  {
    if (iten->en_ulID == viewerEntityID)
    {
      continue;
    }

    if (iten->en_RenderType == CEntity::RT_MODEL)
    {
      // add it as a model and scan for light sources
      RT_AddModelEntity(&iten.Current(), this);
    }
    else if (iten->en_RenderType == CEntity::RT_BRUSH && (iten->en_ulPhysicsFlags & EPF_MOVABLE))
    {
      if ((iten->en_ulFlags & ENF_HIDDEN) /*|| (iten->en_ulFlags & ENF_ZONING)*/)
      {
        continue;
      }

      // if it's a movable brush, update transform
      UpdateMovableBrush(iten->en_ulID, iten->GetLerpedPlacement());
    }
  }
}
