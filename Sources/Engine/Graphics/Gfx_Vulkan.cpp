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

#ifdef SE1_VULKAN

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

  gl_VkLayers.New(1);
  gl_VkLayers[0] = "VK_LAYER_KHRONOS_validation";

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

  gl_VkPhysDeviceExtensions.New(1);
  gl_VkPhysDeviceExtensions[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

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

  return TRUE;
}

void CGfxLibrary::EndDriver_Vulkan(void)
{
  // TODO

#ifndef NDEBUG
  auto pfnDestroyDUMsg = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(gl_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
  if (pfnDestroyDUMsg != nullptr) 
  {
    pfnDestroyDUMsg(gl_VkInstance, gl_VkDebugMessenger, nullptr);
  }
#endif

  vkDestroyInstance(gl_VkInstance, nullptr);
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

  // TODO
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

  // if must init entire D3D
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

    bool foundQueues = GetQueues(physDevice, gl_VkQueueFamGraphics, gl_VkQueueFamTransfer, gl_VkQueueFamPresent);
    bool extensionsSupported = CheckDeviceExtensions(physDevice, gl_VkPhysDeviceExtensions);

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
  VkResult r;
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
      p.queueFlags & VK_QUEUE_GRAPHICS_BIT,
      p.queueFlags & VK_QUEUE_TRANSFER_BIT
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
    bool found = FALSE;
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

BOOL CGfxLibrary::CreateSwapchain(uint32_t width, uint32_t height)
{
  VkResult r1 = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gl_VkPhysDevice, gl_VkSurface, &gl_VkPhSurfCapabilities);

  uint32_t preferredImageCount = gl_VkPhSurfCapabilities.minImageCount;
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
    CPrintF("Vulkan error: Can't create VkSwapchainKHR.\n");
    return FALSE;
  }

  // get images from swapchain
  if (vkGetSwapchainImagesKHR(gl_VkDevice, gl_VkSwapchain, &swapchainImageCount, nullptr) != VK_SUCCESS)
  {
    CPrintF("Vulkan error: Can't get swapchain images.\n");
    return FALSE;
  }

  // allocate arrays
  gl_VkSwapchainImages.New(swapchainImageCount);
  gl_VkSwapchainImageViews.New(swapchainImageCount);
  gl_VkSwapchainDepthImages.New(swapchainImageCount);
  gl_VkSwapchainDepthMemory.New(swapchainImageCount);
  gl_VkSwapchainDepthImageViews.New(swapchainImageCount);

  if (vkGetSwapchainImagesKHR(gl_VkDevice, gl_VkSwapchain, &swapchainImageCount, &gl_VkSwapchainImages[0]) != VK_SUCCESS)
  {
    CPrintF("Vulkan error: Can't get swapchain images.\n");
    return FALSE;
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
      CPrintF("Vulkan error: Can't create image view for swapchain image.\n");
      return FALSE;
    }
  }

  // create depth buffers
  for (uint32_t i = 0; i < swapchainImageCount; i++)
  {
    if (!CreateSwapchainDepth(width, height, i))
    {
      return FALSE;
    }
  }

  return TRUE;
}

void CGfxLibrary::RecreateSwapchain(uint32_t newWidth, uint32_t newHeight)
{
}

void CGfxLibrary::DestroySwapchain()
{
  for (uint32_t i = 0; i < gl_VkSwapchainDepthImageViews.Count(); i++) {
    vkDestroyImageView(gl_VkDevice, gl_VkSwapchainDepthImageViews[i], nullptr);
  }

  for (uint32_t i = 0; i < gl_VkSwapchainDepthImages.Count(); i++) {
    vkDestroyImage(gl_VkDevice, gl_VkSwapchainDepthImages[i], nullptr);
  }

  for (uint32_t i = 0; i < gl_VkSwapchainDepthMemory.Count(); i++) {
    vkFreeMemory(gl_VkDevice, gl_VkSwapchainDepthMemory[i], nullptr);
  }

  for (uint32_t i = 0; i < gl_VkSwapchainImageViews.Count(); i++) {
    vkDestroyImageView(gl_VkDevice, gl_VkSwapchainImageViews[i], nullptr);
  }

  vkDestroySwapchainKHR(gl_VkDevice, gl_VkSwapchain, nullptr);

  gl_VkSwapchainImages.Clear();
  gl_VkSwapchainImageViews.Clear();
  gl_VkSwapchainDepthImages.Clear();
  gl_VkSwapchainDepthMemory.Clear();
  gl_VkSwapchainDepthImageViews.Clear();
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
  depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
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

#endif // SE1_VULKAN