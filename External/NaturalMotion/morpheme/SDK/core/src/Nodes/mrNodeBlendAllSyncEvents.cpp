// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#include "morpheme/Nodes/mrNodeBlendAll.h"
#include "morpheme/Nodes/mrnodeBlendAllSyncEvents.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
namespace
{
  
//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlendAllSyncEventsGetNodeIDIfOnlyOneActive(NodeDef* node, Network* net)
{
  // Helper function to test if the only input is active
  // If so, return the only active node so its transforms can be passed straight through.
  // This avoids doing an unnecessary blend.

  NodeID onlyActiveNode = (net->getNumActiveChildren(node->getNodeID()) == 1) ? 
    net->getActiveChildNodeID(node->getNodeID(), 0) : INVALID_NODE_ID;

  return onlyActiveNode;
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// SyncEventTrack
//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlendAllSyncEventsQueueSyncEventTrack(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  const NodeID activeNode = nodeBlendAllSyncEventsGetNodeIDIfOnlyOneActive(node, net);

  if (activeNode != INVALID_NODE_ID)
  {
    return queuePassThroughOnChildNodeID(
      activeNode,
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    NodeConnections* nodeConnections = net->getActiveNodesConnections(node->getNodeID()); // Get this nodes connections structure.
    NMP_ASSERT(nodeConnections);

    Task* task = queue->createNewTaskOnQueue(
                   CoreTaskIDs::MR_TASKID_BLENDALLSYNCEVENTTRACKS,
                   node->getNodeID(),
                   nodeConnections->m_numActiveChildNodes + 4,
                   dependentParameter, 
                   false, 
                   true, 
                   true); // It manages its own DMA
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);

      net->TaskAddOutputReplaceParam(task, 0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, INVALID_NODE_ID, currFrameNo);

      net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX, node->getNodeID());

      uint32_t param = 1;
      // input source
      for (uint32_t i = 0; i < nodeConnections->m_numActiveChildNodes; i++)
      {
        net->TaskAddParamAndDependency(
          task,
          ++param,
          ATTRIB_SEMANTIC_SYNC_EVENT_TRACK,
          ATTRIB_TYPE_SYNC_EVENT_TRACK,
          nodeConnections->m_activeChildNodeIDs[i],
          INVALID_NODE_ID,
          TPARAM_FLAG_INPUT,
          currFrameNo,
          activeAnimSetIndex);
      }

      // Array of normalised blend weights
      net->TaskAddNetInputParam(task, ++param, ATTRIB_SEMANTIC_CP_FLOAT, node->getNodeID(), INVALID_NODE_ID, currFrameNo);

      // Find if someone (usually a transition) has stored a start sync event adjustment on us or on a filter node parent.
      net->TaskAddOptionalNetInputParamRecurseToParent(task, ++param, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, ANIMATION_SET_ANY);
    }

    return task;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Duration event track sets.
//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlendAllSyncEventsQueueBlend2DurationEventTrackSets(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  const NodeID activeNode = nodeBlendAllSyncEventsGetNodeIDIfOnlyOneActive(node, net);

  if (activeNode != INVALID_NODE_ID)
  {
    return queuePassThroughOnChildNodeID(
      activeNode,
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    NodeConnections* nodeConnections = net->getActiveNodesConnections(node->getNodeID());
    NMP_ASSERT(nodeConnections);

    Task* task = queue->createNewTaskOnQueue(
                   CoreTaskIDs::MR_TASKID_BLENDALLDURATIONEVENTTRACKSETS,
                   node->getNodeID(),
                   nodeConnections->m_numActiveChildNodes + 5,
                   dependentParameter, 
                   false, 
                   true, 
                   true); // It manages its own DMA
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      NMP_ASSERT(currFrameNo > 0);

      net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, INVALID_NODE_ID, currFrameNo);

      net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

      net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP, node->getNodeID());
      net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_LOOP, node->getNodeID());

      uint32_t param = 3;
      for (uint32_t i = 0; i < nodeConnections->m_numActiveChildNodes; i++)
      {
        net->TaskAddParamAndDependency(
          task,
          ++param,
          ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET,
          ATTRIB_TYPE_DURATION_EVENT_TRACK_SET,
          nodeConnections->m_activeChildNodeIDs[i],
          INVALID_NODE_ID,
          TPARAM_FLAG_INPUT,
          currFrameNo);
      }

      // Array of normalised blend weights
      net->TaskAddNetInputParam(task, ++param, ATTRIB_SEMANTIC_CP_FLOAT, node->getNodeID(), INVALID_NODE_ID, currFrameNo);
    }

    return task;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Sampled Event Buffer
//----------------------------------------------------------------------------------------------------------------------
Task* nodeBlendAllSyncEventsQueueSampledEventsBuffers(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  const NodeID activeNode = nodeBlendAllSyncEventsGetNodeIDIfOnlyOneActive(node, net);

  if (activeNode != INVALID_NODE_ID)
  {
    return queuePassThroughOnChildNodeID(
      activeNode,
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    NodeConnections* nodeConnections = net->getActiveNodesConnections(node->getNodeID()); // Get this nodes connections structure.
    NMP_ASSERT(nodeConnections);

    Task* task;
    NodeID activeParentNodeID = nodeConnections->m_activeParentNodeID;
    if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, node->getNodeID()) != INVALID_NODE_ID)
    {
      task = nodeBlendAllQueueSampledEventsBuffers(
               node,
               queue,
               net,
               dependentParameter);
    }
    else
    {
      // Combine the 2 source sampled events buffers and sample the duration events buffers if there are any.
      task = queue->createNewTaskOnQueue(
               CoreTaskIDs::MR_TASKID_COMBINEALLSAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS,
               node->getNodeID(),
               nodeConnections->m_numActiveChildNodes + 6,
               dependentParameter, 
               false, 
               true, 
               true); // It manages its own DMA
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
      NMP_ASSERT(task)
#else
      if (task)
#endif
      {
        FrameCount currFrameNo = net->getCurrentFrameNo();
        NMP_ASSERT(currFrameNo > 0);

        net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);

        net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);

        net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_LOOP, node->getNodeID());

        uint32_t param = 4;
        // inputs
        for (uint32_t i = 0; i < nodeConnections->m_numActiveChildNodes; i++)
        {
          net->TaskAddParamAndDependency(
            task,
            ++param,
            ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER,
            ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER,
            nodeConnections->m_activeChildNodeIDs[i],
            INVALID_NODE_ID,
            TPARAM_FLAG_INPUT,
            currFrameNo);
        }

        // Array of normalised blend weights
        net->TaskAddNetInputParam(task, ++param, ATTRIB_SEMANTIC_CP_FLOAT, node->getNodeID(), INVALID_NODE_ID, currFrameNo);
      }
    }

    return task;
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
