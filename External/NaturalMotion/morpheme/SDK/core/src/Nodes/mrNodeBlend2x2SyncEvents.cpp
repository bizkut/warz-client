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
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeBlend2x2.h"
#include "morpheme/Nodes/mrNodeBlend2x2SyncEvents.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2x2SyncEventsQueueSyncEventTrack(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);

  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_BLEND2X2SYNCEVENTTRACKS,
    nodeDef->getNodeID(),
    8,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(nodeDef->getNodeID());
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);
    const NodeConnections* activeNodeConnections = net->getActiveNodesConnections(nodeDef->getNodeID());
    NMP_ASSERT(activeNodeConnections->m_numActiveChildNodes == 4);
    NodeID activeChildNode0ID = activeNodeConnections->m_activeChildNodeIDs[0];
    NodeID activeChildNode1ID = activeNodeConnections->m_activeChildNodeIDs[1];
    NodeID activeChildNode2ID = activeNodeConnections->m_activeChildNodeIDs[2];
    NodeID activeChildNode3ID = activeNodeConnections->m_activeChildNodeIDs[3];

    net->TaskAddOutputReplaceParam(task, 0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, INVALID_NODE_ID, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNode0ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNode1ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNode2ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNode3ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX, nodeDef->getNodeID());
    net->TaskAddNetInputParam(task, 6, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo, ANIMATION_SET_ANY);

    // Find if someone (usually a transition) has stored a start sync event adjustment on us or on a filter node parent.
    net->TaskAddOptionalNetInputParamRecurseToParent(task, 7, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, nodeDef->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, ANIMATION_SET_ANY);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2x2SyncEventsQueueSampledEventsBuffers(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);

  Task* task;
  NodeID activeParentNodeID = net->getActiveParentNodeID(nodeDef->getNodeID());
  if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, nodeDef->getNodeID()) != INVALID_NODE_ID)
  {
    task = nodeBlend2x2QueueSampledEventsBuffers( nodeDef, queue, net, dependentParameter );
  }
  else
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(nodeDef->getNodeID());
    const BlendOptNodeConnections * activeNodeConnections = static_cast<const BlendOptNodeConnections*>( net->getActiveNodesConnections(nodeDef->getNodeID()) );

    NMP_ASSERT(currFrameNo > 0);
    NMP_ASSERT(activeNodeConnections->m_numActiveChildNodes == 4);
    NMP_ASSERT( activeNodeConnections->m_sampledEventsNumNodeIDs == 4 ||
                activeNodeConnections->m_sampledEventsNumNodeIDs == 2 ||
                activeNodeConnections->m_sampledEventsNumNodeIDs == 1 );

    if( activeNodeConnections->m_sampledEventsNumNodeIDs == 4 )
    {
      // Combine the source sampled events buffers and sample the duration events buffers if there are any.
      NMP_ASSERT( nodeDef->hasChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[0] ));
      NMP_ASSERT( nodeDef->hasChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[1] ));
      NMP_ASSERT( nodeDef->hasChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[2] ));
      NMP_ASSERT( nodeDef->hasChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[3] ));

      task = queue->createNewTaskOnQueue(
        CoreTaskIDs::MR_TASKID_COMBINE2X2SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS,
        nodeDef->getNodeID(),
        10,
        dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
      NMP_ASSERT(task)
#else
      if (task)
#endif
      {
        net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);

        net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, 
          activeNodeConnections->m_sampledEventsNodeIDs[0], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, 
          activeNodeConnections->m_sampledEventsNodeIDs[1], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, 
          activeNodeConnections->m_sampledEventsNodeIDs[2], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, 
          activeNodeConnections->m_sampledEventsNodeIDs[3], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);

        net->TaskAddParamAndDependency(task, 5, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 6, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 7, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddNetInputParam(task, 8, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo, ANIMATION_SET_ANY);
        net->TaskAddDefInputParam(task, 9, ATTRIB_SEMANTIC_LOOP, nodeDef->getNodeID());
      }
    }
    else if( activeNodeConnections->m_sampledEventsNumNodeIDs == 2 )
    {
      // Combine the 2 source sampled events buffers and sample the duration events buffers if there are any.

      NMP_ASSERT( nodeDef->hasChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[0] ));
      NMP_ASSERT( nodeDef->hasChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[1] ));

      task = queue->createNewTaskOnQueue(
        CoreTaskIDs::MR_TASKID_COMBINE2SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS,
        nodeDef->getNodeID(),
        8,
        dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
      NMP_ASSERT(task)
#else
      if (task)
#endif
      {
        net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);

        net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, 
          activeNodeConnections->m_sampledEventsNodeIDs[0], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, 
          activeNodeConnections->m_sampledEventsNodeIDs[1], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        
        net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 5, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddNetInputParam(task, 6, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo);
        net->TaskAddDefInputParam(task, 7, ATTRIB_SEMANTIC_LOOP, nodeDef->getNodeID());
      }
    }
    else
    {
      // Here only one node is active when considering events. Sampled events can be optimised in this case.
      // We will sample the duration events into the one active node's sampled events buffer.
      
      NMP_ASSERT( nodeDef->hasChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[0] ));

      task = queue->createNewTaskOnQueue(
        CoreTaskIDs::MR_TASKID_ADDSAMPLEDDURATIONEVENTSTOSAMPLEDEVENTBUFFER,
        nodeDef->getNodeID(),
        6,
        dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
      NMP_ASSERT(task)
#else
      if (task)
#endif
      {
        net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);
        
        net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, 
          activeNodeConnections->m_sampledEventsNodeIDs[0], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);

        net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_LOOP, nodeDef->getNodeID());
      }
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2x2SyncEventsQueueBlend2x2DurationEventTrackSets(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_BLEND2X2DURATIONEVENTTRACKSETS,
    nodeDef->getNodeID(),
    9,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(nodeDef->getNodeID());
    FrameCount currFrameNo = net->getCurrentFrameNo();
    NMP_ASSERT(currFrameNo > 0);
    const NodeConnections* activeNodeConnections = net->getActiveNodesConnections(nodeDef->getNodeID());
    NMP_ASSERT(activeNodeConnections->m_numActiveChildNodes == 4);
    NodeID activeChildNode0ID = activeNodeConnections->m_activeChildNodeIDs[0];
    NodeID activeChildNode1ID = activeNodeConnections->m_activeChildNodeIDs[1];
    NodeID activeChildNode2ID = activeNodeConnections->m_activeChildNodeIDs[2];
    NodeID activeChildNode3ID = activeNodeConnections->m_activeChildNodeIDs[3];

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, INVALID_NODE_ID, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, activeChildNode0ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, activeChildNode1ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, activeChildNode2ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, activeChildNode3ID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 5, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 6, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo, ANIMATION_SET_ANY);
    net->TaskAddDefInputParam(task, 7, ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP, nodeDef->getNodeID());
    net->TaskAddDefInputParam(task, 8, ATTRIB_SEMANTIC_LOOP, nodeDef->getNodeID());
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2x2SyncEventsUpdateConnections(
  NodeDef*           nodeDef,
  Network*           net)
{
  //------------------------
  // Calculate the blend weights  
  AttribDataBlendWeights* attribBlendWeights = nodeBlend2x2CreateBlendWeights(nodeDef, net);
  BlendOptNodeConnections* activeNodeConnections = (BlendOptNodeConnections*)net->getActiveNodesConnections(nodeDef->getNodeID());
  AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());

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

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
