#pragma once

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

#include <Engine/Entities/Entity.h>
#include <Engine/Brushes/Brush.h>
#include <Engine/Templates/LinearAllocator.h>

#include <memory>
#include <vector>
#include <map>

namespace SSRT
{
// attachments can have attachments, SSRT_MAX_ATTCH_DEPTH defines the max depth
#define SSRT_MAX_ATTACHMENT_DEPTH 8

struct RTObject
{
  CEntity         *pOriginalEntity;
  bool            isEnabled;

public:
  ULONG           GetEnitityID() const;
};

struct CAbstractGeometry : RTObject
{
  // absolute position and rotation
  FLOAT3D         absPosition;
  FLOATmatrix3D   absRotation;

  COLOR           color;

  // Data that should be set to the renderer,
  // vertices are already animated.
  // This data will become invalid after adding CModelGeometry
  // as SE will reset global arrays
  INDEX           vertexCount;
  GFXVertex       *vertices;
  GFXNormal       *normals;
  GFXTexCoord     *texCoords;
  INDEX           indexCount;
  INDEX           *indices;

  // in relative space
  //std::shared_ptr<std::vector<FLOAT3D>> positions;

public:
  virtual ~CAbstractGeometry() = 0;
};


// Dynamic geometry that changes every frame
struct CModelGeometry : public CAbstractGeometry
{
  // path to attachment using attachment position ID,
  // -1 means the end of the list
  INDEX           attchPath[SSRT_MAX_ATTACHMENT_DEPTH];

public:
  CModelObject    *GetModelObject();
};


// Static geometry, vertex data doesn't change. 
// If it's movable, then transformation can be changed
struct CBrushGeometry : public CAbstractGeometry
{
public:
  bool            IsMovable() const;
  CBrush3D        *GetOriginalBrush();
};


struct CDirectionalLight : public RTObject
{
  FLOAT3D         direction;
  COLOR           color;
  // directional light angular size on a sky in degrees
  float           angularSize;
};


struct CSphereLight : public RTObject
{
  FLOAT3D         absPosition;
  COLOR           color;
  float           intensity;
  float           sphereRadius;
};


// TODO: RT: if SE1 doesn't provide info about deleted entities
// then just set isEnabled=false for each object before frame start
class SSRTMain
{
private:
  ULONG           screenWidth;
  ULONG           screenHeight;


  // global arrays; linear allocators will allocate new blocks without relocation of memory,
  // so pointers in CAbstractGeometry won't be obsolete
  CLinearAllocator<GFXVertex>           staticVertices;
  CLinearAllocator<GFXNormal>           staticNormals;
  CLinearAllocator<GFXTexCoord>         staticTexCoords;
  CLinearAllocator<INDEX>               staticIndices;

  CLinearAllocator<GFXVertex>           dynamicVertices;
  CLinearAllocator<GFXNormal>           dynamicNormals;
  CLinearAllocator<GFXTexCoord>         dynamicTexCoords;
  CLinearAllocator<INDEX>               dynamicIndices;


  // these arrays hold information about all objects in a world
  CStaticStackArray<CModelGeometry>     models;
  CStaticStackArray<CBrushGeometry>     staticBrushes;
  CStaticStackArray<CBrushGeometry>     movableBrushes;
  CStaticStackArray<CSphereLight>       sphLights;
  CStaticStackArray<CDirectionalLight>  dirLights;

  // - Every entity can be either model or brush
  // - A model can have attachments that are models too
  // - A brush can have several sectors
  // - An ntity can have one light source "attached" to it.
  // Next maps are for getting associated SSRT object by entity ID
  // for updating their params, if they are created
  std::map<ULONG, std::vector<INDEX>>   entityToModel;
  std::map<ULONG, std::vector<INDEX>>   entityToStaticBrush;
  std::map<ULONG, std::vector<INDEX>>   entityToMovableBrush;
  std::map<ULONG, std::vector<INDEX>>   entityToSphLight;
  std::map<ULONG, std::vector<INDEX>>   entityToDirLight;

public:
  void AddModel(const CModelGeometry &model);
  void AddBrush(const CBrushGeometry &brush);
  void AddLight(const CSphereLight &sphLt);
  void AddLight(const CDirectionalLight &dirLt);

  void SetWorld(CWorld *pwld);

private:
  template<class T>
  void AddRTObject(const T &obj, CStaticStackArray<T> &arr, std::map<ULONG, std::vector<INDEX>> &entToObjs);
};

#pragma region template implementation
template<class T>
inline void SSRTMain::AddRTObject(const T &obj, CStaticStackArray<T> &arr, std::map<ULONG, std::vector<INDEX>> &entToObjs)
{
  ASSERT(obj.pOriginalEntity != nullptr);

  // try to find vector by its enitity
  auto &found = entToObjs.find(obj.GetEnitityID());

  if (found != entToObjs.end())
  {
    for (INDEX i : found->second)
    {
      // entities must be identical
      ASSERT(arr[i].pOriginalEntity->en_ulID == obj.GetEnitityID());

      // if parts are the same
      // (there should be a more reliable and faster way to identify 
      // the same model attachments or brush sectors than just names) 

      if (obj.pOriginalEntity->GetName() == arr[i].pOriginalEntity->GetName() ||
          obj.pOriginalEntity->GetDescription() == arr[i].pOriginalEntity->GetDescription())
      {
        // set it and return
        arr[i] = obj;
        return;
      }
    }

    INDEX i = arr.Count();
    arr.Push() = obj;

    found->second.push_back(i);
  }
  else
  {
    INDEX i = arr.Count();
    arr.Push() = obj;

    entToObjs[obj.GetEnitityID()].push_back(i);
  }
}
#pragma endregion
}