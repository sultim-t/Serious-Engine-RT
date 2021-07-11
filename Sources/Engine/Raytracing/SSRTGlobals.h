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

  FLOAT srt_fSunIntensity = 4.0f;
  FLOAT srt_fSunSaturation = 0.5f;
  FLOAT srt_fSunColorPow = 2.2f;
  FLOAT srt_fSunAngularDiameter = 0.5f;

  INDEX srt_iLightSphMaxCount = 1024;
  FLOAT srt_fLightSphSaturation = 0.75f;
  FLOAT srt_fLightSphColorPow = 2.2f;
  FLOAT srt_fLightSphPolygonOffset = 1.0f;
  INDEX srt_bLightSphIgnoreEditorModels = 0;

  FLOAT srt_fOriginalLightSphIntensity = 1.0f;
  FLOAT srt_fOriginalLightSphRadiusMultiplier = 0.05f;
  FLOAT srt_fOriginalLightSphFalloffMultiplier = 1.0f;

  FLOAT srt_fDynamicLightSphIntensity = 1.0f;
  FLOAT srt_fDynamicLightSphRadius = 0.25f;
  FLOAT srt_fDynamicLightSphFalloffMultiplier = 3.0f;
  FLOAT srt_fDynamicLightSphFalloffMin = 3.0f;
  FLOAT srt_fDynamicLightSphFalloffMax = 128.0f;
 
  FLOAT srt_fPotentialLightSphIntensity = 1.0f;
  FLOAT srt_fPotentialLightSphRadiusMultiplier = 0.5f;
  FLOAT srt_fPotentialLightSphFalloffMultiplier = 0.25f;

  FLOAT srt_fMuzzleLightIntensity = 1.0f;
  FLOAT srt_fMuzzleLightRadius = 0.25f;
  FLOAT srt_fMuzzleLightFalloffMultiplier = 2.0f;
  FLOAT srt_fMuzzleLightOffset = 3.0f;

  FLOAT srt_fModelSpecularMetallicDefault = 0.85f;
  FLOAT srt_fModelSpecularRoughnessDefault = 0.025f;
  FLOAT srt_fModelReflectiveMetallicDefault = 1.0f;
  FLOAT srt_fModelReflectiveRoughnessDefault = 0.0001f;

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
  FLOAT srt_fTonemappingWhitePoint = 1.0f;
  FLOAT srt_fTonemappingMinLogLuminance = -2.0f;
  FLOAT srt_fTonemappingMaxLogLuminance = 0.0f;

  INDEX srt_bFlashlightEnable = 0;
  INDEX srt_bFlashlightEnableTutorial = 0;
  FLOAT3D srt_vFlashlightOffset = { -1.0f, -0.25f, 0.0f }; 
  FLOAT3D srt_vFlashlightOffsetThirdPerson = { 0.15f, 0.4f, 0.2f };
  FLOAT3D srt_vFlashlightColor = { 2.5f, 2.5f, 2.5f };
  FLOAT srt_fFlashlightRadius = 0.05f;
  // In degrees
  ANGLE srt_fFlashlightAngleOuter = 15.0f;
  // In degrees
  ANGLE srt_fFlashlightAngleInner = 0.0f;
  FLOAT srt_fFlashlightFalloffDistance = 50.0f;

  INDEX srt_bMaxBounceShadowsUseDefault = 0;
  INDEX srt_iMaxBounceShadowsDirectionalLights = 8;
  INDEX srt_iMaxBounceShadowsSphereLights = 1;
  INDEX srt_iMaxBounceShadowsSpotlights = 2;

  // If true, translucent models that are changing its alpha (disappering gibs/body parts/blood),
  // are uploaded as alpha-tested geometry
  INDEX srt_bModelChangeableTranslucentToAlphaTested = 1;

  INDEX srt_iCullingMaxSectorDepth = 6;
  FLOAT srt_fCullingMinAngularSize = 1.5f;
  FLOAT srt_fCullingThinSectorSize = 1.1f;

  FLOAT srt_fModelLODMul = 1.0f;
  FLOAT srt_fModelLODAdd = 0.0f;

  FLOAT2D srt_fRenderSize = {};
};

}
