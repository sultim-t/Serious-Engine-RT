#include "stdh.h"
#include <Engine/Graphics/GfxLibrary.h>

#ifdef SE1_VULKAN

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

uint32_t CGfxLibrary::GetMemoryTypeIndex(uint32_t memoryTypeBits, VkFlags requirementsMask)
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

uint32_t CGfxLibrary::GetMemoryTypeIndex(uint32_t memoryTypeBits, VkFlags requirementsMask, VkFlags preferredMask)
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


void CGfxLibrary::CreateSyncPrimitives()
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
#endif