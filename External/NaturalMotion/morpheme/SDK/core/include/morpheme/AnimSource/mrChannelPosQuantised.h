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
#ifndef MR_CHANNEL_POS_QUANTISED_H
#define MR_CHANNEL_POS_QUANTISED_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMBuffer.h"

#include "NMPlatform/NMvpu.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
/// \struct MR::chanPosQuantised
/// \brief A Vector3 position quantised into 32 bits.
/// \ingroup SourceAnimationCompressedModule
///
/// Compressed in the following fashion:
///   - 11 bits,  (0 - 10 = Unsigned x component).
///   - 11 bits,  (11 - 21 = Unsigned y component).
///   - 10 bits,  (22 - 31 = Unsigned z component).
/// See the comments for ChannelPosQuantised below for the details of how the compression works.
//----------------------------------------------------------------------------------------------------------------------
struct chanPosQuantised
{
  union
  {
    struct
    {
      uint32_t z: 10;
      uint32_t y: 11;
      uint32_t x: 11;
    } m_bits;

    uint32_t m_data;
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::chanPosQuantisedInfo
/// \brief Quantisation scale and offset information for a position channel.
/// \ingroup SourceAnimationCompressedModule
//----------------------------------------------------------------------------------------------------------------------
class chanPosQuantisedInfo
{
public:
  chanPosQuantisedInfo() {}
  ~chanPosQuantisedInfo() {}

  void locate();
  void dislocate();

public:
  float m_chanPosScales[4];
  float m_chanPosOffsets[4];
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ChannelPosQuantised
/// \brief A channel of quantised key frame Vector3 position values.
/// \ingroup SourceAnimationCompressedModule
///
/// Compression method:
///   - Start with a channel of full floating point positions.
///   - The min and max variation of x, y and z over the entire channel is determined.
///   - The minimum x, y and z values are subtracted from all of the positions in the channel.
///     This makes the channel zero based and all of the position values positive.
///     We store the minimum x, y and z values in the channel as the bone position offset.
///     This vector is added back to the positions on decompression.
///     Using this method we no longer need to store the sign of each x, y and z in positions.
///   - All position x, y and z values are scaled by the range of x, y and z for the channel.
///     This puts the channels x, y, and z position values into the range of 0.0 to 1.0.
///     These position x, y, and z values are then quantised into into the bits shown above.
///     This method produces maximum accuracy for each x, y and z channel because we
///     are only compressing across the range of movement of each component independently.
///
/// This method is lossy, and artifacts become apparent when the range of movement
/// is large in comparison to the camera viewing distance (large movement range or
/// small movement range viewed close up).
//----------------------------------------------------------------------------------------------------------------------
class ChannelPosQuantised
{
public:
  ChannelPosQuantised() {}
  ~ChannelPosQuantised() {}

  void locate(uint8_t** data);
  void dislocate();
  void relocate(uint8_t** data);

  /// \brief Sample this channel, interpolating between the specified frame and the next
  void getPos(
    const chanPosQuantisedInfo& info,        ///< IN: Quantisation information
    uint32_t                    frameIndex,  ///< IN: Frame to sample from
    float                       interpolant, ///< IN: [0.0f - 1.0f)
    NMP::Vector3&               resultPos    ///< OUT: The interpolated position
  ) const;

#ifdef NMP_PLATFORM_SIMD
  NM_INLINE void getUnPackPos(
    const chanPosQuantisedInfo& info,
    uint32_t                    frameIndex,
    NMP::Vector3&               resultPos) const;

  NM_INLINE void getUnPackPosLerp(
    const chanPosQuantisedInfo& info,
    uint32_t                    frameIndex,
    NMP::vpu::vector4_t         interpolant,
    NMP::Vector3&               resultPos) const;
#endif

  /// \brief Get the position value of this channel at the specified frame.
  /// Does no interpolation, just returns stored key frame data.
  void getKeyFramePos(
    const chanPosQuantisedInfo& info,        ///< IN: Quantisation information
    uint32_t                    frameIndex,  ///< IN: Frame to sample from
    NMP::Vector3&               resultPos    ///< OUT: Resulting sampled position
  ) const;

  /// \brief Return the number of key frames samples that are stored in this channel.
  uint32_t getNumKeyFrames() const { return m_numberOfKeyFrames; }
  void setNumKeyFrames(uint32_t numberOfKeyFrames) { m_numberOfKeyFrames = numberOfKeyFrames; }

  chanPosQuantised* getKeyFrameArray() { return m_keyFrameArray; }
  const chanPosQuantised* getKeyFrameArray() const { return m_keyFrameArray; }

protected:

  /// \brief Extract a full floating point position from a packed vector
  void keyFramePosUnPack(
    const chanPosQuantisedInfo& info,       ///< IN: Quantisation information
    const chanPosQuantised&     packedPos,  ///< IN: 32 bit compressed source
    NMP::Vector3&               resultPos   ///< OUT: Unpacked result
  ) const;

  uint32_t          m_numberOfKeyFrames;    ///< Number of key frames samples that are stored in this channel
  chanPosQuantised* m_keyFrameArray;        ///< Array of sampled quantised translations
};

//----------------------------------------------------------------------------------------------------------------------

#ifdef NMP_PLATFORM_SIMD
NM_INLINE void ChannelPosQuantised::getUnPackPos(
  const chanPosQuantisedInfo& info,
  uint32_t                    frameIndex,
  NMP::Vector3&               resultPos) const
{
  NMP::vpu::vector4_t unpackedPos;
  NMP::vpu::vector4_t tran = NMP::vpu::unpackQuantizedVector((uint32_t*)&m_keyFrameArray[frameIndex].m_data);
  unpackedPos = add_4f(NMP::vpu::load4f((float*)info.m_chanPosOffsets), mul_4f(tran, NMP::vpu::load4f((float*)info.m_chanPosScales)));
  NMP::vpu::store4f((float*)&resultPos, unpackedPos);
}

NM_INLINE void ChannelPosQuantised::getUnPackPosLerp(
  const chanPosQuantisedInfo& info,
  uint32_t                    frameIndex,
  NMP::vpu::vector4_t         interpolant,
  NMP::Vector3&               resultPos) const
{
  NMP::vpu::vector4_t unpackedPos0, unpackedPos1;
  NMP::vpu::vector4_t tran1, tran2;
  NMP::vpu::unpack2QuantizedVectors(tran1, tran2, (uint32_t*)&m_keyFrameArray[frameIndex].m_data);
  NMP::vpu::vector4_t chanPO = NMP::vpu::load4f((float*)info.m_chanPosOffsets);
  NMP::vpu::vector4_t chanPS = NMP::vpu::load4f((float*)info.m_chanPosScales);
  unpackedPos0 = add_4f(chanPO, mul_4f(tran1, chanPS));
  unpackedPos1 = add_4f(chanPO, mul_4f(tran2, chanPS));

  NMP::vpu::store4f((float*)&resultPos, add_4f(unpackedPos0, mul_4f(interpolant, sub_4f(unpackedPos1, unpackedPos0))));
}
#endif

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_CHANNEL_POS_QUANTISED_H
//----------------------------------------------------------------------------------------------------------------------
