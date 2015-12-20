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
#ifndef NM_FAST_FREELIST_H
#define NM_FAST_FREELIST_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

#define NMP_FREELIST_MEM_LOGGINGx

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::FastFreeList
/// \brief Manages a free list of data items with specifiable memory requirements.
/// \ingroup NaturalMotionPlatform
///
/// All memory blocks are of the same size.
/// Memory allocations can allocate blocks larger than the data item required size in order to reduce the number of
/// allocations required.
//----------------------------------------------------------------------------------------------------------------------
class FastFreeList
{
public:

  struct FreeListChunk
  {
    FreeListChunk* m_next;

    void**         freeEntries;
    uint32_t       numFreeEntries;

#ifdef NMP_FREELIST_MEM_LOGGING
    uint32_t       minFreeEntries;
    uint32_t       touches;
    uint32_t       numEmpties;
#endif

    /// This buffer is adequate to accommodate a set of objects of the item size/alignment that this freelist
    ///  provides storage for.
    void*          m_buffer;
    void*          m_bufferEnd;
  };

  static NMP::Memory::Format getMemoryRequirements(
    NMP::Memory::Format entryFormat,
    uint32_t            entriesPerChunk);

  static NMP::FastFreeList* init(
    NMP::Memory::Resource resource,
    NMP::Memory::Format   entryFormat,
    uint32_t              entriesPerChunk,
    NMP::MemoryAllocator* allocator);

  FreeListChunk* addChunk(NMP::Memory::Resource resource);
  FreeListChunk* addChunk();

#ifdef NMP_FREELIST_MEM_LOGGING
  void* allocateEntry(FreeListChunk** stress);
#else
  void* allocateEntry();
#endif
  bool deallocateEntry(void* ptr);

  bool freeChunks();

  void freeChunk(FreeListChunk* chunk);

  NM_INLINE bool memoryIsManagedByFreeList(void* ptr);

  NMP::Memory::Format   m_entryFormat;
  uint32_t              m_entriesPerChunk;
  NMP::Memory::Format   m_chunkFormat;

  NMP::MemoryAllocator* m_allocator;

  uint32_t              m_actualCapacity;

  FreeListChunk*        m_chunks;
};

//----------------------------------------------------------------------------------------------------------------------
// FastFreeList functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool FastFreeList::memoryIsManagedByFreeList(void* ptr)
{
  FreeListChunk** chunk = &m_chunks;

  // Find a chunk with a free slot
  while (*chunk)
  {
    // was the pointer in this block?
    if ((ptr >= (*chunk)->m_buffer) && (ptr < (*chunk)->m_bufferEnd))
    {
      // Found it.
      return true;
    }

    chunk = &((*chunk)->m_next);
  }

  // If we make it here, we got to the end of the chunk list without finding the pointer.
  return false;
}

} // namespace NMP
//----------------------------------------------------------------------------------------------------------------------
#endif // NM_FAST_FREELIST_H
//----------------------------------------------------------------------------------------------------------------------
