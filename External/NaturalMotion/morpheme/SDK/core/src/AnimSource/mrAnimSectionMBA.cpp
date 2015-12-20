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
#include "morpheme/AnimSource/mrAnimSectionMBA.h"
#include "morpheme/mrRigToAnimMap.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetMBAInfo functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void ChannelSetMBAInfo::locate()
{
  NMP::endianSwapArray((float*)&m_chanPosDefault, 4);
  NMP::endianSwapArray((float*)&m_chanQuatDefault, 4);
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetMBAInfo::dislocate()
{
  // The w component is often uninitialised. Set it to 0 to ensure binary invariance
  m_chanPosDefault.w = 0.0f;
  NMP::endianSwapArray((float*)&m_chanPosDefault, 4);
  NMP::endianSwapArray((float*)&m_chanQuatDefault, 4);
}

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetBasic functions.
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetMBA::locate(uint8_t** data)
{
  NMP_ASSERT(data);
  *data = (uint8_t*) NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);

  // Channel data.
  m_channelQuat.locate(data);
  m_channelPos.locate(data);
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetMBA::dislocate()
{
  // Channel data.
  m_channelQuat.dislocate();
  m_channelPos.dislocate();
}
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetMBA::relocate(uint8_t** data)
{
  NMP_ASSERT(data);
  *data = (uint8_t*) NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);

  // Channel data.
  m_channelQuat.relocate(data);
  m_channelPos.relocate(data);
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSectionInfoMBA functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AnimSectionInfoMBA::locate()
{
  NMP::endianSwap(m_startFrame);
  NMP::endianSwap(m_sectionSize);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionInfoMBA::dislocate()
{
  NMP::endianSwap(m_startFrame);
  NMP::endianSwap(m_sectionSize);
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSectionMBA functions.
//----------------------------------------------------------------------------------------------------------------------
void AnimSectionMBA::locate(uint8_t** data)
{
  NMP_ASSERT(data);
  *data = (uint8_t*) this;
  NMP_ASSERT(NMP_IS_ALIGNED(*data, NMP_VECTOR_ALIGNMENT));
  NMP::endianSwap(m_numChannelSets);

  // Channel sets.
  *data += sizeof(AnimSectionMBA);
  *data = (uint8_t*) NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);
  m_channelSets = (ChannelSetMBA*) (*data);
  *data += (sizeof(ChannelSetMBA) * m_numChannelSets);

  // Locate each channel set.
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    *data = (uint8_t*) NMP::Memory::align(*data, NMP_VECTOR_ALIGNMENT);
    m_channelSets[i].locate(data);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionMBA::dislocate()
{
  // Dislocate each channel set.
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    m_channelSets[i].dislocate();
  }
  NMP::endianSwap(m_numChannelSets);

  // For binary invariance. These will be fixed in locate()
  m_channelSets = NULL;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionMBA::relocate()
{
  uint8_t* data = (uint8_t*) this;
  data += sizeof(AnimSectionMBA);
  data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
  m_channelSets = (ChannelSetMBA*) data;
  data += (sizeof(ChannelSetMBA) * m_numChannelSets);

  // Restore pointers within each channel set.
  for (uint32_t i = 0; i < m_numChannelSets; ++i)
  {
    data = (uint8_t*) NMP::Memory::align(data, NMP_VECTOR_ALIGNMENT);
    m_channelSets[i].relocate(&data);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Calculate the animation channel values at a given time for a sub-set of the animation channels set.
void AnimSectionMBA::computeAnimTransformSubSet(
  const ChannelSetMBAInfo*         info,
  const RigToAnimMap*              mapFromRigToAnim,  // Rig bones to Animation channels.
  const uint32_t                   outputSubsetSize,
  const uint16_t*                  outputSubsetArray, // Must be incrementally ordered
                                                      // and must be a sub set of the mapping output set.
  const uint32_t                   sectionFrameIndex,
  const float                      interpolant,
  NMP::DataBuffer*                 outputBuffer) const
{
  uint32_t      outputBufferIndex;
  uint32_t      animChannelSetIndex;
  uint32_t      subsetLoop;
  uint32_t      mapLoop;

  NMP_ASSERT(mapFromRigToAnim && outputBuffer);
  NMP_ASSERT(outputSubsetArray);
  NMP_ASSERT(outputSubsetSize > 0);
  NMP_ASSERT(info);

  NMP_ASSERT(mapFromRigToAnim->getRigToAnimMapType() == RigToAnimMap::MapPairs);
  const RigToAnimEntryMap* rigToAnimMapData = (const RigToAnimEntryMap*) mapFromRigToAnim->getRigToAnimMapData();
  NMP_ASSERT(rigToAnimMapData);

  // Start with no valid data in the output buffer.
  outputBuffer->getUsedFlags()->clearAll();

  NMP::Quat* outQuat = outputBuffer->getPosQuatChannelQuat(0);
  NMP::Vector3* outPos = outputBuffer->getPosQuatChannelPos(0);

  mapLoop = 0;
  for (subsetLoop = 0; subsetLoop < outputSubsetSize; subsetLoop++)
  {
    NMP_ASSERT(outputSubsetArray[subsetLoop] < outputBuffer->getLength());

    // Get output buffer index.
    outputBufferIndex = rigToAnimMapData->getEntryRigChannelIndex(mapLoop);
    NMP_ASSERT(outputBufferIndex < outputBuffer->getLength());

    while (outputSubsetArray[subsetLoop] != outputBufferIndex)
    {
      mapLoop++;
      NMP_ASSERT(mapLoop < rigToAnimMapData->getNumEntries());  // All output buffer indexes referenced in the sub-set
                                                                // array must be contained within the mapping.
      outputBufferIndex = rigToAnimMapData->getEntryRigChannelIndex(mapLoop);
      NMP_ASSERT(outputBufferIndex < outputBuffer->getLength());
    }

    // Get source animation channel index.
    animChannelSetIndex = rigToAnimMapData->getEntryAnimChannelIndex(mapLoop);
    NMP_ASSERT(animChannelSetIndex < m_numChannelSets);

    // Calculate Quat and Pos for this animation channel set and store in the correct location
    // in the output buffer.
    m_channelSets[animChannelSetIndex].getChannelQuat()->getQuat(
      info[animChannelSetIndex].getChannelQuatInfo(),
      sectionFrameIndex,
      interpolant,
      outQuat[outputBufferIndex]);
    m_channelSets[animChannelSetIndex].getChannelPos()->getPos(
      info[animChannelSetIndex].getChannelPosInfo(),
      sectionFrameIndex,
      interpolant,
      outPos[outputBufferIndex]);

    outputBuffer->setChannelUsed(outputBufferIndex);
  }
  
  //--------------------------------
  // Set the channel used flags required for this animation rig. Make sure the first entry, which is
  // not specified in animation files, is initialised to the identity and flagged as used.
  outputBuffer->getPosQuatChannelPos(0)->setToZero();
  outputBuffer->getPosQuatChannelQuat(0)->identity();
  outputBuffer->padOutPosQuat();
  outputBuffer->setChannelUsed(0);
  outputBuffer->calculateFullFlag();
  NMP_ASSERT(outputBuffer->isFull() == (mapFromRigToAnim->getNumRigBones() - rigToAnimMapData->getNumEntries() <= 1));
}

//----------------------------------------------------------------------------------------------------------------------
// Calculate the animation channel values at a given time for all of the animations channels.
void AnimSectionMBA::computeFullAnimTransformSet(
  const ChannelSetMBAInfo* info,
  const RigToAnimMap*      mapFromRigToAnim,    // Rig bones to Animation channels.
  const uint32_t           sectionFrameIndex,
  const float              interpolant,
  NMP::DataBuffer*         outputBuffer) const
{
  uint32_t outputBufferIndex, animChannelSetIndex;
  NMP::Quat     quat;
  NMP::Vector3  pos;

  NMP_ASSERT(mapFromRigToAnim && outputBuffer && info);
  
  NMP_ASSERT(mapFromRigToAnim->getRigToAnimMapType() == RigToAnimMap::MapPairs);
  const RigToAnimEntryMap* rigToAnimMapData = (const RigToAnimEntryMap*) mapFromRigToAnim->getRigToAnimMapData();
  NMP_ASSERT(rigToAnimMapData);

  // Start with no valid data in the output buffer.
  outputBuffer->getUsedFlags()->clearAll();

  NMP::Quat* outQuat = outputBuffer->getPosQuatChannelQuat(0);
  NMP::Vector3* outPos = outputBuffer->getPosQuatChannelPos(0);
  uint32_t mapFromRigToAnimEntries = rigToAnimMapData->getNumEntries();

  for (uint32_t mapLoop = 0; mapLoop < mapFromRigToAnimEntries; ++mapLoop)
  {
    // Get output buffer index.
    outputBufferIndex = rigToAnimMapData->getEntryRigChannelIndex(mapLoop);
    NMP_ASSERT(outputBufferIndex < outputBuffer->getLength());

    // Get source animation channel index
    animChannelSetIndex = rigToAnimMapData->getEntryAnimChannelIndex(mapLoop);
    NMP_ASSERT(animChannelSetIndex < m_numChannelSets);

    // Calculate Quat and Pos for this animation channel set and store in the correct location
    // in the output buffer.
    m_channelSets[animChannelSetIndex].getChannelQuat()->getQuat(
      info[animChannelSetIndex].getChannelQuatInfo(),
      sectionFrameIndex,
      interpolant,
      outQuat[outputBufferIndex]);

    m_channelSets[animChannelSetIndex].getChannelPos()->getPos(
      info[animChannelSetIndex].getChannelPosInfo(),
      sectionFrameIndex,
      interpolant,
      outPos[outputBufferIndex]);

    outputBuffer->setChannelUsed(outputBufferIndex);
  }

  //--------------------------------
  // Set the channel used flags required for this animation rig. Make sure the first entry, which is
  // not specified in animation files, is initialised to the identity and flagged as used.
  outputBuffer->getPosQuatChannelPos(0)->setToZero();
  outputBuffer->getPosQuatChannelQuat(0)->identity();
  outputBuffer->padOutPosQuat();
  outputBuffer->setChannelUsed(0);
  outputBuffer->calculateFullFlag();
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionMBA::computeSingleChannelTransform(
  const ChannelSetMBAInfo* info,
  const uint32_t           animChannelIndex,   // Index into the animations set of channels. (all sections have the same number of channels).
  const uint32_t           sectionFrameIndex,
  const float              interpolant,
  NMP::Quat*               quatResult,
  NMP::Vector3*            posResult) const
{
  NMP_ASSERT(info && quatResult && posResult);

  // Calculate Quat and Pos for this animation channel set and store in the output pos quat.
  m_channelSets[animChannelIndex].getChannelQuat()->getQuat(
    info[animChannelIndex].getChannelQuatInfo(),
    sectionFrameIndex,
    interpolant,
    *quatResult);

  m_channelSets[animChannelIndex].getChannelPos()->getPos(
    info[animChannelIndex].getChannelPosInfo(),
    sectionFrameIndex,
    interpolant,
    *posResult);
}

#if !defined(NM_HOST_CELL_SPU) && !defined(NM_HOST_CELL_PPU)
  #ifdef NM_DEBUG
//----------------------------------------------------------------------------------------------------------------------
void AnimSectionMBA::debugOutputFullAnimTransformSet(
  const ChannelSetMBAInfo* info,
  uint32_t                 sectionFrameIndex,
  float                    interpolant,
  FILE*                    file) const
{
  uint32_t      i;
  NMP::Quat     quat;
  NMP::Vector3  pos;

  NMP_ASSERT(file && info);

  for (i = 0; i < m_numChannelSets; ++i)
  {
    m_channelSets[i].getChannelQuat()->getQuat(
      info[i].getChannelQuatInfo(),
      sectionFrameIndex,
      interpolant,
      quat);
    m_channelSets[i].getChannelPos()->getPos(
      info[i].getChannelPosInfo(),
      sectionFrameIndex,
      interpolant,
      pos);

    fprintf(file, "chan = %4d, quat = [%3.5f, %3.5f, %3.5f, %3.5f], pos = [%3.5f, %3.5f, %3.5f]\n",
            i, quat.x, quat.y, quat.z, quat.w, pos.x, pos.y, pos.z);
  }
}
  #endif // NM_DEBUG
#endif // NM_HOST_CELL_SPU

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
