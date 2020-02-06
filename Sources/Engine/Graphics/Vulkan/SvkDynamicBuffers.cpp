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

#include "StdH.h"
#include <Engine/Graphics/GfxLibrary.h>

#ifdef SE1_VULKAN

void CGfxLibrary::InitDynamicBuffers()
{
#ifndef NDEBUG
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    ASSERT(gl_VkDynamicVBGlobal.sdg_CurrentDynamicBufferSize == 0);
    ASSERT(gl_VkDynamicIBGlobal.sdg_CurrentDynamicBufferSize == 0);
    ASSERT(gl_VkDynamicUBGlobal.sdg_CurrentDynamicBufferSize == 0);
  }
#endif // !NDEBUG

  InitDynamicVertexBuffers(SVK_DYNAMIC_VERTEX_BUFFER_START_SIZE);
  InitDynamicIndexBuffers(SVK_DYNAMIC_INDEX_BUFFER_START_SIZE);
  InitDynamicUniformBuffers(SVK_DYNAMIC_UNIFORM_BUFFER_START_SIZE);
}

void CGfxLibrary::InitDynamicVertexBuffers(uint32_t newSize)
{
  // ASSERT(newSize < gl_VkPhProperties.limits.);

  gl_VkDynamicVBGlobal.sdg_CurrentDynamicBufferSize = newSize;
  InitDynamicBuffer(gl_VkDynamicVBGlobal, gl_VkDynamicVB, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

#ifndef NDEBUG
  CPrintF("SVK: Allocated dynamic vertex buffer: %u MB.\n", gl_VkDynamicVBGlobal.sdg_CurrentDynamicBufferSize / 1024 / 1024);
#endif // !NDEBUG
}

void CGfxLibrary::InitDynamicIndexBuffers(uint32_t newSize)
{
  // ASSERT(newSize < gl_VkPhProperties.limits.);

  gl_VkDynamicIBGlobal.sdg_CurrentDynamicBufferSize = newSize;
  InitDynamicBuffer(gl_VkDynamicIBGlobal, gl_VkDynamicIB, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);

#ifndef NDEBUG
  CPrintF("SVK: Allocated dynamic index buffer: %u MB.\n", gl_VkDynamicIBGlobal.sdg_CurrentDynamicBufferSize / 1024 / 1024);
#endif // !NDEBUG
}

void CGfxLibrary::InitDynamicUniformBuffers(uint32_t newSize)
{
  // ASSERT(newSize < gl_VkPhProperties.limits.);

  SvkDynamicBuffer uniformDynBuffers[gl_VkMaxCmdBufferCount];

  gl_VkDynamicUBGlobal.sdg_CurrentDynamicBufferSize = newSize;
  InitDynamicBuffer(gl_VkDynamicUBGlobal, uniformDynBuffers, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);

#ifndef NDEBUG
  CPrintF("SVK: Allocated dynamic uniform buffer: %u KB.\n", gl_VkDynamicUBGlobal.sdg_CurrentDynamicBufferSize / 1024);
#endif // !NDEBUG

  // allocate descriptor sets for uniform buffers
  VkDescriptorSetAllocateInfo descAllocInfo = {};
  descAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  descAllocInfo.descriptorPool = gl_VkDescriptorPool;
  descAllocInfo.descriptorSetCount = 1;
  descAllocInfo.pSetLayouts = &gl_VkDescriptorSetLayout;

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    VkDescriptorSet descSet;
    vkAllocateDescriptorSets(gl_VkDevice, &descAllocInfo, &descSet);
  
    VkDescriptorBufferInfo bufferInfo = {};
    bufferInfo.buffer = uniformDynBuffers[i].sdb_Buffer;
    // start in uniform buffer is 0
    bufferInfo.offset = 0;
    // max size for uniform data
    bufferInfo.range = SVK_DYNAMIC_UNIFORM_MAX_ALLOC_SIZE;

    // update descriptor set
    VkWriteDescriptorSet write = {};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = descSet;
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    write.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(gl_VkDevice, 1, &write, 0, nullptr);

    gl_VkDynamicUB[i].sdb_Buffer = uniformDynBuffers[i].sdb_Buffer;
    gl_VkDynamicUB[i].sdb_CurrentOffset = uniformDynBuffers[i].sdb_CurrentOffset;
    gl_VkDynamicUB[i].sdb_Data = uniformDynBuffers[i].sdb_Data;
    gl_VkDynamicUB[i].sdu_DescriptorSet = descSet;
  }
}

void CGfxLibrary::InitDynamicBuffer(SvkDynamicBufferGlobal &dynBufferGlobal, SvkDynamicBuffer *buffers, VkBufferUsageFlags usage)
{
  VkResult r;

  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = dynBufferGlobal.sdg_CurrentDynamicBufferSize;
  bufferInfo.usage = usage;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    buffers[i].sdb_CurrentOffset = 0;

    r = vkCreateBuffer(gl_VkDevice, &bufferInfo, nullptr, &buffers[i].sdb_Buffer);
    VK_CHECKERROR(r);
  }

  VkMemoryRequirements memReqs;
  vkGetBufferMemoryRequirements(gl_VkDevice, buffers[0].sdb_Buffer, &memReqs);

  uint32_t mod = memReqs.size % memReqs.alignment;
  uint32_t alignedSize = mod == 0 ?
    memReqs.size :
    memReqs.size + memReqs.alignment - mod;

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = alignedSize * gl_VkMaxCmdBufferCount;
  // VK_MEMORY_PROPERTY_HOST_COHERENT_BIT is not used, as buffers will be flushed maually
  allocInfo.memoryTypeIndex = GetMemoryTypeIndex(memReqs.memoryTypeBits,
      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, VK_MEMORY_PROPERTY_HOST_CACHED_BIT);

  r = vkAllocateMemory(gl_VkDevice, &allocInfo, nullptr, &dynBufferGlobal.sdg_DynamicBufferMemory);
  VK_CHECKERROR(r);

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    uint32_t offset = i * alignedSize;
    r = vkBindBufferMemory(gl_VkDevice, buffers[i].sdb_Buffer, dynBufferGlobal.sdg_DynamicBufferMemory, offset);
    VK_CHECKERROR(r);
  }

  void *data;
  r = vkMapMemory(gl_VkDevice, dynBufferGlobal.sdg_DynamicBufferMemory,
    0, alignedSize * gl_VkMaxCmdBufferCount, 0, &data);

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    uint32_t offset = i * alignedSize;
    buffers[i].sdb_Data = (UBYTE *)data + offset;
  }
}

void CGfxLibrary::AddDynamicBufferToDeletion(SvkDynamicBufferGlobal &dynBufferGlobal, SvkDynamicBuffer *buffers)
{
  auto &toDelete = gl_VkDynamicToDelete[gl_VkCmdBufferCurrent].Push();
  toDelete.sdd_Memory = dynBufferGlobal.sdg_DynamicBufferMemory;
  
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    toDelete.sdd_Buffers[i] = buffers[i].sdb_Buffer;
    toDelete.sdd_DescriptorSets[i] = VK_NULL_HANDLE;
  }
}

void CGfxLibrary::AddDynamicUniformToDeletion(SvkDynamicBufferGlobal &dynBufferGlobal, SvkDynamicUniform *buffers)
{
  auto &toDelete = gl_VkDynamicToDelete[gl_VkCmdBufferCurrent].Push();
  toDelete.sdd_Memory = dynBufferGlobal.sdg_DynamicBufferMemory;
 
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    toDelete.sdd_Buffers[i] = buffers[i].sdb_Buffer;
    toDelete.sdd_DescriptorSets[i] = buffers[i].sdu_DescriptorSet;
  }
}

void CGfxLibrary::ClearCurrentDynamicOffsets(uint32_t cmdBufferIndex)
{
  gl_VkDynamicVB[cmdBufferIndex].sdb_CurrentOffset = 0;
  gl_VkDynamicIB[cmdBufferIndex].sdb_CurrentOffset = 0;
  gl_VkDynamicUB[cmdBufferIndex].sdb_CurrentOffset = 0;
}

void CGfxLibrary::GetVertexBuffer(uint32_t size, SvkDynamicBuffer &outDynBuffer)
{
  SvkDynamicBuffer &commonBuffer = gl_VkDynamicVB[gl_VkCmdBufferCurrent];
  SvkDynamicBufferGlobal &dynBufferGlobal = gl_VkDynamicVBGlobal;

  // if not enough
  if (commonBuffer.sdb_CurrentOffset + size > dynBufferGlobal.sdg_CurrentDynamicBufferSize)
  {
    AddDynamicBufferToDeletion(dynBufferGlobal, gl_VkDynamicVB);

    vkUnmapMemory(gl_VkDevice, dynBufferGlobal.sdg_DynamicBufferMemory);

    uint32_t newSize = dynBufferGlobal.sdg_CurrentDynamicBufferSize + 2 * SVK_DYNAMIC_VERTEX_BUFFER_START_SIZE;
    InitDynamicVertexBuffers(newSize);
  }

  outDynBuffer.sdb_Buffer = commonBuffer.sdb_Buffer;
  outDynBuffer.sdb_CurrentOffset = commonBuffer.sdb_CurrentOffset;
  outDynBuffer.sdb_Data = (UBYTE *)commonBuffer.sdb_Data + commonBuffer.sdb_CurrentOffset;

  commonBuffer.sdb_CurrentOffset += size;
}

void CGfxLibrary::GetIndexBuffer(uint32_t size, SvkDynamicBuffer &outDynBuffer)
{
  SvkDynamicBuffer &commonBuffer = gl_VkDynamicIB[gl_VkCmdBufferCurrent];
  SvkDynamicBufferGlobal &dynBufferGlobal = gl_VkDynamicIBGlobal;

  // if not enough
  if (commonBuffer.sdb_CurrentOffset + size > dynBufferGlobal.sdg_CurrentDynamicBufferSize)
  {
    AddDynamicBufferToDeletion(dynBufferGlobal, gl_VkDynamicIB);

    vkUnmapMemory(gl_VkDevice, dynBufferGlobal.sdg_DynamicBufferMemory);

    uint32_t newSize = dynBufferGlobal.sdg_CurrentDynamicBufferSize + SVK_DYNAMIC_INDEX_BUFFER_START_SIZE;
    InitDynamicIndexBuffers(newSize);
  }

  outDynBuffer.sdb_Buffer = commonBuffer.sdb_Buffer;
  outDynBuffer.sdb_CurrentOffset = commonBuffer.sdb_CurrentOffset;
  outDynBuffer.sdb_Data = (UBYTE *)commonBuffer.sdb_Data + commonBuffer.sdb_CurrentOffset;

  commonBuffer.sdb_CurrentOffset += size;
}

void CGfxLibrary::GetUniformBuffer(uint32_t size, SvkDynamicUniform &outDynUniform)
{
  // size must be aligned by min uniform offset alignment
  uint32_t alignment = gl_VkPhProperties.limits.minUniformBufferOffsetAlignment;
  uint32_t mod = size % alignment;
  uint32_t alignedSize = mod == 0 ? size : size + alignment - mod;
  
  ASSERTMSG(alignedSize <= SVK_DYNAMIC_UNIFORM_MAX_ALLOC_SIZE, "Vulkan: Uniform max size is too small");

  SvkDynamicUniform &commonBuffer = gl_VkDynamicUB[gl_VkCmdBufferCurrent];
  SvkDynamicBufferGlobal &dynBufferGlobal = gl_VkDynamicUBGlobal;

  // if not enough
  if (commonBuffer.sdb_CurrentOffset + SVK_DYNAMIC_UNIFORM_MAX_ALLOC_SIZE > dynBufferGlobal.sdg_CurrentDynamicBufferSize)
  {
    AddDynamicUniformToDeletion(dynBufferGlobal, gl_VkDynamicUB);

    vkUnmapMemory(gl_VkDevice, dynBufferGlobal.sdg_DynamicBufferMemory);

    uint32_t newSize = dynBufferGlobal.sdg_CurrentDynamicBufferSize + SVK_DYNAMIC_UNIFORM_BUFFER_START_SIZE;
    InitDynamicUniformBuffers(newSize);
  }

  outDynUniform.sdb_Buffer = commonBuffer.sdb_Buffer;
  outDynUniform.sdb_CurrentOffset = commonBuffer.sdb_CurrentOffset;
  outDynUniform.sdb_Data = (UBYTE *)commonBuffer.sdb_Data + commonBuffer.sdb_CurrentOffset;
  
  outDynUniform.sdu_DescriptorSet = commonBuffer.sdu_DescriptorSet;

  commonBuffer.sdb_CurrentOffset += alignedSize;
}

void CGfxLibrary::FlushDynamicBuffersMemory()
{
  VkMappedMemoryRange ranges[3];
  memset(&ranges, 0, sizeof(ranges));

  ranges[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  ranges[0].memory = gl_VkDynamicVBGlobal.sdg_DynamicBufferMemory;
  ranges[0].size = VK_WHOLE_SIZE;

  ranges[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  ranges[1].memory = gl_VkDynamicIBGlobal.sdg_DynamicBufferMemory;
  ranges[1].size = VK_WHOLE_SIZE;

  ranges[2].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
  ranges[2].memory = gl_VkDynamicUBGlobal.sdg_DynamicBufferMemory;
  ranges[2].size = VK_WHOLE_SIZE;

  vkFlushMappedMemoryRanges(gl_VkDevice, 3, ranges);
}

void CGfxLibrary::FreeUnusedDynamicBuffers(uint32_t cmdBufferIndex)
{
  VkResult r;
  auto &toDelete = gl_VkDynamicToDelete[cmdBufferIndex];

  for (uint32_t i = 0; i < toDelete.Count(); i++)
  {
    for (uint32_t j = 0; j < gl_VkMaxCmdBufferCount; j++)
    {
      vkDestroyBuffer(gl_VkDevice, toDelete[i].sdd_Buffers[j], nullptr);

      if (toDelete[i].sdd_DescriptorSets[j] != VK_NULL_HANDLE)
      {
        r = vkFreeDescriptorSets(gl_VkDevice, gl_VkDescriptorPool, 1, &toDelete[i].sdd_DescriptorSets[j]);
        VK_CHECKERROR(r);
      }
    }

    vkFreeMemory(gl_VkDevice, toDelete[i].sdd_Memory, nullptr);
  }

  toDelete.PopAll();
}

void CGfxLibrary::DestroyDynamicBuffers()
{
  ASSERT(gl_VkDynamicVBGlobal.sdg_DynamicBufferMemory != VK_NULL_HANDLE);
  ASSERT(gl_VkDynamicIBGlobal.sdg_DynamicBufferMemory != VK_NULL_HANDLE);
  ASSERT(gl_VkDynamicUBGlobal.sdg_DynamicBufferMemory != VK_NULL_HANDLE);

  VkResult r;

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    FreeUnusedDynamicBuffers(i);

    // delete array
    gl_VkDynamicToDelete[i].Clear();

    vkDestroyBuffer(gl_VkDevice, gl_VkDynamicVB[i].sdb_Buffer, nullptr);
    vkDestroyBuffer(gl_VkDevice, gl_VkDynamicIB[i].sdb_Buffer, nullptr);
    vkDestroyBuffer(gl_VkDevice, gl_VkDynamicUB[i].sdb_Buffer, nullptr);
    
    gl_VkDynamicVB[i].sdb_Buffer = VK_NULL_HANDLE;
    gl_VkDynamicIB[i].sdb_Buffer = VK_NULL_HANDLE;
    gl_VkDynamicUB[i].sdb_Buffer = VK_NULL_HANDLE;

    gl_VkDynamicVB[i].sdb_CurrentOffset = 0;
    gl_VkDynamicIB[i].sdb_CurrentOffset = 0;
    gl_VkDynamicUB[i].sdb_CurrentOffset = 0;

    gl_VkDynamicVB[i].sdb_Data = nullptr;
    gl_VkDynamicIB[i].sdb_Data = nullptr;
    gl_VkDynamicUB[i].sdb_Data = nullptr;

    r = vkFreeDescriptorSets(gl_VkDevice, gl_VkDescriptorPool, 1, &gl_VkDynamicUB[i].sdu_DescriptorSet);
    VK_CHECKERROR(r);
    gl_VkDynamicUB[i].sdu_DescriptorSet = VK_NULL_HANDLE;
  }

  vkUnmapMemory(gl_VkDevice, gl_VkDynamicVBGlobal.sdg_DynamicBufferMemory);
  vkUnmapMemory(gl_VkDevice, gl_VkDynamicIBGlobal.sdg_DynamicBufferMemory);
  vkUnmapMemory(gl_VkDevice, gl_VkDynamicUBGlobal.sdg_DynamicBufferMemory);

  vkFreeMemory(gl_VkDevice, gl_VkDynamicVBGlobal.sdg_DynamicBufferMemory, nullptr);
  vkFreeMemory(gl_VkDevice, gl_VkDynamicIBGlobal.sdg_DynamicBufferMemory, nullptr);
  vkFreeMemory(gl_VkDevice, gl_VkDynamicUBGlobal.sdg_DynamicBufferMemory, nullptr);

  gl_VkDynamicVBGlobal.sdg_DynamicBufferMemory = VK_NULL_HANDLE;
  gl_VkDynamicIBGlobal.sdg_DynamicBufferMemory = VK_NULL_HANDLE;
  gl_VkDynamicUBGlobal.sdg_DynamicBufferMemory = VK_NULL_HANDLE;

  gl_VkDynamicVBGlobal.sdg_CurrentDynamicBufferSize = 0;
  gl_VkDynamicIBGlobal.sdg_CurrentDynamicBufferSize = 0;
  gl_VkDynamicUBGlobal.sdg_CurrentDynamicBufferSize = 0;
}

#endif