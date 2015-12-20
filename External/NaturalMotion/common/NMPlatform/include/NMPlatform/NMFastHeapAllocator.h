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
#ifndef NMP_FAST_HEAP_MEMORY_ALLOCATOR_H
#define NMP_FAST_HEAP_MEMORY_ALLOCATOR_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMVectorContainer.h"

#if defined(NM_HOST_CELL_SPU) && defined(NM_DEBUG)
  #include "spu_printf.h"
#endif
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class FastHeapAllocator
/// \brief This memory allocator provides a single continuous memory block.
///  Allocations will increment the head pointer in this block
///
///  It is not possible to free individual allocations, but the entire heap can be wiped down by simply
///  winding down the head pointer to the start of the heap again.  This memory allocator is useful for a large number
///  of small allocations that are only required for a short time (i.e. one frame).
///
///  This allocator implements the TempMemoryAllocator interface and therefore supports child allocators.
///
//----------------------------------------------------------------------------------------------------------------------
class FastHeapAllocator : public TempMemoryAllocator
{
public:

  /// \brief Allocate some memory from this allocator.
  virtual void* memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL);
  /// \brief Deallocate some memory from this allocator
  ///
  ///  This has no effect for this allocator, instead call reset() to deallocate all allocations from this allocator.
  virtual void memFree(void* ptr);

  /// \brief Get the memory requirements for a FastHeapAllocator of the given size and alignment.
  static NM_INLINE NMP::Memory::Format getMemoryRequirements(uint32_t size, uint32_t alignment);
  /// \brief Initialise a FastHeapAllocator of the given size and alignment into the memory stored in resource.
  static FastHeapAllocator* init(NMP::Memory::Resource& resource, uint32_t size, uint32_t alignment);
  //  \brief Clean up a FastHeapAllocator ready for deallocation.
  virtual bool term();

  /// \brief Create a child FastHeapAllocator within this allocator.
  virtual TempMemoryAllocator *createChildAllocator();
  /// \brief Destroy this allocator's child allocator.  This will also effectively free all the memory that was
  ///  allocated.
  virtual void destroyChildAllocator(TempMemoryAllocator *bm);

  /// \brief Deallocate all memory allocated from this allocator.
  virtual uint32_t reset();
  /// \brief Get the total memory used since the last reset.
  virtual size_t getUsedBytes();

  /// \brief Get a pointer to the start of the memory block which this FastHeapAllocator manages.
  NM_INLINE char* getMemoryBlock() { return m_memoryBlock; }
  /// \brief Get the size of the memory block which this FastHeapAllocator manages.
  NM_INLINE uint32_t getSize() { return m_size; }
  /// \brief Get a pointer to the start of the free space within this allocator's memory block.
  NM_INLINE char* getCurrentPtr() { return m_currentPtr; }

  /// \brief Print a list of all the allocations made since the last reset.
  void printAllocations();

protected:
  /// \brief Get the memory requirements for a FastHeapAllocator of the given size and alignment, which is a child of
  ///  and existing allocator
  static NM_INLINE NMP::Memory::Format getChildAllocatorMemoryRequirements(uint32_t size, uint32_t alignment);

  /// \brief Initialise a FastHeapAllocator of the given size and alignment into the memory stored in resource, which is
  ///  a child of an existing allocator.
  static FastHeapAllocator* initChildAllocator(NMP::Memory::Resource& resource, uint32_t size, uint32_t alignment);

public:
#ifdef NMP_MEMORY_LOGGING
  static const uint32_t LOGGING_ALLOC_MAX_RECORDS = 1024;
  static const uint32_t LOGGING_ALLOC_MAX_ALLOCS = 1024 * 256;

  struct LoggedAllocation
  {
    void*       allocPtr;
    uint32_t    formatEntryIndex;
    const char* file;
    uint32_t    line;
    const char* function;
  };

  struct LoggedFormatAllocCount
  {
    NMP::Memory::Format format;
    uint32_t count;
  };
#endif //NMP_MEMORY_LOGGING

  char*     m_memoryBlock;
  uint32_t  m_size;
  char*     m_memoryEnd;
  char*     m_currentPtr;

#ifdef NMP_MEMORY_LOGGING
  uint32_t                                m_maxAllocated;
  NMP::VectorContainer<LoggedAllocation>* m_currentAllocations;
  LoggedFormatAllocCount                  m_currentAllocCount[LOGGING_ALLOC_MAX_RECORDS];
  uint32_t                                m_numRecords;
#endif //NMP_MEMORY_LOGGING

  /// \brief A pointer to the owning TempMemoryAllocator (if one exists)
  TempMemoryAllocator *m_parentAllocator;

  /// \brief A pointer to the child TempMemoryAllocator (if one exists)
  TempMemoryAllocator *m_childAllocator;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class SPUFastHeapAllocator
/// \brief SPU only implementation of FastHeapAllocator.
/// This allocator does extra error checking when SPU_VERBOSE is enabled, to make sure that allocation has happened
/// within the limits of the heap and is not overrunning the stack.
/// This has been implemented as a separate class rather than being optionally compiled into FastHeapAllocator because
/// the PSU uses a FastHeapAllocator to allocate space on main memory, which can obviously safely allocate beyond the
/// 256k limit off an SPU, as well as an SPUFastHeapAllocator that checks for SPU limits.
//----------------------------------------------------------------------------------------------------------------------
#if defined(NM_HOST_CELL_SPU)
class SPUFastHeapAllocator : public FastHeapAllocator
{
public:

  static NM_INLINE SPUFastHeapAllocator* init(NMP::Memory::Resource& resource, uint32_t size, uint32_t alignment);

  virtual void* memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL);
};

//----------------------------------------------------------------------------------------------------------------------
// SPUFastHeapAllocator functions
//----------------------------------------------------------------------------------------------------------------------
SPUFastHeapAllocator* SPUFastHeapAllocator::init(NMP::Memory::Resource& resource, uint32_t size, uint32_t alignment)
{
  NMP::Memory::Format format(sizeof(SPUFastHeapAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  // Set up the vtable by calling in place constructor
  SPUFastHeapAllocator* result = new(resource.ptr) SPUFastHeapAllocator();
  resource.increment(format);

  format.size = size;
  format.alignment = alignment;

  resource.align(format);
  result->m_memoryBlock = (char*)resource.ptr;
  result->m_currentPtr = result->m_memoryBlock;
  result->m_memoryEnd = result->m_memoryBlock + size;
  result->m_size = size;
  result->m_parentAllocator = NULL;
  result->m_childAllocator = NULL;
  resource.increment(format);

  #ifdef NMP_MEMORY_LOGGING
  result->m_currentAllocations = NMP::VectorContainer<LoggedAllocation>::init(resource, LOGGING_ALLOC_MAX_ALLOCS);
  result->m_numRecords = 0;
  result->m_maxAllocated = 0;
  #endif // NMP_MEMORY_LOGGING

  return result;
}
#endif // defined(NM_HOST_CELL_SPU)

//----------------------------------------------------------------------------------------------------------------------
// FastHeapAllocator functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format FastHeapAllocator::getMemoryRequirements(uint32_t size, uint32_t alignment)
{
  NMP::Memory::Format result(sizeof(FastHeapAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  result += NMP::Memory::Format(size, alignment);

#ifdef NMP_MEMORY_LOGGING
  result += NMP::VectorContainer<LoggedAllocation>::getMemoryRequirements(LOGGING_ALLOC_MAX_ALLOCS);
#endif

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format FastHeapAllocator::getChildAllocatorMemoryRequirements(uint32_t size, uint32_t alignment)
{
  NMP::Memory::Format result(sizeof(FastHeapAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  result += NMP::Memory::Format(size, alignment);

  return result;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_FAST_HEAP_MEMORY_ALLOCATOR_H
//----------------------------------------------------------------------------------------------------------------------
