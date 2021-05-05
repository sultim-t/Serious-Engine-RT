/* Copyright (c) 2021 Sultim Tsyrendashiev
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
#include <set>
#include <vector>

#include <Engine/Raytracing/SSRTObjects.h>

namespace SSRT
{

class TextureUploader;
class Scene;

class SceneBrushes
{
public:
  SceneBrushes(RgInstance instance, CWorld *pWorld, TextureUploader *pTextureUploader);
  ~SceneBrushes();

  SceneBrushes(const SceneBrushes &other) = delete;
  SceneBrushes(SceneBrushes &&other) noexcept = delete;
  SceneBrushes & operator=(const SceneBrushes &other) = delete;
  SceneBrushes & operator=(SceneBrushes &&other) noexcept = delete;

  // These functions are called from RTProcessing
  void RegisterBrush(const CBrushGeometry &brush);
  void UpdateBrushTexCoords(const CUpdateTexCoordsInfo &info);

  // Called each frame
  void Update(CEntity *pBrushEntity, Scene *pScene);

private:
  void RegisterBrushTextures(const CBrushGeometry &brush);

  void UpdateMovableBrush(CEntity *pBrushEntity);
  void SetMovableNewTransform(ULONG entityId, const CPlacement3D &placement);
  void SetMovableAsHidden(ULONG entityId);
  static bool ArePlacementsSame(const CPlacement3D &a, const CPlacement3D &b);

private:
  struct BrushPartGeometryIndex
  {
    uint32_t brushPartIndex;
    uint32_t vertexCount;
  };

  struct MovableState
  {
    bool isHidden = false;
    // True, if was moving in previous frame.
    // Need for updating transform in next frame
    // (otherwise motion vectors are incorrect)
    bool wasMoving = true;
    CPlacement3D placement;
  };

private:
  RgInstance instance;
  TextureUploader *pTextureUploader;

  // Movable brush's entity ID to uniqueIDs of that entity's parts
  std::map<ULONG, std::vector<uint64_t>>  entityToMovableBrush;
  // Used to check if movable brush actually moved
  std::map<ULONG, MovableState>           entityToMovableBrushPlacement;

  // True, if entity with that ID (key) has effect texture.
  // Used for updating it every frame, as brushes are processed only once.
  std::set<ULONG>                         entityHasNonStaticTexture;

  // Get brush parts' geometry indices by entity ID.
  // Used for updating dynamic texture coordinates on brushes.
  std::map<ULONG, std::vector<BrushPartGeometryIndex>> entitiesWithDynamicTexCoords;

  // Rasterized brushes should be sent to rendering each frame
  std::set<ULONG>                         entityIsRasterizedBrush;
};

}
