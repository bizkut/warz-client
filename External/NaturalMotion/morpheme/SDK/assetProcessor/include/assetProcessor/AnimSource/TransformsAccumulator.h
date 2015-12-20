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
#ifndef TRANSFORMS_ACCUMULATOR_H
#define TRANSFORMS_ACCUMULATOR_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMHierarchy.h"
#include "morpheme/mrRig.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class    AP::TransformsAccumulator
/// \brief    A transforms buffer class that accumulates worldspace transforms from local
///           transforms or unaccumulates from worldspace.
/// \ingroup  UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class TransformsAccumulator
{
public:
  /// \brief Calculate the memory required to create a TransformsAccumulator from the supplied parameters.
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numRigBones              ///< IN: The number of rig bones in the buffer
  );

  /// \brief Create a new TransformsAccumulator in the supplied memory.
  static TransformsAccumulator* init(
    NMP::Memory::Resource&  resource,     ///< IN: Resource description of where to build the buffer
    uint32_t                numRigBones   ///< IN: The number of rig bones in the buffer
  );

  //-----------------------
  // Enumeration for the channel space type
  enum chanSpaceType
  {
    Local,
    World
  };

public:
  TransformsAccumulator() {}
  ~TransformsAccumulator() {}

  void convertToWorldSpace(const NMP::Hierarchy& hierarchy);
  void convertToLocalSpace(const NMP::Hierarchy& hierarchy);

  float worldspacePosSquaredError(const TransformsAccumulator& rhs, uint32_t channel);
  float worldspacePosSquaredError(const TransformsAccumulator& rhs);

  float worldspacePosError(const TransformsAccumulator& rhs, uint32_t channel);
  float worldspacePosError(const TransformsAccumulator& rhs);

  void setTransformsIdentity(chanSpaceType space = Local);
  void setTransforms(const NMP::DataBuffer* transformBuffer, chanSpaceType space = Local);

  void diff(const TransformsAccumulator& rhs);
  void maxAbsQuatElements(NMP::Quat& quat) const;
  void maxAbsPosElements(NMP::Vector3& pos) const;

  // Accessors
  NM_INLINE uint32_t getNumChannelSets() const;

  NM_INLINE void getChannelQuat(uint32_t channel, NMP::Quat& quat, chanSpaceType& space) const;
  NM_INLINE void getChannelQuat(uint32_t channel, NMP::Quat& quat) const;
  NM_INLINE void getChannelPos(uint32_t channel, NMP::Vector3& pos, chanSpaceType& space) const;
  NM_INLINE void getChannelPos(uint32_t channel, NMP::Vector3& pos) const;

  NM_INLINE void setChannelQuat(uint32_t channel, const NMP::Quat& quat, chanSpaceType space);
  NM_INLINE void setChannelPos(uint32_t channel, const NMP::Vector3& pos, chanSpaceType space);

  NM_INLINE NMP::Quat* getQuatKeys() const;
  NM_INLINE NMP::Vector3* getPosKeys() const;

protected:
  uint32_t        m_numChannelSets;

  chanSpaceType*  m_channelQuatSpace;
  chanSpaceType*  m_channelPosSpace;

  NMP::Quat*      m_channelQuat;
  NMP::Vector3*   m_channelPos;
};

//----------------------------------------------------------------------------------------------------------------------
// TransformsAccumulator Inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t TransformsAccumulator::getNumChannelSets() const
{
  return m_numChannelSets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TransformsAccumulator::getChannelQuat(uint32_t channel, NMP::Quat& quat, chanSpaceType& space) const
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(m_channelQuatSpace);
  NMP_VERIFY(m_channelQuat);
  quat = m_channelQuat[channel];
  space = m_channelQuatSpace[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TransformsAccumulator::getChannelQuat(uint32_t channel, NMP::Quat& quat) const
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(m_channelQuat);
  quat = m_channelQuat[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TransformsAccumulator::getChannelPos(uint32_t channel, NMP::Vector3& pos, chanSpaceType& space) const
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(m_channelPosSpace);
  NMP_VERIFY(m_channelPos);
  pos = m_channelPos[channel];
  space = m_channelPosSpace[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TransformsAccumulator::getChannelPos(uint32_t channel, NMP::Vector3& pos) const
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(m_channelPos);
  pos = m_channelPos[channel];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TransformsAccumulator::setChannelQuat(uint32_t channel, const NMP::Quat& quat, chanSpaceType space)
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(m_channelQuatSpace);
  NMP_VERIFY(m_channelQuat);
  m_channelQuat[channel] = quat;
  m_channelQuatSpace[channel] = space;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TransformsAccumulator::setChannelPos(uint32_t channel, const NMP::Vector3& pos, chanSpaceType space)
{
  NMP_VERIFY(channel < m_numChannelSets);
  NMP_VERIFY(m_channelPosSpace);
  NMP_VERIFY(m_channelPos);
  m_channelPos[channel] = pos;
  m_channelPosSpace[channel] = space;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Quat* TransformsAccumulator::getQuatKeys() const
{
  return m_channelQuat;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3* TransformsAccumulator::getPosKeys() const
{
  return m_channelPos;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // TRANSFORMS_ACCUMULATOR_H
//----------------------------------------------------------------------------------------------------------------------
