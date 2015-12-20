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
#ifndef VECTOR3_TABLE_BUILDER_H
#define VECTOR3_TABLE_BUILDER_H

//----------------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include "NMPlatform/NMBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::Vector3Table
/// \brief A table of multiple Vector3 channels each with a specified number of keyframes.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class Vector3Table
{
public:
  /// \brief Calculate the memory required to create a Vector3Table from the supplied parameters.
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numChannels,                   ///< IN: The number of channels in the table
    uint32_t numKeyFrames                   ///< IN: The number of keyframes for each channel set
  );

  /// \brief Create a new Vector3Table in the supplied memory.
  static Vector3Table* init(
    NMP::Memory::Resource&  resource,       ///< IN: Resource description of where to build the table
    uint32_t                numChannels,    ///< IN: The number of channels in the table
    uint32_t                numKeyFrames    ///< IN: The number of keyframes for each channel set
  );

public:
  Vector3Table() {}
  ~Vector3Table() {}

  // Accessors
  NM_INLINE uint32_t getNumChannels() const;
  NM_INLINE uint32_t getNumKeyFrames() const;

  NM_INLINE const NMP::Vector3* getChannel(uint32_t channel) const;
  void setChannel(uint32_t channel, const NMP::Vector3* keys);

  void getKeysAtFrame(uint32_t frame, NMP::Vector3* keys) const;
  void setKeysAtFrame(uint32_t frame, const NMP::Vector3* keys);

  void getKey(uint32_t channel, uint32_t frame, NMP::Vector3& key) const;
  void setKey(uint32_t channel, uint32_t frame, const NMP::Vector3& key);

  void copy(const Vector3Table& src);
  Vector3Table& operator=(const Vector3Table& src);

  /// \brief Function to write out a channel within the table.
  void writeChannel(
    uint32_t channelIndex,            ///< IN: The channel set whose frame transforms we want to write out
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;

  void writeFrame(
    uint32_t frame,                   ///< IN: The frame at which we want to write out the channel set transforms
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;

  /// \brief Function to write out all channel set transforms.
  void writeTableByChannels(
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;

  /// \brief Function to write out all channel set transforms.
  void writeTableByFrames(
    FILE* file                        ///< IN/OUT: File handle to write out to
  ) const;

protected:
  uint32_t        m_numChannels;
  uint32_t        m_numKeyFrames;
  NMP::Vector3**  m_channels;
};

//----------------------------------------------------------------------------------------------------------------------
// Vector3Table Inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t Vector3Table::getNumChannels() const
{
  return m_numChannels;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t Vector3Table::getNumKeyFrames() const
{
  return m_numKeyFrames;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::Vector3* Vector3Table::getChannel(uint32_t channel) const
{
  NMP_VERIFY(channel < m_numChannels);
  return m_channels[channel];
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // VECTOR3_TABLE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
