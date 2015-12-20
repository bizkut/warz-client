// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#include "morpheme/Nodes/mrNodeScatterBlend2D.h"
#include "morpheme/Nodes/mrNodeScatterBlend2DSyncEvents.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// SyncEventTrack
//----------------------------------------------------------------------------------------------------------------------
Task* nodeScatterBlend2DSyncEventsQueueSyncEventTrack(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);

  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_SYNCEVENTTRACKS,
    nodeDef->getNodeID(),
    7,
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
    NMP_ASSERT(activeNodeConnections->m_numActiveChildNodes == 3);

    net->TaskAddOutputReplaceParam(task, 0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, INVALID_NODE_ID, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeNodeConnections->m_activeChildNodeIDs[0], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeNodeConnections->m_activeChildNodeIDs[1], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeNodeConnections->m_activeChildNodeIDs[2], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX, nodeDef->getNodeID());
    net->TaskAddNetInputParam(task, 5, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo, ANIMATION_SET_ANY);

    // Find if someone (usually a transition) has stored a start sync event adjustment on us or on a filter node parent.
    net->TaskAddOptionalNetInputParamRecurseToParent(task, 6, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, nodeDef->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, ANIMATION_SET_ANY);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Duration event track sets.
//----------------------------------------------------------------------------------------------------------------------
Task* nodeScatterBlend2DSyncEventsQueueBlend3DurationEventTrackSets(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
    CoreTaskIDs::MR_TASKID_TRIANGLE_BLEND_DURATIONEVENTTRACKSETS,
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
    NMP_ASSERT(activeNodeConnections->m_numActiveChildNodes == 3);

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, INVALID_NODE_ID, currFrameNo);
    net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, activeNodeConnections->m_activeChildNodeIDs[0], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, activeNodeConnections->m_activeChildNodeIDs[1], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, activeNodeConnections->m_activeChildNodeIDs[2], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
    net->TaskAddNetInputParam(task, 5, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo, ANIMATION_SET_ANY);
    net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP, nodeDef->getNodeID());
    net->TaskAddDefInputParam(task, 7, ATTRIB_SEMANTIC_LOOP, nodeDef->getNodeID());
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Sampled Event Buffer
//----------------------------------------------------------------------------------------------------------------------
Task* nodeScatterBlend2DSyncEventsQueueSampledEventsBuffers(
  NodeDef*        nodeDef,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  NMP_ASSERT(nodeDef);
  NMP_ASSERT(queue);
  NMP_ASSERT(net);

  FrameCount currFrameNo = net->getCurrentFrameNo();
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(nodeDef->getNodeID());
  NMP_ASSERT(currFrameNo > 0);
  const BlendOptNodeConnections* activeNodeConnections = static_cast<const BlendOptNodeConnections*>( net->getActiveNodesConnections(nodeDef->getNodeID()) );
  NMP_ASSERT(activeNodeConnections->m_numActiveChildNodes == 3);

  Task* task;
  if( activeNodeConnections->m_sampledEventsNumNodeIDs == 3 )
  {
    NodeID activeParentNodeID = activeNodeConnections->m_activeParentNodeID;
    if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, nodeDef->getNodeID()) != INVALID_NODE_ID)
    {
      task = nodeBlendQueueTask( nodeDef,
                                 queue,
                                 net,
                                 dependentParameter,
                                 CoreTaskIDs::MR_TASKID_TRIANGLE_COMBINE_SAMPLEDEVENTSBUFFERS,
                                 ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER,
                                 ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER,
                                 activeNodeConnections->m_numActiveChildNodes,
                                 activeNodeConnections->m_activeChildNodeIDs );
    }
    else
    {
      // Combine the source sampled events buffers and sample the duration events buffers if there are any.
      task = queue->createNewTaskOnQueue( CoreTaskIDs::MR_TASKID_TRIANGLE_COMBINE_SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS,
                                          nodeDef->getNodeID(),
                                          9,
                                          dependentParameter);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
      NMP_ASSERT(task)
#else
      if (task)
#endif
      {
        net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);
        net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeNodeConnections->m_activeChildNodeIDs[0], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeNodeConnections->m_activeChildNodeIDs[1], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeNodeConnections->m_activeChildNodeIDs[2], INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 5, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddParamAndDependency(task, 6, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, nodeDef->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo, activeAnimSetIndex);
        net->TaskAddNetInputParam(task, 7, ATTRIB_SEMANTIC_BLEND_WEIGHTS, nodeDef->getNodeID(), INVALID_NODE_ID, currFrameNo);
        net->TaskAddDefInputParam(task, 8, ATTRIB_SEMANTIC_LOOP, nodeDef->getNodeID());
      }
    }
  }
  else //from if( activeNodeConnections->m_sampledEventsNumNodeIDs == 3 )
  {
    NMP_ASSERT( activeNodeConnections->m_sampledEventsNumNodeIDs == 2 ||
                activeNodeConnections->m_sampledEventsNumNodeIDs == 1 );

    task = nodeBlend2SyncEventsQueueSampledEventsBuffers( nodeDef,
                                                          queue,
                                                          net,                 
                                                          dependentParameter );
  }

  return task;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
