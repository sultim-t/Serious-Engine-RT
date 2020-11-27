#include "StdH.h"
#include "SvkMemoryPool.h"

#include <Engine/Base/Console.h>

SvkMemoryPool::SvkMemoryPool(VkDevice device, uint32_t preferredSize)
{
  smp_VkDevice = device;
  smp_PreferredSize = preferredSize;

  smp_BlockCount = 0;
  smp_BlockSize = 0;
  smp_VkMemory = VK_NULL_HANDLE;
  smp_pNext = nullptr;
  smp_FreeListHeadIndex = 0;
  smp_AllocationCount = 0;
  smp_VkMemoryTypeIndex = 0;
  smp_HandleLastIndex = 1;
  smp_NodeLastIndex = 0;
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

  smp_PreferredSize = 0;
  smp_FreeListHeadIndex = 0;
  smp_BlockCount = 0;
  smp_BlockSize = 0;
  smp_AllocationCount = 0;
  smp_VkMemoryTypeIndex = 0;
  smp_HandleLastIndex = 1;
  smp_NodeLastIndex = 0;
}

void SvkMemoryPool::Init(uint32_t memoryTypeIndex, uint32_t alignment)
{
  ASSERT(smp_VkMemory == VK_NULL_HANDLE);

  smp_BlockSize = alignment;
  smp_BlockCount = smp_PreferredSize / alignment + 1;

  smp_VkMemoryTypeIndex = memoryTypeIndex;

  VkMemoryAllocateInfo allocInfo = {};
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = smp_BlockCount * smp_BlockSize;
  allocInfo.memoryTypeIndex = smp_VkMemoryTypeIndex;

  VkResult r = vkAllocateMemory(smp_VkDevice, &allocInfo, nullptr, &smp_VkMemory);
  ASSERT(r == VK_SUCCESS);

  // allocate max node amount which is the same as block count
  smp_Nodes.New(smp_BlockCount);

  smp_FreeListHeadIndex = AddNode();
  smp_Nodes[smp_FreeListHeadIndex].blockCount = smp_BlockCount;
  smp_Nodes[smp_FreeListHeadIndex].blockIndex = 0;
  smp_Nodes[smp_FreeListHeadIndex].nextNodeIndex = -1;
}

int32_t SvkMemoryPool::AddNode()
{
  if (smp_RemovedIndices.Count() > 0)
  {
    return smp_RemovedIndices.Pop();
  }

  // smp_Nodes count is the same as smp_BlockCount
  ASSERT(smp_NodeLastIndex + 1 < smp_Nodes.Count());

  return smp_NodeLastIndex++;
}

void SvkMemoryPool::RemoveNode(int32_t removed)
{
  smp_RemovedIndices.Push() = removed;
}

uint32_t SvkMemoryPool::Allocate(VkMemoryAllocateInfo allocInfo, VkMemoryRequirements memReqs, VkDeviceMemory &outMemory, uint32_t &outOffset)
{
  smp_AllocationCount++;

  ASSERT(allocInfo.sType == VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO);
  ASSERT(allocInfo.allocationSize > 0);
  ASSERT(allocInfo.allocationSize % memReqs.alignment == 0);

  if (smp_VkMemory == VK_NULL_HANDLE)
  {
    Init(allocInfo.memoryTypeIndex, memReqs.alignment);
  }

  ASSERT(smp_BlockSize > memReqs.alignment ?
    smp_BlockSize % memReqs.alignment == 0 :
    memReqs.alignment % smp_BlockSize == 0);

  // align size
  uint32_t allocSize = allocInfo.allocationSize;
  uint32_t alignmentBlockCount = 0;
  const uint32_t plAlgn = smp_BlockSize;
  const uint32_t rqAlgn = memReqs.alignment;

  if (plAlgn >= rqAlgn)
  {
    ASSERT(plAlgn % rqAlgn == 0);
  }
  else
  {
    ASSERT(rqAlgn % plAlgn == 0);
    alignmentBlockCount = rqAlgn / plAlgn;
  }

  // make sure that memory types are same
  ASSERTMSG(smp_VkMemoryTypeIndex == allocInfo.memoryTypeIndex, "Create new SvkMemoryPool with another memory type index");
  // check sizes
  ASSERT(smp_BlockCount * smp_BlockSize >= allocSize);

  uint32_t reqBlockCount = allocSize / smp_BlockSize + (allocSize % smp_BlockSize > 0 ? 1 : 0);

  int32_t prevNode = -1;
  int32_t curNode = smp_FreeListHeadIndex;

  while (curNode != -1)
  {
    // find first suitable
    if (smp_Nodes[curNode].blockCount >= reqBlockCount)
    {
      if (alignmentBlockCount > 0)
      {
        uint32_t nextAlignment = smp_Nodes[curNode].blockIndex / alignmentBlockCount
          + (smp_Nodes[curNode].blockIndex % alignmentBlockCount > 0 ? 1 : 0);

        nextAlignment *= alignmentBlockCount;
        uint32_t blocksToAlign = nextAlignment - smp_Nodes[curNode].blockIndex;

        if (blocksToAlign > 0)
        {
          uint32_t alignedBlockCount = smp_Nodes[curNode].blockCount - blocksToAlign;

          if (alignedBlockCount >= reqBlockCount)
          {
            // if aligned is suitable, then add new node after cur
            int32_t alignedNode = AddNode();
            smp_Nodes[alignedNode].blockIndex = nextAlignment;
            smp_Nodes[alignedNode].blockCount = alignedBlockCount;
            smp_Nodes[alignedNode].nextNodeIndex = smp_Nodes[curNode].nextNodeIndex;

            smp_Nodes[curNode].nextNodeIndex = alignedNode;
            smp_Nodes[curNode].blockCount = blocksToAlign;

            prevNode = curNode;
            curNode = alignedNode;
          }
          else
          {
            prevNode = curNode;
            curNode = smp_Nodes[curNode].nextNodeIndex;

            continue;
          }
        }
      }

      uint32_t diff = smp_Nodes[curNode].blockCount - reqBlockCount;

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
          // if only head, destroy it
          ASSERT(curNode == smp_FreeListHeadIndex);
          smp_FreeListHeadIndex = -1;
        }
        else if (prevNode != -1 && smp_Nodes[curNode].nextNodeIndex == -1)
        {
          // if there is prev, but not next
          smp_Nodes[prevNode].nextNodeIndex = -1;
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

        RemoveNode(curNode);
      }

      return handle.id;
    }

    prevNode = curNode;
    curNode = smp_Nodes[curNode].nextNodeIndex;
  }
  
  CPrintF("Vulkan: Increase SvkMemoryPool block count or block size");
  ASSERT(0);

  // TODO: memory pool chain
  // if free list is empty or none is found, create new and slightly bigger
  //smp_pNext = new SvkMemoryPool(smp_VkDevice, smp_BlockSize, (uint32_t)(smp_BlockCount * 1.25f));
  //?allocInfo.allocationSize = allocSize;
  //return smp_pNext->Allocate(allocInfo, outMemory, outOffset);

  return 0;
}

#ifndef NDEBUG
struct DebugFreeListNode
{
  int32_t       nextNodeIndex;
  uint32_t      blockIndex;
  uint32_t      blockCount;
};

void CheckConsistency(int32_t smp_FreeListHeadIndex, void *psmp_Nodes)
{
  CStaticArray<DebugFreeListNode> &smp_Nodes = *((CStaticArray<DebugFreeListNode>*)psmp_Nodes);

  int32_t prevNodeD = -1;
  int32_t curNodeD = smp_FreeListHeadIndex;
  while (curNodeD != -1)
  {
    ASSERT(smp_Nodes[curNodeD].blockCount != 0);
    if (prevNodeD != -1)
    {
      ASSERT(smp_Nodes[prevNodeD].blockIndex + smp_Nodes[prevNodeD].blockCount <= smp_Nodes[curNodeD].blockIndex);
    }

    prevNodeD = curNodeD;
    curNodeD = smp_Nodes[curNodeD].nextNodeIndex;
  }
}
#endif // !NDEBUG

void SvkMemoryPool::Free(uint32_t handle)
{
  smp_AllocationCount--;

  bool found = false;
  uint32_t blockIndex, blockCount;

  for (INDEX i = 0; i < smp_Handles.Count(); i++)
  {
    if (smp_Handles[i].id == handle)
    {
      blockIndex = smp_Handles[i].blockIndex;
      blockCount = smp_Handles[i].blockCount;

      // remove handle: just replace this with last and pop to remove duplicate
      smp_Handles[i] = smp_Handles[smp_Handles.Count() - 1];
      smp_Handles.Pop();

      found = true;
      break;
    }
  }

  ASSERT(found);

  // if there is no head, then just create it
  if (smp_FreeListHeadIndex == -1)
  {
    smp_FreeListHeadIndex = AddNode();
    smp_Nodes[smp_FreeListHeadIndex].blockCount = blockCount;
    smp_Nodes[smp_FreeListHeadIndex].blockIndex = blockIndex;
    smp_Nodes[smp_FreeListHeadIndex].nextNodeIndex = -1;

#ifndef NDEBUG
    CheckConsistency(smp_FreeListHeadIndex, &smp_Nodes);
#endif // !NDEBUG

    return;
  }

  // find node that has block index less than given
  int32_t prevNode = -1;
  int32_t curNode = smp_FreeListHeadIndex;

  while (curNode != -1)
  {
    // don't free already freed
    ASSERT(smp_Nodes[curNode].blockIndex != blockIndex);

    if (smp_Nodes[curNode].blockIndex > blockIndex)
    {
      break;
    }

    prevNode = curNode;
    curNode = smp_Nodes[curNode].nextNodeIndex;
  }

  // if last is one that must be added to free list
  if (curNode == -1)
  {
    ASSERT(prevNode != -1);

    // if can merge with previous
    if (smp_Nodes[prevNode].blockIndex + smp_Nodes[prevNode].blockCount == blockIndex)
    {
      smp_Nodes[prevNode].blockCount += blockCount;

#ifndef NDEBUG
      CheckConsistency(smp_FreeListHeadIndex, &smp_Nodes);
#endif // !NDEBUG
      return;
    }
    else
    {
      // otherwise, create new block and link prev with it
      int32_t newNode = AddNode();
      smp_Nodes[newNode].blockIndex = blockIndex;
      smp_Nodes[newNode].blockCount = blockCount;
      smp_Nodes[newNode].nextNodeIndex = -1;

      smp_Nodes[prevNode].nextNodeIndex = newNode;


#ifndef NDEBUG
      CheckConsistency(smp_FreeListHeadIndex, &smp_Nodes);
#endif // !NDEBUG
      return;
    }
  }

  ASSERT(blockIndex + blockCount <= smp_Nodes[curNode].blockIndex);

  // if head is one that must be added to free list
  if (prevNode == -1)
  {
    ASSERT(curNode == smp_FreeListHeadIndex);

    // check if can merge
    if (blockIndex + blockCount == smp_Nodes[curNode].blockIndex)
    {
      // then update existing
      smp_Nodes[curNode].blockIndex = blockIndex;
      smp_Nodes[curNode].blockCount += blockCount;

#ifndef NDEBUG
      CheckConsistency(smp_FreeListHeadIndex, &smp_Nodes);
#endif // !NDEBUG
    }
    else
    {
      // new separate node is required, then update head
      smp_FreeListHeadIndex = AddNode();
      smp_Nodes[smp_FreeListHeadIndex].blockIndex = blockIndex;
      smp_Nodes[smp_FreeListHeadIndex].blockCount = blockCount;
      smp_Nodes[smp_FreeListHeadIndex].nextNodeIndex = curNode;

#ifndef NDEBUG
      CheckConsistency(smp_FreeListHeadIndex, &smp_Nodes);
#endif // !NDEBUG
    }
  }
  else
  {
    // if not head and not last;
    // check if can merge with prev or next (relatively to new node)
    bool withPrev = smp_Nodes[prevNode].blockIndex + smp_Nodes[prevNode].blockCount == blockIndex;
    bool withNext = blockIndex + blockCount == smp_Nodes[curNode].blockIndex;

    if (withPrev && withNext)
    {
      RemoveNode(curNode);

      smp_Nodes[prevNode].blockCount += blockCount + smp_Nodes[curNode].blockCount;
      smp_Nodes[prevNode].nextNodeIndex = smp_Nodes[curNode].nextNodeIndex;

#ifndef NDEBUG
      CheckConsistency(smp_FreeListHeadIndex, &smp_Nodes);
#endif // !NDEBUG
    }
    else if (withPrev)
    {
      smp_Nodes[prevNode].blockCount += blockCount;

#ifndef NDEBUG
      CheckConsistency(smp_FreeListHeadIndex, &smp_Nodes);
#endif // !NDEBUG
    }
    else if (withNext)
    {
      smp_Nodes[curNode].blockIndex = blockIndex;
      smp_Nodes[curNode].blockCount += blockCount;

#ifndef NDEBUG
      CheckConsistency(smp_FreeListHeadIndex, &smp_Nodes);
#endif // !NDEBUG
    }
    else
    {
      int32_t newNode = AddNode();
      smp_Nodes[newNode].blockIndex = blockIndex;
      smp_Nodes[newNode].blockCount = blockCount;
      smp_Nodes[newNode].nextNodeIndex = curNode;

      smp_Nodes[prevNode].nextNodeIndex = newNode;

#ifndef NDEBUG
      CheckConsistency(smp_FreeListHeadIndex, &smp_Nodes);
#endif // !NDEBUG
    }
  }
}
