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
#include "assetProcessor/AnimSource/IntVector3TableBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// IntVector3Table
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format IntVector3Table::getMemoryRequirements(
  uint32_t numChannels,
  uint32_t numKeyFrames)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(IntVector3Table), NMP_NATURAL_TYPE_ALIGNMENT);

  // Pointers array
  NMP::Memory::Format memReqsPtrs(sizeof(int32_t*) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += (memReqsPtrs * 3);

  // Keys
  NMP::Memory::Format memReqsComponent(sizeof(int32_t) * numKeyFrames * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += (memReqsComponent * 3);

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
IntVector3Table* IntVector3Table::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numChannels,
  uint32_t                numKeyFrames)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(IntVector3Table), NMP_NATURAL_TYPE_ALIGNMENT);
  IntVector3Table* result = (IntVector3Table*) resource.alignAndIncrement(memReqs);
  NMP_VERIFY(result);

  result->m_numChannels = numChannels;
  result->m_numKeyFrames = numKeyFrames;
  
  // Pointers array
  NMP::Memory::Format memReqsPtrs(sizeof(int32_t*) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_channelsX = (int32_t**) resource.alignAndIncrement(memReqsPtrs);
  result->m_channelsY = (int32_t**) resource.alignAndIncrement(memReqsPtrs);
  result->m_channelsZ = (int32_t**) resource.alignAndIncrement(memReqsPtrs);
  
  // Keys
  NMP::Memory::Format memReqsKeyStride(sizeof(int32_t) * numKeyFrames, NMP_NATURAL_TYPE_ALIGNMENT);
  // X component
  for (uint32_t iChan = 0; iChan < numChannels; ++iChan)
  {
    result->m_channelsX[iChan] = (int32_t*) resource.alignAndIncrement(memReqsKeyStride);
  }
  // Y component
  for (uint32_t iChan = 0; iChan < numChannels; ++iChan)
  {
    result->m_channelsY[iChan] = (int32_t*) resource.alignAndIncrement(memReqsKeyStride);
  }
  // Z component
  for (uint32_t iChan = 0; iChan < numChannels; ++iChan)
  {
    result->m_channelsZ[iChan] = (int32_t*) resource.alignAndIncrement(memReqsKeyStride);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void IntVector3Table::getKey(uint32_t channel, uint32_t frame, int32_t* key) const
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(frame < m_numKeyFrames);
  NMP_VERIFY(key);
  key[0] = m_channelsX[channel][frame];
  key[1] = m_channelsY[channel][frame];
  key[2] = m_channelsZ[channel][frame];
}

//----------------------------------------------------------------------------------------------------------------------
void IntVector3Table::setKey(uint32_t channel, uint32_t frame, const int32_t* key)
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(frame < m_numKeyFrames);
  NMP_VERIFY(key);
  m_channelsX[channel][frame] = key[0];
  m_channelsY[channel][frame] = key[1];
  m_channelsZ[channel][frame] = key[2];
}

//----------------------------------------------------------------------------------------------------------------------
bool IntVector3Table::isChannelUnchanging(uint32_t channel, int32_t eps) const
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(eps >= 0);
  if (m_numKeyFrames <= 1)
    return true;

  const int32_t* keysX = m_channelsX[channel];
  NMP_VERIFY(keysX);
  const int32_t* keysY = m_channelsY[channel];
  NMP_VERIFY(keysY);
  const int32_t* keysZ = m_channelsZ[channel];
  NMP_VERIFY(keysZ);

  for (uint32_t i = 1; i < m_numKeyFrames; ++i)
  {
    int32_t dx = keysX[i] - keysX[0];
    if (dx < 0) dx = -dx;
    int32_t dy = keysY[i] - keysY[0];
    if (dy < 0) dy = -dy;
    int32_t dz = keysZ[i] - keysZ[0];
    if (dz < 0) dz = -dz;

    if (dx > eps || dy > eps || dz > eps)
    {
      return false;
    }
  }

  return true;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
