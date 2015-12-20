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
#include "assetProcessor/AnimSource/AnimSourceCompressorNSA.h"
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

#define TEST_LOCATE_DISLOCATEx

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceCompressorNSA Functions
//----------------------------------------------------------------------------------------------------------------------
AnimSourceCompressorNSA::AnimSourceCompressorNSA() :
  m_unchangingDataCompiled(NULL),
  m_sampledDataCompiled(NULL),
  m_memReqsTrajectorySource(0, NMP_VECTOR_ALIGNMENT),
  m_trajectorySource(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
AnimSourceCompressorNSA::~AnimSourceCompressorNSA()
{
  termCompressor();
}

//----------------------------------------------------------------------------------------------------------------------
// Callback Functions
//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorNSA::initCompressor()
{
  UnchangingDataLayoutNSA* unchangingDataLayout = NULL;
  UnchangingDataCompiledNSA* unchangingDataCompiled = NULL;
  SampledDataLayoutNSA* sampledDataLayout = NULL;
  SampledDataCompiledNSA* sampledDataCompiled = NULL;

  const AnimSourceUncompressed* animSource = getInputAnimation(); // Resampled animation
  NMP_VERIFY(animSource);

  // Initialise the channel compression options
  ChannelCompressionOptionsNSA* channelCompressionOptions = new ChannelCompressionOptionsNSA;
  channelCompressionOptions->parseOptions(getOptions(), getErrorLogger());
  channelCompressionOptions->initChannelMethods(animSource);

  //-----------------------
  // Compile the trajectory source
  compileTrajectoryData();

  //-----------------------
  // Compile the sampled channel data
  bool compileFlag = true;
  while (compileFlag)
  {
    // Tidy up any previous sampled channel data
    if (sampledDataLayout)
    {
      delete sampledDataLayout;
      sampledDataLayout = NULL;
    }
    if (sampledDataCompiled)
    {
      delete sampledDataCompiled;
      sampledDataCompiled = NULL;
    }

    // Check if we have any sampled channels
    if (!channelCompressionOptions->hasSampledChannelMethods())
      break;

    // Calculate the layout of the 2D section packet grid
    sampledDataLayout = new SampledDataLayoutNSA;
    NMP_VERIFY(sampledDataLayout);
    sampledDataLayout->computeLayout(
      animSource,
      channelCompressionOptions);

    // Compile the sampled channel data
    sampledDataCompiled = new SampledDataCompiledNSA;
    NMP_VERIFY(sampledDataCompiled);
    sampledDataCompiled->compileSampledData(
      animSource,
      sampledDataLayout);

    // Check for any sampled channels that are unchanging and update the required
    // channel compression methods accordingly.
    compileFlag = sampledDataCompiled->checkForUnchangingChannels(
      sampledDataLayout,
      channelCompressionOptions);
  }

  //-----------------------
  // Calculate the layout of the unchanging channels
  unchangingDataLayout = new UnchangingDataLayoutNSA;
  unchangingDataLayout->computeLayout(
    animSource,
    channelCompressionOptions);

  // Compile the unchanging channels
  unchangingDataCompiled = new UnchangingDataCompiledNSA;
  unchangingDataCompiled->compileUnchangingData(
    animSource,
    unchangingDataLayout);

  //-----------------------
  // Compile the compression channel to anim channel maps
  // Note that we may not necessarily have any sampled data if the animation is a fixed pose
  unchangingDataCompiled->compileCompToAnimMaps(unchangingDataLayout);
  if (sampledDataCompiled)
  {
    NMP_VERIFY(sampledDataLayout);
    sampledDataCompiled->compileCompToAnimMaps(sampledDataLayout);  
  }

  //-----------------------
  // Logging information
  if (sampledDataLayout)
  {
    NMP::BasicLogger* messageLogger = getMessageLogger();
    messageLogger->output(
      "Animation file '%s' NSA compiled with number of sections (frame, channel) = %d, %d\n",
      getAnimFileName(),
      sampledDataLayout->getNumFrameSections(),
      sampledDataLayout->getNumChannelSections());
  }

  //-----------------------
  // Tidy up the layout data
  delete channelCompressionOptions;
  delete unchangingDataLayout;
  // Note that we may not necessarily have any sampled data if the animation is a fixed pose
  if (sampledDataLayout)
  {
    delete sampledDataLayout;
  }

  //-----------------------
  // Set the compiled data
  m_unchangingDataCompiled = unchangingDataCompiled;
  m_sampledDataCompiled = sampledDataCompiled;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorNSA::termCompressor()
{
  // Unchanging data
  if (m_unchangingDataCompiled)
  {
    delete m_unchangingDataCompiled;
    m_unchangingDataCompiled = NULL;
  }

  // Sampled data
  if (m_sampledDataCompiled)
  {
    delete m_sampledDataCompiled;
    m_sampledDataCompiled = NULL;
  }

  // Trajectory
  if (m_trajectorySource)
  {
    NMP::Memory::memFree(m_trajectorySource);
    m_trajectorySource = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceCompressorNSA::computeFinalMemoryRequirements() const
{  
  // Information
  uint32_t numFrameSections = 0;
  uint32_t numChannelSections = 0;
  uint32_t sampledPosNumQuantisationSets = 0;
  uint32_t sampledQuatNumQuantisationSets = 0;
  const uint32_t* sectionStartFrames = NULL;
  const MR::CompToAnimChannelMap* unchangingPosCompToAnimMap = NULL;
  const MR::CompToAnimChannelMap* unchangingQuatCompToAnimMap = NULL;
  const MR::CompToAnimChannelMap* const* sampledPosCompToAnimMaps = NULL;
  const MR::CompToAnimChannelMap* const* sampledQuatCompToAnimMaps = NULL;
  
  // Unchanging channels
  if (m_unchangingDataCompiled)
  {
    unchangingPosCompToAnimMap = m_unchangingDataCompiled->getUnchangingPosCompToAnimMap();
    NMP_VERIFY(unchangingPosCompToAnimMap);
    unchangingQuatCompToAnimMap = m_unchangingDataCompiled->getUnchangingQuatCompToAnimMap();
    NMP_VERIFY(unchangingQuatCompToAnimMap);
  }
  
  // Sampled channels
  if (m_sampledDataCompiled)
  {
    numFrameSections = m_sampledDataCompiled->getNumFrameSections();
    numChannelSections = m_sampledDataCompiled->getNumChannelSections();
    sampledPosNumQuantisationSets = m_sampledDataCompiled->getSampledPosNumQuantisationSets();
    sampledQuatNumQuantisationSets = m_sampledDataCompiled->getSampledQuatNumQuantisationSets();
    sectionStartFrames = m_sampledDataCompiled->getSectionStartFrames();
    NMP_VERIFY(sectionStartFrames);
    
    sampledPosCompToAnimMaps = m_sampledDataCompiled->getSampledPosCompToAnimMaps();
    NMP_VERIFY(sampledPosCompToAnimMaps);
    sampledQuatCompToAnimMaps = m_sampledDataCompiled->getSampledQuatCompToAnimMaps();
    NMP_VERIFY(sampledQuatCompToAnimMaps);
  }

  // Channel names
  const NMP::OrderedStringTable* channelNames = NULL;
  if (getAddChannelNamesTable())
  {
    channelNames = getChannelNamesTable();
  }

  // Compute the animation memory requirements
  NMP::Memory::Format memReqsAnimSource = AnimSourceNSABuilder::getMemoryRequirements(
    numFrameSections,
    numChannelSections,
    unchangingPosCompToAnimMap,
    unchangingQuatCompToAnimMap,
    sampledPosCompToAnimMaps,
    sampledQuatCompToAnimMaps,
    sampledPosNumQuantisationSets,
    sampledQuatNumQuantisationSets,
    sectionStartFrames,
    m_trajectorySource,
    channelNames);

  return memReqsAnimSource.size;
}

//----------------------------------------------------------------------------------------------------------------------
// Validation functions
void compareData(const void* srcA, const void* srcB, size_t size)
{
  const uint8_t* ptrA = (uint8_t*)srcA;
  const uint8_t* ptrB = (uint8_t*)srcB;
  size_t byteIndex;
  for (byteIndex = 0; byteIndex < size; ++byteIndex)
  {
    if(ptrA[byteIndex] != ptrB[byteIndex])
      break;
  }

  NMP_VERIFY(byteIndex == size);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorNSA::buildAnimation(NMP::Memory::Resource& resource) const
{
  const AnimSourceUncompressed* animSource = getInputAnimation(); // Resampled animation
  NMP_VERIFY(animSource);

  float duration = animSource->getDuration();
  float sampleFrequency = animSource->getSampleFrequency();
  uint32_t numChannelSets = animSource->getNumChannelSets();

  // Information
  uint32_t numFrameSections = 0;
  uint32_t numChannelSections = 0;
  uint32_t sampledPosNumQuantisationSets = 0;
  uint32_t sampledQuatNumQuantisationSets = 0;
  const uint32_t* sectionStartFrames = NULL;
  const MR::CompToAnimChannelMap* unchangingPosCompToAnimMap = NULL;
  const MR::CompToAnimChannelMap* unchangingQuatCompToAnimMap = NULL;
  const MR::CompToAnimChannelMap* const* sampledPosCompToAnimMaps = NULL;
  const MR::CompToAnimChannelMap* const* sampledQuatCompToAnimMaps = NULL;
  const MR::QuantisationScaleAndOffsetVec3* posMeansQuantisationInfo = NULL;
  const MR::QuantisationScaleAndOffsetVec3* sampledPosQuantisationInfo = NULL;
  const MR::QuantisationScaleAndOffsetVec3* sampledQuatQuantisationInfo = NULL;
  const MR::UnchangingDataNSA* unchangingData = NULL;
  const MR::SectionDataNSA** sectionDataTable = NULL;

  // Unchanging channels
  if (m_unchangingDataCompiled)
  {
    // Comp to anim maps
    unchangingPosCompToAnimMap = m_unchangingDataCompiled->getUnchangingPosCompToAnimMap();
    NMP_VERIFY(unchangingPosCompToAnimMap);
    unchangingQuatCompToAnimMap = m_unchangingDataCompiled->getUnchangingQuatCompToAnimMap();
    NMP_VERIFY(unchangingQuatCompToAnimMap);
    
    // Unchanging data
    unchangingData = m_unchangingDataCompiled->getUnchangingData();
    NMP_VERIFY(unchangingData);
  }

  // Sampled channels
  if (m_sampledDataCompiled)
  {
    numFrameSections = m_sampledDataCompiled->getNumFrameSections();
    numChannelSections = m_sampledDataCompiled->getNumChannelSections();
    sampledPosNumQuantisationSets = m_sampledDataCompiled->getSampledPosNumQuantisationSets();
    sampledQuatNumQuantisationSets = m_sampledDataCompiled->getSampledQuatNumQuantisationSets();

    sectionStartFrames = m_sampledDataCompiled->getSectionStartFrames();
    NMP_VERIFY(sectionStartFrames);
    
    posMeansQuantisationInfo = &m_sampledDataCompiled->getPosMeansQuantisationInfo();
    sampledPosQuantisationInfo = m_sampledDataCompiled->getSampledPosQuantisationInfo();
    sampledQuatQuantisationInfo = m_sampledDataCompiled->getSampledQuatQuantisationInfo();

    // Comp to anim maps
    sampledPosCompToAnimMaps = m_sampledDataCompiled->getSampledPosCompToAnimMaps();
    NMP_VERIFY(sampledPosCompToAnimMaps);
    sampledQuatCompToAnimMaps = m_sampledDataCompiled->getSampledQuatCompToAnimMaps();
    NMP_VERIFY(sampledQuatCompToAnimMaps);
    
    // Section data
    uint32_t numSectionEntries = numFrameSections * numChannelSections;
    NMP::Memory::Format memReqsPtrs(sizeof(const MR::SectionDataNSA*) * numSectionEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Resource memResPtrs = NMPMemoryAllocateFromFormat(memReqsPtrs);
    ZeroMemory(memResPtrs.ptr, memReqsPtrs.size);
    sectionDataTable = (const MR::SectionDataNSA**) memResPtrs.ptr;
    NMP_VERIFY(sectionDataTable);
    
    uint32_t indx = 0;
    for (uint32_t iFrameSection = 0; iFrameSection < numFrameSections; ++iFrameSection)
    {
      for (uint32_t iChanSection = 0; iChanSection < numChannelSections; ++iChanSection, ++indx)
      {
        const SubSectionNSACompiled* subSection = m_sampledDataCompiled->getSubSection(iFrameSection, iChanSection);
        NMP_VERIFY(subSection);
        const MR::SectionDataNSA* sectionData = subSection->getSectionData();
        NMP_VERIFY(sectionData);
        sectionDataTable[indx] = sectionData;
      } 
    }
  }

  // Channel names
  const NMP::OrderedStringTable* channelNames = NULL;
  if (getAddChannelNamesTable())
  {
    channelNames = getChannelNamesTable();
  }

  // Build the animation source
#ifdef TEST_LOCATE_DISLOCATE
  NMP::Memory::Format memReqsAnim = resource.format;
  MR::AnimSourceNSA* outputAnim =
#endif

  AnimSourceNSABuilder::init(
    resource,
    duration,
    sampleFrequency,
    numChannelSets,
    numFrameSections,
    numChannelSections,
    unchangingPosCompToAnimMap,
    unchangingQuatCompToAnimMap,
    sampledPosCompToAnimMaps,
    sampledQuatCompToAnimMaps,
    posMeansQuantisationInfo,
    sampledPosNumQuantisationSets,
    sampledQuatNumQuantisationSets,
    sampledPosQuantisationInfo,
    sampledQuatQuantisationInfo,
    unchangingData,
    sectionStartFrames,
    sectionDataTable,
    m_trajectorySource,
    channelNames);

  //-----------------------
  // Test locate, dislocate and the binary invariance of the compiled animation
#ifdef TEST_LOCATE_DISLOCATE
  // Make a copy of the animation source
  NMP::Memory::Resource memResAnimA = NMPMemoryAllocateFromFormat(memReqsAnim);
  NMP::Memory::memcpy(memResAnimA.ptr, outputAnim, memReqsAnim.size);
  MR::AnimSourceNSA* outputAnimA = (MR::AnimSourceNSA*)memResAnimA.ptr;

  // Build a duplicate animation in another memory resource
  NMP::Memory::Resource memResAnimB = NMPMemoryAllocateFromFormat(memReqsAnim);
  ZeroMemory(memResAnimB.ptr, memReqsAnim.size);

  // Note that we can't just relocate the source animation since the section data
  // is not relocated.
  MR::AnimSourceNSA* outputAnimB = AnimSourceNSABuilder::init(
    memResAnimB,
    duration,
    sampleFrequency,
    numChannelSets,
    numFrameSections,
    numChannelSections,
    unchangingPosCompToAnimMap,
    unchangingQuatCompToAnimMap,
    sampledPosCompToAnimMaps,
    sampledQuatCompToAnimMaps,
    posMeansQuantisationInfo,
    sampledPosNumQuantisationSets,
    sampledQuatNumQuantisationSets,
    sampledPosQuantisationInfo,
    sampledQuatQuantisationInfo,
    unchangingData,
    sectionStartFrames,
    sectionDataTable,
    m_trajectorySource,
    channelNames);
  NMP_VERIFY(memResAnimB.format.size == 0);

  // Dislocate the two built animations and compare
  outputAnim->dislocate();
  outputAnimB->dislocate();
  compareData(outputAnim, outputAnimB, memReqsAnim.size);

  // Locate the source animation and compare with the original
  outputAnim->locate();
  compareData(outputAnim, outputAnimA, memReqsAnim.size);

  // Tidy up
  NMP::Memory::memFree(outputAnimA);
  NMP::Memory::memFree(outputAnimB);
#endif

  //-----------------------
  // Tidy up
  if (sectionDataTable)
  {
    NMP::Memory::memFree(sectionDataTable);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorNSA::buildRigToAnimMaps()
{
  const std::vector<const MR::AnimRigDef*>& rigs = getRigs();
  const std::vector<const MR::RigToAnimEntryMap*>& rigToAnimEntryMaps = getRigToAnimEntryMaps();
  NMP_VERIFY(rigs.size() == rigToAnimEntryMaps.size());
  std::vector<MR::RigToAnimMap*>& rigToAnimMaps = AnimSourceCompressor::getRigToAnimMaps();
  NMP_VERIFY(rigToAnimMaps.empty());
  const MR::RigToAnimEntryMap* conglomerateRigToAnimEntryMap = getConglomerateRigToAnimEntryMap();
  NMP_VERIFY(conglomerateRigToAnimEntryMap);
  
  uint32_t numRigs = (uint32_t) rigs.size();
  for (uint32_t i = 0; i < numRigs; ++i)
  {
    const MR::AnimRigDef* rig = rigs[i];
    const MR::RigToAnimEntryMap* rigToAnimEntryMap = rigToAnimEntryMaps[i];
    MR::RigToAnimMap* rigToAnimMap = buildAnimToRigTableMap(rig, rigToAnimEntryMap, conglomerateRigToAnimEntryMap);
    rigToAnimMaps.push_back(rigToAnimMap);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Compression Functions
//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorNSA::compileTrajectoryData()
{
  const AnimSourceUncompressed* animSource = getInputAnimation(); // Resampled animation
  NMP_VERIFY(animSource);
  
  if (animSource->hasTrajectoryData())
  {
    TrajectorySourceCompressorNSA* trajectorySourceCompressor = new TrajectorySourceCompressorNSA();

    NMP::Memory::Resource memResTrajSource = trajectorySourceCompressor->compressTrajectory(
      animSource->getTrajectorySource(),
      getOptions(),
      getMessageLogger(),
      getErrorLogger());

    NMP_VERIFY(memResTrajSource.ptr);
    m_memReqsTrajectorySource = memResTrajSource.format;
    m_trajectorySource = (MR::TrajectorySourceNSA*) memResTrajSource.ptr;

    delete trajectorySourceCompressor;
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
