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
#define _WCTYPE_INLINE_DEFINED
#include <string.h>
#include <sstream>
#include <vector>
#include <algorithm>
#ifdef WIN32
  #include <crtdbg.h>
#endif

#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrDefines.h"
#include "assetProcessor/AnimSource/AnimSourceCompressor.h"
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceCompressor
//----------------------------------------------------------------------------------------------------------------------
MR::RigToAnimMap* AnimSourceCompressor::buildRigToAnimEntryMap(
  const MR::AnimRigDef* rig,
  const MR::RigToAnimEntryMap* rigToAnimEntryMap)
{
  NMP_VERIFY(rig);
  NMP_VERIFY(rigToAnimEntryMap);

  // Rig to anim map
  uint32_t numRigBones = rig->getNumBones();
  NMP::Memory::Format memReqsEntryMap = rigToAnimEntryMap->getInstanceMemoryRequirements();
  NMP::Memory::Format memReqsRigToAnimMap = MR::RigToAnimMap::getMemoryRequirements(numRigBones, memReqsEntryMap);
  NMP::Memory::Resource memResRigToAnimMap = NMPMemoryAllocateFromFormat(memReqsRigToAnimMap);
  NMP_VERIFY(memResRigToAnimMap.ptr);
  ZeroMemory(memResRigToAnimMap.ptr, memReqsRigToAnimMap.size);
  MR::RigToAnimMap* result = MR::RigToAnimMap::init(memResRigToAnimMap, MR::RigToAnimMap::MapPairs, numRigBones, memReqsEntryMap);

  // Channel used flags
  NMP::BitArray* usedFlags = result->getUsedFlags();
  for (uint32_t i = 0; i < rigToAnimEntryMap->getNumEntries(); ++i)
  {
    uint16_t rigChannelIndex = rigToAnimEntryMap->getEntryRigChannelIndex(i);
    usedFlags->setBit(rigChannelIndex);
  }

  // RigToAnimEntryMap
  void* ptr = result->getRigToAnimMapData();
  NMP::Memory::memcpy(ptr, rigToAnimEntryMap, memReqsEntryMap.size);
  MR::RigToAnimEntryMap::relocate(ptr);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::RigToAnimMap* AnimSourceCompressor::buildAnimToRigTableMap(
  const MR::AnimRigDef* rig,
  const MR::RigToAnimEntryMap* rigToAnimEntryMap,
  const MR::RigToAnimEntryMap* conglomerateRigToAnimEntryMap)
{
  NMP_VERIFY(rig);
  NMP_VERIFY(rigToAnimEntryMap);
  NMP_VERIFY(conglomerateRigToAnimEntryMap);

  // Rig to anim map
  uint32_t numRigBones = rig->getNumBones();
  uint32_t numAnimChannels = conglomerateRigToAnimEntryMap->getNumEntries();
  NMP::Memory::Format memReqsTable = MR::AnimToRigTableMap::getMemoryRequirements(numAnimChannels);
  NMP::Memory::Format memReqsRigToAnimMap = MR::RigToAnimMap::getMemoryRequirements(numRigBones, memReqsTable);
  NMP::Memory::Resource memResRigToAnimMap = NMPMemoryAllocateFromFormat(memReqsRigToAnimMap);
  NMP_VERIFY(memResRigToAnimMap.ptr);
  ZeroMemory(memResRigToAnimMap.ptr, memReqsRigToAnimMap.size);
  MR::RigToAnimMap* result = MR::RigToAnimMap::init(memResRigToAnimMap, MR::RigToAnimMap::AnimToRig, numRigBones, memReqsTable);

  // Channel used flags
  NMP::BitArray* usedFlags = result->getUsedFlags();
  for (uint32_t i = 0; i < rigToAnimEntryMap->getNumEntries(); ++i)
  {
    uint16_t rigChannelIndex = rigToAnimEntryMap->getEntryRigChannelIndex(i);
    usedFlags->setBit(rigChannelIndex);
  }
  
  // AnimToRigTableMap
  NMP::Memory::Resource resource;
  resource.format = memReqsTable;
  resource.ptr = result->getRigToAnimMapData();
  ZeroMemory(resource.ptr, memReqsTable.size);
  MR::AnimToRigTableMap* animToRigTable = MR::AnimToRigTableMap::init(resource, numAnimChannels);

  for (uint32_t i = 0; i < rigToAnimEntryMap->getNumEntries(); ++i)
  {
    uint16_t rigChannelIndex = rigToAnimEntryMap->getEntryRigChannelIndex(i);
    uint16_t animChannelIndex = rigToAnimEntryMap->getEntryAnimChannelIndex(i);
    animToRigTable->setAnimToRigMapEntry(animChannelIndex, rigChannelIndex);
  }
  animToRigTable->setEntryInfo();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnimSourceCompressor::AnimSourceCompressor() :
  m_inputAnimResampled(NULL),
  m_messageLogger(NULL),
  m_errorLogger(NULL),
  m_addChannelNamesTable(false),
  m_memReqsChannelNamesTable(0, NMP_VECTOR_ALIGNMENT),
  m_memReqsAnimSource(0, NMP_VECTOR_ALIGNMENT)
{
}

//----------------------------------------------------------------------------------------------------------------------
AnimSourceCompressor::~AnimSourceCompressor()
{
  termCompressor(); // virtual
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource AnimSourceCompressor::compressAnimation(  
  const AnimSourceUncompressed*              inputAnim,
  const char*                                animFileName,
  const char*                                takeName,
  const acAnimInfo*                          animInfo,
  const char*                                options,
  const MR::AnimRigDef*                      conglomerateRig,
  const MR::RigToAnimEntryMap*               conglomerateRigToAnimEntryMap,
  const std::vector<const MR::AnimRigDef*>*  rigs,
  const std::vector<const MR::RigToAnimEntryMap*>* rigToAnimEntryMaps,
  std::vector<MR::RigToAnimMap*>*            rigToAnimMaps,
  NMP::BasicLogger*                          messageLogger,
  NMP::BasicLogger*                          errorLogger)
{
  //-------------------
  // Initialise the options. Also resample the animation and generate the
  // delta transforms from the first frame if required.
  initOptions(
    inputAnim,
    animFileName,
    takeName,
    animInfo,
    options,
    conglomerateRig,
    conglomerateRigToAnimEntryMap,
    rigs,
    rigToAnimEntryMaps,
    rigToAnimMaps,
    messageLogger,
    errorLogger);

  // Initialise the compressor
  initCompressor(); // virtual

  //-------------------
  // Compute the memory requirements for the channel names table
  m_memReqsChannelNamesTable.size = computeChannelNamesTableRequirements();
  m_memReqsChannelNamesTable.size = NMP::Memory::align(m_memReqsChannelNamesTable.size, NMP_VECTOR_ALIGNMENT); // Ensure DMA alignment

  //-------------------
  // Compute the full memory requirements for the animation
  m_memReqsAnimSource.size = computeFinalMemoryRequirements();
  m_memReqsAnimSource.size = NMP::Memory::align(m_memReqsAnimSource.size, NMP_VECTOR_ALIGNMENT); // Ensure DMA alignment

  // Allocate the memory for the animation
  NMP::Memory::Resource result = NMPMemoryAllocateFromFormat(m_memReqsAnimSource);

  // Zero the memory for binary invariance of any data padding
  ZeroMemory(result.ptr, m_memReqsAnimSource.size);

  //-------------------
  // Compile the animation into the allocated memory buffer
  NMP::Memory::Resource buffer = result;
  buildAnimation(buffer);
  
  // Compile the rig to anim maps
  buildRigToAnimMaps();

  //-------------------
  // Tidy up
  termCompressor();
  termOptions();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float AnimSourceCompressor::getCompressionRate() const
{
  NMP_VERIFY(m_inputAnimUncompressed);
  float duration = m_inputAnimUncompressed->getDuration();
  uint32_t numChannelSets = m_inputAnimUncompressed->getNumChannelSets();
  if (duration <= 0.0f || numChannelSets == 0)
    return 0;

  // Bytes per bone per second
  return m_memReqsAnimSource.size / (float)numChannelSets / duration;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressor::initCompressor()
{
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceCompressor::computeChannelNamesTableRequirements() const
{
  NMP::Memory::Format memReqs(0, NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Channel names table
  if (getAddChannelNamesTable())
  {
    NMP_VERIFY(m_inputAnimResampled);
    const NMP::OrderedStringTable* channelNames = m_inputAnimResampled->getChannelNames();
    NMP_VERIFY(channelNames);
    memReqs = NMP::OrderedStringTable::getMemoryRequirements(channelNames->getNumEntries(), channelNames->getDataLength());

    memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT); // Ensure DMA alignment

  }

  return memReqs.size;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressor::termCompressor()
{
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressor::initOptions(
  const AnimSourceUncompressed*               inputAnim,
  const char*                                 animFileName,
  const char*                                 takeName,
  const acAnimInfo*                           animInfo,
  const char*                                 options,
  const MR::AnimRigDef*                       conglomerateRig,
  const MR::RigToAnimEntryMap*                conglomerateRigToAnimEntryMap,
  const std::vector<const MR::AnimRigDef*>*   rigs,
  const std::vector<const MR::RigToAnimEntryMap*>* rigToAnimEntryMaps,
  std::vector<MR::RigToAnimMap*>*             rigToAnimMaps,
  NMP::BasicLogger*                           messageLogger,
  NMP::BasicLogger*                           errorLogger)
{
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(animFileName);
  NMP_VERIFY(takeName);
  NMP_VERIFY(animInfo);
  NMP_VERIFY(options);
  NMP_VERIFY(conglomerateRig);
  NMP_VERIFY(conglomerateRigToAnimEntryMap);
  NMP_VERIFY(rigs);
  NMP_VERIFY(rigToAnimEntryMaps);
  NMP_VERIFY(rigToAnimMaps);
  NMP_VERIFY(messageLogger);
  NMP_VERIFY(errorLogger);
  NMP_VERIFY(!rigs->empty());
  NMP_VERIFY(!rigToAnimEntryMaps->empty());
  NMP_VERIFY(rigToAnimMaps->empty());
  NMP_VERIFY(rigs->size() == rigToAnimEntryMaps->size());

  // Initialise the input parameters
  m_inputAnimUncompressed = inputAnim;
  m_inputAnimResampled = NULL;
  m_animFileName = animFileName;
  m_takeName = takeName;
  m_animInfo = animInfo;
  m_options = options;
  m_conglomerateRig = conglomerateRig;
  m_conglomerateRigToAnimEntryMap = conglomerateRigToAnimEntryMap;
  m_rigs = rigs;
  m_rigToAnimEntryMaps = rigToAnimEntryMaps;
  m_rigToAnimMaps = rigToAnimMaps;
  m_messageLogger = messageLogger;
  m_errorLogger = errorLogger;

  // Generate transform deltas from the first frame of the animation
  m_generateDeltas = parseOptionsForGenerateDeltas();

  // Resample rate of the animation
  m_resampleRate = parseOptionsForResampleRate();

  //-----------------------
  // Resample the uncompressed animation
  NMP::Memory::Format memReqsAnim = AnimSourceUncompressedBuilder::getMemoryRequirements(inputAnim, m_resampleRate);

  NMP::Memory::Resource memResAnim = NMPMemoryAllocateFromFormat(memReqsAnim);

  m_inputAnimResampled = AnimSourceUncompressedBuilder::init(
                           memResAnim,
                           inputAnim,
                           m_resampleRate,
                           m_generateDeltas);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressor::termOptions()
{
  if (m_inputAnimResampled)
  {
    NMP::Memory::memFree(m_inputAnimResampled);
    m_inputAnimResampled = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimSourceCompressor::parseOptionsForGenerateDeltas()
{
  if (strstr(m_options, "genDeltas") != NULL)
    return true;

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
float AnimSourceCompressor::parseOptionsForResampleRate()
{
  NMP_VERIFY(m_animInfo->m_FramesPerSecond > 0.0f);

  // Temporary buffer used to tokenise the options string
  std::string stringOptions = getOptions();
  char* buffer = (char*)stringOptions.c_str();

  const char seps[] = " ,\t\n";
  const char* token = strtok(buffer, seps);
  while (token)
  {
    // Check for the resampling switch
    if (stricmp(token, "-resample") == 0)
    {
      // Get the value token
      token = strtok(0, seps);
      if (token)
      {
        float fps = (float)atof(token);
        if (fps <= 0.0f)
        {
          m_messageLogger->output(
            "Animation '%s' take '%s': Invalid resample rate: -resample %s. Using rate from animation source.\n",
            m_animFileName,
            m_takeName,
            token);
          break;
        }

        if (fps == m_animInfo->m_FramesPerSecond)
        {
          m_messageLogger->output(
            "Animation '%s' take '%s': Resample rate is the same as the animation source.\n"
            "Resampling option has no effect (Although specifying the option may mean the animation is compiled separately against the rig in the animSet).\n",
            m_animFileName,
            m_takeName);
        }

        return fps;
      }
    }

    // Get the next token
    token = strtok(0, seps);
  }

  return m_animInfo->m_FramesPerSecond;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressor::getUnchangingPosAnimChannelIndices(std::vector<uint32_t>& animChanIndices) const
{
  animChanIndices.clear();

  NMP_VERIFY(m_inputAnimResampled);
  const ChannelSetRequirements* chanSetReqs = m_inputAnimResampled->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);
  uint32_t numChannelSets = chanSetReqs->getNumChannelSets();
  NMP_VERIFY(numChannelSets > 0);

  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    if (chanSetReqs->isChannelPosUnchanging(i))
      animChanIndices.push_back(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressor::getChangingPosAnimChannelIndices(std::vector<uint32_t>& animChanIndices) const
{
  animChanIndices.clear();

  NMP_VERIFY(m_inputAnimResampled);
  const ChannelSetRequirements* chanSetReqs = m_inputAnimResampled->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);
  uint32_t numChannelSets = chanSetReqs->getNumChannelSets();
  NMP_VERIFY(numChannelSets > 0);

  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    if (!chanSetReqs->isChannelPosUnchanging(i))
      animChanIndices.push_back(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressor::getUnchangingQuatAnimChannelIndices(std::vector<uint32_t>& animChanIndices) const
{
  animChanIndices.clear();

  NMP_VERIFY(m_inputAnimResampled);
  const ChannelSetRequirements* chanSetReqs = m_inputAnimResampled->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);
  uint32_t numChannelSets = chanSetReqs->getNumChannelSets();
  NMP_VERIFY(numChannelSets > 0);

  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    if (chanSetReqs->isChannelQuatUnchanging(i))
      animChanIndices.push_back(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressor::getChangingQuatAnimChannelIndices(std::vector<uint32_t>& animChanIndices) const
{
  animChanIndices.clear();

  NMP_VERIFY(m_inputAnimResampled);
  const ChannelSetRequirements* chanSetReqs = m_inputAnimResampled->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);
  uint32_t numChannelSets = chanSetReqs->getNumChannelSets();
  NMP_VERIFY(numChannelSets > 0);

  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    if (!chanSetReqs->isChannelQuatUnchanging(i))
      animChanIndices.push_back(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressor::writeChannelNamesTable(const char* filename) const
{
  NMP_VERIFY(filename);
  const NMP::OrderedStringTable* channelNames = m_inputAnimResampled->getChannelNames();
  NMP_VERIFY(channelNames);

  FILE* filePointer = fopen(filename, "w");
  if (filePointer)
  {
    uint32_t numEntries = channelNames->getNumEntries();
    for (uint32_t i = 0; i < numEntries; ++i)
    {
      const char* boneName = channelNames->getEntryString(i);
      NMP_VERIFY(boneName);
      fprintf(filePointer, "chan = %4d, name = %s\n", i, boneName);
    }
    fclose(filePointer);
  }
}


//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceCompressor
//----------------------------------------------------------------------------------------------------------------------
TrajectorySourceCompressor::TrajectorySourceCompressor() :
  m_inputTrajUncompressed(NULL),
  m_options(NULL),
  m_messageLogger(NULL),
  m_errorLogger(NULL),
  m_memReqsTrajSource(0, NMP_VECTOR_ALIGNMENT)
{
}

//----------------------------------------------------------------------------------------------------------------------
TrajectorySourceCompressor::~TrajectorySourceCompressor()
{
  termCompressor(); // virtual
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource TrajectorySourceCompressor::compressTrajectory(
  const TrajectorySourceUncompressed*        inputTrajectory,
  const char*                                options,
  NMP::BasicLogger*                          messageLogger,
  NMP::BasicLogger*                          errorLogger)
{
  //-------------------
  // Initialise the options
  initOptions(
    inputTrajectory,
    options,
    messageLogger,
    errorLogger);

  // Initialise the compressor
  initCompressor(); // virtual

  // Compute the memory requirements for the trajectory source
  m_memReqsTrajSource.size = computeTrajectoryRequirements();
  m_memReqsTrajSource.size = NMP::Memory::align(m_memReqsTrajSource.size, NMP_VECTOR_ALIGNMENT); // Ensure DMA alignment

  // Allocate the memory for the trajectory source
  NMP::Memory::Resource result = NMPMemoryAllocateFromFormat(m_memReqsTrajSource);

  // Zero the memory for binary invariance of any data padding
  ZeroMemory(result.ptr, m_memReqsTrajSource.size);

  //-------------------
  // Compile the trajectory into the allocated memory buffer
  NMP::Memory::Resource buffer = result;
  buildTrajectorySource(buffer);

  //-------------------
  // Tidy up
  termCompressor(); // virtual

  // Invalidate the options
  termOptions();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressor::initCompressor()
{
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressor::termCompressor()
{
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressor::initOptions(
  const TrajectorySourceUncompressed*        inputTrajectory,
  const char*                                options,
  NMP::BasicLogger*                          messageLogger,
  NMP::BasicLogger*                          errorLogger)
{
  NMP_VERIFY(inputTrajectory);
  NMP_VERIFY(options);
  NMP_VERIFY(messageLogger);
  NMP_VERIFY(errorLogger);

  // Initialise the input parameters
  m_inputTrajUncompressed = inputTrajectory;
  m_options = options;
  m_messageLogger = messageLogger;
  m_errorLogger = errorLogger;
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceCompressor::termOptions()
{
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
