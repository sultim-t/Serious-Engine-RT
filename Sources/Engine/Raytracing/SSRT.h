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

#include <memory>
#include <vector>
#include <map>

namespace SSRT
{
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

    // in relative space
    //std::shared_ptr<std::vector<FLOAT3D>> positions;

  public:
    virtual ~CAbstractGeometry() = 0;
  };


  // Dynamic geometry that changes every frame
  struct CModelGeometry : public CAbstractGeometry
  {
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

    CStaticStackArray<CModelGeometry>     models;
    CStaticStackArray<CBrushGeometry>     staticBrushes;
    CStaticStackArray<CBrushGeometry>     movableBrushes;
    CStaticStackArray<CSphereLight>       sphLights;
    CStaticStackArray<CDirectionalLight>  dirLights;

    // - Every entity can be either model or brush.
    // - Entity can have one light source "attached" to it.
    // Next maps are for getting associated SSRT object by entity ID
    // for updating their params, if they are created
    std::map<ULONG, CModelGeometry*>      entityToModel;
    std::map<ULONG, CBrushGeometry*>      entityToStaticBrush;
    std::map<ULONG, CBrushGeometry*>      entityToMovableBrush;
    std::map<ULONG, CSphereLight*>        entityToSphLight;
    std::map<ULONG, CDirectionalLight*>   entityToDirLight;

  public:
    void AddModel(const CModelGeometry &model);
    void AddBrush(const CBrushGeometry &brush);
    void AddLight(const CSphereLight &sphLt);
    void AddLight(const CDirectionalLight &dirLt);

  private:
    template<class T>
    void AddRTObject(const T &obj, CStaticStackArray<T> &arr, std::map<ULONG, T*> &entToObj);
  };
  
#pragma region template implementation
  template<class T>
  inline void SSRTMain::AddRTObject(const T &obj, CStaticStackArray<T> &arr, std::map<ULONG, T*> &entToObj)
  {
    ASSERT(obj.pOriginalEntity != nullptr);

    // try to find object by its enitity
    auto &found = entToObj.find(obj.GetEnitityID());

    if (found != entToObj.end())
    {
      // entities must be identical
      ASSERT(found->second->pOriginalEntity == obj.pOriginalEntity);

      // set new value
      *(found->second) = obj;
    }
    else
    {
      // if not found, add it
      T *pushed = &arr.Push();
      *pushed = obj;

      entToObj[obj.GetEnitityID()] = pushed;
    }
  }
#pragma endregion
}