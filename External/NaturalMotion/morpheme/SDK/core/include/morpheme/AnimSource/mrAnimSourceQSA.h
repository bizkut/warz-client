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
#ifndef MR_ANIMATION_SOURCE_QSA_H
#define MR_ANIMATION_SOURCE_QSA_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMBuffer.h"
#include "morpheme/mrTask.h"
#include "morpheme/AnimSource/mrAnimSourceUtils.h"
#include "morpheme/AnimSource/mrAnimSectionQSA.h"
#include "morpheme/AnimSource/mrTrajectorySourceQSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AnimSourceQSA
/// \brief Implementation of AnimSourceBase virtual base class.
/// \ingroup SourceAnimationCompressedModule
/// \see MR::AnimSourceBase
///
/// Quaternion spline curve based compression scheme.
/// The last animation frame must be the same as the first in cyclic animations.
/// Note this is a lossy compression scheme.
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceQSA : public AnimSourceBase
{
public:  
  //-----------------------
  /// \brief Calculates the transforms for the requested rig bones.
  ///
  /// Samples the source animation at the requested time through its playback duration.
  /// Inserts the results into the appropriate bone indexes in the output buffer.
  static void computeAtTime(
    const AnimSourceBase* sourceAnimation,       ///< IN: Animation to sample.
    float                 time,                  ///< IN: Time at which to sample the animation (seconds).
    const AnimRigDef*     rig,                   ///< IN: Hierarchy and bind poses
    const RigToAnimMap*   mapFromRigToAnim,      ///< IN: So that results from sampled anim channels can be
                                                 ///<     stored in the correct rig bone ordered transform buffer slot.
    uint32_t              outputSubsetSize,      ///< IN: \see outputSubsetArray
    const uint16_t*       outputSubsetArray,     ///< IN: Channel set values will only be calculated if
                                                 ///<     output indexes appear both in this array and the 
                                                 ///<     mapping array. Ignored if size is zero.
    NMP::DataBuffer*      outputTransformBuffer, ///< OUT: Calculated transform data is stored and returned
                                                 ///<      in this provided set.
    NMP::MemoryAllocator* allocator);

  //-----------------------
  /// \brief Calculate the transform for the requested Animation bone index.
  ///
  /// Samples the request source animation channel at the specified time through its playback duration.
  static void computeAtTimeSingleTransform(
    const AnimSourceBase* sourceAnimation,       ///< IN: Animation to sample.
    float                 time,                  ///< IN: Time at which to sample the animation (seconds).
    uint32_t              rigChannelIndex,       ///< IN: Index of the rig bone to evaluate
    const AnimRigDef*     rig,                   ///< IN: Hierarchy and bind poses
    const RigToAnimMap*   mapFromRigToAnim,      ///< IN: So that results from sampled anim channels can be
                                                 ///<     stored in the correct rig bone ordered transform buffer slot.
    NMP::Vector3*         pos,                   ///< OUT: The resulting position is filled in here.
    NMP::Quat*            quat,                  ///< OUT: The resulting orientation is filled in here.
    NMP::MemoryAllocator* allocator);

  //-----------------------
  /// \brief Returns the duration of this animation in seconds.
  static NM_INLINE float getDuration(
    const AnimSourceBase* sourceAnimation        ///< Animation to query.
    );

  /// \brief Returns the number of channel sets contained within this animation.
  ///
  /// A channel set usually contains the key frame animation data for a bone on a rig.
  static NM_INLINE uint32_t getNumChannelSets(
    const AnimSourceBase* sourceAnimation        ///< Animation to query.
    );

  /// \brief Returns the trajectory channel data related to this animation.  If this function pointer is NULL then
  /// AnimSourceBase::animGetTrajectoryChannelData() returns a NULL trajectory control.
  static NM_INLINE const TrajectorySourceBase* getTrajectoryChannelData(
    const AnimSourceBase* sourceAnimation);

  /// \brief Return the string table which contains the names of the animation channels which this animation contains
  ///
  /// Note that this function may return a NULL pointer if no string table exists.
  static NM_INLINE const NMP::OrderedStringTable* getChannelNameTable(
    const AnimSourceBase* sourceAnimation);

  static uint32_t knotInterval(
    uint16_t        numKnots,
    const uint16_t* knots,
    uint16_t        animFrameIndex);

public:
  AnimSourceQSA();
  ~AnimSourceQSA() {}

  void locate();
  void dislocate();
  void relocate();

  void initOutputBuffer(
    const AnimRigDef* rig,
    InternalDataQSA* internalData,
    NMP::DataBuffer* outputTransformBuffer) const;

  void convertToLocalSpace(
    const AnimRigDef* rig,
    const AnimToRigTableMap* animToRigTableMap,
    InternalDataQSA* internalData,
    NMP::DataBuffer* outputTransformBuffer) const;

  //-----------------------
  /// Accessors
  float getDuration() const { return m_duration; }

  float getSampleFrequency() const { return m_sampleFrequency; }

  uint32_t getNumSections() const { return m_numSections; }

  const DataRef* getSections() const { return m_sections; }

  const AnimSectionInfoQSA* getSectionsInfo() const { return m_sectionsInfo; }

  /// \brief Which section of key frame data does this frame lie within.
  uint32_t findSectionIndexFromFrameIndex(uint32_t frameIndex) const;

  const CompToAnimChannelMap* getUnchangingPosCompToAnimMap() const { return m_unchangingPosCompToAnimMap; }
  const CompToAnimChannelMap* getUnchangingQuatCompToAnimMap() const { return m_unchangingQuatCompToAnimMap; }
  const CompToAnimChannelMap* getSampledPosCompToAnimMap() const { return m_sampledPosCompToAnimMap; }
  const CompToAnimChannelMap* getSampledQuatCompToAnimMap() const { return m_sampledQuatCompToAnimMap; }
  const CompToAnimChannelMap* getSplinePosCompToAnimMap() const { return m_splinePosCompToAnimMap; }
  const CompToAnimChannelMap* getSplineQuatCompToAnimMap() const { return m_splineQuatCompToAnimMap; }

protected:
  //-----------------------
  // Header information
  static AnimFunctionTable        m_functionTable;      ///< Function table needed by each source animation type that inherits
                                                        ///< from AnimSourceBase. Provides basic runtime polymorphisms, removing the 
                                                        ///< requirement for virtual functions.
  float                           m_duration;           ///< Playback length of this animation in seconds.
  float                           m_sampleFrequency;    ///< Number of key frame samples per second.
                                                        ///< The sample frequency is uniform across all the channels in an 
                                                        ///< animation, but it can vary between animations.
  uint32_t                        m_numChannelSets;     ///< The number of channel sets contained within this animation.
                                                        ///< A channel set usually contains the key frame animation data for a 
                                                        ///< bone on a rig.
  uint32_t                        m_numSections;        ///< Keyframe data is divided into sections. Each section records all bone
  bool                            m_useDefaultPose;     ///< Use default pose with unchanging channel data
  bool                            m_useWorldSpace;      ///< The space the animated channels are compressed in.
  bool                            m_generateDeltas;     ///< Flag indicating if the channel data has been converted
                                                        ///< to delta transforms for additive blending.

  //-----------------------
  // Compression to animation channel maps
  uint32_t                        m_maxNumCompChannels;               ///< The maximum number of compression channels that are used
  CompToAnimChannelMap*           m_unchangingPosCompToAnimMap;       ///< The unchanging pos comp to anim channel map
  CompToAnimChannelMap*           m_unchangingQuatCompToAnimMap;      ///< The unchanging quat comp to anim channel map
  CompToAnimChannelMap*           m_sampledPosCompToAnimMap;          ///< The sampled pos comp to anim channel map
  CompToAnimChannelMap*           m_sampledQuatCompToAnimMap;         ///< The sampled quat comp to anim channel map
  CompToAnimChannelMap*           m_splinePosCompToAnimMap;           ///< The spline pos comp to anim channel map
  CompToAnimChannelMap*           m_splineQuatCompToAnimMap;          ///< The spline quat comp to anim channel map

  //-----------------------
  // Sections information
  AnimSectionInfoQSA*             m_sectionsInfo;       ///< Information about each section (i.e. start frame and section size)

  //-----------------------
  // Channel sets quantisation information
  ChannelSetInfoQSA*              m_channelSetsInfo;    ///< Quantisation and channel map information for each section

  //-----------------------
  // Sections
  DataRef*                        m_sections;           ///< transforms over a specified time chunk of the whole animation.
                                                        ///< Chunks are designed to be of optimum size for DMAing to SPUs or 
                                                        ///< to reduce cache misses on other platforms.

  //-----------------------
  // Trajectory
  TrajectorySourceQSA*            m_trajectoryData;     ///< Holds a set of animation data for handling a trajectory bone.
                                                        ///< Can be NULL.

  //-----------------------
  // Channel names table
  NMP::OrderedStringTable*               m_channelNames;       ///< Optional string table holding the names of each channel set in this anim.
};

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceQSA inline functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float AnimSourceQSA::getDuration(const AnimSourceBase* sourceAnimation)
{
  const AnimSourceQSA* compressedSource;

  NMP_ASSERT(sourceAnimation);
  compressedSource = static_cast<const AnimSourceQSA*> (sourceAnimation);
  return compressedSource->m_duration;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AnimSourceQSA::getNumChannelSets(const AnimSourceBase* sourceAnimation)
{
  const AnimSourceQSA* compressedSource;

  NMP_ASSERT(sourceAnimation);
  compressedSource = static_cast<const AnimSourceQSA*> (sourceAnimation);
  return compressedSource->m_numChannelSets;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const TrajectorySourceBase* AnimSourceQSA::getTrajectoryChannelData(const AnimSourceBase* sourceAnimation)
{
  const AnimSourceQSA* compressedSource;

  NMP_ASSERT(sourceAnimation);
  compressedSource = static_cast<const AnimSourceQSA*> (sourceAnimation);
  return compressedSource->m_trajectoryData;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::OrderedStringTable* AnimSourceQSA::getChannelNameTable(const AnimSourceBase* sourceAnimation)
{
  const AnimSourceQSA* compressedSource;

  NMP_ASSERT(sourceAnimation);
  compressedSource = static_cast<const AnimSourceQSA*> (sourceAnimation);

  return compressedSource->m_channelNames;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIMATION_SOURCE_QSA_H
//----------------------------------------------------------------------------------------------------------------------
