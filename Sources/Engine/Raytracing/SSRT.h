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

#pragma once

#include "StdH.h"

#include <memory>
#include <vector>
#include <map>

#include <Engine/Entities/Entity.h>
#include <Engine/Brushes/Brush.h>

#include <Engine/Raytracing/SSRTObjects.h>

namespace SSRT
{
// TODO: RT: if SE1 doesn't provide info about deleted entities
// then just set isEnabled=false for each object before frame start
class SSRTMain
{
private:
  ULONG           screenWidth;
  ULONG           screenHeight;

  CTString        currentWorldName;

  // global arrays;
  // TODO: RT: requires allocator that won't reallocate already allocated memory,
  //           so pointers in CAbstractGeometry won't be obsolete
  CStaticStackArray<GFXVertex>           staticVertices;
  CStaticStackArray<GFXNormal>           staticNormals;
  CStaticStackArray<GFXTexCoord>         staticTexCoords;
  CStaticStackArray<INDEX>               staticIndices;

  // dynamic vertex data will be erased after each frame as it changes frequently
  CStaticStackArray<GFXVertex>           dynamicVertices;
  CStaticStackArray<GFXNormal>           dynamicNormals;
  CStaticStackArray<GFXTexCoord>         dynamicTexCoords;
  CStaticStackArray<INDEX>               dynamicIndices;


  // these arrays hold information about all objects in a world
  CStaticStackArray<CModelGeometry>       models;
  CStaticStackArray<CBrushGeometry>       staticBrushes;
  CStaticStackArray<CBrushGeometry>       movableBrushes;
  CStaticStackArray<CSphereLight>         sphLights;
  CStaticStackArray<CDirectionalLight>    dirLights;

  // - Every entity can be either model or brush
  // - A model can have attachments that are models too
  // - A brush can have several sectors
  // - An ntity can have one light source "attached" to it.
  // Next maps are for getting associated SSRT object by entity ID
  // for updating their params, if they are created
  std::map<ULONG, std::vector<INDEX>>     entityToModel;
  std::map<ULONG, std::vector<INDEX>>     entityToStaticBrush;
  std::map<ULONG, std::vector<INDEX>>     entityToMovableBrush;

public:
  void AddModel(const CModelGeometry &model);
  void AddBrush(const CBrushGeometry &brush);
  void AddLight(const CSphereLight &sphLt);
  void AddLight(const CDirectionalLight &dirLt);

  void StartFrame();
  void EndFrame();

private:
  void SetWorld(CWorld *pwld);
  
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

      if (obj == arr[i])
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