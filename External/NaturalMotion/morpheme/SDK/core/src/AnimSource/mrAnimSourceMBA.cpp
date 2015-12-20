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
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMemory.h"
#include "morpheme/AnimSource/mrAnimSourceMBA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

AnimFunctionTable AnimSourceMBA::m_functionTable =
{
  AnimSourceMBA::computeAtTime,
  AnimSourceMBA::computeAtTimeSingleTransform,
  AnimSourceMBA::getDuration,
  AnimSourceMBA::getNumChannelSets,
  AnimSourceMBA::getTrajectorySourceData,
  AnimSourceMBA::getChannelNameTable
};

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceMBA functions.
//----------------------------------------------------------------------------------------------------------------------
AnimSourceMBA::AnimSourceMBA()
{
  // This constructor should never be called at runtime.
  NMP_ASSERT_FAIL();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceMBA::computeAtTime(
  const AnimSourceBase* sourceAnimation,        ///< IN: Animation to sample.
  float                 time,                   ///< IN: Time at which to sample the animation (seconds).
  const AnimRigDef*     NMP_UNUSED(rig),        ///< IN: Hierarchy and bind poses
  const RigToAnimMap*   mapFromRigToAnim,       ///< IN: So that results from sampled anim channels can be
                                                ///<     stored in the correct rig bone ordered transform buffer slot.
  uint32_t              outputSubsetSize,       ///< IN: \see outputSubsetArray
  const uint16_t*       outputSubsetArray,      ///< IN: Channel set values will only be calculated if
                                                ///<     output indexes appear both in this array and the
                                                ///<     mapping array. Ignored if size is zero.
  NMP::DataBuffer*      outputTransformBuffer,  ///< OUT: Calculated transform data is stored and returned
                                                ///<      in this provided set.
  NMP::MemoryAllocator* allocator)
{
  float                frame;
  float                interpolant;
  uint32_t             animFrameIndex;
  const AnimSourceMBA* source;
  uint32_t             sectionIndex;
  AnimSectionMBA*      section;
  uint32_t             sectionFrameIndex;
  AnimSectionInfoMBA*  sectionInfo;

  NMP_ASSERT(sourceAnimation);
  source = static_cast<const AnimSourceMBA*> (sourceAnimation);

  NMP_ASSERT(mapFromRigToAnim);
  NMP_ASSERT(outputTransformBuffer);
  NMP_ASSERT(time >= 0.0 && time <= source->m_duration);

  // Calculate frame index and interpolant.
  frame = time * source->m_sampleFrequency;
  animFrameIndex = (uint32_t) frame;
  interpolant = frame - animFrameIndex;
  if (interpolant < ERROR_LIMIT)
    interpolant = 0.0f;

  // Find which section this frame lies within.
  sectionIndex = source->findSectionIndexFromFrameIndex(animFrameIndex);
  NMP_ASSERT(sectionIndex < source->m_numSections);
  section = (AnimSectionMBA*) source->m_sections[sectionIndex].getData(
              NMP::Memory::Format(source->m_sectionsInfo[sectionIndex].getSectionSize(), NMP_VECTOR_ALIGNMENT),
              allocator);
  NMP_ASSERT(section);
#ifdef NM_HOST_CELL_SPU
  section->relocate();
#endif // NM_HOST_CELL_SPU

  sectionInfo = &(source->m_sectionsInfo[sectionIndex]);
  sectionFrameIndex = animFrameIndex - sectionInfo->getStartFrame();

  if (outputSubsetSize > 0)
  {
    section->computeAnimTransformSubSet(
      source->m_channelSetsInfo,
      mapFromRigToAnim,
      outputSubsetSize,
      outputSubsetArray,
      sectionFrameIndex,
      interpolant,
      outputTransformBuffer);
  }
  else
  {
    section->computeFullAnimTransformSet(
      source->m_channelSetsInfo,
      mapFromRigToAnim,
      sectionFrameIndex,
      interpolant,
      outputTransformBuffer);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Samples the request source animation channel at the specified time through its playback duration.
void AnimSourceMBA::computeAtTimeSingleTransform(
  const AnimSourceBase* sourceAnimation,        ///< IN: Animation to sample.
  float                 time,                   ///< IN: Time at which to sample the animation (seconds).
  uint32_t              rigChannelIndex,        ///< IN: Index of the rig bone to evaluate
  const AnimRigDef*     NMP_UNUSED(rig),        ///< IN: Hierarchy and bind poses
  const RigToAnimMap*   mapFromRigToAnim,       ///< IN: So that results from sampled anim channels can be
                                                ///<     stored in the correct rig bone ordered transform buffer slot.
  NMP::Vector3*         pos,                    ///< OUT: The resulting position is filled in here.
  NMP::Quat*            quat,                   ///< OUT: The resulting orientation is filled in here.
  NMP::MemoryAllocator* allocator)
{
  float                frame;
  float                interpolant;
  uint16_t             animChannelIndex;
  uint32_t             animFrameIndex;
  const AnimSourceMBA* source;
  uint32_t             sectionIndex;
  AnimSectionMBA*      section;
  uint32_t             sectionFrameIndex;
  AnimSectionInfoMBA*  sectionInfo;

  NMP_ASSERT(pos && quat);
  NMP_ASSERT(sourceAnimation);
  source = static_cast<const AnimSourceMBA*> (sourceAnimation);
  NMP_ASSERT(time >= 0.0 && time <= source->m_duration);
  
  NMP_ASSERT(mapFromRigToAnim->getRigToAnimMapType() == RigToAnimMap::MapPairs);
  const RigToAnimEntryMap* rigToAnimMapData = (const RigToAnimEntryMap*) mapFromRigToAnim->getRigToAnimMapData();
  NMP_ASSERT(rigToAnimMapData);

  // Get the corresponding animation channel index
  animChannelIndex = 0;
#ifdef NMP_ENABLE_ASSERTS
  bool status =
#endif
    rigToAnimMapData->getAnimIndexForRigIndex((uint16_t)rigChannelIndex, animChannelIndex);
  NMP_ASSERT(status);

  // Calculate frame index and interpolant.
  frame = time * source->m_sampleFrequency;
  animFrameIndex = (uint32_t) frame;
  interpolant = frame - animFrameIndex;
  if (interpolant < ERROR_LIMIT)
    interpolant = 0.0f;

  // Find which section this frame lies within.
  sectionIndex = source->findSectionIndexFromFrameIndex(animFrameIndex);
  NMP_ASSERT(sectionIndex < source->m_numSections);
  section = (AnimSectionMBA*) source->m_sections[sectionIndex].getData(
              NMP::Memory::Format(source->m_sectionsInfo[sectionIndex].getSectionSize(), NMP_VECTOR_ALIGNMENT),
              allocator);
  NMP_ASSERT(section);
#ifdef NM_HOST_CELL_SPU
  section->relocate();
#endif // NM_HOST_CELL_SPU

  sectionInfo = &(source->m_sectionsInfo[sectionIndex]);
  sectionFrameIndex = animFrameIndex - sectionInfo->getStartFrame();

  section->computeSingleChannelTransform(
    source->m_channelSetsInfo,
    animChannelIndex,
    sectionFrameIndex,
    interpolant,
    quat,
    pos);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSourceMBA::findSectionIndexFromFrameIndex(uint32_t frameIndex) const
{
  uint32_t i;

  NMP_ASSERT(m_numSections);
  for (i = 0; i < m_numSections - 1; ++i)
  {
    if (frameIndex < m_sectionsInfo[i + 1].getStartFrame())
    {
      return i;
    }
  }

  return m_numSections - 1;
}

#if !defined(NM_HOST_CELL_SPU) && !defined(NM_HOST_CELL_PPU)
  #ifdef NM_DEBUG
//----------------------------------------------------------------------------------------------------------------------
void AnimSourceMBA::debugOutputFullAnimTransformSet(
  const uint32_t   animFrameIndex,
  const float      interpolant,
  FILE*            file) const
{
  uint32_t              sectionIndex;
  AnimSectionMBA*        section;
  uint32_t              sectionFrameIndex;
  AnimSectionInfoMBA*    sectionInfo;

  // Find which section this frame lies within.
  sectionIndex = findSectionIndexFromFrameIndex(animFrameIndex);
  NMP_ASSERT(sectionIndex < m_numSections);
  section = (AnimSectionMBA*) m_sections[sectionIndex].m_data;
  NMP_ASSERT(section);
  sectionInfo = &(m_sectionsInfo[sectionIndex]);
  sectionFrameIndex = animFrameIndex - sectionInfo->getStartFrame();

  // Tell the section to print out a set of key frame values.
  section->debugOutputFullAnimTransformSet(m_channelSetsInfo, sectionFrameIndex, interpolant, file);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceMBA::debugOutputInfo(FILE* file) const
{
  uint32_t          i, k;
  uint32_t          posKeyFrameTotal;
  uint32_t          quatKeyFrameTotal;
  AnimSectionMBA*   section;

  NMP_ASSERT(file);

  fprintf(file, "Duration: %3.5f Num Chan Sets: %5d, Sample Freq %3.2f ",
          m_duration, m_numChannelSets, m_sampleFrequency);

  for (i = 0; i < m_numChannelSets; ++i)
  {
    posKeyFrameTotal = 0;
    quatKeyFrameTotal = 0;
    for (k = 0; k < m_numSections; ++k)
    {
      section = (AnimSectionMBA*) m_sections[k].m_data;
      NMP_ASSERT(section);
      posKeyFrameTotal += section->getPosChanFrameCount(i);
      quatKeyFrameTotal += section->getQuatChanFrameCount(i);
    }

    fprintf(file, "Chan: %5d, Num Translation Keys: %5d, Num Rotation Keys: %5d\n", i, posKeyFrameTotal, quatKeyFrameTotal);
  }
}
  #endif // NM_DEBUG
#endif //!defined(NM_HOST_CELL_SPU) && !defined(NM_HOST_CELL_PPU)

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU

void AnimSourceMBA::locate()
{
  uint32_t i;
  uint8_t*  data;

  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Animation sources must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  AnimSourceBase::locate();
  NMP::endianSwap(m_duration);
  NMP::endianSwap(m_sampleFrequency);
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_numSections);

  // Sections information.
  data = (uint8_t*) this;
  data += sizeof(AnimSourceMBA);
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_sectionsInfo = (AnimSectionInfoMBA*) data;
  for (i = 0; i < m_numSections; ++i)
    m_sectionsInfo[i].locate();
  data += (sizeof(AnimSectionInfoMBA) * m_numSections);

  // Section pointers array.
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_sections = (DataRef*) data;
  data += (sizeof(DataRef) * m_numSections);

  // Locate the array of channel set defaults information.
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_channelSetsInfo = (ChannelSetMBAInfo*) data;
  data += (sizeof(ChannelSetMBAInfo) * m_numChannelSets);

  // Locate each channels default info.
  for (i = 0; i < m_numChannelSets; ++i)
  {
    m_channelSetsInfo[i].locate();
  }

  // Locate each section.
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  for (i = 0; i < m_numSections; ++i)
  {
    AnimSectionMBA* section = (AnimSectionMBA*) data;
    m_sections[i].m_data = (void*) section;
    NMP_ASSERT(((size_t)(m_sections[i].m_data) % NMP_VECTOR_ALIGNMENT) == 0);
    section->locate(&data);
    data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  }

  // Trajectory data.
  NMP::endianSwap(m_trajectoryData);
  if (m_trajectoryData)
  {
    data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
    m_trajectoryData = (TrajectorySourceMBA*) data;
    m_trajectoryData->locate();
  }

  // Channel name table
  if (m_channelNames)
  {
    REFIX_SWAP_PTR(NMP::OrderedStringTable, m_channelNames);
    m_channelNames->locate();
  }

  // Initialise the function pointer table.
  m_funcTable = &m_functionTable;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceMBA::dislocate()
{
  uint32_t i;

  // Channel name table
  if (m_channelNames)
  {
    m_channelNames->dislocate();
    UNFIX_SWAP_PTR(NMP::OrderedStringTable, m_channelNames);
  }

  // Trajectory data.
  if (m_trajectoryData)
  {
    m_trajectoryData->dislocate();
    UNFIX_SWAP_PTR(TrajectorySourceMBA, m_trajectoryData);
  }

  // Dislocate each section.
  for (i = 0; i < m_numSections; ++i)
  {
    AnimSectionMBA* section = (AnimSectionMBA*) m_sections[i].m_data;
    section->dislocate();
    UNFIX_SWAP_PTR(void, m_sections[i].m_data);
  }
  UNFIX_SWAP_PTR(DataRef, m_sections);

  // Dislocate each channels defaults info.
  for (i = 0; i < m_numChannelSets; ++i)
  {
    m_channelSetsInfo[i].dislocate();
  }
  UNFIX_SWAP_PTR(ChannelSetMBAInfo, m_channelSetsInfo);

  // Dislocate each section info structures.
  for (i = 0; i < m_numSections; ++i)
  {
    m_sectionsInfo[i].dislocate();
  }
  UNFIX_SWAP_PTR(AnimSectionInfoMBA, m_sectionsInfo);

  NMP::endianSwap(m_duration);
  NMP::endianSwap(m_sampleFrequency);
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_numSections);
  AnimSourceBase::dislocate();
}

#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceMBA::relocate()
{
  // Anim class itself.
  uint8_t* data = (uint8_t*)this;
  NMP_ASSERT(NMP_IS_ALIGNED(data, NMP_VECTOR_ALIGNMENT));
  data += sizeof(AnimSourceMBA);

  // AnimSectionInfo array (start frames and section sizes).
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_sectionsInfo = (AnimSectionInfoMBA*)data;
  data += (sizeof(AnimSectionInfoMBA) * m_numSections);

  // Section pointers array.
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_sections = (MR::DataRef*)data;
  data += (sizeof(DataRef) * m_numSections);

  // The array of channel set compression information (quantisation scales and offsets).
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_channelSetsInfo = (MR::ChannelSetMBAInfo*)data;
  data += (sizeof(ChannelSetMBAInfo) * m_numChannelSets);

  // Skip the actual section data
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);

  // Trajectory data

  // Channel name table
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
