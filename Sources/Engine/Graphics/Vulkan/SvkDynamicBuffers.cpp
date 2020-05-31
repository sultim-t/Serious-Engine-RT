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
#include <Engine/Graphics/Vulkan/SvkMain.h>

#ifdef SE1_VULKAN

void SvkMain::InitDynamicBuffers()
{
  VmaAllocatorCreateInfo allocatorInfo = {};
  allocatorInfo.physicalDevice = gl_VkPhysDevice;
  allocatorInfo.device = gl_VkDevice;
  allocatorInfo.instance = gl_VkInstance;

  vmaCreateAllocator(&allocatorInfo, &gl_VkVmaAllocator);

  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    gl_VkVmaToDelete[i] = new CStaticStackArray<SvkDBufferToDelete>();
    gl_VkVmaToDelete[i]->New(4096);
    gl_VkVmaToDelete[i]->SetAllocationStep(2048);
  }
}

void SvkMain::GetVertexBuffer(uint32_t size, SvkDynamicBuffer &outDynBuffer)
{
  GetVIBuffer(size, outDynBuffer, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

void SvkMain::GetIndexBuffer(uint32_t size, SvkDynamicBuffer &outDynBuffer)
{
  GetVIBuffer(size, outDynBuffer, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

void SvkMain::GetVIBuffer(uint32_t size, SvkDynamicBuffer &outDynBuffer, VkBufferUsageFlags usage)
{
  VmaAllocationCreateInfo allocInfo = {};
  allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
  allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

  VkBufferCreateInfo bufferInfo = {};
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = size;
  bufferInfo.usage = usage;

  VkBuffer buffer;
  VmaAllocation allocation;
  VmaAllocationInfo allocationInfo;
  vmaCreateBuffer(gl_VkVmaAllocator, &bufferInfo, &allocInfo, &buffer, &allocation, &allocationInfo);

  auto &toAdd = gl_VkVmaToDelete[gl_VkCmdBufferCurrent]->Push();
  toAdd.sdd_Allocation = allocation;
  toAdd.sdd_Buffer = buffer;

  outDynBuffer.sdb_Buffer = buffer;
  outDynBuffer.sdb_CurrentOffset = 0;
  outDynBuffer.sdb_Data = (UBYTE *)allocationInfo.pMappedData;
}

// using VMA, this function deletes all allocations that were created in cmdBufferIndex
void SvkMain::FreeUnusedDynamicBuffers(uint32_t cmdBufferIndex)
{
  auto &toDelete = *gl_VkVmaToDelete[cmdBufferIndex];

  for (uint32_t i = 0; i < toDelete.Count(); i++)
  {
    vmaDestroyBuffer(gl_VkVmaAllocator, toDelete[i].sdd_Buffer, toDelete[i].sdd_Allocation);
  }

  toDelete.PopAll();
}

void SvkMain::DestroyDynamicBuffers()
{
  for (uint32_t i = 0; i < gl_VkMaxCmdBufferCount; i++)
  {
    FreeUnusedDynamicBuffers(i);
    gl_VkVmaToDelete[i]->Delete();

    delete gl_VkVmaToDelete[i];
    gl_VkVmaToDelete[i] = nullptr;
  }

  vmaDestroyAllocator(gl_VkVmaAllocator);
}

#endif