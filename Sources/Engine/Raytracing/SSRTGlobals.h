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
  INDEX srt_bDebugValidation = 0;
  INDEX srt_bVSync = 1;
  INDEX srt_iUpscaleMode = RG_RENDER_UPSCALE_TECHNIQUE_LINEAR;
  INDEX srt_iResolutionMode = RG_RENDER_RESOLUTION_MODE_CUSTOM;
  FLOAT srt_fResolutionScale = 1.0;
  INDEX srt_iOversharpMode = RG_RENDER_SHARPEN_TECHNIQUE_NONE;
  INDEX srt_bCRTMode = 0;
  FLOAT srt_fChromaticAberration = 0.2f;

  INDEX srt_bDLSSAvailable = 0; // variable for settings menu

  INDEX srt_bReloadShaders = 0;
  INDEX srt_bPrintBrushPolygonInfo = 0;
  INDEX srt_bEnableViewerShadows = 1;

  INDEX srt_bIgnoreDynamicTexCoords = 0;
  INDEX srt_bIgnoreWaterEffectTextureUpdates = 0;

  INDEX srt_iReflMaxDepth = 2;
  bool  srt_bReflRefrShadows = 0;
  bool  srt_bReflRefrToIndirect = 0;
  FLOAT srt_fReflRefrIndexOfRefractionGlass = 1.1f;
  FLOAT srt_fReflRefrIndexOfRefractionWater = 1.33f;
  FLOAT srt_fWaterDensityMultiplier = 5.0f;
  FLOAT3D srt_vWaterExtinction = { 0.025f, 0.016f, 0.011f };
  FLOAT srt_fWaterSpeed = 1.0f;
  FLOAT srt_fWaterNormalStrength = 1.0f;
  FLOAT srt_fWaterNormalSharpness = 1.0f;

  INDEX srt_bLightFixWithModels = 1;
  INDEX srt_bLightFixWithTranslucent = 0;

  FLOAT srt_fSunIntensity = 4.0f;
  FLOAT srt_fSunSaturation = 0.5f;
  FLOAT srt_fSunColorPow = 2.2f;
  FLOAT srt_fSunAngularDiameter = 0.5f;

  INDEX srt_iLightSphMaxCount = 1024;
  FLOAT srt_fLightSphGlobalIntensityMultiplier = 1.0f;
  FLOAT srt_fLightSphGlobalFalloffToIntensity = 1.0f;
  FLOAT srt_fLightSphSaturation = 0.5f;
  FLOAT srt_fLightSphColorPow = 2.2f;
  INDEX srt_bLightSphIgnoreEditorModels = 1;
  FLOAT srt_fLightSphPolygonOffset = 1.0f;
  INDEX srt_fLightSphFirefliesClamp = 3.0f;

  FLOAT srt_fOriginalLightSphIntensity = 1.0f;
  FLOAT srt_fOriginalLightSphRadiusMin = 0.01f;
  FLOAT srt_fOriginalLightSphRadiusMultiplier = 0.05f;
  FLOAT srt_fOriginalLightSphFalloffMultiplier = 1.0f;

  FLOAT srt_fDynamicLightSphIntensity = 1.0f;
  FLOAT srt_fDynamicLightSphRadius = 0.25f;
  FLOAT srt_fDynamicLightSphFalloffMultiplier = 5.0f;
  FLOAT srt_fDynamicLightSphFalloffMin = 3.0f;
  FLOAT srt_fDynamicLightSphFalloffMax = 128.0f;
 
  FLOAT srt_fPotentialLightSphDistanceThreshold = 1.0f;
  FLOAT srt_fPotentialLightSphIntensity = 1.0f;
  FLOAT srt_fPotentialLightSphRadiusMin = 0.1f;
  FLOAT srt_fPotentialLightSphRadiusMultiplier = 0.1f;
  FLOAT srt_fPotentialLightSphFalloffMultiplier = 1.0f;
  FLOAT srt_fPotentialLightSphFalloffDefault = 1.0f;
  FLOAT srt_fPotentialLightSphFalloffMin = 5.0f;

  FLOAT srt_fMuzzleLightIntensity = 0.25f;
  FLOAT srt_fMuzzleLightRadius = 0.02f;
  FLOAT srt_fMuzzleLightFalloffMultiplier = 10.0f;
  FLOAT srt_fMuzzleLightOffset = 5.0f;

  FLOAT srt_fModelSpecularMetallicDefault = 0.85f;
  FLOAT srt_fModelSpecularRoughnessDefault = 0.025f;
  FLOAT srt_fModelReflectiveMetallicDefault = 1.0f;
  FLOAT srt_fModelReflectiveRoughnessDefault = 0.0001f;
  FLOAT srt_fBrushMetallicDefault = 0.0f;
  FLOAT srt_fBrushRoughnessDefault = 0.9f;

  INDEX srt_bIndirRoughnessSqrt = 0;

  INDEX srt_bWeaponUseOriginalNormals = 0;
  FLOAT srt_fNormalMapStrength = 1.0f;

  FLOAT srt_fEmissionMapBoost = 16.0f;
  FLOAT srt_fEmissionMaxScreenColor = 32.0f;
  FLOAT srt_fEmissionForFullbright = 1.0f;

  FLOAT srt_fParticlesAlphaMultiplier = 1.0f;

  FLOAT srt_fSkyColorMultiplier = 1.0f;
  FLOAT srt_fSkyColorSaturation = 0.75f;

  INDEX srt_bTonemappingUseDefault = 0;
  FLOAT srt_fTonemappingWhitePoint = 10.0f;
  FLOAT srt_fTonemappingMinLogLuminance = -4.0f;
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
  FLOAT srt_fFlashlightIntensity = 50;
  
  INDEX srt_iMaxBounceShadows = 2;

  // If true, translucent models that are changing its alpha (disappering gibs/body parts/blood),
  // are uploaded as alpha-tested geometry
  INDEX srt_bModelChangeableTranslucentToAlphaTested = 1;

  INDEX srt_iCullingMaxSectorDepth = 6;
  // for settings menu: 0 - default, 1 - less, 1 - reduced, 2 - min
  INDEX srt_iCullingMaxSectorDepthQualityLevel = 0;
  FLOAT srt_fCullingMinAngularSize = 1.5f;
  FLOAT srt_fCullingThinSectorSize = 1.1f;

  FLOAT srt_fModelLODMul = 1.0f;
  FLOAT srt_fModelLODAdd = 0.0f;

  FLOAT2D srt_fRenderSize = {};

  FLOAT3D srt_vAnimatedSunTargetEuler = {};
  // Set current time as the origin time
  INDEX srt_bAnimatedSunRestart = 0;
  // Animation starts from (Origin + OffsetStart) time point
  FLOAT srt_fAnimatedSunTimeOffsetStart = 0;
  // Animation ends on (Origin + OffsetStart + Length) time point.
  // Animation is disabled, if it's negative
  FLOAT srt_fAnimatedSunTimeLength = 0;

  FLOAT srt_fBloomIntensity = 1.0f;
  FLOAT srt_fBloomInputThreshold = 15.0f;
  FLOAT srt_fBloomInputThresholdLength = 0.25f;
  FLOAT srt_fBloomUpsampleRadius = 1.0f;
  FLOAT srt_fBloomEmissionMultiplier = 32.0f;
  FLOAT srt_fBloomSkyMultiplier = 0.1f;

  INDEX srt_bLensFlares = 1;

  FLOAT srt_fFixSunIntensityGlobal = 0.75f;
  FLOAT srt_fFixSkyIntensityGlobal = 0.5f;
};

}
