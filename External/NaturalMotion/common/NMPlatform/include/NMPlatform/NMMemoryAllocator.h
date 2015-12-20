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
#ifndef NMP_MEMORY_ALLOCATOR_H
#define NMP_MEMORY_ALLOCATOR_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MemoryAllocator
/// \brief Pure Virtual interface to memory allocator class.  This allows containers and other classes who are clients
///  of memory allocators to interface with them.
//----------------------------------------------------------------------------------------------------------------------
class MemoryAllocator
{
public:
  MemoryAllocator();
  virtual ~MemoryAllocator();

  NM_INLINE NMP::Memory::Resource allocateFromFormat(NMP::Memory::Format format NMP_MEMORY_TRACKING_DECL);
  virtual void* memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL) = 0;
  virtual void memFree(void* ptr) = 0;
  virtual bool term() = 0;

  NM_INLINE void statReset()
  {
    resetPartialInternal();
    resetPartialExternal();
  }

protected:

  // When NMP_MEMORY_STATS is defined, allocators keep track of several allocation stats.
  // There are four types of stats, based on two aspects: allocations/deallocations can be either
  // internal or external, and they can be total or partial.
  // External allocations/deallocations are those requested by the user of the allocator through
  // its interface.
  // Internal allocations are those that the allocator requests from the underlying memory system
  // (through NMPMemoryAllocate* macros usually). These include allocations made to serve external
  // allocations, as well as those needed to maintain any internal structures that the allocator
  // itself uses.
  // The total stats keep track of the use of the allocator since its creation, while partial
  // stats can be reset at interesting intervals (every frame, every time a new level is loaded...).
  // The stats keep track of the number of allocations, allocated bytes, number of deallocations and
  // deallocated bytes.
  // These provide a low overload insight into the use of the allocators.
  // They allow detecting leaks that would go unnoticed otherwise (because the system memory is freed
  // when the allocator is destroyed, for example), or an excessive number of allocations.
  // It's important, to get reliable results, to be consistent when calling log*Alloc and log*Free.
  // For internal allocations it's advised to use NMP::Memory::memSize for both, instead of relying
  // on the requested size when allocating. For external allocations, each allocator has to make sure that
  // the same amount is passed on matching alloc/free calls.
  NM_INLINE void initStats()
  {
    m_partialInternal.reset();
    m_partialExternal.reset();
    m_totalInternal.reset();
    m_totalExternal.reset();
  }

  NM_INLINE void logInternalAlloc(size_t size)
  {
    m_partialInternal.logAlloc(size);
    m_totalInternal.logAlloc(size);
  }

  NM_INLINE void logInternalFree(size_t size)
  {
    m_partialInternal.logFree(size);
    m_totalInternal.logFree(size);
  }

  NM_INLINE const Memory::Stats& getPartialInternalStats() const
  {
    return m_partialInternal;
  }

  NM_INLINE const Memory::Stats& getTotalInternalStats() const
  {
    return m_totalInternal;
  }

  NM_INLINE void resetPartialInternal()
  {
    m_partialInternal.reset();
  }

  NM_INLINE void logExternalAlloc(size_t size)
  {
    m_partialExternal.logAlloc(size);
    m_totalExternal.logAlloc(size);
  }

  NM_INLINE void logExternalFree(size_t size)
  {
    m_partialExternal.logAlloc(size);
    m_totalExternal.logAlloc(size);
  }

  NM_INLINE const Memory::Stats& getPartialExternalStats() const
  {
    return m_partialExternal;
  }

  NM_INLINE const Memory::Stats& getTotalExternalStats() const
  {
    return m_totalExternal;
  }

  NM_INLINE void resetPartialExternal()
  {
    m_partialExternal.reset();
  }

private:
  NMP::Memory::Stats m_totalInternal;
  NMP::Memory::Stats m_totalExternal;
  NMP::Memory::Stats m_partialInternal;
  NMP::Memory::Stats m_partialExternal;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class TempAllocator
/// \brief This interface provides additional API for allocators which are designed to be used as temporary memory
///   allocators in the morpheme runtime.  The interface adds support for resetting the allocator, which discards all
///   the memory that has been allocated.  It also provides support for creating and destroying sub allocators.  This
///   allows an application to create a child allocator during execution, and then allocate an amount of scratch-pad
///   memory.  Once the application has finished with the scratch-pad memory, it can destroy the child allocator,
///   freeing up the memory to be re-used.  This allows applications to minimise their memory footprint and is
///   especially useful when a large block of working memory is required for a short period of time (such as within an
///   IK or retargetting solver).
///
///  \warning  If a child allocator exists, then it is only valid to allocate from the most recently created child.  An
///             allocator can only have one child at once, but children can be created recursively.
///
///  \warning  It is only valid to allocate directly from this allocator when there are no child allocators.
//----------------------------------------------------------------------------------------------------------------------
class TempMemoryAllocator : public MemoryAllocator
{
public:
  /// \brief Deallocate all memory allocated by this allocator.
  /// \return The total number of bytes allocated since the last reset.
  virtual uint32_t reset() = 0;

  /// \brief get the number of bytes allocated so far by this allocator.
  virtual size_t getUsedBytes() = 0;

  /// \brief Insert a child allocator within this allocator.
  ///
  /// The child allocator can then be allocated from. At a later time the child can be removed with
  /// destroyChildAllocator() and all the allocations made from that child will be rolled back and freed.  Once the 
  /// child has been created, you cannot allocate memory from it's parent, or reset it's parent, until the child is
  /// destroyed.
  ///
  /// \return a pointer to the inserted child allocator.
  virtual TempMemoryAllocator *createChildAllocator() = 0;

  /// \brief Destroy a previously created child allocator. This will deallocate all memory allocated by the child. Once
  ///  you have called destroyChildAllocator, you can make use of the parent allocator again.
  virtual void destroyChildAllocator(TempMemoryAllocator *bm) = 0;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class HeapAllocator
/// \brief This memory allocator allocates directly from the system heap using malloc/free
//----------------------------------------------------------------------------------------------------------------------
class HeapAllocator : public MemoryAllocator
{
public:
  void* memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL);
  void memFree(void* ptr);
  bool term();

  static NM_INLINE NMP::Memory::Format getMemoryRequirements();
  static NM_INLINE HeapAllocator* init(NMP::Memory::Resource& resource);
};

//----------------------------------------------------------------------------------------------------------------------
// MemoryAllocator functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource MemoryAllocator::allocateFromFormat(NMP::Memory::Format format NMP_MEMORY_TRACKING_DECL)
{
  NMP::Memory::Resource result;
  result.format = format;
  result.ptr = memAlloc(format.size, (uint32_t)format.alignment NMP_MEMORY_TRACKING_PASS_THROUGH);
  NMP_ASSERT(result.ptr);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// HeapAllocator functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format HeapAllocator::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(HeapAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
HeapAllocator* HeapAllocator::init(NMP::Memory::Resource& resource)
{
  NMP::Memory::Format format(sizeof(HeapAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  // Set up the vtable by calling in place constructor
  HeapAllocator* result = new(resource.ptr) HeapAllocator();
  resource.increment(format);

  result->initStats();

  return result;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_MEMORY_ALLOCATOR_H
//----------------------------------------------------------------------------------------------------------------------
