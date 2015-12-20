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
#ifndef CHANNEL_QUAT_BUILDER_H
#define CHANNEL_QUAT_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMBuffer.h"
#include "morpheme/AnimSource/mrChannelQuat.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ChannelQuatBuilder
/// \brief Builds a data channel of key frame quaternion values.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelQuatBuilder : public MR::ChannelQuat
{
public:
  /// \brief Get the memory requirements to hold the required number of keyframe samples.
  /// The returned size corresponds to the keyframes only and not the ChannelQuat structure.
  static NMP::Memory::Format channelQuatGetMemoryRequirements(
    uint32_t numKeyFrameSamples      ///< IN: The number of quaternion keyframes
  );

  /// \brief Function to compress quaternion keyframe samples into a data channel.
  static void initChannelQuat(
    uint32_t         numKeyFrames,   ///< IN: The number of quaternion keyframes
    const NMP::Quat* sourceQuatKeys, ///< IN: Source quaternion key frame samples
    MR::ChannelQuat& channelQuat,    ///< OUT: The quaternion channel to fill in
    uint8_t**        data            ///< IN/OUT: The memory buffer for the channel keyframes (Pointer updated after filling in)
  );
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // CHANNEL_QUAT_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
