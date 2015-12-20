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
#include "assetProcessor/AnimSource/ChannelPosBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// ChannelPosBuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ChannelPosBuilder::channelPosGetMemoryRequirements(uint32_t numKeyFrameSamples)
{
  NMP::Memory::Format result(0, NMP_VECTOR_ALIGNMENT);
  result.size += (numKeyFrameSamples * sizeof(NMP::Vector3));
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelPosBuilder::initChannelPos(
  uint32_t            numKeyFrames,
  const NMP::Vector3* sourcePosKeys,
  MR::ChannelPos&     channelPos,
  uint8_t**           data)
{
  NMP_VERIFY(data);
  NMP_VERIFY(NMP_IS_ALIGNED(*data, NMP_VECTOR_ALIGNMENT));

  ChannelPosBuilder& channelPosBuilder = (ChannelPosBuilder&)channelPos;

  // Check for no keyframe samples
  channelPosBuilder.m_numberOfKeyFrames = numKeyFrames;
  if (numKeyFrames == 0)
  {
    channelPosBuilder.m_keyFrameArray = 0;
    return;
  }
  channelPosBuilder.m_keyFrameArray = (NMP::Vector3*) * data;
  NMP_VERIFY(channelPosBuilder.m_keyFrameArray);
  NMP_VERIFY(sourcePosKeys);

  // Set the keyframe data samples
  for (uint32_t i = 0; i < numKeyFrames; ++i)
  {
    channelPosBuilder.m_keyFrameArray[i] = sourcePosKeys[i];
  }

  // Update the memory data pointer
  *data += (numKeyFrames * sizeof(NMP::Vector3));
  *data = (uint8_t*)NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
