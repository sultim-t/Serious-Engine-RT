/* Copyright (c) 2020 Sultim Tsyrendashiev
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

#include "stdh.h"

#include <Engine/Base/Translation.h>
#include <Engine/Base/ErrorReporting.h>
#include <Engine/Base/Memory.h>
#include <Engine/Base/Console.h>

#include <Engine/Graphics/GfxLibrary.h>
#include <Engine/Graphics/Vulkan/SvkMain.h>
#include <Engine/Graphics/ViewPort.h>

#include <Engine/Templates/StaticStackArray.cpp>
#include <Engine/Templates/DynamicContainer.cpp>
#include <Engine/Templates/Stock_CTextureData.h>

FLOAT	VkProjectionMatrix[16];
FLOAT	VkViewMatrix[16];


#ifdef SE1_VULKAN

// fog/haze textures
extern ULONG _fog_ulTexture;
extern ULONG _haze_ulTexture;
static uint32_t _no_ulTexture;
static uint64_t _no_ulTextureDescSet;

extern BOOL GFX_abTexture[GFX_MAXTEXUNITS];

extern BOOL GFX_bDepthTest;
extern BOOL GFX_bDepthWrite;
extern BOOL GFX_bAlphaTest;
extern BOOL GFX_bBlending;
extern BOOL GFX_bDithering;
extern BOOL GFX_bClipping;
extern BOOL GFX_bClipPlane;
extern BOOL GFX_bColorArray;
extern BOOL GFX_bFrontFace;
extern BOOL GFX_bTruform;
extern INDEX GFX_iActiveTexUnit;
extern FLOAT GFX_fMinDepthRange;
extern FLOAT GFX_fMaxDepthRange;
extern GfxBlend GFX_eBlendSrc;
extern GfxBlend GFX_eBlendDst;
extern GfxComp  GFX_eDepthFunc;
extern GfxFace  GFX_eCullFace;
extern INDEX GFX_iTexModulation[GFX_MAXTEXUNITS];
extern INDEX GFX_ctVertices;
extern BOOL  GFX_bViewMatrix;

#pragma region Debug messenger
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) 
{
  // print debug message to console
  CPrintF( "Vulkan Validation layer: %s\n", pCallbackData->pMessage);
  return VK_FALSE;
}

void GetDebugMsgCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &outInfo)
{
  outInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
  outInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
  outInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
  outInfo.pfnUserCallback = DebugCallback;
}
#pragma endregion

BOOL CGfxLibrary::InitDriver_Vulkan()
{
  ASSERT(gl_SvkMain == nullptr);

  gl_SvkMain = new SvkMain();
  return gl_SvkMain->InitDriver_Vulkan();
}

void CGfxLibrary::EndDriver_Vulkan(void)
{
  ASSERT(gl_SvkMain != nullptr);

  gl_SvkMain->EndDriver_Vulkan();
  delete gl_SvkMain;

  gl_SvkMain = nullptr;
}

void CGfxLibrary::Reset_Vulkan()
{
  gl_SvkMain->Reset_Vulkan();
}

void CGfxLibrary::InitContext_Vulkan() 
{
  gl_SvkMain->InitContext_Vulkan();
}

BOOL CGfxLibrary::SetCurrentViewport_Vulkan(CViewPort *pvp) 
{
  return gl_SvkMain->SetCurrentViewport_Vulkan(pvp);
}

void CGfxLibrary::SwapBuffers_Vulkan()
{
  gl_SvkMain->SwapBuffers_Vulkan();
}

void CGfxLibrary::SetViewport_Vulkan(float leftUpperX, float leftUpperY, float width, float height, float minDepth, float maxDepth)
{
  gl_SvkMain->SetViewport_Vulkan(leftUpperX, leftUpperY, width, height, minDepth, maxDepth);
}


SvkMain::SvkMain()
{
  Reset_Vulkan();
}

// initialize Vulkan driver
BOOL SvkMain::InitDriver_Vulkan()
{
  _pGfx->gl_hiDriver = NONE; // must be initialized?

  ASSERT(gl_VkInstance == VK_NULL_HANDLE);
  ASSERT(gl_VkDevice == VK_NULL_HANDLE);

  HINSTANCE hInstance = GetModuleHandle(NULL);

  // startup Vulkan
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.pApplicationName = "Serious App";
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pEngineName = "Serious Engine 1";
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.apiVersion = VK_API_VERSION_1_0;

  VkInstanceCreateInfo instanceInfo = {};
  instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  instanceInfo.pApplicationInfo = &appInfo;

  // hard coded Windows extensions
  const char* extensions[] = {
    VK_KHR_SURFACE_EXTENSION_NAME,
    VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#ifndef NDEBUG
    VK_EXT_DEBUG_UTILS_EXTENSION_NAME
#endif
  };

#ifndef NDEBUG
  VkDebugUtilsMessengerCreateInfoEXT debugMsgInfo = {};
  GetDebugMsgCreateInfo(debugMsgInfo);

  if (gl_VkLayers.Count() == 0)
  {
    gl_VkLayers.New(2);
    gl_VkLayers[0] = "VK_LAYER_KHRONOS_validation";
    gl_VkLayers[1] = "VK_LAYER_LUNARG_monitor";
  }

  instanceInfo.enabledExtensionCount = 3;
  instanceInfo.ppEnabledExtensionNames = extensions;
  instanceInfo.enabledLayerCount = (uint32_t)gl_VkLayers.Count();
  instanceInfo.ppEnabledLayerNames = &gl_VkLayers[0];
  instanceInfo.pNext = &debugMsgInfo;
#else
  instanceInfo.enabledExtensionCount = 2;
  instanceInfo.ppEnabledExtensionNames = extensions;
  instanceInfo.enabledLayerCount = 0;
#endif

  VkResult r = vkCreateInstance(&instanceInfo, nullptr, &gl_VkInstance);

  if (r == VK_ERROR_INCOMPATIBLE_DRIVER)
  {
    CPrintF("Vulkan error: Can't find a compatible Vulkan ICD.\n");
    return FALSE;
  }
  else if (r != VK_SUCCESS)
  {
    CPrintF("Vulkan error: Can't create instance.\n");
    return FALSE;
  }

#ifndef NDEBUG
  auto pfnCreateDUMsg = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(gl_VkInstance, "vkCreateDebugUtilsMessengerEXT");
  if (pfnCreateDUMsg != nullptr) 
  {
    pfnCreateDUMsg(gl_VkInstance, &debugMsgInfo, nullptr, &gl_VkDebugMessenger);
  }
  else 
  {
    CPrintF("Vulkan error: Can't find vkCreateDebugUtilsMessengerEXT.\n");
    return FALSE;
  }
#endif

  extern HWND _hwndMain;
  if (!InitSurface_Win32(hInstance, _hwndMain))
  {
    CPrintF("Vulkan error: Can't create Win32 surface.\n");
    return FALSE;
  }

  if (gl_VkPhysDeviceExtensions.Count() == 0)
  {
    gl_VkPhysDeviceExtensions.New(1);
    gl_VkPhysDeviceExtensions[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
  }

  if (!PickPhysicalDevice())
  {
    CPrintF("Vulkan error: Can't find suitable physical device.\n");
    return FALSE;
  }

  if (!CreateDevice())
  {
    CPrintF("Vulkan error: Can't create VkDevice.\n");
    return FALSE;
  }

  InitSamplers();
  CreateTexturesDataStructure();
  CreateDescriptorPools();
  CreateCmdBuffers();
  CreateSyncPrimitives();
  CreateVertexLayouts();
  CreatePipelineCache();
  CreateRenderPass();
  CreateDescriptorSetLayouts();
  CreateShaderModules();
  InitDynamicBuffers();

  return TRUE;
}

void SvkMain::EndDriver_Vulkan(void)
{
  if (gl_VkInstance == VK_NULL_HANDLE)
  {
    return;
  }

  vkDeviceWaitIdle(gl_VkDevice);

  gl_VkVerts.Clear();

  DestroyTexturesDataStructure();

  DestroySwapchain();
  DestroySyncPrimitives();

  DestroyDynamicBuffers();
  DestroyCmdBuffers();

  DestroyDescriptorSetLayouts();
  DestroyDescriptorPools();

  DestroySamplers();
  DestroyPipelines();
  DestroyVertexLayouts();
  DestroyShaderModules();

  vkDestroyPipelineCache(gl_VkDevice, gl_VkPipelineCache, nullptr);
  vkDestroyRenderPass(gl_VkDevice, gl_VkRenderPass, nullptr);
  vkDestroySurfaceKHR(gl_VkInstance, gl_VkSurface, nullptr);
  vkDestroyDevice(gl_VkDevice, nullptr);

#ifndef NDEBUG
  auto pfnDestroyDUMsg = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(gl_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
  if (pfnDestroyDUMsg != nullptr)
  {
    pfnDestroyDUMsg(gl_VkInstance, gl_VkDebugMessenger, nullptr);
  }
#endif

  vkDestroyInstance(gl_VkInstance, nullptr);

  Reset_Vulkan();
}

void SvkMain::Reset_Vulkan()
{
  gl_VkInstance = VK_NULL_HANDLE;
  gl_VkDevice = VK_NULL_HANDLE;
  gl_VkSurface = VK_NULL_HANDLE;
  gl_VkCurrentImageIndex = 0;
  gl_VkCurrentViewport = {};
  gl_VkCurrentScissor = {};
  gl_VkSwapChainExtent = {};

  gl_VkCmdPool = VK_NULL_HANDLE;;
  gl_VkRenderPass = VK_NULL_HANDLE;

  gl_VkSwapchain = VK_NULL_HANDLE;
  gl_VkSurfColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
  gl_VkSurfColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  gl_VkSurfDepthFormat = VK_FORMAT_D16_UNORM;
  gl_VkSurfPresentMode = VK_PRESENT_MODE_FIFO_KHR;
  gl_VkCmdBufferCurrent = 0;
  gl_VkCmdIsRecording = false;

  gl_VkUniformDescPool = VK_NULL_HANDLE;
  gl_VkDescSetLayoutTexture = VK_NULL_HANDLE;
  gl_VkDescriptorSetLayout = VK_NULL_HANDLE;
  gl_VkPipelineLayout = VK_NULL_HANDLE;
  gl_VkPipelineCache = VK_NULL_HANDLE;
  gl_VkDefaultVertexLayout = nullptr;
  gl_VkShaderModuleVert = VK_NULL_HANDLE;
  gl_VkShaderModuleFrag = VK_NULL_HANDLE;
  gl_VkShaderModuleFragAlpha = VK_NULL_HANDLE;
  gl_VkPreviousPipeline = nullptr;

  // reset states to default
  gl_VkGlobalState = SVK_PLS_DEFAULT_FLAGS;
  gl_VkGlobalSamplerState = 0;

  gl_VkLastTextureId = 1;
  gl_VkImageMemPool = nullptr;

  gl_VkPhysDevice = VK_NULL_HANDLE;
  gl_VkPhMemoryProperties = {};
  gl_VkPhProperties = {};
  gl_VkPhFeatures = {};
  gl_VkPhSurfCapabilities = {};

  gl_VkQueueFamGraphics = VK_NULL_HANDLE;
  gl_VkQueueFamTransfer = VK_NULL_HANDLE;
  gl_VkQueueFamPresent = VK_NULL_HANDLE;
  gl_VkQueueGraphics = VK_NULL_HANDLE;
  gl_VkQueueTransfer = VK_NULL_HANDLE;
  gl_VkQueuePresent = VK_NULL_HANDLE;

  gl_VkDebugMessenger = VK_NULL_HANDLE;

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    gl_VkTextureDescPools[i] = VK_NULL_HANDLE;
    gl_VkTextureDescSets[i] = nullptr;

    gl_VkCmdBuffers[i] = VK_NULL_HANDLE;
    gl_VkImageAvailableSemaphores[i] = VK_NULL_HANDLE;
    gl_VkRenderFinishedSemaphores[i] = VK_NULL_HANDLE;
    gl_VkCmdFences[i] = VK_NULL_HANDLE;

    gl_VkDynamicVB[i].sdb_Buffer = VK_NULL_HANDLE;
    gl_VkDynamicIB[i].sdb_Buffer = VK_NULL_HANDLE;
    gl_VkDynamicUB[i].sdb_Buffer = VK_NULL_HANDLE;

    gl_VkDynamicVB[i].sdb_CurrentOffset = 0;
    gl_VkDynamicIB[i].sdb_CurrentOffset = 0;
    gl_VkDynamicUB[i].sdb_CurrentOffset = 0;

    gl_VkDynamicVB[i].sdb_Data = nullptr;
    gl_VkDynamicIB[i].sdb_Data = nullptr;
    gl_VkDynamicUB[i].sdb_Data = nullptr;

    gl_VkDynamicUB[i].sdu_DescriptorSet = VK_NULL_HANDLE;

    // manually set memory as arrays contain garbage
    gl_VkDynamicToDelete[i].sa_Array = nullptr;
    gl_VkDynamicToDelete[i].sa_Count = 0;
    gl_VkDynamicToDelete[i].sa_UsedCount = 0;
    gl_VkDynamicToDelete[i].sa_ctAllocationStep = 256;

    gl_VkTexturesToDelete[i].sa_Array = nullptr;
    gl_VkTexturesToDelete[i].sa_Count = 0;
    gl_VkTexturesToDelete[i].sa_UsedCount = 0;
    gl_VkTexturesToDelete[i].sa_ctAllocationStep = 2048;
  }

  gl_VkDynamicVBGlobal.sdg_DynamicBufferMemory = VK_NULL_HANDLE;
  gl_VkDynamicIBGlobal.sdg_DynamicBufferMemory = VK_NULL_HANDLE;
  gl_VkDynamicUBGlobal.sdg_DynamicBufferMemory = VK_NULL_HANDLE;

  gl_VkDynamicVBGlobal.sdg_CurrentDynamicBufferSize = 0;
  gl_VkDynamicIBGlobal.sdg_CurrentDynamicBufferSize = 0;
  gl_VkDynamicUBGlobal.sdg_CurrentDynamicBufferSize = 0;

  Svk_MatSetIdentity(VkProjectionMatrix);
  Svk_MatSetIdentity(VkViewMatrix);

  GFX_iActiveTexUnit = 0;
}

// prepares Vulkan drawing context, almost everything copied from OpenGL
void SvkMain::InitContext_Vulkan()
{
  // must have context
  ASSERT(_pGfx->gl_pvpActive != NULL);
  // reset engine's internal state variables
  for (INDEX iUnit = 0; iUnit < GFX_MAXTEXUNITS; iUnit++) {
    GFX_abTexture[iUnit] = FALSE;
    GFX_iTexModulation[iUnit] = 1;
  }
  // set default texture unit and modulation mode
  GFX_iActiveTexUnit = 0;
  _pGfx->gl_ctMaxStreams = 16;
  extern FLOAT GFX_fLastL, GFX_fLastR, GFX_fLastT, GFX_fLastB, GFX_fLastN, GFX_fLastF;
  GFX_fLastL = GFX_fLastR = GFX_fLastT = GFX_fLastB = GFX_fLastN = GFX_fLastF = 0;
  GFX_bViewMatrix = TRUE;
  GFX_bTruform = FALSE;
  GFX_bClipping = TRUE;

  // reset global state for pipeline
  gl_VkGlobalState = SVK_PLS_DEFAULT_FLAGS;

  GFX_abTexture[0] = TRUE;
  GFX_bDithering = TRUE;
  GFX_bBlending = FALSE;
  GFX_bDepthTest = FALSE;
  GFX_bAlphaTest = FALSE;
  GFX_bClipPlane = FALSE;
  GFX_eCullFace = GFX_NONE;
  GFX_bFrontFace = TRUE;
  GFX_bDepthWrite = FALSE;
  GFX_eDepthFunc = GFX_LESS_EQUAL;
  GFX_eBlendSrc = GFX_eBlendDst = GFX_ONE;
  GFX_fMinDepthRange = 0.0f;
  GFX_fMaxDepthRange = 1.0f;

  // vertices array for Vulkan
  gl_VkVerts.New(SVK_VERT_START_COUNT);
  gl_VkVerts.SetAllocationStep(SVK_VERT_ALLOC_STEP);
  // always true on Vulkan
  GFX_bColorArray = TRUE;

  // report header
  CPrintF(TRANS("\n* Vulkan context created: *------------------------------------\n"));
  CDisplayAdapter &da = _pGfx->gl_gaAPI[GAT_VK].ga_adaAdapter[_pGfx->gl_iCurrentAdapter];
  CPrintF("  (%s, %s, %s)\n\n", da.da_strVendor, da.da_strRenderer, da.da_strVersion);

  _pGfx->gl_ctTextureUnits = 4;
  _pGfx->gl_ctRealTextureUnits = 4;

  _pGfx->gl_fMaxTextureLODBias = gl_VkPhProperties.limits.maxSamplerLodBias;
  _pGfx->gl_iMaxTextureAnisotropy = gl_VkPhProperties.limits.maxSamplerAnisotropy;

  _pGfx->gl_iTessellationLevel = 0;
  _pGfx->gl_iMaxTessellationLevel = 0;

  GFX_bColorArray = TRUE;

  _pGfx->gl_ulFlags |= GLF_HASACCELERATION;
  _pGfx->gl_ulFlags |= GLF_32BITTEXTURES;
  _pGfx->gl_ulFlags |= GLF_VSYNC;
  _pGfx->gl_ulFlags &= ~GLF_TEXTURECOMPRESSION;
  _pGfx->gl_ulFlags |= GLF_EXT_EDGECLAMP;

  // setup fog and haze textures
  extern PIX _fog_pixSizeH;
  extern PIX _fog_pixSizeL;
  extern PIX _haze_pixSize;
  // create texture objects for Vulkan
  _fog_ulTexture = CreateTexture();
  _haze_ulTexture = CreateTexture();
  _fog_pixSizeH = 0;
  _fog_pixSizeL = 0;
  _haze_pixSize = 0;

  uint32_t noTexturePixels[] = { 0xFFFFFFFF, 0xFFFFFFFF };
  VkExtent2D noTextureSize = { 1, 1 };
  _no_ulTexture = CreateTexture();
  InitTexture32Bit(_no_ulTexture, VK_FORMAT_R8G8B8A8_UNORM, noTexturePixels, &noTextureSize, 1, false);

  // prepare pattern texture
  extern CTexParams _tpPattern;
  extern ULONG _ulPatternTexture;
  extern ULONG _ulLastUploadedPattern;
  _ulPatternTexture = CreateTexture();
  _ulLastUploadedPattern = 0;
  _tpPattern.Clear();

  // reset texture filtering and array locking
  _tpGlobal[0].Clear();
  _tpGlobal[1].Clear();
  _tpGlobal[2].Clear();
  _tpGlobal[3].Clear();
  GFX_ctVertices = 0;
  _pGfx->gl_dwVertexShader = NONE;

  extern INDEX gap_iTextureFiltering;
  extern INDEX gap_iTextureAnisotropy;

  //extern FLOAT gap_fTextureLODBias;
  gfxSetTextureFiltering(gap_iTextureFiltering, gap_iTextureAnisotropy);
  //gfxSetTextureBiasing(gap_fTextureLODBias);

  // mark pretouching and probing
  extern BOOL _bNeedPretouch;
  _bNeedPretouch = TRUE;
  _pGfx->gl_bAllowProbing = FALSE;

  // update console system vars
  extern void UpdateGfxSysCVars(void);
  UpdateGfxSysCVars();

  // reload all loaded textures and eventually shadows
  extern INDEX shd_bCacheAll;
  extern void ReloadTextures(void);
  extern void CacheShadows(void);
  ReloadTextures();
  if (shd_bCacheAll) CacheShadows();
}

BOOL SvkMain::SetCurrentViewport_Vulkan(CViewPort* pvp)
{
  // determine full screen mode
  CDisplayMode dm;
  RECT rectWindow;
  _pGfx->GetCurrentDisplayMode(dm);
  ASSERT((dm.dm_pixSizeI == 0 && dm.dm_pixSizeJ == 0) || (dm.dm_pixSizeI != 0 && dm.dm_pixSizeJ != 0));
  GetClientRect(pvp->vp_hWnd, &rectWindow);
  const PIX pixWinSizeI = rectWindow.right - rectWindow.left;
  const PIX pixWinSizeJ = rectWindow.bottom - rectWindow.top;

  // full screen allows only one window (main one, which has already been initialized)
  if (dm.dm_pixSizeI == pixWinSizeI && dm.dm_pixSizeJ == pixWinSizeJ) 
  {
    _pGfx->gl_pvpActive = pvp;  // remember as current viewport (must do that BEFORE InitContext)
    if (_pGfx->gl_ulFlags & GLF_INITONNEXTWINDOW) InitContext_Vulkan();
    _pGfx->gl_ulFlags &= ~GLF_INITONNEXTWINDOW;
    return TRUE;
  }

  // if must init entire Vulkan
  if (_pGfx->gl_ulFlags & GLF_INITONNEXTWINDOW) {
    _pGfx->gl_ulFlags &= ~GLF_INITONNEXTWINDOW;
    // reopen window
    pvp->CloseCanvas();
    pvp->OpenCanvas();
    _pGfx->gl_pvpActive = pvp;
    InitContext_Vulkan();
    pvp->vp_ctDisplayChanges = _pGfx->gl_ctDriverChanges;
    return TRUE;
  }

  // if window was not set for this driver
  if (pvp->vp_ctDisplayChanges < _pGfx->gl_ctDriverChanges) {
    // reopen window
    pvp->CloseCanvas();
    pvp->OpenCanvas();
    pvp->vp_ctDisplayChanges = _pGfx->gl_ctDriverChanges;
    _pGfx->gl_pvpActive = pvp;
    return TRUE;
  }

  // no need to set context if it is the same window as last time
  if (_pGfx->gl_pvpActive != NULL && _pGfx->gl_pvpActive->vp_hWnd == pvp->vp_hWnd) return TRUE;

  // set rendering target
  //HRESULT hr;
  //LPDIRECT3DSURFACE8 pColorSurface;
  //hr = pvp->vp_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pColorSurface);
  //if (hr != D3D_OK) return FALSE;
  //hr = gl_pd3dDevice->SetRenderTarget(pColorSurface, pvp->vp_pSurfDepth);
  //D3DRELEASE(pColorSurface, TRUE);
  //if (hr != D3D_OK) return FALSE;

  // remember as current window
  _pGfx->gl_pvpActive = pvp;
  return TRUE;
}

void SvkMain::SwapBuffers_Vulkan()
{
  VkResult r;

  // wait until rendering is finished
  VkSemaphore smpToWait = gl_VkRenderFinishedSemaphores[gl_VkCmdBufferCurrent];

  VkPresentInfoKHR presentInfo = {};
  presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  presentInfo.waitSemaphoreCount = 1;
  presentInfo.pWaitSemaphores = &smpToWait;
  presentInfo.swapchainCount = 1;
  presentInfo.pSwapchains = &gl_VkSwapchain;
  presentInfo.pImageIndices = &gl_VkCurrentImageIndex;

  r = vkQueuePresentKHR(gl_VkQueuePresent, &presentInfo);

  if (r == VK_ERROR_OUT_OF_DATE_KHR || r == VK_SUBOPTIMAL_KHR)
  {

  }
  else if (r != VK_SUCCESS) 
  {
    ASSERTALWAYS("Vulkan error: Can't present swap chain image.\n");
  }
}

void SvkMain::SetViewport_Vulkan(float leftUpperX, float leftUpperY, float width, float height, float minDepth, float maxDepth)
{
  gl_VkCurrentViewport.x = leftUpperX;
  gl_VkCurrentViewport.y = leftUpperY;
  gl_VkCurrentViewport.width = width;
  gl_VkCurrentViewport.height = height;
  gl_VkCurrentViewport.minDepth = minDepth;
  gl_VkCurrentViewport.maxDepth = maxDepth;

  gl_VkCurrentScissor.extent.width = width;
  gl_VkCurrentScissor.extent.height = height;
  gl_VkCurrentScissor.offset.x = leftUpperX;
  gl_VkCurrentScissor.offset.y = leftUpperY;

  ASSERT(gl_VkCmdIsRecording);
  
  vkCmdSetViewport(GetCurrentCmdBuffer(), 0, 1, &gl_VkCurrentViewport);
  vkCmdSetScissor(GetCurrentCmdBuffer(), 0, 1, &gl_VkCurrentScissor);
}

BOOL SvkMain::InitSurface_Win32(HINSTANCE hinstance, HWND hwnd)
{
  VkWin32SurfaceCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  createInfo.pNext = nullptr;
  createInfo.hinstance = hinstance;
  createInfo.hwnd = hwnd;
  VkResult r = vkCreateWin32SurfaceKHR(gl_VkInstance, &createInfo, nullptr, &gl_VkSurface);

  return r == VK_SUCCESS;

  // const BOOL bFullScreen = (pixSizeI > 0 && pixSizeJ > 0);
}

BOOL SvkMain::CreateDevice()
{
  // temporary, graphics and transfer are the same family
  ASSERT(gl_VkQueueFamGraphics == gl_VkQueueFamTransfer);

  uint32_t uniqueFamilies[2] = { gl_VkQueueFamGraphics, gl_VkQueueFamPresent };
  uint32_t uniqueFamilyCount = gl_VkQueueFamGraphics == gl_VkQueueFamPresent ? 1 : 2;

  CStaticArray<VkDeviceQueueCreateInfo> queueInfos;
  queueInfos.New(uniqueFamilyCount);

  // only one queue
  const uint32_t queueCount = 1;
  float priorities[queueCount] = { 1.0f };

  for (uint32_t i = 0; i < uniqueFamilyCount; i++)
  {
    VkDeviceQueueCreateInfo &qinfo = queueInfos[i];
    qinfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    qinfo.pNext = nullptr;
    qinfo.flags = 0;
    qinfo.queueFamilyIndex = uniqueFamilies[i];
    qinfo.queueCount = queueCount;
    qinfo.pQueuePriorities = priorities;
  }

  VkPhysicalDeviceFeatures features = {};
  features.samplerAnisotropy = VK_TRUE;
  features.depthBounds = VK_TRUE;

  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = (uint32_t)queueInfos.Count();
  createInfo.pQueueCreateInfos = &queueInfos[0];
  createInfo.pEnabledFeatures = &features;
  createInfo.enabledExtensionCount = (uint32_t)gl_VkPhysDeviceExtensions.Count();
  createInfo.ppEnabledExtensionNames = &gl_VkPhysDeviceExtensions[0];
  createInfo.enabledLayerCount = (uint32_t)gl_VkLayers.Count();
  createInfo.ppEnabledLayerNames = &gl_VkLayers[0];

  if (vkCreateDevice(gl_VkPhysDevice, &createInfo, nullptr, &gl_VkDevice) != VK_SUCCESS)
  {
    return FALSE;
  }

  vkGetDeviceQueue(gl_VkDevice, gl_VkQueueFamGraphics, 0, &gl_VkQueueGraphics);
  gl_VkQueueTransfer = gl_VkQueueGraphics;

  vkGetDeviceQueue(gl_VkDevice, gl_VkQueueFamPresent, 0, &gl_VkQueuePresent);

  return TRUE;
}

void SvkMain::CreateRenderPass()
{
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = gl_VkSurfColorFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // VK_IMAGE_LAYOUT_GENERAL;

  VkAttachmentDescription depthAttachment = {};
  depthAttachment.format = gl_VkSurfDepthFormat;
  depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkAttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = SVK_RENDERPASS_COLOR_ATTACHMENT_INDEX;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment = SVK_RENDERPASS_DEPTH_ATTACHMENT_INDEX;
  depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpass = {};
  subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;
  subpass.pDepthStencilAttachment = &depthAttachmentRef;

  VkSubpassDependency dependency = {};
  dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependency.dstSubpass = 0;
  dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.srcAccessMask = 0;
  dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkAttachmentDescription attachments[2] = { colorAttachment, depthAttachment };
  VkRenderPassCreateInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.attachmentCount = 2;
  renderPassInfo.pAttachments = attachments;
  renderPassInfo.subpassCount = 1;
  renderPassInfo.pSubpasses = &subpass;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependency;

  if (vkCreateRenderPass(gl_VkDevice, &renderPassInfo, nullptr, &gl_VkRenderPass) != VK_SUCCESS) 
  {
    ASSERTALWAYS("Vulkan error: Can't create render pass.\n");
  }
}

void SvkMain::CreateCmdBuffers()
{
#ifndef NDEBUG
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    ASSERT(gl_VkCmdBuffers[i] == VK_NULL_HANDLE);
  }
#endif // !NDEBUG

  VkResult r;

  VkCommandPoolCreateInfo cmdPoolInfo = {};
  cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmdPoolInfo.pNext = nullptr;
  cmdPoolInfo.queueFamilyIndex = gl_VkQueueFamGraphics;
  cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  r = vkCreateCommandPool(gl_VkDevice, &cmdPoolInfo, nullptr, &gl_VkCmdPool);
  VK_CHECKERROR(r);

  // allocate cmd buffers
  VkCommandBufferAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  allocInfo.commandPool = gl_VkCmdPool;
  allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  allocInfo.commandBufferCount = gl_VkMaxCmdBufferCount;

  r = vkAllocateCommandBuffers(gl_VkDevice, &allocInfo, &gl_VkCmdBuffers[0]);
  VK_CHECKERROR(r);
}

void SvkMain::DestroyCmdBuffers()
{
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    gl_VkCmdBuffers[i] = VK_NULL_HANDLE;
  }

  vkDestroyCommandPool(gl_VkDevice, gl_VkCmdPool, nullptr);
}

void SvkMain::AcquireNextImage()
{
  VkResult r;
  uint32_t nextImageIndex;

  // get image index in swapchain
  r = vkAcquireNextImageKHR(gl_VkDevice, gl_VkSwapchain, UINT64_MAX,
    gl_VkImageAvailableSemaphores[gl_VkCmdBufferCurrent], VK_NULL_HANDLE, &nextImageIndex);

  if (r == VK_ERROR_OUT_OF_DATE_KHR)
  {
    // TODO: Vulkan: recreate swapchain
  }
  else if (r != VK_SUCCESS && r != VK_SUBOPTIMAL_KHR)
  {
    ASSERTALWAYS("Vulkan error: Can't to acquire swap chain image.\n");
  }

  // set to next image index
  gl_VkCurrentImageIndex = nextImageIndex;
}

void SvkMain::StartFrame()
{
  VkResult r;

  // set new index
  gl_VkCmdBufferCurrent = (gl_VkCmdBufferCurrent + 1) % gl_VkMaxCmdBufferCount;

  VkCommandBuffer cmd = gl_VkCmdBuffers[gl_VkCmdBufferCurrent];

  // wait when previous cmd with same index will be done
  r = vkWaitForFences(gl_VkDevice, 1, &gl_VkCmdFences[gl_VkCmdBufferCurrent], VK_TRUE, UINT64_MAX);
  VK_CHECKERROR(r);

  // fences must be set to unsignaled state manually
  vkResetFences(gl_VkDevice, 1, &gl_VkCmdFences[gl_VkCmdBufferCurrent]);

  // get next image index
  AcquireNextImage();

  // previous cmd with same index completely finished,
  // free dynamic buffers that have to be deleted
  FreeUnusedDynamicBuffers(gl_VkCmdBufferCurrent);

  // set 0 offsets to dynamic buffers for current cmd buffer
  ClearCurrentDynamicOffsets(gl_VkCmdBufferCurrent);

  FreeDeletedTextures(gl_VkCmdBufferCurrent);

  // reset previous pipeline
  gl_VkPreviousPipeline = nullptr;

  PrepareDescriptorSets(gl_VkCmdBufferCurrent);

  _no_ulTextureDescSet = GetTextureDescriptor(_no_ulTexture);

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  r = vkBeginCommandBuffer(cmd, &beginInfo);
  VK_CHECKERROR(r);

  VkClearValue clearValues[2];
  clearValues[0].color = { 0.25f, 0.25f, 0.25f, 1.0f };
  clearValues[1].depthStencil = { 0.0f, 0 };

  VkRenderPassBeginInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = gl_VkRenderPass;
  renderPassInfo.framebuffer = gl_VkFramebuffers[gl_VkCurrentImageIndex];
  renderPassInfo.renderArea.offset = { 0, 0 };
  renderPassInfo.renderArea.extent = gl_VkSwapChainExtent;
  renderPassInfo.clearValueCount = 2;
  renderPassInfo.pClearValues = clearValues;

  vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  gl_VkCmdIsRecording = true;

  // it is guaranteed that viewport and scissor will be set dynamically
}

void SvkMain::EndFrame()
{
  VkResult r;
  VkCommandBuffer cmd = gl_VkCmdBuffers[gl_VkCmdBufferCurrent];

  FlushDynamicBuffersMemory();

  vkCmdEndRenderPass(cmd);

  gl_VkCmdIsRecording = false;

  r = vkEndCommandBuffer(cmd);
  VK_CHECKERROR(r);

  // wait until image will be avaialable
  VkSemaphore smpToWait = gl_VkImageAvailableSemaphores[gl_VkCmdBufferCurrent];
  // signal when it's finished
  VkSemaphore smpToSignal = gl_VkRenderFinishedSemaphores[gl_VkCmdBufferCurrent];

  VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  VkSubmitInfo submitInfo[1] = {};
  submitInfo[0].pNext = NULL;
  submitInfo[0].sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo[0].waitSemaphoreCount = 1;
  submitInfo[0].pWaitSemaphores = &smpToWait;
  submitInfo[0].pWaitDstStageMask = &pipelineStageFlags;
  submitInfo[0].commandBufferCount = 1;
  submitInfo[0].pCommandBuffers = &cmd;
  submitInfo[0].signalSemaphoreCount = 1;
  submitInfo[0].pSignalSemaphores = &smpToSignal;

  // submit cmd buffer; fence will be in signaled state when cmd will be done
  r = vkQueueSubmit(gl_VkQueueGraphics, 1, submitInfo, gl_VkCmdFences[gl_VkCmdBufferCurrent]);
  VK_CHECKERROR(r);
}

VkCommandBuffer SvkMain::GetCurrentCmdBuffer()
{
  return gl_VkCmdBuffers[gl_VkCmdBufferCurrent];
}

void SvkMain::DrawTriangles(uint32_t indexCount, const uint32_t *indices)
{
  VkCommandBuffer cmd = gl_VkCmdBuffers[gl_VkCmdBufferCurrent];

  // prepare data
  CStaticStackArray<SvkVertex> &verts = gl_VkVerts;
  ASSERT(verts.Count() > 0);

  uint32_t vertsSize = verts.Count() * SVK_VERT_SIZE;
  uint32_t indicesSize = indexCount * sizeof(UINT);
  uint32_t uniformSize = 16 * sizeof(FLOAT);

  FLOAT mvp[16];
  if (GFX_bViewMatrix)
  {
    Svk_MatMultiply(mvp, VkViewMatrix, VkProjectionMatrix);
  }
  else
  {
    Svk_MatCopy(mvp, VkProjectionMatrix);
  }

  // get buffers
  SvkDynamicBuffer vertexBuffer, indexBuffer;
  SvkDynamicUniform uniformBuffer;

  GetVertexBuffer(vertsSize, vertexBuffer);
  GetIndexBuffer(indicesSize, indexBuffer);
  GetUniformBuffer(uniformSize, uniformBuffer);

  // copy data
  memcpy(vertexBuffer.sdb_Data, &verts[0], vertsSize);
  memcpy(indexBuffer.sdb_Data, indices, indicesSize);
  memcpy(uniformBuffer.sdb_Data, mvp, uniformSize);

  uint32_t descSetOffset = (uint32_t)uniformBuffer.sdb_CurrentOffset;

  // if previously not bound or flags don't match then bind new pipeline
  if (gl_VkPreviousPipeline == nullptr || (gl_VkPreviousPipeline != nullptr && gl_VkPreviousPipeline->sps_Flags != gl_VkGlobalState))
  {
    // bind pipeline
    SvkPipelineState &ps = GetPipeline(gl_VkGlobalState);
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, ps.sps_Pipeline);
    // save as it's most likely that next will be same
    gl_VkPreviousPipeline = &ps;
  }

  VkDescriptorSet sets[5] = { uniformBuffer.sdu_DescriptorSet };
  float textureColorScale = 1.0f;

  // bind texture descriptors
  for (uint32_t i = 0; i < GFX_MAXTEXUNITS; i++)
  {
    if (GFX_abTexture[i])
    {
      VkDescriptorSet textureDescSet = GetTextureDescriptor(gl_VkActiveTextures[i]);
     
      if (textureDescSet != VK_NULL_HANDLE)
      {
        sets[i + 1] = textureDescSet;

        ASSERT(GFX_iTexModulation[i] == 1 || GFX_iTexModulation[i] == 2);
        textureColorScale *= GFX_iTexModulation[i];
        continue;
      }
    }

    sets[i + 1] = _no_ulTextureDescSet;
  }

  // set uniform and textures
  vkCmdBindDescriptorSets(
    cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, gl_VkPipelineLayout,
    0, 5, sets,
    1, &descSetOffset);

  // set texture color scales
  vkCmdPushConstants(cmd, gl_VkPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 
    0, sizeof(float), &textureColorScale);

  // set mesh
  vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer.sdb_Buffer, &vertexBuffer.sdb_CurrentOffset);
  vkCmdBindIndexBuffer(cmd, indexBuffer.sdb_Buffer, indexBuffer.sdb_CurrentOffset, VK_INDEX_TYPE_UINT32);

  // draw
  vkCmdDrawIndexed(cmd, indexCount, 1, 0, 0, 0);
}
#endif // SE1_VULKAN