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
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMUniformQuantisation.h"
#include "assetProcessor/AnimSource/AnimSourceBuilderNSA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// UnchangingDataNSABuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format UnchangingDataNSABuilder::getMemoryRequirements(
  uint32_t unchangingPosNumChannels,
  uint32_t unchangingQuatNumChannels)
{
  // Header
  NMP::Memory::Format result(sizeof(MR::UnchangingDataNSA), NMP_NATURAL_TYPE_ALIGNMENT);

  // Pos
  if (unchangingPosNumChannels > 0)
  {    
    NMP::Memory::Format memReqsPos = MR::UnchangingDataNSA::getMemoryReqsUnchangingPosData(unchangingPosNumChannels);
    result += memReqsPos;
  }

  // Quat
  if (unchangingQuatNumChannels > 0)
  {
    NMP::Memory::Format memReqsQuat = MR::UnchangingDataNSA::getMemoryReqsUnchangingQuatData(unchangingQuatNumChannels);
    result += memReqsQuat;
  }

  // Multiple of the data alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::UnchangingDataNSA* UnchangingDataNSABuilder::init(
  NMP::Memory::Resource& resource,
  uint32_t unchangingPosNumChannels,
  uint32_t unchangingQuatNumChannels,
  const MR::QuantisationScaleAndOffsetVec3& unchangingPosQuantisationInfo,
  const MR::QuantisationScaleAndOffsetVec3& unchangingQuatQuantisationInfo,
  const MR::UnchangingKeyVec3* unchangingPosData,
  const MR::UnchangingKeyVec3* unchangingQuatData)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(MR::UnchangingDataNSA), NMP_NATURAL_TYPE_ALIGNMENT);
  UnchangingDataNSABuilder* result = (UnchangingDataNSABuilder*) resource.alignAndIncrement(memReqsHdr);
  NMP_VERIFY(result);
  result->m_unchangingPosNumChannels = unchangingPosNumChannels;
  result->m_unchangingQuatNumChannels = unchangingQuatNumChannels;
  result->m_unchangingPosData = NULL;
  result->m_unchangingQuatData = NULL;
  
  // Quantisation info
  result->m_unchangingPosQuantisationInfo = unchangingPosQuantisationInfo;
  result->m_unchangingQuatQuantisationInfo = unchangingQuatQuantisationInfo;
  
  // Pos
  if (unchangingPosNumChannels > 0)
  {
    NMP_VERIFY(unchangingPosData);
    NMP::Memory::Format memReqsPos = MR::UnchangingDataNSA::getMemoryReqsUnchangingPosData(unchangingPosNumChannels);
    result->m_unchangingPosData = (MR::UnchangingKeyVec3*) resource.alignAndIncrement(memReqsPos);
    NMP::Memory::memcpy(result->m_unchangingPosData, unchangingPosData, memReqsPos.size);
  }
  
  // Quat
  if (unchangingQuatNumChannels > 0)
  {
    NMP_VERIFY(unchangingQuatData);
    NMP::Memory::Format memReqsQuat = MR::UnchangingDataNSA::getMemoryReqsUnchangingQuatData(unchangingQuatNumChannels);
    result->m_unchangingQuatData = (MR::UnchangingKeyVec3*) resource.alignAndIncrement(memReqsQuat);
    NMP::Memory::memcpy(result->m_unchangingQuatData, unchangingQuatData, memReqsQuat.size);
  }
  
  // Multiple of the data alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  
  return (MR::UnchangingDataNSA*)result;
}  

//----------------------------------------------------------------------------------------------------------------------
// SectionDataNSABuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SectionDataNSABuilder::getMemoryRequirements(
  uint32_t numSectionAnimFrames,
  uint32_t sampledPosNumChannels,
  uint32_t sampledQuatNumChannels)
{
  NMP_VERIFY(numSectionAnimFrames >= 2);

  // Header
  NMP::Memory::Format result(sizeof(MR::SectionDataNSA), NMP_VECTOR_ALIGNMENT);
  
  // Pos
  if (sampledPosNumChannels > 0)
  {    
    // Sampled pos quantised data (Not block-4 aligned)
    NMP::Memory::Format memReqsPos;
    size_t keyFrameDataStride;
    MR::SectionDataNSA::getMemoryReqsSampledPosData(
      numSectionAnimFrames,
      sampledPosNumChannels,
      memReqsPos.alignment,
      keyFrameDataStride,
      memReqsPos.size);
    result += memReqsPos;
    
    // Sampled pos quantisation mean and qSet.
    NMP::Memory::Format memReqsMeanAndSetVec3 = MR::SectionDataNSA::getMemoryReqsMeanAndSetVec3(sampledPosNumChannels);
    result += memReqsMeanAndSetVec3;
  }
  
  // Quat
  if (sampledQuatNumChannels > 0)
  {    
    // Sampled quat quantised data (Not block-4 aligned)
    NMP::Memory::Format memReqsQuat;
    size_t keyFrameDataStride;
    MR::SectionDataNSA::getMemoryReqsSampledQuatData(
      numSectionAnimFrames,
      sampledQuatNumChannels,
      memReqsQuat.alignment,
      keyFrameDataStride,
      memReqsQuat.size);
    result += memReqsQuat;
  
    // Sampled quat quantisation mean and qSet.
    NMP::Memory::Format memReqsMeanAndSetVec3 = MR::SectionDataNSA::getMemoryReqsMeanAndSetVec3(sampledQuatNumChannels);
    result += memReqsMeanAndSetVec3;
  }

  // Multiple of the DMA alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SectionDataNSA* SectionDataNSABuilder::init(
  NMP::Memory::Resource& resource,
  uint32_t numSectionAnimFrames,
  uint32_t sampledPosNumChannels,
  uint32_t sampledQuatNumChannels,
  const MR::QuantisationMeanAndSetVec3* sampledPosQuantisationData,
  const MR::SampledPosKey* sampledPosData,
  const MR::QuantisationMeanAndSetVec3* sampledQuatQuantisationData,
  const MR::SampledQuatKeyTQA* sampledQuatData)
{
  NMP_VERIFY(numSectionAnimFrames >= 2);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(MR::SectionDataNSA), NMP_VECTOR_ALIGNMENT);
  SectionDataNSABuilder* result = (SectionDataNSABuilder*) resource.alignAndIncrement(memReqsHdr);
  NMP_VERIFY(result);

  result->m_numSectionAnimFrames = numSectionAnimFrames;
  result->m_sampledPosNumChannels = sampledPosNumChannels;
  result->m_sampledQuatNumChannels = sampledQuatNumChannels;
  result->m_sampledPosQuantisationData = NULL;
  result->m_sampledPosData = NULL;
  result->m_sampledQuatQuantisationData = NULL;
  result->m_sampledQuatData = NULL;

  // Pos
  if (sampledPosNumChannels > 0)
  {    
    // Sampled pos quantised data (Not block-4 aligned)
    NMP::Memory::Format memReqsPos;
    size_t keyFrameDataStride;
    MR::SectionDataNSA::getMemoryReqsSampledPosData(
      numSectionAnimFrames,
      sampledPosNumChannels,
      memReqsPos.alignment,
      keyFrameDataStride,
      memReqsPos.size);
    result->m_sampledPosData = (MR::SampledPosKey*) resource.alignAndIncrement(memReqsPos);
    
    NMP_VERIFY(sampledPosData);
    NMP::Memory::memcpy(
      result->m_sampledPosData,
      sampledPosData,
      memReqsPos.size);

    // Sampled pos quantisation mean and qSet.
    NMP::Memory::Format memReqsMeanAndSetVec3 = MR::SectionDataNSA::getMemoryReqsMeanAndSetVec3(sampledPosNumChannels);
    result->m_sampledPosQuantisationData = (MR::QuantisationMeanAndSetVec3*) resource.alignAndIncrement(memReqsMeanAndSetVec3);

    NMP_VERIFY(sampledPosQuantisationData);
    NMP::Memory::memcpy(
      result->m_sampledPosQuantisationData,
      sampledPosQuantisationData,
      memReqsMeanAndSetVec3.size);
  }
  
  // Quat
  if (sampledQuatNumChannels > 0)
  {    
    // Sampled quat quantised data (Not block-4 aligned)
    NMP::Memory::Format memReqsQuat;
    size_t keyFrameDataStride;
    MR::SectionDataNSA::getMemoryReqsSampledQuatData(
      numSectionAnimFrames,
      sampledQuatNumChannels,
      memReqsQuat.alignment,
      keyFrameDataStride,
      memReqsQuat.size);
    result->m_sampledQuatData = (MR::SampledQuatKeyTQA*) resource.alignAndIncrement(memReqsQuat);
    
    NMP_VERIFY(sampledQuatData);
    NMP::Memory::memcpy(
      result->m_sampledQuatData,
      sampledQuatData,
      memReqsQuat.size);

    // Sampled quat quantisation mean and qSet.
    NMP::Memory::Format memReqsMeanAndSetVec3 = MR::SectionDataNSA::getMemoryReqsMeanAndSetVec3(sampledQuatNumChannels);
    result->m_sampledQuatQuantisationData = (MR::QuantisationMeanAndSetVec3*) resource.alignAndIncrement(memReqsMeanAndSetVec3);

    NMP_VERIFY(sampledQuatQuantisationData);
    NMP::Memory::memcpy(
      result->m_sampledQuatQuantisationData,
      sampledQuatQuantisationData,
      memReqsMeanAndSetVec3.size);
  }

  // Multiple of the DMA alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return (MR::SectionDataNSA*)result;
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceNSABuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimSourceNSABuilder::getMemoryRequirements(
  uint32_t numFrameSections,
  uint32_t numChannelSections,
  const MR::CompToAnimChannelMap* unchangingPosCompToAnimMap,
  const MR::CompToAnimChannelMap* unchangingQuatCompToAnimMap,
  const MR::CompToAnimChannelMap* const* sampledPosCompToAnimMaps,
  const MR::CompToAnimChannelMap* const* sampledQuatCompToAnimMaps,
  uint32_t sampledPosNumQuantisationSets,
  uint32_t sampledQuatNumQuantisationSets,
  const uint32_t* sectionStartFrames,
  const MR::TrajectorySourceNSA* trajectorySource, // Optional
  const NMP::OrderedStringTable* channelNames) // Optional
{
  NMP_VERIFY(unchangingPosCompToAnimMap);
  NMP_VERIFY(unchangingQuatCompToAnimMap);
  uint32_t numSectionEntries = numFrameSections * numChannelSections;

  //-----------------------
  // Header
  NMP::Memory::Format result(sizeof(MR::AnimSourceNSA), NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Unchanging comp to anim channel maps
  NMP::Memory::Format memReqsUnchangingPosCompToAnimMap = unchangingPosCompToAnimMap->getInstanceMemoryRequirements();
  result += memReqsUnchangingPosCompToAnimMap;

  NMP::Memory::Format memReqsUnchangingQuatCompToAnimMap = unchangingQuatCompToAnimMap->getInstanceMemoryRequirements();
  result += memReqsUnchangingQuatCompToAnimMap;

  if (numSectionEntries > 0)
  {
    NMP_VERIFY(sampledPosCompToAnimMaps);
    NMP_VERIFY(sampledQuatCompToAnimMaps);
    NMP_VERIFY(sectionStartFrames);

    //-----------------------
    // Sampled comp to anim channel maps

    // Pointers table to sub-section CAMs
    NMP::Memory::Format memReqsCAMTable(sizeof(MR::CompToAnimChannelMap*) * numChannelSections, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsCAMTable; // pos
    result += memReqsCAMTable; // quat

    // Sampled pos sub-section CAMs
    for (uint32_t iChanSection = 0; iChanSection < numChannelSections; ++iChanSection)
    {
      const MR::CompToAnimChannelMap* compToAnimMap = sampledPosCompToAnimMaps[iChanSection];
      NMP_VERIFY(compToAnimMap);
      NMP::Memory::Format memReqsSubSectionCompToAnimMap = compToAnimMap->getInstanceMemoryRequirements();
      result += memReqsSubSectionCompToAnimMap;
    }

    // Sampled quat sub-section CAMs
    for (uint32_t iChanSection = 0; iChanSection < numChannelSections; ++iChanSection)
    {
      const MR::CompToAnimChannelMap* compToAnimMap = sampledQuatCompToAnimMaps[iChanSection];
      NMP_VERIFY(compToAnimMap);
      NMP::Memory::Format memReqsSubSectionCompToAnimMap = compToAnimMap->getInstanceMemoryRequirements();
      result += memReqsSubSectionCompToAnimMap;
    }

    //-----------------------
    // Quantisation scale and offset information (Common to all sections)
    NMP::Memory::Format memReqsPosQuantInfo(
      sizeof(MR::QuantisationScaleAndOffsetVec3) * sampledPosNumQuantisationSets,
      NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsPosQuantInfo;

    NMP::Memory::Format memReqsQuatQuantInfo(
      sizeof(MR::QuantisationScaleAndOffsetVec3) * sampledQuatNumQuantisationSets,
      NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsQuatQuantInfo;

    //-----------------------
    // Sections information (i.e. start frame and section size)
    NMP::Memory::Format memReqsSectionsFrameInfo(sizeof(uint32_t) * (numFrameSections + 1), NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsSectionsFrameInfo;
    NMP::Memory::Format memReqsSectionsSizeInfo(sizeof(uint32_t) * numSectionEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsSectionsSizeInfo;

    //-----------------------
    // Sections DataRef table (pointers to sections)
    NMP::Memory::Format memReqsSectionDataRefs(sizeof(MR::DataRef) * numSectionEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsSectionDataRefs;
  }

  //-----------------------
  // Unchanging channel set data
  NMP::Memory::Format memReqUnchangingData = UnchangingDataNSABuilder::getMemoryRequirements(
    unchangingPosCompToAnimMap->getNumChannels(),
    unchangingQuatCompToAnimMap->getNumChannels());
  result += memReqUnchangingData;

  // Alignment to DMA boundary for the animation excluding the section data
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Sections
  for (uint32_t iFrameSection = 0; iFrameSection < numFrameSections; ++iFrameSection)
  {
    NMP_VERIFY(sectionStartFrames);
    uint32_t numSectionAnimFrames = sectionStartFrames[iFrameSection+1] - sectionStartFrames[iFrameSection] + 1;
  
    for (uint32_t iChanSection = 0; iChanSection < numChannelSections; ++iChanSection)
    {
      const MR::CompToAnimChannelMap* compToAnimMapPos = sampledPosCompToAnimMaps[iChanSection];
      NMP_VERIFY(compToAnimMapPos);
      const MR::CompToAnimChannelMap* compToAnimMapQuat = sampledQuatCompToAnimMaps[iChanSection];
      NMP_VERIFY(compToAnimMapQuat);
    
      NMP::Memory::Format memReqsSection = SectionDataNSABuilder::getMemoryRequirements(
        numSectionAnimFrames,
        compToAnimMapPos->getNumChannels(),
        compToAnimMapQuat->getNumChannels());
      result += memReqsSection;
    }
  }
  
  // We have to make sure that we have enough padding for any trailing keyframes to
  // prevent reading beyond the allocated animation memory. There are at most 3
  // keyframes that need to be padded, each with XYZ components of uint16_t. Note that
  // each XYZ component is loaded using unpacku4i16, unpacku4i16f. i.e. 4 uint16_t's
  // so we need at most 20 bytes of padding.
  const size_t keyframePadding = 20;
  size_t sizeAfterLastSection = result.size;

  //-----------------------
  // Trajectory
  if (trajectorySource)
  {
    result += trajectorySource->getInstanceMemoryRequirements();
  }

  //-----------------------
  // Channel names table
  if (channelNames)
  {
    NMP::Memory::Format memReqsChanNames = channelNames->getInstanceMemoryRequirements();
    result += memReqsChanNames;
  }

  // Multiple of the DMA alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  // Add any required padding
  if ((result.size - sizeAfterLastSection) < keyframePadding)
  {
    result.size = sizeAfterLastSection + keyframePadding;
    result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AnimSourceNSA* AnimSourceNSABuilder::init(
  NMP::Memory::Resource& resource,
  float duration,
  float sampleFrequency,
  uint32_t numChannelSets,
  uint32_t numFrameSections,
  uint32_t numChannelSections,
  const MR::CompToAnimChannelMap* unchangingPosCompToAnimMap,
  const MR::CompToAnimChannelMap* unchangingQuatCompToAnimMap,
  const MR::CompToAnimChannelMap* const* sampledPosCompToAnimMaps,
  const MR::CompToAnimChannelMap* const* sampledQuatCompToAnimMaps,
  const MR::QuantisationScaleAndOffsetVec3* posMeansQuantisationInfo,
  uint32_t sampledPosNumQuantisationSets,
  uint32_t sampledQuatNumQuantisationSets,
  const MR::QuantisationScaleAndOffsetVec3* sampledPosQuantisationInfo,
  const MR::QuantisationScaleAndOffsetVec3* sampledQuatQuantisationInfo,
  const MR::UnchangingDataNSA* unchangingData,
  const uint32_t* sectionStartFrames,
  const MR::SectionDataNSA* const* sectionDataTable,
  const MR::TrajectorySourceNSA* trajectorySource, // Optional
  const NMP::OrderedStringTable* channelNames) // Optional
{
  NMP_VERIFY(unchangingPosCompToAnimMap);
  NMP_VERIFY(unchangingQuatCompToAnimMap);
  uint32_t numSectionEntries = numFrameSections * numChannelSections;

  //-----------------------
  // Header
  NMP::Memory::Format memResHdr(sizeof(MR::AnimSourceNSA), NMP_VECTOR_ALIGNMENT);
  AnimSourceNSABuilder* sourceBuilder = (AnimSourceNSABuilder*) resource.alignAndIncrement(memResHdr);
  ZeroMemory(sourceBuilder, memResHdr.size);

  // Information that can be set now
  sourceBuilder->m_animType = ANIM_TYPE_NSA;
  sourceBuilder->m_funcTable = &MR::AnimSourceNSA::m_functionTable;
  sourceBuilder->m_isLocated = true;
  sourceBuilder->m_duration = duration;
  sourceBuilder->m_sampleFrequency = sampleFrequency;
  sourceBuilder->m_numChannelSets = numChannelSets;
  sourceBuilder->m_numFrameSections = numFrameSections;
  sourceBuilder->m_numChannelSections = numChannelSections;
  sourceBuilder->m_sampledPosNumQuantisationSets = sampledPosNumQuantisationSets;
  sourceBuilder->m_sampledQuatNumQuantisationSets = sampledQuatNumQuantisationSets;

  //-----------------------
  // Unchanging comp to anim channel maps
  NMP::Memory::Format memReqsUnchangingPosCompToAnimMap = unchangingPosCompToAnimMap->getInstanceMemoryRequirements();
  MR::CompToAnimChannelMap* dstUnchangingPosCompToAnimMap =
    (MR::CompToAnimChannelMap*) resource.alignAndIncrement(memReqsUnchangingPosCompToAnimMap);
  sourceBuilder->m_unchangingPosCompToAnimMap = dstUnchangingPosCompToAnimMap;
  NMP::Memory::memcpy(
    dstUnchangingPosCompToAnimMap,
    unchangingPosCompToAnimMap,
    memReqsUnchangingPosCompToAnimMap.size);

  // Quat
  NMP::Memory::Format memReqsUnchangingQuatCompToAnimMap = unchangingQuatCompToAnimMap->getInstanceMemoryRequirements();
  MR::CompToAnimChannelMap* dstUnchangingQuatCompToAnimMap =
    (MR::CompToAnimChannelMap*) resource.alignAndIncrement(memReqsUnchangingQuatCompToAnimMap);
  sourceBuilder->m_unchangingQuatCompToAnimMap = dstUnchangingQuatCompToAnimMap;
  NMP::Memory::memcpy(
    dstUnchangingQuatCompToAnimMap,
    unchangingQuatCompToAnimMap,
    memReqsUnchangingQuatCompToAnimMap.size);

  // Update the maximum number of compression channels
  sourceBuilder->m_maxNumCompChannels = NMP::maximum(
    sourceBuilder->m_maxNumCompChannels,
    unchangingPosCompToAnimMap->getNumChannels());

  sourceBuilder->m_maxNumCompChannels = NMP::maximum(
    sourceBuilder->m_maxNumCompChannels,
    unchangingQuatCompToAnimMap->getNumChannels());

  //-----------------------
  sourceBuilder->m_posMeansQuantisationInfo.zero();
  if (numSectionEntries > 0)
  {
    NMP_VERIFY(sampledPosCompToAnimMaps);
    NMP_VERIFY(sampledQuatCompToAnimMaps);
    NMP_VERIFY(posMeansQuantisationInfo);    
    NMP_VERIFY(unchangingData);
    NMP_VERIFY(sectionStartFrames);
    NMP_VERIFY(sectionDataTable);

    // Pointers table to sub-section CAMs
    NMP::Memory::Format memReqsCAMTable(sizeof(MR::CompToAnimChannelMap*) * numChannelSections, NMP_NATURAL_TYPE_ALIGNMENT);
    sourceBuilder->m_sampledPosCompToAnimMaps = (MR::CompToAnimChannelMap**) resource.alignAndIncrement(memReqsCAMTable);
    sourceBuilder->m_sampledQuatCompToAnimMaps = (MR::CompToAnimChannelMap**) resource.alignAndIncrement(memReqsCAMTable);

    // Sampled pos sub-section CAMs
    for (uint32_t iChanSection = 0; iChanSection < numChannelSections; ++iChanSection)
    {
      const MR::CompToAnimChannelMap* compToAnimMap = sampledPosCompToAnimMaps[iChanSection];
      NMP_VERIFY(compToAnimMap);
      NMP::Memory::Format memReqsSubSectionCompToAnimMap = compToAnimMap->getInstanceMemoryRequirements();

      MR::CompToAnimChannelMap* dstCompToAnimMap = (MR::CompToAnimChannelMap*) resource.alignAndIncrement(memReqsSubSectionCompToAnimMap);
      sourceBuilder->m_sampledPosCompToAnimMaps[iChanSection] = dstCompToAnimMap;
      NMP::Memory::memcpy(
        dstCompToAnimMap,
        compToAnimMap,
        memReqsSubSectionCompToAnimMap.size);
    }

    // Sampled quat sub-section CAMs
    for (uint32_t iChanSection = 0; iChanSection < numChannelSections; ++iChanSection)
    {
      const MR::CompToAnimChannelMap* compToAnimMap = sampledQuatCompToAnimMaps[iChanSection];
      NMP_VERIFY(compToAnimMap);
      NMP::Memory::Format memReqsSubSectionCompToAnimMap = compToAnimMap->getInstanceMemoryRequirements();

      MR::CompToAnimChannelMap* dstCompToAnimMap = (MR::CompToAnimChannelMap*) resource.alignAndIncrement(memReqsSubSectionCompToAnimMap);
      sourceBuilder->m_sampledQuatCompToAnimMaps[iChanSection] = dstCompToAnimMap;
      NMP::Memory::memcpy(
        dstCompToAnimMap,
        compToAnimMap,
        memReqsSubSectionCompToAnimMap.size);
    }

    //-----------------------
    // Update the maximum number of compression channels
    for (uint32_t iChanSection = 0; iChanSection < numChannelSections; ++iChanSection)
    {
      const MR::CompToAnimChannelMap* compToAnimMapPos = sampledPosCompToAnimMaps[iChanSection];
      NMP_VERIFY(compToAnimMapPos);
      sourceBuilder->m_maxNumCompChannels = NMP::maximum(
        sourceBuilder->m_maxNumCompChannels,
        compToAnimMapPos->getNumChannels());

      const MR::CompToAnimChannelMap* compToAnimMapQuat = sampledQuatCompToAnimMaps[iChanSection];
      NMP_VERIFY(compToAnimMapQuat);
      sourceBuilder->m_maxNumCompChannels = NMP::maximum(
        sourceBuilder->m_maxNumCompChannels,
        compToAnimMapQuat->getNumChannels());
    }

    //-----------------------
    // Quantisation scale and offset information (Common to all sections)
    sourceBuilder->m_posMeansQuantisationInfo = *posMeansQuantisationInfo;

    if (sampledPosNumQuantisationSets > 0)
    {
      NMP_VERIFY(sampledPosQuantisationInfo);
      NMP::Memory::Format memReqsPosQuantInfo(
        sizeof(MR::QuantisationScaleAndOffsetVec3) * sampledPosNumQuantisationSets,
        NMP_NATURAL_TYPE_ALIGNMENT);
      sourceBuilder->m_sampledPosQuantisationInfo =
        (MR::QuantisationScaleAndOffsetVec3*) resource.alignAndIncrement(memReqsPosQuantInfo);
      NMP::Memory::memcpy(
        sourceBuilder->m_sampledPosQuantisationInfo,
        sampledPosQuantisationInfo,
        memReqsPosQuantInfo.size);
    }
    
    if (sampledQuatNumQuantisationSets > 0)
    {
      NMP_VERIFY(sampledQuatQuantisationInfo);
      NMP::Memory::Format memReqsQuatQuantInfo(
        sizeof(MR::QuantisationScaleAndOffsetVec3) * sampledQuatNumQuantisationSets,
        NMP_NATURAL_TYPE_ALIGNMENT);
      sourceBuilder->m_sampledQuatQuantisationInfo =
        (MR::QuantisationScaleAndOffsetVec3*) resource.alignAndIncrement(memReqsQuatQuantInfo);
      NMP::Memory::memcpy(
        sourceBuilder->m_sampledQuatQuantisationInfo,
        sampledQuatQuantisationInfo,
        memReqsQuatQuantInfo.size);
    }

    //-----------------------
    // Sections information (i.e. start frame and section size)
    NMP::Memory::Format memReqsSectionsFrameInfo(sizeof(uint32_t) * (numFrameSections + 1), NMP_NATURAL_TYPE_ALIGNMENT);
    sourceBuilder->m_sectionStartFrames = (uint32_t*) resource.alignAndIncrement(memReqsSectionsFrameInfo);
    NMP::Memory::memcpy(
      sourceBuilder->m_sectionStartFrames,
      sectionStartFrames,
      memReqsSectionsFrameInfo.size);

    NMP::Memory::Format memReqsSectionsSizeInfo(sizeof(uint32_t) * numSectionEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    sourceBuilder->m_sectionSizes = (uint32_t*) resource.alignAndIncrement(memReqsSectionsSizeInfo);

    //-----------------------
    // Sections DataRef table (pointers to sections)
    NMP::Memory::Format memReqsSectionDataRefs(sizeof(MR::DataRef) * numSectionEntries, NMP_NATURAL_TYPE_ALIGNMENT);
    sourceBuilder->m_sectionData = (MR::DataRef*) resource.alignAndIncrement(memReqsSectionDataRefs);
  }

  //-----------------------
  // Unchanging channel set data
  NMP::Memory::Format memReqUnchangingData = UnchangingDataNSABuilder::getMemoryRequirements(
    unchangingPosCompToAnimMap->getNumChannels(),
    unchangingQuatCompToAnimMap->getNumChannels());
  MR::UnchangingDataNSA* dstUnchangingData = (MR::UnchangingDataNSA*) resource.alignAndIncrement(memReqUnchangingData);
  sourceBuilder->m_unchangingData = dstUnchangingData;
  NMP::Memory::memcpy(
    dstUnchangingData,
    unchangingData,
    memReqUnchangingData.size);
  dstUnchangingData->relocate();

  //-----------------------
  // Store the memory requirements of the animation class minus any section data.
  resource.align(NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Format memReqsAnimInstance((size_t)resource.ptr - (size_t)sourceBuilder, NMP_VECTOR_ALIGNMENT);
  sourceBuilder->m_animInstanceMemReqs = memReqsAnimInstance;

  //-----------------------
  // Sections
  uint32_t indx = 0;
  for (uint32_t iFrameSection = 0; iFrameSection < numFrameSections; ++iFrameSection)
  {
    NMP_VERIFY(sectionStartFrames);
    uint32_t numSectionAnimFrames = sectionStartFrames[iFrameSection+1] - sectionStartFrames[iFrameSection] + 1;

    for (uint32_t iChanSection = 0; iChanSection < numChannelSections; ++iChanSection, ++indx)
    {
      const MR::CompToAnimChannelMap* compToAnimMapPos = sampledPosCompToAnimMaps[iChanSection];
      NMP_VERIFY(compToAnimMapPos);
      const MR::CompToAnimChannelMap* compToAnimMapQuat = sampledQuatCompToAnimMaps[iChanSection];
      NMP_VERIFY(compToAnimMapQuat);

      const MR::SectionDataNSA* sectionData = sectionDataTable[indx];

      NMP::Memory::Format memReqsSection = SectionDataNSABuilder::getMemoryRequirements(
        numSectionAnimFrames,
        compToAnimMapPos->getNumChannels(),
        compToAnimMapQuat->getNumChannels());

      MR::SectionDataNSA* dstSectionData = (MR::SectionDataNSA*) resource.alignAndIncrement(memReqsSection);
      sourceBuilder->m_sectionData[indx].m_data = (void*) dstSectionData;
      NMP::Memory::memcpy(
        dstSectionData,
        sectionData,
        memReqsSection.size);
      dstSectionData->relocate();

      sourceBuilder->m_sectionSizes[indx] = (uint32_t)memReqsSection.size;
    }
  }

  //-----------------------
  // Work out the maximum section size
  uint32_t maxSectionSize = 0;
  for (uint32_t i = 0; i < numSectionEntries; ++i)
  {
    maxSectionSize = NMP::maximum(maxSectionSize, sourceBuilder->m_sectionSizes[i]);
  }
  sourceBuilder->m_maxSectionSize = maxSectionSize;
  
  // We have to make sure that we have enough padding for any trailing keyframes to
  // prevent reading beyond the allocated animation memory. There are at most 3
  // keyframes that need to be padded, each with XYZ components of uint16_t. Note that
  // each XYZ component is loaded using unpacku4i16, unpacku4i16f. i.e. 4 uint16_t's
  // so we need at most 20 bytes of padding.
  const size_t keyframePadding = 20;
  void* ptrWithPadding = NMP::Memory::increment(resource.ptr, keyframePadding);

  //-----------------------
  // Trajectory
  if (trajectorySource)
  {
    NMP::Memory::Format memReqsTraj = trajectorySource->getInstanceMemoryRequirements();
    MR::TrajectorySourceNSA* dstTrajectorySource = (MR::TrajectorySourceNSA*) resource.alignAndIncrement(memReqsTraj);
    sourceBuilder->m_trajectoryData.m_data = (void*)dstTrajectorySource;

    // Copy and relocate the data
    NMP::Memory::memcpy(dstTrajectorySource, trajectorySource, memReqsTraj.size);
    dstTrajectorySource->relocate();
  }

  //-----------------------
  // Channel names table
  if (channelNames)
  {
    NMP::Memory::Format memReqsChanNames = channelNames->getInstanceMemoryRequirements();
    NMP::OrderedStringTable* dstNamesTable = (NMP::OrderedStringTable*) resource.alignAndIncrement(memReqsChanNames);
    sourceBuilder->m_channelNames = dstNamesTable;
    
    // Copy and relocate the data
    NMP::Memory::memcpy(dstNamesTable, channelNames, memReqsChanNames.size);
    dstNamesTable->relocate();
  }

  // Multiple of the DMA alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  // Add any required padding
  if ((size_t)ptrWithPadding > (size_t)resource.ptr)
  {
    resource.increment((size_t)ptrWithPadding - (size_t)resource.ptr);
    resource.align(NMP_VECTOR_ALIGNMENT);
  }

  return (MR::AnimSourceNSA*)sourceBuilder;
}

//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceNSABuilder
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TrajectorySourceNSABuilder::getMemoryRequirements(
  uint32_t numAnimFrames,
  bool isDeltaPosChanging,
  bool isDeltaQuatChanging)
{
  NMP_VERIFY(numAnimFrames > 0);

  // Header
  NMP::Memory::Format result(sizeof(MR::TrajectorySourceNSA), NMP_VECTOR_ALIGNMENT);

  // Pos
  if (isDeltaPosChanging)
  {
    NMP::Memory::Format memReqsPos(sizeof(MR::SampledPosKey) * numAnimFrames, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsPos;
  }

  // Quat
  if (isDeltaQuatChanging)
  {
    uint32_t numAnimFramesPadded = numAnimFrames + 1; // Allocate an extra frame to pad SIMD optimizations
    NMP::Memory::Format memReqsQuat(sizeof(MR::SampledQuatKeyTQA) * numAnimFramesPadded, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsQuat;
  }

  // Multiple of the DMA alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::TrajectorySourceNSA* TrajectorySourceNSABuilder::init(
  NMP::Memory::Resource& resource,
  uint32_t numAnimFrames,
  float sampleFrequency,
  const MR::QuantisationScaleAndOffsetVec3& sampledDeltaPosKeysInfo,
  const MR::QuantisationScaleAndOffsetVec3& sampledDeltaQuatKeysInfo,
  const MR::SampledPosKey* sampledDeltaPosKeys,
  const MR::SampledQuatKeyTQA* sampledDeltaQuatKeys)
{
  NMP_VERIFY(numAnimFrames > 0);
  NMP_VERIFY(sampleFrequency > 0.0f);

  // Header
  NMP::Memory::Format memReqs(sizeof(MR::TrajectorySourceNSA), NMP_VECTOR_ALIGNMENT);
  TrajectorySourceNSABuilder* sourceBuilder = (TrajectorySourceNSABuilder*) resource.alignAndIncrement(memReqs);

  sourceBuilder->m_sampleFrequency = sampleFrequency;
  sourceBuilder->m_numAnimFrames = numAnimFrames;
  sourceBuilder->m_sampledDeltaPosKeysInfo = sampledDeltaPosKeysInfo;
  sourceBuilder->m_sampledDeltaQuatKeysInfo = sampledDeltaQuatKeysInfo;

  sourceBuilder->m_trajType = TRAJ_TYPE_NSA;
  sourceBuilder->m_getTrajAtTime = MR::TrajectorySourceNSA::computeTrajectoryTransformAtTime;

  // Pos
  if (sampledDeltaPosKeys != NULL)
  {
    NMP::Memory::Format memReqsPos(sizeof(MR::SampledPosKey) * numAnimFrames, NMP_NATURAL_TYPE_ALIGNMENT);
    sourceBuilder->m_sampledDeltaPosKeys = (MR::SampledPosKey*) resource.alignAndIncrement(memReqsPos);
    for (uint32_t i = 0; i < numAnimFrames; ++i)
    {
      sourceBuilder->m_sampledDeltaPosKeys[i] = sampledDeltaPosKeys[i];
    }

    memReqs += memReqsPos;
  }
  else
  {
    sourceBuilder->m_sampledDeltaPosKeys = NULL;
  }

  // Quat
  if (sampledDeltaQuatKeys != NULL)
  {
    uint32_t numAnimFramesPadded = numAnimFrames + 1; // Allocate an extra frame to pad SIMD optimizations
    NMP::Memory::Format memReqsQuat(sizeof(MR::SampledQuatKeyTQA) * numAnimFramesPadded, NMP_NATURAL_TYPE_ALIGNMENT);
    sourceBuilder->m_sampledDeltaQuatKeys = (MR::SampledQuatKeyTQA*) resource.alignAndIncrement(memReqsQuat);
    for (uint32_t i = 0; i < numAnimFrames; ++i)
    {
      sourceBuilder->m_sampledDeltaQuatKeys[i] = sampledDeltaQuatKeys[i];
    }

    // Initialize SIMD padding to safe values
    sourceBuilder->m_sampledDeltaQuatKeys[numAnimFrames].zero();
    memReqs += memReqsQuat;
  }
  else
  {
    sourceBuilder->m_sampledDeltaQuatKeys = NULL;
  }

  // Multiple of the DMA alignment
  resource.align(NMP_VECTOR_ALIGNMENT);
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  // Set the instance memory requirements
  sourceBuilder->m_trajectoryInstanceMemReqs = memReqs;

  return (MR::TrajectorySourceNSA*)sourceBuilder;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
