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
#include "NMPlatform/NMMemory.h"
#include "assetProcessor/AnimSource/Vector3TableBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// Vector3Table
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format Vector3Table::getMemoryRequirements(
  uint32_t numChannels,
  uint32_t numKeyFrames)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(Vector3Table), NMP_NATURAL_TYPE_ALIGNMENT);

  // Pointers array
  NMP::Memory::Format memReqsPtrs(sizeof(NMP::Vector3*) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += memReqsPtrs;

  // Channels
  NMP::Memory::Format memReqsChans(sizeof(NMP::Vector3) * numKeyFrames * numChannels, NMP_VECTOR_ALIGNMENT);
  memReqs += memReqsChans;

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
Vector3Table* Vector3Table::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numChannels,
  uint32_t                numKeyFrames)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(Vector3Table), NMP_NATURAL_TYPE_ALIGNMENT);
  Vector3Table* result = (Vector3Table*) resource.alignAndIncrement(memReqs);
  NMP_VERIFY(result);

  result->m_numChannels = numChannels;
  result->m_numKeyFrames = numKeyFrames;

  // Pointers array
  NMP::Memory::Format memReqsPtrs(sizeof(NMP::Vector3*) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_channels = (NMP::Vector3**) resource.alignAndIncrement(memReqsPtrs);

  // Channels
  NMP::Memory::Format memReqsChanStride(sizeof(NMP::Vector3) * numKeyFrames, NMP_VECTOR_ALIGNMENT);
  for (uint32_t i = 0; i < numChannels; ++i)
  {
    result->m_channels[i] = (NMP::Vector3*) resource.alignAndIncrement(memReqsChanStride);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3Table::setChannel(uint32_t channel, const NMP::Vector3* keys)
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(keys);
  NMP::Vector3* destChan = m_channels[channel];
  NMP_VERIFY(destChan);

  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    destChan[i] = keys[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3Table::getKeysAtFrame(uint32_t frame, NMP::Vector3* keys) const
{
  NMP_VERIFY(keys);
  NMP_VERIFY(frame < m_numKeyFrames);

  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    keys[i] = m_channels[i][frame];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3Table::setKeysAtFrame(uint32_t frame, const NMP::Vector3* keys)
{
  NMP_VERIFY(keys);
  NMP_VERIFY(frame < m_numKeyFrames);

  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    m_channels[i][frame] = keys[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3Table::getKey(uint32_t channel, uint32_t frame, NMP::Vector3& key) const
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(frame < m_numKeyFrames);
  key = m_channels[channel][frame];
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3Table::setKey(uint32_t channel, uint32_t frame, const NMP::Vector3& key)
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(frame < m_numKeyFrames);
  m_channels[channel][frame] = key;
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3Table::copy(const Vector3Table& src)
{
  NMP_VERIFY(m_numChannels == src.m_numChannels);
  NMP_VERIFY(m_numKeyFrames == src.m_numKeyFrames);

  if (&src != this)
  {
    for (uint32_t i = 0; i < m_numChannels; ++i)
    {
      const NMP::Vector3* srcKeys = src.m_channels[i];
      NMP::Vector3* dstKeys = m_channels[i];

      for (uint32_t k = 0; k < m_numKeyFrames; ++k)
      {
        dstKeys[k] = srcKeys[k];
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
Vector3Table& Vector3Table::operator=(const Vector3Table& src)
{
  copy(src);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3Table::writeChannel(uint32_t channelIndex, FILE* file) const
{
  NMP_VERIFY(file);
  const NMP::Vector3* chan = getChannel(channelIndex);

  fprintf(file, "---------------------------------------------------------------------\n");

  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    fprintf(
      file,
      "frame = %4d, vec = [%3.5f, %3.5f, %3.5f]\n",
      i,
      chan[i].x, chan[i].y, chan[i].z);
  }

  fprintf(file, "---------------------------------------------------------------------\n\n");
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3Table::writeFrame(uint32_t frame, FILE* file) const
{
  NMP_VERIFY(file);
  NMP::Vector3 vec;

  fprintf(file, "---------------------------------------------------------------------\n");

  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    getKey(i, frame, vec);

    fprintf(
      file,
      "chan = %4d, vec = [%3.5f, %3.5f, %3.5f]\n",
      i,
      vec.x, vec.y, vec.z);
  }

  fprintf(file, "---------------------------------------------------------------------\n\n");
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3Table::writeTableByChannels(FILE* file) const
{
  NMP_VERIFY(file);
  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    fprintf(file, "---------------------------------------------------------------------\n");
    fprintf(file, "Channel = %4d\n", i);

    writeChannel(i, file);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Vector3Table::writeTableByFrames(FILE* file) const
{
  NMP_VERIFY(file);
  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    fprintf(file, "---------------------------------------------------------------------\n");
    fprintf(file, "Frame = %4d\n", i);

    writeFrame(i, file);
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
