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
#include "NMPlatform/NMBuffer.h"
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMVector3.h"
#include "morpheme/mrBlendOps.h"
#include "NMPlatform/NMProfiler.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef NMP_PLATFORM_SIMD
  #include "mrBlendOpsSIMD.cpp"
#endif

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::accumulateTransform(
  int32_t                boneIndex,
  const NMP::Vector3&    rootPos,
  const NMP::Quat&       rootQuat,
  const NMP::DataBuffer* sourceBuffer,
  const AnimRigDef*      rig,
  NMP::Vector3&          bonePosOut,
  NMP::Quat&             boneQuatOut)
{
  NMP_ASSERT(sourceBuffer);
  NMP_ASSERT(rig);
  NMP_ASSERT(sourceBuffer->getLength() == rig->getNumBones());
  const NMP::Hierarchy* hierarchy = rig->getHierarchy();
  NMP_ASSERT(boneIndex < (int32_t)hierarchy->getNumEntries());
  const AttribDataTransformBuffer* bindPoseBufferAttrib = rig->getBindPose();
  NMP_ASSERT(bindPoseBufferAttrib);
  const NMP::DataBuffer* bindPoseBuffer = bindPoseBufferAttrib->m_transformBuffer;
  NMP_ASSERT(bindPoseBuffer);

  // Bind pose channels
  const NMP::Vector3* bindChannelPos = bindPoseBuffer->getPosQuatChannelPos(0);
  const NMP::Quat* bindChannelAtt = bindPoseBuffer->getPosQuatChannelQuat(0);
  
  // Source buffer channels
  const NMP::Vector3* srcChannelPos = sourceBuffer->getPosQuatChannelPos(0);
  const NMP::Quat* srcChannelAtt = sourceBuffer->getPosQuatChannelQuat(0);

  // Accumulate the bone transform
  bonePosOut.setToZero();
  boneQuatOut.identity();
  while (boneIndex > 0)
  {
    NMP::Vector3 localPos;
    NMP::Quat localAtt;
    if (sourceBuffer->hasChannel(boneIndex))
    {
      localPos = srcChannelPos[boneIndex];
      localAtt = srcChannelAtt[boneIndex];
    }
    else
    {
      localPos = bindChannelPos[boneIndex];
      localAtt = bindChannelAtt[boneIndex];
    }

    boneQuatOut = localAtt * boneQuatOut;
    bonePosOut = localAtt.rotateVector(bonePosOut) + localPos;

    boneIndex = hierarchy->getParentIndex(boneIndex);
  }
  
  // Apply the root transform
  boneQuatOut = rootQuat * boneQuatOut;
  bonePosOut = rootQuat.rotateVector(bonePosOut) + rootPos;
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::accumulateTransformTM(
  int32_t                boneIndex,
  const NMP::Matrix34&   rootTM,
  const NMP::DataBuffer* sourceBuffer,
  const AnimRigDef*      rig,
  NMP::Matrix34&         boneTMOut)
{
  NMP_ASSERT(rig && sourceBuffer);
  NMP_ASSERT(rig->getHierarchy());
  NMP_ASSERT((boneIndex == -1) || (sourceBuffer->getLength() > (uint32_t)boneIndex));
  NMP_ASSERT(sourceBuffer->getLength() == rig->getNumBones());
  NMP_ASSERT(rig->getHierarchy()->getParentIndex(0) == -1);

  int32_t parentIndex = rig->getParentBoneIndex(boneIndex);
  if (parentIndex == -1)
  {
    boneTMOut = rootTM;
  }
  else
  {
    NMP::Matrix34 accumParentTM;
    accumulateTransformTM(parentIndex, rootTM, sourceBuffer, rig, accumParentTM);
#if defined(NMP_PLATFORM_SIMD) && !defined(NM_HOST_CELL_PPU) && !defined(NM_HOST_CELL_SPU)
    NMP::Matrix34 sourceTM;
    NMP::vpu::tmFromQuatPos((float*)&sourceTM,
                            ((NMP::Quat*)sourceBuffer->getElementData(1))[boneIndex],
                            ((NMP::Vector3*)sourceBuffer->getElementData(0))[boneIndex]);
#else
    NMP::Matrix34 sourceTM(*sourceBuffer->getPosQuatChannelQuat(boneIndex), *sourceBuffer->getPosQuatChannelPos(boneIndex));
#endif
    boneTMOut = sourceTM * accumParentTM;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::applyBindPoseToUnusedChannels(
  const AttribDataTransformBuffer* bindPose,
  NMP::DataBuffer*                 transformBuffer)
{
  NMP_ASSERT(bindPose);
  NMP_ASSERT(transformBuffer);
  NMP_ASSERT(transformBuffer->getLength() == bindPose->m_transformBuffer->getLength());

  // early out
  if(transformBuffer->isFull())
  {
    return;
  }

  NMP::Vector3* localPos = transformBuffer->getPosQuatChannelPos(0);
  NMP::Quat* localQuat = transformBuffer->getPosQuatChannelQuat(0);
  NMP::BitArray* usedFlags = transformBuffer->getUsedFlags();

  for (uint32_t i = 0; i < bindPose->m_transformBuffer->getLength(); ++i)
  {
    if (!usedFlags->isBitSet(i))
    {
      localPos[i] = *(bindPose->m_transformBuffer->getPosQuatChannelPos(i)); 
      localQuat[i] = *(bindPose->m_transformBuffer->getPosQuatChannelQuat(i)); 
    }
  }
  usedFlags->setAll();
  transformBuffer->setFullFlag(true);
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::filterUsedChannels(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1)
{
  uint32_t numRigJoints = destBuffer->getLength();
  NMP_ASSERT(sourceBuffer0->getLength() == numRigJoints);
  NMP_ASSERT(sourceBuffer1->getLength() == numRigJoints);
  NMP::Vector3* destPos = destBuffer->getPosQuatChannelPos(0);
  NMP::Quat* destQuat = destBuffer->getPosQuatChannelQuat(0);
  NMP::BitArray* destFlags = destBuffer->getUsedFlags();
  const NMP::Vector3* sourcePos0 = sourceBuffer0->getPosQuatChannelPos(0);
  const NMP::Quat* sourceQuat0 = sourceBuffer0->getPosQuatChannelQuat(0);
  const NMP::BitArray* sourceFlags0 = sourceBuffer0->getUsedFlags();
  const NMP::Vector3* sourcePos1 = sourceBuffer1->getPosQuatChannelPos(0);
  const NMP::Quat* sourceQuat1 = sourceBuffer1->getPosQuatChannelQuat(0);
  const NMP::BitArray* sourceFlags1 = sourceBuffer1->getUsedFlags();

  // Iterate over channels individually so that the destination buffer
  // can point to either of the source buffers for in-place filtering
  for (uint32_t i = 0; i < numRigJoints; ++i)
  {
    if (sourceFlags0->isBitSet(i))
    {
      destPos[i] = sourcePos0[i];
      destQuat[i] = sourceQuat0[i];
      destFlags->setBit(i);
    }
    else if (sourceFlags1->isBitSet(i))
    {
      destPos[i] = sourcePos1[i];
      destQuat[i] = sourceQuat1[i];
      destFlags->setBit(i);
    }
    else
    {
      destFlags->clearBit(i);
    }
  }

  destBuffer->calculateFullFlag();
}

//----------------------------------------------------------------------------------------------------------------------
// Accumulate the full set of rig transforms against the given rig hierarchy.
void BlendOpsBase::accumulateTransforms(
  const NMP::Vector3&    rootPos,
  const NMP::Quat&       rootQuat,
  const NMP::DataBuffer* sourceBuffer,
  const AnimRigDef*      rig,
  NMP::DataBuffer*       outBuffer)
{
  // if sourceBuffer may not be full, use applyBindPoseToUnusedChannels before calling accumulateTransforms
  // alternately, accumulateTransformsPartial can be used
  NMP_ASSERT(sourceBuffer->isFull());

  int32_t parentIndex;
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_accumulateTransforms");

  NMP_ASSERT(rig && sourceBuffer && outBuffer);
  NMP_ASSERT(rig->getHierarchy());
  NMP_ASSERT(sourceBuffer->getLength() >= rig->getNumBones());
  NMP_ASSERT(outBuffer->getLength() >= rig->getNumBones());
  NMP_ASSERT(rig->getHierarchy()->getParentIndex(0) == -1);
  const NMP::Vector3* srcChannelPos = sourceBuffer->getPosQuatChannelPos(0);
  const NMP::Quat* srcChannelAtt = sourceBuffer->getPosQuatChannelQuat(0);
  NMP::Vector3* dstChannelPos = outBuffer->getPosQuatChannelPos(0);
  NMP::Quat* dstChannelAtt = outBuffer->getPosQuatChannelQuat(0);

  // Set the root transform of the output buffer.
  dstChannelPos[0] = rootPos;
  dstChannelAtt[0] = rootQuat;

  uint32_t boneNum = rig->getNumBones();
  for (uint32_t i = 1; i < boneNum; ++i)
  {
    parentIndex = rig->getParentBoneIndex(i);
    NMP_ASSERT(parentIndex < (int32_t) i);

#if defined(NMP_PLATFORM_SIMD) && !defined(NM_HOST_CELL_PPU) && !defined(NM_HOST_CELL_SPU)
    NMP::vpu::prefetchCacheLine(&dstChannelPos[i], 0);
    NMP::vpu::prefetchCacheLine(&dstChannelAtt[i], 0);
    NMP::vpu::vector4_t pPos  = NMP::vpu::load4f((float*) &dstChannelPos[parentIndex]);
    NMP::vpu::vector4_t pQuat = NMP::vpu::load4f((float*) &dstChannelAtt[parentIndex]);
    NMP::vpu::vector4_t sPos  = NMP::vpu::load4f((float*) &srcChannelPos[i]);
    NMP::vpu::vector4_t sQuat = NMP::vpu::load4f((float*) &srcChannelAtt[i]);

    // oPos = pPos + pQuat.rotate(sPos);
    NMP::vpu::store4f((float*) &dstChannelPos[i], NMP::vpu::add4f(pPos, NMP::vpu::quatRotVec(pQuat, sPos)));
    // oQuat = pQuat * sQuat;
    NMP::vpu::store4f((float*) &dstChannelAtt[i], NMP::vpu::qqMul(pQuat, sQuat));
#else
    dstChannelPos[i] = dstChannelPos[parentIndex] + dstChannelAtt[parentIndex].rotateVector(srcChannelPos[i]);
    dstChannelAtt[i] = dstChannelAtt[parentIndex] * srcChannelAtt[i];
#endif
  }

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::accumulateTransformsPartial(
  const NMP::Vector3&    rootPos,
  const NMP::Quat&       rootQuat,
  const NMP::DataBuffer* sourceBuffer,
  const AnimRigDef*      rig,
  uint32_t               numEntries,
  const uint16_t*        rigChannels,
  bool*                  validRigChannelFlags,
  NMP::Vector3*          outChannelPos,
  NMP::Quat*             outChannelAtt)
{
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_accumulateTransformsPartial");

  NMP_ASSERT(sourceBuffer);
  NMP_ASSERT(rig);
  NMP_ASSERT(rig->getHierarchy());
  NMP_ASSERT(numEntries > 0);
  NMP_ASSERT(rigChannels);
  NMP_ASSERT(validRigChannelFlags);
  NMP_ASSERT(outChannelPos);
  NMP_ASSERT(outChannelAtt);
  NMP_ASSERT(sourceBuffer->getLength() == rig->getNumBones());
  NMP_ASSERT(rig->getHierarchy()->getParentIndex(0) == -1);
  const NMP::BitArray* usedFlags = sourceBuffer->getUsedFlags();
  const NMP::Vector3* srcChannelPos = sourceBuffer->getPosQuatChannelPos(0);
  const NMP::Quat* srcChannelAtt = sourceBuffer->getPosQuatChannelQuat(0);
  uint32_t numRigBones = rig->getNumBones();
  const AttribDataTransformBuffer* bindPoseBuffer = rig->getBindPose();
  NMP_ASSERT(bindPoseBuffer);
  const NMP::Vector3* bindPosePos = bindPoseBuffer->m_transformBuffer->getPosQuatChannelPos(0);
  const NMP::Quat* bindPoseAtt = bindPoseBuffer->m_transformBuffer->getPosQuatChannelQuat(0);

  // Set the root transform of the output buffer.
  outChannelPos[0] = rootPos;
  outChannelAtt[0] = rootQuat;
  validRigChannelFlags[0] = true;
  
  // Invalidate the rig channel flags
  for (uint32_t i = 1; i < numRigBones; ++i)
    validRigChannelFlags[i] = false;
    
  // Set the required output channels with the local space transforms of the
  // source buffer. Substitute any missing channels in the source buffer with
  // transforms from the bind pose.
  for (uint32_t entryIndex = 0; entryIndex < numEntries; ++entryIndex)
  {
    int32_t curRigIndex = rigChannels[entryIndex];
    NMP_ASSERT(curRigIndex > 0);

    while (curRigIndex > 0 && !validRigChannelFlags[curRigIndex])
    {
      if (usedFlags->isBitSet(curRigIndex))
      {
        outChannelPos[curRigIndex] = srcChannelPos[curRigIndex];
        outChannelAtt[curRigIndex] = srcChannelAtt[curRigIndex];
      }
      else
      {
        outChannelPos[curRigIndex] = bindPosePos[curRigIndex];
        outChannelAtt[curRigIndex] = bindPoseAtt[curRigIndex];
      }
      validRigChannelFlags[curRigIndex] = true;
      curRigIndex = rig->getParentBoneIndex(curRigIndex);
    }
  }
  
  // Accumulate the transforms
  for (uint32_t i = 1; i < numRigBones; ++i)
  {
    if (validRigChannelFlags[i])
    {
      int32_t parentIndex = rig->getParentBoneIndex(i);
      NMP_ASSERT(parentIndex >= 0);
      NMP_ASSERT(validRigChannelFlags[parentIndex]);
      
#if defined(NMP_PLATFORM_SIMD) && !defined(NM_HOST_CELL_PPU) && !defined(NM_HOST_CELL_SPU)
      NMP::vpu::prefetchCacheLine(&outChannelPos[i], 0);
      NMP::vpu::prefetchCacheLine(&outChannelAtt[i], 0);
      NMP::vpu::vector4_t pPos  = NMP::vpu::load4f((float*) &outChannelPos[parentIndex]);
      NMP::vpu::vector4_t pQuat = NMP::vpu::load4f((float*) &outChannelAtt[parentIndex]);
      NMP::vpu::vector4_t sPos  = NMP::vpu::load4f((float*) &outChannelPos[i]);
      NMP::vpu::vector4_t sQuat = NMP::vpu::load4f((float*) &outChannelAtt[i]);
      // oPos = pPos + pQuat.rotate(sPos);
      NMP::vpu::store4f((float*) &outChannelPos[i], NMP::vpu::add4f(pPos, NMP::vpu::quatRotVec(pQuat, sPos)));
      // oQuat = pQuat * sQuat;
      NMP::vpu::store4f((float*) &outChannelAtt[i], NMP::vpu::qqMul(pQuat, sQuat));
#else
      outChannelPos[i] = outChannelPos[parentIndex] + outChannelAtt[parentIndex].rotateVector(outChannelPos[i]);
      outChannelAtt[i] = outChannelAtt[parentIndex] * outChannelAtt[i];
#endif
    }
  }

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::interpQuatInterpPosPartial(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha)
{
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_interpQuatInterpPosPartial");
  
  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer0->getLength());
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer1->getLength());

  uint32_t  numElements = destBuffer->getLength();
  float     clampedAlpha;
  bool      isFull = true;
  uint32_t  channelID = 0;
  
  // Weighting must be limited between 0.0 and 1.0
  clampedAlpha = NMP::clampValue(alpha, 0.0f, 1.0f);

  destBuffer->getUsedFlags()->clearAll();

  // Blend up to the smallest buffer count
  while (channelID < numElements)
  {
    if (sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID))
    {
      // Both buffers have this channel so interpolate.
      interpQuatChannelMatching(destBuffer, channelID, sourceBuffer0, sourceBuffer1, clampedAlpha);

      // Both buffers have this channel so interpolate.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID].lerp(
        ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID],
        ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID],
        clampedAlpha);
    }
    else if (sourceBuffer0->hasChannel(channelID))
    {
      // Only buffer 0 has this channel so copy it into the output buffer.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    else if (sourceBuffer1->hasChannel(channelID))
    {
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    else
    {
      isFull = false;
    }
    channelID++;
  }

  destBuffer->setFullFlag(isFull);

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::interpQuatAddPosPartial(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha)
{
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_interpQuatAddPosPartial");

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();
  float     clampedAlpha;
  bool      isFull = true;

  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer0->getLength());
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer1->getLength());

  // Weighting for quat interp must be limited between 0.0 and 1.0
  clampedAlpha = NMP::clampValue(alpha, 0.0f, 1.0f);

  destBuffer->getUsedFlags()->clearAll();

  // Blend up to the smallest buffer count
  while (channelID < numElements)
  {
    if (sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID))
    {
      // Both buffers have this channel so interpolate.
      interpQuatChannelMatching(destBuffer, channelID, sourceBuffer0, sourceBuffer1, clampedAlpha);

      // Both buffers have this channel so interpolate.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] =
            ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]
            + ((((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]) * clampedAlpha);
    }
    else if (sourceBuffer0->hasChannel(channelID))
    {
      // Only buffer 0 has this channel so copy it into the output buffer.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    else if (sourceBuffer1->hasChannel(channelID))
    {
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    else
    {
      isFull = false;
    }
    channelID++;
  }

  destBuffer->setFullFlag(isFull);

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::addQuatAddPosPartial(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha)
{
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_addQuatAddPosPartial");

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();

  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() >= sourceBuffer0->getLength());

  destBuffer->getUsedFlags()->clearAll();

  // Add
  while (channelID < numElements)
  {
    if (sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID))
    {
      // Both buffers have this channel so interpolate
      addQuatChannelMatching(destBuffer, channelID, sourceBuffer0, sourceBuffer1, alpha);

      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] =
        ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]
        + ((((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]) * alpha);
    }
    else if (sourceBuffer0->hasChannel(channelID))
    {
      // Only buffer 0 has this channel so copy it into the output buffer.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    channelID++;
  }

  destBuffer->calculateFullFlag();

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::addQuatLeavePosPartial(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha)
{
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_addQuatLeavePosPartial");

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();

  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() >= sourceBuffer0->getLength());

  destBuffer->getUsedFlags()->clearAll();

  // Add
  while (channelID < numElements)
  {
    if (sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID))
    {
      // Both buffers have this channel so interpolate
      addQuatChannelMatching(destBuffer, channelID, sourceBuffer0, sourceBuffer1, alpha);
    }
    else if (sourceBuffer0->hasChannel(channelID))
    {
      // Only buffer 0 has this channel so copy it into the output buffer.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    channelID++;
  }

  destBuffer->calculateFullFlag();

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::subtractQuatSubtractPosPartial(
  NMP::DataBuffer*        destBuffer,
  const NMP::DataBuffer*  sourceBuffer0,
  const NMP::DataBuffer*  sourceBuffer1,
  const float             alpha /*/< Global blend weight. */ )
{
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_subtractQuatSubtractPosPartial");

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();

  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() >= sourceBuffer0->getLength());

  destBuffer->getUsedFlags()->clearAll();

  // Subtract
  while (channelID < numElements)
  {
    if (sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID))
    {
      // Both buffers have this channel so interpolate
      subtractQuatChannelMatching(destBuffer, channelID, sourceBuffer0, sourceBuffer1, alpha);

      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] =
        ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID] - 
        ((((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]) * alpha);
    }
    else if (sourceBuffer0->hasChannel(channelID))
    {
      // Only buffer 0 has this channel so copy it into the output buffer.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    channelID++;
  }

  destBuffer->calculateFullFlag();

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::subtractQuatInterpPosPartial(
  NMP::DataBuffer*        destBuffer,
  const NMP::DataBuffer*  sourceBuffer0,
  const NMP::DataBuffer*  sourceBuffer1,
  const float             alpha /*/< Global blend weight. */ )
{
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_subtractQuatInterpPosPartial");

  uint32_t  channelID = 1; // 0 never used...
  uint32_t  numElements = destBuffer->getLength();
  float     clampedAlpha;
  bool      isFull = true;

  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer0->getLength());
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer1->getLength());

  // Weighting must be limited between 0.0 and 1.0
  clampedAlpha = NMP::clampValue(alpha, 0.0f, 1.0f);

  destBuffer->getUsedFlags()->clearAll();

  // Blend up to the smallest buffer count
  while (channelID < numElements)
  {
    if (sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID))
    {
      // Both buffers have this channel so subtract.
      subtractQuatChannelMatching(destBuffer, channelID, sourceBuffer0, sourceBuffer1, clampedAlpha);

      // Both buffers have this channel so interpolate.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID].lerp(
        ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID],
        ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID],
        clampedAlpha);
    }
    else if (sourceBuffer0->hasChannel(channelID))
    {
      // Only buffer 0 has this channel so copy it into the output buffer.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    else if (sourceBuffer1->hasChannel(channelID))
    {
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    else
    {
      isFull = false;
    }
    channelID++;
  }

  destBuffer->setFullFlag(isFull);

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::interpQuatSubtractPosPartial(
  NMP::DataBuffer*        destBuffer,
  const NMP::DataBuffer*  sourceBuffer0,
  const NMP::DataBuffer*  sourceBuffer1,
  const float             alpha /*/< Global blend weight. */ )
{
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_interpQuatSubtractPosPartial");

  uint32_t  channelID = 1; // 0 never used...
  uint32_t  numElements = destBuffer->getLength();
  float     clampedAlpha;
  bool      isFull = true;

  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer0->getLength());
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer1->getLength());

  // Weighting must be limited between 0.0 and 1.0
  clampedAlpha = NMP::clampValue(alpha, 0.0f, 1.0f);

  destBuffer->getUsedFlags()->clearAll();

  // Blend up to the smallest buffer count
  while (channelID < numElements)
  {
    if (sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID))
    {
      // Both buffers have this channel so interpolate.
      interpQuatChannelMatching(destBuffer, channelID, sourceBuffer0, sourceBuffer1, clampedAlpha);

      // Both buffers have this channel so subtract.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] =
        ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID] - 
        ((((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]) * clampedAlpha);
    }
    else if (sourceBuffer0->hasChannel(channelID))
    {
      // Only buffer 0 has this channel so copy it into the output buffer.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    else if (sourceBuffer1->hasChannel(channelID))
    {
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    else
    {
      isFull = false;
    }
    channelID++;
  }

  destBuffer->setFullFlag(isFull);

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::interpQuatAddPosPartialFeathered(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha,
  const uint32_t         numAlphaValues,
  const float*           alphaValues)
{
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_interpQuatAddPosPartialFeathered");

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();
  float     clampedAlpha;
  float     boneAlphaScale;

  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1 && alphaValues);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer0->getLength());
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer1->getLength());

  // Weighting for quat interp must be limited between 0.0 and 1.0
  clampedAlpha = NMP::clampValue(alpha, 0.0f, 1.0f);

  destBuffer->getUsedFlags()->clearAll();

  // Blend up to the smallest buffer count
  while (channelID < numElements)
  {
    if (sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID))
    {
      // Both buffers have this channel so interpolate.
      if (numAlphaValues > channelID)
        boneAlphaScale = alphaValues[channelID];
      else
        boneAlphaScale = 1.0f;

      interpQuatChannelMatching(destBuffer, channelID, sourceBuffer0, sourceBuffer1, clampedAlpha * boneAlphaScale);

      // Both buffers have this channel so interpolate.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] =
        ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]
        + ((((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]) * clampedAlpha * boneAlphaScale);
    }
    else if (sourceBuffer0->hasChannel(channelID))
    {
      // Only buffer 0 has this channel so copy it into the output buffer.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    else if (sourceBuffer1->hasChannel(channelID))
    {
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    channelID++;
  }

  destBuffer->calculateFullFlag();

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::interpQuatInterpPosPartialFeathered(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha,
  const uint32_t         numAlphaValues,
  const float*           alphaValues)
{
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_interpQuatAddPosPartialFeathered");

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();
  float     clampedAlpha;
  float     boneAlphaScale;

  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1 && alphaValues);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer0->getLength());
  NMP_ASSERT(destBuffer->getLength() == sourceBuffer1->getLength());

  // Weighting must be limited between 0.0 and 1.0
  clampedAlpha = NMP::clampValue(alpha, 0.0f, 1.0f);

  destBuffer->getUsedFlags()->clearAll();

  // Blend up to the smallest buffer count
  while (channelID < numElements)
  {
    if (sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID))
    {
      // Both buffers have this channel so interpolate.
      if (numAlphaValues > channelID)
      {
        boneAlphaScale = alphaValues[channelID];
      }
      else
      {
        boneAlphaScale = 1.0f;
      }

      interpQuatChannelMatching(destBuffer, channelID, sourceBuffer0, sourceBuffer1, clampedAlpha * boneAlphaScale);

      // Both buffers have this channel so interpolate.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID].lerp(
        ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID],
        ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID],
        clampedAlpha * boneAlphaScale);
    }
    else if (sourceBuffer0->hasChannel(channelID))
    {
      // Only buffer 0 has this channel so copy it into the output buffer.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    else if (sourceBuffer1->hasChannel(channelID))
    {
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer1->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    channelID++;
  }

  destBuffer->calculateFullFlag();

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::addQuatAddPosPartialFeathered(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha,
  const uint32_t         numAlphaValues,
  const float*           alphaValues)
{
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_addQuatAddPosPartialFeathered");

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();
  float     boneAlphaScale;

  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1 && alphaValues);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() >= sourceBuffer0->getLength());

  destBuffer->getUsedFlags()->clearAll();

  // Add
  while (channelID < numElements)
  {
    if (sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID))
    {
      // Both buffers have this channel so interpolate
      if (numAlphaValues > channelID)
      {
        boneAlphaScale = alphaValues[channelID];
      }
      else
      {
        boneAlphaScale = 1.0f;
      }

      addQuatChannelMatching(destBuffer, channelID, sourceBuffer0, sourceBuffer1, alpha * boneAlphaScale);

      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] =
        ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID]
        + ((((NMP::Vector3*)sourceBuffer1->getElementData(0))[channelID]) * alpha * boneAlphaScale);
    }
    else if (sourceBuffer0->hasChannel(channelID))
    {
      // Only buffer 0 has this channel so copy it into the output buffer.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    channelID++;
  }

  destBuffer->calculateFullFlag();

  NM_BLENDOPS_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void BlendOpsBase::addQuatLeavePosPartialFeathered(
  NMP::DataBuffer*       destBuffer,
  const NMP::DataBuffer* sourceBuffer0,
  const NMP::DataBuffer* sourceBuffer1,
  const float            alpha,
  const uint32_t         numAlphaValues,
  const float*           alphaValues)
{
  NM_BLENDOPS_BEGIN_PROFILING("BLENDOPPS_addQuatLeavePosPartialFeathered");

  uint32_t  channelID = 0;
  uint32_t  numElements = destBuffer->getLength();
  float     boneAlphaScale;

  NMP_ASSERT(destBuffer && sourceBuffer0 && sourceBuffer1 && alphaValues);
  NMP_ASSERT((destBuffer != sourceBuffer0) && (destBuffer != sourceBuffer1));
  NMP_ASSERT(destBuffer->getLength() >= sourceBuffer0->getLength());

  destBuffer->getUsedFlags()->clearAll();

  // Add
  while (channelID < numElements)
  {
    if (sourceBuffer0->hasChannel(channelID) && sourceBuffer1->hasChannel(channelID))
    {
      // Both buffers have this channel so interpolate
      if (numAlphaValues > channelID)
        boneAlphaScale = alphaValues[channelID];
      else
        boneAlphaScale = 1.0f;
      addQuatChannelMatching(destBuffer, channelID, sourceBuffer0, sourceBuffer1, alpha * boneAlphaScale);
    }
    else if (sourceBuffer0->hasChannel(channelID))
    {
      // Only buffer 0 has this channel so copy it into the output buffer.
      ((NMP::Vector3*)destBuffer->getElementData(0))[channelID] = ((NMP::Vector3*)sourceBuffer0->getElementData(0))[channelID];
      ((NMP::Quat*)destBuffer->getElementData(1))[channelID] = ((NMP::Quat*)sourceBuffer0->getElementData(1))[channelID];
      destBuffer->getUsedFlags()->setBit(channelID);
    }
    channelID++;
  }

  destBuffer->calculateFullFlag();

  NM_BLENDOPS_END_PROFILING();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
