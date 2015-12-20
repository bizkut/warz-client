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
#include <algorithm>
#ifdef WIN32
  #include <crtdbg.h>
#endif

#include "assetProcessor/AnimSource/ChannelQuatBuilder.h"
#include "assetProcessor/AnimSource/ChannelPosBuilder.h"
#include "assetProcessor/AnimSource/AnimSectionCompressorASA.h"
#include "assetProcessor/AnimSource/AnimSourceCompressorASA.h"
#include "assetProcessor/AnimSource/TrajectorySourceCompressorASA.h"
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceBasicBuilder
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceASABuilder : public MR::AnimSourceASA
{
public:
  /// \brief Function to compute the required memory for a basic animation
  /// with the specified section sizes
  static NMP::Memory::Format getMemoryRequirements(
    const AnimSourceCompressorASA& manager);

  /// \brief Main function to build a basic animation in the allocated memory block
  static void init(
    const AnimSourceCompressorASA& manager,
    NMP::Memory::Resource&         buffer);
};

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetASAInfoBuilder Functions
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetASAInfoBuilder::init(
  ChannelSetASAInfo&   chanSetInfo,
  uint32_t             NMP_UNUSED(numRequiredQuatSamples),
  const NMP::Quat*     quatKeyframes,
  uint32_t             NMP_UNUSED(numRequiredPosSamples),
  const NMP::Vector3*  posKeyframes)
{
  NMP_VERIFY(quatKeyframes);
  NMP_VERIFY(posKeyframes);

  ChannelSetASAInfoBuilder& chanSetInfoBuilder = (ChannelSetASAInfoBuilder&)chanSetInfo;

  // Quat channel
  chanSetInfoBuilder.m_chanQuatDefault = quatKeyframes[0];

  // Pos channel
  chanSetInfoBuilder.m_chanPosDefault = posKeyframes[0];
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceASABuilder Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimSourceASABuilder::getMemoryRequirements(const AnimSourceCompressorASA& manager)
{
  NMP::Memory::Format memReqs(0, NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Information
  AnimSourceASALayout& memLayout = (AnimSourceASALayout&)manager.getAnimSourceMemLayout();
  const AnimSourceUncompressed* inputAnim = manager.getInputAnimation();
  NMP_VERIFY(inputAnim);
  uint32_t numChannelSets = inputAnim->getNumChannelSets();
  const std::vector<size_t>& sectionSizes = manager.getSectionSizes();
  NMP_VERIFY(sectionSizes.size() > 0);
  uint32_t numSections = (uint32_t) sectionSizes.size();

  //-----------------------
  // Header
  memLayout.m_headerOffset = 0;
  memReqs.size += NMP::Memory::align(sizeof(MR::AnimSourceASA), NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // AnimSectionInfo array (start frames, section and header sizes)
  memLayout.m_sectionInfoOffset = memReqs.size;
  memReqs.size += (sizeof(MR::AnimSectionInfoASA) * numSections);
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // AnimSectionASA (section pointers array)
  memLayout.m_sectionPtrsOffset = memReqs.size;
  memReqs.size += (sizeof(MR::DataRef) * numSections);
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // The array of channel set default information
  memLayout.m_chanSetInfoOffset = memReqs.size;
  memReqs.size += (sizeof(MR::ChannelSetASAInfo) * numChannelSets);
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // AnimSections themselves
  memLayout.m_sectionsOffset = memReqs.size;
  for (uint32_t i = 0; i < numSections; ++i)
  {
    memReqs.size += sectionSizes[i];
    memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);
  }

  //-----------------------
  // Trajectory source
  if (inputAnim->hasTrajectoryData())
  {
    memLayout.m_trajectoryOffset = memReqs.size;
    memLayout.m_trajectoryMemReqs = TrajectorySourceASABuilder::getMemoryRequirements(inputAnim);
    memReqs.size += memLayout.m_trajectoryMemReqs.size;
    memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);
  }
  else
  {
    memLayout.m_trajectoryOffset = 0;
  }

  //-----------------------
  // Channel name table
  if (manager.getAddChannelNamesTable())
  {
    memLayout.m_chanNamesOffset = memReqs.size;
    const NMP::OrderedStringTable* channelNames = inputAnim->getChannelNames();
    NMP_VERIFY(channelNames);
    NMP::Memory::Format chanNamesMemReqs = NMP::OrderedStringTable::getMemoryRequirements(
        channelNames->getNumEntries(),
        channelNames->getDataLength());
    memReqs.size += chanNamesMemReqs.size;
    memReqs.size = NMP::Memory::align(memReqs.size, chanNamesMemReqs.alignment);
  }
  else
  {
    memLayout.m_chanNamesOffset = 0;
  }

  //-----------------------
  // Total memory size
  memLayout.m_totalSize = memReqs.size;
  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceASABuilder::init(
  const AnimSourceCompressorASA& manager,
  NMP::Memory::Resource&         buffer)
{
  //-----------------------
  // Information
  const AnimSourceUncompressed* inputAnim = manager.getInputAnimation();
  NMP_VERIFY(inputAnim);
  const AnimSourceASALayout& memLayout  = manager.getAnimSourceMemLayout();
  AnimSourceASABuilder* anim            = (AnimSourceASABuilder*)buffer.ptr;
  NMP_VERIFY(anim);
  NMP_VERIFY(NMP_IS_ALIGNED(buffer.ptr, NMP_VECTOR_ALIGNMENT));

  // Set the animation type.
  anim->m_animType = ANIM_TYPE_ASA;

  // Section information
  const std::vector<uint32_t>& sectionStartFrames = manager.getSectionStartFrames();
  const std::vector<uint32_t>& sectionEndFrames   = manager.getSectionEndFrames();
  const std::vector<size_t>&   sectionSizes       = manager.getSectionSizes();

  //-----------------------
  // Animation information.
  uint32_t numChannelSets = inputAnim->getNumChannelSets();
  uint32_t numSections    = (uint32_t)sectionSizes.size();
  const ChannelSetRequirements* chanSetReqs = inputAnim->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);
  NMP_VERIFY(chanSetReqs->getNumChannelSets() == numChannelSets);

  //-----------------------
  // Header.
  uint8_t* data = (uint8_t*)buffer.ptr + memLayout.m_headerOffset;
  anim->m_funcTable = &MR::AnimSourceASA::m_functionTable;
  anim->m_isLocated = true;
  anim->m_duration = inputAnim->getDuration();
  anim->m_sampleFrequency = inputAnim->getSampleFrequency();
  anim->m_numSections = numSections;
  anim->m_numChannelSets = numChannelSets;
  anim->m_sections = NULL;
  anim->m_sectionsInfo = NULL;
  anim->m_channelSetsInfo = NULL;
  anim->m_trajectoryData = NULL;
  anim->m_channelNames = NULL;

  // Store the memory requirements of the animation class minus any section data.
  anim->m_animInstanceMemReqs = NMP::Memory::Format(memLayout.m_sectionsOffset, NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // AnimSectionInfo array (start frames, section and header sizes)
  data = (uint8_t*)buffer.ptr + memLayout.m_sectionInfoOffset;
  anim->m_sectionsInfo = (MR::AnimSectionInfoASA*)data;

  // Fill in section info data.
  for (uint32_t i = 0; i < numSections; ++i)
  {
    anim->m_sectionsInfo[i].setStartFrame(sectionStartFrames[i]);
    anim->m_sectionsInfo[i].setSectionSize((uint32_t)sectionSizes[i]);
    anim->m_sectionsInfo[i].setHeaderSize(sizeof(MR::AnimSectionASA) + (sizeof(MR::DataRef) * manager.getNumSubSections()));
  }

  //-----------------------
  // AnimSectionASA (section pointers array)
  data = (uint8_t*)buffer.ptr + memLayout.m_sectionPtrsOffset;
  anim->m_sections = (MR::DataRef*) data;  // Values filled in later

  //-----------------------
  // The array of channel set default information
  data = (uint8_t*)buffer.ptr + memLayout.m_chanSetInfoOffset;
  anim->m_channelSetsInfo = (MR::ChannelSetASAInfo*) data;

  //-----------------------
  // Compute the default information for each channel set in the animation
  const ChannelSetRequirements::channelSetReqs_t* chanReqs = chanSetReqs->getChannelSetRequirements();
  NMP_VERIFY(chanReqs);
  const ChannelSetTable* channelSets = inputAnim->getChannelSets();
  NMP_VERIFY(channelSets);

  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    // Default info
    ChannelSetASAInfoBuilder::init(
      anim->m_channelSetsInfo[i],
      chanReqs[i].m_numRequiredQuatSamples,
      channelSets->getChannelQuat(i),
      chanReqs[i].m_numRequiredPosSamples,
      channelSets->getChannelPos(i));
  }

  //-----------------------
  // AnimSections themselves
  data = (uint8_t*)buffer.ptr + memLayout.m_sectionsOffset;
  for (uint32_t k = 0; k < numSections; ++k)
  {
    // AnimSection.
    anim->m_sections[k].m_data = (void*) data;
    AnimSectionASABuilder::init(
      inputAnim,
      (MR::AnimSectionASA*) anim->m_sections[k].m_data,
      anim->m_channelSetsInfo,
      manager.getNumSubSections(),
      sectionStartFrames[k],
      sectionEndFrames[k]);

    // Move pointer on to the start of the next section
    data += sectionSizes[k];
    data = (uint8_t*)NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  }

  //-----------------------
  // Trajectory source
  if (inputAnim->hasTrajectoryData())
  {
    data = (uint8_t*)buffer.ptr + memLayout.m_trajectoryOffset;
    anim->m_trajectoryData = (MR::TrajectorySourceASA*)data;
    TrajectorySourceASABuilder::init(inputAnim, anim->m_trajectoryData, memLayout.m_trajectoryMemReqs);
  }

  //-----------------------
  // Channel name table
  if (manager.getAddChannelNamesTable())
  {
    data = (uint8_t*)buffer.ptr + memLayout.m_chanNamesOffset;
    anim->m_channelNames = (NMP::OrderedStringTable*)data;
    const NMP::OrderedStringTable* channelNames = inputAnim->getChannelNames();
    NMP_VERIFY(channelNames);

    NMP::Memory::Resource channelNamesRes;
    channelNamesRes.ptr = anim->m_channelNames;
    channelNamesRes.format = NMP::OrderedStringTable::getMemoryRequirements(
                               channelNames->getNumEntries(),
                               channelNames->getDataLength());
    NMP::OrderedStringTable::init(
      channelNamesRes,
      channelNames->getNumEntries(),
      channelNames->getOffsets(),
      channelNames->getData(),
      channelNames->getDataLength());
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceCompressorBasic
//----------------------------------------------------------------------------------------------------------------------
bool AnimSourceCompressorASA::computeSectionRequirements(
  uint32_t NMP_UNUSED(startFrame),
  uint32_t NMP_UNUSED(endFrame),
  size_t& NMP_UNUSED(sectionSize)) const
{
  NMP_VERIFY_FAIL("AnimSourceCompressorASA::computeSectionRequirements");
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimSourceCompressorASA::computeSectionRequirements()
{
  computeSectionBudgets();
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceCompressorASA::computeFinalMemoryRequirements() const
{
  NMP::Memory::Format memReqs;

  memReqs = AnimSourceASABuilder::getMemoryRequirements(*this);

  return memReqs.size;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorASA::buildAnimation(NMP::Memory::Resource& buffer) const
{
  AnimSourceASABuilder::init(*this, buffer);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorASA::setDefaults()
{
  // Default options
  m_maxFramesPerSection = 60;
  m_numSubsections      = 2;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorASA::parseOptions()
{
  std::string stringOptions = getOptions();
  char* buffer = (char*)stringOptions.c_str();

  // Parse the options string
  const char seps[] = " ,\t\n";
  char* token;
  token = strtok(buffer, seps);
  while (token)
  {
    bool status = true; // Eat any unrecognised tokens

    if (stricmp(token, "-mfps") == 0)
    {
      //-----------------------
      // Max frames per section
      status = false;
      token = strtok(0, seps);
      if (token && token[0] != '-')
      {
        m_maxFramesPerSection = (uint32_t)atoi(token);
        status = true;
      }
    }
    else if (stricmp(token, "-nsub") == 0)
    {
      //-----------------------
      // Num of subsections
      status = false;
      token = strtok(0, seps);
      if (token && token[0] != '-')
      {
        m_numSubsections = (uint32_t)atof(token);
        uint32_t const maxSubsections = 30;
        m_numSubsections  = NMP::clampValue(m_numSubsections, (uint32_t)2, maxSubsections);
        status = true;
      }
    }

    // Get the next token if required
    if (status) token = strtok(0, seps);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Callback Functions
//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorASA::initCompressor()
{
  //-----------------------
  // Process the compressor options
  setDefaults();
  parseOptions();
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceCompressorASA::computeSectionMemoryRequirements(uint32_t numFrames, uint32_t numSubSections)
{
  NMP::Memory::Format memReqs(0, NMP_VECTOR_ALIGNMENT);
  NMP_VERIFY(m_inputAnimResampled);

  // Get the number of animation channel sets
  uint32_t numChannelSets = m_inputAnimResampled->getNumChannelSets();

  // Compute the memory requirements of a section without any keyframe data
  memReqs.size += NMP::Memory::align(sizeof(MR::AnimSectionASA), NMP_VECTOR_ALIGNMENT);
  memReqs.size += NMP::Memory::align(sizeof(MR::AnimSubSectionASA) * numSubSections, NMP_VECTOR_ALIGNMENT);
  memReqs.size += (sizeof(MR::ChannelSetASA) * numSubSections * numChannelSets);
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  // Compute the memory requirements of the keyframe data
  const ChannelSetRequirements* chanSetReqs = m_inputAnimResampled->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);
  NMP_VERIFY(chanSetReqs->getNumChannelSets() == numChannelSets);

  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    // Quat channel
    if (!chanSetReqs->isChannelQuatUnchanging(i))
    {
      NMP::Memory::Format memReqsQuat =
        AP::ChannelQuatBuilder::channelQuatGetMemoryRequirements(numFrames);
      memReqs.size += memReqsQuat.size;
    }

    // Pos channel
    if (!chanSetReqs->isChannelPosUnchanging(i))
    {
      NMP::Memory::Format memReqsPos =
        AP::ChannelPosBuilder::channelPosGetMemoryRequirements(numFrames);
      memReqs.size += memReqsPos.size;
    }
  }

  return memReqs.size;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorASA::computeSectionBudgets()
{
  NMP_VERIFY(m_numSubsections > 0 && m_numSubsections < 101);
  NMP_VERIFY(m_maxFramesPerSection > 1 && m_maxFramesPerSection < 121);
  uint32_t numSections;
  uint32_t framesPerSection;
  size_t bytesPerSection;
  const size_t budgetMaxSectionSize = MR::getMaxAnimSectionSize() * m_numSubsections;

  //-----------------------
  // Compute an even distribution of frames amongst the sections
  numSections = computeNumSections(m_maxFramesPerSection);
  framesPerSection = distributeSectionFrames(numSections, m_inputAnimResampled->getMaxNumKeyframes());

  //-----------------------
  // Adjust the number of guessed sections if this budget is greater than the max section limit
  bytesPerSection = computeSectionMemoryRequirements(framesPerSection, m_numSubsections);
  if (bytesPerSection > budgetMaxSectionSize)
  {
    // Estimate the maximum number of frames in any of the sections
    float bytesPerFrame = (float)bytesPerSection / (float)framesPerSection;
    framesPerSection = (uint32_t)(budgetMaxSectionSize / bytesPerFrame);

    // Check if the calculated max frames per section is within the section
    // budget limit
    size_t sectionBudget = computeSectionMemoryRequirements(framesPerSection, m_numSubsections);
    while (sectionBudget > budgetMaxSectionSize)
    {
      framesPerSection--;
      NMP_VERIFY(framesPerSection > 1);
      sectionBudget = computeSectionMemoryRequirements(framesPerSection, m_numSubsections);
    }
  }
  //-----------------------
  // Recompute an even distribution of frames amongst the sections
  numSections = computeNumSections(framesPerSection);
  framesPerSection = distributeSectionFrames(numSections, m_inputAnimResampled->getMaxNumKeyframes());
  for (uint32_t i = 0; i < numSections; ++i)
  {
    uint32_t numSectionFrames = getNumSectionFrameSamples(i);
    m_sectionSizes[i] = (uint32_t)computeSectionMemoryRequirements(numSectionFrames, m_numSubsections);
    NMP_VERIFY(m_sectionSizes[i] <= budgetMaxSectionSize);
  }
  //-----------------------
  // Check number of subsections. Limit size = MR::getMaxAnimSectionSize()
  size_t maxSectionSize = 0;
  for (uint32_t i = 0; i < numSections; ++i)
  {
    if (m_sectionSizes[i] > maxSectionSize)
    {
      maxSectionSize = m_sectionSizes[i];
    }
  }
  NMP_VERIFY((uint32_t)ceil((float)(maxSectionSize / m_numSubsections)) < MR::getMaxAnimSectionSize());
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorASA::buildRigToAnimMaps()
{
  const std::vector<const MR::AnimRigDef*>& rigs = getRigs();
  const std::vector<const MR::RigToAnimEntryMap*>& rigToAnimEntryMaps = getRigToAnimEntryMaps();
  NMP_VERIFY(rigs.size() == rigToAnimEntryMaps.size());
  std::vector<MR::RigToAnimMap*>& rigToAnimMaps = AnimSourceCompressor::getRigToAnimMaps();
  NMP_VERIFY(rigToAnimMaps.empty());

  uint32_t numRigs = (uint32_t) rigs.size();
  for (uint32_t i = 0; i < numRigs; ++i)
  {
    const MR::AnimRigDef* rig = rigs[i];
    const MR::RigToAnimEntryMap* rigToAnimEntryMap = rigToAnimEntryMaps[i];
    MR::RigToAnimMap* rigToAnimMap = buildRigToAnimEntryMap(rig, rigToAnimEntryMap);
    rigToAnimMaps.push_back(rigToAnimMap);
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
