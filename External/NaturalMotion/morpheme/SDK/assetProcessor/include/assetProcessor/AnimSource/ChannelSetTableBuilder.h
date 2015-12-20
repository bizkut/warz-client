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
#ifndef CHANNEL_SET_TABLE_BUILDER_H
#define CHANNEL_SET_TABLE_BUILDER_H

//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "NMPlatform/NMBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ChannelSetTable
/// \brief A table of multiple Quat and Pos channel sets each with a specified number of keyframes.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetTable
{
public:
  /// \brief Calculate the memory required to create a ChannelSetTable from the supplied parameters.
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numChannelSets,          ///< IN: The number of channel sets in the table
    uint32_t numKeyFrames             ///< IN: The number of keyframes for each channel set
  );

  /// \brief Create a new ChannelSetTable in the supplied memory.
  static ChannelSetTable* init(
    NMP::Memory::Resource&  resource,       ///< IN: Resource description of where to build the table
    uint32_t                numChannelSets, ///< IN: The number of channel sets in the table
    uint32_t                numKeyFrames    ///< IN: The number of keyframes for each channel set
  );

public:
  ChannelSetTable() {}
  ~ChannelSetTable() {}

  //---------------------------------------------------------------------
  /// \name   Accessors
  /// \brief  Functions that retrieve information about the channel set data.
  //---------------------------------------------------------------------
  //@{
  NM_INLINE uint32_t getNumChannelSets() const;
  NM_INLINE uint32_t getNumKeyFrames() const;

  NM_INLINE const NMP::Quat* getChannelQuat(uint32_t channel) const;
  NM_INLINE NMP::Quat* getChannelQuat(uint32_t channel);
  NM_INLINE const NMP::Vector3* getChannelPos(uint32_t channel) const;
  NM_INLINE NMP::Vector3* getChannelPos(uint32_t channel);
  void setChannelQuat(uint32_t channel, const NMP::Quat* quatKeys);
  void setChannelQuatKeys(uint32_t channel, const NMP::Quat& quatKey);
  void setChannelPos(uint32_t channel, const NMP::Vector3* posKeys);
  void setChannelPosKeys(uint32_t channel, const NMP::Vector3& posKey);

  void getQuatKeysAtFrame(uint32_t frame, NMP::Quat* quatKeys) const;
  void setQuatKeysAtFrame(uint32_t frame, const NMP::Quat* quatKeys);
  void getPosKeysAtFrame(uint32_t frame, NMP::Vector3* posKeys) const;
  void setPosKeysAtFrame(uint32_t frame, const NMP::Vector3* posKeys);
  void getQuatKey(uint32_t channel, uint32_t frame, NMP::Quat& quatKey) const;
  void setQuatKey(uint32_t channel, uint32_t frame, const NMP::Quat& quatKey);
  void getPosKey(uint32_t channel, uint32_t frame, NMP::Vector3& posKey) const;
  void setPosKey(uint32_t channel, uint32_t frame, const NMP::Vector3& posKey);

  bool isChannelQuatUnchanging(uint32_t channel, float quatEps) const;
  bool isChannelPosUnchanging(uint32_t channel, float posEps) const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Transformations
  /// \brief  Functions that can be used to transform or condition the data.
  //---------------------------------------------------------------------
  //@{
  void copy(const ChannelSetTable& src);
  ChannelSetTable& operator=(const ChannelSetTable& src);
  
  // Resample assuming both channel sets have the same duration
  void resampleChannelSets(const ChannelSetTable& src);

  void conditionChannelQuat(uint32_t channel);
  void normaliseChannelQuat(uint32_t channel);
  void conditionChannelQuat();
  void normaliseChannelQuat();

  float maxAngleChannelQuat(uint32_t channel);
  float maxAngleChannelQuat();

  void applyToChannelQuat(uint32_t channel, const NMP::Quat* quatKeys);
  void inverseChannelQuat(uint32_t channel);
  void applyToChannelPos(uint32_t channel, const NMP::Vector3* posKeys);
  void inverseChannelPos(uint32_t channel);

  void applyToChannelQuat(const ChannelSetTable& src);
  void inverseChannelQuat();
  void applyToChannelPos(const ChannelSetTable& src);
  void inverseChannelPos();

  void computeDeltaTransforms(const ChannelSetTable& from, const ChannelSetTable& to);

  void scaleChannelPos(uint32_t channel, float scale);
  void scaleChannelPos(float scale);

  // y = a*x + b
  void linearTransformChannelPos(uint32_t channel, const NMP::Vector3& a, const NMP::Vector3& b);
  void linearTransformChannelPos(const NMP::Vector3& a, const NMP::Vector3& b);
  //@}

  //---------------------------------------------------------------------
  /// \name   Output
  /// \brief  Functions that can be used to write the channel set table to file.
  //---------------------------------------------------------------------
  //@{

  /// \brief Function to write out the transforms of a channel set
  void writeChannelSet(
    uint32_t channelIndex,            ///< IN: The channel set whose frame transforms we want to write out
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;

  void writeFrame(
    uint32_t frame,                   ///< IN: The frame at which we want to write out the channel set transforms
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;

  /// \brief Function to write out all channel set transforms
  void writeTableByChannelSets(
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;

  /// \brief Function to write out all channel set transforms
  void writeTableByFrames(
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;

  /// \brief Function to write out the maximal magnitude of the position channels over frames
  void writeMaxPosErrors(
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;

  /// \brief Function to write out the mean magnitude of the position channels over frames
  void writeMeanPosErrors(
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;
  //@}

protected:
  uint32_t        m_numChannelSets;
  uint32_t        m_numKeyFrames;
  NMP::Quat**     m_channelQuat;
  NMP::Vector3**  m_channelPos;
};

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetTable Inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t ChannelSetTable::getNumChannelSets() const
{
  return m_numChannelSets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t ChannelSetTable::getNumKeyFrames() const
{
  return m_numKeyFrames;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::Quat* ChannelSetTable::getChannelQuat(uint32_t channel) const
{
  NMP_VERIFY_MSG(channel < m_numChannelSets, "Trying to query a ChannelSetTable with a channel index that is out of range");
  return m_channelQuat[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat* ChannelSetTable::getChannelQuat(uint32_t channel)
{
  NMP_VERIFY_MSG(channel < m_numChannelSets, "Trying to query a ChannelSetTable with a channel index that is out of range");
  return m_channelQuat[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::Vector3* ChannelSetTable::getChannelPos(uint32_t channel) const
{
  NMP_VERIFY_MSG(channel < m_numChannelSets, "Trying to query a ChannelSetTable with a channel index that is out of range");
  return m_channelPos[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3* ChannelSetTable::getChannelPos(uint32_t channel)
{
  NMP_VERIFY_MSG(channel < m_numChannelSets, "Trying to query a ChannelSetTable with a channel index that is out of range");
  return m_channelPos[channel];
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // CHANNEL_SET_TABLE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
