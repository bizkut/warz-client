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
#include "NMPlatform/NMvpu.h"
#include "NMPlatform/NMProfiler.h"
#include "morpheme/AnimSource/mrAnimSourceNSA.h"

#ifdef NM_HOST_CELL_SPU
#include "NMPlatform/ps3/NMSPUDMAController.h"
#endif // NM_HOST_CELL_SPU

#define NM_ANIM_PROFILING_NSA_INTERNALx

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

AnimFunctionTable AnimSourceNSA::m_functionTable =
{
  AnimSourceNSA::computeAtTime,
  AnimSourceNSA::computeAtTimeSingleTransform,
  AnimSourceNSA::getDuration,
  AnimSourceNSA::getNumChannelSets,
  AnimSourceNSA::getTrajectoryChannelData,
  AnimSourceNSA::getChannelNameTable
};

//----------------------------------------------------------------------------------------------------------------------
// AnimSourceNSA functions.
//----------------------------------------------------------------------------------------------------------------------
AnimSourceNSA::AnimSourceNSA()
{
  // This constructor should never be called at runtime.
  NMP_ASSERT_FAIL();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceNSA::computeAtTime(
  const AnimSourceBase* sourceAnimation,        // IN: Animation to sample.
  float                 time,                   // IN: Time at which to sample the animation (seconds).
  const AnimRigDef*     NMP_UNUSED(rig),        // IN: Hierarchy and bind poses
  const RigToAnimMap*   mapFromRigToAnim,       // IN: Mapping between rig bones and animation channels for the anim set.
  uint32_t              outputSubsetSize,       // IN: \see outputSubsetArray
  const uint16_t*       outputSubsetArray,      // IN: Channel set values will only be calculated if
                                                //     output indexes appear both in this array and the
                                                //     mapping array. Ignored if size is zero.
  NMP::DataBuffer*      outputTransformBuffer,  // OUT: Calculated transform data is stored and returned
                                                //      in this provided set.
  NMP::MemoryAllocator* allocator)              // IN: Memory allocator used for temporary storage.
{
  NMP_ASSERT(sourceAnimation);
  NMP_ASSERT(mapFromRigToAnim);
  NMP_ASSERT(outputTransformBuffer);

  // Partial channel decompression is not yet supported for this animation format
  NMP_ASSERT(outputSubsetSize == 0);
  (void)outputSubsetSize;
  (void)outputSubsetArray;

  const AnimSourceNSA* compressedSource = static_cast<const AnimSourceNSA*> (sourceAnimation);
  NMP_ASSERT(time >= 0.0 && time <= compressedSource->m_duration);
  
  NMP_ASSERT(mapFromRigToAnim->getRigToAnimMapType() == RigToAnimMap::AnimToRig);
  const AnimToRigTableMap* animToRigTableMap = (const AnimToRigTableMap*) mapFromRigToAnim->getRigToAnimMapData();
  NMP_ASSERT(animToRigTableMap);

  NM_ANIM_BEGIN_PROFILING("NSA_DECOMPRESS");

  //--------------------------------
  // Decompress the unchanging compression channels
  const UnchangingDataNSA* unchangingData = compressedSource->m_unchangingData;
  NMP_ASSERT(unchangingData);

#ifdef NM_ANIM_PROFILING_NSA_INTERNAL
  NM_ANIM_BEGIN_PROFILING("NSA_UNCHANGING_CHANS");
#endif

  //--------------------------------
  // Unchanging pos
#ifdef NM_ANIM_PROFILING_NSA_INTERNAL
  NM_ANIM_BEGIN_PROFILING("NSA_UNCHANGING_POS");
#endif
  NMP_ASSERT(compressedSource->m_unchangingPosCompToAnimMap);

  unchangingData->unchangingPosDecompress(
    animToRigTableMap,
    compressedSource->m_unchangingPosCompToAnimMap,
    outputTransformBuffer);

#ifdef NM_ANIM_PROFILING_NSA_INTERNAL
  NM_ANIM_END_PROFILING(); // NSA_UNCHANGING_POS
#endif

  //--------------------------------
  // Unchanging quat
#ifdef NM_ANIM_PROFILING_NSA_INTERNAL
  NM_ANIM_BEGIN_PROFILING("NSA_UNCHANGING_QUAT");
#endif
  NMP_ASSERT(compressedSource->m_unchangingQuatCompToAnimMap);

  unchangingData->unchangingQuatDecompress(
    animToRigTableMap,
    compressedSource->m_unchangingQuatCompToAnimMap,
    outputTransformBuffer);

#ifdef NM_ANIM_PROFILING_NSA_INTERNAL
  NM_ANIM_END_PROFILING(); // NSA_UNCHANGING_QUAT
  NM_ANIM_END_PROFILING(); // NSA_UNCHANGING_CHANS
#endif

  //--------------------------------
  // Decompress the sampled compression channels 
  uint32_t numSectionEntries = compressedSource->m_numFrameSections * compressedSource->m_numChannelSections;
  if (numSectionEntries > 0)
  {
#ifdef NM_ANIM_PROFILING_NSA_INTERNAL
    NM_ANIM_BEGIN_PROFILING("NSA_SAMPLED_CHANS");
#endif

    // Calculate frame index and interpolant.
    float frame = time * compressedSource->m_sampleFrequency;
    uint32_t animFrameIndex = (uint32_t)frame;
    NMP_ASSERT(animFrameIndex <= compressedSource->m_sectionStartFrames[compressedSource->m_numFrameSections]);
    float interpolant = frame - animFrameIndex;
    
    // Find which section this frame lies within.    
    uint32_t sectionIndex = compressedSource->findSectionIndexFromFrameIndex(animFrameIndex);
    NMP_ASSERT(sectionIndex < compressedSource->m_numFrameSections);
    
    // Ensure that we don't interpolate beyond the last section frame
    if (animFrameIndex == compressedSource->m_sectionStartFrames[sectionIndex + 1])
    {
      animFrameIndex--;
      interpolant = 1.0f;
    }

    // Compute the sample frame relative to the section start frame
    uint32_t sectionAnimFrameIndex = animFrameIndex - compressedSource->m_sectionStartFrames[sectionIndex];

    //--------------------------------  
    const SectionDataNSA* sectionData;
    uint32_t sectionFrameOffset = sectionIndex * compressedSource->m_numChannelSections;
    const DataRef* sectionDataRef = &compressedSource->m_sectionData[sectionFrameOffset];

#ifdef NM_HOST_CELL_SPU
    const uint32_t* sectionSizes = &compressedSource->m_sectionSizes[sectionFrameOffset];

    // Allocate local memory to store the referenced data in.
    NMP::Memory::Format memReqsSectionData(compressedSource->m_maxSectionSize, NMP_VECTOR_ALIGNMENT);
    NMP::Memory::Resource memResSectionData = NMPAllocatorAllocateFromFormat(allocator, memReqsSectionData);
    NMP_SPU_ASSERT(memResSectionData.ptr != NULL);
    sectionData = (const SectionDataNSA*) memResSectionData.ptr;
#endif

    // Decompress all channel-wise sections    
    for (uint32_t iChanSection = 0; iChanSection < compressedSource->m_numChannelSections; ++iChanSection)
    {
      //--------------------------------
      // Get the section data block
#ifdef NM_HOST_CELL_SPU
      // DMA section data from system mem to locally allocated memory
      NMP::SPUDMAController::dmaGet(
        memResSectionData.ptr, // Where in local memory
        (uint32_t)sectionDataRef[iChanSection].m_data, // Where in system memory
        sectionSizes[iChanSection]); // Size of memory block

      // Wait for transfer of section data to complete
      NMP::SPUDMAController::dmaWaitAll();
      
      // Relocate the section data in local memory
      ((SectionDataNSA*)(memResSectionData.ptr))->relocate();
#else
      (void)allocator; // Unused
      sectionData = (const SectionDataNSA*) sectionDataRef[iChanSection].m_data;
#endif

      //--------------------------------
      // Decompress the sampled pos channels in the section
#ifdef NM_ANIM_PROFILING_NSA_INTERNAL
      NM_ANIM_BEGIN_PROFILING("NSA_SAMPLED_POS");
#endif
      const CompToAnimChannelMap* sampledPosCompToAnimMap = compressedSource->m_sampledPosCompToAnimMaps[iChanSection];
      NMP_ASSERT(sampledPosCompToAnimMap);

      sectionData->sampledPosDecompress(
        compressedSource->m_posMeansQuantisationInfo,
        compressedSource->m_sampledPosQuantisationInfo,
        animToRigTableMap,
        sampledPosCompToAnimMap,
        sectionAnimFrameIndex,
        interpolant,
        outputTransformBuffer);

#ifdef NM_ANIM_PROFILING_NSA_INTERNAL
      NM_ANIM_END_PROFILING(); // NSA_SAMPLED_POS
#endif

      //--------------------------------
      // Decompress the sampled quat channels in the section
#ifdef NM_ANIM_PROFILING_NSA_INTERNAL
      NM_ANIM_BEGIN_PROFILING("NSA_SAMPLED_QUAT");
#endif

      const CompToAnimChannelMap* sampledQuatCompToAnimMap = compressedSource->m_sampledQuatCompToAnimMaps[iChanSection];
      NMP_ASSERT(sampledQuatCompToAnimMap);

      sectionData->sampledQuatDecompress(
        compressedSource->m_sampledQuatQuantisationInfo,
        animToRigTableMap,
        sampledQuatCompToAnimMap,
        sectionAnimFrameIndex,
        interpolant,
        outputTransformBuffer);

#ifdef NM_ANIM_PROFILING_NSA_INTERNAL
      NM_ANIM_END_PROFILING(); // NSA_SAMPLED_QUAT
#endif
    }

#ifdef NM_HOST_CELL_SPU
    allocator->memFree(memResSectionData.ptr); // Tidy up
#endif

#ifdef NM_ANIM_PROFILING_NSA_INTERNAL
    NM_ANIM_END_PROFILING(); // NSA_SAMPLED_CHANS
#endif
  }

  //--------------------------------
  // Set the channel used flags required for this animation rig. Make sure the first entry, which is
  // not specified in animation files, is initialised to the identity and flagged as used.
  outputTransformBuffer->getPosQuatChannelPos(0)->setToZero();
  outputTransformBuffer->getPosQuatChannelQuat(0)->identity();
  outputTransformBuffer->padOutPosQuat();
  outputTransformBuffer->getUsedFlags()->copy(mapFromRigToAnim->getUsedFlags());
  outputTransformBuffer->setChannelUsed(0);
  outputTransformBuffer->calculateFullFlag();
  NMP_ASSERT(outputTransformBuffer->isFull() == (mapFromRigToAnim->getNumRigBones() - animToRigTableMap->getNumUsedEntries() <= 1));

  NM_ANIM_END_PROFILING(); // NSA_DECOMPRESS
}

//----------------------------------------------------------------------------------------------------------------------
// Samples the request source animation channel at the specified time through its playback duration.
void AnimSourceNSA::computeAtTimeSingleTransform(
  const AnimSourceBase* NMP_UNUSED(sourceAnimation),  // IN: Animation to sample.
  float                 NMP_UNUSED(time),             // IN: Time at which to sample the animation (seconds).
  uint32_t              NMP_UNUSED(rigChannelIndex),  // IN: Index of the rig bone to evaluate
  const AnimRigDef*     NMP_UNUSED(rig),              // IN: Hierarchy and bind poses
  const RigToAnimMap*   NMP_UNUSED(mapFromRigToAnim), // IN: Mapping between rig bones and animation channels for the anim set.
  NMP::Vector3*         NMP_UNUSED(pos),              // OUT: The resulting position is filled in here.
  NMP::Quat*            NMP_UNUSED(quat),             // OUT: The resulting orientation is filled in here.
  NMP::MemoryAllocator* NMP_UNUSED(allocator))        // IN: Memory allocator used for temporary storage.
{
  NMP_ASSERT_FAIL(); // Not supported
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AnimSourceNSA::locate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Animation sources must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Header information
  AnimSourceBase::locate();
  m_funcTable = &m_functionTable;
  NMP::endianSwap(m_duration);
  NMP::endianSwap(m_sampleFrequency);
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_numFrameSections);
  NMP::endianSwap(m_numChannelSections);
  uint32_t numSectionEntries = m_numFrameSections * m_numChannelSections;

  //-----------------------
  // Compression to animation channel maps
  NMP::endianSwap(m_maxNumCompChannels);

  REFIX_SWAP_PTR(MR::CompToAnimChannelMap, m_unchangingPosCompToAnimMap);
  m_unchangingPosCompToAnimMap->locate();
  REFIX_SWAP_PTR(MR::CompToAnimChannelMap, m_unchangingQuatCompToAnimMap);
  m_unchangingQuatCompToAnimMap->locate();

  //-----------------------
  if (numSectionEntries > 0)
  {
    // Pointers table to sub-section CAMs
    REFIX_SWAP_PTR(MR::CompToAnimChannelMap*, m_sampledPosCompToAnimMaps);
    for (uint32_t i = 0; i < m_numChannelSections; ++i)
    {
      REFIX_SWAP_PTR(MR::CompToAnimChannelMap, m_sampledPosCompToAnimMaps[i]);
    }
    REFIX_SWAP_PTR(MR::CompToAnimChannelMap*, m_sampledQuatCompToAnimMaps);
    for (uint32_t i = 0; i < m_numChannelSections; ++i)
    {
      REFIX_SWAP_PTR(MR::CompToAnimChannelMap, m_sampledQuatCompToAnimMaps[i]);
    }

    // Sub-section CAMs themselves
    for (uint32_t i = 0; i < m_numChannelSections; ++i)
      m_sampledPosCompToAnimMaps[i]->locate();

    for (uint32_t i = 0; i < m_numChannelSections; ++i)
      m_sampledQuatCompToAnimMaps[i]->locate();

    //-----------------------
    // Quantisation scale and offset information (Common to all sections)
    m_posMeansQuantisationInfo.locate();

    NMP::endianSwap(m_sampledPosNumQuantisationSets);
    if (m_sampledPosNumQuantisationSets > 0)
    {
      REFIX_SWAP_PTR(QuantisationScaleAndOffsetVec3, m_sampledPosQuantisationInfo);
      for (uint32_t i = 0; i < m_sampledPosNumQuantisationSets; ++i)
      {
        m_sampledPosQuantisationInfo[i].locate();
      }
    }

    NMP::endianSwap(m_sampledQuatNumQuantisationSets);
    if (m_sampledQuatNumQuantisationSets > 0)
    {
      REFIX_SWAP_PTR(QuantisationScaleAndOffsetVec3, m_sampledQuatQuantisationInfo);
      for (uint32_t i = 0; i < m_sampledQuatNumQuantisationSets; ++i)
      {
        m_sampledQuatQuantisationInfo[i].locate();
      }
    }

    //-----------------------
    // Sectioning information
    NMP::endianSwap(m_maxSectionSize);
    REFIX_SWAP_PTR(uint32_t, m_sectionStartFrames);
    NMP::endianSwapArray(m_sectionStartFrames, m_numFrameSections + 1);

    REFIX_SWAP_PTR(uint32_t, m_sectionSizes);
    NMP::endianSwapArray(m_sectionSizes, numSectionEntries);

    // Sections DataRef grid (pointers to section data packets)
    REFIX_SWAP_PTR(MR::DataRef, m_sectionData);
  }
  
  //-----------------------
  // Unchanging channel set data
  NMP_ASSERT(m_unchangingData);
  REFIX_SWAP_PTR(UnchangingDataNSA, m_unchangingData);
  m_unchangingData->locate();
  
  //-----------------------
  // Section data (DMA alignment)
  for (uint32_t i = 0; i < numSectionEntries; ++i)
  {
    REFIX_SWAP_PTR(void, m_sectionData[i].m_data);
    SectionDataNSA* sectionData = (SectionDataNSA*) m_sectionData[i].m_data;
    NMP_ASSERT(sectionData);
    sectionData->locate();
  }

  //-----------------------
  // Trajectory (DMA alignment)
  if (m_trajectoryData.m_data)
  {
    REFIX_SWAP_PTR(void, m_trajectoryData.m_data);
    TrajectorySourceNSA* trajectoryData = (TrajectorySourceNSA*) m_trajectoryData.m_data;
    trajectoryData->locate();
  }

  //-----------------------
  // Channel name table (DMA alignment)
  if (m_channelNames)
  {
    REFIX_SWAP_PTR(NMP::OrderedStringTable, m_channelNames);
    m_channelNames->locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceNSA::dislocate()
{
  uint32_t numSectionEntries = m_numFrameSections * m_numChannelSections;

  //-----------------------
  // Channel name table
  if (m_channelNames)
  {
    m_channelNames->dislocate();
    UNFIX_SWAP_PTR(NMP::OrderedStringTable, m_channelNames);
  }

  //-----------------------
  // Trajectory (DMA alignment)
  if (m_trajectoryData.m_data)
  {
    TrajectorySourceNSA* trajectoryData = (TrajectorySourceNSA*) m_trajectoryData.m_data;
    trajectoryData->dislocate();
    UNFIX_SWAP_PTR(void, m_trajectoryData.m_data);
  }

  //-----------------------
  // Section data (DMA alignment)
  for (uint32_t i = 0; i < numSectionEntries; ++i)
  {
    SectionDataNSA* sectionData = (SectionDataNSA*) m_sectionData[i].m_data;
    NMP_ASSERT(sectionData);
    sectionData->dislocate();
    UNFIX_SWAP_PTR(void, m_sectionData[i].m_data);
  }
  
  //-----------------------
  // Unchanging channel set data (DMA alignment)
  NMP_ASSERT(m_unchangingData);
  m_unchangingData->dislocate();
  UNFIX_SWAP_PTR(UnchangingDataNSA, m_unchangingData);
  
  //-----------------------
  if (numSectionEntries > 0)
  {
    // Sectioning information
    UNFIX_SWAP_PTR(MR::DataRef, m_sectionData);

    NMP::endianSwapArray(m_sectionSizes, numSectionEntries);
    UNFIX_SWAP_PTR(uint32_t, m_sectionSizes);

    NMP::endianSwapArray(m_sectionStartFrames, m_numFrameSections + 1);
    UNFIX_SWAP_PTR(uint32_t, m_sectionStartFrames);
    NMP::endianSwap(m_maxSectionSize);

    //-----------------------
    // Quantisation scale and offset information (Common to all sections)
    if (m_sampledQuatNumQuantisationSets > 0)
    {
      for (uint32_t i = 0; i < m_sampledQuatNumQuantisationSets; ++i)
      {
        m_sampledQuatQuantisationInfo[i].dislocate();
      }
      UNFIX_SWAP_PTR(QuantisationScaleAndOffsetVec3, m_sampledQuatQuantisationInfo);
    }    
    NMP::endianSwap(m_sampledQuatNumQuantisationSets);

    if (m_sampledPosNumQuantisationSets > 0)
    {
      for (uint32_t i = 0; i < m_sampledPosNumQuantisationSets; ++i)
      {
        m_sampledPosQuantisationInfo[i].dislocate();
      }
      UNFIX_SWAP_PTR(QuantisationScaleAndOffsetVec3, m_sampledPosQuantisationInfo);
    }
    NMP::endianSwap(m_sampledPosNumQuantisationSets);

    m_posMeansQuantisationInfo.dislocate();

    //-----------------------
    // Compression to animation channel maps
    for (uint32_t i = 0; i < m_numChannelSections; ++i)
      m_sampledQuatCompToAnimMaps[i]->dislocate();

    for (uint32_t i = 0; i < m_numChannelSections; ++i)
      m_sampledPosCompToAnimMaps[i]->dislocate();

    // Pointers table to sub-section CAMs
    for (uint32_t i = 0; i < m_numChannelSections; ++i)
    {
      UNFIX_SWAP_PTR(MR::CompToAnimChannelMap, m_sampledQuatCompToAnimMaps[i]);
    }
    UNFIX_SWAP_PTR(MR::CompToAnimChannelMap*, m_sampledQuatCompToAnimMaps);

    for (uint32_t i = 0; i < m_numChannelSections; ++i)
    {
      UNFIX_SWAP_PTR(MR::CompToAnimChannelMap, m_sampledPosCompToAnimMaps[i]);
    }
    UNFIX_SWAP_PTR(MR::CompToAnimChannelMap*, m_sampledPosCompToAnimMaps);
  }
  
  //-----------------------
  m_unchangingQuatCompToAnimMap->dislocate();
  UNFIX_SWAP_PTR(MR::CompToAnimChannelMap, m_unchangingQuatCompToAnimMap);

  m_unchangingPosCompToAnimMap->dislocate();
  UNFIX_SWAP_PTR(MR::CompToAnimChannelMap, m_unchangingPosCompToAnimMap);

  NMP::endianSwap(m_maxNumCompChannels);

  //-----------------------
  // Header information
  NMP::endianSwap(m_numChannelSections);
  NMP::endianSwap(m_numFrameSections);
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_sampleFrequency);
  NMP::endianSwap(m_duration);
  m_funcTable = NULL;
  AnimSourceBase::dislocate();
}
#endif //NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AnimSourceNSA::relocate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Animation sources must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  uint32_t numSectionEntries = m_numFrameSections * m_numChannelSections;

  //-----------------------
  // Header information
  void* ptr = this;
  NMP::Memory::Format memReqsHdr(sizeof(AnimSourceNSA), NMP_VECTOR_ALIGNMENT);
  NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  m_funcTable = &m_functionTable;

  //-----------------------
  // Compression to animation channel maps
  m_unchangingPosCompToAnimMap = CompToAnimChannelMap::relocate(ptr);
  m_unchangingQuatCompToAnimMap = CompToAnimChannelMap::relocate(ptr);

  //-----------------------
  if (numSectionEntries > 0)
  {
    // Pointers table to sub-section CAMs
    NMP::Memory::Format memReqsCAMTable(sizeof(MR::CompToAnimChannelMap*) * m_numChannelSections, NMP_NATURAL_TYPE_ALIGNMENT);
    m_sampledPosCompToAnimMaps = (MR::CompToAnimChannelMap**) NMP::Memory::alignAndIncrement(ptr, memReqsCAMTable);
    m_sampledQuatCompToAnimMaps = (MR::CompToAnimChannelMap**) NMP::Memory::alignAndIncrement(ptr, memReqsCAMTable);

    // Sub-section CAMs themselves
    for (uint32_t i = 0; i < m_numChannelSections; ++i)
    {
      m_sampledPosCompToAnimMaps[i] = CompToAnimChannelMap::relocate(ptr);
    }
    for (uint32_t i = 0; i < m_numChannelSections; ++i)
    {
      m_sampledQuatCompToAnimMaps[i] = CompToAnimChannelMap::relocate(ptr);
    }

    //-----------------------
    // Quantisation scale and offset information (Common to all sections)
    if (m_sampledPosNumQuantisationSets > 0)
    {
      NMP::Memory::Format memReqsPosQuantInfo(
        sizeof(MR::QuantisationScaleAndOffsetVec3) * m_sampledPosNumQuantisationSets,
        NMP_NATURAL_TYPE_ALIGNMENT);
      m_sampledPosQuantisationInfo = (QuantisationScaleAndOffsetVec3*) NMP::Memory::alignAndIncrement(ptr, memReqsPosQuantInfo);
    }

    if (m_sampledQuatNumQuantisationSets > 0)
    {
      NMP::Memory::Format memReqsQuatQuantInfo(
        sizeof(MR::QuantisationScaleAndOffsetVec3) * m_sampledQuatNumQuantisationSets,
        NMP_NATURAL_TYPE_ALIGNMENT);
      m_sampledQuatQuantisationInfo = (QuantisationScaleAndOffsetVec3*) NMP::Memory::alignAndIncrement(ptr, memReqsQuatQuantInfo);
    }

    //-----------------------
    // Quantisation scale and offset information (Common to all sections)
    NMP::Memory::Format memReqsSectionsFrameInfo(
      sizeof(uint32_t) * (m_numFrameSections + 1),
      NMP_NATURAL_TYPE_ALIGNMENT);
    m_sectionStartFrames = (uint32_t*) NMP::Memory::alignAndIncrement(ptr, memReqsSectionsFrameInfo);
    
    //-----------------------
    // Sections information (i.e. start frame and section size)
    NMP::Memory::Format memReqsSectionsSizeInfo(
      sizeof(uint32_t) * numSectionEntries,
      NMP_NATURAL_TYPE_ALIGNMENT);
    m_sectionSizes = (uint32_t*) NMP::Memory::alignAndIncrement(ptr, memReqsSectionsSizeInfo);

    //-----------------------
    // Sections DataRef table (pointers to sections)
    NMP::Memory::Format memReqsSectionDataRefs(
      sizeof(MR::DataRef) * numSectionEntries,
      NMP_NATURAL_TYPE_ALIGNMENT);
    m_sectionData = (DataRef*) NMP::Memory::alignAndIncrement(ptr, memReqsSectionDataRefs);
  }
  
  //-----------------------
  // Unchanging channel set data
  m_unchangingData = UnchangingDataNSA::relocate(ptr);

  //-----------------------
  // Sections are relocated as required (DMA aligned)
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
