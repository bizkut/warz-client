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
#include "morpheme/AnimSource/mrTrajectorySourceQSA.h"
#include "morpheme/AnimSource/mrAnimSourceUtils.h"

#include "NMPlatform/NMvpu.h"
#include "NMPlatform/NMProfiler.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// TrajectoryPosKeyQSA
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void TrajectoryPosKeyQSA::locate()
{
  NMP::endianSwap(m_data);
}

void TrajectoryPosKeyQSA::dislocate()
{
  NMP::endianSwap(m_data);
}

//----------------------------------------------------------------------------------------------------------------------
// TrajectoryQuatKeyQSA
//----------------------------------------------------------------------------------------------------------------------
void TrajectoryQuatKeyQSA::locate()
{
  NMP::endianSwapArray(m_data, 3);
}

void TrajectoryQuatKeyQSA::dislocate()
{
  NMP::endianSwapArray(m_data, 3);
}

//----------------------------------------------------------------------------------------------------------------------
// TrajectoryKeyInfoQSA
//----------------------------------------------------------------------------------------------------------------------
void TrajectoryKeyInfoQSA::locate()
{
  NMP::endianSwapArray(m_scales, 3);
  NMP::endianSwapArray(m_offsets, 3);
}

void TrajectoryKeyInfoQSA::dislocate()
{
  NMP::endianSwapArray(m_offsets, 3);
  NMP::endianSwapArray(m_scales, 3);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceQSA functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void TrajectorySourceQSA::locate()
{
  // Verify alignment.
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Trajectory sources must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  // Base class
  TrajectorySourceBase::locate();
  m_getTrajAtTime = TrajectorySourceQSA::computeTrajectoryTransformAtTime;

  // Header
  NMP::endianSwap(m_sampleFrequency);
  NMP::endianSwap(m_numAnimFrames);
  NMP::endianSwap(m_flags);
  m_sampledDeltaPosKeysInfo.locate();
  m_sampledDeltaQuatKeysInfo.locate();

  // Pos
  if (m_sampledDeltaPosKeys)
  {
    REFIX_SWAP_PTR(TrajectoryPosKeyQSA, m_sampledDeltaPosKeys);
    for (uint16_t i = 0; i < m_numAnimFrames; ++i)
      m_sampledDeltaPosKeys[i].locate();
  }

  // Quat
  if (m_sampledDeltaQuatKeys)
  {
    // Account for extra frame (SIMD padding)
    REFIX_SWAP_PTR(TrajectoryQuatKeyQSA, m_sampledDeltaQuatKeys);
    for (uint16_t i = 0; i < m_numAnimFrames + 1; ++i)
      m_sampledDeltaQuatKeys[i].locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceQSA::dislocate()
{
  // Quat
  if (m_sampledDeltaQuatKeys)
  {
    // Account for extra frame (SIMD padding)
    for (uint16_t i = 0; i < m_numAnimFrames + 1; ++i)
      m_sampledDeltaQuatKeys[i].dislocate();
    UNFIX_SWAP_PTR(TrajectoryQuatKeyQSA, m_sampledDeltaQuatKeys);
  }

  // Pos
  if (m_sampledDeltaPosKeys)
  {
    for (uint16_t i = 0; i < m_numAnimFrames; ++i)
      m_sampledDeltaPosKeys[i].dislocate();
    UNFIX_SWAP_PTR(TrajectoryPosKeyQSA, m_sampledDeltaPosKeys);
  }

  // Header
  m_sampledDeltaQuatKeysInfo.dislocate();
  m_sampledDeltaPosKeysInfo.dislocate();
  NMP::endianSwap(m_flags);
  NMP::endianSwap(m_numAnimFrames);
  NMP::endianSwap(m_sampleFrequency);

  // Base class
  m_getTrajAtTime = NULL;
  TrajectorySourceBase::dislocate();
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceQSA::relocate()
{
  // Verify alignment
  NMP_ASSERT_MSG(
    NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT),
    "Trajectory sources must be aligned to %d bytes.",
    NMP_VECTOR_ALIGNMENT);

  // Header
  uint8_t* data = (uint8_t*)this;
  data += sizeof(TrajectorySourceQSA);
  m_getTrajAtTime = TrajectorySourceQSA::computeTrajectoryTransformAtTime;

  // Pos
  if (m_sampledDeltaPosKeys != NULL)
  {
    NMP::Memory::Format memReqsPos(sizeof(TrajectoryPosKeyQSA) * m_numAnimFrames, NMP_NATURAL_TYPE_ALIGNMENT);
    data = (uint8_t*) NMP::Memory::align(data, memReqsPos.alignment);
    m_sampledDeltaPosKeys = (TrajectoryPosKeyQSA*) data;
    data += memReqsPos.size;
  }

  // Quat
  if (m_sampledDeltaQuatKeys != NULL)
  {
    // Allocate an extra frame to pad SIMD optimizations
    NMP::Memory::Format memReqsQuat(sizeof(TrajectoryQuatKeyQSA) * (m_numAnimFrames + 1), NMP_NATURAL_TYPE_ALIGNMENT);
    data = (uint8_t*) NMP::Memory::align(data, memReqsQuat.alignment);
    m_sampledDeltaQuatKeys = (TrajectoryQuatKeyQSA*) data;
    data += memReqsQuat.size;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceQSA::unchangingPosFn(DecompressionArgs& args)
{
  NMP_ASSERT(args.source);
  const TrajectoryKeyInfoQSA& info = args.source->m_sampledDeltaPosKeysInfo;
  args.pOut->set(info.x, info.y, info.z);
}

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceQSA::unchangingQuatFn(DecompressionArgs& args)
{
  NMP_ASSERT(args.source);
  const TrajectoryKeyInfoQSA& info = args.source->m_sampledDeltaQuatKeysInfo;
  args.qOut->setXYZW(info.x, info.y, info.z, info.w);
}

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceQSA::sampledPosFn(DecompressionArgs& args)
{
  NMP_ASSERT(args.source);
  NMP_ASSERT(args.animFrameIndex < (uint32_t)args.source->m_numAnimFrames - 1); // don't interpolate beyond last frame
  NMP_ASSERT(args.interpolant >= 0.0f && args.interpolant <= 1.0f);
  NMP_ASSERT(args.source->m_sampledDeltaPosKeys);
  const TrajectoryKeyInfoQSA& deltaPosKeyInfo = args.source->m_sampledDeltaPosKeysInfo;

  NMP::vpu::vector4_t keyA, keyB;
  NMP::vpu::vector4_t alpha, result;
  NMP::vpu::vector4_t qScale, qOffset;

  // Unpack the two keys
  const uint32_t* posKeys = &args.source->m_sampledDeltaPosKeys[args.animFrameIndex].m_data;
  NMP::vpu::unpack2QuantizedVectors(keyA, keyB, posKeys);

  // Dequantise
  qScale = NMP::vpu::set4f(deltaPosKeyInfo.m_scales[0], deltaPosKeyInfo.m_scales[1], deltaPosKeyInfo.m_scales[2], 0.0f);
  qOffset = NMP::vpu::set4f(deltaPosKeyInfo.m_offsets[0], deltaPosKeyInfo.m_offsets[1], deltaPosKeyInfo.m_offsets[2], 0.0f);
  keyA = NMP::vpu::madd4f(qScale, keyA, qOffset);
  keyB = NMP::vpu::madd4f(qScale, keyB, qOffset);

  // Interpolate
  alpha = NMP::vpu::set4f(args.interpolant);
  result = NMP::vpu::lerpMP4(keyA, keyB, alpha);

  // Store the result
  NMP::vpu::store4f((float*)args.pOut, result);
}

#else
//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceQSA::sampledPosFn(DecompressionArgs& args)
{
  NMP_ASSERT(args.source);
  NMP_ASSERT(args.animFrameIndex < (uint32_t)args.source->m_numAnimFrames - 1); // don't interpolate beyond last frame
  NMP_ASSERT(args.interpolant >= 0.0f && args.interpolant <= 1.0f);
  NMP_ASSERT(args.source->m_sampledDeltaPosKeys);

  uint32_t v[3];
  NMP::Vector3 pa, pb;
  NMP::Vector3 qScale, qOffset;

  const TrajectoryKeyInfoQSA& deltaPosKeyInfo = args.source->m_sampledDeltaPosKeysInfo;
  qScale.set(deltaPosKeyInfo.m_scales[0], deltaPosKeyInfo.m_scales[1], deltaPosKeyInfo.m_scales[2]);
  qOffset.set(deltaPosKeyInfo.m_offsets[0], deltaPosKeyInfo.m_offsets[1], deltaPosKeyInfo.m_offsets[2]);

  // key A
  const TrajectoryPosKeyQSA& deltaPosKeyA = args.source->m_sampledDeltaPosKeys[args.animFrameIndex];
  MR::unpackQuantisedVector(deltaPosKeyA.m_data, v);
  MR::dequantise(qScale, qOffset, v, pa);

  // key B
  const TrajectoryPosKeyQSA& deltaPosKeyB = args.source->m_sampledDeltaPosKeys[args.animFrameIndex+1];
  MR::unpackQuantisedVector(deltaPosKeyB.m_data, v);
  MR::dequantise(qScale, qOffset, v, pb);

  // Interpolation
  args.pOut->lerp(pa, pb, args.interpolant);
}
#endif

#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceQSA::sampledQuatFn(DecompressionArgs& args)
{
  NMP_ASSERT(args.source);
  NMP_ASSERT(args.animFrameIndex < (uint32_t)args.source->m_numAnimFrames - 1); // don't interpolate beyond last frame
  NMP_ASSERT(args.interpolant >= 0.0f && args.interpolant <= 1.0f);
  NMP_ASSERT(args.source->m_sampledDeltaQuatKeys);

  const TrajectoryKeyInfoQSA& deltaQuatKeyInfo = args.source->m_sampledDeltaQuatKeysInfo;

  NMP::vpu::vector4_t keyA, keyB;
  NMP::vpu::vector4_t alpha, result;
  NMP::vpu::vector4_t qScale, qOffset;

  // Unpack the two keys
  const uint16_t* quatKeys = (const uint16_t*)&args.source->m_sampledDeltaQuatKeys[args.animFrameIndex];
  NMP::vpu::unpacku6i16f(keyA, keyB, quatKeys);

  // Dequantise
  qScale = NMP::vpu::set4f(deltaQuatKeyInfo.m_scales[0], deltaQuatKeyInfo.m_scales[1], deltaQuatKeyInfo.m_scales[2], 0.0f);
  qOffset = NMP::vpu::set4f(deltaQuatKeyInfo.m_offsets[0], deltaQuatKeyInfo.m_offsets[1], deltaQuatKeyInfo.m_offsets[2], 0.0f);
  keyA = NMP::vpu::madd4f(qScale, keyA, qOffset);
  keyB = NMP::vpu::madd4f(qScale, keyB, qOffset);

  // Convert tan quarter angle rotation vector to quat
  keyA = NMP::vpu::convertTQARV2Quat(keyA);
  keyB = NMP::vpu::convertTQARV2Quat(keyB);

  // Interpolate
  alpha = NMP::vpu::set4f(args.interpolant);
  result = NMP::vpu::interpBlendQuats(keyA, keyB, alpha);

  // Store the result
  NMP::vpu::store4f((float*)args.qOut, result);
}

#else
//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceQSA::sampledQuatFn(DecompressionArgs& args)
{
  NMP_ASSERT(args.source);
  NMP_ASSERT(args.animFrameIndex < (uint32_t)args.source->m_numAnimFrames - 1); // don't interpolate beyond last frame
  NMP_ASSERT(args.interpolant >= 0.0f && args.interpolant <= 1.0f);
  NMP_ASSERT(args.source->m_sampledDeltaQuatKeys);

  NMP::Vector3 qScale, qOffset, v;
  NMP::Quat qa, qb;
  float fromDotTo;

  const TrajectoryKeyInfoQSA& deltaQuatKeyInfo = args.source->m_sampledDeltaQuatKeysInfo;
  qScale.set(deltaQuatKeyInfo.m_scales[0], deltaQuatKeyInfo.m_scales[1], deltaQuatKeyInfo.m_scales[2]);
  qOffset.set(deltaQuatKeyInfo.m_offsets[0], deltaQuatKeyInfo.m_offsets[1], deltaQuatKeyInfo.m_offsets[2]);

  // Key A
  const TrajectoryQuatKeyQSA& deltaQuatKeyA = args.source->m_sampledDeltaQuatKeys[args.animFrameIndex];
  MR::dequantise(qScale, qOffset, deltaQuatKeyA.m_data, v);
  MR::fromRotationVectorTQA(v, qa);

  // Key B
  const TrajectoryQuatKeyQSA& deltaQuatKeyB = args.source->m_sampledDeltaQuatKeys[args.animFrameIndex+1];
  MR::dequantise(qScale, qOffset, deltaQuatKeyB.m_data, v);
  MR::fromRotationVectorTQA(v, qb);

  // Interpolation
  fromDotTo = qa.dot(qb);
  if (fromDotTo < 0.0f)
  {
    qb.negate();
    fromDotTo = -fromDotTo;
  }
  args.qOut->fastSlerp(qa, qb, args.interpolant, fromDotTo);
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void TrajectorySourceQSA::computeTrajectoryTransformAtTime(
  const TrajectorySourceBase* trajectoryControl,
  float                       time,
  NMP::Quat&                  resultQuat,
  NMP::Vector3&               resultPos)
{
  NMP_ASSERT(trajectoryControl);
  TrajectorySourceQSA* compressedSource = (TrajectorySourceQSA*)trajectoryControl;

  // Initialise the decompression args
  DecompressionArgs decompArgs;
  decompArgs.source = compressedSource;
  decompArgs.pOut = &resultPos;
  decompArgs.qOut = &resultQuat;

  static DecompressFn posFnTable[2] =
  {
    unchangingPosFn,
    sampledPosFn
  };

  static DecompressFn quatFnTable[2] =
  {
    unchangingQuatFn,
    sampledQuatFn
  };

  // Calculate frame index and interpolant.
  float frame = time * compressedSource->m_sampleFrequency;
  decompArgs.animFrameIndex = (uint32_t)frame;
  NMP_ASSERT(decompArgs.animFrameIndex < compressedSource->m_numAnimFrames);
  decompArgs.interpolant = frame - decompArgs.animFrameIndex;

  // Special case for the last frame. mustn't interpolate beyond the last frame
  uint32_t lastAnimFrame = compressedSource->m_numAnimFrames - 1;
  if (decompArgs.animFrameIndex == lastAnimFrame)
  {
    decompArgs.animFrameIndex--;
    decompArgs.interpolant = 1.0f;
  }

  // Decompress the delta pos channel data
  uint16_t chanPosMethod = compressedSource->getCompChanMethodForDeltaPosChan();
  NMP_ASSERT(chanPosMethod <= 1);
  posFnTable[chanPosMethod](decompArgs);

  // Decompress the delta quat channel data
  uint16_t chanQuatMethod = compressedSource->getCompChanMethodForDeltaQuatChan();
  NMP_ASSERT(chanQuatMethod <= 1);
  quatFnTable[chanQuatMethod](decompArgs);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
