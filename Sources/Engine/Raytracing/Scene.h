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
  void AddParticles(const CParticlesGeometry &info);
  void AddBrush(const CBrushGeometry &brush);
  void AddLight(const CSphereLight &sphLt);
  void AddLight(const CDirectionalLight &dirLt);
  ULONG GetViewerEntityID() const;
  const FLOAT3D &GetViewerPosition() const;
  const FLOATmatrix3D &GetViewerRotation() const;

  void AddFirstPersonModel(const CFirstPersonModelInfo &info, ULONG entityId);
  void Update(const FLOAT3D &viewerPosition, const FLOATmatrix3D &viewerRotation, ULONG viewerEntityID);

  const CTString &GetWorldName() const;
  const CWorld *GetWorld() const;
  CWorld *GetWorld();

  static void InitShellVariables();

private:
  void ProcessBrushes();
  void ProcessDynamicGeometry();
  void UpdateMovableBrush(ULONG entityId, const CPlacement3D &placement);
  void HideMovableBrush(ULONG entityId);

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

  ULONG viewerEntityID;
  FLOAT3D viewerPosition;
  FLOATmatrix3D viewerRotation;

  // Get movable brush geometry index by entity ID
  std::map<ULONG, std::vector<RgGeometry>> entityToMovableBrush;

  // Scene light sources, cleaned up by the end of a frame
  std::vector<CSphereLight>       sphLights;
  std::vector<CDirectionalLight>  dirLights;
};

}
