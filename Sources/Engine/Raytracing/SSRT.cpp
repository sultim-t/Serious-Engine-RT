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
  info.rasterizedMaxIndexCount = 8192;
  info.rasterizedMaxVertexCount = 4096;

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

  if (instance != RG_NULL_HANDLE)
  {
    RgResult r = rgDestroyInstance(instance);
    RG_CHECKERROR(r);

    instance = RG_NULL_HANDLE;
  }
}

void SSRT::SSRTMain::StartFrame(CViewPort *pvp)
{
  curWindowWidth = pvp->vp_Raster.ra_Width;
  curWindowHeight = pvp->vp_Raster.ra_Height;
}

void SSRT::SSRTMain::ProcessWorld(const CWorldRenderingInfo &info)
{
#pragma region START FRAME
  // TODO: MOVE OUT
  {
    currentFirstPersonModelCount = 0;

    RgResult r = rgStartFrame(instance, curWindowWidth, curWindowHeight, true);
    RG_CHECKERROR(r);

    isFrameStarted = true;
  }
#pragma endregion



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
    currentScene = new Scene(instance, info.world);
  }

  worldRenderInfo = info;

  // update models and movable brushes in scene
  currentScene->Update(info.viewerEntityID);



#pragma region END FRAME
  // TODO: DELETE
  {
    EndFrame();
  }
#pragma endregion 
}

void SSRT::SSRTMain::ProcessFirstPersonModel(const CFirstPersonModelInfo &info)
{
  //RT_AddFirstPersonModel(info.modelObject, info.renderModel, SSRT_FIRSTPERSON_ENTITY_START_ID + currentFirstPersonModelCount, this);
  currentFirstPersonModelCount++;
}

void SSRT::SSRTMain::ProcessHudElement(const CHudElementInfo &hud)
{
  if (!isFrameStarted)
  {
    return;
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
  rasterInfo.textures = { RG_NO_MATERIAL,RG_NO_MATERIAL, RG_NO_MATERIAL };

  RgResult r = rgUploadRasterizedGeometry(instance, &rasterInfo);
  RG_CHECKERROR(r);
}

void SSRT::SSRTMain::EndFrame()
{
  if (!isFrameStarted)
  {
    return;
  }

  // if world processing wasn't done, then there is no world
  if (!wasWorldProcessed)
  {
    delete currentScene;
    currentScene = nullptr;
  }

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
