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


#include "StdH.h"
#include "SSRT.h"

#include <stdio.h>

#include <Engine/Graphics/ViewPort.h>
#include <Engine/World/World.h>
#include <Engine/Base/Shell.h>

#include <Engine/Templates/DynamicContainer.cpp>

#include "SSRTObjects.h"
#include "SSRTGlobals.h"
#include "Utils.h"


extern SSRT::SSRTGlobals _srtGlobals = SSRT::SSRTGlobals();


void SSRT::SSRTMain::InitShellVariables()
{
  _pShell->DeclareSymbol("persistent user INDEX srt_bDebugValidation;", &_srtGlobals.srt_bDebugValidation);
  _pShell->DeclareSymbol("persistent user INDEX srt_bVSync;", &_srtGlobals.srt_bVSync);
  _pShell->DeclareSymbol("persistent user INDEX srt_bTonemappingUseDefault;", &_srtGlobals.srt_bTonemappingUseDefault);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fTonemappingWhitePoint;", &_srtGlobals.srt_fTonemappingWhitePoint);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fTonemappingMinLogLuminance;", &_srtGlobals.srt_fTonemappingMinLogLuminance);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fTonemappingMaxLogLuminance;", &_srtGlobals.srt_fTonemappingMaxLogLuminance);
  _pShell->DeclareSymbol("           user INDEX srt_bShowGradients;", &_srtGlobals.srt_bShowGradients);
  _pShell->DeclareSymbol("           user INDEX srt_bShowMotionVectors;", &_srtGlobals.srt_bShowMotionVectors);
  _pShell->DeclareSymbol("           user INDEX srt_bReloadShaders;", &_srtGlobals.srt_bReloadShaders);
  _pShell->DeclareSymbol("           user INDEX srt_bPrintBrushPolygonInfo;", &_srtGlobals.srt_bPrintBrushPolygonInfo);
  _pShell->DeclareSymbol("persistent user INDEX srt_bTexturesOriginalSRGB;", &_srtGlobals.srt_bTexturesOriginalSRGB);
  _pShell->DeclareSymbol("persistent user INDEX srt_bIgnoreDynamicTexCoords;", &_srtGlobals.srt_bIgnoreDynamicTexCoords);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fSkyColorMultiplier;", &_srtGlobals.srt_fSkyColorMultiplier);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fSkyColorSaturation;", &_srtGlobals.srt_fSkyColorSaturation);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fNormalMapStrength;", &_srtGlobals.srt_fNormalMapStrength);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fEmissionMapBoost;", &_srtGlobals.srt_fEmissionMapBoost);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fEmissionMaxScreenColor;", &_srtGlobals.srt_fEmissionMaxScreenColor);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fEmissionForFullbright;", &_srtGlobals.srt_fEmissionForFullbright);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fModelSpecularMetallicDefault;", &_srtGlobals.srt_fModelSpecularMetallicDefault);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fModelSpecularRoughnessDefault;", &_srtGlobals.srt_fModelSpecularRoughnessDefault);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fModelReflectiveMetallicDefault;", &_srtGlobals.srt_fModelReflectiveMetallicDefault);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fModelReflectiveRoughnessDefault;", &_srtGlobals.srt_fModelReflectiveRoughnessDefault);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fBrushRoughnessDefault;", &_srtGlobals.srt_fBrushRoughnessDefault);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fBrushMetallicDefault;", &_srtGlobals.srt_fBrushMetallicDefault);

  _pShell->DeclareSymbol("persistent user INDEX srt_bEnableViewerShadows;", &_srtGlobals.srt_bEnableViewerShadows);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fSunIntensity;", &_srtGlobals.srt_fSunIntensity);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fSunSaturation;", &_srtGlobals.srt_fSunSaturation);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fSunColorPow;", &_srtGlobals.srt_fSunColorPow);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fSunAngularDiameter;", &_srtGlobals.srt_fSunAngularDiameter);

  _pShell->DeclareSymbol("persistent user INDEX srt_iLightSphMaxCount;", &_srtGlobals.srt_iLightSphMaxCount);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightSphSaturation;", &_srtGlobals.srt_fLightSphSaturation);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightSphColorPow;", &_srtGlobals.srt_fLightSphColorPow);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fLightSphPolygonOffset;", &_srtGlobals.srt_fLightSphPolygonOffset);
  _pShell->DeclareSymbol("persistent user INDEX srt_bLightSphIgnoreEditorModels;", &_srtGlobals.srt_bLightSphIgnoreEditorModels);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fOriginalLightSphIntensity;", &_srtGlobals.srt_fOriginalLightSphIntensity);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fOriginalLightSphRadiusMultiplier;", &_srtGlobals.srt_fOriginalLightSphRadiusMultiplier);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fOriginalLightSphFalloffMultiplier;", &_srtGlobals.srt_fOriginalLightSphFalloffMultiplier);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fDynamicLightSphIntensity;", &_srtGlobals.srt_fDynamicLightSphIntensity);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDynamicLightSphRadius;", &_srtGlobals.srt_fDynamicLightSphRadius);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDynamicLightSphFalloffMultiplier;", &_srtGlobals.srt_fDynamicLightSphFalloffMultiplier);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDynamicLightSphFalloffMin;", &_srtGlobals.srt_fDynamicLightSphFalloffMin);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fDynamicLightSphFalloffMax;", &_srtGlobals.srt_fDynamicLightSphFalloffMax);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fPotentialLightSphIntensity;", &_srtGlobals.srt_fPotentialLightSphIntensity);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fPotentialLightSphRadiusMin;", &_srtGlobals.srt_fPotentialLightSphRadiusMin);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fPotentialLightSphRadiusMultiplier;", &_srtGlobals.srt_fPotentialLightSphRadiusMultiplier);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fPotentialLightSphFalloffMultiplier;", &_srtGlobals.srt_fPotentialLightSphFalloffMultiplier);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fPotentialLightSphFalloffDefault;", &_srtGlobals.srt_fPotentialLightSphFalloffDefault);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fMuzzleLightIntensity;", &_srtGlobals.srt_fMuzzleLightIntensity);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fMuzzleLightRadius;", &_srtGlobals.srt_fMuzzleLightRadius);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fMuzzleLightFalloffMultiplier;", &_srtGlobals.srt_fMuzzleLightFalloffMultiplier);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fMuzzleLightOffset;", &_srtGlobals.srt_fMuzzleLightOffset);

  _pShell->DeclareSymbol("persistent user INDEX srt_bLightFixWithModels;", &_srtGlobals.srt_bLightFixWithModels);
  _pShell->DeclareSymbol("persistent user INDEX srt_bLightFixWithTranslucent;", &_srtGlobals.srt_bLightFixWithTranslucent);

  _pShell->DeclareSymbol("persistent user INDEX srt_bFlashlightEnableTutorial;", &_srtGlobals.srt_bFlashlightEnableTutorial);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fFlashlightRadius;", &_srtGlobals.srt_fFlashlightRadius);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fFlashlightAngleOuter;", &_srtGlobals.srt_fFlashlightAngleOuter);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fFlashlightAngleInner;", &_srtGlobals.srt_fFlashlightAngleInner);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fFlashlightFalloffDistance;", &_srtGlobals.srt_fFlashlightFalloffDistance);

  _pShell->DeclareSymbol("persistent user FLOAT srt_vFlashlightOffset[3];", &_srtGlobals.srt_vFlashlightOffset);
  _pShell->DeclareSymbol("persistent user FLOAT srt_vFlashlightOffsetThirdPerson[3];", &_srtGlobals.srt_vFlashlightOffsetThirdPerson);
  _pShell->DeclareSymbol("persistent user FLOAT srt_vFlashlightColor[3];", &_srtGlobals.srt_vFlashlightColor);

  _pShell->DeclareSymbol("persistent user INDEX srt_bMaxBounceShadowsUseDefault;", &_srtGlobals.srt_bMaxBounceShadowsUseDefault);
  _pShell->DeclareSymbol("persistent user INDEX srt_iMaxBounceShadowsDirectionalLights;", &_srtGlobals.srt_iMaxBounceShadowsDirectionalLights);
  _pShell->DeclareSymbol("persistent user INDEX srt_iMaxBounceShadowsSphereLights;", &_srtGlobals.srt_iMaxBounceShadowsSphereLights);
  _pShell->DeclareSymbol("persistent user INDEX srt_iMaxBounceShadowsSpotlights;", &_srtGlobals.srt_iMaxBounceShadowsSpotlights);

  _pShell->DeclareSymbol("persistent user INDEX srt_bModelChangeableTranslucentToAlphaTested;", &_srtGlobals.srt_bModelChangeableTranslucentToAlphaTested);
  _pShell->DeclareSymbol("persistent user INDEX srt_iCullingMaxSectorDepth;", &_srtGlobals.srt_iCullingMaxSectorDepth);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fCullingMinAngularSize;", &_srtGlobals.srt_fCullingMinAngularSize);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fCullingThinSectorSize;", &_srtGlobals.srt_fCullingThinSectorSize);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fModelLODMul;", &_srtGlobals.srt_fModelLODMul);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fModelLODAdd;", &_srtGlobals.srt_fModelLODAdd);

  _pShell->DeclareSymbol("persistent user INDEX srt_bWeaponUseOriginalNormals;", &_srtGlobals.srt_bWeaponUseOriginalNormals);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fParticlesAlphaMultiplier;", &_srtGlobals.srt_fParticlesAlphaMultiplier);

  _pShell->DeclareSymbol("           user FLOAT srt_vAnimatedSunTargetEuler[3];", &_srtGlobals.srt_vAnimatedSunTargetEuler);
  _pShell->DeclareSymbol("           user INDEX srt_bAnimatedSunRestart;", &_srtGlobals.srt_bAnimatedSunRestart);
  _pShell->DeclareSymbol("           user FLOAT srt_fAnimatedSunTimeOffsetStart;", &_srtGlobals.srt_fAnimatedSunTimeOffsetStart);
  _pShell->DeclareSymbol("           user FLOAT srt_fAnimatedSunTimeLength;", &_srtGlobals.srt_fAnimatedSunTimeLength);

  _pShell->DeclareSymbol("persistent user FLOAT srt_fBloomIntensity;", &_srtGlobals.srt_fBloomIntensity);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fBloomInputThreshold;", &_srtGlobals.srt_fBloomInputThreshold);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fBloomInputThresholdLength;", &_srtGlobals.srt_fBloomInputThresholdLength);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fBloomUpsampleRadius;", &_srtGlobals.srt_fBloomUpsampleRadius);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fBloomEmissionMultiplier;", &_srtGlobals.srt_fBloomEmissionMultiplier);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fBloomSkyMultiplier;", &_srtGlobals.srt_fBloomSkyMultiplier);

  // user controls
  _pShell->DeclareSymbol("user INDEX ctl_bFlashlight;", &_srtGlobals.srt_bFlashlightEnable);
}

void SSRT::SSRTMain::NormalizeShellVariables()
{
  _srtGlobals.srt_bVSync = !!_srtGlobals.srt_bVSync;
  _srtGlobals.srt_bTonemappingUseDefault = !!_srtGlobals.srt_bTonemappingUseDefault;
  _srtGlobals.srt_bShowGradients = !!_srtGlobals.srt_bShowGradients;
  _srtGlobals.srt_bShowMotionVectors = !!_srtGlobals.srt_bShowMotionVectors;
  _srtGlobals.srt_bReloadShaders = !!_srtGlobals.srt_bReloadShaders;
  _srtGlobals.srt_bTexturesOriginalSRGB = !!_srtGlobals.srt_bTexturesOriginalSRGB;
  _srtGlobals.srt_bIgnoreDynamicTexCoords = !!_srtGlobals.srt_bIgnoreDynamicTexCoords;

  _srtGlobals.srt_fModelSpecularMetallicDefault = Clamp(_srtGlobals.srt_fModelSpecularMetallicDefault, 0.0f, 1.0f);
  _srtGlobals.srt_fModelSpecularRoughnessDefault = Clamp(_srtGlobals.srt_fModelSpecularRoughnessDefault, 0.0f, 1.0f);
  _srtGlobals.srt_fModelReflectiveMetallicDefault = Clamp(_srtGlobals.srt_fModelReflectiveMetallicDefault, 0.0f, 1.0f);
  _srtGlobals.srt_fModelReflectiveRoughnessDefault = Clamp(_srtGlobals.srt_fModelReflectiveRoughnessDefault, 0.0f, 1.0f);

  _srtGlobals.srt_bEnableViewerShadows = !!_srtGlobals.srt_bEnableViewerShadows;

  _srtGlobals.srt_fSunIntensity = Max(_srtGlobals.srt_fSunIntensity, 0.0f);
  _srtGlobals.srt_fSunSaturation = Max(_srtGlobals.srt_fSunSaturation, 0.0f);
  _srtGlobals.srt_fSunColorPow = Max(_srtGlobals.srt_fSunColorPow, 0.0f);
  _srtGlobals.srt_fSunAngularDiameter = Max(_srtGlobals.srt_fSunAngularDiameter, 0.0f);

  _srtGlobals.srt_fOriginalLightSphIntensity = Max(_srtGlobals.srt_fOriginalLightSphIntensity, 0.0f);
  _srtGlobals.srt_fOriginalLightSphRadiusMultiplier = Max(_srtGlobals.srt_fOriginalLightSphRadiusMultiplier, 0.0f);
  _srtGlobals.srt_fOriginalLightSphFalloffMultiplier = Max(_srtGlobals.srt_fOriginalLightSphFalloffMultiplier, 0.0f);

  _srtGlobals.srt_fDynamicLightSphIntensity = Max(_srtGlobals.srt_fDynamicLightSphIntensity, 0.0f);
  _srtGlobals.srt_fDynamicLightSphRadius = Max(_srtGlobals.srt_fDynamicLightSphRadius, 0.0f);
  _srtGlobals.srt_fDynamicLightSphFalloffMultiplier = Max(_srtGlobals.srt_fDynamicLightSphFalloffMultiplier, 0.0f);
  _srtGlobals.srt_fDynamicLightSphFalloffMin = Max(_srtGlobals.srt_fDynamicLightSphFalloffMin, 0.0f);
  _srtGlobals.srt_fDynamicLightSphFalloffMax = Max(_srtGlobals.srt_fDynamicLightSphFalloffMax, 0.0f);

  _srtGlobals.srt_fPotentialLightSphIntensity = Max(_srtGlobals.srt_fPotentialLightSphIntensity, 0.0f);
  _srtGlobals.srt_fPotentialLightSphRadiusMultiplier = Max(_srtGlobals.srt_fPotentialLightSphRadiusMultiplier, 0.0f);
  _srtGlobals.srt_fPotentialLightSphFalloffMultiplier = Max(_srtGlobals.srt_fPotentialLightSphFalloffMultiplier, 0.0f);

  _srtGlobals.srt_fMuzzleLightIntensity = Max(_srtGlobals.srt_fMuzzleLightIntensity, 0.0f);
  _srtGlobals.srt_fMuzzleLightRadius = Max(_srtGlobals.srt_fMuzzleLightRadius, 0.0f);
  _srtGlobals.srt_fMuzzleLightFalloffMultiplier = Max(_srtGlobals.srt_fMuzzleLightFalloffMultiplier, 0.0f);
  _srtGlobals.srt_fMuzzleLightOffset = Max(_srtGlobals.srt_fMuzzleLightOffset, 0.0f);

  _srtGlobals.srt_bLightSphIgnoreEditorModels = !!_srtGlobals.srt_bLightSphIgnoreEditorModels;
  _srtGlobals.srt_bWeaponUseOriginalNormals = !!_srtGlobals.srt_bWeaponUseOriginalNormals;

  _srtGlobals.srt_fParticlesAlphaMultiplier = Max(_srtGlobals.srt_fParticlesAlphaMultiplier, 0.0f);

  _srtGlobals.srt_bMaxBounceShadowsUseDefault = !!_srtGlobals.srt_bMaxBounceShadowsUseDefault;
  _srtGlobals.srt_iMaxBounceShadowsDirectionalLights = Max(_srtGlobals.srt_iMaxBounceShadowsDirectionalLights, (INDEX)0);
  _srtGlobals.srt_iMaxBounceShadowsSphereLights      = Max(_srtGlobals.srt_iMaxBounceShadowsSphereLights,      (INDEX)0);
  _srtGlobals.srt_iMaxBounceShadowsSpotlights        = Max(_srtGlobals.srt_iMaxBounceShadowsSpotlights,        (INDEX)0);

  _srtGlobals.srt_bModelChangeableTranslucentToAlphaTested = !!_srtGlobals.srt_bModelChangeableTranslucentToAlphaTested;

  _srtGlobals.srt_fCullingMinAngularSize = Clamp(_srtGlobals.srt_fCullingMinAngularSize, -1.0f, 89.9f);

  _srtGlobals.srt_bAnimatedSunRestart = !!_srtGlobals.srt_bAnimatedSunRestart;

  _srtGlobals.srt_fEmissionMapBoost = Max(_srtGlobals.srt_fEmissionMapBoost, 0.0f);
  _srtGlobals.srt_fEmissionMaxScreenColor = Max(_srtGlobals.srt_fEmissionMaxScreenColor, 0.0f);
  _srtGlobals.srt_fEmissionForFullbright = Max(_srtGlobals.srt_fEmissionForFullbright, 0.0f);
}


SSRT::SSRTMain::SSRTMain() :
  worldRenderInfo({}),
  currentScene(nullptr),
  wasWorldProcessed(false),
  currentFirstPersonModelCount(0),
  isFrameStarted(false),
  curWindowWidth(0),
  curWindowHeight(0),
  instance(RG_NULL_HANDLE)
{
  extern CTFileName _fnmApplicationPath;
  const CTFileName overridenTexturesPath = _fnmApplicationPath + "OverridenTextures\\Compressed\\";
  const CTFileName blueNoiseFilePath = _fnmApplicationPath + "OverridenTextures\\BlueNoise_LDR_RGBA_128.ktx2";
  const CTFileName shadersPath = _fnmApplicationPath + "Sources\\RTGL1\\Build\\";

  extern HWND _hwndMain;

  RgWin32SurfaceCreateInfo win32SurfaceInfo = {};
  win32SurfaceInfo.hinstance = GetModuleHandle(NULL);
  win32SurfaceInfo.hwnd = _hwndMain;

  RgInstanceCreateInfo info = {};
  info.pName = "Serious Engine RT";
  info.enableValidationLayer = _srtGlobals.srt_bDebugValidation;
  info.pShaderFolderPath = shadersPath;
  info.pBlueNoiseFilePath = blueNoiseFilePath;

  // all textre layers
  info.primaryRaysMaxAlbedoLayers = 3;
  // without detail textures
  info.indirectIlluminationMaxAlbedoLayers = 2;

  info.vertexPositionStride = sizeof(GFXVertex);
  info.vertexNormalStride = sizeof(GFXNormal);
  info.vertexTexCoordStride = sizeof(GFXTexCoord);
  info.vertexColorStride = sizeof(uint32_t);

  info.rasterizedMaxVertexCount = 1 << 16;
  info.rasterizedMaxIndexCount = info.rasterizedMaxVertexCount * 3 / 2;
  info.rasterizedSkyMaxVertexCount = 4096;
  info.rasterizedSkyMaxIndexCount = 4096;
  info.rasterizedSkyCubemapSize = 256;
  info.rasterizedVertexColorGamma = RG_TRUE;

  info.pOverridenTexturesFolderPath = overridenTexturesPath;
  info.pOverridenAlbedoAlphaTexturePostfix = "";
  info.pOverridenRoughnessMetallicEmissionTexturePostfix = "_rme";
  info.pOverridenNormalTexturePostfix = "_n";

  info.overridenAlbedoAlphaTextureIsSRGB = RG_TRUE;
  info.overridenRoughnessMetallicEmissionTextureIsSRGB = RG_FALSE;
  info.overridenNormalTextureIsSRGB = RG_FALSE;

  info.pWin32SurfaceInfo = &win32SurfaceInfo;

  info.pfnPrint = [] (const char *pMessage, void *pUserData)
  {
    CPrintF(pMessage);
  };

  RgResult r = rgCreateInstance(&info, &instance);
  RG_CHECKERROR(r);

  textureUploader = new TextureUploader(instance);
}
SSRT::SSRTMain::~SSRTMain()
{
  if (currentScene != nullptr)
  {
    delete currentScene;
    currentScene = nullptr;
  }

  worldRenderInfo = {};
  currentFirstPersonModelCount = 0;

  curWindowWidth = 0;
  curWindowHeight = 0;

  delete textureUploader;

  if (instance != RG_NULL_HANDLE)
  {
    RgResult r = rgDestroyInstance(instance);
    RG_CHECKERROR(r);

    instance = RG_NULL_HANDLE;
  }
}

void SSRT::SSRTMain::StartFrame(CViewPort *pvp)
{
  NormalizeShellVariables();

  if (isFrameStarted)
  {
    // ASSERTALWAYS("Frame was already started.");
    return;
  }

  RgStartFrameInfo startInfo = {};
  startInfo.surfaceSize = { curWindowWidth, curWindowHeight };
  startInfo.requestShaderReload = _srtGlobals.srt_bReloadShaders;
  startInfo.requestVSync = _srtGlobals.srt_bVSync;
  startInfo.requestRasterizedSkyGeometryReuse = false;

  RgResult r = rgStartFrame(instance, &startInfo);
  RG_CHECKERROR(r);
  _srtGlobals.srt_bReloadShaders = 0;

  isFrameStarted = true;

  wasWorldProcessed = false;
  currentFirstPersonModelCount = 0;

  curWindowWidth = pvp->vp_Raster.ra_Width;
  curWindowHeight = pvp->vp_Raster.ra_Height;

  _srtGlobals.srt_fRenderSize = { (float)curWindowWidth, (float)curWindowHeight };
}

void SSRT::SSRTMain::ProcessWorld(const CWorldRenderingInfo &info)
{
  if (!isFrameStarted || info.pWorld == nullptr)
  {
    return;  
  }

  // don't process more than once
  if (wasWorldProcessed)
  {
    return;
  }
  wasWorldProcessed = true;
  worldRenderInfo = info;

  // if a new world was requested, recreate the scene
  if (currentScene == nullptr || info.pWorld->GetName() != currentScene->GetWorldName())
  {
    delete currentScene;
    currentScene = new Scene(instance, info.pWorld, textureUploader);
  }

  // update models and movable brushes in scene
  currentScene->Update(worldRenderInfo);
}

void SSRT::SSRTMain::ProcessFirstPersonModel(const CFirstPersonModelInfo &info)
{
  if (currentScene != nullptr && wasWorldProcessed)
  {
    currentScene->ProcessFirstPersonModel(info, SSRT_FIRSTPERSON_ENTITY_START_ID + currentFirstPersonModelCount);
    currentFirstPersonModelCount++;
  }
}

void SSRT::SSRTMain::ProcessHudElement(const CHudElementInfo &hud)
{
  if (!isFrameStarted)
  {
    return;
  }

  // override texture wrapping
  if (hud.textureData != nullptr)
  {
    hud.textureData->td_tpLocal.tp_eWrapU = hud.textureWrapU;
    hud.textureData->td_tpLocal.tp_eWrapV = hud.textureWrapV;
  }

  RgRasterizedGeometryVertexArrays vertData = {};
  vertData.pVertexData = hud.pPositions;
  vertData.pTexCoordData = hud.pTexCoords;
  vertData.pColorData = hud.pColors;
  vertData.vertexStride = sizeof(GFXVertex4);
  vertData.texCoordStride = sizeof(GFXTexCoord);
  vertData.colorStride = sizeof(GFXColor);

  RgRasterizedGeometryUploadInfo hudInfo = {};
  hudInfo.vertexCount = hud.vertexCount;
  hudInfo.pArrays = &vertData;
  hudInfo.indexCount = hud.indexCount;
  hudInfo.pIndexData = hud.pIndices;
  hudInfo.material = textureUploader->GetMaterial(hud.textureData);
  hudInfo.color = { 1, 1, 1, 1 };
  hudInfo.blendEnable = RG_TRUE;
  hudInfo.blendFuncSrc = RG_BLEND_FACTOR_SRC_ALPHA;
  hudInfo.blendFuncDst = RG_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
  hudInfo.depthTest = RG_FALSE;
  hudInfo.depthWrite = RG_FALSE;
  hudInfo.renderType = RG_RASTERIZED_GEOMETRY_RENDER_TYPE_SWAPCHAIN;
  hudInfo.transform = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0
  };

  float hudViewProj[16];
  extern void Svk_MatMultiply(float *result, const float *a, const float *b);
  Svk_MatMultiply(hudViewProj, viewMatrix, projMatrix);

  RgResult r = rgUploadRasterizedGeometry(instance, &hudInfo, hudViewProj, &currentViewport);
  RG_CHECKERROR(r);
}

void SSRT::SSRTMain::EndFrame()
{
  if (!isFrameStarted)
  {
    // ASSERTALWAYS("Frame wasn't started.");
    return;
  }


  // check shell variable, if it's null, then the game was stopped
  if (Utils::GetCurrentWorld() == nullptr)
  {
    delete currentScene;
    currentScene = nullptr;
  }


  if (currentScene != nullptr)
  {
    currentScene->OnFrameEnd(currentFirstPersonModelCount > 0);
  }


  RgDrawFrameShadowParams shadowParams = {};
  shadowParams.maxBounceShadowsDirectionalLights = _srtGlobals.srt_iMaxBounceShadowsDirectionalLights;
  shadowParams.maxBounceShadowsSphereLights = _srtGlobals.srt_iMaxBounceShadowsSphereLights;
  shadowParams.maxBounceShadowsSpotlights = _srtGlobals.srt_iMaxBounceShadowsSpotlights;


  RgDrawFrameSkyParams skyParams = {};
  {
    skyParams.skyType = currentScene == nullptr ? RG_SKY_TYPE_COLOR : currentScene->GetCustomInfo()->GetLevelSkyType();

    FLOAT3D backgroundViewerPos = currentScene == nullptr ? FLOAT3D(0, 0, 0) : currentScene->GetBackgroundViewerPosition();

    skyParams.skyViewerPosition = { backgroundViewerPos(1), backgroundViewerPos(2), backgroundViewerPos(3) };
    skyParams.skyColorDefault = { 0, 0, 0 };
    skyParams.skyColorMultiplier = currentScene == nullptr ? 1.0f : currentScene->GetCustomInfo()->GetSkyIntensity();
    skyParams.skyColorSaturation = currentScene == nullptr ? 1.0f : currentScene->GetCustomInfo()->GetSkySaturation();
  }


  RgDrawFrameTonemappingParams tmParams = {};
  tmParams.luminanceWhitePoint = _srtGlobals.srt_fTonemappingWhitePoint;
  tmParams.minLogLuminance = _srtGlobals.srt_fTonemappingMinLogLuminance;
  tmParams.maxLogLuminance = _srtGlobals.srt_fTonemappingMaxLogLuminance;


  RgDrawFrameBloomParams blParams = {};
  blParams.bloomIntensity = _srtGlobals.srt_fBloomIntensity;
  blParams.inputThreshold = _srtGlobals.srt_fBloomInputThreshold;
  blParams.inputThresholdLength = _srtGlobals.srt_fBloomInputThresholdLength;
  blParams.upsampleRadius = _srtGlobals.srt_fBloomUpsampleRadius;
  blParams.bloomEmissionMultiplier = _srtGlobals.srt_fBloomEmissionMultiplier;
  blParams.bloomSkyMultiplier = _srtGlobals.srt_fBloomSkyMultiplier;


  RgDrawFrameOverridenTexturesParams tdParams = {};
  tdParams.normalMapStrength = _srtGlobals.srt_fNormalMapStrength;
  tdParams.emissionMapBoost = _srtGlobals.srt_fEmissionMapBoost;
  tdParams.emissionMaxScreenColor = _srtGlobals.srt_fEmissionMaxScreenColor;


  RgDrawFrameDebugParams dbgParams = {};
  dbgParams.showMotionVectors = !!_srtGlobals.srt_bShowMotionVectors;
  dbgParams.showGradients = !!_srtGlobals.srt_bShowGradients;


  RgDrawFrameInfo frameInfo = {};
  frameInfo.renderSize = { curWindowWidth, curWindowHeight };
  frameInfo.rayCullMaskWorld = currentScene == nullptr ? 0xFF : currentScene->GetCustomInfo()->GetCullMask(currentScene->GetCameraPosition());
  frameInfo.rayLength = currentScene == nullptr ? 10000.0f : currentScene->GetCustomInfo()->GetRayLength(currentScene->GetCameraPosition());

  frameInfo.currentTime = _pTimer->GetLerpedCurrentTick();
  // realTime = _pTimer->GetHighPrecisionTimer().GetSeconds();

  // if world wasn't rendered, don't adapt
  frameInfo.disableEyeAdaptation = !wasWorldProcessed;
  frameInfo.disableRayTracing = !wasWorldProcessed;
  frameInfo.pTonemappingParams = _srtGlobals.srt_bTonemappingUseDefault ? nullptr : &tmParams;
  frameInfo.pShadowParams = _srtGlobals.srt_bMaxBounceShadowsUseDefault ? nullptr : &shadowParams;
  frameInfo.pBloomParams = &blParams;
  frameInfo.pSkyParams = &skyParams;
  frameInfo.pOverridenTexturesParams = &tdParams;
  frameInfo.pDebugParams = &dbgParams;
  
  memcpy(frameInfo.view,        worldRenderInfo.viewMatrix,       16 * sizeof(float));
  memcpy(frameInfo.projection,  worldRenderInfo.projectionMatrix, 16 * sizeof(float));

  RgResult r = rgDrawFrame(instance, &frameInfo);
  RG_CHECKERROR(r);


  wasWorldProcessed = false;
  isFrameStarted = false;
}

void SSRT::SSRTMain::SetProjectionMatrix(const float *pMatrix)
{
  memcpy(projMatrix, pMatrix, 16 * sizeof(float));
}

void SSRT::SSRTMain::SetViewMatrix(const float *pMatrix)
{
  memcpy(viewMatrix, pMatrix, 16 * sizeof(float));
}

void SSRT::SSRTMain::SetViewport(float leftUpperX, float leftUpperY, float width, float height, float minDepth, float maxDepth)
{
  currentViewport.x = leftUpperX;
  currentViewport.y = leftUpperY;
  currentViewport.width = width;
  currentViewport.height = height;
  currentViewport.minDepth = 0.0f;
  currentViewport.maxDepth = 1.0f;
}

void SSRT::SSRTMain::DeleteTexture(uint32_t textureID)
{
  textureUploader->DeleteTexture(textureID);
}
