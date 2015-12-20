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
#include "assetProcessor/AnimSource/ChannelSetInfoCompressorQSA.h"
#include "assetProcessor/AnimSource/AnimSourceCompressorQSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class ChannelSetInfoQSABuilder : private MR::ChannelSetInfoQSA
{
public:
  friend class ChannelSetInfoCompressorQSA; // Allow the compressor access to the internal parameters
};

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetInfoCompressorQSA Functions
//----------------------------------------------------------------------------------------------------------------------
ChannelSetInfoCompressorQSA::ChannelSetInfoCompressorQSA(AnimSourceCompressorQSA* manager)
{
  NMP_VERIFY(manager);
  m_manager = manager;

  //-----------------------
  // Channel compression methods
  m_posChanCompMethods = NULL;
  m_quatChanCompMethods = NULL;

  //-----------------------
  // Unchanging pos channel
  m_unchangingPosTable = NULL;
  m_unchangingPosQuantisation = NULL;
  m_unchangingPosQuantised = NULL;

  //-----------------------
  // Unchanging quat channel
  m_unchangingQuatTable = NULL;
  m_unchangingQuatQuantisation = NULL;
  m_unchangingQuatQuantised = NULL;

  //-----------------------
  // Intermediate compiled data
  m_unchangingPosData = NULL;
  m_unchangingQuatData = NULL;

  //-----------------------
  // Compiled channel set information
  m_channelSetInfoData = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
ChannelSetInfoCompressorQSA::~ChannelSetInfoCompressorQSA()
{
  termCompressor();
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::setDefaults()
{
  // Information
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  const ChannelSetOverrides* channelSetOverrides = inputAnim->getChannelSetOverrides();
  NMP_VERIFY(channelSetOverrides);
  const ChannelSetRequirements* channelSetRequirements = inputAnim->getChannelSetRequirements();
  NMP_VERIFY(channelSetRequirements);
  const MR::RigToAnimEntryMap* rigToAnimEntryMap = m_manager->getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(rigToAnimEntryMap);
  uint32_t numRigToAnimMapEntries = rigToAnimEntryMap->getNumEntries();

  //-----------------------
  // Initialise to defaults (catch all case for when rig has multiple
  // bones of the same name)
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    setCompChanMethodForPosChan(i, Sampled);
    setCompChanMethodForQuatChan(i, Sampled);
  }

  //-----------------------
  // Initialise the compression methods to use splines
  for (uint32_t i = 0; i < numRigToAnimMapEntries; ++i)
  {
    uint16_t rigIndex = rigToAnimEntryMap->getEntryRigChannelIndex(i);
    uint16_t animIndex = rigToAnimEntryMap->getEntryAnimChannelIndex(i);

    // Check for a root bone override
    if (!channelSetOverrides->isARootBone(rigIndex))
    {
      setCompChanMethodForPosChan(animIndex, Spline);
      setCompChanMethodForQuatChan(animIndex, Spline);
    }
  }
  
  // Validate the default compression methods
  validateOptions();
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::validateOptions()
{
  // Information
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  const ChannelSetRequirements* channelSetRequirements = inputAnim->getChannelSetRequirements();
  NMP_VERIFY(channelSetRequirements);
  const std::vector<const MR::AnimRigDef*>& rigs = m_manager->getRigs();
  const std::vector<const MR::RigToAnimEntryMap*>& rigToAnimEntryMaps = m_manager->getRigToAnimEntryMaps();
  const MR::RigToAnimEntryMap* rigToAnimEntryMap = m_manager->getConglomerateRigToAnimEntryMap();  
  NMP_VERIFY(rigToAnimEntryMap);
  uint32_t numRigToAnimMapEntries = rigToAnimEntryMap->getNumEntries();
  uint32_t numFrames = inputAnim->getMaxNumKeyframes();
  bool useDefaultPose = m_manager->getUseDefaultPose();

  //-----------------------
  // Validate the position channel compression methods
  for (uint32_t i = 0; i < numRigToAnimMapEntries; ++i)
  {
    uint16_t animIndex = rigToAnimEntryMap->getEntryAnimChannelIndex(i);

    if (getCompChanMethodForPosChan(animIndex) == Spline)
    {
      // For low frame count animations (i.e. a single spline interval) the size
      // overhead of the spline data outweighs the benefit of fewer keys.
      if (numFrames < 4)
      {
        setCompChanMethodForPosChan(animIndex, Sampled);
      }
    }

    // Check for an unvarying position channel that would be best compressed
    // in local space using the regularly sampled pos compression method.
    // i.e. no sample data is required for this channel, with the quantisation offset
    // encoding the unvarying value
    if (channelSetRequirements->isChannelPosUnchanging(animIndex))
    {
      // Check if the unchanging value is the same as the default. i.e the channel
      // is the same as the bind pose (or identity if compiled as a delta animation).
      if (useDefaultPose && checkForUnchangingPosDefault(i, inputAnim, rigs, rigToAnimEntryMaps))
      {
        setCompChanMethodForPosChan(animIndex, UnchangingDefault);
      }
      else
      {
        setCompChanMethodForPosChan(animIndex, Unchanging);
      }
    }
  }

  //-----------------------
  // Validate the quaternion channel compression methods
  for (uint32_t i = 0; i < numRigToAnimMapEntries; ++i)
  {
    uint16_t animIndex = rigToAnimEntryMap->getEntryAnimChannelIndex(i);

    if (getCompChanMethodForQuatChan(animIndex) == Spline)
    {
      // For low frame count animations (i.e. a single spline interval) the size
      // overhead of the spline data outweighs the benefit of fewer keys.
      if (numFrames < 4)
      {
        setCompChanMethodForQuatChan(animIndex, Sampled);
      }
    }

    // Check for an unvarying quaternion channel that would be best compressed
    // in local space using the regularly sampled quat compression method.
    // i.e. no sample data is required for this channel, with the quantisation offset
    // encoding the unvarying value
    if (channelSetRequirements->isChannelQuatUnchanging(animIndex))
    {
      // Check if the unchanging value is the same as the default. i.e the channel
      // is the same as the bind pose (or identity if compiled as a delta animation).
      if (useDefaultPose && checkForUnchangingQuatDefault(i, inputAnim, rigs, rigToAnimEntryMaps))
      {
        setCompChanMethodForQuatChan(animIndex, UnchangingDefault);
      }
      else
      {
        setCompChanMethodForQuatChan(animIndex, Unchanging);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool ChannelSetInfoCompressorQSA::checkForUnchangingPosDefault(
  uint32_t animChannelIndex,
  const AnimSourceUncompressed* animSource,
  const std::vector<const MR::AnimRigDef*>& rigs,
  const std::vector<const MR::RigToAnimEntryMap*>& rigToAnimEntryMaps) const
{
  NMP_VERIFY(rigs.size() == rigToAnimEntryMaps.size());
  uint32_t numRigs = (uint32_t)rigs.size();

  // Get the unchanging epsilon tolerance
  const float posEps = animSource->getChannelSetRequirements()->getUnchangingPosChannelEps();

  // Get the unchanging pos key
  const ChannelSetTable* channelSets = animSource->getChannelSets();
  NMP_VERIFY(channelSets);
  NMP::Vector3 posKey;
  channelSets->getPosKey(animChannelIndex, 0, posKey);

  if (animSource->getGenerateDeltas())
  {
    // Compare the unchanging channel with the identity
    NMP::Vector3 vZero(NMP::Vector3::InitZero);
    if (!NMP::Vector3::compare(posKey, vZero, posEps))
    {
      return false;
    }
  }
  else
  {
    // Compare the unchanging channel with the rig bind poses
    for (uint32_t rigIndex = 0; rigIndex < numRigs; ++rigIndex)
    {
      // Check if the anim channel is in the mapping
      const MR::RigToAnimEntryMap* rigToAnimEntryMap = rigToAnimEntryMaps[rigIndex];
      NMP_VERIFY(rigToAnimEntryMap);
      uint16_t rigChannelIndex;
      if (rigToAnimEntryMap->getRigIndexForAnimIndex((uint16_t) animChannelIndex, rigChannelIndex))
      {
        // Get the rig bind pose value
        const MR::AnimRigDef* rig = rigs[rigIndex];
        NMP_VERIFY(rig);
        NMP::Vector3 bindPosePosKey = *(rig->getBindPoseBonePos(rigChannelIndex));

        // Compare the unchanging values
        if (!NMP::Vector3::compare(posKey, bindPosePosKey, posEps))
        {
          return false;
        }
      }
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ChannelSetInfoCompressorQSA::checkForUnchangingQuatDefault(
  uint32_t animChannelIndex,
  const AnimSourceUncompressed* animSource,
  const std::vector<const MR::AnimRigDef*>& rigs,
  const std::vector<const MR::RigToAnimEntryMap*>& rigToAnimEntryMaps) const
{
  NMP_VERIFY(rigs.size() == rigToAnimEntryMaps.size());
  uint32_t numRigs = (uint32_t)rigs.size();

  // Get the unchanging epsilon tolerance
  const float quatEps = animSource->getChannelSetRequirements()->getUnchangingQuatChannelEps();

  // Get the unchanging quat key
  const ChannelSetTable* channelSets = animSource->getChannelSets();
  NMP_VERIFY(channelSets);
  NMP::Quat quatKey;
  channelSets->getQuatKey(animChannelIndex, 0, quatKey);

  if (animSource->getGenerateDeltas())
  {
    // Compare the unchanging channel with the identity
    NMP::Quat qIdentity(NMP::Quat::kIdentity);
    if (quatKey.w < 0)
      qIdentity.w = -1.0f;

    if (!NMP::Quat::compare(quatKey, qIdentity, quatEps))
    {
      return false;
    }
  }
  else
  {
    // Compare the unchanging channel with the rig bind poses
    for (uint32_t rigIndex = 0; rigIndex < numRigs; ++rigIndex)
    {
      // Check if the anim channel is in the mapping
      const MR::RigToAnimEntryMap* rigToAnimEntryMap = rigToAnimEntryMaps[rigIndex];
      NMP_VERIFY(rigToAnimEntryMap);
      uint16_t rigChannelIndex;
      if (rigToAnimEntryMap->getRigIndexForAnimIndex((uint16_t) animChannelIndex, rigChannelIndex))
      {
        // Get the rig bind pose value
        const MR::AnimRigDef* rig = rigs[rigIndex];
        NMP_VERIFY(rig);
        NMP::Quat bindPoseQuatKey = *(rig->getBindPoseBoneQuat(rigChannelIndex));

        // Ensure that the keys are in the same semi-arc
        float d = quatKey.dot(bindPoseQuatKey);
        if (d < 0)
          quatKey *= -1.0f;

        // Compare the unchanging values
        if (!NMP::Quat::compare(quatKey, bindPoseQuatKey, quatEps))
        {
          return false;
        }
      }
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::compileChannelSetInfo()
{
  computeCompChannelMaps();
  computeChannelSetRootInfo();
  unchangingPosComputeInfo();
  unchangingQuatComputeInfo();
  buildChannelSetInfo();
}

//----------------------------------------------------------------------------------------------------------------------
ChannelSetInfoCompressorQSA::chanMethodType
ChannelSetInfoCompressorQSA::getCompChanMethodForPosChan(uint32_t animChanIndex) const
{
#ifdef NMP_ENABLE_ASSERTS
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(inputAnim->isValidChannelSet(animChanIndex));
  NMP_VERIFY(m_posChanCompMethods);
#endif
  return m_posChanCompMethods[animChanIndex];
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::setCompChanMethodForPosChan(
  uint32_t animChanIndex,
  ChannelSetInfoCompressorQSA::chanMethodType method)
{
#ifdef NMP_ENABLE_ASSERTS
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(inputAnim->isValidChannelSet(animChanIndex));
  NMP_VERIFY(m_posChanCompMethods);
#endif
  m_posChanCompMethods[animChanIndex] = method;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::setDesiredCompChanMethodForPosChan(
  uint32_t animChanIndex,
  ChannelSetInfoCompressorQSA::chanMethodType method)
{
#ifdef NMP_ENABLE_ASSERTS
  NMP_VERIFY(method == Sampled || method == Spline);
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(inputAnim->isValidChannelSet(animChanIndex));
  NMP_VERIFY(m_posChanCompMethods);
#endif
  if (m_posChanCompMethods[animChanIndex] != UnchangingDefault &&
     m_posChanCompMethods[animChanIndex] != Unchanging)
  {
    m_posChanCompMethods[animChanIndex] = method;
  }
}

//----------------------------------------------------------------------------------------------------------------------
ChannelSetInfoCompressorQSA::chanMethodType
ChannelSetInfoCompressorQSA::getCompChanMethodForQuatChan(uint32_t animChanIndex) const
{
#ifdef NMP_ENABLE_ASSERTS
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(inputAnim->isValidChannelSet(animChanIndex));
#endif
  return m_quatChanCompMethods[animChanIndex];
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::setCompChanMethodForQuatChan(
  uint32_t animChanIndex,
  ChannelSetInfoCompressorQSA::chanMethodType method)
{
#ifdef NMP_ENABLE_ASSERTS
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(inputAnim->isValidChannelSet(animChanIndex));
#endif
  m_quatChanCompMethods[animChanIndex] = method;
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::setDesiredCompChanMethodForQuatChan(
  uint32_t animChanIndex,
  ChannelSetInfoCompressorQSA::chanMethodType method)
{
#ifdef NMP_ENABLE_ASSERTS
  NMP_VERIFY(method == Sampled || method == Spline);
  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(inputAnim->isValidChannelSet(animChanIndex));
  NMP_VERIFY(m_quatChanCompMethods);
#endif
  if (m_quatChanCompMethods[animChanIndex] != UnchangingDefault &&
     m_quatChanCompMethods[animChanIndex] != Unchanging)
  {
    m_quatChanCompMethods[animChanIndex] = method;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::writeDebug(FILE* filePointer)
{
  NMP_VERIFY(filePointer);
  fprintf(filePointer, "\n-----------------------------------------------------------------------------------------------\n");
  fprintf(filePointer, "ChannelSetInfo Compressor Dump\n");
  fprintf(filePointer, "-----------------------------------------------------------------------------------------------\n");

  writeCompToAnimMaps(filePointer);
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::writeCompToAnimMaps(FILE* filePointer)
{
  NMP_VERIFY(filePointer);

  uint32_t unchangingPosNumChans = getUnchangingPosNumChans();
  uint32_t unchangingQuatNumChans = getUnchangingQuatNumChans();
  uint32_t sampledPosNumChans = getSampledPosNumChans();
  uint32_t sampledQuatNumChans = getSampledQuatNumChans();
  uint32_t splinePosNumChans = getSplinePosNumChans();
  uint32_t splineQuatNumChans = getSplineQuatNumChans();

  fprintf(filePointer, "\n_________________ UnchangingPosCompToAnimMap _________________\n");
  for (uint32_t i = 0; i < unchangingPosNumChans; ++i)
  {
    fprintf(filePointer, "comp chan = %d, anim chan = %d\n", i, m_unchangingPosCompToAnimMap[i]);
  }

  fprintf(filePointer, "\n_________________ UnchangingQuatCompToAnimMap _________________\n");
  for (uint32_t i = 0; i < unchangingQuatNumChans; ++i)
  {
    fprintf(filePointer, "comp chan = %d, anim chan = %d\n", i, m_unchangingQuatCompToAnimMap[i]);
  }

  fprintf(filePointer, "\n_________________ SampledPosCompToAnimMap _________________\n");
  for (uint32_t i = 0; i < sampledPosNumChans; ++i)
  {
    fprintf(filePointer, "comp chan = %d, anim chan = %d\n", i, m_sampledPosCompToAnimMap[i]);
  }

  fprintf(filePointer, "\n_________________ SampledQuatCompToAnimMap _________________\n");
  for (uint32_t i = 0; i < sampledQuatNumChans; ++i)
  {
    fprintf(filePointer, "comp chan = %d, anim chan = %d\n", i, m_sampledQuatCompToAnimMap[i]);
  }

  fprintf(filePointer, "\n_________________ SplinePosCompToAnimMap _________________\n");
  for (uint32_t i = 0; i < splinePosNumChans; ++i)
  {
    fprintf(filePointer, "comp chan = %d, anim chan = %d\n", i, m_splinePosCompToAnimMap[i]);
  }

  fprintf(filePointer, "\n_________________ SplineQuatCompToAnimMap _________________\n");
  for (uint32_t i = 0; i < splineQuatNumChans; ++i)
  {
    fprintf(filePointer, "comp chan = %d, anim chan = %d\n", i, m_splineQuatCompToAnimMap[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Callback Functions
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::initCompressor()
{
  //-----------------------
  // Information
  m_numChannelSets = m_manager->getNumAnimChannelSets();
  NMP_VERIFY(m_numChannelSets > 0);

  //-----------------------
  // Allocate the memory for the channel set compression methods
  m_posChanCompMethods = (chanMethodType*) NMPMemoryAlloc(sizeof(chanMethodType) * m_numChannelSets);
  NMP_ASSERT(m_posChanCompMethods);
  m_quatChanCompMethods = (chanMethodType*) NMPMemoryAlloc(sizeof(chanMethodType) * m_numChannelSets);
  NMP_ASSERT(m_quatChanCompMethods);
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::termCompressor()
{
  //-----------------------
  // Channel compression methods
  if (m_posChanCompMethods)
  {
    NMP::Memory::memFree(m_posChanCompMethods);
    m_posChanCompMethods = NULL;
  }

  if (m_quatChanCompMethods)
  {
    NMP::Memory::memFree(m_quatChanCompMethods);
    m_quatChanCompMethods = NULL;
  }

  //-----------------------
  // Unchanging pos channel
  if (m_unchangingPosTable)
  {
    NMP::Memory::memFree(m_unchangingPosTable);
    m_unchangingPosTable = NULL;
  }

  if (m_unchangingPosQuantisation)
  {
    NMP::Memory::memFree(m_unchangingPosQuantisation);
    m_unchangingPosQuantisation = NULL;
  }

  if (m_unchangingPosQuantised)
  {
    NMP::Memory::memFree(m_unchangingPosQuantised);
    m_unchangingPosQuantised = NULL;
  }

  if (m_unchangingPosData)
  {
    NMP::Memory::memFree(m_unchangingPosData);
    m_unchangingPosData = NULL;
  }

  //-----------------------
  // Unchanging quat channel
  if (m_unchangingQuatTable)
  {
    NMP::Memory::memFree(m_unchangingQuatTable);
    m_unchangingQuatTable = NULL;
  }

  if (m_unchangingQuatQuantisation)
  {
    NMP::Memory::memFree(m_unchangingQuatQuantisation);
    m_unchangingQuatQuantisation = NULL;
  }

  if (m_unchangingQuatQuantised)
  {
    NMP::Memory::memFree(m_unchangingQuatQuantised);
    m_unchangingQuatQuantised = NULL;
  }

  if (m_unchangingQuatData)
  {
    NMP::Memory::memFree(m_unchangingQuatData);
    m_unchangingQuatData = NULL;
  }

  //-----------------------
  // Compiled channel set information
  if (m_channelSetInfoData)
  {
    NMP::Memory::memFree(m_channelSetInfoData);
    m_channelSetInfoData = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
size_t ChannelSetInfoCompressorQSA::computeChannelSetInfoRequirements() const
{
  NMP::Memory::Format memReqs(0, NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Header info
  memReqs += NMP::Memory::Format(sizeof(MR::ChannelSetInfoQSA), NMP_NATURAL_TYPE_ALIGNMENT);

  //-----------------------
  // Unchanging channels quantisation info
  uint32_t unchangingPosNumChannels = (uint32_t)m_unchangingPosCompToAnimMap.size();
  if (unchangingPosNumChannels > 0)
  {
    memReqs += NMP::Memory::Format(sizeof(MR::UnchangingKeyQSA) * unchangingPosNumChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  }

  uint32_t unchangingQuatNumChannels = (uint32_t)m_unchangingQuatCompToAnimMap.size();
  if (unchangingQuatNumChannels > 0)
  {
    memReqs += NMP::Memory::Format(sizeof(MR::UnchangingKeyQSA) * unchangingQuatNumChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  }

  //-----------------------
  // Pack whole structure for optimal DMA transfers.
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  return memReqs.size;
}

//----------------------------------------------------------------------------------------------------------------------
// Compression Functions
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::computeCompChannelMaps()
{
  //-----------------------
  // Information
  NMP_VERIFY(m_manager);
  const AnimSourceUncompressed *inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);

  //-----------------------
  // Clear the comp to anim channel index maps
  m_unchangingDefaultPosCompToAnimMap.clear();
  m_unchangingDefaultQuatCompToAnimMap.clear();
  m_unchangingPosCompToAnimMap.clear();
  m_unchangingQuatCompToAnimMap.clear();
  m_sampledPosCompToAnimMap.clear();
  m_sampledQuatCompToAnimMap.clear();
  m_splinePosCompToAnimMap.clear();
  m_splineQuatCompToAnimMap.clear();

  //-----------------------
  // Build the comp to anim channel maps
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {

    // Pos channel maps
    uint32_t chanPosMethod = getCompChanMethodForPosChan(i);
    if (chanPosMethod == UnchangingDefault)
      m_unchangingDefaultPosCompToAnimMap.push_back(i);
    if (chanPosMethod == Unchanging)
      m_unchangingPosCompToAnimMap.push_back(i);
    if (chanPosMethod == Sampled)
      m_sampledPosCompToAnimMap.push_back(i);
    if (chanPosMethod == Spline)
      m_splinePosCompToAnimMap.push_back(i);

    // Quat channel maps
    uint32_t chanQuatMethod = getCompChanMethodForQuatChan(i);
    if (chanQuatMethod == UnchangingDefault)
      m_unchangingDefaultQuatCompToAnimMap.push_back(i);
    if (chanQuatMethod == Unchanging)
      m_unchangingQuatCompToAnimMap.push_back(i);
    if (chanQuatMethod == Sampled)
      m_sampledQuatCompToAnimMap.push_back(i);
    if (chanQuatMethod == Spline)
      m_splineQuatCompToAnimMap.push_back(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::computeChannelSetRootInfo()
{
  // Default value if lowerRigIndex = 0. Note that at runtime the rigToAnim
  // map will convert -1 entries to 0
  m_worldspaceRootID = 0xFFFF;
  m_worldspaceRootQuat.identity();

  // Check for an animation compressed in local space (additive anims etc)
  if (!m_manager->getUseWorldSpace())
  {
    return;
  }

  // Information
  const MR::AnimRigDef* rig = m_manager->getConglomerateRig();
  NMP_VERIFY(rig);
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();
  NMP_VERIFY(hierarchy);
  const MR::RigToAnimEntryMap* rigToAnimEntryMap = m_manager->getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(rigToAnimEntryMap);
  const ChannelSetTable* channelSetsWS = m_manager->getChannelSetsWS();
  NMP_VERIFY(channelSetsWS);

  // Allocate an array to hold the worldspace rig channel flags
  uint32_t numRigBones = rig->getNumBones();
  NMP::Memory::Format memReqsChans(sizeof(bool) * numRigBones, NMP_NATURAL_TYPE_ALIGNMENT);
  NMP::Memory::Resource memResChans = NMPMemoryAllocateFromFormat(memReqsChans);
  bool* rigChannelFlags = (bool*)memResChans.ptr;

  // Initialise each of the rig channels corresponding to the sampled or spline channels. These
  // channels are being stored in worldspace while the unchanging channels are stored in local space.
  for (uint32_t i = 0; i < numRigBones; ++i)
    rigChannelFlags[i] = false;

  // Sampled quat channels
  uint32_t sampledQuatNumChans = (uint32_t)m_sampledQuatCompToAnimMap.size();
  for (uint32_t i = 0; i < sampledQuatNumChans; ++i)
  {
    // Get the rig channel index corresponding to the sampled channel
    uint32_t animChannelIndex = m_sampledQuatCompToAnimMap[i];    
    uint16_t rigChannelIndex;
    if (rigToAnimEntryMap->getRigIndexForAnimIndex((uint16_t) animChannelIndex, rigChannelIndex))
    {
      rigChannelFlags[rigChannelIndex] = true;
    }
  }

  // Spline quat channels
  uint32_t splineQuatNumChans = (uint32_t)m_splineQuatCompToAnimMap.size();
  for (uint32_t i = 0; i < splineQuatNumChans; ++i)
  {
    // Get the rig channel index corresponding to the sampled channel
    uint32_t animChannelIndex = m_splineQuatCompToAnimMap[i];    
    uint16_t rigChannelIndex;
    if (rigToAnimEntryMap->getRigIndexForAnimIndex((uint16_t) animChannelIndex, rigChannelIndex))
    {
      rigChannelFlags[rigChannelIndex] = true;
    }
  }

  // Find the highest worldspace rig channel index
  uint32_t upperRigIndex = numRigBones - 1;
  for (; upperRigIndex > 0; --upperRigIndex)
  {
    if (rigChannelFlags[upperRigIndex])
      break;
  }

  // Find the lowest valid conglomerate rig channel index. Note that all channels below
  // the lower channel are unchanging channels.
  uint32_t lowerRigIndex = 0;
  for (; lowerRigIndex < upperRigIndex; ++lowerRigIndex)
  {
    if (rigChannelFlags[lowerRigIndex])
      break;
  }

  // Set the lowest sampled rig index
  uint32_t lowestSampledRigIndex = lowerRigIndex;

  // Merge the lower and upper ranges until they are common
  for (; upperRigIndex > lowerRigIndex; --upperRigIndex)
  {
    // Update the rig channel flags
    if (rigChannelFlags[upperRigIndex])
    {
      int32_t parentRigIndex = hierarchy->getParentIndex(upperRigIndex);
      NMP_VERIFY(parentRigIndex != INVALID_HIERARCHY_INDEX);
      rigChannelFlags[parentRigIndex] = true;
      rigChannelFlags[upperRigIndex] = false;
      if (parentRigIndex < (int32_t)lowerRigIndex)
      {
        lowerRigIndex = parentRigIndex;
      }
    }
  }

  // Although we have found the common root bone, this bone could still be animated.
  // We must traverse up the hierarchy until we find an unchanging channel. All channels
  // below this channel will also be unchanging.
  while (lowerRigIndex >= lowestSampledRigIndex)
  {
    lowerRigIndex = hierarchy->getParentIndex(lowerRigIndex);
    NMP_VERIFY(lowerRigIndex != INVALID_HIERARCHY_INDEX);
  }

  // The conglomerate rig is used to define the set of animation channels that can be used
  // with the set of rigs compiled against the animation. The conglomerate rig will be
  // thrown away after compiling the animation. Since the common root bone corresponds to
  // a bone in the conglomerate rig, we must convert it to an animation channel so that at
  // runtime we can recover the common root bone for the selected anim set rig. Unfortunately
  // for partial animations there may be no animation channel for the corresponding rig channel
  // and we must search up the hierarchy until we find a valid channel.
  while (lowerRigIndex > 0)
  {
    if (rigToAnimEntryMap->getAnimIndexForRigIndex((uint16_t)lowerRigIndex, m_worldspaceRootID))
    {
      channelSetsWS->getQuatKey(lowerRigIndex, 0, m_worldspaceRootQuat);
      break;
    }
    lowerRigIndex = hierarchy->getParentIndex(lowerRigIndex);
  }

  // Tidy up
  NMP::Memory::memFree(memResChans.ptr);
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::unchangingPosComputeInfo()
{
  NMP::Vector3 pos;
  uint32_t     qx, qy, qz;
  NMP::Vector3 qMin, qMax;

  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);

  uint32_t unchangingPosNumChannels = (uint32_t)m_unchangingPosCompToAnimMap.size();
  if (unchangingPosNumChannels > 0)
  {
    const ChannelSetTable* channelSets = inputAnim->getChannelSets();
    NMP_VERIFY(channelSets);

    // Allocate the memory for the unchanging pos key table
    NMP::Memory::Format memReqsTable = AP::Vector3Table::getMemoryRequirements(unchangingPosNumChannels, 1);
    NMP::Memory::Resource memResTable = NMPMemoryAllocateFromFormat(memReqsTable);
    m_unchangingPosTable = AP::Vector3Table::init(memResTable, unchangingPosNumChannels, 1);

    // Allocate the memory for the quantised unchanging pos keys
    NMP::Memory::Format memReqsPosQuant(0, NMP_NATURAL_TYPE_ALIGNMENT);
    memReqsPosQuant.size = NMP::Memory::align(sizeof(NMP::Vector3) * unchangingPosNumChannels, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResPosQuant = NMPMemoryAllocateFromFormat(memReqsPosQuant);
    m_unchangingPosQuantised = (NMP::Vector3*)memResPosQuant.ptr;

    // Set the unchanging table data
    for (uint32_t i = 0; i < unchangingPosNumChannels; ++i)
    {
      uint32_t animChanIndx = m_unchangingPosCompToAnimMap[i];
      channelSets->getPosKey(animChanIndx, 0, pos);
      m_unchangingPosTable->setKey(i, 0, pos);
    }

    // Allocate the memory for the sampled pos key table quantisation info
    NMP::Memory::Format memReqsInfo = AP::Vector3QuantisationTable::getMemoryRequirements(unchangingPosNumChannels);
    NMP::Memory::Resource memResInfo = NMPMemoryAllocateFromFormat(memReqsInfo);
    m_unchangingPosQuantisation = AP::Vector3QuantisationTable::init(memResInfo, unchangingPosNumChannels);

    // Compute the quantisation info
    m_unchangingPosQuantisation->setPrecisionsX(16);
    m_unchangingPosQuantisation->setPrecisionsY(16);
    m_unchangingPosQuantisation->setPrecisionsZ(16);

    // qMin, qMax of each channel globally
    m_unchangingPosQuantisation->setQuantisationBounds(m_unchangingPosTable);
    m_unchangingPosQuantisation->getGlobalQuantisationBounds(qMin, qMax);
    m_unchangingPosQuantisation->setGlobalQuantisationBounds(qMin, qMax);
    m_unchangingPosQuantisationInfo.m_qMin[0] = qMin.x;
    m_unchangingPosQuantisationInfo.m_qMin[1] = qMin.y;
    m_unchangingPosQuantisationInfo.m_qMin[2] = qMin.z;
    m_unchangingPosQuantisationInfo.m_qMax[0] = qMax.x;
    m_unchangingPosQuantisationInfo.m_qMax[1] = qMax.y;
    m_unchangingPosQuantisationInfo.m_qMax[2] = qMax.z;

    // Allocate the memory for the compiled unchanging keys
    m_unchangingPosData = (MR::UnchangingKeyQSA*) NMPMemoryAlloc(sizeof(MR::UnchangingKeyQSA) * unchangingPosNumChannels);
    NMP_ASSERT(m_unchangingPosData);

    // Compile the unchanging keys
    for (uint32_t i = 0; i < unchangingPosNumChannels; ++i)
    {
      // Quantise
      m_unchangingPosTable->getKey(i, 0, pos);
      m_unchangingPosQuantisation->quantise(i, pos, qx, qy, qz);
      m_unchangingPosData[i].m_data[0] = (uint16_t)qx;
      m_unchangingPosData[i].m_data[1] = (uint16_t)qy;
      m_unchangingPosData[i].m_data[2] = (uint16_t)qz;

      // Dequantise
      m_unchangingPosQuantisation->dequantise(i, pos, qx, qy, qz);
      m_unchangingPosQuantised[i] = pos;
    }
  }
  else
  {
    m_unchangingPosQuantisationInfo.clear();
    m_unchangingQuatQuantisationInfo.clear();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::unchangingQuatComputeInfo()
{
  NMP::Quat    quat;
  NMP::Vector3 v;
  uint32_t     qx, qy, qz;
  NMP::Vector3 qMin, qMax;

  const AnimSourceUncompressed* inputAnim = m_manager->getInputAnimation();
  NMP_VERIFY(inputAnim);

  uint32_t unchangingQuatNumChannels = (uint32_t)m_unchangingQuatCompToAnimMap.size();
  if (unchangingQuatNumChannels > 0)
  {
    // Set the unchanging values
    const ChannelSetTable* channelSets = inputAnim->getChannelSets();
    NMP_VERIFY(channelSets);

    // Allocate the memory for the unchanging quat key table
    NMP::Memory::Format memReqsTable = AP::RotVecTable::getMemoryRequirements(unchangingQuatNumChannels, 1);
    NMP::Memory::Resource memResTable = NMPMemoryAllocateFromFormat(memReqsTable);
    m_unchangingQuatTable = AP::RotVecTable::init(memResTable, unchangingQuatNumChannels, 1);

    // Allocate the memory for the quantised unchanging quat keys
    NMP::Memory::Format memReqsQuatQuant(0, NMP_NATURAL_TYPE_ALIGNMENT);
    memReqsQuatQuant.size = NMP::Memory::align(sizeof(NMP::Quat) * unchangingQuatNumChannels, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResQuatQuant = NMPMemoryAllocateFromFormat(memReqsQuatQuant);
    m_unchangingQuatQuantised = (NMP::Quat*)memResQuatQuant.ptr;

    // Set the unchanging table data
    for (uint32_t i = 0; i < unchangingQuatNumChannels; ++i)
    {
      uint32_t animChanIndx = m_unchangingQuatCompToAnimMap[i];
      channelSets->getQuatKey(animChanIndx, 0, quat);
      m_unchangingQuatTable->setKey(i, 0, quat, true);
    }

    // Allocate the memory for the sampled quat key table quantisation info
    NMP::Memory::Format memReqsInfo = AP::Vector3QuantisationTable::getMemoryRequirements(unchangingQuatNumChannels);
    NMP::Memory::Resource memResInfo = NMPMemoryAllocateFromFormat(memReqsInfo);
    m_unchangingQuatQuantisation = AP::Vector3QuantisationTable::init(memResInfo, unchangingQuatNumChannels);

    // Compute the quantisation info
    m_unchangingQuatQuantisation->setPrecisionsX(16);
    m_unchangingQuatQuantisation->setPrecisionsY(16);
    m_unchangingQuatQuantisation->setPrecisionsZ(16);

    // qMin, qMax of each channel globally
    m_unchangingQuatQuantisation->setQuantisationBounds(m_unchangingQuatTable);
    m_unchangingQuatQuantisation->getGlobalQuantisationBounds(qMin, qMax);
    m_unchangingQuatQuantisation->setGlobalQuantisationBounds(qMin, qMax);
    m_unchangingQuatQuantisationInfo.m_qMin[0] = qMin.x;
    m_unchangingQuatQuantisationInfo.m_qMin[1] = qMin.y;
    m_unchangingQuatQuantisationInfo.m_qMin[2] = qMin.z;
    m_unchangingQuatQuantisationInfo.m_qMax[0] = qMax.x;
    m_unchangingQuatQuantisationInfo.m_qMax[1] = qMax.y;
    m_unchangingQuatQuantisationInfo.m_qMax[2] = qMax.z;

    // Allocate the memory for the unchanging keys
    m_unchangingQuatData = (MR::UnchangingKeyQSA*) NMPMemoryAlloc(sizeof(MR::UnchangingKeyQSA) * unchangingQuatNumChannels);
    NMP_ASSERT(m_unchangingQuatData);

    // Compile the unchanging keys
    for (uint32_t i = 0; i < unchangingQuatNumChannels; ++i)
    {
      // Quantise
      m_unchangingQuatTable->getKey(i, 0, v);
      m_unchangingQuatQuantisation->quantise(i, v, qx, qy, qz);
      m_unchangingQuatData[i].m_data[0] = (uint16_t)qx;
      m_unchangingQuatData[i].m_data[1] = (uint16_t)qy;
      m_unchangingQuatData[i].m_data[2] = (uint16_t)qz;

      // Dequantise
      m_unchangingQuatQuantisation->dequantise(i, v, qx, qy, qz);
      m_unchangingQuatQuantised[i].fromRotationVector(v, true);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoCompressorQSA::buildChannelSetInfo()
{
  NMP_VERIFY(!m_channelSetInfoData);

  // Information
  uint32_t unchangingDefaultPosNumChans = getUnchangingDefaultPosNumChans();
  uint32_t unchangingDefaultQuatNumChans = getUnchangingDefaultQuatNumChans();
  uint32_t unchangingPosNumChans = getUnchangingPosNumChans();
  uint32_t unchangingQuatNumChans = getUnchangingQuatNumChans();
  m_channelSetInfoDataSize = computeChannelSetInfoRequirements();
  m_unchangingPosDataSize = NMP::Memory::align(sizeof(MR::UnchangingKeyQSA) * unchangingPosNumChans, NMP_NATURAL_TYPE_ALIGNMENT);
  m_unchangingQuatDataSize = NMP::Memory::align(sizeof(MR::UnchangingKeyQSA) * unchangingQuatNumChans, NMP_NATURAL_TYPE_ALIGNMENT);

  //-----------------------
  // Allocate the memory for the channel set info data
  NMP::Memory::Format memReqs(m_channelSetInfoDataSize, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(memReqs);
  m_channelSetInfoData = (MR::ChannelSetInfoQSA*)memRes.ptr;

  //-----------------------
  // Header
  ChannelSetInfoQSABuilder* channelSetInfoBuilder = (ChannelSetInfoQSABuilder*)m_channelSetInfoData;
  channelSetInfoBuilder->m_worldspaceRootQuat = m_worldspaceRootQuat;
  channelSetInfoBuilder->m_worldspaceRootID = m_worldspaceRootID;
  channelSetInfoBuilder->m_numChannelSets = (uint16_t)m_numChannelSets;
  channelSetInfoBuilder->m_unchangingDefaultPosNumChannels = (uint16_t)unchangingDefaultPosNumChans;
  channelSetInfoBuilder->m_unchangingDefaultQuatNumChannels = (uint16_t)unchangingDefaultQuatNumChans;
  channelSetInfoBuilder->m_unchangingPosNumChannels = (uint16_t)unchangingPosNumChans;
  channelSetInfoBuilder->m_unchangingQuatNumChannels = (uint16_t)unchangingQuatNumChans;

  // Relocate the data pointers
  channelSetInfoBuilder->relocate();

  //-----------------------
  // Unchanging channels quantisation info
  channelSetInfoBuilder->m_unchangingPosQuantisationInfo = m_unchangingPosQuantisationInfo;
  channelSetInfoBuilder->m_unchangingQuatQuantisationInfo = m_unchangingQuatQuantisationInfo;

  // Pos
  for (uint32_t channel = 0; channel < unchangingPosNumChans; ++channel)
    channelSetInfoBuilder->m_unchangingPosData[channel] = m_unchangingPosData[channel];

  // Quat
  for (uint32_t channel = 0; channel < unchangingQuatNumChans; ++channel)
    channelSetInfoBuilder->m_unchangingQuatData[channel] = m_unchangingQuatData[channel];
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
