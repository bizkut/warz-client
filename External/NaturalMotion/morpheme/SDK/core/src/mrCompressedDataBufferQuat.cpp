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
#include "morpheme/mrCompressedDataBufferQuat.h"
#include "NMPlatform/NMvpu.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferQuat::locate()
{
  NMP::endianSwap(m_scales);
  NMP::endianSwap(m_offsets);
  NMP::endianSwap(m_length);
  REFIX_SWAP_PTR(uint16_t, m_sampledKeysX);
  NMP::endianSwapArray(m_sampledKeysX, m_length);
  REFIX_SWAP_PTR(uint16_t, m_sampledKeysY);
  NMP::endianSwapArray(m_sampledKeysY, m_length);
  REFIX_SWAP_PTR(uint16_t, m_sampledKeysZ);
  NMP::endianSwapArray(m_sampledKeysZ, m_length);
}

//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferQuat::dislocate()
{
  NMP::endianSwapArray(m_sampledKeysZ, m_length);
  UNFIX_SWAP_PTR(uint16_t, m_sampledKeysZ);
  NMP::endianSwapArray(m_sampledKeysY, m_length);
  UNFIX_SWAP_PTR(uint16_t, m_sampledKeysY);
  NMP::endianSwapArray(m_sampledKeysX, m_length);
  UNFIX_SWAP_PTR(uint16_t, m_sampledKeysX);
  NMP::endianSwap(m_length);
  NMP::endianSwap(m_offsets);
  NMP::endianSwap(m_scales);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format CompressedDataBufferQuat::getMemoryRequirements(uint32_t length)
{
  uint32_t length4 = (length + 3) & (~0x03);
  NMP::Memory::Format result(sizeof(CompressedDataBufferQuat), NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Format memReqsData(sizeof(uint16_t) * length4, NMP_NATURAL_TYPE_ALIGNMENT); // unaligned unpack
  memReqsData *= 3; // XYZ component planes
  result += memReqsData;
  
  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
CompressedDataBufferQuat* CompressedDataBufferQuat::init(
  NMP::Memory::Resource&   resource,
  uint32_t                 length)
{
  // Header
  uint32_t length4 = (length + 3) & (~0x03);
  NMP::Memory::Format memReqsHdr(sizeof(CompressedDataBufferQuat), NMP_VECTOR_ALIGNMENT);
  CompressedDataBufferQuat* result = (CompressedDataBufferQuat*) resource.alignAndIncrement(memReqsHdr);
  result->m_length = length;
  
  // Data
  NMP::Memory::Format memReqsData(sizeof(uint16_t) * length4, NMP_NATURAL_TYPE_ALIGNMENT); // unaligned unpack
  result->m_sampledKeysX = (uint16_t*) resource.alignAndIncrement(memReqsData);
  result->m_sampledKeysY = (uint16_t*) resource.alignAndIncrement(memReqsData);
  result->m_sampledKeysZ = (uint16_t*) resource.alignAndIncrement(memReqsData);
  
  // Make sure size is a multiple of the alignment requirement.
  resource.align(NMP_VECTOR_ALIGNMENT);
  
  return result;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferQuat::encode(const NMP::Quat* dataBuffer, NMP::Vector3* tempBuffer)
{
  NMP_ASSERT(dataBuffer && tempBuffer);
  
  // Convert the quaternions into tan quarter angle rotation vectors
  for (uint32_t i = 0; i < m_length; ++i)
  {
    tempBuffer[i] = dataBuffer[i].toRotationVector(true);
  }
  
  // Compute the quantisation bounds
  NMP::Vector3 qMin, qMax;
  vBounds(m_length, tempBuffer, qMin, qMax);
  
  // Set the quantisation scale and offset
  const float step16 = 1.0f / 0xFFFF;
  m_scales.set(
    (qMax.x - qMin.x) * step16,
    (qMax.y - qMin.y) * step16,
    (qMax.z - qMin.z) * step16);
  m_offsets = qMin;

  // Calculate the quantisation factors such that:
  // q = (int)( (x - xmin) * ((1 << p - 1) / (xmax - xmin)) + 0.5 )
  float kx = 0xFFFF / (qMax.x - qMin.x);
  float tx = 0.5f - qMin.x * kx;
  float ky = 0xFFFF / (qMax.y - qMin.y);
  float ty = 0.5f - qMin.y * ky;
  float kz = 0xFFFF / (qMax.z - qMin.z);
  float tz = 0.5f - qMin.z * kz;

  // Quantise the data samples 
  for (uint32_t i = 0; i < m_length; ++i)
  {
    m_sampledKeysX[i] = (uint16_t)(kx * tempBuffer[i].x + tx);
    m_sampledKeysY[i] = (uint16_t)(ky * tempBuffer[i].y + ty);
    m_sampledKeysZ[i] = (uint16_t)(kz * tempBuffer[i].z + tz);
  }

  // Pad the remaining data
  uint32_t length4 = (m_length + 3) & (~0x03);
  for (uint32_t i = m_length; i < length4; ++i)
  {
    m_sampledKeysX[i] = 0;
    m_sampledKeysY[i] = 0;
    m_sampledKeysZ[i] = 0;
  }
}

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferQuat::decode(NMP::Quat* dataBuffer) const
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
    float* qOut = (float*)&dataBuffer[indx];
    NMP::vpu::prefetchCacheLine(qOut, 0);
    NMP::vpu::prefetchCacheLine(qOut, NMP_VPU_CACHESIZE);
    
    // Dequantise the tan quarter angle rotation vectors in SOA format
    NMP::vpu::Vector3MP tqa4;
    tqa4.x = NMP::vpu::unpacku4i16(&m_sampledKeysX[indx]);
    tqa4.y = NMP::vpu::unpacku4i16(&m_sampledKeysY[indx]);
    tqa4.z = NMP::vpu::unpacku4i16(&m_sampledKeysZ[indx]);
    tqa4.convert2f();
    tqa4 = tqa4 * qScales4 + qOffsets4;
    
    // Convert the tan quarter angle rotation vectors to quats in SOA format
    NMP::vpu::QuatMP quat4 = tqa4.convertTQARotVecToQuat();
    
    // Unpack quat data into AOS format and store in the output buffer
    quat4.unpack(vecX, vecY, vecZ, vecW);
    NMP::vpu::store4v(qOut, vecX, vecY, vecZ, vecW);
  }
}

#else
//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferQuat::decode(NMP::Quat* dataBuffer) const
{
  NMP_ASSERT(dataBuffer);

  for (uint32_t i = 0; i < m_length; ++i)
  {
    // Dequantise the tan quarter angle rotation vector
    NMP::Vector3 v((float)m_sampledKeysX[i], (float)m_sampledKeysY[i], (float)m_sampledKeysZ[i]);
    v = v * m_scales + m_offsets;

    // Convert the tan quarter angle rotation vector to a quaternion
    float mag2 = v.magnitudeSquared();
    float opm2 = 1 + mag2;
    float fac = 2 / opm2;
    v *= fac;
    dataBuffer[i].setXYZW(v.x, v.y, v.z, (1 - mag2) / opm2);
  }
}
#endif

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferQuat::decodeMP(NMP::Quat* dataBuffer) const
{
  NMP_ASSERT(dataBuffer);

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
    float* qOut = (float*)&dataBuffer[indx];
    NMP::vpu::prefetchCacheLine(qOut, 0);
    NMP::vpu::prefetchCacheLine(qOut, NMP_VPU_CACHESIZE);

    // Dequantise the tan quarter angle rotation vectors in SOA format
    NMP::vpu::Vector3MP tqa4;
    tqa4.x = NMP::vpu::unpacku4i16(&m_sampledKeysX[indx]);
    tqa4.y = NMP::vpu::unpacku4i16(&m_sampledKeysY[indx]);
    tqa4.z = NMP::vpu::unpacku4i16(&m_sampledKeysZ[indx]);
    tqa4.convert2f();
    tqa4 = tqa4 * qScales4 + qOffsets4;

    // Convert the tan quarter angle rotation vectors to quats in SOA format
    NMP::vpu::QuatMP quat4 = tqa4.convertTQARotVecToQuat();
    
    // Store the packed data in the output buffer
    NMP::vpu::store4v(qOut, quat4.x, quat4.y, quat4.z, quat4.w);
  }
}

#else
//----------------------------------------------------------------------------------------------------------------------
void CompressedDataBufferQuat::decodeMP(NMP::Quat* dataBuffer) const
{
  (void)dataBuffer;
  NMP_ASSERT_FAIL(); // No multiplexed implementation for non SIMD
}

#endif // NMP_PLATFORM_SIMD

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
