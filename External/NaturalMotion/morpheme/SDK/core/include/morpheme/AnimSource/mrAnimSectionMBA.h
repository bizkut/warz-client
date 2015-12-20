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
#ifndef MR_ANIM_SECTION_MBA_H
#define MR_ANIM_SECTION_MBA_H
//----------------------------------------------------------------------------------------------------------------------
#if !defined(NM_HOST_CELL_SPU) && !defined(NM_HOST_CELL_PPU)
  #ifdef NM_DEBUG
    #include <stdio.h>
  #endif // NM_DEBUG
#endif //#if !defined(NM_HOST_CELL_SPU) && !defined(NM_HOST_CELL_PPU)

#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/AnimSource/mrChannelPos.h"
#include "morpheme/AnimSource/mrChannelQuat.h"
#include "NMPlatform/NMBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ChannelSetBasicInfo
/// \brief Stores information about the overall channel frame data.
/// \ingroup SourceAnimationBasicModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetMBAInfo
{
public:
  ChannelSetMBAInfo() {}
  ~ChannelSetMBAInfo() {}

  void locate();
  void dislocate();

  const NMP::Vector3& getChannelPosInfo() const { return m_chanPosDefault; }
  const NMP::Quat& getChannelQuatInfo() const { return m_chanQuatDefault; }

protected:
  NMP::Vector3 m_chanPosDefault;
  NMP::Quat    m_chanQuatDefault;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ChannelSetMBA
/// \brief A position and quaternion channel associated together.
/// \ingroup SourceAnimationBasicModule
///
/// Associated together because each Quat Pos pair forms a single transform at a set time.
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetMBA
{
public:
  ChannelSetMBA() {}
  ~ChannelSetMBA() {}

  MR::ChannelQuat* getChannelQuat() { return &m_channelQuat; }
  MR::ChannelPos* getChannelPos() { return &m_channelPos; }

  void locate(uint8_t** data);
  void dislocate();
  void relocate(uint8_t** data);

protected:
  MR::ChannelQuat m_channelQuat;
  MR::ChannelPos  m_channelPos;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AnimSectionInfoMBA
/// \brief Describes aspects of an AnimSectionBasic.
/// \ingroup SourceAnimationBasicModule
/// \see MR::AnimBase
//----------------------------------------------------------------------------------------------------------------------
class AnimSectionInfoMBA
{
public:
  AnimSectionInfoMBA() {}
  ~AnimSectionInfoMBA() {}

  void locate();
  void dislocate();

  uint32_t getStartFrame() const { return m_startFrame; }
  uint32_t getSectionSize() const { return m_sectionSize; }

  void setStartFrame(uint32_t startFrame) { m_startFrame = startFrame; }
  void setSectionSize(uint32_t sectionSize) { m_sectionSize = sectionSize; }

protected:
  uint32_t m_startFrame;  ///< Specified within the space of the animations total frame count.
  uint32_t m_sectionSize; ///< How large is this section in bytes.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AnimSectionMBA
/// \brief Cross section of an animation.
/// \ingroup SourceAnimationBasicModule
/// \see MR::AnimBase
///
/// Keyframe data is divided into sections. Each section records all bone
/// transforms over a specified time chunk of the whole animation.
/// Chunks are designed to be of optimum size for DMAing to SPUs or to reduce cache misses on other platforms.
//----------------------------------------------------------------------------------------------------------------------
class AnimSectionMBA
{
public:
  AnimSectionMBA() {}
  ~AnimSectionMBA() {}

  void locate(uint8_t** data); ///< OUT: updated data pointer (from this)
  void dislocate();
  void relocate();

  void computeFullAnimTransformSet(
    const ChannelSetMBAInfo*  info,
    const RigToAnimMap*       mapFromRigToAnim,    ///< Rig bones to Animation channels.
    const uint32_t            sectionFrameIndex,
    const float               interpolant,
    NMP::DataBuffer*          outputBuffer) const;

  void computeAnimTransformSubSet(
    const ChannelSetMBAInfo*  info,
    const RigToAnimMap*       mapFromRigToAnim,    ///< Rig bones to Animation channels.
    const uint32_t            outputSubsetSize,
    const uint16_t*           outputSubsetArray,   ///< Must be incrementally ordered
                                                   ///< and must be a sub set of the mapping output set.
    const uint32_t            sectionFrameIndex,
    const float               interpolant,
    NMP::DataBuffer*          outputBuffer) const;

  void computeSingleChannelTransform(
    const ChannelSetMBAInfo*  info,
    const uint32_t            animChannelSetIndex,  ///< Index into the animations set of channels.
                                                    ///< (all sections have the same number of channels).
    const uint32_t            sectionFrameIndex,
    const float               interpolant,
    NMP::Quat*                quatResult,
    NMP::Vector3*             posResult) const;

  NM_INLINE uint32_t getPosChanFrameCount(uint32_t animChannelSetIndex) const;
  NM_INLINE uint32_t getQuatChanFrameCount(uint32_t animChannelSetIndex) const;

  NM_INLINE const ChannelSetMBA* getChannelSets() const { return m_channelSets; }

  #if !defined(NM_HOST_CELL_SPU) && !defined(NM_HOST_CELL_PPU)
    #ifdef NM_DEBUG
  /// \brief Debug only facility for writing out sampled key frame data to file.
  ///
  /// Write out the value of all channels, interpolating between the specified key frame and the next.
  /// Specific to this classes method of fixed frame interval storage.
  void debugOutputFullAnimTransformSet(
    const ChannelSetMBAInfo* info,
    uint32_t                 sectionFrameIndex,  ///< Frame index to sample from.
    float                    interpolant,        ///< Interpolant (0.0f - 1.0f). Controls interpolation between
                                                 ///< this frame and the next.
    FILE*                    file                ///< File handle to write out to.
  ) const;
    #endif // NM_DEBUG
  #endif // NM_HOST_CELL_SPU

protected:
  uint32_t       m_numChannelSets;  ///< The number of channel sets contained within this animation section.
                                    ///< A channel set usually contains the key frame animation data for a
                                    ///< bone on a rig. Will be the same for all AnimSections in an animation.
  ChannelSetMBA* m_channelSets;     ///< All the channel sets for this animation.
                                    ///< Usually one channel set for a bone in a destination rig.
};

//----------------------------------------------------------------------------------------------------------------------
// AnimSectionMBA functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AnimSectionMBA::getPosChanFrameCount(uint32_t animChannelSetIndex) const
{
  return m_channelSets[animChannelSetIndex].getChannelPos()->getNumKeyFrames();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AnimSectionMBA::getQuatChanFrameCount(uint32_t animChannelSetIndex) const
{
  return m_channelSets[animChannelSetIndex].getChannelQuat()->getNumKeyFrames();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIM_SECTION_MBA_H
//----------------------------------------------------------------------------------------------------------------------
