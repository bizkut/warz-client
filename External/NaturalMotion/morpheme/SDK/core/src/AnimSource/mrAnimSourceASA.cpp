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
#include "morpheme/AnimSource/mrAnimSourceASA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

AnimFunctionTable AnimSourceASA::m_functionTable =
{
  AnimSourceASA::computeAtTime,
  AnimSourceASA::computeAtTimeSingleTransform,
  AnimSourceASA::getDuration,
  AnimSourceASA::getNumChannelSets,
  AnimSourceASA::getTrajectorySourceData,
  AnimSourceASA::getChannelNameTable
};

//----------------------------------------------------------------------------------------------------------------------
// AnimSourcASA functions.
//----------------------------------------------------------------------------------------------------------------------
AnimSourceASA::AnimSourceASA()
{
  // This constructor should never be called at runtime.
  NMP_ASSERT_FAIL();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceASA::computeAtTime(
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
  const AnimSourceASA* source;
  uint32_t             sectionIndex;
  AnimSectionASA*      section;
  uint32_t             sectionFrameIndex;
  AnimSectionInfoASA*  sectionInfo;

  NMP_ASSERT(sourceAnimation);
  source = static_cast<const AnimSourceASA*> (sourceAnimation);

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
  sectionInfo       = &(source->m_sectionsInfo[sectionIndex]);
  sectionFrameIndex = animFrameIndex - sectionInfo->getStartFrame();
#ifdef NM_HOST_CELL_SPU

  // Allocate just the header of the section
  section = (AnimSectionASA*) source->m_sections[sectionIndex].getData(
              NMP::Memory::Format(NMP::Memory::align(source->m_sectionsInfo[sectionIndex].getHeaderSize(), NMP_VECTOR_ALIGNMENT), NMP_VECTOR_ALIGNMENT),
              allocator);
  section->relocate();
#else
  section = (AnimSectionASA*) source->m_sections[sectionIndex].getData(
              NMP::Memory::Format(source->m_sectionsInfo[sectionIndex].getSectionSize(), NMP_VECTOR_ALIGNMENT),
              allocator);
#endif // NM_HOST_CELL_SPU
  NMP_ASSERT(section);

  if (outputSubsetSize > 0)
  {
    section->computeAnimTransformSubSet(
      source->m_channelSetsInfo,
      mapFromRigToAnim,
      outputSubsetSize,
      outputSubsetArray,
      sectionFrameIndex,
      interpolant,
      allocator,
      outputTransformBuffer);
  }
  else
  {
    section->computeFullAnimTransformSet(
      source->m_channelSetsInfo,
      mapFromRigToAnim,
      sectionFrameIndex,
      interpolant,
      allocator,
      outputTransformBuffer);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Samples the request source animation channel at the specified time through its playback duration.
void AnimSourceASA::computeAtTimeSingleTransform(
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
  const AnimSourceASA* source;
  uint32_t             sectionIndex;
  AnimSectionASA*      section;
  uint32_t             sectionFrameIndex;
  AnimSectionInfoASA*  sectionInfo;

  NMP_ASSERT(pos && quat);
  NMP_ASSERT(sourceAnimation);
  source = static_cast<const AnimSourceASA*> (sourceAnimation);
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
  section = (AnimSectionASA*) source->m_sections[sectionIndex].getData(
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
    allocator,
    quat,
    pos);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSourceASA::findSectionIndexFromFrameIndex(uint32_t frameIndex) const
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

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU

void AnimSourceASA::locate()
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
  data += sizeof(AnimSourceASA);
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_sectionsInfo = (AnimSectionInfoASA*) data;
  for (i = 0; i < m_numSections; ++i)
    m_sectionsInfo[i].locate();
  data += (sizeof(AnimSectionInfoASA) * m_numSections);

  // Section pointers array.
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_sections = (DataRef*) data;
  data += (sizeof(DataRef) * m_numSections);

  // Locate the array of channel set defaults information.
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_channelSetsInfo = (ChannelSetASAInfo*) data;
  data += (sizeof(ChannelSetASAInfo) * m_numChannelSets);

  // Locate each channels default info.
  for (i = 0; i < m_numChannelSets; ++i)
  {
    m_channelSetsInfo[i].locate();
  }

  // Locate each section.
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  for (i = 0; i < m_numSections; ++i)
  {
    AnimSectionASA* section = (AnimSectionASA*) data;
    m_sections[i].m_data = (void*) section;
    NMP_ASSERT(((size_t)(m_sections[i].m_data) % NMP_VECTOR_ALIGNMENT) == 0);
    section->locate(section);
    data += (m_sectionsInfo[i]).getSectionSize();
    data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  }

  // Trajectory data.
  NMP::endianSwap(m_trajectoryData);
  if (m_trajectoryData)
  {
    data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
    m_trajectoryData = (TrajectorySourceASA*) data;
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
void AnimSourceASA::dislocate()
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
    UNFIX_SWAP_PTR(TrajectorySourceASA, m_trajectoryData);
  }

  // Dislocate each section.
  for (i = 0; i < m_numSections; ++i)
  {
    AnimSectionASA* section = (AnimSectionASA*) m_sections[i].m_data;
    section->dislocate();
    UNFIX_SWAP_PTR(void, m_sections[i].m_data);
  }
  UNFIX_SWAP_PTR(DataRef, m_sections);

  // Dislocate each channels defaults info.
  for (i = 0; i < m_numChannelSets; ++i)
  {
    m_channelSetsInfo[i].dislocate();
  }
  UNFIX_SWAP_PTR(ChannelSetASAInfo, m_channelSetsInfo);

  // Dislocate each section info structures.
  for (i = 0; i < m_numSections; ++i)
  {
    m_sectionsInfo[i].dislocate();
  }
  UNFIX_SWAP_PTR(AnimSectionInfoASA, m_sectionsInfo);

  NMP::endianSwap(m_duration);
  NMP::endianSwap(m_sampleFrequency);
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_numSections);
  AnimSourceBase::dislocate();
}

#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceASA::relocate()
{
  // Anim class itself.
  uint8_t* data = (uint8_t*)this;
  NMP_ASSERT(NMP_IS_ALIGNED(data, NMP_VECTOR_ALIGNMENT));
  data += sizeof(AnimSourceASA);

  // AnimSectionInfo array (start frames and section sizes).
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_sectionsInfo = (AnimSectionInfoASA*)data;
  data += (sizeof(AnimSectionInfoASA) * m_numSections);

  // Section pointers array.
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_sections = (MR::DataRef*)data;
  data += (sizeof(DataRef) * m_numSections);

  // The array of channel set compression information (quantisation scales and offsets).
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_channelSetsInfo = (MR::ChannelSetASAInfo*)data;

  // Skip the actual section data

  // Trajectory data

  // Channel name table
}

} // namespace MR

//---------------------------------------------------------------------------------------------------------------------- -------------------------------------------------------
