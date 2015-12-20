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
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeSubtractiveBlend.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribDataBlendWeights* nodeSubtractiveBlendCreateBlendWeights(
  NodeDef* nodeDef,
  Network* net)
{
  FrameCount currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);

  //------------------------
  // If not already existing, create an attribute on the Network to store the desired blend weights.  
  NodeBinEntry* networkEntry = net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  AttribDataBlendWeights* attribBlendWeights;
  if (!networkEntry)
  {
    AttribDataHandle handle = AttribDataBlendWeights::create(net->getPersistentMemoryAllocator(), 1);
    attribBlendWeights = (AttribDataBlendWeights*) handle.m_attribData;
    AttribAddress attribAddess(ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo);
    net->addAttribData(attribAddess, handle, LIFESPAN_FOREVER);

    // Initialise the fixed weights
    attribBlendWeights->setAllWeights(1.0f);
  }
  else
  {
    networkEntry->m_address.m_validFrame = currFrameNo;
    attribBlendWeights = networkEntry->getAttribData<AttribDataBlendWeights>();

    // Validate the fixed weights
    NMP_ASSERT(attribBlendWeights->m_eventsNumWeights == 1);
    NMP_ASSERT(attribBlendWeights->m_eventsWeights[0] == 1.0f);
    NMP_ASSERT(attribBlendWeights->m_sampledEventsNumWeights == 1);
    NMP_ASSERT(attribBlendWeights->m_sampledEventsWeights[0] == 1.0f);
    NMP_ASSERT(attribBlendWeights->m_trajectoryAndTransformsNumWeights == 1);
    NMP_ASSERT(attribBlendWeights->m_trajectoryAndTransformsWeights[0] == 1.0f);
  }
  NMP_ASSERT(attribBlendWeights);

  return attribBlendWeights;
}

//----------------------------------------------------------------------------------------------------------------------
// Transforms
Task* nodeSubtractiveBlendQueueTransformBuffsSubtractAttSubtractPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NodeID activeChildNodeID0 = net->getActiveChildNodeID(nodeDef->getNodeID(), 0);
  NodeID activeChildNodeID1 = net->getActiveChildNodeID(nodeDef->getNodeID(), 1);
  return nodeBlend2QueueTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSSUBTRACTATTSUBTRACTPOS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER,
    activeChildNodeID0,
    activeChildNodeID1);
}

//----------------------------------------------------------------------------------------------------------------------
// Trajectory delta
Task* nodeSubtractiveBlendQueueTrajectoryDeltaTransformSubtractAttSubtractPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NodeID activeChildNodeID0 = net->getActiveChildNodeID(nodeDef->getNodeID(), 0);
  NodeID activeChildNodeID1 = net->getActiveChildNodeID(nodeDef->getNodeID(), 1);
  return nodeBlend2QueueTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSSUBTRACTATTSUBTRACTPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
    ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,
    activeChildNodeID0,
    activeChildNodeID1);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeSubtractiveBlendQueueTrajectoryDeltaTransformSubtractAttSlerpPos( 
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NodeID activeChildNodeID0 = net->getActiveChildNodeID(nodeDef->getNodeID(), 0);
  NodeID activeChildNodeID1 = net->getActiveChildNodeID(nodeDef->getNodeID(), 1);
  return nodeBlend2QueueTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSSUBTRACTATTSLERPPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
    ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,
    activeChildNodeID0,
    activeChildNodeID1);
}

//----------------------------------------------------------------------------------------------------------------------
// Transforms and Trajectory delta
Task* nodeSubtractiveBlendQueueTrajectoryDeltaAndTransformsSubtractAttSubtractPosSlerpTraj(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NodeID activeChildNodeID0 = net->getActiveChildNodeID(nodeDef->getNodeID(), 0);
  NodeID activeChildNodeID1 = net->getActiveChildNodeID(nodeDef->getNodeID(), 1);
  return nodeBlend2QueueTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSSUBTRACTATTSUBTRACTPOSSLERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER,
    activeChildNodeID0,
    activeChildNodeID1);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeSubtractiveBlendQueueTrajectoryDeltaAndTransformsSubtractAttSubtractPosInterpTraj(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NodeID activeChildNodeID0 = net->getActiveChildNodeID(nodeDef->getNodeID(), 0);
  NodeID activeChildNodeID1 = net->getActiveChildNodeID(nodeDef->getNodeID(), 1);
  return nodeBlend2QueueTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSSUBTRACTATTSUBTRACTPOSINTERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER,
    activeChildNodeID0,
    activeChildNodeID1);
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeSubtractiveBlendUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net)
{
  //------------------------
  // Calculate the fixed blend weights
  nodeSubtractiveBlendCreateBlendWeights(nodeDef, net);

  NMP_ASSERT(net->getNumActiveChildren(nodeDef->getNodeID()) == 2);
  NMP_ASSERT(net->getActiveChildNodeID(nodeDef->getNodeID(), 0) == nodeDef->getChildNodeID(0));
  NMP_ASSERT(net->getActiveChildNodeID(nodeDef->getNodeID(), 1) == nodeDef->getChildNodeID(1));
  NodeID childNodeIDs[2];
  childNodeIDs[0] = nodeDef->getChildNodeID(0);
  childNodeIDs[1] = nodeDef->getChildNodeID(1);

  //------------------------
  // This node re-uses the Blend2 Sampled Events Buffers tasks.
  // So we need to set the sampled events blend optimisation info.
  BlendOptNodeConnections* activeNodeConnections = static_cast<BlendOptNodeConnections*>( net->getActiveNodesConnections(nodeDef->getNodeID()) );
  activeNodeConnections->setSampledEventsNodeIDs(2,childNodeIDs);

  //------------------------
  // Initialise the playback pos
  nodeBlendPlaybackPosInit(nodeDef, net);

  //------------------------
  // Recurse to children
  AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());
  net->updateNodeInstanceConnections(childNodeIDs[0], animSet);
  net->updateNodeInstanceConnections(childNodeIDs[1], animSet);

  return nodeDef->getNodeID();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
