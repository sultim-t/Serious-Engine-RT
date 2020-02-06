#include "StdH.h"
#include "SvkMemoryPool.h"

SvkMemoryPool::SvkMemoryPool(VkDevice device, uint32_t preferredBlockSize, uint32_t blockCount)
{
  ASSERT(smp_VkDevice == VK_NULL_HANDLE);
  ASSERT(smp_VkMemory == VK_NULL_HANDLE);

  smp_VkDevice = device;
  smp_BlockCount = blockCount;
  smp_BlockSize = preferredBlockSize;

  smp_VkMemory = VK_NULL_HANDLE;
  smp_pNext = nullptr;
  smp_FreeListHeadIndex = 0;
  smp_AllocationCount = 0;
  smp_VkMemoryTypeIndex = 0;
  smp_HandleLastIndex = 0;
}

SvkMemoryPool::~SvkMemoryPool()
{
  ASSERT(smp_VkDevice != VK_NULL_HANDLE);

  if (smp_VkMemory != VK_NULL_HANDLE)
  {
    vkFreeMemory(smp_VkDevice, smp_VkMemory, nullptr);
  }

  if (smp_pNext != nullptr)
  {
    delete smp_pNext;
  }

  smp_Nodes.Clear();
  smp_Handles.Clear();

  smp_FreeListHeadIndex = 0;
  smp_BlockCount = 0;
  smp_BlockSize = 0;
  smp_AllocationCount = 0;
  smp_VkMemoryTypeIndex = 0;
  smp_HandleLastIndex = 0;
}

void SvkMemoryPool::Init(uint32_t memoryTypeIndex)
{
  ASSERT(smp_VkMemory == VK_NULL_HANDLE);

  smp_VkMemoryTypeIndex = memoryTypeIndex;

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = smp_BlockCount * smp_BlockSize;
  allocInfo.memoryTypeIndex = smp_VkMemoryTypeIndex;

  VkResult r = vkAllocateMemory(smp_VkDevice, &allocInfo, nullptr, &smp_VkMemory);
  ASSERT(r == VK_SUCCESS);

  // allocate max node amount which is the same as block count
  smp_Nodes.New(smp_BlockCount);

  smp_FreeListHeadIndex = 0;
  smp_Nodes[smp_FreeListHeadIndex].blockCount = smp_BlockCount;
  smp_Nodes[smp_FreeListHeadIndex].blockIndex = 0;
  smp_Nodes[smp_FreeListHeadIndex].nextNodeIndex = -1;
}

uint32_t SvkMemoryPool::Allocate(VkMemoryAllocateInfo allocInfo, VkDeviceMemory &outMemory, uint32_t &outOffset)
{
  ASSERT(allocInfo.sType == VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);
  ASSERT(allocInfo.allocationSize > 0);

  if (smp_VkMemory == VK_NULL_HANDLE)
  {
    Init(allocInfo.memoryTypeIndex);
  }

  // make sure that memory types are same
  ASSERTMSG(smp_VkMemoryTypeIndex == allocInfo.memoryTypeIndex, "Create new SvkMemoryPool with another memory type index");
  // check sizes
  ASSERT(smp_BlockCount * smp_BlockSize >= allocInfo.allocationSize);

  uint32_t reqBlockCount = allocInfo.allocationSize / smp_BlockSize + 1;

  int32_t prevNode = -1;
  int32_t curNode = smp_FreeListHeadIndex;

  while (curNode != -1)
  {
    // find first suitable
    if (smp_Nodes[curNode].blockCount > reqBlockCount)
    {
      int32_t diff = smp_Nodes[curNode].blockCount - reqBlockCount;

      AllocHandle &handle = smp_Handles.Push();
      handle.id = smp_HandleLastIndex++;
      handle.blockIndex = smp_Nodes[curNode].blockIndex;
      handle.blockCount = reqBlockCount;

      outMemory = smp_VkMemory;
      outOffset = smp_Nodes[curNode].blockIndex * smp_BlockSize;

      if (diff != 0)
      {
        // shift and reduce node's size
        smp_Nodes[curNode].blockIndex += reqBlockCount;
        smp_Nodes[curNode].blockCount -= reqBlockCount;
      }
      // if can merge next free range with previous one
      else
      {
        if (prevNode == -1 && smp_Nodes[curNode].nextNodeIndex == -1)
        {
          // if only head
          smp_Nodes[curNode].blockIndex += reqBlockCount;
          smp_Nodes[curNode].blockCount -= reqBlockCount;
        }
        else if (prevNode != -1 && smp_Nodes[curNode].nextNodeIndex == -1)
        {
          // if there is prev, but not next
          smp_Nodes[prevNode].nextNodeIndex = -1;
          smp_Nodes[prevNode].blockCount -= reqBlockCount;
        }
        else if (prevNode == -1 && smp_Nodes[curNode].nextNodeIndex != -1)
        {
          // if there is next, but not prev; then it's head
          ASSERT(curNode == smp_FreeListHeadIndex);
          smp_FreeListHeadIndex = smp_Nodes[curNode].nextNodeIndex;
        }
        else
        {
          // if there are prev and next, skip current
          smp_Nodes[prevNode].nextNodeIndex = smp_Nodes[curNode].nextNodeIndex;
        }
      }

      return handle.id;
    }

    prevNode = curNode;
    curNode = smp_Nodes[curNode].nextNodeIndex;
  }

  ASSERTALWAYS("Increase SvkMemoryPool block count or block size");

  // TODO: memory pool chain: what pool check on freeing?
  // if free list is empty or none is found, create new and slightly bigger
  //smp_pNext = new SvkMemoryPool(smp_VkDevice, smp_BlockSize, (uint32_t)(smp_BlockCount * 1.25f));
  //return smp_pNext->Allocate(allocInfo, outMemory, outOffset);
}

void SvkMemoryPool::Free(uint32_t handle)
{
  bool found = false;
  uint32_t blockIndex, blockCount;

  for (INDEX i = 0; i < smp_Handles.Count(); i++)
  {
    if (smp_Handles[i].id == handle)
    {
      blockIndex = smp_Handles[i].blockIndex;
      blockCount = smp_Handles[i].blockCount;

      // replace this with last and pop previous last
      smp_Handles[i] = smp_Handles[smp_Handles.Count() - 1];
      smp_Handles.Pop();

      found = true;
      break;
    }
  }

  ASSERT(found);

  // TODO


}
