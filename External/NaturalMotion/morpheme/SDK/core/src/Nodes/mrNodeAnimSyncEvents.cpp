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
#include "morpheme/Nodes/mrNodeAnimSyncEvents.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/AnimSource/mrAnimSource.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeAnimSyncEventsUpdateConnections(
  NodeDef* node,
  Network* net)
{
  NodeID thisNodeID = node->getNodeID();
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(thisNodeID);
    
  //------------------------------------------
  // Create AttribDataSyncEventTrack if it does not already exist.
  AttribDataHandle* syncEventTrackDefHandle = node->getOptionalAttribDataHandle(ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, activeAnimSetIndex);
  AttribDataSyncEventTrack* syncEventTrack = syncEventTrackDefHandle ? (AttribDataSyncEventTrack*)syncEventTrackDefHandle->m_attribData : NULL;

  if (!syncEventTrack)
  {
    NodeBinEntry* syncEventTrackEntry = net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, thisNodeID, INVALID_NODE_ID, VALID_FRAME_ANY_FRAME, activeAnimSetIndex);

    if (!syncEventTrackEntry)
    {
      AttribDataSourceEventTrackSet* sourceEventTracks = node->getAttribData<AttribDataSourceEventTrackSet>(ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS, activeAnimSetIndex);
      AttribDataBool* loopable = node->getAttribData<AttribDataBool>(ATTRIB_SEMANTIC_LOOP);
      AttribDataSourceAnim* sourceAnim = node->getAttribData<AttribDataSourceAnim>(ATTRIB_SEMANTIC_SOURCE_ANIM, activeAnimSetIndex);

      // Find if someone (usually a transition) has stored a start sync event adjustment on us or on a filter node parent.
      NodeBinEntry* playbackPosInitEntry = net->getAttribDataNodeBinEntryRecurseToParent(ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, thisNodeID, INVALID_NODE_ID, VALID_FOREVER, ANIMATION_SET_ANY);
      AttribDataPlaybackPosInit* playbackPosInit = playbackPosInitEntry ? (AttribDataPlaybackPosInit*)playbackPosInitEntry->getAttribData() : NULL;

      AttribDataHandle syncEventTrackHandle = AttribDataSyncEventTrack::create(net->getPersistentMemoryAllocator());
      syncEventTrack = (AttribDataSyncEventTrack*)syncEventTrackHandle.m_attribData;

      // Initialise the synchronisation event track from the appropriate discrete event track.
      NMP_ASSERT(sourceAnim->m_syncEventTrackIndex < sourceEventTracks->m_numDiscreteEventTracks);

      // Add any adjust start sync event to the start sync event specified in the def.
      int32_t startSyncEvent = sourceAnim->m_startSyncEventIndex;
      if (playbackPosInit)
        startSyncEvent += playbackPosInit->m_adjustStartEventIndex;

      syncEventTrack->m_syncEventTrack.init(
                                        sourceAnim->m_clipStartFraction,
                                        (sourceAnim->m_clipEndFraction - sourceAnim->m_clipStartFraction),
                                        sourceAnim->m_clipStartSyncEventIndex,
                                        sourceEventTracks->m_sourceDiscreteEventTracks[sourceAnim->m_syncEventTrackIndex],
                                        loopable->m_value,
                                        startSyncEvent,
                                        (sourceAnim->m_sourceAnimDuration * (sourceAnim->m_clipEndFraction - sourceAnim->m_clipStartFraction)),
                                        sourceAnim->m_playBackwards);

      AttribAddress syncEventTrackAddress(ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, thisNodeID, INVALID_NODE_ID, VALID_FOREVER, activeAnimSetIndex);
      net->addAttribData(syncEventTrackAddress, syncEventTrackHandle, LIFESPAN_FOREVER);
    }
    else
    {
      syncEventTrack = (AttribDataSyncEventTrack*)syncEventTrackEntry->getAttribData();
    }
  }

  //------------------------------------------
  // Create AttribDataDurationEventTrackSet if it does not already exist.
  NodeBinEntry* durationEventTrackSetEntry = net->getAttribDataNodeBinEntry(ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, thisNodeID, INVALID_NODE_ID, VALID_FRAME_ANY_FRAME, activeAnimSetIndex);
  if (!durationEventTrackSetEntry)
  {
    AttribDataSourceEventTrackSet* sourceEventTracks = node->getAttribData<AttribDataSourceEventTrackSet>(ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS, activeAnimSetIndex);
    AttribDataBool* loopable = node->getAttribData<AttribDataBool>(ATTRIB_SEMANTIC_LOOP);
    AttribDataSourceAnim* sourceAnim = node->getAttribData<AttribDataSourceAnim>(ATTRIB_SEMANTIC_SOURCE_ANIM, activeAnimSetIndex);

    // Work out the total number of tracks and events required in the output set.
    uint32_t numEventTracks = sourceEventTracks->m_numDurEventTracks;
    uint32_t numEvents = 0;
    for (uint32_t i = 0; i < numEventTracks; ++i)
    {
      numEvents += sourceEventTracks->m_sourceDurEventTracks[i]->getNumEvents();
    }

    AttribDataHandle durationEventTrackSetHandle = AttribDataDurationEventTrackSet::create(net->getPersistentMemoryAllocator(), numEventTracks, numEvents);
    AttribDataDurationEventTrackSet* durationEventTrackSet = (AttribDataDurationEventTrackSet*)durationEventTrackSetHandle.m_attribData;

    // Initialise the duration event track set from the source event tracks.
    durationEventTrackSet->m_durationEventTrackSet->initFromSourceDef(
                                          sourceAnim->m_clipStartFraction,
                                          (sourceAnim->m_clipEndFraction - sourceAnim->m_clipStartFraction),
                                          loopable->m_value,
                                          numEventTracks,
                                          sourceEventTracks->m_sourceDurEventTracks,
                                          &(syncEventTrack->m_syncEventTrack),
                                          sourceAnim->m_playBackwards);

    AttribAddress durationEventTrackSetAddress(ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, thisNodeID, INVALID_NODE_ID, VALID_FOREVER, activeAnimSetIndex);
    net->addAttribData(durationEventTrackSetAddress, durationEventTrackSetHandle, LIFESPAN_FOREVER);
  }
    

  return thisNodeID;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueTimePosPreCompSyncTrack(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // Build list of all parameters (attributes) that are needed for the task to operate.
  // Also build a list of attribute addresses that we are dependent on having been updated before us.
  Task* task;
  NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());

  if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, node->getNodeID()) != INVALID_NODE_ID)
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_ANIMSYNCEVENTSUPDATESYNCEVENTPOS,
             node->getNodeID(),
             9,
             dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
      NMP_ASSERT(currFrameNo > 0);

      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 1, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);

      net->TaskAddOptionalNetInputOutputParam(task, 2, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);

      net->TaskAddOutputReplaceParam(task, 3, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 4, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 5, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);
      net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_LOOP, node->getNodeID());
      net->TaskAddDefInputParam(task, 7, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
      net->TaskAddDefInputParam(task, 8, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, node->getNodeID(), activeAnimSetIndex);
    }
  }
  else
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_ANIMSYNCEVENTSUPDATETIMEPOS,
             node->getNodeID(),
             10,
             dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
      NMP_ASSERT(currFrameNo > 0);

      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOptionalNetInputOutputParam(task, 1, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 2, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 3, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 4, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 5, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);
      net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_LOOP, node->getNodeID());
      net->TaskAddDefInputParam(task, 7, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
      net->TaskAddDefInputParam(task, 8, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, node->getNodeID(), activeAnimSetIndex);

      // Find if someone (usually a transition) has stored a start playback pos adjustment on us or on a filter node parent.
      net->TaskAddOptionalNetInputParamRecurseToParent(task, 9, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, ANIMATION_SET_ANY);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueTimePosDynamicSyncTrack(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // Build list of all parameters (attributes) that are needed for the task to operate.
  // Also build a list of attribute addresses that we are dependent on having been updated before us.
  Task* task;
  NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());

  if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, node->getNodeID()) != INVALID_NODE_ID)
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_ANIMSYNCEVENTSUPDATESYNCEVENTPOS,
             node->getNodeID(),
             9,
             dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
      NMP_ASSERT(currFrameNo > 0);

      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 1, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);

      net->TaskAddOptionalNetInputOutputParam(task, 2, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);

      net->TaskAddOutputReplaceParam(task, 3, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 4, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 5, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);
      net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_LOOP, node->getNodeID());

      net->TaskAddDefInputParam(task, 7, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
      net->TaskAddNetInputParam(task, 8, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, activeAnimSetIndex);
    }
  }
  else
  {
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_ANIMSYNCEVENTSUPDATETIMEPOS,
             node->getNodeID(),
             10,
             dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
      NMP_ASSERT(currFrameNo > 0);

      net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_UPDATE_TIME_POS, ATTRIB_TYPE_UPDATE_PLAYBACK_POS, activeParentNodeID, node->getNodeID(), TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddOptionalNetInputOutputParam(task, 1, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 2, ATTRIB_SEMANTIC_TIME_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 3, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputReplaceParam(task, 4, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, INVALID_NODE_ID, currFrameNo);
      net->TaskAddOutputParamZeroLifespan(task, 5, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE, ATTRIB_TYPE_BOOL, INVALID_NODE_ID, currFrameNo);
      net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_LOOP, node->getNodeID());
      net->TaskAddDefInputParam(task, 7, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
      net->TaskAddNetInputParam(task, 8, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, activeAnimSetIndex);

      // Find if someone (usually a transition) has stored a start playback pos adjustment on us or on a filter node parent.
      net->TaskAddOptionalNetInputParamRecurseToParent(task, 9, ATTRIB_SEMANTIC_PLAYBACK_POS_INIT, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, ANIMATION_SET_ANY);
    }
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// All the supported anim formats.
//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueSampleTransformsFromAnimSource(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // Get the active animation set from the network
  Task* task = NULL;
  uint16_t activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

  // Get the animation source attrib for the current animation set in the
  // network definition
  NetworkDef* netDef = net->getNetworkDef();
  NMP_ASSERT(netDef);

  AttribDataHandle* handle = netDef->getAttribDataHandle(
                               ATTRIB_SEMANTIC_SOURCE_ANIM,
                               node->getNodeID(),
                               activeAnimSetIndex);
  NMP_ASSERT(handle);
  AttribDataSourceAnim* sourceAnimAttrib = static_cast<AttribDataSourceAnim*>(handle->m_attribData);
  if (sourceAnimAttrib)
  {
    // Get the animation registry entry for the source animation format
    const Manager::AnimationFormatRegistryEntry* entry = Manager::getInstance().getAnimationFormatRegistryEntry(sourceAnimAttrib->m_registeredAnimFormatIndex);
    NMP_ASSERT(entry);

    // Queue the correct sample transforms task
    task = entry->m_queueAttrTaskAnimSourceFn(node, queue, net, dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueSampleTransformsFromAnimSourceASA(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_SAMPLETRANSFORMSFROMANIMSOURCEASA,
                 node->getNodeID(),
                 3,
                 dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);
    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueSampleTransformsFromAnimSourceMBA(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_SAMPLETRANSFORMSFROMANIMSOURCEMBA,
                 node->getNodeID(),
                 3,
                 dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueSampleTransformsFromAnimSourceNSA(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_SAMPLETRANSFORMSFROMANIMSOURCENSA,
                 node->getNodeID(),
                 3,
                 dependentParameter);

#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);
   
    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueSampleTransformsFromAnimSourceQSA(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_SAMPLETRANSFORMSFROMANIMSOURCEQSA,
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
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    net->TaskAddParamAndDependency(task, 0, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, activeAnimSetIndex);
    net->TaskAddOutputParamZeroLifespan(task, 3, ATTRIB_SEMANTIC_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// All the supported anim trajectory formats.
//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueTrajectoryDeltaTransform(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // Get the active animation set from the network
  Task* task = NULL;
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

  // Get the animation source attrib for the current animation set in the
  // network definition
  NetworkDef* netDef = net->getNetworkDef();
  NMP_ASSERT(netDef);

  AttribDataHandle* handle = netDef->getAttribDataHandle(
                               ATTRIB_SEMANTIC_SOURCE_ANIM,
                               node->getNodeID(),
                               activeAnimSetIndex);
  NMP_ASSERT(handle);
  AttribDataSourceAnim* sourceAnimAttrib = static_cast<AttribDataSourceAnim*>(handle->m_attribData);
  if (sourceAnimAttrib)
  {
    // Get the animation registry entry for the source animation format
    const Manager::AnimationFormatRegistryEntry* entry = Manager::getInstance().getAnimationFormatRegistryEntry(sourceAnimAttrib->m_registeredAnimFormatIndex);
    NMP_ASSERT(entry);

    // Queue the correct sample transforms task
    task = entry->m_queueAttrTaskTrajectorySourceFn(node, queue, net, dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueTrajectoryDeltaTransformASA(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYFROMTRAJECTORYSOURCEASA,
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
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);
    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE, ATTRIB_TYPE_BOOL, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueTrajectoryDeltaTransformMBA(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYFROMTRAJECTORYSOURCEMBA,
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
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE, ATTRIB_TYPE_BOOL, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueTrajectoryDeltaTransformNSA(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYFROMTRAJECTORYSOURCENSA,
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
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE, ATTRIB_TYPE_BOOL, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueTrajectoryDeltaTransformQSA(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  Task* task = queue->createNewTaskOnQueue(
                 CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYFROMTRAJECTORYSOURCEQSA,
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
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM, INVALID_NODE_ID, currFrameNo);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE, ATTRIB_TYPE_BOOL, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// All the supported anim formats.
//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSource(
  NodeDef* node,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  // Get the active animation set from the network
  Task* task = NULL;
  AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

  // Get the animation source attrib for the current animation set in the
  // network definition
  NetworkDef* netDef = net->getNetworkDef();
  NMP_ASSERT(netDef);

  AttribDataHandle* handle = netDef->getAttribDataHandle(
    ATTRIB_SEMANTIC_SOURCE_ANIM,
    node->getNodeID(),
    activeAnimSetIndex);
  NMP_ASSERT(handle);
  AttribDataSourceAnim* sourceAnimAttrib = static_cast<AttribDataSourceAnim*>(handle->m_attribData);
  if (sourceAnimAttrib)
  {
    // Get the animation registry entry for the source animation format
    const Manager::AnimationFormatRegistryEntry* entry = Manager::getInstance().getAnimationFormatRegistryEntry(sourceAnimAttrib->m_registeredAnimFormatIndex);
    NMP_ASSERT(entry);

    // Queue the correct sample transforms task
    task = entry->m_queueAttrTaskTrajectoryTransformSourceFn(node, queue, net, dependentParameter);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Task* nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceCommon(
  TaskID taskID,
  NodeDef* node,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter,
  uint32_t noOfTaskParams = 4)
{
  Task* task = queue->createNewTaskOnQueue(
    taskID,
    node->getNodeID(),
    noOfTaskParams,
    dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
  NMP_ASSERT(task)
#else
  if (task)
#endif
  {
    FrameCount currFrameNo = net->getCurrentFrameNo();
    AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());
    NMP_ASSERT(currFrameNo > 0);

    net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, ATTRIB_TYPE_TRANSFORM_BUFFER, INVALID_NODE_ID, currFrameNo, activeAnimSetIndex);
    net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
    net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_ANIM_SAMPLE_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
    net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_LOOPED_ON_UPDATE, ATTRIB_TYPE_BOOL, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
  }

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceASA(
  NodeDef* node,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  return nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceCommon(
    CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYANDTRANSFORMSFROMSOURCEASA,
    node,
    queue,
    net,
    dependentParameter);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceMBA(
  NodeDef* node,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  return nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceCommon(
    CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYANDTRANSFORMSFROMSOURCEMBA,
    node,
    queue,
    net,
    dependentParameter);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceNSA(
  NodeDef* node,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  return nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceCommon(
    CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYANDTRANSFORMSFROMSOURCENSA,
    node,
    queue,
    net,
    dependentParameter);
}

//----------------------------------------------------------------------------------------------------------------------
Task* nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceQSA(
  NodeDef* node,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter)
{
  MR::Task* task = nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSourceCommon(
    CoreTaskIDs::MR_TASKID_UPDATEDELTATRAJECTORYANDTRANSFORMSFROMSOURCEQSA,
    node,
    queue,
    net,
    dependentParameter,
    5);

  net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_RIG, NETWORK_NODE_ID, net->getActiveAnimSetIndex());

  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Sample event buffers not including the duration events buffers.
Task* nodeAnimSyncEventsQueueSampledEventsBufferPreCompSyncTrack(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // Build list of all parameters (attributes) that are needed for the task to operate.
  // Also build a list of attribute addresses that we are dependent on having been updated before us.
  Task* task;
  NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
  if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, node->getNodeID()) != INVALID_NODE_ID)
  {
    // Don't sample duration events.
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_SAMPLEEVENTSFROMSOURCETRACKS,
             node->getNodeID(),
             5,
             dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

      net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);
      net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS, node->getNodeID(), activeAnimSetIndex);
      net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
      net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_LOOP, node->getNodeID());
    }
  }
  else
  {
    // Sample duration events.
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_SAMPLEEVENTSFROMSOURCETRACKSINCDURATIONEVENTS,
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
      AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

      net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);
      net->TaskAddNetInputParam(task, 1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, activeAnimSetIndex);
      net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS, node->getNodeID(), activeAnimSetIndex);
      net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
      net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_LOOP, node->getNodeID());
      net->TaskAddDefInputParam(task, 6, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, node->getNodeID(), activeAnimSetIndex);
    }
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
// Sample event buffers not including the duration events buffers.
Task* nodeAnimSyncEventsQueueSampledEventsBufferDynamicSyncTrack(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter)
{
  // Build list of all parameters (attributes) that are needed for the task to operate.
  // Also build a list of attribute addresses that we are dependent on having been updated before us.
  Task* task;
  NodeID activeParentNodeID = net->getActiveParentNodeID(node->getNodeID());
  if (net->nodeFindGeneratingNodeForSemantic(activeParentNodeID, ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, false, node->getNodeID()) != INVALID_NODE_ID)
  {
    // Don't sample duration events.
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_SAMPLEEVENTSFROMSOURCETRACKS,
             node->getNodeID(),
             5,
             dependentParameter);
#ifdef NODE_TASKS_QUEUE_BY_SEMANTICS
    NMP_ASSERT(task)
#else
    if (task)
#endif
    {
      FrameCount currFrameNo = net->getCurrentFrameNo();
      AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

      net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);
      net->TaskAddDefInputParam(task, 1, ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS, node->getNodeID(), activeAnimSetIndex);
      net->TaskAddParamAndDependency(task, 2, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddDefInputParam(task, 3, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
      net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_LOOP, node->getNodeID());
    }
  }
  else
  {
    // Sample duration events.
    task = queue->createNewTaskOnQueue(
             CoreTaskIDs::MR_TASKID_SAMPLEEVENTSFROMSOURCETRACKSINCDURATIONEVENTS,
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
      AnimSetIndex activeAnimSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

      net->TaskAddOutputParamZeroLifespan(task, 0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER, INVALID_NODE_ID, currFrameNo);
      net->TaskAddNetInputParam(task, 1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, activeAnimSetIndex);
      net->TaskAddDefInputParam(task, 2, ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS, node->getNodeID(), activeAnimSetIndex);
      net->TaskAddParamAndDependency(task, 3, ATTRIB_SEMANTIC_FRACTION_POS, ATTRIB_TYPE_PLAYBACK_POS, node->getNodeID(), INVALID_NODE_ID, TPARAM_FLAG_INPUT, currFrameNo);
      net->TaskAddDefInputParam(task, 4, ATTRIB_SEMANTIC_SOURCE_ANIM, node->getNodeID(), activeAnimSetIndex);
      net->TaskAddDefInputParam(task, 5, ATTRIB_SEMANTIC_LOOP, node->getNodeID());
      net->TaskAddNetInputParam(task, 6, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER, activeAnimSetIndex);
    }
  }
  return task;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeAnimSyncEventsFindGeneratingNodeForSemantic(
  NodeID              NMP_UNUSED(callingNodeID),
  bool                NMP_UNUSED(fromParent),   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            NMP_UNUSED(network))
{
  NodeID result = INVALID_NODE_ID;

  switch (semantic)
  {
  case ATTRIB_SEMANTIC_FRACTION_POS:
  case ATTRIB_SEMANTIC_TRANSFORM_BUFFER:
  case ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER:
  case ATTRIB_SEMANTIC_TIME_POS:
  case ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS:
  case ATTRIB_SEMANTIC_ANIM_SAMPLE_POS:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM:
  case ATTRIB_SEMANTIC_LOOPED_ON_UPDATE:
  case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER:
    result = node->getNodeID();
    break;
  default:
    break;
  }

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
