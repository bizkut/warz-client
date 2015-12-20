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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMvpu.h"
#include "morpheme/Nodes/mrScatterBlendProjectionUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ScatterBlendProjectionClosestFacetSubSampleBuffer::getMemoryRequirements(
  uint16_t bufferSize)
{
  NMP_ASSERT(bufferSize > 0);
  NMP::Memory::Format result(sizeof(ScatterBlendProjectionClosestFacetSubSampleBuffer), NMP_NATURAL_TYPE_ALIGNMENT);

  uint32_t bufferSize4 = NMP::nmAlignedValue4(bufferSize);
  NMP::Memory::Format memReqsEntryBuffer(sizeof(ScatterBlendProjectionClosestFacetSubSampleBuffer::Entry) * bufferSize4, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsEntryBuffer;

  NMP::Memory::Format memReqsSortedBuffer(sizeof(Entry*) * bufferSize4, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsSortedBuffer;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
ScatterBlendProjectionClosestFacetSubSampleBuffer* ScatterBlendProjectionClosestFacetSubSampleBuffer::init(
  NMP::Memory::Resource& resource,
  uint16_t bufferSize)
{
  NMP_ASSERT(bufferSize > 0);
  NMP::Memory::Format memReqsHdr(sizeof(ScatterBlendProjectionClosestFacetSubSampleBuffer), NMP_NATURAL_TYPE_ALIGNMENT);
  ScatterBlendProjectionClosestFacetSubSampleBuffer* result = (ScatterBlendProjectionClosestFacetSubSampleBuffer*)resource.alignAndIncrement(memReqsHdr);

  result->m_bufferSize = bufferSize;
  result->m_numEntries = 0;

  uint32_t bufferSize4 = NMP::nmAlignedValue4(bufferSize);
  NMP::Memory::Format memReqsEntryBuffer(sizeof(ScatterBlendProjectionClosestFacetSubSampleBuffer::Entry) * bufferSize4, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_entryBuffer = (ScatterBlendProjectionClosestFacetSubSampleBuffer::Entry*)resource.alignAndIncrement(memReqsEntryBuffer);

  NMP::Memory::Format memReqsSortedBuffer(sizeof(Entry*) * bufferSize4, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_sortedBuffer = (ScatterBlendProjectionClosestFacetSubSampleBuffer::Entry**)resource.alignAndIncrement(memReqsSortedBuffer);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ScatterBlendProjectionClosestFacetSubSampleBuffer::partialSort(uint16_t numSortedEntries)
{
  NMP_ASSERT(numSortedEntries > 0);
  NMP_ASSERT(numSortedEntries <= m_numEntries);

  // Initialise the sorted buffer
  for (uint16_t i = 0; i < m_numEntries; ++i)
    m_sortedBuffer[i] = &m_entryBuffer[i];

  // Initialise the heap buffer
  ScatterBlendProjectionClosestFacetSubSampleBuffer::Entry** heap = &m_sortedBuffer[numSortedEntries];
  uint16_t heapSize = m_numEntries - numSortedEntries;

  // Perform an initial exchange sort
  for (uint16_t i = 0; i < numSortedEntries - 1; ++i)
  {
    for (uint16_t j = i + 1; j < numSortedEntries; ++j)
    {
      if (m_sortedBuffer[j]->m_distanceSq < m_sortedBuffer[i]->m_distanceSq)
      {
        NMP::nmSwap(m_sortedBuffer[i], m_sortedBuffer[j]);
      }
    }
  }

  // Heap sort the remaining unsorted entries
  while (heapSize > 0)
  {
    // Get the new pivot value
    float pivot = m_sortedBuffer[numSortedEntries - 1]->m_distanceSq;

    // Sift the heap for entries less than the pivot value
    uint16_t newHeapSize = 0;
    for (uint16_t i = 0; i < heapSize; ++i)
    {
      if (heap[i]->m_distanceSq < pivot)
      {
        heap[newHeapSize] = heap[i];
        ++newHeapSize;
      }
    }

    // Check for termination
    if (newHeapSize == 0)
      break;

    // Exchange the pivot with the last entry in the heap and update the sorted matches via bubble sort
    heapSize = newHeapSize - 1;
    m_sortedBuffer[numSortedEntries - 1] = heap[heapSize];
    for (uint16_t i = numSortedEntries - 1; i > 0; --i)
    {
      if (m_sortedBuffer[i]->m_distanceSq < m_sortedBuffer[i - 1]->m_distanceSq)
      {
        NMP::nmSwap(m_sortedBuffer[i - 1], m_sortedBuffer[i]);
      }
      else
      {
        break;
      }
    }
  }

  // Pad to a block of 4 for SIMD
  Entry* ptr = m_sortedBuffer[numSortedEntries - 1];
  for (uint16_t i = numSortedEntries; i & 0x03; ++i)
    m_sortedBuffer[i] = ptr;
}

//----------------------------------------------------------------------------------------------------------------------
void ScatterBlendProjectionClosestFacetSubSampleBuffer::sort1()
{
  NMP_ASSERT(m_numEntries > 0);
  m_sortedBuffer[0] = &m_entryBuffer[0];
  for (uint16_t i = 1; i < m_numEntries; ++i)
  {
    if (m_entryBuffer[i].m_distanceSq < m_sortedBuffer[0]->m_distanceSq)
    {
      m_sortedBuffer[0] = &m_entryBuffer[i];
    }
  }

  // Pad to a block of 4 for SIMD
  m_sortedBuffer[1] = m_sortedBuffer[0];
  m_sortedBuffer[2] = m_sortedBuffer[0];
  m_sortedBuffer[3] = m_sortedBuffer[0];
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
