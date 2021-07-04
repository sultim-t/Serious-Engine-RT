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

#include <Engine/Entities/Entity.h>
#include <RTGL1/RTGL1.h>

namespace SSRT
{

struct SSRTGlobals
{
  INDEX srt_bVSync = 1;
  INDEX srt_bReloadShaders = 0;
  INDEX srt_bTexturesOriginalSRGB = 1;
  INDEX srt_bEnableViewerShadows = 1;
  INDEX srt_bIgnoreDynamicTexCoords = 0;
  INDEX srt_bShowGradients = 0;
  INDEX srt_bShowMotionVectors = 0;

  INDEX srt_bLightFixWithModels = 0;
  INDEX srt_bLightFixWithTranslucent = 0;

  FLOAT srt_fLightDirectionalIntensityMultiplier = 10.0f;
  FLOAT srt_fLightDirectionalSaturation = 0.5f;
  FLOAT srt_fLightDirectionalColorPow = 2.2f;
  FLOAT srt_fLightDirectionalAngularDiameter = 0.5f;
  INDEX srt_iLightSphericalMaxCount = 1024;
  INDEX srt_iLightSphericalHSVThresholdHLower = 20;
  INDEX srt_iLightSphericalHSVThresholdHUpper = 45;
  INDEX srt_iLightSphericalHSVThresholdVLower = 80;
  INDEX srt_iLightSphericalHSVThresholdVUpper = 255;
  FLOAT srt_fLightSphericalIntensityMultiplier = 1.0f;
  FLOAT srt_fLightSphericalSaturation = 0.75f;
  FLOAT srt_fLightSphericalColorPow = 2.2f;
  FLOAT srt_fLightSphericalRadiusMultiplier = 0.1f;
  FLOAT srt_fLightSphericalRadiusOfDynamic = 0.5f;
  FLOAT srt_fLightSphericalRadiusOfPotentialMultiplier = 0.5f;
  FLOAT srt_fLightSphericalFalloffMultiplier = 1.0f;
  FLOAT srt_fLightSphericalFalloffOfDynamicMultiplier = 1.0f;
  FLOAT srt_fLightSphericalFalloffOfPotentialMultiplier = 0.3f;
  FLOAT srt_fLightSphericalFalloffOfDynamicMin = 1.0f;
  FLOAT srt_fLightSphericalFalloffOfDynamicMax = 8.0f;
  FLOAT srt_fLightSphericalPolygonOffset = 1.0f;
  INDEX srt_bLightSphericalIgnoreEditorModels = 0;
  FLOAT srt_fLightMuzzleOffset = 2.0f;

  FLOAT srt_fDefaultSpecularMetallic = 0.7f;
  FLOAT srt_fDefaultSpecularRoughness = 0.02f;
  FLOAT srt_fDefaultReflectiveMetallic = 1.0f;
  FLOAT srt_fDefaultReflectiveRoughness = 0.0f;

  INDEX srt_bWeaponUseOriginalNormals = 0;
  FLOAT srt_fNormalMapStrength = 1.0f;

  FLOAT srt_fEmissionMapBoost = 100.0f;
  FLOAT srt_fEmissionMaxScreenColor = 2.0f;

  FLOAT srt_fParticlesAlphaMultiplier = 1.0f;

  RgSkyType srt_iSkyType = RG_SKY_TYPE_RASTERIZED_GEOMETRY;
  FLOAT srt_fSkyColorMultiplier = 1.0f;
  FLOAT srt_fSkyColorSaturation = 0.7f;
  FLOAT3D srt_fSkyColorDefault = { 1, 1, 1 };

  INDEX srt_bTonemappingUseDefault = 0;
  FLOAT srt_fTonemappingWhitePoint = 1.5f;
  FLOAT srt_fTonemappingMinLogLuminance = 2.0f;
  FLOAT srt_fTonemappingMaxLogLuminance = 10.0f;

  INDEX srt_bSpotlightEnable = 0;
  FLOAT3D srt_vSpotlightOffset = { -1.0f, -0.25f, 0.0f };
  FLOAT3D srt_vSpotlightColor = { 2.5f, 2.5f, 2.5f };
  FLOAT srt_fSpotlightRadius = 0.05f;
  // In degrees
  ANGLE srt_fSpotlightAngleOuter = 15.0f;
  // In degrees
  ANGLE srt_fSpotlightAngleInner = 3.0f;
  FLOAT srt_fSpotlightFalloffDistance = 50.0f;
};

}
