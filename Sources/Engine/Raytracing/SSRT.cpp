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

namespace SSRT
{
ULONG RTObject::GetEnitityID() const
{
  ASSERT(pOriginalEntity != nullptr);
  return pOriginalEntity->en_ulID;
}


CAbstractGeometry::~CAbstractGeometry()
{}


inline CBrush3D *CBrushGeometry::GetOriginalBrush()
{
  ASSERT(pOriginalEntity != nullptr);
  return pOriginalEntity->en_pbrBrush;
}


inline CModelObject *CModelGeometry::GetModelObject()
{
  ASSERT(pOriginalEntity != nullptr);
  return pOriginalEntity->en_pmoModelObject;
}


inline bool CBrushGeometry::IsMovable() const
{
  ASSERT(pOriginalEntity != nullptr);
  return pOriginalEntity->en_ulPhysicsFlags & EPF_MOVABLE;
}


void SSRTMain::AddModel(const CModelGeometry &model)
{
  // copy arrays to the global SSRT list
  GFXVertex *vertDest = staticVertices.Push(model.vertexCount);
  GFXNormal *normDest = staticNormals.Push(model.vertexCount);
  GFXTexCoord *texcDest = staticTexCoords.Push(model.vertexCount);
  INDEX *indcDest = staticIndices.Push(model.indexCount);

  memcpy(vertDest, model.vertices, sizeof(GFXVertex) * model.vertexCount);
  memcpy(normDest, model.normals, sizeof(GFXNormal) * model.vertexCount);
  memcpy(texcDest, model.texCoords, sizeof(GFXTexCoord) * model.vertexCount);
  memcpy(indcDest, model.indices, sizeof(INDEX) * model.indexCount);

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
  // copy arrays to the global SSRT list
  GFXVertex *vertDest = staticVertices.Push(brush.vertexCount);
  GFXNormal *normDest = staticNormals.Push(brush.vertexCount);
  GFXTexCoord *texcDest = staticTexCoords.Push(brush.vertexCount);
  INDEX *indcDest = staticIndices.Push(brush.indexCount);

  memcpy(vertDest, brush.vertices, sizeof(GFXVertex) * brush.vertexCount);
  memcpy(normDest, brush.normals, sizeof(GFXNormal) * brush.vertexCount);
  memcpy(texcDest, brush.texCoords, sizeof(GFXTexCoord) * brush.vertexCount);
  memcpy(indcDest, brush.indices, sizeof(INDEX) * brush.indexCount);

  // create a copy
  CBrushGeometry newBrush = brush;
  newBrush.vertices = vertDest;
  newBrush.normals = normDest;
  newBrush.texCoords = texcDest;
  newBrush.indices = indcDest;

  if (brush.IsMovable())
  {
    AddRTObject(brush, staticBrushes, entityToStaticBrush);
  }
  else
  {
    AddRTObject(brush, movableBrushes, entityToMovableBrush);
  }
}


void SSRTMain::AddLight(const CSphereLight &sphLt)
{
  AddRTObject(sphLt, sphLights, entityToSphLight);
}


void SSRTMain::AddLight(const CDirectionalLight &dirLt)
{
  AddRTObject(dirLt, dirLights, entityToDirLight);
}
}