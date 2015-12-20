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
#include "morpheme/Nodes/mrNodeTransitSyncEvents.h"
#include "morpheme/Nodes/mrNodeTransit.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/mrTransitDeadBlend.h"
#include "morpheme/Nodes/mrNodeStateMachine.h" // MORPH-21351: Remove the necessity for this include.
//----------------------------------------------------------------------------------------------------------------------

// non-public template definitions
#include "mrNodeTransit.inl"

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitSyncEventsQueueTransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // We need to decide what type of blend to perform.
  if (isDeadBlending(node, net))
  {
    // Dead Blend
    return nodeQueueAnimationDeadBlendUpdateTransforms(
             node,
             queue,
             net,
             dependentParameter);
  }
  else if (isBlendingToPhysics(node, net))
  {
    AttribDataDeadBlendState* deadBlendState = getDeadBlendState(node, net);
    // We currently support the default deadblend only.
    NMP_ASSERT(deadBlendState);

    // Standard blend 2
    return nodeTransitQueueBlend2TransformBuffs(
             node,
             queue,
             net,
             dependentParameter,
             deadBlendState->m_blendMode);
  }
  else
  {
    // Look up for blending attributes in the transition definition.
    AttribDataHandle* attrib = net->getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    NMP_ASSERT(attrib && attrib->m_attribData);
    NMP_ASSERT(attrib->m_attribData->getType() == ATTRIB_TYPE_TRANSIT_SYNC_EVENTS_DEF);
    AttribDataTransitSyncEventsDef* transitDef = (AttribDataTransitSyncEventsDef*)attrib->m_attribData;

    return nodeTransitQueueBlend2TransformBuffs(
             node,
             queue,
             net,
             dependentParameter,
             transitDef->m_blendMode);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitSyncEventsQueueTrajectoryDeltaAndTransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // We need to decide what type of blend to perform.
  if (isDeadBlending(node, net))
  {
    // Dead Blend
    return nodeQueueAnimationDeadBlendUpdateTrajectoryDeltaAndTransforms(
      node,
      queue,
      net,
      dependentParameter);
  }
  else if (isBlendingToPhysics(node, net))
  {
    AttribDataDeadBlendState* deadBlendState = getDeadBlendState(node, net);
    // We currently support the default deadblend only.
    NMP_ASSERT(deadBlendState);

    // Standard blend 2
    return nodeTransitQueueBlend2TrajectoryDeltaAndTransforms(
      node,
      queue,
      net,
      dependentParameter,
      deadBlendState->m_slerpTrajectoryPosition,
      deadBlendState->m_blendMode);
  }
  else
  {
    // Look up for blending attributes in the transition definition.
    AttribDataHandle* attrib = net->getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    NMP_ASSERT(attrib && attrib->m_attribData);
    NMP_ASSERT(attrib->m_attribData->getType() == ATTRIB_TYPE_TRANSIT_SYNC_EVENTS_DEF);
    AttribDataTransitSyncEventsDef* transitDef = (AttribDataTransitSyncEventsDef*)attrib->m_attribData;

    return nodeTransitQueueBlend2TrajectoryDeltaAndTransforms(
      node,
      queue,
      net,
      dependentParameter,
      transitDef->m_slerpTrajectoryPosition,
      transitDef->m_blendMode);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitSyncEventsQueueTransformBuffsPassThroughDestTrajDelta(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // Physics networks are not supported using the combined trajectory delta and transforms semantic
  NMP_ASSERT(!isBlendingToPhysics(node, net));

  // We need to decide what type of blend to perform.
  if (isDeadBlending(node, net))
  {
    // Dead Blend
    return nodeQueueAnimationDeadBlendUpdateTrajectoryDeltaAndTransforms(
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    // Look up for blending attributes in the transition definition.
    AttribDataHandle* attrib = net->getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    NMP_ASSERT(attrib && attrib->m_attribData);
    NMP_ASSERT(attrib->m_attribData->getType() == ATTRIB_TYPE_TRANSIT_SYNC_EVENTS_DEF);

    AttribDataTransitSyncEventsDef* transitDef = (AttribDataTransitSyncEventsDef*)attrib->m_attribData;

    return nodeTransitQueueBlend2TransformsPassDestTrajDelta(
      node,
      queue,
      net,
      dependentParameter,
      transitDef->m_blendMode);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitSyncEventsQueueTransformBuffsPassThroughSourceTrajDelta(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter)
{
  // Physics networks are not supported using the combined trajectory delta and transforms semantic
  NMP_ASSERT(!isBlendingToPhysics(node, net));

  // We need to decide what type of blend to perform.
  if (isDeadBlending(node, net))
  {
    // Dead Blend
    return nodeQueueAnimationDeadBlendUpdateTrajectoryDeltaAndTransforms(
      node,
      queue,
      net,
      dependentParameter);
  }
  else
  {
    // Look up for blending attributes in the transition definition.
    AttribDataHandle* attrib = net->getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
    NMP_ASSERT(attrib && attrib->m_attribData);
    NMP_ASSERT(attrib->m_attribData->getType() == ATTRIB_TYPE_TRANSIT_SYNC_EVENTS_DEF);

    AttribDataTransitSyncEventsDef* transitDef = (AttribDataTransitSyncEventsDef*)attrib->m_attribData;

    return nodeTransitQueueBlend2TransformsPassSourceTrajDelta(
      node,
      queue,
      net,
      dependentParameter,
      transitDef->m_blendMode);
  }
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitSyncEventsQueueUpdateTimePos(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = NULL;
  NodeID thisNodeID = node->getNodeID();
  NodeID activeParentNodeID = net->getActiveParentNodeID(thisNodeID);
  FrameCount currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);

  if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, thisNodeID) != INVALID_NODE_ID)
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_TRANSITSYNCEVENTSUPDATETIMEVIASYNCEVENTPOS,
             thisNodeID,
             10,
             dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      // Update via sync event.
      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, activeParentNodeID, thisNodeID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, thisNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, thisNodeID);
      net->TaskAddOptionalNetInputOutputParam(task, 5, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddNetInputOutputParam(task, 6, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, ATTRIB_TYPE_TRANSIT_SYNC_EVENTS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 7, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, INVALID_NODE_ID, currFrameNo); // Blend weight.
      AttribDataTransitSyncEventsDef* attrDataDef = node->getAttribData<AttribDataTransitSyncEventsDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
      net->TaskAddOptionalInputCP(task, 8, ATTRIB_SEMANTIC_CP_BOOL, &(attrDataDef->m_reverseInputCPConnection));
      net->TaskAddOutputReplaceParam(task, 9, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
    }
  }
  else
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_TRANSITSYNCEVENTSUPDATETIMEVIATIMEPOS,
             thisNodeID,
             10,
             dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      // Update via time.
      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, thisNodeID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 1, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, thisNodeID, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, thisNodeID);
      net->TaskAddOptionalNetInputOutputParam(task, 5, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddNetInputOutputParam(task, 6, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, ATTRIB_TYPE_TRANSIT_SYNC_EVENTS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 7, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, INVALID_NODE_ID, currFrameNo); // Blend weight.
      AttribDataTransitSyncEventsDef* attrDataDef = node->getAttribData<AttribDataTransitSyncEventsDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
      net->TaskAddOptionalInputCP(task, 8, ATTRIB_SEMANTIC_CP_BOOL, &(attrDataDef->m_reverseInputCPConnection));
      net->TaskAddOutputReplaceParam(task, 9, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitSyncEventsQueueSyncEventTrack(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = NULL;
  if (net->getNumActiveChildren(node->getNodeID()) == 2)
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_TRANSITSYNCEVENTSBLENDSYNCEVENTTRACKS,
             node->getNodeID(),
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
      NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
      NodeID activeChildNodeID1 = net->getActiveChildNodeID(node->getNodeID(), 1);

      net->TaskAddOutputReplaceParam(task, 0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, INVALID_NODE_ID, currFrameNo);
      net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOptionalNetInputParam(task, 3, ATTRIB_SEMANTIC_BLEND_WEIGHTS, node->getNodeID(), INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
    }
  }
  else
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_TRANSITSYNCEVENTSPASSTHROUGHSYNCEVENTTRACK,
             node->getNodeID(),
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
      NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);

      net->TaskAddOutputReplaceParam(task, 0, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, INVALID_NODE_ID, currFrameNo);
      net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
      net->TaskAddOptionalNetInputOutputParam(task, 3, ATTRIB_SEMANTIC_CP_UINT, ATTRIB_TYPE_UINT, INVALID_NODE_ID, VALID_FOREVER);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitSyncEventsQueueSampledEventsBuffers(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = NULL;
  if (net->getNumActiveChildren(node->getNodeID()) == 2)
  {
    NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
    if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, node->getNodeID()) != INVALID_NODE_ID)
    {
      // Just combine the 2 source sampled events buffers.
      task = queue->createNewTaskOnQueue(
               CoreTaskIDs::MR_TASKID_COMBINE2SAMPLEDEVENTSBUFFERS,
               node->getNodeID(),
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
        NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
        NodeID activeChildNodeID1 = net->getActiveChildNodeID(node->getNodeID(), 1);

        net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);
        net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, node->getNodeID(), node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);
      }
    }
    else
    {
      // Combine the 2 source sampled events buffers and sample the duration events buffers if there are any.
      task = queue->createNewTaskOnQueue(
               CoreTaskIDs::MR_TASKID_COMBINE2SAMPLEDEVENTSBUFFERSANDSAMPLEDURATIONEVENTS,
               node->getNodeID(),
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
        NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
        NodeID activeChildNodeID1 = net->getActiveChildNodeID(node->getNodeID(), 1);

        net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);
        net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddParamAndDependency(task, 5, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddParamAndDependency(task, 6, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, node->getNodeID(), node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);
        net->TaskAddDefInputParam(task, 7, ATTRIB_SEMANTIC_LOOP, node->getNodeID());
      }
    }
  }
  else
  {
    task = queuePassThroughChild0(
             node,
             queue,
             net,
             dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeTransitSyncEventsQueueBlend2DurationEventTrackSets(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = NULL;
  if (net->getNumActiveChildren(node->getNodeID()) == 2)
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_BLEND2DURATIONEVENTTRACKSETS,
             node->getNodeID(),
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
      NodeID activeChildNodeID0 = net->getActiveChildNodeID(node->getNodeID(), 0);
      NodeID activeChildNodeID1 = net->getActiveChildNodeID(node->getNodeID(), 1);

      net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, INVALID_NODE_ID, currFrameNo);
      net->TaskAddParamAndDependency(task, 1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, activeChildNodeID0, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, ATTRIB_TYPE_DURATION_EVENT_TRACK_SET, activeChildNodeID1, INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, ATTRIB_TYPE_SYNC_EVENT_TRACK, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddParamAndDependency(task, 4, ATTRIB_SEMANTIC_BLEND_WEIGHTS, ATTRIB_TYPE_BLEND_WEIGHTS, node->getNodeID(), node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP, node->getNodeID());
      net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_LOOP, node->getNodeID());
    }
  }
  else
  {
    task = queuePassThroughChild0(
             node,
             queue,
             net,
             dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------  
namespace
{

//----------------------------------------------------------------------------------------------------------------------
// On the first frame of update of the transition, initialise the override start event index of the destination.
void initOverrideStartEventIndexOfDestination(
  Network*                              net,
  NodeID                                destNodeID,
  AttribDataTransitSyncEventsDef*       transitSyncEventsDef,
  AttribDataUpdateSyncEventPlaybackPos* sourceSyncEventPlaybackPos)
{
  int32_t destAdjustStartEventIndex = 0; // Used by destination being updated by sync events.
  float   destStartSyncEvent = 0;        // Used by destination being updated by time.

  if (transitSyncEventsDef->m_usingDestStartEventIndex)
  {
    // We are trying to find destAdjIndex such that:
    // destAdjIndex + srcAdjTime = destEventIndex + srcAdjFraction
    // Moving srcAdjTime to the right hand side we get:
    // destAdjIndex = destEventIndex + srcAdjFraction - srcAdjTime = destEventIndex + srcAdjFraction - srcAdjIndex - srcAdjFraction = destEventIndex - srcAdjIndex
    // We are being updated via delta sync event from our parent and or we are not an immediate transition.
    destAdjustStartEventIndex = transitSyncEventsDef->m_destStartEventIndex;
    destStartSyncEvent = ((float) transitSyncEventsDef->m_destStartEventIndex);
    if (sourceSyncEventPlaybackPos)
    {
      float sourceEventPos = sourceSyncEventPlaybackPos->m_absPosAdj.get();
      int32_t sourceIndexPart = (int32_t)(sourceEventPos);
      float sourceFractionPart = sourceEventPos - ((float)sourceIndexPart);
      destAdjustStartEventIndex -= sourceIndexPart;
      destStartSyncEvent += sourceFractionPart;
    }
  }
  else
  {
    // We are trying to find destAdjIndex such that:
    // destAdjIndex + srcAdjTime = srcRealTime + transOffset = srcAdjTime + srcStartEventIndex + transOffset
    // Moving srcRealTime to the right hand side we get:
    // destAdjIndex = srcRealTime + transOffset - srcAdjTime = srcRealIndex + transOffset - srcAdjIndex
    // The last equality is true because the real and adjusted fractional parts are always equal, and the
    // difference of the real and adjusted times is an integer that is equal to the difference of the indices.
    destAdjustStartEventIndex = transitSyncEventsDef->m_destEventTrackOffset + sourceSyncEventPlaybackPos->m_absPosReal.index() - sourceSyncEventPlaybackPos->m_absPosAdj.index();
    if (sourceSyncEventPlaybackPos)
      destStartSyncEvent = destAdjustStartEventIndex + sourceSyncEventPlaybackPos->m_absPosAdj.get();
  }

  AttribAddress address(ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, destNodeID, INVALID_NODE_ID);
  AttribDataPlaybackPosInit* destAdjustStartSyncEventAttrib = net->getOptionalAttribData<AttribDataPlaybackPosInit>(address);

  // Attrib already exists?
  if (!destAdjustStartSyncEventAttrib)
  {
    // Create the new adjust index for the destination.
    AttribDataHandle handle = AttribDataPlaybackPosInit::create(
                                net->getPersistentMemoryAllocator(),
                                false,
                                destStartSyncEvent,
                                destAdjustStartEventIndex);
    net->addAttribData(address, handle, LIFESPAN_FOREVER);
  }
  else
  {
    // Overwrite existing ATTRIB_SEMANTIC_PLAYBACK_POS_INIT parameters.
    // Occurs on a second wildcard / global self-transition.
    destAdjustStartSyncEventAttrib->m_initWithDurationFraction = false;
    destAdjustStartSyncEventAttrib->m_startValue               = destStartSyncEvent;
    destAdjustStartSyncEventAttrib->m_adjustStartEventIndex    = destAdjustStartEventIndex;
  }
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTransitSyncEvents* initTransition(
  NodeDef*                        node,
  Network*                        net,
  AttribDataTransitSyncEventsDef* transitSyncEventsDef,
  NodeConnections*                nodeConnections)
{
  NodeID thisNodeID = node->getNodeID();
  AttribAddress address(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, thisNodeID, INVALID_NODE_ID, VALID_FRAME_ANY_FRAME);
  AttribDataTransitSyncEvents* transitState = net->getOptionalAttribData<AttribDataTransitSyncEvents>(address);

  if (!transitState)
  {
    // This transition has just become active initialise the transition state.
    FrameCount lastFrameNo = net->getCurrentFrameNo() - 1;

    // Create the state attrib and initialise.
    AttribDataHandle handle = AttribDataTransitSyncEvents::create(net->getPersistentMemoryAllocator());
    transitState = (AttribDataTransitSyncEvents*) handle.m_attribData;
    address.m_validFrame = lastFrameNo;
    net->addAttribData(address, handle, LIFESPAN_FOREVER);

    // Get the source nodes current event index.
    const NodeID activeChildNodeID0 = net->getActiveChildNodeID(thisNodeID, 0);
    AttribDataUpdateSyncEventPlaybackPos* sourceSyncEventPlaybackPos =
      (AttribDataUpdateSyncEventPlaybackPos*) net->getAttribDataRecurseFilterNodes(
                                                      ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS,
                                                      activeChildNodeID0,
                                                      INVALID_NODE_ID,
                                                      lastFrameNo);

    NMP_ASSERT(nodeConnections->m_numActiveChildNodes == 2);
    const NodeID activeChildNodeID1 = nodeConnections->m_activeChildNodeIDs[1];

    if (sourceSyncEventPlaybackPos)
    {
      //-----------
      // Initialise our (transition) starting event position with the source event position.
      address.init(ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, thisNodeID, INVALID_NODE_ID, lastFrameNo);
      AttribDataHandle myLastFrameUpdateSyncEventHandle = AttribDataUpdateSyncEventPlaybackPos::create(net->getPersistentMemoryAllocator());
      AttribDataUpdateSyncEventPlaybackPos::copy(sourceSyncEventPlaybackPos, myLastFrameUpdateSyncEventHandle.m_attribData);
      net->addAttribData(address, myLastFrameUpdateSyncEventHandle, LIFESPAN_FOREVER);

      if (activeChildNodeID0 != activeChildNodeID1)
      {
        // Delete any previously generated sync event pos or sync event track that are hanging around on the destination.
        address.init(ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, activeChildNodeID1, INVALID_NODE_ID, lastFrameNo);
        AttribDataSemantic attribDataSemantics[] = { ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK };
        NodeBin* nodeBin = net->getNodeBin(activeChildNodeID1);
        nodeBin->deleteAllAttribDataSpecified(2, attribDataSemantics);

        // Supply the destination with a event start position.
        AttribDataHandle childLastFrameUpdateSyncEventHandle = AttribDataUpdateSyncEventPlaybackPos::create(net->getPersistentMemoryAllocator());
        AttribDataUpdateSyncEventPlaybackPos::copy(sourceSyncEventPlaybackPos, childLastFrameUpdateSyncEventHandle.m_attribData);        
        net->addAttribData(address, childLastFrameUpdateSyncEventHandle, 1);
      }
      else
      {
        // A self-transition - will be an immediate dead blend, already has sync event pos from previous frame
        NMP_ASSERT(net->getAttribDataNodeBinEntryRecurseFilterNodes(ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, activeChildNodeID1, INVALID_NODE_ID, lastFrameNo));
        
        // Delete any previously generated sync event track
        AttribDataSemantic attribDataSemantic = ATTRIB_SEMANTIC_SYNC_EVENT_TRACK;
        NodeBin* nodeBin = net->getNodeBin(activeChildNodeID1);
        nodeBin->deleteAllAttribDataSpecified(1, &attribDataSemantic);
      }

      //-----------
      // Initialise the override start event index of our destination.

      // We are being updated via delta sync event from our parent.
      initOverrideStartEventIndexOfDestination(
        net,
        activeChildNodeID1,
        transitSyncEventsDef,
        sourceSyncEventPlaybackPos);
    }
  }
 
  NMP_ASSERT(transitState);
  return transitState;
}

//----------------------------------------------------------------------------------------------------------------------  
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
// Calculate dead blend duration from transit duration in events
float getDeadBlendDuration(
  Network*                        net,
  NodeID                          nodeID,
  AttribDataTransitSyncEventsDef* transitDef)
{
  FrameCount lastFrameNo = net->getCurrentFrameNo() - 1;

  // Convert the duration in events in to a duration in time.
  // If the specified for the duration < 0, the dead blend task ignores it.
  float duration = -1.0f;

  NodeID activeChildNodeID0 = net->getActiveChildNodeID(nodeID, 0);
  AttribDataUpdateSyncEventPlaybackPos* sourceSyncEventPlaybackPos = (AttribDataUpdateSyncEventPlaybackPos*)
  net->getAttribDataRecurseFilterNodes(
    ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS,
    activeChildNodeID0,
    activeChildNodeID0,
    lastFrameNo);
  if (sourceSyncEventPlaybackPos)
  {
    AttribDataSyncEventTrack* synchEventTrack = (AttribDataSyncEventTrack*) net->getAttribDataRecurseFilterNodes(
      ATTRIB_SEMANTIC_SYNC_EVENT_TRACK,
      activeChildNodeID0,
      activeChildNodeID0,
      lastFrameNo);
    if (synchEventTrack)
    {
      float startFraction = synchEventTrack->m_syncEventTrack.getRealPosFractionFromAdjSyncEventPos(sourceSyncEventPlaybackPos->m_absPosAdj);
      float endEventPos = sourceSyncEventPlaybackPos->m_absPosAdj.get() + transitDef->m_durationInEvents;
      float endFraction = synchEventTrack->m_syncEventTrack.getRealPosFractionFromAdjSyncEventPos(endEventPos);
      float durationFraction = endFraction - startFraction;
      if (durationFraction < 0.0f)
        durationFraction += 1.0f;
      duration = durationFraction * synchEventTrack->m_syncEventTrack.getDuration();
    }
  }

  return duration;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeTransitSyncEventsUpdateConnections(
  NodeDef* node,
  Network* net)
{
  NodeID thisNodeID = node->getNodeID();
  NodeID resultingNodeID = thisNodeID;
  NodeConnections* nodeConnections = net->getActiveNodesConnections(thisNodeID); // Get this node's connections structure.
  
  // Get the transit def attrib data.
  AttribDataTransitSyncEventsDef* transitDef = node->getAttribData<AttribDataTransitSyncEventsDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  
  // Get any existing transit state data. Usually updated last frame.
  AttribDataTransitSyncEvents* transitState = initTransition(node, net, transitDef, nodeConnections);

  // If we have a transition into a sub state
  if (transitDef->m_nodeInitData && nodeConnections->m_justBecameActive)
  {
    NodeInitDataArrayDef* destInitData = transitDef->m_nodeInitData;
    for (uint32_t i = 0; i < destInitData->m_numNodeInitDatas; ++i)
    {
      // MORPH-21356: We only deal with init of state machine start state at the moment.
      NMP_ASSERT(destInitData->m_nodeInitDataArray[i]->getType() == NODE_INIT_DATA_TYPE_STATE_MACHINE);
      NodeID targetSMNodeID = destInitData->m_nodeInitDataArray[i]->getTargetNodeID();
      StateMachineInitData* smInitData = (StateMachineInitData*) destInitData->m_nodeInitDataArray[i];
      NodeID startingStateNodeID = smInitData->getInitialSubStateID();

      net->queueSetStateMachineStateByNodeID(targetSMNodeID, startingStateNodeID);
    }
  }

  // The transition is complete when the flag in its state is set or if the duration is 0.
  if (transitState->m_completed)
  {
    // Transition has completed.
    resultingNodeID = nodeTransitCompleteTransition(node, net, nodeConnections, transitState->m_reversed);
  }
  else
  {
    // Transition not complete.
    updateTransition(node, net, transitDef, nodeConnections);
  }

  NodeBinEntry* deadBlendTransformsStateEntry = net->getAttribDataNodeBinEntry(
    ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE,
    node->getNodeID(),
    INVALID_NODE_ID,
    VALID_FRAME_ANY_FRAME);
  if (deadBlendTransformsStateEntry)
  {
    NMP_ASSERT(deadBlendTransformsStateEntry->m_address.m_animSetIndex != ANIMATION_SET_ANY);
    AnimSetIndex animSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    if (deadBlendTransformsStateEntry->m_address.m_animSetIndex != animSetIndex)
    {
      NodeBinEntry* deadBlendTransformsRateEntry = net->getAttribDataNodeBinEntry(
        ATTRIB_SEMANTIC_TRANSFORM_RATES,
        node->getNodeID(),
        INVALID_NODE_ID,
        VALID_FRAME_ANY_FRAME);
      NMP_ASSERT(deadBlendTransformsRateEntry);
      NMP_ASSERT(deadBlendTransformsStateEntry->m_address.m_animSetIndex == deadBlendTransformsRateEntry->m_address.m_animSetIndex);

      // The animation set has changed. Create a new one and map the old one to it.
      AnimRigDef* targetRigDef = net->getRig(animSetIndex);

      {
        NMP::Memory::Format internalBuffMemReqs;
        NMP::Memory::Format buffMemReqs;
        uint32_t numRigJoints = targetRigDef->getNumBones();
        AttribDataTransformBuffer::getPosQuatMemoryRequirements(numRigJoints, buffMemReqs, internalBuffMemReqs);
        AttribDataTransformBuffer* targetTransformsAttr = AttribDataTransformBuffer::createPosQuat(deadBlendTransformsStateEntry->m_allocatorUsed, buffMemReqs, internalBuffMemReqs, numRigJoints, 1);
        AttribDataTransformBuffer* transformsAttr = (AttribDataTransformBuffer*)deadBlendTransformsStateEntry->getAttribData();
        net->getNetworkDef()->mapCopyTransformBuffers(transformsAttr->m_transformBuffer, deadBlendTransformsStateEntry->m_address.m_animSetIndex, targetTransformsAttr->m_transformBuffer, animSetIndex);

        deadBlendTransformsStateEntry->removeReference();
        deadBlendTransformsStateEntry->m_address.m_animSetIndex = animSetIndex;
        deadBlendTransformsStateEntry->m_attribDataHandle.m_attribData = targetTransformsAttr;
        deadBlendTransformsStateEntry->m_attribDataHandle.m_format = buffMemReqs;
      }

      {
        NMP::Memory::Format internalBuffMemReqs;
        NMP::Memory::Format buffMemReqs;
        uint32_t numRigJoints = targetRigDef->getNumBones();
        AttribDataTransformBuffer::getPosQuatMemoryRequirements(numRigJoints, buffMemReqs, internalBuffMemReqs);
        AttribDataTransformBuffer* targetTransformsAttr = AttribDataTransformBuffer::createPosVelAngVel(deadBlendTransformsRateEntry->m_allocatorUsed, buffMemReqs, internalBuffMemReqs, numRigJoints, 1);
        AttribDataTransformBuffer* transformsAttr = (AttribDataTransformBuffer*)deadBlendTransformsRateEntry->getAttribData();
        net->getNetworkDef()->mapCopyVelocityBuffers(transformsAttr->m_transformBuffer, deadBlendTransformsRateEntry->m_address.m_animSetIndex, targetTransformsAttr->m_transformBuffer, animSetIndex);

        deadBlendTransformsRateEntry->removeReference();
        deadBlendTransformsRateEntry->m_address.m_animSetIndex = animSetIndex;
        deadBlendTransformsRateEntry->m_attribDataHandle.m_attribData = targetTransformsAttr;
        deadBlendTransformsRateEntry->m_attribDataHandle.m_format = buffMemReqs;
      }
    }
  }

  return resultingNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeTransitSyncEventsFindGeneratingNodeForSemantic(
  NodeID              NMP_UNUSED(callingNodeID),
  bool                NMP_UNUSED(fromParent),   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            NMP_UNUSED(network))
{
  NodeID result = INVALID_NODE_ID;

  switch (semantic)
  {
  case ATTRIB_SEMANTIC_TIME_POS:
  case ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS:
  case ATTRIB_SEMANTIC_UPDATE_TIME_POS:
  case ATTRIB_SEMANTIC_SYNC_EVENT_TRACK:
  case ATTRIB_SEMANTIC_TRANSFORM_BUFFER:
  case ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_T_MINUS_ONE:
  case ATTRIB_SEMANTIC_BLEND_WEIGHTS:
    result = node->getNodeID();
    break;
  default:
    break;
  }

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
