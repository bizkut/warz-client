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
#include "NMPlatform/NMBitStreamCoder.h"
#include "NMPlatform/NMvpu.h"
#include "NMPlatform/NMProfiler.h"
#include "morpheme/AnimSource/mrAnimSourceUtils.h"
#include "morpheme/AnimSource/mrAnimSourceQSA.h"
#include "morpheme/AnimSource/mrAnimSectionQSA.h"
#include "morpheme/mrRigToAnimMap.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4701) // potentially uninitialized local variable
#endif

#define QSA_CHAN_METHOD_UNCHANGING    (0)
#define QSA_CHAN_METHOD_SAMPLED       (1)
#define QSA_CHAN_METHOD_SPLINE        (2)

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// UnchangingKeyQSA
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void UnchangingKeyQSA::locate()
{
  NMP::endianSwapArray(m_data, 3);
}

void UnchangingKeyQSA::dislocate()
{
  NMP::endianSwapArray(m_data, 3);
}

//----------------------------------------------------------------------------------------------------------------------
// QuantisationInfoQSA
//----------------------------------------------------------------------------------------------------------------------
void QuantisationInfoQSA::locate()
{
  NMP::endianSwapArray(m_qMin, 3);
  NMP::endianSwapArray(m_qMax, 3);
}

void QuantisationInfoQSA::dislocate()
{
  NMP::endianSwapArray(m_qMin, 3);
  NMP::endianSwapArray(m_qMax, 3);
}

void QuantisationInfoQSA::clear()
{
  m_qMin[0] = m_qMin[1] = m_qMin[2] = 0.0f;
  m_qMax[0] = m_qMax[1] = m_qMax[2] = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
// QuantisationDataQSA
//----------------------------------------------------------------------------------------------------------------------
void QuantisationDataQSA::locate()
{
  NMP::endianSwapArray(m_prec, 3);
  NMP::endianSwapArray(m_mean, 3);
  NMP::endianSwapArray(m_qSet, 3);
}

void QuantisationDataQSA::dislocate()
{
  NMP::endianSwapArray(m_qSet, 3);
  NMP::endianSwapArray(m_mean, 3);
  NMP::endianSwapArray(m_prec, 3);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// ChannelSetInfoQSA
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void ChannelSetInfoQSA::locate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Channel set quantisation information must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Header info
  NMP::endianSwapArray((float*)&m_worldspaceRootQuat, 4);
  NMP::endianSwap(m_worldspaceRootID);
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_unchangingDefaultPosNumChannels);
  NMP::endianSwap(m_unchangingDefaultQuatNumChannels);
  NMP::endianSwap(m_unchangingPosNumChannels);
  NMP::endianSwap(m_unchangingQuatNumChannels);

  //-----------------------
  // Relocate the data pointers
  relocate();

  //-----------------------
  // Unchanging channels quantisation info
  m_unchangingPosQuantisationInfo.locate();
  for (uint16_t i = 0; i < m_unchangingPosNumChannels; ++i)
    m_unchangingPosData[i].locate();

  m_unchangingQuatQuantisationInfo.locate();
  for (uint16_t i = 0; i < m_unchangingQuatNumChannels; ++i)
    m_unchangingQuatData[i].locate();
}

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoQSA::dislocate()
{
  //-----------------------
  // Unchanging channels quantisation info
  m_unchangingPosQuantisationInfo.dislocate();
  for (uint16_t i = 0; i < m_unchangingPosNumChannels; ++i)
    m_unchangingPosData[i].dislocate();

  m_unchangingQuatQuantisationInfo.dislocate();
  for (uint16_t i = 0; i < m_unchangingQuatNumChannels; ++i)
    m_unchangingQuatData[i].dislocate();

  //-----------------------
  // Header info
  NMP::endianSwapArray((float*)&m_worldspaceRootQuat, 4);
  NMP::endianSwap(m_worldspaceRootID);
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_unchangingDefaultPosNumChannels);
  NMP::endianSwap(m_unchangingDefaultQuatNumChannels);
  NMP::endianSwap(m_unchangingPosNumChannels);
  NMP::endianSwap(m_unchangingQuatNumChannels);

  // For binary invariance. These will be fixed in locate()
  m_unchangingPosData = NULL;
  m_unchangingQuatData = NULL;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoQSA::relocate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Channel set quantisation information must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  uint8_t* data = (uint8_t*) this;

  //-----------------------
  // Header info
  data += sizeof(ChannelSetInfoQSA);
  data = (uint8_t*) NMP::Memory::align(data, NMP_NATURAL_TYPE_ALIGNMENT);

  //-----------------------
  // Unchanging channels quantisation info
  if (m_unchangingPosNumChannels > 0)
  {
    m_unchangingPosData = (UnchangingKeyQSA*)data;
    data += sizeof(UnchangingKeyQSA) * m_unchangingPosNumChannels;
    data = (uint8_t*) NMP::Memory::align(data, NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    m_unchangingPosData = NULL;
  }

  if (m_unchangingQuatNumChannels > 0)
  {
    m_unchangingQuatData = (UnchangingKeyQSA*)data;
    data += sizeof(UnchangingKeyQSA) * m_unchangingQuatNumChannels;
    data = (uint8_t*) NMP::Memory::align(data, NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    m_unchangingQuatData = NULL;
  }
}

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoQSA::unchangingPosDecompress(
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
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  if (animChannelIndices[0] >= numLODEntries)
    return;

  NMP::Vector3* pOut = outputBuffer->getPosQuatChannelPos(0);
  NMP_ASSERT(pOut);

  //-----------------------
  // Compute the quantisation scale and offset information
  NMP::vpu::vector4_t qMin = NMP::vpu::loadu4f(m_unchangingPosQuantisationInfo.m_qMin);
  NMP::vpu::vector4_t qMax = NMP::vpu::loadu4f(m_unchangingPosQuantisationInfo.m_qMax);
  NMP::vpu::vector4_t qScale = NMP::vpu::uniformQuantisation16(qMin, qMax);

  //-----------------------
  // Iterate over the compression channels in blocks of four
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; indx += 4)
  {
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
    NMP::vpu::Vector4MP pos = vec4 * qScale + qMin;

    // Store the pos data in the correct rig locations
    NMP::vpu::store4f((float*)&pOut[rigChannelIndex0], pos.x);
    NMP::vpu::store4f((float*)&pOut[rigChannelIndex1], pos.y);
    NMP::vpu::store4f((float*)&pOut[rigChannelIndex2], pos.z);
    NMP::vpu::store4f((float*)&pOut[rigChannelIndex3], pos.w);
  }
}
#else
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoQSA::unchangingPosDecompress(
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
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  if (animChannelIndices[0] >= numLODEntries)
    return;

  NMP::Vector3* pOut = outputBuffer->getPosQuatChannelPos(0);
  NMP_ASSERT(pOut);

  // Compute the quantisation scale and offset information
  NMP::Vector3 qScale, qOffset;
  MR::uniformQuantisation16(
    m_unchangingPosQuantisationInfo.m_qMin,
    m_unchangingPosQuantisationInfo.m_qMax,
    qScale,
    qOffset);

  // Iterate over the compression channels
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; ++indx)
  {
    uint32_t rigChannelIndex = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);

    // Get a reference to the output buffer data
    NMP::Vector3& pos = pOut[rigChannelIndex];

    // Dequantise the data
    MR::dequantise(qScale, qOffset, m_unchangingPosData[indx].m_data, pos);
  }
}
#endif // NMP_PLATFORM_SIMD

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoQSA::unchangingQuatDecompress(
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
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  if (animChannelIndices[0] >= numLODEntries)
    return;

  NMP::Quat* qOut = outputBuffer->getPosQuatChannelQuat(0);
  NMP_ASSERT(qOut);

  //-----------------------
  // Compute the quantisation scale and offset information
  NMP::vpu::vector4_t qMin = NMP::vpu::loadu4f(m_unchangingQuatQuantisationInfo.m_qMin);
  NMP::vpu::vector4_t qMax = NMP::vpu::loadu4f(m_unchangingQuatQuantisationInfo.m_qMax);
  NMP::vpu::vector4_t qScale = NMP::vpu::uniformQuantisation16(qMin, qMax);

  NMP::vpu::Vector3MP qScaleVec(qScale);
  NMP::vpu::Vector3MP qMinVec(qMin);

  //-----------------------
  // Iterate over the compression channels in blocks of four
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; indx += 4)
  {
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
    vec = vec * qScaleVec + qMinVec;

    // Convert the tan quarter angle rotation vectors back to quaternions
    NMP::vpu::QuatMP quat = vec.convertTQARotVecToQuat();

    // Store the quat data in the correct rig locations
    quat.unpack(vecX, vecY, vecZ, vecW);

    // Store the pos data in the correct rig locations
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex0], vecX);
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex1], vecY);
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex2], vecZ);
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex3], vecW);
  }
}
#else
//----------------------------------------------------------------------------------------------------------------------
void ChannelSetInfoQSA::unchangingQuatDecompress(
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
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  if (animChannelIndices[0] >= numLODEntries)
    return;

  NMP::Quat* qOut = outputBuffer->getPosQuatChannelQuat(0);
  NMP_ASSERT(qOut);

  // Compute the quantisation scale and offset information
  NMP::Vector3 qScale, qOffset, v;
  MR::uniformQuantisation16(
    m_unchangingQuatQuantisationInfo.m_qMin,
    m_unchangingQuatQuantisationInfo.m_qMax,
    qScale,
    qOffset);

  // Iterate over the compression channels
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; ++indx)
  {
    uint32_t rigChannelIndex = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);

    // Get a reference to the output buffer data
    NMP::Quat& quat = qOut[rigChannelIndex];

    // Dequantise the data
    MR::dequantise(qScale, qOffset, m_unchangingQuatData[indx].m_data, v);

    // Convert the tan quarter angle rotation vector back to a quaternion
    MR::fromRotationVectorTQA(v, quat);
  }
}
#endif // NMP_PLATFORM_SIMD

//----------------------------------------------------------------------------------------------------------------------
// AnimSectionInfoQSA functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AnimSectionInfoQSA::locate()
{
  NMP::endianSwap(m_startFrame);
  NMP::endianSwap(m_sectionSize);
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionInfoQSA::dislocate()
{
  NMP::endianSwap(m_startFrame);
  NMP::endianSwap(m_sectionSize);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// InternalDataQSA Functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format InternalDataQSA::getMemoryRequirements(uint32_t numRigBones)
{
  // Header
  NMP::Memory::Format result(sizeof(InternalDataQSA), NMP_NATURAL_TYPE_ALIGNMENT);
 
  // Channel quat spaces
  NMP::Memory::Format memReqsSpaces(sizeof(uint8_t) * numRigBones, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsSpaces;
  
  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
InternalDataQSA* InternalDataQSA::init(
  NMP::Memory::Resource resource,
  uint32_t numRigBones)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(InternalDataQSA), NMP_NATURAL_TYPE_ALIGNMENT);
  InternalDataQSA* result = (InternalDataQSA*) resource.alignAndIncrement(memReqsHdr);
  
  // Channel quat spaces
  NMP::Memory::Format memReqsSpaces(sizeof(uint8_t) * numRigBones, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_chanQuatSpaces = (uint8_t*) resource.alignAndIncrement(memReqsSpaces);
  
  // Set all channels as being local
  for (uint32_t i = 0; i < numRigBones; ++i)
    result->m_chanQuatSpaces[i] = QSA_CHAN_LOCAL_FLAG;
  
  // Multiple of the alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AnimSectionQSA
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AnimSectionQSA::locate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Sections must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Section header info
  NMP::endianSwap(m_sectionSize);
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_sectionAnimStartFrame);
  NMP::endianSwap(m_sectionNumAnimFrames);

  NMP::endianSwap(m_sampledPosNumChannels);
  NMP::endianSwap(m_sampledQuatNumChannels);
  NMP::endianSwap(m_splineNumKnots);
  NMP::endianSwap(m_splinePosNumChannels);
  NMP::endianSwap(m_splineQuatNumChannels);

  // Bitstream data strides
  NMP::endianSwap(m_sampledPosByteStride);
  NMP::endianSwap(m_sampledQuatByteStride);
  NMP::endianSwap(m_splinePosByteStride);
  NMP::endianSwap(m_splineQuatWNegsByteStride);
  NMP::endianSwap(m_splineQuatKeysByteStride);
  NMP::endianSwap(m_splineQuatTangentsByteStride);

  // Quantisation sets info
  NMP::endianSwap(m_sampledPosNumQuantisationSets);
  NMP::endianSwap(m_sampledQuatNumQuantisationSets);
  NMP::endianSwap(m_splinePosNumQuantisationSets);
  NMP::endianSwap(m_splineQuatKeysNumQuantisationSets);
  NMP::endianSwap(m_splineQuatTangentsNumQuantisationSets);

  // Quantisation means
  m_posMeansQuantisationInfo.locate();
  m_splineQuatTangentMeansQuantisationInfo.locate();

  //-----------------------
  // Relocate the data pointers
  relocate();

  //-----------------------
  // Sampled channels
  for (uint16_t i = 0; i < m_sampledPosNumQuantisationSets; ++i)
    m_sampledPosQuantisationInfo[i].locate();
  // The quantization data is padded to a multiple of 4 entries.
  // Check QuantisationDataQSA::getMemoryRequirementsSize.
  uint32_t sampledPosNumChannels4 = NMP::nmAlignedValue4(m_sampledPosNumChannels);
  for (uint16_t i = 0; i < sampledPosNumChannels4; ++i)
    m_sampledPosQuantisationData[i].locate();

  for (uint16_t i = 0; i < m_sampledQuatNumQuantisationSets; ++i)
    m_sampledQuatQuantisationInfo[i].locate();
  // The quantization data is padded to a multiple of 4 entries.
  // Check QuantisationDataQSA::getMemoryRequirementsSize.
  uint32_t sampledQuatNumChannels4 = NMP::nmAlignedValue4(m_sampledQuatNumChannels);
  for (uint16_t i = 0; i < sampledQuatNumChannels4; ++i)
    m_sampledQuatQuantisationData[i].locate();

  //-----------------------
  // Spline channels
  NMP::endianSwapArray(m_splineKnots, m_splineNumKnots);

  for (uint16_t i = 0; i < m_splinePosNumQuantisationSets; ++i)
    m_splinePosQuantisationInfo[i].locate();
  // The quantization data is padded to a multiple of 4 entries.
  // Check QuantisationDataQSA::getMemoryRequirementsSize.
  uint32_t splinePosNumChannels4 = NMP::nmAlignedValue4(m_splinePosNumChannels);
  for (uint16_t i = 0; i < splinePosNumChannels4; ++i)
    m_splinePosQuantisationData[i].locate();

  for (uint16_t i = 0; i < m_splineQuatKeysNumQuantisationSets; ++i)
    m_splineQuatKeysQuantisationInfo[i].locate();
  for (uint16_t i = 0; i < m_splineQuatTangentsNumQuantisationSets; ++i)
    m_splineQuatTangentsQuantisationInfo[i].locate();
  // The quantization data is padded to a multiple of 4 entries.
  // Check QuantisationDataQSA::getMemoryRequirementsSize.
  uint32_t splineQuatNumChannels4 = NMP::nmAlignedValue4(m_splineQuatNumChannels);
  for (uint16_t i = 0; i < splineQuatNumChannels4; ++i)
  {
    m_splineQuatKeysQuantisationData[i].locate();
    m_splineQuatTangentsQuantisationData[i].locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionQSA::dislocate()
{
  //-----------------------
  // Spline channels
  NMP::endianSwapArray(m_splineKnots, m_splineNumKnots);

  for (uint16_t i = 0; i < m_splinePosNumQuantisationSets; ++i)
    m_splinePosQuantisationInfo[i].dislocate();
  // The quantization data is padded to a multiple of 4 entries.
  // Check QuantisationDataQSA::getMemoryRequirementsSize.
  uint32_t splinePosNumChannels4 = NMP::nmAlignedValue4(m_splinePosNumChannels);
  for (uint16_t i = 0; i < splinePosNumChannels4; ++i)
    m_splinePosQuantisationData[i].dislocate();

  for (uint16_t i = 0; i < m_splineQuatKeysNumQuantisationSets; ++i)
    m_splineQuatKeysQuantisationInfo[i].dislocate();
  for (uint16_t i = 0; i < m_splineQuatTangentsNumQuantisationSets; ++i)
    m_splineQuatTangentsQuantisationInfo[i].dislocate();
  // The quantization data is padded to a multiple of 4 entries.
  // Check QuantisationDataQSA::getMemoryRequirementsSize.
  uint32_t splineQuatNumChannels4 = NMP::nmAlignedValue4(m_splineQuatNumChannels);
  for (uint16_t i = 0; i < splineQuatNumChannels4; ++i)
  {
    m_splineQuatKeysQuantisationData[i].dislocate();
    m_splineQuatTangentsQuantisationData[i].dislocate();
  }

  //-----------------------
  // Sampled channels
  for (uint16_t i = 0; i < m_sampledPosNumQuantisationSets; ++i)
    m_sampledPosQuantisationInfo[i].dislocate();
  // The quantization data is padded to a multiple of 4 entries.
  // Check QuantisationDataQSA::getMemoryRequirementsSize.
  uint32_t sampledPosNumChannels4 = NMP::nmAlignedValue4(m_sampledPosNumChannels);
  for (uint16_t i = 0; i < sampledPosNumChannels4; ++i)
    m_sampledPosQuantisationData[i].dislocate();

  for (uint16_t i = 0; i < m_sampledQuatNumQuantisationSets; ++i)
    m_sampledQuatQuantisationInfo[i].dislocate();
  // The quantization data is padded to a multiple of 4 entries.
  // Check QuantisationDataQSA::getMemoryRequirementsSize.
  uint32_t sampledQuatNumChannels4 = NMP::nmAlignedValue4(m_sampledQuatNumChannels);
  for (uint16_t i = 0; i < sampledQuatNumChannels4; ++i)
    m_sampledQuatQuantisationData[i].dislocate();

  //-----------------------
  // Section header info
  NMP::endianSwap(m_sectionSize);
  NMP::endianSwap(m_numChannelSets);
  NMP::endianSwap(m_sectionAnimStartFrame);
  NMP::endianSwap(m_sectionNumAnimFrames);

  NMP::endianSwap(m_sampledPosNumChannels);
  NMP::endianSwap(m_sampledQuatNumChannels);
  NMP::endianSwap(m_splineNumKnots);
  NMP::endianSwap(m_splinePosNumChannels);
  NMP::endianSwap(m_splineQuatNumChannels);

  // Bitstream data strides
  NMP::endianSwap(m_sampledPosByteStride);
  NMP::endianSwap(m_sampledQuatByteStride);
  NMP::endianSwap(m_splinePosByteStride);
  NMP::endianSwap(m_splineQuatWNegsByteStride);
  NMP::endianSwap(m_splineQuatKeysByteStride);
  NMP::endianSwap(m_splineQuatTangentsByteStride);

  // Quantisation sets info
  NMP::endianSwap(m_sampledPosNumQuantisationSets);
  NMP::endianSwap(m_sampledQuatNumQuantisationSets);
  NMP::endianSwap(m_splinePosNumQuantisationSets);
  NMP::endianSwap(m_splineQuatKeysNumQuantisationSets);
  NMP::endianSwap(m_splineQuatTangentsNumQuantisationSets);

  // Quantisation means
  m_posMeansQuantisationInfo.dislocate();
  m_splineQuatTangentMeansQuantisationInfo.dislocate();

  // For binary invariance. These will be fixed in locate()
  {
    m_sampledPosQuantisationInfo = NULL;
    m_sampledPosQuantisationData = NULL;
    m_sampledPosData = NULL;

    m_sampledQuatQuantisationInfo = NULL;
    m_sampledQuatQuantisationData = NULL;
    m_sampledQuatData = NULL;

    m_splineKnots = NULL;

    m_splinePosQuantisationInfo = NULL;
    m_splinePosQuantisationData = NULL;
    m_splinePosData = NULL;

    m_splineQuatKeysQuantisationInfo = NULL;
    m_splineQuatKeysQuantisationData = NULL;
    m_splineQuatWNegsData = NULL;
    m_splineQuatKeysData = NULL;
    m_splineQuatTangentsQuantisationInfo = NULL;
    m_splineQuatTangentsQuantisationData = NULL;
    m_splineQuatTangentsData = NULL;
  }
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AnimSectionQSA::relocate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Sections must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  //-----------------------
  // Section header info
  NMP::BitStreamDecoder decoder;
  decoder.init(m_sectionSize, (uint8_t*)this);
  decoder.incrementBytes(sizeof(AnimSectionQSA));
  decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

  //-----------------------
  // Sampled Pos channels
  if (m_sampledPosNumChannels > 0)
  {
    NMP_ASSERT(m_sampledPosNumQuantisationSets > 0);
    m_sampledPosQuantisationInfo = (QuantisationInfoQSA*)decoder.getCurrentBytePos();
    decoder.incrementBytes(sizeof(QuantisationInfoQSA) * m_sampledPosNumQuantisationSets);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    m_sampledPosQuantisationData = (QuantisationDataQSA*)decoder.getCurrentBytePos();
    decoder.incrementBytes(QuantisationDataQSA::getMemoryRequirementsSize(m_sampledPosNumChannels));
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    m_sampledPosData = (uint8_t*)decoder.getCurrentBytePos();
    decoder.incrementBytes(m_sampledPosByteStride * m_sectionNumAnimFrames);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    m_sampledPosQuantisationInfo = NULL;
    m_sampledPosQuantisationData = NULL;
    m_sampledPosData = NULL;
  }

  //-----------------------
  // Sampled Quat channels
  if (m_sampledQuatNumChannels > 0)
  {
    NMP_ASSERT(m_sampledQuatNumQuantisationSets > 0);
    m_sampledQuatQuantisationInfo = (QuantisationInfoQSA*)decoder.getCurrentBytePos();
    decoder.incrementBytes(sizeof(QuantisationInfoQSA) * m_sampledQuatNumQuantisationSets);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    m_sampledQuatQuantisationData = (QuantisationDataQSA*)decoder.getCurrentBytePos();
    decoder.incrementBytes(QuantisationDataQSA::getMemoryRequirementsSize(m_sampledQuatNumChannels));
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    m_sampledQuatData = (uint8_t*)decoder.getCurrentBytePos();
    decoder.incrementBytes(m_sampledQuatByteStride * m_sectionNumAnimFrames);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    m_sampledQuatQuantisationInfo = NULL;
    m_sampledQuatQuantisationData = NULL;
    m_sampledQuatData = NULL;
  }

  //-----------------------
  // Spline Knots
  if (m_splineNumKnots > 0)
  {
    NMP_ASSERT(m_splineNumKnots >= 2);
    m_splineKnots = (uint16_t*)decoder.getCurrentBytePos();
    decoder.incrementBytes(sizeof(uint16_t) * m_splineNumKnots);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    m_splineKnots = NULL;
  }

  //-----------------------
  // Spline Pos channels
  if (m_splinePosNumChannels > 0)
  {
    NMP_ASSERT(m_splinePosNumQuantisationSets > 0);
    m_splinePosQuantisationInfo = (QuantisationInfoQSA*)decoder.getCurrentBytePos();
    decoder.incrementBytes(sizeof(QuantisationInfoQSA) * m_splinePosNumQuantisationSets);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    m_splinePosQuantisationData = (QuantisationDataQSA*)decoder.getCurrentBytePos();
    decoder.incrementBytes(QuantisationDataQSA::getMemoryRequirementsSize(m_splinePosNumChannels));
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    m_splinePosData = (uint8_t*)decoder.getCurrentBytePos();
    decoder.incrementBytes(m_splinePosByteStride * (3 * m_splineNumKnots - 2)); // All Keys and Tangents sections
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    m_splinePosQuantisationInfo = NULL;
    m_splinePosQuantisationData = NULL;
    m_splinePosData = NULL;
  }

  //-----------------------
  // Spline Quat channels
  if (m_splineQuatNumChannels > 0)
  {
    NMP_ASSERT(m_splineQuatKeysNumQuantisationSets > 0);
    m_splineQuatKeysQuantisationInfo = (QuantisationInfoQSA*)decoder.getCurrentBytePos();
    decoder.incrementBytes(sizeof(QuantisationInfoQSA) * m_splineQuatKeysNumQuantisationSets);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    m_splineQuatKeysQuantisationData = (QuantisationDataQSA*)decoder.getCurrentBytePos();
    decoder.incrementBytes(QuantisationDataQSA::getMemoryRequirementsSize(m_splineQuatNumChannels));
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    m_splineQuatWNegsData = (uint8_t*)decoder.getCurrentBytePos();
    decoder.incrementBytes(m_splineQuatWNegsByteStride * m_splineNumKnots);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    m_splineQuatKeysData = (uint8_t*)decoder.getCurrentBytePos();
    decoder.incrementBytes(m_splineQuatKeysByteStride * m_splineNumKnots);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    NMP_ASSERT(m_splineQuatTangentsNumQuantisationSets > 0);
    m_splineQuatTangentsQuantisationInfo = (QuantisationInfoQSA*)decoder.getCurrentBytePos();
    decoder.incrementBytes(sizeof(QuantisationInfoQSA) * m_splineQuatTangentsNumQuantisationSets);
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    m_splineQuatTangentsQuantisationData = (QuantisationDataQSA*)decoder.getCurrentBytePos();
    decoder.incrementBytes(QuantisationDataQSA::getMemoryRequirementsSize(m_splineQuatNumChannels));
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);

    m_splineQuatTangentsData = (uint8_t*)decoder.getCurrentBytePos();
    decoder.incrementBytes(m_splineQuatTangentsByteStride * 2 * (m_splineNumKnots - 1));
    decoder.align(NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else
  {
    m_splineQuatKeysQuantisationInfo = NULL;
    m_splineQuatKeysQuantisationData = NULL;
    m_splineQuatWNegsData = NULL;
    m_splineQuatKeysData = NULL;
    m_splineQuatTangentsQuantisationInfo = NULL;
    m_splineQuatTangentsQuantisationData = NULL;
    m_splineQuatTangentsData = NULL;
  }
}

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void AnimSectionQSA::sampledPosDecompress(
  const AnimToRigTableMap* animToRigTableMap,
  const CompToAnimChannelMap* compToAnimMap,
  InternalDataQSA* internalData,
  NMP::DataBuffer* outputTransformBuffer) const
{
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimMap);
  NMP_ASSERT(internalData);
  NMP_ASSERT(outputTransformBuffer);

  const uint16_t* animChannelIndices = compToAnimMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  if (animChannelIndices[0] >= numLODEntries)
    return;

  NMP::Vector3* pOut = outputTransformBuffer->getPosQuatChannelPos(0);
  NMP_ASSERT(pOut);

  NMP::vpu::vector4_t sampledUParam = NMP::vpu::set4f(internalData->m_sampledUParam);

  const uint8_t* sampledKeys[2];
  sampledKeys[0] = m_sampledPosData + m_sampledPosByteStride * internalData->m_sampledIndex;
  sampledKeys[1] = sampledKeys[0] + m_sampledPosByteStride;
  uint32_t curBitStreamOffset = 0;

  //-----------------------
  // Compute the quantisation scale and offset information for the channel means
  NMP::vpu::vector4_t qMinMeanv = NMP::vpu::loadu4f(m_posMeansQuantisationInfo.m_qMin);
  NMP::vpu::vector4_t qMaxMeanv = NMP::vpu::loadu4f(m_posMeansQuantisationInfo.m_qMax);
  NMP::vpu::vector4_t qScaleMeanv = NMP::vpu::uniformQuantisation8(qMinMeanv, qMaxMeanv);

  NMP::vpu::Vector3MP qScaleMean(qScaleMeanv);
  NMP::vpu::Vector3MP qMinMean(qMinMeanv);

  //-----------------------
  // Iterate over the compression channels in blocks of four  
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; indx += 4)
  {
    NMP::vpu::prefetchCacheLine(&animChannelIndices[indx], NMP_VPU_CACHESIZE);
    NMP::vpu::prefetchCacheLine(&animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]), NMP_VPU_CACHESIZE);
    uint32_t rigChannelIndex0 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);
    uint32_t rigChannelIndex1 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 1]);
    uint32_t rigChannelIndex2 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 2]);
    uint32_t rigChannelIndex3 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 3]);

    NMP::vpu::vector4_t vecX, vecY, vecZ, vecW;

    // Get a reference to the quantisation data: mean/prec/qset
    NMP::vpu::prefetchCacheLine(&m_sampledPosQuantisationData[indx], NMP_VPU_CACHESIZE);
    const QuantisationDataQSA& qData0 = m_sampledPosQuantisationData[indx];
    const QuantisationDataQSA& qData1 = m_sampledPosQuantisationData[indx + 1];
    const QuantisationDataQSA& qData2 = m_sampledPosQuantisationData[indx + 2];
    const QuantisationDataQSA& qData3 = m_sampledPosQuantisationData[indx + 3];

    // Compose the fragmented quantised channel mean data into a single continuous block
    vecX = NMP::vpu::unpacku4i8(qData0.getMean());
    vecY = NMP::vpu::unpacku4i8(qData1.getMean());
    vecZ = NMP::vpu::unpacku4i8(qData2.getMean());
    vecW = NMP::vpu::unpacku4i8(qData3.getMean());

    NMP::vpu::Vector3MP vecMean;
    vecMean.pack(vecX, vecY, vecZ, vecW);
    vecMean.convert2f();

    // Dequantise the channel means
    NMP::vpu::Vector3MP pBar = vecMean * qScaleMean + qMinMean;

    // Compose the fragmented precision data into a single continuous block
    vecX = NMP::vpu::unpacku4i8(qData0.getPrecision());
    vecY = NMP::vpu::unpacku4i8(qData1.getPrecision());
    vecZ = NMP::vpu::unpacku4i8(qData2.getPrecision());
    vecW = NMP::vpu::unpacku4i8(qData3.getPrecision());

    NMP::vpu::Vector3MP vecPrec;
    vecPrec.pack(vecX, vecY, vecZ, vecW);

    // Compose the fragmented qMin, qMax data into a single continuous block
    const QuantisationInfoQSA& qSetX0 = m_sampledPosQuantisationInfo[qData0.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetX1 = m_sampledPosQuantisationInfo[qData1.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetX2 = m_sampledPosQuantisationInfo[qData2.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetX3 = m_sampledPosQuantisationInfo[qData3.getQuantisationSetX()];
    NMP::vpu::vector4_t qMinX = NMP::vpu::set4f(qSetX0.m_qMin[0], qSetX1.m_qMin[0], qSetX2.m_qMin[0], qSetX3.m_qMin[0]);
    NMP::vpu::vector4_t qMaxX = NMP::vpu::set4f(qSetX0.m_qMax[0], qSetX1.m_qMax[0], qSetX2.m_qMax[0], qSetX3.m_qMax[0]);

    const QuantisationInfoQSA& qSetY0 = m_sampledPosQuantisationInfo[qData0.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetY1 = m_sampledPosQuantisationInfo[qData1.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetY2 = m_sampledPosQuantisationInfo[qData2.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetY3 = m_sampledPosQuantisationInfo[qData3.getQuantisationSetY()];
    NMP::vpu::vector4_t qMinY = NMP::vpu::set4f(qSetY0.m_qMin[1], qSetY1.m_qMin[1], qSetY2.m_qMin[1], qSetY3.m_qMin[1]);
    NMP::vpu::vector4_t qMaxY = NMP::vpu::set4f(qSetY0.m_qMax[1], qSetY1.m_qMax[1], qSetY2.m_qMax[1], qSetY3.m_qMax[1]);

    const QuantisationInfoQSA& qSetZ0 = m_sampledPosQuantisationInfo[qData0.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetZ1 = m_sampledPosQuantisationInfo[qData1.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetZ2 = m_sampledPosQuantisationInfo[qData2.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetZ3 = m_sampledPosQuantisationInfo[qData3.getQuantisationSetZ()];
    NMP::vpu::vector4_t qMinZ = NMP::vpu::set4f(qSetZ0.m_qMin[2], qSetZ1.m_qMin[2], qSetZ2.m_qMin[2], qSetZ3.m_qMin[2]);
    NMP::vpu::vector4_t qMaxZ = NMP::vpu::set4f(qSetZ0.m_qMax[2], qSetZ1.m_qMax[2], qSetZ2.m_qMax[2], qSetZ3.m_qMax[2]);

    NMP::vpu::Vector3MP qMin(qMinX, qMinY, qMinZ);
    NMP::vpu::Vector3MP qMax(qMaxX, qMaxY, qMaxZ);

    // Compute the uniform quantisation scale and offset
    NMP::vpu::Vector3MP qScale;
    qScale.uniformQuantisation(qMin, qMax, vecPrec);

    // Compose the fragmented data into a single continuous block
    uint32_t prec;
    NMP::vpu::vector4_t dataX[2], dataY[2], dataZ[2];
    NMP::vpu::vector4_t precX, precY, precZ;
    NMP::vpu::vector4_t bitStreamOffsetX, bitStreamOffsetY, bitStreamOffsetZ;

    // This tight loop is faster than expanding out the code in sequence
    for (uint32_t i = 0; i < 4; ++i)
    {
      // X
      for (uint32_t entryIndex = 0; entryIndex < 2; ++entryIndex)
      {
        const uint8_t* bitsPos = &(sampledKeys[entryIndex][curBitStreamOffset >> 3]);
        NMP::vpu::prefetchCacheLine(bitsPos, NMP_VPU_CACHESIZE);
        ((uint32_t*)&dataX[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_sampledPosQuantisationData[indx + i].getPrecisionX();
      ((uint32_t*)&precX)[i] = prec;
      ((uint32_t*)&bitStreamOffsetX)[i] = curBitStreamOffset;
      curBitStreamOffset += prec;

      // Y
      for (uint32_t entryIndex = 0; entryIndex < 2; ++entryIndex)
      {
        const uint8_t* bitsPos = &(sampledKeys[entryIndex][curBitStreamOffset >> 3]);
        ((uint32_t*)&dataY[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_sampledPosQuantisationData[indx + i].getPrecisionY();
      ((uint32_t*)&precY)[i] = prec;
      ((uint32_t*)&bitStreamOffsetY)[i] = curBitStreamOffset;
      curBitStreamOffset += prec;

      // Z
      for (uint32_t entryIndex = 0; entryIndex < 2; ++entryIndex)
      {
        const uint8_t* bitsPos = &(sampledKeys[entryIndex][curBitStreamOffset >> 3]);
        ((uint32_t*)&dataZ[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_sampledPosQuantisationData[indx + i].getPrecisionZ();
      ((uint32_t*)&precZ)[i] = prec;
      ((uint32_t*)&bitStreamOffsetZ)[i] = curBitStreamOffset;
      curBitStreamOffset += prec;
    }

    // Shift mask and convert to float    
    NMP::vpu::vector4_t andMask = NMP::vpu::set4i(0x07);
    NMP::vpu::vector4_t shiftX = NMP::vpu::and4(bitStreamOffsetX, andMask);
    NMP::vpu::vector4_t shiftY = NMP::vpu::and4(bitStreamOffsetY, andMask);
    NMP::vpu::vector4_t shiftZ = NMP::vpu::and4(bitStreamOffsetZ, andMask);

    NMP::vpu::vector4_t shiftMask = NMP::vpu::set4i(0xffffffff);
    NMP::vpu::vector4_t maskX = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precX));
    NMP::vpu::vector4_t maskY = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precY));
    NMP::vpu::vector4_t maskZ = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precZ));

    NMP::vpu::Vector3MP vecA(
      NMP::vpu::and4(NMP::vpu::shr4(dataX[0], shiftX), maskX),
      NMP::vpu::and4(NMP::vpu::shr4(dataY[0], shiftY), maskY),
      NMP::vpu::and4(NMP::vpu::shr4(dataZ[0], shiftZ), maskZ));
    vecA.convert2f();

    NMP::vpu::Vector3MP vecB(
      NMP::vpu::and4(NMP::vpu::shr4(dataX[1], shiftX), maskX),
      NMP::vpu::and4(NMP::vpu::shr4(dataY[1], shiftY), maskY),
      NMP::vpu::and4(NMP::vpu::shr4(dataZ[1], shiftZ), maskZ));
    vecB.convert2f();

    // Dequantise the keys  data
    vecA = vecA * qScale + qMin;
    vecB = vecB * qScale + qMin;

    // Interpolation
    NMP::vpu::Vector3MP pos;
    pos.lerp(vecA, vecB, sampledUParam);
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
void AnimSectionQSA::sampledPosDecompress(
  const AnimToRigTableMap* animToRigTableMap,
  const CompToAnimChannelMap* compToAnimMap,
  InternalDataQSA* internalData,
  NMP::DataBuffer* outputTransformBuffer) const
{
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimMap);
  NMP_ASSERT(internalData);
  NMP_ASSERT(outputTransformBuffer);

  const uint16_t* animChannelIndices = compToAnimMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  if (animChannelIndices[0] >= numLODEntries)
    return;

  NMP::Vector3* pOut = outputTransformBuffer->getPosQuatChannelPos(0);
  NMP_ASSERT(pOut);

  const uint8_t* sampledKeysA = m_sampledPosData + m_sampledPosByteStride * internalData->m_sampledIndex;
  const uint8_t* sampledKeysB = sampledKeysA + m_sampledPosByteStride;
  uint32_t curBitStreamOffset = 0;

  NMP::Vector3 qScale, qOffset, p;
  NMP::Vector3 qScaleMean, qOffsetMean, pbar;
  uint32_t prec[3];
  uint32_t qSet[3];
  uint32_t qMean[3];
  float qMin[3];
  float qMax[3];

  // Compute the quantisation scale and offset information for the means
  MR::uniformQuantisation8(
    m_posMeansQuantisationInfo.m_qMin,
    m_posMeansQuantisationInfo.m_qMax,
    qScaleMean,
    qOffsetMean);
    
  // Iterate over the compression channels
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; ++indx)
  {
    uint32_t rigChannelIndex = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);

    // Get a reference to the quantisation data: mean/prec/qset
    const QuantisationDataQSA& qData = m_sampledPosQuantisationData[indx];

    // Get the quantised means for this channel
    qMean[0] = qData.getMeanX();
    qMean[1] = qData.getMeanY();
    qMean[2] = qData.getMeanZ();

    // Get the precisions for this channel
    prec[0] = qData.getPrecisionX();
    prec[1] = qData.getPrecisionY();
    prec[2] = qData.getPrecisionZ();

    // Get the quantisation sets for this channel
    qSet[0] = qData.getQuantisationSetX();
    qSet[1] = qData.getQuantisationSetY();
    qSet[2] = qData.getQuantisationSetZ();
    NMP_ASSERT(qSet[0] < m_sampledPosNumQuantisationSets);
    NMP_ASSERT(qSet[1] < m_sampledPosNumQuantisationSets);
    NMP_ASSERT(qSet[2] < m_sampledPosNumQuantisationSets);

    // Set the quantisation range
    const QuantisationInfoQSA& qs0 = m_sampledPosQuantisationInfo[qSet[0]];
    qMin[0] = qs0.m_qMin[0];
    qMax[0] = qs0.m_qMax[0];
    const QuantisationInfoQSA& qs1 = m_sampledPosQuantisationInfo[qSet[1]];
    qMin[1] = qs1.m_qMin[1];
    qMax[1] = qs1.m_qMax[1];
    const QuantisationInfoQSA& qs2 = m_sampledPosQuantisationInfo[qSet[2]];
    qMin[2] = qs2.m_qMin[2];
    qMax[2] = qs2.m_qMax[2];

    // Compute the quantisation scale and offset information for the channel data
    MR::uniformQuantisation(
      qMin,
      qMax,
      prec,
      qScale,
      qOffset);

    // Decode the bitstream data
    const uint8_t* bitsPosA;
    const uint8_t* bitsPosB;
    uint32_t tempA, tempB;
    NMP::Vector3 vecA, vecB;
    uint32_t shift, mask;

    // X
    bitsPosA = &sampledKeysA[curBitStreamOffset >> 3];
    bitsPosB = &sampledKeysB[curBitStreamOffset >> 3];
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    shift = curBitStreamOffset & 0x07;
    mask = ~(0xffffffff << prec[0]);
    vecA.x = (float)((tempA >> shift) & mask);
    vecB.x = (float)((tempB >> shift) & mask);
    curBitStreamOffset += prec[0];
    // Y
    bitsPosA = &sampledKeysA[curBitStreamOffset >> 3];
    bitsPosB = &sampledKeysB[curBitStreamOffset >> 3];    
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    shift = curBitStreamOffset & 0x07;
    mask = ~(0xffffffff << prec[1]);
    vecA.y = (float)((tempA >> shift) & mask);
    vecB.y = (float)((tempB >> shift) & mask);
    curBitStreamOffset += prec[1];
    // Z
    bitsPosA = &sampledKeysA[curBitStreamOffset >> 3];
    bitsPosB = &sampledKeysB[curBitStreamOffset >> 3];    
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    shift = curBitStreamOffset & 0x07;
    mask = ~(0xffffffff << prec[2]);
    vecA.z = (float)((tempA >> shift) & mask);
    vecB.z = (float)((tempB >> shift) & mask);
    curBitStreamOffset += prec[2];

    // Dequantise the keys data
    vecA = vecA * qScale + qOffset;
    vecB = vecB * qScale + qOffset;

    // Dequantise the channel mean
    MR::dequantise(qScaleMean, qOffsetMean, qMean, pbar);

    // Interpolation
    p.lerp(vecA, vecB, internalData->m_sampledUParam);

    // Apply the mean to the interpolated value
    NMP::Vector3& pos = pOut[rigChannelIndex];
    pos = p + pbar;
  }
}
#endif // NMP_PLATFORM_SIMD

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void AnimSectionQSA::sampledQuatDecompress(
  const AnimToRigTableMap* animToRigTableMap,
  const CompToAnimChannelMap* compToAnimMap,
  InternalDataQSA* internalData,
  NMP::DataBuffer* outputTransformBuffer) const
{
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimMap);
  NMP_ASSERT(internalData);
  NMP_ASSERT(outputTransformBuffer);

  // Compression to rig channel map
  const uint16_t* animChannelIndices = compToAnimMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  if (animChannelIndices[0] >= numLODEntries)
    return;

  uint8_t spaceFlag = internalData->m_useSpace;
  NMP::Quat* qOut = outputTransformBuffer->getPosQuatChannelQuat(0);
  NMP_ASSERT(qOut);

  NMP::vpu::vector4_t sampledUParam = NMP::vpu::set4f(internalData->m_sampledUParam);

  const uint8_t* sampledKeys[2];
  sampledKeys[0] = m_sampledQuatData + m_sampledQuatByteStride * internalData->m_sampledIndex;
  sampledKeys[1] = sampledKeys[0] + m_sampledQuatByteStride;
  uint32_t curBitStreamOffset = 0;

  //-----------------------
  // Compute the quantisation scale and offset information for the channel means
  NMP::vpu::vector4_t qMinMean = NMP::vpu::negOne4f();
  NMP::vpu::vector4_t qMaxMean = NMP::vpu::one4f();
  NMP::vpu::vector4_t qScaleMean = NMP::vpu::uniformQuantisation8(qMinMean, qMaxMean);

  //-----------------------
  // Iterate over the compression channels in blocks of four
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; indx += 4)
  {
    NMP::vpu::prefetchCacheLine(&animChannelIndices[indx], NMP_VPU_CACHESIZE);
    NMP::vpu::prefetchCacheLine(&animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]), NMP_VPU_CACHESIZE);
    uint32_t rigChannelIndex0 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);
    uint32_t rigChannelIndex1 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 1]);
    uint32_t rigChannelIndex2 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 2]);
    uint32_t rigChannelIndex3 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 3]);

    // Get a reference to the quantisation data: mean/prec/qset
    NMP::vpu::prefetchCacheLine(&m_sampledQuatQuantisationData[indx], NMP_VPU_CACHESIZE);
    const QuantisationDataQSA& qData0 = m_sampledQuatQuantisationData[indx];
    const QuantisationDataQSA& qData1 = m_sampledQuatQuantisationData[indx + 1];
    const QuantisationDataQSA& qData2 = m_sampledQuatQuantisationData[indx + 2];
    const QuantisationDataQSA& qData3 = m_sampledQuatQuantisationData[indx + 3];

    //-----------------------
    // Compose the fragmented quantised channel mean data into a single continuous block
    NMP::vpu::vector4_t vecX, vecY, vecZ, vecW;
    vecX = NMP::vpu::unpacku4i8(qData0.getMean());
    vecY = NMP::vpu::unpacku4i8(qData1.getMean());
    vecZ = NMP::vpu::unpacku4i8(qData2.getMean());
    vecW = NMP::vpu::unpacku4i8(qData3.getMean());

    NMP::vpu::Vector3MP vecMean;
    vecMean.pack(vecX, vecY, vecZ, vecW);
    vecMean.convert2f();

    // Dequantise the channel means
    vecMean = vecMean * qScaleMean + qMinMean;

    // Convert the tan quarter angle rotation vectors back to quaternions
    NMP::vpu::QuatMP qBar = vecMean.convertTQARotVecToQuat();

    // Compose the fragmented precision data into a single continuous block
    vecX = NMP::vpu::unpacku4i8(qData0.getPrecision());
    vecY = NMP::vpu::unpacku4i8(qData1.getPrecision());
    vecZ = NMP::vpu::unpacku4i8(qData2.getPrecision());
    vecW = NMP::vpu::unpacku4i8(qData3.getPrecision());

    NMP::vpu::Vector3MP vecPrec;
    vecPrec.pack(vecX, vecY, vecZ, vecW);

    // Compose the fragmented qMin, qMax data into a single continuous block
    const QuantisationInfoQSA& qSetX0 = m_sampledQuatQuantisationInfo[qData0.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetX1 = m_sampledQuatQuantisationInfo[qData1.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetX2 = m_sampledQuatQuantisationInfo[qData2.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetX3 = m_sampledQuatQuantisationInfo[qData3.getQuantisationSetX()];
    NMP::vpu::vector4_t qMinX = NMP::vpu::set4f(qSetX0.m_qMin[0], qSetX1.m_qMin[0], qSetX2.m_qMin[0], qSetX3.m_qMin[0]);
    NMP::vpu::vector4_t qMaxX = NMP::vpu::set4f(qSetX0.m_qMax[0], qSetX1.m_qMax[0], qSetX2.m_qMax[0], qSetX3.m_qMax[0]);

    const QuantisationInfoQSA& qSetY0 = m_sampledQuatQuantisationInfo[qData0.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetY1 = m_sampledQuatQuantisationInfo[qData1.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetY2 = m_sampledQuatQuantisationInfo[qData2.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetY3 = m_sampledQuatQuantisationInfo[qData3.getQuantisationSetY()];
    NMP::vpu::vector4_t qMinY = NMP::vpu::set4f(qSetY0.m_qMin[1], qSetY1.m_qMin[1], qSetY2.m_qMin[1], qSetY3.m_qMin[1]);
    NMP::vpu::vector4_t qMaxY = NMP::vpu::set4f(qSetY0.m_qMax[1], qSetY1.m_qMax[1], qSetY2.m_qMax[1], qSetY3.m_qMax[1]);

    const QuantisationInfoQSA& qSetZ0 = m_sampledQuatQuantisationInfo[qData0.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetZ1 = m_sampledQuatQuantisationInfo[qData1.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetZ2 = m_sampledQuatQuantisationInfo[qData2.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetZ3 = m_sampledQuatQuantisationInfo[qData3.getQuantisationSetZ()];
    NMP::vpu::vector4_t qMinZ = NMP::vpu::set4f(qSetZ0.m_qMin[2], qSetZ1.m_qMin[2], qSetZ2.m_qMin[2], qSetZ3.m_qMin[2]);
    NMP::vpu::vector4_t qMaxZ = NMP::vpu::set4f(qSetZ0.m_qMax[2], qSetZ1.m_qMax[2], qSetZ2.m_qMax[2], qSetZ3.m_qMax[2]);

    NMP::vpu::Vector3MP qMin(qMinX, qMinY, qMinZ);
    NMP::vpu::Vector3MP qMax(qMaxX, qMaxY, qMaxZ);

    // Compute the uniform quantisation scale and offset
    NMP::vpu::Vector3MP qScale;
    qScale.uniformQuantisation(qMin, qMax, vecPrec);

    // Compose the fragmented data into a single continuous block
    uint32_t prec;
    NMP::vpu::vector4_t dataX[2], dataY[2], dataZ[2];
    NMP::vpu::vector4_t precX, precY, precZ;
    NMP::vpu::vector4_t bitStreamOffsetX, bitStreamOffsetY, bitStreamOffsetZ;

    // This tight loop is faster than expanding out the code in sequence
    for (uint32_t i = 0; i < 4; ++i)
    {
      // X
      for (uint32_t entryIndex = 0; entryIndex < 2; ++entryIndex)
      {
        const uint8_t* bitsPos = &(sampledKeys[entryIndex][curBitStreamOffset >> 3]);
        NMP::vpu::prefetchCacheLine(bitsPos, NMP_VPU_CACHESIZE);
        ((uint32_t*)&dataX[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_sampledQuatQuantisationData[indx + i].getPrecisionX();
      ((uint32_t*)&precX)[i] = prec;
      ((uint32_t*)&bitStreamOffsetX)[i] = curBitStreamOffset;
      curBitStreamOffset += prec;

      // Y
      for (uint32_t entryIndex = 0; entryIndex < 2; ++entryIndex)
      {
        const uint8_t* bitsPos = &(sampledKeys[entryIndex][curBitStreamOffset >> 3]);
        ((uint32_t*)&dataY[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_sampledQuatQuantisationData[indx + i].getPrecisionY();
      ((uint32_t*)&precY)[i] = prec;
      ((uint32_t*)&bitStreamOffsetY)[i] = curBitStreamOffset;
      curBitStreamOffset += prec;

      // Z
      for (uint32_t entryIndex = 0; entryIndex < 2; ++entryIndex)
      {
        const uint8_t* bitsPos = &(sampledKeys[entryIndex][curBitStreamOffset >> 3]);
        ((uint32_t*)&dataZ[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_sampledQuatQuantisationData[indx + i].getPrecisionZ();
      ((uint32_t*)&precZ)[i] = prec;
      ((uint32_t*)&bitStreamOffsetZ)[i] = curBitStreamOffset;
      curBitStreamOffset += prec;
    }

    // Shift mask and convert to float    
    NMP::vpu::vector4_t andMask = NMP::vpu::set4i(0x07);
    NMP::vpu::vector4_t shiftX = NMP::vpu::and4(bitStreamOffsetX, andMask);
    NMP::vpu::vector4_t shiftY = NMP::vpu::and4(bitStreamOffsetY, andMask);
    NMP::vpu::vector4_t shiftZ = NMP::vpu::and4(bitStreamOffsetZ, andMask);

    NMP::vpu::vector4_t shiftMask = NMP::vpu::set4i(0xffffffff);
    NMP::vpu::vector4_t maskX = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precX));
    NMP::vpu::vector4_t maskY = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precY));
    NMP::vpu::vector4_t maskZ = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precZ));

    NMP::vpu::Vector3MP vecA(
      NMP::vpu::and4(NMP::vpu::shr4(dataX[0], shiftX), maskX),
      NMP::vpu::and4(NMP::vpu::shr4(dataY[0], shiftY), maskY),
      NMP::vpu::and4(NMP::vpu::shr4(dataZ[0], shiftZ), maskZ));
    vecA.convert2f();

    NMP::vpu::Vector3MP vecB(
      NMP::vpu::and4(NMP::vpu::shr4(dataX[1], shiftX), maskX),
      NMP::vpu::and4(NMP::vpu::shr4(dataY[1], shiftY), maskY),
      NMP::vpu::and4(NMP::vpu::shr4(dataZ[1], shiftZ), maskZ));
    vecB.convert2f();

    // Dequantise the keys  data
    vecA = vecA * qScale + qMin;
    vecB = vecB * qScale + qMin;

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

    // Set the channel as being worldspace
    internalData->m_chanQuatSpaces[rigChannelIndex0] = spaceFlag;
    internalData->m_chanQuatSpaces[rigChannelIndex1] = spaceFlag;
    internalData->m_chanQuatSpaces[rigChannelIndex2] = spaceFlag;
    internalData->m_chanQuatSpaces[rigChannelIndex3] = spaceFlag;
  }
}

#else
//----------------------------------------------------------------------------------------------------------------------
void AnimSectionQSA::sampledQuatDecompress(
  const AnimToRigTableMap* animToRigTableMap,
  const CompToAnimChannelMap* compToAnimMap,
  InternalDataQSA* internalData,
  NMP::DataBuffer* outputTransformBuffer) const
{
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimMap);
  NMP_ASSERT(internalData);
  NMP_ASSERT(outputTransformBuffer);

  // Compression to rig channel map
  const uint16_t* animChannelIndices = compToAnimMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  if (animChannelIndices[0] >= numLODEntries)
    return;

  uint8_t spaceFlag = internalData->m_useSpace;
  NMP::Quat* qOut = outputTransformBuffer->getPosQuatChannelQuat(0);
  NMP_ASSERT(qOut);

  const uint8_t* sampledKeysA = m_sampledQuatData + m_sampledQuatByteStride * internalData->m_sampledIndex;
  const uint8_t* sampledKeysB = sampledKeysA + m_sampledQuatByteStride;
  uint32_t curBitStreamOffset = 0;

  NMP::Vector3 qScale, qOffset;
  NMP::Vector3 qScaleMean, qOffsetMean, qbar;
  NMP::Quat q, qm;
  uint32_t prec[3];
  uint32_t qSet[3];
  uint32_t qMean[3];
  float qMin[3];
  float qMax[3];
  float fromDotTo;
  const float qMinTQA[3] = { -1.0f, -1.0f, -1.0f };
  const float qMaxTQA[3] = { 1.0f, 1.0f, 1.0f };

  // Compute the quantisation scale and offset information for the means
  MR::uniformQuantisation8(
    qMinTQA,
    qMaxTQA,
    qScaleMean,
    qOffsetMean);
    
  // Iterate over the compression channels
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; ++indx)
  {
    uint32_t rigChannelIndex = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);

    // Get a reference to the quantisation data: mean/prec/qset
    const QuantisationDataQSA& qData = m_sampledQuatQuantisationData[indx];

    // Get the quantised means for this channel
    qMean[0] = qData.getMeanX();
    qMean[1] = qData.getMeanY();
    qMean[2] = qData.getMeanZ();

    // Get the precisions for this channel
    prec[0] = qData.getPrecisionX();
    prec[1] = qData.getPrecisionY();
    prec[2] = qData.getPrecisionZ();

    // Get the quantisation sets for this channel
    qSet[0] = qData.getQuantisationSetX();
    qSet[1] = qData.getQuantisationSetY();
    qSet[2] = qData.getQuantisationSetZ();
    NMP_ASSERT(qSet[0] < m_sampledQuatNumQuantisationSets);
    NMP_ASSERT(qSet[1] < m_sampledQuatNumQuantisationSets);
    NMP_ASSERT(qSet[2] < m_sampledQuatNumQuantisationSets);

    // Set the quantisation range
    const QuantisationInfoQSA& qs0 = m_sampledQuatQuantisationInfo[qSet[0]];
    qMin[0] = qs0.m_qMin[0];
    qMax[0] = qs0.m_qMax[0];
    const QuantisationInfoQSA& qs1 = m_sampledQuatQuantisationInfo[qSet[1]];
    qMin[1] = qs1.m_qMin[1];
    qMax[1] = qs1.m_qMax[1];
    const QuantisationInfoQSA& qs2 = m_sampledQuatQuantisationInfo[qSet[2]];
    qMin[2] = qs2.m_qMin[2];
    qMax[2] = qs2.m_qMax[2];

    // Compute the quantisation scale and offset information
    MR::uniformQuantisation(
      qMin,
      qMax,
      prec,
      qScale,
      qOffset);
      
    // Decode the bitstream data
    const uint8_t* bitsPosA;
    const uint8_t* bitsPosB;
    uint32_t tempA, tempB;
    NMP::Vector3 vecA, vecB;
    uint32_t shift, mask;
    
    // X
    bitsPosA = &sampledKeysA[curBitStreamOffset >> 3];
    bitsPosB = &sampledKeysB[curBitStreamOffset >> 3];
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    shift = curBitStreamOffset & 0x07;
    mask = ~(0xffffffff << prec[0]);
    vecA.x = (float)((tempA >> shift) & mask);
    vecB.x = (float)((tempB >> shift) & mask);
    curBitStreamOffset += prec[0];
    // Y
    bitsPosA = &sampledKeysA[curBitStreamOffset >> 3];
    bitsPosB = &sampledKeysB[curBitStreamOffset >> 3];    
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    shift = curBitStreamOffset & 0x07;
    mask = ~(0xffffffff << prec[1]);
    vecA.y = (float)((tempA >> shift) & mask);
    vecB.y = (float)((tempB >> shift) & mask);
    curBitStreamOffset += prec[1];
    // Z
    bitsPosA = &sampledKeysA[curBitStreamOffset >> 3];
    bitsPosB = &sampledKeysB[curBitStreamOffset >> 3];    
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    shift = curBitStreamOffset & 0x07;
    mask = ~(0xffffffff << prec[2]);
    vecA.z = (float)((tempA >> shift) & mask);
    vecB.z = (float)((tempB >> shift) & mask);
    curBitStreamOffset += prec[2];

    // Dequantise the keys data
    NMP::Quat qa, qb;
    vecA = vecA * qScale + qOffset;
    MR::fromRotationVectorTQA(vecA, qa);
    vecB = vecB * qScale + qOffset;
    MR::fromRotationVectorTQA(vecB, qb);

    // Dequantise the channel mean
    MR::dequantise(qScaleMean, qOffsetMean, qMean, qbar);
    MR::fromRotationVectorTQA(qbar, qm);

    // Interpolation
    fromDotTo = qa.dot(qb);
    if (fromDotTo < 0.0f)
    {
      qb.negate();
      fromDotTo = -fromDotTo;
    }
    q.fastSlerp(qa, qb, internalData->m_sampledUParam, fromDotTo);

    // Apply the channel mean to the interpolated value
    NMP::Quat& quat = qOut[rigChannelIndex];
    quat = qm * q;

    // Set the channel as being worldspace
    internalData->m_chanQuatSpaces[rigChannelIndex] = spaceFlag;
  }
}
#endif // NMP_PLATFORM_SIMD

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void AnimSectionQSA::splinePosDecompress(
  const AnimToRigTableMap* animToRigTableMap,
  const CompToAnimChannelMap* compToAnimMap,
  InternalDataQSA* internalData,
  NMP::DataBuffer* outputTransformBuffer) const
{
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimMap);
  NMP_ASSERT(internalData);
  NMP_ASSERT(outputTransformBuffer);

  const uint16_t* animChannelIndices = compToAnimMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  if (animChannelIndices[0] >= numLODEntries)
    return;
    
  NMP::Vector3* pOut = outputTransformBuffer->getPosQuatChannelPos(0);
  NMP_ASSERT(pOut);

  // Set the spline basis functions
  NMP::vpu::vector4_t splinePosBasisV = NMP::vpu::loadu4f(internalData->m_splinePosBasis);
  NMP::vpu::Vector4MP splinePosBasis(splinePosBasisV);

  uint32_t splineKeysSectionSize = m_splinePosByteStride * m_splineNumKnots;
  uint32_t splineTangentsSectionSize = m_splinePosByteStride * (m_splineNumKnots - 1);
  uint32_t intervalOffset = m_splinePosByteStride * internalData->m_knotIndex;
  const uint8_t* splineKeys[4];
  splineKeys[0] = m_splinePosData + intervalOffset; // KeyA
  splineKeys[1] = splineKeys[0] + splineKeysSectionSize; // TangentA
  splineKeys[2] = splineKeys[1] + splineTangentsSectionSize; // TangentB
  splineKeys[3] = splineKeys[0] + m_splinePosByteStride; // KeyB
  uint32_t curBitStreamOffset = 0;

  // Compute the quantisation scale and offset information for the channel means
  NMP::vpu::vector4_t qMinMeanv = NMP::vpu::loadu4f(m_posMeansQuantisationInfo.m_qMin);
  NMP::vpu::vector4_t qMaxMeanv = NMP::vpu::loadu4f(m_posMeansQuantisationInfo.m_qMax);
  NMP::vpu::vector4_t qScaleMeanv = NMP::vpu::uniformQuantisation8(qMinMeanv, qMaxMeanv);

  NMP::vpu::Vector3MP qScaleMean(qScaleMeanv);
  NMP::vpu::Vector3MP qMinMean(qMinMeanv);

  //-----------------------
  // Iterate over the compression channels in blocks of four
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; indx += 4)
  {
    NMP::vpu::prefetchCacheLine(&animChannelIndices[indx], NMP_VPU_CACHESIZE);
    NMP::vpu::prefetchCacheLine(&animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]), NMP_VPU_CACHESIZE);
    uint32_t rigChannelIndex0 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);
    uint32_t rigChannelIndex1 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 1]);
    uint32_t rigChannelIndex2 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 2]);
    uint32_t rigChannelIndex3 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 3]);

    //-----------------------
    // Get a reference to the quantisation data: mean/prec/qset
    NMP::vpu::prefetchCacheLine(&m_splinePosQuantisationData[indx], NMP_VPU_CACHESIZE);
    const QuantisationDataQSA& qData0 = m_splinePosQuantisationData[indx];
    const QuantisationDataQSA& qData1 = m_splinePosQuantisationData[indx + 1];
    const QuantisationDataQSA& qData2 = m_splinePosQuantisationData[indx + 2];
    const QuantisationDataQSA& qData3 = m_splinePosQuantisationData[indx + 3];

    // Compose the fragmented quantised channel mean data into a single continuous block
    NMP::vpu::vector4_t vecX, vecY, vecZ, vecW;
    vecX = NMP::vpu::unpacku4i8(qData0.getMean());
    vecY = NMP::vpu::unpacku4i8(qData1.getMean());
    vecZ = NMP::vpu::unpacku4i8(qData2.getMean());
    vecW = NMP::vpu::unpacku4i8(qData3.getMean());

    NMP::vpu::Vector3MP vecMean;
    vecMean.pack(vecX, vecY, vecZ, vecW);
    vecMean.convert2f();
    NMP::vpu::Vector3MP pBar = vecMean * qScaleMean + qMinMean;

    //-----------------------
    // Compose the fragmented qMin, qMax data into a single continuous block
    const QuantisationInfoQSA& qSetX0 = m_splinePosQuantisationInfo[qData0.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetX1 = m_splinePosQuantisationInfo[qData1.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetX2 = m_splinePosQuantisationInfo[qData2.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetX3 = m_splinePosQuantisationInfo[qData3.getQuantisationSetX()];
    NMP::vpu::vector4_t qMinX = NMP::vpu::set4f(qSetX0.m_qMin[0], qSetX1.m_qMin[0], qSetX2.m_qMin[0], qSetX3.m_qMin[0]);
    NMP::vpu::vector4_t qMaxX = NMP::vpu::set4f(qSetX0.m_qMax[0], qSetX1.m_qMax[0], qSetX2.m_qMax[0], qSetX3.m_qMax[0]);

    const QuantisationInfoQSA& qSetY0 = m_splinePosQuantisationInfo[qData0.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetY1 = m_splinePosQuantisationInfo[qData1.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetY2 = m_splinePosQuantisationInfo[qData2.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetY3 = m_splinePosQuantisationInfo[qData3.getQuantisationSetY()];
    NMP::vpu::vector4_t qMinY = NMP::vpu::set4f(qSetY0.m_qMin[1], qSetY1.m_qMin[1], qSetY2.m_qMin[1], qSetY3.m_qMin[1]);
    NMP::vpu::vector4_t qMaxY = NMP::vpu::set4f(qSetY0.m_qMax[1], qSetY1.m_qMax[1], qSetY2.m_qMax[1], qSetY3.m_qMax[1]);

    const QuantisationInfoQSA& qSetZ0 = m_splinePosQuantisationInfo[qData0.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetZ1 = m_splinePosQuantisationInfo[qData1.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetZ2 = m_splinePosQuantisationInfo[qData2.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetZ3 = m_splinePosQuantisationInfo[qData3.getQuantisationSetZ()];
    NMP::vpu::vector4_t qMinZ = NMP::vpu::set4f(qSetZ0.m_qMin[2], qSetZ1.m_qMin[2], qSetZ2.m_qMin[2], qSetZ3.m_qMin[2]);
    NMP::vpu::vector4_t qMaxZ = NMP::vpu::set4f(qSetZ0.m_qMax[2], qSetZ1.m_qMax[2], qSetZ2.m_qMax[2], qSetZ3.m_qMax[2]);

    NMP::vpu::Vector3MP qMin(qMinX, qMinY, qMinZ);
    NMP::vpu::Vector3MP qMax(qMaxX, qMaxY, qMaxZ);

    // Compose the fragmented precision data into a single continuous block
    vecX = NMP::vpu::unpacku4i8(qData0.getPrecision());
    vecY = NMP::vpu::unpacku4i8(qData1.getPrecision());
    vecZ = NMP::vpu::unpacku4i8(qData2.getPrecision());
    vecW = NMP::vpu::unpacku4i8(qData3.getPrecision());

    NMP::vpu::Vector3MP vecPrec;
    vecPrec.pack(vecX, vecY, vecZ, vecW);

    // Compute the uniform quantisation scale and offset
    NMP::vpu::Vector3MP qScale;
    qScale.uniformQuantisation(qMin, qMax, vecPrec);

    // Compose the fragmented data into a single continuous block
    uint32_t prec;
    NMP::vpu::vector4_t dataX[4], dataY[4], dataZ[4];
    NMP::vpu::vector4_t precX, precY, precZ;
    NMP::vpu::vector4_t bitStreamOffsetX, bitStreamOffsetY, bitStreamOffsetZ;

    // This tight loop is faster than expanding out the code in sequence
    for (uint32_t i = 0; i < 4; ++i)
    {
      // X
      uint32_t curByteOffset = curBitStreamOffset >> 3;
      for (uint32_t entryIndex = 0; entryIndex < 4; ++entryIndex)
      {
        const uint8_t* bitsPos = splineKeys[entryIndex] + curByteOffset;
        NMP::vpu::prefetchCacheLine(bitsPos, NMP_VPU_CACHESIZE);
        ((uint32_t*)&dataX[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_splinePosQuantisationData[indx + i].getPrecisionX();
      ((uint32_t*)&precX)[i] = prec;
      ((uint32_t*)&bitStreamOffsetX)[i] = curBitStreamOffset;
      curBitStreamOffset += prec;

      // Y
      curByteOffset = curBitStreamOffset >> 3;
      for (uint32_t entryIndex = 0; entryIndex < 4; ++entryIndex)
      {
        const uint8_t* bitsPos = splineKeys[entryIndex] + curByteOffset;
        ((uint32_t*)&dataY[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_splinePosQuantisationData[indx + i].getPrecisionY();
      ((uint32_t*)&precY)[i] = prec;
      ((uint32_t*)&bitStreamOffsetY)[i] = curBitStreamOffset;
      curBitStreamOffset += prec;

      // Z
      curByteOffset = curBitStreamOffset >> 3;
      for (uint32_t entryIndex = 0; entryIndex < 4; ++entryIndex)
      {
        const uint8_t* bitsPos = splineKeys[entryIndex] + curByteOffset;
        ((uint32_t*)&dataZ[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_splinePosQuantisationData[indx + i].getPrecisionZ();
      ((uint32_t*)&precZ)[i] = prec;
      ((uint32_t*)&bitStreamOffsetZ)[i] = curBitStreamOffset;
      curBitStreamOffset += prec;
    }

    // Shift mask and convert to float    
    NMP::vpu::vector4_t andMask = NMP::vpu::set4i(0x07);
    NMP::vpu::vector4_t shiftX = NMP::vpu::and4(bitStreamOffsetX, andMask);
    NMP::vpu::vector4_t shiftY = NMP::vpu::and4(bitStreamOffsetY, andMask);
    NMP::vpu::vector4_t shiftZ = NMP::vpu::and4(bitStreamOffsetZ, andMask);

    NMP::vpu::vector4_t shiftMask = NMP::vpu::set4i(0xffffffff);
    NMP::vpu::vector4_t maskX = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precX));
    NMP::vpu::vector4_t maskY = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precY));
    NMP::vpu::vector4_t maskZ = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precZ));

    NMP::vpu::Vector3MP vecKeyA(
      NMP::vpu::and4(NMP::vpu::shr4(dataX[0], shiftX), maskX),
      NMP::vpu::and4(NMP::vpu::shr4(dataY[0], shiftY), maskY),
      NMP::vpu::and4(NMP::vpu::shr4(dataZ[0], shiftZ), maskZ));
    vecKeyA.convert2f();

    NMP::vpu::Vector3MP vecTA(
      NMP::vpu::and4(NMP::vpu::shr4(dataX[1], shiftX), maskX),
      NMP::vpu::and4(NMP::vpu::shr4(dataY[1], shiftY), maskY),
      NMP::vpu::and4(NMP::vpu::shr4(dataZ[1], shiftZ), maskZ));
    vecTA.convert2f();

    NMP::vpu::Vector3MP vecTB(
      NMP::vpu::and4(NMP::vpu::shr4(dataX[2], shiftX), maskX),
      NMP::vpu::and4(NMP::vpu::shr4(dataY[2], shiftY), maskY),
      NMP::vpu::and4(NMP::vpu::shr4(dataZ[2], shiftZ), maskZ));
    vecTB.convert2f();

    NMP::vpu::Vector3MP vecKeyB(
      NMP::vpu::and4(NMP::vpu::shr4(dataX[3], shiftX), maskX),
      NMP::vpu::and4(NMP::vpu::shr4(dataY[3], shiftY), maskY),
      NMP::vpu::and4(NMP::vpu::shr4(dataZ[3], shiftZ), maskZ));
    vecKeyB.convert2f();

    // Dequantise
    vecKeyA = vecKeyA * qScale + qMin;
    vecTA = vecTA * qScale + qMin;
    vecTB = vecTB * qScale + qMin;
    vecKeyB = vecKeyB * qScale + qMin;

    //-----------------------
    // Cubic Bezier Interpolation
    NMP::vpu::Vector3MP pos;
    pos.interpCubicBezier(vecKeyA, vecTA, vecTB, vecKeyB, splinePosBasis);

    // Apply the mean to the interpolated value
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
void AnimSectionQSA::splinePosDecompress(
  const AnimToRigTableMap* animToRigTableMap,
  const CompToAnimChannelMap* compToAnimMap,
  InternalDataQSA* internalData,
  NMP::DataBuffer* outputTransformBuffer) const
{
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimMap);
  NMP_ASSERT(internalData);
  NMP_ASSERT(outputTransformBuffer);

  const uint16_t* animChannelIndices = compToAnimMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  if (animChannelIndices[0] >= numLODEntries)
    return;

  NMP::Vector3* pOut = outputTransformBuffer->getPosQuatChannelPos(0);
  NMP_ASSERT(pOut);
  const float* splinePosBasis = internalData->m_splinePosBasis;
  NMP_ASSERT(splinePosBasis);

  uint32_t splineKeysSectionSize = m_splinePosByteStride * m_splineNumKnots;
  uint32_t splineTangentsSectionSize = m_splinePosByteStride * (m_splineNumKnots - 1);
  uint32_t intervalOffset = m_splinePosByteStride * internalData->m_knotIndex;
  const uint8_t* splineKeysA = m_splinePosData + intervalOffset;
  const uint8_t* splineKeysB = splineKeysA + m_splinePosByteStride;
  const uint8_t* splineTangentsA = splineKeysA + splineKeysSectionSize;
  const uint8_t* splineTangentsB = splineTangentsA + splineTangentsSectionSize;
  uint32_t curBitStreamOffset = 0;

  NMP::Vector3 qScale, qOffset;
  NMP::Vector3 qScaleMean, qOffsetMean, qBar;
  uint32_t prec[3];
  uint32_t qSet[3];
  float qMin[3];
  float qMax[3];
  uint32_t qMean[3];

  // Compute the quantisation scale and offset information for the pos control point mean
  MR::uniformQuantisation8(
    m_posMeansQuantisationInfo.m_qMin,
    m_posMeansQuantisationInfo.m_qMax,
    qScaleMean,
    qOffsetMean);
    
  // Iterate over the compression channels
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; ++indx)
  {
    uint32_t rigChannelIndex = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);

    // Get a reference to the quantisation pos control point data: mean/prec/qset
    const QuantisationDataQSA& qData = m_splinePosQuantisationData[indx];

    //-----------------------
    // Get the quantised pos control point means for this channel
    qMean[0] = qData.getMeanX();
    qMean[1] = qData.getMeanY();
    qMean[2] = qData.getMeanZ();
    MR::dequantise(qScaleMean, qOffsetMean, qMean, qBar);

    // Get the pos control point precisions for this channel
    prec[0] = qData.getPrecisionX();
    prec[1] = qData.getPrecisionY();
    prec[2] = qData.getPrecisionZ();

    // Get the pos control point quantisation sets for this channel
    qSet[0] = qData.getQuantisationSetX();
    qSet[1] = qData.getQuantisationSetY();
    qSet[2] = qData.getQuantisationSetZ();
    NMP_ASSERT(qSet[0] < m_splinePosNumQuantisationSets);
    NMP_ASSERT(qSet[1] < m_splinePosNumQuantisationSets);
    NMP_ASSERT(qSet[2] < m_splinePosNumQuantisationSets);

    // Set the quantisation range for the pos control points
    const QuantisationInfoQSA& qsk0 = m_splinePosQuantisationInfo[qSet[0]];
    qMin[0] = qsk0.m_qMin[0];
    qMax[0] = qsk0.m_qMax[0];
    const QuantisationInfoQSA& qsk1 = m_splinePosQuantisationInfo[qSet[1]];
    qMin[1] = qsk1.m_qMin[1];
    qMax[1] = qsk1.m_qMax[1];
    const QuantisationInfoQSA& qsk2 = m_splinePosQuantisationInfo[qSet[2]];
    qMin[2] = qsk2.m_qMin[2];
    qMax[2] = qsk2.m_qMax[2];

    // Compute the quantisation scale and offset information for the pos control points
    MR::uniformQuantisation(
      qMin,
      qMax,
      prec,
      qScale,
      qOffset);

    // Decode the bitstream data
    uint32_t curByteOffset;
    const uint8_t* bitsPosA;
    const uint8_t* bitsPosB;
    const uint8_t* bitsPosC;
    const uint8_t* bitsPosD;
    uint32_t tempA, tempB, tempC, tempD;
    uint32_t shift, mask;
    NMP::Vector3 p, vecA, vecB, vecC, vecD;

    // X
    curByteOffset = curBitStreamOffset >> 3;
    bitsPosA = splineKeysA + curByteOffset;
    bitsPosB = splineKeysB + curByteOffset;
    bitsPosC = splineTangentsA + curByteOffset;
    bitsPosD = splineTangentsB + curByteOffset;
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    tempC = (uint32_t)bitsPosC[0] | ((uint32_t)bitsPosC[1] << 8) | ((uint32_t)bitsPosC[2] << 16);
    tempD = (uint32_t)bitsPosD[0] | ((uint32_t)bitsPosD[1] << 8) | ((uint32_t)bitsPosD[2] << 16);
    shift = curBitStreamOffset & 0x07;
    mask = ~(0xffffffff << prec[0]);
    vecA.x = (float)((tempA >> shift) & mask);
    vecB.x = (float)((tempB >> shift) & mask);
    vecC.x = (float)((tempC >> shift) & mask);
    vecD.x = (float)((tempD >> shift) & mask);
    curBitStreamOffset += prec[0];
    // Y
    curByteOffset = curBitStreamOffset >> 3;
    bitsPosA = splineKeysA + curByteOffset;
    bitsPosB = splineKeysB + curByteOffset;
    bitsPosC = splineTangentsA + curByteOffset;
    bitsPosD = splineTangentsB + curByteOffset;
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    tempC = (uint32_t)bitsPosC[0] | ((uint32_t)bitsPosC[1] << 8) | ((uint32_t)bitsPosC[2] << 16);
    tempD = (uint32_t)bitsPosD[0] | ((uint32_t)bitsPosD[1] << 8) | ((uint32_t)bitsPosD[2] << 16);
    shift = curBitStreamOffset & 0x07;
    mask = ~(0xffffffff << prec[1]);
    vecA.y = (float)((tempA >> shift) & mask);
    vecB.y = (float)((tempB >> shift) & mask);
    vecC.y = (float)((tempC >> shift) & mask);
    vecD.y = (float)((tempD >> shift) & mask);
    curBitStreamOffset += prec[1];
    // Z
    curByteOffset = curBitStreamOffset >> 3;
    bitsPosA = splineKeysA + curByteOffset;
    bitsPosB = splineKeysB + curByteOffset;
    bitsPosC = splineTangentsA + curByteOffset;
    bitsPosD = splineTangentsB + curByteOffset;
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    tempC = (uint32_t)bitsPosC[0] | ((uint32_t)bitsPosC[1] << 8) | ((uint32_t)bitsPosC[2] << 16);
    tempD = (uint32_t)bitsPosD[0] | ((uint32_t)bitsPosD[1] << 8) | ((uint32_t)bitsPosD[2] << 16);
    shift = curBitStreamOffset & 0x07;
    mask = ~(0xffffffff << prec[2]);
    vecA.z = (float)((tempA >> shift) & mask);
    vecB.z = (float)((tempB >> shift) & mask);
    vecC.z = (float)((tempC >> shift) & mask);
    vecD.z = (float)((tempD >> shift) & mask);
    curBitStreamOffset += prec[2];

    //-------------------
    // dequantise the control points (relative to mean)
    vecA = vecA * qScale + qOffset;
    vecB = vecB * qScale + qOffset;
    vecC = vecC * qScale + qOffset;
    vecD = vecD * qScale + qOffset;

    // Evaluate
    p = (vecA * splinePosBasis[0]) +
        (vecC * splinePosBasis[1]) +
        (vecD * splinePosBasis[2]) +
        (vecB * splinePosBasis[3]);

    // Apply the mean translation to the interpolated key
    NMP::Vector3& pos = pOut[rigChannelIndex];
    pos = qBar + p;
  }
}
#endif // NMP_PLATFORM_SIMD

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void AnimSectionQSA::splineQuatDecompress(
  const AnimToRigTableMap* animToRigTableMap,
  const CompToAnimChannelMap* compToAnimMap,
  InternalDataQSA* internalData,
  NMP::DataBuffer* outputTransformBuffer) const
{
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimMap);
  NMP_ASSERT(internalData);
  NMP_ASSERT(outputTransformBuffer);
  
  const uint16_t* animChannelIndices = compToAnimMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  if (animChannelIndices[0] >= numLODEntries)
    return;

  uint8_t spaceFlag = internalData->m_useSpace;
  NMP::Quat* qOut = outputTransformBuffer->getPosQuatChannelQuat(0);
  NMP_ASSERT(qOut);

  // Set the spline basis functions
  NMP::vpu::vector4_t splineQuatBasisV = NMP::vpu::loadu4f(internalData->m_splineQuatBasis);
  NMP::vpu::vector4_t splineQuatBasis1 = NMP::vpu::splatY(splineQuatBasisV);
  NMP::vpu::vector4_t splineQuatBasis2 = NMP::vpu::splatZ(splineQuatBasisV);
  NMP::vpu::vector4_t splineQuatBasis3 = NMP::vpu::splatW(splineQuatBasisV);

  const uint8_t* splineQuatTangentsBData = m_splineQuatTangentsData + m_splineQuatTangentsByteStride * (m_splineNumKnots - 1);
  uint32_t knotIndex = internalData->m_knotIndex;
  const uint8_t* splineKeys[2];
  splineKeys[0] = m_splineQuatKeysData + m_splineQuatKeysByteStride * knotIndex;
  splineKeys[1] = splineKeys[0] + m_splineQuatKeysByteStride;
  uint32_t curBitStreamOffsetKeys = 0;
  const uint8_t* splineWNegs[2];
  splineWNegs[0] = m_splineQuatWNegsData + m_splineQuatWNegsByteStride * knotIndex;
  splineWNegs[1] = splineWNegs[0] + m_splineQuatWNegsByteStride;
  uint32_t tangentsOffset = m_splineQuatTangentsByteStride * knotIndex;
  const uint8_t* splineTangents[2];
  splineTangents[0] = m_splineQuatTangentsData + tangentsOffset;
  splineTangents[1] = splineQuatTangentsBData + tangentsOffset;
  uint32_t curBitStreamOffsetTangents = 0;

  // Compute the quantisation scale and offset information for the channel means
  NMP::vpu::vector4_t qMaxKeyMean = NMP::vpu::one4f();
  NMP::vpu::vector4_t qMinKeyMean = NMP::vpu::negOne4f();
  NMP::vpu::vector4_t qScaleKeyMean = NMP::vpu::uniformQuantisation8(qMinKeyMean, qMaxKeyMean);

  // Compute the quantisation scale and offset information for the quat tangent mean
  NMP::vpu::vector4_t qMinTangentMeanv = NMP::vpu::loadu4f(m_splineQuatTangentMeansQuantisationInfo.m_qMin);
  NMP::vpu::vector4_t qMaxTangentMeanv = NMP::vpu::loadu4f(m_splineQuatTangentMeansQuantisationInfo.m_qMax);
  NMP::vpu::vector4_t qScaleTangentMeanv = NMP::vpu::uniformQuantisation8(qMinTangentMeanv, qMaxTangentMeanv);

  NMP::vpu::Vector3MP qScaleTangentMean(qScaleTangentMeanv);
  NMP::vpu::Vector3MP qMinTangentMean(qMinTangentMeanv);

  //-----------------------
  // Iterate over the compression channels in blocks of four
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; indx += 4)
  {
    NMP::vpu::prefetchCacheLine(&animChannelIndices[indx], NMP_VPU_CACHESIZE);
    NMP::vpu::prefetchCacheLine(&animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]), NMP_VPU_CACHESIZE);
    uint32_t rigChannelIndex0 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);
    uint32_t rigChannelIndex1 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 1]);
    uint32_t rigChannelIndex2 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 2]);
    uint32_t rigChannelIndex3 = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx + 3]);

    //-----------------------
    // Get a reference to the quantisation keys data: mean/prec/qset
    NMP::vpu::prefetchCacheLine(&m_splineQuatKeysQuantisationData[indx], NMP_VPU_CACHESIZE);
    const QuantisationDataQSA& qDataKeys0 = m_splineQuatKeysQuantisationData[indx];
    const QuantisationDataQSA& qDataKeys1 = m_splineQuatKeysQuantisationData[indx + 1];
    const QuantisationDataQSA& qDataKeys2 = m_splineQuatKeysQuantisationData[indx + 2];
    const QuantisationDataQSA& qDataKeys3 = m_splineQuatKeysQuantisationData[indx + 3];    

    // Keys: Compose the fragmented quantised channel mean data into a single continuous block
    NMP::vpu::vector4_t vecX, vecY, vecZ, vecW;
    vecX = NMP::vpu::unpacku4i8(qDataKeys0.getMean());
    vecY = NMP::vpu::unpacku4i8(qDataKeys1.getMean());
    vecZ = NMP::vpu::unpacku4i8(qDataKeys2.getMean());
    vecW = NMP::vpu::unpacku4i8(qDataKeys3.getMean());

    NMP::vpu::Vector3MP vecKeyMean;
    vecKeyMean.pack(vecX, vecY, vecZ, vecW);
    vecKeyMean.convert2f();
    vecKeyMean = vecKeyMean * qScaleKeyMean + qMinKeyMean;

    // Convert the tan quarter angle rotation vectors back to quaternions
    NMP::vpu::QuatMP qBarKey = vecKeyMean.convertTQARotVecToQuat();

    //-----------------------
    // Get a reference to the quantisation tangents data: mean/prec/qset
    NMP::vpu::prefetchCacheLine(&m_splineQuatTangentsQuantisationData[indx], NMP_VPU_CACHESIZE);
    const QuantisationDataQSA& qDataTangents0 = m_splineQuatTangentsQuantisationData[indx];
    const QuantisationDataQSA& qDataTangents1 = m_splineQuatTangentsQuantisationData[indx + 1];
    const QuantisationDataQSA& qDataTangents2 = m_splineQuatTangentsQuantisationData[indx + 2];
    const QuantisationDataQSA& qDataTangents3 = m_splineQuatTangentsQuantisationData[indx + 3];

    // Tangents: Compose the fragmented quantised channel mean data into a single continuous block
    vecX = NMP::vpu::unpacku4i8(qDataTangents0.getMean());
    vecY = NMP::vpu::unpacku4i8(qDataTangents1.getMean());
    vecZ = NMP::vpu::unpacku4i8(qDataTangents2.getMean());
    vecW = NMP::vpu::unpacku4i8(qDataTangents3.getMean());

    NMP::vpu::Vector3MP vecTangentMean;
    vecTangentMean.pack(vecX, vecY, vecZ, vecW);
    vecTangentMean.convert2f();

    NMP::vpu::Vector3MP qbarTangent = vecTangentMean * qScaleTangentMean + qMinTangentMean;

    //-----------------------
    // Keys: Compose the fragmented precision data into a single continuous block
    vecX = NMP::vpu::unpacku4i8(qDataKeys0.getPrecision());
    vecY = NMP::vpu::unpacku4i8(qDataKeys1.getPrecision());
    vecZ = NMP::vpu::unpacku4i8(qDataKeys2.getPrecision());
    vecW = NMP::vpu::unpacku4i8(qDataKeys3.getPrecision());

    NMP::vpu::Vector3MP vecKeyPrec;
    vecKeyPrec.pack(vecX, vecY, vecZ, vecW);

    const QuantisationInfoQSA& qSetKeysX0 = m_splineQuatKeysQuantisationInfo[qDataKeys0.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetKeysX1 = m_splineQuatKeysQuantisationInfo[qDataKeys1.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetKeysX2 = m_splineQuatKeysQuantisationInfo[qDataKeys2.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetKeysX3 = m_splineQuatKeysQuantisationInfo[qDataKeys3.getQuantisationSetX()];
    NMP::vpu::vector4_t qMinKeyX = NMP::vpu::set4f(qSetKeysX0.m_qMin[0], qSetKeysX1.m_qMin[0], qSetKeysX2.m_qMin[0], qSetKeysX3.m_qMin[0]);
    NMP::vpu::vector4_t qMaxKeyX = NMP::vpu::set4f(qSetKeysX0.m_qMax[0], qSetKeysX1.m_qMax[0], qSetKeysX2.m_qMax[0], qSetKeysX3.m_qMax[0]);

    const QuantisationInfoQSA& qSetKeysY0 = m_splineQuatKeysQuantisationInfo[qDataKeys0.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetKeysY1 = m_splineQuatKeysQuantisationInfo[qDataKeys1.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetKeysY2 = m_splineQuatKeysQuantisationInfo[qDataKeys2.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetKeysY3 = m_splineQuatKeysQuantisationInfo[qDataKeys3.getQuantisationSetY()];
    NMP::vpu::vector4_t qMinKeyY = NMP::vpu::set4f(qSetKeysY0.m_qMin[1], qSetKeysY1.m_qMin[1], qSetKeysY2.m_qMin[1], qSetKeysY3.m_qMin[1]);
    NMP::vpu::vector4_t qMaxKeyY = NMP::vpu::set4f(qSetKeysY0.m_qMax[1], qSetKeysY1.m_qMax[1], qSetKeysY2.m_qMax[1], qSetKeysY3.m_qMax[1]);

    const QuantisationInfoQSA& qSetKeysZ0 = m_splineQuatKeysQuantisationInfo[qDataKeys0.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetKeysZ1 = m_splineQuatKeysQuantisationInfo[qDataKeys1.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetKeysZ2 = m_splineQuatKeysQuantisationInfo[qDataKeys2.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetKeysZ3 = m_splineQuatKeysQuantisationInfo[qDataKeys3.getQuantisationSetZ()];
    NMP::vpu::vector4_t qMinKeyZ = NMP::vpu::set4f(qSetKeysZ0.m_qMin[2], qSetKeysZ1.m_qMin[2], qSetKeysZ2.m_qMin[2], qSetKeysZ3.m_qMin[2]);
    NMP::vpu::vector4_t qMaxKeyZ = NMP::vpu::set4f(qSetKeysZ0.m_qMax[2], qSetKeysZ1.m_qMax[2], qSetKeysZ2.m_qMax[2], qSetKeysZ3.m_qMax[2]);

    NMP::vpu::Vector3MP qMinKey(qMinKeyX, qMinKeyY, qMinKeyZ);
    NMP::vpu::Vector3MP qMaxKey(qMaxKeyX, qMaxKeyY, qMaxKeyZ);

    // Compute the uniform quantisation scale and offset
    NMP::vpu::Vector3MP qScaleKey;
    qScaleKey.uniformQuantisation(qMinKey, qMaxKey, vecKeyPrec);

    //-----------------------
    // Tangents: Compose the fragmented data into a single continuous block
    vecX = NMP::vpu::unpacku4i8(qDataTangents0.getPrecision());
    vecY = NMP::vpu::unpacku4i8(qDataTangents1.getPrecision());
    vecZ = NMP::vpu::unpacku4i8(qDataTangents2.getPrecision());
    vecW = NMP::vpu::unpacku4i8(qDataTangents3.getPrecision());

    NMP::vpu::Vector3MP vecTangentPrec;
    vecTangentPrec.pack(vecX, vecY, vecZ, vecW);

    const QuantisationInfoQSA& qSetTangentsX0 = m_splineQuatTangentsQuantisationInfo[qDataTangents0.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetTangentsX1 = m_splineQuatTangentsQuantisationInfo[qDataTangents1.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetTangentsX2 = m_splineQuatTangentsQuantisationInfo[qDataTangents2.getQuantisationSetX()];
    const QuantisationInfoQSA& qSetTangentsX3 = m_splineQuatTangentsQuantisationInfo[qDataTangents3.getQuantisationSetX()];
    NMP::vpu::vector4_t qMinTangentX = NMP::vpu::set4f(qSetTangentsX0.m_qMin[0], qSetTangentsX1.m_qMin[0], qSetTangentsX2.m_qMin[0], qSetTangentsX3.m_qMin[0]);
    NMP::vpu::vector4_t qMaxTangentX = NMP::vpu::set4f(qSetTangentsX0.m_qMax[0], qSetTangentsX1.m_qMax[0], qSetTangentsX2.m_qMax[0], qSetTangentsX3.m_qMax[0]);

    const QuantisationInfoQSA& qSetTangentsY0 = m_splineQuatTangentsQuantisationInfo[qDataTangents0.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetTangentsY1 = m_splineQuatTangentsQuantisationInfo[qDataTangents1.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetTangentsY2 = m_splineQuatTangentsQuantisationInfo[qDataTangents2.getQuantisationSetY()];
    const QuantisationInfoQSA& qSetTangentsY3 = m_splineQuatTangentsQuantisationInfo[qDataTangents3.getQuantisationSetY()];
    NMP::vpu::vector4_t qMinTangentY = NMP::vpu::set4f(qSetTangentsY0.m_qMin[1], qSetTangentsY1.m_qMin[1], qSetTangentsY2.m_qMin[1], qSetTangentsY3.m_qMin[1]);
    NMP::vpu::vector4_t qMaxTangentY = NMP::vpu::set4f(qSetTangentsY0.m_qMax[1], qSetTangentsY1.m_qMax[1], qSetTangentsY2.m_qMax[1], qSetTangentsY3.m_qMax[1]);

    const QuantisationInfoQSA& qSetTangentsZ0 = m_splineQuatTangentsQuantisationInfo[qDataTangents0.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetTangentsZ1 = m_splineQuatTangentsQuantisationInfo[qDataTangents1.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetTangentsZ2 = m_splineQuatTangentsQuantisationInfo[qDataTangents2.getQuantisationSetZ()];
    const QuantisationInfoQSA& qSetTangentsZ3 = m_splineQuatTangentsQuantisationInfo[qDataTangents3.getQuantisationSetZ()];
    NMP::vpu::vector4_t qMinTangentZ = NMP::vpu::set4f(qSetTangentsZ0.m_qMin[2], qSetTangentsZ1.m_qMin[2], qSetTangentsZ2.m_qMin[2], qSetTangentsZ3.m_qMin[2]);
    NMP::vpu::vector4_t qMaxTangentZ = NMP::vpu::set4f(qSetTangentsZ0.m_qMax[2], qSetTangentsZ1.m_qMax[2], qSetTangentsZ2.m_qMax[2], qSetTangentsZ3.m_qMax[2]);

    NMP::vpu::Vector3MP qMinTangent(qMinTangentX, qMinTangentY, qMinTangentZ);
    NMP::vpu::Vector3MP qMaxTangent(qMaxTangentX, qMaxTangentY, qMaxTangentZ);

    // Compute the uniform quantisation scale and offset
    NMP::vpu::Vector3MP qScaleTangent, qOffsetTangent;
    qScaleTangent.uniformQuantisation(qMinTangent, qMaxTangent, vecTangentPrec);
    qOffsetTangent = qMinTangent + qbarTangent; // Apply mean

    //-----------------------
    // Keys: Compose the fragmented data into a single continuous block
    uint32_t prec;
    NMP::vpu::vector4_t dataX[2], dataY[2], dataZ[2];
    NMP::vpu::vector4_t precX, precY, precZ;
    NMP::vpu::vector4_t bitStreamOffsetX, bitStreamOffsetY, bitStreamOffsetZ;

    // This tight loop is faster than expanding out the code in sequence
    for (uint32_t i = 0; i < 4; ++i)
    {
      // X
      for (uint32_t entryIndex = 0; entryIndex < 2; ++entryIndex)
      {
        const uint8_t* bitsPos = &(splineKeys[entryIndex][curBitStreamOffsetKeys >> 3]);
        NMP::vpu::prefetchCacheLine(bitsPos, NMP_VPU_CACHESIZE);
        ((uint32_t*)&dataX[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_splineQuatKeysQuantisationData[indx + i].getPrecisionX();
      ((uint32_t*)&precX)[i] = prec;
      ((uint32_t*)&bitStreamOffsetX)[i] = curBitStreamOffsetKeys;
      curBitStreamOffsetKeys += prec;

      // Y
      for (uint32_t entryIndex = 0; entryIndex < 2; ++entryIndex)
      {
        const uint8_t* bitsPos = &(splineKeys[entryIndex][curBitStreamOffsetKeys >> 3]);
        ((uint32_t*)&dataY[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_splineQuatKeysQuantisationData[indx + i].getPrecisionY();
      ((uint32_t*)&precY)[i] = prec;
      ((uint32_t*)&bitStreamOffsetY)[i] = curBitStreamOffsetKeys;
      curBitStreamOffsetKeys += prec;

      // Z
      for (uint32_t entryIndex = 0; entryIndex < 2; ++entryIndex)
      {
        const uint8_t* bitsPos = &(splineKeys[entryIndex][curBitStreamOffsetKeys >> 3]);
        ((uint32_t*)&dataZ[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_splineQuatKeysQuantisationData[indx + i].getPrecisionZ();
      ((uint32_t*)&precZ)[i] = prec;
      ((uint32_t*)&bitStreamOffsetZ)[i] = curBitStreamOffsetKeys;
      curBitStreamOffsetKeys += prec;
    }

    // Shift mask and convert to float    
    NMP::vpu::vector4_t andMask = NMP::vpu::set4i(0x07);
    NMP::vpu::vector4_t shiftX = NMP::vpu::and4(bitStreamOffsetX, andMask);
    NMP::vpu::vector4_t shiftY = NMP::vpu::and4(bitStreamOffsetY, andMask);
    NMP::vpu::vector4_t shiftZ = NMP::vpu::and4(bitStreamOffsetZ, andMask);

    NMP::vpu::vector4_t shiftMask = NMP::vpu::set4i(0xffffffff);
    NMP::vpu::vector4_t maskX = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precX));
    NMP::vpu::vector4_t maskY = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precY));
    NMP::vpu::vector4_t maskZ = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precZ));

    NMP::vpu::Vector3MP vecKeyA(
      NMP::vpu::and4(NMP::vpu::shr4(dataX[0], shiftX), maskX),
      NMP::vpu::and4(NMP::vpu::shr4(dataY[0], shiftY), maskY),
      NMP::vpu::and4(NMP::vpu::shr4(dataZ[0], shiftZ), maskZ));
    vecKeyA.convert2f();

    NMP::vpu::Vector3MP vecKeyB(
      NMP::vpu::and4(NMP::vpu::shr4(dataX[1], shiftX), maskX),
      NMP::vpu::and4(NMP::vpu::shr4(dataY[1], shiftY), maskY),
      NMP::vpu::and4(NMP::vpu::shr4(dataZ[1], shiftZ), maskZ));
    vecKeyB.convert2f();

    //-----------------------
    // WNegs
    uint32_t wNegBitsA = splineWNegs[0][indx >> 3];
    uint32_t wNegBitsB = splineWNegs[1][indx >> 3];
    uint32_t wNegShift = indx & 0x07;
    uint32_t wNegBitsAShifted = wNegBitsA >> wNegShift;
    uint32_t wNegBitsBShifted = wNegBitsB >> wNegShift;
    NMP::vpu::vector4_t wNegMaskA = NMP::vpu::maskr4i(wNegBitsAShifted);
    NMP::vpu::vector4_t wNegMaskB = NMP::vpu::maskr4i(wNegBitsBShifted);

    // Convert the tan quarter angle rotation vectors back to quaternions
    vecKeyA = vecKeyA * qScaleKey + qMinKey;
    vecKeyB = vecKeyB * qScaleKey + qMinKey;
    NMP::vpu::QuatMP keyA = vecKeyA.convertTQARotVecToQuat(wNegMaskA);
    NMP::vpu::QuatMP keyB = vecKeyB.convertTQARotVecToQuat(wNegMaskB);

    //-----------------------
    // Tangents: Compose the fragmented data into a single continuous block
    
    // This tight loop is faster than expanding out the code in sequence
    for (uint32_t i = 0; i < 4; ++i)
    {
      // X
      for (uint32_t entryIndex = 0; entryIndex < 2; ++entryIndex)
      {
        const uint8_t* bitsPos = &(splineTangents[entryIndex][curBitStreamOffsetTangents >> 3]);
        NMP::vpu::prefetchCacheLine(bitsPos, NMP_VPU_CACHESIZE);
        ((uint32_t*)&dataX[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_splineQuatTangentsQuantisationData[indx + i].getPrecisionX();
      ((uint32_t*)&precX)[i] = prec;
      ((uint32_t*)&bitStreamOffsetX)[i] = curBitStreamOffsetTangents;
      curBitStreamOffsetTangents += prec;

      // Y
      for (uint32_t entryIndex = 0; entryIndex < 2; ++entryIndex)
      {
        const uint8_t* bitsPos = &(splineTangents[entryIndex][curBitStreamOffsetTangents >> 3]);
        ((uint32_t*)&dataY[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_splineQuatTangentsQuantisationData[indx + i].getPrecisionY();
      ((uint32_t*)&precY)[i] = prec;
      ((uint32_t*)&bitStreamOffsetY)[i] = curBitStreamOffsetTangents;
      curBitStreamOffsetTangents += prec;

      // Z
      for (uint32_t entryIndex = 0; entryIndex < 2; ++entryIndex)
      {
        const uint8_t* bitsPos = &(splineTangents[entryIndex][curBitStreamOffsetTangents >> 3]);
        ((uint32_t*)&dataZ[entryIndex])[i] = (uint32_t)bitsPos[0] | ((uint32_t)bitsPos[1] << 8) | ((uint32_t)bitsPos[2] << 16);
      }
      prec = m_splineQuatTangentsQuantisationData[indx + i].getPrecisionZ();
      ((uint32_t*)&precZ)[i] = prec;
      ((uint32_t*)&bitStreamOffsetZ)[i] = curBitStreamOffsetTangents;
      curBitStreamOffsetTangents += prec;
    }

    // Shift mask and convert to float    
    shiftX = NMP::vpu::and4(bitStreamOffsetX, andMask);
    shiftY = NMP::vpu::and4(bitStreamOffsetY, andMask);
    shiftZ = NMP::vpu::and4(bitStreamOffsetZ, andMask);

    maskX = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precX));
    maskY = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precY));
    maskZ = NMP::vpu::not4(NMP::vpu::shl4(shiftMask, precZ));

    NMP::vpu::Vector3MP vecTA(
      NMP::vpu::and4(NMP::vpu::shr4(dataX[0], shiftX), maskX),
      NMP::vpu::and4(NMP::vpu::shr4(dataY[0], shiftY), maskY),
      NMP::vpu::and4(NMP::vpu::shr4(dataZ[0], shiftZ), maskZ));
    vecTA.convert2f();

    NMP::vpu::Vector3MP vecTB(
      NMP::vpu::and4(NMP::vpu::shr4(dataX[1], shiftX), maskX),
      NMP::vpu::and4(NMP::vpu::shr4(dataY[1], shiftY), maskY),
      NMP::vpu::and4(NMP::vpu::shr4(dataZ[1], shiftZ), maskZ));
    vecTB.convert2f();

    vecTA = vecTA * qScaleTangent + qOffsetTangent;
    vecTB = vecTB * qScaleTangent + qOffsetTangent;

    //-----------------------
    // Interpolation

    // Compute the q1 control quaternion: q1 = qqmul(q0, qexp(w1));
    NMP::vpu::vector4_t tangentA_phi = vecTA.toAngleAxis();
    NMP::vpu::QuatMP dq = vecTA.qExp(tangentA_phi);

    NMP::vpu::QuatMP q1 = keyA * dq;

    // Compute the q2 control quaternion: q2 = qqmul(q3, qinv(qexp(w3)));
    NMP::vpu::vector4_t tangentB_phi = vecTB.toAngleAxis();
    dq = vecTB.qExp(tangentB_phi);

    NMP::vpu::QuatMP q2 = keyB * dq.conjugate();

    // Evaluate the mid angular velocity vector w2 = qlog(qqmul(qinv(q1), q2));
    NMP::vpu::vector4_t w2_phi;
    NMP::vpu::Vector3MP w2 = (q1.conjugate() * q2).qLog(w2_phi);

    // Compute the quaternion spline:
    NMP::vpu::vector4_t e1_phi = NMP::vpu::mul4f(splineQuatBasis1, tangentA_phi);
    NMP::vpu::vector4_t e2_phi = NMP::vpu::mul4f(splineQuatBasis2, w2_phi);
    NMP::vpu::vector4_t e3_phi = NMP::vpu::mul4f(splineQuatBasis3, tangentB_phi);

    // qt = qqmul(q0, qqmul(qexp(w1*C1), qqmul(qexp(w2*C2), qexp(w3*C3))))
    NMP::vpu::QuatMP quat = qBarKey * (keyA * (vecTA.qExp(e1_phi) * (w2.qExp(e2_phi) * vecTB.qExp(e3_phi))));

    quat.unpack(vecX, vecY, vecZ, vecW);

    //-----------------------
    // Store the quat data in the correct rig locations
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex0], vecX);
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex1], vecY);
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex2], vecZ);
    NMP::vpu::store4f((float*)&qOut[rigChannelIndex3], vecW);

    // Set the channel as being worldspace
    internalData->m_chanQuatSpaces[rigChannelIndex0] = spaceFlag;
    internalData->m_chanQuatSpaces[rigChannelIndex1] = spaceFlag;
    internalData->m_chanQuatSpaces[rigChannelIndex2] = spaceFlag;
    internalData->m_chanQuatSpaces[rigChannelIndex3] = spaceFlag;
  }
}
#else
//----------------------------------------------------------------------------------------------------------------------
void AnimSectionQSA::splineQuatDecompress(
  const AnimToRigTableMap* animToRigTableMap,
  const CompToAnimChannelMap* compToAnimMap,
  InternalDataQSA* internalData,
  NMP::DataBuffer* outputTransformBuffer) const
{
  NMP_ASSERT(animToRigTableMap);
  NMP_ASSERT(compToAnimMap);
  NMP_ASSERT(internalData);
  NMP_ASSERT(outputTransformBuffer);

  const uint16_t* animChannelIndices = compToAnimMap->getAnimChannels();
  NMP_ASSERT(animChannelIndices);
  uint32_t numLODEntries = animToRigTableMap->getNumAnimChannelsForLOD();
  if (animChannelIndices[0] >= numLODEntries)
    return;

  uint8_t spaceFlag = internalData->m_useSpace;
  NMP::Quat* qOut = outputTransformBuffer->getPosQuatChannelQuat(0);
  NMP_ASSERT(qOut);
  const float* splineQuatBasis = internalData->m_splineQuatBasis;
  NMP_ASSERT(splineQuatBasis);

  const uint8_t* splineQuatTangentsBData = m_splineQuatTangentsData + m_splineQuatTangentsByteStride * (m_splineNumKnots - 1);
  uint32_t knotIndex = internalData->m_knotIndex;
  const uint8_t* splineKeysA = m_splineQuatKeysData + m_splineQuatKeysByteStride * knotIndex;
  const uint8_t* splineKeysB = splineKeysA + m_splineQuatKeysByteStride;
  uint32_t curBitStreamOffsetKeys = 0;
  const uint8_t* splineWNegsA = m_splineQuatWNegsData + m_splineQuatWNegsByteStride * knotIndex;
  const uint8_t* splineWNegsB = splineWNegsA + m_splineQuatWNegsByteStride;
  uint32_t tangentsOffset = m_splineQuatTangentsByteStride * knotIndex;
  const uint8_t* splineTangentsA = m_splineQuatTangentsData + tangentsOffset;
  const uint8_t* splineTangentsB = splineQuatTangentsBData + tangentsOffset;
  uint32_t curBitStreamOffsetTangents = 0;

  NMP::Vector3 qScaleKeys, qOffsetKeys;
  NMP::Vector3 qScaleTangents, qOffsetTangents;
  NMP::Vector3 qScaleKeyMean, qOffsetKeyMean;
  NMP::Vector3 qScaleTangentMean, qOffsetTangentMean;
  NMP::Vector3 qBarKeys, qBarTangents;
  NMP::Vector3 w1, w2, w3, v;
  NMP::Quat q0, q1, q2, q3, q, dq, qkm;
  NMP::Quat e1, e2, e3;
  uint32_t precKeys[3];
  uint32_t qSetKeys[3];
  uint32_t qMeanKeys[3];
  uint32_t precTangents[3];
  uint32_t qSetTangents[3];
  uint32_t qMeanTangents[3];
  float qMinKeys[3];
  float qMaxKeys[3];
  float qMinTangents[3];
  float qMaxTangents[3];
  const float qMinTQA[3] = { -1.0f, -1.0f, -1.0f };
  const float qMaxTQA[3] = { 1.0f, 1.0f, 1.0f };

  // Compute the quantisation scale and offset information for the quat key mean
  MR::uniformQuantisation8(
    qMinTQA,
    qMaxTQA,
    qScaleKeyMean,
    qOffsetKeyMean);

  // Compute the quantisation scale and offset information for the quat tangent mean
  MR::uniformQuantisation8(
    m_splineQuatTangentMeansQuantisationInfo.m_qMin,
    m_splineQuatTangentMeansQuantisationInfo.m_qMax,
    qScaleTangentMean,
    qOffsetTangentMean);

  // Iterate over the compression channels
  for (uint32_t indx = 0; animChannelIndices[indx] < numLODEntries; ++indx)
  {
    uint32_t rigChannelIndex = animToRigTableMap->getAnimToRigMapEntry(animChannelIndices[indx]);

    // Get a reference to the quantisation quat key data: mean/prec/qset
    const QuantisationDataQSA& qDataKeys = m_splineQuatKeysQuantisationData[indx];

    // Get a reference to the quantisation quat tangent data: mean/prec/qset
    const QuantisationDataQSA& qDataTangents = m_splineQuatTangentsQuantisationData[indx];

    //-----------------------
    // Get the quantised quat key means for this channel
    qMeanKeys[0] = qDataKeys.getMeanX();
    qMeanKeys[1] = qDataKeys.getMeanY();
    qMeanKeys[2] = qDataKeys.getMeanZ();
    MR::dequantise(qScaleKeyMean, qOffsetKeyMean, qMeanKeys, qBarKeys);
    MR::fromRotationVectorTQA(qBarKeys, qkm);

    // Get the quat key precisions for this channel
    precKeys[0] = qDataKeys.getPrecisionX();
    precKeys[1] = qDataKeys.getPrecisionY();
    precKeys[2] = qDataKeys.getPrecisionZ();

    // Get the quat key quantisation sets for this channel
    qSetKeys[0] = qDataKeys.getQuantisationSetX();
    qSetKeys[1] = qDataKeys.getQuantisationSetY();
    qSetKeys[2] = qDataKeys.getQuantisationSetZ();
    NMP_ASSERT(qSetKeys[0] < m_splineQuatKeysNumQuantisationSets);
    NMP_ASSERT(qSetKeys[1] < m_splineQuatKeysNumQuantisationSets);
    NMP_ASSERT(qSetKeys[2] < m_splineQuatKeysNumQuantisationSets);

    // Set the quantisation range for the quat keys
    const QuantisationInfoQSA& qsk0 = m_splineQuatKeysQuantisationInfo[qSetKeys[0]];
    qMinKeys[0] = qsk0.m_qMin[0];
    qMaxKeys[0] = qsk0.m_qMax[0];
    const QuantisationInfoQSA& qsk1 = m_splineQuatKeysQuantisationInfo[qSetKeys[1]];
    qMinKeys[1] = qsk1.m_qMin[1];
    qMaxKeys[1] = qsk1.m_qMax[1];
    const QuantisationInfoQSA& qsk2 = m_splineQuatKeysQuantisationInfo[qSetKeys[2]];
    qMinKeys[2] = qsk2.m_qMin[2];
    qMaxKeys[2] = qsk2.m_qMax[2];

    // Compute the quantisation scale and offset information for the quat keys
    MR::uniformQuantisation(
      qMinKeys,
      qMaxKeys,
      precKeys,
      qScaleKeys,
      qOffsetKeys);

    //-----------------------
    // Get the quantised quat tangent means for this channel
    qMeanTangents[0] = qDataTangents.getMeanX();
    qMeanTangents[1] = qDataTangents.getMeanY();
    qMeanTangents[2] = qDataTangents.getMeanZ();
    MR::dequantise(qScaleTangentMean, qOffsetTangentMean, qMeanTangents, qBarTangents);

    // Get the quat key precisions for this channel
    precTangents[0] = qDataTangents.getPrecisionX();
    precTangents[1] = qDataTangents.getPrecisionY();
    precTangents[2] = qDataTangents.getPrecisionZ();

    // Get the quat key quantisation sets for this channel
    qSetTangents[0] = qDataTangents.getQuantisationSetX();
    qSetTangents[1] = qDataTangents.getQuantisationSetY();
    qSetTangents[2] = qDataTangents.getQuantisationSetZ();
    NMP_ASSERT(qSetTangents[0] < m_splineQuatTangentsNumQuantisationSets);
    NMP_ASSERT(qSetTangents[1] < m_splineQuatTangentsNumQuantisationSets);
    NMP_ASSERT(qSetTangents[2] < m_splineQuatTangentsNumQuantisationSets);

    // Set the quantisation range for the quat keys
    const QuantisationInfoQSA& qst0 = m_splineQuatTangentsQuantisationInfo[qSetTangents[0]];
    qMinTangents[0] = qst0.m_qMin[0];
    qMaxTangents[0] = qst0.m_qMax[0];
    const QuantisationInfoQSA& qst1 = m_splineQuatTangentsQuantisationInfo[qSetTangents[1]];
    qMinTangents[1] = qst1.m_qMin[1];
    qMaxTangents[1] = qst1.m_qMax[1];
    const QuantisationInfoQSA& qst2 = m_splineQuatTangentsQuantisationInfo[qSetTangents[2]];
    qMinTangents[2] = qst2.m_qMin[2];
    qMaxTangents[2] = qst2.m_qMax[2];

    // Compute the quantisation scale and offset information for the quat keys
    MR::uniformQuantisation(
      qMinTangents,
      qMaxTangents,
      precTangents,
      qScaleTangents,
      qOffsetTangents);

    // Apply the tangent mean to the offset
    qOffsetTangents += qBarTangents;

    //-------------------
    // Decode the bitstream data
    const uint8_t* bitsPosA;
    const uint8_t* bitsPosB;
    uint32_t tempA, tempB;
    NMP::Vector3 vecA, vecB;
    uint32_t shift, mask;

    // X
    bitsPosA = &splineKeysA[curBitStreamOffsetKeys >> 3];
    bitsPosB = &splineKeysB[curBitStreamOffsetKeys >> 3];
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    shift = curBitStreamOffsetKeys & 0x07;
    mask = ~(0xffffffff << precKeys[0]);
    vecA.x = (float)((tempA >> shift) & mask);
    vecB.x = (float)((tempB >> shift) & mask);
    curBitStreamOffsetKeys += precKeys[0];
    // Y
    bitsPosA = &splineKeysA[curBitStreamOffsetKeys >> 3];
    bitsPosB = &splineKeysB[curBitStreamOffsetKeys >> 3];
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    shift = curBitStreamOffsetKeys & 0x07;
    mask = ~(0xffffffff << precKeys[1]);
    vecA.y = (float)((tempA >> shift) & mask);
    vecB.y = (float)((tempB >> shift) & mask);
    curBitStreamOffsetKeys += precKeys[1];
    // Z
    bitsPosA = &splineKeysA[curBitStreamOffsetKeys >> 3];
    bitsPosB = &splineKeysB[curBitStreamOffsetKeys >> 3];
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    shift = curBitStreamOffsetKeys & 0x07;
    mask = ~(0xffffffff << precKeys[2]);
    vecA.z = (float)((tempA >> shift) & mask);
    vecB.z = (float)((tempB >> shift) & mask);
    curBitStreamOffsetKeys += precKeys[2];
    
    // WNegs
    uint8_t wNegBitsA = splineWNegsA[indx >> 3];
    uint8_t wNegBitsB = splineWNegsB[indx >> 3];
    shift = indx & 0x07;
    uint32_t wNegA = (wNegBitsA >> shift) & 0x01;
    uint32_t wNegB = (wNegBitsB >> shift) & 0x01;

    vecA = vecA * qScaleKeys + qOffsetKeys;
    vecB = vecB * qScaleKeys + qOffsetKeys;
    MR::fromRotationVectorTQA(vecA, q0, wNegA);
    MR::fromRotationVectorTQA(vecB, q3, wNegB);

    //-------------------
    // Compute the tangent angular velocity w1 (relative to q0 key)
    // X
    bitsPosA = &splineTangentsA[curBitStreamOffsetTangents >> 3];
    bitsPosB = &splineTangentsB[curBitStreamOffsetTangents >> 3];
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    shift = curBitStreamOffsetTangents & 0x07;
    mask = ~(0xffffffff << precTangents[0]);
    vecA.x = (float)((tempA >> shift) & mask);
    vecB.x = (float)((tempB >> shift) & mask);
    curBitStreamOffsetTangents += precTangents[0];
    // Y
    bitsPosA = &splineTangentsA[curBitStreamOffsetTangents >> 3];
    bitsPosB = &splineTangentsB[curBitStreamOffsetTangents >> 3];
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    shift = curBitStreamOffsetTangents & 0x07;
    mask = ~(0xffffffff << precTangents[1]);
    vecA.y = (float)((tempA >> shift) & mask);
    vecB.y = (float)((tempB >> shift) & mask);
    curBitStreamOffsetTangents += precTangents[1];
    // Z
    bitsPosA = &splineTangentsA[curBitStreamOffsetTangents >> 3];
    bitsPosB = &splineTangentsB[curBitStreamOffsetTangents >> 3];
    tempA = (uint32_t)bitsPosA[0] | ((uint32_t)bitsPosA[1] << 8) | ((uint32_t)bitsPosA[2] << 16);
    tempB = (uint32_t)bitsPosB[0] | ((uint32_t)bitsPosB[1] << 8) | ((uint32_t)bitsPosB[2] << 16);
    shift = curBitStreamOffsetTangents & 0x07;
    mask = ~(0xffffffff << precTangents[2]);
    vecA.z = (float)((tempA >> shift) & mask);
    vecB.z = (float)((tempB >> shift) & mask);
    curBitStreamOffsetTangents += precTangents[2];
    
    w1 = vecA * qScaleTangents + qOffsetTangents;
    w3 = vecB * qScaleTangents + qOffsetTangents;

    //-------------------
    // Compute the Bezier quaternion spline coefficients: q0, w1, w2, w3

    // Compute the q1 control quaternion: q1 = qqmul(q0, qexp(w1));
    dq.fastExp(w1);
    q1.multiply(q0, dq);

    // Compute the q2 control quaternion: q2 = qqmul(q3, qinv(qexp(w3)));
    dq.fastExp(w3);
    dq.conjugate();
    q2.multiply(q3, dq);

    // Evaluate the mid angular velocity vector w2 = qlog(qqmul(qinv(q1), q2));
    q1.conjugate();
    dq.multiply(q1, q2);
    w2 = dq.fastLog();

    //-------------------
    // Compute the quaternion spline: qt = qqmul(q0, qqmul(qexp(w1*C1), qqmul(qexp(w2*C2), qexp(w3*C3))))
    e1.fastExp(w1 * splineQuatBasis[1]);
    e2.fastExp(w2 * splineQuatBasis[2]);
    e3.fastExp(w3 * splineQuatBasis[3]);
    q = q0 * (e1 * (e2 * e3));

    // Apply the mean rotation to the interpolated key
    NMP::Quat& quat = qOut[rigChannelIndex];
    quat = qkm * q;

    // Set the channel as being worldspace
    internalData->m_chanQuatSpaces[rigChannelIndex] = spaceFlag;
  }
}
#endif // NMP_PLATFORM_SIMD

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
#pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
