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

FLOAT	VkProjectionMatrix[16];
FLOAT	VkViewMatrix[16];


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

// initialize Vulkan driver
BOOL CGfxLibrary::InitDriver_Vulkan()
{
  gl_hiDriver = NONE; // must be initialized?

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

void CGfxLibrary::EndDriver_Vulkan(void)
{
  if (gl_VkInstance == VK_NULL_HANDLE)
  {
    return;
  }

  vkDeviceWaitIdle(gl_VkDevice);

  gl_VkVerts.Clear();

  DestroySwapchain();
  DestroySyncPrimitives();

  DestroyDynamicBuffers();
  DestroyCmdBuffers();

  DestroyDescriptorSetLayouts();
  DestroyDescriptorPools();

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

void CGfxLibrary::Reset_Vulkan()
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
  gl_VkCmdBufferCurrent = 0;

  gl_VkDescriptorPool = VK_NULL_HANDLE;
  gl_VkDescriptorSetLayout = VK_NULL_HANDLE;
  gl_VkPipelineLayout = VK_NULL_HANDLE;
  gl_VkPipelineCache = VK_NULL_HANDLE;
  gl_VkDefaultVertexLayout = nullptr;
  gl_VkShaderModuleVert = VK_NULL_HANDLE;
  gl_VkShaderModuleFrag = VK_NULL_HANDLE;
  gl_VkPreviousPipeline = nullptr;

  // reset to default
  gl_VkGlobalState = SVK_PLS_DEFAULT_FLAGS;

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
  }

  gl_VkDynamicVBGlobal.sdg_DynamicBufferMemory = VK_NULL_HANDLE;
  gl_VkDynamicIBGlobal.sdg_DynamicBufferMemory = VK_NULL_HANDLE;
  gl_VkDynamicUBGlobal.sdg_DynamicBufferMemory = VK_NULL_HANDLE;

  gl_VkDynamicVBGlobal.sdg_CurrentDynamicBufferSize = 0;
  gl_VkDynamicIBGlobal.sdg_CurrentDynamicBufferSize = 0;
  gl_VkDynamicUBGlobal.sdg_CurrentDynamicBufferSize = 0;
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
  gl_VkVerts.New(SVK_VERT_START_COUNT);
  gl_VkVerts.SetAllocationStep(SVK_VERT_ALLOC_STEP);

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

void CGfxLibrary::CreateCmdBuffers()
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

void CGfxLibrary::DestroyCmdBuffers()
{
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    gl_VkCmdBuffers[i] = VK_NULL_HANDLE;
  }

  vkDestroyCommandPool(gl_VkDevice, gl_VkCmdPool, nullptr);
}

void CGfxLibrary::AcquireNextImage()
{
  VkResult r;
  uint32_t nextImageIndex;

  // get image index in swapchain
  r = vkAcquireNextImageKHR(gl_VkDevice, gl_VkSwapchain, UINT64_MAX,
    gl_VkImageAvailableSemaphores[gl_VkCmdBufferCurrent], VK_NULL_HANDLE, &nextImageIndex);

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

void CGfxLibrary::StartFrame()
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

  // do NOT reset full state
  //gl_VkGlobalState = SVK_PLS_DEFAULT_FLAGS;
  // reset only flags that can't be reset in Gfx_wrapper_Vulkan
  gl_VkGlobalState &= ~SVK_PLS_DEPTH_BOUNDS_BOOL;
  gl_VkGlobalState &= ~SVK_PLS_DEPTH_BIAS_BOOL;

  // reset previous pipeline
  gl_VkPreviousPipeline = nullptr;

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  r = vkBeginCommandBuffer(cmd, &beginInfo);
  VK_CHECKERROR(r);

  VkClearValue clearValues[2];
  clearValues[0].color = { 0.25f, 0.25f, 0.25f, 1.0f };
  clearValues[1].depthStencil = { 1.0f, 0 };

  VkRenderPassBeginInfo renderPassInfo = {};
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  renderPassInfo.renderPass = gl_VkRenderPass;
  renderPassInfo.framebuffer = gl_VkFramebuffers[gl_VkCurrentImageIndex];
  renderPassInfo.renderArea.offset = { 0, 0 };
  renderPassInfo.renderArea.extent = gl_VkSwapChainExtent;
  renderPassInfo.clearValueCount = 2;
  renderPassInfo.pClearValues = clearValues;

  vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

  // set viewport and scissor dynamically
  if (gl_VkCurrentViewport.width != 0 && gl_VkCurrentViewport.height != 0)
  {
    vkCmdSetViewport(cmd, 0, 1, &gl_VkCurrentViewport);
  }
  else
  {
    // use default if wasn't set
    VkViewport vp;
    vp.minDepth = 0; vp.maxDepth = 1; vp.x = 0; vp.y = 0;
    vp.width = gl_VkSwapChainExtent.width; vp.height = gl_VkSwapChainExtent.height;
    vkCmdSetViewport(cmd, 0, 1, &vp);
  }

  if (gl_VkCurrentScissor.extent.width != 0 && gl_VkCurrentScissor.extent.height != 0)
  {
    vkCmdSetScissor(cmd, 0, 1, &gl_VkCurrentScissor);
  }
  else
  {
    // use default if wasn't set
    VkRect2D sc;
    sc.offset = { 0, 0 };
    sc.extent = { gl_VkSwapChainExtent.width, gl_VkSwapChainExtent.height };
    vkCmdSetScissor(cmd, 0, 1, &sc);
  }

  // vkCmdSetDepthBounds(cmd, 0.0f, 1.0f);
}

void CGfxLibrary::EndFrame()
{
  VkResult r;
  VkCommandBuffer cmd = gl_VkCmdBuffers[gl_VkCmdBufferCurrent];

  FlushDynamicBuffersMemory();

  vkCmdEndRenderPass(cmd);

  /*// transition from general layout to presentation
  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = gl_VkSwapchainImages[gl_VkCurrentImageIndex];
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = 1;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;
  barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  barrier.dstAccessMask = 0;

  vkCmdPipelineBarrier(
    cmd,
    VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
    VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
    0, 0, NULL, 0, NULL, 1, &barrier);*/

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

VkCommandBuffer CGfxLibrary::GetCurrentCmdBuffer()
{
  return gl_VkCmdBuffers[gl_VkCmdBufferCurrent];
}

void CGfxLibrary::DrawTriangles(uint32_t indexCount, const uint32_t *indices)
{
  VkCommandBuffer cmd = gl_VkCmdBuffers[gl_VkCmdBufferCurrent];

  // prepare data
  CStaticStackArray<SvkVertex> &verts = gl_VkVerts;
  ASSERT(verts.Count() > 0);

  uint32_t vertsSize = verts.Count() * SVK_VERT_SIZE;
  uint32_t indicesSize = indexCount * sizeof(UINT);
  uint32_t uniformSize = 16 * sizeof(FLOAT);

  FLOAT vp[16];
  if (GFX_bViewMatrix)
  {
    Svk_MatMultiply(vp, VkViewMatrix, VkProjectionMatrix);
  }
  else
  {
    Svk_MatCopy(vp, VkProjectionMatrix);
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
  memcpy(uniformBuffer.sdb_Data, vp, uniformSize);

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

  // bind descriptor set
  vkCmdBindDescriptorSets(
    cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, gl_VkPipelineLayout,
    0, 1, &uniformBuffer.sdu_DescriptorSet,
    1, &descSetOffset);

  // set mesh
  vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer.sdb_Buffer, &vertexBuffer.sdb_CurrentOffset);
  vkCmdBindIndexBuffer(cmd, indexBuffer.sdb_Buffer, indexBuffer.sdb_CurrentOffset, VK_INDEX_TYPE_UINT32);

  //// draw
  vkCmdDrawIndexed(cmd, indexCount, 1, 0, 0, 0);
}
#endif // SE1_VULKAN