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
#include "morpheme/AnimSource/mrAnimSourceMBA.h"
#include "assetProcessor/AnimSource/ChannelQuatBuilder.h"
#include "assetProcessor/AnimSource/ChannelPosBuilder.h"
#include "assetProcessor/AnimSource/AnimSourceCompressorMBA.h"
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about deprecated functions (sprintf, fopen)
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4996)
#endif

// For testing the channel set transforms after building
#define TEST_TRANSFORMSx

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ChannelSetMBAInfoBuilder
/// \brief A class to build the channel set information
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetMBAInfoBuilder : public MR::ChannelSetMBAInfo
{
public:

  /// \brief Initialisation function to build the channel set information for the
  /// position and orientation channels of an animation.
  static void init(
    ChannelSetMBAInfo&   chanSetInfo,
    uint32_t             numRequiredQuatSamples,
    const NMP::Quat*     quatKeyframes,
    uint32_t             numRequiredPosSamples,
    const NMP::Vector3*  posKeyframes);
};

//----------------------------------------------------------------------------------------------------------------------
// AnimSectionMBABuilder
//----------------------------------------------------------------------------------------------------------------------
class AnimSectionMBABuilder : public MR::AnimSectionMBA
{
public:
  /// \brief Function to compute the required memory for a section between the
  /// specified frames
  static NMP::Memory::Format getSectionMemoryRequirements(
    const AnimSourceUncompressed* inputAnim,
    uint32_t                      startFrame,
    uint32_t                      endFrame);

  /// \brief Function to build the animation section within the specified memory buffer
  static void init(
    const AnimSourceUncompressed* inputAnim,
    MR::AnimSectionMBA*           section,
    const MR::ChannelSetMBAInfo*  channelSetInfo,
    uint32_t                      startFrame,
    uint32_t                      endFrame);
};

//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceMBABuilder
//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceMBABuilder : public MR::TrajectorySourceMBA
{
public:
  /// \brief Function to compute the required memory for the trajectory source
  static NMP::Memory::Format getMemoryRequirements(
    const AnimSourceUncompressed* inputAnim);

  /// \brief Function to build the trajectory source within the specified memory buffer
  static void init(
    const AnimSourceUncompressed* inputAnim,
    MR::TrajectorySourceMBA*      trajectorySource,
    const NMP::Memory::Format&    memoryReqs);
};

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceBasicBuilder
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceMBABuilder : public MR::AnimSourceMBA
{
public:
  /// \brief Function to compute the required memory for a basic animation
  /// with the specified section sizes
  static NMP::Memory::Format getMemoryRequirements(
    const AnimSourceCompressorMBA& manager);

  /// \brief Main function to build a basic animation in the allocated memory block
  static void init(
    const AnimSourceCompressorMBA& manager,
    NMP::Memory::Resource&         buffer);
};

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetMBAInfoBuilder Functions
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetMBAInfoBuilder::init(
  ChannelSetMBAInfo&   chanSetInfo,
  uint32_t             NMP_UNUSED(numRequiredQuatSamples),
  const NMP::Quat*     quatKeyframes,
  uint32_t             NMP_UNUSED(numRequiredPosSamples),
  const NMP::Vector3*  posKeyframes)
{
  NMP_VERIFY(quatKeyframes);
  NMP_VERIFY(posKeyframes);

  ChannelSetMBAInfoBuilder& chanSetInfoBuilder = (ChannelSetMBAInfoBuilder&)chanSetInfo;

  // Quat channel
  chanSetInfoBuilder.m_chanQuatDefault = quatKeyframes[0];

  // Pos channel
  chanSetInfoBuilder.m_chanPosDefault = posKeyframes[0];
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSectionMBABuilder Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimSectionMBABuilder::getSectionMemoryRequirements(
  const AnimSourceUncompressed* inputAnim,
  uint32_t                      startFrame,
  uint32_t                      endFrame)
{
  NMP::Memory::Format memReqs(0, NMP_VECTOR_ALIGNMENT);
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(endFrame > startFrame);

  // Get the number of animation channel sets
  uint32_t numChannelSets = inputAnim->getNumChannelSets();

  // Get the number of keyframe samples
  uint32_t numSamples = endFrame - startFrame + 1;

  // Compute the memory requirements of a section without any keyframe data
  memReqs.size += NMP::Memory::align(sizeof(MR::AnimSectionMBA), NMP_VECTOR_ALIGNMENT);
  memReqs.size += (sizeof(MR::ChannelSetMBA) * numChannelSets);
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  // Compute the memory requirements of the keyframe data
  const ChannelSetRequirements* chanSetReqs = inputAnim->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);
  NMP_VERIFY(chanSetReqs->getNumChannelSets() == numChannelSets);

  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    // Quat channel
    if (!chanSetReqs->isChannelQuatUnchanging(i))
    {
      NMP::Memory::Format memReqsQuat =
        AP::ChannelQuatBuilder::channelQuatGetMemoryRequirements(numSamples);
      memReqs.size += memReqsQuat.size;
    }

    // Pos channel
    if (!chanSetReqs->isChannelPosUnchanging(i))
    {
      NMP::Memory::Format memReqsPos =
        AP::ChannelPosBuilder::channelPosGetMemoryRequirements(numSamples);
      memReqs.size += memReqsPos.size;
    }
  }

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionMBABuilder::init(
  const AnimSourceUncompressed*  inputAnim,
  MR::AnimSectionMBA*            section,
  const MR::ChannelSetMBAInfo*   NMP_UNUSED(channelSetInfo),
  uint32_t                       startFrame,
  uint32_t                       endFrame)
{
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(section);
  NMP_VERIFY(NMP_IS_ALIGNED(section, NMP_VECTOR_ALIGNMENT));
  NMP_VERIFY(startFrame < endFrame);

  //-----------------------
  // Information
  uint32_t numSamples = endFrame - startFrame + 1;
  const ChannelSetRequirements* chanSetReqs = inputAnim->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);
  const ChannelSetTable* channelSets = inputAnim->getChannelSets();
  NMP_VERIFY(channelSets);
  uint32_t numChannelSets = channelSets->getNumChannelSets();

  //-----------------------
  // Header
  AnimSectionMBABuilder* sectionBuilder = (AnimSectionMBABuilder*)section;
  uint8_t* data = (uint8_t*)section + sizeof(MR::AnimSectionMBA);
  data = (uint8_t*)NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  sectionBuilder->m_numChannelSets = numChannelSets;
  sectionBuilder->m_channelSets = (MR::ChannelSetMBA*)data;

  //-----------------------
  // Channel sets
  data += (sizeof(MR::ChannelSetMBA) * numChannelSets);
  data = (uint8_t*)NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    // Input anim
    const NMP::Quat* chanQuatKeyframes = channelSets->getChannelQuat(i);
    const NMP::Vector3* chanPosKeyframes = channelSets->getChannelPos(i);
    NMP_VERIFY(chanQuatKeyframes);
    NMP_VERIFY(chanPosKeyframes);

    // Channel Set
    MR::ChannelSetMBA& basicChanSet = sectionBuilder->m_channelSets[i];

    //-----------------------
    // Quat channel
    if (chanSetReqs->isChannelQuatUnchanging(i))
    {
      AP::ChannelQuatBuilder::initChannelQuat(
        0,
        chanQuatKeyframes,
        *basicChanSet.getChannelQuat(),
        &data);
    }
    else
    {
      AP::ChannelQuatBuilder::initChannelQuat(
        numSamples,
        &chanQuatKeyframes[startFrame],
        *basicChanSet.getChannelQuat(),
        &data);
    }

    //-----------------------
    // Pos channel
    if (chanSetReqs->isChannelPosUnchanging(i))
    {
      AP::ChannelPosBuilder::initChannelPos(
        0,
        chanPosKeyframes,
        *basicChanSet.getChannelPos(),
        &data);
    }
    else
    {
      AP::ChannelPosBuilder::initChannelPos(
        numSamples,
        &chanPosKeyframes[startFrame],
        *basicChanSet.getChannelPos(),
        &data);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceMBABuilder Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TrajectorySourceMBABuilder::getMemoryRequirements(const AnimSourceUncompressed* inputAnim)
{
  //-----------------------
  // Information
  NMP_VERIFY(inputAnim);
  const TrajectorySourceUncompressed* trajectory = inputAnim->getTrajectorySource();
  NMP_VERIFY(trajectory);
  const ChannelSetRequirements* deltaTrajChanSetReqs = trajectory->getDeltaTrajectroyChannelSetRequirements();
  NMP_VERIFY(deltaTrajChanSetReqs);

  //-----------------------
  // Header
  NMP::Memory::Format memReqs(0, NMP_VECTOR_ALIGNMENT);
  memReqs += NMP::Memory::Format(sizeof(MR::TrajectorySourceMBA), NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Delta trajectory channel set
  const ChannelSetTable* trajChannelSet = trajectory->getDeltaTrajectroyChannelSet();

  // Quat channel
  if (!deltaTrajChanSetReqs->isChannelQuatUnchanging(0))
  {
    // Pack the required number of quaternion values into the channel
    NMP::Memory::Format memReqsQuat =
      AP::ChannelQuatBuilder::channelQuatGetMemoryRequirements(trajChannelSet->getNumKeyFrames());
    memReqs += memReqsQuat;
  }

  // Pos channel
  if (!deltaTrajChanSetReqs->isChannelPosUnchanging(0))
  {
    // Pack the required number of position values into the channel
    NMP::Memory::Format memReqsPos =
      AP::ChannelPosBuilder::channelPosGetMemoryRequirements(trajChannelSet->getNumKeyFrames());
    memReqs += memReqsPos;
  }

  //-----------------------
  // Pack whole structure to be DMA aligned.
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceMBABuilder::init(
  const AnimSourceUncompressed* inputAnim,
  MR::TrajectorySourceMBA*      trajectorySource,
  const NMP::Memory::Format&    memoryReqs)
{
  NMP_VERIFY(inputAnim);
  NMP_VERIFY(trajectorySource);

  TrajectorySourceMBABuilder* trajectorySourceBuilder = (TrajectorySourceMBABuilder*)trajectorySource;

  // Set the compression type.
  trajectorySourceBuilder->m_trajType = TRAJ_TYPE_MBA;
  trajectorySourceBuilder->m_getTrajAtTime = MR::TrajectorySourceMBA::computeTrajectoryTransformAtTime;

  MR::ChannelSetMBAInfo& trajChanSetInfo = trajectorySourceBuilder->m_deltaTrajectoryInfo;

  //-----------------------
  // Input anim
  const TrajectorySourceUncompressed* trajSource = inputAnim->getTrajectorySource();
  NMP_VERIFY(trajSource);
  const ChannelSetTable* trajDeltaChannelSet = trajSource->getDeltaTrajectroyChannelSet();
  const NMP::Quat* trajChannelQuat = trajDeltaChannelSet->getChannelQuat(0);
  const NMP::Vector3* trajChannelPos = trajDeltaChannelSet->getChannelPos(0);

  const ChannelSetRequirements* deltaTrajChanSetReqs = trajSource->getDeltaTrajectroyChannelSetRequirements();
  NMP_VERIFY(deltaTrajChanSetReqs);
  const ChannelSetRequirements::channelSetReqs_t* trajChanReqs = deltaTrajChanSetReqs->getChannelSetRequirements();
  NMP_VERIFY(trajChanReqs);

  //-----------------------
  // Header
  uint8_t* data = (uint8_t*)trajectorySource;
  data += NMP::Memory::align(sizeof(MR::TrajectorySourceMBA), NMP_VECTOR_ALIGNMENT);
  trajectorySourceBuilder->m_sampleFrequency = inputAnim->getSampleFrequency();

  // Store overall memory requirements.
  trajectorySourceBuilder->m_trajectoryInstanceMemReqs = memoryReqs;

  //-----------------------
  // Channel set default info
  ChannelSetMBAInfoBuilder::init(
    trajChanSetInfo,
    trajChanReqs->m_numRequiredQuatSamples,
    trajChannelQuat,
    trajChanReqs->m_numRequiredPosSamples,
    trajChannelPos);

  //-----------------------
  // Quat channel
  if (deltaTrajChanSetReqs->isChannelQuatUnchanging(0))
  {
    AP::ChannelQuatBuilder::initChannelQuat(
      0, // Store no samples
      trajChannelQuat,
      *trajectorySourceBuilder->m_deltaTrajectoryChannelSet.getChannelQuat(),
      &data);
  }
  else
  {
    AP::ChannelQuatBuilder::initChannelQuat(
      trajChanReqs->m_numRequiredQuatSamples,
      trajChannelQuat,
      *trajectorySourceBuilder->m_deltaTrajectoryChannelSet.getChannelQuat(),
      &data);
  }

  //-----------------------
  // Pos channel
  if (deltaTrajChanSetReqs->isChannelPosUnchanging(0))
  {
    AP::ChannelPosBuilder::initChannelPos(
      0, // Store no samples
      trajChannelPos,
      *trajectorySourceBuilder->m_deltaTrajectoryChannelSet.getChannelPos(),
      &data);
  }
  else
  {
    AP::ChannelPosBuilder::initChannelPos(
      trajChanReqs->m_numRequiredPosSamples,
      trajChannelPos,
      *trajectorySourceBuilder->m_deltaTrajectoryChannelSet.getChannelPos(),
      &data);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceMBABuilder Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimSourceMBABuilder::getMemoryRequirements(const AnimSourceCompressorMBA& manager)
{
  NMP::Memory::Format memReqs(0, NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Information
  AnimSourceMBALayout& memLayout = (AnimSourceMBALayout&)manager.getAnimSourceMemLayout();
  const AnimSourceUncompressed* inputAnim = manager.getInputAnimation();
  NMP_VERIFY(inputAnim);
  uint32_t numChannelSets = inputAnim->getNumChannelSets();
  const std::vector<size_t>& sectionSizes = manager.getSectionSizes();
  NMP_VERIFY(sectionSizes.size() > 0);
  uint32_t numSections = (uint32_t) sectionSizes.size();

  //-----------------------
  // Header
  memLayout.m_headerOffset = 0;
  memReqs.size += NMP::Memory::align(sizeof(MR::AnimSourceMBA), NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // AnimSectionInfo array (start frames and section sizes)
  memLayout.m_sectionInfoOffset = memReqs.size;
  memReqs.size += (sizeof(MR::AnimSectionInfoMBA) * numSections);
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // AnimSectionMBA (section pointers array)
  memLayout.m_sectionPtrsOffset = memReqs.size;
  memReqs.size += (sizeof(MR::DataRef) * numSections);
  memReqs.size = NMP::Memory::align(memReqs.size, NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // The array of channel set default information
  memLayout.m_chanSetInfoOffset = memReqs.size;
  memReqs.size += (sizeof(MR::ChannelSetMBAInfo) * numChannelSets);
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
    memLayout.m_trajectoryMemReqs = TrajectorySourceMBABuilder::getMemoryRequirements(inputAnim);
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
void AnimSourceMBABuilder::init(
  const AnimSourceCompressorMBA& manager,
  NMP::Memory::Resource&         buffer)
{
  //-----------------------
  // Information
  const AnimSourceUncompressed* inputAnim = manager.getInputAnimation();
  NMP_VERIFY(inputAnim);
  const AnimSourceMBALayout& memLayout = manager.getAnimSourceMemLayout();
  AnimSourceMBABuilder* anim = (AnimSourceMBABuilder*)buffer.ptr;
  NMP_VERIFY(anim);
  NMP_VERIFY(NMP_IS_ALIGNED(buffer.ptr, NMP_VECTOR_ALIGNMENT));

  // Set the animation type.
  anim->m_animType = ANIM_TYPE_MBA;

  // Section information
  const std::vector<uint32_t>& sectionStartFrames = manager.getSectionStartFrames();
  const std::vector<uint32_t>& sectionEndFrames = manager.getSectionEndFrames();
  const std::vector<size_t>& sectionSizes = manager.getSectionSizes();

  //-----------------------
  // Animation information.
  uint32_t numChannelSets = inputAnim->getNumChannelSets();
  uint32_t numSections = (uint32_t)sectionSizes.size();
  const ChannelSetRequirements* chanSetReqs = inputAnim->getChannelSetRequirements();
  NMP_VERIFY(chanSetReqs);
  NMP_VERIFY(chanSetReqs->getNumChannelSets() == numChannelSets);

  //-----------------------
  // Header.
  uint8_t* data = (uint8_t*)buffer.ptr + memLayout.m_headerOffset;
  anim->m_funcTable = &MR::AnimSourceMBA::m_functionTable;
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
  // AnimSectionInfo array (start frames and section sizes)
  data = (uint8_t*)buffer.ptr + memLayout.m_sectionInfoOffset;
  anim->m_sectionsInfo = (MR::AnimSectionInfoMBA*)data;

  // Fill in section info data.
  for (uint32_t i = 0; i < numSections; ++i)
  {
    anim->m_sectionsInfo[i].setStartFrame(sectionStartFrames[i]);
    anim->m_sectionsInfo[i].setSectionSize((uint32_t)sectionSizes[i]);
  }

  //-----------------------
  // AnimSectionMBA (section pointers array)
  data = (uint8_t*)buffer.ptr + memLayout.m_sectionPtrsOffset;
  anim->m_sections = (MR::DataRef*) data;  // Values filled in later

  //-----------------------
  // The array of channel set default information
  data = (uint8_t*)buffer.ptr + memLayout.m_chanSetInfoOffset;
  anim->m_channelSetsInfo = (MR::ChannelSetMBAInfo*) data;

  //-----------------------
  // Compute the default information for each channel set in the animation
  const ChannelSetRequirements::channelSetReqs_t* chanReqs = chanSetReqs->getChannelSetRequirements();
  NMP_VERIFY(chanReqs);
  const ChannelSetTable* channelSets = inputAnim->getChannelSets();
  NMP_VERIFY(channelSets);

  for (uint32_t i = 0; i < numChannelSets; ++i)
  {
    // Default info
    ChannelSetMBAInfoBuilder::init(
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
    AnimSectionMBABuilder::init(
      inputAnim,
      (MR::AnimSectionMBA*) anim->m_sections[k].m_data,
      anim->m_channelSetsInfo,
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
    anim->m_trajectoryData = (MR::TrajectorySourceMBA*)data;
    TrajectorySourceMBABuilder::init(inputAnim, anim->m_trajectoryData, memLayout.m_trajectoryMemReqs);
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
// AnimSourceCompressorMBA
//----------------------------------------------------------------------------------------------------------------------
AnimSourceCompressorMBA::AnimSourceCompressorMBA()
{
}

//----------------------------------------------------------------------------------------------------------------------
AnimSourceCompressorMBA::~AnimSourceCompressorMBA()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimSourceCompressorMBA::computeSectionRequirements(
  uint32_t startFrame,
  uint32_t endFrame,
  size_t&  sectionSize) const
{
  NMP::Memory::Format memReqs;

  memReqs = AnimSectionMBABuilder::getSectionMemoryRequirements(m_inputAnimResampled, startFrame, endFrame);
  sectionSize = memReqs.size;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
size_t AnimSourceCompressorMBA::computeFinalMemoryRequirements() const
{
  NMP::Memory::Format memReqs;

  memReqs = AnimSourceMBABuilder::getMemoryRequirements(*this);

  return memReqs.size;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorMBA::buildAnimation(NMP::Memory::Resource& buffer) const
{
  AnimSourceMBABuilder::init(*this, buffer);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorMBA::testLocation(NMP::Memory::Resource& buffer) const
{
  MR::AnimSourceMBA* animSrc = (MR::AnimSourceMBA*)buffer.ptr;
  NMP_VERIFY(animSrc);
  uint8_t* src = (uint8_t*)buffer.ptr;

  // Create a memory copy of the source animation
  NMP::Memory::Resource memRes = NMPMemoryAllocateFromFormat(buffer.format);
  MR::AnimSourceMBA* animDst = (MR::AnimSourceMBA*)memRes.ptr;
  NMP::Memory::memcpy(animDst, animSrc, buffer.format.size);


  // Create temporary working memory buffer
  NMP::Memory::Resource memResTemp = NMPMemoryAllocateFromFormat(buffer.format);
  MR::AnimSourceMBA* animTemp = (MR::AnimSourceMBA*)memResTemp.ptr;

  //-----------------------
  // TEST location / dislocation

  // All pointers are valid in the memory space of the source buffer
  animSrc->dislocate();
  animSrc->locate();

#ifdef NMP_ENABLE_ASSERTS
  uint8_t* dst = (uint8_t*)memRes.ptr;
  // Compare the data to ensure that it has not changed
  for (uint32_t i = 0; i < buffer.format.size; ++i)
  {
    NMP_VERIFY(src[i] == dst[i]);
  }
#endif

  // Reset the source buffer
  NMP::Memory::memcpy(animSrc, animDst, buffer.format.size);

  //-----------------------
  // TEST relocate

  // Copy to new location
  NMP::Memory::memcpy(animTemp, animSrc, buffer.format.size);

  // Relocate in new memory
  animTemp->relocate();

  // Clear source memory
  for (uint32_t i = 0; i < buffer.format.size; ++i)
    src[i] = 0;

  // Copy back to old location
  NMP::Memory::memcpy(animSrc, animTemp, buffer.format.size);

  // Relocate in original memory
  animSrc->relocate();

#ifdef NMP_ENABLE_ASSERTS
  // Compare the data to ensure that it has not changed
  for (uint32_t i = 0; i < buffer.format.size; ++i)
  {
    NMP_VERIFY(src[i] == dst[i]);
  }
#endif

  // Reset the source buffer
  NMP::Memory::memcpy(animSrc, animDst, buffer.format.size);

  // Tidy up
  NMP::Memory::memFree(memRes.ptr);
  NMP::Memory::memFree(memResTemp.ptr);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorMBA::writeDebugMBAInfo(
  FILE* filePointer,
  NMP::Memory::Resource& buffer) const
{
  NMP_VERIFY(filePointer);
  MR::AnimSourceMBA* anim = (MR::AnimSourceMBA*)buffer.ptr;
  NMP_VERIFY(anim);

  // Information
  uint32_t numChannelSets = MR::AnimSourceMBA::getNumChannelSets(anim);
  uint32_t numSections = anim->getNumSections();
  fprintf(filePointer, "\n_______________ Information _______________\n");
  fprintf(filePointer, "Num channel sets = %d\n", numChannelSets);
  fprintf(filePointer, "Num sections = %d\n", numSections);

  // Channel set names
  fprintf(filePointer, "\n_______________ Channel Set Name Table _______________\n");
  const NMP::OrderedStringTable* names = MR::AnimSourceMBA::getChannelNameTable(anim);
  if (names)
  {
    uint32_t numChannelSets = names->getNumEntries();
    for (uint32_t i = 0; i < numChannelSets; ++i)
    {
      const char* name = names->getStringForID(i);
      NMP_VERIFY(name);

      fprintf(filePointer, "chan = %4d, name = %s\n", i, name);
    }
  }
  else
  {
    fprintf(filePointer, "Not stored\n");
  }

  // Channel set frame data
#ifdef NM_DEBUG
  uint32_t numFrames = m_inputAnimResampled->getMaxNumKeyframes();
  for (uint32_t i = 0; i < numFrames; ++i)
  {
    fprintf(filePointer, "\n_______________ Frame = %4d _______________\n", i);
    anim->debugOutputFullAnimTransformSet(i, 0.0f, filePointer);
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorMBA::onEndBuildAnimation(NMP::Memory::Resource& buffer)
{
#ifdef TEST_TRANSFORMS

  //-----------------------
  // TEST location / dislocation
  MR::AnimSourceMBA* anim = (MR::AnimSourceMBA*)buffer.ptr;
  NMP_VERIFY(anim);
  testLocation(buffer);

  //-----------------------
  // Basic animation information
  FILE* filePointer;
  filePointer = fopen("C:/anim_mba_info.txt", "w");
  NMP_VERIFY(filePointer);
  if (filePointer)
  {
    writeDebugMBAInfo(filePointer, buffer);
    fclose(filePointer);
  }
#else
  (void)buffer;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceCompressorMBA::buildRigToAnimMaps()
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
