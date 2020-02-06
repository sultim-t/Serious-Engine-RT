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

#ifndef SE_INCL_SVKSTATICMEMORYPOOL_H
#define SE_INCL_SVKSTATICMEMORYPOOL_H
#ifdef PRAGMA_ONCE
#pragma once
#endif
#ifdef SE1_VULKAN

#include <Engine/Graphics/Vulkan/VulkanInclude.h>
#include <Engine/Templates/StaticStackArray.cpp>

class SvkMemoryPool
{
private:
  struct FreeListNode
  {
    // index of the next node; -1 if none
    int32_t       nextNodeIndex;
    uint32_t      blockIndex;
    uint32_t      blockCount;

    FreeListNode()
    {
      nextNodeIndex = -1;
      blockIndex = blockCount = 0;
    }
  };

  struct AllocHandle
  {
    uint32_t      id;
    uint32_t      blockIndex;
    uint32_t      blockCount;

    AllocHandle()
    {
      id = blockIndex = blockCount = 0;
    }
  };

private:
  VkDevice        smp_VkDevice;
  VkDeviceMemory  smp_VkMemory;
  uint32_t        smp_VkMemoryTypeIndex;

  // next pool, if this became full
  SvkMemoryPool   *smp_pNext;
  
  // all nodes
  CStaticArray<FreeListNode> smp_Nodes;
  // head of the free ranges list; -1 if none
  int32_t         smp_FreeListHeadIndex;
  int32_t         smp_NodeLastIndex;
  CStaticStackArray<int32_t> smp_RemovedIndices;

  // handles for freeing
  CStaticStackArray<AllocHandle> smp_Handles;
  uint32_t        smp_HandleLastIndex;

  uint32_t        smp_PreferredSize;
  // overall block count
  uint32_t        smp_BlockCount;
  // size of one block in bytes, aligned
  uint32_t        smp_BlockSize;
  // current allocation count
  uint32_t        smp_AllocationCount;

private:
  void Init(uint32_t memoryTypeIndex, uint32_t alignment);
  int32_t AddNode();
  void RemoveNode(int32_t removed);

public:
  SvkMemoryPool(VkDevice device, uint32_t preferredSize);
  ~SvkMemoryPool();

  // Allocate memory with specified size, out params are memory and offset in it.
  // Returns handle which must be used to free memory.
  uint32_t Allocate(VkMemoryAllocateInfo allocInfo, VkMemoryRequirements memReqs, VkDeviceMemory &outMemory, uint32_t &outOffset);

  // Free allocated memory. Handle is a number that was returned in Allocate.
  void Free(uint32_t handle);
};

#endif // SE1_VULKAN
#endif