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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NMP_STATIC_FREELIST_ALLOCATOR_H
#define NMP_STATIC_FREELIST_ALLOCATOR_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMStaticFreeList.h"

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class StaticFreeListAllocator
/// \brief This memory allocator uses NMP::StaticFreeLists to provide memory space for common-sized allocations.  It
///  can be used to prevent heavy memory thrashing and fragmentation when objects of a constant size are being allocated
///  and freed frequently.  If a free list is not available to service the request, the allocator will go to the heap.
//----------------------------------------------------------------------------------------------------------------------
class StaticFreeListAllocator : public MemoryAllocator
{
public:
  void* memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL);
  void memFree(void* ptr);
  bool term();

  static NM_INLINE NMP::Memory::Format getMemoryRequirements(uint32_t numFreeLists, NMP::Memory::Format* formats, uint32_t* counts);
  static NM_INLINE StaticFreeListAllocator* init(NMP::Memory::Resource& resource, uint32_t numFreeLists, NMP::Memory::Format* formats, uint32_t* counts);
private:

  uint32_t m_numFreeLists;

  NMP::StaticFreeList** m_freeLists;
};

//----------------------------------------------------------------------------------------------------------------------
// StaticFreeListAllocator functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format StaticFreeListAllocator::getMemoryRequirements(uint32_t numFreeLists, NMP::Memory::Format* formats, uint32_t* counts)
{
  NMP::Memory::Format result(sizeof(StaticFreeListAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  result += NMP::Memory::Format(sizeof(NMP::StaticFreeList*), NMP_NATURAL_TYPE_ALIGNMENT) * numFreeLists;

  for (uint32_t i = 0; i < numFreeLists; i++)
  {
    result += NMP::StaticFreeList::getMemoryRequirements(formats[i], counts[i]);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
StaticFreeListAllocator* StaticFreeListAllocator::init(NMP::Memory::Resource& resource, uint32_t numFreeLists, NMP::Memory::Format* formats, uint32_t* counts)
{
  NMP::Memory::Format format(sizeof(StaticFreeListAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  // Set up the vtable by calling in place constructor
  StaticFreeListAllocator* result = new(resource.ptr) StaticFreeListAllocator();
  resource.increment(format);

  result->initStats();
  result->m_numFreeLists = numFreeLists;

  format = NMP::Memory::Format(sizeof(NMP::StaticFreeList*) , NMP_NATURAL_TYPE_ALIGNMENT) * numFreeLists;
  resource.align(format);
  result->m_freeLists = (NMP::StaticFreeList**)resource.ptr;
  resource.increment(format);

  // Initialize all the free lists
  for (uint32_t i = 0; i < numFreeLists; i++)
  {
    format = NMP::StaticFreeList::getMemoryRequirements(formats[i], counts[i]);
    resource.align(format);
    result->m_freeLists[i] = NMP::StaticFreeList::init(resource, formats[i], counts[i]);
  }

  return result;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_STATIC_FREELIST_ALLOCATOR_H
//----------------------------------------------------------------------------------------------------------------------
