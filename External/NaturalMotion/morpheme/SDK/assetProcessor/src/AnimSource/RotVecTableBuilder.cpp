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
#include "assetProcessor/AnimSource/RotVecTableBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// RotVecTable
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format RotVecTable::getMemoryRequirements(
  uint32_t numChannels,
  uint32_t numKeyFrames)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(RotVecTable), NMP_NATURAL_TYPE_ALIGNMENT);

  // Pointers array
  NMP::Memory::Format memReqsPtrsWNeg(sizeof(uint32_t*) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += memReqsPtrsWNeg;
  NMP::Memory::Format memReqsPtrsV(sizeof(NMP::Vector3*) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += memReqsPtrsV;

  // W negative flags
  NMP::Memory::Format memReqsWNeg(sizeof(uint32_t) * numKeyFrames * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += memReqsWNeg;

  // Channels
  NMP::Memory::Format memReqsChans(sizeof(NMP::Vector3) * numKeyFrames * numChannels, NMP_VECTOR_ALIGNMENT);
  memReqs += memReqsChans;

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
RotVecTable* RotVecTable::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numChannels,
  uint32_t                numKeyFrames)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(RotVecTable), NMP_NATURAL_TYPE_ALIGNMENT);
  RotVecTable* result = (RotVecTable*) resource.alignAndIncrement(memReqs);
  NMP_VERIFY(result);

  // Header
  result->m_numChannels = numChannels;
  result->m_numKeyFrames = numKeyFrames;

  // Pointers array
  NMP::Memory::Format memReqsPtrsWNeg(sizeof(uint32_t*) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_channelsWNeg = (uint32_t**) resource.alignAndIncrement(memReqsPtrsWNeg);
  NMP::Memory::Format memReqsPtrsV(sizeof(NMP::Vector3*) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_channels = (NMP::Vector3**) resource.alignAndIncrement(memReqsPtrsV);

  // W negative flags
  NMP::Memory::Format memReqsWNegStride(sizeof(uint32_t) * numKeyFrames, NMP_NATURAL_TYPE_ALIGNMENT);
  for (uint32_t i = 0; i < numChannels; ++i)
  {
    result->m_channelsWNeg[i] = (uint32_t*) resource.alignAndIncrement(memReqsWNegStride);
  }

  // Channels
  NMP::Memory::Format memReqsChansStride(sizeof(NMP::Vector3) * numKeyFrames, NMP_VECTOR_ALIGNMENT);
  for (uint32_t i = 0; i < numChannels; ++i)
  {
    result->m_channels[i] = (NMP::Vector3*) resource.alignAndIncrement(memReqsChansStride);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float RotVecTable::getWNegSign(uint32_t wNeg)
{
  if (wNeg == 0)
    return 1.0f;
  return -1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::fromRotationVector(
  NMP::Quat&          quat,
  const NMP::Vector3& v,
  uint32_t            wNeg,
  bool                tanQuarterAngle)
{
  quat.fromRotationVector(v, tanQuarterAngle);
  quat *= getWNegSign(wNeg);
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::initChannel(uint32_t channel, const NMP::Quat* quatKeys, bool tanQuarterAngle)
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(quatKeys);
  NMP::Vector3* dst = m_channels[channel];
  NMP_VERIFY(dst);
  uint32_t* dstWNeg = m_channelsWNeg[channel];
  NMP_VERIFY(dstWNeg);

  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    dst[i] = quatKeys[i].toRotationVector(tanQuarterAngle);

    if (quatKeys[i].w < 0.0f)
      dstWNeg[i] = 1;
    else
      dstWNeg[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::setKey(uint32_t channel, uint32_t frame, const NMP::Quat& key, bool tanQuarterAngle)
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(frame < m_numKeyFrames);
  m_channels[channel][frame] = key.toRotationVector(tanQuarterAngle);

  if (key.w < 0.0f)
    m_channelsWNeg[channel][frame] = 1;
  else
    m_channelsWNeg[channel][frame] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool RotVecTable::isChannelUnchanging(uint32_t channel, float eps) const
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(eps >= 0.0f);
  if (m_numKeyFrames <= 1)
    return true;

  const NMP::Vector3* keys = m_channels[channel];
  NMP_VERIFY(keys);
  NMP::Vector3 dv;
  for (uint32_t i = 1; i < m_numKeyFrames; ++i)
  {
    dv = keys[i] - keys[0];
    dv.x = fabs(dv.x);
    dv.y = fabs(dv.y);
    dv.z = fabs(dv.z);
    if (dv.x > eps || dv.y > eps || dv.z > eps)
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::setChannel(uint32_t channel, const NMP::Vector3* keys)
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(keys);
  NMP::Vector3* dst = m_channels[channel];
  NMP_VERIFY(dst);
  uint32_t* dstWNeg = m_channelsWNeg[channel];
  NMP_VERIFY(dstWNeg);

  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    dst[i] = keys[i];
    dstWNeg[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::getKeysAtFrame(uint32_t frame, NMP::Vector3* keys) const
{
  NMP_VERIFY(keys);
  NMP_VERIFY(frame < m_numKeyFrames);

  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    keys[i] = m_channels[i][frame];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::setKeysAtFrame(uint32_t frame, const NMP::Vector3* keys)
{
  NMP_VERIFY(keys);
  NMP_VERIFY(frame < m_numKeyFrames);

  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    m_channels[i][frame] = keys[i];
    m_channelsWNeg[i][frame] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::getKey(uint32_t channel, uint32_t frame, NMP::Vector3& key) const
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(frame < m_numKeyFrames);
  key = m_channels[channel][frame];
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::getKey(uint32_t channel, uint32_t frame, NMP::Vector3& key, uint32_t& wNeg) const
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(frame < m_numKeyFrames);
  key = m_channels[channel][frame];
  wNeg = m_channelsWNeg[channel][frame];
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::setKey(uint32_t channel, uint32_t frame, const NMP::Vector3& key)
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(frame < m_numKeyFrames);
  m_channels[channel][frame] = key;
  m_channelsWNeg[channel][frame] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::setKey(uint32_t channel, uint32_t frame, const NMP::Vector3& key, uint32_t wNeg)
{
  NMP_VERIFY(channel < m_numChannels);
  NMP_VERIFY(frame < m_numKeyFrames);
  m_channels[channel][frame] = key;
  m_channelsWNeg[channel][frame] = wNeg;
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::copy(const RotVecTable& src)
{
  NMP_VERIFY(m_numChannels == src.m_numChannels);
  NMP_VERIFY(m_numKeyFrames == src.m_numKeyFrames);

  if (&src != this)
  {
    for (uint32_t i = 0; i < m_numChannels; ++i)
    {
      const NMP::Vector3* srcKeys = src.m_channels[i];
      const uint32_t* srcWNeg = src.m_channelsWNeg[i];
      NMP::Vector3* dstKeys = m_channels[i];
      uint32_t* dstWNeg = m_channelsWNeg[i];

      for (uint32_t k = 0; k < m_numKeyFrames; ++k)
      {
        dstKeys[k] = srcKeys[k];
        dstWNeg[k] = srcWNeg[k];
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
RotVecTable& RotVecTable::operator=(const RotVecTable& src)
{
  copy(src);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::writeChannel(uint32_t channelIndex, FILE* file) const
{
  NMP_VERIFY(file);
  NMP::Vector3* chan = getChannel(channelIndex);
  uint32_t* chanWNeg = getChannelWNeg(channelIndex);

  fprintf(file, "---------------------------------------------------------------------\n");

  for (uint32_t i = 0; i < m_numKeyFrames; ++i)
  {
    fprintf(
      file,
      "frame = %4d, qVec = [%3.5f, %3.5f, %3.5f], wNeg = %d\n",
      i,
      chan[i].x, chan[i].y, chan[i].z,
      chanWNeg[i]);
  }

  fprintf(file, "---------------------------------------------------------------------\n\n");
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::writeFrame(uint32_t frame, FILE* file) const
{
  NMP_VERIFY(file);
  NMP::Vector3 qVec;
  uint32_t wNeg;

  fprintf(file, "---------------------------------------------------------------------\n");

  for (uint32_t i = 0; i < m_numChannels; ++i)
  {
    getKey(i, frame, qVec, wNeg);

    fprintf(
      file,
      "chan = %4d, qVec = [%3.5f, %3.5f, %3.5f], wNeg = %d\n",
      i,
      qVec.x, qVec.y, qVec.z,
      wNeg);
  }

  fprintf(file, "---------------------------------------------------------------------\n\n");
}

//----------------------------------------------------------------------------------------------------------------------
void RotVecTable::writeTableByChannels(FILE* file) const
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
void RotVecTable::writeTableByFrames(FILE* file) const
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
