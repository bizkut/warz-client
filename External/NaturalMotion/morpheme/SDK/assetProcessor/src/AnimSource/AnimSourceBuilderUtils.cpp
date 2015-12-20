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
#include <algorithm>
#include "assetProcessor/AnimSource/AnimSourceBuilderUtils.h"
#include "NMPlatform/NMMathUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
uint32_t sectionAnimFramesByNumSections(
  uint32_t numSections,
  uint32_t numAnimFrames,
  std::vector<uint32_t>& sectionStartFrames,
  std::vector<uint32_t>& sectionEndFrames)
{
  // Note that we have repeated frames between adjacent sections. i.e. the total
  // number of (actual) keyframes in the animation can be written:
  //   numAnimFrames = numSections * (samplesPerSection - 1) + 1
  // where samplesPerSection is the number of frame samples stored within each
  // section (including the repeated boundary frames).
  NMP_VERIFY(numSections > 0);
  NMP_VERIFY(numAnimFrames >= 2);

  // Verification
  numSections = NMP::minimum(numSections, numAnimFrames - 1);

  //-----------------------
  // Compute the number of frames per section
  uint32_t numSpans = numAnimFrames - 1;
  uint32_t spansPerSection = numSpans / numSections;
  uint32_t residual = numSpans % numSections;
  if (residual > 0)
    spansPerSection++;
  uint32_t samplesPerSection = spansPerSection + 1; // Max number of frames in any of the sections

  //-----------------------
  std::vector<uint32_t> sectionCounts;
  sectionCounts.resize(numSections);
  sectionStartFrames.resize(numSections);
  sectionEndFrames.resize(numSections);

  //-----------------------
  // Evenly distribute the number of frames over all sections to minimise
  // the quantisation errors.
  uint32_t numRequiredSamples = numAnimFrames + (numSections - 1);
  uint32_t numSamples = samplesPerSection * numSections;

  // Init all sections with max number of samples per section
  for (uint32_t i = 0; i < numSections; ++i)
    sectionCounts[i] = samplesPerSection;

  // Loop over all sections removing a frame from each one until we have just enough to contain the animation
  // This gives an even distribution of frames, reducing quantization errors
  for (uint32_t i = 0; i < numSections; ++i)
  {
    if (numSamples == numRequiredSamples)
      break;
    --(sectionCounts[i]);
    --numSamples;
  }
  NMP_VERIFY(numSamples == numRequiredSamples);

  //-----------------------
  // Now calc the start and end frame numbers for each section based on the section sizes
  sectionStartFrames[0] = 0;
  uint32_t frame = 0;
  for (uint32_t i = 1; i < numSections; ++i)
  {
    frame += (uint32_t)(sectionCounts[i] - 1);
    sectionEndFrames[i - 1] = sectionStartFrames[i] = frame;
  }
  sectionEndFrames[numSections - 1] = numAnimFrames - 1;

  return samplesPerSection;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t sectionAnimFramesBySamplesPerSection(
  uint32_t samplesPerSection,
  uint32_t numAnimFrames,
  bool roundToNearestNumSections,
  std::vector<uint32_t>& sectionStartFrames,
  std::vector<uint32_t>& sectionEndFrames)
{
  // Note that we have repeated frames between adjacent sections. i.e. the total
  // number of (actual) keyframes in the animation can be written:
  //   numAnimFrames = numSections * (samplesPerSection - 1) + 1
  // where samplesPerSection is the number of frame samples stored within each
  // section (including the repeated boundary frames).
  NMP_VERIFY(samplesPerSection >= 2);
  NMP_VERIFY(numAnimFrames >= 2);

  //-----------------------
  // Compute the number of requires sections
  uint32_t spansPerSection = samplesPerSection - 1;
  uint32_t numSpans = numAnimFrames - 1;
  uint32_t numSections;
  if (roundToNearestNumSections)
  {
    // Round to the nearest number of sections - may increase the desired
    // samples per section slightly
    numSections = (uint32_t)(((float)numSpans / (float)spansPerSection) + 0.5f);
    if (numSections == 0)
      numSections = 1;
  }
  else
  {
    // Truncation - ensuring that we never exceed the requested samples per section
    numSections = numSpans / spansPerSection;
    uint32_t residual = numSpans % spansPerSection;
    if (residual > 0)
      numSections++;
  }
  

  //-----------------------
  // Distribute the frames
  sectionAnimFramesByNumSections(
    numSections,
    numAnimFrames,
    sectionStartFrames,
    sectionEndFrames);

  return numSections;
}

//----------------------------------------------------------------------------------------------------------------------
bool checkForUnchangingChannelQuantised(
  uint32_t numAnimFrames,
  const uint32_t* data)
{  
  if (numAnimFrames < 2)
    return true;
    
  // Get min and max of the quantisation data
  uint32_t minVal = data[0];
  uint32_t maxVal = data[0];
  for (uint32_t i = 1; i < numAnimFrames; ++i)
  {
    minVal = NMP::minimum(minVal, data[i]);
    maxVal = NMP::maximum(maxVal, data[i]);
  }
  
  // Check if the sample differs by more than a single quantisation step
  return (maxVal - minVal <= 1);
}

//----------------------------------------------------------------------------------------------------------------------
// CompToAnimChannelMapLayout
//----------------------------------------------------------------------------------------------------------------------
CompToAnimChannelMapLayout::CompToAnimChannelMapLayout(const CompToAnimChannelMapLayout& rhs)
{
  m_animChannels = rhs.m_animChannels;
}

//----------------------------------------------------------------------------------------------------------------------
CompToAnimChannelMapLayout::CompToAnimChannelMapLayout()
{
}

//----------------------------------------------------------------------------------------------------------------------
CompToAnimChannelMapLayout::~CompToAnimChannelMapLayout()
{
}

//----------------------------------------------------------------------------------------------------------------------
// SectionDataChannelLayout::SectionLayout
//----------------------------------------------------------------------------------------------------------------------
SectionDataChannelLayout::SectionLayout::SectionLayout(uint32_t numChannelTypes)
{
  NMP_VERIFY(numChannelTypes > 0);
  m_numChannelTypes = numChannelTypes;
  m_channelTypeLayouts = new CompToAnimChannelMapLayout*[numChannelTypes];
  for (uint32_t i = 0; i < numChannelTypes; ++i)
  {
    m_channelTypeLayouts[i] = new CompToAnimChannelMapLayout;
  }

  m_sectionSize = 0;
}

//----------------------------------------------------------------------------------------------------------------------
SectionDataChannelLayout::SectionLayout::~SectionLayout()
{
  tidy();
}

//----------------------------------------------------------------------------------------------------------------------
void SectionDataChannelLayout::SectionLayout::tidy()
{
  if (m_channelTypeLayouts)
  {
    for (uint32_t i = 0; i < m_numChannelTypes; ++i)
    {
      CompToAnimChannelMapLayout* channelTypeLayout = m_channelTypeLayouts[i];
      NMP_VERIFY(channelTypeLayout);
      delete channelTypeLayout;
    }

    delete[] m_channelTypeLayouts;
    m_channelTypeLayouts = NULL;
  }

  m_numChannelTypes = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SectionDataChannelLayout::SectionLayout::sort()
{
  for (uint32_t i = 0; i < m_numChannelTypes; ++i)
  {
    CompToAnimChannelMapLayout* channelTypeLayout = m_channelTypeLayouts[i];
    NMP_VERIFY(channelTypeLayout);
    channelTypeLayout->sort();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// SectionDataChannelLayout
//----------------------------------------------------------------------------------------------------------------------
SectionDataChannelLayout::SectionDataChannelLayout() :
  m_numSections(0),
  m_sectionLayouts(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
SectionDataChannelLayout::~SectionDataChannelLayout()
{
  tidy();
}

//----------------------------------------------------------------------------------------------------------------------
void SectionDataChannelLayout::tidy()
{
  if (m_sectionLayouts)
  {
    for (uint32_t i = 0; i < m_numSections; ++i)
    {
      SectionLayout* sectionLayout = m_sectionLayouts[i];
      NMP_VERIFY(sectionLayout);
      delete sectionLayout;
    }

    delete[] m_sectionLayouts;
    m_sectionLayouts = NULL;
  }
  m_numSections = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void SectionDataChannelLayout::computeLayout(
  uint32_t numSections,
  uint32_t numChannelTypes,
  const std::vector<uint32_t>* const* channelTypeAnimIds,
  const size_t* memReqsPerChannelType)
{
  NMP_VERIFY(numSections > 0);
  NMP_VERIFY(numChannelTypes > 0);
  NMP_VERIFY(memReqsPerChannelType);
  m_numSections = numSections;

  // Set up section layouts
  NMP_VERIFY(!m_sectionLayouts);
  m_sectionLayouts = new SectionLayout*[numSections];
  for (uint32_t i = 0; i < numSections; ++i)
  {
    m_sectionLayouts[i] = new SectionLayout(numChannelTypes);
  }

  //-----------------------
  // Simple case
  if (numSections == 1)
  {
    SectionLayout* sectionLayout = m_sectionLayouts[0];
    size_t sectionSize = 0;
    for (uint32_t channelTypeIndx = 0; channelTypeIndx < numChannelTypes; ++channelTypeIndx)
    {
      const std::vector<uint32_t>* animIds = channelTypeAnimIds[channelTypeIndx];
      NMP_VERIFY(animIds);
      sectionLayout->m_channelTypeLayouts[channelTypeIndx]->setAnimChannels(*animIds);

      uint32_t numEntries = (uint32_t)animIds->size();
      sectionSize += (memReqsPerChannelType[channelTypeIndx] * numEntries);
    }
    sectionLayout->m_sectionSize = sectionSize;
    return;
  }

  //-----------------------
  // Initialise the section sizes
  std::vector<size_t> sectionSizes;
  sectionSizes.resize(numSections);
  for (uint32_t i = 0; i < numSections; ++i)
    sectionSizes[i] = 0;

  //-----------------------
  // Assign anim Ids to section layouts in blocks of four (for SIMD optimisation)
  for (uint32_t channelTypeIndx = 0; channelTypeIndx < numChannelTypes; ++channelTypeIndx)
  {
    NMP_VERIFY(channelTypeAnimIds[channelTypeIndx]);
    const std::vector<uint32_t>& animIds = *(channelTypeAnimIds[channelTypeIndx]);
    uint32_t numAnimIds = (uint32_t)animIds.size();
    size_t memReqsChan = memReqsPerChannelType[channelTypeIndx];

    // Iterate over anim ids vector
    for (uint32_t indx = 0; indx < numAnimIds; indx += 4)
    {
      //-----------------------
      // Find the section layout with the smallest size      
      uint32_t sectionIndex = 0;
      for (uint32_t i = 1; i < numSections; ++i)
      {
        if (sectionSizes[i] < sectionSizes[sectionIndex])
        {
          sectionIndex = i;
        }
      }

      //-----------------------
      // Add the channel ids to the section layout
      SectionLayout* sectionLayout = m_sectionLayouts[sectionIndex];
      NMP_VERIFY(sectionLayout);
      CompToAnimChannelMapLayout* comToAnimMap = sectionLayout->m_channelTypeLayouts[channelTypeIndx];
      NMP_VERIFY(comToAnimMap);

      {
        comToAnimMap->push_back(animIds[indx]);
        sectionSizes[sectionIndex] += memReqsChan;
      }
      if (indx + 1 < numAnimIds)
      {
        comToAnimMap->push_back(animIds[indx + 1]);
        sectionSizes[sectionIndex] += memReqsChan;
      }
      if (indx + 2 < numAnimIds)
      {
        comToAnimMap->push_back(animIds[indx + 2]);
        sectionSizes[sectionIndex] += memReqsChan;
      }
      if (indx + 3 < numAnimIds)
      {
        comToAnimMap->push_back(animIds[indx + 3]);
        sectionSizes[sectionIndex] += memReqsChan;
      }
    }
  }

  //-----------------------
  // Balence the memory requirements for each section by re-assigning channels to different
  // sections with the goal of reducing the memory size variance amoungst the sections.
  // Note that this procedure possibly adds additional 4-block layout channels, which is
  // why we preform it after optimally clumping channels into blocks of four.
  std::vector<size_t> sectionSizesNew;
  sectionSizesNew.resize(numSections);

  bool flag;
  do
  {
    // Reset the iteration flag
    flag = false;

    // Compute the mean and variance of the section sizes
    float xbar, xvar;
    sectionSizeMeanAndVar(sectionSizes, xbar, xvar);

    //-----------------------
    // Find the best re-assignment that reduces the section size variance
    uint32_t channelTypeIndxBest = 0;
    uint32_t sectionIndexFromBest = 0;
    uint32_t sectionIndexToBest = 0;
    float varBest = xvar;
    for (uint32_t channelTypeIndx = 0; channelTypeIndx < numChannelTypes; ++channelTypeIndx)
    {
      size_t memReqsChan = memReqsPerChannelType[channelTypeIndx];

      for (uint32_t sectionIndexFrom = 0; sectionIndexFrom < numSections; ++sectionIndexFrom)
      {
        SectionLayout* sectionLayoutFrom = m_sectionLayouts[sectionIndexFrom];
        NMP_VERIFY(sectionLayoutFrom);
        CompToAnimChannelMapLayout* comToAnimMapFrom = sectionLayoutFrom->m_channelTypeLayouts[channelTypeIndx];
        NMP_VERIFY(comToAnimMapFrom);
        if (comToAnimMapFrom->getNumEntries() > 0)
        {
          for (uint32_t sectionIndexTo = 0; sectionIndexTo < numSections; ++sectionIndexTo)
          {
            if (sectionIndexFrom != sectionIndexTo)
            {
              // Compute the section sizes after re-assignment
              for (uint32_t i = 0; i < numSections; ++i)
                sectionSizesNew[i] = sectionSizes[i];
              sectionSizesNew[sectionIndexFrom] -= memReqsChan;
              sectionSizesNew[sectionIndexTo] += memReqsChan;

              // Compute the mean and variance of the new section sizes
              sectionSizeMeanAndVar(sectionSizesNew, xbar, xvar);
 
              // Update the best re-assignment
              if (xvar < varBest)
              {
                channelTypeIndxBest = channelTypeIndx;
                sectionIndexFromBest = sectionIndexFrom;
                sectionIndexToBest = sectionIndexTo;
                varBest = xvar;              
              }
            }
          } // sectionIndexTo
        }
      } // sectionIndexFrom
    } // channelTypeIndx

    //-----------------------
    // Perform the re-assignment
    if (sectionIndexFromBest != sectionIndexToBest)
    {
      size_t memReqsChan = memReqsPerChannelType[channelTypeIndxBest];

      SectionLayout* sectionLayoutFrom = m_sectionLayouts[sectionIndexFromBest];
      NMP_VERIFY(sectionLayoutFrom);
      CompToAnimChannelMapLayout* comToAnimMapFrom = sectionLayoutFrom->m_channelTypeLayouts[channelTypeIndxBest];
      NMP_VERIFY(comToAnimMapFrom);
      uint32_t animId = comToAnimMapFrom->pop_back();
      sectionSizes[sectionIndexFromBest] -= memReqsChan;

      SectionLayout* sectionLayoutTo = m_sectionLayouts[sectionIndexToBest];
      NMP_VERIFY(sectionLayoutTo);
      CompToAnimChannelMapLayout* comToAnimMapTo = sectionLayoutTo->m_channelTypeLayouts[channelTypeIndxBest];
      NMP_VERIFY(comToAnimMapTo);
      comToAnimMapTo->push_back(animId);
      sectionSizes[sectionIndexToBest] += memReqsChan;

      flag = true;
    }

  } while(flag);

  //-----------------------
  // Set the resulting section sizes
  for (uint32_t sectionIndex = 0; sectionIndex < numSections; ++sectionIndex)
  {
    SectionLayout* sectionLayout = m_sectionLayouts[sectionIndex];
    NMP_VERIFY(sectionLayout);

    // Ensure that the animation channels are strictly increasing.
    sectionLayout->sort();

    sectionLayout->m_sectionSize = sectionSizes[sectionIndex];
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t SectionDataChannelLayout::getLargestSectionLayoutIndex() const
{
  NMP_VERIFY(m_numSections > 0);
  uint32_t indx = 0;
  for (uint32_t i = 1; i < m_numSections; ++i)
  {
    NMP_VERIFY(m_sectionLayouts[i]);
    if (m_sectionLayouts[i]->m_sectionSize > m_sectionLayouts[indx]->m_sectionSize)
    {
      indx = i;
    }
  }
  return indx;
}

//----------------------------------------------------------------------------------------------------------------------
void SectionDataChannelLayout::sectionSizeMeanAndVar(
  const std::vector<size_t>& sectionSizes,
  float& sectionSizeMean,
  float& sectionSizeVar)
{
  uint32_t numSections = (uint32_t)sectionSizes.size();
  NMP_VERIFY(numSections > 0);

  // Compute the mean of the section sizes
  float ooN = 1.0f / (float)numSections;
  size_t sum = 0;
  for (uint32_t i = 0; i < numSections; ++i)
  {
    sum += sectionSizes[i];
  }
  sectionSizeMean = ooN * (float)sum;

  // Compute the variance of the section sizes
  float sumsq = 0.0f;
  for (uint32_t i = 0; i < numSections; ++i)
  {
    float dx = (float)sectionSizes[i] - sectionSizeMean;
    sumsq += dx*dx;
  }
  sectionSizeVar = ooN * sumsq;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
