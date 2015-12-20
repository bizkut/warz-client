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
#ifndef CHANNEL_ROTVEC_QUANTISED_BUILDER_H
#define CHANNEL_ROTVEC_QUANTISED_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMBuffer.h"
#include "morpheme/AnimSource/mrChannelRotVecQuantised.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ChannelRotVecQuantisedBuilder
/// \brief Builds a channel of quantised key frame rotation vector values.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelRotVecQuantisedBuilder : public MR::ChannelRotVecQuantised
{
public:
  /// \brief Get the memory requirements to hold the required number of keyframe samples.
  /// The returned size corresponds to the keyframes only and not the ChannelRotVecQuantised
  /// structure.
  static NMP::Memory::Format channelRotVecGetMemoryRequirements(
    uint32_t numKeyFrameSamples             ///< IN: The number of quaternion keyframes
  );

  /// \brief Function to compute the quantisation scale and offset information for the rotation vector channel.
  static void computeRotVecQuantisedInfo(
    uint32_t         numKeyFrames,          ///< IN: The number of quaternion keyframes
    const NMP::Quat* sourceQuatKeys,        ///< IN: Quaternion key frame samples
    MR::chanRotVecQuantisedInfo& info       ///< OUT: The quantisation info for the rotation vector samples
  );

  /// \brief Function to compress quaternion keyframe samples into a quantised 48-bit channel.
  static void initChannelRotVec(
    uint32_t                           numKeyFrames,   ///< IN: The number of quaternions to compress
    const NMP::Quat*                   sourceQuatKeys, ///< IN: Uncompressed quaternion key frame samples
    const MR::chanRotVecQuantisedInfo& info,           ///< IN: The quantisation info for the rotation vector samples
    MR::ChannelRotVecQuantised&        channelRotVec,  ///< OUT: The quantised rotation vector channel to fill in
    uint8_t**                          data            ///< IN/OUT: The memory buffer for the channel keyframes (Pointer updated after filling in)
  );
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // CHANNEL_ROTVEC_QUANTISED_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
