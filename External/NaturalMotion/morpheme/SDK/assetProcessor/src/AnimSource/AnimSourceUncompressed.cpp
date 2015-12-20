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
#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrDefines.h"
#include "assetProcessor/AnimSource/ChannelSetTableBuilder.h"
#include "assetProcessor/AnimSource/AnimSourceUncompressed.h"
//----------------------------------------------------------------------------------------------------------------------

// For debugging the channel set transforms after building
#define DEBUG_TRANSFORMSx

#if defined(DEBUG_TRANSFORMS)
  // Disable warnings about deprecated functions (sprintf, fopen)
  #ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
  #endif
#endif

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceUncompressedOptions
//----------------------------------------------------------------------------------------------------------------------
AnimSourceUncompressedOptions::AnimSourceUncompressedOptions() :
  m_unchangingChannelPosEps(0.0001f),
  m_unchangingChannelQuatEps(0.0001f)
{
}

//----------------------------------------------------------------------------------------------------------------------
AnimSourceUncompressedOptions::~AnimSourceUncompressedOptions()
{
}

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetOverrides
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ChannelSetOverrides::getMemoryRequirements(uint32_t numOverrides)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(ChannelSetOverrides), NMP_NATURAL_TYPE_ALIGNMENT);

  // Rig bone IDs
  NMP::Memory::Format memReqsIDs(sizeof(uint32_t) * numOverrides, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += memReqsIDs;

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
ChannelSetOverrides* ChannelSetOverrides::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numOverrides)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(ChannelSetOverrides), NMP_NATURAL_TYPE_ALIGNMENT);
  ChannelSetOverrides* result = (ChannelSetOverrides*) resource.alignAndIncrement(memReqs);
  NMP_VERIFY(result);
  result->m_numOverrides = numOverrides;

  // Rig bone IDs
  NMP::Memory::Format memReqsIDs(sizeof(uint32_t) * numOverrides, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_rigBoneIDs = (uint32_t*) resource.alignAndIncrement(memReqsIDs);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetOverrides::computeChannelSetOverrides(const MR::AnimRigDef* rig)
{
  // MORPH-21329: Handle multiple character root bone indices.
  NMP_VERIFY(rig);

  bool rigBoneRoots[2];
  uint32_t rigBoneIDs[2];
  uint32_t numRigBoneIDs;

  //-------------------
  // Build up an array of our trajectory bone followed by all the character root bone indices.
  int32_t trajectoryIndex = rig->getTrajectoryBoneIndex();
  if (trajectoryIndex < 1)
    trajectoryIndex = 1;
  int32_t characterRootBoneIndex = rig->getCharacterRootBoneIndex();
  if (characterRootBoneIndex < 1)
    characterRootBoneIndex = 1;

  if (trajectoryIndex == characterRootBoneIndex)
  {
    rigBoneIDs[0] = trajectoryIndex;
    numRigBoneIDs = 1;
  }
  else
  {
    rigBoneIDs[0] = trajectoryIndex;
    rigBoneIDs[1] = characterRootBoneIndex;
    numRigBoneIDs = 2;
  }

  //-------------------
  // Work out which of the bones in this array are root bones i.e. the set of bones that have
  // unique parents. The resulting bones are the ones that we need to store extra trajectory
  // adjustment data for
  const NMP::Hierarchy* rigHierarchy = rig->getHierarchy();
  NMP_VERIFY(rigHierarchy);

  for (uint32_t i = 0; i < numRigBoneIDs; ++i)
  {
    // Initially set rig bone i as a unique parent
    rigBoneRoots[i] = true;
    
    // Check if rig bone i is actually a unique parent
    for (uint32_t k = 0; k < numRigBoneIDs; ++k)
    {
      if (rigBoneIDs[k] != rigBoneIDs[i])
      {
        // Test if rig bone k is a parent (ancestor) of rig bone i
        if (rigHierarchy->isParentOf(rigBoneIDs[k], rigBoneIDs[i]))
        {
          rigBoneRoots[i] = false;
          break;
        }
      }
    }
  }

  //-------------------
  // Store the valid channel overrides
  m_numOverrides = 0;
  for (uint32_t i = 0; i < numRigBoneIDs; ++i)
  {
    if (rigBoneRoots[i])
    {
      m_rigBoneIDs[m_numOverrides] = rigBoneIDs[i];
      ++m_numOverrides;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool ChannelSetOverrides::hasValidTrajectorySourceOverrides(const MR::RigToAnimEntryMap* rigToAnimEntryMap) const
{
  NMP_VERIFY(rigToAnimEntryMap);
  if (m_numOverrides == 0)
    return false;

  // Check if all rig root bones have animation data (needed for trajectory calculation)
  bool calcTrajectory = true;
  for (uint32_t i = 0; i < m_numOverrides; ++i)
  {
    // Check if the rig index to anim channel index map has an entry for the overridden channel
    uint16_t animChannelIndex = 0;
    bool boneInAnim = false;

    if(rigToAnimEntryMap->getNumEntries() > 0)
    {
      boneInAnim = rigToAnimEntryMap->getAnimIndexForRigIndex((uint16_t)m_rigBoneIDs[i], animChannelIndex);
    }

    if (!boneInAnim)
    {
      calcTrajectory = false;
      break;
    }
  }

  return calcTrajectory;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetOverrides::copy(const ChannelSetOverrides& rhs)
{
  NMP_VERIFY(m_numOverrides == rhs.m_numOverrides);
  if (this != &rhs)
  {
    for (uint32_t i = 0; i < m_numOverrides; ++i)
    {
      m_rigBoneIDs[i] = rhs.m_rigBoneIDs[i];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool ChannelSetOverrides::isARootBone(uint32_t rigBoneID) const
{
  for (uint32_t i = 0; i < m_numOverrides; ++i)
  {
    if (m_rigBoneIDs[i] == rigBoneID) return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetRequirements
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format ChannelSetRequirements::getMemoryRequirements(uint32_t numChannelSets)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(ChannelSetRequirements), NMP_NATURAL_TYPE_ALIGNMENT);

  // Channel set requirements
  NMP::Memory::Format memReqsInfo(sizeof(channelSetReqs_t) * numChannelSets, NMP_NATURAL_TYPE_ALIGNMENT);
  memReqs += memReqsInfo;

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
ChannelSetRequirements* ChannelSetRequirements::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numChannelSets)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(ChannelSetRequirements), NMP_NATURAL_TYPE_ALIGNMENT);
  ChannelSetRequirements* result = (ChannelSetRequirements*) resource.alignAndIncrement(memReqs);
  NMP_VERIFY(result);

  result->m_maxNumKeyFrames = 0;
  result->m_numChannelSets = numChannelSets;
  result->m_unchangingPosEps = 0.0001f;
  result->m_unchangingQuatEps = 0.0001f;

  // Channel set requirements
  NMP::Memory::Format memReqsInfo(sizeof(channelSetReqs_t) * numChannelSets, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_channelSetRequirements = (channelSetReqs_t*) resource.alignAndIncrement(memReqsInfo);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetRequirements::computeChannelSetRequirements(
  const ChannelSetTable*     channelSetTable,
  const MR::RigToAnimEntryMap* rigToAnimEntryMap,
  const ChannelSetOverrides* channelSetOverrides,
  float                      posEps,
  float                      quatEps)
{
  NMP_VERIFY(channelSetTable);
  NMP_VERIFY(rigToAnimEntryMap);
  NMP_VERIFY(channelSetOverrides);
  NMP_VERIFY(posEps >= 0.0f);
  NMP_VERIFY(quatEps >= 0.0f);
  uint32_t numChannelSets = channelSetTable->getNumChannelSets();
  NMP_VERIFY(numChannelSets == m_numChannelSets);
  uint32_t numKeyFrames = channelSetTable->getNumKeyFrames();
  NMP_VERIFY(numKeyFrames > 0);

  // Information
  m_maxNumKeyFrames = numKeyFrames;
  m_unchangingPosEps = posEps;
  m_unchangingQuatEps = quatEps;

  // Calculate and store the key frame sample counts of each channel
  for (uint32_t iChan = 0; iChan < numChannelSets; ++iChan)
  {
    // Work out where this anim channel index maps to on the rig.
    uint16_t animChannelIndex = (uint16_t)iChan;
    uint16_t rigBoneIndex;
    rigToAnimEntryMap->getRigIndexForAnimIndex(animChannelIndex, rigBoneIndex);

    // Overridden channels must contain full keyframe data (for trajectory control calculation)
    if (channelSetOverrides->isARootBone(rigBoneIndex))
    {
      // Set the actual keyframe sample counts
      m_channelSetRequirements[iChan].m_numActualQuatSamples = numKeyFrames;
      m_channelSetRequirements[iChan].m_numActualPosSamples = numKeyFrames;
      m_channelSetRequirements[iChan].m_numRequiredQuatSamples = numKeyFrames;
      m_channelSetRequirements[iChan].m_numRequiredPosSamples = numKeyFrames;
    }
    else
    {
      updateChannelSetRequirements(channelSetTable, iChan);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetRequirements::computeChannelSetRequirements(
  const ChannelSetTable*     channelSetTable,
  float                      posEps,
  float                      quatEps)
{
  NMP_VERIFY(channelSetTable);
  NMP_VERIFY(posEps >= 0.0f);
  NMP_VERIFY(quatEps >= 0.0f);
  uint32_t numChannelSets = channelSetTable->getNumChannelSets();
  NMP_VERIFY(numChannelSets == m_numChannelSets);
  uint32_t numKeyFrames = channelSetTable->getNumKeyFrames();
  NMP_VERIFY(numKeyFrames > 0);

  // Information
  m_maxNumKeyFrames = numKeyFrames;
  m_unchangingPosEps = posEps;
  m_unchangingQuatEps = quatEps;

  // Calculate and store the key frame sample counts of each channel
  for (uint32_t iChan = 0; iChan < numChannelSets; ++iChan)
  {
    updateChannelSetRequirements(channelSetTable, iChan);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetRequirements::updateChannelSetRequirements(
  const ChannelSetTable*     channelSetTable,
  uint32_t                   animChannelIndex)
{
  NMP_VERIFY(channelSetTable);
  NMP_VERIFY(animChannelIndex < m_numChannelSets);
  
  uint32_t numKeyFrames = channelSetTable->getNumKeyFrames();
  NMP_VERIFY(numKeyFrames > 0);
  
  // Set the actual keyframe sample counts
  m_channelSetRequirements[animChannelIndex].m_numActualQuatSamples = numKeyFrames;
  m_channelSetRequirements[animChannelIndex].m_numActualPosSamples = numKeyFrames;
  
  // Default required keyframe counts
  m_channelSetRequirements[animChannelIndex].m_numRequiredQuatSamples = numKeyFrames;
  m_channelSetRequirements[animChannelIndex].m_numRequiredPosSamples = numKeyFrames;

  if (channelSetTable->isChannelQuatUnchanging(animChannelIndex, m_unchangingQuatEps))
    m_channelSetRequirements[animChannelIndex].m_numRequiredQuatSamples = 1;

  if (channelSetTable->isChannelPosUnchanging(animChannelIndex, m_unchangingPosEps))
    m_channelSetRequirements[animChannelIndex].m_numRequiredPosSamples = 1;
}


//----------------------------------------------------------------------------------------------------------------------
void ChannelSetRequirements::copy(const ChannelSetRequirements& rhs)
{
  NMP_VERIFY(m_numChannelSets == rhs.m_numChannelSets);
  if (this != &rhs)
  {
    m_maxNumKeyFrames = rhs.m_maxNumKeyFrames;
    m_unchangingQuatEps = rhs.m_unchangingQuatEps;
    m_unchangingPosEps = rhs.m_unchangingPosEps;
    
    for (uint32_t iChan = 0; iChan < m_numChannelSets; ++iChan)
    {
      m_channelSetRequirements[iChan] = rhs.m_channelSetRequirements[iChan];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool ChannelSetRequirements::isChannelQuatUnchanging(uint32_t i) const
{
  NMP_VERIFY(i >= 0 && i < m_numChannelSets);
  const channelSetReqs_t& chanSetReq = m_channelSetRequirements[i];
  return (chanSetReq.m_numRequiredQuatSamples == 1);
}

//----------------------------------------------------------------------------------------------------------------------
bool ChannelSetRequirements::isChannelPosUnchanging(uint32_t i) const
{
  NMP_VERIFY(i >= 0 && i < m_numChannelSets);
  const channelSetReqs_t& chanSetReq = m_channelSetRequirements[i];
  return (chanSetReq.m_numRequiredPosSamples == 1);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t ChannelSetRequirements::getNumUnchangingPosChannels() const
{
  uint32_t n = 0;
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    const channelSetReqs_t& chanSetReq = m_channelSetRequirements[i];
    if (chanSetReq.m_numRequiredPosSamples == 1)
      n++;
  }

  return n;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t ChannelSetRequirements::getNumUnchangingQuatChannels() const
{
  uint32_t n = 0;
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    const channelSetReqs_t& chanSetReq = m_channelSetRequirements[i];
    if (chanSetReq.m_numRequiredQuatSamples == 1)
      n++;
  }

  return n;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t ChannelSetRequirements::getNumChangingPosChannels() const
{
  uint32_t n = 0;
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    const channelSetReqs_t& chanSetReq = m_channelSetRequirements[i];
    if (chanSetReq.m_numRequiredPosSamples > 1)
      n++;
  }

  return n;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t ChannelSetRequirements::getNumChangingQuatChannels() const
{
  uint32_t n = 0;
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    const channelSetReqs_t& chanSetReq = m_channelSetRequirements[i];
    if (chanSetReq.m_numRequiredQuatSamples > 1)
      n++;
  }

  return n;
}

//----------------------------------------------------------------------------------------------------------------------
bool ChannelSetRequirements::isUnchangingPose() const
{
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    const channelSetReqs_t& chanSetReq = m_channelSetRequirements[i];
    if (chanSetReq.m_numRequiredPosSamples > 1 ||
        chanSetReq.m_numRequiredQuatSamples > 1)
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetTableBuilderLS
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTableBuilderLS::computeChannelSetTableLS(
  ChannelSetTable*          result,
  const ChannelSetTable*    localChannelSets, // Animation channel set keyframes
  const MR::AnimRigDef*     rig,
  const MR::RigToAnimEntryMap*  rigToAnimEntryMap)
{
  NMP_VERIFY(result);
  NMP_VERIFY(localChannelSets);
  NMP_VERIFY(rig);
  NMP_VERIFY(rigToAnimEntryMap);
  
  uint32_t numKeyFrames = localChannelSets->getNumKeyFrames();
  NMP_VERIFY(result->getNumKeyFrames() > 0);
  NMP_VERIFY(result->getNumKeyFrames() == localChannelSets->getNumKeyFrames());
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();
  NMP_VERIFY(hierarchy);
  uint32_t numRigBones = hierarchy->getNumEntries();
  NMP_VERIFY(result->getNumChannelSets() == numRigBones);

  for (uint32_t rigBoneID = 0; rigBoneID < numRigBones; ++rigBoneID)
  {
    // Find corresponding anim channel index
    uint16_t animChannelIndex = 0;
    bool boneInAnim = rigToAnimEntryMap->getAnimIndexForRigIndex((uint16_t) rigBoneID, animChannelIndex);
    if (boneInAnim)
    {
      NMP::Vector3 posKey;
      NMP::Quat qutKey;
      for (uint32_t iFrame = 0; iFrame < numKeyFrames; ++iFrame)
      {
        localChannelSets->getPosKey(animChannelIndex, iFrame, posKey);
        localChannelSets->getQuatKey(animChannelIndex, iFrame, qutKey);
        result->setPosKey(rigBoneID, iFrame, posKey);
        result->setQuatKey(rigBoneID, iFrame, qutKey);
      }
    }
    else
    {
      NMP::Vector3 posKey = *rig->getBindPoseBonePos(rigBoneID);
      NMP::Quat qutKey = *rig->getBindPoseBoneQuat(rigBoneID);
      for (uint32_t iFrame = 0; iFrame < numKeyFrames; ++iFrame)
      {
        result->setPosKey(rigBoneID, iFrame, posKey);
        result->setQuatKey(rigBoneID, iFrame, qutKey);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
AP::ChannelSetTable* ChannelSetTableBuilderLS::createChannelSetTableLS(
  const ChannelSetTable*  localChannelSets,
  const MR::AnimRigDef*   rig,
  const MR::RigToAnimEntryMap*  rigToAnimEntryMap)
{
  NMP::Memory::Format memReqs = ChannelSetTable::getMemoryRequirements(
    rig->getNumBones(),
    localChannelSets->getNumKeyFrames());

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  ChannelSetTable* result = ChannelSetTable::init(
    memRes,
    rig->getNumBones(),
    localChannelSets->getNumKeyFrames());
  
  ChannelSetTableBuilderLS::computeChannelSetTableLS(
    result,
    localChannelSets,
    rig,
    rigToAnimEntryMap);
    
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetTableBuilderWS
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetTableBuilderWS::computeChannelSetTableWS(
  ChannelSetTable*          result,
  const ChannelSetTable*    localChannelSets, // Animation channel set keyframes
  const MR::AnimRigDef*     rig,
  const MR::RigToAnimEntryMap*  rigToAnimEntryMap)
{
  NMP_VERIFY(result);
  NMP_VERIFY(localChannelSets);
  NMP_VERIFY(rig);
  NMP_VERIFY(rigToAnimEntryMap);

  // Information
  NMP::Quat* worldSpaceQuatChannel;
  NMP::Vector3* worldSpacePosChannel;
  NMP::Quat* parentWorldSpaceQuatChannel;
  NMP::Vector3* parentWorldSpacePosChannel;
  uint32_t numKeyFrames = localChannelSets->getNumKeyFrames();
  NMP_VERIFY(result->getNumKeyFrames() > 0);
  NMP_VERIFY(result->getNumKeyFrames() == localChannelSets->getNumKeyFrames());
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();
  NMP_VERIFY(hierarchy);
  uint32_t numRigBones = hierarchy->getNumEntries();
  NMP_VERIFY(result->getNumChannelSets() == numRigBones);

  //-------------------
  // Calculate world space channels for the whole hierarchy
  for (uint32_t i = 0; i < numKeyFrames; ++i)
  {
    // Set root bone entry to identity.
    worldSpaceQuatChannel = result->getChannelQuat(0);
    worldSpacePosChannel = result->getChannelPos(0);
    worldSpaceQuatChannel[i].identity();
    worldSpacePosChannel[i].setToZero();

    // Accumulate over the rig hierarchy
    for (uint32_t rigBoneID = 1; rigBoneID < numRigBones; ++rigBoneID)
    {
      NMP::Quat localQuat, parentWorldQuat;
      NMP::Vector3 localPos, parentWorldPos;
      worldSpaceQuatChannel = result->getChannelQuat(rigBoneID);
      worldSpacePosChannel = result->getChannelPos(rigBoneID);

      // Get this bones local transform
      uint16_t animChannelIndex = 0;
      bool boneInAnim = rigToAnimEntryMap->getAnimIndexForRigIndex((uint16_t) rigBoneID, animChannelIndex);
      if (boneInAnim)
      {
        // This anim contains this rig bone so we can safely accumulate with it
        const NMP::Quat* quatKeys = localChannelSets->getChannelQuat(animChannelIndex);
        const NMP::Vector3* posKeys = localChannelSets->getChannelPos(animChannelIndex);
        localQuat = quatKeys[i];
        localPos = posKeys[i];
      }
      else
      {
        // This anim does not contain animation data for this rig bone so accumulate with
        // the bind pose transforms
        const NMP::Quat* rigBoneBindPoseQuat = rig->getBindPoseBoneQuat(rigBoneID);
        const NMP::Vector3* rigBoneBindPosePos = rig->getBindPoseBonePos(rigBoneID);
        localQuat = *rigBoneBindPoseQuat;
        localPos = *rigBoneBindPosePos;
      }

      // Get this bone's parent world transform
      uint32_t rigParentIndex = hierarchy->getParentIndex(rigBoneID);
      NMP_VERIFY(rigParentIndex < rigBoneID && rigParentIndex >= 0);
      parentWorldSpaceQuatChannel = result->getChannelQuat(rigParentIndex);
      parentWorldSpacePosChannel = result->getChannelPos(rigParentIndex);
      parentWorldQuat = parentWorldSpaceQuatChannel[i];
      parentWorldPos = parentWorldSpacePosChannel[i];

      // Accumulate the transforms
      worldSpaceQuatChannel[i] = parentWorldQuat * localQuat;
      worldSpacePosChannel[i] = parentWorldQuat.rotateVector(localPos);
      worldSpacePosChannel[i] += parentWorldPos;
      // Initialise the w component to ensure binary invariance
      worldSpacePosChannel[i].w = 0.0f;
    }
  }

  //-------------------
  // Condition consecutive quaternions to lie in the same hemisphere + normalise
  result->conditionChannelQuat();
}

//----------------------------------------------------------------------------------------------------------------------
ChannelSetTable* ChannelSetTableBuilderWS::createChannelSetTableWS(
  const ChannelSetTable*        localChannelSets,     // IN: The local space animation channel set table
  const MR::AnimRigDef*         rig,                  // IN: The animation rig
  const MR::RigToAnimEntryMap*  rigToAnimEntryMap)    // IN: Rig channel index to animation channel index map
{
  NMP::Memory::Format memReqs = ChannelSetTable::getMemoryRequirements(
    rig->getNumBones(),
    localChannelSets->getNumKeyFrames());

  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);

  ChannelSetTable* result = ChannelSetTable::init(
    memRes,
    rig->getNumBones(),
    localChannelSets->getNumKeyFrames());

  ChannelSetTableBuilderWS::computeChannelSetTableWS(
    result,
    localChannelSets,
    rig,
    rigToAnimEntryMap);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceUncompressed
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TrajectorySourceUncompressed::getMemoryRequirements(uint32_t numKeyframes)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(TrajectorySourceUncompressed), NMP_NATURAL_TYPE_ALIGNMENT);

  // Channel set requirements
  NMP::Memory::Format memReqsChanSetReqs = ChannelSetRequirements::getMemoryRequirements(1);
  memReqs += memReqsChanSetReqs;

  // Channel set table
  NMP::Memory::Format memReqsChanSetTable = AP::ChannelSetTable::getMemoryRequirements(1, numKeyframes);
  memReqs += memReqsChanSetTable;

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
TrajectorySourceUncompressed* TrajectorySourceUncompressed::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numKeyframes)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(TrajectorySourceUncompressed), NMP_NATURAL_TYPE_ALIGNMENT);
  TrajectorySourceUncompressed* result = (TrajectorySourceUncompressed*) resource.alignAndIncrement(memReqs);
  NMP_VERIFY(result);
  result->m_duration = 0.0f;
  result->m_sampleFrequency = 0.0f;

  // Channel set requirements
  result->m_deltaTrajectoryChannelSetReqs = ChannelSetRequirements::init(resource, 1);
  
  // Channel set table
  result->m_deltaTrajectoryChannelSet = ChannelSetTable::init(resource, 1, numKeyframes);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceUncompressed::computeTrajectorySource(
  const ChannelSetOverrides*  channelSetOverrides,
  ChannelSetRequirements*     channelSetRequirements, // Overriden channels are modified
  ChannelSetTable*            channelSets, // Overriden channels are modified
  const MR::AnimRigDef*       rig,
  const MR::RigToAnimEntryMap*  rigToAnimEntryMap,
  float                       animSampleRate)
{
  NMP_VERIFY(channelSetOverrides);
  NMP_VERIFY(channelSetRequirements);
  NMP_VERIFY(channelSets);
  NMP_VERIFY(rig);
  NMP_VERIFY(rigToAnimEntryMap);
  NMP_VERIFY(animSampleRate > 0);

  // Information
  uint32_t numKeyFrames = channelSets->getNumKeyFrames();
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();
  NMP_VERIFY(hierarchy);
  uint32_t numRigBones = hierarchy->getNumEntries();
  const NMP::Quat* blendFrameOrientation = rig->getBlendFrameOrientation();
  NMP_VERIFY(blendFrameOrientation);
  const NMP::Vector3* blendFrameTranslation = rig->getBlendFrameTranslation();
  NMP_VERIFY(blendFrameTranslation);
  int32_t rigTrajectoryBoneIndex = rig->getTrajectoryBoneIndex();
  NMP_VERIFY(rigTrajectoryBoneIndex >= 0);
  float duration = (numKeyFrames - 1) / animSampleRate; // secs

  //-------------------
  // Header
  m_duration = duration;
  m_sampleFrequency = animSampleRate;

  //-------------------
  // Allocate temporary memory for the worldspace rig bone channel set keyframes
  NMP::Memory::Format channelSetWSMemReqs = AP::ChannelSetTable::getMemoryRequirements(numRigBones, numKeyFrames);
  NMP::Memory::Resource channelSetWSRes = NMPMemoryAllocateFromFormat(channelSetWSMemReqs);
  ChannelSetTable* channelSetWS = AP::ChannelSetTable::init(channelSetWSRes, numRigBones, numKeyFrames);

  // Compute the worldspace rig bone channel sets
  ChannelSetTableBuilderWS::computeChannelSetTableWS(
    channelSetWS,
    channelSets,
    rig,
    rigToAnimEntryMap);

  // Store convenience pointers to worldspace trajectory bone channel
  NMP::Quat* worldTrajQuatChan = channelSetWS->getChannelQuat(rigTrajectoryBoneIndex);
  NMP::Vector3* worldTrajPosChan = channelSetWS->getChannelPos(rigTrajectoryBoneIndex);

  // Get the delta trajectory quat and pos keyframe arrays
  NMP::Quat* deltaTrajectoryChannelQuat = m_deltaTrajectoryChannelSet->getChannelQuat(0);
  NMP::Vector3* deltaTrajectoryChannelPos = m_deltaTrajectoryChannelSet->getChannelPos(0);

  //-------------------
  // Compute the new Delta Trajectory World Transform channel which is the offset of the Trajectory
  // from its first frame transform. In the canonical space of our animations the y axis is upward
  // and the trajectory typically moves within the XZ plane. If the user requires a different axis
  // for the up vector with the trajectory moving within a different plane then a further blend frame
  // transform is applied.
  NMP::Quat invTrajWorldQuat1stFrame = worldTrajQuatChan[0];
  invTrajWorldQuat1stFrame.conjugate();
  NMP::Quat invBlendFrameOrientation = *blendFrameOrientation;
  invBlendFrameOrientation.conjugate();

  for (uint32_t i = 0; i < numKeyFrames; ++i)
  {
    // Get the worldspace transform (canonical space) of the trajectory at the current keyframe.
    // Tw_i = [R_i | t_i]
    NMP::Quat trajQuat = worldTrajQuatChan[i];
    NMP::Vector3 trajPos = worldTrajPosChan[i];

    // Put into the canonical space of the first worldspace trajectory keyframe.
    // Dw_i = Tw_0^{-1} * Tw_i    i.e.  Dw_i = [R_0^{-1} | -R_0^{-1} * t_0] * [R_i | t_i]
    // Dw_i = [R_0^{-1} * R_i | R_0^{-1} * (t_i - t_0)]
    deltaTrajectoryChannelQuat[i] = invTrajWorldQuat1stFrame * trajQuat;
    deltaTrajectoryChannelQuat[i].normalise();
    deltaTrajectoryChannelPos[i] = invTrajWorldQuat1stFrame.rotateVector(trajPos - worldTrajPosChan[0]);

    // Apply the Blend frame transform Dw'_i = B * Dw_i * B^{-1}. This can be seen as follows:
    // Apply the rotation that maps the coordinate frame of the user's space back to the canonical axes.
    // This is the inverse transform B^{-1} = [Rb^{-1} | -Rb^{-1} * tb]. We then apply our canonical space
    // delta trajectory transform Dw_i. Finally, we apply the transform that maps our canonical space back to
    // the user's coordinate frame B = [Rb | tb]. Combining all three transforms we get:
    // Dw'_i = [Rb | tb] * [dR_i | dt_i] * [Rb^{-1} | -Rb^{-1} * tb]
    NMP::Quat qb = (*blendFrameOrientation) * deltaTrajectoryChannelQuat[i] * invBlendFrameOrientation;
    qb.normalise();
    deltaTrajectoryChannelQuat[i] = qb;
    NMP::Vector3 tb = (*blendFrameTranslation) - qb.rotateVector(*blendFrameTranslation);
    deltaTrajectoryChannelPos[i] = (*blendFrameOrientation).rotateVector(deltaTrajectoryChannelPos[i]) + tb;
  }

  //-------------------
  // Calculate the overridden root bone channels
  const uint32_t* overrides = channelSetOverrides->getChannelSetOverrides();
  uint32_t numOverrides = channelSetOverrides->getNumChannelSetOverrides();
  NMP_VERIFY(overrides);
  NMP_VERIFY(numOverrides > 0);

  for (uint32_t k = 0; k < numOverrides; ++k)
  {
    uint32_t rigBoneIndex = overrides[k];
    uint16_t animChannelIndex;

    // Work out the anim bone index of the rig bone we are interested in.
    // We assume that if these bones exist then so do their parent hierarchies up to the root
#if defined(NMP_ENABLE_ASSERTS) || NM_ENABLE_EXCEPTIONS
    bool boneInAnim = 
#endif
        rigToAnimEntryMap->getAnimIndexForRigIndex((uint16_t)rigBoneIndex, animChannelIndex);
    NMP_VERIFY(boneInAnim);

    // Get the overridden channel quat and pos keyframe arrays
    NMP::Quat* overriddenChannelQuat = channelSets->getChannelQuat(animChannelIndex);
    NMP::Vector3* overriddenChannelPos = channelSets->getChannelPos(animChannelIndex);

    // Make sure the overridden root bone has a parent bone in the hierarchy
    int32_t rigParentIndex = hierarchy->getParentIndex(rigBoneIndex);
    NMP_VERIFY(rigParentIndex != -1);

    uint16_t parentAnimChannnelIndx;
    bool parentBoneInAnim = rigToAnimEntryMap->getAnimIndexForRigIndex((uint16_t)rigParentIndex, parentAnimChannnelIndx);

    // Store convenience pointers to the rig bone channels we are interested in
    NMP::Quat* worldOverriddenQuatChan = channelSetWS->getChannelQuat(rigBoneIndex);
    NMP::Vector3* worldOverriddenPosChan = channelSetWS->getChannelPos(rigBoneIndex);

    // Compute overridden transforms over all keyframe samples
    for (uint32_t i = 0; i < numKeyFrames; ++i)
    {
      // Calculate delta transform from the trajectory to the overridden bone. i.e. trajectory to hips
      NMP::Quat invTrajQuat = worldTrajQuatChan[i]; // This frames trajectory quat
      NMP::Vector3 invTrajPos = worldTrajPosChan[i]; // This frames trajectory pos
      invTrajQuat.conjugate();
      invTrajPos *= -1.0f;

      // Apply this translation and rotation to the Hips to get the relative offset of Hips from trajectory.
      NMP::Quat hipsQuat = worldOverriddenQuatChan[i]; // This frames hips quat
      NMP::Vector3 hipsPos = worldOverriddenPosChan[i]; // This frames hips pos
      hipsQuat = invTrajQuat * hipsQuat;
      hipsPos = hipsPos + invTrajPos;
      hipsPos = invTrajQuat.rotateVector(hipsPos);

      // Multiply this by the Blend Frame transform to get a new hips delta transform
      hipsQuat = (*blendFrameOrientation) * hipsQuat;
      hipsQuat.normalise();
      hipsPos = blendFrameOrientation->rotateVector(hipsPos) + (*blendFrameTranslation);

      // Since we are overwriting the old hips channel with the new hips delta transform (from trajectory)
      // we must take into account any parent transforms in the hierarchy above the hip channel.
      // i.e. if H_d is the new hips delta transform, H_0 ... H_i are the tranforms in the hierarchy above
      // the hips channel, H_c is the adjusted delta transform we are writing back to the buffer then the
      // accumulated hips delta transform can be written H_d = (H_0 * ... * H_i) * H_c
      // Thus H_c = (H_0 * ... * H_i)^-1 * H_d
      NMP::Quat invHipsParentQuat;
      NMP::Vector3 invHipsParentPos;
      if (parentBoneInAnim)
      {
        NMP::Quat* worldHipsParentQuatChan = channelSetWS->getChannelQuat(rigParentIndex);
        NMP::Vector3* worldHipsParentPosChan = channelSetWS->getChannelPos(rigParentIndex);
        invHipsParentQuat = worldHipsParentQuatChan[i]; // This frames hips parent quat
        invHipsParentPos = worldHipsParentPosChan[i]; // This frames hips parent pos
        invHipsParentQuat.conjugate();
        invHipsParentPos *= -1.0f;
      }
      else
      {
        invHipsParentQuat.identity();
        invHipsParentPos.setToZero();
      }

      NMP::Quat newHipsQuat = invHipsParentQuat * hipsQuat;
      NMP::Vector3 newHipsPos = hipsPos + invHipsParentPos;
      newHipsPos = invHipsParentQuat.rotateVector(newHipsPos);
      // Initialise the w component to ensure binary invariance
      newHipsPos.w = 0.0f;

      // Set the overridden channel data
      overriddenChannelQuat[i] = newHipsQuat;
      overriddenChannelPos[i] = newHipsPos;
    }

    //-------------------
    // Recompute the channel set requirements for the overridden channel. NOTE: if the overridden channel
    // is the trajectory channel we can't just assume that the overridden buffer will be unchanging
    // because the parent bones of the trajectory channel may be animated
    channelSetRequirements->updateChannelSetRequirements(channelSets, animChannelIndex);
  }

  //-------------------
  // Compute the channel set requirements for the delta trajectory channel set
  NMP_VERIFY(m_deltaTrajectoryChannelSetReqs);
  m_deltaTrajectoryChannelSetReqs->computeChannelSetRequirements(
    m_deltaTrajectoryChannelSet,
    channelSetRequirements->getUnchangingPosChannelEps(),
    channelSetRequirements->getUnchangingQuatChannelEps());

  //-------------------
  // Tidy up the temporary workspace memory
  NMP::Memory::memFree(channelSetWS);
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceUncompressed::computeTrajectorySource(const TrajectorySourceUncompressed* trajectorySource)
{
  NMP_VERIFY(trajectorySource);

  // Recover the resample rate
  float duration = trajectorySource->getDuration();
  NMP_VERIFY(duration > 0.0f);
  uint32_t numKeyFrames = m_deltaTrajectoryChannelSet->getNumKeyFrames();
  NMP_VERIFY(numKeyFrames > 0);
  float resampleFrequency = (numKeyFrames - 1) / duration;

  //-------------------
  // Header
  m_duration = duration;
  m_sampleFrequency = resampleFrequency;

  //-------------------
  // Resample the trajectory source transforms
  const ChannelSetTable* srcChannelSet = trajectorySource->getDeltaTrajectroyChannelSet();
  NMP_VERIFY(srcChannelSet);
  NMP_VERIFY(m_deltaTrajectoryChannelSet);
  m_deltaTrajectoryChannelSet->resampleChannelSets(*srcChannelSet);

  //-------------------
  // Compute the channel set requirements
  const ChannelSetRequirements* chanSetReqs = trajectorySource->getDeltaTrajectroyChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);  
  NMP_VERIFY(m_deltaTrajectoryChannelSetReqs);
  m_deltaTrajectoryChannelSetReqs->computeChannelSetRequirements(
    m_deltaTrajectoryChannelSet,
    chanSetReqs->getUnchangingPosChannelEps(),
    chanSetReqs->getUnchangingQuatChannelEps());
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceUncompressed Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimSourceUncompressed::getMemoryRequirements(
  uint32_t                numOverrides,
  uint32_t                numAnimChannelSets,
  uint32_t                numKeyframes,
  const NMP::OrderedStringTable* channelNames,
  bool                    calcTrajectory)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(AnimSourceUncompressed), NMP_NATURAL_TYPE_ALIGNMENT);

  // Overrides
  NMP::Memory::Format memReqsOverrides = ChannelSetOverrides::getMemoryRequirements(numOverrides);
  memReqs += memReqsOverrides;

  // Channel set requirements
  NMP::Memory::Format memReqsChanSetReq = ChannelSetRequirements::getMemoryRequirements(numAnimChannelSets);
  memReqs += memReqsChanSetReq;

  // Channel set table
  NMP::Memory::Format memReqsChanSetTable = AP::ChannelSetTable::getMemoryRequirements(numAnimChannelSets, numKeyframes);
  memReqs += memReqsChanSetTable;

  // Trajectory control
  if (calcTrajectory)
  {
    NMP::Memory::Format memReqsTrajectory = TrajectorySourceUncompressed::getMemoryRequirements(numKeyframes);
    memReqs += memReqsTrajectory;
  }

  // Channel names
  NMP::Memory::Format memReqsChanNames = channelNames->getInstanceMemoryRequirements();
  memReqs += memReqsChanNames;

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
AnimSourceUncompressed* AnimSourceUncompressed::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numOverrides,
  uint32_t                numAnimChannelSets,
  uint32_t                numKeyframes,
  const NMP::OrderedStringTable* channelNames,
  bool                    calcTrajectory)
{
  // Header
  NMP::Memory::Format memReqs(sizeof(AnimSourceUncompressed), NMP_NATURAL_TYPE_ALIGNMENT);
  AnimSourceUncompressed* result = (AnimSourceUncompressed*) resource.alignAndIncrement(memReqs);
  NMP_VERIFY(result);
  result->m_duration = 0.0f;
  result->m_sampleFrequency = 0.0f;
  result->m_generateDeltas = false;

  // Overrides
  result->m_channelSetOverrides = ChannelSetOverrides::init(resource, numOverrides);

  // Channel set requirements
  result->m_channelSetRequirements = ChannelSetRequirements::init(resource, numAnimChannelSets);

  // Channel set table
  result->m_channelSets = ChannelSetTable::init(resource, numAnimChannelSets, numKeyframes);

  // Trajectory control
  if (calcTrajectory)
  {
    result->m_trajectoryData = TrajectorySourceUncompressed::init(resource, numKeyframes);
  }
  else
  {
    result->m_trajectoryData = NULL; // No trajectory source
  }

  // Channel names 
  result->m_channelNames = NMP::OrderedStringTable::init(
    resource,
    channelNames->getNumEntries(),
    channelNames->getOffsets(),
    channelNames->getData(),
    channelNames->getDataLength());

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceUncompressedBuilder Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimSourceUncompressedBuilder::getMemoryRequirements(
  const MR::AnimRigDef*   rig,
  const MR::RigToAnimEntryMap* rigToAnimEntryMap,
  const ChannelSetTable*  animChannelSets,
  const NMP::OrderedStringTable* channelNames)
{
  NMP_VERIFY(rig);
  NMP_VERIFY(rigToAnimEntryMap);
  NMP_VERIFY(animChannelSets);
  NMP_VERIFY(channelNames);

  // Information
  uint32_t numAnimChannelSets = animChannelSets->getNumChannelSets();
  uint32_t numAnimKeyFrames = animChannelSets->getNumKeyFrames();
  NMP_VERIFY(numAnimKeyFrames >= 2);

  //-------------------
  // Calculate the number of channel set overrides
  NMP::Memory::Format memReqs = ChannelSetOverrides::getMemoryRequirements(2);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ChannelSetOverrides* chanSetOverrides = ChannelSetOverrides::init(memRes, 2);
  chanSetOverrides->computeChannelSetOverrides(rig);
  uint32_t numOverrides = chanSetOverrides->getNumChannelSetOverrides();

  // Check if all rig root bones have animation data (needed for trajectory calculation)
  bool calcTrajectory = chanSetOverrides->hasValidTrajectorySourceOverrides(rigToAnimEntryMap);

  //-------------------
  // Compute the memory requirements for the animation source
  NMP::Memory::Format result = AnimSourceUncompressed::getMemoryRequirements(
    numOverrides,
    numAnimChannelSets,
    numAnimKeyFrames,
    channelNames,
    calcTrajectory);

  //-------------------
  // Tidy up
  NMP::Memory::memFree(chanSetOverrides);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnimSourceUncompressed* AnimSourceUncompressedBuilder::init(
  NMP::Memory::Resource&  resource,
  const MR::AnimRigDef*   rig,
  const MR::RigToAnimEntryMap* rigToAnimEntryMap,
  const ChannelSetTable*  animChannelSets,
  const NMP::OrderedStringTable* channelNames,
  float                   animSampleRate,
  const AnimSourceUncompressedOptions& animSourceOptions)
{
  NMP_VERIFY(rig);
  NMP_VERIFY(rigToAnimEntryMap);
  NMP_VERIFY(animChannelSets);
  NMP_VERIFY(channelNames);
  NMP_VERIFY(animSampleRate > 0.0f);

  // Information
  uint32_t numAnimChannelSets = animChannelSets->getNumChannelSets();
  uint32_t numAnimKeyFrames = animChannelSets->getNumKeyFrames();
  NMP_VERIFY(numAnimKeyFrames > 0);
  float animDuration = (numAnimKeyFrames - 1) / animSampleRate; // secs

  //-------------------
  // Calculate the number of channel set overrides
  NMP::Memory::Format memReqs = ChannelSetOverrides::getMemoryRequirements(2);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  ChannelSetOverrides* chanSetOverrides = ChannelSetOverrides::init(memRes, 2);
  chanSetOverrides->computeChannelSetOverrides(rig);
  uint32_t numOverrides = chanSetOverrides->getNumChannelSetOverrides();

  // Check if all rig root bones have animation data (needed for trajectory calculation)
  bool calcTrajectory = chanSetOverrides->hasValidTrajectorySourceOverrides(rigToAnimEntryMap);

  //---------------------
  // Initialise the animation source
  AnimSourceUncompressedBuilder* result = (AnimSourceUncompressedBuilder*)AnimSourceUncompressed::init(
    resource,
    numOverrides,
    numAnimChannelSets,
    numAnimKeyFrames,
    channelNames,
    calcTrajectory);

  // Header
  result->m_duration = animDuration;
  result->m_sampleFrequency = animSampleRate;

  //---------------------
  // Overrides
  NMP_VERIFY(result->m_channelSetOverrides);
  result->m_channelSetOverrides->copy(*chanSetOverrides);

  //---------------------
  // Channel set keyframe data
  NMP_VERIFY(result->m_channelSets);
  result->m_channelSets->copy(*animChannelSets);

  //---------------------
  // Channel set requirements
  NMP_VERIFY(result->m_channelSetRequirements);
  result->m_channelSetRequirements->computeChannelSetRequirements(
    result->m_channelSets,
    rigToAnimEntryMap,
    result->m_channelSetOverrides,
    animSourceOptions.getUnchangingChannelPosEps(),
    animSourceOptions.getUnchangingChannelQuatEps());

  //---------------------
  // Trajectory control
  if (calcTrajectory)
  {
    NMP_VERIFY(result->m_trajectoryData);
    result->m_trajectoryData->computeTrajectorySource(
      result->m_channelSetOverrides,
      result->m_channelSetRequirements,
      result->m_channelSets,
      rig,
      rigToAnimEntryMap,
      animSampleRate);
  }

  //---------------------
  // Channel names
  NMP_VERIFY(result->m_channelNames);
  NMP::Memory::Format memReqsChanNames = channelNames->getInstanceMemoryRequirements();
  NMP::Memory::memcpy(result->m_channelNames, channelNames, memReqsChanNames.size);
  result->m_channelNames->relocate();

  //-------------------
  // Tidy up
  NMP::Memory::memFree(chanSetOverrides);

  //---------------------
  // DEBUG
#ifdef NM_DEBUG
  #ifdef DEBUG_TRANSFORMS
  FILE* filePointer = fopen("C:/uncompressed_anim.dat", "w");
  NMP_VERIFY(filePointer);
  if (filePointer)
  {
    //-----------------------
    // Rig channels
    fprintf(filePointer, "\n_______________ Rig Channels _______________\n");
    const NMP::Hierarchy* hierarchy = rig->getHierarchy();
    uint32_t numBones = hierarchy->getNumEntries();
    for (uint32_t i = 0; i < numBones; ++i)
    {
      const char* name = rig->getBoneName(i);
      int32_t parentID = hierarchy->getParentIndex(i);
      fprintf(filePointer, "chan = %4d, parent chan = %4d, name = %s\n", i, parentID, name);
    }

    //-----------------------
    // Rig bind pose transforms
    const MR::AttribDataTransformBuffer* bindPose = rig->getBindPose();
    if (bindPose)
    {
      fprintf(filePointer, "\n_______________ Rig Bind Pose _______________\n");
      NMP_VERIFY(bindPose);
      NMP::Vector3* bindPosePos = bindPose->m_transformBuffer->getPosQuatChannelPos(0);
      NMP_VERIFY(bindPosePos);
      NMP::Quat* bindPoseQuat = bindPose->m_transformBuffer->getPosQuatChannelQuat(0);
      NMP_VERIFY(bindPoseQuat);

      for (uint32_t i = 0; i < numBones; ++i)
      {
        fprintf(filePointer,
          "chan = %4d, quat = [%f, %f, %f, %f], pos = [%f, %f, %f]\n",
          i,
          bindPoseQuat[i].x, bindPoseQuat[i].y, bindPoseQuat[i].z, bindPoseQuat[i].w,
          bindPosePos[i].x, bindPosePos[i].y, bindPosePos[i].z);
      }    
    }

    //-----------------------
    // Rig to animation map
    fprintf(filePointer, "\n_______________ Rig To Anim Map _______________\n");
    uint32_t numEntries = rigToAnimEntryMap->getNumEntries();
    for (uint32_t i = 0; i < numEntries; ++i)
    {
      uint16_t rigIndex = rigToAnimEntryMap->getEntryRigChannelIndex(i);
      uint16_t animIndex = rigToAnimEntryMap->getEntryAnimChannelIndex(i);
      fprintf(filePointer, "rig chan index = %4d, anim chan index = %4d\n", rigIndex, animIndex);
    }

    fprintf(filePointer, "\n_______________ Channel Set Requirements _______________\n");
    uint32_t numChannelSets = result->m_channelSetRequirements->getNumChannelSets();
    for (uint32_t i = 0; i < numChannelSets; ++i)
    {
      bool posUnchanging = result->m_channelSetRequirements->isChannelPosUnchanging(i);
      bool quatUnchanging = result->m_channelSetRequirements->isChannelQuatUnchanging(i);
      fprintf(filePointer, "anim chan index = %4d, posUnchanging = %d, quatUnchanging = %d\n", i, posUnchanging, quatUnchanging);
    }

    //-----------------------
    // Channel set table
    fprintf(filePointer, "\n_______________ Anim channel sets _______________\n");
    //result->m_channelSets->writeTableByFrames(filePointer);
    result->m_channelSets->writeTableByChannelSets(filePointer);
    fclose(filePointer);
  }
  #endif // DEBUG_TRANSFORMS
#endif // NM_DEBUG

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Converts the animation channel set data into a set of additive delta components
// relative to the first animation keyframe. Note that the (quat, pos) components are treated
// independently so that at runtime the channels can be additively blended:
// q(u) = qSlerp(I, q1, u) * q0
// t(u) = Lerp(0, t1, u) + t0
// where u is the interpolant, (q0, t0) are the base components and (q1, t1) are the delta components.
void AnimSourceUncompressedBuilder::convertToDeltaTransforms(ChannelSetTable* animChannelSets)
{
  NMP_VERIFY(animChannelSets);

  uint32_t numChannels = animChannelSets->getNumChannelSets();
  if (numChannels && animChannelSets->getNumKeyFrames() > 0)
  {
    // Allocate space for working first frame inverse transforms.
    NMP::Memory::Format memReqs(sizeof(NMP::Quat) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Quat*    inverseFirstFrameQuats = (NMP::Quat*) NMPMemoryAlloc(sizeof(NMP::Quat) * numChannels);
    NMP_ASSERT(inverseFirstFrameQuats);
    NMP::Vector3* inverseFirstFramePosits = (NMP::Vector3*) NMPMemoryAlloc(sizeof(NMP::Vector3) * numChannels);
    NMP_ASSERT(inverseFirstFramePosits);

    // Calculate inverse first frame components.
    for (uint32_t i = 0; i < numChannels; ++i)
    {
      animChannelSets->getQuatKey(i, 0, inverseFirstFrameQuats[i]);
      inverseFirstFrameQuats[i].conjugate();
      animChannelSets->getPosKey(i, 0, inverseFirstFramePosits[i]);
      inverseFirstFramePosits[i] *= -1.0f;
    }

    // Apply the first frame inverse components to all frames transforms in order to derive the delta transforms.
    for (uint32_t i = 0; i < animChannelSets->getNumKeyFrames(); ++i)
    {
      for (uint32_t k = 0; k < numChannels; ++k)
      {
        NMP::Quat currentQuat;
        animChannelSets->getQuatKey(k, i, currentQuat);
        NMP::Quat deltaQuat = currentQuat * inverseFirstFrameQuats[k];
        animChannelSets->setQuatKey(k, i, deltaQuat);

        NMP::Vector3 currentPos;
        animChannelSets->getPosKey(k, i, currentPos);
        NMP::Vector3 deltaPos = currentPos + inverseFirstFramePosits[k];
        animChannelSets->setPosKey(k, i, deltaPos);
      }
    }

    // Free working memory.
    NMP::Memory::memFree(inverseFirstFrameQuats);
    NMP::Memory::memFree(inverseFirstFramePosits);

    // Condition the quat channels
    animChannelSets->conditionChannelQuat();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimSourceUncompressedBuilder::getMemoryRequirements(
  const AnimSourceUncompressed* anim,
  float desiredAnimResampleRate)
{
  NMP_VERIFY(anim);

  //---------------------
  // Compute the number of resampled animation frames
  NMP_VERIFY(desiredAnimResampleRate > 0.0f);
  NMP_VERIFY(anim->getSampleFrequency() > 0.0f);
  NMP_VERIFY(anim->getMaxNumKeyframes() > 0);

  // The duration of the anim: T = (Nc - 1) / Rc
  // where Nc is the number of frames, Rc is the current sample rate
  uint32_t curKeyFrameEnd = anim->getMaxNumKeyframes() - 1;

  // Similarly the duration of the resampled anim: T = (Nr - 1) / Rr
  // where Nr is the number of resampled frames, Rr is the actual resample rate
  float resampleRatio = desiredAnimResampleRate / anim->getSampleFrequency();
  uint32_t resampledKeyFrameEnd = (uint32_t) (resampleRatio * curKeyFrameEnd + 0.5f);
  uint32_t numResampledKeyFrames = resampledKeyFrameEnd + 1;

  // Runtime animations require at least two frames
  numResampledKeyFrames = NMP::maximum(numResampledKeyFrames, static_cast<uint32_t>(2));

  //-------------------
  // Compute the memory requirements for the animation source
  NMP::Memory::Format result = AnimSourceUncompressed::getMemoryRequirements(
    anim->getNumChannelSetOverrides(),
    anim->getNumChannelSets(),
    numResampledKeyFrames,
    anim->getChannelNames(),
    anim->hasTrajectoryData());

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnimSourceUncompressed* AnimSourceUncompressedBuilder::init(
  NMP::Memory::Resource&        resource,
  const AnimSourceUncompressed* anim,
  float                         desiredAnimResampleRate,
  bool                          generateDeltas)
{
  NMP_VERIFY(anim);

  //---------------------
  // Compute the number of resampled animation frames
  NMP_VERIFY(desiredAnimResampleRate > 0.0f);
  NMP_VERIFY(anim->getSampleFrequency() > 0.0f);
  NMP_VERIFY(anim->getMaxNumKeyframes() > 0);

  // The duration of the anim: T = (Nc - 1) / Rc
  // where Nc is the number of frames, Rc is the current sample rate
  uint32_t curKeyFrameEnd = anim->getMaxNumKeyframes() - 1;
  float duration = curKeyFrameEnd / anim->getSampleFrequency();

  // Similarly the duration of the resampled anim: T = (Nr - 1) / Rr
  // where Nr is the number of resampled frames, Rr is the actual resample rate
  float resampleRatio = desiredAnimResampleRate / anim->getSampleFrequency();
  uint32_t resampledKeyFrameEnd = (uint32_t) (resampleRatio * curKeyFrameEnd + 0.5f);
  uint32_t numResampledKeyFrames = resampledKeyFrameEnd + 1;

  // Runtime animations require at least two frames
  numResampledKeyFrames = NMP::maximum(numResampledKeyFrames, static_cast<uint32_t>(2));

  // Recover the resample rate
  float resampleFrequency = (numResampledKeyFrames - 1) / duration;

  //---------------------
  // Initialise the animation source memory 
  AnimSourceUncompressedBuilder* result =
    (AnimSourceUncompressedBuilder*) AnimSourceUncompressed::init(
      resource,
      anim->getNumChannelSetOverrides(),
      anim->getNumChannelSets(),
      numResampledKeyFrames,
      anim->getChannelNames(),
      anim->hasTrajectoryData());

  //---------------------
  // Header
  result->m_duration = duration;
  result->m_sampleFrequency = resampleFrequency;

  //---------------------
  // Overrides
  const ChannelSetOverrides* channelSetOverrides = anim->getChannelSetOverrides();
  NMP_VERIFY(channelSetOverrides);
  NMP_VERIFY(result->m_channelSetOverrides);
  result->m_channelSetOverrides->copy(*channelSetOverrides);

  //---------------------
  // Resampled channel set keyframe data
  const ChannelSetTable* animChannelSets = anim->getChannelSets();
  NMP_VERIFY(animChannelSets);
  NMP_VERIFY(result->m_channelSets);
  result->m_channelSets->resampleChannelSets(*animChannelSets);

  //---------------------
  // Channel set requirements
  const ChannelSetRequirements* chanSetReqs = anim->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);

  NMP_VERIFY(result->m_channelSetRequirements);
  result->m_channelSetRequirements->computeChannelSetRequirements(
    result->m_channelSets,
    chanSetReqs->getUnchangingPosChannelEps(),
    chanSetReqs->getUnchangingQuatChannelEps());   

  //---------------------
  // Trajectory control
  if (anim->hasTrajectoryData())
  {  
    NMP_VERIFY(result->m_trajectoryData);
    result->m_trajectoryData->computeTrajectorySource(anim->getTrajectorySource());
  }

  //---------------------
  // Store this animation as delta transform components from the first frame
  if (generateDeltas)
  {
    // The channel set requirements should remain the same after conversion
    convertToDeltaTransforms(result->m_channelSets);
    result->m_generateDeltas = true;
  }

  //---------------------
  // Channel names
  const NMP::OrderedStringTable* channelNames = anim->getChannelNames();
  NMP_VERIFY(channelNames);
  NMP_VERIFY(result->m_channelNames);
  NMP::Memory::Format memReqsChanNames = channelNames->getInstanceMemoryRequirements();
  NMP::Memory::memcpy(result->m_channelNames, channelNames, memReqsChanNames.size);
  result->m_channelNames->relocate();

  //---------------------
  // DEBUG
#ifdef NM_DEBUG
  #ifdef DEBUG_TRANSFORMS
  FILE* filePointer = fopen("C:/resampled_anim.dat", "w");
  NMP_VERIFY(filePointer);
  if (filePointer)
  {
    // Channel set table
    fprintf(filePointer, "\n_______________ Anim channel sets _______________\n");
    //result->m_channelSets->writeTableByFrames(filePointer);
    result->m_channelSets->writeTableByChannelSets(filePointer);
    fclose(filePointer);
  }
  #endif // DEBUG_TRANSFORMS
#endif // NM_DEBUG

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
