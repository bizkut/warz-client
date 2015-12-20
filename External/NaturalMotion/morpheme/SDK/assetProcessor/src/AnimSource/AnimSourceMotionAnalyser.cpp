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
#include <limits>
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMPosSplineFitterTangents.h"
#include "NMNumerics/NMQuatSplineFitterTangents.h"
#include "assetProcessor/AnimSource/ChannelSetTableBuilder.h"
#include "assetProcessor/AnimSource/AnimSourceMotionAnalyser.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceMotionAnalyser
//----------------------------------------------------------------------------------------------------------------------
AnimSourceMotionAnalyser::AnimSourceMotionAnalyser() :
  m_inputAnim(NULL),
  m_rig(NULL),
  m_rigToAnimEntryMap(NULL),
  m_messageLogger(NULL),
  m_errorLogger(NULL),
  m_channelSetsLS(NULL),
  m_channelSetsWS(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
AnimSourceMotionAnalyser::~AnimSourceMotionAnalyser()
{
  tidy();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceMotionAnalyser::analyse(
  const AnimSourceUncompressed* inputAnim,
  const MR::AnimRigDef*         rig,
  const MR::RigToAnimEntryMap*  rigToAnimEntryMap,
  NMP::BasicLogger*             messageLogger,
  NMP::BasicLogger*             errorLogger)
{
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(rig);
  NMP_VERIFY(rigToAnimEntryMap);

  //-----------------------
  // Pointers to external data
  m_inputAnim = inputAnim;
  m_rig = rig;
  m_rigToAnimEntryMap = rigToAnimEntryMap;
  m_messageLogger = messageLogger;
  m_errorLogger = errorLogger;

  //-----------------------
  // Clear internally allocated data
  tidy();

  //-----------------------
  // Compute the local and worldspace channel set transforms for the original sampled data
  bool generateDeltas = inputAnim->getGenerateDeltas();
  const ChannelSetTable* channelSets = inputAnim->getChannelSets();
  NMP_VERIFY(channelSets);
  
  m_channelSetsLS = ChannelSetTableBuilderLS::createChannelSetTableLS(
    channelSets,
    m_rig,
    m_rigToAnimEntryMap);

  if (!generateDeltas)
  {
    m_channelSetsWS = ChannelSetTableBuilderWS::createChannelSetTableWS(
      channelSets,
      m_rig,
      m_rigToAnimEntryMap);
      
    // Statistics
    computeAverageMaxBoundsWS();
    computeAverageBoneLength();
    computeAverageInterFrameDisplacementWS();
  }
  else
  {
    // Can't analyse the world joint motion because the input animation is additive
    m_averageMaxBoundsWS = 0.0f;
    m_averageBoneLength = 0.0f;
    m_averageInterFrameDisplacement = 0.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceMotionAnalyser::tidy()
{
  if (m_channelSetsLS)
  {
    NMP::Memory::memFree(m_channelSetsLS);
    m_channelSetsLS = NULL;
  }

  if (m_channelSetsWS)
  {
    NMP::Memory::memFree(m_channelSetsWS);
    m_channelSetsWS = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceMotionAnalyser::computeAverageMaxBoundsWS()
{
  NMP_VERIFY(m_channelSetsWS);
  uint32_t numFrames = m_channelSetsWS->getNumKeyFrames();
  NMP_VERIFY(numFrames > 0);
  uint32_t numRigBones = m_channelSetsWS->getNumChannelSets();
  NMP_VERIFY(numRigBones > 0);
  NMP_VERIFY(m_inputAnim);
  const ChannelSetOverrides* channelSetOverrides = m_inputAnim->getChannelSetOverrides();
  NMP_VERIFY(channelSetOverrides);
  NMP::Vector3 posKey, minPos, maxPos;
  NMP::Vector3 diffPos;
  float maxDelta;

  // Iterate over the animation frames
  m_averageMaxBoundsWS = 0.0f;
  for (uint32_t frame = 0; frame < numFrames; ++frame)
  {
    // Find min and max character bounds
    minPos.set(std::numeric_limits<float>::infinity());
    maxPos.set(-std::numeric_limits<float>::infinity());

    for (uint32_t rigChannelIndex = 1; rigChannelIndex < numRigBones; ++rigChannelIndex)
    {
      // Check for a channel set override. i.e. is it the hips or trajectory bone
      if (channelSetOverrides->isARootBone(rigChannelIndex))
        continue;

      m_channelSetsWS->getPosKey(rigChannelIndex, frame, posKey);
      minPos.x = NMP::minimum(posKey.x, minPos.x);
      minPos.y = NMP::minimum(posKey.y, minPos.y);
      minPos.z = NMP::minimum(posKey.z, minPos.z);
      maxPos.x = NMP::maximum(posKey.x, maxPos.x);
      maxPos.y = NMP::maximum(posKey.y, maxPos.y);
      maxPos.z = NMP::maximum(posKey.z, maxPos.z);
    }

    // Get the maximal character bound delta
    diffPos = maxPos - minPos;
    maxDelta = NMP::maximum(diffPos.x, NMP::maximum(diffPos.y, diffPos.z));

    // Update the average bound delta
    m_averageMaxBoundsWS += maxDelta;
  }

  // Set the average character bound delta
  m_averageMaxBoundsWS /= (float)numFrames;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceMotionAnalyser::computeAverageBoneLength()
{
  NMP_VERIFY(m_rig);
  const NMP::Hierarchy* hierarchy = m_rig->getHierarchy();
  NMP_VERIFY(hierarchy);
  uint32_t numRigBones = hierarchy->getNumEntries();
  NMP_VERIFY(m_channelSetsWS);
  uint32_t numFrames = m_channelSetsWS->getNumKeyFrames();
  NMP_VERIFY(numRigBones == m_channelSetsWS->getNumChannelSets());
  NMP_VERIFY(m_inputAnim);
  const ChannelSetOverrides* channelSetOverrides = m_inputAnim->getChannelSetOverrides();
  NMP_VERIFY(channelSetOverrides);

  NMP::Vector3 posKeyA, posKeyB, diffPos;
  float d, sum;
  uint32_t n;

  // Iterate over the rig bones
  n = 0;
  sum = 0.0f;
  for (uint32_t rigChannelIndex = 1; rigChannelIndex < numRigBones; ++rigChannelIndex)
  {
    // Check for a channel set override. i.e. is it the hips or trajectory bone
    if (channelSetOverrides->isARootBone(rigChannelIndex))
      continue;

    // Get the parent bone of this channel
    uint32_t parentID = hierarchy->getParentIndex(rigChannelIndex);
    NMP_VERIFY(parentID != INVALID_HIERARCHY_INDEX);

    // Iterate over the animation frames
    for (uint32_t frame = 0; frame < numFrames; ++frame)
    {
      m_channelSetsWS->getPosKey(rigChannelIndex, frame, posKeyA);
      m_channelSetsWS->getPosKey(parentID, frame, posKeyB);
      diffPos = posKeyA - posKeyB;
      d = diffPos.magnitude();

      // Update the average sum
      sum += d;
      n++;
    }
  }

  // Set the average bone length
  if (n > 0)
    m_averageBoneLength = sum / n;
  else
    m_averageBoneLength = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceMotionAnalyser::computeAverageInterFrameDisplacementWS()
{
  NMP_VERIFY(m_rig);
  const NMP::Hierarchy* hierarchy = m_rig->getHierarchy();
  NMP_VERIFY(hierarchy);
  uint32_t numRigBones = hierarchy->getNumEntries();
  NMP_VERIFY(m_channelSetsWS);
  uint32_t numFrames = m_channelSetsWS->getNumKeyFrames();
  NMP_VERIFY(numFrames > 1);
  NMP_VERIFY(numRigBones == m_channelSetsWS->getNumChannelSets());
  NMP_VERIFY(m_inputAnim);
  const ChannelSetOverrides* channelSetOverrides = m_inputAnim->getChannelSetOverrides();
  NMP_VERIFY(channelSetOverrides);

  NMP::Vector3 posKeyA, posKeyB, dy;
  float mag, sum;
  uint32_t n;

  // Compute the worldspace displacements of rig bones between frames
  sum = 0.0f;
  n = 0;
  for (uint32_t rigChannelIndex = 1; rigChannelIndex < numRigBones; ++rigChannelIndex)
  {
    // Check for a channel set override. i.e. is it the hips or trajectory bone
    if (channelSetOverrides->isARootBone(rigChannelIndex))
      continue;

    for (uint32_t frame = 0; frame < numFrames - 1; ++frame)
    {
      // Compute the inter-frame displacement
      m_channelSetsWS->getPosKey(rigChannelIndex, frame, posKeyA);
      m_channelSetsWS->getPosKey(rigChannelIndex, frame + 1, posKeyB);
      dy = posKeyB - posKeyA;
      mag = dy.magnitude();

      // Update the average sum
      sum += mag;
      n++;
    }
  }

  // Set the average inter-frame displacement
  if (n > 0)
    m_averageInterFrameDisplacement = sum / n;
  else
    m_averageInterFrameDisplacement = 0.0f;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
