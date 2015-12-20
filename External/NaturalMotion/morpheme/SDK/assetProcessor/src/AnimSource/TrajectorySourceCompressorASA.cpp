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
#include "morpheme/AnimSource/mrAnimSourceUtils.h"
#include "assetProcessor/AnimSource/ChannelQuatBuilder.h"
#include "assetProcessor/AnimSource/ChannelPosBuilder.h"
#include "assetProcessor/AnimSource/TrajectorySourceCompressorASA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TrajectorySourceASABuilder::getMemoryRequirements(const AnimSourceUncompressed* inputAnim)
{
  //-----------------------
  // Information
  NMP_VERIFY(inputAnim);
  const TrajectorySourceUncompressed* trajectory = inputAnim->getTrajectorySource();
  NMP_VERIFY(trajectory);
  const ChannelSetRequirements* deltaTrajChanSetReqs = trajectory->getDeltaTrajectroyChannelSetRequirements();
  NMP_VERIFY(deltaTrajChanSetReqs);

  //-----------------------
  // Header
  NMP::Memory::Format memReqs(0, NMP_VECTOR_ALIGNMENT);
  memReqs += NMP::Memory::Format(sizeof(MR::TrajectorySourceASA), NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Delta trajectory channel set
  const ChannelSetTable* trajChannelSet = trajectory->getDeltaTrajectroyChannelSet();

  // Quat channel
  if (!deltaTrajChanSetReqs->isChannelQuatUnchanging(0))
  {
    // Pack the required number of quaternion values into the channel
    NMP::Memory::Format memReqsQuat =
      AP::ChannelQuatBuilder::channelQuatGetMemoryRequirements(trajChannelSet->getNumKeyFrames());
    memReqs += memReqsQuat;
  }

  // Pos channel
  if (!deltaTrajChanSetReqs->isChannelPosUnchanging(0))
  {
    // Pack the required number of position values into the channel
    NMP::Memory::Format memReqsPos =
      AP::ChannelPosBuilder::channelPosGetMemoryRequirements(trajChannelSet->getNumKeyFrames());
    memReqs += memReqsPos;
  }

  //-----------------------
  // Pack whole structure for optimal DMA transfers.
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceASABuilder::init(
  const AnimSourceUncompressed* inputAnim,
  MR::TrajectorySourceASA*      trajectorySource,
  const NMP::Memory::Format&    memoryReqs)
{
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(trajectorySource);

  TrajectorySourceASABuilder* trajectorySourceBuilder = (TrajectorySourceASABuilder*)trajectorySource;

  // Set the compression type.
  trajectorySourceBuilder->m_trajType = TRAJ_TYPE_ASA;

  MR::ChannelSetASAInfo& trajChanSetInfo = trajectorySourceBuilder->m_deltaTrajectoryInfo;

  //-----------------------
  // Input anim
  const TrajectorySourceUncompressed* trajSource = inputAnim->getTrajectorySource();
  NMP_VERIFY(trajSource);
  const ChannelSetTable* trajDeltaChannelSet = trajSource->getDeltaTrajectroyChannelSet();
  const NMP::Quat* trajChannelQuat = trajDeltaChannelSet->getChannelQuat(0);
  const NMP::Vector3* trajChannelPos = trajDeltaChannelSet->getChannelPos(0);

  const ChannelSetRequirements* deltaTrajChanSetReqs = trajSource->getDeltaTrajectroyChannelSetRequirements();
  NMP_VERIFY(deltaTrajChanSetReqs);
  const ChannelSetRequirements::channelSetReqs_t* trajChanReqs = deltaTrajChanSetReqs->getChannelSetRequirements();
  NMP_VERIFY(trajChanReqs);

  //-----------------------
  // Header
  uint8_t* data = (uint8_t*)trajectorySource;
  data += NMP::Memory::align(sizeof(MR::TrajectorySourceASA), NMP_VECTOR_ALIGNMENT);
  trajectorySourceBuilder->m_sampleFrequency = inputAnim->getSampleFrequency();

  // Store overall memory requirements.
  trajectorySourceBuilder->m_trajectoryInstanceMemReqs = memoryReqs;

  //-----------------------
  // Channel set default info
  ChannelSetASAInfoBuilder::init(
    trajChanSetInfo,
    trajChanReqs->m_numRequiredQuatSamples,
    trajChannelQuat,
    trajChanReqs->m_numRequiredPosSamples,
    trajChannelPos);

  //-----------------------
  // Quat channel
  if (deltaTrajChanSetReqs->isChannelQuatUnchanging(0))
  {
    AP::ChannelQuatBuilder::initChannelQuat(
      0, // Store no samples
      trajChannelQuat,
      *trajectorySourceBuilder->m_deltaTrajectoryChannelSet.getChannelQuat(),
      &data);
  }
  else
  {
    AP::ChannelQuatBuilder::initChannelQuat(
      trajChanReqs->m_numRequiredQuatSamples,
      trajChannelQuat,
      *trajectorySourceBuilder->m_deltaTrajectoryChannelSet.getChannelQuat(),
      &data);
  }

  //-----------------------
  // Pos channel
  if (deltaTrajChanSetReqs->isChannelPosUnchanging(0))
  {
    AP::ChannelPosBuilder::initChannelPos(
      0, // Store no samples
      trajChannelPos,
      *trajectorySourceBuilder->m_deltaTrajectoryChannelSet.getChannelPos(),
      &data);
  }
  else
  {
    AP::ChannelPosBuilder::initChannelPos(
      trajChanReqs->m_numRequiredPosSamples,
      trajChannelPos,
      *trajectorySourceBuilder->m_deltaTrajectoryChannelSet.getChannelPos(),
      &data);
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
