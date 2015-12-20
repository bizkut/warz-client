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
#ifndef MR_CHANNEL_QUAT_H
#define MR_CHANNEL_QUAT_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ChannelQuat
/// \brief A channel of key frame quaternion values.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelQuat
{
public:
  ChannelQuat() {}
  ~ChannelQuat() {}

  void locate(uint8_t** data);
  void dislocate();
  void relocate(uint8_t** data);

  /// \brief Sample this channel, interpolating between the specified frame and the next.
  void getQuat(
    const NMP::Quat& defaultValue, ///< IN: The default value to return if there are no keyframes
    uint32_t         frameIndex,   ///< IN: Frame to sample from.
    float            interpolant,  ///< IN: [0.0f - 1.0f)
    NMP::Quat&       resultQuat    ///< OUT: The interpolated quaternion
  ) const;

  /// \brief Get the quaternion value of this channel at the specified frame.
  /// Does no interpolation, just returns stored key frame data.
  void getKeyFrameQuat(
    const NMP::Quat& defaultValue, ///< IN: The default value to return if there are no keyframes
    uint32_t         frameIndex,   ///< IN: Frame to sample from
    NMP::Quat&       resultQuat    ///< OUT: The sampled quaternion
  ) const;

  /// \brief Return the number of key frames samples that are stored in this channel.
  uint32_t getNumKeyFrames() const { return m_numberOfKeyFrames; }
  void setNumKeyFrames(uint32_t numberOfKeyFrames) { m_numberOfKeyFrames = numberOfKeyFrames; }

  NMP::Quat* getKeyFrameArray() { return m_keyFrameArray; }
  const NMP::Quat* getKeyFrameArray() const { return m_keyFrameArray; }

  /// \brief Get the quaternion value of this channel at the specified frame
  NMP::Quat& operator[] (uint32_t i);
  const NMP::Quat& operator[] (uint32_t i) const;

protected:
  uint32_t   m_numberOfKeyFrames; ///< Number of key frames samples that are stored in this channel
  NMP::Quat* m_keyFrameArray;     ///< Array of sampled quaternions
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_CHANNEL_QUAT_H
//----------------------------------------------------------------------------------------------------------------------
