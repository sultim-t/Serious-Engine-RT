/* Copyright (c) 2020 Sultim Tsyrendashiev
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
#include "SSRT.h"

#include <stdio.h>

#include <Engine/Base/Shell.h>
#include <Engine/World/World.h>

#include <Engine/Templates/DynamicContainer.cpp>

#include <Engine/Raytracing/SSRTObjects.h>
#include <Engine/Raytracing/RTProcessing.h>

extern CShell *_pShell;

// dump brushes and models to .obj files;
// use it with care, all calls must be done for one frame
// (offsets are local static variables)
#define DUMP_GEOMETRY_TO_OBJ 0

namespace SSRT
{

#if DUMP_GEOMETRY_TO_OBJ
static void ExportGeometry(const CAbstractGeometry &geom, INDEX offset, const char *path)
{
  FILE *file = fopen(path, "a");
  if (file == nullptr)
  {
    return;
  }

  for (INDEX i = 0; i < geom.vertexCount; i++)
  {
    FLOAT3D p = FLOAT3D(geom.vertices[i].x, geom.vertices[i].y, geom.vertices[i].z);
    FLOAT3D n = geom.normals ? FLOAT3D(geom.normals[i].nx, geom.normals[i].ny, geom.normals[i].nz) : FLOAT3D(0, 1, 0);
    FLOAT2D t = geom.texCoords ? FLOAT2D(geom.texCoords[i].s, geom.texCoords[i].t) : FLOAT2D();

    p = p * geom.absRotation + geom.absPosition;
    n = n * geom.absRotation;

    fprintf(file, "v %.3f %.3f %.3f\n", p(1), p(2), p(3));
    fprintf(file, "vn %.3f %.3f %.3f\n", n(1), n(2), n(3));
    fprintf(file, "vt %.3f %.3f\n", t(1), t(2));
  }

  fprintf(file, "g %d\n", geom.entityID);

  INDEX triangleCount = geom.indexCount / 3;

  for (INDEX i = 0; i < triangleCount; i++)
  {
    // obj indices start with 1
    INDEX a = geom.indices[i * 3 + 0] + offset + 1;
    INDEX b = geom.indices[i * 3 + 1] + offset + 1;
    INDEX c = geom.indices[i * 3 + 2] + offset + 1;

    fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n", a, a, a, b, b, b, c, c, c);
  }

  fclose(file);
}
#endif


void SSRTMain::AddModel(const CModelGeometry &model)
{
  ASSERT(model.vertices != nullptr && model.vertexCount != 0);
  ASSERT(model.indices != nullptr && model.indexCount != 0);

#if DUMP_GEOMETRY_TO_OBJ
  static INDEX offset = 0;
  ExportGeometry(model, offset, "MODELS.obj");
  offset += model.vertexCount;
#endif

  AddRTObject(model, models, entityToModel);
}


void SSRTMain::AddBrush(const CBrushGeometry &brush, bool isMovable)
{
  auto &brushArray = isMovable ? movableBrushes : staticBrushes;
  auto &entityToBrush = isMovable ? entityToMovableBrush : entityToStaticBrush;

  auto &found = entityToBrush.find(brush.entityID);

  // if it's already added
  //if (found != entityToBrush.end())
  //{
  //  /*// try to find its sector
  //  auto &foundSector = std::find_if(found->second.begin(), found->second.end(), [ &brush, &brushArray ] (INDEX i)
  //  {
  //    const CBrushGeometry &otherBrush = brushArray[i];

  //    // ? finding by a sector in a brush
  //    return true;
  //  });

  //  // if sector is already added
  //  if (foundSector != found->second.end())
  //  {
  //    brushArray[*foundSector] = brush;
  //    return;
  //  }*/

  //  // entity must have only 1 brush
  //  // TODO: RT: delete "entityToBrush" maps?
  //  ASSERT(found->second.size() == 1);
  //  ASSERT(brush.entityID == brushArray[found->second[0]].entityID);

  //  auto &targetBrush = brushArray[found->second[0]];
  //  
  //  targetBrush.isEnabled = brush.isEnabled;
  //  targetBrush.absPosition = brush.absPosition;
  //  targetBrush.absRotation = brush.absRotation;
  //  targetBrush.color = brush.color;

  //  return;
  //}

  if (brush.vertices == nullptr || brush.vertexCount == 0 ||
      brush.indices == nullptr || brush.indexCount == 0)
  {
    return;
  }

#if DUMP_GEOMETRY_TO_OBJ
  static INDEX offset = 0;
  ExportGeometry(brush, offset, "BRUSHES.obj");
  offset += brush.vertexCount;
#endif

  AddRTObject(brush, brushArray, entityToBrush);
}

void SSRTMain::AddLight(const CSphereLight &sphLt)
{
  sphLights.push_back(sphLt);
}

void SSRTMain::AddLight(const CDirectionalLight &dirLt)
{
  dirLights.push_back(dirLt);
}

CWorld *SSRTMain::GetCurrentWorld()
{
  return (CWorld *) _pShell->GetINDEX("pwoCurrentWorld");
}

void SSRTMain::StartFrame()
{
  CWorld *pwo = GetCurrentWorld();

  if (pwo == nullptr)
  {
    StopWorld();
    return;
  }

  // check if need to reload new world
  if (pwo->GetName() != currentWorldName)
  {
    SetWorld(pwo);
  }

  // check all movable brushes, models and light sources
  FOREACHINDYNAMICCONTAINER(pwo->wo_cenEntities, CEntity, iten)
  {
    if (iten->en_RenderType == CEntity::RT_MODEL)
    {
      // add it as a model and scan for light sources
      RT_AddModelEntity(&iten.Current(), this);
    }
    else if (iten->en_RenderType == CEntity::RT_BRUSH && (iten->en_ulPhysicsFlags & EPF_MOVABLE))
    {
      if ((iten->en_ulFlags & ENF_HIDDEN) || (iten->en_ulFlags & ENF_ZONING))
      {
        return;
      }

      // if it's a movable brush
      const CPlacement3D &placement = iten->GetLerpedPlacement();

      FLOAT3D position = placement.pl_PositionVector;
      FLOATmatrix3D rotation;
      MakeRotationMatrix(rotation, placement.pl_OrientationAngle);

      // update its transform
      CBrushGeometry brushInfo = {};
      brushInfo.entityID = iten->en_ulID;
      brushInfo.isEnabled = true;
      brushInfo.absPosition = position;
      brushInfo.absRotation = rotation;

      // leave vertex data empty, it won't be updated
      AddBrush(brushInfo, true);
    }
  }
}

void SSRTMain::EndFrame()
{
  // models will be rescanned in the beginning of the frame
  // it's done because of dynamic vertex data
  entityToModel.clear();
  models.clear();

  // lights will be readded too
  sphLights.clear();
  dirLights.clear();
}

void SSRTMain::SetWorld(CWorld *pwld)
{
  currentWorldName = pwld->GetName();

  // find all brushes, their geomtry won't change,
  // but movable brushes have dynamic transformation 
  // and they'll be updated on a frame start

  FOREACHINDYNAMICCONTAINER(pwld->wo_cenEntities, CEntity, iten)
  {
    // if it is any brush
    if (iten->en_RenderType == CEntity::RT_BRUSH)
    {
      // add all of its sectors
      RT_AddNonZoningBrush(&iten.Current(), NULL, this);
    }
  }
}

void SSRTMain::StopWorld()
{
  currentWorldName = "";

  models.clear();
  staticBrushes.clear();
  movableBrushes.clear();
  sphLights.clear();
  dirLights.clear();

  entityToModel.clear();
  entityToStaticBrush.clear();
  entityToMovableBrush.clear();
}

}