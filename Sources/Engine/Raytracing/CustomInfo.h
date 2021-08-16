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
  enum class EWorld
  {
    Other,
    Hatshepsut,
    SandCanyon,
    TombOfRamses,
    ValleyOfTheKings,
    MoonMountains,
    Oasis,
    Dunes,
    Suburbs,
    Sewers,
    Metropolis,
    AlleyOfSphinxes,
    Karnak,
    Luxor,
    SacredYards,
    TheGreatPyramid,
  };

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
  bool IsFullMetallicForced(CTextureObject *pTo) const;
  bool IsAlphaTestForced(CTextureObject *pTo, bool isTranslucent) const;
  bool IsAlphaTestForcedForParticles(CTextureData *pTd) const;
  bool IsEmissionForced(CTextureObject *pTo) const;
  bool IsReflectRefractForced(CTextureObject *pTo) const;
  bool IsReflectForced(CTextureObject *pTo) const;
  bool IsCalcNormalsForced(CTextureObject *pTo) const;

  bool IsBrushIgnored(CEntity *penBrush) const;
  bool IsBrushSectorIgnored(const CBrushSector *pSector) const;
  bool IsBrushPolygonIgnored(const CBrushPolygon *pPolygon) const;
  bool AreDynamicTexCoordsIgnored(CEntity *penBrush) const;
  
  RgGeometryPrimaryVisibilityType GetBrushMaskBit(const CBrushPolygon *pPolygon) const;
  uint32_t GetCullMask(const FLOAT3D &vCameraPosition) const;

  bool IsDirectionalLightIgnored(const CLightSource *plsLight) const;
  bool IsSphericalLightIgnored(const CLightSource *plsLight) const;
  FLOAT3D GetAnimatedSunDirection(const FLOAT3D &vOriginalEuler) const;
  FLOAT GetCloudsOpacity() const;
  FLOAT GetSkyIntensity() const;
  FLOAT GetSkySaturation() const;
  RgSkyType GetLevelSkyType() const;

  void Update(const FLOAT3D &vCameraPosition);
  void OnSkyBrushAdd(const CBrushGeometry &brush);

private:
  bool HasLightEntityVertices(CEntity *pen) const;

  void EnableFlashlightHint();
  void DisableFlashlightHint();

private:

  struct IgnoredBrushSector
  {    
    INDEX iBrushSectorIndex;
  };
  struct IgnoredBrushPoly
  {
    INDEX iBrushPolygonIndex;
  };
  struct IgnoredBrushPolyRange
  {
    INDEX iBrushPolygonIndexStart;
    INDEX iBrushPolygonIndexEnd;
  };

  struct MaskedBrushSector
  {
    INDEX iBrushSectorIndex;
    RgGeometryPrimaryVisibilityType eMaskBit;
  };
  struct MaskedBrushPoly
  {
    INDEX iBrushPolygonIndex;
    RgGeometryPrimaryVisibilityType eMaskBit;
  };
  struct MaskedBrushPolyRange
  {
    INDEX iBrushPolygonIndexStart;
    INDEX iBrushPolygonIndexEnd;
    RgGeometryPrimaryVisibilityType eMaskBit;
  };

private:
  EWorld eCurrentWorld;

  bool bCheckOnlyZoningForBrushIgnored;
  std::vector<FLOAT3D> worldBasePositionsToIgnore;
  
  std::vector<std::string> dirLightsNamesToIgnore;

  struct
  {
    std::vector<CTextureData *> aDisabledCulling;
    std::vector<CTextureData *> aAlphaTest;
    std::vector<CTextureData *> aAlphaTestParticles;
    std::vector<CTextureData *> aFullyMetallic;
    std::vector<CTextureData *> aEmission;
    std::vector<CTextureData *> aReflectRefract;
    std::vector<CTextureData *> aReflect;
    std::vector<CTextureData *> aCalcNormals;
    std::vector<CTextureData *> aWater;
    std::vector<CTextureData *> aFire;

  } ptdCachedTextures;


  bool isFlashlightHintEnabled;
  TIME tmFlashlightHintStart;
  TIME tmFlashlightHintEnd;

  TIME tmAnimatedSunOrigin;

  TIME tmAnimatedSkyIntensityOrigin;

  bool bSunIntensityDependsOnSkyClouds;
  FLOAT fSkyCloudsOpacity;

  bool bSkyboxIntensityDependsOnSkyClouds;
  FLOAT fSkyboxCloudyIntensity;

  std::vector<IgnoredBrushPolyRange> brushPolygonRangesToIgnore;
  std::vector<IgnoredBrushPoly> brushPolygonsToIgnore;
  std::vector<IgnoredBrushSector> brushSectorsToIgnore;

  std::vector<MaskedBrushPolyRange> brushPolygonRangesToMask;
  std::vector<MaskedBrushPoly> brushPolygonsToMask;
  std::vector<MaskedBrushSector> brushSectorsToMask;
};

}
