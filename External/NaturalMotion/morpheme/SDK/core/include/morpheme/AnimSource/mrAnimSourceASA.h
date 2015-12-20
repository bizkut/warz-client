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
#ifndef MR_ANIMATION_SOURCE_ASA_H
#define MR_ANIMATION_SOURCE_ASA_H
//----------------------------------------------------------------------------------------------------------------------
#if !defined(NM_HOST_CELL_SPU) && !defined(NM_HOST_CELL_PPU)
  #ifdef NM_DEBUG
    #include <stdio.h>
  #endif // NM_DEBUG
#endif //#if !defined(NM_HOST_CELL_SPU) && !defined(NM_HOST_CELL_PPU)

#include "morpheme/mrTask.h"
#include "morpheme/AnimSource/mrAnimSourceUtils.h"
#include "morpheme/AnimSource/mrTrajectorySourceASA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup SourceAnimationBasicModule Source Animation Basic.
/// \ingroup SourceAnimationModule
///
/// Implementation of MR::AnimSourceBase.
/// Fixed sample frequency format with no compression.
//----------------------------------------------------------------------------------------------------------------------

class ChannelSetASA;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AnimSourceASA
/// \brief Implementation of AnimSourceBase virtual base class.
/// \ingroup SourceAnimationBasicModule
/// \see MR::AnimSourceBase
///
/// Fixed sample rate, quaternion Quat and vector Pos channels.
/// The last animation frame must be the same as the first in cyclic animations.
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceASA : public AnimSourceBase
{
public:
  AnimSourceASA();
  ~AnimSourceASA() {}

  void locate();
  void dislocate();
  void relocate();

  /// \brief Calculates the transforms for the requested rig bones.
  ///
  /// Samples the source animation at the requested time through its playback duration.
  /// Inserts the results into the appropriate bone indexes in the output buffer.
  static void computeAtTime(
    const AnimSourceBase* sourceAnimation,       ///< IN: Animation to sample.
    float                 time,                  ///< IN: Time at which to sample the animation (seconds).
    const AnimRigDef*     rig,                   ///< IN: Hierarchy and bind poses (not used)
    const RigToAnimMap*   mapFromRigToAnim,      ///< IN: So that results from sampled anim channels can be
                                                 ///<     stored in the correct rig bone ordered transform buffer slot.
    uint32_t              outputSubsetSize,      ///< IN: \see outputSubsetArray
    const uint16_t*       outputSubsetArray,     ///< IN: Channel set values will only be calculated if
                                                 ///<     output indexes appear both in this array and the
                                                 ///<     mapping array. Ignored if size is zero.
    NMP::DataBuffer*      outputTransformBuffer, ///< OUT: Calculated transform data is stored and returned
                                                 ///<      in this provided set.
    NMP::MemoryAllocator* allocator);

  /// \brief Calculate the transform for the requested Animation bone index.
  ///
  /// Samples the request source animation channel at the specified time through its playback duration.
  static void computeAtTimeSingleTransform(
    const AnimSourceBase* sourceAnimation,      ///< IN: Animation to sample.
    float                 time,                 ///< IN: Time at which to sample the animation (seconds).
    uint32_t              rigChannelIndex,      ///< IN: Index of the rig bone to evaluate
    const AnimRigDef*     rig,                  ///< IN: Hierarchy and bind poses (not used)
    const RigToAnimMap*   mapFromRigToAnim,     ///< IN: So that results from sampled anim channels can be
                                                ///<     stored in the correct rig bone ordered transform buffer slot.
    NMP::Vector3*         pos,                  ///< OUT: The resulting position is filled in here.
    NMP::Quat*            quat,                 ///< OUT: The resulting orientation is filled in here.
    NMP::MemoryAllocator* allocator);

  /// \brief Returns the duration of this animation in seconds.
  static NM_INLINE float getDuration(
    const AnimSourceBase* sourceAnimation       ///< Animation to query.
  );

  /// \brief Returns the number of channel sets contained within this animation.
  ///
  /// A channel set usually contains the key frame animation data for a bone on a rig.
  static NM_INLINE uint32_t getNumChannelSets(
    const AnimSourceBase* sourceAnimation       ///< Animation to query.
  );

  /// \brief Returns the trajectory channel data related to this animation.  If this function pointer is NULL then
  /// AnimSourceBase::animgetTrajectorySourceData() returns a NULL trajectory control.
  static NM_INLINE const TrajectorySourceBase* getTrajectorySourceData(
    const AnimSourceBase* sourceAnimation);

  /// \brief Return the string table which contains the names of the animation channels which this animation contains
  /// Note that this function may return a NULL pointer if no string table exists.
  static NM_INLINE const NMP::OrderedStringTable* getChannelNameTable(
    const AnimSourceBase* sourceAnimation);

  /// Accessors.
  float getSampleFrequency() const { return m_sampleFrequency; }
  uint32_t getNumSections() const  { return m_numSections; }

  /// \brief Which section of key frame data does this frame lie within.
  uint32_t findSectionIndexFromFrameIndex(uint32_t frameIndex) const;

protected:
  static AnimFunctionTable m_functionTable;   ///< Function table needed by each source animation type that inherits
                                              ///< from AnimSourceBase. Provides basic runtime polymorphisms, removing the
                                              ///< requirement for virtual functions.
  float                    m_duration;        ///< Playback length of this animation in seconds.
  float                    m_sampleFrequency; ///< Number of key frame samples per second.
                                              ///< The sample frequency is uniform across all the channels in an
                                              ///< animation, but it can vary between animations.
  uint32_t                 m_numSections;     ///< Keyframe data is divided into sections. Each section records all
                                              ///< bone transforms over a specified time chunk of the whole animation.
                                              ///< Chunks are designed to be of optimum size for DMAing to SPUs or
                                              ///< to reduce cache misses on other platforms.
  DataRef*                 m_sections;        ///< \see m_numSections
  AnimSectionInfoASA*      m_sectionsInfo;    ///< Information about each section.
  uint32_t                 m_numChannelSets;  ///< The number of channel sets contained within this animation.
                                              ///< A channel set usually contains the key frame animation data for a
                                              ///< bone on a rig.
  ChannelSetASAInfo*       m_channelSetsInfo; ///< Contains default information about each channel
  TrajectorySourceASA*     m_trajectoryData;  ///< Holds a set of animation data for handling a trajectory bone.
                                              ///< Can be NULL.
  NMP::OrderedStringTable*        m_channelNames;    ///< Optional string table holding the names of each channel set in this
                                              ///< anim.
};

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceASA functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float AnimSourceASA::getDuration(const AnimSourceBase* sourceAnimation)
{
  const AnimSourceASA* source;

  NMP_ASSERT(sourceAnimation);
  source = static_cast<const AnimSourceASA*> (sourceAnimation);
  return source->m_duration;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AnimSourceASA::getNumChannelSets(const AnimSourceBase* sourceAnimation)
{
  const AnimSourceASA* source;

  NMP_ASSERT(sourceAnimation);
  source = static_cast<const AnimSourceASA*> (sourceAnimation);
  return source->m_numChannelSets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const TrajectorySourceBase* AnimSourceASA::getTrajectorySourceData(const AnimSourceBase* sourceAnimation)
{
  const AnimSourceASA* source;

  NMP_ASSERT(sourceAnimation);
  source = static_cast<const AnimSourceASA*> (sourceAnimation);
  return source->m_trajectoryData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::OrderedStringTable* AnimSourceASA::getChannelNameTable(const AnimSourceBase* sourceAnimation)
{
  const AnimSourceASA* source;

  NMP_ASSERT(sourceAnimation);
  source = static_cast<const AnimSourceASA*> (sourceAnimation);

  return source->m_channelNames;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIMATION_SOURCE_ASA_H
//----------------------------------------------------------------------------------------------------------------------
