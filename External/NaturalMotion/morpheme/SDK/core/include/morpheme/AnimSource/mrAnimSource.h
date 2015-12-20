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
#ifndef MR_ANIM_SOURCE
#define MR_ANIM_SOURCE
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMStringTable.h"
#include "NMPlatform/NMBuffer.h"
#include "morpheme/mrRigToAnimMap.h"
#include "morpheme/AnimSource/mrTrajectorySourceBase.h"
//----------------------------------------------------------------------------------------------------------------------

// To enable animation decompression profiling output.
#define NM_ANIM_PROFILINGx
#if defined(NM_ANIM_PROFILING)
  #define NM_ANIM_BEGIN_PROFILING(name)    NM_BEGIN_PROFILING(name)
  #define NM_ANIM_END_PROFILING()          NM_END_PROFILING()
#else // NM_ANIM_PROFILING
  #define NM_ANIM_BEGIN_PROFILING(name)
  #define NM_ANIM_END_PROFILING()
#endif // NM_ANIM_PROFILING

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup SourceAnimationModule Source Animation.
///
/// Classes and functions for extracting transform data from source animation data which may be in any possible format;
/// compressed or not.
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceBase;

//----------------------------------------------------------------------------------------------------------------------
/// \typedef MR::AnimFunctionTable
/// \brief Function pointer table which all Source Anim Types, which are to be used with NaturalMotion Node types,
/// must implement.
/// \ingroup SourceAnimationModule
///
/// Provides runtime polymorphisms for Animation Sources, removing the requirement for virtual functions.
/// This table only implements the base requirements of Animation functionality.
/// This is not intended as a general system of polymorphism for inherited Source Animation Types.
//----------------------------------------------------------------------------------------------------------------------
typedef struct
{
  /// \brief Calculates the transforms for the requested rig bones.
  ///
  /// Samples the source animation at the requested time through its playback duration.
  /// Inserts the results into the appropriate bone indexes in the output buffer.
  void (*animComputeAtTime) (
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

  /// \brief Calculate the transform for the requested Animation bone index.
  ///
  /// Samples the request source animation channel at the specified time through its playback duration.
  void (*animComputeAtTimeSingleTransform) (
    const AnimSourceBase* sourceAnimation,  ///< IN: Animation to sample.
    float                 time,             ///< IN: Time at which to sample the animation (seconds).
    uint32_t              rigChannelIndex,  ///< IN: Index of the rig bone to evaluate
    const AnimRigDef*     rig,              ///< IN: Hierarchy and bind poses
    const RigToAnimMap*   mapFromRigToAnim, ///< IN: So that results from sampled anim channels can be
                                            ///<     stored in the correct rig bone ordered transform buffer slot.
    NMP::Vector3*         pos,              ///< OUT: The resulting position is filled in here.
    NMP::Quat*            quat,             ///< OUT: The resulting orientation is filled in here.
    NMP::MemoryAllocator* allocator);

  /// \brief Returns the duration of this animation in seconds.
  float (*animGetDuration) (
    const AnimSourceBase* sourceAnimation   ///< Animation to query.
  );

  /// \brief Returns the number of channel sets contained within this animation.
  ///
  /// A channel set usually contains the key frame animation data for a bone on a rig.
  uint32_t (*animGetNumChannelSets) (
    const AnimSourceBase* sourceAnimation   ///< Animation to query.
  );

  /// \brief Returns the trajectory channel data related to this animation.  If this function pointer is NULL then
  /// AnimBase::animgetTrajectorySourceData() returns a NULL trajectory control.
  const TrajectorySourceBase* (*animGetTrajectorySourceData) (
    const AnimSourceBase* sourceAnimation);

  /// \brief Return the string table which contains the names of the animation channels which this animation contains
  ///
  /// Note that this function may return a NULL pointer if no string table exists.
  const NMP::OrderedStringTable* (*animGetChannelNameTable) (
    const AnimSourceBase* sourceAnimation);
} AnimFunctionTable;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AnimSourceBase
/// \brief Pseudo virtual interface for sampling of source animation data in to transform buffers.
/// \ingroup SourceAnimationModule
///
/// Provides a virtual interface to classes implementing any arbitrary method of animation decompression and sampling.
/// If Nodes are implemented to access animation data via this class, the underlying method of compression can be
/// replaced at any point and the existing Node types will still work.
/// This method also allows for different compression methods to be used concurrently and in different situations
/// allowing the use of the most appropriate method for different forms of data etc.
/// AnimSourceBase has an internal check if it is located or not to support caching transparency: the animation cache
/// (aka. registry) does not need to locate the source before giving it to the network.
/// NOTE: Nodes are not limited to use of this class it is entirely up to the Node author how to extract transform
/// data.
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceBase
{
public:

  void locate();
  void dislocate();

  /// \brief Calculates the transforms for the requested rig bones.
  ///
  /// Samples the source animation at the requested time through its playback duration.
  /// Inserts the results into the appropriate bone indexes in the output buffer.
  void animComputeAtTime(
    float               time,                     ///< IN: Time at which to sample the animation (seconds).
    const AnimRigDef*   rig,                      ///< IN: Hierarchy and bind poses
    const RigToAnimMap* mapFromRigToAnim,         ///< IN: So that results from sampled anim channels can be
                                                  ///<     stored in the correct rig bone ordered transform buffer slot.
    uint32_t            outputSubsetSize,         ///< IN: \see outputSubsetArray
    const uint16_t*     outputSubsetArray,        ///< IN: Channel set values will only be calculated if
                                                  ///<     output indexes appear both in this array and the
                                                  ///<     mapping array. Ignored if size is zero.
    NMP::DataBuffer*    outputTransformBuffer,    ///< OUT: Calculated transform data is stored and returned
                                                  ///<      in this provided set.
    NMP::MemoryAllocator* allocator) const;

  /// \brief Returns the duration of this animation in seconds.
  float animGetDuration() const;

  /// \brief Returns the number of channel sets contained within this animation.
  ///
  /// A channel set usually contains the key frame animation data for a bone on a rig.
  uint32_t animGetNumChannelSets() const;

  /// \brief
  const TrajectorySourceBase* animGetTrajectorySourceData() const;

  /// \brief Return the string table which contains the names of the animation channels which this animation contains.
  ///
  /// Note that this function may return a NULL pointer if no string table exists.
  const NMP::OrderedStringTable* animGetChannelNameTable() const;

  // \brief
  NM_INLINE AnimType getType() { return m_animType; }

  // Get the requirements of this animation instance. NOTE: excludes any section data.
  NM_INLINE const NMP::Memory::Format& getInstanceMemoryRequirements() const { return m_animInstanceMemReqs; }

  // use some pointer arithmetic to check if the animation is already located
  NM_INLINE bool isLocated() const { return m_isLocated; }

protected:
  AnimFunctionTable*  m_funcTable;           ///< Function pointer table providing runtime polymorphisms for Animation Source Class Types.
                                             ///<  Inherited Objects must set this pointer to their own table of functions.
                                             ///<  This table is only intended to implement the functions required by Animation sources
                                             ///<  that are to be used by NaturalMotion Source Nodes.
  NMP::Memory::Format m_animInstanceMemReqs; ///< Requirements of this animation.
  AnimType            m_animType;
  bool                m_isLocated;
  uint8_t             m_padding[2];
#ifdef NM_HOST_64_BIT
  // This padding is required to ensure that llvm and msvc 64-bit compilers pack sub-class members in the same alignment
  uint32_t            m_padding64;
#endif // NM_HOST_64_BIT

};

//----------------------------------------------------------------------------------------------------------------------
class AttribDataSourceAnim : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataSourceAnim* init(
    NMP::Memory::Resource&      resource,
    uint8_t                     registeredAnimFormatIndex,
    RuntimeAnimAssetID          animAssetID,
    RuntimeAssetID              rigToAnimMapAssetID,
    const NMP::Memory::Format&  rigToAnimMapMemoryFormat,
    uint16_t                    refCount);

  NM_INLINE AttribDataSourceAnim() { setType(ATTRIB_TYPE_SOURCE_ANIM); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_SOURCE_ANIM; }

  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void prepAnimForSPU(AttribData* target, NMP::MemoryAllocator* allocator);
  static void prepRigToAnimMapForSPU(AttribData* target, NMP::MemoryAllocator* allocator);
  static void prepTrajectoryForSPU(AttribData* target, NMP::MemoryAllocator* allocator);

  bool setAnimation(AnimSourceBase* anim);

  bool fixupRigToAnimMap();
  bool unfixRigToAnimMap();

  void setTrajectorySource(const TrajectorySourceBase* trajectorySource);

public:
  /// This is the actual animation.  Note that this pointer may be NULL if the animation has not yet been loaded.
  AnimSourceBase*             m_anim;
  RigToAnimMap*               m_rigToAnimMap;               ///< Maps from rig bone index to animation bone index.

  NMP::Vector3                m_transformAtStartPos;
  NMP::Quat                   m_transformAtStartQuat;
  NMP::Vector3                m_transformAtEndPos;
  NMP::Quat                   m_transformAtEndQuat;

  const TrajectorySourceBase* m_sourceTrajectoryChannel;    ///< Pointer into shared array of resources.
  uint32_t                    m_animSize;                   ///< Requirements of referenced animation.
  uint32_t                    m_rigToAnimMapSize;           ///< Requirements of referenced rig to anim map.
  uint32_t                    m_trajectorySize;             ///< Requirements of referenced trajectory.

  
  /// This is the global ID that will be passed to the runtime animation loader function.  It is the app's
  /// responsibility to convert this to a filename, or resolve it to a resource handle, depending on their asset
  /// packing scheme.
  RuntimeAnimAssetID          m_animAssetID;

  RuntimeAssetID              m_rigToAnimMapAssetID;        ///< The Network specific runtime ID of the RigToAnimMap asset used by
                                                            ///< this binding.

  uint8_t                     m_registeredAnimFormatIndex;  ///< This is the registry index for the animation source format.

  uint8_t                     m_startSyncEventIndex;        ///< Where this node should start playback.
                                                            ///<  Specified within the sync space extracted by the clip range.
  uint8_t                     m_clipStartSyncEventIndex;    ///< The event index within which the clip start point lies.
                                                            ///<  Specified within the source discrete tracks event space.
                                                            ///<  Note that if before event zero this number will still be zero.
  uint8_t                     m_clipEndSyncEventIndex;      ///< The event index within which the clip end point lies.
                                                            ///<  Specified within the source discrete tracks event space.
  
  float                       m_clipStartFraction;          ///< Fractional playback start point. 0.0 - 1.0.
  float                       m_clipEndFraction;            ///< Fractional playback end point. 0.0 - 1.0.
  float                       m_sourceAnimDuration;         ///< Source anim duration in seconds.

  uint32_t                    m_syncEventTrackIndex;        ///< ID of discrete event track to create sync event track from.

  bool                        m_playBackwards;              ///< Bool - update this node backwards.
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIM_SOURCE
//----------------------------------------------------------------------------------------------------------------------
