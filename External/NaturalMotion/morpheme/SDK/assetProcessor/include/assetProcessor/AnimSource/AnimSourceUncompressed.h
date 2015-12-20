// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#ifndef ANIM_SOURCE_UNCOMPRESSED_H
#define ANIM_SOURCE_UNCOMPRESSED_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "morpheme/mrRig.h"
#include "morpheme/mrRigToAnimMap.h"
#include "morpheme/AnimSource/mrChannelPos.h"
#include "morpheme/AnimSource/mrChannelQuat.h"
#include "assetProcessor/AnimSource/ChannelSetTableBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup UncompressedAnimationAssetProcessorModule Uncompressed Runtime Animation.
/// \ingroup AssetProcessorModule
///
/// Classes and functions for compiling source animation files into basic runtime ready animation assets.
/// These uncompressed runtime animations represent an intermediate format that is passed to any of the
/// animation compressors for compilation into the final runtime binary animation assets.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnimSourceUncompressedOptions
/// \brief A structure containing options for compiling uncompressed animations. Specifically the
/// numerical tolerances used for determining if a channel is changing or static.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceUncompressedOptions
{
public:
  AnimSourceUncompressedOptions();
  ~AnimSourceUncompressedOptions();

  NM_INLINE float getUnchangingChannelPosEps() const { return m_unchangingChannelPosEps; }
  NM_INLINE void setUnchangingChannelPosEps(float eps) { m_unchangingChannelPosEps = eps; }

  NM_INLINE float getUnchangingChannelQuatEps() const { return m_unchangingChannelQuatEps; }
  NM_INLINE void setUnchangingChannelQuatEps(float eps) { m_unchangingChannelQuatEps = eps; }

protected:
  float m_unchangingChannelPosEps;
  float m_unchangingChannelQuatEps;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ChannelSetOverrides
/// \brief A structure containing the bone IDs of the rig channels that have been overridden
/// for the trajectory system. Each root bone (i.e. the character's hips) that is overridden
/// is essentially re-parented from the worldspace under the trajectory bone. Multiple overridden
/// root bones may exist for animations containing multiple bodies with a single common trajectory.
/// i.e. a) The human charater, b) The character's skate-board.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetOverrides
{
public:
  /// \brief Calculate the memory required to create a ChannelSetOverrides from the supplied parameters
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numOverrides);

  /// \brief Create a new ChannelSetOverrides in the supplied memory
  static ChannelSetOverrides* init(
    NMP::Memory::Resource&  resource,
    uint32_t                numOverrides);

public:
  ChannelSetOverrides() {}
  ~ChannelSetOverrides() {}

  /// \brief Compute the channel set overrides for the specified rig.
  /// Form a set of bone indexes out of the trajectory and the character root bones.
  /// Work out which of the bones in this array are root bones i.e. the set of bones that have
  /// unique parents. The resulting bones are the ones that we need to store extra trajectory
  /// adjustment data for.
  void computeChannelSetOverrides(const MR::AnimRigDef* rig);

  /// \brief checks the overrides to determine if the channels are animated, enabling us
  /// to process the trajectory source transforms.
  bool hasValidTrajectorySourceOverrides(const MR::RigToAnimEntryMap* rigToAnimEntryMap) const;

  void copy(const ChannelSetOverrides& rhs);

  NM_INLINE uint32_t getNumChannelSetOverrides() const { return m_numOverrides; }
  NM_INLINE uint32_t* getChannelSetOverrides() { return m_rigBoneIDs; }
  NM_INLINE const uint32_t* getChannelSetOverrides() const { return m_rigBoneIDs; }

  void setChannelSetOverrides(uint32_t numOverrides, const uint32_t* rigBoneIDs);

  bool isARootBone(uint32_t rigBoneID) const;

protected:
  uint32_t  m_numOverrides;
  uint32_t* m_rigBoneIDs; ///< Bones that need to be adjusted when using trajectory system to convert
                          ///< the world motion of the character to an 'on the spot' animation.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ChannelSetRequirements
/// \brief A structure containing information about the actual number of position and quaternion
/// keyframes from the original animation file, and after analysis of the variation of values within
/// each channel, the number of required keyframe samples required to represent the data.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetRequirements
{
public:
  struct channelSetReqs_t
  {
    uint32_t m_numActualQuatSamples;    ///< Number of actual keyframe quat samples for the channel in the animation
    uint32_t m_numActualPosSamples;     ///< Number of actual keyframe position samples for the channel in the animation
    uint32_t m_numRequiredQuatSamples;  ///< Number of quats required for the channel (one if all quats the same)
    uint32_t m_numRequiredPosSamples;   ///< Number of positions required for the channel (one if all positions are the same)
  };

  /// \brief Calculate the memory required to create a ChannelSetRequirements from the supplied parameters.
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numChannelSets);

  /// \brief Create a new ChannelSetRequirements in the supplied memory.
  static ChannelSetRequirements* init(
    NMP::Memory::Resource&  resource,
    uint32_t                numChannelSets);

public:
  ChannelSetRequirements() {}
  ~ChannelSetRequirements() {}

  void computeChannelSetRequirements(
    const ChannelSetTable*        channelSetTable,      ///< IN: The table of animation channel set keyframes
    const MR::RigToAnimEntryMap*  rigToAnimEntryMap,    ///< IN: Rig channel index to animation channel index map
    const ChannelSetOverrides*    channelSetOverrides,  ///< IN: The set of overridden root bones modified by the trajectory control
    float                         posEps,               ///< IN: Epsilon tolerance used to determine an unchanging channel
    float                         quatEps               ///< IN: Epsilon tolerance used to determine an unchanging channel
    );

  void computeChannelSetRequirements(
    const ChannelSetTable*     channelSetTable,         ///< IN: The table of animation channel set keyframes
    float                      posEps,                  ///< IN: Epsilon tolerance used to determine an unchanging channel
    float                      quatEps                  ///< IN: Epsilon tolerance used to determine an unchanging channel
    );

  void updateChannelSetRequirements(
    const ChannelSetTable*     channelSetTable,       ///< IN: The table of animation channel set keyframes
    uint32_t                   animChannelIndex       ///< IN: The channel index for the information we wish to update.
    );

  void copy(const ChannelSetRequirements& rhs);

  NM_INLINE uint32_t getMaxNumKeyframes() const { return m_maxNumKeyFrames; }
  NM_INLINE void setMaxNumKeyframes(uint32_t maxNumKeyFrames) { m_maxNumKeyFrames = maxNumKeyFrames; }

  NM_INLINE float getUnchangingQuatChannelEps() const { return m_unchangingQuatEps; }
  NM_INLINE void setUnchangingQuatChannelEps(float quatEps) { m_unchangingQuatEps = quatEps; }

  NM_INLINE float getUnchangingPosChannelEps() const { return m_unchangingPosEps; }
  NM_INLINE void setUnchangingPosChannelEps(float posEps) { m_unchangingPosEps = posEps; }

  NM_INLINE uint32_t getNumChannelSets() const { return m_numChannelSets; }
  NM_INLINE channelSetReqs_t* getChannelSetRequirements() { return m_channelSetRequirements; }
  NM_INLINE const channelSetReqs_t* getChannelSetRequirements() const { return m_channelSetRequirements; }

  bool isChannelQuatUnchanging(uint32_t i) const;
  bool isChannelPosUnchanging(uint32_t i) const;

  uint32_t getNumUnchangingPosChannels() const;
  uint32_t getNumUnchangingQuatChannels() const;
  uint32_t getNumChangingPosChannels() const;
  uint32_t getNumChangingQuatChannels() const;

  bool isUnchangingPose() const;

protected:
  uint32_t          m_maxNumKeyFrames;        ///< The maximum number of keyframe samples required for any channel

  float             m_unchangingQuatEps;      ///< The epsilon tolerance use to determine an unchanging quat channel
  float             m_unchangingPosEps;       ///< The epsilon tolerance use to determine an unchanging pos channel

  uint32_t          m_numChannelSets;         ///< The number of (quat, pos) channel sets
  channelSetReqs_t* m_channelSetRequirements; ///< Array of channel set requirement information
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ChannelSetTableBuilderLS
/// \brief Builds a ChannelSetTable of local rig bone channel set keyframes.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetTableBuilderLS : public AP::ChannelSetTable
{
public:
  static void computeChannelSetTableLS(
    ChannelSetTable*              result,               ///< OUT: Where to write the local rig transforms
    const ChannelSetTable*        localChannelSets,     ///< IN: The local space animation channel set table
    const MR::AnimRigDef*         rig,                  ///< IN: The animation rig
    const MR::RigToAnimEntryMap*  rigToAnimEntryMap     ///< IN: Rig channel index to animation channel index map
    );

  static AP::ChannelSetTable* createChannelSetTableLS(
    const ChannelSetTable*        localChannelSets,     ///< IN: The local space animation channel set table
    const MR::AnimRigDef*         rig,                  ///< IN: The animation rig
    const MR::RigToAnimEntryMap*  rigToAnimEntryMap     ///< IN: Rig channel index to animation channel index map
    );
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ChannelSetTableBuilderWS
/// \brief Builds a ChannelSetTable of worldspace rig bone channel set keyframes by accumulating the
/// local space channel set keyframe data.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetTableBuilderWS : public AP::ChannelSetTable
{
public:
  /// \brief Function to compute the worldspace transforms in the specified memory buffer, which must
  /// correspond to a ChannelSetTable with the full complement of rig bones and number of keyframes.
  static void computeChannelSetTableWS(
    ChannelSetTable*              result,               ///< OUT: Where to accumulate the worldspace transforms
    const ChannelSetTable*        localChannelSets,     ///< IN: The local space animation channel set table
    const MR::AnimRigDef*         rig,                  ///< IN: The animation rig
    const MR::RigToAnimEntryMap*  rigToAnimEntryMap     ///< IN: Rig channel index to animation channel index map
    );

  static AP::ChannelSetTable* createChannelSetTableWS(
    const ChannelSetTable*        localChannelSets,     ///< IN: The local space animation channel set table
    const MR::AnimRigDef*         rig,                  ///< IN: The animation rig
    const MR::RigToAnimEntryMap*  rigToAnimEntryMap     ///< IN: Rig channel index to animation channel index map
    );
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::TrajectorySourceUncompressed
/// \brief A set of trajectory control data that amends existing overridden root bone channel data appropriately for
/// use with a labeled trajectory.
/// \ingroup UncompressedAnimationAssetProcessorModule
///
/// Bone index 0 is a non-authored bone which we insert as a Root to the whole hierarchy.
/// The bone can be assumed to lie on the world origin when authoring (identity).
/// This should be the only bone with parent index of -1 in the hierarchy.
/// Bones which when authored had no explicit parent will be parented to this bone in the export process (parent index 0).
/// This Root bone will be used to locate the character in the World, by setting a desired World Root Transform prior to
/// accumulating the worldspace transforms.
/// Delta transformations of trajectory bones are calculated relative to this Root.
/// Compiled animations never contain data for rig bone 0 as this is automatically added by us.
//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceUncompressed
{
public:
  /// \brief Calculate the memory required to create a TrajectorySourceUncompressed from the supplied parameters.
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numKeyframes);

  /// \brief Create a new TrajectorySourceUncompressed in the supplied memory.
  static TrajectorySourceUncompressed* init(
    NMP::Memory::Resource&  resource,
    uint32_t                numKeyframes);

public:
  TrajectorySourceUncompressed() {}
  ~TrajectorySourceUncompressed() {}

  void computeTrajectorySource(
    const ChannelSetOverrides*    channelSetOverrides,    ///< IN: Information about channels that are overridden by the trajectory system
    ChannelSetRequirements*       channelSetRequirements, ///< IN/OUT Information about the keyframe counts of each channel
    ChannelSetTable*              channelSets,            ///< IN/OUT: The channel set animation keyframe data (overriden channels are modified)
    const MR::AnimRigDef*         rig,                    ///< IN: The animation rig
    const MR::RigToAnimEntryMap*  rigToAnimEntryMap,      ///< IN: Rig channel index to animation channel index map
    float                         animSampleRate          ///< IN: The sample frame rate of the input source animation
    );

  void computeTrajectorySource(
    const TrajectorySourceUncompressed* trajectorySource  ///< IN: The trajectory source to resample
    );

  // Accessors
  NM_INLINE float getDuration() const { return m_duration; }
  NM_INLINE float getSampleFrequency() const { return m_sampleFrequency; }
  NM_INLINE uint32_t getMaxNumKeyframes() const { return m_deltaTrajectoryChannelSetReqs->getMaxNumKeyframes(); }

  NM_INLINE ChannelSetRequirements* getDeltaTrajectroyChannelSetRequirements() { return m_deltaTrajectoryChannelSetReqs; }
  NM_INLINE const ChannelSetRequirements* getDeltaTrajectroyChannelSetRequirements() const { return m_deltaTrajectoryChannelSetReqs; }

  NM_INLINE ChannelSetTable* getDeltaTrajectroyChannelSet() { return m_deltaTrajectoryChannelSet; }
  NM_INLINE const ChannelSetTable* getDeltaTrajectroyChannelSet() const { return m_deltaTrajectoryChannelSet; }

protected:
  // Animation information
  float                    m_duration;        ///< Playback length of this animation in seconds.
  float                    m_sampleFrequency; ///< Number of key frame samples per second.

  ChannelSetRequirements*  m_deltaTrajectoryChannelSetReqs; ///< The channel set requirements for the delta channel

  ChannelSetTable*         m_deltaTrajectoryChannelSet;     ///< Each key frame records the offset from the first frame of
                                                            ///< this channel.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnimSourceUncompressed
/// \brief This is a preprocessed animation with the correct joint channels and keyframe data stored in a
/// single contiguous memory block. This class contains additional compilation information that is not
/// necessary for a typical compressed animation source used in the runtime.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceUncompressed
{
public:
  /// \brief Calculate the memory required to create a AnimSourceUncompressed from the supplied parameters.
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t                numOverrides,
    uint32_t                numAnimChannelSets,
    uint32_t                numKeyframes,
    const NMP::OrderedStringTable* channelNames,
    bool                    calcTrajectory);

  /// \brief Create a new AnimSourceUncompressed in the supplied memory.
  static AnimSourceUncompressed* init(
    NMP::Memory::Resource&  resource,
    uint32_t                numOverrides,
    uint32_t                numAnimChannelSets,
    uint32_t                numKeyframes,
    const NMP::OrderedStringTable* channelNames,
    bool                    calcTrajectory);

public:
  AnimSourceUncompressed() {}
  ~AnimSourceUncompressed() {}

  // Accessors
  NM_INLINE float getDuration() const { return m_duration; }
  NM_INLINE float getSampleFrequency() const { return m_sampleFrequency; }
  NM_INLINE uint32_t getMaxNumKeyframes() const { return m_channelSetRequirements->getMaxNumKeyframes(); }
  bool getGenerateDeltas() const { return m_generateDeltas; }

  NM_INLINE const ChannelSetOverrides* getChannelSetOverrides() const { return m_channelSetOverrides; }
  NM_INLINE uint32_t getNumChannelSetOverrides() const { return m_channelSetOverrides->getNumChannelSetOverrides(); }

  NM_INLINE const ChannelSetRequirements* getChannelSetRequirements() const { return m_channelSetRequirements; }

  NM_INLINE uint32_t getNumChannelSets() const { return m_channelSets->getNumChannelSets(); }
  NM_INLINE const ChannelSetTable* getChannelSets() const { return m_channelSets; }
  NM_INLINE bool isValidChannelSet(uint32_t chanIndex) const { return chanIndex < m_channelSets->getNumChannelSets(); }

  NM_INLINE TrajectorySourceUncompressed* getTrajectorySource() { return m_trajectoryData; }
  NM_INLINE const TrajectorySourceUncompressed* getTrajectorySource() const { return m_trajectoryData; }
  NM_INLINE bool hasTrajectoryData() const { return m_trajectoryData != 0; }

  NM_INLINE const NMP::OrderedStringTable* getChannelNames() const { return m_channelNames; }

protected:
  // Animation information
  float                         m_duration;         ///< Playback length of this animation in seconds.
  float                         m_sampleFrequency;  ///< Number of key frame samples per second.
  bool                          m_generateDeltas;   ///< Flag indicating if the channel data has been converted
                                                    ///< to delta transforms for additive blending.

  // Channel set information
  ChannelSetOverrides*          m_channelSetOverrides;    ///< Information about channels that are overridden by the trajectory system
  ChannelSetRequirements*       m_channelSetRequirements; ///< Information about the keyframe counts of each channel

  // Channel set keyframe data
  ChannelSetTable*              m_channelSets;    ///< The channel set keyframe data table

  // Trajectory
  TrajectorySourceUncompressed* m_trajectoryData; ///< Holds a set of animation data for handling a trajectory bone.

  // Channel names
  NMP::OrderedStringTable*             m_channelNames;   ///< String table holding the names of each channel set in this anim.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnimSourceUncompressedBuilder
/// \brief Builds the basic uncompressed animation format required for morpheme runtime.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceUncompressedBuilder : public AnimSourceUncompressed
{
public:
  /// \brief Calculate the memory required to create a AnimSourceUncompressed from the supplied parameters
  static NMP::Memory::Format getMemoryRequirements(
    const MR::AnimRigDef*         rig,                      ///< IN: Animation rig (needed for calculation of trajectory info)
    const MR::RigToAnimEntryMap*  rigToAnimEntryMap,        ///< IN: Rig channel index to animation channel index map
    const ChannelSetTable*        animChannelSets,          ///< IN: Animation channel set keyframe data
    const NMP::OrderedStringTable*       channelNames              ///< IN: String table holding the names of each channel set in this anim
  );

  /// \brief Generate a correctly formatted animation for use with morpheme runtime from an XMD take/clip.
  static AnimSourceUncompressed* init(
    NMP::Memory::Resource&        resource,                 ///< IN/OUT: A resource describing where to build the animation source
    const MR::AnimRigDef*         rig,                      ///< IN: Animation rig (needed for calculation of trajectory info)
    const MR::RigToAnimEntryMap*  rigToAnimEntryMap,        ///< IN: Rig channel index to animation channel index map
    const ChannelSetTable*        animChannelSets,          ///< IN: Animation channel set keyframe data
    const NMP::OrderedStringTable*       channelNames,             ///< IN: String table holding the names of each channel set in this anim
    float                         animSampleRate,           ///< IN: The sample frame rate of the input source animation
    const AnimSourceUncompressedOptions& animSourceOptions  ///< IN: The options used to compile the uncompressed animation
  );

  /// \brief Converts the animation channel set data into a set of additive delta components
  /// relative to the first animation keyframe. Note that the (quat, pos) components are treated
  /// independently so that at runtime the channels can be additively blended:
  /// q(u) = qSlerp(I, q1, u) * q0
  /// t(u) = Lerp(0, t1, u) + t0
  /// where u is the interpolant, (q0, t0) are the base components and (q1, t1) are the delta components.
  static void convertToDeltaTransforms(
    ChannelSetTable*              animChannelSets           ///< IN/OUT: The channel set transform data
  );

  /// \brief Calculate the memory required to create a resampled AnimSourceUncompressed
  static NMP::Memory::Format getMemoryRequirements(
    const AnimSourceUncompressed* anim,                     ///< IN: The input uncompressed animation
    float                         desiredAnimResampleRate   ///< IN: The desired frame rate to resample the animation at
                                                            ///<     The actual resampling rate for the animation will be
                                                            ///<     chosen as the closest value that interpolates both the
                                                            ///<     start and end keyframes of the animation.
  );

  /// \brief Build a resampled AnimSourceUncompressed
  static AnimSourceUncompressed* init(
    NMP::Memory::Resource&        resource,                 ///< IN/OUT: A resource describing where to build the animation source
    const AnimSourceUncompressed* anim,                     ///< IN: The input uncompressed animation
    float                         desiredAnimResampleRate,  ///< IN: The desired frame rate to resample the animation at
                                                            ///<     The actual resampling rate for the animation will be
                                                            ///<     chosen as the closest value that interpolates both the
                                                            ///<     start and end keyframes of the animation.
    bool                          generateDeltas            ///< IN: Should this animation be stored as deltas from the first frame.
  );
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SOURCE_UNCOMPRESSED_H
//----------------------------------------------------------------------------------------------------------------------
