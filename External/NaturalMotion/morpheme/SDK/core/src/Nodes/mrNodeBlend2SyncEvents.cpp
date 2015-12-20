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
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2SyncEventsFindGeneratingNodeForSemantic(
  NodeID             callingNodeID,
  bool               fromParent,   // Was this query from a parent or child node.
  AttribDataSemantic semantic,
  NodeDef*           nodeDef,
  Network*           network)
{
  NodeID result = INVALID_NODE_ID;

  switch (semantic)
  {
    case ATTRIB_SEMANTIC_TIME_POS:
    case ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS:
    case ATTRIB_SEMANTIC_SYNC_EVENT_TRACK:
    case ATTRIB_SEMANTIC_TRANSFORM_BUFFER:
    case ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER:
    case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM:
    case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER:
    case ATTRIB_SEMANTIC_FRACTION_POS:
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
Task* nodeBlend2SyncEventsQueueTimePos(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task;
  NodeID thisNodeID = nodeDef->getNodeID();
  NodeID activeParentNodeID = net->getActiveParentNodeID(thisNodeID);

  if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, nodeDef->getNodeID()) != INVALID_NODE_ID)
  {
    task = queue->createNewTaskOnQueue(
      CoreTaskIDs::MR_TASKID_BLEND2SYNCEVENTSUPDATETIMEVIASYNCEVENTPOS,
      thisNodeID,
      7,
      dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);

      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, activeParentNodeID, thisNodeID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 1, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 2, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, thisNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_LOOP, thisNodeID);
      net->TaskAddOptionalNetInputOutputParam(task, 5, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 6, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
    }
  }
  else
  {
    task = queue->createNewTaskOnQueue(
      CoreTaskIDs::MR_TASKID_BLEND2SYNCEVENTSUPDATETIMEVIATIMEPOS,
      thisNodeID,
      8,
      dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);

      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, thisNodeID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 1, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 2, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);      
      net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_LOOP, thisNodeID);
      net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, thisNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOptionalNetInputOutputParam(task, 5, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 6, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);

      // Find if someone (usually a transition) has stored a start playback pos adjustment on us or on a filter node parent.
      net->TaskAddOptionalNetInputParamRecurseToParent(task, 7, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, nodeDef->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, ANIMATION_SET_ANY);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2SyncEventsQueueSyncEventTrack(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_BLEND2SYNCEVENTTRACKS,
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
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(nodeDef->getNodeID());
    NMP_ASSERT(currFrameNo > 0);
    const NodeConnections* activeNodeConnections = net->getActiveNodesConnections(nodeDef->getNodeID());
    NMP_ASSERT(activeNodeConnections->m_numActiveChildNodes == 2);
    NodeID activeChildNodeID0 = activeNodeConnections->m_activeChildNodeIDs[0];
    NodeID activeChildNodeID1 = activeNodeConnections->m_activeChildNodeIDs[1];

    net->TaskAddOutputReplaceParam(task, 0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, INVALID_NODE_ID, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX, nodeDef->getNodeID());
    net->TaskAddNetInputParam(task, 4, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo);

    // Find if someone (usually a transition) has stored a start sync event adjustment on us or on a filter node parent.
    net->TaskAddOptionalNetInputParamRecurseToParent(task, 5, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, nodeDef->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, ANIMATION_SET_ANY);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2SyncEventsQueueBlend2DurationEventTrackSets(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);

  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_BLEND2DURATIONEVENTTRACKSETS,
    nodeDef->getNodeID(),
    7,
    dependentParameter);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(nodeDef->getNodeID());
    NMP_ASSERT(currFrameNo > 0);
    const NodeConnections* activeNodeConnections = net->getActiveNodesConnections(nodeDef->getNodeID());
    NMP_ASSERT(activeNodeConnections->m_numActiveChildNodes == 2);
    NodeID activeChildNodeID0 = activeNodeConnections->m_activeChildNodeIDs[0];
    NodeID activeChildNodeID1 = activeNodeConnections->m_activeChildNodeIDs[1];

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, INVALID_NODE_ID, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 4, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo);
    net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP, nodeDef->getNodeID());
    net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_LOOP, nodeDef->getNodeID());
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2SyncEventsQueueSampledEventsBuffers(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2SyncEventsQueueSampledEventsBuffersBase(
    nodeDef, queue, net, dependentParameter, 
    CoreTaskIDs::MR_TASKID_COMBINE2SAMPLEDEVENTSBUFFERS,
    CoreTaskIDs::MR_TASKID_COMBINE2SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2SyncEventsQueueAddSampledEventsBuffers(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  return nodeBlend2SyncEventsQueueSampledEventsBuffersBase(
    nodeDef, queue, net, dependentParameter, 
    CoreTaskIDs::MR_TASKID_ADD2SAMPLEDEVENTSBUFFERS,
    CoreTaskIDs::MR_TASKID_ADD2SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlend2SyncEventsQueueSampledEventsBuffersBase(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter,
  MR::TaskID      taskIDSampledEventBuffers,
  MR::TaskID      taskIDSampledEventBuffersDurationEventBuffers)
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);

  FrameCount currFrameNo = net->getCurrentFrameNo();
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(nodeDef->getNodeID());
  NMP_ASSERT(currFrameNo > 0);
  const BlendOptNodeConnections * activeNodeConnections = static_cast<const BlendOptNodeConnections *>( net->getActiveNodesConnections(nodeDef->getNodeID()) );

  Task* task( NULL );
  NodeID activeParentNodeID = net->getActiveParentNodeID(nodeDef->getNodeID());

  // This is a common construct.
  // We ask our parent if they know how to build a sync event position - that is do they know how to build a sync event track?
  // If they do not then we take the duration events and sample them into the sample events buffer.

  if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, nodeDef->getNodeID()) != INVALID_NODE_ID)
  {
    // The parent knows how to create a sync event track.

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
      NMP_ASSERT( activeNodeConnections->m_sampledEventsNumNodeIDs == 2 );
      NMP_ASSERT( activeNodeConnections->hasActiveChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[0] ) );
      NMP_ASSERT( activeNodeConnections->hasActiveChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[1] ) );

      // Just process the 2 source sampled events buffers.
      // Either combine (merge) or add dependent on the calling function.
      task = queue->createNewTaskOnQueue(
        taskIDSampledEventBuffers,
        nodeDef->getNodeID(),
        4,
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

        net->TaskAddNetInputParam(task, 3, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo);
      }
    }
  }
  else
  {
    // Here only one node is active when considering events. Sampled events can be optimised in this case.
    // We will sample the duration events into the one active node's sampled events buffer.
    if (activeNodeConnections->m_sampledEventsNumNodeIDs == 1)
    {
      NMP_ASSERT( activeNodeConnections->hasActiveChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[0] ));

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
    else
    {
      NMP_ASSERT( activeNodeConnections->m_sampledEventsNumNodeIDs == 2 );
      NMP_ASSERT( activeNodeConnections->hasActiveChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[0] ) );
      NMP_ASSERT( activeNodeConnections->hasActiveChildNodeID( activeNodeConnections->m_sampledEventsNodeIDs[1] ) );

      // Process the 2 source sampled events buffers and sample the duration events buffers if there are any.
      // Either combine (merge) or add dependent on the calling function.
      task = queue->createNewTaskOnQueue(
        taskIDSampledEventBuffersDurationEventBuffers,
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
  }

  return task;
}


//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2SyncEventsUpdateConnections(
  NodeDef* nodeDef,
  Network* net)
{
  return nodeBlend2UpdateConnectionsBase( nodeDef, net, true, false );
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlend2SyncEventsAdditiveUpdateConnections(
  NodeDef* nodeDef,
  Network* net)
{
  return nodeBlend2UpdateConnectionsBase( nodeDef, net, true, true );
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
