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
#include "morpheme/Nodes/mrNodes.h"

#include "morpheme/mrDefines.h"
#include "morpheme/mrRigToAnimMap.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrCommonTaskQueuingFns.h"
#include "morpheme/mrManager.h"

#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/Nodes/mrNodeAnimSyncEvents.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeSubtractiveBlend.h"
#include "morpheme/Nodes/mrNodeBlendN.h"
#include "morpheme/Nodes/mrNodeScatterBlend1D.h"
#include "morpheme/Nodes/mrNodeScatterBlend2D.h"
#include "morpheme/Nodes/mrNodeBlend2x2.h"
#include "morpheme/Nodes/mrNodeBlendNXM.h"
#include "morpheme/Nodes/mrNodeBlendAll.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeSubtractiveBlendSyncEvents.h"
#include "morpheme/Nodes/mrNodeBlendNSyncEvents.h"
#include "morpheme/Nodes/mrNodeScatterBlend2DSyncEvents.h"
#include "morpheme/Nodes/mrNodeBlend2x2SyncEvents.h"
#include "morpheme/Nodes/mrNodeBlendAllSyncEvents.h"
#include "morpheme/Nodes/mrNodeControlParamFloat.h"
#include "morpheme/Nodes/mrNodeControlParamVector3.h"
#include "morpheme/Nodes/mrNodeControlParamVector4.h"
#include "morpheme/Nodes/mrNodeControlParamBool.h"
#include "morpheme/Nodes/mrNodeControlParamInt.h"
#include "morpheme/Nodes/mrNodeControlParamUInt.h"
#include "morpheme/Nodes/mrNodeFreeze.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodeApplyBindPose.h"
#include "morpheme/Nodes/mrNodeApplyGlobalTime.h"
#include "morpheme/Nodes/mrNodeFeatherBlend2.h"
#include "morpheme/Nodes/mrNodeFeatherBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeHeadLook.h"
#include "morpheme/Nodes/mrNodeTwoBoneIK.h"
#include "morpheme/Nodes/mrNodeLockFoot.h"
#include "morpheme/Nodes/mrNodeGunAimIK.h"
#include "morpheme/Nodes/mrNodeHipsIK.h"
#include "morpheme/Nodes/mrNodeSmoothTransforms.h"
#include "morpheme/Nodes/mrNodeBasicUnevenTerrain.h"
#include "morpheme/Nodes/mrNodePredictiveUnevenTerrain.h"
#include "morpheme/Nodes/mrNodeExtractJointInfo.h"
#include "morpheme/Nodes/mrNodeEmitMessageOnDiscreteEvent.h"
#include "morpheme/Nodes/mrNodeSingleFrame.h"
#include "morpheme/Nodes/mrNodeClosestAnim.h"
#include "morpheme/Nodes/mrNodeSwitch.h"
#include "morpheme/Nodes/mrNodeSequence.h"
#include "morpheme/Nodes/mrNodePlaySpeedModifier.h"
#include "morpheme/Nodes/mrNodeScaleToDuration.h"
#include "morpheme/Nodes/mrNodeMirrorTransforms.h"
#include "morpheme/Nodes/mrNodeOperatorFunction.h"
#include "morpheme/Nodes/mrNodeOperatorBoolean.h"
#include "morpheme/Nodes/mrNodeOperatorCPCompare.h"
#include "morpheme/Nodes/mrNodeOperatorArithmetic.h"
#include "morpheme/Nodes/mrNodeOperatorNoiseGen.h"
#include "morpheme/Nodes/mrNodeOperatorRandomFloat.h"
#include "morpheme/Nodes/mrNodeOperatorFloatsToVector3.h"
#include "morpheme/Nodes/mrNodeOperatorFloatToInt.h"
#include "morpheme/Nodes/mrNodeOperatorIntToFloat.h"
#include "morpheme/Nodes/mrNodeOperatorVector3ToFloats.h"
#include "morpheme/Nodes/mrNodeOperatorVector3Dot.h"
#include "morpheme/Nodes/mrNodeOperatorVector3Normalise.h"
#include "morpheme/Nodes/mrNodeOperatorVector3Distance.h"
#include "morpheme/Nodes/mrNodeOperatorVector3CrossProduct.h"
#include "morpheme/Nodes/mrNodeOperatorVector3Angle.h"
#include "morpheme/Nodes/mrNodeTransit.h"
#include "morpheme/Nodes/mrNodeTransitSyncEvents.h"
#include "morpheme/Nodes/mrNodeOperatorOneInputArithmetic.h"
#include "morpheme/Nodes/mrNodeOperatorSmoothFloat.h"
#include "morpheme/Nodes/mrNodeOperatorRateOfChange.h"
#include "morpheme/Nodes/mrNodeOperatorRampFloat.h"
#include "morpheme/Nodes/mrNodeOperatorRandomFloat.h"
#include "morpheme/Nodes/mrNodeOperatorRayCast.h"
#include "morpheme/Nodes/mrNodeOperatorTimeLagInput.h"
#include "morpheme/Nodes/mrNodeSequence.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeModifyJointTransform.h"
#include "morpheme/Nodes/mrNodeModifyTrajectoryTransform.h"
#include "morpheme/mrMirroredAnimMapping.h"
#include "morpheme/mrRigRetargetMapping.h"
#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/mrCharacterControllerDef.h"
#include "morpheme/mrTransitDeadBlend.h"
#include "morpheme/Nodes/mrNodeRetarget.h"
#include "morpheme/Nodes/mrNodeScaleCharacter.h"
#include "morpheme/Nodes/mrNodeCharacterControllerOverride.h"


#include "morpheme/TransitConditions/mrTransitConditionControlParamFloatGreater.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamFloatInRange.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamFloatLess.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamIntGreater.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamIntInRange.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamIntLess.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamUIntGreater.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamUIntInRange.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamUIntLess.h"
#include "morpheme/TransitConditions/mrTransitConditionCrossedDurationFraction.h"
#include "morpheme/TransitConditions/mrTransitConditionDiscreteEventTriggered.h"
#include "morpheme/TransitConditions/mrTransitConditionFalse.h"
#include "morpheme/TransitConditions/mrTransitConditionCrossedCurveEventValueDecreasing.h"
#include "morpheme/TransitConditions/mrTransitConditionInDurationEvent.h"
#include "morpheme/TransitConditions/mrTransitConditionInSyncEventRange.h"
#include "morpheme/TransitConditions/mrTransitConditionOnMessage.h"
#include "morpheme/TransitConditions/mrTransitConditionNodeActive.h"
#include "morpheme/TransitConditions/mrTransitConditionRayHit.h"

#include "morpheme/Prediction/mrPredictionModelNDMesh.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// Register the default node types.
void registerCoreQueuingFnsAndOutputCPTasks()
{
  Manager& manager = Manager::getInstance();
  //-------------
  // Register task queuing functions.
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(queueInitUnitLengthSyncEventTrack));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(queueInitEmptyEventTrackDurationSet));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(queueInitSampledEventsTrack));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeAnimSyncEventsQueueTimePosPreCompSyncTrack));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeAnimSyncEventsQueueTimePosDynamicSyncTrack));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeAnimSyncEventsQueueSampledEventsBufferPreCompSyncTrack));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeAnimSyncEventsQueueSampledEventsBufferDynamicSyncTrack));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeAnimSyncEventsQueueSampleTransformsFromAnimSource));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeAnimSyncEventsQueueTrajectoryDeltaTransform));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeAnimSyncEventsQueueTrajectoryDeltaAndTransformsFromSource));
  
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFreezePassThroughLastTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFreezePassThroughLastTrajectoryDeltaAndTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFreezePassThroughLastTransformsOnce));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFreezePassThroughLastTrajectoryDeltaAndTransformsOnce));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueBlend2TransformBuffsAddAttAddPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueBlend2TransformBuffsAddAttInterpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueBlend2TransformBuffsInterpAttAddPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueBlend2TransformBuffsInterpAttInterpPos));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaTransformAddAttAddPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaTransformAddAttInterpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaTransformAddAttSlerpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaTransformInterpAttAddPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaTransformInterpAttInterpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaTransformInterpAttSlerpPos));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj));
  
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosInterpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosInterpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosInterpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2SyncEventsQueueSyncEventTrack));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2SyncEventsQueueBlend2DurationEventTrackSets));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2SyncEventsQueueTimePos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueSampledEventsBuffers));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2QueueAddSampledEventsBuffers));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2SyncEventsQueueSampledEventsBuffers));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2SyncEventsQueueAddSampledEventsBuffers));
  
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeSubtractiveBlendQueueTransformBuffsSubtractAttSubtractPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeSubtractiveBlendQueueTrajectoryDeltaTransformSubtractAttSubtractPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeSubtractiveBlendQueueTrajectoryDeltaTransformSubtractAttSlerpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeSubtractiveBlendQueueTrajectoryDeltaAndTransformsSubtractAttSubtractPosSlerpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeSubtractiveBlendQueueTrajectoryDeltaAndTransformsSubtractAttSubtractPosInterpTraj));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScatterBlend2DQueueBlendTransformBuffsInterpAttInterpPos));  
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScatterBlend2DQueueTrajectoryDeltaTransformInterpAttInterpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScatterBlend2DQueueTrajectoryDeltaTransformInterpAttSlerpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScatterBlend2DQueueTrajectoryDeltaAndTransformsInterpPosInterpAttInterpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScatterBlend2DQueueTrajectoryDeltaAndTransformsInterpPosInterpAttSlerpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScatterBlend2DQueueSampledEventsBuffers));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScatterBlend2DSyncEventsQueueSyncEventTrack));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScatterBlend2DSyncEventsQueueBlend3DurationEventTrackSets));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScatterBlend2DSyncEventsQueueSampledEventsBuffers));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2x2QueueBlendTransformBuffsInterpAttInterpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2x2QueueTrajectoryDeltaTransformInterpAttInterpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2x2QueueTrajectoryDeltaTransformInterpAttSlerpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2x2QueueTrajectoryDeltaAndTransformsInterpPosInterpAttInterpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2x2QueueTrajectoryDeltaAndTransformsInterpPosInterpAttSlerpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2x2QueueSampledEventsBuffers));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2x2SyncEventsQueueSyncEventTrack));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2x2SyncEventsQueueBlend2x2DurationEventTrackSets));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlend2x2SyncEventsQueueSampledEventsBuffers));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlendAllQueueBlendTransformBuffsInterpAttInterpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlendAllQueueTrajectoryDeltaTransformInterpAttInterpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlendAllQueueTrajectoryDeltaTransformInterpAttSlerpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlendAllQueueTrajectoryDeltaAndTransformsInterpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlendAllQueueTrajectoryDeltaAndTransformsSlerpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlendAllQueueSampledEventsBuffers));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlendAllSyncEventsQueueSyncEventTrack));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlendAllSyncEventsQueueBlend2DurationEventTrackSets));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBlendAllSyncEventsQueueSampledEventsBuffers));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(queuePassThroughChild0));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(grouperQueuePassThroughFromParent));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(queuePassThroughChild1));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFilterTransformsQueueFilterTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFilterTransformsQueueFilterTrajectoryDeltaTransform));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFilterTransformsQueueFilterTrajectoryDeltaAndTransforms));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeApplyBindPoseQueueApplyBindPoseTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeApplyBindPoseQueueApplyBindPoseTrajectoryDeltaAndTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeApplyGlobalTimeQueueUpdateTimePos));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueFeatherBlend2TransformBuffsAddAttAddPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueFeatherBlend2TransformBuffsAddAttInterpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueFeatherBlend2TransformBuffsInterpAttAddPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueFeatherBlend2TransformBuffsInterpAttInterpPos));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttAddPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttInterpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaTransformAddAttSlerpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttAddPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttInterpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaTransformInterpAttSlerpPos));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosSlerpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosSlerpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosSlerpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosSlerpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttAddPosInterpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsAddAttInterpPosInterpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttAddPosInterpTraj));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeFeatherBlend2QueueTrajectoryDeltaAndTransformsInterpAttInterpPosInterpTraj));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeHeadLookQueueHeadLookTransformBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeHeadLookQueueHeadLookTrajectoryDeltaAndTransformBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeHeadLookQueueHeadLookSetup));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeGunAimIKQueueGunAimIKTransformBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeGunAimIKQueueGunAimIKTrajectoryDeltaAndTransformBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeGunAimIKQueueGunAimIKSetup));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTwoBoneIKQueueTwoBoneIKTransformBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTwoBoneIKQueueTwoBoneIKTrajectoryDeltaAndTransformBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTwoBoneIKQueueTwoBoneIKSetup));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeLockFootQueueLockFootTransformBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeLockFootQueueLockFootTrajectoryDeltaAndTransformsBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeHipsIKQueueUpdateTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeHipsIKQueueUpdateTrajectoryDeltaAndTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeRetargetQueueUpdateTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeRetargetQueueTrajectoryDeltaTransform));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeRetargetQueueTrajectoryDeltaAndTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScaleCharacterQueueUpdateTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScaleCharacterQueueTrajectoryDeltaTransform));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScaleCharacterQueueTrajectoryDeltaAndTransforms));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBasicUnevenTerrainQueueIKSetupTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBasicUnevenTerrainQueueFootLiftingTargets));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBasicUnevenTerrainQueueTransformBuffs));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePredictiveUnevenTerrainQueueIKSetup));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePredictiveUnevenTerrainQueueFootLiftingTargets));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePredictiveUnevenTerrainQueueTransformBuffs));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeClosestAnimQueueTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeClosestAnimQueueTrajectoryDeltaTransform));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeClosestAnimQueueTrajectoryDeltaAndTransforms));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeSingleFrameQueueUpdateTimePos));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePlaySpeedModifierQueueUpdateTime));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePlaySpeedModifierQueueEventTrack));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScaleToDurationQueueUpdateTime));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeScaleToDurationQueueSyncEventTrack));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeMirrorQueueTransformBuff));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeMirrorQueueTrajectoryDeltaTransform));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeMirrorQueueTrajectoryDeltaAndTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeMirrorQueueSampledEventsBuffer));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeMirrorQueueSyncEventsBuffer));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeMirrorQueueDurationEventsBuffer));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeMirrorQueueTimePos));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(stateMachineNodeQueuePassThrough));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeNetworkQueueUpdateCharacterController));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeNetworkQueueUpdatePhysics));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeNetworkQueueUpdateRoot));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeNetworkQueueMergePhysicsRigTransformBuffer));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitQueueUpdateTimePos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitBlend2QueueSampledEventsBuffers));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitQueueTransformBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitQueueTrajectoryDeltaAndTransformBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitQueueTransformBuffsPassThroughDestTrajDelta));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitQueueTransformBuffsPassThroughSourceTrajDelta));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitQueueDeadBlendCacheState));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(queuePassThroughChild0IfNotPhysics));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(queuePassThroughChild1IfNotPhysics));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(transitionQueueTrajectoryDeltaAddAttSlerpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(transitionQueueTrajectoryDeltaInterpAttSlerpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(transitionQueueTrajectoryDeltaAddAttAddPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(transitionQueueTrajectoryDeltaAddAttInterpPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(transitionQueueTrajectoryDeltaInterpAttAddPos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(transitionQueueTrajectoryDeltaInterpAttInterpPos));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitSyncEventsQueueUpdateTimePos));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitSyncEventsQueueSyncEventTrack));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitSyncEventsQueueSampledEventsBuffers));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitSyncEventsQueueBlend2DurationEventTrackSets));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitSyncEventsQueueTransformBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitSyncEventsQueueTrajectoryDeltaAndTransformBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitSyncEventsQueueTransformBuffsPassThroughDestTrajDelta));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTransitSyncEventsQueueTransformBuffsPassThroughSourceTrajDelta));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeSmoothTransformsQueueTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeSmoothTransformsQueueTrajectoryDeltaAndTransforms));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeExtractJointInfoObjectQueueTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeExtractJointInfoObjectQueueTrajectoryDeltaAndTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeExtractJointInfoLocalQueueTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeExtractJointInfoLocalQueueTrajectoryDeltaAndTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeExtractJointInfoObjectJointSelectQueueTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeExtractJointInfoObjectJointSelectQueueTrajectoryDeltaAndTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeExtractJointInfoLocalJointSelectQueueTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeExtractJointInfoLocalJointSelectQueueTrajectoryDeltaAndTransforms));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeEmitMessageOnDiscreteEventQueueSampledEventsBuffers));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeModifyJointTransformQueueTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeModifyJointTransformQueueTrajectoryDeltaAndTransforms));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeModifyTrajectoryTransformQueueTrajectoryDelta));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeModifyTrajectoryTransformQueueTrajectoryDeltaAndTransforms));

  //-------------
  // Register outputCP update tasks.
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeControlParamFloatEmittedCPUpdateFloat));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeControlParamVector3EmittedCPUpdateVector3));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeControlParamVector4EmittedCPUpdateVector4));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeControlParamBoolEmittedCPUpdateBool));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeControlParamIntEmittedCPUpdateInt));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeControlParamUIntEmittedCPUpdateUInt));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorFunctionOutputCPUpdateFloat));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorRandomFloatOutputCPUpdateFloat));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorArithmeticOutputCPUpdateFloat));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorArithmeticOutputCPUpdateVector));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorOneInputArithmeticOutputCPUpdateFloat));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorOneInputArithmeticOutputCPUpdateVector));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorNoiseGenOutputCPUpdateFloat));

  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorSmoothFloatCriticallyDampFloat));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorSmoothFloatCriticallyDampVector));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorRateOfChangeFloat));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorRateOfChangeVector));

  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorRampFloatOutputCPUpdateFloat));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorFloatsToVector3OutputCPUpdateVector3));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorFloatToIntRoundToNearestOutputCPUpdateInt));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorFloatToIntFloorOutputCPUpdateInt));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorFloatToIntCeilingOutputCPUpdateInt));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorIntToFloatOutputCPUpdateFloat));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorRayCastOutputCPUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorVector3ToFloatsOutputCPUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorVector3DotOutputCPUpdate));

  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorVector3NormaliseOutputCPUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorVector3DistanceOutputCPUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorVector3CrossProductOutputCPUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorVector3AngleOutputCPUpdate));

  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorBooleanOutputCPUpdateBool));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorCPCompareFloatOutputCPUpdateBool));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorCPCompareIntOutputCPUpdateBool));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorCPCompareUIntOutputCPUpdateBool));

  manager.registerOutputCPTask(nodeOperatorTimeLagInputOutputCPUpdate<float, AttribDataFloat, ATTRIB_TYPE_TIME_LAG_INPUT_FLOAT>, "nodeOperatorTimeLagInputOutputCPUpdateFloat");
  manager.registerOutputCPTask(nodeOperatorTimeLagInputOutputCPUpdate<int32_t, AttribDataInt, ATTRIB_TYPE_TIME_LAG_INPUT_INT>, "nodeOperatorTimeLagInputOutputCPUpdateInt");
  manager.registerOutputCPTask(nodeOperatorTimeLagInputOutputCPUpdate<NMP::Vector3, AttribDataVector3, ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR3>, "nodeOperatorTimeLagInputOutputCPUpdateVector3");
  manager.registerOutputCPTask(nodeOperatorTimeLagInputOutputCPUpdate<NMP::Quat, AttribDataVector4, ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR4>, "nodeOperatorTimeLagInputOutputCPUpdateVector4");
  manager.registerOutputCPTask(nodeOperatorTimeLagInputOutputCPUpdate<bool, AttribDataBool, ATTRIB_TYPE_TIME_LAG_INPUT_BOOL>, "nodeOperatorTimeLagInputOutputCPUpdateBool");
    
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeNullEmitMessageOutputCPUpdate));

  //-------------
  // Register node delete instance functions.
  manager.registerDeleteNodeInstanceFn(REG_FUNC_ARGS(nodeShareDeleteInstanceNULL));
  manager.registerDeleteNodeInstanceFn(REG_FUNC_ARGS(nodeShareDeleteInstanceNoChildren));
  manager.registerDeleteNodeInstanceFn(REG_FUNC_ARGS(nodeShareDeleteInstanceWithChildren));
  manager.registerDeleteNodeInstanceFn(REG_FUNC_ARGS(nodeNetworkDeleteInstance));
  manager.registerDeleteNodeInstanceFn(REG_FUNC_ARGS(nodeTransitDeleteInstance));
  manager.registerDeleteNodeInstanceFn(REG_FUNC_ARGS(nodeStateMachineDeleteInstance));
  manager.registerDeleteNodeInstanceFn(REG_FUNC_ARGS(nodeClosestAnimDeleteInstance));
  manager.registerDeleteNodeInstanceFn(REG_FUNC_ARGS(nodeSequenceAndSwitchDeleteInstance));
  manager.registerDeleteNodeInstanceFn(REG_FUNC_ARGS(nodeCCOverrideDeleteInstance));

  //-------------
  // Register node update connectivity functions.
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeShareUpdateConnectionsNULL));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeShareUpdateConnectionsChildren));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeShareUpdateConnectionsChildrenInputCPs));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeShareUpdateConnectionsChildrenOptionalInputCPs));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeShareUpdateConnectionsChildren1CompulsoryManyOptionalInputCPs));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeShareUpdateConnectionsChildren2CompulsoryManyOptionalInputCPs));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeShareUpdateConnections1Child1InputCP));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeShareUpdateConnections1Child1OptionalInputCP));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeShareUpdateConnections1Child2InputCPs));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeBlend2SyncEventsAdditiveUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeBlend2SyncEventsUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeBlend2AdditiveUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeBlend2UpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeSubtractiveBlendUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeSubtractiveBlendSyncEventsUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeFeatherBlend2UpdateConnectionsFixBlendWeight));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeFeatherBlend2SyncEventsUpdateConnectionsFixBlendWeight));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeBlendNUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeScatterBlend1DOfflineUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeScatterBlend1DUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeScatterBlend2DOfflineUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeScatterBlend2DUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeBlend2x2UpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeBlend2x2SyncEventsUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeBlendNxMUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeBlendAllUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeTwoBoneIKUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeLockFootUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeStateMachineUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodePredictiveUnevenTerrainUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeClosestAnimUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeTransitUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeTransitSyncEventsUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeSwitchUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeSequenceUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeRetargetUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeScaleCharacterUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeExtractJointInfoJointSelectUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeCCOverrideUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeAnimSyncEventsUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeFreezeUpdateConnections));

  //-------------
  // Register generates attrib semantic type functions.
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(filterNodeFindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeNetworkFindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeStateMachineFindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeFreezeHasQueuingFunction));

  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeClosestAnimFindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeAnimSyncEventsFindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeBlend2FindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeBlendNFindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeBlend2SyncEventsFindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeBlendNSyncEventsFindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeTransitSyncEventsFindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeTransitFindGeneratingNodeForSemantic));

  //-------------
  // Register node initialisation functions. This is initialisation that needs to happen at network instantiation.
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeShareInitInstanceInvalidateAllChildren));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeShareInitInstanceInvalidateAllChildrenInitPinAttrib));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeShareInitInstanceCreateFloatOutputAttribute));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeShareInitInstanceCreateFloatOutputAttributeInitPinAttrib));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeShareInitInstanceCreateIntOutputAttribute));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeShareInitInstanceCreateIntOutputAttributeInitPinAttrib));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeShareInitInstanceCreateVector3OutputAttribute));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeShareInitInstanceCreateVector3OutputAttributeInitPinAttrib));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeShareInitInstanceCreateBoolOutputAttribute));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeShareInitInstanceCreateBoolOutputAttributeInitPinAttrib));

  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeStateMachineInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeInitPinAttribDataInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeControlParamBoolInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeControlParamFloatInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeControlParamIntInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeControlParamUIntInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeControlParamVector3InitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeControlParamVector4InitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorRayCastInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorRampFloatInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorSmoothDampFloatInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorSmoothDampVectorInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorRateOfChangeFloatInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorRateOfChangeVectorInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorVector3ToFloatsInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorRandomFloatInitInstance));
  
  manager.registerInitNodeInstanceFn(nodeOperatorTimeLagInputInitInstance<float, AttribDataFloat, ATTRIB_TYPE_TIME_LAG_INPUT_FLOAT>, "nodeOperatorTimeLagInputInitInstanceFloat");
  manager.registerInitNodeInstanceFn(nodeOperatorTimeLagInputInitInstance<int32_t, AttribDataInt, ATTRIB_TYPE_TIME_LAG_INPUT_INT>, "nodeOperatorTimeLagInputInitInstanceInt");
  manager.registerInitNodeInstanceFn(nodeOperatorTimeLagInputInitInstance<NMP::Vector3, AttribDataVector3, ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR3>, "nodeOperatorTimeLagInputInitInstanceVector3");
  manager.registerInitNodeInstanceFn(nodeOperatorTimeLagInputInitInstance<NMP::Quat, AttribDataVector4, ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR4>, "nodeOperatorTimeLagInputInitInstanceVector4");
  manager.registerInitNodeInstanceFn(nodeOperatorTimeLagInputInitInstance<bool, AttribDataBool, ATTRIB_TYPE_TIME_LAG_INPUT_BOOL>, "nodeOperatorTimeLagInputInitInstanceBool");
  
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeClosestAnimInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeCCOverrideInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeFreezeInitInstance));

  //-------------
  // Message Handlers
  manager.registerMessageDataType(MESSAGE_TYPE_SCALE_CHARACTER, &nodeScaleCharacterMessageLocate, &nodeScaleCharacterMessageDislocate);

  manager.registerMessageHandlerFn(REG_FUNC_ARGS(nodeStateMachineHandleMessages));
  manager.registerMessageHandlerFn(REG_FUNC_ARGS(nodeRetargetUpdateOffsetsAndScaleMessageHandler));
  manager.registerMessageHandlerFn(REG_FUNC_ARGS(nodeScaleCharacterMessageHandler));
  manager.registerMessageHandlerFn(REG_FUNC_ARGS(nodeCCOverrideHandleMessages));
}

//----------------------------------------------------------------------------------------------------------------------
/// Register the morpheme core attribute semantics.
void registerCoreAttribSemantics(
  bool computeRegistryRequirements ///< true:  Determines required array sizes for storage of registered functions, data types etc.
                                   ///< false: Registers functions, data types etc.
  )
{
  Manager& manager = Manager::getInstance();

  //-------------
  // Register attribute semantics

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_UPDATE_TIME_POS), ATTRIB_SENSE_FROM_PARENT_TO_CHILD);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS), ATTRIB_SENSE_FROM_PARENT_TO_CHILD);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_TIME_POS), ATTRIB_SENSE_FROM_CHILD_TO_PARENT);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_ANIM_SAMPLE_POS), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_FRACTION_POS), ATTRIB_SENSE_FROM_CHILD_TO_PARENT);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_LOOPED_ON_UPDATE), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_TRANSFORM_BUFFER), ATTRIB_SENSE_FROM_CHILD_TO_PARENT);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM), ATTRIB_SENSE_FROM_CHILD_TO_PARENT);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER), ATTRIB_SENSE_FROM_CHILD_TO_PARENT);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_VELOCITY), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_TRANSFORM_RATES), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_SYNC_EVENT_TRACK), ATTRIB_SENSE_FROM_CHILD_TO_PARENT);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER), ATTRIB_SENSE_FROM_CHILD_TO_PARENT);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET), ATTRIB_SENSE_FROM_CHILD_TO_PARENT);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_LOOP), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_RIG), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_SOURCE_ANIM), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_START_SYNC_EVENT_INDEX), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_ACTIVE_ANIM_SET_INDEX), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_BLEND_FLAGS), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_BLEND_WEIGHTS), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_CP_FLOAT), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_CP_UINT), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_CP_INT), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_CP_BOOL), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_CP_VECTOR3), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_CP_VECTOR4), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_RIG_RETARGET_MAPPING), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_RETARGET_STORAGE_STATS), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_JOINT_LIMITS), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_SYNC_EVENT_OFFSET), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_BONE_WEIGHTS), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_BONE_IDS), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2), ATTRIB_SENSE_NONE);


  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_PHYSICS_RIG_DEF), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_PHYSICS_RIG), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_CHARACTER_PROPERTIES), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_PHYSICS_INITIALISATION), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_OUTPUT_MASK), ATTRIB_SENSE_FROM_PARENT_TO_CHILD);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_UPDATED), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_PHYSICS_UPDATED), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_ROOT_UPDATED), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_MERGED_PHYSICS_RIG_TRANSFORM_BUFFER), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_TRANSIT_COMPLETE), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_TRANSIT_REVERSED), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_DEAD_BLEND_DEF), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_DEAD_BLEND_STATE), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_DEAD_BLEND_TRANSFORMS_STATE), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_PLAYBACK_POS_INIT), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_EMITTED_MESSAGES_MAP), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_EMITTED_MESSAGES), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_UNEVEN_TERRAIN_IK_SETUP), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_UNEVEN_TERRAIN_PREDICTION_DEF), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_UNEVEN_TERRAIN_PREDICTION_STATE), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_RETARGET_RIG_MAP), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_UPDATE_TIME_POS_T_MINUS_ONE), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_TRAJECTORY_DELTA_T_MINUS_ONE), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_ONE), ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_TRANSFORM_BUFFER_T_MINUS_TWO), ATTRIB_SENSE_NONE);

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_RETARGET_STATE), ATTRIB_SENSE_NONE);
}

//----------------------------------------------------------------------------------------------------------------------
void registerCorePredictionModelTypes()
{
  Manager& manager = Manager::getInstance();
  manager.registerPredictionModel(
    REG_FUNC_ARGS(PREDICTMODELTYPE_ND_MESH),
    PredictionModelNDMesh::locate,
    PredictionModelNDMesh::dislocate);
}

//----------------------------------------------------------------------------------------------------------------------
void registerCoreNodeInitDatas()
{
  Manager::getInstance().registerNodeInitDataType(
    NODE_INIT_DATA_TYPE_STATE_MACHINE,
    StateMachineInitData::locate,
    StateMachineInitData::dislocate);
}

//----------------------------------------------------------------------------------------------------------------------
void registerCoreAttribDataTypes()
{
  Manager& manager = Manager::getInstance();

  // mrRig
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_RIG),
    NMP_NULL_ON_SPU(AttribDataRig::locate),
    NMP_NULL_ON_SPU(AttribDataRig::dislocate),
    NULL,
    NULL,
    NMP_NULL_NOT_ON_SPU(AttribDataRig::prepForSPU));

  // mrAnimSource
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_SOURCE_ANIM),
    NMP_NULL_ON_SPU(AttribDataSourceAnim::locate),
    NMP_NULL_ON_SPU(AttribDataSourceAnim::dislocate));

  // Mirrored animation rig mapping.
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_ANIM_MIRRORED_MAPPING),
    NMP_NULL_ON_SPU(AttribDataMirroredAnimMapping::locate),
    NMP_NULL_ON_SPU(AttribDataMirroredAnimMapping::dislocate),
    NULL,
    AttribDataMirroredAnimMapping::relocate);

  // Joint limits
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_JOINT_LIMITS),
    NMP_NULL_ON_SPU(AttribDataJointLimits::locate),
    NMP_NULL_ON_SPU(AttribDataJointLimits::dislocate),
    NULL,
    AttribDataJointLimits::relocate);

  // Rig retarget mapping.
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_RIG_RETARGET_MAPPING),
    NMP_NULL_ON_SPU(AttribDataRigRetargetMapping::locate),
    NMP_NULL_ON_SPU(AttribDataRigRetargetMapping::dislocate),
    NULL,
    AttribDataRigRetargetMapping::relocate);

  // Retarget state data.
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_RETARGET_STATE),
    NMP_NULL_ON_SPU(AttribDataRetargetState::locate),
    NMP_NULL_ON_SPU(AttribDataRetargetState::dislocate),
    NULL,
    AttribDataRetargetState::relocate);

  // Scale character state data.
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_SCALECHARACTER_STATE),
    NMP_NULL_ON_SPU(AttribDataScaleCharacterState::locate),
    NMP_NULL_ON_SPU(AttribDataScaleCharacterState::dislocate),
    NULL,
    AttribDataScaleCharacterState::relocate);

  // Retarget storage stats.
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_RETARGET_STORAGE_STATS),
    NMP_NULL_ON_SPU(AttribDataRetargetStorageStats::locate),
    NMP_NULL_ON_SPU(AttribDataRetargetStorageStats::dislocate));

  // mrAttribData
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BOOL),
    NMP_NULL_ON_SPU(AttribDataBool::locate),
    NMP_NULL_ON_SPU(AttribDataBool::dislocate),
    NMP_NULL_ON_SPU(AttribDataBool::outputMemReqs),
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataBool::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_UINT),
    NMP_NULL_ON_SPU(AttribDataUInt::locate),
    NMP_NULL_ON_SPU(AttribDataUInt::dislocate),
    NMP_NULL_ON_SPU(AttribDataUInt::outputMemReqs),
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataUInt::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_INT),
    NMP_NULL_ON_SPU(AttribDataInt::locate),
    NMP_NULL_ON_SPU(AttribDataInt::dislocate),
    NULL,
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataInt::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_FLOAT),
    NMP_NULL_ON_SPU(AttribDataFloat::locate),
    NMP_NULL_ON_SPU(AttribDataFloat::dislocate),
    NMP_NULL_ON_SPU(AttribDataFloat::outputMemReqs),
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataFloat::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_VECTOR3),
    NMP_NULL_ON_SPU(AttribDataVector3::locate),
    NMP_NULL_ON_SPU(AttribDataVector3::dislocate),
    NMP_NULL_ON_SPU(AttribDataVector3::outputMemReqs),
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataVector3::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_VECTOR4),
    NMP_NULL_ON_SPU(AttribDataVector4::locate),
    NMP_NULL_ON_SPU(AttribDataVector4::dislocate),
    NULL,
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataVector4::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BOOL_ARRAY),
    NMP_NULL_ON_SPU(AttribDataBoolArray::locate),
    NMP_NULL_ON_SPU(AttribDataBoolArray::dislocate),
    NMP_NULL_ON_SPU(AttribDataBoolArray::outputMemReqs),
    AttribDataBoolArray::relocate,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataBoolArray::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_UINT_ARRAY),
    NMP_NULL_ON_SPU(AttribDataUIntArray::locate),
    NMP_NULL_ON_SPU(AttribDataUIntArray::dislocate),
    NULL,
    AttribDataUIntArray::relocate,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataUIntArray::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_UINT64_ARRAY),
    NMP_NULL_ON_SPU(AttribDataUInt64Array::locate),
    NMP_NULL_ON_SPU(AttribDataUInt64Array::dislocate),
    NULL,
    AttribDataUInt64Array::relocate,
    NULL,
    NULL);
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_INT_ARRAY),
    NMP_NULL_ON_SPU(AttribDataIntArray::locate),
    NMP_NULL_ON_SPU(AttribDataIntArray::dislocate),
    NULL,
    AttribDataIntArray::relocate,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataIntArray::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_FLOAT_ARRAY),
    NMP_NULL_ON_SPU(AttribDataFloatArray::locate),
    NMP_NULL_ON_SPU(AttribDataFloatArray::dislocate),
    NULL,
    AttribDataFloatArray::relocate,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataFloatArray::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_UPDATE_PLAYBACK_POS),
    NMP_NULL_ON_SPU(AttribDataUpdatePlaybackPos::locate),
    NMP_NULL_ON_SPU(AttribDataUpdatePlaybackPos::dislocate),
    NMP_NULL_ON_SPU(AttribDataUpdatePlaybackPos::outputMemReqs),
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataUpdatePlaybackPos::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PLAYBACK_POS),
    NMP_NULL_ON_SPU(AttribDataPlaybackPos::locate),
    NMP_NULL_ON_SPU(AttribDataPlaybackPos::dislocate),
    NMP_NULL_ON_SPU(AttribDataPlaybackPos::outputMemReqs),
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataPlaybackPos::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS),
    NMP_NULL_ON_SPU(AttribDataUpdateSyncEventPlaybackPos::locate),
    NMP_NULL_ON_SPU(AttribDataUpdateSyncEventPlaybackPos::dislocate),
    NMP_NULL_ON_SPU(AttribDataUpdateSyncEventPlaybackPos::outputMemReqs),
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataUpdateSyncEventPlaybackPos::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TRANSFORM_BUFFER),
    NMP_NULL_ON_SPU(AttribDataTransformBuffer::locate),
    NMP_NULL_ON_SPU(AttribDataTransformBuffer::dislocate),
    NMP_NULL_ON_SPU(AttribDataTransformBuffer::outputMemReqs),
    AttribDataTransformBuffer::relocate,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataTransformBuffer::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM),
    NMP_NULL_ON_SPU(AttribDataTrajectoryDeltaTransform::locate),
    NMP_NULL_ON_SPU(AttribDataTrajectoryDeltaTransform::dislocate),
    NMP_NULL_ON_SPU(AttribDataTrajectoryDeltaTransform::outputMemReqs),
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataTrajectoryDeltaTransform::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TRANSFORM),
    NMP_NULL_ON_SPU(AttribDataTransform::locate),
    NMP_NULL_ON_SPU(AttribDataTransform::dislocate),
    NMP_NULL_ON_SPU(AttribDataTransform::outputMemReqs));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_VELOCITY),
    NMP_NULL_ON_SPU(AttribDataVelocity::locate),
    NMP_NULL_ON_SPU(AttribDataVelocity::dislocate),
    NULL,
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataVelocity::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_SYNC_EVENT_TRACK),
    NMP_NULL_ON_SPU(AttribDataSyncEventTrack::locate),
    NMP_NULL_ON_SPU(AttribDataSyncEventTrack::dislocate),
    NMP_NULL_ON_SPU(AttribDataSyncEventTrack::outputMemReqs),
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataSyncEventTrack::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER),
    NULL,
    NULL,
    NMP_NULL_ON_SPU(AttribDataSampledEvents::outputMemReqs),
    AttribDataSampledEvents::relocate,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataSampledEvents::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_DURATION_EVENT_TRACK_SET),
    NULL,
    NULL,
    NMP_NULL_ON_SPU(AttribDataDurationEventTrackSet::outputMemReqs),
    AttribDataDurationEventTrackSet::relocate,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataDurationEventTrackSet::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_SOURCE_EVENT_TRACKS),
    NMP_NULL_ON_SPU(AttribDataSourceEventTrackSet::locate),
    NMP_NULL_ON_SPU(AttribDataSourceEventTrackSet::dislocate),
    NULL,
    AttribDataSourceEventTrackSet::relocate,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataSourceEventTrackSet::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_HEAD_LOOK_SETUP),
    NMP_NULL_ON_SPU(AttribDataHeadLookSetup::locate),
    NMP_NULL_ON_SPU(AttribDataHeadLookSetup::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_HEAD_LOOK_CHAIN),
    NMP_NULL_ON_SPU(AttribDataHeadLookChain::locate),
    NMP_NULL_ON_SPU(AttribDataHeadLookChain::dislocate),
    NULL,
    AttribDataHeadLookChain::relocate);
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_GUN_AIM_SETUP),
    NMP_NULL_ON_SPU(AttribDataGunAimSetup::locate),
    NMP_NULL_ON_SPU(AttribDataGunAimSetup::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_GUN_AIM_IK_CHAIN),
    NMP_NULL_ON_SPU(AttribDataGunAimIKChain::locate),
    NMP_NULL_ON_SPU(AttribDataGunAimIKChain::dislocate),
    NULL,
    AttribDataGunAimIKChain::relocate);
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TWO_BONE_IK_SETUP),
    NMP_NULL_ON_SPU(AttribDataTwoBoneIKSetup::locate),
    NMP_NULL_ON_SPU(AttribDataTwoBoneIKSetup::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TWO_BONE_IK_CHAIN),
    NMP_NULL_ON_SPU(AttribDataTwoBoneIKChain::locate),
    NMP_NULL_ON_SPU(AttribDataTwoBoneIKChain::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_LOCK_FOOT_SETUP),
    NMP_NULL_ON_SPU(AttribDataLockFootSetup::locate),
    NMP_NULL_ON_SPU(AttribDataLockFootSetup::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_LOCK_FOOT_CHAIN),
    NMP_NULL_ON_SPU(AttribDataLockFootChain::locate),
    NMP_NULL_ON_SPU(AttribDataLockFootChain::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_LOCK_FOOT_STATE),
    NMP_NULL_ON_SPU(AttribDataLockFootState::locate),
    NMP_NULL_ON_SPU(AttribDataLockFootState::dislocate),
    NMP_NULL_ON_SPU(AttribDataLockFootState::outputMemReqs),
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataLockFootState::serializeTx));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_HIPS_IK_DEF),
    NMP_NULL_ON_SPU(AttribDataHipsIKDef::locate),
    NMP_NULL_ON_SPU(AttribDataHipsIKDef::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_HIPS_IK_ANIM_SET_DEF),
    NMP_NULL_ON_SPU(AttribDataHipsIKAnimSetDef::locate),
    NMP_NULL_ON_SPU(AttribDataHipsIKAnimSetDef::dislocate));

  // Basic uneven terrain
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_SETUP),
    NMP_NULL_ON_SPU(AttribDataBasicUnevenTerrainSetup::locate),
    NMP_NULL_ON_SPU(AttribDataBasicUnevenTerrainSetup::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_SETUP),
    NULL,
    NULL,
    NMP_NULL_ON_SPU(AttribDataBasicUnevenTerrainIKSetup::outputMemReqs),
    AttribDataBasicUnevenTerrainIKSetup::relocate);
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET),
    NULL,
    NULL,
    NMP_NULL_ON_SPU(AttribDataBasicUnevenTerrainFootLiftingTarget::outputMemReqs),
    AttribDataBasicUnevenTerrainFootLiftingTarget::relocate);
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_STATE),
    NULL,
    NULL,
    NMP_NULL_ON_SPU(AttribDataBasicUnevenTerrainIKState::outputMemReqs),
    AttribDataBasicUnevenTerrainIKState::relocate);
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_CHAIN),
    NMP_NULL_ON_SPU(AttribDataBasicUnevenTerrainChain::locate),
    NMP_NULL_ON_SPU(AttribDataBasicUnevenTerrainChain::dislocate),
    NULL,
    AttribDataBasicUnevenTerrainChain::relocate);

  // Predictive uneven terrain
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_IK_PREDICTION_STATE),
    NULL,
    NULL,
    NMP_NULL_ON_SPU(AttribDataPredictiveUnevenTerrainPredictionState::outputMemReqs),
    AttribDataPredictiveUnevenTerrainPredictionState::relocate);
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_FOOT_LIFTING_STATE),
    NULL,
    NULL,
    NMP_NULL_ON_SPU(AttribDataPredictiveUnevenTerrainFootLiftingState::outputMemReqs),
    AttribDataPredictiveUnevenTerrainFootLiftingState::relocate);
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_PREDICTION_DEF),
    NMP_NULL_ON_SPU(AttribDataPredictiveUnevenTerrainPredictionDef::locate),
    NMP_NULL_ON_SPU(AttribDataPredictiveUnevenTerrainPredictionDef::dislocate),
    NULL,
    AttribDataPredictiveUnevenTerrainPredictionDef::relocate);

  // Character controller
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_CHARACTER_PROPERTIES),
    NMP_NULL_ON_SPU(AttribDataCharacterProperties::locate),
    NMP_NULL_ON_SPU(AttribDataCharacterProperties::dislocate),
    NULL
#ifdef NM_DEBUG
    , AttribDataCharacterProperties::relocate
#endif
    );

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_CHARACTER_CONTROLLER_DEF),
    NMP_NULL_ON_SPU(AttribDataCharacterControllerDef::locate),
    NMP_NULL_ON_SPU(AttribDataCharacterControllerDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_CLOSEST_ANIM_DEF),
    NMP_NULL_ON_SPU(AttribDataClosestAnimDef::locate),
    NMP_NULL_ON_SPU(AttribDataClosestAnimDef::dislocate));
    
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_CLOSEST_ANIM_DEF_ANIM_SET),
    NMP_NULL_ON_SPU(AttribDataClosestAnimDefAnimSet::locate),
    NMP_NULL_ON_SPU(AttribDataClosestAnimDefAnimSet::dislocate));
    
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_CLOSEST_ANIM_STATE),
    NMP_NULL_ON_SPU(AttribDataClosestAnimState::locate),
    NMP_NULL_ON_SPU(AttribDataClosestAnimState::dislocate),
    NMP_NULL_ON_SPU(AttribDataClosestAnimState::outputMemReqs));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PLAYBACK_POS_INIT),
    NMP_NULL_ON_SPU(AttribDataPlaybackPosInit::locate),
    NMP_NULL_ON_SPU(AttribDataPlaybackPosInit::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_EMITTED_MESSAGE_MAP),
    NMP_NULL_ON_SPU(AttribDataEmittedMessagesMap::locate),
    NMP_NULL_ON_SPU(AttribDataEmittedMessagesMap::dislocate));

#ifndef NM_HOST_CELL_SPU
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_STATE_MACHINE_DEF),
    NMP_NULL_ON_SPU(AttribDataStateMachineDef::locate),
    NMP_NULL_ON_SPU(AttribDataStateMachineDef::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_STATE_MACHINE),
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataStateMachine::serializeTx));
#endif // NM_HOST_CELL_SPU

#ifndef NM_HOST_CELL_SPU
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_FLOAT_OPERATION),
    NMP_NULL_ON_SPU(AttribDataFunctionOperation::locate),
    NMP_NULL_ON_SPU(AttribDataFunctionOperation::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_2_FLOAT_OPERATION),
    NMP_NULL_ON_SPU(AttribDataArithmeticOperation::locate),
    NMP_NULL_ON_SPU(AttribDataArithmeticOperation::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_SMOOTH_FLOAT_OPERATION),
    NMP_NULL_ON_SPU(AttribDataSmoothFloatOperation::locate),
    NMP_NULL_ON_SPU(AttribDataSmoothFloatOperation::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_RATE_OF_CHANGE_OPERATION),
    NMP_NULL_ON_SPU(AttribDataRateOfChangeState::locate),
    NMP_NULL_ON_SPU(AttribDataRateOfChangeState::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_RANDOM_FLOAT_OPERATION),
    NMP_NULL_ON_SPU(AttribDataRandomFloatOperation::locate),
    NMP_NULL_ON_SPU(AttribDataRandomFloatOperation::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BOOLEAN_OPERATION),
    NMP_NULL_ON_SPU(AttribDataBooleanOperation::locate),
    NMP_NULL_ON_SPU(AttribDataBooleanOperation::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_VALUE_COMPARE_OPERATION),
    NMP_NULL_ON_SPU(AttribDataValueCompareOperation::locate),
    NMP_NULL_ON_SPU(AttribDataValueCompareOperation::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TIME_LAG_INPUT_FLOAT)
#ifndef NM_HOST_CELL_SPU
    , AttribDataTimeLagInput<float, ATTRIB_TYPE_TIME_LAG_INPUT_FLOAT>::locate,
    AttribDataTimeLagInput<float, ATTRIB_TYPE_TIME_LAG_INPUT_FLOAT>::dislocate
#endif //NM_HOST_CELL_SPU
    );

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TIME_LAG_INPUT_INT)
#ifndef NM_HOST_CELL_SPU
    , AttribDataTimeLagInput<int32_t, ATTRIB_TYPE_TIME_LAG_INPUT_INT>::locate,
    AttribDataTimeLagInput<int32_t, ATTRIB_TYPE_TIME_LAG_INPUT_INT>::dislocate
#endif //NM_HOST_CELL_SPU
    );

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR3)
#ifndef NM_HOST_CELL_SPU
    , AttribDataTimeLagInput<NMP::Vector3, ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR3>::locate,
    AttribDataTimeLagInput<NMP::Vector3, ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR3>::dislocate
#endif //NM_HOST_CELL_SPU
    );
  
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR4)
#ifndef NM_HOST_CELL_SPU
    , AttribDataTimeLagInput<NMP::Quat, ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR4>::locate,
    AttribDataTimeLagInput<NMP::Quat, ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR4>::dislocate
#endif //NM_HOST_CELL_SPU
    );

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TIME_LAG_INPUT_BOOL)
#ifndef NM_HOST_CELL_SPU
    , AttribDataTimeLagInput<bool, ATTRIB_TYPE_TIME_LAG_INPUT_BOOL>::locate,
    AttribDataTimeLagInput<bool, ATTRIB_TYPE_TIME_LAG_INPUT_BOOL>::dislocate
#endif //NM_HOST_CELL_SPU
    );

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_RANDOM_FLOAT_DEF),
    NMP_NULL_ON_SPU(AttribDataRandomFloatDef::locate),
    NMP_NULL_ON_SPU(AttribDataRandomFloatDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_NOISE_GEN_DEF),
    NMP_NULL_ON_SPU(AttribDataNoiseGenDef::locate),
    NMP_NULL_ON_SPU(AttribDataNoiseGenDef::dislocate),
    NULL,
    AttribDataNoiseGenDef::relocate);
#endif // NM_HOST_CELL_SPU
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_SWITCH_DEF),
    NMP_NULL_ON_SPU(AttribDataSwitchDef::locate),
    NMP_NULL_ON_SPU(AttribDataSwitchDef::dislocate));

#ifndef NM_HOST_CELL_SPU
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_RAY_CAST_DEF),
    NMP_NULL_ON_SPU(AttribDataRayCastDef::locate),
    NMP_NULL_ON_SPU(AttribDataRayCastDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PHYSICS_INFO_DEF),
    NMP_NULL_ON_SPU(AttribDataPhysicsInfoDef::locate),
    NMP_NULL_ON_SPU(AttribDataPhysicsInfoDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PHYSICS_SETUP),
    NMP_NULL_ON_SPU(AttribDataPhysicsSetup::locate),
    NMP_NULL_ON_SPU(AttribDataPhysicsSetup::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PHYSICS_SETUP_ANIM_SET),
    NMP_NULL_ON_SPU(AttribDataPhysicsSetupPerAnimSet::locate),
    NMP_NULL_ON_SPU(AttribDataPhysicsSetupPerAnimSet::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PHYSICS_STATE),
    NMP_NULL_ON_SPU(AttribDataPhysicsState::locate),
    NMP_NULL_ON_SPU(AttribDataPhysicsState::dislocate),
    NMP_NULL_ON_SPU(AttribDataPhysicsState::outputMemReqs),
    AttribDataPhysicsState::relocate);
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PHYSICS_INITIALISATION),
    NMP_NULL_ON_SPU(AttribDataPhysicsInitialisation::locate),
    NMP_NULL_ON_SPU(AttribDataPhysicsInitialisation::dislocate),
    NULL,
    AttribDataPhysicsInitialisation::relocate);
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PHYSICAL_EFFECT_DATA),
    NMP_NULL_ON_SPU(AttribDataPhysicalEffectData::locate),
    NMP_NULL_ON_SPU(AttribDataPhysicalEffectData::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PHYSICS_GROUPER_CONFIG),
    NMP_NULL_ON_SPU(AttribDataPhysicsGrouperConfig::locate),
    NMP_NULL_ON_SPU(AttribDataPhysicsGrouperConfig::dislocate));
#endif // NM_HOST_CELL_SPU

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TRANSIT_DEF),
    NMP_NULL_ON_SPU(AttribDataTransitDef::locate),
    NMP_NULL_ON_SPU(AttribDataTransitDef::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TRANSIT_SYNC_EVENTS_DEF),
    NMP_NULL_ON_SPU(AttribDataTransitSyncEventsDef::locate),
    NMP_NULL_ON_SPU(AttribDataTransitSyncEventsDef::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TRANSIT_SYNC_EVENTS),
    NMP_NULL_ON_SPU(AttribDataTransitSyncEvents::locate),
    NMP_NULL_ON_SPU(AttribDataTransitSyncEvents::dislocate),
    NMP_NULL_ON_SPU(AttribDataTransitSyncEvents::outputMemReqs),
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataTransitSyncEvents::serializeTx));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BLEND_NXM_DEF),
    NMP_NULL_ON_SPU(AttribDataBlendNxMDef::locate),
    NMP_NULL_ON_SPU(AttribDataBlendNxMDef::dislocate),
    NULL,
    AttribDataBlendNxMDef::relocate);

  // Dead Blends.
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_DEAD_BLEND_DEF),
    NMP_NULL_ON_SPU(AttribDataDeadBlendDef::locate),
    NMP_NULL_ON_SPU(AttribDataDeadBlendDef::dislocate));
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_DEAD_BLEND_STATE),
    NMP_NULL_ON_SPU(AttribDataDeadBlendState::locate),
    NMP_NULL_ON_SPU(AttribDataDeadBlendState::dislocate));

#ifndef NM_HOST_CELL_SPU
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_EMIT_MESSAGE_ON_CP_VALUE),
    NMP_NULL_ON_SPU(AttribDataEmitMessageOnCPValueDef::locate),
    NMP_NULL_ON_SPU(AttribDataEmitMessageOnCPValueDef::dislocate));
#endif // NM_HOST_CELL_SPU

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BLEND_FLAGS),
    NMP_NULL_ON_SPU(AttribDataBlendFlags::locate),
    NMP_NULL_ON_SPU(AttribDataBlendFlags::dislocate),
    NULL);

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BLEND_WEIGHTS),
    NMP_NULL_ON_SPU(AttribDataBlendWeights::locate),
    NMP_NULL_ON_SPU(AttribDataBlendWeights::dislocate),
    NMP_NULL_ON_SPU(AttribDataBlendWeights::outputMemReqs),
    AttribDataBlendWeights::relocate);

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_FEATHER_BLEND2_CHANNEL_ALPHAS),
    NMP_NULL_ON_SPU(AttribDataFeatherBlend2ChannelAlphas::locate),
    NMP_NULL_ON_SPU(AttribDataFeatherBlend2ChannelAlphas::dislocate),
    NULL,
    AttribDataFeatherBlend2ChannelAlphas::relocate);

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_SCATTER_BLEND_ANALYSIS_DEF),
    NMP_NULL_ON_SPU(AttribDataScatterBlendAnalysisDef::locate),
    NMP_NULL_ON_SPU(AttribDataScatterBlendAnalysisDef::dislocate),
    NULL,
    AttribDataScatterBlendAnalysisDef::relocate);

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_SCATTER_BLEND_1D_DEF),
    NMP_NULL_ON_SPU(AttribDataScatterBlend1DDef::locate),
    NMP_NULL_ON_SPU(AttribDataScatterBlend1DDef::dislocate),
    NULL,
    AttribDataScatterBlend1DDef::relocate);
    
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_SCATTER_BLEND_2D_DEF),
    NMP_NULL_ON_SPU(AttribDataScatterBlend2DDef::locate),
    NMP_NULL_ON_SPU(AttribDataScatterBlend2DDef::dislocate),
    NULL,
    AttribDataScatterBlend2DDef::relocate);

#ifndef NM_HOST_CELL_SPU
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_C_C_OVERRIDE_CONDITIONS_DEF),
    NMP_NULL_ON_SPU(AttribDataCCOverrideConditionsDef::locate),
    NMP_NULL_ON_SPU(AttribDataCCOverrideConditionsDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_C_C_OVERRIDE_PROPERTIES_DEF),
    NMP_NULL_ON_SPU(AttribDataCCOverridePropertiesDef::locate),
    NMP_NULL_ON_SPU(AttribDataCCOverridePropertiesDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_C_C_OVERRIDE_CONDITIONS));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TIME_LAG_DEF),
    NMP_NULL_ON_SPU(AttribDataTimeLagInputDef::locate),
    NMP_NULL_ON_SPU(AttribDataTimeLagInputDef::dislocate));
#endif // NM_HOST_CELL_SPU

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_MODIFY_JOINT_DEF),
    NMP_NULL_ON_SPU(AttribDataModifyJointDef::locate),
    NMP_NULL_ON_SPU(AttribDataModifyJointDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_MODIFY_TRAJECTORY_DEF),
    NMP_NULL_ON_SPU(AttribDataModifyTrajectoryDef::locate),
    NMP_NULL_ON_SPU(AttribDataModifyTrajectoryDef::dislocate));
}

//----------------------------------------------------------------------------------------------------------------------
void registerCoreTransitConditions()
{
  Manager& manager = Manager::getInstance();

  manager.registerTransitCondType(
    TRANSCOND_ON_MESSAGE_ID,
    TransitConditionDefOnMessage::defLocate,
    TransitConditionDefOnMessage::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefOnMessage::instanceInit,
    TransitConditionDef::defaultInstanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDefOnMessage::instanceReset);
  manager.registerTransitCondType(
    TRANSCOND_DISCRETE_EVENT_TRIGGERED_ID,
    TransitConditionDefDiscreteEventTriggered::defLocate,
    TransitConditionDefDiscreteEventTriggered::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefDiscreteEventTriggered::instanceInit,
    TransitConditionDefDiscreteEventTriggered::instanceUpdate,
    TransitConditionDefDiscreteEventTriggered::instanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);
  manager.registerTransitCondType(
    TRANSCOND_CROSSED_DURATION_FRACTION_ID,
    TransitConditionDefCrossedDurationFraction::defLocate,
    TransitConditionDefCrossedDurationFraction::defDislocate,
    NULL, // no relocate function required
    TransitConditionDefCrossedDurationFraction::instanceGetMemoryRequirements,
    TransitConditionDefCrossedDurationFraction::instanceInit,
    TransitConditionDefCrossedDurationFraction::instanceUpdate,
    TransitConditionDefCrossedDurationFraction::instanceQueueDeps,
    TransitConditionDefCrossedDurationFraction::instanceReset);
  manager.registerTransitCondType(
    TRANSCOND_IN_DURATION_EVENT_ID,
    TransitConditionDefInDurationEvent::defLocate,
    TransitConditionDefInDurationEvent::defDislocate,
    NULL, // no relocate function required
    TransitConditionDefInDurationEvent::instanceGetMemoryRequirements,
    TransitConditionDefInDurationEvent::instanceInit,
    TransitConditionDefInDurationEvent::instanceUpdate,
    TransitConditionDefInDurationEvent::instanceQueueDeps,
    TransitConditionDefInDurationEvent::instanceReset);
  manager.registerTransitCondType(
    TRANSCOND_CONTROL_PARAM_FLOAT_GREATER_ID,
    TransitConditionDefControlParamFloatGreater::defLocate,
    TransitConditionDefControlParamFloatGreater::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefControlParamFloatGreater::instanceInit,
    TransitConditionDefControlParamFloatGreater::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);
  manager.registerTransitCondType(
    TRANSCOND_FALSE_ID,
    TransitConditionDefFalse::defLocate,
    TransitConditionDefFalse::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefFalse::instanceInit,
    TransitConditionDef::defaultInstanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);
  manager.registerTransitCondType(
    TRANSCOND_CONTROL_PARAM_FLOAT_LESS_ID,
    TransitConditionDefControlParamFloatLess::defLocate,
    TransitConditionDefControlParamFloatLess::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefControlParamFloatLess::instanceInit,
    TransitConditionDefControlParamFloatLess::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);
  manager.registerTransitCondType(
    TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID,
    TransitConditionDefControlParamFloatInRange::defLocate,
    TransitConditionDefControlParamFloatInRange::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefControlParamFloatInRange::instanceInit,
    TransitConditionDefControlParamFloatInRange::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);
  manager.registerTransitCondType(
    TRANSCOND_CROSSED_CURVE_EVENT_VALUE_DECREASING_ID,
    TransitConditionDefCrossedCurveEventValueDecreasing::defLocate,
    TransitConditionDefCrossedCurveEventValueDecreasing::defDislocate,
    NULL, // no relocate function required
    TransitConditionDefCrossedCurveEventValueDecreasing::instanceGetMemoryRequirements,
    TransitConditionDefCrossedCurveEventValueDecreasing::instanceInit,
    TransitConditionDefCrossedCurveEventValueDecreasing::instanceUpdate,
    TransitConditionDefCrossedCurveEventValueDecreasing::instanceQueueDeps,
    TransitConditionDefCrossedCurveEventValueDecreasing::instanceReset);
  manager.registerTransitCondType(
    TRANSCOND_IN_SYNC_EVENT_RANGE_ID,
    TransitConditionDefInSyncEventRange::defLocate,
    TransitConditionDefInSyncEventRange::defDislocate,
    NULL, // no relocate function required
    TransitConditionDefInSyncEventRange::instanceGetMemoryRequirements,
    TransitConditionDefInSyncEventRange::instanceInit,
    TransitConditionDefInSyncEventRange::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDefInSyncEventRange::instanceReset);

  manager.registerTransitCondType(
    TRANSCOND_NODE_ACTIVE_ID,
    TransitConditionDefNodeActive::defLocate,
    TransitConditionDefNodeActive::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefNodeActive::instanceInit,
    TransitConditionDefNodeActive::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);

  manager.registerTransitCondType(
    TRANSCOND_RAY_HIT_ID,
    TransitConditionDefRayHit::defLocate,
    TransitConditionDefRayHit::defDislocate,
    NULL,
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefRayHit::instanceInit,
    TransitConditionDefRayHit::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);

  manager.registerTransitCondType(
    TRANSCOND_CONTROL_PARAM_INT_GREATER_ID,
    TransitConditionDefControlParamIntGreater::defLocate,
    TransitConditionDefControlParamIntGreater::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefControlParamIntGreater::instanceInit,
    TransitConditionDefControlParamIntGreater::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);

  manager.registerTransitCondType(
    TRANSCOND_CONTROL_PARAM_INT_LESS_ID,
    TransitConditionDefControlParamIntLess::defLocate,
    TransitConditionDefControlParamIntLess::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefControlParamIntLess::instanceInit,
    TransitConditionDefControlParamIntLess::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);

  manager.registerTransitCondType(
    TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID,
    TransitConditionDefControlParamIntInRange::defLocate,
    TransitConditionDefControlParamIntInRange::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefControlParamIntInRange::instanceInit,
    TransitConditionDefControlParamIntInRange::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);

  manager.registerTransitCondType(
    TRANSCOND_CONTROL_PARAM_UINT_GREATER_ID,
    TransitConditionDefControlParamUIntGreater::defLocate,
    TransitConditionDefControlParamUIntGreater::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefControlParamUIntGreater::instanceInit,
    TransitConditionDefControlParamUIntGreater::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);

  manager.registerTransitCondType(
    TRANSCOND_CONTROL_PARAM_UINT_LESS_ID,
    TransitConditionDefControlParamUIntLess::defLocate,
    TransitConditionDefControlParamUIntLess::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefControlParamUIntLess::instanceInit,
    TransitConditionDefControlParamUIntLess::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);

  manager.registerTransitCondType(
    TRANSCOND_CONTROL_PARAM_UINT_IN_RANGE_ID,
    TransitConditionDefControlParamUIntInRange::defLocate,
    TransitConditionDefControlParamUIntInRange::defDislocate,
    NULL, // no relocate function required
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefControlParamUIntInRange::instanceInit,
    TransitConditionDefControlParamUIntInRange::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);
}

//----------------------------------------------------------------------------------------------------------------------
void registerCoreAssets()
{
  MR::Manager& manager = MR::Manager::getInstance();

  manager.registerAsset(MR::Manager::kAsset_NetworkDef, locateNetworkDef);

  manager.registerAsset(MR::Manager::kAsset_Rig, locateAnimRigDef);

  manager.registerAsset(MR::Manager::kAsset_RigToAnimMap, locateRigToAnimMap);

  manager.registerAsset(MR::Manager::kAsset_EventTrackDuration, locateEventTrackDefDuration);

  manager.registerAsset(MR::Manager::kAsset_EventTrackDiscrete, locateEventTrackDefDiscrete);

  manager.registerAsset(MR::Manager::kAsset_EventTrackCurve, locateEventTrackDefCurve);

  manager.registerAsset(MR::Manager::kAsset_CharacterControllerDef, locateCharacterControllerDef);

  manager.registerAsset(MR::Manager::kAsset_NetworkPredictionDef, locateNetworkPredictionDef);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
