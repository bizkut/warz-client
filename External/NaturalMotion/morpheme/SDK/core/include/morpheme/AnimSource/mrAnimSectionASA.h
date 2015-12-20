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
#ifndef MR_ANIM_SECTION_ASA_H
#define MR_ANIM_SECTION_ASA_H
//----------------------------------------------------------------------------------------------------------------------
#if !defined(NM_HOST_CELL_SPU) && !defined(NM_HOST_CELL_PPU)
  #ifdef NM_DEBUG
    #include <stdio.h>
  #endif // NM_DEBUG
#endif //#if !defined(NM_HOST_CELL_SPU) && !defined(NM_HOST_CELL_PPU)

#include "morpheme/mrTask.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/AnimSource/mrChannelPos.h"
#include "morpheme/AnimSource/mrChannelQuat.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ChannelSetBasicInfo
/// \brief Stores information about the overall channel frame data.
/// \ingroup SourceAnimationBasicModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetASAInfo
{
public:
  ChannelSetASAInfo() {}
  ~ChannelSetASAInfo() {}

  void locate();
  void dislocate();

  const NMP::Vector3&  getChannelPosInfo()  const { return m_chanPosDefault; }
  const NMP::Quat&     getChannelQuatInfo() const { return m_chanQuatDefault; }

protected:
  NMP::Vector3 m_chanPosDefault;
  NMP::Quat    m_chanQuatDefault;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::ChannelSetASA
/// \brief A position and quaternion channel associated together.
/// \ingroup SourceAnimationBasicModule
///
/// Associated together because each Quat Pos pair forms a single transform at a set time.
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetASA
{
public:
  ChannelSetASA() {}
  ~ChannelSetASA() {}

  MR::ChannelQuat* getChannelQuat() { return &m_channelQuat; }
  MR::ChannelPos*  getChannelPos()  { return &m_channelPos; }

  void locate(uint8_t** data);
  void dislocate();
  void relocate(uint8_t** data);

protected:
  MR::ChannelQuat m_channelQuat;
  MR::ChannelPos  m_channelPos;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AnimSectionInfoASA
/// \brief Describes aspects of an AnimSectionBasic.
/// \ingroup SourceAnimationBasicModule
/// \see MR::AnimBase
//----------------------------------------------------------------------------------------------------------------------
class AnimSectionInfoASA
{
public:
  AnimSectionInfoASA() {}
  ~AnimSectionInfoASA() {}

  void locate();
  void dislocate();

  uint32_t getStartFrame()  const { return m_startFrame; }
  uint32_t getSectionSize() const { return m_sectionSize; }
  uint32_t getHeaderSize() const { return m_headerSize; }

  void setStartFrame(uint32_t startFrame)   { m_startFrame = startFrame; }
  void setSectionSize(uint32_t sectionSize) { m_sectionSize = sectionSize; }
  void setHeaderSize(uint32_t headerSize)   { m_headerSize = headerSize; }

protected:
  uint32_t m_startFrame;  ///< Specified within the space of the animations total frame count.
  uint32_t m_sectionSize; ///< How large is this section in bytes.
  uint32_t m_headerSize;  ///< Size of the AnimSectionASA header plus the DataRefs pointing to the subsections.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AnimSubSectionASA
/// \brief Subsection of a Cross section of an animation.
/// \ingroup SourceAnimationBasicModule
/// \see MR::AnimBase
//----------------------------------------------------------------------------------------------------------------------
class AnimSubSectionASA
{
public:
  AnimSubSectionASA() {}
  ~AnimSubSectionASA() {}

  void locate(uint8_t* data);
  void dislocate();
  void relocate();

  void computeFullAnimTransformSet(
    const ChannelSetASAInfo*  info,
    const RigToAnimMap*       mapFromRigToAnim,    ///< Rig bones to Animation channels.
    const uint32_t            sectionFrameIndex,
    const float               interpolant,
    NMP::Quat*                quatResult,
    NMP::Vector3*             posResult,
    NMP::DataBuffer*          outputBuffer) const;

  void computeAnimTransformSubSet(
    const ChannelSetASAInfo*         info,
    const RigToAnimMap*              mapFromRigToAnim,  ///< Rig bones to Animation channels.
    const uint32_t                   outputSubsetSize,
    const uint16_t*                  outputSubsetArray, ///< Must be incrementally ordered
                                                        ///< and must be a sub set of the mapping output set.
    const uint32_t                   sectionFrameIndex,
    const float                      interpolant,
    NMP::Quat*                       outQuat,
    NMP::Vector3*                    outPos,
    NMP::DataBuffer*                 outputBuffer) const;

  void computeSingleChannelTransform(
    const ChannelSetASAInfo*  info,
    const uint32_t            animChannelSetIndex,  ///< Index into the animations set of channels.
                                                    ///< (all sections have the same number of channels).
    const uint32_t            sectionFrameIndex,
    const float               interpolant,
    NMP::Quat*                quatResult,
    NMP::Vector3*             posResult) const;

  NM_INLINE uint32_t getPosChanFrameCount(uint32_t animChannelSetIndex) const;
  NM_INLINE uint32_t getQuatChanFrameCount(uint32_t animChannelSetIndex) const;
  NM_INLINE ChannelSetASA* getChannelSets() { return m_channelSets; }
  NM_INLINE uint32_t getNumChannelSets() const { return m_numChannelSets; }
  NM_INLINE uint32_t getOffset() const { return m_offset; }
  NM_INLINE size_t   getSubsectionSize() const { return m_subsectionSize; }

protected:
  size_t         m_subsectionSize;   ///<
  uint32_t       m_subsectionID;     ///< Number of the subsection within the section
  uint32_t       m_numChannelSets;   ///< The number of channel sets contained within this animation section.
                                     ///< A channel set usually contains the key frame animation data for a
                                     ///< bone on a rig. Will be the same for all AnimSections in an animation.
  uint32_t       m_offset;           ///< The number of channels of the previous subsections
  ChannelSetASA* m_channelSets;      ///< All the channel sets for this animation.
                                     ///< Usually one channel set for a bone in a destination rig.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AnimSectionASA
/// \brief Cross section of an animation.
/// \ingroup SourceAnimationBasicModule
/// \see MR::AnimBase
///
/// Keyframe data is divided into sections. Each section records all bone
/// transforms over a specified time chunk of the whole animation.
/// Chunks are designed to be of optimum size for DMAing to SPUs or to reduce cache misses on other platforms.
//----------------------------------------------------------------------------------------------------------------------
class AnimSectionASA
{
public:
  AnimSectionASA() {}
  ~AnimSectionASA() {}

  void locate(MR::AnimSectionASA* section);
  void dislocate();
  void relocate();

  void computeFullAnimTransformSet(
    const ChannelSetASAInfo*  info,
    const RigToAnimMap*       mapFromRigToAnim,    ///< Rig bones to Animation channels.
    const uint32_t            sectionFrameIndex,
    const float               interpolant,
    NMP::MemoryAllocator*     allocator,
    NMP::DataBuffer*          outputBuffer) const;

  void computeAnimTransformSubSet(
    const ChannelSetASAInfo*  info,
    const RigToAnimMap*       mapFromRigToAnim,    ///< Rig bones to Animation channels.
    const uint32_t            outputSubsetSize,
    const uint16_t*           outputSubsetArray,   ///< Must be incrementally ordered
                                                   ///< and must be a sub set of the mapping output set.
    const uint32_t            sectionFrameIndex,
    const float               interpolant,
    NMP::MemoryAllocator*     allocator,
    NMP::DataBuffer*          outputBuffer) const;

  void computeSingleChannelTransform(
    const ChannelSetASAInfo*  info,
    const uint32_t            animChannelSetIndex,  ///< Index into the animations set of channels.
                                                    ///< (all sections have the same number of channels).
    const uint32_t            sectionFrameIndex,
    const float               interpolant,
    NMP::MemoryAllocator*     allocator,
    NMP::Quat*                quatResult,
    NMP::Vector3*             posResult) const;

  uint32_t getSubsectionIndex(
    const uint32_t        animChannelIndex,
    NMP::MemoryAllocator* allocator) const;
  ChannelSetASA* getChannelSets(
    const size_t          subsectionSize,
    const uint32_t        subsectionID,
    NMP::MemoryAllocator* allocator) const;
  uint32_t getPosChanFrameCount(
    uint32_t              animChannelSetIndex,
    NMP::MemoryAllocator* allocator) const;
  uint32_t getQuatChanFrameCount(
    uint32_t              animChannelSetIndex,
    NMP::MemoryAllocator* allocator) const;

  NM_INLINE DataRef* getSubsections() const { return m_subsections; }
  NM_INLINE void      setSubsections(DataRef* subsection) { m_subsections = subsection; }

protected:

  uint32_t         m_numSubSections;    ///< Total number of subsections within the section
  size_t           m_maxSubsectionSize; ///< Size of the biggest subsection. Useful to allocate subsections in the DMA
  DataRef*         m_subsections;       ///< Array of pointers to each subsection
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIM_SECTION_ASA_H
//----------------------------------------------------------------------------------------------------------------------
