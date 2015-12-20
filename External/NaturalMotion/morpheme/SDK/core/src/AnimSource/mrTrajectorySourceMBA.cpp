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
#include "NMPlatform/NMMemory.h"
#include "morpheme/AnimSource/mrTrajectorySourceMBA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceMBA functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void TrajectorySourceMBA::locate()
{
  // Verify alignment.
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Trajectory sources must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  TrajectorySourceBase::locate();

  NMP::endianSwap(m_sampleFrequency);
  m_deltaTrajectoryInfo.locate();

  // Step over TrajectorySourceMBA class instance.
  uint8_t* data = (uint8_t*) this;
  data += NMP::Memory::align(sizeof(TrajectorySourceMBA), NMP_VECTOR_ALIGNMENT);

  // Delta trajectory channel set
  m_deltaTrajectoryChannelSet.locate(&data);

  m_getTrajAtTime = TrajectorySourceMBA::computeTrajectoryTransformAtTime;
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceMBA::dislocate()
{
  // Delta trajectory channel set.
  m_deltaTrajectoryChannelSet.dislocate();

  // Default information
  m_deltaTrajectoryInfo.dislocate();

  NMP::endianSwap(m_sampleFrequency);

  TrajectorySourceBase::dislocate();

  // For binary invariance. These will be fixed by in locate()
  m_getTrajAtTime = NULL;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceMBA::relocate()
{
  // Trajectory class itself.
  uint8_t* data = (uint8_t*)this;
  NMP_ASSERT(NMP_IS_ALIGNED(data, NMP_VECTOR_ALIGNMENT));
  data += sizeof(TrajectorySourceMBA);

  // Delta trajectory channel set.
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_deltaTrajectoryChannelSet.relocate(&data);
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceMBA::computeTrajectoryTransformAtTime(
  const TrajectorySourceBase* trajectoryControl,
  float                       bufferPlaybackPos,
  NMP::Quat&                  resultQuat,
  NMP::Vector3&               resultPos)
{
  float     frame;
  float     interpolant;
  uint32_t  frameIndex;
  TrajectorySourceMBA* trajControl = (TrajectorySourceMBA*)trajectoryControl;
  NMP_ASSERT(trajectoryControl);

  // Calculate frame index and interpolant.
  frame = bufferPlaybackPos * trajControl->m_sampleFrequency;
  frameIndex = (uint32_t) frame;
  interpolant = frame - frameIndex;
  if (interpolant < ERROR_LIMIT)
    interpolant = 0.0f;

  // Calculate Quat and Pos for this animation channel set.
  trajControl->m_deltaTrajectoryChannelSet.getChannelQuat()->getQuat(
    trajControl->m_deltaTrajectoryInfo.getChannelQuatInfo(),
    frameIndex,
    interpolant,
    resultQuat);

  trajControl->m_deltaTrajectoryChannelSet.getChannelPos()->getPos(
    trajControl->m_deltaTrajectoryInfo.getChannelPosInfo(),
    frameIndex,
    interpolant,
    resultPos);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
