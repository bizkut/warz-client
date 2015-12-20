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
#include "morpheme/mrTransitDeadBlend.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

// non-public template definitions
#include "mrTransitDeadBlend.inl"

// Disable warnings constant conditionals
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4127)
#endif

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
// AttribDataDeadBlendDefDefault functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataDeadBlendDef::locate(AttribData* target)
{
  AttribData::locate(target);

  AttribDataDeadBlendDef* result = (AttribDataDeadBlendDef*)target;
  NMP::endianSwap(result->m_blendToDestinationPhysicsBones);
  NMP::endianSwap(result->m_deadReckoningAngularVelocityDamping);
  NMP::endianSwap(result->m_useDeadReckoningWhenDeadBlending);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataDeadBlendDef::dislocate(AttribData* target)
{
  AttribDataDeadBlendDef* result = (AttribDataDeadBlendDef*)target;
  NMP::endianSwap(result->m_blendToDestinationPhysicsBones);
  NMP::endianSwap(result->m_deadReckoningAngularVelocityDamping);
  NMP::endianSwap(result->m_useDeadReckoningWhenDeadBlending);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
AttribDataDeadBlendDef* AttribDataDeadBlendDef::create(
  NMP::MemoryAllocator* allocator,
  uint16_t              refCount)
{
  AttribDataDeadBlendDef* result;

  NMP::Memory::Format memReqs = AttribDataDeadBlendDef::getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = AttribDataDeadBlendDef::init(resource, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataDeadBlendDef::getMemoryRequirements()
{
  return NMP::Memory::Format(
           NMP::Memory::align(sizeof(AttribDataDeadBlendDef), MR_ATTRIB_DATA_ALIGNMENT),
           MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataDeadBlendDef* AttribDataDeadBlendDef::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  resource.align(format);

  AttribDataDeadBlendDef* result = (AttribDataDeadBlendDef*)resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_DEAD_BLEND_DEF);
  result->setRefCount(refCount);

  result->m_deadReckoningAngularVelocityDamping = 0.0f;
  result->m_useDeadReckoningWhenDeadBlending = true;
  result->m_blendToDestinationPhysicsBones = false;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataDeadBlendStateDefault functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataDeadBlendState::locate(AttribData* target)
{
  AttribDataDeadBlendState* result = (AttribDataDeadBlendState*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_blendMode);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataDeadBlendState::dislocate(AttribData* target)
{
  AttribDataDeadBlendState* result = (AttribDataDeadBlendState*)target;

  AttribData::dislocate(target);

  NMP::endianSwap(result->m_blendMode);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
AttribDataDeadBlendState* AttribDataDeadBlendState::create(
  NMP::MemoryAllocator* allocator,
  uint16_t              refCount)
{
  AttribDataDeadBlendState* result;

  NMP::Memory::Format memReqs = AttribDataDeadBlendState::getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = AttribDataDeadBlendState::init(resource, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataDeadBlendState::getMemoryRequirements()
{
  return NMP::Memory::Format(
           NMP::Memory::align(sizeof(AttribDataDeadBlendState), MR_ATTRIB_DATA_ALIGNMENT),
           MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataDeadBlendState* AttribDataDeadBlendState::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  resource.align(format);

  AttribDataDeadBlendState* result = (AttribDataDeadBlendState*)resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_DEAD_BLEND_STATE);
  result->setRefCount(refCount);

  result->m_performDeadBlend = false;
  result->m_isBlendingToPhysics = false;

  result->m_blendMode = 0; // 0 means: BLEND_MODE_INTERP_ATT_INTERP_POS;
  result->m_slerpTrajectoryPosition = false;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool isDeadBlending(NodeDef* node, Network* net)
{
  bool result = false;
  AttribDataDeadBlendState* deadBlendState = getDeadBlendState(node, net);
  if (deadBlendState)
    result = deadBlendState->m_performDeadBlend;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool isBlendingToPhysics(NodeDef* node, Network* net)
{
  bool result = false;
  AttribDataDeadBlendState* deadBlendState = getDeadBlendState(node, net);
  if (deadBlendState)
    result = deadBlendState->m_isBlendingToPhysics;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool isDeadBlendingOrBlendingToPhysics(NodeDef* node, Network* net)
{
  return (isDeadBlending(node, net) || isBlendingToPhysics(node, net));
}

//----------------------------------------------------------------------------------------------------------------------
NodeID getPhysicalNodeID(NodeDef* node, Network* net)
{
  if (net->isNodePhysical(node))
  {
    return node->getNodeID();
  }

  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  for (uint32_t iChild = 0; iChild < connections->m_numActiveChildNodes; ++iChild)
  {
    NodeDef* child = net->getNetworkDef()->getNodeDef(connections->m_activeChildNodeIDs[iChild]);
    NodeID physNodeID = getPhysicalNodeID(child, net);
    if (physNodeID != INVALID_NODE_ID)
      return physNodeID;
  }

  return INVALID_NODE_ID;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t getNodeRootControlMethod(NodeDef* node, Network* net)
{
  NodeID nodeID = getPhysicalNodeID(node, net);
  if (nodeID != INVALID_NODE_ID)
  {
    AttribDataHandle* nodeSetupAttr = net->getNetworkDef()->getAttribDataHandle(
                                        ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,
                                        nodeID);
    NMP_ASSERT(nodeSetupAttr && nodeSetupAttr->m_attribData);
    if (nodeSetupAttr->m_attribData->getType() == AttribDataPhysicsSetup::getDefaultType())
    {
      AttribDataPhysicsSetup* physicsSetup = (AttribDataPhysicsSetup*)nodeSetupAttr->m_attribData;
      return physicsSetup->m_rootControlMethod;
    }
    else
    {
      return Network::ROOT_CONTROL_PHYSICS;
    }
  }

  return Network::ROOT_CONTROL_PHYSICAL_ANIMATION;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataDeadBlendState* getDeadBlendState(NodeDef* node, Network* net)
{
  AttribDataDeadBlendState* deadBlendAttr = net->getOptionalAttribData<AttribDataDeadBlendState>(
        ATTRIB_SEMANTIC_DEAD_BLEND_STATE,
        node->getNodeID(),
        INVALID_NODE_ID);
  return deadBlendAttr;
}

//----------------------------------------------------------------------------------------------------------------------
bool initialiseDeadBlend(NodeDef* node, Network* net)
{
  // Initialize the dead blend means to find the proper Dead Blend Setup
  // as an attribute of the node, already created in the network.
  AttribDataDeadBlendState* deadBlendState = getDeadBlendState(node, net);
  if (!deadBlendState)
    return false;

  // Initialize dead blend when needed.
  if (deadBlendState->m_performDeadBlend)
  {
    // The following code creates:
    // - dead blend transform rates - use a transform buffer attribute data, a transform semantic and a "0" target node to store the rates
    // - dead blend transform state - transform buffer attribute, node specific state
    // - dead blend trajectory rate - velocity attribute, semantic velocity
    // - dead blend trajectory state - node specific state 2

    const AnimRigDef* rig = net->getActiveRig();

    const FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(net->getCurrentFrameNo() > 0);

    // Get the previous subnetwork states.
    NMP::DataBuffer* prevDataBuffer = 0;
    NMP::DataBuffer* prevPrevDataBuffer = 0;
    AttribDataTrajectoryDeltaTransform* prevDeltaTrajAttr = 0;
    AttribDataUpdatePlaybackPos* prevUpdatePlaybackPos = 0;
    NodeID parentSubNetworkID = INVALID_NODE_ID;

    FrameCount prevFrame = net->getCurrentFrameNo();
    if (prevFrame > 1)
    {
      --prevFrame;
      uint32_t prevPrevFrame = prevFrame;
      if (prevPrevFrame > 1)
        --prevPrevFrame;

      // Find a parent state machine or the network that can provide us with two transform buffers with which to
      // initialise physics.
      parentSubNetworkID = node->getNodeID();
      while (true)
      {
        parentSubNetworkID = getParentStateMachineOrNetworkRootNodeID(net->getNetworkDef()->getNodeDef(parentSubNetworkID), net);
        prevDataBuffer = getNodeTransformsBuffer(parentSubNetworkID, net, prevFrame, net->getOutputAnimSetIndex(parentSubNetworkID));
        prevPrevDataBuffer = getNodeTransformsBuffer(parentSubNetworkID, net, prevPrevFrame, net->getOutputAnimSetIndex(parentSubNetworkID));
        if (prevDataBuffer && prevPrevDataBuffer)
        {
          break;
        }
        else if (parentSubNetworkID == net->getNetworkDef()->getRootNodeID())
        {
          // If we have one frame output then use it - otherwise bind pose
          if (!prevDataBuffer)
            prevDataBuffer = rig->getBindPose()->m_transformBuffer;
          // either way have no history - so initialise with two identical poses.
          prevPrevDataBuffer = prevDataBuffer;
          break;
        }
      }
    }
    else
    {
      prevDataBuffer = prevPrevDataBuffer = rig->getBindPose()->m_transformBuffer;
    }

    // Get the last frames delta time update
    // note that dt may be 0
    float dt = net->getLastUpdateTimeStep();

    // If node was explicitly caching state for dead blends, use that instead
    if (node->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION_CAN_DEAD_BLEND)
    {
      NodeID nodeID = node->getNodeID();

      NodeBinEntry* attrUpdatePlaybackPosTMinusOne = net->getAttribDataNodeBinEntry(
            MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS_T_MINUS_ONE,  nodeID, nodeID, currFrameNo);

      if (attrUpdatePlaybackPosTMinusOne)
      {
        NodeBinEntry* attrDeltaTrajTMinusOne = net->getAttribDataNodeBinEntry(
            MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_T_MINUS_ONE, nodeID, nodeID, currFrameNo);

        NodeBinEntry* attrTransformsTMinusOne = net->getAttribDataNodeBinEntry(
            MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_ONE, nodeID, nodeID, currFrameNo);

        NodeBinEntry* attrTransformsTMinusTwo = net->getAttribDataNodeBinEntry(
            MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_TWO, nodeID, nodeID, currFrameNo);

        NMP_ASSERT(attrUpdatePlaybackPosTMinusOne);
        NMP_ASSERT(attrDeltaTrajTMinusOne);
        NMP_ASSERT(attrTransformsTMinusOne);
        NMP_ASSERT(attrTransformsTMinusTwo);

        prevUpdatePlaybackPos = attrUpdatePlaybackPosTMinusOne->getAttribData<AttribDataUpdatePlaybackPos>();
        prevDeltaTrajAttr     = attrDeltaTrajTMinusOne->getAttribData<AttribDataTrajectoryDeltaTransform>();
        prevDataBuffer        = attrTransformsTMinusOne->getAttribData<AttribDataTransformBuffer>()->m_transformBuffer;
        prevPrevDataBuffer    = attrTransformsTMinusTwo->getAttribData<AttribDataTransformBuffer>()->m_transformBuffer;

        NMP_ASSERT(prevUpdatePlaybackPos);

        // Get the last frames delta time update
        if (prevUpdatePlaybackPos->m_isAbs == false)
        {
          dt = prevUpdatePlaybackPos->m_value;
        }
      }
    }

    AttribDataDeadBlendDef* deadBlendDef = node->getAttribData<AttribDataDeadBlendDef>(ATTRIB_SEMANTIC_DEAD_BLEND_DEF);
    bool useDeadReckoning = deadBlendDef->m_useDeadReckoningWhenDeadBlending;

    // Initialise dead blend transforms and trajectory using state from previous frames
    NMP::DataBuffer* deadBlendTransforms;
    NMP::DataBuffer* transRates;
    initialiseDeadBlendSeparateTransformsAndTrajectory(net, node, useDeadReckoning, dt, rig, prevDataBuffer, prevPrevDataBuffer, deadBlendTransforms, transRates, prevDeltaTrajAttr, parentSubNetworkID, currFrameNo);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool initialiseTrajectoryDeltaAndTransformsDeadBlend(NodeDef* node, Network* net)
{
  // Initialize the dead blend means to find the proper Dead Blend Setup
  // as an attribute of the node, already created in the network.
  AttribDataDeadBlendState* deadBlendState = getDeadBlendState(node, net);
  if (!deadBlendState)
    return false;

  // Initialize dead blend when needed.
  if (deadBlendState->m_performDeadBlend)
  {
    // The following code creates:
    // - dead blend transform rates - use a transform buffer attribute data, a transform semantic and a "0" target node to store the rates
    // - dead blend transform state - transform buffer attribute, node specific state
    // - dead blend trajectory rate - velocity attribute, semantic velocity
    // - dead blend trajectory state - node specific state 2

    const AnimRigDef* rig = net->getActiveRig();

    const FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(net->getCurrentFrameNo() > 0);

    // Get the previous subnetwork states.
    NMP::DataBuffer* prevDataBuffer = 0;
    NMP::DataBuffer* prevPrevDataBuffer = 0;
    AttribDataTransformBuffer* prevDeltaTrajAttr = 0;
    AttribDataUpdatePlaybackPos* prevUpdatePlaybackPos = 0;
    NodeID parentSubNetworkID = INVALID_NODE_ID;

    FrameCount prevFrame = net->getCurrentFrameNo();
    if (prevFrame > 1)
    {
      --prevFrame;
      uint32_t prevPrevFrame = prevFrame;
      if (prevPrevFrame > 1)
        --prevPrevFrame;

      // Find a parent state machine or the network that can provide us with two transform buffers with which to
      // initialise physics.
      parentSubNetworkID = node->getNodeID();
      while (true)
      {
        parentSubNetworkID = getParentStateMachineOrNetworkRootNodeID(net->getNetworkDef()->getNodeDef(parentSubNetworkID), net);
        prevDataBuffer = getNodeTransformsBuffer(parentSubNetworkID, net, prevFrame, net->getOutputAnimSetIndex(parentSubNetworkID));
        prevPrevDataBuffer = getNodeTransformsBuffer(parentSubNetworkID, net, prevPrevFrame, net->getOutputAnimSetIndex(parentSubNetworkID));
        if (prevDataBuffer && prevPrevDataBuffer)
        {
          break;
        }
        else if (parentSubNetworkID == net->getNetworkDef()->getRootNodeID())
        {
          // If we have one frame output then use it - otherwise bind pose
          if (!prevDataBuffer)
            prevDataBuffer = rig->getBindPose()->m_transformBuffer;
          // either way have no history - so initialise with two identical poses.
          prevPrevDataBuffer = prevDataBuffer;
          break;
        }
      }
    }
    else
    {
      prevDataBuffer = prevPrevDataBuffer = rig->getBindPose()->m_transformBuffer;
    }

    // Get the last frames delta time update
    // note that dt may be 0
    float dt = net->getLastUpdateTimeStep();

    // If node was explicitly caching state for dead blends, use that instead
    if (node->getNodeFlags() & NodeDef::NODE_FLAG_IS_TRANSITION_CAN_DEAD_BLEND)
    {
      NodeID nodeID = node->getNodeID();

      NodeBinEntry* attrUpdatePlaybackPosTMinusOne = net->getAttribDataNodeBinEntry(
        MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS_T_MINUS_ONE,  nodeID, nodeID, currFrameNo);

      if (attrUpdatePlaybackPosTMinusOne)
      {
        NodeBinEntry* attrDeltaTrajTMinusOne = net->getAttribDataNodeBinEntry(
          MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_T_MINUS_ONE, nodeID, nodeID, currFrameNo);

        NodeBinEntry* attrTransformsTMinusOne = net->getAttribDataNodeBinEntry(
          MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_ONE, nodeID, nodeID, currFrameNo);

        NodeBinEntry* attrTransformsTMinusTwo = net->getAttribDataNodeBinEntry(
          MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_TWO, nodeID, nodeID, currFrameNo);

        NMP_ASSERT(attrUpdatePlaybackPosTMinusOne);
        NMP_ASSERT(attrDeltaTrajTMinusOne);
        NMP_ASSERT(attrTransformsTMinusOne);
        NMP_ASSERT(attrTransformsTMinusTwo);

        prevUpdatePlaybackPos = attrUpdatePlaybackPosTMinusOne->getAttribData<AttribDataUpdatePlaybackPos>();
        prevDeltaTrajAttr     = attrDeltaTrajTMinusOne->getAttribData<AttribDataTransformBuffer>();
        prevDataBuffer        = attrTransformsTMinusOne->getAttribData<AttribDataTransformBuffer>()->m_transformBuffer;
        prevPrevDataBuffer    = attrTransformsTMinusTwo->getAttribData<AttribDataTransformBuffer>()->m_transformBuffer;

        NMP_ASSERT(prevUpdatePlaybackPos);

        // Get the last frames delta time update
        if (prevUpdatePlaybackPos->m_isAbs == false)
        {
          dt = prevUpdatePlaybackPos->m_value;
        }
      }
    }

    AttribDataDeadBlendDef* deadBlendDef = node->getAttribData<AttribDataDeadBlendDef>(ATTRIB_SEMANTIC_DEAD_BLEND_DEF);
    bool useDeadReckoning = deadBlendDef->m_useDeadReckoningWhenDeadBlending;

    NMP::DataBuffer* deadBlendTransforms;
    NMP::DataBuffer* transRates;
    // Initialise dead blend transforms and trajectory using state from previous frames
    initialiseDeadBlendTrajectoryAndTransforms(
      net,
      node,
      useDeadReckoning,
      dt,
      rig,
      prevDataBuffer,
      prevPrevDataBuffer,
      prevDeltaTrajAttr,
      deadBlendTransforms,
      transRates,
      currFrameNo,
      parentSubNetworkID);
  }

  return true;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------

