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
#ifndef INTVECTOR3_TABLE_BUILDER_H
#define INTVECTOR3_TABLE_BUILDER_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::IntVector3Table
/// \brief A table of multiple IntVector3 channels each with a specified number of keyframes.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class IntVector3Table
{
public:
  /// \brief Calculate the memory required to create a Vector3Table from the supplied parameters.
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numChannels,                 ///< IN: The number of channels in the table
    uint32_t numKeyFrames                 ///< IN: The number of keyframes for each channel set
  );

  /// \brief Create a new Vector3Table in the supplied memory.
  static IntVector3Table* init(
    NMP::Memory::Resource&  resource,     ///< IN: Resource description of where to build the table
    uint32_t                numChannels,  ///< IN: The number of channels in the table
    uint32_t                numKeyFrames  ///< IN: The number of keyframes for each channel set
  );

public:
  IntVector3Table() {}
  ~IntVector3Table() {}

  //---------------------------------------------------------------------
  /// \name   Accessors
  /// \brief  Functions that retrieve information about the channel data.
  //---------------------------------------------------------------------
  //@{
  NM_INLINE uint32_t getNumChannels() const;
  NM_INLINE uint32_t getNumKeyFrames() const;

  void getKey(uint32_t channel, uint32_t frame, int32_t* key) const;
  void setKey(uint32_t channel, uint32_t frame, const int32_t* key);

  bool isChannelUnchanging(uint32_t channel, int32_t eps) const;
  //@}

protected:
  uint32_t        m_numChannels;
  uint32_t        m_numKeyFrames; 
  int32_t**       m_channelsX;
  int32_t**       m_channelsY;
  int32_t**       m_channelsZ;
};

//----------------------------------------------------------------------------------------------------------------------
// Vector3Table Inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t IntVector3Table::getNumChannels() const
{
  return m_numChannels;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t IntVector3Table::getNumKeyFrames() const
{
  return m_numKeyFrames;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // INTVECTOR3_TABLE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
