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
#include <Engine/Graphics/Vulkan/SvkMain.h>

#ifdef SE1_VULKAN

VkShaderModule SvkMain::CreateShaderModule(const uint32_t *spvCode, uint32_t codeSize)
{
  VkResult r;
  VkShaderModule shaderModule;

  VkShaderModuleCreateInfo moduleInfo = {};
  moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  moduleInfo.codeSize = codeSize;
  moduleInfo.pCode = spvCode;

  r = vkCreateShaderModule(gl_VkDevice, &moduleInfo, nullptr, &shaderModule);
  VK_CHECKERROR(r);

  return shaderModule;
}

uint32_t SvkMain::GetMemoryTypeIndex(uint32_t memoryTypeBits, VkFlags requirementsMask)
{
  // for each memory type available for this device
  for (uint32_t i = 0; i < gl_VkPhMemoryProperties.memoryTypeCount; i++)
  {
    // if type is available
    if ((memoryTypeBits & 1u) == 1) 
    {
      if ((gl_VkPhMemoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)
      {
        return i;
      }
    }

    memoryTypeBits >>= 1u;
  }

  CPrintF("Vulkan error: Can't find memory type in device memory properties");
  return 0;
}

uint32_t SvkMain::GetMemoryTypeIndex(uint32_t memoryTypeBits, VkFlags requirementsMask, VkFlags preferredMask)
{
  // for each memory type available for this device
  for (uint32_t i = 0; i < gl_VkPhMemoryProperties.memoryTypeCount; i++)
  {
    // if type is available
    if ((memoryTypeBits & 1u) == 1)
    {
      if ((gl_VkPhMemoryProperties.memoryTypes[i].propertyFlags & (requirementsMask | preferredMask)) == (requirementsMask | preferredMask))
      {
        return i;
      }
    }

    memoryTypeBits >>= 1u;
  }

  for (uint32_t i = 0; i < gl_VkPhMemoryProperties.memoryTypeCount; i++)
  {
    // if type is available
    if ((memoryTypeBits & 1u) == 1)
    {
      if ((gl_VkPhMemoryProperties.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)
      {
        return i;
      }
    }

    memoryTypeBits >>= 1u;
  }

  CPrintF("Vulkan error: Can't find memory type in device memory properties");
  return 0;
}

VkFormat SvkMain::FindSupportedFormat(const VkFormat *formats, uint32_t formatCount, VkImageTiling tiling, VkFormatFeatureFlags features)
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

void SvkMain::CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
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

void SvkMain::CopyToDeviceMemory(VkDeviceMemory deviceMemory, const void *data, VkDeviceSize size)
{
  void *mapped;
  vkMapMemory(gl_VkDevice, deviceMemory, 0, size, 0, &mapped);
  memcpy(mapped, data, (size_t)size);
  vkUnmapMemory(gl_VkDevice, deviceMemory);
}

BOOL SvkMain::GetQueues(VkPhysicalDevice physDevice,
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
      ((uint32_t)p.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT,
      ((uint32_t)p.queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT
    };

    if (found[0] && found[1])
    {
      graphicsFamily = transferFamily = i;
      return TRUE;
    }
  }

  return FALSE;
}

BOOL SvkMain::CheckDeviceExtensions(VkPhysicalDevice physDevice, const CStaticArray<const char *> &requiredExtensions)
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


void SvkMain::CreateSyncPrimitives()
{
  VkResult r;

#ifndef NDEBUG
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    ASSERT(gl_VkImageAvailableSemaphores[i] == VK_NULL_HANDLE);
    ASSERT(gl_VkRenderFinishedSemaphores[i] == VK_NULL_HANDLE);
    ASSERT(gl_VkCmdFences[i] == VK_NULL_HANDLE);
  }
#endif // !NDEBUG

  VkSemaphoreCreateInfo semaphoreInfo = {};
  semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

  VkFenceCreateInfo fenceInfo = {};
  fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    r = vkCreateSemaphore(gl_VkDevice, &semaphoreInfo, nullptr, &gl_VkImageAvailableSemaphores[i]);
    VK_CHECKERROR(r);

    r = vkCreateSemaphore(gl_VkDevice, &semaphoreInfo, nullptr, &gl_VkRenderFinishedSemaphores[i]);
    VK_CHECKERROR(r);

    r = vkCreateFence(gl_VkDevice, &fenceInfo, nullptr, &gl_VkCmdFences[i]);
    VK_CHECKERROR(r);
  }
}

void SvkMain::DestroySyncPrimitives()
{
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    vkDestroySemaphore(gl_VkDevice, gl_VkImageAvailableSemaphores[i], nullptr);
    vkDestroySemaphore(gl_VkDevice, gl_VkRenderFinishedSemaphores[i], nullptr);
    vkDestroyFence(gl_VkDevice, gl_VkCmdFences[i], nullptr);

    gl_VkImageAvailableSemaphores[i] = VK_NULL_HANDLE;
    gl_VkRenderFinishedSemaphores[i] = VK_NULL_HANDLE;
    gl_VkCmdFences[i] = VK_NULL_HANDLE;
  }
}

void SvkMain::CreateVertexLayouts()
{
  gl_VkDefaultVertexLayout = new SvkVertexLayout();
  auto &binds = gl_VkDefaultVertexLayout->svl_Bindings;
  auto &attrs = gl_VkDefaultVertexLayout->svl_Attributes;

  binds.New(1);
  attrs.New(5);

  binds[0].binding = 0;
  binds[0].stride = SVK_VERT_SIZE;
  binds[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

  attrs[0].binding = 0;
  attrs[0].location = SVK_VERT_POS_LOC;
  attrs[0].format = SVK_VERT_POS_FORMAT;
  attrs[0].offset = SVK_VERT_POS_OFFSET;

  attrs[1].binding = 0;
  attrs[1].location = SVK_VERT_COL_LOC;
  attrs[1].format = SVK_VERT_COL_FORMAT;
  attrs[1].offset = SVK_VERT_COL_OFFSET;

  attrs[2].binding = 0;
  attrs[2].location = SVK_VERT_NOR_LOC;
  attrs[2].format = SVK_VERT_NOR_FORMAT;
  attrs[2].offset = SVK_VERT_NOR_OFFSET;

  attrs[3].binding = 0;
  attrs[3].location = SVK_VERT_TEX01_LOC;
  attrs[3].format = SVK_VERT_TEX01_FORMAT;
  attrs[3].offset = SVK_VERT_TEX01_OFFSET;

  attrs[4].binding = 0;
  attrs[4].location = SVK_VERT_TEX23_LOC;
  attrs[4].format = SVK_VERT_TEX23_FORMAT;
  attrs[4].offset = SVK_VERT_TEX23_OFFSET;
}

void SvkMain::DestroyVertexLayouts()
{
  delete gl_VkDefaultVertexLayout;
  gl_VkDefaultVertexLayout = nullptr;
}

BOOL SvkMain::PickPhysicalDevice()
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

      for (uint32_t j = 0; j < formatsCount; j++)
      {
        if (gl_VkPhSurfFormats[j].format == VK_FORMAT_R8G8B8A8_UNORM)
        {
          gl_VkSurfColorFormat = gl_VkPhSurfFormats[j].format;
          gl_VkSurfColorSpace = gl_VkPhSurfFormats[j].colorSpace;
          break;
        }
      }

      extern INDEX gfx_vk_iPresentMode;
      VkPresentModeKHR preferredPresentMode;

      switch (gfx_vk_iPresentMode)
      {
      case 1: preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR; break;
      case 2: preferredPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR; break;
      default: preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR; break;
      }

      for (uint32_t j = 0; j < presentModesCount; j++)
      {
        if (gl_VkPhSurfPresentModes[j] == preferredPresentMode)
        {
          gl_VkSurfPresentMode = gl_VkPhSurfPresentModes[j];
          break;
        }
      }

      VkFormat depthFormats[3] = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
      gl_VkSurfDepthFormat = FindSupportedFormat(depthFormats, 3, VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

      if (gl_VkSurfColorFormat == VK_FORMAT_UNDEFINED)
      {
        gl_VkSurfColorFormat = gl_VkPhSurfFormats[0].format;
        gl_VkSurfColorSpace = gl_VkPhSurfFormats[0].colorSpace;
      }

      // it's guaranteed that maxPushConstantSize will be >=128
      ASSERT(gl_VkPhProperties.limits.maxPushConstantsSize >= 128);

      // get max sample count
      VkSampleCountFlags counts = gl_VkPhProperties.limits.framebufferColorSampleCounts & gl_VkPhProperties.limits.framebufferDepthSampleCounts;
      
      extern INDEX gfx_vk_iMSAA;

      if (counts & VK_SAMPLE_COUNT_8_BIT && gfx_vk_iMSAA >= 3)
      {
        gl_VkMaxSampleCount = VK_SAMPLE_COUNT_8_BIT;
      }
      else if (counts & VK_SAMPLE_COUNT_4_BIT && gfx_vk_iMSAA >= 2)
      {
        gl_VkMaxSampleCount = VK_SAMPLE_COUNT_4_BIT;
      }
      else if (counts & VK_SAMPLE_COUNT_2_BIT && gfx_vk_iMSAA >= 1)
      {
        gl_VkMaxSampleCount = VK_SAMPLE_COUNT_2_BIT;
      }
      else
      {
        gl_VkMaxSampleCount = VK_SAMPLE_COUNT_1_BIT;
      }
      
      return TRUE;
    }
  }

  return FALSE;
}

void SvkMain::ClearColor(int32_t x, int32_t y, uint32_t width, uint32_t height, float *rgba)
{
  // must be in recording state
  ASSERT(gl_VkCmdIsRecording);

  VkClearAttachment ca = {};
  ca.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  ca.colorAttachment = SVK_RENDERPASS_COLOR_ATTACHMENT_INDEX;

  VkClearColorValue &cv = ca.clearValue.color;
  cv.float32[0] = rgba[0];
  cv.float32[1] = rgba[1];
  cv.float32[2] = rgba[2];
  cv.float32[3] = rgba[3];

  VkClearRect cr = {};
  cr.baseArrayLayer = 0;
  cr.layerCount = 1;
  cr.rect.extent.width = width;
  cr.rect.extent.height = height;
  cr.rect.offset.x = x;
  cr.rect.offset.y = y;

  vkCmdClearAttachments(GetCurrentCmdBuffer(), 1, &ca, 1, &cr);
}

void SvkMain::ClearDepth(int32_t x, int32_t y, uint32_t width, uint32_t height, float depth)
{
  // must be in recording state
  ASSERT(gl_VkCmdIsRecording);

  VkClearAttachment ca = {};
  ca.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

  VkClearDepthStencilValue &cd = ca.clearValue.depthStencil;
  cd.depth = depth;

  VkClearRect cr = {};
  cr.baseArrayLayer = 0;
  cr.layerCount = 1;
  cr.rect.extent.width = width;
  cr.rect.extent.height = height;
  cr.rect.offset.x = x;
  cr.rect.offset.y = y;

  vkCmdClearAttachments(GetCurrentCmdBuffer(), 1, &ca, 1, &cr);
}

void SvkMain::ClearColor(float *rgba)
{
  ClearColor(
    gl_VkCurrentViewport.x, gl_VkCurrentViewport.y, 
    gl_VkCurrentViewport.width, gl_VkCurrentViewport.height, rgba);
}
void SvkMain::ClearDepth(float depth)
{
  ClearDepth(
    gl_VkCurrentViewport.x, gl_VkCurrentViewport.y,
    gl_VkCurrentViewport.width, gl_VkCurrentViewport.height, depth);
}
#endif