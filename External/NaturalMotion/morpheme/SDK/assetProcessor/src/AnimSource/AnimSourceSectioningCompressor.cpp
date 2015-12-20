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

#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrDefines.h"
#include "assetProcessor/AnimSource/AnimSourceBuilderUtils.h"
#include "assetProcessor/AnimSource/AnimSourceSectioningCompressor.h"
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
// AnimSourceSectioningCompressor
//----------------------------------------------------------------------------------------------------------------------
AnimSourceSectioningCompressor::AnimSourceSectioningCompressor() :
  m_memReqsAnimSourceHdr(0, NMP_VECTOR_ALIGNMENT),
  m_memReqsChannelSetInfo(0, NMP_VECTOR_ALIGNMENT),
  m_memReqsTrajectorySource(0, NMP_VECTOR_ALIGNMENT),
  m_memReqsAdditionalData(0, NMP_VECTOR_ALIGNMENT),
  m_sectionsByteOffset(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
AnimSourceSectioningCompressor::~AnimSourceSectioningCompressor()
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource AnimSourceSectioningCompressor::compressAnimation(
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
  NMP::Memory::Resource buffer;
  buffer.ptr = NULL;

  //-------------------
  // Initialise the options
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

  // Wipe down the section requirements
  m_startFrames.clear();
  m_endFrames.clear();
  m_sectionSizes.clear();

  //-------------------
  // Initialise the compressor
  initCompressor();

  //-------------------
  // Compute the memory requirements for the channel names table
  m_memReqsChannelNamesTable.size = computeChannelNamesTableRequirements();
  m_memReqsChannelNamesTable.size = NMP::Memory::align(m_memReqsChannelNamesTable.size, NMP_VECTOR_ALIGNMENT); // Ensure DMA alignment

  //-------------------
  // Compute the memory requirements for the trajectory source. Note you must check to see if
  // the uncompressed animation has a trajectory source.
  m_memReqsTrajectorySource.size = computeTrajectoryRequirements();
  m_memReqsTrajectorySource.size = NMP::Memory::align(m_memReqsTrajectorySource.size, NMP_VECTOR_ALIGNMENT); // Ensure DMA alignment

  //-------------------
  // Build the section requirements information
  onBeginComputeSectionRequirements();
  if (!computeSectionRequirements())
  {
    termCompressor();
    termOptions();
    return buffer;
  }
  onEndComputeSectionRequirements();

  //-------------------
  // Compute the memory requirements for the channel set information
  m_memReqsChannelSetInfo.size = computeChannelSetInfoRequirements();
  m_memReqsChannelSetInfo.size = NMP::Memory::align(m_memReqsChannelSetInfo.size, NMP_VECTOR_ALIGNMENT); // Ensure DMA alignment

  //-------------------
  // Compute the memory requirements for the anim source header.
  m_memReqsAnimSourceHdr.size = computeAnimSourceHeaderRequirements();
  m_memReqsAnimSourceHdr.size = NMP::Memory::align(m_memReqsAnimSourceHdr.size, NMP_VECTOR_ALIGNMENT); // Ensure DMA alignment

  //-------------------
  // Compute the byte offset of the section data
  m_sectionsByteOffset = m_memReqsAnimSourceHdr.size + m_memReqsChannelSetInfo.size;

  //-------------------
  // Compute the memory requirements for any additional data
  m_memReqsAdditionalData.size = computeAdditionalDataRequirements();

  //-------------------
  // Compute the full memory requirements for the animation
  m_memReqsAnimSource.size = computeFinalMemoryRequirements();
  m_memReqsAnimSource.size = NMP::Memory::align(m_memReqsAnimSource.size, NMP_VECTOR_ALIGNMENT); // Ensure DMA alignment

  // Allocate the memory for the animation
  buffer = NMPMemoryAllocateFromFormat(m_memReqsAnimSource);
  ZeroMemory(buffer.ptr, m_memReqsAnimSource.size);

  //-------------------
  // Compile the animation into the allocated memory buffer
  onBeginBuildAnimation(buffer);
  buildAnimation(buffer);

  buildRigToAnimMaps();
  onEndBuildAnimation(buffer);

  //-------------------
  // Tidy up
  termCompressor();
  termOptions();

  return buffer;
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceSectioningCompressor::getSectionsSize() const
{
  size_t sumSize = 0;

  // Sections
  std::vector<size_t>::const_iterator sit = m_sectionSizes.begin();
  std::vector<size_t>::const_iterator sit_end = m_sectionSizes.end();
  while (sit != sit_end)
  {
    size_t sectionSize = *sit;
    sumSize += sectionSize;
    sit++;
  }

  return sumSize;
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceSectioningCompressor::computeAdditionalDataRequirements() const
{
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceSectioningCompressor::onBeginComputeSectionRequirements()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimSourceSectioningCompressor::computeSectionRequirements()
{
  // Get the number of frames
  const ChannelSetRequirements* chanSetReqs = m_inputAnimResampled->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);
  uint32_t numFrames = chanSetReqs->getMaxNumKeyframes();
  NMP_VERIFY(numFrames >= 2);

  uint32_t startFrame = 0;
  uint32_t lastFrame = numFrames - 1;
  while (startFrame < lastFrame)
  {
    uint32_t low = startFrame;
    uint32_t high = lastFrame;
    uint32_t endFrame = lastFrame;
    size_t validSectionSize = 0;

    // Find the largest section size by binary search
    while (endFrame != low)
    {
      // Compute the section size between the current start and end frames (inclusive)
      size_t currentSectionSize;
      bool status = computeSectionRequirements(startFrame, endFrame, currentSectionSize);

      // Ensure DMA alignment
      currentSectionSize = NMP::Memory::align(currentSectionSize, NMP_VECTOR_ALIGNMENT);

      // Check if the section size is too big
      if (!status || currentSectionSize > MR::getMaxAnimSectionSize())
      {
        high = endFrame;
      } else {
        low = endFrame;
        validSectionSize = currentSectionSize;
      }
      endFrame = (high + low) >> 1;
    }

    if (validSectionSize == 0)
      return false;

    // Add the section information
    NMP_VERIFY(endFrame != startFrame);
    appendSectionInformation(startFrame, endFrame, validSectionSize);
    startFrame = m_endFrames.back();
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceSectioningCompressor::appendSectionInformation(
  uint32_t startFrame,
  uint32_t endFrame,
  size_t   sectionSize)
{
  m_startFrames.push_back(startFrame);
  m_endFrames.push_back(endFrame);
  m_sectionSizes.push_back(sectionSize);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceSectioningCompressor::onEndComputeSectionRequirements()
{
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceSectioningCompressor::computeFinalMemoryRequirements() const
{
  // Header
  NMP::Memory::Format memReqs(0, NMP_VECTOR_ALIGNMENT);
  memReqs += m_memReqsAnimSourceHdr;

  // Channel set info
  memReqs += m_memReqsChannelSetInfo;

  // Sections
  std::vector<size_t>::const_iterator sit = m_sectionSizes.begin();
  std::vector<size_t>::const_iterator sit_end = m_sectionSizes.end();
  while (sit != sit_end)
  {
    size_t sectionSize = *sit;
    memReqs.size += sectionSize;
    sit++;
  }

  // Trajectory
  memReqs += m_memReqsTrajectorySource;

  // Channel names table
  memReqs += m_memReqsChannelNamesTable;

  // Additional data
  memReqs += m_memReqsAdditionalData;

  // Ensure DMA alignment of whole animation
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  return memReqs.size;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceSectioningCompressor::onBeginBuildAnimation(NMP::Memory::Resource& NMP_UNUSED(buffer))
{
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceSectioningCompressor::buildAnimation(NMP::Memory::Resource& buffer) const
{
  NMP_VERIFY(buffer.ptr);

  // Header
  uint8_t* data = (uint8_t*)buffer.ptr;
  NMP_VERIFY(NMP_IS_ALIGNED(data, NMP_VECTOR_ALIGNMENT));
  buildAnimSourceHeader(buffer, data);
  data += m_memReqsAnimSourceHdr.size;

  // Channel set info
  NMP_VERIFY(NMP_IS_ALIGNED(data, NMP_VECTOR_ALIGNMENT));
  buildChannelSetInfo(buffer, data);
  data += m_memReqsChannelSetInfo.size;

  // Sections
  std::vector<size_t>::const_iterator sit = m_sectionSizes.begin();
  std::vector<size_t>::const_iterator sit_end = m_sectionSizes.end();
  uint32_t sectionIndx = 0;
  while (sit != sit_end)
  {
    size_t sectionSize = *sit;
    NMP_VERIFY(NMP_IS_ALIGNED(data, NMP_VECTOR_ALIGNMENT));
    buildSection(buffer, sectionIndx, data);
    data += sectionSize;
    sit++;
    sectionIndx++;
  }

  // Trajectory
  NMP_VERIFY(NMP_IS_ALIGNED(data, NMP_VECTOR_ALIGNMENT));
  buildTrajectorySource(buffer, data);
  data += m_memReqsTrajectorySource.size;

  // Channel names table
  NMP_VERIFY(NMP_IS_ALIGNED(data, NMP_VECTOR_ALIGNMENT));
  buildChannelNamesTable(buffer, data);
  data += m_memReqsChannelNamesTable.size;

  // Additional data
  NMP_VERIFY(NMP_IS_ALIGNED(data, NMP_VECTOR_ALIGNMENT));
  buildAdditionalData(buffer, data);
  data += m_memReqsAdditionalData.size;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceSectioningCompressor::onEndBuildAnimation(NMP::Memory::Resource& NMP_UNUSED(buffer))
{
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceSectioningCompressor::buildChannelNamesTable(
  NMP::Memory::Resource& buffer,
  uint8_t*               data) const
{
  NMP_VERIFY(buffer.ptr);
  NMP_VERIFY(data);
  (void)buffer;

  if (getAddChannelNamesTable())
  {
    const NMP::OrderedStringTable* channelNames = m_inputAnimResampled->getChannelNames();
    NMP_VERIFY(channelNames);

    NMP::Memory::Resource channelNamesRes;
    channelNamesRes.ptr = data;
    channelNamesRes.format = m_memReqsChannelNamesTable;
    NMP::OrderedStringTable::init(
      channelNamesRes,
      channelNames->getNumEntries(),
      channelNames->getOffsets(),
      channelNames->getData(),
      channelNames->getDataLength());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceSectioningCompressor::buildAdditionalData(
  NMP::Memory::Resource& buffer,
  uint8_t*               data) const
{
  NMP_VERIFY(buffer.ptr);
  NMP_VERIFY(data);
  NMP_VERIFY(m_memReqsAdditionalData.size == 0); // This is the NULL implementation
  (void)buffer;
  (void)data;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSourceSectioningCompressor::computeNumSections(uint32_t framesPerSection) const
{
  NMP_VERIFY(m_inputAnimResampled);
  uint32_t numFrames = m_inputAnimResampled->getMaxNumKeyframes();
  NMP_VERIFY(numFrames >= 2);

  // Note that we have repeated frames between adjacent sections. i.e. the total
  // number of frames in the animation can be written:
  // numAnimFrames = numSections * (framesPerSection - 1) + 1
  uint32_t numSpans = (numFrames - 1);
  uint32_t spansPerSection = framesPerSection - 1;
  uint32_t numSections = numSpans / spansPerSection;
  uint32_t residual = numSpans % spansPerSection;
  if (residual > 0)
    numSections++;

  return numSections;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSourceSectioningCompressor::distributeSectionFrames(uint32_t numSections, uint32_t numFrames)
{
  // Zero the section sizes
  m_sectionSizes.resize(numSections);
  for (uint32_t i = 0; i < numSections; ++i)
    m_sectionSizes[i] = 0;
  
  // Compute the distribution of section frames
  return sectionAnimFramesByNumSections(
    numSections,
    numFrames,
    m_startFrames,
    m_endFrames);
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceSectioningCompressor::distributeSectionSizes(uint32_t numSections, size_t budgetSections)
{
  uint32_t numChannelSets = m_inputAnimResampled->getNumChannelSets();
  NMP_VERIFY(numChannelSets > 0);

  // Evenly distribute the number of frames over all sections to minimise
  // the quantisation errors.
  distributeSectionFrames(numSections, m_inputAnimResampled->getMaxNumKeyframes());

  // Compute the effective compression rate (bytes/bone/frame) of the total section budget
  uint32_t numSamples = getNumSectionFrameSamples();
  float rateSections = (float)budgetSections / (float)(numSamples * numChannelSets);

  //-----------------------
  // Compute the byte budget for each section
  size_t maxSectionBudget = 0;
  for (uint32_t i = 0; i < numSections; ++i)
  {
    // Compute the desired section budget
    uint32_t numSectionFrames = getNumSectionFrameSamples(i);
    size_t sectionBudget = (size_t)(rateSections * (numChannelSets * numSectionFrames));
    sectionBudget = NMP::Memory::align(sectionBudget, NMP_VECTOR_ALIGNMENT); // Ensure DMA alignment

    // Update the maximum section budget
    maxSectionBudget = NMP::maximum(maxSectionBudget, sectionBudget);

    // Set the initial desired section budget. Note that if you set the compression rate very
    // high then the desired budget will exceed the actual used budget due to the maximum
    // allowable precision of the coefficients. The actual budgets are set after the section
    // data has been compiled
    m_sectionSizes[i] = sectionBudget;
  }

  return maxSectionBudget;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSourceSectioningCompressor::getNumSectionFrameSamples() const
{
  uint32_t numSections = (uint32_t)m_startFrames.size();
  uint32_t numSamples = 0;
  for (uint32_t i = 0; i < numSections; ++i)
  {
    uint32_t numSectionFrames = m_endFrames[i] - m_startFrames[i] + 1;
    numSamples += numSectionFrames;
  }
  return numSamples;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSourceSectioningCompressor::getNumSectionFrameSamples(uint32_t sectionIndex) const
{
  NMP_VERIFY(sectionIndex < (uint32_t)m_startFrames.size());
  return m_endFrames[sectionIndex] - m_startFrames[sectionIndex] + 1;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
