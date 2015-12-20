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
#ifndef CHANNEL_POS_QUANTISED_BUILDER_H
#define CHANNEL_POS_QUANTISED_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMBuffer.h"
#include "morpheme/AnimSource/mrChannelPosQuantised.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ChannelPosQuantisedBuilder
/// \brief Builds a channel of quantised key frame position values.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelPosQuantisedBuilder : public MR::ChannelPosQuantised
{
public:
  /// \brief Get the memory requirements to hold the required number of keyframe samples.
  /// The returned size corresponds to the keyframes only and not the ChannelPosQuantised
  /// structure.
  static NMP::Memory::Format channelPosGetMemoryRequirements(
    uint32_t numKeyFrameSamples           ///< IN: The number of position keyframes
  );

  /// \brief Function to compute the quantisation scale and offset information for the position channel.
  static void computePosQuantisedInfo(
    uint32_t                  numKeyFrames,  ///< IN: The number of position keyframes
    const NMP::Vector3*       sourcePosKeys, ///< IN: Position key frame samples
    MR::chanPosQuantisedInfo& info           ///< OUT: The quantisation info for the position samples
  );

  /// \brief Function to compress position keyframe samples into a quantised 32-bit channel.
  static void initChannelPos(
    uint32_t                        numKeyFrames,  ///< IN: The number of positions to compress
    const NMP::Vector3*             sourcePosKeys, ///< IN: Uncompressed position key frame samples
    const MR::chanPosQuantisedInfo& info,          ///< IN: The quantisation info for the position samples
    MR::ChannelPosQuantised&        channelPos,    ///< OUT: The quantised position channel to fill in
    uint8_t**                       data           ///< IN/OUT: The memory buffer for the channel keyframes (Pointer updated after filling in)
  );
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // CHANNEL_POS_QUANTISED_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
