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

FLOAT	VkProjectionMatrix[16];
FLOAT	VkViewMatrix[16];
//FLOAT	VkViewProjMatrix[16];
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
  InitDynamicBuffers();
  CreateCmdBuffers();
  CreateSyncPrimitives();
  CreateRenderPass();
  CreateDescriptorSetLayout();
  CreateGraphicsPipeline();

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

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    vkDestroySemaphore(gl_VkDevice, gl_VkImageAvailableSemaphores[i], nullptr);
    vkDestroySemaphore(gl_VkDevice, gl_VkRenderFinishedSemaphores[i], nullptr);
    vkDestroyFence(gl_VkDevice, gl_VkInFlightFences[i], nullptr);

    gl_VkImageAvailableSemaphores[i] = VK_NULL_HANDLE;
    gl_VkRenderFinishedSemaphores[i] = VK_NULL_HANDLE;
    gl_VkInFlightFences[i] = VK_NULL_HANDLE;
  }

#ifndef NDEBUG
  auto pfnDestroyDUMsg = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(gl_VkInstance, "vkDestroyDebugUtilsMessengerEXT");
  if (pfnDestroyDUMsg != nullptr) 
  {
    pfnDestroyDUMsg(gl_VkInstance, gl_VkDebugMessenger, nullptr);
  }
#endif

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    FreeDynamicBuffers(i);
  }

  DestroyCmdBuffers();

  vkDestroyDescriptorSetLayout(gl_VkDevice, gl_VkDescriptorSetLayout, nullptr);
  DestroyDescriptorPools();

  vkDestroyPipelineLayout(gl_VkDevice, gl_VkPipelineLayout, nullptr);
  vkDestroyPipeline(gl_VkDevice, gl_VkGraphicsPipeline, nullptr);

  vkDestroyRenderPass(gl_VkDevice, gl_VkRenderPass, nullptr);
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
  gl_VkCmdBufferCurrent = 0;
  gl_VkCmdBufferCurrent = 0;

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

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    gl_VkCmdBuffers[i] = VK_NULL_HANDLE;
    gl_VkDescriptorPools[i] = VK_NULL_HANDLE;
    gl_VkImageAvailableSemaphores[i] = VK_NULL_HANDLE;
    gl_VkRenderFinishedSemaphores[i] = VK_NULL_HANDLE;
    gl_VkInFlightFences[i] = VK_NULL_HANDLE;

    // manually set memory as arrays contain garbage
    gl_VkDescriptorSets[i].sa_Array = nullptr;
    gl_VkVertexBuffers[i].sa_Array = nullptr;
    gl_VkIndexBuffers[i].sa_Array = nullptr;
    gl_VkUniformBuffers[i].sa_Array = nullptr;
    gl_VkDescriptors[i].sa_Array = nullptr;

    gl_VkDescriptorSets[i].sa_Count =
      gl_VkVertexBuffers[i].sa_Count =
      gl_VkIndexBuffers[i].sa_Count =
      gl_VkUniformBuffers[i].sa_Count =
      gl_VkDescriptors[i].sa_Count = 0;

    gl_VkDescriptorSets[i].sa_UsedCount =
      gl_VkVertexBuffers[i].sa_UsedCount =
      gl_VkIndexBuffers[i].sa_UsedCount =
      gl_VkUniformBuffers[i].sa_UsedCount =
      gl_VkDescriptors[i].sa_UsedCount = 0;

    gl_VkDescriptorSets[i].sa_ctAllocationStep =
      gl_VkVertexBuffers[i].sa_ctAllocationStep =
      gl_VkIndexBuffers[i].sa_ctAllocationStep =
      gl_VkUniformBuffers[i].sa_ctAllocationStep =
      gl_VkDescriptors[i].sa_ctAllocationStep = 256;
  }
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
  bindingDescriptions[0].stride = sizeof(SvkVertex);
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

void CGfxLibrary::StartFrame()
{
  VkResult r;
  uint32_t nextImageIndex;

  // set new indices
  gl_VkCmdBufferCurrent = (gl_VkCmdBufferCurrent + 1) % gl_VkMaxCmdBufferCount;

  // wait when previous cmd with same index will be done
  r = vkWaitForFences(gl_VkDevice, 1, &gl_VkInFlightFences[gl_VkCmdBufferCurrent], VK_TRUE, UINT64_MAX);
  VK_CHECKERROR(r);

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


  // previous cmd with same index completely finished, 
  // free its data: vertex, index, uniform buffers, descriptor sets
  FreeDynamicBuffers(gl_VkCmdBufferCurrent);
}

void CGfxLibrary::StartCommandBuffer()
{
  VkResult r;
  VkCommandBuffer cmd = gl_VkCmdBuffers[gl_VkCmdBufferCurrent];

  if (gl_VkImagesInFlight[gl_VkCurrentImageIndex] != VK_NULL_HANDLE)
  {
    // wait for another fence that uses same image index
    r = vkWaitForFences(gl_VkDevice, 1, &gl_VkImagesInFlight[gl_VkCurrentImageIndex], VK_TRUE, UINT64_MAX);
    VK_CHECKERROR(r);
  }
  // set reference
  gl_VkImagesInFlight[gl_VkCurrentImageIndex] = gl_VkInFlightFences[gl_VkCmdBufferCurrent];

  VkCommandBufferBeginInfo beginInfo = {};
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

  r = vkBeginCommandBuffer(cmd, &beginInfo);
  VK_CHECKERROR(r);

  VkClearValue clearValues[2];
  clearValues[0].color = { 0.5f, 0.5f, 0.5f, 1.0f };
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
  vkCmdSetViewport(cmd, 0, 1, &gl_VkCurrentViewport);
  vkCmdSetScissor(cmd, 0, 1, &gl_VkCurrentScissor);
}

void CGfxLibrary::EndCommandBuffer()
{
  VkResult r;
  VkCommandBuffer cmd = gl_VkCmdBuffers[gl_VkCmdBufferCurrent];

  vkCmdEndRenderPass(cmd);
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

  // fences must be set to unsignaled state manually
  vkResetFences(gl_VkDevice, 1, &gl_VkInFlightFences[gl_VkCmdBufferCurrent]);

  // submit cmd buffer; fence will be in signaled state when cmd will be done
  r = vkQueueSubmit(gl_VkQueueGraphics, 1, submitInfo, gl_VkInFlightFences[gl_VkCmdBufferCurrent]);
  VK_CHECKERROR(r);
}

VkCommandBuffer CGfxLibrary::GetCurrentCmdBuffer()
{
  return gl_VkCmdBuffers[gl_VkCmdBufferCurrent];
}

void CGfxLibrary::DrawTriangles(uint32_t indexCount, const uint32_t *indices)
{
  CStaticArray<SvkVertex> &verts = gl_VkVerts;
  ASSERT(verts.Count() > 0);

  VkCommandBuffer cmd = gl_VkCmdBuffers[gl_VkCmdBufferCurrent];
  
  // TODO: dynamic vertex and index buffer
  // TODO: dynamic descriptor sets
  SvkBufferObject vertexBuffer = GetVertexBuffer(&verts[0], gl_VkVerts.Count() * VK_VERT_SIZE);
  SvkBufferObject indexBuffer = GetIndexBuffer(&indices[0], sizeof(UINT) * indexCount);

  FLOAT mvp[16];
  // TODO: check
  Svk_MatMultiply(mvp, VkViewMatrix, VkProjectionMatrix);

  const SvkDescriptorObject &descObj = GetUniformBuffer(mvp, 16 * sizeof(FLOAT));

  // bind current pipeline
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, gl_VkGraphicsPipeline);

  // bind descriptor set
  vkCmdBindDescriptorSets(
    cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, gl_VkPipelineLayout,
    0, 1, &descObj.sdo_DescSet,
    0, nullptr);
    // TODO: uncomment this when uniform buffer will be dynamic
    //1, &descObj.sdo_Offset);

  // set mesh
  VkDeviceSize offsets[] = { 0 };
  vkCmdBindVertexBuffers(cmd, 0, 1, &vertexBuffer.sbo_Buffer, offsets);
  vkCmdBindIndexBuffer(cmd, indexBuffer.sbo_Buffer, 0, VK_INDEX_TYPE_UINT32);

  // draw
  vkCmdDrawIndexed(cmd, indexCount, 1, 0, 0, 0);
}

void Svk_MatCopy(const float *src, float *dest)
{
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      dest[i * 4 + j] = src[i * 4 + j];
    }
  }
}

void Svk_MatSetIdentity(float *result)
{
  result[0] = 1.0f;
  result[1] = 0.0f;
  result[2] = 0.0f;
  result[3] = 0.0f;
  result[4] = 0.0f;
  result[5] = 1.0f;
  result[6] = 0.0f;
  result[7] = 0.0f;
  result[8] = 0.0f;
  result[9] = 0.0f;
  result[10] = 1.0f;
  result[11] = 0.0f;
  result[12] = 0.0f;
  result[13] = 0.0f;
  result[14] = 0.0f;
  result[15] = 1.0f;
}

void Svk_MatMultiply(float *result, const float *a, const float *b)
{
  for (int i = 0; i < 4; i++)
  {
    for (int j = 0; j < 4; j++)
    {
      result[i * 4 + j] = 0.0f;

      for (int s = 0; s < 4; s++)
      {
        result[i * 4 + j] += a[i * 4 + s] * b[s * 4 + j];
      }
    }
  }
}

void Svk_MatFrustum(float *result, float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar)
{
  const float fRpL = fRight + fLeft;  const float fRmL = fRight - fLeft;  const float fFpN = fFar + fNear;
  const float fTpB = fTop + fBottom;  const float fTmB = fTop - fBottom;  const float fFmN = fFar - fNear;
  const float f2Fm2N = 2.0f * fFar - 2.0f * fNear;

  result[0 * 4 + 0] = 2.0f * fNear / fRmL;
  result[0 * 4 + 1] = 0.0f;
  result[0 * 4 + 2] = 0.0f;
  result[0 * 4 + 3] = 0.0f;

  result[1 * 4 + 0] = 0.0f;
  result[1 * 4 + 1] = -2.0f * fNear / fTmB;
  result[1 * 4 + 2] = 0.0f;
  result[1 * 4 + 3] = 0.0f;

  result[2 * 4 + 0] = fRpL / fRmL;
  result[2 * 4 + 1] = -fTpB / fTmB;
  result[2 * 4 + 2] = -(2 * fFar - fNear) / f2Fm2N;
  result[2 * 4 + 3] = -1.0f;

  result[3 * 4 + 0] = 0.0f;
  result[3 * 4 + 1] = 0.0f;
  result[3 * 4 + 2] = -fFar * fNear / f2Fm2N;
  result[3 * 4 + 3] = 0.0f;
}

void Svk_MatOrtho(float *result, float fLeft, float fRight, float fBottom, float fTop, float fNear, float fFar)
{
  const float fRpL = fRight + fLeft;  const float fRmL = fRight - fLeft;  const float fFpN = fFar + fNear;
  const float fTpB = fTop + fBottom;  const float fTmB = fTop - fBottom;  const float fFmN = fFar - fNear;
  const float f2Fm2N = 2 * fFar - 2 * fNear;

  result[0 * 4 + 0] = 2.0f / fRmL;
  result[0 * 4 + 1] = 0.0f;
  result[0 * 4 + 2] = 0.0f;
  result[0 * 4 + 3] = 0.0f;

  result[1 * 4 + 0] = 0.0f;
  result[1 * 4 + 1] = -2.0f / fTmB;
  result[1 * 4 + 2] = 0.0f;
  result[1 * 4 + 3] = 0.0f;

  result[2 * 4 + 0] = 0.0f;
  result[2 * 4 + 1] = 0.0f;
  result[2 * 4 + 2] = -1.0f / f2Fm2N;
  result[2 * 4 + 3] = 0.0f;

  result[3 * 4 + 0] = -fRpL / fRmL;
  result[3 * 4 + 1] = fTpB / fTmB;
  result[3 * 4 + 2] = (2.0f * fFar - 3.0f * fNear) / f2Fm2N;
  result[3 * 4 + 3] = 1.0f;
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