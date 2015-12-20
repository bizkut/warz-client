// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrCompressedDataBufferVector3.h"
#include "NMPlatform/NMvpu.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferVector3::locate()
{
  NMP::endianSwap(m_scales);
  NMP::endianSwap(m_offsets);
  NMP::endianSwap(m_length);
  REFIX_SWAP_PTR(uint32_t, m_sampledKeys);
  NMP::endianSwapArray(m_sampledKeys, m_length);
}

//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferVector3::dislocate()
{
  NMP::endianSwapArray(m_sampledKeys, m_length);
  UNFIX_SWAP_PTR(uint32_t, m_sampledKeys);
  NMP::endianSwap(m_length);
  NMP::endianSwap(m_offsets);
  NMP::endianSwap(m_scales);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format CompressedDataBufferVector3::getMemoryRequirements(uint32_t length)
{
  uint32_t length4 = (length + 3) & (~0x03);
  NMP::Memory::Format result(sizeof(CompressedDataBufferVector3), NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Format memReqsData(sizeof(uint32_t) * length4, NMP_VECTOR_ALIGNMENT);
  result += memReqsData;
  
  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
CompressedDataBufferVector3* CompressedDataBufferVector3::init(
  NMP::Memory::Resource&   resource,
  uint32_t                 length)
{  
  // Header
  uint32_t length4 = (length + 3) & (~0x03);
  NMP::Memory::Format memReqsHdr(sizeof(CompressedDataBufferVector3), NMP_VECTOR_ALIGNMENT);
  CompressedDataBufferVector3* result = (CompressedDataBufferVector3*) resource.alignAndIncrement(memReqsHdr);
  result->m_length = length;
  
  // Data
  NMP::Memory::Format memReqsData(sizeof(uint32_t) * length4, NMP_VECTOR_ALIGNMENT);
  result->m_sampledKeys = (uint32_t*) resource.alignAndIncrement(memReqsData);
  
  // Make sure size is a multiple of the alignment requirement.
  resource.align(NMP_VECTOR_ALIGNMENT);
  
  return result;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferVector3::encode(const NMP::Vector3* dataBuffer)
{ 
  // Compute the quantisation bounds
  NMP::Vector3 qMin, qMax;
  vBounds(m_length, dataBuffer, qMin, qMax);
  
  // Set the quantisation scale and offset
  const float step11 = 1.0f / 0x7FF;
  const float step10 = 1.0f / 0x3FF;
  m_scales.set(
    (qMax.x - qMin.x) * step11,
    (qMax.y - qMin.y) * step11,
    (qMax.z - qMin.z) * step10);
  m_offsets = qMin;
  
  // Calculate the quantisation factors such that:
  // q = (int)( (x - xmin) * ((1 << p - 1) / (xmax - xmin)) + 0.5 )
  float kx = 0x7FF / (qMax.x - qMin.x);
  float tx = 0.5f - qMin.x * kx;
  float ky = 0x7FF / (qMax.y - qMin.y);
  float ty = 0.5f - qMin.y * ky;
  float kz = 0x3FF / (qMax.z - qMin.z);
  float tz = 0.5f - qMin.z * kz;
  
  // Quantise the data samples 
  for (uint32_t i = 0; i < m_length; ++i)
  {
    uint32_t qx = (uint32_t)(kx * dataBuffer[i].x + tx);
    uint32_t qy = (uint32_t)(ky * dataBuffer[i].y + ty);
    uint32_t qz = (uint32_t)(kz * dataBuffer[i].z + tz);
    m_sampledKeys[i] = (qx << 21) | (qy << 10) | qz;
  }
  
  // Pad the remaining data
  uint32_t length4 = (m_length + 3) & (~0x03);
  for (uint32_t i = m_length; i < length4; ++i)
  {
    m_sampledKeys[i] = 0;
  }
}

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferVector3::decode(NMP::Vector3* dataBuffer) const
{
  NMP_ASSERT(dataBuffer);

  // Quantisation scales and offsets in SOA format
  NMP::vpu::vector4_t vecX, vecY, vecZ, vecW;
  NMP::vpu::vector4_t qScales = NMP::vpu::load4f((float*)&m_scales);
  NMP::vpu::vector4_t qOffsets = NMP::vpu::load4f((float*)&m_offsets);
  vecX = NMP::vpu::splatX(qScales);
  vecY = NMP::vpu::splatY(qScales);
  vecZ = NMP::vpu::splatZ(qScales);
  NMP::vpu::Vector3MP qScales4(vecX, vecY, vecZ);
  vecX = NMP::vpu::splatX(qOffsets);
  vecY = NMP::vpu::splatY(qOffsets);
  vecZ = NMP::vpu::splatZ(qOffsets);
  NMP::vpu::Vector3MP qOffsets4(vecX, vecY, vecZ);

  // Iterate over the compression channels in blocks of four
  uint32_t length4 = (m_length + 3) & (~0x03);
  for (uint32_t indx = 0; indx < length4; indx += 4)
  {
    float* pOut = (float*)&dataBuffer[indx];
    NMP::vpu::prefetchCacheLine(pOut, 0);
    NMP::vpu::prefetchCacheLine(pOut, NMP_VPU_CACHESIZE);
    
    // Dequantise the key sample vectors in SOA format
    NMP::vpu::Vector3MP pos4;
    NMP::vpu::unpack4QuantizedVectorsPacked(pos4.x, pos4.y, pos4.z, &m_sampledKeys[indx]);
    pos4 = pos4 * qScales4 + qOffsets4;
    
    // Unpack quat data into AOS format and store in the output buffer
    pos4.unpack(vecX, vecY, vecZ, vecW);
    NMP::vpu::store4v(pOut, vecX, vecY, vecZ, vecW);
  }
}

#else
//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferVector3::decode(NMP::Vector3* dataBuffer) const
{
  NMP_ASSERT(dataBuffer);

  for (uint32_t i = 0; i < m_length; ++i)
  {
    uint32_t key = m_sampledKeys[i];
    uint32_t qx = ((key >> 21) & 0x07FF);  // 11 bits
    uint32_t qy = ((key >> 10) & 0x07FF);  // 11 bits
    uint32_t qz = (key & 0x03FF);   // 10 bits
    NMP::Vector3 v((float)qx, (float)qy, (float)qz);
    v.multiplyElements(m_scales);
    dataBuffer[i] = v + m_offsets;
  }
}
#endif

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferVector3::decodeMP(NMP::Vector3* dataBuffer) const
{
  NMP_ASSERT(dataBuffer);
  
  // Zero vector
  NMP::vpu::vector4_t vecW = NMP::vpu::zero4f();

  // Quantisation scales and offsets in SOA format
  NMP::vpu::vector4_t vecX, vecY, vecZ;
  NMP::vpu::vector4_t qScales = NMP::vpu::load4f((float*)&m_scales);
  NMP::vpu::vector4_t qOffsets = NMP::vpu::load4f((float*)&m_offsets);
  vecX = NMP::vpu::splatX(qScales);
  vecY = NMP::vpu::splatY(qScales);
  vecZ = NMP::vpu::splatZ(qScales);
  NMP::vpu::Vector3MP qScales4(vecX, vecY, vecZ);
  vecX = NMP::vpu::splatX(qOffsets);
  vecY = NMP::vpu::splatY(qOffsets);
  vecZ = NMP::vpu::splatZ(qOffsets);
  NMP::vpu::Vector3MP qOffsets4(vecX, vecY, vecZ);

  // Iterate over the compression channels in blocks of four
  uint32_t length4 = (m_length + 3) & (~0x03);
  for (uint32_t indx = 0; indx < length4; indx += 4)
  {
    float* pOut = (float*)&dataBuffer[indx];
    NMP::vpu::prefetchCacheLine(pOut, 0);
    NMP::vpu::prefetchCacheLine(pOut, NMP_VPU_CACHESIZE);

    // Dequantise the key sample vectors in SOA format
    NMP::vpu::Vector3MP pos4;
    NMP::vpu::unpack4QuantizedVectorsPacked(pos4.x, pos4.y, pos4.z, &m_sampledKeys[indx]);
    pos4 = pos4 * qScales4 + qOffsets4;
    
    // Store the packed data in the output buffer
    NMP::vpu::store4v(pOut, pos4.x, pos4.y, pos4.z, vecW);
  }
}

#else
//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferVector3::decodeMP(NMP::Vector3* dataBuffer) const
{
  (void)dataBuffer;
  NMP_ASSERT_FAIL(); // No multiplexed implementation for non SIMD
}

#endif // NMP_PLATFORM_SIMD

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
