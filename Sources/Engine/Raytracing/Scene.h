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

#include <vector>
#include <unordered_set>

#include "SSRTObjects.h"
#include "TextureUploader.h"
#include "SceneBrushes.h"
#include "CustomInfo.h"

namespace SSRT
{

class Scene
{
public:
  Scene(RgInstance instance, CWorld *pWorld, TextureUploader *pTextureUploader);
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
  void AddLight(const CSpotLight &spotLt);
  void UpdateBrush(CEntity *pEntity);
  void UpdateBrushNonStaticTexture(CTextureData *pTexture, uint32_t textureFrameIndex);
  void UpdateBrushTexCoords(const CUpdateTexCoordsInfo &info);
  void AddWarpPortal(CEntity *penBrush, INDEX iMirrorType);
  void SetCameraIsInHaze();

  CEntity *GetViewerEntity() const;
  ULONG GetViewerEntityID() const;
  const FLOAT3D &GetCameraPosition() const;
  const FLOATmatrix3D &GetCameraRotation() const;

  void ProcessFirstPersonModel(const CFirstPersonModelInfo &info, ULONG entityId);
  void Update(const CWorldRenderingInfo &info);
  void ResetOnGameStop();

  const CTString &GetWorldName() const;
  const CWorld *GetWorld() const;
  CDrawPort *GetDrawPort();
  CAnyProjection3D &GetSEProjection();
  CAnyProjection3D &GetSEProjectionBackground();
  CWorld *GetWorld();
  FLOAT3D GetBackgroundViewerPosition() const;
  ANGLE3D GetBackgroundViewerOrientationAngle() const;
  void GetNearestToCameraPortalInfo(FLOAT3D &vRefPortalInputPos, FLOAT3D &vRefPortalOutputPos, FLOATmatrix3D &mRefPortalRelativeRotation) const;
  bool HasBrushWarpPortal(CEntity *penBrush) const;
  bool IsCameraInHaze() const;
  static bool IsBrushMovable(CEntity *penBrush);

  const CustomInfo *GetCustomInfo() const;
  CustomInfo *GetCustomInfo();

  void OnFrameEnd(bool isCameraFirstPerson);

private:
  void ProcessBrushes();
  void ProcessDynamicGeometry();

private:
  struct FlashlightState
  {
    bool isAdded;
    RgSpotLightUploadInfo spotlightInfo;
  };

  struct WarpPortalState
  {
    CEntity *penBrush;
    INDEX iMirrorType;

    bool operator==(const WarpPortalState &s) const
    {
      ASSERT(this->penBrush != nullptr && s.penBrush != nullptr);
      return this->penBrush->en_ulID == s.penBrush->en_ulID && this->iMirrorType == s.iMirrorType;
    }
  };

  struct WarpPortalStateHash
  {
    size_t operator()(const WarpPortalState &s) const
    {
      return ((uint64_t)s.iMirrorType << 32ULL) | (uint64_t)s.penBrush->en_ulID;
    }
  };

private:
  RgInstance        instance;
  TextureUploader   *pTextureUploader;
  SceneBrushes      *pSceneBrushes;
  CustomInfo        *pCustomInfo;

  // - Every entity can be either model or brush
  // - A model can have attachments that are models too
  // - A brush can have several sectors (but they're combined 
  //        in one geometry while processing so vector.size()==1)
  // - An entity can have one light source "attached" to it.
  CWorld            *pWorld;
  CTString          worldName;

  CEntity           *pViewerEntity;
  FLOAT3D           cameraPosition;
  FLOATmatrix3D     cameraRotation;

  CDrawPort         *pdpDrawport{};
  CAnyProjection3D  prSEProjection;
  CAnyProjection3D  prSEProjectionBackground;

  bool              isCameraInHaze{};

  // Scene light sources, cleaned up by the end of a frame
  std::vector<CSphereLight>         sphLights{};
  std::vector<CDirectionalLight>    dirLights{};
  FlashlightState                   firstPersonFlashlight{};
  FlashlightState                   thirdPersonFlashlight{};
  // Used to correct flashlight outer angle
  float                             currentFOV{};
  
  std::unordered_set<WarpPortalState, WarpPortalStateHash> warpPortals{};
};

}
