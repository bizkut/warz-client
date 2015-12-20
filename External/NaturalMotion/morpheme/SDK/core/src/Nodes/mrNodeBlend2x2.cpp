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
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeBlend2x2.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
AttribDataBlendWeights* nodeBlend2x2CreateBlendWeights(
  NodeDef*  nodeDef,
  Network*  net)
{
  FrameCount currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);

  //------------------------
  // If not already existing, create an attribute on the Network to store the desired blend weights.  
  NodeBinEntry* networkEntry = net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  AttribDataBlendWeights* attribBlendWeights;
  if (!networkEntry)
  {
    AttribDataHandle handle = AttribDataBlendWeights::create(net->getPersistentMemoryAllocator(), 2);
    attribBlendWeights = (AttribDataBlendWeights*) handle.m_attribData;
    AttribAddress attribAddess(ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo);
    net->addAttribData(attribAddess, handle, LIFESPAN_FOREVER);
  }
  else
  {
    networkEntry->m_address.m_validFrame = currFrameNo;
    attribBlendWeights = networkEntry->getAttribData<AttribDataBlendWeights>();
  }
  NMP_ASSERT(attribBlendWeights);

  return attribBlendWeights;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeBlend2x2BlendWeightsCheck(
  bool          alwaysBlendFlag,          ///< IN: Flag to enable blend optimisation
  const float*  blendWeightsAlwaysBlend,  ///< IN: The interpolation weight for non optimal blending
  const NodeID* childNodeIDsAlwaysBlend,  ///< IN: The Node IDs of the non optimal active connected children
  uint16_t&     numBlendWeights,          ///< OUT: The number of optimised weights
  float*        blendWeights,             ///< OUT: The optimised blend weights
  uint16_t&     numChildNodeIDs,          ///< OUT: The number of optimised active connected children
  NodeID*       childNodeIDs,             ///< OUT: The optimised node IDs of the active connected children
  uint16_t&     numInvalidChildNodeIDs,   ///< OUT: The number of children node ids that are optimised away
  NodeID*       invalidChildNodeIDs)      ///< OUT: The optimised away node IDs
{
  NMP_ASSERT(blendWeightsAlwaysBlend);
  NMP_ASSERT(childNodeIDsAlwaysBlend);
  NMP_ASSERT(blendWeights);
  NMP_ASSERT(childNodeIDs);
  NMP_ASSERT(invalidChildNodeIDs);
  NMP_ASSERT(blendWeightsAlwaysBlend[0] >= 0.0f && blendWeightsAlwaysBlend[0] <= 1.0f);
  NMP_ASSERT(blendWeightsAlwaysBlend[1] >= 0.0f && blendWeightsAlwaysBlend[1] <= 1.0f);

  // Perform blend optimisation checking
  if (alwaysBlendFlag)
  {
    numBlendWeights = 2;
    for (uint16_t i = 0; i < 2; ++i)
      blendWeights[i] = blendWeightsAlwaysBlend[i];
    numChildNodeIDs = 4;
    for (uint16_t i = 0; i < 4; ++i)
      childNodeIDs[i] = childNodeIDsAlwaysBlend[i];
    numInvalidChildNodeIDs = 0;
    return;
  }

  // Initialise all child nodes as valid
  bool validWeights[2] = {true, true};
  bool validChildNodes[4] = {true, true, true, true};

  //------------------------
  // X
  if (blendWeightsAlwaysBlend[0] < ERROR_LIMIT)
  {
    // X1Y0 and X1Y1 invalid
    validChildNodes[1] = false;
    validChildNodes[3] = false;
    // X weight invalid
    validWeights[0] = false;
  }
  else if (blendWeightsAlwaysBlend[0] > 1.0f - ERROR_LIMIT)
  {
    // X0Y0 and X0Y1 invalid
    validChildNodes[0] = false;
    validChildNodes[2] = false;
    // X weight invalid
    validWeights[0] = false;
  }

  //------------------------
  // Y
  if (blendWeightsAlwaysBlend[1] < ERROR_LIMIT)
  {
    // X0Y1 and X1Y1 invalid
    validChildNodes[2] = false;
    validChildNodes[3] = false;
    // Y weight invalid
    validWeights[1] = false;
  }
  else if (blendWeightsAlwaysBlend[1] > 1.0f - ERROR_LIMIT)
  {
    // X0Y0 and X1Y0 invalid
    validChildNodes[0] = false;
    validChildNodes[1] = false;
    // Y weight invalid
    validWeights[1] = false;
  }

  //------------------------
  // Compute the active blend weights
  numBlendWeights = 0;
  for (uint16_t i = 0; i < 2; ++i)
  {
    if (validWeights[i])
    {
      blendWeights[numBlendWeights] = blendWeightsAlwaysBlend[i];
      ++numBlendWeights;
    }
  }

  // Compute the active child nodes
  numInvalidChildNodeIDs = 0;
  numChildNodeIDs = 0;
  for (uint16_t i = 0; i < 4; ++i)
  {
    if (validChildNodes[i])
    {
      childNodeIDs[numChildNodeIDs] = childNodeIDsAlwaysBlend[i];
      ++numChildNodeIDs;
    }
    else
    {
      invalidChildNodeIDs[numInvalidChildNodeIDs] = childNodeIDsAlwaysBlend[i];
      ++numInvalidChildNodeIDs;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2x2QueueTask(
  NodeDef*            nodeDef,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  taskDataSemantic,
  AttribDataType      attribType,
  NodeID              activeChildNode0ID,
  NodeID              activeChildNode1ID,
  NodeID              activeChildNode2ID,
  NodeID              activeChildNode3ID)
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);

  Task* task = queue->createNewTaskOnQueue(
    taskID,
    nodeDef->getNodeID(),
    6,
    dependentParameter);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(nodeDef->getNodeID());

    net->TaskAddOutputParamZeroLifespan(task, 0, taskDataSemantic, attribType, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 1, taskDataSemantic, attribType, activeChildNode0ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, taskDataSemantic, attribType, activeChildNode1ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 3, taskDataSemantic, attribType, activeChildNode2ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 4, taskDataSemantic, attribType, activeChildNode3ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 5, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo, ANIMATION_SET_ANY);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE
Task* nodeBlend2x2QueueOptimisedTask(
  NodeDef*            nodeDef,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID2x2,
  TaskID              taskID2,
  AttribDataSemantic  taskDataSemantic,
  AttribDataType      attribType,
  uint16_t            numNodeIDs,
  const NodeID*       nodeIDs )
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);
  NMP_ASSERT(numNodeIDs == 1 || numNodeIDs == 2 || numNodeIDs == 4);

  Task* task;
  if (numNodeIDs == 4)
  {
    NMP_ASSERT(net->nodeIsActive( nodeIDs[0] ) );
    NMP_ASSERT(net->nodeIsActive( nodeIDs[1] ) );
    NMP_ASSERT(net->nodeIsActive( nodeIDs[2] ) );
    NMP_ASSERT(net->nodeIsActive( nodeIDs[3] ) );
    NMP_ASSERT(nodeDef->hasChildNodeID( nodeIDs[0] ) );
    NMP_ASSERT(nodeDef->hasChildNodeID( nodeIDs[1] ) );
    NMP_ASSERT(nodeDef->hasChildNodeID( nodeIDs[2] ) );
    NMP_ASSERT(nodeDef->hasChildNodeID( nodeIDs[3] ) );
    
    task = nodeBlend2x2QueueTask(
      nodeDef, queue, net, dependentParameter,
      taskID2x2,
      taskDataSemantic,
      attribType,
      nodeIDs[0],
      nodeIDs[1],
      nodeIDs[2],
      nodeIDs[3]);
  }
  else if (numNodeIDs == 2)
  {
    NMP_ASSERT(net->nodeIsActive( nodeIDs[0] ) );
    NMP_ASSERT(net->nodeIsActive( nodeIDs[1] ) );
    NMP_ASSERT(nodeDef->hasChildNodeID( nodeIDs[0] ) );
    NMP_ASSERT(nodeDef->hasChildNodeID( nodeIDs[1] ) );

    task = nodeBlend2QueueTask(
      nodeDef, queue, net, dependentParameter,
      taskID2,
      taskDataSemantic,
      attribType,
      nodeIDs[0],
      nodeIDs[1]);
  }
  else
  {
    NMP_ASSERT(net->nodeIsActive( nodeIDs[0] ) );
    NMP_ASSERT(nodeDef->hasChildNodeID( nodeIDs[0] ) );

    task = queuePassThroughOnChildNodeID(
      nodeIDs[0],
      nodeDef,
      queue,
      net,
      dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Time and events
Task* nodeBlend2x2QueueSampledEventsBuffers(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  const BlendOptNodeConnections* activeNodeConnections = static_cast<const BlendOptNodeConnections*>( net->getActiveNodesConnections(nodeDef->getNodeID()) );

  return nodeBlend2x2QueueOptimisedTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_COMBINE2X2SAMPLEDEVENTSBUFFERS,
    CoreTaskIDs::MR_TASKID_COMBINE2SAMPLEDEVENTSBUFFERS,
    ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, 
    ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER,
    activeNodeConnections->m_sampledEventsNumNodeIDs,
    activeNodeConnections->m_sampledEventsNodeIDs );
}

//----------------------------------------------------------------------------------------------------------------------
// Transforms
Task* nodeBlend2x2QueueBlendTransformBuffsInterpAttInterpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  const BlendOptNodeConnections* activeNodeConnections = static_cast<const BlendOptNodeConnections*>( net->getActiveNodesConnections(nodeDef->getNodeID()) );

  return nodeBlend2x2QueueOptimisedTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2X2TRANSFORMBUFFSINTERPATTINTERPPOS,
    CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSINTERPATTINTERPPOS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER,
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
    activeNodeConnections->m_trajectoryAndTransformsNodeIDs );
}

//----------------------------------------------------------------------------------------------------------------------
// Trajectory delta
Task* nodeBlend2x2QueueTrajectoryDeltaTransformInterpAttInterpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  const BlendOptNodeConnections* activeNodeConnections = static_cast<const BlendOptNodeConnections*>( net->getActiveNodesConnections(nodeDef->getNodeID()) );

  return nodeBlend2x2QueueOptimisedTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2X2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
    activeNodeConnections->m_trajectoryAndTransformsNodeIDs );
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2x2QueueTrajectoryDeltaTransformInterpAttSlerpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  const BlendOptNodeConnections* activeNodeConnections = static_cast<const BlendOptNodeConnections*>( net->getActiveNodesConnections(nodeDef->getNodeID()) );

  return nodeBlend2x2QueueOptimisedTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2X2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM,
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
    activeNodeConnections->m_trajectoryAndTransformsNodeIDs );
}

//----------------------------------------------------------------------------------------------------------------------
// Trajectory delta and transforms
Task* nodeBlend2x2QueueTrajectoryDeltaAndTransformsInterpPosInterpAttInterpTraj(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter)
{
  const BlendOptNodeConnections* activeNodeConnections = static_cast<const BlendOptNodeConnections*>( net->getActiveNodesConnections(nodeDef->getNodeID()) );

  return nodeBlend2x2QueueOptimisedTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2X2TRAJECTORYANDTRANSFORMSINTERPTRAJ,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTINTERPPOSINTERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER,
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
    activeNodeConnections->m_trajectoryAndTransformsNodeIDs );
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2x2QueueTrajectoryDeltaAndTransformsInterpPosInterpAttSlerpTraj(NodeDef* nodeDef, TaskQueue* queue, Network* net, TaskParameter* dependentParameter)
{
  const BlendOptNodeConnections* activeNodeConnections = static_cast<const BlendOptNodeConnections*>( net->getActiveNodesConnections(nodeDef->getNodeID()) );

  return nodeBlend2x2QueueOptimisedTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2X2TRAJECTORYANDTRANSFORMSSLERPTRAJ,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTINTERPPOSSLERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER,
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
    activeNodeConnections->m_trajectoryAndTransformsNodeIDs );
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2x2UpdateConnections(
  NodeDef*           nodeDef,
  Network*           net)
{
  //------------------------
  // Calculate the blend weights 
  AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());

  AttribDataBlendWeights* attribBlendWeights = nodeBlend2x2CreateBlendWeights(nodeDef, net);
  BlendOptNodeConnections* activeNodeConnections = (BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());

  const AttribDataBlendNxMDef* nodeChildWeights = nodeDef->getAttribData<AttribDataBlendNxMDef>(ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS);
  NMP_ASSERT(nodeChildWeights->m_numWeightsX == 2);
  NMP_ASSERT(nodeChildWeights->m_numWeightsY == 2);
  AttribDataFloat* inputCPFloatX = net->updateInputCPConnection<AttribDataFloat>(nodeDef->getInputCPConnection(0), animSet);
  AttribDataFloat* inputCPFloatY = net->updateInputCPConnection<AttribDataFloat>(nodeDef->getInputCPConnection(1), animSet);

  float blendWeightsAlwaysBlend[2];
  blendWeightsAlwaysBlend[0] = nodeBlend2CalculateBlendWeight(
    inputCPFloatX->m_value,
    nodeChildWeights->m_weightsX);
  blendWeightsAlwaysBlend[1] = nodeBlend2CalculateBlendWeight(
    inputCPFloatY->m_value,
    nodeChildWeights->m_weightsY);

  //------------------------
  // Sets members of attribBlendWeights and activeNodeConnections. Indicating the number of and which nodes are active for 
  // the calculation of trajectory and transform and events. These are used for the optimisation of trajectory/transforms and 
  // events where not all children are effectively active.
  nodeBlend2x2UpdateConnectionsSetBlendWeightsCheckForOptimisation( attribBlendWeights,
                                                                    activeNodeConnections,
                                                                    net,
                                                                    nodeDef,
                                                                    blendWeightsAlwaysBlend );

  //------------------------
  // Initialise the playback pos
  nodeBlendPlaybackPosInit(nodeDef, net);

  //------------------------
  // Recurse to children
  NMP_ASSERT(nodeDef->hasChildNodeID(activeNodeConnections->m_activeChildNodeIDs[0]));
  NMP_ASSERT(nodeDef->hasChildNodeID(activeNodeConnections->m_activeChildNodeIDs[1]));
  NMP_ASSERT(nodeDef->hasChildNodeID(activeNodeConnections->m_activeChildNodeIDs[2]));
  NMP_ASSERT(nodeDef->hasChildNodeID(activeNodeConnections->m_activeChildNodeIDs[3]));
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[0], animSet);
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[1], animSet);
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[2], animSet);
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[3], animSet);

  return nodeDef->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
// Checks the blend weights caching information about active nodes to that may be used to later optimise.
// The only things that will be modified by this function:-
//
// attribBlendWeights->m_trajectoryAndTransformsNumWeights,
// attribBlendWeights->m_trajectoryAndTransformsWeights,
// attribBlendWeights->m_sampledEventsNumWeights,
// attribBlendWeights->m_sampledEventsWeights,
//
// activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
// activeNodeConnections->m_trajectoryAndTransformsNodeIDs);
// activeNodeConnections->m_sampledEventsNumNodeIDs,
// activeNodeConnections->m_sampledEventsNodeIDs);
//
void nodeBlend2x2UpdateConnectionsSetBlendWeightsCheckForOptimisation(
  AttribDataBlendWeights*   attribBlendWeights,
  BlendOptNodeConnections*  activeNodeConnections,
  Network*                  net,
  const NodeDef*            nodeDef,
  const float*              blendWeightsAlwaysBlend )
{
  NMP_ASSERT(net->getNumActiveChildren(nodeDef->getNodeID()) == 4);
  NMP_ASSERT(nodeDef->getMaxNumActiveChildNodes() == 4);

  NMP_ASSERT(nodeDef->hasChildNodeID(net->getActiveChildNodeID(nodeDef->getNodeID(),0) ) );
  NMP_ASSERT(nodeDef->hasChildNodeID(net->getActiveChildNodeID(nodeDef->getNodeID(),1) ) );
  NMP_ASSERT(nodeDef->hasChildNodeID(net->getActiveChildNodeID(nodeDef->getNodeID(),2) ) );
  NMP_ASSERT(nodeDef->hasChildNodeID(net->getActiveChildNodeID(nodeDef->getNodeID(),3) ) );

  NMP_ASSERT(net->getActiveChildNodeID(nodeDef->getNodeID(),0) == activeNodeConnections->m_activeChildNodeIDs[0] );
  NMP_ASSERT(net->getActiveChildNodeID(nodeDef->getNodeID(),1) == activeNodeConnections->m_activeChildNodeIDs[1] );
  NMP_ASSERT(net->getActiveChildNodeID(nodeDef->getNodeID(),2) == activeNodeConnections->m_activeChildNodeIDs[2] );
  NMP_ASSERT(net->getActiveChildNodeID(nodeDef->getNodeID(),3) == activeNodeConnections->m_activeChildNodeIDs[3] );
  
  // will populate this array with node ids that have been optimised for both
  // trajectory/transforms and sampled events.
  uint16_t numInvalidChildNodeIDs;
  NodeID invalidChildNodeIDs[ 3 ];

  //------------------------
  // Optimise transforms
  const AttribDataBlendFlags* blendFlags = nodeDef->getAttribData<AttribDataBlendFlags>(ATTRIB_SEMANTIC_BLEND_FLAGS);
  nodeBlend2x2BlendWeightsCheck(  blendFlags->m_alwaysBlendTrajectoryAndTransforms,
                                  blendWeightsAlwaysBlend,
                                  activeNodeConnections->m_activeChildNodeIDs,
                                  attribBlendWeights->m_trajectoryAndTransformsNumWeights,
                                  attribBlendWeights->m_trajectoryAndTransformsWeights,
                                  activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
                                  activeNodeConnections->m_trajectoryAndTransformsNodeIDs,
                                  numInvalidChildNodeIDs,
                                  invalidChildNodeIDs );

  NMP_ASSERT( numInvalidChildNodeIDs <= 3 );
  NMP_ASSERT( (numInvalidChildNodeIDs + activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs) == 4 );

  //------------------------
  // Optimise sampled events.
  // as we do not have a separate event weight if the blend flags are 
  // the same we can re-use the trajectory and transforms result.
  if( blendFlags->m_alwaysBlendTrajectoryAndTransforms == blendFlags->m_alwaysCombineSampledEvents )
  {
    attribBlendWeights->setSampledEventsWeights( attribBlendWeights->m_trajectoryAndTransformsNumWeights,
                                                 attribBlendWeights->m_trajectoryAndTransformsWeights );
    activeNodeConnections->setSampledEventsNodeIDs( activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
                                                    activeNodeConnections->m_trajectoryAndTransformsNodeIDs );

    //------------------------
    // If we are optimising both trajectory/transforms and events then we need to delete nodes that 
    // have both; otherwise we do not create a dependency on time and the time is not updated. 
    // The will lead to this transform source being out of sync with others connected.
    // By deleting; when the sync track is requested the node is recreated and the temporal data is
    // set correctly.
    if( blendFlags->m_alwaysBlendTrajectoryAndTransforms == false )
    {
      for( uint32_t i=0; i<numInvalidChildNodeIDs; ++i )
      {
        net->deleteNodeInstance(invalidChildNodeIDs[i]);
      }
    }
  }
  else
  {
    nodeBlend2x2BlendWeightsCheck(
      blendFlags->m_alwaysCombineSampledEvents,
      blendWeightsAlwaysBlend,
      activeNodeConnections->m_activeChildNodeIDs,
      attribBlendWeights->m_sampledEventsNumWeights,
      attribBlendWeights->m_sampledEventsWeights,
      activeNodeConnections->m_sampledEventsNumNodeIDs,
      activeNodeConnections->m_sampledEventsNodeIDs,
      numInvalidChildNodeIDs,
      invalidChildNodeIDs );

      NMP_ASSERT( numInvalidChildNodeIDs <= 3 );
      NMP_ASSERT( (numInvalidChildNodeIDs + activeNodeConnections->m_sampledEventsNumNodeIDs) == 4 );
  }

  //------------------------
  // Sync event track and events are always blended
  attribBlendWeights->setEventsWeights(2, blendWeightsAlwaysBlend);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
