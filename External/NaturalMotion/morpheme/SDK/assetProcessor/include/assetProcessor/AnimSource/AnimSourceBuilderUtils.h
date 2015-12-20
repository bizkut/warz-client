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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef ANIM_SOURCE_BUILDER_UTILS_H
#define ANIM_SOURCE_BUILDER_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include <vector>
#include <algorithm>
#include "morpheme/AnimSource/mrAnimSourceUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
uint32_t sectionAnimFramesByNumSections(
  uint32_t numSections,
  uint32_t numAnimFrames,
  std::vector<uint32_t>& sectionStartFrames,
  std::vector<uint32_t>& sectionEndFrames);
  
//----------------------------------------------------------------------------------------------------------------------
uint32_t sectionAnimFramesBySamplesPerSection(
  uint32_t samplesPerSection,
  uint32_t numAnimFrames,
  bool roundToNearestNumSections,
  std::vector<uint32_t>& sectionStartFrames,
  std::vector<uint32_t>& sectionEndFrames);

//----------------------------------------------------------------------------------------------------------------------
/// \brief returns true if the channel data is considered to be unchanging. Due to numerical error
/// a value may be bouncing between two quantisation intervals if the step size is sufficiently small.
/// This function checks to see if the quantised values remain within a single interval of one another.
bool checkForUnchangingChannelQuantised(
  uint32_t numAnimFrames,
  const uint32_t* data);

//----------------------------------------------------------------------------------------------------------------------
// CompToAnimChannelMapLayout
//----------------------------------------------------------------------------------------------------------------------
class CompToAnimChannelMapLayout
{
public:
  CompToAnimChannelMapLayout(const CompToAnimChannelMapLayout& rhs);
  CompToAnimChannelMapLayout();
  ~CompToAnimChannelMapLayout();

  NM_INLINE uint32_t getNumEntries() const { return (uint32_t)m_animChannels.size(); }
  NM_INLINE void setAnimChannels(const std::vector<uint32_t>& animChannels) { m_animChannels = animChannels; }
  NM_INLINE const std::vector<uint32_t>& getAnimChannels() const { return m_animChannels; }
  NM_INLINE uint32_t getAnimChannel(uint32_t indx) const { return m_animChannels[indx]; }

  NM_INLINE void push_back(uint32_t animId) { m_animChannels.push_back(animId); }
  NM_INLINE uint32_t pop_back() { uint32_t result = m_animChannels.back(); m_animChannels.pop_back(); return result; }

  NM_INLINE void sort() { std::sort(m_animChannels.begin(), m_animChannels.end()); }

protected:
  std::vector<uint32_t>   m_animChannels;
};

//----------------------------------------------------------------------------------------------------------------------
// SectionDataChannelLayout
//----------------------------------------------------------------------------------------------------------------------
class SectionDataChannelLayout
{
public:
  class SectionLayout
  {
    friend class SectionDataChannelLayout;
  public:
    SectionLayout(uint32_t numChannelTypes);
    ~SectionLayout();

    void tidy();
    void sort();

    NM_INLINE uint32_t getNumChannelTypes() const { return m_numChannelTypes; }
    NM_INLINE const CompToAnimChannelMapLayout* getChannelTypeLayout(uint32_t channelType) const;

    NM_INLINE size_t getSectionSize() const { return m_sectionSize; }

  protected:
    uint32_t m_numChannelTypes;
    CompToAnimChannelMapLayout** m_channelTypeLayouts;
    size_t m_sectionSize;
  };

public:
  SectionDataChannelLayout();
  ~SectionDataChannelLayout();

  void tidy();

  void computeLayout(
    uint32_t numSections,
    uint32_t numChannelTypes,
    const std::vector<uint32_t>* const* channelTypeAnimIds,
    const size_t* memReqsPerChannelType);

  uint32_t getLargestSectionLayoutIndex() const;

  NM_INLINE uint32_t getNumSections() const { return m_numSections; }
  NM_INLINE const SectionLayout* getSectionLayout(uint32_t sectionIndex) const;

protected:
  static void sectionSizeMeanAndVar(
    const std::vector<size_t>& sectionSizes,
    float& sectionSizeMean,
    float& sectionSizeVar);

protected:
  uint32_t m_numSections;
  SectionLayout** m_sectionLayouts;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const CompToAnimChannelMapLayout* SectionDataChannelLayout::SectionLayout::getChannelTypeLayout(uint32_t channelType) const
{
  NMP_VERIFY(m_channelTypeLayouts);
  NMP_VERIFY(channelType < m_numChannelTypes);
  return m_channelTypeLayouts[channelType];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const SectionDataChannelLayout::SectionLayout* SectionDataChannelLayout::getSectionLayout(uint32_t sectionIndex) const
{
  NMP_VERIFY(m_sectionLayouts);
  NMP_VERIFY(sectionIndex < m_numSections);
  return m_sectionLayouts[sectionIndex];
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SOURCE_BUILDER_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
