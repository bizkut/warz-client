// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "MultiThreadTempAllocator.h"

//----------------------------------------------------------------------------------------------------------------------
// ThreadSafeMemoryPool functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ThreadSafeMemoryPool::getMemoryRequirements(uint32_t numPages, uint32_t pageSize, uint32_t pageAlignment)
{
  NMP::Memory::Format result(sizeof(ThreadSafeMemoryPool), NMP_NATURAL_TYPE_ALIGNMENT);

  NMP::Memory::Format pageFormat(pageSize, pageAlignment);
  result += NMP::StaticFreeList::getMemoryRequirements(pageFormat, numPages);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
ThreadSafeMemoryPool* ThreadSafeMemoryPool::init(NMP::Memory::Resource& resource, uint32_t numPages, uint32_t pageSize, uint32_t pageAlignment)
{
  NMP::Memory::Format format(sizeof(ThreadSafeMemoryPool), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  ThreadSafeMemoryPool* result = (ThreadSafeMemoryPool*)resource.ptr;
  resource.increment(format);

  // Set up the freelist, which contains the memory pages.
  NMP::Memory::Format pageFormat(pageSize, pageAlignment);
  result->m_freelist = NMP::StaticFreeList::init(resource, pageFormat, numPages);
  
  // Create a mutex to protect the freelist in multi-threaded environments
  result->m_mutex = CreateMutex(0, false, 0);
  NMP_ASSERT(result->m_mutex);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool ThreadSafeMemoryPool::term()
{
  // Free up the mutex.
  CloseHandle(m_mutex);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void* ThreadSafeMemoryPool::memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL)
{
#ifdef NMP_MEMORY_LOGGING
  (void) func;
  (void) line;
  (void) file;
#endif

  // Early out if we can't service this request.
  if(size != m_freelist->m_entryFormat.size || alignment != m_freelist->m_entryFormat.alignment)
  {
    return NULL;
  }

  // Lock the freelist to read from it.
#ifdef NM_DEBUG
  BOOL lockResult = 
#endif // NM_DEBUG
    WaitForSingleObject(m_mutex, INFINITE);
  NMP_ASSERT(lockResult == WAIT_OBJECT_0);

  // Perform the allocation
  void* result = m_freelist->allocateEntry();

  // Release the freelist mutex
#ifdef NM_DEBUG
  BOOL releaseResult = 
#endif // NM_DEBUG
    ReleaseMutex(m_mutex);
  NMP_ASSERT(releaseResult);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ThreadSafeMemoryPool::memFree(void* ptr)
{
  // Lock the freelist to read from it.
#ifdef NM_DEBUG
  BOOL lockResult = 
#endif // NM_DEBUG
    WaitForSingleObject(m_mutex, INFINITE);
  NMP_ASSERT(lockResult == WAIT_OBJECT_0);

  // Perform the free operation
  m_freelist->deallocateEntry(ptr);

  // Release the freelist mutex
#ifdef NM_DEBUG
  BOOL releaseResult = 
#endif // NM_DEBUG
    ReleaseMutex(m_mutex);
  NMP_ASSERT(releaseResult);
}

//----------------------------------------------------------------------------------------------------------------------
// MultiThreadTempAllocator functions
//----------------------------------------------------------------------------------------------------------------------
MultiThreadTempAllocator* MultiThreadTempAllocator::init(NMP::Memory::Resource& resource, ThreadSafeMemoryPool* memoryPool)
{
  NMP_ASSERT(memoryPool);

  NMP::Memory::Format format(sizeof(MultiThreadTempAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  // Set up the vtable by calling in place constructor
  MultiThreadTempAllocator* result = new(resource.ptr) MultiThreadTempAllocator();
  resource.increment(format);

  // We get an initial block of memory from the shared pool to use.
  uint32_t size = (uint32_t)memoryPool->getPageSize();

  result->initStats();
  result->m_memoryPool = memoryPool;
  result->m_memoryBlock = (char*)memoryPool->memAlloc(size, NMP_VECTOR_ALIGNMENT NMP_MEMORY_TRACKING_ARGS);
  result->m_currentPtr = result->m_memoryBlock;
  result->m_memoryEnd = result->m_memoryBlock + size;
  result->m_size = size;
  result->m_parentAllocator = NULL;
  result->m_childAllocator = NULL;
  result->m_numFilledPages = 0;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool MultiThreadTempAllocator::term()
{
  // Make sure it hasn't already been terminated
  NMP_ASSERT(m_memoryBlock);

  // Free all filled pages to the shared pool
  for(uint32_t i = 0; i < m_numFilledPages; i++)
  {
    m_memoryPool->memFree(m_filledPages[i]);
  }
  m_numFilledPages = 0;

  // Free the current page
  m_memoryPool->memFree(m_memoryBlock);
  m_memoryBlock = NULL;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void* MultiThreadTempAllocator::memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL)
{
  // You are not allowed to allocate from a TempMemoryAllocator while it has an active child
  NMP_ASSERT(m_childAllocator == NULL);

#ifdef NMP_MEMORY_LOGGING
  (void)file;
  (void)line;
  (void)func;
#endif // NMP_MEMORY_LOGGING

  char* alignedAddr = (char*)NMP::Memory::align(m_currentPtr, alignment);

  // Check that there's space left for this allocation. 
  // If not then we request another page from the shared pool.  If the pool has no memory available, we assert.
  if((alignedAddr + size) > m_memoryEnd)
  {
    // We didn't have space in the current page for this allocation, so we should get a new page.
    void *newPage = m_memoryPool->memAlloc(m_size, NMP_VECTOR_ALIGNMENT NMP_MEMORY_TRACKING_PASS_THROUGH);
    NMP_ASSERT(newPage);
    NMP_ASSERT(m_numFilledPages < MAX_FILLED_PAGES); // We make sure there's space to store the filled page in our list.
    m_filledPages[m_numFilledPages++] = m_memoryBlock;

    m_memoryBlock = (char*)newPage;
    m_currentPtr = m_memoryBlock;
    m_memoryEnd = m_memoryBlock + m_size;
    alignedAddr = (char*)NMP::Memory::align(m_currentPtr, alignment);
  }
  
  // Make sure the allocation fits in the new page (this would catch an allocation request which was over the page size).
  NMP_ASSERT_MSG((alignedAddr + size) <= m_memoryEnd, "MultiThreadTempAllocator::memAlloc FAIL.\n");

  logExternalAlloc(alignedAddr + size - m_currentPtr);
  void* result = alignedAddr;
  m_currentPtr = alignedAddr + size;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void MultiThreadTempAllocator::memFree(void* NMP_UNUSED(ptr))
{
  // This does nothing in the MultiThreadTempAllocator
  return;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::TempMemoryAllocator *MultiThreadTempAllocator::createChildAllocator()
{
  // You cannot add a child allocator if there's already one there.
  NMP_ASSERT(m_childAllocator == NULL);

  // Create the new allocator
  NMP::Memory::Format allocatorFormat = MultiThreadTempAllocator::getMemoryRequirements();

  // This will call allocateFromFormat, which will request a new page if necessary
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(this, allocatorFormat);
  NMP_ASSERT(resource.ptr);

  // This will cause the allocator to request a new page from the shared pool, which is inefficient.  It would be better
  //  to keep using the parent allocator's current memory page until it fills up.
  MultiThreadTempAllocator *result = MultiThreadTempAllocator::init(resource, m_memoryPool);

  m_childAllocator = result;
  result->m_parentAllocator = this;

  // We recover the memory used to create the MultiThreadTempAllocator itself. Note that we may have switched pages
  //  during this function.
  m_currentPtr -= sizeof(MultiThreadTempAllocator);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void MultiThreadTempAllocator::destroyChildAllocator(NMP::TempMemoryAllocator *bm)
{
  // Check it's our child allocator
  NMP_ASSERT(bm == m_childAllocator);
  NMP_ASSERT(((MultiThreadTempAllocator*)bm)->m_parentAllocator == this);

  // Free all pages back to the pool.
  bm->term();

  // Remove the child allocator
  m_childAllocator = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t MultiThreadTempAllocator::reset()
{
  // You cannot reset an allocator which has an active child.
  NMP_ASSERT(m_childAllocator == NULL);
  size_t size = getUsedBytes();
  logExternalFree(size);
  NMP_ASSERT((getTotalExternalStats().getAllocedBytes() - getTotalExternalStats().getFreedBytes()) == 0);
  uint32_t result = (uint32_t)(m_currentPtr - m_memoryBlock);
  m_currentPtr = m_memoryBlock;

  // we need to release all used pages back to the pool, except the current one, which is emptied and retained.
  for(uint32_t i = 0; i < m_numFilledPages; i++)
  {
    m_memoryPool->memFree(m_filledPages[i]);
  }
  m_numFilledPages = 0;

#ifdef DEBUG
  static const uint32_t FILL_PATTERN = 0xCDCDCDCD;
  memset(m_memoryBlock, FILL_PATTERN, m_size);
#endif

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
size_t MultiThreadTempAllocator::getUsedBytes()
{
  return (uint32_t)(m_currentPtr - m_memoryBlock) + (m_numFilledPages * m_size);
}

//----------------------------------------------------------------------------------------------------------------------
