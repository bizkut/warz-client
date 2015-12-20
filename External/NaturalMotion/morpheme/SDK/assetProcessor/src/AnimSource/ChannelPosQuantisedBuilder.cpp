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
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMemory.h"
#include "morpheme/mrDefines.h"
#include "assetProcessor/AnimSource/ChannelPosQuantisedBuilder.h"
//----------------------------------------------------------------------------------------------------------------------
// Disable warnings constant conditionals
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4127)
#endif

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// ChannelPosQuantisedBuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format
ChannelPosQuantisedBuilder::channelPosGetMemoryRequirements(uint32_t numKeyFrameSamples)
{
  NMP::Memory::Format result(0, NMP_NATURAL_TYPE_ALIGNMENT);
  result.size += (numKeyFrameSamples * sizeof(MR::chanPosQuantised));
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelPosQuantisedBuilder::computePosQuantisedInfo(
  uint32_t                  numKeyFrames,
  const NMP::Vector3*       sourcePosKeys,
  MR::chanPosQuantisedInfo& info)
{
  NMP_VERIFY(sourcePosKeys);
  NMP_VERIFY(numKeyFrames > 0);

  // Compute the minimum and maximum of the position components (independently)
  NMP::Vector3 minP, maxP;
  NMP::vBounds(numKeyFrames, sourcePosKeys, minP, maxP);

  // Compute the quantisation scaling factor
  MR::chanPosQuantised chanPos;
  chanPos.m_data = 0xFFFFFFFF;
  NMP::Vector3 diffP = maxP - minP;

  // X Component
  if (diffP.x > MR::ERROR_LIMIT)
  {
    info.m_chanPosScales[0] = diffP.x / chanPos.m_bits.x; // 11 bits
  }
  else
  {
    info.m_chanPosScales[0] = 0.0f; // Essentially static
  }

  // Y Component
  if (diffP.y > MR::ERROR_LIMIT)
  {
    info.m_chanPosScales[1] = diffP.y / chanPos.m_bits.y; // 11 bits
  }
  else
  {
    info.m_chanPosScales[1] = 0.0f; // Essentially static
  }

  // Z Component
  if (diffP.z > MR::ERROR_LIMIT)
  {
    info.m_chanPosScales[2] = diffP.z / chanPos.m_bits.z; // 10 bits
  }
  else
  {
    info.m_chanPosScales[2] = 0.0f; // Essentially static
  }

  info.m_chanPosScales[3] = 0.0f;

  // Set the quantisation offset factor
  info.m_chanPosOffsets[0] = minP.x;
  info.m_chanPosOffsets[1] = minP.y;
  info.m_chanPosOffsets[2] = minP.z;
  info.m_chanPosOffsets[3] = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelPosQuantisedBuilder::initChannelPos(
  uint32_t                        numKeyFrames,
  const NMP::Vector3*             sourcePosKeys,
  const MR::chanPosQuantisedInfo& info,
  MR::ChannelPosQuantised&        channelPos,
  uint8_t**                       data)
{
  NMP_VERIFY(data);

  ChannelPosQuantisedBuilder& channelPosBuilder = (ChannelPosQuantisedBuilder&)channelPos;

  // Check for no keyframe samples
  channelPosBuilder.m_numberOfKeyFrames = numKeyFrames;
  if (numKeyFrames == 0)
  {
    channelPosBuilder.m_keyFrameArray = 0;
    return;
  }
  channelPosBuilder.m_keyFrameArray = (MR::chanPosQuantised*) * data;
  NMP_VERIFY(channelPosBuilder.m_keyFrameArray);

  // Compute the inverse scaling coefficients
  float invScale[3], invOffset[3];
  for (uint32_t i = 0; i < 3; ++i)
  {
    if (info.m_chanPosScales[i] > 0.0f)
    {
      invScale[i] = 1.0f / info.m_chanPosScales[i];
      invOffset[i] = -info.m_chanPosOffsets[i] * invScale[i];
    }
    else
    {
      invScale[i] = 0.0f;
      invOffset[i] = 0.0f;
    }
  }

  // Compute the minimum and maximum component limits for clamping
  NMP::Vector3 minP(info.m_chanPosOffsets[0], info.m_chanPosOffsets[1], info.m_chanPosOffsets[2]);
  MR::chanPosQuantised chanPos;
  chanPos.m_data = 0xFFFFFFFF;
  NMP::Vector3 maxP;
  maxP.x = info.m_chanPosScales[0] * chanPos.m_bits.x + info.m_chanPosOffsets[0]; // 11 bits
  maxP.y = info.m_chanPosScales[1] * chanPos.m_bits.y + info.m_chanPosOffsets[1]; // 11 bits
  maxP.z = info.m_chanPosScales[2] * chanPos.m_bits.z + info.m_chanPosOffsets[2]; // 10 bits

  // Quantise the position data samples
  NMP::Vector3 p;
  for (uint32_t i = 0; i < numKeyFrames; ++i)
  {
    // Clamp components of the keyframe to the representable range
    p.x = NMP::clampValue(sourcePosKeys[i].x, minP.x, maxP.x);
    p.y = NMP::clampValue(sourcePosKeys[i].y, minP.y, maxP.y);
    p.z = NMP::clampValue(sourcePosKeys[i].z, minP.z, maxP.z);

    // Quantise the value
    MR::chanPosQuantised& value = channelPosBuilder.m_keyFrameArray[i];
    value.m_bits.x = (uint32_t)(invScale[0] * p.x + invOffset[0]);
    value.m_bits.y = (uint32_t)(invScale[1] * p.y + invOffset[1]);
    value.m_bits.z = (uint32_t)(invScale[2] * p.z + invOffset[2]);
  }

  // Update the memory data pointer
  *data += (numKeyFrames * sizeof(MR::chanPosQuantised));
  *data = (uint8_t*)NMP::Memory::align(*data, NMP_NATURAL_TYPE_ALIGNMENT);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
