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

#pragma once

#include <map>
#include <vector>

#include <Engine/Raytracing/SSRTObjects.h>
#include <Engine/Raytracing/TextureUploader.h>

namespace SSRT
{

class Scene
{
public:
  Scene(RgInstance instance, CWorld *pWorld, TextureUploader *textureUploader);
  ~Scene();

  Scene(const Scene &other) = delete;
  Scene(Scene &&other) noexcept = delete;
  Scene &operator=(const Scene &other) = delete;
  Scene &operator=(Scene &&other) noexcept = delete;

  void AddModel(const CModelGeometry &model);
  void AddBrush(const CBrushGeometry &brush);
  void AddLight(const CSphereLight &sphLt);
  void AddLight(const CDirectionalLight &dirLt);

  void AddFirstPersonModel(const CFirstPersonModelInfo &info, ULONG entityId);
  void Update(const FLOAT3D &viewerPos, ULONG viewerEntityID);

  const CTString &GetWorldName() const;

private:
  void ProcessBrushes();
  void ProcessDynamicGeometry(const FLOAT3D &viewerPos, ULONG viewerEntityID);
  void UpdateMovableBrush(ULONG entityId, const CPlacement3D &placement);

private:
  RgInstance instance;
  TextureUploader *textureUploader;

  // - Every entity can be either model or brush
  // - A model can have attachments that are models too
  // - A brush can have several sectors (but they're combined 
  //        in one geometry while processing so vector.size()==1)
  // - An entity can have one light source "attached" to it.
  CWorld *pWorld;
  CTString worldName;

  // Get movable brush geometry index by entity ID
  std::map<ULONG, RgGeometry>     entityToMovableBrush;

  // Scene light sources, cleaned up by the end of a frame
  std::vector<CSphereLight>       sphLights;
  std::vector<CDirectionalLight>  dirLights;
};

}
