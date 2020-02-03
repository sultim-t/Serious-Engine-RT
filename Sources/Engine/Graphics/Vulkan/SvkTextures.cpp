#include "stdh.h"
#include <Engine/Graphics/GfxLibrary.h>

#ifdef SE1_VULKAN

void CGfxLibrary::CreateTexturesDataStructure()
{
  ASSERT(gl_VkTextures.Count() == 0);

  gl_VkTextures.SetAllocationStep(1024);
  
  // push one so zero index will not be used
  gl_VkTextures.Push();
}

void CGfxLibrary::DestroyTexturesDataStructure()
{
  // TODO: Vulkan: texture has table
  for (uint32_t i = 1; i < gl_VkTextures.Count(); i++)
  {
    DeleteTexture(i);
  }

  gl_VkTextures.Clear();
}

void CGfxLibrary::SetTextureParams(uint32_t textureUnit, uint32_t textureId, SvkSamplerFlags samplerFlags)
{
  ASSERT(textureUnit >= 0 && textureUnit < GFX_MAXTEXUNITS);

  // set new sampler
  // TODO: hash table
  gl_VkTextures[textureId].sto_Sampler = GetSampler(samplerFlags);

  gl_VkActiveTextures[textureUnit].sat_IsActivated = true;
  gl_VkActiveTextures[textureUnit].sat_TextureID = textureId;
}

VkDescriptorSet CGfxLibrary::GetTextureDescriptor(uint32_t textureId)
{
  const SvkTextureObject &sto = gl_VkTextures[textureId];

  VkDescriptorSetAllocateInfo setAllocInfo = {};
  setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  setAllocInfo.pNext = nullptr;
  setAllocInfo.descriptorPool = gl_VkDescriptorPool;
  setAllocInfo.descriptorSetCount = 1;
  setAllocInfo.pSetLayouts = &gl_VkDescSetLayoutTexture;

  VkDescriptorSet descSet;
  VkResult r = vkAllocateDescriptorSets(gl_VkDevice, &setAllocInfo, &descSet);
  VK_CHECKERROR(r);

  VkDescriptorImageInfo imageInfo = {};
  imageInfo.imageLayout = sto.sto_Layout;
  imageInfo.imageView = sto.sto_ImageView;
  imageInfo.sampler = sto.sto_Sampler;

  VkWriteDescriptorSet write = {};
  write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  write.dstSet = descSet;
  write.dstBinding = 0;
  write.descriptorCount = 1;
  write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
  write.pImageInfo = &imageInfo;

  vkUpdateDescriptorSets(gl_VkDevice, 1, &write, 0, nullptr);

  return descSet;
}

uint32_t CGfxLibrary::CreateTexture()
{
  // texture IDs start with 1, not 0
  uint32_t textureId = 0;

  // for now, just return index of stack top
  textureId = gl_VkTextures.Count();
  SvkTextureObject &sto = gl_VkTextures.Push();
  sto.Reset();

  ASSERT(textureId != 0);
  return textureId;
}

void CGfxLibrary::InitTexture32Bit(
  uint32_t textureId, VkFormat format, void *textureData,
  VkExtent2D *mipLevels, uint32_t mipLevelsCount)
{
  const uint32_t PixelSize = 4;
  const uint32_t MaxMipLevelsCount = 32;

  VkResult r;
  VkMemoryRequirements memoryReq;
  SvkTextureObject &sto = gl_VkTextures[textureId];

  ASSERT(mipLevelsCount > 0 && mipLevelsCount < MaxMipLevelsCount);
  ASSERT(sto.sto_Image == VK_NULL_HANDLE);
  ASSERT(sto.sto_ImageView == VK_NULL_HANDLE);
  ASSERT(sto.sto_Memory == VK_NULL_HANDLE);
  ASSERT(sto.sto_Format == VK_FORMAT_UNDEFINED);
  ASSERT(sto.sto_Layout == VK_IMAGE_LAYOUT_UNDEFINED);

  sto.sto_Format = format;
  sto.sto_Layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  sto.sto_Width = mipLevels[0].width;
  sto.sto_Height = mipLevels[0].height;

  // size of texture with all mipmaps
  uint32_t textureBufferSize = 0;
  for (uint32_t i = 0; i < mipLevelsCount; i++)
  {
    textureBufferSize += mipLevels[i].width * mipLevels[i].height * PixelSize;
  }

  // TODO: common staging memory
  // -----
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingMemory;

  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = textureBufferSize;
  bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  r = vkCreateBuffer(gl_VkDevice, &bufferInfo, nullptr, &stagingBuffer);
  VK_CHECKERROR(r);

  vkGetBufferMemoryRequirements(gl_VkDevice, stagingBuffer, &memoryReq);
  
  VkMemoryAllocateInfo stagingAllocInfo = {};
  stagingAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  stagingAllocInfo.allocationSize = memoryReq.size;
  stagingAllocInfo.memoryTypeIndex = GetMemoryTypeIndex(memoryReq.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  
  r = vkAllocateMemory(gl_VkDevice, &stagingAllocInfo, nullptr, &stagingMemory);
  VK_CHECKERROR(r);
  r = vkBindBufferMemory(gl_VkDevice, stagingBuffer, stagingMemory, 0);
  VK_CHECKERROR(r);

  void *mapped;
  r = vkMapMemory(gl_VkDevice, stagingMemory, 0, memoryReq.size, 0, &mapped);
  VK_CHECKERROR(r);

  memcpy(mapped, textureData, textureBufferSize);
  vkUnmapMemory(gl_VkDevice, stagingMemory);
  // -----



  // create image
  VkImageCreateInfo imageInfo = {};
  imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  imageInfo.imageType = VK_IMAGE_TYPE_2D;
  imageInfo.format = format;
  imageInfo.extent.width = mipLevels[0].width;
  imageInfo.extent.height = mipLevels[0].height;
  imageInfo.extent.depth = 1;
  imageInfo.mipLevels = mipLevelsCount;
  imageInfo.arrayLayers = 1;
  imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
  imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
  // for shaders and loading into
  imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  r = vkCreateImage(gl_VkDevice, &imageInfo, nullptr, &sto.sto_Image);
  VK_CHECKERROR(r);

  vkGetImageMemoryRequirements(gl_VkDevice, sto.sto_Image, &memoryReq);

  // allocate memory for image
  VkMemoryAllocateInfo imageAllocInfo = {};
  imageAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  imageAllocInfo.allocationSize = memoryReq.size;
  imageAllocInfo.memoryTypeIndex = GetMemoryTypeIndex(memoryReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  r = vkAllocateMemory(gl_VkDevice, &imageAllocInfo, nullptr, &sto.sto_Memory);
  VK_CHECKERROR(r);
  r = vkBindImageMemory(gl_VkDevice, sto.sto_Image, sto.sto_Memory, 0);
  VK_CHECKERROR(r);

  // prepare regions for copying
  VkBufferImageCopy bufferCopyRegions[MaxMipLevelsCount];
  memset(bufferCopyRegions, 0, mipLevelsCount * sizeof(VkBufferImageCopy));

  uint32_t regionOffset = 0;

  for (uint32_t i = 0; i < mipLevelsCount; i++)
  {
    VkBufferImageCopy &region = bufferCopyRegions[i];

    region.bufferOffset = regionOffset;
    region.imageExtent.width = mipLevels[i].width;
    region.imageExtent.height = mipLevels[i].height;
    region.imageExtent.depth = 1;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = i;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    regionOffset += mipLevels[i].width * mipLevels[i].height * PixelSize;
  }



  // TODO: common staging memory
  // -----
  VkCommandBufferAllocateInfo cmdInfo = {};
  cmdInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  cmdInfo.commandPool = gl_VkCmdPool;
  cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmdInfo.commandBufferCount = 1;

  VkCommandBuffer cmdBuffer;
  r = vkAllocateCommandBuffers(gl_VkDevice, &cmdInfo, &cmdBuffer);
  VK_CHECKERROR(r);

  VkCommandBufferBeginInfo cmdBeginInfo = {};
  cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  r = vkBeginCommandBuffer(cmdBuffer, &cmdBeginInfo);
  VK_CHECKERROR(r);
  // -----



  // layout transition
  VkImageMemoryBarrier barrier = {};
  barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  barrier.image = sto.sto_Image;
  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  barrier.subresourceRange.baseMipLevel = 0;
  barrier.subresourceRange.levelCount = mipLevelsCount;
  barrier.subresourceRange.baseArrayLayer = 0;
  barrier.subresourceRange.layerCount = 1;

  // prepare for transfer
  barrier.srcAccessMask = 0;
  barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;

  vkCmdPipelineBarrier(
    cmdBuffer,
    VK_PIPELINE_STAGE_HOST_BIT,
    VK_PIPELINE_STAGE_TRANSFER_BIT, // for copying
    0,
    0, nullptr,
    0, nullptr,
    1, &barrier);

  // copy with mipmaps
  vkCmdCopyBufferToImage(
    cmdBuffer,
    stagingBuffer,                        // source
    sto.sto_Image,                        // dest
    VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, // image layout
    mipLevelsCount,
    bufferCopyRegions);

  // prepare for reading in shaders
  barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
  barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
  barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
  barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

  vkCmdPipelineBarrier(
    cmdBuffer,
    VK_PIPELINE_STAGE_TRANSFER_BIT,
    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
    0,
    0, nullptr,
    0, nullptr,
    1, &barrier);



  // TODO: common staging memory
  // -----
  r = vkEndCommandBuffer(cmdBuffer);
  VK_CHECKERROR(r);

  VkSubmitInfo submitInfo = {};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.commandBufferCount = 1;
  submitInfo.pCommandBuffers = &cmdBuffer;

  r = vkQueueSubmit(gl_VkQueueGraphics, 1, &submitInfo, VK_NULL_HANDLE);
  VK_CHECKERROR(r);
  r = vkQueueWaitIdle(gl_VkQueueGraphics);
  VK_CHECKERROR(r);
  vkFreeCommandBuffers(gl_VkDevice, gl_VkCmdPool, 1, &cmdBuffer);

  vkFreeMemory(gl_VkDevice, stagingMemory, nullptr);
  vkDestroyBuffer(gl_VkDevice, stagingBuffer, nullptr);
  // -----



  VkImageViewCreateInfo viewInfo = {};
  viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  viewInfo.format = format;
  viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
  viewInfo.subresourceRange.baseMipLevel = 0;
  viewInfo.subresourceRange.levelCount = mipLevelsCount;
  viewInfo.subresourceRange.baseArrayLayer = 0;
  viewInfo.subresourceRange.layerCount = 1;
  viewInfo.image = sto.sto_Image;
  r = vkCreateImageView(gl_VkDevice, &viewInfo, nullptr, &sto.sto_ImageView);
  VK_CHECKERROR(r);
}

void CGfxLibrary::DeleteTexture(uint32_t textureId)
{
  ASSERT(textureId != 0);

  // TODO: Vulkan: hash table for textures

  uint32_t count = gl_VkTextures.Count();

  if (textureId >= count)
  {
    // silently ignore (like OpenGL)
    return;
  }

  SvkTextureObject &sto = gl_VkTextures[textureId];

  // destroy everything except sampler, as texture object doesn't own it
  vkDestroyImage(gl_VkDevice, sto.sto_Image, nullptr);
  vkDestroyImageView(gl_VkDevice, sto.sto_ImageView, nullptr);
  vkFreeMemory(gl_VkDevice, sto.sto_Memory, nullptr);
  
  sto.Reset();
}

#endif