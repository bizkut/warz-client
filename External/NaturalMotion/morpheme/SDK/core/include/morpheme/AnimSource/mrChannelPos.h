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
#ifndef MR_CHANNEL_POS_H
#define MR_CHANNEL_POS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ChannelPos
/// \brief A channel of key frame Vector3 position values.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelPos
{
public:
  ChannelPos() {}
  ~ChannelPos() {}

  void locate(uint8_t** data);
  void dislocate();
  void relocate(uint8_t** data);

  /// \brief Sample this channel, interpolating between the specified frame and the next.
  void getPos(
    const NMP::Vector3& defaultValue, ///< IN: The default value to return if there are no keyframes
    uint32_t            frameIndex,   ///< IN: Frame to sample from.
    float               interpolant,  ///< IN: [0.0f - 1.0f)
    NMP::Vector3&       resultPos     ///< OUT: The interpolated position
  ) const;

  /// \brief Get the position value of this channel at the specified frame.
  /// Does no interpolation, just returns stored key frame data.
  void getKeyFramePos(
    const NMP::Vector3& defaultValue, ///< IN: The default value to return if there are no keyframes
    uint32_t            frameIndex,   ///< IN: Frame to sample from
    NMP::Vector3&       resultPos     ///< OUT: The sampled position
  ) const;

  /// \brief Return the number of key frames samples that are stored in this channel.
  uint32_t getNumKeyFrames() const { return m_numberOfKeyFrames; }
  void setNumKeyFrames(uint32_t numberOfKeyFrames) { m_numberOfKeyFrames = numberOfKeyFrames; }

  NMP::Vector3* getKeyFrameArray() { return m_keyFrameArray; }
  const NMP::Vector3* getKeyFrameArray() const { return m_keyFrameArray; }

  /// \brief Get the position value of this channel at the specified frame.
  NMP::Vector3& operator[] (uint32_t i);
  const NMP::Vector3& operator[] (uint32_t i) const;

protected:
  uint32_t      m_numberOfKeyFrames; ///< Number of key frames samples that are stored in this channel
  NMP::Vector3* m_keyFrameArray;     ///< Array of sampled translations
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_CHANNEL_POS_H
//----------------------------------------------------------------------------------------------------------------------
