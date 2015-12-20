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
#include "morpheme/AnimSource/mrAnimSectionASA.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef NM_HOST_CELL_SPU
  #include "NMPlatform/ps3/NMSPUDMAController.h"
#endif // NM_HOST_CELL_SPU

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetASAInfo functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void ChannelSetASAInfo::locate()
{
  NMP::endianSwapArray((float*)&m_chanPosDefault, 4);
  NMP::endianSwapArray((float*)&m_chanQuatDefault, 4);
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetASAInfo::dislocate()
{
  // The w component is often uninitialised. Set it to 0 to ensure binary invariance
  m_chanPosDefault.w = 0.0f;
  NMP::endianSwapArray((float*)&m_chanPosDefault, 4);
  NMP::endianSwapArray((float*)&m_chanQuatDefault, 4);
}

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetBasic functions.
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetASA::locate(uint8_t** data)
{
  NMP_ASSERT(data);
  *data = (uint8_t*) NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);

  // Channel data.
  m_channelQuat.locate(data);
  m_channelPos.locate(data);
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetASA::dislocate()
{
  // Channel data.
  m_channelQuat.dislocate();
  m_channelPos.dislocate();
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetASA::relocate(uint8_t** data)
{
  NMP_ASSERT(data);
  *data = (uint8_t*) NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);

  // Channel data.
  m_channelQuat.relocate(data);
  m_channelPos.relocate(data);
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSectionInfoASA functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AnimSectionInfoASA::locate()
{
  NMP::endianSwap(m_startFrame);
  NMP::endianSwap(m_sectionSize);
  NMP::endianSwap(m_headerSize);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionInfoASA::dislocate()
{
  NMP::endianSwap(m_startFrame);
  NMP::endianSwap(m_sectionSize);
  NMP::endianSwap(m_headerSize);
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSubSectionASA functions.
//----------------------------------------------------------------------------------------------------------------------
void AnimSubSectionASA::locate(uint8_t* data)
{
  NMP_ASSERT(data);
  data = (uint8_t*) this;
  NMP_ASSERT(NMP_IS_ALIGNED(data, NMP_VECTOR_ALIGNMENT));
  NMP::endianSwap(m_subsectionSize);
  NMP::endianSwap(m_subsectionID);
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_offset);

  // Channel sets.
  data += sizeof(AnimSubSectionASA);
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_channelSets = (ChannelSetASA*) (data);
  data += (sizeof(ChannelSetASA) * m_numChannelSets);

  // Locate each channel set.
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
    m_channelSets[i].locate(&data);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSubSectionASA::dislocate()
{
  // Dislocate each channel set.
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    m_channelSets[i].dislocate();
  }
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_offset);
  NMP::endianSwap(m_subsectionID);
  NMP::endianSwap(m_subsectionSize);
  // For binary invariance. These will be fixed in locate()
  m_channelSets = NULL;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AnimSubSectionASA::relocate()
{
  uint8_t* data = (uint8_t*) this;
  NMP_ASSERT(data);
  data += sizeof(AnimSubSectionASA);
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_channelSets = (ChannelSetASA*) data;
  data += (sizeof(ChannelSetASA) * m_numChannelSets);

  // Restore pointers within each channel set.
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
    m_channelSets[i].relocate(&data);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Calculate the animation channel values at a given time for all of the animations channels.
void AnimSubSectionASA::computeFullAnimTransformSet(
  const ChannelSetASAInfo* info,
  const RigToAnimMap*      mapFromRigToAnim,    // Rig bones to Animation channels.
  const uint32_t           sectionFrameIndex,
  const float              interpolant,
  NMP::Quat*               outQuat,
  NMP::Vector3*            outPos,
  NMP::DataBuffer*         outputBuffer) const
{
  uint32_t outputBufferIndex;
  uint32_t animChannelSetIndex;
  
  NMP_ASSERT(mapFromRigToAnim->getRigToAnimMapType() == RigToAnimMap::MapPairs);
  const RigToAnimEntryMap* rigToAnimMapData = (const RigToAnimEntryMap*) mapFromRigToAnim->getRigToAnimMapData();
  NMP_ASSERT(rigToAnimMapData);

  for (uint32_t numChannel = 0; numChannel < m_numChannelSets; ++numChannel)
  {
    // Get source animation channel index
    animChannelSetIndex = rigToAnimMapData->getEntryAnimChannelIndex(numChannel + m_offset);

    // Get output buffer index.
    outputBufferIndex = rigToAnimMapData->getEntryRigChannelIndex(numChannel + m_offset);

    // Calculate Quat and Pos for this animation channel set and store in the correct location
    // in the output buffer.
    m_channelSets[numChannel].getChannelQuat()->getQuat(
      info[animChannelSetIndex].getChannelQuatInfo(),
      sectionFrameIndex,
      interpolant,
      outQuat[outputBufferIndex]);

    m_channelSets[numChannel].getChannelPos()->getPos(
      info[animChannelSetIndex].getChannelPosInfo(),
      sectionFrameIndex,
      interpolant,
      outPos[outputBufferIndex]);
      
    outputBuffer->setChannelUsed(outputBufferIndex);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Calculate the animation channel values at a given time for a sub-set of the animation channels set.
void AnimSubSectionASA::computeAnimTransformSubSet(
  const ChannelSetASAInfo*         info,
  const RigToAnimMap*              mapFromRigToAnim,  // Rig bones to Animation channels.
  const uint32_t                   outputSubsetSize,
  const uint16_t*                  outputSubsetArray, // Must be incrementally ordered
                                                      // and must be a sub set of the mapping output set.
  const uint32_t                   sectionFrameIndex,
  const float                      interpolant,
  NMP::Quat*                       outQuat,
  NMP::Vector3*                    outPos,
  NMP::DataBuffer*                 outputBuffer) const
{
  uint32_t      outputBufferIndex;
  uint32_t      animChannelSetIndex;
  uint32_t      mapLoop = 0;
  
  NMP_ASSERT(mapFromRigToAnim->getRigToAnimMapType() == RigToAnimMap::MapPairs);
  const RigToAnimEntryMap* rigToAnimMapData = (const RigToAnimEntryMap*) mapFromRigToAnim->getRigToAnimMapData();
  NMP_ASSERT(rigToAnimMapData);

  for (uint32_t subsetLoop = 0; subsetLoop < outputSubsetSize; subsetLoop++)
  {
    NMP_ASSERT(outputSubsetArray[subsetLoop] < outputBuffer->getLength());

    // Get source animation channel index
    outputBufferIndex = rigToAnimMapData->getEntryRigChannelIndex(mapLoop + m_offset);
    NMP_ASSERT(outputBufferIndex < outputBuffer->getLength());

    while (outputSubsetArray[subsetLoop] != outputBufferIndex)
    {
      mapLoop++;
      NMP_ASSERT(mapLoop < rigToAnimMapData->getNumEntries());  // All output buffer indexes referenced in the sub-set
      // array must be contained within the mapping.
      outputBufferIndex = rigToAnimMapData->getEntryRigChannelIndex(mapLoop + m_offset);
      NMP_ASSERT(outputBufferIndex < outputBuffer->getLength());
    }

    animChannelSetIndex = rigToAnimMapData->getEntryAnimChannelIndex(mapLoop + m_offset);
    NMP_ASSERT(animChannelSetIndex < m_numChannelSets + m_offset);

    // Calculate Quat and Pos for this animation channel set and store in the correct location
    // in the output buffer.
    m_channelSets[animChannelSetIndex - m_offset].getChannelQuat()->getQuat(
      info[animChannelSetIndex].getChannelQuatInfo(),
      sectionFrameIndex,
      interpolant,
      outQuat[outputBufferIndex]);

    m_channelSets[animChannelSetIndex - m_offset].getChannelPos()->getPos(
      info[animChannelSetIndex].getChannelPosInfo(),
      sectionFrameIndex,
      interpolant,
      outPos[outputBufferIndex]);

    outputBuffer->setChannelUsed(outputBufferIndex);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSubSectionASA::computeSingleChannelTransform(
  const ChannelSetASAInfo* info,
  const uint32_t           animChannelIndex,   // Index into the animations set of channels. (all sections have the same number of channels).
  const uint32_t           sectionFrameIndex,
  const float              interpolant,
  NMP::Quat*               quatResult,
  NMP::Vector3*            posResult) const
{
  NMP_ASSERT(info && quatResult && posResult);

  // Calculate Quat and Pos for this animation channel set and store in the output pos quat.
  m_channelSets[animChannelIndex - m_offset].getChannelQuat()->getQuat(
    info[animChannelIndex].getChannelQuatInfo(),
    sectionFrameIndex,
    interpolant,
    *quatResult);
  m_channelSets[animChannelIndex - m_offset].getChannelPos()->getPos(
    info[animChannelIndex].getChannelPosInfo(),
    sectionFrameIndex,
    interpolant,
    *posResult);
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSectionASA functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AnimSectionASA::locate(MR::AnimSectionASA* section)
{
  uint8_t* data = (uint8_t*)section + sizeof(MR::AnimSectionASA);
  NMP::endianSwap(m_numSubSections);
  NMP::endianSwap(m_maxSubsectionSize);

  // Subsections pointers array.
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_subsections = (DataRef*) data;
  data += (sizeof(DataRef) * m_numSubSections);

  // Locate each subsection.
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  for (uint32_t i = 0; i < m_numSubSections; ++i)
  {
    AnimSubSectionASA* subsection = (AnimSubSectionASA*) data;
    m_subsections[i].m_data = (void*) subsection;
    NMP_ASSERT(((size_t)(m_subsections[i].m_data) % NMP_VECTOR_ALIGNMENT) == 0);
    subsection->locate(data);
    data += subsection->getSubsectionSize();
    data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionASA::dislocate()
{
  // Dislocate each subsection
  for (uint32_t i = 0; i < m_numSubSections; ++i)
  {
    AnimSubSectionASA* subsection = (AnimSubSectionASA*) m_subsections[i].m_data;
    subsection->dislocate();
    UNFIX_PTR(void, m_subsections[i].m_data);
  }
  NMP::endianSwap(m_numSubSections);
  NMP::endianSwap(m_maxSubsectionSize);
  // For binary invariance. These will be fixed in locate()
  m_subsections = NULL;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionASA::relocate()
{
  uint8_t* data = (uint8_t*) this;
  data += sizeof(AnimSectionASA);
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);

  m_subsections = (MR::DataRef*) data;
  data += (sizeof(DataRef) * m_numSubSections);
}

//----------------------------------------------------------------------------------------------------------------------
// Calculate the animation channel values at a given time for all of the animations channels.
void AnimSectionASA::computeFullAnimTransformSet(
  const ChannelSetASAInfo* info,
  const RigToAnimMap*      mapFromRigToAnim,    // Rig bones to Animation channels.
  const uint32_t           sectionFrameIndex,
  const float              interpolant,
  NMP::MemoryAllocator*    allocator,
  NMP::DataBuffer*         outputBuffer) const
{
  NMP_ASSERT(mapFromRigToAnim && outputBuffer && info);
  
  // Start with no valid data in the output buffer.
  outputBuffer->getUsedFlags()->clearAll();

  NMP::Quat*     outQuat  = outputBuffer->getPosQuatChannelQuat(0);
  NMP::Vector3*  outPos   = outputBuffer->getPosQuatChannelPos(0);

  // Allocate and compute first subsection
  AnimSubSectionASA* subsection = (AnimSubSectionASA*) m_subsections[0].getData(
                                    NMP::Memory::Format(m_maxSubsectionSize, NMP_VECTOR_ALIGNMENT),
                                    allocator);
  NMP_ASSERT(subsection);
#ifdef NM_HOST_CELL_SPU
  subsection->relocate();
#endif // NM_HOST_CELL_SPU
  subsection->computeFullAnimTransformSet(
    info,
    mapFromRigToAnim,
    sectionFrameIndex,
    interpolant,
    outQuat,
    outPos,
    outputBuffer);

  // Compute rest of subsections replacing the first one
  if (m_numSubSections > 1)
  {
    for (uint32_t subsectionIndex = 1; subsectionIndex < m_numSubSections; subsectionIndex++)
    {
#ifdef NM_HOST_CELL_SPU
      NMP::SPUDMAController::dmaGet((void*)subsection,
                                    (uint32_t)m_subsections[subsectionIndex].m_data,
                                    NMP::Memory::Format(m_maxSubsectionSize, NMP_VECTOR_ALIGNMENT).size);
      NMP::SPUDMAController::dmaWaitAll();
      subsection->relocate();
#else
      subsection = (AnimSubSectionASA*) m_subsections[subsectionIndex].getData(
                     NMP::Memory::Format(m_maxSubsectionSize, NMP_VECTOR_ALIGNMENT),
                     allocator);
#endif // NM_HOST_CELL_SPU
      NMP_ASSERT(subsection);
      subsection->computeFullAnimTransformSet(
        info,
        mapFromRigToAnim,
        sectionFrameIndex,
        interpolant,
        outQuat,
        outPos,
        outputBuffer);
    }
  }
  
  //--------------------------------
  // Set the channel used flags required for this animation rig. Make sure the first entry, which is
  // not specified in animation files, is initialised to the identity and flagged as used.
  outputBuffer->getPosQuatChannelPos(0)->setToZero();
  outputBuffer->getPosQuatChannelQuat(0)->identity();
  outputBuffer->padOutPosQuat();

  // Flag the output buffer as full.
  outputBuffer->setChannelUsed(0);
  outputBuffer->calculateFullFlag();
  
  NMP_ASSERT(mapFromRigToAnim->getRigToAnimMapType() == RigToAnimMap::MapPairs);
  const RigToAnimEntryMap* rigToAnimMapData = (const RigToAnimEntryMap*) mapFromRigToAnim->getRigToAnimMapData();
  NMP_ASSERT(rigToAnimMapData);
  (void)rigToAnimMapData;
  NMP_ASSERT(outputBuffer->isFull() == (mapFromRigToAnim->getNumRigBones() - rigToAnimMapData->getNumEntries() <= 1));
}

//----------------------------------------------------------------------------------------------------------------------
// Calculate the animation channel values at a given time for a sub-set of the animation channels set.
void AnimSectionASA::computeAnimTransformSubSet(
  const ChannelSetASAInfo*         info,
  const RigToAnimMap*              mapFromRigToAnim,  // Rig bones to Animation channels.
  const uint32_t                   outputSubsetSize,
  const uint16_t*                  outputSubsetArray, // Must be incrementally ordered
                                                      // and must be a sub set of the mapping output set.
  const uint32_t                   sectionFrameIndex,
  const float                      interpolant,
  NMP::MemoryAllocator*            allocator,
  NMP::DataBuffer*                 outputBuffer) const
{
  NMP_ASSERT(mapFromRigToAnim && outputBuffer && info);
  NMP_ASSERT(outputSubsetArray);
  NMP_ASSERT(outputSubsetSize > 0);

  // Start with no valid data in the output buffer.
  outputBuffer->getUsedFlags()->clearAll();

  NMP::Quat*     outQuat  = outputBuffer->getPosQuatChannelQuat(0);
  NMP::Vector3*  outPos   = outputBuffer->getPosQuatChannelPos(0);

  for (uint32_t subsectionIndex = 0; subsectionIndex < m_numSubSections; ++subsectionIndex)
  {
    AnimSubSectionASA* subsection = (AnimSubSectionASA*) m_subsections[subsectionIndex].getData(
                                      NMP::Memory::Format(m_maxSubsectionSize, NMP_VECTOR_ALIGNMENT),
                                      allocator);
    subsection->computeAnimTransformSubSet(
      info,
      mapFromRigToAnim,
      outputSubsetSize,
      outputSubsetArray,
      sectionFrameIndex,
      interpolant,
      outQuat,
      outPos,
      outputBuffer);
  }
  
  //--------------------------------
  // Set the channel used flags required for this animation rig. Make sure the first entry, which is
  // not specified in animation files, is initialised to the identity and flagged as used.
  outputBuffer->getPosQuatChannelPos(0)->setToZero();
  outputBuffer->getPosQuatChannelQuat(0)->identity();
  outputBuffer->padOutPosQuat();
  outputBuffer->setChannelUsed(0);
  outputBuffer->calculateFullFlag();
  
  NMP_ASSERT(mapFromRigToAnim->getRigToAnimMapType() == RigToAnimMap::MapPairs);
  const RigToAnimEntryMap* rigToAnimMapData = (const RigToAnimEntryMap*) mapFromRigToAnim->getRigToAnimMapData();
  NMP_ASSERT(rigToAnimMapData);
  (void)rigToAnimMapData;
  NMP_ASSERT(outputBuffer->isFull() == (mapFromRigToAnim->getNumRigBones() - rigToAnimMapData->getNumEntries() <= 1));
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionASA::computeSingleChannelTransform(
  const ChannelSetASAInfo* info,
  const uint32_t           animChannelIndex,   // Index into the animations set of channels. (all sections have the same number of channels).
  const uint32_t           sectionFrameIndex,
  const float              interpolant,
  NMP::MemoryAllocator*    allocator,
  NMP::Quat*               quatResult,
  NMP::Vector3*            posResult) const
{
  uint32_t subsectionIndex          = getSubsectionIndex(animChannelIndex, allocator);
  AnimSubSectionASA* subsection     = (AnimSubSectionASA*) m_subsections[subsectionIndex].getData(
                                        NMP::Memory::Format(m_maxSubsectionSize, NMP_VECTOR_ALIGNMENT),
                                        allocator);
  NMP_ASSERT(animChannelIndex >= subsection->getOffset());  // This ensures we don't overflow on the line below.
  uint32_t channelIndex             = animChannelIndex - subsection->getOffset();

  subsection->computeSingleChannelTransform(info, channelIndex, sectionFrameIndex, interpolant, quatResult, posResult);
}

//----------------------------------------------------------------------------------------------------------------------
ChannelSetASA* AnimSectionASA::getChannelSets(
  const size_t          subsectionSize,
  const uint32_t        subsectionID,
  NMP::MemoryAllocator* allocator) const
{
  ChannelSetASA* channelSet = NULL;

  AnimSubSectionASA* subsection = (AnimSubSectionASA*) m_subsections[subsectionID].getData(
                                    NMP::Memory::Format(subsectionSize, NMP_VECTOR_ALIGNMENT),
                                    allocator);
  channelSet = subsection->getChannelSets();

  return channelSet;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSectionASA::getPosChanFrameCount(
  uint32_t              animChannelSetIndex,
  NMP::MemoryAllocator* allocator) const
{
  uint32_t subsectionIndex          = getSubsectionIndex(animChannelSetIndex, allocator);
  AnimSubSectionASA* subSection     = (AnimSubSectionASA*) m_subsections[subsectionIndex].getData(
                                        NMP::Memory::Format(m_maxSubsectionSize, NMP_VECTOR_ALIGNMENT),
                                        allocator);
  uint32_t channelIndex             = animChannelSetIndex - subSection->getOffset();
  return subSection->getChannelSets()[channelIndex].getChannelPos()->getNumKeyFrames();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSectionASA::getQuatChanFrameCount(
  uint32_t              animChannelSetIndex,
  NMP::MemoryAllocator* allocator) const
{
  uint32_t subsectionIndex          = getSubsectionIndex(animChannelSetIndex, allocator);
  AnimSubSectionASA* subSection     = (AnimSubSectionASA*) m_subsections[subsectionIndex].getData(
                                        NMP::Memory::Format(m_maxSubsectionSize, NMP_VECTOR_ALIGNMENT),
                                        allocator);
  uint32_t channelIndex             = animChannelSetIndex - subSection->getOffset();
  return subSection->getChannelSets()[channelIndex].getChannelQuat()->getNumKeyFrames();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimSectionASA::getSubsectionIndex(
  const uint32_t        animChannelIndex,
  NMP::MemoryAllocator* allocator) const
{
  uint32_t numChannelSets;
  uint32_t offset;
  for (uint32_t subsectionIndex = 0; subsectionIndex < m_numSubSections; ++subsectionIndex)
  {
    AnimSubSectionASA* subsection     = (AnimSubSectionASA*) m_subsections[subsectionIndex].getData(
                                          NMP::Memory::Format(m_maxSubsectionSize, NMP_VECTOR_ALIGNMENT),
                                          allocator);
    numChannelSets  = subsection->getNumChannelSets();
    offset          = subsection->getOffset();
    if (animChannelIndex < (numChannelSets + offset))
    {
      return subsectionIndex;
    }
  }
  return 0;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
