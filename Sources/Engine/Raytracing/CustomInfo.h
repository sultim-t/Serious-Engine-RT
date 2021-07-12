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

#include <string>
#include <vector>

#include <Engine/Math/Vector.h>
#include <Engine/World/World.h>
#include <Engine/Raytracing/SSRTObjects.h>

namespace SSRT
{

class CustomInfo
{
public:
  CustomInfo(CWorld *pWorld);
  ~CustomInfo();

  CustomInfo(const CustomInfo &other) = delete;
  CustomInfo(CustomInfo &&other) noexcept = delete;
  CustomInfo &operator=(const CustomInfo &other) = delete;
  CustomInfo &operator=(CustomInfo &&other) noexcept = delete;

  bool IsWaterTexture(CTextureData *ptd) const;
  bool IsFireTexture(CTextureData *ptd) const;
  bool HasModelFireTexture(CEntity *penModel) const;

  bool IsAngularSizeCullingDisabled(CEntity *penModel) const;
  bool IsReflectiveForced(CTextureObject *pTo) const;
  bool IsAlphaTestForced(CTextureObject *pTo, bool isTranslucent) const;

  bool IsBrushIgnored(CEntity *penBrush) const;
  bool AreDynamicTexCoordsIgnored(CEntity *penBrush) const;

  bool IsDirectionalLightIgnored(const CLightSource *plsLight) const;
  bool IsSphericalLightIgnored(const CLightSource *plsLight) const;
  FLOAT3D GetAnimatedSunDirection(const FLOAT3D &vOriginalEuler) const;
  FLOAT GetCloudsOpacity() const;

  void Update();
  void OnSkyBrushAdd(const CBrushGeometry &brush);

private:
  static bool IsTextureNameIn(CTextureData *ptd, const char * const pCollection[], uint32_t iCount);
  bool HasLightEntityVertices(CEntity *pen) const;

  void EnableFlashlightHint();
  void DisableFlashlightHint();

private:
  std::vector<FLOAT3D>     worldBasePositionsToIgnore;
  std::vector<std::string> dirLightsNamesToIgnore;

  std::vector<CTextureData *> ptdCachedBulletHoleTextures;

  bool isFlashlightHintEnabled;
  TIME tmFlashlightHintStart;
  TIME tmFlashlightHintEnd;

  TIME tmAnimatedSunOrigin;

  bool bSunIntensityDependsOnSkyClouds;
  FLOAT fSkyCloudsOpacity;
};

}
