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
#ifndef MR_TRAJECTORY_SOURCE_MBA_H
#define MR_TRAJECTORY_SOURCE_MBA_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMBuffer.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/AnimSource/mrAnimSectionMBA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class ChannelSetMBA;

//----------------------------------------------------------------------------------------------------------------------
/// \class TrajectorySourceMBA
/// \brief Implementation of MR::TrajectorySourceBase with no compression.
/// \ingroup SourceTrajectoryModule
/// \see MR::TrajectorySourceBase
///
/// This data is specific to a Rig.
/// Bone index 0 is a non-authored bone which we insert as a Root to the whole hierarchy.
/// The bone can be assumed to lie on the world origin when authoring (identity).
/// This should be the only bone with parent index of -1 in the hierarchy.
/// Bones which when authored had no explicit parent will be parented to this bone in the export process (parent index 0).
/// This Root bone will be used to place the character in the World, by setting a desired World Root Transform prior to
/// accumulating world transforms.
/// Delta transformations of trajectory bones are calculated relative to this Root.
/// Animations never contain data for rig bone 0 as this is automatically added by us.
//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceMBA : public TrajectorySourceBase
{
public:
  TrajectorySourceMBA() {}
  ~TrajectorySourceMBA() {}

  void locate();
  void dislocate();
  void relocate();

  static void computeTrajectoryTransformAtTime(
    const TrajectorySourceBase* trajectoryControl,
    float                       bufferPlaybackPos,
    NMP::Quat&                  resultQuat,
    NMP::Vector3&               resultPos);

protected:
  float             m_sampleFrequency;            ///< Number of key frame samples per second.
  ChannelSetMBAInfo m_deltaTrajectoryInfo;        ///< Contains default information about the delta trajectory channel
  ChannelSetMBA     m_deltaTrajectoryChannelSet;  ///< Each key frames records the offset from the first frame of
                                                  ///< this channel.
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRAJECTORY_SOURCE_MBA_H
//----------------------------------------------------------------------------------------------------------------------
