// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMStaticFreeListAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
//----------------------------------------------------------------------------------------------------------------------
void* StaticFreeListAllocator::memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL)
{
  // See if we can find a freelist to make this allocation from
  for (uint32_t i = 0; i < m_numFreeLists; i++)
  {
    if (m_freeLists[i]->m_entryFormat.size == size &&
        m_freeLists[i]->m_entryFormat.alignment == alignment)
    {
      // We found a suitable freelist
      void* result = m_freeLists[i]->allocateEntry();
      if (!result)
      {
        NMP_DEBUG_MSG("WARNING: StaticFreeListAllocator could not find space for allocation of size %i alignment %i, so going to the heap\n", size, alignment);
        result = NMP::Memory::memAllocAligned(size, alignment NMP_MEMORY_TRACKING_PASS_THROUGH);
        size_t allocSize = NMP::Memory::memSize(result);
        logInternalAlloc(allocSize);
        logExternalAlloc(allocSize);
        return result;
      }
      else
      {
        // We allocated successfully
        logExternalAlloc(m_freeLists[i]->m_entryFormat.size);
        return result;
      }
    }
  }

  NMP_DEBUG_MSG("WARNING: StaticFreeListAllocator could not find a suitable freelist for allocation of size %i alignment %i, so going to the heap\n", size, alignment);
  void* result = NMP::Memory::memAllocAligned(size, alignment NMP_MEMORY_TRACKING_PASS_THROUGH);
  size_t allocSize = NMP::Memory::memSize(result);
  logInternalAlloc(allocSize);
  logExternalAlloc(allocSize);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void StaticFreeListAllocator::memFree(void* ptr)
{
  for (uint32_t i = 0; i < m_numFreeLists; i++)
  {
    // Try to find the free list this allocation came from
    if (m_freeLists[i]->memoryIsManagedByFreeList(ptr))
    {
      logExternalFree(m_freeLists[i]->m_entryFormat.size);
      m_freeLists[i]->deallocateEntry(ptr);
      return;
    }
  }

  // If we get here, then none of the freelists managed the memory and we should free it from the heap
  size_t size = NMP::Memory::memSize(ptr);
  logExternalFree(size);
  logInternalFree(size);
  NMP::Memory::memFree(ptr);

  return;
}

//----------------------------------------------------------------------------------------------------------------------
bool StaticFreeListAllocator::term()
{
  return true;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
