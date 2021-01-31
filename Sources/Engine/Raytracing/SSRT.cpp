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

#include <Engine/Templates/DynamicContainer.cpp>

#include <Engine/Raytracing/SSRTObjects.h>

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>

#ifndef NDEBUG
constexpr const char *OverridenTexturesPath = "../../OverridenTextures/";
#else
constexpr const char *OverridenTexturesPath = "../OverridenTextures/";
#endif


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
  RgInstanceCreateInfo info = {};
  info.name = "Serious Engine RT";
  info.physicalDeviceIndex = 0;
  info.enableValidationLayer = RG_TRUE;

  info.vertexPositionStride = sizeof(GFXVertex);
  info.vertexNormalStride = sizeof(GFXNormal);
  info.vertexTexCoordStride = sizeof(GFXTexCoord);
  info.vertexColorStride = sizeof(uint32_t);
  info.rasterizedMaxVertexCount = 1 << 16;
  info.rasterizedMaxIndexCount = info.rasterizedMaxVertexCount * 3 / 2;

  info.overridenTexturesFolderPath = OverridenTexturesPath;
  info.overrideAlbedoAlphaTexturePostfix = "";
  info.overrideNormalMetallicTexturePostfix = "_n";
  info.overrideEmissionRoughnessTexturePostfix = "_e";

  const char *pWindowExtensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME
  };

  info.ppWindowExtensions = pWindowExtensions;
  info.windowExtensionCount = sizeof(pWindowExtensions) / sizeof(const char *);

  info.pfnCreateSurface = [] (uint64_t vkInstance, uint64_t *pResultVkSurfaceKHR)
  {
    VkInstance instance = reinterpret_cast<VkInstance>(vkInstance);

    VkSurfaceKHR surface;

    HINSTANCE hInstance = GetModuleHandle(NULL);
    extern HWND _hwndMain;

    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.pNext = nullptr;
    createInfo.hinstance = hInstance;
    createInfo.hwnd = _hwndMain;
    VkResult r = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface);
    ASSERT(r == VK_SUCCESS);

    *pResultVkSurfaceKHR = reinterpret_cast<uint64_t>(surface);
  };

  info.pfnDebugPrint = [] (const char *msg)
  {
    CPrintF(msg);
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
  if (isFrameStarted)
  {
    // ASSERTALWAYS("Frame was already started.");
    return;
  }

  RgResult r = rgStartFrame(instance, curWindowWidth, curWindowHeight, true);
  RG_CHECKERROR(r);

  isFrameStarted = true;

  wasWorldProcessed = false;
  currentFirstPersonModelCount = 0;

  curWindowWidth = pvp->vp_Raster.ra_Width;
  curWindowHeight = pvp->vp_Raster.ra_Height;
}

void SSRT::SSRTMain::ProcessWorld(const CWorldRenderingInfo &info)
{
  if (!isFrameStarted)
  {
    // ASSERTALWAYS("Frame must be started to process world.");
    return;
  }

  wasWorldProcessed = true;

  // if no world
  if (info.world == nullptr)
  {
    // delete current scene
    delete currentScene;
    currentScene = nullptr;

    return;
  }

  // if a new world was requested, recreate the scene
  if (currentScene == nullptr || info.world->GetName() != currentScene->GetWorldName())
  {
    delete currentScene;
    currentScene = new Scene(instance, info.world, textureUploader);
  }

  worldRenderInfo = info;

  // update models and movable brushes in scene
  currentScene->Update(info.viewerPosition, info.viewerRotation, info.viewerEntityID);
}

void SSRT::SSRTMain::ProcessFirstPersonModel(const CFirstPersonModelInfo &info)
{
  if (currentScene != nullptr && wasWorldProcessed)
  {
    currentScene->AddFirstPersonModel(info, SSRT_FIRSTPERSON_ENTITY_START_ID + currentFirstPersonModelCount);
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

  RgRasterizedGeometryUploadInfo rasterInfo = {};
  rasterInfo.vertexCount = hud.vertexCount;
  rasterInfo.vertexData = (float*)hud.pPositions;
  rasterInfo.vertexStride = sizeof(GFXVertex4);
  rasterInfo.texCoordData = (float*)hud.pTexCoords;
  rasterInfo.texCoordStride = sizeof(GFXTexCoord);
  rasterInfo.colorData = (uint32_t*)hud.pColors;
  rasterInfo.colorStride = sizeof(GFXColor);
  rasterInfo.indexCount = hud.indexCount;
  rasterInfo.indexData = (uint32_t*)hud.pIndices;
  rasterInfo.viewport = currentViewport;
  rasterInfo.textures = 
  {
    textureUploader->GetMaterial(hud.textureData),
    RG_NO_MATERIAL,
    RG_NO_MATERIAL
  };

  extern void Svk_MatMultiply(float *result, const float *a, const float *b);
  Svk_MatMultiply(rasterInfo.viewProjection, viewMatrix, projMatrix);

  RgResult r = rgUploadRasterizedGeometry(instance, &rasterInfo);
  RG_CHECKERROR(r);
}

void SSRT::SSRTMain::EndFrame()
{
  if (!isFrameStarted)
  {
    // ASSERTALWAYS("Frame wasn't started.");
    return;
  }

  // if world processing wasn't done, then there is no world
  /*if (!wasWorldProcessed)
  {
    delete currentScene;
    currentScene = nullptr;
  }*/

  wasWorldProcessed = false;

  RgDrawFrameInfo frameInfo = {};
  frameInfo.renderWidth = curWindowWidth;
  frameInfo.renderHeight = curWindowHeight;

  memcpy(frameInfo.view,        worldRenderInfo.viewMatrix,       16 * sizeof(float));
  memcpy(frameInfo.projection,  worldRenderInfo.projectionMatrix, 16 * sizeof(float));

  RgResult r = rgDrawFrame(instance, &frameInfo);
  RG_CHECKERROR(r);

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
}

void SSRT::SSRTMain::DeleteTexture(uint32_t textureID)
{
  textureUploader->DeleteTexture(textureID);
}
