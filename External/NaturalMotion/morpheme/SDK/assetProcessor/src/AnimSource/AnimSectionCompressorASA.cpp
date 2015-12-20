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
#include "assetProcessor/AnimSource/ChannelQuatBuilder.h"
#include "assetProcessor/AnimSource/ChannelPosBuilder.h"
#include "assetProcessor/AnimSource/AnimSectionCompressorASA.h"
//----------------------------------------------------------------------------------------------------------------------

// For profiling compressor
// #define PROFILE_ASA
// #define DEBUG_TRANSFORMS

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

namespace AP
{

class AnimSubSectionASABuilder : public MR::AnimSubSectionASA
{
public:
  /// \brief Function to compute the required memory for a section between the specified frames
  static NMP::Memory::Format getSectionMemoryRequirements(
    const AnimSourceUncompressed* inputAnim,
    uint32_t                      startFrame,
    uint32_t                      endFrame);

  /// \brief Function to build the animation section within the specified memory buffer
  static size_t init(
    const AnimSourceUncompressed* inputAnim,
    MR::AnimSubSectionASA*        subsection,
    uint32_t                      subsectionID,
    uint32_t                      offset,
    uint32_t                      numChannelSets,
    uint32_t                      startFrame,
    uint32_t                      endFrame,
    uint8_t**                     data);
};

//----------------------------------------------------------------------------------------------------------------------
// AnimSubSectionASABuilder Functions
//----------------------------------------------------------------------------------------------------------------------
size_t AnimSubSectionASABuilder::init(
  const AnimSourceUncompressed*  inputAnim,
  MR::AnimSubSectionASA*         subsection,
  uint32_t                       subsectionID,
  uint32_t                       offset,
  uint32_t                       numChannelSets,
  uint32_t                       startFrame,
  uint32_t                       endFrame,
  uint8_t**                      data)
{
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(startFrame < endFrame);

  //-----------------------
  // Information
  uint32_t numSamples = endFrame - startFrame + 1;
  const ChannelSetRequirements* chanSetReqs = inputAnim->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);
  const ChannelSetTable* channelSets = inputAnim->getChannelSets();
  NMP_VERIFY(channelSets);

  AnimSubSectionASABuilder* subsectionBuilder = (AnimSubSectionASABuilder*)subsection;
  subsectionBuilder->m_subsectionSize         = 0; // Value filled in later
  subsectionBuilder->m_subsectionID           = subsectionID;
  subsectionBuilder->m_numChannelSets         = numChannelSets;
  subsectionBuilder->m_offset                 = offset;

  //-----------------------
  // Channel sets
  subsectionBuilder->m_channelSets = (MR::ChannelSetASA*) * data;
  *data += (sizeof(MR::ChannelSetASA) * numChannelSets);
  *data  = (uint8_t*)NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);
  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    uint32_t channelSet = i + subsection->getOffset();
    const NMP::Quat* chanQuatKeyframes    = channelSets->getChannelQuat(channelSet);
    const NMP::Vector3* chanPosKeyframes  = channelSets->getChannelPos(channelSet);
    NMP_VERIFY(chanQuatKeyframes);
    NMP_VERIFY(chanPosKeyframes);

    // Channel Set
    MR::ChannelSetASA& basicChanSet = subsectionBuilder->m_channelSets[i];

    //-----------------------
    // Quat channel
    if (chanSetReqs->isChannelQuatUnchanging(channelSet))
    {
      AP::ChannelQuatBuilder::initChannelQuat(
        0,
        chanQuatKeyframes,
        *basicChanSet.getChannelQuat(),
        data);
    }
    else
    {
      AP::ChannelQuatBuilder::initChannelQuat(
        numSamples,
        &chanQuatKeyframes[startFrame],
        *basicChanSet.getChannelQuat(),
        data);
    }

    //-----------------------
    // Pos channel
    if (chanSetReqs->isChannelPosUnchanging(channelSet))
    {
      AP::ChannelPosBuilder::initChannelPos(
        0,
        chanPosKeyframes,
        *basicChanSet.getChannelPos(),
        data);
    }
    else
    {
      AP::ChannelPosBuilder::initChannelPos(
        numSamples,
        &chanPosKeyframes[startFrame],
        *basicChanSet.getChannelPos(),
        data);
    }
  }

  *data = (uint8_t*)NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);
  //-----------------------
  // Subsection size
  subsectionBuilder->m_subsectionSize = (ptrdiff_t)((uint8_t*) * data - (uint8_t*)&*subsection);
  return subsectionBuilder->m_subsectionSize;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionASABuilder::init(
  const AnimSourceUncompressed*  inputAnim,
  MR::AnimSectionASA*            section,
  const MR::ChannelSetASAInfo*   NMP_UNUSED(channelSetInfo),
  uint32_t                       numSubSections,
  uint32_t                       startFrame,
  uint32_t                       endFrame)
{
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(section);
  NMP_VERIFY(NMP_IS_ALIGNED(section, NMP_VECTOR_ALIGNMENT));
  NMP_VERIFY(startFrame < endFrame);
  size_t subsectionSize;

  //-----------------------
  // Header
  AnimSectionASABuilder* sectionBuilder = (AnimSectionASABuilder*)section;
  uint8_t* data = (uint8_t*)section + sizeof(MR::AnimSectionASA);
  data = (uint8_t*)NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  sectionBuilder->m_maxSubsectionSize = 0;  // Value filled in later
  sectionBuilder->m_numSubSections    = numSubSections;
  NMP_VERIFY(numSubSections > 0);
  uint32_t numChannelSets = (uint32_t)(inputAnim->getNumChannelSets() / numSubSections);
  uint32_t offset         = 0;

  section->setSubsections((MR::DataRef*) data);  // Values filled in later
  data += sizeof(MR::DataRef) * numSubSections;
  data = (uint8_t*)NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);

  // Create each subsection within the section
  data = (uint8_t*)NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  for (uint32_t i = 0; i < numSubSections; ++i)
  {
    if (i == numSubSections - 1)
    {
      numChannelSets = inputAnim->getNumChannelSets() - offset;
    }
    MR::DataRef* subsections          = section->getSubsections();
    subsections[i].m_data             = (void*) data;
    data += sizeof(MR::AnimSubSectionASA);
    data = (uint8_t*)NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);

    subsectionSize = AnimSubSectionASABuilder::init(
                       inputAnim,
                       (MR::AnimSubSectionASA*)subsections[i].m_data,
                       i,
                       offset,
                       numChannelSets,
                       startFrame,
                       endFrame,
                       &data);
    offset += numChannelSets;
    if (subsectionSize > sectionBuilder->m_maxSubsectionSize)
    {
      sectionBuilder->m_maxSubsectionSize = subsectionSize;
    }
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
