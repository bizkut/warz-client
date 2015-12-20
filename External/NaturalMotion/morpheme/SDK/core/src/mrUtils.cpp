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
#include "morpheme/mrUtils.h"
#include "morpheme/mrNetwork.h"
#include "NMPlatform/NMProfiler.h"
#include "morpheme/mrBlendOps.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
uint32_t computeNetworkWorldTransforms(
  Network*            net,                 // Network to compute world transforms of.
  const NMP::Vector3& rootPos,             // Position to set the root of the hierarchy to.
  const NMP::Quat&    rootQuat,            // Attitude to set the root of the hierarchy to.
  NMP::DataBuffer*    outputBuffer)        // Where to put results.
{
  NMP_ASSERT(net && outputBuffer);
  const uint32_t numTransforms = net->getCurrentTransformCount();
  NMP::DataBuffer* sourceBuffer = net->getTransforms();

  if (sourceBuffer && numTransforms > 0)
  {
    NM_BEGIN_PROFILING("computeNetworkWorldTransforms: NMP::DataBuffer");

    // Accumulate world transforms.
    const MR::AnimRigDef* rig = net->getActiveRig();

    MR::BlendOpsBase::applyBindPoseToUnusedChannels(rig->getBindPose(), sourceBuffer);
    MR::BlendOpsBase::accumulateTransforms(
      rootPos,
      rootQuat,
      sourceBuffer,
      rig,
      outputBuffer);

    NM_END_PROFILING(); // "computeNetworkWorldTransforms: NMP::DataBuffer"

    return outputBuffer->getLength();
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t computeNetworkWorldTransforms(
  Network*            net,                    // Network to compute world transforms of.
  const NMP::Vector3& rootPos,                // Position to set the root of the hierarchy to.
  const NMP::Quat&    rootQuat,               // Attitude to set the root of the hierarchy to.
  uint32_t            NMP_USED_FOR_ASSERTS(numOutputBufferEntries), // Size of the output buffer.
  NMP::PosQuat*       outputBuffer)           // Where to put results.
{
  NMP_ASSERT(net && outputBuffer);
  const uint32_t resultTransformCount = net->getCurrentTransformCount();
  const MR::AnimRigDef* rig = net->getActiveRig();
  NMP::DataBuffer* sourceBuffer = net->getTransforms();

  if (resultTransformCount > 0)
  {
    NM_BEGIN_PROFILING("computeNetworkWorldTransforms: NMP::PosQuat");

    NMP_ASSERT(numOutputBufferEntries >= resultTransformCount);

    if (sourceBuffer)
    {
      // Allocate a pos quat buffer to accumulate world transforms into.
      // Use the networks temporary memory allocator for the buffer
      NMP::TempMemoryAllocator* allocator = net->getTempMemoryAllocator();
      NMP_ASSERT(allocator->getUsedBytes() == 0);
      NMP::TempMemoryAllocator* childAllocator = allocator->createChildAllocator();

      NMP::Memory::Format buffMemReqs = NMP::DataBuffer::getPosQuatMemoryRequirements(resultTransformCount);
      NMP::DataBuffer* tempBuffer = NMP::DataBuffer::createPosQuat(childAllocator, buffMemReqs, resultTransformCount);

      // Accumulate world transforms.
      MR::BlendOpsBase::applyBindPoseToUnusedChannels(rig->getBindPose(), sourceBuffer);
      MR::BlendOpsBase::accumulateTransforms(
        rootPos,
        rootQuat,
        sourceBuffer,
        rig,
        tempBuffer);

      // Copy pos and quat components from working buffer to output.
      for (uint32_t i = 0; i < resultTransformCount; i++)
      {
        outputBuffer[i].m_pos = *(tempBuffer->getPosQuatChannelPos(i));
        outputBuffer[i].m_quat = *(tempBuffer->getPosQuatChannelQuat(i));
      }

      childAllocator->memFree(tempBuffer);
      allocator->destroyChildAllocator(childAllocator);
    }
    else
    {
      // A source buffer doesn't exist so use the rig bind pose

      // Set the root transform of the output buffer.
      outputBuffer[0].m_pos = rootPos;
      outputBuffer[0].m_quat = rootQuat;

      // Copy pos and quat components from the bind pose buffer to output.
      for (uint32_t i = 1; i < resultTransformCount; ++i)
      {
        const int32_t parentIndex = rig->getParentBoneIndex(i);

        if (parentIndex != MR::AnimRigDef::NO_PARENT_BONE)
        {
          NMP::Vector3 rotatedBonePos = outputBuffer[parentIndex].m_quat.rotateVector(*(rig->getBindPoseBonePos(i)));
          outputBuffer[i].m_pos = outputBuffer[parentIndex].m_pos + rotatedBonePos;
          outputBuffer[i].m_quat = outputBuffer[parentIndex].m_quat * (*(rig->getBindPoseBoneQuat(i)));
        }
        else
        {
          outputBuffer[i].m_pos = *rig->getBindPoseBonePos(i);
          outputBuffer[i].m_quat = *rig->getBindPoseBoneQuat(i);
        }
      }
    }


    NM_END_PROFILING(); // "computeNetworkWorldTransforms: NMP::PosQuat"

    return resultTransformCount;
  }

  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t computeNetworkWorldTransforms(
  Network*            net,                    // Network to compute world transforms of.
  const NMP::Vector3& rootPos,                // Position to set the root of the hierarchy to.
  const NMP::Quat&    rootQuat,               // Attitude to set the root of the hierarchy to.
  uint32_t            NMP_USED_FOR_ASSERTS(numOutputBufferEntries), // Size of the output buffer.
  NMP::Matrix34*      outputBuffer)           // Where to put results.
{
  NMP_ASSERT(net && outputBuffer);
  const uint32_t resultTransformCount = net->getCurrentTransformCount();
  NMP::DataBuffer* sourceBuffer = net->getTransforms();

  if (sourceBuffer && resultTransformCount > 0)
  {
    NM_BEGIN_PROFILING("computeNetworkWorldTransforms: NMP::Matrix34");

    NMP_ASSERT(numOutputBufferEntries >= resultTransformCount);

    // Allocate a pos quat buffer to accumulate world transforms into.
    // Use the networks temporary memory allocator for the buffer
    NMP::TempMemoryAllocator* allocator = net->getTempMemoryAllocator();
    NMP_ASSERT(allocator->getUsedBytes() == 0);
    NMP::TempMemoryAllocator* childAllocator = allocator->createChildAllocator();

    NMP::Memory::Format buffMemReqs = NMP::DataBuffer::getPosQuatMemoryRequirements(resultTransformCount);
    NMP::DataBuffer* worldPosQuatBuffer = NMP::DataBuffer::createPosQuat(childAllocator, buffMemReqs, resultTransformCount);

    // The input buffer is not guaranteed to be aligned properly, so allocate a temporary one
    // to be used in simd computation (which requires 16 alignment).
    NMP::Matrix34* tempTargetBuffer = (NMP::Matrix34*)NMPAllocatorMemAlloc(
      childAllocator, sizeof(NMP::Matrix34) * resultTransformCount, NMP_VECTOR_ALIGNMENT);
    NMP_ASSERT(tempTargetBuffer);

    // Accumulate world transforms.
    const MR::AnimRigDef* rig = net->getActiveRig();
    MR::BlendOpsBase::applyBindPoseToUnusedChannels(rig->getBindPose(), sourceBuffer);
    MR::BlendOpsBase::accumulateTransforms(
      rootPos,
      rootQuat,
      sourceBuffer,
      rig,
      worldPosQuatBuffer);

    // Generate output world transform matrices from working world space pos quat buffer.
    for (uint32_t i = 0; i < sourceBuffer->getLength(); i++)
    {
#if defined(NMP_PLATFORM_SIMD) && !defined(NM_HOST_CELL_PPU) && !defined(NM_HOST_CELL_SPU)
      NMP::vpu::tmFromQuatPos(
        (float*)&tempTargetBuffer[i],
        ((NMP::Quat*)worldPosQuatBuffer->getElementData(1))[i],
        ((NMP::Vector3*)worldPosQuatBuffer->getElementData(0))[i]);
#else
      tempTargetBuffer[i].initialise(*worldPosQuatBuffer->getPosQuatChannelQuat(i), *worldPosQuatBuffer->getPosQuatChannelPos(i));
#endif
    }

    memcpy(outputBuffer, tempTargetBuffer, sizeof(NMP::Matrix34) * resultTransformCount);

    // Deallocate working memory.
    allocator->memFree(worldPosQuatBuffer);
    allocator->memFree(tempTargetBuffer);
    allocator->destroyChildAllocator(childAllocator);

    NM_END_PROFILING(); // "computeNetworkWorldTransforms: NMP::Matrix34"

    return resultTransformCount;
  }

  return 0;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
