#include "stdh.h"
#include <Engine/Graphics/GfxLibrary.h>

void CGfxLibrary::CreateSwapchain(uint32_t width, uint32_t height)
{
  // check consistency
  ASSERT(gl_VkSwapchainImages.Count() == gl_VkSwapchainImageViews.Count());
  ASSERT(gl_VkSwapchainImages.Count() == gl_VkSwapchainDepthImages.Count());
  ASSERT(gl_VkSwapchainImages.Count() == gl_VkSwapchainDepthMemory.Count());
  ASSERT(gl_VkSwapchainImages.Count() == gl_VkSwapchainDepthImageViews.Count());
  ASSERT(gl_VkSwapchainImages.Count() == gl_VkFramebuffers.Count());

  // destroy if was created
  if (gl_VkSwapchainImages.Count() > 0)
  {
    DestroySwapchain();
  }

  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gl_VkPhysDevice, gl_VkSurface, &gl_VkPhSurfCapabilities);

  const uint32_t preferredImageCount = 2; 
  ASSERT(gl_VkPhSurfCapabilities.maxImageCount >= 2);
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
  depthImageInfo.pNext = nullptr;
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
  depthImageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
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
  allocInfo.pNext = nullptr; 
  allocInfo.allocationSize = memReqs.size;
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
  depthViewInfo.format = gl_VkSurfDepthFormat;
  depthViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
  depthViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
  depthViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
  depthViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;
  depthViewInfo.subresourceRange.aspectMask = gl_VkSurfDepthFormat == VK_FORMAT_D32_SFLOAT ?
    VK_IMAGE_ASPECT_DEPTH_BIT : (VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT);
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
