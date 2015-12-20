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
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/Nodes/mrNodeClosestAnim.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/mrBlendOps.h"
#include "NMPlatform/NMProfiler.h"
#include "NMPlatform/NMvpu.h"
//----------------------------------------------------------------------------------------------------------------------

// non-public template definitions
#include "../mrTransitDeadBlend.inl"

// To enable animation decompression profiling output.
#define NM_CLOSEST_ANIM_PROFILINGx
#if defined(NM_CLOSEST_ANIM_PROFILING)
#define NM_CLOSEST_ANIM_BEGIN_PROFILING(name)    NM_BEGIN_PROFILING(name)
#define NM_CLOSEST_ANIM_END_PROFILING()          NM_END_PROFILING()
#else // NM_CLOSEST_ANIM_PROFILING
#define NM_CLOSEST_ANIM_BEGIN_PROFILING(name)
#define NM_CLOSEST_ANIM_END_PROFILING()
#endif // NM_CLOSEST_ANIM_PROFILING

// For logging information about cost scores
#define TEST_CLOSEST_ANIM_LOGGINGx

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
class ClosestAnimResult
{
public:
  // Root rotation
  NMP::Quat       m_activeChildRootQuat;

  // Best matching source node index
  uint32_t        m_activeChildIndex;
};

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
// Common matching functions
//----------------------------------------------------------------------------------------------------------------------
void accumulateMatchingSpaceTransforms(
  const AnimRigDef*                 rig,                    ///< Animation rig for transforms accumulation
  AttribDataClosestAnimDef*         setupData,              ///< Non anim set specific setup data
  AttribDataClosestAnimDefAnimSet*  setupDataAnimSet,       ///< Anim set specific setup data
  const NMP::DataBuffer*            inputTransforms,        ///< Input transforms (local space)
  NMP::DataBuffer*                  inputTransformsAcc,     ///< Temporary data buffer for accumulation
  bool*                             validRigChannelFlags,   ///< Temporary channel flags for partial accumulation
  NMP::Vector3*                     outputChannelPos,       ///< Output position channel (matching space)
  NMP::Quat*                        outputChannelAtt)       ///< Output orientation channel (matching space)
{
  uint32_t numAnimJointsPadded = (setupData->m_numAnimJoints + 3) & (~0x03); // Padded to block of four
  NMP::Vector3* inputTransformsAccPos = inputTransformsAcc->getPosQuatChannelPos(0);
  NMP::Quat* inputTransformsAccAtt = inputTransformsAcc->getPosQuatChannelQuat(0);

  MR::BlendOpsBase::accumulateTransformsPartial(
    NMP::Vector3(NMP::Vector3::InitZero),
    setupData->m_upAlignAtt,
    inputTransforms,
    rig,
    setupDataAnimSet->m_numEntries,
    setupDataAnimSet->m_rigChannels,
    validRigChannelFlags,
    inputTransformsAccPos,
    inputTransformsAccAtt);

  // Initialise the comparison buffer
  NMP::Vector3 vZero(NMP::Vector3::InitZero);
  NMP::Quat qIdentity(NMP::Quat::kIdentity);
  for (uint32_t i = 0; i < numAnimJointsPadded; ++i)
  {
    outputChannelPos[i] = vZero;
    outputChannelAtt[i] = qIdentity;
  }

  // Recover the comparison buffer (Network output transforms)
  for (uint32_t entryIndex = 0; entryIndex < setupDataAnimSet->m_numEntries; ++entryIndex)
  {
    uint32_t rigChannelIndex = setupDataAnimSet->m_rigChannels[entryIndex];
    uint32_t animChannelIndex = setupDataAnimSet->m_animChannels[entryIndex];
    outputChannelPos[animChannelIndex] = inputTransformsAccPos[rigChannelIndex];
    outputChannelAtt[animChannelIndex] = inputTransformsAccAtt[rigChannelIndex];
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Offline pre-computed source matching (SIMD optimised)
//----------------------------------------------------------------------------------------------------------------------
#ifdef NMP_PLATFORM_SIMD  
//----------------------------------------------------------------------------------------------------------------------
void convertTransformsMP(
  uint32_t length,
  NMP::Vector3* channelPos,
  NMP::Quat* channelAtt)
{
  // Position
  for (uint32_t indx = 0; indx < length; indx += 4)
  {
    float* pOut = (float*)&channelPos[indx];
    NMP::vpu::prefetchCacheLine(pOut, 0);
    NMP::vpu::prefetchCacheLine(pOut, NMP_VPU_CACHESIZE);
    
    NMP::vpu::vector4_t vecX, vecY, vecZ, vecW;
    NMP::vpu::load4v(pOut, vecX, vecY, vecZ, vecW);
    NMP::vpu::transpose4v(vecX, vecY, vecZ, vecW);
    NMP::vpu::store4v(pOut, vecX, vecY, vecZ, vecW);
  }
  
  // Orientation
  for (uint32_t indx = 0; indx < length; indx += 4)
  {
    float* qOut = (float*)&channelAtt[indx];
    NMP::vpu::prefetchCacheLine(qOut, 0);
    NMP::vpu::prefetchCacheLine(qOut, NMP_VPU_CACHESIZE);

    NMP::vpu::vector4_t vecX, vecY, vecZ, vecW;
    NMP::vpu::load4v(qOut, vecX, vecY, vecZ, vecW);
    NMP::vpu::transpose4v(vecX, vecY, vecZ, vecW);
    NMP::vpu::store4v(qOut, vecX, vecY, vecZ, vecW);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void computeNetworkMatchingSpaceTransforms(
  const AnimRigDef*                 rig,
  AttribDataClosestAnimDef*         setupData,
  AttribDataClosestAnimDefAnimSet*  setupDataAnimSet,
  const NMP::DataBuffer*            inputTransforms,
  NMP::DataBuffer*                  inputTransformsAcc,
  bool*                             validRigChannelFlags,
  NMP::Vector3*                     outputChannelPos,
  NMP::Quat*                        outputChannelAtt)
{
  // Accumulate the matching space transforms
  accumulateMatchingSpaceTransforms(
    rig,
    setupData,
    setupDataAnimSet,
    inputTransforms,
    inputTransformsAcc,
    validRigChannelFlags,
    outputChannelPos,
    outputChannelAtt);

  // Transpose into SOA block-4 format
  convertTransformsMP(
    setupData->m_numAnimJoints,
    outputChannelPos,
    outputChannelAtt);
}

//----------------------------------------------------------------------------------------------------------------------
void computeSourceTransformsFromVelocity(
  float deltaTime,
  uint32_t numEntries,
  const NMP::Vector3* srcDecompBufferPos0,
  const NMP::Quat* srcDecompBufferAtt0,
  const NMP::Vector3* srcDecompBufferPosVel,
  const NMP::Vector3* srcDecompBufferAttVel,
  NMP::Vector3* srcDecompBufferPos1,
  NMP::Quat* srcDecompBufferAtt1)
{
  NMP::vpu::vector4_t vdt = NMP::vpu::load1fp(&deltaTime);
  NMP::vpu::vector4_t vZero = NMP::vpu::zero4f();
  NMP::vpu::vector4_t vdt_half = NMP::vpu::mul4f(vdt, NMP::vpu::half4f());

  // Position
  for (uint32_t indx = 0; indx < numEntries; indx += 4)
  {
    const float* pIn = (const float*)&srcDecompBufferPos0[indx];
    const float* dpIn = (const float*)&srcDecompBufferPosVel[indx];
    float* pOut = (float*)&srcDecompBufferPos1[indx];
    NMP::vpu::prefetchCacheLine(pOut, 0);
    NMP::vpu::prefetchCacheLine(pOut, NMP_VPU_CACHESIZE);
    NMP::vpu::Vector3MP vp(pIn);
    NMP::vpu::Vector3MP vdp(dpIn);
    vdp *= vdt;
    vp += vdp;
    NMP::vpu::store4v(pOut, vp.x, vp.y, vp.z, vZero);
  }

  // Orientation
  for (uint32_t indx = 0; indx < numEntries; indx += 4)
  {
    const float* qIn = (const float*)&srcDecompBufferAtt0[indx];
    const float* dqIn = (const float*)&srcDecompBufferAttVel[indx];
    float* qOut = (float*)&srcDecompBufferAtt1[indx];
    NMP::vpu::prefetchCacheLine(qOut, 0);
    NMP::vpu::prefetchCacheLine(qOut, NMP_VPU_CACHESIZE);
    NMP::vpu::QuatMP vq(qIn);
    NMP::vpu::Vector3MP vdq(dqIn);
    NMP::vpu::QuatMP vqdt(vdq, vZero);
    vqdt = (vqdt * vq) * vdt_half;
    vq += vqdt;
    vq.normalise();
    NMP::vpu::store4v(qOut, vq.x, vq.y, vq.z, vq.w);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void computeOfflineSourceMatchingSpaceTransforms(
  uint32_t childIndex,
  AttribDataClosestAnimDef* setupData,
  NMP::Vector3** srcCompBufferPosArray,
  NMP::Quat** srcDecompBufferAttArray,
  bool useVelocity,
  float deltaTime,
  NMP::Vector3* srcDecompBufferPosVel,
  NMP::Vector3* srcDecompBufferAttVel)
{
  const AttribDataClosestAnimDef::ClosestAnimSourceData* animSourceData = setupData->getAnimSourceData(childIndex);
  NMP_ASSERT(animSourceData);    
  // Position
  NMP_ASSERT(animSourceData->m_sourceTransformsPos);
  animSourceData->m_sourceTransformsPos->decodeMP(srcCompBufferPosArray[0]);
  // Orientation
  NMP_ASSERT(animSourceData->m_sourceTransformsAtt);
  animSourceData->m_sourceTransformsAtt->decodeMP(srcDecompBufferAttArray[0]);

  if (useVelocity)
  {
    // Positional velocity
    NMP_ASSERT(animSourceData->m_sourceTransformsPosVel);
    animSourceData->m_sourceTransformsPosVel->decodeMP(srcDecompBufferPosVel);
    // Angular velocity
    NMP_ASSERT(animSourceData->m_sourceTransformsAngVel);
    animSourceData->m_sourceTransformsAngVel->decodeMP(srcDecompBufferAttVel);

    computeSourceTransformsFromVelocity(
      deltaTime,
      setupData->m_numAnimJoints,
      srcCompBufferPosArray[0],
      srcDecompBufferAttArray[0],
      srcDecompBufferPosVel,
      srcDecompBufferAttVel,
      srcCompBufferPosArray[1],
      srcDecompBufferAttArray[1]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void computeRootOffsetRotation(
  uint32_t numSets,
  uint32_t numEntries,
  const float* weights,
  const NMP::Vector3** netCompBufferPosArray,
  const NMP::Vector3** srcCompBufferPosArray,
  NMP::Quat& rootOffsetAtt)
{  
  NMP::vpu::vector4_t vD = NMP::vpu::zero4f();
  NMP::vpu::vector4_t vb0 = NMP::vpu::zero4f();
  NMP::vpu::vector4_t vb1 = NMP::vpu::zero4f();
  
  //---------------------------
  // Set up the linear system of equations
  for (uint32_t iSet = 0; iSet < numSets; ++iSet)
  {
    const NMP::Vector3* netCompBufferPos = netCompBufferPosArray[iSet];
    NMP_ASSERT(netCompBufferPos);
    const NMP::Vector3* srcCompBufferPos = srcCompBufferPosArray[iSet];
    NMP_ASSERT(srcCompBufferPos);

    for (uint32_t indx = 0; indx < numEntries; indx += 4)
    {
      // SOA packed position data
      NMP::vpu::vector4_t w = NMP::vpu::load4f(&weights[indx]);
      NMP::vpu::vector4_t netX = NMP::vpu::load4f((float*) &netCompBufferPos[indx]);
      NMP::vpu::vector4_t netZ = NMP::vpu::load4f((float*) &netCompBufferPos[indx+2]);
      NMP::vpu::vector4_t srcX = NMP::vpu::load4f((float*) &srcCompBufferPos[indx]);
      NMP::vpu::vector4_t srcZ = NMP::vpu::load4f((float*) &srcCompBufferPos[indx+2]);

      // D += w * (posSrc.x * posSrc.x + posSrc.z * posSrc.z);
      NMP::vpu::vector4_t t;
      t = NMP::vpu::mul4f(w, NMP::vpu::add4f(NMP::vpu::mul4f(srcX, srcX), NMP::vpu::mul4f(srcZ, srcZ)));
      vD = NMP::vpu::add4f(vD, t);

      // b[0] += w * (posSrc.x * posNet.x + posSrc.z * posNet.z);
      t = NMP::vpu::mul4f(w, NMP::vpu::add4f(NMP::vpu::mul4f(srcX, netX), NMP::vpu::mul4f(srcZ, netZ)));
      vb0 = NMP::vpu::add4f(vb0, t);

      // b[1] += w * (posSrc.x * posNet.z - posSrc.z * posNet.x);      
      t = NMP::vpu::mul4f(w, NMP::vpu::sub4f(NMP::vpu::mul4f(srcX, netZ), NMP::vpu::mul4f(srcZ, netX)));
      vb1 = NMP::vpu::add4f(vb1, t);
    }
  }

  // Extract the summations
  float D, b[2];
  vD = NMP::vpu::sum4f(vD);
  vb0 = NMP::vpu::sum4f(vb0);
  vb1 = NMP::vpu::sum4f(vb1);
  D = NMP::vpu::floatX(vD);
  b[0] = NMP::vpu::floatX(vb0);
  b[1] = NMP::vpu::floatX(vb1);

  //---------------------------
  // Solve the linear system
  float c, s;
  if (D > 1e-7f)
  {
    c = b[0] / D;
    s = b[1] / D;
  }
  else
  {
    c = s = 0.0f;
  }

  // Enforce the unit norm constraint
  float mag = sqrtf(c * c + s * s);
  if (mag > 1e-7f)
  {
    c /= mag;
    s /= mag;
  }
  else
  {
    c = 1.0f;
    s = 0.0f;
  }

  // Matrix to quat for rotation about Y axis  
  float qy, qw;
  float t = 2 * c + 1; // trace
  if (t > 0.0f)
  {
    qw = 0.5f * sqrtf(t + 1);
    qy = -s / (2 * qw);
  }
  else
  {
    qy = 0.5f * sqrtf(3 - t);
    qw = -s / (2 * qy);
  }
  
  rootOffsetAtt.setXYZW(0.0f, qy, 0.0f, qw);
}

//----------------------------------------------------------------------------------------------------------------------
void applyRootOffsetTransform(
  const NMP::Quat& rootOffsetAtt,
  uint32_t numEntries,
  NMP::Vector3* channelPos,
  NMP::Quat* channelAtt)
{
  NMP::vpu::QuatMP rootOffsetAtt4(NMP::vpu::load4f((float*) &rootOffsetAtt));
  NMP::vpu::vector4_t vZero = NMP::vpu::zero4f();

  // Position
  for (uint32_t indx = 0; indx < numEntries; indx += 4)
  {
    float* pOut = (float*) &channelPos[indx];
    NMP::vpu::prefetchCacheLine(pOut, 0);
    NMP::vpu::prefetchCacheLine(pOut, NMP_VPU_CACHESIZE);
    NMP::vpu::Vector3MP vp(pOut);
    vp = rootOffsetAtt4.rotateVector(vp);
    NMP::vpu::store4v(pOut, vp.x, vp.y, vp.z, vZero);
  }

  // Orientation
  for (uint32_t indx = 0; indx < numEntries; indx += 4)
  {
    float* qOut = (float*) &channelAtt[indx];
    NMP::vpu::prefetchCacheLine(qOut, 0);
    NMP::vpu::prefetchCacheLine(qOut, NMP_VPU_CACHESIZE);
    NMP::vpu::QuatMP vq(qOut);
    vq = rootOffsetAtt4 * vq;
    NMP::vpu::store4v(qOut, vq.x, vq.y, vq.z, vq.w);
  }
}

//----------------------------------------------------------------------------------------------------------------------
float computeErrorMetric(
  uint32_t              numEntries,
  const float*          weightsPos,
  const float*          weightsAtt,
  const NMP::Vector3*   netCompBufferPos,
  const NMP::Quat*      netCompBufferAtt,
  const NMP::Vector3*   srcCompBufferPos,
  const NMP::Quat*      srcCompBufferAtt)
{
  NMP::vpu::vector4_t vCost = NMP::vpu::zero4f();

  // Position  
  for (uint32_t indx = 0; indx < numEntries; indx += 4)
  {
    NMP::vpu::vector4_t wPos = NMP::vpu::load4f(&weightsPos[indx]);
    NMP::vpu::Vector3MP netPos((float*) &netCompBufferPos[indx]);
    NMP::vpu::Vector3MP srcPos((float*) &srcCompBufferPos[indx]);
    NMP::vpu::Vector3MP dx = srcPos - netPos;
    NMP::vpu::vector4_t mag2 = dx.lengthSquared();
    NMP::vpu::vector4_t costPos = NMP::vpu::mul4f(wPos, mag2);
    
    // Update the cost sum
    vCost = NMP::vpu::add4f(vCost, costPos);
  }
  
  // Orientation
  for (uint32_t indx = 0; indx < numEntries; indx += 4)
  {
    NMP::vpu::vector4_t wAtt = NMP::vpu::load4f(&weightsAtt[indx]);
    NMP::vpu::QuatMP netAtt((float*) &netCompBufferAtt[indx]);
    NMP::vpu::QuatMP srcAtt((float*) &srcCompBufferAtt[indx]);
    NMP::vpu::vector4_t vDot = srcAtt.dot(netAtt);
    NMP::vpu::vector4_t dq = NMP::vpu::sub4f(NMP::vpu::one4f(), NMP::vpu::abs4f(vDot));
    NMP::vpu::vector4_t costAtt = NMP::vpu::mul4f(wAtt, NMP::vpu::mul4f(dq, dq));

    // Update the cost sum
    vCost = NMP::vpu::add4f(vCost, costAtt);
  }

  // Extract the cost
  vCost = NMP::vpu::sum4f(vCost);
  float cost = NMP::vpu::floatX(vCost);

  return cost;
}


//----------------------------------------------------------------------------------------------------------------------
// Offline pre-computed source matching (Vanilla FPU)
//----------------------------------------------------------------------------------------------------------------------
#else // NMP_PLATFORM_SIMD

//----------------------------------------------------------------------------------------------------------------------
void computeNetworkMatchingSpaceTransforms(
  const AnimRigDef*                 rig,
  AttribDataClosestAnimDef*         setupData,
  AttribDataClosestAnimDefAnimSet*  setupDataAnimSet,
  const NMP::DataBuffer*            inputTransforms,
  NMP::DataBuffer*                  inputTransformsAcc,
  bool*                             validRigChannelFlags,
  NMP::Vector3*                     outputChannelPos,
  NMP::Quat*                        outputChannelAtt)
{
  // Accumulate the matching space transforms
  accumulateMatchingSpaceTransforms(
    rig,
    setupData,
    setupDataAnimSet,
    inputTransforms,
    inputTransformsAcc,
    validRigChannelFlags,
    outputChannelPos,
    outputChannelAtt);
}

//----------------------------------------------------------------------------------------------------------------------
void computeSourceTransformsFromVelocity(
  float deltaTime,
  uint32_t numEntries,
  const NMP::Vector3* srcDecompBufferPos0,
  const NMP::Quat* srcDecompBufferAtt0,
  const NMP::Vector3* srcDecompBufferPosVel,
  const NMP::Vector3* srcDecompBufferAttVel,
  NMP::Vector3* srcDecompBufferPos1,
  NMP::Quat* srcDecompBufferAtt1)
{
  // Position
  for (uint32_t indx = 0; indx < numEntries; ++indx)
  {
    NMP::Vector3 p = srcDecompBufferPos0[indx];
    NMP::Vector3 posVel = srcDecompBufferPosVel[indx];
    p += (posVel * deltaTime);
    srcDecompBufferPos1[indx] = p;
  }

  // Orientation
  for (uint32_t indx = 0; indx < numEntries; ++indx)
  {
    NMP::Quat q = srcDecompBufferAtt0[indx];
    NMP::Vector3 angVel = srcDecompBufferAttVel[indx];
    q += (NMP::Quat(angVel.x, angVel.y, angVel.z, 0.0f) * q) * (0.5f * deltaTime);
    q.fastNormalise();
    srcDecompBufferAtt1[indx] = q;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void computeOfflineSourceMatchingSpaceTransforms(
  uint32_t childIndex,
  AttribDataClosestAnimDef* setupData,
  NMP::Vector3** srcCompBufferPosArray,
  NMP::Quat** srcDecompBufferAttArray,
  bool useVelocity,
  float deltaTime,
  NMP::Vector3* srcDecompBufferPosVel,
  NMP::Vector3* srcDecompBufferAttVel)
{
  const AttribDataClosestAnimDef::ClosestAnimSourceData* animSourceData = setupData->getAnimSourceData(childIndex);
  NMP_ASSERT(animSourceData);    
  // Position
  NMP_ASSERT(animSourceData->m_sourceTransformsPos);
  animSourceData->m_sourceTransformsPos->decode(srcCompBufferPosArray[0]);
  // Orientation
  NMP_ASSERT(animSourceData->m_sourceTransformsAtt);
  animSourceData->m_sourceTransformsAtt->decode(srcDecompBufferAttArray[0]);

  if (useVelocity)
  {
    // Positional velocity
    NMP_ASSERT(animSourceData->m_sourceTransformsPosVel);
    animSourceData->m_sourceTransformsPosVel->decode(srcDecompBufferPosVel);
    // Angular velocity
    NMP_ASSERT(animSourceData->m_sourceTransformsAngVel);
    animSourceData->m_sourceTransformsAngVel->decode(srcDecompBufferAttVel);

    computeSourceTransformsFromVelocity(
      deltaTime,
      setupData->m_numAnimJoints,
      srcCompBufferPosArray[0],
      srcDecompBufferAttArray[0],
      srcDecompBufferPosVel,
      srcDecompBufferAttVel,
      srcCompBufferPosArray[1],
      srcDecompBufferAttArray[1]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void computeRootOffsetRotation(
  uint32_t numSets,
  uint32_t numEntries,
  const float* weights,
  const NMP::Vector3** netCompBufferPosArray,
  const NMP::Vector3** srcCompBufferPosArray,
  NMP::Quat& rootOffsetAtt)
{
  float D;
  float b[2];

  // Set up the linear system of equations
  D = 0.0f;
  b[0] = b[1] = 0.0f;
  for (uint32_t iSet = 0; iSet < numSets; ++iSet)
  {
    const NMP::Vector3* netCompBufferPos = netCompBufferPosArray[iSet];
    NMP_ASSERT(netCompBufferPos);
    const NMP::Vector3* srcCompBufferPos = srcCompBufferPosArray[iSet];
    NMP_ASSERT(srcCompBufferPos);

    for (uint32_t i = 0; i < numEntries; ++i)
    {
      float w = weights[i];
      NMP::Vector3 posNet = netCompBufferPos[i];
      NMP::Vector3 posSrc = srcCompBufferPos[i];
      D += w * (posSrc.x * posSrc.x + posSrc.z * posSrc.z);
      b[0] += w * (posSrc.x * posNet.x + posSrc.z * posNet.z);
      b[1] += w * (posSrc.x * posNet.z - posSrc.z * posNet.x);
    }
  }

  //---------------------------
  // Solve the linear system
  float c, s;
  if (D > 1e-7f)
  {
    c = b[0] / D;
    s = b[1] / D;
  }
  else
  {
    c = s = 0.0f;
  }

  // Enforce the unit norm constraint
  float mag = sqrtf(c * c + s * s);
  if (mag > 1e-7f)
  {
    c /= mag;
    s /= mag;
  }
  else
  {
    c = 1.0f;
    s = 0.0f;
  }

  // Matrix to quat for rotation about Y axis  
  float qy, qw;
  float t = 2 * c + 1; // trace
  if (t > 0.0f)
  {
    qw = 0.5f * sqrtf(t + 1);
    qy = -s / (2 * qw);
  }
  else
  {
    qy = 0.5f * sqrtf(3 - t);
    qw = -s / (2 * qy);
  }

  rootOffsetAtt.setXYZW(0.0f, qy, 0.0f, qw);
}

//----------------------------------------------------------------------------------------------------------------------
void applyRootOffsetTransform(
  const NMP::Quat& rootOffsetAtt,
  uint32_t numEntries,
  NMP::Vector3* channelPos,
  NMP::Quat* channelAtt)
{
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    channelPos[i] = rootOffsetAtt.rotateVector(channelPos[i]);
    channelAtt[i] = rootOffsetAtt * channelAtt[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
float computeErrorMetric(
  uint32_t              numEntries,
  const float*          weightsPos,
  const float*          weightsAtt,
  const NMP::Vector3*   netCompBufferPos,
  const NMP::Quat*      netCompBufferAtt,
  const NMP::Vector3*   srcCompBufferPos,
  const NMP::Quat*      srcCompBufferAtt)
{
  float cost = 0.0f;
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    // Position error
    NMP::Vector3 dx = srcCompBufferPos[i] - netCompBufferPos[i];
    float costPos = weightsPos[i] * dx.magnitudeSquared();

    // Orientation error
    float dq = 1.0f - fabs(srcCompBufferAtt[i].dot(netCompBufferAtt[i]));
    float costAtt = weightsAtt[i] * (dq * dq);
    
    // Total cost
    cost += (costPos + costAtt);
  }

  return cost;
}
  
#endif // NMP_PLATFORM_SIMD

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace
  
//----------------------------------------------------------------------------------------------------------------------
// Online source matching
//----------------------------------------------------------------------------------------------------------------------
class ClosestAnimSubNetworkEvalData
{
public:
  // State of the network before sub-network evaluation
  FrameCount                    m_currentFrameNo;
  TaskQueue*                    m_mainNetworkQueue;

  // Temporary state data during the sub-network evaluation
  FrameCount                    m_subNetworkEvalFrameNo;
  TaskQueue*                    m_subNetworkEvalQueue;
  AttribDataUpdatePlaybackPos*  m_timeAttrib;
};

//----------------------------------------------------------------------------------------------------------------------  
namespace
{

//----------------------------------------------------------------------------------------------------------------------
void initSubNetworkEval(
  NodeDef* node,
  Network* net,
  AttribDataClosestAnimState* closestAnimState,
  ClosestAnimSubNetworkEvalData& subNetworkEvalData)
{
  NMP_ASSERT(node);
  NMP_ASSERT(net);
  NMP_ASSERT(closestAnimState);

  // Flag the sub-network evaluation as in progress
  closestAnimState->m_isPerformingSubNetworkEval = true;

  //---------------------------
  // State of the network before sub-network evaluation
  subNetworkEvalData.m_currentFrameNo = net->getCurrentFrameNo();
  subNetworkEvalData.m_mainNetworkQueue = net->getTaskQueue();

  //---------------------------
  // Create a temporary queue for sub-network evaluation
  NMP::Memory::Format memReqsTempQueue = TaskQueue::getMemoryRequirements();
  memReqsTempQueue.size = NMP::Memory::align(memReqsTempQueue.size, memReqsTempQueue.alignment);
  NMP::Memory::Resource memResTempQueue = NMPAllocatorAllocateFromFormat(net->getTempMemoryAllocator(), memReqsTempQueue);
  NMP_ASSERT(memResTempQueue.ptr);
  TaskQueue* childEvalQueue = TaskQueue::init(memResTempQueue, net);

  // Replace the network queue and start queuing with the temporary queue  
  net->setTaskQueue(childEvalQueue);
  childEvalQueue->startQueuing();
  subNetworkEvalData.m_subNetworkEvalQueue = childEvalQueue;
  subNetworkEvalData.m_subNetworkEvalFrameNo = 1;

  //---------------------------
  // Create the time attributes on this node required for sub-network evaluation
  AttribAddress attribAddr(ATTRIB_SEMANTIC_UPDATE_TIME_POS, node->getNodeID());
  AttribDataHandle handle = AttribDataUpdatePlaybackPos::create(
    net->getPersistentMemoryAllocator(),
    false,  // time in secs
    true,   // abs time
    0.0f);  // time value

  subNetworkEvalData.m_timeAttrib = (AttribDataUpdatePlaybackPos*) handle.m_attribData;
  NMP_ASSERT(subNetworkEvalData.m_timeAttrib);
  net->addAttribData(attribAddr, handle, LIFESPAN_FOREVER);

  // Default sync event update time
  attribAddr.init(ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, node->getNodeID());
  handle = AttribDataUpdateSyncEventPlaybackPos::create(
    net->getPersistentMemoryAllocator(),
    true, // abs time
    SyncEventPos(),
    SyncEventPos(),
    SyncEventPos());
  net->addAttribData(attribAddr, handle, LIFESPAN_FOREVER);
}

//----------------------------------------------------------------------------------------------------------------------
// Perform a sub network update step, using a temporary task queue.
void updateSubNetworkEval(
  NodeDef*                        node,
  Network*                        net,
  ClosestAnimSubNetworkEvalData&  subNetworkEvalData,
  uint32_t                        childNodeIndex)
{
  NMP_ASSERT(node);
  NMP_ASSERT(net);
  
  const uint16_t subNetworkEvalLifespan = 1;
  
  //---------------------------
  // Set the current sub-network update frame
  net->setCurrentFrameNo(subNetworkEvalData.m_subNetworkEvalFrameNo);
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  nodeBin->setLastFrameUpdate(subNetworkEvalData.m_subNetworkEvalFrameNo);
  
  // Set the single active connection
  NodeID childNodeID = node->getChildNodeID(childNodeIndex);
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  connections->m_numActiveChildNodes = 1;
  connections->m_activeChildNodeIDs[0] = childNodeID;

  // Update the lifespan for the attributes owned by nodes that belong to the sub network
  net->updateAttribDataLifespans(childNodeID);

  // Update the child node connections
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  net->updateNodeInstanceConnections(childNodeID, animSet);

  // Tell the network we want to watch the transform output from the root node.
  net->addPostUpdateAccessAttrib(childNodeID, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, subNetworkEvalLifespan);

  //---------------------------
  // Queue a dummy task for the children evaluation.
  TaskQueue* subNetworkEvalQueue = net->getTaskQueue();
  Task* childEvalTask = subNetworkEvalQueue->createNewRootTaskOnQueue(
    CoreTaskIDs::MR_TASKID_EMPTYTASK, // ID of the task to add to the queue.
    childNodeID,                      // Task owning Node ID.
    1,                                // Num task parameters.
    true);                            // Make this task external.

  if (childEvalTask)
  {
    // Transforms task
    childEvalTask->m_params[0].init(
      TPARAM_FLAG_INPUT,
      ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
      ATTRIB_TYPE_TRANSFORM_BUFFER,
      childNodeID,
      INVALID_NODE_ID,
      subNetworkEvalData.m_subNetworkEvalFrameNo,
      ANIMATION_SET_ANY,
      0);

    // This call will queue the tasks required to generate transform attributes for child nodes.
    net->queueTasksFor(&(childEvalTask->m_params[0]));

    subNetworkEvalQueue->addToTaskList(childEvalTask);
  }

  // finalize the task queue
  subNetworkEvalQueue->finishQueuing();

  //---------------------------
  // Task execution
  Task* task = NULL;
  ExecuteResult execResult;
  do
  {
    execResult = net->getDispatcher()->execute(subNetworkEvalQueue, task);

    // We handle each of the empty tasks externally to stop the dispatcher deleting the transforms just yet
    if (execResult == EXECUTE_RESULT_BLOCKED)
    {
      NMP_ASSERT(task->m_taskid == CoreTaskIDs::MR_TASKID_EMPTYTASK);

      net->getTaskQueue()->m_numUnprocessedTasks--;

      task = NULL;

      // Set execResult to in progress so we can carry on iterating
      execResult = EXECUTE_RESULT_IN_PROGRESS;
    }

  } while (execResult == EXECUTE_RESULT_IN_PROGRESS);
  
  // Check we have processed all tasks in the queue
  if (task)
  {
    NET_LOG_ERROR_MESSAGE("updateSubNetworkEval: Some tasks were not dispatched due to un-computed dependencies.\n");
    NMP_ASSERT_FAIL();
  }

  //---------------------------
  // Let the network know that we no longer need to watch the transform output from the child node
  net->removePostUpdateAccessAttrib(childNodeID, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void clearSubNetworkEval(
  NodeDef*      node,
  Network*      net,
  uint32_t      childNodeIndex)
{
  NMP_ASSERT(node);
  NMP_ASSERT(net);

  // Delete the child node instance data created during the sub-network evaluation
  NodeID childNodeID = node->getChildNodeID(childNodeIndex);
  net->deleteNodeInstance(childNodeID);

  // Reset the active node connections
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  connections->m_numActiveChildNodes = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void deleteSubNetworkEval(
  NodeDef* node,
  Network* net,
  AttribDataClosestAnimState* closestAnimState,
  ClosestAnimSubNetworkEvalData& subNetworkEvalData)
{
  NMP_ASSERT(node);
  NMP_ASSERT(net);
  NMP_ASSERT(closestAnimState);

  // Tidy up this nodes attrib data (time attributes)
  const AttribDataSemantic excludeSemantics[2] = {
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    ATTRIB_SEMANTIC_PLAYBACK_POS_INIT};
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  nodeBin->deleteAllAttribDataExcluding(2, excludeSemantics);

  // Clear this nodes queued task list.
  nodeBin->clearQueuedTasks();

  // Restore the right frame number.
  net->setCurrentFrameNo(subNetworkEvalData.m_currentFrameNo);

  // Restore the right task queue
  net->setTaskQueue(subNetworkEvalData.m_mainNetworkQueue);

  // Restore the dispatcher state
  Dispatcher* dispatcher = net->getDispatcher();
  dispatcher->setTaskQueue(subNetworkEvalData.m_mainNetworkQueue);

  // Need to reset the update frame because the delete function wipes it
  nodeBin->setLastFrameUpdate(subNetworkEvalData.m_currentFrameNo);

  // Flag the sub-network evaluation as complete
  closestAnimState->m_isPerformingSubNetworkEval = false;
}

//----------------------------------------------------------------------------------------------------------------------
void accumulateOnlineSourceMatchingSpaceTransforms(
  NodeDef*                          node,
  Network*                          net,
  ClosestAnimSubNetworkEvalData&    subNetworkEvalData,
  uint32_t                          childIndex,
  const AnimRigDef*                 rig,
  AttribDataClosestAnimDef*         setupData,
  AttribDataClosestAnimDefAnimSet*  setupDataAnimSet,
  NMP::DataBuffer*                  inputTransformsAcc,
  bool*                             validRigChannelFlags,
  NMP::Vector3**                    srcDecompBufferPosArray,
  NMP::Quat**                       srcDecompBufferAttArray,
  bool                              useVelocity,
  float                             deltaTime)
{
  AttribDataTransformBuffer* transformBufferAttrib;
  NMP::DataBuffer* transformBuffer;

  NodeID childNodeID = node->getChildNodeID(childIndex);
  AnimSetIndex animSetIndex = net->getActiveAnimSetIndex();

  //---------------------------
  // Evaluate the sub-network at the first frame
  subNetworkEvalData.m_subNetworkEvalFrameNo = 1;
  subNetworkEvalData.m_timeAttrib->m_isFraction = false;
  subNetworkEvalData.m_timeAttrib->m_isAbs = true;
  subNetworkEvalData.m_timeAttrib->m_value = 0.0f;

  updateSubNetworkEval(
    node,
    net,
    subNetworkEvalData,
    childIndex);

  // Recover the transforms
  transformBufferAttrib = net->getAttribData<AttribDataTransformBuffer>(
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    childNodeID,
    INVALID_NODE_ID,
    subNetworkEvalData.m_subNetworkEvalFrameNo,
    animSetIndex);

  transformBuffer = transformBufferAttrib->m_transformBuffer;
  NMP_ASSERT(transformBuffer);

  // Accumulate the matching space transforms
  accumulateMatchingSpaceTransforms(
    rig,
    setupData,
    setupDataAnimSet,
    transformBuffer,
    inputTransformsAcc,
    validRigChannelFlags,
    srcDecompBufferPosArray[0],
    srcDecompBufferAttArray[0]);

  if (useVelocity)
  {
    //---------------------------
    // Evaluate the sub-network at the second frame
    subNetworkEvalData.m_subNetworkEvalFrameNo = 2;
    subNetworkEvalData.m_timeAttrib->m_isAbs = false;
    subNetworkEvalData.m_timeAttrib->m_value = deltaTime;

    updateSubNetworkEval(
      node,
      net,
      subNetworkEvalData,
      childIndex);

    // Recover the transforms
    transformBufferAttrib = net->getAttribData<AttribDataTransformBuffer>(
      ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
      childNodeID,
      INVALID_NODE_ID,
      subNetworkEvalData.m_subNetworkEvalFrameNo,
      animSetIndex);

    transformBuffer = transformBufferAttrib->m_transformBuffer;
    NMP_ASSERT(transformBuffer);

    // Accumulate the matching space transforms
    accumulateMatchingSpaceTransforms(
      rig,
      setupData,
      setupDataAnimSet,
      transformBuffer,
      inputTransformsAcc,
      validRigChannelFlags,
      srcDecompBufferPosArray[1],
      srcDecompBufferAttArray[1]);
  }

  // Clean up the child node's sub-network evaluation data
  clearSubNetworkEval(
    node,
    net,
    childIndex);
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NMP_PLATFORM_SIMD
//----------------------------------------------------------------------------------------------------------------------
void computeOnlineSourceMatchingSpaceTransforms(
  NodeDef*                          node,
  Network*                          net,
  ClosestAnimSubNetworkEvalData&    subNetworkEvalData,
  uint32_t                          childIndex,
  const AnimRigDef*                 rig,
  AttribDataClosestAnimDef*         setupData,
  AttribDataClosestAnimDefAnimSet*  setupDataAnimSet,
  NMP::DataBuffer*                  inputTransformsAcc,
  bool*                             validRigChannelFlags,
  uint32_t                          numSets,
  NMP::Vector3**                    srcDecompBufferPosArray,
  NMP::Quat**                       srcDecompBufferAttArray,
  float                             deltaTime)
{
  // Accumulate the source matching space transforms
  bool useVelocity = (numSets > 1);
  accumulateOnlineSourceMatchingSpaceTransforms(
    node,
    net,
    subNetworkEvalData,
    childIndex,
    rig,
    setupData,
    setupDataAnimSet,
    inputTransformsAcc,
    validRigChannelFlags,
    srcDecompBufferPosArray,
    srcDecompBufferAttArray,
    useVelocity,
    deltaTime);

  // Transpose into SOA block-4 format
  for (uint32_t iSet = 0; iSet < numSets; ++iSet)
  {
    convertTransformsMP(
      setupData->m_numAnimJoints,
      srcDecompBufferPosArray[iSet],
      srcDecompBufferAttArray[iSet]);
  }
    
}
#else // NMP_PLATFORM_SIMD

//----------------------------------------------------------------------------------------------------------------------
void computeOnlineSourceMatchingSpaceTransforms(
  NodeDef*                          node,
  Network*                          net,
  ClosestAnimSubNetworkEvalData&    subNetworkEvalData,
  uint32_t                          childIndex,
  const AnimRigDef*                 rig,
  AttribDataClosestAnimDef*         setupData,
  AttribDataClosestAnimDefAnimSet*  setupDataAnimSet,
  NMP::DataBuffer*                  inputTransformsAcc,
  bool*                             validRigChannelFlags,
  uint32_t                          numSets,
  NMP::Vector3**                    srcDecompBufferPosArray,
  NMP::Quat**                       srcDecompBufferAttArray,
  float                             deltaTime)
{
  // Accumulate the source matching space transforms
  bool useVelocity = (numSets > 1);
  accumulateOnlineSourceMatchingSpaceTransforms(
    node,
    net,
    subNetworkEvalData,
    childIndex,
    rig,
    setupData,
    setupDataAnimSet,
    inputTransformsAcc,
    validRigChannelFlags,
    srcDecompBufferPosArray,
    srcDecompBufferAttArray,
    useVelocity,
    deltaTime);
}
#endif // NMP_PLATFORM_SIMD

//----------------------------------------------------------------------------------------------------------------------
// Main source matching function
//----------------------------------------------------------------------------------------------------------------------
void computeClosestSourceNode(
  NodeDef*                          node,
  Network*                          net,
  const AnimRigDef*                 rig,
  AttribDataClosestAnimDef*         setupData,
  AttribDataClosestAnimDefAnimSet*  setupDataAnimSet,
  NMP::DataBuffer*                  prevNetworkTransforms,
  NMP::DataBuffer*                  prevPrevNetworkTransforms,
  AttribDataClosestAnimState*       closestAnimState,
  ClosestAnimResult&                result)
{
  NMP_ASSERT(setupData);
  NMP_ASSERT(setupDataAnimSet);
  NMP_ASSERT(prevNetworkTransforms);
  NMP_ASSERT(setupData->m_numSources > 0);
  NMP_ASSERT(setupData->m_numAnimJoints > 0);
  
  NM_CLOSEST_ANIM_BEGIN_PROFILING("CLOSEST_ANIM_DO_MATCHING");

  //---------------------------
  // Remember the rollback point, so we can free everything in one shot.
  NMP::TempMemoryAllocator* tempMemAllocator = net->getTempMemoryAllocator();
  NMP_ASSERT(tempMemAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempMemAllocator->createChildAllocator();
  net->setTempMemoryAllocator(childAllocator);

  // Initialise the sub-network evaluation data
  ClosestAnimSubNetworkEvalData subNetworkEvalData;  
  if (!setupData->m_precomputeSourcesOffline)
  {
    initSubNetworkEval(
      node,
      net,
      closestAnimState,
      subNetworkEvalData);
  }

  // Information
  uint32_t numAnimJointsPadded = (setupData->m_numAnimJoints + 3) & (~0x03); // Padded to block of four
  uint32_t numRigJoints = rig->getNumBones();
  bool useVelocity = (prevPrevNetworkTransforms != NULL);
  float deltaTime = net->getLastUpdateTimeStep();

  //---------------------------
  // Allocate temporary memory for the cost errors
  NMP::Memory::Format memReqsCosts(sizeof(float) * setupData->m_numSources, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResCosts = NMPAllocatorAllocateFromFormat(childAllocator, memReqsCosts);
  float* costs = (float*) memResCosts.ptr;

  // Allocate the temporary memory for the root alignment quats
  NMP::Memory::Format memReqsRootQuats(sizeof(NMP::Quat) * setupData->m_numSources, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResRootQuats = NMPAllocatorAllocateFromFormat(childAllocator, memReqsRootQuats);
  NMP::Quat* rootAlignmentQuats = (NMP::Quat*) memResRootQuats.ptr;

  //---------------------------
  // Allocate temporary memory for the accumulated network output transforms
  NMP::Memory::Format buffMemReqs;
  NMP::Memory::Format internalBuffMemReqs;
  MR::AttribDataTransformBuffer::getPosQuatMemoryRequirements(
    numRigJoints,
    buffMemReqs,
    internalBuffMemReqs);
  NMP::Memory::Resource buffMemRes = NMPAllocatorAllocateFromFormat(childAllocator, buffMemReqs);
  MR::AttribDataTransformBuffer* networkTransformsAccAttrib = MR::AttribDataTransformBuffer::initPosQuat(
    buffMemRes,
    internalBuffMemReqs,
    numRigJoints);
  NMP::DataBuffer* networkTransformsAcc = networkTransformsAccAttrib->m_transformBuffer;
  
  // Memory for the valid channel flags for partial accumulation
  NMP::Memory::Format memReqsRigChanFlags(sizeof(bool) * numRigJoints, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResRigChanFlags = NMPAllocatorAllocateFromFormat(childAllocator, memReqsRigChanFlags);
  bool* validRigChannelFlags = (bool*) memResRigChanFlags.ptr;

  //---------------------------
  // Allocate temporary memory for the anim channel weights
  NMP::Memory::Format memReqsWeights(sizeof(float) * numAnimJointsPadded, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResWeights;
  memResWeights = NMPAllocatorAllocateFromFormat(childAllocator, memReqsWeights);
  float* weights = (float*) memResWeights.ptr;
  memResWeights = NMPAllocatorAllocateFromFormat(childAllocator, memReqsWeights);
  float* weightsPos = (float*) memResWeights.ptr;
  memResWeights = NMPAllocatorAllocateFromFormat(childAllocator, memReqsWeights);
  float* weightsAtt = (float*) memResWeights.ptr;

  // Initialise the weights vector
  for (uint32_t i = 0; i < numAnimJointsPadded; ++i)
    weights[i] = 0.0f;

  // Unpack the weights vector
  for (uint32_t entryIndex = 0; entryIndex < setupDataAnimSet->m_numEntries; ++entryIndex)
  {
    uint32_t animChannelIndex = setupDataAnimSet->m_animChannels[entryIndex];
    weights[animChannelIndex] = setupDataAnimSet->m_weights[entryIndex];
  }

  // Position and orientation interpolation weights
  float interpPos = (1.0f - setupData->m_influenceBetweenPosAndOrient) * setupData->m_positionScaleFactor;
  float interpAtt = setupData->m_influenceBetweenPosAndOrient * setupData->m_orientationScaleFactor;
  for (uint32_t i = 0; i < numAnimJointsPadded; ++i)
  {
    weightsPos[i] = weights[i] * interpPos;
    weightsAtt[i] = weights[i] * interpAtt;
  }

  //---------------------------
  // Allocate temporary memory for the child source decompression buffer
  NMP::Memory::Format memReqsDecompBufferPos(sizeof(NMP::Vector3) * numAnimJointsPadded, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Format memReqsDecompBufferAtt(sizeof(NMP::Quat) * numAnimJointsPadded, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResDecompBufferPos;
  NMP::Memory::Resource memResDecompBufferAtt;
  
  uint32_t numSets = 0;
  NMP::DataBuffer* networkTransformsArray[2] = {NULL, NULL};
  NMP::Vector3* netCompBufferPosArray[2] = {NULL, NULL};
  NMP::Quat* netCompBufferAttArray[2] = {NULL, NULL};
  NMP::Vector3* srcCompBufferPosArray[2] = {NULL, NULL};
  NMP::Quat* srcCompBufferAttArray[2] = {NULL, NULL};
  NMP::Vector3* srcDecompBufferPosVel = NULL;
  NMP::Vector3* srcDecompBufferAttVel = NULL;

  if (useVelocity)
  {
    networkTransformsArray[0] = prevPrevNetworkTransforms;
    networkTransformsArray[1] = prevNetworkTransforms;
    numSets = 2;
    
    // Velocities
    memResDecompBufferPos = NMPAllocatorAllocateFromFormat(childAllocator, memReqsDecompBufferPos);
    srcDecompBufferPosVel = (NMP::Vector3*) memResDecompBufferPos.ptr;
    memResDecompBufferPos = NMPAllocatorAllocateFromFormat(childAllocator, memReqsDecompBufferPos);
    srcDecompBufferAttVel = (NMP::Vector3*) memResDecompBufferPos.ptr;
  }
  else
  {
    networkTransformsArray[0] = prevNetworkTransforms;    
    numSets = 1;
  }
  
  // Compare buffers
  for (uint32_t iSet = 0; iSet < numSets; ++iSet)
  {
    // Source
    memResDecompBufferPos = NMPAllocatorAllocateFromFormat(childAllocator, memReqsDecompBufferPos);
    srcCompBufferPosArray[iSet] = (NMP::Vector3*) memResDecompBufferPos.ptr;
    memResDecompBufferAtt = NMPAllocatorAllocateFromFormat(childAllocator, memReqsDecompBufferAtt);
    srcCompBufferAttArray[iSet] = (NMP::Quat*) memResDecompBufferAtt.ptr;
    
    // Network
    memResDecompBufferPos = NMPAllocatorAllocateFromFormat(childAllocator, memReqsDecompBufferPos);
    netCompBufferPosArray[iSet] = (NMP::Vector3*) memResDecompBufferPos.ptr;
    memResDecompBufferAtt = NMPAllocatorAllocateFromFormat(childAllocator, memReqsDecompBufferAtt);
    netCompBufferAttArray[iSet] = (NMP::Quat*) memResDecompBufferAtt.ptr;
  }

  //---------------------------
  // Compute the network output transforms (in character matching space)
  NM_CLOSEST_ANIM_BEGIN_PROFILING("CLOSEST_ANIM_NETWORK_TMS");
  for (uint32_t iSet = 0; iSet < numSets; ++iSet)
  {
    computeNetworkMatchingSpaceTransforms(
      rig,
      setupData,
      setupDataAnimSet,
      networkTransformsArray[iSet],
      networkTransformsAcc,
      validRigChannelFlags,
      netCompBufferPosArray[iSet],
      netCompBufferAttArray[iSet]);
  }
  NM_CLOSEST_ANIM_END_PROFILING();

  //---------------------------
  // Store the last updated from of each descendant so it can be restored once pose matching is complete
  FrameCount* lastUpdatedFrame = NULL;
  if (!setupData->m_precomputeSourcesOffline)
  {
    NMP::Memory::Format lastFrameUpdates(sizeof(FrameCount) * setupData->m_numDescendants);
    lastUpdatedFrame = (FrameCount*)NMPAllocatorAllocateFromFormat(childAllocator, lastFrameUpdates).ptr;
    for (uint32_t i = 0; i < setupData->m_numDescendants; ++i)
    {
      NodeBin* nodeBin = net->getNodeBin(setupData->m_descendantIDs[i]);
      lastUpdatedFrame[i] = nodeBin->getLastFrameUpdate();
    }
  }

  //---------------------------
  // Matching
#ifdef TEST_CLOSEST_ANIM_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n---------------------------\n");
#endif
  NM_CLOSEST_ANIM_BEGIN_PROFILING("CLOSEST_ANIM_MATCH_SOURCES");
  for (uint32_t childIndex = 0; childIndex < setupData->m_numSources; ++childIndex)
  {
    // Compute the source node matching space transforms
    NM_CLOSEST_ANIM_BEGIN_PROFILING("CLOSEST_ANIM_SOURCE_TM");
    if (setupData->m_precomputeSourcesOffline)
    {
      computeOfflineSourceMatchingSpaceTransforms(
        childIndex,
        setupData,
        srcCompBufferPosArray,
        srcCompBufferAttArray,
        useVelocity,
        deltaTime,
        srcDecompBufferPosVel,
        srcDecompBufferAttVel);
    }
    else
    {
      computeOnlineSourceMatchingSpaceTransforms(
        node,
        net,
        subNetworkEvalData,
        childIndex,
        rig,
        setupData,
        setupDataAnimSet,
        networkTransformsAcc,
        validRigChannelFlags,
        numSets,
        srcCompBufferPosArray,
        srcCompBufferAttArray,
        deltaTime);
    }
    NM_CLOSEST_ANIM_END_PROFILING();

    //---------------------------
    // Root offset rotation
    if (setupData->m_useRootRotationBlending)
    {
      NM_CLOSEST_ANIM_BEGIN_PROFILING("CLOSEST_ANIM_ROOT_ROTATION");

      // Compute the root offset rotation (in character matching space) that aligns the
      // source node position data with the network comparison buffer data.
      NMP::Quat rootOffsetAtt;
      computeRootOffsetRotation(
        numSets,
        setupDataAnimSet->m_numAnimWeights,
        weights,
        (const NMP::Vector3**)netCompBufferPosArray,
        (const NMP::Vector3**)srcCompBufferPosArray,
        rootOffsetAtt); // Rotation about the Y axis

      // Check rotation offset limits
      if (fabs(rootOffsetAtt.w) < setupData->m_maxRootRotationAngle)
      {
        // Root offset rotation is larger than the limit maximum
        rootOffsetAtt.identity();
      }
      else
      {
        // Apply the root rotation to the character match space transforms
        for (uint32_t iSet = 0; iSet < numSets; ++iSet)
        {
          applyRootOffsetTransform(
            rootOffsetAtt,
            setupDataAnimSet->m_numAnimWeights,
            srcCompBufferPosArray[iSet],
            srcCompBufferAttArray[iSet]);
        }
      }

      // Compute the actual root offset rotation R about the specified root rotation axis
      NMP::Vector3 v = setupData->m_rootRotationAxis * rootOffsetAtt.y;
      rootAlignmentQuats[childIndex].setXYZW(v.x, v.y, v.z, rootOffsetAtt.w);

      NM_CLOSEST_ANIM_END_PROFILING();
    }

    //---------------------------
    // Errot metric
    NM_CLOSEST_ANIM_BEGIN_PROFILING("CLOSEST_ANIM_ERROR_METRIC");

    costs[childIndex] = 0.0f;
    for (uint32_t iSet = 0; iSet < numSets; ++iSet)
    {
      float err = computeErrorMetric(
        setupDataAnimSet->m_numAnimWeights,
        weightsPos,
        weightsAtt,
        netCompBufferPosArray[iSet],
        netCompBufferAttArray[iSet],
        srcCompBufferPosArray[iSet],
        srcCompBufferAttArray[iSet]);
        
      costs[childIndex] += err;
      
#ifdef TEST_CLOSEST_ANIM_LOGGING
      NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "costs[%d] = %f ", childIndex, err);
#endif
    }
#ifdef TEST_CLOSEST_ANIM_LOGGING
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, ", Total costs[%d] = %f\n", childIndex, costs[childIndex]);
#endif

    NM_CLOSEST_ANIM_END_PROFILING(); // CLOSEST_ANIM_ERROR_METRIC
  }
  NM_CLOSEST_ANIM_END_PROFILING(); // CLOSEST_ANIM_MATCH_SOURCES

  //---------------------------
  // Decide the best matching source
  result.m_activeChildIndex = 0;
  for (uint32_t i = 1; i < setupData->m_numSources; ++i)
  {
    if (costs[i] < costs[result.m_activeChildIndex])
      result.m_activeChildIndex = i;
  }

  // Set the best matching root alignment rotation
  result.m_activeChildRootQuat = rootAlignmentQuats[result.m_activeChildIndex];

  //---------------------------
  // Tidy up the node's attrib data after sub-network evaluation
  if (!setupData->m_precomputeSourcesOffline)
  {
    deleteSubNetworkEval(
      node,
      net,
      closestAnimState,
      subNetworkEvalData);
  }

  //---------------------------
  // Restore the last updated from of every node under the closest anim.
  if (!setupData->m_precomputeSourcesOffline)
  {
    for (uint32_t i = 0; i < setupData->m_numDescendants; ++i)
    {
      NodeBin* nodeBin = net->getNodeBin(setupData->m_descendantIDs[i]);
      nodeBin->setLastFrameUpdate(lastUpdatedFrame[i]);
    }
  }

  // Tidy up the temporary working memory
  net->setTempMemoryAllocator(tempMemAllocator);
  tempMemAllocator->destroyChildAllocator(childAllocator);
  
  NM_CLOSEST_ANIM_END_PROFILING(); // CLOSEST_ANIM_DO_MATCHING
}


//----------------------------------------------------------------------------------------------------------------------
// Common functionality
//----------------------------------------------------------------------------------------------------------------------
AttribDataClosestAnimState* createClosestAnimState(Network* net, NodeDef* node)
{
  // Check it doesn't already exist
  NMP_ASSERT(!net->getOptionalAttribData(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID));

  // Create the closest anim node state attribute data
  AttribDataHandle handle = AttribDataClosestAnimState::create(net->getPersistentMemoryAllocator(), NULL);
  AttribDataClosestAnimState* result = (AttribDataClosestAnimState*) handle.m_attribData;
  result->m_isPerformingSubNetworkEval = false;

  // Add the attribute data to the network node
  AttribAddress attribAddr(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID());
  net->addAttribData(attribAddr, handle, LIFESPAN_FOREVER);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Initialise the dead blend state to predict the network output transforms over the coming
// frames. We also replace the hips channel transform with the accumulated character space
// values from the network output. We blend the character space hips transforms from the
// network output and the 'root rotation aligned' input animation rather than the root channel
// itself, since the hips are usually located at the center of the body so distributing the
// offset rotation here will minimize the likelihood of ground penetration from the limbs
// during the blending.
void initialiseClosestAnimDeadBlend(
  Network*            net,
  NodeDef*            node)
{
  bool useDeadBlend = true;
  NetworkDef* networkDef = net->getNetworkDef();
  const AnimRigDef* rig = net->getActiveRig();
  const uint32_t currentFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currentFrameNo > 0);
  uint32_t hipsIndex = rig->getCharacterRootBoneIndex();

  //---------------------------
  // Get the previous network output transforms
  NMP::DataBuffer* prevDataBuffer = getNodeTransformsBuffer(
    networkDef->getRootNodeID(),
    net,
    currentFrameNo - 1,
    net->getOutputAnimSetIndex(networkDef->getRootNodeID()));
  NMP_ASSERT(prevDataBuffer); // Must exist

  NMP::DataBuffer* prevPrevDataBuffer = getNodeTransformsBuffer(
    networkDef->getRootNodeID(),
    net,
    currentFrameNo - 2,
    net->getOutputAnimSetIndex(networkDef->getRootNodeID()));
  if (!prevPrevDataBuffer)
    prevPrevDataBuffer = prevDataBuffer;

  // Get the last frames delta time update
  float dt = net->getLastUpdateTimeStep();

  // Initialise dead blend transforms using state from the previous frames
  NMP::DataBuffer* transRates;
  NMP::DataBuffer* deadBlendTransforms;
  initialiseDeadBlendTransforms(
    net,
    node,
    useDeadBlend,
    dt,
    rig,
    prevDataBuffer,
    prevPrevDataBuffer,
    deadBlendTransforms,
    transRates);

  //---------------------------
  // Accumulate the character space hips transform
  NMP::Vector3 charSpaceHipsPos;
  NMP::Quat charSpaceHipsQuat;
  BlendOps::accumulateTransform(
    hipsIndex,
    NMP::Vector3(NMP::Vector3::InitZero),
    NMP::Quat(NMP::Quat::kIdentity),
    prevDataBuffer,
    rig,
    charSpaceHipsPos,
    charSpaceHipsQuat);

  // Set the character space hips transforms
  deadBlendTransforms->setPosQuatChannelPos(hipsIndex, charSpaceHipsPos);
  deadBlendTransforms->setPosQuatChannelQuat(hipsIndex, charSpaceHipsQuat);
  deadBlendTransforms->setChannelUsed(hipsIndex);
  transRates->setPosVelAngVelChannelAngVel(hipsIndex, NMP::Vector3(NMP::Vector3::InitZero));
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void nodeClosestAnimInitInstance(NodeDef* node, Network* net)
{
  NodeID rootNodeID = net->getNetworkDef()->getRootNodeID();

  //----------------------
  // Add post update access attribs for semantics required to match the network output to the closest anim input.
  AttribDataClosestAnimDef* closestAnimDef = 
    net->getNetworkDef()->getAttribData<AttribDataClosestAnimDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID(), 0);

  uint16_t transformBufferLifespan = 2;
  if (closestAnimDef->m_useVelocity)
  {
    // The closest anim node requires 2 previous frames of transforms if doing velocity blending.
    transformBufferLifespan = 3;
  }

  net->addPostUpdateAccessAttrib(rootNodeID, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, transformBufferLifespan);
  net->addPostUpdateAccessAttrib(rootNodeID, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, transformBufferLifespan);

  // The trajectory is already a delta so only a single previous frame is required.
  net->addPostUpdateAccessAttrib(rootNodeID, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, 2);

  nodeShareInitInstanceInvalidateAllChildren(node, net);

  if (node->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(node, net);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void nodeClosestAnimDeleteInstance(const NodeDef* node, Network* net)
{
  nodeShareDeleteInstanceWithChildren(node, net);
  
  // Reset the active connections
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  connections->m_numActiveChildNodes = 0;
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
void finaliseActiveChild0(
  NodeDef*                      node,
  Network*                      net,
  NodeConnections*              connections,
  AttribDataClosestAnimDef*     setupData,
  AttribDataClosestAnimState*   closestAnimState)
{
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the active connections
  connections->m_numActiveChildNodes = 1;
  connections->m_activeChildNodeIDs[0] = node->getChildNodeID(0);
  net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[0], animSet);

  if (setupData->m_useRootRotationBlending)
  {
    NMP_ASSERT(net->getAttribData<AttribDataClosestAnimState>(
      ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
      node->getNodeID(),
      INVALID_NODE_ID,
      VALID_FRAME_ANY_FRAME,
      ANIMATION_SET_ANY) == closestAnimState);

    closestAnimState->m_blendCompleteFlag = true;
    closestAnimState->m_rootRotationOffset.identity();
    closestAnimState->m_fractionThroughSource = setupData->m_fractionThroughSource;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void finaliseActiveChild(
  NodeDef*                      node,
  Network*                      net,
  NodeConnections*              connections,
  AttribDataClosestAnimDef*     setupData,
  AttribDataClosestAnimState*   closestAnimState,
  ClosestAnimResult&            result)
{
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the active connections
  connections->m_numActiveChildNodes = 1;
  connections->m_activeChildNodeIDs[0] = node->getChildNodeID(result.m_activeChildIndex);
  net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[0], animSet);

  if (setupData->m_useRootRotationBlending)
  {
    NMP_ASSERT(net->getAttribData<AttribDataClosestAnimState>(
      ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
      node->getNodeID(),
      INVALID_NODE_ID,
      VALID_FRAME_ANY_FRAME,
      ANIMATION_SET_ANY) == closestAnimState);

    closestAnimState->m_blendCompleteFlag = false;
    closestAnimState->m_rootRotationOffset = result.m_activeChildRootQuat;
    closestAnimState->m_fractionThroughSource = setupData->m_fractionThroughSource;

    // Initialise the dead blend for predicting the network output transforms
    initialiseClosestAnimDeadBlend(net, node);
  }
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeClosestAnimUpdateConnections(
  NodeDef*           node,
  Network*           net)
{
  FrameCount currentFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currentFrameNo > 0);

  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
  
  // Update blend weight control parameter.
  net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(0), activeAnimSetIndex);

  // Check if we have already set the active child node
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  if (connections->m_numActiveChildNodes == 1)
  {
    net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[0], activeAnimSetIndex);
    return node->getNodeID();
  }

  // Information
  NetworkDef* networkDef = net->getNetworkDef();
  uint32_t numChildren = node->getNumChildNodes();
  const AnimRigDef* rig = net->getActiveRig();

  //---------------------------
  // Get the closest anim setup data
  AttribDataClosestAnimDef* setupData = networkDef->getAttribData<AttribDataClosestAnimDef>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,
    node->getNodeID());
  NMP_ASSERT(setupData);

  AttribDataClosestAnimDefAnimSet* setupDataAnimSet = networkDef->getAttribData<AttribDataClosestAnimDefAnimSet>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,
    node->getNodeID(),
    activeAnimSetIndex);
  NMP_ASSERT(setupDataAnimSet);

  //---------------------------
  // Get the previous frame's computed network output transforms
  NMP::DataBuffer* prevNetworkTransforms = getNodeTransformsBuffer(
    networkDef->getRootNodeID(),
    net,
    currentFrameNo - 1,
    net->getOutputAnimSetIndex(networkDef->getRootNodeID()));

  // Check for a potential change in the animation set
  if (prevNetworkTransforms && prevNetworkTransforms->getLength() != rig->getNumBones())
  {
    NET_LOG_ERROR_MESSAGE(
      "Closest anim node %s is trying to match transforms that have different sizes. Possible anim set change. The first valid child node has been selected.\n",
      node->getName());
    prevNetworkTransforms = NULL;
  }
  
  // Get the computed network output transforms from two frames ago
  NMP::DataBuffer* prevPrevNetworkTransforms = NULL;
  if (setupData->m_useVelocity)
  {
    prevPrevNetworkTransforms = getNodeTransformsBuffer(
      networkDef->getRootNodeID(),
      net,
      currentFrameNo - 2,
      net->getOutputAnimSetIndex(networkDef->getRootNodeID()));

    if (prevPrevNetworkTransforms && prevPrevNetworkTransforms->getLength() != rig->getNumBones())
    {
      NET_LOG_ERROR_MESSAGE(
        "Closest anim node %s is trying to match transforms that have different sizes. Possible anim set change for transforms two frames ago.\n",
        node->getName());
      prevPrevNetworkTransforms = NULL;
    }
  }

  // Create the closest anim node state attribute data
  AttribDataClosestAnimState* closestAnimState = createClosestAnimState(net, node);

  //---------------------------
  // Check if we need to perform the closest animation matching
  bool runEvaluation = false;
  // Must perform matching if we have more than one input source
  if (numChildren > 1)
    runEvaluation = true;
  // Must perform matching to compute the root rotation
  if (setupData->m_useRootRotationBlending)
    runEvaluation = true;
  // Check we have enough data to perform the matching
  if (runEvaluation && !prevNetworkTransforms)
  {
    NET_LOG_ERROR_MESSAGE(
      "Closest anim node %s doesn't have enough cached transforms data for the computation. The first valid child node has been selected.\n",
      node->getName());
    runEvaluation = false;
  }
  // Check if the anim set data has any enties
  if (setupDataAnimSet->m_numEntries == 0)
  {
    NET_LOG_ERROR_MESSAGE(
      "Closest anim node %s there are no weighted joint channels to compare. The first valid child node has been selected.\n",
      node->getName());
    runEvaluation = false;
  }

  // Check we are above any retargeting
  if (activeAnimSetIndex != net->getActiveAnimSetIndex())
  {
    NET_LOG_ERROR_MESSAGE("Closest anim nodes must be above any retarget nodes in the network.\n");
    runEvaluation = false;
  }

  //---------------------------
  // Evaluate which child node should be activated.
  if (runEvaluation)
  {
    // Decide the best matching source
    ClosestAnimResult result;
    computeClosestSourceNode(
      node,
      net,
      rig,
      setupData,
      setupDataAnimSet,
      prevNetworkTransforms,
      prevPrevNetworkTransforms,
      closestAnimState,
      result);

    // Update the connectivity of the chosen child
    finaliseActiveChild(
      node,
      net,
      connections,
      setupData,
      closestAnimState,
      result);
  }
  else
  {
    finaliseActiveChild0(
      node,
      net,
      connections,
      setupData,
      closestAnimState);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeClosestAnimQueueTransforms(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter)
{
  // Get the closest anim state data
  AttribDataClosestAnimState* stateData = net->getAttribData<AttribDataClosestAnimState>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    node->getNodeID(),
    INVALID_NODE_ID,
    VALID_FRAME_ANY_FRAME,
    ANIMATION_SET_ANY);
  NMP_ASSERT(stateData);

  if (stateData->m_blendCompleteFlag)
  {
    // The blend is complete so just pass through the input source
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }

  // Create a task to perform the blend from the network output transforms to the input source
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_CLOSESTANIM_TRANSFORMS,
                 node->getNodeID(),
                 11,
                 dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);

    // Input transforms of active child
    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Output transforms
    net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    // Animation rig
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    // Sync event tack
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Current playback position in the active child
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Dead blend control weight
    net->TaskAddInputCP(task, 5, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(0));
    // Trajectory delta transform
    net->TaskAddParamAndDependency(task, 6, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Input/Output closest anim state
    net->TaskAddNetInputOutputParam(task, 7, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, ATTRIB_TYPE_CLOSEST_ANIM_STATE, INVALID_NODE_ID, currFrameNo);
    // Dead blend delta time update (network dt)
    net->TaskAddParamAndDependency(task, 8, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, MR::NETWORK_NODE_ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Dead blend transform rates
    net->TaskAddNetInputParam(task, 9, ATTRIB_SEMANTIC_TRANSFORM_RATES, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    // Dead blend input transform state
    net->TaskAddNetInputOutputParam(task, 10, ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeClosestAnimQueueTrajectoryDeltaTransform(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter)
{
  // Get the closest anim state data
  AttribDataClosestAnimState* stateData = net->getAttribData<AttribDataClosestAnimState>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    node->getNodeID(),
    INVALID_NODE_ID,
    VALID_FRAME_ANY_FRAME,
    ANIMATION_SET_ANY);
  NMP_ASSERT(stateData);
  
  if (stateData->m_blendCompleteFlag)
  {
    // The blend is complete so just pass through the input source
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }

  // Create a task to perform the blend from the network output transforms to the input source
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_CLOSESTANIM_TRAJECTORYDELTA,
                 node->getNodeID(),
                 5,
                 dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);
    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);

    // Input trajectory delta transform
    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Output trajectory delta transform
    net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
    // Sync event track
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Current playback position in the active child
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Input closest anim state
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, ATTRIB_TYPE_CLOSEST_ANIM_STATE, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, VALID_FRAME_ANY_FRAME);
  }

  return task;
}
//----------------------------------------------------------------------------------------------------------------------
Task* nodeClosestAnimQueueTrajectoryDeltaAndTransforms(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // Get the closest anim state data
  AttribDataClosestAnimState* stateData = net->getAttribData<AttribDataClosestAnimState>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    node->getNodeID(),
    INVALID_NODE_ID,
    VALID_FRAME_ANY_FRAME,
    ANIMATION_SET_ANY);
  NMP_ASSERT(stateData);

  if (stateData->m_blendCompleteFlag)
  {
    // The blend is complete so just pass through the input source
    return queuePassThroughChild0(node, queue, net, dependentParameter);
  }

  // Create a task to perform the blend from the network output transforms to the input source
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_CLOSESTANIM_TRAJECTORY_DELTA_AND_TRANSFORMS,
    node->getNodeID(),
    10,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);

    // Input transforms of active child
    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Output transforms
    net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    // Animation rig
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    // Sync event tack
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Current playback position in the active child
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, activeChildNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Dead blend control weight
    net->TaskAddInputCP(task, 5, ATTRIB_SEMANTIC_CP_FLOAT, node->getInputCPConnection(0));
    // Input/Output closest anim state
    net->TaskAddNetInputOutputParam(task, 6, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, ATTRIB_TYPE_CLOSEST_ANIM_STATE, INVALID_NODE_ID, currFrameNo);
    // Dead blend delta time update (network dt)
    net->TaskAddParamAndDependency(task, 7, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, MR::NETWORK_NODE_ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    // Dead blend transform rates
    net->TaskAddNetInputParam(task, 8, ATTRIB_SEMANTIC_TRANSFORM_RATES, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    // Dead blend input transform state
    net->TaskAddNetInputOutputParam(task, 9, ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}


//----------------------------------------------------------------------------------------------------------------------
NodeID nodeClosestAnimFindGeneratingNodeForSemantic(
  NodeID              NMP_UNUSED(callingNodeID),
  bool                fromParent, // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            net)
{
  NodeID result = INVALID_NODE_ID;
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());

  // Transforms and trajectory delta tasks (for root rotation blending)
  if ((semantic == ATTRIB_SEMANTIC_TRANSFORM_BUFFER) ||
      (semantic == ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM))
  {
    // Get the closest anim setup data
    AttribDataClosestAnimDef* setupData = net->getNetworkDef()->getAttribData<AttribDataClosestAnimDef>(
      ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,
      node->getNodeID());
    NMP_ASSERT(setupData);

    if (setupData->m_useRootRotationBlending)
      return node->getNodeID();
  }

  // Pass on to parent or child appropriately.
  if (fromParent)
  {
    NMP_ASSERT(connections->m_numActiveChildNodes == 1);
    result = net->nodeFindGeneratingNodeForSemantic(connections->m_activeChildNodeIDs[0], semantic, fromParent, node->getNodeID());
  }
  else
  {
    // While performing sub-network evaluation, prevent querying the parent nodes for their capabilities
    // to process update time by sync events. The update time attribute is explicitly placed on this
    // node during the sub-network evaluation. Return INVALID_NODE_ID to ensure that time is processed by
    // update time rather than by update sync events.
    AttribDataClosestAnimState* closestAnimState = net->getAttribData<AttribDataClosestAnimState>(
      ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
      node->getNodeID(),
      INVALID_NODE_ID,
      VALID_FRAME_ANY_FRAME,
      ANIMATION_SET_ANY);
    if (!closestAnimState->m_isPerformingSubNetworkEval)
    {
      // We are not performing the sub-network evaluation to just pass the request to our parent
      result = net->nodeFindGeneratingNodeForSemantic(connections->m_activeParentNodeID, semantic, fromParent, node->getNodeID());
    }
  }

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
