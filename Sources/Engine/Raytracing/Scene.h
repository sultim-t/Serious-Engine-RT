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
#include <set>

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
  void UpdateBrushNonStaticTexture(CTextureData *pTexture, uint32_t textureFrameIndex);
  void UpdateBrushTexCoords(const CUpdateTexCoordsInfo &info);
  ULONG GetViewerEntityID() const;
  const FLOAT3D &GetViewerPosition() const;
  const FLOATmatrix3D &GetViewerRotation() const;

  void ProcessFirstPersonModel(const CFirstPersonModelInfo &info, ULONG entityId);
  void Update(const FLOAT3D &viewerPosition, const FLOATmatrix3D &viewerRotation, ULONG viewerEntityID);

  const CTString &GetWorldName() const;
  const CWorld *GetWorld() const;
  CWorld *GetWorld();
  FLOAT3D GetBackgroundViewerPosition() const;
  ANGLE3D GetBackgroundViewerOrientationAngle() const;

  static void InitShellVariables();
  static void NormalizeShellVariables();

private:
  struct BrushPartGeometryIndex
  {
    uint32_t brushPartIndex;
    uint32_t vertexCount;
  };

private:
  void ProcessBrushes();
  void ProcessDynamicGeometry();
  void UpdateMovableBrush(ULONG entityId, const CPlacement3D &placement);
  void HideMovableBrush(ULONG entityId);

private:
  RgInstance        instance;
  TextureUploader   *textureUploader;

  // - Every entity can be either model or brush
  // - A model can have attachments that are models too
  // - A brush can have several sectors (but they're combined 
  //        in one geometry while processing so vector.size()==1)
  // - An entity can have one light source "attached" to it.
  CWorld            *pWorld;
  CTString          worldName;

  ULONG             viewerEntityID;
  FLOAT3D           viewerPosition;
  FLOATmatrix3D     viewerRotation;

  // Movable brush's entity ID to uniqueIDs of that entity's parts
  std::map<ULONG, std::vector<uint64_t>>    entityToMovableBrush;

  // True, if entity with that ID (key) has effect texture.
  // Used for updating it every frame, as brushes are processed only once.
  std::map<ULONG, bool>                     entityHasNonStaticTexture;

  // Get brush parts' geometry indices by entity ID.
  // Used for updating dynamic texture coordinates on brushes.
  std::map<ULONG, std::vector<BrushPartGeometryIndex>> entitiesWithDynamicTexCoords;

  // Scene light sources, cleaned up by the end of a frame
  std::vector<CSphereLight>       sphLights;
  std::vector<CDirectionalLight>  dirLights;
};

}
