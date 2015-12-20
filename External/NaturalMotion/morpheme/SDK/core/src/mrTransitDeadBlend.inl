// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Note that a compiler bug(?) results in deltaQuat.fastLog() or log generating invalid data when this 
// function is inlined. MORPH-17524
static void initialiseDeadBlendTransformsImpl(
  AttribDataTransformBuffer* deadBlendTransformsState,
  AttribDataTransformBuffer* deadBlendTransformsRate,
  bool                       useDeadReckoning,
  float                      dt,
  uint32_t                   numRigJoints,
  const NMP::DataBuffer*     prevDataBuffer,
  const NMP::DataBuffer*     prevPrevDataBuffer,
  NMP::DataBuffer*&          deadBlendTransforms,
  NMP::DataBuffer*&          transRates)
{
  // Initialize dead blend transform rates
  transRates = deadBlendTransformsRate->m_transformBuffer;
  NMP_ASSERT(transRates);
  NMP::Vector3* transRatesAngVel = transRates->getPosVelAngVelChannelAngVel(0);
  NMP::Vector3* transRatesVel = transRates->getPosVelAngVelChannelPosVel(0);
  NMP::BitArray* transRatesFlags = transRates->getUsedFlags();

  // Compute the dead blend transform velocities
  if (useDeadReckoning && (dt > 0.0f))
  {
    NMP_ASSERT(prevPrevDataBuffer);
    const NMP::Quat* prevDataBufferQuat = prevDataBuffer->getPosQuatChannelQuat(0);
    const NMP::Quat* prevPrevDataBufferQuat = prevPrevDataBuffer->getPosQuatChannelQuat(0);
    const NMP::Vector3* prevDataBufferPos = prevDataBuffer->getPosQuatChannelPos(0);
    const NMP::Vector3* prevPrevDataBufferPos = prevPrevDataBuffer->getPosQuatChannelPos(0);
    const NMP::BitArray* prevDataBufferFlags = prevDataBuffer->getUsedFlags();
    const NMP::BitArray* prevPrevDataBufferFlags = prevPrevDataBuffer->getUsedFlags();

    for (uint32_t j = 0 ; j < numRigJoints; ++j)
    {
      if (prevDataBufferFlags->isBitSet(j) && prevPrevDataBufferFlags->isBitSet(j))
      {
        NMP::Quat deltaQuat;
        deltaQuat.multiply(prevDataBufferQuat[j], ~prevPrevDataBufferQuat[j]);
        if (deltaQuat.w < 0.0f)
          deltaQuat *= -1.0f; // Ensure in same semi-arc
        transRatesAngVel[j] = (2.0f / dt) * deltaQuat.fastLog(); // NB: VS2010 compiler bug here if this function is inlined
        transRatesVel[j] = (prevDataBufferPos[j] - prevPrevDataBufferPos[j]) / dt;
      }
      else
      {
        transRatesAngVel[j].setToZero();
        transRatesVel[j].setToZero();
      }
    }

    transRatesFlags->setAll();
    transRates->setFullFlag(true);
  }
  else
  {
    // Non dead blend or zero time delta
    for (uint32_t j = 1; j < numRigJoints; ++j)
    {
      transRatesAngVel[j].setToZero();
      transRatesVel[j].setToZero();
    }


    transRatesFlags->setAll();
    transRatesFlags->clearBit(0);
    transRates->setFullFlag(false);
  }

  // Prepare the node specific states to the initial right value.
  deadBlendTransforms = deadBlendTransformsState->m_transformBuffer;
  prevDataBuffer->copyTo(deadBlendTransforms);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void initialiseDeadBlendTransforms(
  Network*                net,
  NodeDef*                node,
  bool                    useDeadReckoning,
  float                   dt,
  const AnimRigDef*       rig,
  const NMP::DataBuffer*  prevDataBuffer,
  const NMP::DataBuffer*  prevPrevDataBuffer,
  NMP::DataBuffer*&       deadBlendTransforms,
  NMP::DataBuffer*&       transRates)
{
  NMP_ASSERT(rig);
  NMP_ASSERT(prevDataBuffer);

  NodeID nodeID = node->getNodeID();
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(nodeID);

  // Create the dead blend transform buffers
  AttribAddress transStateAddr(ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, nodeID, INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  AttribAddress transRateAddr(ATTRIB_SEMANTIC_TRANSFORM_RATES, nodeID, INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);

  // These must not already exist
  NMP_ASSERT(!net->getAttribDataNodeBinEntry(transStateAddr));
  NMP_ASSERT(!net->getAttribDataNodeBinEntry(transRateAddr));

  AttribDataHandle handle;
  const uint32_t numRigJoints = rig->getNumBones();

  //----------------
  NMP::Memory::Format internalBuffMemReqs;
  NMP::Memory::Format buffMemReqs;
  AttribDataTransformBuffer::getPosVelAngVelMemoryRequirements(numRigJoints, buffMemReqs, internalBuffMemReqs);
  AttribDataTransformBuffer* deadBlendTransformsRate = AttribDataTransformBuffer::createPosVelAngVel(
                              net->getPersistentMemoryAllocator(),
                              buffMemReqs,
                              internalBuffMemReqs,
                              numRigJoints);
  handle.set(deadBlendTransformsRate, buffMemReqs);
  transRateAddr.m_animSetIndex = activeAnimSetIndex;
  net->addAttribData(transRateAddr, handle, LIFESPAN_FOREVER);

  //----------------
  AttribDataTransformBuffer::getPosQuatMemoryRequirements(numRigJoints, buffMemReqs, internalBuffMemReqs);
  AttribDataTransformBuffer* deadBlendTransformsState = AttribDataTransformBuffer::createPosQuat(
                               net->getPersistentMemoryAllocator(),
                               buffMemReqs,
                               internalBuffMemReqs,
                               numRigJoints);
  handle.set(deadBlendTransformsState, buffMemReqs);
  transStateAddr.m_animSetIndex = activeAnimSetIndex;
  net->addAttribData(transStateAddr, handle, LIFESPAN_FOREVER);

  initialiseDeadBlendTransformsImpl(
    deadBlendTransformsState,
    deadBlendTransformsRate,
    useDeadReckoning,
    dt,
    numRigJoints,
    prevDataBuffer,
    prevPrevDataBuffer,
    deadBlendTransforms,
    transRates);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void initialiseDeadBlendSeparateTransformsAndTrajectory(
  Network*                            net,
  NodeDef*                            node,
  bool                                useDeadReckoning,
  float                               dt,
  const AnimRigDef*                   rig,
  const NMP::DataBuffer*              prevDataBuffer,
  const NMP::DataBuffer*              prevPrevDataBuffer,
  NMP::DataBuffer*&                   deadBlendTransforms,
  NMP::DataBuffer*&                   transRates,
  AttribDataTrajectoryDeltaTransform* lastDeltaTrajAttr,
  NodeID                              parentSubNetworkID,
  const uint32_t                      currFrameNo)
{
  NMP_ASSERT(rig);
  NMP_ASSERT(prevDataBuffer);

  NodeID nodeID = node->getNodeID();
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(nodeID);

  // Create the dead blend transform buffers
  AttribAddress transStateAddr(ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, nodeID, INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  AttribAddress transRateAddr(ATTRIB_SEMANTIC_TRANSFORM_RATES, nodeID, INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);

  // These must not already exist
  NMP_ASSERT(!net->getAttribDataNodeBinEntry(transStateAddr));
  NMP_ASSERT(!net->getAttribDataNodeBinEntry(transRateAddr));

  AttribDataHandle handle;
  const uint32_t numRigJoints = rig->getNumBones();

  //----------------
  NMP::Memory::Format internalBuffMemReqs;
  NMP::Memory::Format buffMemReqs;
  AttribDataTransformBuffer::getPosVelAngVelMemoryRequirements(numRigJoints, buffMemReqs, internalBuffMemReqs);
  AttribDataTransformBuffer* deadBlendTransformsRate = AttribDataTransformBuffer::createPosVelAngVel(
                              net->getPersistentMemoryAllocator(),
                              buffMemReqs,
                              internalBuffMemReqs,
                              numRigJoints);
  handle.set(deadBlendTransformsRate, buffMemReqs);
  transRateAddr.m_animSetIndex = activeAnimSetIndex;
  net->addAttribData(transRateAddr, handle, LIFESPAN_FOREVER);

  //----------------
  AttribDataTransformBuffer::getPosQuatMemoryRequirements(numRigJoints, buffMemReqs, internalBuffMemReqs);
  AttribDataTransformBuffer* deadBlendTransformsState = AttribDataTransformBuffer::createPosQuat(
                               net->getPersistentMemoryAllocator(),
                               buffMemReqs,
                               internalBuffMemReqs,
                               numRigJoints);
  handle.set(deadBlendTransformsState, buffMemReqs);
  transStateAddr.m_animSetIndex = activeAnimSetIndex;
  net->addAttribData(transStateAddr, handle, LIFESPAN_FOREVER);

  initialiseDeadBlendTransformsImpl(
    deadBlendTransformsState,
    deadBlendTransformsRate,
    useDeadReckoning,
    dt,
    numRigJoints,
    prevDataBuffer,
    prevPrevDataBuffer,
    deadBlendTransforms,
    transRates);

  if (!lastDeltaTrajAttr)
  {
    // Attempt to retrieve last delta trajectory
    if (parentSubNetworkID != INVALID_NODE_ID)
    {
      NodeBinEntry* deltaTrajEntry = net->getAttribDataNodeBinEntry(
                                       ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
                                       parentSubNetworkID,
                                       INVALID_NODE_ID,
                                       currFrameNo - 1);
      // deltaTrajEntry can be 0 if the parent state machine wasn't calculating trajectory output - in which
      // case we hope that it will continue to be ignored, and we assume it's 0
      lastDeltaTrajAttr = deltaTrajEntry ?
                          deltaTrajEntry->getAttribData<AttribDataTrajectoryDeltaTransform>()
                          : 0;
    }
  }

  if (lastDeltaTrajAttr)
  {
    if (useDeadReckoning && lastDeltaTrajAttr && dt > 0.0f)
    {
      transRates->getPosVelAngVelChannelAngVel(0)->setToZero();
      *transRates->getPosVelAngVelChannelPosVel(0) = lastDeltaTrajAttr->m_deltaPos / dt;
    }
    else
    {
      transRates->getPosVelAngVelChannelAngVel(0)->setToZero();
      transRates->getPosVelAngVelChannelPosVel(0)->setToZero();
    }

    if (!lastDeltaTrajAttr || !lastDeltaTrajAttr->m_filteredOut)
    {
      transRates->setChannelUsed(0);
    }
    else
    {
      transRates->setChannelUnused(0);
    }
  }
  else
  {
    transRates->getPosVelAngVelChannelAngVel(0)->setToZero();
    transRates->getPosVelAngVelChannelPosVel(0)->setToZero();
    transRates->setChannelUnused(0);
  }

  transRates->calculateFullFlag();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void initialiseDeadBlendTrajectoryAndTransforms(
  Network*                          net,
  NodeDef*                          node,
  bool                              useDeadReckoning,
  float                             dt,
  const AnimRigDef*                 rig,
  const NMP::DataBuffer*            prevDataBuffer,
  const NMP::DataBuffer*            prevPrevDataBuffer,
  const AttribDataTransformBuffer*  prevDeltaTrajBuffer,
  NMP::DataBuffer*&                 deadBlendTransforms,
  NMP::DataBuffer*&                 transRates,
  FrameCount                        currFrameNo,
  NodeID                            parentSubNetworkID)
{
  NMP_ASSERT(rig);
  NMP_ASSERT(prevDataBuffer);

  NodeID nodeID = node->getNodeID();
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(nodeID);

  // Create the dead blend transform buffers
  AttribAddress transStateAddr(ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE, nodeID, INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  AttribAddress transRateAddr(ATTRIB_SEMANTIC_TRANSFORM_RATES, nodeID, INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);

  // These must not already exist
  NMP_ASSERT(!net->getAttribDataNodeBinEntry(transStateAddr));
  NMP_ASSERT(!net->getAttribDataNodeBinEntry(transRateAddr));

  AttribDataHandle handle;
  const uint32_t numRigJoints = rig->getNumBones();

  //----------------
  NMP::Memory::Format internalBuffMemReqs;
  NMP::Memory::Format buffMemReqs;
  AttribDataTransformBuffer::getPosVelAngVelMemoryRequirements(numRigJoints, buffMemReqs, internalBuffMemReqs);
  AttribDataTransformBuffer* deadBlendTransformsRate = AttribDataTransformBuffer::createPosVelAngVel(
    net->getPersistentMemoryAllocator(),
    buffMemReqs,
    internalBuffMemReqs,
    numRigJoints);
  handle.set(deadBlendTransformsRate, buffMemReqs);
  transRateAddr.m_animSetIndex = activeAnimSetIndex;
  net->addAttribData(transRateAddr, handle, LIFESPAN_FOREVER);

  //----------------
  AttribDataTransformBuffer::getPosQuatMemoryRequirements(numRigJoints, buffMemReqs, internalBuffMemReqs);
  AttribDataTransformBuffer* deadBlendTransformsState = AttribDataTransformBuffer::createPosQuat(
    net->getPersistentMemoryAllocator(),
    buffMemReqs,
    internalBuffMemReqs,
    numRigJoints);
  handle.set(deadBlendTransformsState, buffMemReqs);
  transStateAddr.m_animSetIndex = activeAnimSetIndex;
  net->addAttribData(transStateAddr, handle, LIFESPAN_FOREVER);

  initialiseDeadBlendTransformsImpl(
    deadBlendTransformsState,
    deadBlendTransformsRate,
    useDeadReckoning,
    dt,
    numRigJoints,
    prevDataBuffer,
    prevPrevDataBuffer,
    deadBlendTransforms,
    transRates);

  NMP_ASSERT(prevDataBuffer);

  if (!prevDeltaTrajBuffer)
  {
    // Attempt to retrieve last delta trajectory
    if (parentSubNetworkID != INVALID_NODE_ID)
    {
      NodeBinEntry* deltaTrajEntry = net->getAttribDataNodeBinEntry(
        ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
        parentSubNetworkID,
        INVALID_NODE_ID,
        currFrameNo - 1);
      // deltaTrajEntry can be 0 if the parent state machine wasn't calculating trajectory output - in which
      // case we hope that it will continue to be ignored, and we assume it's 0
      prevDeltaTrajBuffer = deltaTrajEntry ?
        deltaTrajEntry->getAttribData<AttribDataTransformBuffer>()
        : 0;
    }
  }

  transRates->getPosVelAngVelChannelAngVel(0)->setToZero();

  if (prevDeltaTrajBuffer && useDeadReckoning && dt > 0.0f)
  {
    *transRates->getPosVelAngVelChannelPosVel(0) = *prevDeltaTrajBuffer->m_transformBuffer->getPosQuatChannelPos(0) / dt;
  }
  else
  {
    transRates->getPosVelAngVelChannelPosVel(0)->setToZero();
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
