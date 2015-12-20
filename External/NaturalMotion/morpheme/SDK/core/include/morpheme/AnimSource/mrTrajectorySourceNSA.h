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
#ifndef MR_TRAJECTORY_SOURCE_NSA_H
#define MR_TRAJECTORY_SOURCE_NSA_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMBuffer.h"
#include "morpheme/AnimSource/mrTrajectorySourceBase.h"
#include "morpheme/AnimSource/mrAnimSourceUtils.h"
//----------------------------------------------------------------------------------------------------------------------

// nonstandard extension used : nameless struct/union
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4201)
#endif

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup SourceTrajectoryCompressedModule Source Trajectory Compressed
/// \ingroup SourceAnimationModule
///
/// Implementation of MR::TrajectorySourceBase with some level of compression.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TrajectorySourceNSA
/// \brief Implementation of MR::TrajectorySourceBase with some level of compression.
/// \ingroup SourceTrajectoryCompressedModule
/// \see MR::TrajectorySourceBase
///
/// Bone index 0 is a non-authored bone which we insert as a Root to the whole hierarchy.
/// The bone can be assumed to lie on the world origin when authoring (identity).
/// This should be the only bone with parent index of -1 in the hierarchy.
/// Bones which when authored had no explicit parent will be parented to this bone in the export process (parent index 0).
/// This Root bone will be used to place the character in the World, by setting a desired World Root Transform prior to
/// accumulating world transforms.
/// Delta transformations of trajectory bones are calculated relative to this Root.
/// Animations never contain data for rig bone 0 as this is automatically added by us.
//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceNSA : public TrajectorySourceBase
{
public:
  TrajectorySourceNSA() {}
  ~TrajectorySourceNSA() {}

  void locate();
  void dislocate();
  void relocate();

  static void computeTrajectoryTransformAtTime(
    const TrajectorySourceBase* trajectoryControl,
    float                       time,
    NMP::Quat&                  resultQuat,
    NMP::Vector3&               resultPos);

protected:
  void sampledDeltaPosDecompress(
    uint32_t animFrameIndex,
    float interpolant,
    NMP::Vector3& pOut) const;
  
  void sampledDeltaQuatDecompress(
    uint32_t animFrameIndex,
    float interpolant,
    NMP::Quat& qOut) const;

protected:
  // Trajectory Header info
  float                               m_sampleFrequency;    ///< Number of key frame samples per second.
  uint32_t                            m_numAnimFrames;

  // Quantisation information for the delta trajectory channels
  QuantisationScaleAndOffsetVec3      m_sampledDeltaPosKeysInfo;
  QuantisationScaleAndOffsetVec3      m_sampledDeltaQuatKeysInfo;

  // Sampled delta trajectory channels
  SampledPosKey*                      m_sampledDeltaPosKeys;
  SampledQuatKeyTQA*                  m_sampledDeltaQuatKeys;
};

} // namespace MR

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRAJECTORY_SOURCE_NSA_H
//----------------------------------------------------------------------------------------------------------------------
