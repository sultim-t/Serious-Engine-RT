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
#include <Engine/Graphics/GfxLibrary.h>
#include <Engine/Graphics/Vulkan/SvkMain.h>

#ifdef SE1_VULKAN

void SvkMain::CreateTexturesDataStructure()
{
  ASSERT(!gl_VkTextures.IsAllocated());

  // make hash table tall to reduce linear searches
  gl_VkTextures.New(512, 8);
  gl_VkLastTextureId = 0;

  // average texture size with mipmaps in bytes
  const uint32_t AvgTextureSize = 256 * 256 * 4 * 4 / 3;
  gl_VkImageMemPool = new SvkMemoryPool(gl_VkDevice, AvgTextureSize * 512);

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    gl_VkTexturesToDelete[i] = new CStaticStackArray<SvkTextureObject>();
    gl_VkTexturesToDelete[i]->SetAllocationStep(2048);
  }
}

void SvkMain::DestroyTexturesDataStructure()
{
  delete gl_VkImageMemPool;

  // destroy all texture objects; memory handles will be ignored
  // as image memory pool is freed already
  gl_VkTextures.Map(DestroyTextureObject);

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    gl_VkTexturesToDelete[i]->Clear();
  }

  gl_VkTextures.Clear();
  gl_VkLastTextureId = 0;
}

void SvkMain::SetTexture(uint32_t textureUnit, uint32_t textureId, SvkSamplerFlags samplerFlags)
{
  ASSERT(textureUnit >= 0 && textureUnit < GFX_MAXTEXUNITS);


  SvkTextureObject *psto = gl_VkTextures.TryGet(textureId);
  if (psto == nullptr)
  {
    return;
  }

  gl_VkActiveTextures[textureUnit] = textureId;
  psto->sto_SamplerFlags = samplerFlags;
}

VkDescriptorSet SvkMain::GetTextureDescriptor(uint32_t textureId)
{
  SvkTextureObject *psto = gl_VkTextures.TryGet(textureId);
  if (psto == nullptr)
  {
    return VK_NULL_HANDLE;
  }

  SvkTextureObject &sto = *psto;

  // if wasn't uploaded
  if (sto.sto_Image == VK_NULL_HANDLE)
  {
    return VK_NULL_HANDLE;
  }

  VkDescriptorSet descSet;

  // allocate new
  VkDescriptorSetAllocateInfo setAllocInfo = {};
  setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  setAllocInfo.pNext = nullptr;
  setAllocInfo.descriptorPool = gl_VkTextureDescPools[gl_VkCmdBufferCurrent];
  setAllocInfo.descriptorSetCount = 1;
  setAllocInfo.pSetLayouts = &gl_VkDescSetLayoutTexture;

  VkResult r = vkAllocateDescriptorSets(gl_VkDevice, &setAllocInfo, &descSet);
  VK_CHECKERROR(r);

  // prepare info for desc set update
  VkDescriptorImageInfo imageInfo = {};
  imageInfo.imageLayout = sto.sto_Layout;
  imageInfo.imageView = sto.sto_ImageView;
  imageInfo.sampler = GetSampler(sto.sto_SamplerFlags);

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

void SvkMain::AddTextureToDeletion(uint32_t textureId)
{
  SvkTextureObject *sto = gl_VkTextures.TryGet(textureId);
  if (sto != nullptr)
  {
    gl_VkTexturesToDelete[gl_VkCmdBufferCurrent]->Push() = *sto;
    gl_VkTextures.Delete(textureId);
  }
}

uint32_t SvkMain::GetTexturePixCount(uint32_t textureId)
{
  SvkTextureObject *psto = gl_VkTextures.TryGet(textureId);
  if (psto == nullptr)
  {
    return 0;
  }

  return psto->sto_Width * psto->sto_Height;
}

void SvkMain::FreeDeletedTextures(uint32_t cmdBufferIndex)
{
  auto &toDelete = *(gl_VkTexturesToDelete[cmdBufferIndex]);

  for (INDEX i = 0; i < toDelete.Count(); i++)
  {
    SvkTextureObject &sto = toDelete[i];

    // only if was uploaded
    if (sto.sto_Image != VK_NULL_HANDLE)
    {
      // free image memory from pool
      gl_VkImageMemPool->Free(sto.sto_MemoryHandle);

      // free image, image view and desc set, if exist
      DestroyTextureObject(sto);
    }
  }

  toDelete.PopAll();
}

void SvkMain::DestroyTextureObject(SvkTextureObject &sto)
{  
  // if was uploaded
  if (sto.sto_Image != VK_NULL_HANDLE)
  {
    vkDestroyImage(sto.sto_VkDevice, sto.sto_Image, nullptr);
    vkDestroyImageView(sto.sto_VkDevice, sto.sto_ImageView, nullptr);
  }

  sto.Reset();
}

uint32_t SvkMain::CreateTexture()
{
  uint32_t textureId = gl_VkLastTextureId++;
  return CreateTexture(textureId);
}

uint32_t SvkMain::CreateTexture(uint32_t textureId)
{
  SvkTextureObject sto = {};
  sto.sto_VkDevice = gl_VkDevice;
  gl_VkTextures.Add(textureId, sto);

  return textureId;
}

void SvkMain::InitTexture32Bit(
  uint32_t &textureId, VkFormat format, void *textureData,
  VkExtent2D *mipLevels, uint32_t mipLevelsCount, bool onlyUpdate)
{
  const uint32_t PixelSize = 4;
  const uint32_t MaxMipLevelsCount = 32;
 
  VkResult r;

  SvkTextureObject *psto = gl_VkTextures.TryGet(textureId);
  ASSERT(psto != nullptr);

  SvkTextureObject &sto = *psto;

  ASSERT(mipLevelsCount > 0 && mipLevelsCount < MaxMipLevelsCount);

  if (!onlyUpdate && sto.sto_Image != VK_NULL_HANDLE)
  {
    onlyUpdate = true;
  }

  // if texture is already initialized, it can be only updated
  ASSERT(sto.sto_Image == VK_NULL_HANDLE || (sto.sto_Image != VK_NULL_HANDLE && onlyUpdate));
 
  if (onlyUpdate && (mipLevels[0].width != sto.sto_Width || mipLevels[0].height != sto.sto_Height))
  {
    // safely delete and create new with the same id
    AddTextureToDeletion(textureId);

    CreateTexture(textureId);

    sto = gl_VkTextures.Get(textureId);
  }

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

  VkMemoryRequirements stagingMemoryReq;
  vkGetBufferMemoryRequirements(gl_VkDevice, stagingBuffer, &stagingMemoryReq);
  
  VkMemoryAllocateInfo stagingAllocInfo = {};
  stagingAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  stagingAllocInfo.allocationSize = stagingMemoryReq.size;
  stagingAllocInfo.memoryTypeIndex = GetMemoryTypeIndex(
    stagingMemoryReq.memoryTypeBits, 
    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
  
  r = vkAllocateMemory(gl_VkDevice, &stagingAllocInfo, nullptr, &stagingMemory);
  VK_CHECKERROR(r);
  r = vkBindBufferMemory(gl_VkDevice, stagingBuffer, stagingMemory, 0);
  VK_CHECKERROR(r);

  void *mapped;
  r = vkMapMemory(gl_VkDevice, stagingMemory, 0, stagingMemoryReq.size, 0, &mapped);
  VK_CHECKERROR(r);

  memcpy(mapped, textureData, textureBufferSize);
  vkUnmapMemory(gl_VkDevice, stagingMemory);
  // -----


  if (!onlyUpdate)
  {
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

    VkMemoryRequirements imageMemoryReq;
    vkGetImageMemoryRequirements(gl_VkDevice, sto.sto_Image, &imageMemoryReq);

    // allocate memory for image
    VkMemoryAllocateInfo imageAllocInfo = {};
    imageAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    imageAllocInfo.allocationSize = imageMemoryReq.size % imageMemoryReq.alignment == 0 ?
      imageMemoryReq.size : imageMemoryReq.size + imageMemoryReq.alignment - imageMemoryReq.size % imageMemoryReq.alignment;
    imageAllocInfo.memoryTypeIndex = GetMemoryTypeIndex(imageMemoryReq.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    uint32_t imageMemoryOffset;
    sto.sto_MemoryHandle = gl_VkImageMemPool->Allocate(imageAllocInfo, imageMemoryReq, sto.sto_Memory, imageMemoryOffset);
    VK_CHECKERROR(r);
    r = vkBindImageMemory(gl_VkDevice, sto.sto_Image, sto.sto_Memory, imageMemoryOffset);
    VK_CHECKERROR(r);
  }

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
  cmdInfo.commandPool = gl_VkCmdPools[gl_VkCmdBufferCurrent];
  cmdInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  cmdInfo.commandBufferCount = 1;

  VkCommandBuffer cmdBuffer;
  r = vkAllocateCommandBuffers(gl_VkDevice, &cmdInfo, &cmdBuffer);
  VK_CHECKERROR(r);

  VkCommandBufferBeginInfo cmdBeginInfo = {};
  cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  cmdBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
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

  ASSERT(sto.sto_Image != VK_NULL_HANDLE);
}

#endif