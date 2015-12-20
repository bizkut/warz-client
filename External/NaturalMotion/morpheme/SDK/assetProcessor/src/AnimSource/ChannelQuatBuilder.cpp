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
#include "assetProcessor/AnimSource/ChannelQuatBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// ChannelQuatBuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ChannelQuatBuilder::channelQuatGetMemoryRequirements(uint32_t numKeyFrameSamples)
{
  NMP::Memory::Format result(0, NMP_VECTOR_ALIGNMENT);
  result.size += (numKeyFrameSamples * sizeof(NMP::Quat));
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelQuatBuilder::initChannelQuat(
  uint32_t         numKeyFrames,
  const NMP::Quat* sourceQuatKeys,
  MR::ChannelQuat& channelQuat,
  uint8_t**        data)
{
  NMP_VERIFY(data);
  NMP_VERIFY(NMP_IS_ALIGNED(*data, NMP_VECTOR_ALIGNMENT));

  ChannelQuatBuilder& channelQuatBuilder = (ChannelQuatBuilder&)channelQuat;

  // Check for no keyframe samples
  channelQuatBuilder.m_numberOfKeyFrames = numKeyFrames;
  if (numKeyFrames == 0)
  {
    channelQuatBuilder.m_keyFrameArray = 0;
    return;
  }
  channelQuatBuilder.m_keyFrameArray = (NMP::Quat*) * data;
  NMP_VERIFY(channelQuatBuilder.m_keyFrameArray);
  NMP_VERIFY(sourceQuatKeys);

  // Set the keyframe data samples
  for (uint32_t i = 0; i < numKeyFrames; ++i)
  {
    channelQuatBuilder.m_keyFrameArray[i] = sourceQuatKeys[i];
  }

  // Update the memory data pointer
  *data += (numKeyFrames * sizeof(NMP::Quat));
  *data = (uint8_t*)NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
