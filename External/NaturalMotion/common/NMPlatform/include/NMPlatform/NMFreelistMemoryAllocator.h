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
#ifndef NMP_FREELIST_MEMORY_ALLOCATOR_H
#define NMP_FREELIST_MEMORY_ALLOCATOR_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMBasicLogger.h"
#include "NMPlatform/NMFastFreeList.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
//----------------------------------------------------------------------------------------------------------------------

// how many freelist buckets to use, set to zero to disable
#define NMP_FREELIST_NUM_CHUNK_BUCKETS 4

// defines last pre-bucket freelist size (2<<5=64)
#define NMP_FREELIST_PRE_BUCKET_SHIFT 5

// default freelist chunk size
#define NMP_FREELIST_DEFAULT_CHUNK_SIZE (16 * 1024)

//----------------------------------------------------------------------------------------------------------------------

#if NMP_FREELIST_NUM_CHUNK_BUCKETS > 0
#define NMP_FREELIST_BUCKETS_ENABLED
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class FreelistMemoryAllocator
/// \brief This memory allocator uses a chain of freelists to try and provide memory for allocations.  If a suitable
///  freelist cannot be found, it will create one.  Heap memory is used to allocate the chunks for freelists.  The
///  allocator will make the chunk sizes as close as possible to the chunk size which the user sets.
//----------------------------------------------------------------------------------------------------------------------
class FreelistMemoryAllocator : public MemoryAllocator
{
public:
#ifdef NMP_FREELIST_MEM_LOGGING
  size_t memGetTop(size_t top);
  static void memStatusInfo();
  void processFreeListMemStatus(FastFreeList* freelist,
    FastFreeList::FreeListChunk* stress,
    uint32_t* memAlloced, uint32_t* memMaxUsed, uint32_t* memUsed);
  void memStatus(FastFreeList::FreeListChunk* stress);
#endif

  void* memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL);
  void memFree(void* ptr);
  NM_INLINE bool term();

  /// \brief Returns the size of a block of memory.  If the block was not allocated by this allocator returns 0.
  size_t memSize(void* ptr);

  static NM_INLINE NMP::Memory::Format getMemoryRequirements();
  static NM_INLINE FreelistMemoryAllocator* init(NMP::Memory::Resource& resource);
  

  NM_INLINE void setDefaultChunkSize(uint32_t numBytesPerChunk);
  NM_INLINE uint32_t getDefaultChunkSize();

private:

#ifdef NMP_FREELIST_BUCKETS_ENABLED
  NMP::FastFreeList* m_freelistBucket[NMP_FREELIST_NUM_CHUNK_BUCKETS];
#endif

  struct FreelistLink
  {
    NMP::FastFreeList* freelist;
    FreelistLink*      next;
  };

  HeapAllocator m_heapAllocator;
  FreelistLink* m_freelists;
  uint32_t      m_defaultChunkSize;
};

//----------------------------------------------------------------------------------------------------------------------
// FreelistMemoryAllocator functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format FreelistMemoryAllocator::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(FreelistMemoryAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
FreelistMemoryAllocator* FreelistMemoryAllocator::init(NMP::Memory::Resource& resource)
{
#ifdef NMP_FREELIST_MEM_LOGGING
  memStatusInfo();
#endif

  NMP::Memory::Format format(sizeof(FreelistMemoryAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  // Set up the vtable by calling in place constructor
  FreelistMemoryAllocator* result = new(resource.ptr) FreelistMemoryAllocator();
  resource.increment(format);

  result->initStats();

#ifdef NMP_FREELIST_BUCKETS_ENABLED
  uint32_t buckets = NMP_FREELIST_NUM_CHUNK_BUCKETS;
  while(buckets--)
  {
    result->m_freelistBucket[buckets] = NULL;
  }
#endif

  new(&(result->m_heapAllocator)) HeapAllocator();

  result->m_freelists = NULL;
  result->m_defaultChunkSize = NMP_FREELIST_DEFAULT_CHUNK_SIZE;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void FreelistMemoryAllocator::setDefaultChunkSize(uint32_t numBytesPerChunk)
{
  NMP_ASSERT(numBytesPerChunk);

  m_defaultChunkSize = numBytesPerChunk;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t FreelistMemoryAllocator::getDefaultChunkSize()
{
  return m_defaultChunkSize;
}

//----------------------------------------------------------------------------------------------------------------------
bool FreelistMemoryAllocator::term()
{
  // Empty all the freelists of memory they allocated.
  FreelistLink* freelistLink = m_freelists;

#ifdef NMP_FREELIST_BUCKETS_ENABLED
  uint32_t buckets = NMP_FREELIST_NUM_CHUNK_BUCKETS;
  while(buckets--)
  {
    if (m_freelistBucket[buckets])
    {
      // Free all the freelist chunks
      m_freelistBucket[buckets]->freeChunks();
      // Free the freelist
      size_t size = NMP::Memory::memSize(m_freelistBucket[buckets]);
      logInternalFree(size);
      NMP::Memory::memFree(m_freelistBucket[buckets]);
    }
  }
#endif

  while (freelistLink)
  {
    // Free all the freelist chunks
    freelistLink->freelist->freeChunks();
    // Free the freelist
    size_t size = NMP::Memory::memSize(freelistLink->freelist);
    logInternalFree(size);
    NMP::Memory::memFree(freelistLink->freelist);
    // free the link and go to the next link
    FreelistLink* tempPtr = freelistLink;
    freelistLink = freelistLink->next;
    size = NMP::Memory::memSize(tempPtr);
    logInternalFree(size);
    NMP::Memory::memFree(tempPtr);
  }

  return true;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_FREELIST_MEMORY_ALLOCATOR_H
//----------------------------------------------------------------------------------------------------------------------
