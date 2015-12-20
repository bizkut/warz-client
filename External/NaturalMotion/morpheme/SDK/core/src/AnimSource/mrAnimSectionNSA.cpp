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
#include "morpheme/AnimSource/mrAnimSourceUtils.h"
#include "morpheme/AnimSource/mrAnimSectionNSA.h"
#include "morpheme/mrRigToAnimMap.h"

#include "NMPlatform/NMvpu.h"
#include "NMPlatform/NMProfiler.h"

#ifdef NM_HOST_CELL_SPU
  #include "NMPlatform/ps3/NMSPUDMAController.h"
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// UnchangingDataNSA
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void UnchangingDataNSA::locate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_NATURAL_TYPE_ALIGNMENT),
    "Unchanging channel data must be aligned to %d bytes.",
    NMP_NATURAL_TYPE_ALIGNMENT);

  // Header info
  NMP::endianSwap(m_unchangingPosNumChannels);
  NMP::endianSwap(m_unchangingQuatNumChannels);

  m_unchangingPosQuantisationInfo.locate();
  m_unchangingQuatQuantisationInfo.locate();

  // Pos
  if (m_unchangingPosNumChannels > 0)
  {
    REFIX_SWAP_PTR(UnchangingKeyVec3, m_unchangingPosData);
    for (uint16_t i = 0; i < m_unchangingPosNumChannels; ++i)
      m_unchangingPosData[i].locate();  
  }

  // Quat
  if (m_unchangingQuatNumChannels > 0)
  {
    REFIX_SWAP_PTR(UnchangingKeyVec3, m_unchangingQuatData);
    for (uint16_t i = 0; i < m_unchangingQuatNumChannels; ++i)
      m_unchangingQuatData[i].locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataNSA::dislocate()
{    
  // Quat
  if (m_unchangingQuatNumChannels > 0)
  {
    for (uint16_t i = 0; i < m_unchangingQuatNumChannels; ++i)
      m_unchangingQuatData[i].dislocate();
    UNFIX_SWAP_PTR(UnchangingKeyVec3, m_unchangingQuatData);
  }

  // Pos
  if (m_unchangingPosNumChannels > 0)
  {
    for (uint16_t i = 0; i < m_unchangingPosNumChannels; ++i)
      m_unchangingPosData[i].dislocate();
    UNFIX_SWAP_PTR(UnchangingKeyVec3, m_unchangingPosData);
  }

  // Header info
  m_unchangingQuatQuantisationInfo.locate();
  m_unchangingPosQuantisationInfo.locate();

  NMP::endianSwap(m_unchangingQuatNumChannels);
  NMP::endianSwap(m_unchangingPosNumChannels);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
UnchangingDataNSA* UnchangingDataNSA::relocate(void*& ptr)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(UnchangingDataNSA), NMP_NATURAL_TYPE_ALIGNMENT);
  UnchangingDataNSA* result = (UnchangingDataNSA*) NMP::Memory::alignAndIncrement(ptr, memReqsHdr);
 
  // Pos
  if (result->m_unchangingPosNumChannels > 0)
  {
    NMP::Memory::Format memReqsPos = getMemoryReqsUnchangingPosData(result->m_unchangingPosNumChannels);
    result->m_unchangingPosData = (MR::UnchangingKeyVec3*) NMP::Memory::alignAndIncrement(ptr, memReqsPos);
  }  

  // Quat
  if (result->m_unchangingQuatNumChannels > 0)
  {
    NMP::Memory::Format memReqsQuat = getMemoryReqsUnchangingQuatData(result->m_unchangingQuatNumChannels);
    result->m_unchangingQuatData = (MR::UnchangingKeyVec3*) NMP::Memory::alignAndIncrement(ptr, memReqsQuat);
  }

  // Multiple of the memory alignment
  ptr = (void*) NMP::Memory::align(ptr, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataNSA::unchangingPosDecompress(
  const AnimToRigTableMap*    animToRigTableMap,
  const CompToAnimChannelMap* compToAnimTableMap,
  NMP::DataBuffer*            outputBuffer) const
{
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimTableMap);
  NMP_ASSERT(outputBuffer);

  // Avoid decompressing any channels that are not required by this LOD
  const uint16_t* animChannelIndices = compToAnimTableMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);

  NMP::Vector3* pOut = outputBuffer->getPosQuatChannelPos(0);
  NMP_ASSERT(pOut);

  //-----------------------
  // Compute the quantisation scale and offset information 
  NMP::Vector3 qScale_, qOffset_;
  m_unchangingPosQuantisationInfo.unpack(qScale_, qOffset_);
  NMP::vpu::vector4_t qOffset = NMP::vpu::load4f(qOffset_.getPtr());
  NMP::vpu::vector4_t qScale = NMP::vpu::load4f(qScale_.getPtr());  

  // Iterate over the compression channels in blocks of four
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; indx += 4)
  {
    NMP_ASSERT(indx < compToAnimTableMap->getNumChannels());

    // The loop is short, so we prefetch two cache lines ahead.
    NMP::vpu::prefetchCacheLine(&animChannelIndices[indx], (NMP_VPU_CACHESIZE << 1));
    NMP::vpu::prefetchCacheLine(&animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]), (NMP_VPU_CACHESIZE << 1));
    uint32_t rigChannelIndex0 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);
    uint32_t rigChannelIndex1 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 1]);
    uint32_t rigChannelIndex2 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 2]);
    uint32_t rigChannelIndex3 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 3]);

    // Compose the fragmented data into a single continuous block
    NMP::vpu::vector4_t vecX, vecY, vecZ, vecW;
    NMP::vpu::prefetchCacheLine(&m_unchangingPosData[indx], (NMP_VPU_CACHESIZE << 1));
    vecX = NMP::vpu::unpacku4i16f((const uint16_t*) &(m_unchangingPosData[indx]));
    vecY = NMP::vpu::unpacku4i16f((const uint16_t*) &(m_unchangingPosData[indx + 1]));
    vecZ = NMP::vpu::unpacku4i16f((const uint16_t*) &(m_unchangingPosData[indx + 2]));
    vecW = NMP::vpu::unpacku4i16f((const uint16_t*) &(m_unchangingPosData[indx + 3]));

    // use a Vector4MP and skip packing/unpacking
    NMP::vpu::Vector4MP vec4(vecX, vecY, vecZ, vecW);

    // Dequantise the data
    NMP::vpu::Vector4MP pos = vec4 * qScale + qOffset;

    // Store the pos data in the correct rig locations
    NMP::vpu::store4f((float*)&pOut[rigChannelIndex0], pos.x);
    NMP::vpu::store4f((float*)&pOut[rigChannelIndex1], pos.y);
    NMP::vpu::store4f((float*)&pOut[rigChannelIndex2], pos.z);
    NMP::vpu::store4f((float*)&pOut[rigChannelIndex3], pos.w);
  }
}
#else
//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataNSA::unchangingPosDecompress(
  const AnimToRigTableMap*    animToRigTableMap,
  const CompToAnimChannelMap* compToAnimTableMap,
  NMP::DataBuffer*            outputBuffer) const
{
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimTableMap);
  NMP_ASSERT(outputBuffer);

  // Avoid decompressing any channels that are not required by this LOD
  const uint16_t* animChannelIndices = compToAnimTableMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);

  NMP::Vector3* pOut = outputBuffer->getPosQuatChannelPos(0);
  NMP_ASSERT(pOut);

  //-----------------------
  // Compute the quantisation scale and offset information 
  NMP::Vector3 qScale, qOffset;
  m_unchangingPosQuantisationInfo.unpack(qScale, qOffset);

  // Iterate over the compression channels
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; ++indx)
  {
    NMP_ASSERT(indx < compToAnimTableMap->getNumChannels());
    uint32_t rigChannelIndex = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);

    // Dequantise
    NMP::Vector3 pos;
    m_unchangingPosData[indx].unpack(pos);
    pOut[rigChannelIndex] = pos * qScale + qOffset;
  }
}
#endif // NMP_PLATFORM_SIMD

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataNSA::unchangingQuatDecompress(
  const AnimToRigTableMap*    animToRigTableMap,
  const CompToAnimChannelMap* compToAnimTableMap,
  NMP::DataBuffer*            outputBuffer) const
{
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimTableMap);
  NMP_ASSERT(outputBuffer);
  
  // Avoid decompressing any channels that are not required by this LOD
  const uint16_t* animChannelIndices = compToAnimTableMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);

  NMP::Quat* qOut = outputBuffer->getPosQuatChannelQuat(0);
  NMP_ASSERT(qOut);

  //-----------------------
  // Compute the quantisation scale and offset information
  NMP::Vector3 qScale_, qOffset_;
  m_unchangingQuatQuantisationInfo.unpack(qScale_, qOffset_);
  NMP::vpu::Vector3MP qOffset(NMP::vpu::load4f(qOffset_.getPtr()));
  NMP::vpu::Vector3MP qScale(NMP::vpu::load4f(qScale_.getPtr()));

  // Iterate over the compression channels in blocks of four
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; indx += 4)
  {
    NMP_ASSERT(indx < compToAnimTableMap->getNumChannels());

    // The loop is short, so we prefetch two cache lines ahead.
    NMP::vpu::prefetchCacheLine(&animChannelIndices[indx], (NMP_VPU_CACHESIZE << 1));
    NMP::vpu::prefetchCacheLine(&animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]), (NMP_VPU_CACHESIZE << 1));
    uint32_t rigChannelIndex0 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);
    uint32_t rigChannelIndex1 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 1]);
    uint32_t rigChannelIndex2 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 2]);
    uint32_t rigChannelIndex3 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 3]);

    // Compose the fragmented data into a single continuous block
    NMP::vpu::vector4_t vecX, vecY, vecZ, vecW;    
    NMP::vpu::prefetchCacheLine(&m_unchangingQuatData[indx], (NMP_VPU_CACHESIZE << 1));
    vecX = NMP::vpu::unpacku4i16((const uint16_t*) &(m_unchangingQuatData[indx]));
    vecY = NMP::vpu::unpacku4i16((const uint16_t*) &(m_unchangingQuatData[indx + 1]));
    vecZ = NMP::vpu::unpacku4i16((const uint16_t*) &(m_unchangingQuatData[indx + 2]));
    vecW = NMP::vpu::unpacku4i16((const uint16_t*) &(m_unchangingQuatData[indx + 3]));

    NMP::vpu::Vector3MP vec;
    vec.pack(vecX, vecY, vecZ, vecW);
    vec.convert2f();

    // Dequantise the rotation vector data
    vec = vec * qScale + qOffset;

    // Convert the tan quarter angle rotation vectors back to quaternions
    NMP::vpu::QuatMP quat = vec.convertTQARotVecToQuat();

    // Store the quat data in the correct rig locations
    quat.unpack(vecX, vecY, vecZ, vecW);

    NMP::vpu::store4f((float*)&qOut[rigChannelIndex0], vecX);
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex1], vecY);
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex2], vecZ);
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex3], vecW);
  }
}
#else
//----------------------------------------------------------------------------------------------------------------------
void UnchangingDataNSA::unchangingQuatDecompress(
  const AnimToRigTableMap*    animToRigTableMap,
  const CompToAnimChannelMap* compToAnimTableMap,
  NMP::DataBuffer*            outputBuffer) const
{
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimTableMap);
  NMP_ASSERT(outputBuffer);

  // Avoid decompressing any channels that are not required by this LOD
  const uint16_t* animChannelIndices = compToAnimTableMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);

  NMP::Quat* qOut = outputBuffer->getPosQuatChannelQuat(0);
  NMP_ASSERT(qOut);

  //-----------------------
  // Compute the quantisation scale and offset information 
  NMP::Vector3 qScale, qOffset;
  m_unchangingQuatQuantisationInfo.unpack(qScale, qOffset);

  // Iterate over the compression channels
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; ++indx)
  {
    NMP_ASSERT(indx < compToAnimTableMap->getNumChannels());

    uint32_t rigChannelIndex = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);

    // Dequantise the tan quarter angle rotation vector
    NMP::Vector3 v;
    m_unchangingQuatData[indx].unpack(v);
    NMP::Vector3 quatTQA = v * qScale + qOffset;

    // Convert the tan quarter angle rotation vector back to a quaternion
    MR::fromRotationVectorTQA(quatTQA, qOut[rigChannelIndex]);
  }
}
#endif // NMP_PLATFORM_SIMD

//----------------------------------------------------------------------------------------------------------------------
// SectionDataNSA
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format SectionDataNSA::getMemoryReqsMeanAndSetVec3(uint32_t numChannels)
{
  // The quantization data is padded to a multiple of 4 entries to allow for faster decoding.
  // Quantisation data is fetched for 4 bones at a time. This data is then used to look up
  // the quantisation info tables, so the contents of the padded data must be correct (The
  // last entry is repeated). Check AP::SubSectionNSACompiled::compileSectionData.
  //
  // Invalid sample data is written to the output buffer at rig channel 0, so we don't need
  // to worry about padding the keyframe data to a multiple of 4 entries (except after the very
  // last data section to prevent a potential buffer overrun).
  // Check MR::RigToAnimMap::getMemoryRequirements, MR::RigToAnimMap::init,
  // MR::CompToAnimChannelMap::getMemoryRequirements and MR::CompToAnimChannelMap::init.
  //
  // We have to make sure that we have enough padding for any trailing keyframes to
  // prevent reading beyond the allocated animation memory. There are at most 3
  // keyframes that need to be padded, each with XYZ components of uint16_t. Note that
  // each XYZ component is loaded using unpacku4i16, unpacku4i16f. i.e. 4 uint16_t's
  // so we need at most 20 bytes of padding.
  // Check AP::AnimSourceNSABuilder::getMemoryRequirements.
  uint32_t numChannels4 = NMP::nmAlignedValue4(numChannels);
  NMP::Memory::Format result(sizeof(MR::QuantisationMeanAndSetVec3) * numChannels4, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void SectionDataNSA::getMemoryReqsSampledPosData(
  uint32_t    numSectionAnimFrames,
  uint32_t    sampledPosNumChannels,
  size_t&   alignment,
  size_t&     keyFrameDataStride,
  size_t&     keyFrameDataMemReqs)
{
  // Alignment
  alignment = NMP_NATURAL_TYPE_ALIGNMENT;

  // Keyframe stride
  keyFrameDataStride = sizeof(MR::SampledPosKey) * sampledPosNumChannels;
  keyFrameDataStride = NMP::Memory::align(keyFrameDataStride, NMP_NATURAL_TYPE_ALIGNMENT);

  // Memory requirements
  keyFrameDataMemReqs = keyFrameDataStride * numSectionAnimFrames;
}

//----------------------------------------------------------------------------------------------------------------------
void SectionDataNSA::getMemoryReqsSampledQuatData(
  uint32_t    numSectionAnimFrames,
  uint32_t    sampledQuatNumChannels,
  size_t&   alignment,
  size_t&     keyFrameDataStride,
  size_t&     keyFrameDataMemReqs)
{
  // Alignment
  alignment = NMP_NATURAL_TYPE_ALIGNMENT;

  // Keyframe stride
  keyFrameDataStride = sizeof(MR::SampledQuatKeyTQA) * sampledQuatNumChannels;
  keyFrameDataStride = NMP::Memory::align(keyFrameDataStride, NMP_NATURAL_TYPE_ALIGNMENT);

  // Memory requirements
  keyFrameDataMemReqs = keyFrameDataStride * numSectionAnimFrames;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void SectionDataNSA::locate()
{
  uint8_t* data;

  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Sub-sections must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Header
  NMP::endianSwap(m_numSectionAnimFrames);
  NMP::endianSwap(m_sampledPosNumChannels);
  NMP::endianSwap(m_sampledQuatNumChannels);

  //-----------------------
  // Pos
  if (m_sampledPosNumChannels > 0)
  {
    // Sampled pos quantised data
    NMP_ASSERT(m_sampledPosData);
    REFIX_SWAP_PTR(SampledPosKey, m_sampledPosData);

    // Stride of sampled pos keyframe data
    size_t alignment;
    size_t keyFrameDataStride;
    size_t keyFrameDataMemReqs;
    SectionDataNSA::getMemoryReqsSampledPosData(
      m_numSectionAnimFrames,
      m_sampledPosNumChannels,
      alignment,
      keyFrameDataStride,
      keyFrameDataMemReqs);

    // Locate the keyframes
    data = (uint8_t*) m_sampledPosData;
    for (uint16_t iFrame = 0; iFrame < m_numSectionAnimFrames; ++iFrame)
    {
      MR::SampledPosKey* keys = (MR::SampledPosKey*)data;
      for (uint32_t i = 0; i < m_sampledPosNumChannels; ++i)
        keys[i].locate();

      data += keyFrameDataStride;
    }

    // Sampled pos quantisation mean and qSet
    NMP_ASSERT(m_sampledPosQuantisationData);
    REFIX_SWAP_PTR(QuantisationMeanAndSetVec3, m_sampledPosQuantisationData);
    // The quantization data is padded to a multiple of 4 entries.
    // Check SectionDataNSA::getMemoryReqsMeanAndSetVec3.
    uint32_t sampledPosNumChannels4 = NMP::nmAlignedValue4(m_sampledPosNumChannels);
    for (uint32_t i = 0; i < sampledPosNumChannels4; ++i)
    {
      m_sampledPosQuantisationData[i].locate();
    }
  }

  //-----------------------
  // Quat
  if (m_sampledQuatNumChannels > 0)
  {
    // Sampled quat quantised data
    NMP_ASSERT(m_sampledQuatData);
    REFIX_SWAP_PTR(SampledQuatKeyTQA, m_sampledQuatData);

    // Stride of sampled quat keyframe data
    size_t alignment;
    size_t keyFrameDataStride;
    size_t keyFrameDataMemReqs;
    SectionDataNSA::getMemoryReqsSampledQuatData(
      m_numSectionAnimFrames,
      m_sampledQuatNumChannels,
      alignment,
      keyFrameDataStride,
      keyFrameDataMemReqs);

    // Locate the keyframes
    data = (uint8_t*) m_sampledQuatData;
    for (uint16_t iFrame = 0; iFrame < m_numSectionAnimFrames; ++iFrame)
    {
      MR::SampledQuatKeyTQA* keys = (MR::SampledQuatKeyTQA*)data;
      for (uint32_t i = 0; i < m_sampledQuatNumChannels; ++i)
        keys[i].locate();

      data += keyFrameDataStride;
    }
    
    // Sampled quat quantisation mean and qSet
    NMP_ASSERT(m_sampledQuatQuantisationData);
    REFIX_SWAP_PTR(QuantisationMeanAndSetVec3, m_sampledQuatQuantisationData);
    // The quantization data is padded to a multiple of 4 entries.
    // Check SectionDataNSA::getMemoryReqsMeanAndSetVec3.
    uint32_t sampledQuatNumChannels4 = NMP::nmAlignedValue4(m_sampledQuatNumChannels);
    for (uint32_t i = m_sampledQuatNumChannels; i < sampledQuatNumChannels4; ++i)
    {
      m_sampledQuatQuantisationData[i].locate();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void SectionDataNSA::dislocate()
{
  uint8_t* data;

  //-----------------------
  // Quat
  if (m_sampledQuatNumChannels > 0)
  {
    // Sampled quat quantisation mean and qSet
    NMP_ASSERT(m_sampledQuatQuantisationData);
    // The quantization data is padded to a multiple of 4 entries.
    // Check SectionDataNSA::getMemoryReqsMeanAndSetVec3.
    uint32_t sampledQuatNumChannels4 = NMP::nmAlignedValue4(m_sampledQuatNumChannels);
    for (uint32_t i = m_sampledQuatNumChannels; i < sampledQuatNumChannels4; ++i)
    {
      m_sampledQuatQuantisationData[i].dislocate();
    }
    UNFIX_SWAP_PTR(QuantisationMeanAndSetVec3, m_sampledQuatQuantisationData);

    // Stride of sampled quat keyframe data
    size_t alignment;
    size_t keyFrameDataStride;
    size_t keyFrameDataMemReqs;
    SectionDataNSA::getMemoryReqsSampledQuatData(
      m_numSectionAnimFrames,
      m_sampledQuatNumChannels,
      alignment,
      keyFrameDataStride,
      keyFrameDataMemReqs);

    // Locate the keyframes
    NMP_ASSERT(m_sampledQuatData);
    data = (uint8_t*) m_sampledQuatData;
    for (uint16_t iFrame = 0; iFrame < m_numSectionAnimFrames; ++iFrame)
    {
      MR::SampledQuatKeyTQA* keys = (MR::SampledQuatKeyTQA*)data;
      for (uint32_t i = 0; i < m_sampledQuatNumChannels; ++i)
        keys[i].dislocate();

      data += keyFrameDataStride;
    }
    UNFIX_SWAP_PTR(SampledQuatKeyTQA, m_sampledQuatData);
  }
  else
  {
    // For binary invariance
    m_sampledQuatData = NULL;
    m_sampledQuatQuantisationData = NULL;
  }
  
  //-----------------------
  // Pos
  if (m_sampledPosNumChannels > 0)
  {
    // Sampled pos quantisation mean and qSet
    NMP_ASSERT(m_sampledPosQuantisationData);    
    // The quantization data is padded to a multiple of 4 entries.
    // Check SectionDataNSA::getMemoryReqsMeanAndSetVec3.
    uint32_t sampledPosNumChannels4 = NMP::nmAlignedValue4(m_sampledPosNumChannels);
    for (uint32_t i = 0; i < sampledPosNumChannels4; ++i)
    {
      m_sampledPosQuantisationData[i].dislocate();
    }
    UNFIX_SWAP_PTR(QuantisationMeanAndSetVec3, m_sampledPosQuantisationData);

    // Stride of sampled quat keyframe data
    size_t alignment;
    size_t keyFrameDataStride;
    size_t keyFrameDataMemReqs;
    SectionDataNSA::getMemoryReqsSampledPosData(
      m_numSectionAnimFrames,
      m_sampledPosNumChannels,
      alignment,
      keyFrameDataStride,
      keyFrameDataMemReqs);

    // Locate the keyframes
    NMP_ASSERT(m_sampledPosData);
    data = (uint8_t*) m_sampledPosData;
    for (uint16_t iFrame = 0; iFrame < m_numSectionAnimFrames; ++iFrame)
    {
      MR::SampledPosKey* keys = (MR::SampledPosKey*)data;
      for (uint32_t i = 0; i < m_sampledPosNumChannels; ++i)
        keys[i].dislocate();

      data += keyFrameDataStride;
    }
    UNFIX_SWAP_PTR(SampledPosKey, m_sampledPosData);
  }
  else
  {
    // For binary invariance
    m_sampledPosData = NULL;
    m_sampledPosQuantisationData = NULL;
  }

  //-----------------------
  // Header
  NMP::endianSwap(m_sampledQuatNumChannels);
  NMP::endianSwap(m_sampledPosNumChannels);
  NMP::endianSwap(m_numSectionAnimFrames);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
SectionDataNSA* SectionDataNSA::relocate(void*& ptr)
{
  //-----------------------
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(SectionDataNSA), NMP_VECTOR_ALIGNMENT);
  SectionDataNSA* result = (SectionDataNSA*) NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  //-----------------------
  // Pos
  if (result->m_sampledPosNumChannels > 0)
  {
    // Sampled pos quantised data (Not block-4 aligned)
    NMP::Memory::Format memReqsPos;
    size_t keyFrameDataStride;
    MR::SectionDataNSA::getMemoryReqsSampledPosData(
      result->m_numSectionAnimFrames,
      result->m_sampledPosNumChannels,
      memReqsPos.alignment,
      keyFrameDataStride,
      memReqsPos.size);
    result->m_sampledPosData = (MR::SampledPosKey*) NMP::Memory::alignAndIncrement(ptr, memReqsPos);

    // Sampled pos quantisation mean and qSet.
    NMP::Memory::Format memReqsMeanAndSetVec3 = MR::SectionDataNSA::getMemoryReqsMeanAndSetVec3(result->m_sampledPosNumChannels);
    result->m_sampledPosQuantisationData = (MR::QuantisationMeanAndSetVec3*) NMP::Memory::alignAndIncrement(ptr, memReqsMeanAndSetVec3);
  }

  //-----------------------
  // Quat
  if (result->m_sampledQuatNumChannels > 0)
  {
    // Sampled quat quantised data (Not block-4 aligned)
    NMP::Memory::Format memReqsQuat;
    size_t keyFrameDataStride;
    MR::SectionDataNSA::getMemoryReqsSampledQuatData(
      result->m_numSectionAnimFrames,
      result->m_sampledQuatNumChannels,
      memReqsQuat.alignment,
      keyFrameDataStride,
      memReqsQuat.size);
    result->m_sampledQuatData = (MR::SampledQuatKeyTQA*) NMP::Memory::alignAndIncrement(ptr, memReqsQuat);

    // Sampled quat quantisation mean and qSet.
    NMP::Memory::Format memReqsMeanAndSetVec3 = MR::SectionDataNSA::getMemoryReqsMeanAndSetVec3(result->m_sampledQuatNumChannels);
    result->m_sampledQuatQuantisationData = (MR::QuantisationMeanAndSetVec3*) NMP::Memory::alignAndIncrement(ptr, memReqsMeanAndSetVec3);
  }

  // Multiple of the memory alignment
  ptr = (void*) NMP::Memory::align(ptr, NMP_VECTOR_ALIGNMENT);

  return result;
}

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void SectionDataNSA::sampledPosDecompress(
  const QuantisationScaleAndOffsetVec3&   posMeansQuantisationInfo,
  const QuantisationScaleAndOffsetVec3*   sampledPosQuantisationInfo,
  const AnimToRigTableMap*                animToRigTableMap,
  const CompToAnimChannelMap*             compToAnimTableMap,
  uint32_t                                sectionFrameIndex,
  float                                   interpolant,
  NMP::DataBuffer*                        outputBuffer) const
{
  NMP_ASSERT(outputBuffer);
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimTableMap);
  NMP_ASSERT(sectionFrameIndex < (uint32_t)m_numSectionAnimFrames - 1); // Can't be the last frame because of interpolation
  NMP_ASSERT(interpolant >= 0.0f && interpolant <= 1.0f); // Last frame is recovered with interpolant = 1.0

  // Check we have some key-frame data
  if (m_sampledPosNumChannels == 0)
    return;

  NMP_ASSERT(sampledPosQuantisationInfo);
  NMP_ASSERT(NMP_IS_ALIGNED(m_sampledPosQuantisationData, NMP_NATURAL_TYPE_ALIGNMENT));

  // Avoid decompressing any channels that are not required by this LOD
  const uint16_t* animChannelIndices = compToAnimTableMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);

  NMP::Vector3* pOut = outputBuffer->getPosQuatChannelPos(0);
  NMP_ASSERT(pOut);
 
  // Get the sampled keys
  size_t alignment;
  size_t keyFrameDataStride;
  size_t keyFrameDataMemReqs;
  SectionDataNSA::getMemoryReqsSampledPosData(
    m_numSectionAnimFrames,
    m_sampledPosNumChannels,
    alignment,
    keyFrameDataStride,
    keyFrameDataMemReqs);

  NMP_ASSERT(m_sampledPosData);
  NMP_ASSERT(NMP_IS_ALIGNED(m_sampledPosData, alignment));
  const uint8_t* data = (const uint8_t*) m_sampledPosData;
  data += (sectionFrameIndex * keyFrameDataStride);
  const SampledPosKey* sampledPosKeysA = (const SampledPosKey*)data;
  data += keyFrameDataStride;
  const SampledPosKey* sampledPosKeysB = (const SampledPosKey*)data;

  // Compute the quantisation scale and offset information for the channel means
  NMP::Vector3 qScale_, qOffset_;
  posMeansQuantisationInfo.unpack(qScale_, qOffset_);
  NMP::vpu::Vector3MP qOffsetMean(NMP::vpu::load4f(qOffset_.getPtr()));
  NMP::vpu::Vector3MP qScaleMean(NMP::vpu::load4f(qScale_.getPtr()));

  NMP::vpu::vector4_t sampledUParam = NMP::vpu::set4f(interpolant);

  // Iterate over the compression channels in blocks of four
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; indx += 4)
  {
    NMP_ASSERT(indx < compToAnimTableMap->getNumChannels());

    NMP::vpu::prefetchCacheLine(&animChannelIndices[indx], NMP_VPU_CACHESIZE);
    NMP::vpu::prefetchCacheLine(&animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]), NMP_VPU_CACHESIZE);
    uint32_t rigChannelIndex0 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);
    uint32_t rigChannelIndex1 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 1]);
    uint32_t rigChannelIndex2 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 2]);
    uint32_t rigChannelIndex3 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 3]);

    NMP::vpu::vector4_t vecX, vecY, vecZ, vecW;

    // Get a reference to the quantisation data: mean/qset
    NMP::vpu::prefetchCacheLine(&m_sampledPosQuantisationData[indx], NMP_VPU_CACHESIZE);
    const QuantisationMeanAndSetVec3& qData0 = m_sampledPosQuantisationData[indx];
    const QuantisationMeanAndSetVec3& qData1 = m_sampledPosQuantisationData[indx + 1];
    const QuantisationMeanAndSetVec3& qData2 = m_sampledPosQuantisationData[indx + 2];
    const QuantisationMeanAndSetVec3& qData3 = m_sampledPosQuantisationData[indx + 3];

    // Compose the fragmented quantised channel mean data into a single continuous block
    vecX = NMP::vpu::unpacku4i8(qData0.getQuantisedMean());
    vecY = NMP::vpu::unpacku4i8(qData1.getQuantisedMean());
    vecZ = NMP::vpu::unpacku4i8(qData2.getQuantisedMean());
    vecW = NMP::vpu::unpacku4i8(qData3.getQuantisedMean());

    NMP::vpu::Vector3MP vecMean;
    vecMean.pack(vecX, vecY, vecZ, vecW);
    vecMean.convert2f();

    // Dequantise the channel means
    NMP::vpu::Vector3MP pBar = vecMean * qScaleMean + qOffsetMean;

    // Compose the fragmented quantisation set data into a single continuous block
    uint32_t qSet0[3], qSet1[3], qSet2[3], qSet3[3];
    qData0.unpackQSet(qSet0);
    qData1.unpackQSet(qSet1);
    qData2.unpackQSet(qSet2);
    qData3.unpackQSet(qSet3);
    
    // X Components
    const QuantisationScaleAndOffsetVec3& qSetX0 = sampledPosQuantisationInfo[qSet0[0]];
    const QuantisationScaleAndOffsetVec3& qSetX1 = sampledPosQuantisationInfo[qSet1[0]];
    const QuantisationScaleAndOffsetVec3& qSetX2 = sampledPosQuantisationInfo[qSet2[0]];
    const QuantisationScaleAndOffsetVec3& qSetX3 = sampledPosQuantisationInfo[qSet3[0]];
    NMP::vpu::vector4_t qOffsetX = NMP::vpu::set4f(
      qSetX0.getQuantisationSetOffsetX(),
      qSetX1.getQuantisationSetOffsetX(),
      qSetX2.getQuantisationSetOffsetX(),
      qSetX3.getQuantisationSetOffsetX());
    NMP::vpu::vector4_t qScaleX = NMP::vpu::set4f(
      qSetX0.getQuantisationSetScaleX(),
      qSetX1.getQuantisationSetScaleX(),
      qSetX2.getQuantisationSetScaleX(),
      qSetX3.getQuantisationSetScaleX());

    // Y Components
    const QuantisationScaleAndOffsetVec3& qSetY0 = sampledPosQuantisationInfo[qSet0[1]];
    const QuantisationScaleAndOffsetVec3& qSetY1 = sampledPosQuantisationInfo[qSet1[1]];
    const QuantisationScaleAndOffsetVec3& qSetY2 = sampledPosQuantisationInfo[qSet2[1]];
    const QuantisationScaleAndOffsetVec3& qSetY3 = sampledPosQuantisationInfo[qSet3[1]];
    NMP::vpu::vector4_t qOffsetY = NMP::vpu::set4f(
      qSetY0.getQuantisationSetOffsetY(),
      qSetY1.getQuantisationSetOffsetY(),
      qSetY2.getQuantisationSetOffsetY(),
      qSetY3.getQuantisationSetOffsetY());
    NMP::vpu::vector4_t qScaleY = NMP::vpu::set4f(
      qSetY0.getQuantisationSetScaleY(),
      qSetY1.getQuantisationSetScaleY(),
      qSetY2.getQuantisationSetScaleY(),
      qSetY3.getQuantisationSetScaleY());

    // Z Components
    const QuantisationScaleAndOffsetVec3& qSetZ0 = sampledPosQuantisationInfo[qSet0[2]];
    const QuantisationScaleAndOffsetVec3& qSetZ1 = sampledPosQuantisationInfo[qSet1[2]];
    const QuantisationScaleAndOffsetVec3& qSetZ2 = sampledPosQuantisationInfo[qSet2[2]];
    const QuantisationScaleAndOffsetVec3& qSetZ3 = sampledPosQuantisationInfo[qSet3[2]];
    NMP::vpu::vector4_t qOffsetZ = NMP::vpu::set4f(
      qSetZ0.getQuantisationSetOffsetZ(),
      qSetZ1.getQuantisationSetOffsetZ(),
      qSetZ2.getQuantisationSetOffsetZ(),
      qSetZ3.getQuantisationSetOffsetZ());
    NMP::vpu::vector4_t qScaleZ = NMP::vpu::set4f(
      qSetZ0.getQuantisationSetScaleZ(),
      qSetZ1.getQuantisationSetScaleZ(),
      qSetZ2.getQuantisationSetScaleZ(),
      qSetZ3.getQuantisationSetScaleZ());

    // SOA format
    NMP::vpu::Vector3MP qOffset(qOffsetX, qOffsetY, qOffsetZ);
    NMP::vpu::Vector3MP qScale(qScaleX, qScaleY, qScaleZ);

    // Compose the fragmented data into a single continuous block
    NMP::vpu::prefetchCacheLine(&sampledPosKeysA[indx], NMP_VPU_CACHESIZE);
    vecX = NMP::vpu::unpackQuantizedVectorData((const uint32_t*) &(sampledPosKeysA[indx]));
    vecY = NMP::vpu::unpackQuantizedVectorData((const uint32_t*) &(sampledPosKeysA[indx + 1]));
    vecZ = NMP::vpu::unpackQuantizedVectorData((const uint32_t*) &(sampledPosKeysA[indx + 2]));
    vecW = NMP::vpu::unpackQuantizedVectorData((const uint32_t*) &(sampledPosKeysA[indx + 3]));

    NMP::vpu::Vector3MP vecA;
    vecA.pack(vecX, vecY, vecZ, vecW);
    vecA.convert2f();

    NMP::vpu::prefetchCacheLine(&sampledPosKeysB[indx], NMP_VPU_CACHESIZE);
    vecX = NMP::vpu::unpackQuantizedVectorData((const uint32_t*) &(sampledPosKeysB[indx]));
    vecY = NMP::vpu::unpackQuantizedVectorData((const uint32_t*) &(sampledPosKeysB[indx + 1]));
    vecZ = NMP::vpu::unpackQuantizedVectorData((const uint32_t*) &(sampledPosKeysB[indx + 2]));
    vecW = NMP::vpu::unpackQuantizedVectorData((const uint32_t*) &(sampledPosKeysB[indx + 3]));

    NMP::vpu::Vector3MP vecB;
    vecB.pack(vecX, vecY, vecZ, vecW);
    vecB.convert2f();

    // Interpolate
    NMP::vpu::Vector3MP pos;
    pos.lerp(vecA, vecB, sampledUParam);
    
    // Re-apply the channel means
    pos = pos * qScale + qOffset;
    pos += pBar;
    pos.unpack(vecX, vecY, vecZ, vecW);

    // Store the pos data in the correct rig locations
    NMP::vpu::store4f((float*)&pOut[rigChannelIndex0], vecX);
    NMP::vpu::store4f((float*)&pOut[rigChannelIndex1], vecY);
    NMP::vpu::store4f((float*)&pOut[rigChannelIndex2], vecZ);
    NMP::vpu::store4f((float*)&pOut[rigChannelIndex3], vecW);
  }
}

#else
//----------------------------------------------------------------------------------------------------------------------
void SectionDataNSA::sampledPosDecompress(
  const QuantisationScaleAndOffsetVec3&   posMeansQuantisationInfo,
  const QuantisationScaleAndOffsetVec3*   sampledPosQuantisationInfo,
  const AnimToRigTableMap*                animToRigTableMap,
  const CompToAnimChannelMap*             compToAnimTableMap,
  uint32_t                                sectionFrameIndex,
  float                                   interpolant,
  NMP::DataBuffer*                        outputBuffer) const
{
  NMP_ASSERT(outputBuffer);
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimTableMap);
  NMP_ASSERT(sectionFrameIndex < (uint32_t)m_numSectionAnimFrames - 1); // Can't be the last frame because of interpolation
  NMP_ASSERT(interpolant >= 0.0f && interpolant <= 1.0f); // Last frame is recovered with interpolant = 1.0

  // Check we have some key-frame data
  if (m_sampledPosNumChannels == 0)
    return;

  NMP_ASSERT(sampledPosQuantisationInfo);
  NMP_ASSERT(NMP_IS_ALIGNED(m_sampledPosQuantisationData, NMP_NATURAL_TYPE_ALIGNMENT));

  // Avoid decompressing any channels that are not required by this LOD
  const uint16_t* animChannelIndices = compToAnimTableMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);

  NMP::Vector3* pOut = outputBuffer->getPosQuatChannelPos(0);
  NMP_ASSERT(pOut);

  // Get the sampled keys
  size_t alignment;
  size_t keyFrameDataStride;
  size_t keyFrameDataMemReqs;
  SectionDataNSA::getMemoryReqsSampledPosData(
    m_numSectionAnimFrames,
    m_sampledPosNumChannels,
    alignment,
    keyFrameDataStride,
    keyFrameDataMemReqs);

  NMP_ASSERT(m_sampledPosData);
  NMP_ASSERT(NMP_IS_ALIGNED(m_sampledPosData, alignment));
  const uint8_t* data = (const uint8_t*) m_sampledPosData;
  data += (sectionFrameIndex * keyFrameDataStride);
  const SampledPosKey* sampledPosKeysA = (const SampledPosKey*)data;
  data += keyFrameDataStride;
  const SampledPosKey* sampledPosKeysB = (const SampledPosKey*)data;

  // Compute the quantisation scale and offset information for the channel means
  NMP::Vector3 qScaleMean, qOffsetMean;
  posMeansQuantisationInfo.unpack(qScaleMean, qOffsetMean);

  // Iterate over the compression channels
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; ++indx)
  {
    NMP_ASSERT(indx < compToAnimTableMap->getNumChannels());
    uint32_t rigChannelIndex = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);

    // Dequantise the channel mean
    NMP::Vector3 vecMean;
    const QuantisationMeanAndSetVec3& qData = m_sampledPosQuantisationData[indx];
    qData.unpackQMean(vecMean);
    NMP::Vector3 pbar = vecMean * qScaleMean + qOffsetMean;

    // Quantisation set basis
    uint32_t qSet[3];
    qData.unpackQSet(qSet);
    const QuantisationScaleAndOffsetVec3& qSetX = sampledPosQuantisationInfo[qSet[0]];
    const QuantisationScaleAndOffsetVec3& qSetY = sampledPosQuantisationInfo[qSet[1]];
    const QuantisationScaleAndOffsetVec3& qSetZ = sampledPosQuantisationInfo[qSet[2]];
    NMP::Vector3 qScale;
    qScale.x = qSetX.getQuantisationSetScaleX();
    qScale.y = qSetY.getQuantisationSetScaleY();
    qScale.z = qSetZ.getQuantisationSetScaleZ();
    NMP::Vector3 qOffset;
    qOffset.x = qSetX.getQuantisationSetOffsetX();
    qOffset.y = qSetY.getQuantisationSetOffsetY();
    qOffset.z = qSetZ.getQuantisationSetOffsetZ();

    // Dequantise the channel data
    NMP::Vector3 vecA, vecB;
    sampledPosKeysA[indx].unpack(vecA);
    sampledPosKeysB[indx].unpack(vecB);
    
    // Interpolate
    NMP::Vector3 posRel;
    posRel.lerp(vecA, vecB, interpolant);
    posRel = posRel * qScale + qOffset;

    // Re-apply the channel mean
    pOut[rigChannelIndex] = posRel + pbar;
  }
}
#endif // NMP_PLATFORM_SIMD

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void SectionDataNSA::sampledQuatDecompress(
  const QuantisationScaleAndOffsetVec3*   sampledQuatQuantisationInfo,
  const AnimToRigTableMap*                animToRigTableMap,
  const CompToAnimChannelMap*             compToAnimTableMap,
  uint32_t                                sectionFrameIndex,
  float                                   interpolant,
  NMP::DataBuffer*                        outputBuffer) const
{
  NMP_ASSERT(outputBuffer);
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimTableMap);
  NMP_ASSERT(sectionFrameIndex < (uint32_t)m_numSectionAnimFrames - 1); // Can't be the last frame because of interpolation
  NMP_ASSERT(interpolant >= 0.0f && interpolant <= 1.0f); // Last frame is recovered with interpolant = 1.0

  // Check we have some key-frame data
  if (m_sampledQuatNumChannels == 0)
    return;

  NMP_ASSERT(sampledQuatQuantisationInfo);
  NMP_ASSERT(NMP_IS_ALIGNED(m_sampledQuatQuantisationData, NMP_NATURAL_TYPE_ALIGNMENT));

  // Avoid decompressing any channels that are not required by this LOD
  const uint16_t* animChannelIndices = compToAnimTableMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);

  NMP::Quat* qOut = outputBuffer->getPosQuatChannelQuat(0);
  NMP_ASSERT(qOut);

  // Get the sampled keys
  size_t alignment;
  size_t keyFrameDataStride;
  size_t keyFrameDataMemReqs;
  SectionDataNSA::getMemoryReqsSampledQuatData(
    m_numSectionAnimFrames,
    m_sampledQuatNumChannels,
    alignment,
    keyFrameDataStride,
    keyFrameDataMemReqs);

  NMP_ASSERT(m_sampledQuatData);
  NMP_ASSERT(NMP_IS_ALIGNED(m_sampledQuatData, alignment));
  const uint8_t* data = (const uint8_t*) m_sampledQuatData;
  data += (sectionFrameIndex * keyFrameDataStride);
  const SampledQuatKeyTQA* sampledQuatKeysA = (const SampledQuatKeyTQA*)data;
  data += keyFrameDataStride;
  const SampledQuatKeyTQA* sampledQuatKeysB = (const SampledQuatKeyTQA*)data;

  // Compute the quantisation scale and offset information for the channel means
  NMP::vpu::vector4_t qMinMean = NMP::vpu::negOne4f();
  NMP::vpu::vector4_t qScaleMean = NMP::vpu::set4f(2.0f / 255);

  NMP::vpu::vector4_t sampledUParam = NMP::vpu::set4f(interpolant);

  // Iterate over the compression channels in blocks of four
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; indx += 4)
  {
    NMP_ASSERT(indx < compToAnimTableMap->getNumChannels());

    NMP::vpu::prefetchCacheLine(&animChannelIndices[indx], NMP_VPU_CACHESIZE);
    NMP::vpu::prefetchCacheLine(&animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]), NMP_VPU_CACHESIZE);
    uint32_t rigChannelIndex0 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);
    uint32_t rigChannelIndex1 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 1]);
    uint32_t rigChannelIndex2 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 2]);
    uint32_t rigChannelIndex3 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 3]);

    // Get a reference to the quantisation data: mean/prec/qset
    NMP::vpu::prefetchCacheLine(&m_sampledQuatQuantisationData[indx], NMP_VPU_CACHESIZE);
    const QuantisationMeanAndSetVec3& qData0 = m_sampledQuatQuantisationData[indx];
    const QuantisationMeanAndSetVec3& qData1 = m_sampledQuatQuantisationData[indx + 1];
    const QuantisationMeanAndSetVec3& qData2 = m_sampledQuatQuantisationData[indx + 2];
    const QuantisationMeanAndSetVec3& qData3 = m_sampledQuatQuantisationData[indx + 3];

    // Compose the fragmented quantised channel mean data into a single continuous block
    NMP::vpu::vector4_t vecX, vecY, vecZ, vecW;

    vecX = NMP::vpu::unpacku4i8(qData0.getQuantisedMean());
    vecY = NMP::vpu::unpacku4i8(qData1.getQuantisedMean());
    vecZ = NMP::vpu::unpacku4i8(qData2.getQuantisedMean());
    vecW = NMP::vpu::unpacku4i8(qData3.getQuantisedMean());

    NMP::vpu::Vector3MP vecMean;
    vecMean.pack(vecX, vecY, vecZ, vecW);
    vecMean.convert2f();

    // Dequantise the channel means
    vecMean = vecMean * qScaleMean + qMinMean;

    // Convert the tan quarter angle rotation vectors back to quaternions
    NMP::vpu::QuatMP qBar = vecMean.convertTQARotVecToQuat();

    //-----------------------
    // Compose the fragmented quantisation set data into a single continuous block
    uint32_t qSet0[3], qSet1[3], qSet2[3], qSet3[3];
    qData0.unpackQSet(qSet0);
    qData1.unpackQSet(qSet1);
    qData2.unpackQSet(qSet2);
    qData3.unpackQSet(qSet3);
    
    // X Components
    const QuantisationScaleAndOffsetVec3& qSetX0 = sampledQuatQuantisationInfo[qSet0[0]];
    const QuantisationScaleAndOffsetVec3& qSetX1 = sampledQuatQuantisationInfo[qSet1[0]];
    const QuantisationScaleAndOffsetVec3& qSetX2 = sampledQuatQuantisationInfo[qSet2[0]];
    const QuantisationScaleAndOffsetVec3& qSetX3 = sampledQuatQuantisationInfo[qSet3[0]];
    NMP::vpu::vector4_t qOffsetX = NMP::vpu::set4f(
      qSetX0.getQuantisationSetOffsetX(),
      qSetX1.getQuantisationSetOffsetX(),
      qSetX2.getQuantisationSetOffsetX(),
      qSetX3.getQuantisationSetOffsetX());
    NMP::vpu::vector4_t qScaleX = NMP::vpu::set4f(
      qSetX0.getQuantisationSetScaleX(),
      qSetX1.getQuantisationSetScaleX(),
      qSetX2.getQuantisationSetScaleX(),
      qSetX3.getQuantisationSetScaleX());
      
    // Y Components
    const QuantisationScaleAndOffsetVec3& qSetY0 = sampledQuatQuantisationInfo[qSet0[1]];
    const QuantisationScaleAndOffsetVec3& qSetY1 = sampledQuatQuantisationInfo[qSet1[1]];
    const QuantisationScaleAndOffsetVec3& qSetY2 = sampledQuatQuantisationInfo[qSet2[1]];
    const QuantisationScaleAndOffsetVec3& qSetY3 = sampledQuatQuantisationInfo[qSet3[1]];
    NMP::vpu::vector4_t qOffsetY = NMP::vpu::set4f(
      qSetY0.getQuantisationSetOffsetY(),
      qSetY1.getQuantisationSetOffsetY(),
      qSetY2.getQuantisationSetOffsetY(),
      qSetY3.getQuantisationSetOffsetY());
    NMP::vpu::vector4_t qScaleY = NMP::vpu::set4f(
      qSetY0.getQuantisationSetScaleY(),
      qSetY1.getQuantisationSetScaleY(),
      qSetY2.getQuantisationSetScaleY(),
      qSetY3.getQuantisationSetScaleY());

    // Z Components
    const QuantisationScaleAndOffsetVec3& qSetZ0 = sampledQuatQuantisationInfo[qSet0[2]];
    const QuantisationScaleAndOffsetVec3& qSetZ1 = sampledQuatQuantisationInfo[qSet1[2]];
    const QuantisationScaleAndOffsetVec3& qSetZ2 = sampledQuatQuantisationInfo[qSet2[2]];
    const QuantisationScaleAndOffsetVec3& qSetZ3 = sampledQuatQuantisationInfo[qSet3[2]];
    NMP::vpu::vector4_t qOffsetZ = NMP::vpu::set4f(
      qSetZ0.getQuantisationSetOffsetZ(),
      qSetZ1.getQuantisationSetOffsetZ(),
      qSetZ2.getQuantisationSetOffsetZ(),
      qSetZ3.getQuantisationSetOffsetZ());
    NMP::vpu::vector4_t qScaleZ = NMP::vpu::set4f(
      qSetZ0.getQuantisationSetScaleZ(),
      qSetZ1.getQuantisationSetScaleZ(),
      qSetZ2.getQuantisationSetScaleZ(),
      qSetZ3.getQuantisationSetScaleZ());

    // SOA format
    NMP::vpu::Vector3MP qOffset(qOffsetX, qOffsetY, qOffsetZ);
    NMP::vpu::Vector3MP qScale(qScaleX, qScaleY, qScaleZ);

    // Compose the fragmented data into a single continuous block
    NMP::vpu::prefetchCacheLine(&sampledQuatKeysA[indx], NMP_VPU_CACHESIZE);
    vecX = NMP::vpu::unpacku4i16((const uint16_t*) &(sampledQuatKeysA[indx]));
    vecY = NMP::vpu::unpacku4i16((const uint16_t*) &(sampledQuatKeysA[indx + 1]));
    vecZ = NMP::vpu::unpacku4i16((const uint16_t*) &(sampledQuatKeysA[indx + 2]));
    vecW = NMP::vpu::unpacku4i16((const uint16_t*) &(sampledQuatKeysA[indx + 3]));

    NMP::vpu::Vector3MP vecA;
    vecA.pack(vecX, vecY, vecZ, vecW);
    vecA.convert2f();

    NMP::vpu::prefetchCacheLine(&sampledQuatKeysB[indx], NMP_VPU_CACHESIZE);
    vecX = NMP::vpu::unpacku4i16((const uint16_t*) &(sampledQuatKeysB[indx]));
    vecY = NMP::vpu::unpacku4i16((const uint16_t*) &(sampledQuatKeysB[indx + 1]));
    vecZ = NMP::vpu::unpacku4i16((const uint16_t*) &(sampledQuatKeysB[indx + 2]));
    vecW = NMP::vpu::unpacku4i16((const uint16_t*) &(sampledQuatKeysB[indx + 3]));

    NMP::vpu::Vector3MP vecB;
    vecB.pack(vecX, vecY, vecZ, vecW);
    vecB.convert2f();

    // Dequantise the keys data
    vecA = vecA * qScale + qOffset;
    vecB = vecB * qScale + qOffset;

    // Convert the tan quarter angle rotation vectors back to quaternions
    NMP::vpu::QuatMP quatA = vecA.convertTQARotVecToQuat();
    NMP::vpu::QuatMP quatB = vecB.convertTQARotVecToQuat();

    // Interpolation
    NMP::vpu::QuatMP qRel;
    qRel.interpBlend(quatA, quatB, sampledUParam);

    // Apply the mean to the interpolated value
    NMP::vpu::QuatMP quat = qBar * qRel;
    quat.unpack(vecX, vecY, vecZ, vecW);

    // Store the quat data in the correct rig locations
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex0], vecX);
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex1], vecY);
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex2], vecZ);
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex3], vecW);
  }
}

#else
//----------------------------------------------------------------------------------------------------------------------
void SectionDataNSA::sampledQuatDecompress(
  const QuantisationScaleAndOffsetVec3*   sampledQuatQuantisationInfo,
  const AnimToRigTableMap*                animToRigTableMap,
  const CompToAnimChannelMap*             compToAnimTableMap,
  uint32_t                                sectionFrameIndex,
  float                                   interpolant,
  NMP::DataBuffer*                        outputBuffer) const
{
  NMP_ASSERT(outputBuffer);
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimTableMap);
  NMP_ASSERT(sectionFrameIndex < (uint32_t)m_numSectionAnimFrames - 1); // Can't be the last frame because of interpolation
  NMP_ASSERT(interpolant >= 0.0f && interpolant <= 1.0f); // Last frame is recovered with interpolant = 1.0

  // Check we have some key-frame data
  if (m_sampledQuatNumChannels == 0)
    return;

  NMP_ASSERT(sampledQuatQuantisationInfo);
  NMP_ASSERT(NMP_IS_ALIGNED(m_sampledQuatQuantisationData, NMP_NATURAL_TYPE_ALIGNMENT));

  // Avoid decompressing any channels that are not required by this LOD
  const uint16_t* animChannelIndices = compToAnimTableMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);

  NMP::Quat* qOut = outputBuffer->getPosQuatChannelQuat(0);
  NMP_ASSERT(qOut);

  // Get the sampled keys
  size_t alignment;
  size_t keyFrameDataStride;
  size_t keyFrameDataMemReqs;
  SectionDataNSA::getMemoryReqsSampledQuatData(
    m_numSectionAnimFrames,
    m_sampledQuatNumChannels,
    alignment,
    keyFrameDataStride,
    keyFrameDataMemReqs);

  NMP_ASSERT(m_sampledQuatData);
  NMP_ASSERT(NMP_IS_ALIGNED(m_sampledQuatData, alignment));
  const uint8_t* data = (const uint8_t*) m_sampledQuatData;
  data += (sectionFrameIndex * keyFrameDataStride);
  const SampledQuatKeyTQA* sampledQuatKeysA = (const SampledQuatKeyTQA*)data;
  data += keyFrameDataStride;
  const SampledQuatKeyTQA* sampledQuatKeysB = (const SampledQuatKeyTQA*)data;

  // Compute the quantisation scale and offset information for the means
  NMP::Vector3 qScaleMean, qOffsetMean;
  qScaleMean.set(2.0f / 255);
  qOffsetMean.set(-1.0f);

  // Iterate over the compression channels
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; ++indx)
  {
    NMP_ASSERT(indx < compToAnimTableMap->getNumChannels());
    uint32_t rigChannelIndex = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);

    // Dequantise the channel mean
    NMP::Vector3 vecMean;
    const QuantisationMeanAndSetVec3& qData = m_sampledQuatQuantisationData[indx];
    qData.unpackQMean(vecMean);
    vecMean = vecMean * qScaleMean + qOffsetMean;
    NMP::Quat qbar;
    MR::fromRotationVectorTQA(vecMean, qbar);

    // Quantisation set basis
    uint32_t qSet[3];
    qData.unpackQSet(qSet);
    const QuantisationScaleAndOffsetVec3& qSetX = sampledQuatQuantisationInfo[qSet[0]];
    const QuantisationScaleAndOffsetVec3& qSetY = sampledQuatQuantisationInfo[qSet[1]];
    const QuantisationScaleAndOffsetVec3& qSetZ = sampledQuatQuantisationInfo[qSet[2]];
    NMP::Vector3 qScale;
    qScale.x = qSetX.getQuantisationSetScaleX();
    qScale.y = qSetY.getQuantisationSetScaleY();
    qScale.z = qSetZ.getQuantisationSetScaleZ();
    NMP::Vector3 qOffset;
    qOffset.x = qSetX.getQuantisationSetOffsetX();
    qOffset.y = qSetY.getQuantisationSetOffsetY();
    qOffset.z = qSetZ.getQuantisationSetOffsetZ();

    // Dequantise the channel data
    NMP::Vector3 vecA, vecB;
    sampledQuatKeysA[indx].unpack(vecA);
    sampledQuatKeysB[indx].unpack(vecB);
    vecA = vecA * qScale + qOffset;
    vecB = vecB * qScale + qOffset;

    // Convert the tan quarter angle rotation vectors back to quaternions
    NMP::Quat quatA, quatB;
    MR::fromRotationVectorTQA(vecA, quatA);
    MR::fromRotationVectorTQA(vecB, quatB);

    // Interpolation
    float fromDotTo = quatA.dot(quatB);
    float dotSign = NMP::floatSelect(fromDotTo, 1.0f, -1.0f);
    quatB *= dotSign;
    fromDotTo *= dotSign;
    NMP::Quat quatRel;
    quatRel.fastSlerp(quatA, quatB, interpolant, fromDotTo);

    // Apply the channel mean to the interpolated value
    qOut[rigChannelIndex] = qbar * quatRel;
  }
}
#endif // NMP_PLATFORM_SIMD

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
