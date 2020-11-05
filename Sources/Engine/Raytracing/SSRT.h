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
  CWorldRenderingInfo   worldRenderInfo;
  CTString              currentWorldName;
  // each first person model (left, right revolver, other weapons) should have its 
  // fake entity to attach to, this counter will be used to simulate ID for fake entity
  ULONG                 currentFirstPersonModelCount;

  // true, if rendering 3D world using ray tracing,
  // otherwise rasterization pass will be used (e.g. for HUD)
  bool                  isRenderingWorld;

  // these arrays hold information about all objects in a world
  std::vector<CModelGeometry>             models;
  std::vector<CBrushGeometry>             staticBrushes;
  std::vector<CBrushGeometry>             movableBrushes;
  std::vector<CSphereLight>               sphLights;
  std::vector<CDirectionalLight>          dirLights;

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
  void Init();
  
  void AddModel(const CModelGeometry &model);
  void AddBrush(const CBrushGeometry &brush, bool isMovable);
  void AddLight(const CSphereLight &sphLt);
  void AddLight(const CDirectionalLight &dirLt);

  void StartFrame();
  // Process world geometry and build acceleration structures
  void ProcessWorld(const CWorldRenderingInfo &info);
  // First person models are rendered directly from .es script,
  // so a separate function is required to handle this
  void ProcessFirstPersonModel(const CFirstPersonModelInfo &info);

  // Try to start rendering HUD: rasterization functions will be enabled,
  // if previously a world was being rendered (and return true)
  bool StartHUDRendering();
  void ProcessHUD(const CHudElementInfo &info);

  void EndFrame();

private:
  void SetWorld(CWorld *pwld);
  void StopWorld();

  // Get world from shell variable
  CWorld *GetCurrentWorld();

  template<class T>
  void AddRTObject(const T &obj, std::vector<T> &arr, std::map<ULONG, std::vector<INDEX>> &entToObjs);
};

#pragma region template implementation
template<class T>
inline void SSRTMain::AddRTObject(const T &obj, std::vector<T> &arr, std::map<ULONG, std::vector<INDEX>> &entToObjs)
{
  // try to find vector by its enitity
  auto &found = entToObjs.find(obj.entityID);

  if (found != entToObjs.end())
  {
    for (INDEX i : found->second)
    {
      // entities must be identical
      ASSERT(arr[i].entityID == obj.entityID);

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

    INDEX i = arr.size();
    arr.push_back(obj);

    found->second.push_back(i);
  }
  else
  {
    INDEX i = arr.size();
    arr.push_back(obj);

    entToObjs[obj.entityID].push_back(i);
  }
}
#pragma endregion
}