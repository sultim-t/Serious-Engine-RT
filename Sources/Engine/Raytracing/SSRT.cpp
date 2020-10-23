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

#include <Engine/Base/Shell.h>
#include <Engine/World/World.h>

#include <Engine/Templates/DynamicContainer.cpp>

#include <Engine/Raytracing/SSRTObjects.h>
#include <Engine/Raytracing/RTProcessing.h>

extern CShell *_pShell;

namespace SSRT
{

void SSRTMain::AddModel(const CModelGeometry &model)
{
  ASSERT(model.vertices != nullptr && model.vertexCount != 0);
  ASSERT(model.indices != nullptr && model.indexCount != 0);

  // copy arrays to the global SSRT list
  GFXVertex     *vertDest = model.vertices;// dynamicVertices.Push(model.vertexCount);
  INDEX         *indcDest = model.indices;// dynamicIndices.Push(model.indexCount);
  GFXNormal     *normDest = model.normals;//dynamicNormals.Push(model.vertexCount);
  GFXTexCoord   *texcDest = model.texCoords;//dynamicTexCoords.Push(model.vertexCount);

  //memcpy(vertDest, model.vertices, sizeof(GFXVertex) * model.vertexCount);
  //memcpy(indcDest, model.indices, sizeof(INDEX) * model.indexCount);

  //if (model.normals != nullptr)
  //{
  //  memcpy(normDest, model.normals, sizeof(GFXNormal) * model.vertexCount);
  //}
  //else
  //{
  //  // generate normals
  //}

  //if (model.texCoords != nullptr)
  //{
  //  memcpy(texcDest, model.texCoords, sizeof(GFXTexCoord) * model.vertexCount);
  //}

  // create a copy
  CModelGeometry newModel = model;
  newModel.vertices = vertDest;
  newModel.normals = normDest;
  newModel.texCoords = texcDest;
  newModel.indices = indcDest;

  AddRTObject(newModel, models, entityToModel);
}


void SSRTMain::AddBrush(const CBrushGeometry &brush)
{
  auto &brushArray = brush.IsMovable() ? movableBrushes : staticBrushes;
  auto &entityToBrush = brush.IsMovable() ? entityToMovableBrush : entityToStaticBrush;

  auto &found = entityToBrush.find(brush.GetEnitityID());

  // if it's already added
  if (found != entityToBrush.end())
  {
    /*// try to find its sector
    auto &foundSector = std::find_if(found->second.begin(), found->second.end(), [ &brush, &brushArray ] (INDEX i)
    {
      const CBrushGeometry &otherBrush = brushArray[i];

      // ? finding by a sector in a brush
      return true;
    });

    // if sector is already added
    if (foundSector != found->second.end())
    {
      brushArray[*foundSector] = brush;
      return;
    }*/

    // entity must have only 1 brush
    // TODO: RT: delete "entityToBrush" maps?
    ASSERT(found->second.size() == 1);
    ASSERT(brush.GetEnitityID() == brushArray[found->second[0]].GetEnitityID());

    auto &targetBrush = brushArray[found->second[0]];
    
    targetBrush.isEnabled = brush.isEnabled;
    targetBrush.absPosition = brush.absPosition;
    targetBrush.absRotation = brush.absRotation;
    targetBrush.color = brush.color;

    return;
  }

  ASSERT(brush.vertices != nullptr && brush.vertexCount != 0);
  ASSERT(brush.indices != nullptr && brush.indexCount != 0);

  // copy arrays to the global SSRT list
  GFXVertex *vertDest = brush.vertices;//staticVertices.Push(brush.vertexCount);
  INDEX *indcDest = brush.indices;//staticIndices.Push(brush.indexCount);
  GFXNormal *normDest = brush.normals;//staticNormals.Push(brush.vertexCount);
  GFXTexCoord *texcDest = brush.texCoords;//staticTexCoords.Push(brush.vertexCount);

  //memcpy(vertDest, brush.vertices, sizeof(GFXVertex) * brush.vertexCount);
  //memcpy(indcDest, brush.indices, sizeof(INDEX) * brush.indexCount);

  //if (brush.normals != nullptr)
  //{
  //  memcpy(normDest, brush.normals, sizeof(GFXNormal) * brush.vertexCount);
  //}
  //else
  //{
  //  // generate normals
  //}

  //if (brush.texCoords != nullptr)
  //{
  //  memcpy(texcDest, brush.texCoords, sizeof(GFXTexCoord) * brush.vertexCount);
  //}

  // create a copy
  CBrushGeometry newBrush = brush;
  newBrush.vertices = vertDest;
  newBrush.normals = normDest;
  newBrush.texCoords = texcDest;
  newBrush.indices = indcDest;

  AddRTObject(brush, brushArray, entityToBrush);
}


void SSRTMain::AddLight(const CSphereLight &sphLt)
{
  ASSERT(sphLt.pOriginalEntity != nullptr);

  sphLights.Push() = sphLt; 
}


void SSRTMain::AddLight(const CDirectionalLight &dirLt)
{
  ASSERT(dirLt.pOriginalEntity != nullptr);

  dirLights.Push() = dirLt;
}

void SSRTMain::StartFrame()
{
  CWorld *pwo = (CWorld *) _pShell->GetINDEX("pwoCurrentWorld");

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
      // if it's a movable brush
      const CPlacement3D &placement = iten->GetLerpedPlacement();

      FLOAT3D position = placement.pl_PositionVector;
      FLOATmatrix3D rotation;
      MakeRotationMatrix(rotation, placement.pl_OrientationAngle);

      // update its transform
      CBrushGeometry brushInfo = {};
      brushInfo.pOriginalEntity = &iten.Current();
      brushInfo.isEnabled = true;
      brushInfo.absPosition = position;
      brushInfo.absRotation = rotation;

      // leave vertex data empty, it won't be updated
      AddBrush(brushInfo);
    }
  }
}

void SSRTMain::EndFrame()
{
  // models will be rescanned in the beginning of the frame
  // it's done because of dynamic vertex data
  entityToModel.clear();
  models.Clear();

  dynamicVertices.Clear();
  dynamicNormals.Clear();
  dynamicTexCoords.Clear();
  dynamicIndices.Clear();

  // lights will be readded too
  sphLights.Clear();
  dirLights.Clear();
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

}