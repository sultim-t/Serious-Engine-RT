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
  _pShell->DeclareSymbol("persistent user INDEX srt_bVSync;", &_srtGlobals.srt_bVSync);
  _pShell->DeclareSymbol("persistent user INDEX srt_bTonemappingUseDefault;", &_srtGlobals.srt_bTonemappingUseDefault);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fTonemappingWhitePoint;", &_srtGlobals.srt_fTonemappingWhitePoint);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fTonemappingMinLogLuminance;", &_srtGlobals.srt_fTonemappingMinLogLuminance);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fTonemappingMaxLogLuminance;", &_srtGlobals.srt_fTonemappingMaxLogLuminance);
  _pShell->DeclareSymbol("persistent user INDEX srt_iSkyType;", &_srtGlobals.srt_iSkyType);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fSkyColorDefault[3];", &_srtGlobals.srt_fSkyColorDefault);
  _pShell->DeclareSymbol("persistent user FLOAT srt_fSkyColorMultiplier;", &_srtGlobals.srt_fSkyColorMultiplier);
  _pShell->DeclareSymbol("persistent user INDEX srt_bShowGradients;", &_srtGlobals.srt_bShowGradients);
  _pShell->DeclareSymbol("persistent user INDEX srt_bShowMotionVectors;", &_srtGlobals.srt_bShowMotionVectors);
  _pShell->DeclareSymbol("persistent user INDEX srt_bReloadShaders;", &_srtGlobals.srt_bReloadShaders);
  _pShell->DeclareSymbol("persistent user INDEX srt_bTexturesOriginalSRGB;", &_srtGlobals.srt_bTexturesOriginalSRGB);
  _pShell->DeclareSymbol("persistent user INDEX srt_bIgnoreDynamicTexCoords;", &_srtGlobals.srt_bIgnoreDynamicTexCoords);
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

  switch (_srtGlobals.srt_iSkyType)
  {
  case RG_SKY_TYPE_COLOR:
  case RG_SKY_TYPE_CUBEMAP: 
  case RG_SKY_TYPE_RASTERIZED_GEOMETRY: 
  case RG_SKY_TYPE_RAY_TRACED_GEOMETRY:
    break;
  default:  
    _srtGlobals.srt_iSkyType = RG_SKY_TYPE_COLOR;
  }

  for (uint32_t i = 1; i <= 3; i++)
  {
    _srtGlobals.srt_fSkyColorDefault(i) = ClampDn(_srtGlobals.srt_fSkyColorDefault(i), 0.0f);
  }
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
  info.enableValidationLayer = RG_TRUE;
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

  SSRTMain::InitShellVariables();
  Scene::InitShellVariables();
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

  RgDrawFrameInfo frameInfo = {};
  frameInfo.renderSize = { curWindowWidth, curWindowHeight };

  frameInfo.currentTime = _pTimer->GetLerpedCurrentTick();
  // realTime = _pTimer->GetHighPrecisionTimer().GetSeconds();

  // if world wasn't rendered, don't adapt
  frameInfo.disableEyeAdaptation = !wasWorldProcessed;

  frameInfo.overrideTonemappingParams = !_srtGlobals.srt_bTonemappingUseDefault;
  frameInfo.luminanceWhitePoint = _srtGlobals.srt_fTonemappingWhitePoint;
  frameInfo.minLogLuminance = _srtGlobals.srt_fTonemappingMinLogLuminance;
  frameInfo.maxLogLuminance = _srtGlobals.srt_fTonemappingMaxLogLuminance;
  
  frameInfo.skyType = _srtGlobals.srt_iSkyType == 3 ? RG_SKY_TYPE_RAY_TRACED_GEOMETRY : 
                      _srtGlobals.srt_iSkyType == 2 ? RG_SKY_TYPE_RASTERIZED_GEOMETRY :
                      _srtGlobals.srt_iSkyType == 1 ? RG_SKY_TYPE_CUBEMAP :
                                          RG_SKY_TYPE_COLOR;

  FLOAT3D backgroundViewerPos = currentScene == nullptr ? FLOAT3D(0, 0, 0) : currentScene->GetBackgroundViewerPosition();

  frameInfo.skyViewerPosition = { backgroundViewerPos(1), backgroundViewerPos(2), backgroundViewerPos(3) };
  frameInfo.skyColorDefault = { _srtGlobals.srt_fSkyColorDefault(1), _srtGlobals.srt_fSkyColorDefault(2), _srtGlobals.srt_fSkyColorDefault(3) };
  frameInfo.skyColorMultiplier = _srtGlobals.srt_fSkyColorMultiplier;

  memcpy(frameInfo.view,        worldRenderInfo.viewMatrix,       16 * sizeof(float));
  memcpy(frameInfo.projection,  worldRenderInfo.projectionMatrix, 16 * sizeof(float));

  frameInfo.dbgShowMotionVectors = !!_srtGlobals.srt_bShowMotionVectors;
  frameInfo.dbgShowGradients = !!_srtGlobals.srt_bShowGradients;

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
