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
#ifndef MR_CHANNEL_ROTVEC_QUANTISED_H
#define MR_CHANNEL_ROTVEC_QUANTISED_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::chanRotVecQuantised
/// \brief A tan quarter angle rotation vector representation of rotation quantised into 48 bits.
/// \ingroup SourceAnimationCompressedModule
///
/// Quaternions q = [cos(theta/2), sin(theta/2)*a] have a double covering of the rotational space,
/// i.e. q = -q. Where theta is the angle of rotation and a is the unit-vector axis of rotation.
/// A quaternion can therefore represent the set of rotations for angles in the range:
/// theta = [-2*pi : 2*pi], with angles theta = [-pi : pi] representing quaternions in the positive
/// hemisphere w >= 0, and the remaining angles the negative hemisphere w < 0.
///
/// A tan quarter angle rotation vector has the advantage that it can encode a representation of
/// a quaternion that lies in the positive hemisphere (w >= 0) into a three-vector: tan(theta/4) * a
/// Since theta = [-pi : pi], tan(theta/4) = [-1 : 1] and is actually pretty linear in this range,
/// certainly more so than sin(theta/2).
///
/// If we quantise the values [-1 : 1] of a tan quarter angle rotation vector, (i.e. represent
/// the theta angles corresponding to the set of evenly sampled tan(theta/4) plot values), then
/// because of the function's near linearity the representable resolution of angles is accurate
/// over the entire range (good quantisation). This is in contrast to sin(theta/2), which is a lot
/// less linear and in fact has zero function gradient at theta = -pi or pi, resulting in good
/// angle resolution near theta = 0, but increasingly poorer resolution towards -pi and pi.
///
/// Importantly, conversion from tan quarter angle rotation vector back to a quaternion in the
/// positive hemisphere requires no trigonometric functions or square roots. Nor does the
/// resulting quaternion need to be re-normalised to unit length since the math guarantees a
/// unit quaternion.
///
/// When SLERPing between two quat keyframes, we interpolate along the shortest arc of the great
/// circle. Since we can only represent quaternions in the positive hemisphere, shortest arc
/// interpolation is achieved by computing the dot product between the two keyframes and just
/// negating one of them if the dot was negative).
///
/// Compressed in the following fashion:
///   - 16 bits,  (0 - 15 = Unsigned x component of rotation vector).
///   - 16 bits,  (16 - 31 = Unsigned y component of rotation vector).
///   - 16 bits,  (32 - 47 = Unsigned z component of rotation vector).
//----------------------------------------------------------------------------------------------------------------------
struct chanRotVecQuantised
{
  uint16_t m_data[3]; ///< Quantised tan quarter angle rotation Vector3
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::chanRotVecQuantisedInfo
/// \brief Quantisation scale and offset information for a rotation vector channel.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class chanRotVecQuantisedInfo
{
public:
  chanRotVecQuantisedInfo() {}
  ~chanRotVecQuantisedInfo() {}

  void locate();
  void dislocate();

public:
  float m_chanRotVecScales[4];
  float m_chanRotVecOffsets[4];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ChannelRotVecQuantised
/// \brief A channel of quantised key frame rotation vector values (tan quarter angle).
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelRotVecQuantised
{
public:
  ChannelRotVecQuantised() {}
  ~ChannelRotVecQuantised() {}

  void locate(uint8_t** data);
  void dislocate();
  void relocate(uint8_t** data);

  /// \brief Sample this channel, interpolating between the specified frame and the next.
  void getQuat(
    const chanRotVecQuantisedInfo& info,  ///< IN: Quantisation information
    uint32_t   frameIndex,                ///< IN: Frame to sample from
    float      interpolant,               ///< IN: [0.0f - 1.0f)
    NMP::Quat& resultQuat                 ///< OUT: The interpolated quaternion
  ) const;

  /// \brief Get the quaternion value of this channel at the specified frame.
  /// Does no interpolation, just returns stored key frame data.
  void getKeyFrameQuat(
    const chanRotVecQuantisedInfo& info,    ///< IN: Quantisation information
    uint32_t frameIndex,                    ///< IN: Frame to sample from
    NMP::Quat& resultQuat                   ///< OUT: The sampled quaternion
  ) const;

  /// \brief Return the number of key frames samples that are stored in this channel.
  uint32_t getNumKeyFrames() const { return m_numberOfKeyFrames; }
  void setNumKeyFrames(uint32_t numberOfKeyFrames) { m_numberOfKeyFrames = numberOfKeyFrames; }

  chanRotVecQuantised* getKeyFrameArray() { return m_keyFrameArray; }
  const chanRotVecQuantised* getKeyFrameArray() const { return m_keyFrameArray; }

protected:
  /// \brief Extract a full floating point quaternion from a packed quat
  void keyFrameQuatUnPack(
    const chanRotVecQuantisedInfo& info,        ///< IN: Quantisation information
    const chanRotVecQuantised&     packedQuat,  ///< IN: 48 bit compressed source
    NMP::Quat&                     resultQuat   ///< OUT: Unpacked result
  ) const;

  /// \brief Extract a full floating point quaternion from the zero quantisation vector
  void keyFrameQuatUnPack(
    const chanRotVecQuantisedInfo& info,    ///< IN: Quantisation information
    NMP::Quat& resultQuat                   ///< OUT: Unpacked result
  ) const;

protected:
  uint32_t             m_numberOfKeyFrames;  ///< Number of key frames samples that are stored in this channel
  chanRotVecQuantised* m_keyFrameArray;      ///< Array of sampled quantised quaternions
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_CHANNEL_ROTVEC_QUANTISED_H
//----------------------------------------------------------------------------------------------------------------------
