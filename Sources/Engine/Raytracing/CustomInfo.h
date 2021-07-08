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
  bool IsBrushIgnored(CEntity *penBrush) const;
  bool IsDirectionalLightIgnored(const CLightSource *plsLight) const;
  bool IsSphericalLightIgnored(const CLightSource *plsLight) const;
  bool AreDynamicTexCoordsIgnored(CEntity *penBrush) const;

private:
  static bool IsTextureNameIn(CTextureData *ptd, const std::vector<std::string> &collection);
  bool HasLightEntityVertices(CEntity *pen) const;

private:
  struct WorldBaseIgnore
  {
    std::string worldName;
    FLOAT3D position;
  };

  struct LightIgnore
  {
    std::string worldName;
    std::string entityName;
  };

private:
  std::vector<std::string> waterTextureNames;
  std::vector<std::string> fireTextureNames;
  std::vector<WorldBaseIgnore> worldBaseToIgnore;
  std::vector<LightIgnore> dirLightsToIgnore;
  // Names of worlds in which dynamic tex coords are ignored
  std::vector<std::string> dynTexCoordsWldToIgnore;
};

}
