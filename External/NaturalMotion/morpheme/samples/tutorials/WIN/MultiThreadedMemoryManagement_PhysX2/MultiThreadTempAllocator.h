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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MULTI_THREAD_TEMP_MEMORY_ALLOCATOR_H
#define MULTI_THREAD_TEMP_MEMORY_ALLOCATOR_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMVectorContainer.h"
#include "NMPlatform/NMStaticFreeList.h"
#include "windows.h"


//----------------------------------------------------------------------------------------------------------------------
/// \class ThreadSafeMemoryPool
/// \brief This class provides fixed-size pages of memory in a thread-safe manner.
///
//----------------------------------------------------------------------------------------------------------------------
class ThreadSafeMemoryPool
{
public:
  /// \brief Get the memory requirements for a ThreadSafeMemoryPool with the given number of pages
  static NMP::Memory::Format getMemoryRequirements(uint32_t numPages, uint32_t pageSize, uint32_t pageAlignment);

  /// \brief Initialise a ThreadSafeMemoryPool of the given size and alignment into the memory stored in resource.
  static ThreadSafeMemoryPool* init(NMP::Memory::Resource& resource, uint32_t numPages, uint32_t pageSize, uint32_t pageAlignment);

  //  \brief Clean up a ThreadSafeMemoryPool ready for deallocation.
  bool term();

  /// \brief Allocate some memory from this allocator.
  ///
  /// This function will return NULL if you do not specify the size and alignment that matches the page size and alignment.
  void* memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL);

  /// \brief Deallocate some memory from this allocator
  ///
  ///  This will return the page to the freelist.
  void memFree(void* ptr);

  // \brief Return the size of the managed pages of memory
  size_t getPageSize() {return m_freelist->m_entryFormat.size;}

protected:
  NMP::StaticFreeList* m_freelist;
  HANDLE m_mutex;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MultiThreadTempAllocator
/// \brief This memory allocator provides a set of pages of continuous memory. Allocations will increment the head 
///  pointer in this block.  When a page fills up, another one is allocated from a central store of pages in a thread-
///  safe manner.  This means that worker threads will only allocate pages on-demand, which can significantly reduce
///  overall memory use in a multi-threaded application.
///
///  Note: this allocator is deliberately quite simple so that it can be easily understood.  There are many ways in
///   which it's performance and memory usage efficiency could be improved.
//----------------------------------------------------------------------------------------------------------------------
class MultiThreadTempAllocator : public NMP::TempMemoryAllocator
{
public:

  /// \brief Allocate some memory from this allocator.
  virtual void* memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL);
  /// \brief Deallocate some memory from this allocator
  ///
  ///  This has no effect for this allocator, instead call reset() to deallocate all allocations from this allocator.
  virtual void memFree(void* ptr);

  /// \brief Get the memory requirements for a MultiThreadTempAllocator of the given size and alignment.
  static NM_INLINE NMP::Memory::Format getMemoryRequirements();
  /// \brief Initialise a MultiThreadTempAllocator of the given size and alignment into the memory stored in resource.
  static MultiThreadTempAllocator* init(NMP::Memory::Resource& resource, ThreadSafeMemoryPool* memoryPool);
  //  \brief Clean up a MultiThreadTempAllocator ready for deallocation.
  virtual bool term();

  /// \brief Create a child MultiThreadTempAllocator within this allocator.
  virtual TempMemoryAllocator *createChildAllocator();
  /// \brief Destroy this allocator's child allocator.  This will also effectively free all the memory that was
  ///  allocated.
  virtual void destroyChildAllocator(TempMemoryAllocator *bm);

  /// \brief Deallocate all memory allocated from this allocator.
  virtual uint32_t reset();
  /// \brief Get the total memory used since the last reset.
  virtual size_t getUsedBytes();

  /// \brief Get a pointer to the start of the memory block which this MultiThreadTempAllocator manages.
  NM_INLINE char* getMemoryBlock() { return m_memoryBlock; }
  /// \brief Get the size of the memory block which this FastHeapAllocator manages.
  NM_INLINE uint32_t getSize() { return m_size; }
  /// \brief Get a pointer to the start of the free space within this allocator's memory block.
  NM_INLINE char* getCurrentPtr() { return m_currentPtr; }

  char*     m_memoryBlock;
  uint32_t  m_size;
  char*     m_memoryEnd;
  char*     m_currentPtr;

  static const int MAX_FILLED_PAGES = 64;
  void*     m_filledPages[MAX_FILLED_PAGES];
  uint32_t  m_numFilledPages;

  /// \brief A pointer to the owning TempMemoryAllocator (if one exists)
  TempMemoryAllocator* m_parentAllocator;

  /// \brief A pointer to the child TempMemoryAllocator (if one exists)
  TempMemoryAllocator* m_childAllocator;

  /// \brief the threadsafe memory pool which we will draw our local memory pages from as they are required.
  ThreadSafeMemoryPool* m_memoryPool;
};

//----------------------------------------------------------------------------------------------------------------------
// MultiThreadTempAllocator functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format MultiThreadTempAllocator::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(MultiThreadTempAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#endif // MULTI_THREAD_TEMP_MEMORY_ALLOCATOR_H
//----------------------------------------------------------------------------------------------------------------------
