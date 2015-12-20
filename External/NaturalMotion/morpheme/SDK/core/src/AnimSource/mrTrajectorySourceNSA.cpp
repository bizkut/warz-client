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
#include "NMPlatform/NMMemory.h"
#include "morpheme/AnimSource/mrTrajectorySourceNSA.h"
#include "morpheme/AnimSource/mrAnimSourceUtils.h"

#include "NMPlatform/NMvpu.h"
#include "NMPlatform/NMProfiler.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceNSA functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void TrajectorySourceNSA::locate()
{
  // Verify alignment.
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Trajectory sources must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  // Base class
  TrajectorySourceBase::locate();
  m_getTrajAtTime = TrajectorySourceNSA::computeTrajectoryTransformAtTime;
  
  // Header
  NMP::endianSwap(m_sampleFrequency);
  NMP::endianSwap(m_numAnimFrames);
  m_sampledDeltaPosKeysInfo.locate();
  m_sampledDeltaQuatKeysInfo.locate();
  
  // Pos
  if (m_sampledDeltaPosKeys)
  {
    REFIX_SWAP_PTR(SampledPosKey, m_sampledDeltaPosKeys);
    for (uint32_t i = 0; i < m_numAnimFrames; ++i)
      m_sampledDeltaPosKeys[i].locate();
  }

  // Quat
  if (m_sampledDeltaQuatKeys)
  {
    REFIX_SWAP_PTR(SampledQuatKeyTQA, m_sampledDeltaQuatKeys);
    for (uint32_t i = 0; i < m_numAnimFrames; ++i)
      m_sampledDeltaQuatKeys[i].locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceNSA::dislocate()
{
  // Quat
  if (m_sampledDeltaQuatKeys)
  {
    for (uint32_t i = 0; i < m_numAnimFrames; ++i)
      m_sampledDeltaQuatKeys[i].dislocate();
    UNFIX_SWAP_PTR(SampledQuatKeyTQA, m_sampledDeltaQuatKeys);
  }
  
  // Pos
  if (m_sampledDeltaPosKeys)
  {
    for (uint32_t i = 0; i < m_numAnimFrames; ++i)
      m_sampledDeltaPosKeys[i].dislocate();
    UNFIX_SWAP_PTR(SampledPosKey, m_sampledDeltaPosKeys);
  }
  
  // Header
  m_sampledDeltaQuatKeysInfo.dislocate();
  m_sampledDeltaPosKeysInfo.dislocate();
  NMP::endianSwap(m_numAnimFrames);
  NMP::endianSwap(m_sampleFrequency);
  
  // Base class
  m_getTrajAtTime = NULL;
  TrajectorySourceBase::dislocate();
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceNSA::relocate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Trajectory sources must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  // Header
  uint8_t* data = (uint8_t*)this;
  data += sizeof(TrajectorySourceNSA);
  m_getTrajAtTime = TrajectorySourceNSA::computeTrajectoryTransformAtTime;
  
  // Pos
  if (m_sampledDeltaPosKeys)
  {
    NMP::Memory::Format memReqsPos(sizeof(SampledPosKey) * m_numAnimFrames, NMP_NATURAL_TYPE_ALIGNMENT);
    data = (uint8_t*) NMP::Memory::align(data, memReqsPos.alignment);
    m_sampledDeltaPosKeys = (SampledPosKey*) data;
    data += memReqsPos.size;
  }
  
  // Quat
  if (m_sampledDeltaQuatKeys)
  {
    NMP::Memory::Format memReqsQuat(sizeof(SampledQuatKeyTQA) * m_numAnimFrames, NMP_NATURAL_TYPE_ALIGNMENT);
    data = (uint8_t*) NMP::Memory::align(data, memReqsQuat.alignment);
    m_sampledDeltaQuatKeys = (SampledQuatKeyTQA*) data;
    data += memReqsQuat.size;
  }
}

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceNSA::sampledDeltaPosDecompress(
  uint32_t animFrameIndex,
  float interpolant,
  NMP::Vector3& pOut) const
{
  NMP_ASSERT(animFrameIndex < m_numAnimFrames - 1); // don't interpolate beyond last frame
  NMP_ASSERT(interpolant >= 0.0f && interpolant <= 1.0f);
  NMP_ASSERT(m_sampledDeltaPosKeys);

  // Unpack the two keys
  NMP::vpu::vector4_t keyA, keyB;
  const uint32_t* posKeys = (const uint32_t*) &(m_sampledDeltaPosKeys[animFrameIndex]);
  NMP::vpu::unpack2QuantizedVectors(keyA, keyB, posKeys);

  // Dequantise
  NMP::Vector3 qScale_, qOffset_;
  m_sampledDeltaPosKeysInfo.unpack(qScale_, qOffset_);
  NMP::vpu::vector4_t qOffset = NMP::vpu::load4f(qOffset_.getPtr());
  NMP::vpu::vector4_t qScale = NMP::vpu::load4f(qScale_.getPtr());
  
  keyA = NMP::vpu::madd4f(qScale, keyA, qOffset);
  keyB = NMP::vpu::madd4f(qScale, keyB, qOffset);

  // Interpolate
  NMP::vpu::vector4_t alpha = NMP::vpu::set4f(interpolant);
  NMP::vpu::vector4_t result = NMP::vpu::lerpMP4(keyA, keyB, alpha);

  // Store the result
  NMP::vpu::store4f((float*)&pOut, result);
}

#else
//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceNSA::sampledDeltaPosDecompress(
  uint32_t animFrameIndex,
  float interpolant,
  NMP::Vector3& pOut) const
{
  NMP_ASSERT(animFrameIndex < m_numAnimFrames - 1); // don't interpolate beyond last frame
  NMP_ASSERT(interpolant >= 0.0f && interpolant <= 1.0f);
  NMP_ASSERT(m_sampledDeltaPosKeys);

  NMP::Vector3 qScale, qOffset;
  m_sampledDeltaPosKeysInfo.unpack(qScale, qOffset);

  NMP::Vector3 vecA, vecB;
  m_sampledDeltaPosKeys[animFrameIndex].unpack(vecA);
  m_sampledDeltaPosKeys[animFrameIndex + 1].unpack(vecB);

  NMP::Vector3 pos;
  pos.lerp(vecA, vecB, interpolant);

  pOut = pos * qScale + qOffset;
}
#endif

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceNSA::sampledDeltaQuatDecompress(
  uint32_t animFrameIndex,
  float interpolant,
  NMP::Quat& qOut) const
{
  NMP_ASSERT(animFrameIndex < m_numAnimFrames - 1); // don't interpolate beyond last frame
  NMP_ASSERT(interpolant >= 0.0f && interpolant <= 1.0f);
  NMP_ASSERT(m_sampledDeltaQuatKeys);

  // Unpack the two keys
  NMP::vpu::vector4_t keyA, keyB;
  const uint16_t* quatKeys = (const uint16_t*) &(m_sampledDeltaQuatKeys[animFrameIndex]);
  NMP::vpu::unpacku6i16f(keyA, keyB, quatKeys);
  
  // Dequantise
  NMP::Vector3 qScale_, qOffset_;
  m_sampledDeltaQuatKeysInfo.unpack(qScale_, qOffset_);
  NMP::vpu::vector4_t qOffset = NMP::vpu::load4f(qOffset_.getPtr());
  NMP::vpu::vector4_t qScale = NMP::vpu::load4f(qScale_.getPtr());

  keyA = NMP::vpu::madd4f(qScale, keyA, qOffset);
  keyB = NMP::vpu::madd4f(qScale, keyB, qOffset);

  // Convert tan quarter angle rotation vector to quat
  keyA = NMP::vpu::convertTQARV2Quat(keyA);
  keyB = NMP::vpu::convertTQARV2Quat(keyB);

  // Interpolate
  NMP::vpu::vector4_t alpha = NMP::vpu::set4f(interpolant);
  NMP::vpu::vector4_t result = NMP::vpu::interpBlendQuats(keyA, keyB, alpha);

  // Store the result
  NMP::vpu::store4f((float*)&qOut, result);
}

#else
//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceNSA::sampledDeltaQuatDecompress(
  uint32_t animFrameIndex,
  float interpolant,
  NMP::Quat& qOut) const
{
  NMP_ASSERT(animFrameIndex < m_numAnimFrames - 1); // don't interpolate beyond last frame
  NMP_ASSERT(interpolant >= 0.0f && interpolant <= 1.0f);
  NMP_ASSERT(m_sampledDeltaQuatKeys);

  NMP::Vector3 qScale, qOffset;
  m_sampledDeltaQuatKeysInfo.unpack(qScale, qOffset);

  // Unpack
  NMP::Vector3 vecA, vecB;
  m_sampledDeltaQuatKeys[animFrameIndex].unpack(vecA);
  m_sampledDeltaQuatKeys[animFrameIndex + 1].unpack(vecB);
  
  // Dequantise
  vecA = vecA * qScale + qOffset;
  vecB = vecB * qScale + qOffset;
  
  // Convert tan quarter angle rotation vector into a quaternion
  NMP::Quat quatA, quatB;
  MR::fromRotationVectorTQA(vecA, quatA);
  MR::fromRotationVectorTQA(vecB, quatB);
  
  // Interpolation
  float fromDotTo = quatA.dot(quatB);
  float dotSign = NMP::floatSelect(fromDotTo, 1.0f, -1.0f);
  quatB *= dotSign;
  fromDotTo *= dotSign;
  qOut.fastSlerp(quatA, quatB, interpolant, fromDotTo);
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceNSA::computeTrajectoryTransformAtTime(
  const TrajectorySourceBase* trajectoryControl,
  float                       time,
  NMP::Quat&                  resultQuat,
  NMP::Vector3&               resultPos)
{
  NMP_ASSERT(trajectoryControl);
  const TrajectorySourceNSA* compressedSource = static_cast<const TrajectorySourceNSA*> (trajectoryControl);

  // Calculate frame index and interpolant.
  float frame = time * compressedSource->m_sampleFrequency;
  uint32_t animFrameIndex = (uint32_t)frame;
  NMP_ASSERT(animFrameIndex < (uint32_t)compressedSource->m_numAnimFrames);
  float interpolant = frame - animFrameIndex;

  //-----------------------
  // Special case for the last frame. mustn't interpolate beyond the last frame
  uint32_t lastAnimFrame = (uint32_t)compressedSource->m_numAnimFrames - 1;
  if (animFrameIndex == lastAnimFrame)
  {
    animFrameIndex--;
    interpolant = 1.0f;
  }
  
  //-----------------------
  // Pos
  if (compressedSource->m_sampledDeltaPosKeys)
  {
    compressedSource->sampledDeltaPosDecompress(animFrameIndex, interpolant, resultPos);
  }
  else
  {
    // Set the unchanging position
    compressedSource->m_sampledDeltaPosKeysInfo.unpack4(resultPos.getPtr());
  }
  
  //-----------------------
  // Quat
  if (compressedSource->m_sampledDeltaQuatKeys)
  {
    compressedSource->sampledDeltaQuatDecompress(animFrameIndex, interpolant, resultQuat);
  }
  else
  {
    // Set the unchanging orientation
    compressedSource->m_sampledDeltaQuatKeysInfo.unpack4(resultQuat.getPtr());
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
