#include "stdh.h"

#include <Engine/Base/Translation.h>
#include <Engine/Base/ErrorReporting.h>
#include <Engine/Base/Memory.h>
#include <Engine/Base/Console.h>

#include <Engine/Graphics/GfxLibrary.h>
#include <Engine/Graphics/ViewPort.h>

#include <Engine/Templates/StaticStackArray.cpp>
#include <Engine/Templates/DynamicContainer.cpp>
#include <Engine/Templates/Stock_CTextureData.h>

#pragma region Shaders
#include <Engine/Graphics/Vulkan/Shaders/Compiled/TexturedVertSpv.h>
#include <Engine/Graphics/Vulkan/Shaders/Compiled/TexturedFragSpv.h>

extern unsigned char TexturedVert_Spirv[];
extern unsigned int TexturedVert_Size;
extern unsigned char TexturedFrag_Spirv[];
extern unsigned int TexturedFrag_Size;
#pragma endregion


#ifdef SE1_VULKAN

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

// initialize Vulkan driver
BOOL CGfxLibrary::InitDriver_Vulkan()
{
  gl_hiDriver = NONE; // must be initialized?

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
    gl_VkLayers.New(1);
    gl_VkLayers[0] = "VK_LAYER_KHRONOS_validation";
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

  // it's required to do that
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

  VkCommandPoolCreateInfo cmdPoolInfo = {};
  cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  cmdPoolInfo.pNext = nullptr;
  cmdPoolInfo.queueFamilyIndex = gl_VkQueueFamGraphics;
  cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

  r = vkCreateCommandPool(gl_VkDevice, &cmdPoolInfo, nullptr, &gl_VkCmdPool);
  VK_CHECKERROR(r);

  VkDescriptorPoolSize poolSizes[2];
  poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSizes[0].descriptorCount = 16; // temporary
  poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  poolSizes[1].descriptorCount = 16; // temporary

  VkDescriptorPoolCreateInfo descPoolInfo = {};
  descPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  descPoolInfo.poolSizeCount = 2;
  descPoolInfo.pPoolSizes = poolSizes;
  descPoolInfo.maxSets = 32; // temporary

  r = vkCreateDescriptorPool(gl_VkDevice, &descPoolInfo, nullptr, &gl_VkDescriptorPool);
  VK_CHECKERROR(r);

  CreateSyncPrimitives();
  CreateRenderPass();
  CreateDescriptorSetLayout();
  CreateGraphicsPipeline();
  CreateMeshData();

  return TRUE;
}

void CGfxLibrary::EndDriver_Vulkan(void)
{
  if (gl_VkInstance == VK_NULL_HANDLE)
  {
    return;
  }

  gl_VkVerts.Clear();

  vkDeviceWaitIdle(gl_VkDevice);

  DestroySwapchain();

  for (uint32_t i = 0; i < gl_VkMaxFramesInFlight; i++)
  {
    vkDestroySemaphore(gl_VkDevice, gl_VkImageAvailableSemaphores[i], nullptr);
    vkDestroySemaphore(gl_VkDevice, gl_VkRenderFinishedSemaphores[i], nullptr);
    vkDestroyFence(gl_VkDevice, gl_VkInFlightFences[i], nullptr);
  }

  gl_VkImageAvailableSemaphores.Clear();
  gl_VkRenderFinishedSemaphores.Clear();
  gl_VkInFlightFences.Clear();

  gl_VkCmdBuffers.Clear();
  gl_VkDescriptorSets.Clear();

#ifndef NDEBUG
  auto pfnDestroyDUMsg = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(gl_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
  if (pfnDestroyDUMsg != nullptr) 
  {
    pfnDestroyDUMsg(gl_VkInstance, gl_VkDebugMessenger, nullptr);
  }
#endif

  DestroyMeshData();
  DestroyUniformBuffers();

  vkDestroyDescriptorSetLayout(gl_VkDevice, gl_VkDescriptorSetLayout, nullptr);
  vkDestroyPipelineLayout(gl_VkDevice, gl_VkPipelineLayout, nullptr);
  vkDestroyPipeline(gl_VkDevice, gl_VkGraphicsPipeline, nullptr);

  vkDestroyRenderPass(gl_VkDevice, gl_VkRenderPass, nullptr);
  vkDestroyCommandPool(gl_VkDevice, gl_VkCmdPool, nullptr);
  vkDestroySurfaceKHR(gl_VkInstance, gl_VkSurface, nullptr);
  vkDestroyDevice(gl_VkDevice, nullptr);
  vkDestroyInstance(gl_VkInstance, nullptr);

  Reset_Vulkan();
}

void CGfxLibrary::Reset_Vulkan()
{
  gl_VkInstance = VK_NULL_HANDLE;
  gl_VkDevice = VK_NULL_HANDLE;
  gl_VkSurface = VK_NULL_HANDLE;
  gl_VkCurrentImageIndex = 0;
  gl_VkCurrentViewport = {};
  gl_VkSwapChainExtent = {};
  gl_VkCmdPool = VK_NULL_HANDLE;;
  gl_VkRenderPass = VK_NULL_HANDLE;

  gl_VkSwapchain = VK_NULL_HANDLE;
  gl_VkSurfColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
  gl_VkSurfColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  gl_VkSurfDepthFormat = VK_FORMAT_D16_UNORM;
  gl_VkSurfPresentMode = VK_PRESENT_MODE_FIFO_KHR;
  gl_VkCurrentFrame = 0;

  gl_VkVertexBuffer = VK_NULL_HANDLE;
  gl_VkVertexBufferMemory = VK_NULL_HANDLE;
  gl_VkIndexBuffer = VK_NULL_HANDLE;
  gl_VkIndexBufferMemory = VK_NULL_HANDLE;

  gl_VkDescriptorSetLayout = VK_NULL_HANDLE;
  gl_VkPipelineLayout = VK_NULL_HANDLE;
  gl_VkGraphicsPipeline = VK_NULL_HANDLE;

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
}

// prepares Vulkan drawing context
void CGfxLibrary::InitContext_Vulkan()
{
  // must have context
  ASSERT(gl_pvpActive != NULL);

  // report header
  CPrintF(TRANS("\n* Vulkan context created: *------------------------------------\n"));
  CDisplayAdapter &da = gl_gaAPI[GAT_VK].ga_adaAdapter[gl_iCurrentAdapter];
  CPrintF("  (%s, %s, %s)\n\n", da.da_strVendor, da.da_strRenderer, da.da_strVersion);

  gl_ctMaxStreams = 16;
  gl_ctTextureUnits = 4;
  gl_ctRealTextureUnits = 4;
  gl_fMaxTextureLODBias = 0.0f;
  gl_iMaxTextureAnisotropy = 16;
  gl_iTessellationLevel = 0;
  gl_iMaxTessellationLevel = 0;
  GFX_ctVertices = 0;
  gl_VkVerts.Clear();

  GFX_bColorArray = TRUE;

  gl_ulFlags |= GLF_HASACCELERATION;
  gl_ulFlags |= GLF_32BITTEXTURES;
  gl_ulFlags |= GLF_VSYNC;
  gl_ulFlags &= ~GLF_TEXTURECOMPRESSION;

  // TODO

  // mark pretouching and probing
  extern BOOL _bNeedPretouch;
  _bNeedPretouch = TRUE;
  gl_bAllowProbing = FALSE;

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

BOOL CGfxLibrary::SetCurrentViewport_Vulkan(CViewPort* pvp)
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
    gl_pvpActive = pvp;  // remember as current viewport (must do that BEFORE InitContext)
    if (gl_ulFlags & GLF_INITONNEXTWINDOW) InitContext_Vulkan();
    gl_ulFlags &= ~GLF_INITONNEXTWINDOW;
    return TRUE;
  }

  // if must init entire Vulkan
  if (gl_ulFlags & GLF_INITONNEXTWINDOW) {
    gl_ulFlags &= ~GLF_INITONNEXTWINDOW;
    // reopen window
    pvp->CloseCanvas();
    pvp->OpenCanvas();
    gl_pvpActive = pvp;
    InitContext_Vulkan();
    pvp->vp_ctDisplayChanges = gl_ctDriverChanges;
    return TRUE;
  }

  // if window was not set for this driver
  if (pvp->vp_ctDisplayChanges < gl_ctDriverChanges) {
    // reopen window
    pvp->CloseCanvas();
    pvp->OpenCanvas();
    pvp->vp_ctDisplayChanges = gl_ctDriverChanges;
    gl_pvpActive = pvp;
    return TRUE;
  }

  // no need to set context if it is the same window as last time
  if (gl_pvpActive != NULL && gl_pvpActive->vp_hWnd == pvp->vp_hWnd) return TRUE;

  // set rendering target
  //HRESULT hr;
  //LPDIRECT3DSURFACE8 pColorSurface;
  //hr = pvp->vp_pSwapChain->GetBackBuffer(0, D3DBACKBUFFER_TYPE_MONO, &pColorSurface);
  //if (hr != D3D_OK) return FALSE;
  //hr = gl_pd3dDevice->SetRenderTarget(pColorSurface, pvp->vp_pSurfDepth);
  //D3DRELEASE(pColorSurface, TRUE);
  //if (hr != D3D_OK) return FALSE;

  // remember as current window
  gl_pvpActive = pvp;
  return TRUE;
}

void CGfxLibrary::SwapBuffers_Vulkan()
{
  VkResult r;

  // wait until rendering is finished
  VkSemaphore smpToWait = gl_VkRenderFinishedSemaphores[gl_VkCurrentFrame];

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

  gl_VkCurrentFrame = (gl_VkCurrentFrame + 1) % gl_VkMaxFramesInFlight;
}

void CGfxLibrary::SetViewport_Vulkan(float leftUpperX, float leftUpperY, float width, float height, float minDepth, float maxDepth)
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
}

BOOL CGfxLibrary::PickPhysicalDevice()
{
  VkResult r;
  uint32_t physDeviceCount = 0;

  r = vkEnumeratePhysicalDevices(gl_VkInstance, &physDeviceCount, NULL);
  VK_CHECKERROR(r)
    ASSERT(physDeviceCount > 0);

  CStaticArray<VkPhysicalDevice> physDevices;
  physDevices.New(physDeviceCount);

  r = vkEnumeratePhysicalDevices(gl_VkInstance, &physDeviceCount, &physDevices[0]);
  VK_CHECKERROR(r)
    ASSERT(physDeviceCount > 0);

  for (uint32_t i = 0; i < physDeviceCount; i++)
  {
    VkPhysicalDevice physDevice = physDevices[i];

    BOOL foundQueues = GetQueues(physDevice, gl_VkQueueFamGraphics, gl_VkQueueFamTransfer, gl_VkQueueFamPresent);
    BOOL extensionsSupported = CheckDeviceExtensions(physDevice, gl_VkPhysDeviceExtensions);

    if (foundQueues && extensionsSupported)
    {
      uint32_t formatsCount = 0, presentModesCount = 0;

      gl_VkPhysDevice = physDevice;
      vkGetPhysicalDeviceFeatures(physDevice, &gl_VkPhFeatures);
      vkGetPhysicalDeviceMemoryProperties(physDevice, &gl_VkPhMemoryProperties);
      vkGetPhysicalDeviceProperties(physDevice, &gl_VkPhProperties);
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physDevice, gl_VkSurface, &gl_VkPhSurfCapabilities);

      vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, gl_VkSurface, &formatsCount, nullptr);
      vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, gl_VkSurface, &presentModesCount, nullptr);

      if (formatsCount == 0 || presentModesCount == 0)
      {
        CPrintF("Vulkan error: Physical device doesn't have formats or present modes.\n");
        return FALSE;
      }

      if (gl_VkPhSurfFormats.Count() > 0) gl_VkPhSurfFormats.Delete();
      if (gl_VkPhSurfPresentModes.Count() > 0) gl_VkPhSurfPresentModes.Delete();

      gl_VkPhSurfFormats.New(formatsCount);
      gl_VkPhSurfPresentModes.New(presentModesCount);

      vkGetPhysicalDeviceSurfaceFormatsKHR(physDevice, gl_VkSurface, &formatsCount, &gl_VkPhSurfFormats[0]);
      vkGetPhysicalDeviceSurfacePresentModesKHR(physDevice, gl_VkSurface, &presentModesCount, &gl_VkPhSurfPresentModes[0]);

      // now select preferred settings
      gl_VkSurfColorFormat = VK_FORMAT_UNDEFINED;

      for (uint32_t i = 0; i < formatsCount; i++)
      {
        if (gl_VkPhSurfFormats[i].format == VK_FORMAT_R8G8B8A8_UNORM)
        {
          gl_VkSurfColorFormat = gl_VkPhSurfFormats[i].format;
          gl_VkSurfColorSpace = gl_VkPhSurfFormats[i].colorSpace;
          break;
        }
      }

      for (uint32_t i = 0; i < presentModesCount; i++)
      {
        if (gl_VkPhSurfPresentModes[i] == VK_PRESENT_MODE_FIFO_KHR)
        {
          gl_VkSurfPresentMode = gl_VkPhSurfPresentModes[i];
          break;
        }
      }

      VkFormat depthFormats[3] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
      gl_VkSurfDepthFormat = FindSupportedFormat(depthFormats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

      // TODO: if not found

      return TRUE;
    }
  }

  return FALSE;
}

BOOL CGfxLibrary::GetQueues(VkPhysicalDevice physDevice, 
  uint32_t &graphicsFamily, uint32_t &transferFamily, uint32_t &presentQueueFamily)
{
  uint32_t queueFamilyCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, nullptr);

  CStaticArray<VkQueueFamilyProperties> queuesFamilies;
  queuesFamilies.New(queueFamilyCount);
  vkGetPhysicalDeviceQueueFamilyProperties(physDevice, &queueFamilyCount, &queuesFamilies[0]);

  // actually, finds graphics and transfer queued that present in one family
  for (uint32_t i = 0; i < queueFamilyCount; i++)
  {
    VkQueueFamilyProperties &p = queuesFamilies[i];

    if (p.queueCount == 0)
    {
      continue;
    }

    VkBool32 presentSupport = FALSE;
    vkGetPhysicalDeviceSurfaceSupportKHR(physDevice, i, gl_VkSurface, &presentSupport);

    if (presentSupport)
    {
      presentQueueFamily = i;
    }

    bool found[] = {
      (uint32_t)p.queueFlags & VK_QUEUE_GRAPHICS_BIT,
      (uint32_t)p.queueFlags & VK_QUEUE_TRANSFER_BIT
    };

    if (found[0] && found[1])
    {
      graphicsFamily = transferFamily = i;
      return TRUE;
    }
  }

  return FALSE;
}

BOOL CGfxLibrary::CheckDeviceExtensions(VkPhysicalDevice physDevice, const CStaticArray<const char *> &requiredExtensions)
{
  uint32_t deviceExtCount;
  vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &deviceExtCount, nullptr);

  CStaticArray<VkExtensionProperties> deviceExts;
  deviceExts.New(deviceExtCount);
  vkEnumerateDeviceExtensionProperties(physDevice, nullptr, &deviceExtCount, &deviceExts[0]);

  for (uint32_t i = 0; i < requiredExtensions.Count(); i++)
  {
    BOOL found = FALSE;
    for (uint32_t j = 0; j < deviceExtCount; j++)
    {
      if (CTString(requiredExtensions[i]) == deviceExts[j].extensionName)
      {
        found = TRUE;
        break;
      }
    }

    if (!found)
    {
      return FALSE;
    }
  }

  return TRUE;
}

BOOL CGfxLibrary::InitSurface_Win32(HINSTANCE hinstance, HWND hwnd)
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

BOOL CGfxLibrary::CreateDevice()
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

  VkDeviceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  createInfo.queueCreateInfoCount = (uint32_t)queueInfos.Count();
  createInfo.pQueueCreateInfos = &queueInfos[0];
  createInfo.pEnabledFeatures = &gl_VkPhFeatures;
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

void CGfxLibrary::CreateRenderPass()
{
  VkAttachmentDescription colorAttachment = {};
  colorAttachment.format = gl_VkSurfColorFormat;
  colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
  colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
  colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
  colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

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
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkAttachmentReference depthAttachmentRef = {};
  depthAttachmentRef.attachment = 1;
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

void CGfxLibrary::CreateSyncPrimitives()
{
  VkResult r;

  ASSERT(gl_VkImageAvailableSemaphores.Count() == gl_VkRenderFinishedSemaphores.Count()
    && gl_VkRenderFinishedSemaphores.Count() == gl_VkInFlightFences.Count());

  if (gl_VkImageAvailableSemaphores.Count() > 0
    /*&& gl_VkRenderFinishedSemaphores.Count() > 0
    && gl_VkInFlightFences.Count() > 0*/)
  {
    return;
  }

  gl_VkImageAvailableSemaphores.New(gl_VkMaxFramesInFlight);
  gl_VkRenderFinishedSemaphores.New(gl_VkMaxFramesInFlight);
  gl_VkInFlightFences.New(gl_VkMaxFramesInFlight);

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < gl_VkMaxFramesInFlight; i++) 
  {
    r = vkCreateSemaphore(gl_VkDevice, &semaphoreInfo, nullptr, &gl_VkImageAvailableSemaphores[i]);
    VK_CHECKERROR(r);

    r = vkCreateSemaphore(gl_VkDevice, &semaphoreInfo, nullptr, &gl_VkRenderFinishedSemaphores[i]);
    VK_CHECKERROR(r);

    r = vkCreateFence(gl_VkDevice, &fenceInfo, nullptr, &gl_VkInFlightFences[i]);
    VK_CHECKERROR(r);
  }
}

void CGfxLibrary::CreateDescriptorSetLayout()
{
  VkResult r;

  VkDescriptorSetLayoutBinding uniformBinding = {};
  uniformBinding.binding = 0;
  uniformBinding.descriptorCount = 1;
  uniformBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  uniformBinding.pImmutableSamplers = nullptr;
  uniformBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

  VkDescriptorSetLayoutBinding samplerBinding = {};
  samplerBinding.binding = 1;
  samplerBinding.descriptorCount = 1;
  samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  samplerBinding.pImmutableSamplers = nullptr;
  samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

  VkDescriptorSetLayoutBinding bindings[2] = { uniformBinding, samplerBinding };
  VkDescriptorSetLayoutCreateInfo layoutInfo = {};
  layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  // TODO: textures
  layoutInfo.bindingCount = 1;
  layoutInfo.pBindings = bindings;

  r = vkCreateDescriptorSetLayout(gl_VkDevice, &layoutInfo, nullptr, &gl_VkDescriptorSetLayout);
}

void CGfxLibrary::CreateGraphicsPipeline()
{
  VkShaderModule vertShaderModule = CreateShaderModule((uint32_t*)TexturedVert_Spirv, TexturedVert_Size);
  VkShaderModule fragShaderModule = CreateShaderModule((uint32_t*)TexturedFrag_Spirv, TexturedFrag_Size);

  VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
  vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertShaderStageInfo.module = vertShaderModule;
  vertShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
  fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragShaderStageInfo.module = fragShaderModule;
  fragShaderStageInfo.pName = "main";

  VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

  VkVertexInputBindingDescription bindingDescriptions[1];
  bindingDescriptions[0].binding = 0;
  bindingDescriptions[0].stride = sizeof(VkVertex);
  bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  VkVertexInputAttributeDescription attributeDescriptions[4];
  attributeDescriptions[0].binding = 0;
  attributeDescriptions[0].location = VK_VERT_POS_LOC;
  attributeDescriptions[0].format = VK_VERT_POS_FORMAT;
  attributeDescriptions[0].offset = VK_VERT_POS_OFFSET;

  attributeDescriptions[1].binding = 0;
  attributeDescriptions[1].location = VK_VERT_COL_LOC;
  attributeDescriptions[1].format = VK_VERT_COL_FORMAT;
  attributeDescriptions[1].offset = VK_VERT_COL_OFFSET;

  attributeDescriptions[2].binding = 0;
  attributeDescriptions[2].location = VK_VERT_NOR_LOC;
  attributeDescriptions[2].format = VK_VERT_NOR_FORMAT;
  attributeDescriptions[2].offset = VK_VERT_NOR_OFFSET;

  attributeDescriptions[3].binding = 0;
  attributeDescriptions[3].location = VK_VERT_TEX_LOC;
  attributeDescriptions[3].format = VK_VERT_TEX_FORMAT;
  attributeDescriptions[3].offset = VK_VERT_TEX_OFFSET;

  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions;
  vertexInputInfo.vertexAttributeDescriptionCount = 4;
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;

  VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
  inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputAssembly.primitiveRestartEnable = VK_FALSE;

  VkPipelineViewportStateCreateInfo viewportState = {};
  viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportState.viewportCount = 1;
  viewportState.pViewports = nullptr; // will be set dynamically
  viewportState.scissorCount = 1;
  viewportState.pScissors = nullptr; // will be set dynamically

  VkPipelineRasterizationStateCreateInfo rasterizer = {};
  rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizer.depthClampEnable = VK_FALSE;
  rasterizer.rasterizerDiscardEnable = VK_FALSE;
  rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizer.lineWidth = 1.0f;
  rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
  rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  rasterizer.depthBiasEnable = VK_FALSE;

  VkPipelineMultisampleStateCreateInfo multisampling = {};
  multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampling.sampleShadingEnable = VK_FALSE;
  multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

  VkPipelineDepthStencilStateCreateInfo depthStencil = {};
  depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencil.depthTestEnable = VK_TRUE;
  depthStencil.depthWriteEnable = VK_TRUE;
  depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
  depthStencil.depthBoundsTestEnable = VK_FALSE;
  depthStencil.stencilTestEnable = VK_FALSE;

  VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending = {};
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;

  VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
  pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  pipelineLayoutInfo.setLayoutCount = 1;
  pipelineLayoutInfo.pSetLayouts = &gl_VkDescriptorSetLayout;

  if (vkCreatePipelineLayout(gl_VkDevice, &pipelineLayoutInfo, nullptr, &gl_VkPipelineLayout) != VK_SUCCESS)
  {
    ASSERTALWAYS("Vulkan error: Can't create pipeline layout.\n");
  }

  VkDynamicState dynamicStates[3] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
    VK_DYNAMIC_STATE_DEPTH_BOUNDS
  };

  VkPipelineDynamicStateCreateInfo dynamicInfo = {};
  dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
  dynamicInfo.dynamicStateCount = 3;
  dynamicInfo.pDynamicStates = dynamicStates;


  VkGraphicsPipelineCreateInfo pipelineInfo = {};
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.pInputAssemblyState = &inputAssembly;
  pipelineInfo.pViewportState = &viewportState;
  pipelineInfo.pRasterizationState = &rasterizer;
  pipelineInfo.pMultisampleState = &multisampling;
  pipelineInfo.pDepthStencilState = &depthStencil;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pDynamicState = &dynamicInfo;
  pipelineInfo.layout = gl_VkPipelineLayout;
  pipelineInfo.renderPass = gl_VkRenderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

  if (vkCreateGraphicsPipelines(gl_VkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &gl_VkGraphicsPipeline) != VK_SUCCESS)
  {
    ASSERTALWAYS("Vulkan error: Can't create graphics pipeline.\n");
  }
}

void CGfxLibrary::CreateSwapchain(uint32_t width, uint32_t height)
{
  // check consistency
  ASSERT(gl_VkSwapchainImages.Count() == gl_VkSwapchainImageViews.Count());
  ASSERT(gl_VkSwapchainImages.Count() == gl_VkSwapchainDepthImages.Count());
  ASSERT(gl_VkSwapchainImages.Count() == gl_VkSwapchainDepthMemory.Count());
  ASSERT(gl_VkSwapchainImages.Count() == gl_VkSwapchainDepthImageViews.Count());
  ASSERT(gl_VkSwapchainImages.Count() == gl_VkFramebuffers.Count());
  ASSERT(gl_VkSwapchainImages.Count() == gl_VkImagesInFlight.Count());

  // destroy if was created
  if (gl_VkSwapchainImages.Count() > 0)
  {
    DestroySwapchain();
  }

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gl_VkPhysDevice, gl_VkSurface, &gl_VkPhSurfCapabilities);

  const uint32_t preferredImageCount = gl_VkPhSurfCapabilities.minImageCount;
  uint32_t swapchainImageCount;

  VkSwapchainCreateInfoKHR createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  createInfo.surface = gl_VkSurface;
  createInfo.minImageCount = preferredImageCount;
  createInfo.imageFormat = gl_VkSurfColorFormat;
  createInfo.imageColorSpace = gl_VkSurfColorSpace;
  createInfo.imageExtent = GetSwapchainExtent(width, height);
  createInfo.imageArrayLayers = 1;
  createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

  uint32_t queueFamilyIndices[] = { gl_VkQueueFamGraphics, gl_VkQueueFamPresent };

  if (gl_VkQueueFamGraphics != gl_VkQueueFamPresent)
  {
    createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    createInfo.queueFamilyIndexCount = 2;
    createInfo.pQueueFamilyIndices = queueFamilyIndices;
  }
  else
  {
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  }

  createInfo.preTransform = gl_VkPhSurfCapabilities.currentTransform;
  createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  createInfo.presentMode = gl_VkSurfPresentMode;
  createInfo.clipped = VK_TRUE;

  if (vkCreateSwapchainKHR(gl_VkDevice, &createInfo, nullptr, &gl_VkSwapchain) != VK_SUCCESS)
  {
    ASSERTALWAYS("Vulkan error: Can't create VkSwapchainKHR.\n");
  }

  // get images from swapchain
  if (vkGetSwapchainImagesKHR(gl_VkDevice, gl_VkSwapchain, &swapchainImageCount, nullptr) != VK_SUCCESS)
  {
    ASSERTALWAYS("Vulkan error: Can't get swapchain images.\n");
  }

  gl_VkSwapchainImages.New(swapchainImageCount);
  gl_VkSwapchainImageViews.New(swapchainImageCount);
  gl_VkSwapchainDepthImages.New(swapchainImageCount);
  gl_VkSwapchainDepthMemory.New(swapchainImageCount);
  gl_VkSwapchainDepthImageViews.New(swapchainImageCount);
  gl_VkFramebuffers.New(swapchainImageCount);
  gl_VkImagesInFlight.New(swapchainImageCount);

  if (vkGetSwapchainImagesKHR(gl_VkDevice, gl_VkSwapchain, &swapchainImageCount, &gl_VkSwapchainImages[0]) != VK_SUCCESS)
  {
    ASSERTALWAYS("Vulkan error: Can't get swapchain images.\n");
  }

  // init image views
  for (uint32_t i = 0; i < swapchainImageCount; i++)
  {
    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.pNext = nullptr;
    viewInfo.format = gl_VkSurfColorFormat;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.flags = 0;
    viewInfo.image = gl_VkSwapchainImages[i];

    if (vkCreateImageView(gl_VkDevice, &viewInfo, nullptr, &gl_VkSwapchainImageViews[i]) != VK_SUCCESS)
    {
      ASSERTALWAYS("Vulkan error: Can't create image view for swapchain image.\n");
    }
  }

  // create depth buffers
  for (uint32_t i = 0; i < swapchainImageCount; i++)
  {
    if (!CreateSwapchainDepth(width, height, i))
    {
      ASSERTALWAYS("Vulkan error: Can't create depth buffers for swapchain.\n");
    }
  }

  // allocate cmd buffers, if required
  INDEX oldCount = gl_VkCmdBuffers.Count();
  int diff = (int)swapchainImageCount - (int)oldCount;
  if (diff > 0)
  {
    VkResult r;

    gl_VkCmdBuffers.Expand(swapchainImageCount);

    // allocate cmd buffers
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = gl_VkCmdPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)diff;

    r = vkAllocateCommandBuffers(gl_VkDevice, &allocInfo, &gl_VkCmdBuffers[oldCount]);
    VK_CHECKERROR(r);

    // allocate descriptor sets
    gl_VkDescriptorSets.Expand(swapchainImageCount);

    CStaticArray<VkDescriptorSetLayout> layouts;
    layouts.New(swapchainImageCount);
    for (uint32_t i = 0; i < swapchainImageCount; i++)
    {
      layouts[i] = gl_VkDescriptorSetLayout;
    }

    VkDescriptorSetAllocateInfo setAllocInfo = {};
    setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    setAllocInfo.descriptorPool = gl_VkDescriptorPool;
    setAllocInfo.descriptorSetCount = (uint32_t)diff;
    setAllocInfo.pSetLayouts = &layouts[0];

    r = vkAllocateDescriptorSets(gl_VkDevice, &setAllocInfo, &gl_VkDescriptorSets[oldCount]);
    VK_CHECKERROR(r);
  }

  DestroyUniformBuffers();
  CreateUniformBuffers(swapchainImageCount);
  UpdateDescriptorSet();

  gl_VkSwapChainExtent.width = width;
  gl_VkSwapChainExtent.height = height;

  // create framebuffers
  for (uint32_t i = 0; i < swapchainImageCount; i++)
  {
    VkImageView attachments[] = {
      gl_VkSwapchainImageViews[i],
      gl_VkSwapchainDepthImageViews[i]
    };

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = gl_VkRenderPass;
    framebufferInfo.attachmentCount = 2;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = gl_VkSwapChainExtent.width;
    framebufferInfo.height = gl_VkSwapChainExtent.height;
    framebufferInfo.layers = 1;

    if (vkCreateFramebuffer(gl_VkDevice, &framebufferInfo, nullptr, &gl_VkFramebuffers[i]) != VK_SUCCESS)
    {
      ASSERTALWAYS("Vulkan error: Can't create framebuffer.\n");
    }
  }

  for (uint32_t i = 0; i < swapchainImageCount; i++)
  {
    gl_VkImagesInFlight[i] = VK_NULL_HANDLE;
  }
}

void CGfxLibrary::RecreateSwapchain(uint32_t newWidth, uint32_t newHeight)
{
  if (gl_VkSwapChainExtent.width == newWidth && gl_VkSwapChainExtent.height == newHeight)
  {
    return;
  }

  // TODO

  gl_VkSwapChainExtent.width = newWidth;
  gl_VkSwapChainExtent.height = newHeight;
}

void CGfxLibrary::DestroySwapchain()
{
  if (gl_VkDevice == VK_NULL_HANDLE || gl_VkSwapchain == VK_NULL_HANDLE)
  {
    return;
  }

  vkDeviceWaitIdle(gl_VkDevice);

  gl_VkSwapChainExtent = {};

  for (uint32_t i = 0; i < gl_VkSwapchainDepthImages.Count(); i++) {
    vkDestroyImage(gl_VkDevice, gl_VkSwapchainDepthImages[i], nullptr);
  }

  for (uint32_t i = 0; i < gl_VkSwapchainDepthImageViews.Count(); i++) {
    vkDestroyImageView(gl_VkDevice, gl_VkSwapchainDepthImageViews[i], nullptr);
  }

  for (uint32_t i = 0; i < gl_VkSwapchainDepthMemory.Count(); i++) {
    vkFreeMemory(gl_VkDevice, gl_VkSwapchainDepthMemory[i], nullptr);
  }

  for (uint32_t i = 0; i < gl_VkFramebuffers.Count(); i++) {
    vkDestroyFramebuffer(gl_VkDevice, gl_VkFramebuffers[i], nullptr);
  }

  for (uint32_t i = 0; i < gl_VkSwapchainImageViews.Count(); i++) {
    vkDestroyImageView(gl_VkDevice, gl_VkSwapchainImageViews[i], nullptr);
  }


  vkDestroySwapchainKHR(gl_VkDevice, gl_VkSwapchain, nullptr);
  gl_VkSwapchain = VK_NULL_HANDLE;

  gl_VkSwapchainImages.Clear();
  gl_VkSwapchainImageViews.Clear();
  gl_VkSwapchainDepthImages.Clear();
  gl_VkSwapchainDepthMemory.Clear();
  gl_VkSwapchainDepthImageViews.Clear();
  gl_VkFramebuffers.Clear();
  gl_VkImagesInFlight.Clear();
}

VkExtent2D CGfxLibrary::GetSwapchainExtent(uint32_t width, uint32_t height)
{
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gl_VkPhysDevice, gl_VkSurface, &gl_VkPhSurfCapabilities);

  if (gl_VkPhSurfCapabilities.currentExtent.width == 0xFFFFFFFF) 
  {
    VkExtent2D extent;

    extent.width = Clamp<uint32_t>(width, 
      gl_VkPhSurfCapabilities.minImageExtent.width, 
      gl_VkPhSurfCapabilities.maxImageExtent.width);

    extent.height = Clamp<uint32_t>(height,
      gl_VkPhSurfCapabilities.minImageExtent.height,
      gl_VkPhSurfCapabilities.maxImageExtent.height);

    return extent;
  }
  else 
  {
    // if defined
    return gl_VkPhSurfCapabilities.currentExtent;
  }
}

BOOL CGfxLibrary::CreateSwapchainDepth(uint32_t width, uint32_t height, uint32_t imageIndex)
{
  VkImageCreateInfo depthImageInfo = {};

  depthImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  depthImageInfo.pNext = NULL;
  depthImageInfo.imageType = VK_IMAGE_TYPE_2D;
  depthImageInfo.format = gl_VkSurfDepthFormat;
  depthImageInfo.extent.width = width;
  depthImageInfo.extent.height = height;
  depthImageInfo.extent.depth = 1;
  depthImageInfo.mipLevels = 1;
  depthImageInfo.arrayLayers = 1;
  depthImageInfo.samples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
  depthImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  depthImageInfo.queueFamilyIndexCount = 0;
  depthImageInfo.pQueueFamilyIndices = nullptr;
  depthImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  depthImageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
  depthImageInfo.flags = 0;

  if (vkCreateImage(gl_VkDevice, &depthImageInfo, nullptr, &gl_VkSwapchainDepthImages[imageIndex]) != VK_SUCCESS)
  {
    CPrintF("Vulkan error: Can't create image for depth buffer.\n");
    return FALSE;
  }

  VkMemoryRequirements memReqs;
  vkGetImageMemoryRequirements(gl_VkDevice, gl_VkSwapchainDepthImages[imageIndex], &memReqs);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.pNext = NULL; allocInfo.allocationSize = memReqs.size;
  allocInfo.memoryTypeIndex = GetMemoryTypeIndex(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
  
  if (vkAllocateMemory(gl_VkDevice, &allocInfo, NULL, &gl_VkSwapchainDepthMemory[imageIndex]) != VK_SUCCESS)
  {
    CPrintF("Vulkan error: Can't allocate memory for depth buffer.\n");
    return FALSE;
  }

  if (vkBindImageMemory(gl_VkDevice, gl_VkSwapchainDepthImages[imageIndex], gl_VkSwapchainDepthMemory[imageIndex], 0) != VK_SUCCESS)
  {
    CPrintF("Vulkan error: Can't bind allocated memory to image in depth buffer.\n");
    return FALSE;
  }

  VkImageViewCreateInfo depthViewInfo = {};
  depthViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  depthViewInfo.pNext = nullptr;
  depthViewInfo.image = VK_NULL_HANDLE;
  depthViewInfo.format = gl_VkSurfDepthFormat;
  depthViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
  depthViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
  depthViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
  depthViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
  depthViewInfo.subresourceRange.aspectMask = gl_VkSurfDepthFormat == VK_FORMAT_D32_SFLOAT ?
    VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
  depthViewInfo.subresourceRange.baseMipLevel = 0;
  depthViewInfo.subresourceRange.levelCount = 1;
  depthViewInfo.subresourceRange.baseArrayLayer = 0;
  depthViewInfo.subresourceRange.layerCount = 1;
  depthViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  depthViewInfo.flags = 0;
  depthViewInfo.image = gl_VkSwapchainDepthImages[imageIndex];
 
  if (vkCreateImageView(gl_VkDevice, &depthViewInfo, nullptr, &gl_VkSwapchainDepthImageViews[imageIndex]) != VK_SUCCESS)
  {
    CPrintF("Vulkan error: Can't bind allocated memory to image in depth buffer.\n");
    return FALSE;
  }

  return TRUE;
}

void CGfxLibrary::CreateUniformBuffers(uint32_t swapchainImageCount)
{
  ASSERT(gl_VkUniformBuffers.Count() == 0);
  ASSERT(gl_VkUniformBuffersMemory.Count() == 0);

  // TODO
  VkDeviceSize bufferSize = 16;

  gl_VkUniformBuffers.New(swapchainImageCount);
  gl_VkUniformBuffersMemory.New(swapchainImageCount);

  for (size_t i = 0; i < swapchainImageCount; i++)
  {
    CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
      gl_VkUniformBuffers[i], gl_VkUniformBuffersMemory[i]);
  }
}

void CGfxLibrary::DestroyUniformBuffers()
{
  for (size_t i = 0; i < gl_VkUniformBuffers.Count(); i++)
  {
    vkDestroyBuffer(gl_VkDevice, gl_VkUniformBuffers[i], nullptr);
    vkFreeMemory(gl_VkDevice, gl_VkUniformBuffersMemory[i], nullptr);
  }

  gl_VkUniformBuffers.Clear();
  gl_VkUniformBuffersMemory.Clear();
}

void CGfxLibrary::StartFrame()
{
  VkResult r;
  uint32_t nextImageIndex;

  // wait when previous cmd with same frame index will be done
  r = vkWaitForFences(gl_VkDevice, 1, &gl_VkInFlightFences[gl_VkCurrentFrame], VK_TRUE, UINT64_MAX);
  VK_CHECKERROR(r);

  r = vkAcquireNextImageKHR(gl_VkDevice, gl_VkSwapchain, UINT64_MAX,
    gl_VkImageAvailableSemaphores[gl_VkCurrentFrame], VK_NULL_HANDLE, &nextImageIndex);

  if (r == VK_ERROR_OUT_OF_DATE_KHR)
  {
    // TODO: recreate swapchain
  }
  else if (r != VK_SUCCESS && r != VK_SUBOPTIMAL_KHR)
  {
    ASSERTALWAYS("Vulkan error: Can't to acquire swap chain image.\n");
  }

  // set to next image index
  gl_VkCurrentImageIndex = nextImageIndex;
}

void CGfxLibrary::StartCommandBuffer()
{
  VkResult r;
  uint32_t index = gl_VkCurrentImageIndex;
  VkCommandBuffer cmd = gl_VkCmdBuffers[index];

  if (gl_VkImagesInFlight[gl_VkCurrentImageIndex] != VK_NULL_HANDLE)
  {
    // wait for another fence that uses same image index
    r = vkWaitForFences(gl_VkDevice, 1, &gl_VkImagesInFlight[gl_VkCurrentImageIndex], VK_TRUE, UINT64_MAX);
    VK_CHECKERROR(r);
  }
  // set reference
  gl_VkImagesInFlight[gl_VkCurrentImageIndex] = gl_VkInFlightFences[gl_VkCurrentFrame];

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  r = vkBeginCommandBuffer(cmd, &beginInfo);
  VK_CHECKERROR(r);

  VkClearValue clearValues[2];
  clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
  clearValues[1].depthStencil = { 1.0f, 0 };

  VkRenderPassBeginInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = gl_VkRenderPass;
  renderPassInfo.framebuffer = gl_VkFramebuffers[index];
  renderPassInfo.renderArea.offset = { 0, 0 };
  renderPassInfo.renderArea.extent = gl_VkSwapChainExtent;
  renderPassInfo.clearValueCount = 2;
  renderPassInfo.pClearValues = clearValues;

  vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
}

void CGfxLibrary::EndCommandBuffer()
{
  VkResult r;
  VkCommandBuffer cmd = gl_VkCmdBuffers[gl_VkCurrentImageIndex];

  vkCmdEndRenderPass(cmd);
  r = vkEndCommandBuffer(cmd);
  VK_CHECKERROR(r);

  // wait until image will be avaialable
  VkSemaphore smpToWait = gl_VkImageAvailableSemaphores[gl_VkCurrentFrame];
  // signal when it's finished
  VkSemaphore smpToSignal = gl_VkRenderFinishedSemaphores[gl_VkCurrentFrame];

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

  // fences must be set to unsignaled state manually
  vkResetFences(gl_VkDevice, 1, &gl_VkInFlightFences[gl_VkCurrentFrame]);

  // submit cmd buffer; fence will be in signaled state when cmd will be done
  r = vkQueueSubmit(gl_VkQueueGraphics, 1, submitInfo, gl_VkInFlightFences[gl_VkCurrentFrame]);
  VK_CHECKERROR(r);
}

void CGfxLibrary::CreateMeshData()
{
  VkDeviceSize vertBufferSize = VK_VERT_SIZE * gl_VkMaxVertexCount;
  VkDeviceSize indexBufferSize = sizeof(UINT) * gl_VkMaxVertexCount;

  CreateBuffer(vertBufferSize, 
    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, 
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
    gl_VkVertexBuffer, gl_VkVertexBufferMemory);

  CreateBuffer(indexBufferSize,
    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    gl_VkIndexBuffer, gl_VkIndexBufferMemory);
}

void CGfxLibrary::DestroyMeshData()
{
  vkDestroyBuffer(gl_VkDevice, gl_VkVertexBuffer, nullptr);
  vkFreeMemory(gl_VkDevice, gl_VkVertexBufferMemory, nullptr);
  vkDestroyBuffer(gl_VkDevice, gl_VkIndexBuffer, nullptr);
  vkFreeMemory(gl_VkDevice, gl_VkIndexBufferMemory, nullptr);
}

VkShaderModule CGfxLibrary::CreateShaderModule(const uint32_t *spvCode, uint32_t codeSize)
{
  VkResult r;
  VkShaderModule shaderModule;

  VkShaderModuleCreateInfo moduleInfo = {};
  moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  moduleInfo.codeSize = codeSize;
  moduleInfo.pCode = spvCode;

  r = vkCreateShaderModule(gl_VkDevice, &moduleInfo, nullptr, &shaderModule);

  return shaderModule;
}

VkCommandBuffer CGfxLibrary::GetCurrentCmdBuffer()
{
  return gl_VkCmdBuffers[gl_VkCurrentImageIndex];
}

void CGfxLibrary::DrawTriangles(uint32_t indexCount, const uint32_t *indices)
{
  CStaticArray<VkVertex> &verts = gl_VkVerts;
  ASSERT(verts.Count() > 0);

  VkCommandBuffer cmd = GetCurrentCmdBuffer();

  // firstly, copy vertex and index data to buffers
  CopyToDeviceMemory(gl_VkVertexBufferMemory, &gl_VkVerts[0], gl_VkVerts.Count() * VK_VERT_SIZE);
  CopyToDeviceMemory(gl_VkIndexBufferMemory, indices, sizeof(uint32_t) * indexCount);

  vkCmdSetViewport(cmd, 0, 1, &gl_VkCurrentViewport);
  vkCmdSetScissor(cmd, 0, 1, &gl_VkCurrentScissor);

  // bind current pipeline
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, gl_VkGraphicsPipeline);

  // bind descriptor set
  vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, gl_VkPipelineLayout, 0, 1, &gl_VkDescriptorSets[gl_VkCurrentImageIndex], 0, nullptr);

  // set mesh
  VkDeviceSize offsets[] = { 0 };
  vkCmdBindVertexBuffers(cmd, 0, 1, &gl_VkVertexBuffer, offsets);
  vkCmdBindIndexBuffer(cmd, gl_VkIndexBuffer, 0, VK_INDEX_TYPE_UINT32);

  // draw
  vkCmdDrawIndexed(cmd, indexCount, 1, 0, 0, 0);
}

uint32_t CGfxLibrary::GetMemoryTypeIndex(uint32_t memoryTypeBits, VkFlags requirementsMask) 
{
  // for each memory type available for this device
  for (uint32_t i = 0; i < gl_VkPhMemoryProperties.memoryTypeCount; i++)
  {
    // if type is available
    if ((memoryTypeBits & 1u) == 1) {
      if ((gl_VkPhMemoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)
      {
        return i;
      }
    }

    memoryTypeBits >>= 1u;
  }

  CPrintF("Vulkan error: Can't find memory type in device memory properties");
  ASSERT(FALSE);
  return 0;
}

VkFormat CGfxLibrary::FindSupportedFormat(const VkFormat *formats, uint32_t formatCount, VkImageTiling tiling, VkFormatFeatureFlags features)
{
  for (uint32_t i = 0; i < formatCount; i++)
  {
    VkFormat format = formats[i];

    VkFormatProperties props;
    vkGetPhysicalDeviceFormatProperties(gl_VkPhysDevice, format, &props);

    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) 
    {
      return format;
    }
    else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
    {
      return format;
    }
  }

  CPrintF("Vulkan error: Can't find required format");
  ASSERT(FALSE);
  return VK_FORMAT_UNDEFINED;
}

void CGfxLibrary::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{
  VkResult r;

  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  r = vkCreateBuffer(gl_VkDevice, &bufferInfo, nullptr, &buffer);
  VK_CHECKERROR(r);

  VkMemoryRequirements memRequirements;
  vkGetBufferMemoryRequirements(gl_VkDevice, buffer, &memRequirements);

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memRequirements.size;
  allocInfo.memoryTypeIndex = GetMemoryTypeIndex(memRequirements.memoryTypeBits, properties);

  r = vkAllocateMemory(gl_VkDevice, &allocInfo, nullptr, &bufferMemory);
  VK_CHECKERROR(r);

  vkBindBufferMemory(gl_VkDevice, buffer, bufferMemory, 0);
}

void CGfxLibrary::CopyToDeviceMemory(VkDeviceMemory deviceMemory, const void *data, VkDeviceSize size)
{
  void *mapped;
  vkMapMemory(gl_VkDevice, deviceMemory, 0, size, 0, &mapped);
  memcpy(mapped, data, (size_t)size);
  vkUnmapMemory(gl_VkDevice, deviceMemory);
}

//BOOL CGfxLibrary::InitDisplay_Vulkan(INDEX iAdapter, PIX pixSizeI, PIX pixSizeJ, DisplayDepth eColorDepth)
//{
//  // prepare display mode
//  extern HWND _hwndMain;
//  HINSTANCE hInstance = GetModuleHandle(NULL);
//
//  VkWin32SurfaceCreateInfoKHR createInfo = {};
//  createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
//  createInfo.pNext = nullptr;
//  createInfo.hinstance = hInstance;
//  createInfo.hwnd = _hwndMain;
//  VkResult r = vkCreateWin32SurfaceKHR(gl_VkInstance, &createInfo, nullptr, &gl_VkSurface);
//
//  if (r != VK_SUCCESS)
//  {
//    CPrintF("Vulkan error: Can't create Win32 surface.\n");
//    return FALSE;
//  }
//
//  const BOOL bFullScreen = (pixSizeI > 0 && pixSizeJ > 0);
//
//  // ASSERT(eColorDepth == DD_32BIT);
//
//  //VK_FORMAT_D24_UNORM_S8_UINT
//
//
//  return 0;
//}

extern void setIdentity(float outMatrix[4][4])
{
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      outMatrix[i][j] = i != j ? 0.0f : 1.0f;
    }
  }
}

extern void orthoMatrix(float left, float right, float bottom, float top, float zNear, float zFar, float outMatrix[4][4])
{
  setIdentity(outMatrix);

  outMatrix[0][0] = 2.0f / (right - left);
  outMatrix[1][1] = 2.0f / (top - bottom);
  outMatrix[2][2] = 1.0f / (zFar - zNear);
  outMatrix[3][0] = -(right + left) / (right - left);
  outMatrix[3][1] = -(top + bottom) / (top - bottom);
  outMatrix[3][2] = -zNear / (zFar - zNear);
}

void CGfxLibrary::UpdateDescriptorSet()
{
  VkDescriptorBufferInfo bufferInfo = {};
  bufferInfo.buffer = gl_VkUniformBuffers[gl_VkCurrentImageIndex];
  bufferInfo.offset = 0;
  // TODO
  bufferInfo.range = 16;

  // TODO: textures
  //VkDescriptorImageInfo imageInfo = {};
  //imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
  //imageInfo.imageView = textureImageView;
  //imageInfo.sampler = textureSampler;

  VkWriteDescriptorSet descriptorWrites[2];

  descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  descriptorWrites[0].pNext = nullptr;
  descriptorWrites[0].dstSet = gl_VkDescriptorSets[gl_VkCurrentImageIndex];
  descriptorWrites[0].dstBinding = 0;
  descriptorWrites[0].dstArrayElement = 0;
  descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  descriptorWrites[0].descriptorCount = 1;
  descriptorWrites[0].pBufferInfo = &bufferInfo;
  descriptorWrites[0].pImageInfo = nullptr;
  descriptorWrites[0].pTexelBufferView = nullptr;

  //descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  //descriptorWrites[1].pNext = nullptr;
  //descriptorWrites[1].dstSet = gl_VkDescriptorSets[gl_VkCurrentImageIndex];
  //descriptorWrites[1].dstBinding = 1;
  //descriptorWrites[1].dstArrayElement = 0;
  //descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  //descriptorWrites[1].descriptorCount = 1;
  //descriptorWrites[1].pImageInfo = &imageInfo;
  //descriptorWrites[1].pBufferInfo = nullptr;
  //descriptorWrites[1].pTexelBufferView = nullptr;

  // TODO: textures - change to 2
  vkUpdateDescriptorSets(gl_VkDevice, 1, descriptorWrites, 0, nullptr);
}

#endif // SE1_VULKAN