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
#ifndef ROT_VEC_TABLE_BUILDER_H
#define ROT_VEC_TABLE_BUILDER_H

//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "NMPlatform/NMBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::RotVecTable
/// \brief A table of multiple rotation vector channels each with a specified number of keyframes.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class RotVecTable
{
public:
  /// \brief Calculate the memory required to create a ChannelSetTable from the supplied parameters.
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numChannels,               ///< IN: The number of channels in the table
    uint32_t numKeyFrames               ///< IN: The number of keyframes for each channel set
  );

  /// \brief Create a new RotVecTable in the supplied memory.
  static RotVecTable* init(
    NMP::Memory::Resource&  resource,     ///< IN: Resource description of where to build the table
    uint32_t                numChannels,  ///< IN: The number of channels in the table
    uint32_t                numKeyFrames  ///< IN: The number of keyframes for each channel set
  );

  /// \brief Function to get the sign of the wNeg component.
  /// 1 if wNeg = 0, -1 otherwise
  static float getWNegSign(uint32_t wNeg);

  /// \brief Function to recover a quaternion from the rotation vector.
  static void fromRotationVector(
    NMP::Quat&          quat,
    const NMP::Vector3& v,
    uint32_t            wNeg,
    bool                tanQuarterAngle);

public:
  RotVecTable() {}
  ~RotVecTable() {}

  // Conversion functions
  void initChannel(uint32_t channel, const NMP::Quat* quatKeys, bool tanQuarterAngle);
  void setKey(uint32_t channel, uint32_t frame, const NMP::Quat& key, bool tanQuarterAngle);

  //---------------------------------------------------------------------
  /// \name   Accessors
  /// \brief  Functions that retrieve information about the channel data.
  //---------------------------------------------------------------------
  //@{
  NM_INLINE uint32_t getNumChannels() const;
  NM_INLINE uint32_t getNumKeyFrames() const;

  NM_INLINE uint32_t* getChannelWNeg(uint32_t channel) const;
  NM_INLINE NMP::Vector3* getChannel(uint32_t channel) const;
  void setChannel(uint32_t channel, const NMP::Vector3* keys);

  void getKeysAtFrame(uint32_t frame, NMP::Vector3* keys) const;
  void setKeysAtFrame(uint32_t frame, const NMP::Vector3* keys);

  void getKey(uint32_t channel, uint32_t frame, NMP::Vector3& key) const;
  void getKey(uint32_t channel, uint32_t frame, NMP::Vector3& key, uint32_t& wNeg) const;

  void setKey(uint32_t channel, uint32_t frame, const NMP::Vector3& key);
  void setKey(uint32_t channel, uint32_t frame, const NMP::Vector3& key, uint32_t wNeg);

  bool isChannelUnchanging(uint32_t channel, float eps) const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Transformations
  /// \brief  Functions that can be used to transform or condition the data.
  //---------------------------------------------------------------------
  //@{
  void copy(const RotVecTable& src);
  RotVecTable& operator=(const RotVecTable& src);
  //@}

  //---------------------------------------------------------------------
  /// \name   Output
  /// \brief  Functions that can be used to write the channel table to file.
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to write out the transforms of a channel set
  void writeChannel(
    uint32_t channelIndex,            ///< IN: The channel set whose frame transforms we want to write out
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;

  void writeFrame(
    uint32_t frame,                   ///< IN: The frame at which we want to write out the channel set transforms
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;

  /// \brief Function to write out all channel set transforms
  void writeTableByChannels(
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;

  /// \brief Function to write out all channel set transforms
  void writeTableByFrames(
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;
  //@}

protected:
  uint32_t        m_numChannels;
  uint32_t        m_numKeyFrames;
  NMP::Vector3**  m_channels;
  uint32_t**      m_channelsWNeg;
};

//----------------------------------------------------------------------------------------------------------------------
// RotVecTable Inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t RotVecTable::getNumChannels() const
{
  return m_numChannels;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t RotVecTable::getNumKeyFrames() const
{
  return m_numKeyFrames;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* RotVecTable::getChannelWNeg(uint32_t channel) const
{
  NMP_VERIFY(channel < m_numChannels);
  return m_channelsWNeg[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3* RotVecTable::getChannel(uint32_t channel) const
{
  NMP_VERIFY(channel < m_numChannels);
  return m_channels[channel];
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ROT_VEC_TABLE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
