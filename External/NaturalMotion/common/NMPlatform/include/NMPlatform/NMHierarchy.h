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
#ifndef NM_HIERARCHY_H
#define NM_HIERARCHY_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

#define INVALID_HIERARCHY_INDEX (0xFFFFFFFF)

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::Hierarchy
/// \brief Stores an array of hierarchical parent child relationships between objects.
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class Hierarchy
{
public:
  /// \brief Calculate the required memory for a hierarchy with the specified number of nodes
  static NM_INLINE NMP::Memory::Format getMemoryRequirements(uint32_t numEntries);

  /// \brief Initialise the Hierarchy in the supplied memory resource.
  ///
  /// Increments resource pointer.
  static NM_INLINE Hierarchy* init(
    NMP::Memory::Resource& resource,
    uint32_t               numEntries,
    int32_t*               hierarchyBuffer = NULL);

  /// \brief Locate the hierarchy, fixing it in a location in memory
  NM_INLINE bool locate();

  /// \brief Dislocate the hierarchy, to prepare it for streaming out or moving in memory.
  NM_INLINE bool dislocate();

  /// \brief Re-seat a Hierarchy instance after copying to a new memory location.
  NM_INLINE void relocate();

  /// \brief Get the number of nodes in the hierarchy.
  NM_INLINE uint32_t getNumEntries() const;

  /// \brief Get the parent node index for a given node in the hierarchy.
  NM_INLINE int32_t getParentIndex(uint32_t index) const;

  /// \brief Get the pod hierarchy array
  NM_INLINE const int32_t* getHierarchyData() const;
  
  /// \brief Tests if a bone is a parent relation to another bone.
  NM_INLINE bool isParentOf(
    uint32_t parentIndex,   ///< The test bone to be the parent of the specified child bone.
                            ///< The function returns true if it is, and false otherwise.
    uint32_t childIndex     ///< The specified child bone
    ) const;

protected:
  // We don't allow default construction
  Hierarchy();
  ~Hierarchy();

  uint32_t  m_NumEntries;     ///< Num elements in hierarchy array.
  int32_t*  m_HierarchyArray; ///< Parent channel of each channel.
                              ///< Channels must be ordered such that parent channels
                              ///< always appear before their children.
                              ///< Provides for fast accumulation of transforms from
                              ///< local space to a common space such as world or object.
};

//----------------------------------------------------------------------------------------------------------------------
// Hierarchy functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format Hierarchy::getMemoryRequirements(uint32_t numEntries)
{
  // Hierarchy class itself.
  NMP::Memory::Format result(NMP::Memory::align(sizeof(Hierarchy), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);

  // Hierarchy index array.
  result += NMP::Memory::Format(NMP::Memory::align(sizeof(int32_t) * numEntries, NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Hierarchy* Hierarchy::init(NMP::Memory::Resource& resource, uint32_t numEntries, int32_t* hierarchyBuffer)
{
  NMP::Memory::Format format(NMP::Memory::align(sizeof(Hierarchy), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);

  // Hierarchy class itself.
  resource.align(format);
  Hierarchy* result = (Hierarchy*)resource.ptr;
  resource.increment(format);

  NMP_ASSERT(numEntries > 0);
  result->m_NumEntries = numEntries;

  // Hierarchy index array.
  format.set(NMP::Memory::align(sizeof(int32_t) * numEntries, NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  result->m_HierarchyArray = (int32_t*)resource.ptr;
  resource.increment(format);

  // Initialize hierarchy index values.
  if (hierarchyBuffer)
  {
    memcpy(result->m_HierarchyArray, hierarchyBuffer, sizeof(int32_t) * result->m_NumEntries);
  }
  else
  {
    for (uint32_t i = 0; i < result->m_NumEntries; i++)
    {
      result->m_HierarchyArray[i] = 0;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Hierarchy::locate()
{
  m_HierarchyArray = (int32_t*) (this + 1);
  NMP::endianSwap(m_NumEntries);
  NMP::endianSwapArray(m_HierarchyArray, m_NumEntries);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Hierarchy::dislocate()
{
  NMP::endianSwapArray(m_HierarchyArray, m_NumEntries);
  NMP::endianSwap(m_NumEntries);

  // For binary invariance. Will be fixed in locate()
  m_HierarchyArray = NULL;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Hierarchy::relocate()
{
  void* ptr = this;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(Hierarchy));
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_HierarchyArray = (int32_t*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t Hierarchy::getNumEntries() const
{
  return m_NumEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int32_t Hierarchy::getParentIndex(uint32_t index) const
{
  NMP_ASSERT(m_HierarchyArray);
  if (index >= m_NumEntries)
    return INVALID_HIERARCHY_INDEX;
  return m_HierarchyArray[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const int32_t* Hierarchy::getHierarchyData() const
{
  return m_HierarchyArray;
}

//----------------------------------------------------------------------------------------------------------------------
bool Hierarchy::isParentOf(
  uint32_t parentIndex,
  uint32_t childIndex) const
{
  NMP_ASSERT(parentIndex < m_NumEntries);
  NMP_ASSERT(childIndex < m_NumEntries);

  int32_t curParentIndex = (int32_t)childIndex;
  while (curParentIndex != (int32_t)INVALID_HIERARCHY_INDEX)
  {
    curParentIndex = getParentIndex(curParentIndex);
    if (curParentIndex == (int32_t)parentIndex)
      return true;
  }

  return false;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_HIERARCHY_H
//----------------------------------------------------------------------------------------------------------------------
