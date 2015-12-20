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
#ifndef NM_STATIC_FREELIST_H
#define NM_STATIC_FREELIST_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::StaticFreeList
/// \brief Manages a static free list of data items with specifiable memory requirements.  This free list cannot grow.
/// \ingroup NaturalMotionPlatform
///
/// All memory blocks are of the same size.
/// Memory allocations can allocate blocks larger than the data item required size in order to reduce the number of
/// allocations required.
/// Performance is faster than NMP::FreeList
//----------------------------------------------------------------------------------------------------------------------
class StaticFreeList
{
public:

  static NM_INLINE NMP::Memory::Format getMemoryRequirements(
    const NMP::Memory::Format& entryFormat,
    uint32_t                   numEntries);

  static NM_INLINE NMP::StaticFreeList* init(
    NMP::Memory::Resource&     resource,
    const NMP::Memory::Format& entryFormat,
    uint32_t                   numEntries);

  NM_INLINE void* allocateEntry();
  NM_INLINE bool  deallocateEntry(void* ptr);
  NM_INLINE bool  isEmpty() const { return (m_numFreeEntries == 0); }

  NM_INLINE bool memoryIsManagedByFreeList(void* ptr);

  NM_INLINE bool relocate();

  NMP::Memory::Format m_entryFormat;
  uint32_t            m_numEntries;
  uint32_t            m_numFreeEntries;

  void** m_freeEntries;
  void*  m_memory;
};

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format StaticFreeList::getMemoryRequirements(
  const NMP::Memory::Format& entryFormat,
  uint32_t                   numEntries)
{
  NMP::Memory::Format result(sizeof(StaticFreeList), NMP_NATURAL_TYPE_ALIGNMENT);

  result += NMP::Memory::Format(sizeof(void*) * numEntries, NMP_NATURAL_TYPE_ALIGNMENT);

  result += (entryFormat * numEntries);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::StaticFreeList* StaticFreeList::init(
  NMP::Memory::Resource&     resource,
  const NMP::Memory::Format& entryFormat,
  uint32_t                   numEntries)
{
  resource.align(NMP::Memory::Format(sizeof(StaticFreeList), NMP_NATURAL_TYPE_ALIGNMENT));
  StaticFreeList* result = (StaticFreeList*)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(StaticFreeList), NMP_NATURAL_TYPE_ALIGNMENT));

  result->m_entryFormat = entryFormat;
  result->m_numEntries = numEntries;
  result->m_numFreeEntries = numEntries;

  resource.align(NMP::Memory::Format(sizeof(void*) * numEntries, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_freeEntries = (void**)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(void*) * numEntries, NMP_NATURAL_TYPE_ALIGNMENT));

  NMP::Memory::Format memBlockFmt(entryFormat * numEntries);
  resource.align(memBlockFmt);
  result->m_memory = (void*)resource.ptr;

  // Initialize the array of free entries
  ptrdiff_t stepSize = NMP::Memory::align(entryFormat.size, entryFormat.alignment);
  char* currentPtr = (char*)result->m_memory;
  for (uint32_t i = 0; i < numEntries; i++)
  {
    result->m_freeEntries[i] = (void*)currentPtr;
    currentPtr += stepSize;
  }

  resource.increment(memBlockFmt);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void* StaticFreeList::allocateEntry()
{
  if (!m_numFreeEntries)
  {
    return NULL;
  }

  return m_freeEntries[--m_numFreeEntries];
}

//----------------------------------------------------------------------------------------------------------------------
bool StaticFreeList::memoryIsManagedByFreeList(void* ptr)
{
  return ((ptr >= m_memory) && (ptr < ((char*)m_memory + NMP::Memory::align(m_entryFormat.size, m_entryFormat.alignment) * m_numEntries)));
}

//----------------------------------------------------------------------------------------------------------------------
bool StaticFreeList::deallocateEntry(void* ptr)
{
  // Make sure this pointer is inside the memory block we're managing!
  NMP_ASSERT(memoryIsManagedByFreeList(ptr));

  m_freeEntries[m_numFreeEntries++] = ptr;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool StaticFreeList::relocate()
{
  NMP::Memory::Resource resource = { this, getMemoryRequirements(m_entryFormat, m_numEntries) };

  resource.increment(NMP::Memory::Format(sizeof(StaticFreeList), NMP_NATURAL_TYPE_ALIGNMENT));

  resource.align(NMP::Memory::Format(sizeof(void*) * m_numEntries, NMP_NATURAL_TYPE_ALIGNMENT));
  m_freeEntries = (void**)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(void*) * m_numEntries, NMP_NATURAL_TYPE_ALIGNMENT));

  NMP::Memory::Format memBlockFmt(m_entryFormat * m_numEntries);
  resource.align(memBlockFmt);
  char* oldMemory = (char*)m_memory;
  m_memory = (void*)resource.ptr;

  // relocate the array of free entries
  ptrdiff_t addressDelta = (char*)m_memory - oldMemory;
  for (uint32_t i = 0; i < m_numFreeEntries; i++)
  {
    m_freeEntries[i] = (void*)((char*)m_freeEntries[i] + addressDelta);
  }

  return true;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_STATIC_FREELIST_H
//----------------------------------------------------------------------------------------------------------------------
