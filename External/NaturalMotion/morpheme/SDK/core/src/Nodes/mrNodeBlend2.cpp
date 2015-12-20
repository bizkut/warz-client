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
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2FindGeneratingNodeForSemantic(
  NodeID              callingNodeID,
  bool                fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            nodeDef,
  Network*            network)
{
  NodeID result = INVALID_NODE_ID;

  switch (semantic)
  {
  case ATTRIB_SEMANTIC_TIME_POS:
  case ATTRIB_SEMANTIC_TRANSFORM_BUFFER:
  case ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER:
    result = nodeDef->getNodeID();
    break;
  default:
  {
    // if the queuing function is either queuePassThroughChild0 or queuePassThroughChild1 then
    // the node was built with PassThroughMode kNodePassThroughSource0 or kNodePassThroughSource1.
    QueueAttrTaskFn queuingFn = nodeDef->getTaskQueueingFn(semantic);
    if (queuingFn == queuePassThroughChild0 || queuingFn == queuePassThroughChild1)
    {
      if (fromParent)
      {
        NodeID activeChildNodeID = network->getActiveChildNodeID(nodeDef->getNodeID(), nodeDef->getPassThroughChildIndex());
        result = network->nodeFindGeneratingNodeForSemantic(activeChildNodeID, semantic, fromParent, nodeDef->getNodeID());
      }
      else
      {
        NodeID childPassThroughNodeID = network->getActiveChildNodeID(nodeDef->getNodeID(), nodeDef->getPassThroughChildIndex());
        if (callingNodeID == childPassThroughNodeID)
        {
          NodeID activeParentNodeID = network->getActiveParentNodeID(nodeDef->getNodeID());
          result = network->nodeFindGeneratingNodeForSemantic(activeParentNodeID, semantic, fromParent, nodeDef->getNodeID());
        }
      }
    }
  }
  break;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBlendWeights* nodeBlend2CreateBlendWeights(
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
void nodeBlendPlaybackPosInit(
  NodeDef*           nodeDef,
  Network*           net)
{
  NodeConnections* nodeConnections = net->getActiveNodesConnections(nodeDef->getNodeID());
  NMP_ASSERT(nodeConnections);

  // if became active as state transition destination, handle pass-through of playback position event initialization
  if (nodeConnections->m_justBecameActive)
  {
    AttribDataPlaybackPosInit* parentPlaybackPosInitAttrib = (AttribDataPlaybackPosInit*) net->getAttribDataRecurseFilterNodes(
      ATTRIB_SEMANTIC_PLAYBACK_POS_INIT,
      nodeDef->getNodeID(),
      INVALID_NODE_ID,
      net->getCurrentFrameNo(),
      ANIMATION_SET_ANY,
      false);
    if (parentPlaybackPosInitAttrib)
    {
      AttribAddress address(ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, INVALID_NODE_ID, INVALID_NODE_ID);
      for (uint32_t i = 0; i < nodeConnections->m_numActiveChildNodes; ++i)
      {
        // Do not add the AttribData if it already exists in the network. This can happen when origin and destination share 
        // a pass down pin directly in a blend node via a 0 duration transit.
        address.m_owningNodeID = nodeConnections->m_activeChildNodeIDs[i];
        if (!net->getAttribDataNodeBinEntry(address))
        {
          AttribDataHandle handleChild = AttribDataPlaybackPosInit::create(
            net->getPersistentMemoryAllocator(),
            parentPlaybackPosInitAttrib->m_initWithDurationFraction,
            parentPlaybackPosInitAttrib->m_startValue,
            parentPlaybackPosInitAttrib->m_adjustStartEventIndex);
          net->addAttribData(address, handleChild, LIFESPAN_FOREVER);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlendQueueTask(
  NodeDef*            nodeDef,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  taskDataSemantic,
  AttribDataType      attribType,
  uint16_t            numChildNodes,
  const NodeID*       childNodeIDs)
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);
  NMP_ASSERT(numChildNodes > 0);

  Task* task = queue->createNewTaskOnQueue(
    taskID,
    nodeDef->getNodeID(),
    2 + numChildNodes,
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

    for (uint16_t i = 0; i < numChildNodes; ++i)
    {
      net->TaskAddParamAndDependency(task, i + 1, taskDataSemantic, attribType, childNodeIDs[i], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    }

    net->TaskAddNetInputParam(task, numChildNodes + 1, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo, ANIMATION_SET_ANY);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
float nodeBlend2CalculateBlendWeight(
  float                 interpolant,            // IN: Across any range.
  const float*          sourceWeights)          // IN: The array of source weights (can be increasing or decreasing)
{
  float blendWeight;

  // Scale factor for reflecting the weight sequence if required
  float delta = sourceWeights[1] - sourceWeights[0];
  float signum = NMP::floatSelect(delta, 1.0f, -1.0f);

  // Note we determine the two connected sources in an adjusted space that assumes the
  // weights are increasing in value. This is achieved by applying the scaling factor to
  // the weight value to perform the necessary reflection.
  float interpAdj = signum * interpolant;
  float weight0Adj = signum * sourceWeights[0];
  float weight1Adj = signum * sourceWeights[1];
  
  // Also note that the two connected sources bounding the interpolant and the corresponding
  // blend weight are dependent on the weight sequence and not the (reflected) space the
  // test is performed in.
  if (interpAdj <= weight0Adj)
  {
    // Less than the first weight
    blendWeight = 0.0f;
  }
  else if (interpAdj >= weight1Adj)
  {
    // Greater than the last weight
    blendWeight = 1.0f;
  }
  else
  {
    // Calculate weighting between 2 source inputs.
    float interpDivision = weight1Adj - weight0Adj;
    if (interpDivision < ERROR_LIMIT)
      blendWeight = 0.0f;
    else
      blendWeight = (interpAdj - weight0Adj) / interpDivision;
    NMP_ASSERT(blendWeight >= 0.0f && blendWeight <= 1.0f);
  }

  return blendWeight;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeBlend2BlendWeightCheck(
  bool alwaysBlendFlag,                         // IN: Flag to enable blend optimisation
  float blendWeightAlwaysBlend,                 // IN: The interpolation weight for non optimal blending
  const NodeID* childNodeIDsAlwaysBlend,        // IN: The Node IDs of the non optimal active connected children
  uint16_t& numWeights,                         // OUT: The number of optimised weights
  float* blendWeights,                          // OUT: The optimised blend weights
  uint16_t& numChildNodeIDs,                    // OUT: The number of optimised active connected children
  NodeID* childNodeIDs)                         // OUT: The optimised node IDs of the active connected children
{
  NMP_ASSERT(childNodeIDsAlwaysBlend);
  NMP_ASSERT(blendWeights);
  NMP_ASSERT(childNodeIDs);
  NMP_ASSERT(blendWeightAlwaysBlend >= 0.0f && blendWeightAlwaysBlend <= 1.0f);

  // Perform blend optimisation checking
  if (!alwaysBlendFlag)
  {
    if (blendWeightAlwaysBlend < ERROR_LIMIT)
    {
      numWeights = 0;
      numChildNodeIDs = 1;
      childNodeIDs[0] = childNodeIDsAlwaysBlend[0];
      return;
    }
    else if (blendWeightAlwaysBlend > 1.0f - ERROR_LIMIT)
    {
      numWeights = 0;
      numChildNodeIDs = 1;
      childNodeIDs[0] = childNodeIDsAlwaysBlend[1];
      return;
    }
  }

  // Always blend
  numWeights = 1;
  blendWeights[0] = blendWeightAlwaysBlend;
  numChildNodeIDs = 2;
  childNodeIDs[0] = childNodeIDsAlwaysBlend[0];
  childNodeIDs[1] = childNodeIDsAlwaysBlend[1];
}

//----------------------------------------------------------------------------------------------------------------------
void nodeBlend2AdditiveBlendWeightCheck(
  bool alwaysBlendFlag,                         // IN: Flag to enable blend optimisation
  float blendWeightAlwaysBlend,                 // IN: The interpolation weight for non optimal blending
  const NodeID* childNodeIDsAlwaysBlend,        // IN: The Node IDs of the non optimal active connected children
  uint16_t& numWeights,                         // OUT: The number of optimised weights
  float* blendWeights,                          // OUT: The optimised blend weights
  uint16_t& numChildNodeIDs,                    // OUT: The number of optimised active connected children
  NodeID* childNodeIDs)                         // OUT: The optimised node IDs of the active connected children
{
  NMP_ASSERT(childNodeIDsAlwaysBlend);
  NMP_ASSERT(blendWeights);
  NMP_ASSERT(childNodeIDs);
  NMP_ASSERT(blendWeightAlwaysBlend >= 0.0f && blendWeightAlwaysBlend <= 1.0f);

  // Perform blend optimisation checking
  if (!alwaysBlendFlag)
  {
    if (blendWeightAlwaysBlend < ERROR_LIMIT)
    {
      numWeights = 0;
      numChildNodeIDs = 1;
      childNodeIDs[0] = childNodeIDsAlwaysBlend[0];
      return;
    }
  }

  // Always blend
  numWeights = 1;
  blendWeights[0] = blendWeightAlwaysBlend;
  numChildNodeIDs = 2;
  childNodeIDs[0] = childNodeIDsAlwaysBlend[0];
  childNodeIDs[1] = childNodeIDsAlwaysBlend[1];
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTask(
  NodeDef*            nodeDef,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  taskDataSemantic,
  AttribDataType      attribType,
  NodeID              activeChildNodeID0,
  NodeID              activeChildNodeID1)
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);
  NMP_ASSERT(activeChildNodeID0 != activeChildNodeID1);

  Task* task = queue->createNewTaskOnQueue(
    taskID,
    nodeDef->getNodeID(),
    4,
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
    net->TaskAddParamAndDependency(task, 1, taskDataSemantic, attribType, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, taskDataSemantic, attribType, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 3, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo, ANIMATION_SET_ANY);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE
Task* nodeBlend2QueueTransformsTask(
  NodeDef*            nodeDef,
  TaskQueue*          queue,
  Network*            net,
  TaskParameter*      dependentParameter,
  TaskID              taskID,
  AttribDataSemantic  taskDataSemantic,
  AttribDataType      attribType)
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);

  // Get the blend node optimised connections.
  const BlendOptNodeConnections* activeNodeConnections = (const BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  NMP_ASSERT(
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 1 ||
    activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 2);

  Task* task;
  if (activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 2)
  {
    task = nodeBlend2QueueTask(
      nodeDef, queue, net, dependentParameter,
      taskID,
      taskDataSemantic,
      attribType,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs[0],
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs[1]);
  }
  else
  {
    task = queuePassThroughOnChildNodeID(
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs[0],
      nodeDef,
      queue,
      net,
      dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Time and events
Task* nodeBlend2QueueSampledEventsBuffers(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2QueueSampledEventsBuffersBase(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_COMBINE2SAMPLEDEVENTSBUFFERS );
}

//----------------------------------------------------------------------------------------------------------------------
// Time and events
Task* nodeBlend2QueueAddSampledEventsBuffers(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2QueueSampledEventsBuffersBase(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_ADD2SAMPLEDEVENTSBUFFERS );
}

//----------------------------------------------------------------------------------------------------------------------
// Time and events
Task* nodeBlend2QueueSampledEventsBuffersBase(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter,
  MR::TaskID      taskID )
{
  // Get the blend node optimised connections.
  const BlendOptNodeConnections* activeNodeConnections = static_cast<const BlendOptNodeConnections*>( net->getActiveNodesConnections(nodeDef->getNodeID()) );
  NMP_ASSERT( activeNodeConnections->m_sampledEventsNumNodeIDs == 1 ||
              activeNodeConnections->m_sampledEventsNumNodeIDs == 2 );

  Task* task( NULL );

  // Here only one node is active when considering events. Sampled events can be optimised in this case.
  if (activeNodeConnections->m_sampledEventsNumNodeIDs == 1)
  {
    NMP_ASSERT( activeNodeConnections->hasActiveChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[0] ) );

    task = queuePassThroughOnChildNodeID(
      activeNodeConnections->m_sampledEventsNodeIDs[0],
      nodeDef,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    NMP_ASSERT( activeNodeConnections->hasActiveChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[0] ) );
    NMP_ASSERT( activeNodeConnections->hasActiveChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[1] ) );

    task = nodeBlend2QueueTask(
      nodeDef, queue, net, dependentParameter,
      taskID,
      ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER,
      ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER,
      activeNodeConnections->m_sampledEventsNodeIDs[0],
      activeNodeConnections->m_sampledEventsNodeIDs[1]);
  }

  return task;    
}

//----------------------------------------------------------------------------------------------------------------------
// Transforms
Task* nodeBlend2QueueBlend2TransformBuffsAddAttAddPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSADDATTADDPOS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueBlend2TransformBuffsAddAttInterpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSADDATTINTERPPOS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueBlend2TransformBuffsInterpAttAddPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSINTERPATTADDPOS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueBlend2TransformBuffsInterpAttInterpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRANSFORMBUFFSINTERPATTINTERPPOS,
    ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
// Trajectory
Task* nodeBlend2QueueTrajectoryDeltaTransformAddAttAddPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTADDPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
    ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTrajectoryDeltaTransformAddAttInterpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTINTERPPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
    ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTrajectoryDeltaTransformAddAttSlerpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSADDATTSLERPPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
    ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTrajectoryDeltaTransformInterpAttAddPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTADDPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
    ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTrajectoryDeltaTransformInterpAttInterpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTINTERPPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
    ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTrajectoryDeltaTransformInterpAttSlerpPos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYDELTATRANSFORMSINTERPATTSLERPPOS,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM,
    ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
}

//----------------------------------------------------------------------------------------------------------------------
// Transforms and Trajectory
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj(
  NodeDef* nodeDef,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTADDPOSSLERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj(
  NodeDef* nodeDef,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTINTERPPOSSLERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj(
  NodeDef* nodeDef,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTADDPOSSLERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj(
  NodeDef* nodeDef,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTINTERPPOSSLERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosInterpTraj(
  NodeDef* nodeDef,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTADDPOSINTERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosInterpTraj(
  NodeDef* nodeDef,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSADDATTINTERPPOSINTERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosInterpTraj(
  NodeDef* nodeDef,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTADDPOSINTERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj(
  NodeDef* nodeDef,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  return nodeBlend2QueueTransformsTask(
    nodeDef, queue, net, dependentParameter,
    CoreTaskIDs::MR_TASKID_BLEND2TRAJECTORYANDTRANSFORMSINTERPATTINTERPPOSINTERPTRAJ,
    ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ATTRIB_TYPE_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2UpdateConnections(
  NodeDef*           nodeDef,
  Network*           net)
{
  return nodeBlend2UpdateConnectionsBase( nodeDef, net, false, false );
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2AdditiveUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net)
{
  return nodeBlend2UpdateConnectionsBase( nodeDef, net, false, true );
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2UpdateConnectionsBase(
  NodeDef*  nodeDef,
  Network*  net,
  bool      isSyncEvents,
  bool      isAdditive)
{
  //------------------------
  // Calculate the blend weights
  AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());

  AttribDataBlendWeights*     attribBlendWeights    = nodeBlend2CreateBlendWeights(nodeDef, net);
  BlendOptNodeConnections*    activeNodeConnections = (BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  const AttribDataFloatArray* nodeChildWeights      = nodeDef->getAttribData<AttribDataFloatArray>(ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS);
  AttribDataFloat*            inputCPFloat0         = net->updateInputCPConnection<AttribDataFloat>(nodeDef->getInputCPConnection(0), animSet);
  float                       blendWeight           = nodeBlend2CalculateBlendWeight(inputCPFloat0->m_value, nodeChildWeights->m_values);
  
  // Get the events blend weight
  float blendWeightEvents = blendWeight;
  const CPConnection* connection = nodeDef->getInputCPConnection(1);
  if (connection->m_sourceNodeID != INVALID_NODE_ID)
  {
    NMP_ASSERT( nodeDef->getNumInputCPConnections() == 2 );
    AttribDataFloat* inputCPFloat1 = net->updateInputCPConnection<AttribDataFloat>(nodeDef->getInputCPConnection(1), animSet);
    blendWeightEvents = nodeBlend2CalculateBlendWeight(inputCPFloat1->m_value, nodeChildWeights->m_values);
  }

  //------------------------
  // Sets members of attribBlendWeights and activeNodeConnections. Indicating the number of and which nodes are active for 
  // the calculation of trajectory and transform and events. These are used for the optimisation of trajectory/transforms and 
  // events where only one child is effectively active.
  nodeBlend2UpdateConnectionsSetBlendWeightsCheckForOptimisation( attribBlendWeights,
                                                                  activeNodeConnections,
                                                                  net,
                                                                  nodeDef,
                                                                  blendWeight,
                                                                  blendWeightEvents,
                                                                  isAdditive );

  //------------------------
  // If we are not syncing events then
  // initialise the playback pos
  if( isSyncEvents == false )
  {
    nodeBlendPlaybackPosInit(nodeDef, net);
  }

  //------------------------
  // Recurse to children (remember we always want to blend sync event tracks)
  NMP_ASSERT(activeNodeConnections->m_activeChildNodeIDs[0] == nodeDef->getChildNodeID(0));
  NMP_ASSERT(activeNodeConnections->m_activeChildNodeIDs[1] == nodeDef->getChildNodeID(1));
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[0], animSet);
  net->updateNodeInstanceConnections(activeNodeConnections->m_activeChildNodeIDs[1], animSet);

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
// This will delete a node that is being optimised for both sampled events and trajectory and transforms
// If deleted the NodeID of the deleted will be returned, else INVALID_NODE_ID is returned.
NodeID nodeBlend2UpdateConnectionsSetBlendWeightsCheckForOptimisation(
  AttribDataBlendWeights*   attribBlendWeights,
  BlendOptNodeConnections*  activeNodeConnections,
  Network*                  net,
  const NodeDef*            nodeDef,
  float                     blendWeight,
  float                     blendWeightEvents,
  bool                      isAdditive)
{
  // store the node ids of the children to be blended.
  NMP_ASSERT(activeNodeConnections == net->getActiveNodesConnections(nodeDef->getNodeID()));
  NMP_ASSERT(net->getNumActiveChildren(nodeDef->getNodeID()) == 2);
  NMP_ASSERT(nodeDef->hasChildNodeID( net->getActiveChildNodeID(nodeDef->getNodeID(), 0)));
  NMP_ASSERT(nodeDef->hasChildNodeID( net->getActiveChildNodeID(nodeDef->getNodeID(), 1)));

  // Check blend weights.
  const AttribDataBlendFlags* blendFlags = nodeDef->getAttribData<AttribDataBlendFlags>(ATTRIB_SEMANTIC_BLEND_FLAGS);
  if( isAdditive )
  {
    // Optimise transforms
    nodeBlend2AdditiveBlendWeightCheck(
      blendFlags->m_alwaysBlendTrajectoryAndTransforms,
      blendWeight,
      activeNodeConnections->m_activeChildNodeIDs,
      attribBlendWeights->m_trajectoryAndTransformsNumWeights,
      attribBlendWeights->m_trajectoryAndTransformsWeights,
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs);

    // Optimise sampled events
    nodeBlend2AdditiveBlendWeightCheck(
      blendFlags->m_alwaysCombineSampledEvents,
      blendWeightEvents,
      activeNodeConnections->m_activeChildNodeIDs,
      attribBlendWeights->m_sampledEventsNumWeights,
      attribBlendWeights->m_sampledEventsWeights,
      activeNodeConnections->m_sampledEventsNumNodeIDs,
      activeNodeConnections->m_sampledEventsNodeIDs);
  }
  else
  {
    // Optimise transforms
    nodeBlend2BlendWeightCheck(
      blendFlags->m_alwaysBlendTrajectoryAndTransforms,
      blendWeight,
      activeNodeConnections->m_activeChildNodeIDs,
      attribBlendWeights->m_trajectoryAndTransformsNumWeights,
      attribBlendWeights->m_trajectoryAndTransformsWeights,
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs,
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs);

    // Optimise sampled events
    nodeBlend2BlendWeightCheck(
      blendFlags->m_alwaysCombineSampledEvents,
      blendWeightEvents,
      activeNodeConnections->m_activeChildNodeIDs,
      attribBlendWeights->m_sampledEventsNumWeights,
      attribBlendWeights->m_sampledEventsWeights,
      activeNodeConnections->m_sampledEventsNumNodeIDs,
      activeNodeConnections->m_sampledEventsNodeIDs);
  }

  //------------------------
  // Sync event tracks are always blended
  attribBlendWeights->setEventsWeights(1, &blendWeightEvents); 

  //------------------------
  // If a node is being optimised for both transforms and sampled events we will delete it.
  // This is because when a sync event track is requested the node is re-created requesting the correct time
  // if we did not delete it no time would be requested so the time would not update hence the two sources for the
  // blend node would get out of sync.
  NodeID nodeIDToDel = INVALID_NODE_ID;
  if( activeNodeConnections->m_sampledEventsNumNodeIDs == 1 &&
      activeNodeConnections->m_trajectoryAndTransformsNumNodeIDs == 1 && 
      activeNodeConnections->m_trajectoryAndTransformsNodeIDs[0] == activeNodeConnections->m_sampledEventsNodeIDs[0] )
  {
    // we want to the node we are not going to process for trajectory/transforms or sampled events
    nodeIDToDel = activeNodeConnections->m_activeChildNodeIDs[0];
    if( nodeIDToDel == activeNodeConnections->m_trajectoryAndTransformsNodeIDs[0] )
    {
      // That node id was the one that was to be processed. Choose the other one.
      nodeIDToDel = activeNodeConnections->m_activeChildNodeIDs[1];
    }
    net->deleteNodeInstance(nodeIDToDel);
  }

  return nodeIDToDel;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
