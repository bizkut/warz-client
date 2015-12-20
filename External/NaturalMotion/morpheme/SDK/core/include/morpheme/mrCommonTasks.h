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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_TASKS
#define MR_TASKS
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrAttribData.h"
#include "morpheme/mrDispatcher.h"
#include "morpheme/AnimSource/mrAnimSource.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
struct SampledDiscreteTrack
{
  uint32_t m_eventsArrayStartIndex;
  uint32_t m_numTriggeredEvents;
  uint32_t m_runtimeID;
  uint32_t m_userData;
};

// Task functions.

void TaskAnimSyncEventsUpdateTimePos(Dispatcher::TaskParameters* parameters);
void TaskAnimSyncEventsUpdateSyncEventPos(Dispatcher::TaskParameters* parameters);

void TaskSampleTransformsFromAnimSourceASA(Dispatcher::TaskParameters* parameters);
void TaskSampleTransformsFromAnimSourceMBA(Dispatcher::TaskParameters* parameters);
void TaskSampleTransformsFromAnimSourceNSA(Dispatcher::TaskParameters* parameters);
void TaskSampleTransformsFromAnimSourceQSA(Dispatcher::TaskParameters* parameters);

void TaskUpdateDeltaTrajectoryFromTrajectorySourceASA(Dispatcher::TaskParameters* parameters);
void TaskUpdateDeltaTrajectoryFromTrajectorySourceMBA(Dispatcher::TaskParameters* parameters);
void TaskUpdateDeltaTrajectoryFromTrajectorySourceNSA(Dispatcher::TaskParameters* parameters);
void TaskUpdateDeltaTrajectoryFromTrajectorySourceQSA(Dispatcher::TaskParameters* parameters);

void TaskUpdateDeltaTrajectoryAndTransformsFromSourceASA(Dispatcher::TaskParameters* parameters);
void TaskUpdateDeltaTrajectoryAndTransformsFromSourceMBA(Dispatcher::TaskParameters* parameters);
void TaskUpdateDeltaTrajectoryAndTransformsFromSourceNSA(Dispatcher::TaskParameters* parameters);
void TaskUpdateDeltaTrajectoryAndTransformsFromSourceQSA(Dispatcher::TaskParameters* parameters);

void TaskInitSyncEventTrackFromDiscreteEventTrack(Dispatcher::TaskParameters* parameters);
void TaskInitUnitLengthSyncEventTrack(Dispatcher::TaskParameters* parameters);
void TaskInitEventTrackDurationSetFromSource(Dispatcher::TaskParameters* parameters);
void TaskInitEmptyEventTrackDurationSet(Dispatcher::TaskParameters* parameters);
void TaskInitSampledEventsBuffer(Dispatcher::TaskParameters* parameters);
void TaskSampleEventsFromSourceTracks(Dispatcher::TaskParameters* parameters);
void TaskSampleEventsFromSourceTracksIncDurationEvents(Dispatcher::TaskParameters* parameters);
void TaskBufferLastFramesTransformBuffer(Dispatcher::TaskParameters* parameters);
void TaskBufferLastFramesTrajectoryDeltaAndTransformBuffer(Dispatcher::TaskParameters* parameters);
void TaskBufferLastFramesTransformsZeroTrajectory(Dispatcher::TaskParameters* parameters);

void TaskApplyGlobalTimeUpdateTimePos(Dispatcher::TaskParameters* parameters);

void TaskBlend2TransformBuffsAddAttAddPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2TransformBuffsAddAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2TransformBuffsInterpAttAddPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2TransformBuffsInterpAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2TransformBuffsSubtractAttSubtractPos(Dispatcher::TaskParameters* parameters);

void TaskBlend2TrajectoryAndTransformsAddAttAddPosSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryAndTransformsAddAttInterpPosSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryAndTransformsInterpAttAddPosSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryAndTransformsInterpAttInterpPosSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryAndTransformsSubtractAttSubtractPosSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryAndTransformsSubtractAttInterpPosSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryAndTransformsInterpAttSubtractPosSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryAndTransformsAddAttAddPosInterpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryAndTransformsAddAttInterpPosInterpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryAndTransformsInterpAttAddPosInterpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryAndTransformsInterpAttInterpPosInterpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryAndTransformsSubtractAttSubtractPosInterpTraj(Dispatcher::TaskParameters* parameters);

void TaskBlend2TransformsAddAttAddPosPassDestTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TransformsAddAttInterpPosPassDestTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TransformsInterpAttAddPosPassDestTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TransformsInterpAttInterpPosPassDestTraj(Dispatcher::TaskParameters* parameters);

void TaskBlend2TransformsAddAttAddPosPassSourceTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TransformsAddAttInterpPosPassSourceTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TransformsInterpAttAddPosPassSourceTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2TransformsInterpAttInterpPosPassSourceTraj(Dispatcher::TaskParameters* parameters);

void TaskBlend2x2TransformBuffsInterpAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2x2TrajectoryDeltaTransformsInterpAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2x2TrajectoryDeltaTransformsInterpAttSlerpPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2x2TrajectoryDeltaAndTransformsInterpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlend2x2TrajectoryDeltaAndTransformsSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskCombine2x2SampledEventsBuffers(Dispatcher::TaskParameters* parameters);
void TaskCombine2x2SampledEventsBuffersAndSampleDurationEvents(Dispatcher::TaskParameters* parameters);
void TaskBlend2x2DurationEventTrackSets(Dispatcher::TaskParameters* parameters);
void TaskBlend2x2SyncEventTracks(Dispatcher::TaskParameters* parameters);

void TaskTriangleBlendTransformBuffsInterpAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskTriangleBlendTrajectoryDeltaTransformsInterpAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskTriangleBlendTrajectoryDeltaTransformsInterpAttSlerpPos(Dispatcher::TaskParameters* parameters);
void TaskTriangleBlendTrajectoryDeltaAndTransformsInterpTraj(Dispatcher::TaskParameters* parameters);
void TaskTriangleBlendTrajectoryDeltaAndTransformsSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskTriangleCombineSampledEventsBuffers(Dispatcher::TaskParameters* parameters);
void TaskTriangleCombineSampledEventsBuffersAndSampleDurationEvents(Dispatcher::TaskParameters* parameters);
void TaskTriangleBlendDurationEventTrackSets(Dispatcher::TaskParameters* parameters);
void TaskTriangleBlendSyncEventTracks(Dispatcher::TaskParameters* parameters);

void TaskBlendAllTransformBuffsInterpAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskCombineAllSampledEventsBuffers(Dispatcher::TaskParameters* parameters);
void TaskCombineAllSampledEventsBuffersAndSampleDurationEvents(Dispatcher::TaskParameters* parameters);
void TaskBlendAllTrajectoryDeltaTransformsInterpAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskBlendAllTrajectoryDeltaTransformsInterpAttSlerpPos(Dispatcher::TaskParameters* parameters);
void TaskBlendAllTrajectoryDeltaAndTransformsInterpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlendAllTrajectoryDeltaAndTransformsSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskBlendAllDurationEventTrackSets(Dispatcher::TaskParameters* parameters);
void TaskBlendAllSyncEventTracks(Dispatcher::TaskParameters* parameters);

void TaskFeatherBlend2TransformBuffsAddAttAddPos(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TransformBuffsAddAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TransformBuffsInterpAttAddPos(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TransformBuffsInterpAttInterpPos(Dispatcher::TaskParameters* parameters);

void TaskRetargetTransforms(Dispatcher::TaskParameters *parameters);
void TaskRetargetTrajectoryDeltaTransform(Dispatcher::TaskParameters *parameters);
void TaskRetargetTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters *parameters);
void TaskScaleCharacterTransforms(Dispatcher::TaskParameters* parameters);
void TaskScaleCharacterDeltaTransform(Dispatcher::TaskParameters* parameters);
void TaskScaleCharacterDeltaAndTransforms(Dispatcher::TaskParameters* parameters);

void TaskHeadLookTransforms(Dispatcher::TaskParameters* parameters);
void TaskHeadLookSetup(Dispatcher::TaskParameters* parameters);
void TaskHeadLookTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters);
void TaskGunAimTransforms(Dispatcher::TaskParameters* parameters);
void TaskGunAimTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters);
void TaskGunAimSetup(Dispatcher::TaskParameters* parameters);
void TaskTwoBoneIKTransforms(Dispatcher::TaskParameters* parameters);
void TaskTwoBoneIKTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters);
void TaskTwoBoneIKSetup(Dispatcher::TaskParameters* parameters);
void TaskLockFootTransforms(Dispatcher::TaskParameters* parameters);
void TaskLockFootTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters);
void TaskHipsIKTransforms(Dispatcher::TaskParameters* parameters);
void TaskHipsIKTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters);

void TaskModifyJointTransformTransforms(Dispatcher::TaskParameters* parameters);
void TaskModifyJointTransformTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters);

void TaskModifyTrajectoryTransformTrajectoryDelta(Dispatcher::TaskParameters* parameters);
void TaskModifyTrajectoryTransformTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters);

void TaskBasicUnevenTerrainIKSetup(Dispatcher::TaskParameters* parameters);
void TaskBasicUnevenTerrainFootLiftingTarget(Dispatcher::TaskParameters* parameters);
void TaskBasicUnevenTerrainTransforms(Dispatcher::TaskParameters* parameters);

void TaskPredictiveUnevenTerrainIKSetup(Dispatcher::TaskParameters* parameters);
void TaskPredictiveUnevenTerrainIKTrajectoryDeltaAndTransformsSetup(Dispatcher::TaskParameters* parameters);
void TaskPredictiveUnevenTerrainFootLiftingTarget(Dispatcher::TaskParameters* parameters);

void TaskCombine2SampledEventsBuffers(Dispatcher::TaskParameters* parameters);
void TaskAdd2SampledEventsBuffers(Dispatcher::TaskParameters* parameters);
void TaskCombine2SampledEventsBuffersAndSampleDurationEvents(Dispatcher::TaskParameters* parameters);
void TaskAdd2SampledEventsBuffersAndSampleDurationEvents(Dispatcher::TaskParameters* parameters);
void TaskAddSampledDurationEventsToSampleEventBuffer(Dispatcher::TaskParameters* parameters);
void TaskBlend2DurationEventTrackSets(Dispatcher::TaskParameters* parameters);

void TaskBlend2TrajectoryDeltaTransformsAddAttAddPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryDeltaTransformsAddAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryDeltaTransformsAddAttSlerpPos(Dispatcher::TaskParameters* parameters);

void TaskBlend2TrajectoryDeltaTransformsInterpAttAddPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryDeltaTransformsInterpAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryDeltaTransformsInterpAttSlerpPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryDeltaTransformsInterpAttSubtractPos(Dispatcher::TaskParameters* parameters);

void TaskBlend2TrajectoryDeltaTransformsSubtractAttSubtractPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryDeltaTransformsSubtractAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskBlend2TrajectoryDeltaTransformsSubtractAttSlerpPos(Dispatcher::TaskParameters* parameters);

void TaskFeatherBlend2TrajectoryDeltaTransformsAddAttAddPos(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TrajectoryDeltaTransformsAddAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TrajectoryDeltaTransformsAddAttSlerpPos(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TrajectoryDeltaTransformsInterpAttAddPos(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TrajectoryDeltaTransformsInterpAttInterpPos(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TrajectoryDeltaTransformsInterpAttSlerpPos(Dispatcher::TaskParameters* parameters);

void TaskFeatherBlend2TrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TrajectoryDeltaAndTransformsAddAttAddPosInterpTraj(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TrajectoryDeltaAndTransformsAddAttInterpPosInterpTraj(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TrajectoryDeltaAndTransformsInterpAttAddPosInterpTraj(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlend2TrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj(Dispatcher::TaskParameters* parameters);

void TaskBlend2SyncEventsUpdateTimeViaTimePos(Dispatcher::TaskParameters* parameters);
void TaskBlend2SyncEventsUpdateTimeViaSyncEventPos(Dispatcher::TaskParameters* parameters);

void TaskBlend2SyncEventTracks(Dispatcher::TaskParameters* parameters);

void TaskFilterTransforms(Dispatcher::TaskParameters* parameters);
void TaskFilterTrajectoryDelta(Dispatcher::TaskParameters* parameters);
void TaskFilterTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters);
void TaskApplyBindPoseTransforms(Dispatcher::TaskParameters* parameters);
void TaskApplyBindPoseTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters);
void TaskGetBindPoseTransforms(Dispatcher::TaskParameters* parameters);
void TaskMirrorTransforms(Dispatcher::TaskParameters* parameters);
void TaskMirrorTrajectoryDelta(Dispatcher::TaskParameters* parameters);
void TaskMirrorTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters);
void TaskMirrorSampledEvents(Dispatcher::TaskParameters* parameters);
void TaskMirrorSyncEventsAndOffset(Dispatcher::TaskParameters* parameters);
void TaskMirrorDurationEvents(Dispatcher::TaskParameters* parameters);
void TaskMirrorSampledAndSampleDurationEvents(Dispatcher::TaskParameters* parameters);
void TaskMirrorUpdateTimeViaSyncEventPos(Dispatcher::TaskParameters* parameters);
void TaskMirrorUpdateTimeViaTimePos(Dispatcher::TaskParameters* parameters);

void TaskApplyPhysicsJointLimitsTransforms(Dispatcher::TaskParameters* parameters);
void TaskClosestAnimTransforms(Dispatcher::TaskParameters* parameters);
void TaskClosestAnimTrajectoryDelta(Dispatcher::TaskParameters* parameters);
void TaskClosestAnimTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters);

void TaskPhysicsUpdateAnimatedTrajectory(Dispatcher::TaskParameters* parameters);
void TaskPhysicsUpdatePhysicalTrajectoryPostPhysics(Dispatcher::TaskParameters* parameters);
void TaskPhysicsUpdateTransformsPrePhysics(Dispatcher::TaskParameters* parameters);
void TaskPhysicsUpdateTransformsPostPhysics(Dispatcher::TaskParameters* parameters);
void TaskPhysicsSampledEventsBuffer(Dispatcher::TaskParameters* parameters);
void TaskSetNonPhysicsTransforms(Dispatcher::TaskParameters* parameters);

void TaskExpandLimitsTransforms(Dispatcher::TaskParameters* parameters);
void TaskExpandLimitsDeltaAndTransforms(Dispatcher::TaskParameters* parameters);

void TaskPhysicsGrouperUpdateTransforms(Dispatcher::TaskParameters* parameters);
void TaskPhysicsGrouperUpdateTransformsPrePhysics(Dispatcher::TaskParameters* parameters);
void TaskPhysicsGrouperUpdateTrajectory(Dispatcher::TaskParameters* parameters);
void TaskPhysicsGrouperGetOutputMaskBase(Dispatcher::TaskParameters* parameters);
void TaskPhysicsGrouperGetOutputMaskOverride(Dispatcher::TaskParameters* parameters);

void TaskScaleUpdateTime(Dispatcher::TaskParameters* parameters);
void TaskScaleUpdateSyncEventTrack(Dispatcher::TaskParameters* parameters);
void TaskSetUpdateTimeViaControlParam(Dispatcher::TaskParameters* parameters);

void TaskScaleToDuration(Dispatcher::TaskParameters* parameters);
void TaskScaleToDurationSyncEventTrack(Dispatcher::TaskParameters* parameters);

void TaskNetworkUpdateCharacterController(Dispatcher::TaskParameters* parameters);
void TaskNetworkUpdatePhysics(Dispatcher::TaskParameters* parameters);
void TaskNetworkUpdateRoot(Dispatcher::TaskParameters* parameters);
void TaskNetworkMergePhysicsRigTransformBuffers(Dispatcher::TaskParameters* parameters);

void TaskNetworkDummyTask(Dispatcher::TaskParameters* parameters);
void TaskEmptyTask(Dispatcher::TaskParameters* parameters);

void TaskTransitUpdateTimePos(Dispatcher::TaskParameters* parameters);
void TaskTransitDeadBlendUpdateTimePos(Dispatcher::TaskParameters* parameters);

void TaskTransitSyncEventsUpdateTimeViaSyncEventPos(Dispatcher::TaskParameters* parameters);
void TaskTransitSyncEventsUpdateTimeViaTimePos(Dispatcher::TaskParameters* parameters);
void TaskTransitSyncEventsBlendSyncEventTracks(Dispatcher::TaskParameters* parameters);
void TaskTransitSyncEventsPassThroughSyncEventTrack(Dispatcher::TaskParameters* parameters);

void TaskDeadBlendCacheState(Dispatcher::TaskParameters* parameters);
void TaskAnimDeadBlendTransformBuffs(Dispatcher::TaskParameters* parameters);
void TaskAnimDeadBlendTrajectoryDeltaAndTransformBuffs(Dispatcher::TaskParameters* parameters);
void TaskPhysicalDeadBlendTransformBuffs(Dispatcher::TaskParameters* parameters);
void TaskDeadBlendTrajectory(Dispatcher::TaskParameters* parameters);
void TaskFeatherBlendToPhysicsTransformBuffs(Dispatcher::TaskParameters* parameters);

void TaskOutputSmoothTransformsTransforms(Dispatcher::TaskParameters* parameters);
void TaskOutputSmoothTransformsTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters);

void TaskCreateReferenceToInputAttribTypeBool(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeUInt(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeFloat(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeVector3(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeVector4(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeBoolArray(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeIntArray(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeUIntArray(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeFloatArray(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeUpdatePlaybackPos(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypePlaybackPos(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeUpdateSyncEventPlaybackPos(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeTransformBuffer(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeTrajectoryDeltaTransform(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeTransform(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeSyncEventTrack(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeSampledEvents(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeDurationEventTrackSet(Dispatcher::TaskParameters* parameters);
void TaskCreateReferenceToInputAttribTypeClipFraction(Dispatcher::TaskParameters* parameters);

void TaskPassThroughTransformsExtractJointInfoObjectSpace(Dispatcher::TaskParameters* parameters);
void TaskPassThroughTransformsExtractJointInfoLocalSpace(Dispatcher::TaskParameters* parameters);
void TaskPassThroughTransformsExtractJointInfoObjectSpaceJointSelect(Dispatcher::TaskParameters* parameters);
void TaskPassThroughTransformsExtractJointInfoLocalSpaceJointSelect(Dispatcher::TaskParameters* parameters);

void TaskPassThroughTrajectoryDeltaAndTransformsExtractJointInfoObjectSpace(Dispatcher::TaskParameters* parameters);
void TaskPassThroughTrajectoryDeltaAndTransformsExtractJointInfoLocalSpace(Dispatcher::TaskParameters* parameters);
void TaskPassThroughTrajectoryDeltaAndTransformsExtractJointInfoObjectSpaceJointSelect(Dispatcher::TaskParameters* parameters);
void TaskPassThroughTrajectoryDeltaAndTransformsExtractJointInfoLocalSpaceJointSelect(Dispatcher::TaskParameters* parameters);

void TaskSampledEventsBufferEmitMessageAndPassThrough(Dispatcher::TaskParameters* parameters);

/// \brief
float updateWeightingFromTransitionEventCount(
  float durationInEvents,
  float transitionEventCount);

/// \brief
float calcTransitionEventsDelta(
  SyncEventPos&    posFrom,
  SyncEventPos&    posTo,
  EventTrackSync&  syncEventTrack);

/// \brief
void blend2TrajectoryDeltaTransform(
  NMP::Vector3*       trajectoryDeltaPos,
  NMP::Quat*          trajectoryDeltaAtt,
  bool*               trajectoryFilteredOut,
  const NMP::Vector3& source0TrajectoryDeltaPos,
  const NMP::Quat&    source0TrajectoryDeltaAtt,
  bool                source0TrajectoryFilteredOut,
  const NMP::Vector3& source1TrajectoryDeltaPos,
  const NMP::Quat&    source1TrajectoryDeltaAtt,
  bool                source1TrajectoryFilteredOut,
  uint32_t            blendMode,
  bool                slerpTrajectoryPosition,
  float               blendWeight);

/// \brief Integrates transforms according to transformRates and dt. If rateDamping > 0 then an
///        exponential damping is also applied to transformRates.
void IntegrateTransforms(
  NMP::DataBuffer& transforms,
  NMP::DataBuffer& transformRates,
  float            dt,
  float            rateDamping);

/// \brief Given an event sampling period expressed as a fraction of the animation clip length
/// calculate the sampling period as a fraction of the duration of the whole animation.
void calculateTrueEventSamplingTimeFractions(
  AttribDataPlaybackPos* fractionTimePos,
  AttribDataSourceAnim*  sourceAnim,
  float&                 trueSamplePrevFraction,
  float&                 trueSampleCurrentFraction,
  float&                 trueSampleDeltaFraction);

/// \brief Find the triggered discrete events on an AaimSource node over a specified period.
void findTriggeredDiscreteEvents(
  uint32_t&                             numSampledDiscreteTracks,         // OUT
  SampledDiscreteTrack*                 sampledDiscreteTracks,            // OUT
  uint32_t&                             totalNumTriggeredDiscreteEvents,  // OUT
  EventDefDiscrete**                    triggeredDiscreteEvents,          // OUT
  const AttribDataSourceEventTrackSet*  sourceDiscreteEventTracks,        // IN
  const AttribDataSourceAnim*           sourceAnim,                       // IN
  bool                                  allowLooping,                     // IN
  float                                 sampleStartFraction,              // IN
  float                                 sampleDeltaFraction,              // IN
  bool                                  inclusiveStart                    // IN
);

/// \brief accumulate the transforms in localTransf buffer and write the output to charSpaceTransf, creating a buffer
/// containing each part's transform in character space.
void subTaskCalculateCharSpaceTransforms(
  const MR::AnimRigDef* rig,
  const NMP::DataBuffer* localTransf,
  NMP::DataBuffer* charSpaceTransf
);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TASKS
//----------------------------------------------------------------------------------------------------------------------

