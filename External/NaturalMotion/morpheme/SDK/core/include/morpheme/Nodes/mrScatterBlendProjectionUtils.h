// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_SCATTER_BLEND_PROJECTION_UTILS_H
#define MR_SCATTER_BLEND_PROJECTION_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ScatterBlendProjectionClosestFacetSubSampleBuffer
/// \brief A buffer to score the closest matches when projecting back onto the annotation
//----------------------------------------------------------------------------------------------------------------------
class ScatterBlendProjectionClosestFacetSubSampleBuffer
{
public:
  class Entry
  {
  public:
    uint16_t  m_extIndex;
    uint16_t  m_subDivIndex;
    float     m_distanceSq;
  };

public:
  static NMP::Memory::Format getMemoryRequirements(
    uint16_t bufferSize);

  static ScatterBlendProjectionClosestFacetSubSampleBuffer* init(
    NMP::Memory::Resource& resource,
    uint16_t bufferSize);

  NM_INLINE void reset();

  NM_INLINE uint16_t getNumEntries() const;

  NM_INLINE void addBufferEntry(
    uint16_t extIndex,
    uint16_t subDivIndex,
    float distanceSq);

  NM_INLINE const Entry* getSortedEntry(
    uint32_t index) const;

  void partialSort(
    uint16_t numSortedEntries);

  void sort1();

private:
  uint16_t  m_bufferSize;
  uint16_t  m_numEntries;
  Entry*    m_entryBuffer;
  Entry**   m_sortedBuffer;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ScatterBlendProjectionClosestFacetSubSampleBuffer::reset()
{
  m_numEntries = 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint16_t ScatterBlendProjectionClosestFacetSubSampleBuffer::getNumEntries() const
{
  return m_numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void ScatterBlendProjectionClosestFacetSubSampleBuffer::addBufferEntry(
  uint16_t  extIndex,
  uint16_t  subDivIndex,
  float     distanceSq)
{
  NMP_ASSERT(m_numEntries < m_bufferSize);
  Entry& entry = m_entryBuffer[m_numEntries];
  entry.m_extIndex = extIndex;
  entry.m_subDivIndex = subDivIndex;
  entry.m_distanceSq = distanceSq;
  ++m_numEntries;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const ScatterBlendProjectionClosestFacetSubSampleBuffer::Entry*
ScatterBlendProjectionClosestFacetSubSampleBuffer::getSortedEntry(uint32_t index) const
{
  NMP_ASSERT(index < m_numEntries);
  return m_sortedBuffer[index];
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_SCATTER_BLEND_PROJECTION_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
