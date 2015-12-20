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
#ifndef MR_TRAJECTORY_SOURCE_BASE_H
#define MR_TRAJECTORY_SOURCE_BASE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup SourceTrajectoryModule Source Trajectory.
/// \ingroup SourceAnimationModule
//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceBase;

//----------------------------------------------------------------------------------------------------------------------
/// \typedef getTrajectoryTransformAtTimeFn
/// \brief Type definition for the function to get trajectory data at a given time from a Trajectory control.
/// \ingroup SourceTrajectoryModule
//----------------------------------------------------------------------------------------------------------------------
typedef void (getTrajectoryTransformAtTimeFn)(
  const TrajectorySourceBase*  trajectoryControl,
  float                        bufferPlaybackPos,
  NMP::Quat&                   newTrajectoryAtt,
  NMP::Vector3&                newTrajectoryPos);

//----------------------------------------------------------------------------------------------------------------------
// Animation format type identifier.
typedef uint32_t TrajType;

#define TRAJ_TYPE_UNDEFINED ((MR::TrajType) 0xFFFFFFFF)
#define TRAJ_TYPE_MBA    ((MR::TrajType) GEN_NAMESPACED_ID_32(NM_ID_NAMESPACE, 0))
#define TRAJ_TYPE_ASA    ((MR::TrajType) GEN_NAMESPACED_ID_32(NM_ID_NAMESPACE, 1))
#define TRAJ_TYPE_NSA    ((MR::TrajType) GEN_NAMESPACED_ID_32(NM_ID_NAMESPACE, 2))
#define TRAJ_TYPE_QSA    ((MR::TrajType) GEN_NAMESPACED_ID_32(NM_ID_NAMESPACE, 3))

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TrajectorySourceBase
/// \brief A set of animation data that amends existing animation data appropriately for use with a labeled trajectory.
/// \ingroup SourceTrajectoryModule
///
/// Bone index 0 is a non-authored bone which we insert as a Root to the whole hierarchy.
/// The bone can be assumed to lie on the world origin when authoring (identity).
/// This should be the only bone with parent index of -1 in the hierarchy.
/// Bones which when authored had no explicit parent will be parented to this bone in the export process (parent index 0).
/// This Root bone will be used to place the character in the World, by setting a desired World Root Transform prior to
/// accumulating world transforms.
/// Delta transformations of trajectory bones are calculated relative to this Root.
/// Animations never contain data for rig bone 0 as this is automatically added by us.
///
/// This class is overridden by each implementation of an animation source format to provide trajectory data.
//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceBase
{
public:
  NM_INLINE void locate();
  NM_INLINE void dislocate();

  NM_INLINE void trajComputeTrajectoryTransformAtTime(
    float          bufferPlaybackPos,
    NMP::Quat&     newTrajectoryAtt,
    NMP::Vector3&  newTrajectoryPos) const;

  // \brief
  NM_INLINE TrajType getType() const { return m_trajType; }

  // Get the requirements of this animation instance. NOTE: excludes any section data.
  NM_INLINE NMP::Memory::Format getInstanceMemoryRequirements() const { return m_trajectoryInstanceMemReqs; }

protected:
  TrajectorySourceBase() {}
  ~TrajectorySourceBase() {}

protected:
  TrajType                        m_trajType;                  ///< Type of compression used for this trajectory.
  getTrajectoryTransformAtTimeFn* m_getTrajAtTime;             ///<
  NMP::Memory::Format             m_trajectoryInstanceMemReqs; ///< Requirements of this animation.

};

//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceBase functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TrajectorySourceBase::trajComputeTrajectoryTransformAtTime(
  float         bufferPlaybackPos,
  NMP::Quat&    newTrajectoryAtt,
  NMP::Vector3& newTrajectoryPos) const
{
  if (m_getTrajAtTime)
    m_getTrajAtTime(this, bufferPlaybackPos, newTrajectoryAtt, newTrajectoryPos);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TrajectorySourceBase::locate()
{
  NMP::endianSwap(m_trajType);
  NMP::endianSwap(m_trajectoryInstanceMemReqs);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void TrajectorySourceBase::dislocate()
{
  NMP::endianSwap(m_trajType);
  NMP::endianSwap(m_trajectoryInstanceMemReqs);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRAJECTORY_SOURCE_BASE_H
//----------------------------------------------------------------------------------------------------------------------
