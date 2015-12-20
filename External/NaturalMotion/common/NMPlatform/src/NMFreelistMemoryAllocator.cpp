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
#include "NMPlatform/NMFreelistMemoryAllocator.h"
#include <stdio.h>
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
//----------------------------------------------------------------------------------------------------------------------

#ifdef NMP_FREELIST_BUCKETS_ENABLED
#define NMP_FREELIST_PRE_BUCKET_SIZE (2<<(NMP_FREELIST_PRE_BUCKET_SHIFT))
#define NMP_FREELIST_MAX_BUCKET_SIZE (NMP_FREELIST_PRE_BUCKET_SIZE<<(NMP_FREELIST_NUM_CHUNK_BUCKETS))
#else
#define NMP_FREELIST_PRE_BUCKET_SIZE 1
#define NMP_FREELIST_MAX_BUCKET_SIZE 0
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifdef NMP_FREELIST_MEM_LOGGING
//----------------------------------------------------------------------------------------------------------------------
size_t FreelistMemoryAllocator::memGetTop(size_t top)
{
  FreelistLink** freelistLink = &(m_freelists);
  size_t newtop = 0;

  while (*freelistLink)
  {
    FastFreeList::FreeListChunk* chunk = (*freelistLink)->freelist->m_chunks;

    while (chunk)
    {
      size_t size = (*freelistLink)->freelist->m_entryFormat.size;
      if (size < top && size > newtop )
      {
        newtop = size;
      }
      chunk = chunk->m_next;
    }
    freelistLink = &((*freelistLink)->next);
  }

  return newtop;
}

//----------------------------------------------------------------------------------------------------------------------
void FreelistMemoryAllocator::memStatusInfo()
{
  NMP_STDOUT("memStatusInfo:");
  NMP_STDOUT("");
  NMP_STDOUT(">  current trigger");
  NMP_STDOUT("");
  NMP_STDOUT(" O currently empty");
  NMP_STDOUT(" + emptied more than once");
  NMP_STDOUT("");
  NMP_STDOUT(" X once full");
  NMP_STDOUT(" - no longer full");
  NMP_STDOUT("");
  NMP_STDOUT(" # 'busy'");
  NMP_STDOUT(" @ no deallocations");
  NMP_STDOUT("");
  NMP_STDOUT(".  a bucketed freelist");
  NMP_STDOUT("?  a (non bucketed) freelist only used once. What is it?");
  NMP_STDOUT("");
  NMP_STDOUT("format: [>.?]O+X-[#@]");
  NMP_STDOUT("");
}

//----------------------------------------------------------------------------------------------------------------------
void FreelistMemoryAllocator::processFreeListMemStatus(FastFreeList* freelist, FastFreeList::FreeListChunk* stress, uint32_t* memAlloced, uint32_t* memMaxUsed, uint32_t* memUsed)
{
  FastFreeList::FreeListChunk* chunk = freelist->m_chunks;

  int cc = 1;
  while (chunk)
  {
    size_t size = freelist->m_entryFormat.size;

    uint32_t alignment  = freelist->m_entryFormat.alignment;
    uint32_t capacity   = freelist->m_entriesPerChunk;  
    uint32_t numEmpties = chunk->numEmpties;
    uint32_t numUsed = capacity - chunk->numFreeEntries;
    uint32_t maxUse  = capacity - chunk->minFreeEntries;
    uint32_t thrash  = (chunk->touches + maxUse)/maxUse; // ??

    (*memAlloced) += capacity * (uint32_t)NMP::Memory::align(size, alignment);
    (*memMaxUsed) += maxUse   * (uint32_t)NMP::Memory::align(size, alignment);
    (*memUsed)    += numUsed  * (uint32_t)NMP::Memory::align(size, alignment);

    char label[7] = "      ";
    // This always evaluates to true, but only for default values of BUCKET_SIZE
    if (size > NMP_FREELIST_MAX_BUCKET_SIZE || size <= NMP_FREELIST_PRE_BUCKET_SIZE) {
      if ((numUsed == 0) && (numEmpties == 1)) {
        if (thrash == 2) {
          label[0] = '?';
        }
      }
    }
    else {
      label[0] = '.';
    }
    if (chunk == stress) {
      label[0] = '>';
    }    
    if (numUsed == 0) {
      label[1] = 'O';      
    }
    if (numEmpties > 1) {
      label[2] = '+';
    }
    if (chunk->minFreeEntries == 0) {
      label[3] = 'X';
      if (chunk->numFreeEntries > 0) {
        if (numUsed != 0) {
          label[4] = '-';
        }
      }
    }
    if (chunk->touches == 0) {
      label[5] = '@';
    }
    else if (thrash > 2) {
      label[5] = '#';
    }
 
    NMP_STDOUT("%s chunk=%i size=%i align=%i slots=%i used=%i max=%i(%i%%) emptied=%i t=%i", label, cc++, size, alignment, capacity, numUsed, maxUse, maxUse*100/capacity, numEmpties, thrash);   

    chunk = chunk->m_next;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FreelistMemoryAllocator::memStatus(FastFreeList::FreeListChunk* stress)
{
  uint32_t memAlloced = 0;
  uint32_t memMaxUsed = 0;
  uint32_t memUsed = 0;

#ifdef NMP_FREELIST_BUCKETS_ENABLED
  bool doneBuckets = false;
#endif

  size_t newtop = memGetTop(m_defaultChunkSize);

  while (newtop)
  {
#ifdef NMP_FREELIST_BUCKETS_ENABLED
    if (!doneBuckets && newtop <= NMP_FREELIST_PRE_BUCKET_SIZE)
    {
      NMP_STDOUT("");
      uint32_t buckets = NMP_FREELIST_NUM_CHUNK_BUCKETS;
      while(buckets--)
      {
        if (m_freelistBucket[buckets])
        {
          processFreeListMemStatus(m_freelistBucket[buckets], stress, &memAlloced, &memMaxUsed, &memUsed);                
        }
      }
      NMP_STDOUT("");
      doneBuckets = true;
    }
#endif

    FreelistLink** freelistLink = &(m_freelists);

    while (*freelistLink)
    {
      size_t size = (*freelistLink)->freelist->m_entryFormat.size;
      if (size == newtop)
      {
        processFreeListMemStatus((*freelistLink)->freelist, stress, &memAlloced, &memMaxUsed, &memUsed);                
      }
      freelistLink = &((*freelistLink)->next);
    }

    newtop = memGetTop(newtop);
  }

  NMP_STDOUT("\n allocated=%i used=%i(%i%%) max=%i(%i%%)", memAlloced, memUsed, memUsed*100/memAlloced, memMaxUsed, memMaxUsed*100/memAlloced);

  NMP_STDOUT("##############################################################################");
}
#endif // NMP_FREELIST_MEM_LOGGING

//----------------------------------------------------------------------------------------------------------------------
void* FreelistMemoryAllocator::memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL)
{
#ifdef NMP_MEMORY_LOGGING
  (void)file;
  (void)line;
  (void)func;
#endif // NMP_MEMORY_LOGGING  

#ifdef NMP_FREELIST_BUCKETS_ENABLED
  int bucketNumber = -1;
  if (alignment <= 16) // custom allocations with > 16 byte alignment are passed through to the standard freelists.
  {
    size_t chunkBucketSize = NMP_FREELIST_PRE_BUCKET_SIZE;
    size_t shiftSize = (size-1) >> (NMP_FREELIST_PRE_BUCKET_SHIFT+1);
    if (shiftSize)
    {    
      while (shiftSize)
      {
        shiftSize>>=1;
        chunkBucketSize<<=1;
        bucketNumber++;
      }
      if (chunkBucketSize <= NMP_FREELIST_MAX_BUCKET_SIZE)
      {
        size = chunkBucketSize;
      }
      else
      {
        bucketNumber = -1;
      }
    }
  }
#endif

#ifdef NMP_MEMORY_LOGGING
  NMP_DEBUG_MSG("FLMA: size=%i align=%i func=%s line=%i file=%s", size, alignment, func, line, file);
#endif // NMP_MEMORY_LOGGIN

  // First, we see if we can find a freelist that can service this request
  FreelistLink** freelistLink = &(m_freelists);

#ifdef NMP_FREELIST_BUCKETS_ENABLED
  if (bucketNumber >= 0)
  {
    // ignore alignments for bucketed freelists, which are all 16 byte aligned
    alignment = 16;

    if (m_freelistBucket[bucketNumber])
    {
      logExternalAlloc(m_freelistBucket[bucketNumber]->m_entryFormat.size);
#ifdef NMP_FREELIST_MEM_LOGGING
      FastFreeList::FreeListChunk* stress = NULL;
      void* ptr = m_freelistBucket[bucketNumber]->allocateEntry(&stress);
      if (stress) {
        memStatus(stress);
      }
#else
      void* ptr = m_freelistBucket[bucketNumber]->allocateEntry();
#endif
      return ptr;
    }
  }
  else
#endif
  {
    // any bucketNumber < 0 comes here, if we don't find a freelist, then
    // the freelistLink is left pointing at the end of the chain
    while (*freelistLink)
    {
      if ((*freelistLink)->freelist->m_entryFormat.size == size &&
        (*freelistLink)->freelist->m_entryFormat.alignment == alignment)
      {
        // This freelist matches!      
        logExternalAlloc((*freelistLink)->freelist->m_entryFormat.size);
#ifdef NMP_FREELIST_MEM_LOGGING
        FastFreeList::FreeListChunk* stress = NULL;
        void* ptr = (*freelistLink)->freelist->allocateEntry(&stress);
        if (stress) {
          memStatus(stress);
        }
#else
        void* ptr = (*freelistLink)->freelist->allocateEntry();
#endif
        return ptr;
      }
      freelistLink = &((*freelistLink)->next);
    }
  }

  // If we get here, we didn't find a suitable freelist, so we should create one.
  NMP::Memory::Format format(size, alignment);

  // We need to use the default chunk size to work out how many entries per chunk to go for. 
  uint32_t entriesPerChunk = m_defaultChunkSize / (uint32_t)NMP::Memory::align(size, alignment);
  if (entriesPerChunk == 0)
    entriesPerChunk = 1;

  NMP::Memory::Format allocFormat = NMP::FastFreeList::getMemoryRequirements(format, entriesPerChunk);
  NMP::Memory::Resource freelistResource = NMPMemoryAllocateFromFormat(allocFormat);
  logInternalAlloc(NMP::Memory::memSize(freelistResource.ptr));
  NMP_ASSERT(freelistResource.ptr);

  NMP::FastFreeList* newFreelist = NMP::FastFreeList::init(freelistResource, format, entriesPerChunk, &m_heapAllocator);  

#ifdef NMP_FREELIST_BUCKETS_ENABLED
  if (bucketNumber >= 0)
  {
    NMP_ASSERT(bucketNumber < NMP_FREELIST_NUM_CHUNK_BUCKETS)
    m_freelistBucket[bucketNumber] = newFreelist;
  }
  else
#endif
  {
    // Add the freelist to the chain.
    NMP::Memory::Format allocFormat2(sizeof(FreelistLink), NMP_NATURAL_TYPE_ALIGNMENT);
    FreelistLink* newLink = (FreelistLink*)(NMPMemoryAllocateFromFormat(allocFormat2).ptr);
    logInternalAlloc(NMP::Memory::memSize(newLink));
    NMP_ASSERT(newLink);
    newLink->next = NULL;
    newLink->freelist = newFreelist;

    // the freelistLink from above points to the last link's m_next pointer so we can immediately insert at the end.
    (*freelistLink) = newLink;
  }

  // Allocate from this new freelist
  logExternalAlloc(newFreelist->m_entryFormat.size);
#ifdef NMP_FREELIST_MEM_LOGGING
  FastFreeList::FreeListChunk* stress = 0;
  void* ptr = newFreelist->allocateEntry(&stress);
  if (stress)  {
    memStatus(stress);
  }
#else
  void* ptr = newFreelist->allocateEntry();
#endif
  return ptr;
}

//----------------------------------------------------------------------------------------------------------------------
void FreelistMemoryAllocator::memFree(void* ptr)
{
  // find the freelist this came from
  FreelistLink** freelistLink = &(m_freelists);

  // check the freelist chain
  while (*freelistLink)
  {
    if ((*freelistLink)->freelist->memoryIsManagedByFreeList(ptr))
    {
      // This freelist matches!
      logExternalFree((*freelistLink)->freelist->m_entryFormat.size);
      (*freelistLink)->freelist->deallocateEntry(ptr);  

#ifdef NMP_MEMORY_LOGGING
      size_t size = (*freelistLink)->freelist->m_entryFormat.size;
      int alignment = (*freelistLink)->freelist->m_entryFormat.alignment;
      int capacity = (*freelistLink)->freelist->m_actualCapacity;
      (void) size;
      (void) alignment;
      (void) capacity;
      NMP_DEBUG_MSG("FLMF: size=%i align=%i capacity=%i", size, alignment, capacity);
#endif // NMP_MEMORY_LOGGING

      return;
    }
    freelistLink = &((*freelistLink)->next);
  }

#ifdef NMP_FREELIST_BUCKETS_ENABLED
  // check the buckets
  uint32_t bucket = NMP_FREELIST_NUM_CHUNK_BUCKETS;
  while(bucket--)
  {
    if (m_freelistBucket[bucket] && m_freelistBucket[bucket]->memoryIsManagedByFreeList(ptr))
    {
      // This freelist matches!
      logExternalFree(m_freelistBucket[bucket]->m_entryFormat.size);
      m_freelistBucket[bucket]->deallocateEntry(ptr);

#ifdef NMP_MEMORY_LOGGING
      size_t size = m_freelistBucket[bucket]->m_entryFormat.size;
      int alignment = m_freelistBucket[bucket]->m_entryFormat.alignment;
      int capacity = m_freelistBucket[bucket]->m_actualCapacity;
      (void) size;
      (void) alignment;
      (void) capacity;
      NMP_DEBUG_MSG("FLMF: size=%i align=%i capacity=%i", size, alignment, capacity);
#endif // NMP_MEMORY_LOGGING

      return;
    }   
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
size_t FreelistMemoryAllocator::memSize(void* ptr)
{
  // find the freelist this came from
  FreelistLink** freelistLink = &(m_freelists);

  while (*freelistLink)
  {
    if ((*freelistLink)->freelist->memoryIsManagedByFreeList(ptr))
    {
      return (*freelistLink)->freelist->m_entryFormat.size;
    }
    freelistLink = &((*freelistLink)->next);
  }

#ifdef NMP_FREELIST_BUCKETS_ENABLED
  uint32_t bucket = NMP_FREELIST_NUM_CHUNK_BUCKETS;
  while(bucket--)
  {
    if (m_freelistBucket[bucket] && m_freelistBucket[bucket]->memoryIsManagedByFreeList(ptr))
    {
      return m_freelistBucket[bucket]->m_entryFormat.size;
    }   
  }
#endif

  return 0;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
