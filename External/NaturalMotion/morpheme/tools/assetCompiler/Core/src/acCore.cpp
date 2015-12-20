// CopyegisterStight (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "acCore.h"
#include "NetworkDefBuilder.h"
#include "RigBuilder.h"
#include "CharacterControllerDefBuilder.h"
#include "NetworkPredictionDefBuilder.h"
#include "EventTrackDiscreteBuilder.h"
#include "EventTrackCurveBuilder.h"
#include "EventTrackDurationBuilder.h"

#pragma warning(disable: 4996)
#include "NodeAnimSyncEventsBuilder.h"
#include "NodeBlend2Builder.h"
#include "NodeBlendNBuilder.h"
#include "NodeBlend2x2Builder.h"
#include "NodeBlendNxMBuilder.h"
#include "NodeBlendAllBuilder.h"
#include "NodeSubtractiveBlendBuilder.h"
#include "NodeScatterBlend1DBuilder.h"
#include "NodeScatterBlendBuilder.h"
#include "NodeControlParamFloatBuilder.h"
#include "NodeControlParamVector3Builder.h"
#include "NodeControlParamVector4Builder.h"
#include "NodeControlParamBoolBuilder.h"
#include "NodeControlParamIntBuilder.h"
#include "NodeControlParamUIntBuilder.h"
#include "NodeFilterTransformsBuilder.h"
#include "NodeApplyBindPoseBuilder.h"
#include "NodeApplyGlobalTimeBuilder.h"
#include "NodeStateMachineBuilder.h"
#include "NodeFeatherBlend2Builder.h"
#include "NodeHeadLookBuilder.h"
#include "NodeTwoBoneIKBuilder.h"
#include "NodeLockFootBuilder.h"
#include "NodeHipsIKBuilder.h"
#include "NodeModifyJointTransformBuilder.h"
#include "NodeModifyTrajectoryTransformBuilder.h"
#include "NodeSingleFrameBuilder.h"
#include "NodeClosestAnimBuilder.h"
#include "NodeFreezeBuilder.h"
#include "NodeSwitchBuilder.h"
#include "NodeSequenceBuilder.h"
#include "NodePassThroughBuilder.h"
#include "NodePlaySpeedModifierBuilder.h"
#include "NodeScaleToDurationBuilder.h"
#include "NodeGunAimIKBuilder.h"
#include "NodeBasicUnevenTerrainBuilder.h"
#include "NodePredictiveUnevenTerrainBuilder.h"
#include "NodeCharacterControllerOverrideBuilder.h"
#include "TransitConditionDefBuilderOnMessage.h"
#include "TransitConditionDefBuilderDiscreteEventTriggered.h"
#include "TransitConditionDefBuilderCrossedDurationFraction.h"
#include "TransitConditionDefBuilderFalse.h"
#include "TransitConditionDefBuilderControlParamInRange.h"
#include "TransitConditionDefBuilderControlParamTest.h"
#include "TransitConditionDefBuilderControlParamBoolSet.h"
#include "TransitConditionDefBuilderCrossedCurveEventValueDecreasing.h"
#include "TransitConditionDefBuilderInSyncEventRange.h"
#include "TransitConditionDefBuilderNodeActive.h"
#include "TransitConditionDefBuilderInDurationEvent.h"
#include "TransitConditionDefBuilderRayHit.h"
#include "NodeOperatorFunctionBuilder.h"
#include "NodeOperatorArithmeticBuilder.h"
#include "NodeOperatorArithmeticVector3Builder.h"
#include "NodeOperatorOneInputArithmeticBuilder.h"
#include "NodeOperatorOneInputArithmeticVector3Builder.h"
#include "NodeOperatorBooleanBuilder.h"
#include "NodeOperatorCPCompareBuilder.h"

#include "NodeOperatorNoiseGenBuilder.h"
#include "NodeOperatorSmoothFloatBuilder.h"
#include "NodeOperatorVector3AngleBuilder.h"
#include "NodeOperatorRateOfChangeBuilder.h"
#include "NodeOperatorRampFloatBuilder.h"
#include "NodeOperatorFloatsToVector3Builder.h"
#include "NodeOperatorFloatToIntBuilder.h"
#include "NodeOperatorIntToFloatBuilder.h"
#include "NodeOperatorVector3ToFloatsBuilder.h"
#include "NodeOperatorVector3DotBuilder.h"
#include "NodeOperatorVector3NormaliseBuilder.h"
#include "NodeOperatorVector3DistanceBuilder.h"
#include "NodeOperatorVector3CrossProductBuilder.h"
#include "NodeOperatorRandomFloatBuilder.h"
#include "NodeOperatorRayCastBuilder.h"
#include "NodeOperatorTimeLagInputBuilder.h"
#include "NodeTransitBuilder.h"
#include "NodeSmoothTransformsBuilder.h"
#include "NodeTransitSyncEventsBuilder.h"
#include "TransitDeadBlendDefBuilderDefault.h"
#include "NodeMirrorTransformsBuilder.h"
#include "NodeExtractJointInfoBuilder.h"

#include "NodeRetargetBuilder.h"
#include "NodeScaleCharacterBuilder.h"

#include "MessageScaleCharacterBuilder.h"
#include "NodeEmitMessageOnDiscreteEventBuilder.h"

#include "Analysis/DemoPredictionModel.h"
#include "Analysis/PredictionModelNDMeshAnalyser.h"
#include "Analysis/ScatterBlend1DTask.h"
#include "Analysis/ScatterBlendVertexSourceAnalyser.h"
#include "Analysis/ScatterBlendTriangleAnnotationAnalyser.h"
#include "Analysis/ScatterBlend2DBSPTreeAnalyser.h"

#include "Analysis/AnalysisConditionAtFrame.h"
#include "Analysis/AnalysisConditionDiscreteEventTriggered.h"
#include "Analysis/AnalysisConditionCrossedDurationFraction.h"
#include "Analysis/AnalysisConditionInSyncEventRange.h"
#include "Analysis/AnalysisConditionStateMachineInState.h"

#include "Analysis/AnalysisPropertyPositionArcDistance.h"
#include "Analysis/AnalysisPropertyPositionDeltaDistance.h"
#include "Analysis/AnalysisPropertyPositionDeltaInAxis.h"
#include "Analysis/AnalysisPropertyVelocityInAxis.h"
#include "Analysis/AnalysisPropertyTurningRate.h"
#include "Analysis/AnalysisPropertyRotationDeltaInAxis.h"
#include "Analysis/AnalysisPropertyRateOfTurnWithForwardTravel.h"
#include "Analysis/AnalysisPropertyTravelDirectionInPlane.h"
#include "Analysis/AnalysisPropertySpeedForCutTurns.h"
#include "Analysis/AnalysisPropertySpeedInPlane.h"
#include "Analysis/AnalysisPropertySpeed.h"
#include "Analysis/AnalysisPropertyTravelSlopeAngle.h"
#include "Analysis/AnalysisPropertyTakeOffDirectionInPlane.h"
#include "Analysis/AnalysisPropertyPositionValueInAxis.h"
#include "Analysis/AnalysisPropertyCustom.h"
#include "Analysis/AnalysisPropertyStrideLength.h"
#include "Analysis/AnalysisPropertyEulerAngleComponent.h"

#include "morpheme/Nodes/mrNodes.h"

#include "NetworkPredictionDefBuilder.h"
#include "Analysis/PredictionModelNDMeshBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace ME;

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
void initBuilders(AssetProcessor& assetProc)
{
  //-------------------------------------
  // Initialise the network builder
  NetworkDefBuilder* networkDefBuilder;
  networkDefBuilder = new NetworkDefBuilder();
  assetProc.registerNetworkDefBuilder(networkDefBuilder);

  //-------------------------------------
  // Initialise node builders.
  NodeDefBuilder* nodeBuilder;

  nodeBuilder = new NodePassThroughBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_PASSTHROUGH, nodeBuilder);

  nodeBuilder = new NodeAnimWithEventsBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_ANIM_EVENTS, nodeBuilder);

  nodeBuilder = new NodeBlend2Builder();
  assetProc.registerNodeBuilder(NODE_TYPE_BLEND_2, nodeBuilder);

  nodeBuilder = new NodeBlendNBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_BLEND_N, nodeBuilder);

  nodeBuilder = new NodeFeatherBlend2Builder();
  assetProc.registerNodeBuilder(NODE_TYPE_FEATHER_BLEND_2, nodeBuilder);

  nodeBuilder = new NodeBlend2x2Builder();
  assetProc.registerNodeBuilder(NODE_TYPE_BLEND_2X2, nodeBuilder);

  nodeBuilder = new NodeBlendNxMBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_BLEND_NXM, nodeBuilder);

  nodeBuilder = new NodeScatterBlend1DBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_SCATTER_BLEND_1D, nodeBuilder);

  nodeBuilder = new NodeScatterBlendBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_SCATTER_BLEND_2D, nodeBuilder);

  nodeBuilder = new NodeBlendAllBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_BLEND_ALL, nodeBuilder);

  nodeBuilder = new NodeSubtractiveBlendBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_SUBTRACTIVE_BLEND, nodeBuilder);

  nodeBuilder = new NodeControlParamFloatBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_FLOAT, nodeBuilder);

  nodeBuilder = new NodeControlParamVector3Builder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_VECTOR3, nodeBuilder);

  nodeBuilder = new NodeControlParamVector4Builder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_VECTOR4, nodeBuilder);

  nodeBuilder = new NodeControlParamBoolBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_BOOL, nodeBuilder);

  nodeBuilder = new NodeControlParamIntBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_INT, nodeBuilder);

  nodeBuilder = new NodeControlParamUIntBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_UINT, nodeBuilder);

  nodeBuilder = new NodeFilterTransformsBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_FILTER_TRANSFORMS, nodeBuilder);

  nodeBuilder = new NodeMirrorBuilder();
  assetProc.registerNodeBuilder(NODE_MIRROR_TRANSFORMS_ID, nodeBuilder);

  nodeBuilder = new NodeApplyBindPoseBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_APPLY_BIND_POSE, nodeBuilder);

  nodeBuilder = new NodeApplyGlobalTimeBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_APPLY_GLOBAL_TIME, nodeBuilder);

  nodeBuilder = new NodeStateMachineBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_STATE_MACHINE, nodeBuilder);

  nodeBuilder = new NodeTransitBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_TRANSIT, nodeBuilder);

  nodeBuilder = new NodeTransitSyncEventsBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_TRANSIT_SYNC_EVENTS, nodeBuilder);

  nodeBuilder = new NodeHeadLookBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_HEAD_LOOK, nodeBuilder);

  nodeBuilder = new NodeTwoBoneIKBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_TWO_BONE_IK, nodeBuilder);

  nodeBuilder = new NodeGunAimIKBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_GUN_AIM_IK, nodeBuilder);

  nodeBuilder = new NodeLockFootBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_LOCK_FOOT, nodeBuilder);

  nodeBuilder = new NodeBasicUnevenTerrainBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_BASIC_UNEVEN_TERRAIN, nodeBuilder);

  nodeBuilder = new NodePredictiveUnevenTerrainBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN, nodeBuilder);

  nodeBuilder = new NodeHipsIKBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_HIPS_IK, nodeBuilder);

  nodeBuilder = new NodeModifyJointTransformBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_MODIFY_JOINT_TRANSFORM, nodeBuilder);

  nodeBuilder = new NodeModifyTrajectoryTransformBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_MODIFY_TRAJECTORY_TRANSFORM, nodeBuilder);
  
  nodeBuilder = new NodeSingleFrameBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_SINGLEFRAME, nodeBuilder);

  nodeBuilder = new NodeClosestAnimBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CLOSEST_ANIM, nodeBuilder);

  nodeBuilder = new NodeFreezeBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_FREEZE, nodeBuilder);

  nodeBuilder = new NodeSwitchBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_SWITCH, nodeBuilder);

  nodeBuilder = new NodeSequenceBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_SEQUENCE, nodeBuilder);

  nodeBuilder = new NodeSmoothTransformsBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_SMOOTH_TRANSFORMS, nodeBuilder);

  nodeBuilder = new NodePlaySpeedModifierBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_PLAY_SPEED_MODIFIER, nodeBuilder);

  nodeBuilder = new NodeScaleToDurationBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_SCALE_TO_DURATION, nodeBuilder);

  nodeBuilder = new NodeExtractJointInfoBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_EMIT_JOINT_CP_INFO, nodeBuilder);

  nodeBuilder = new NodeEmitMessageOnDiscreteEventBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_EMIT_MESSAGE_ON_DISCRETE_EVENT, nodeBuilder);

  nodeBuilder = new NodeOperatorFunctionBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_FUNCTION, nodeBuilder);

  nodeBuilder = new NodeOperatorArithmeticBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_ARITHMETIC, nodeBuilder);

  nodeBuilder = new NodeOperatorArithmeticVector3Builder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_ARITHMETIC_VECTOR3, nodeBuilder);

 nodeBuilder = new NodeOperatorOneInputArithmeticBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC, nodeBuilder);

  nodeBuilder = new NodeOperatorOneInputArithmeticVector3Builder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_ONE_INPUT_ARITHMETIC_VECTOR3, nodeBuilder);

  nodeBuilder = new NodeOperatorBooleanBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_BOOLEAN, nodeBuilder);

  nodeBuilder = new NodeOperatorCPCompareBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_COND_CP_COMPARE, nodeBuilder);
  
  nodeBuilder = new NodeOperatorNoiseGenBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_NOISE_GEN, nodeBuilder);

  nodeBuilder = new NodeOperatorSmoothFloatBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_SMOOTH_FLOAT, nodeBuilder);

  nodeBuilder = new NodeOperatorVector3AngleBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_VECTOR3_ANGLE, nodeBuilder);

  nodeBuilder = new NodeOperatorRateOfChangeBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_RATE_OF_CHANGE, nodeBuilder);

  nodeBuilder = new NodeOperatorRampFloatBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_RAMP_FLOAT, nodeBuilder);

  nodeBuilder = new NodeOperatorFloatsToVector3Builder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_FLOATS_TO_VECTOR3, nodeBuilder);

  nodeBuilder = new NodeOperatorFloatToIntBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_FLOAT_TO_INT, nodeBuilder);

  nodeBuilder = new NodeOperatorIntToFloatBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_INT_TO_FLOAT, nodeBuilder);

  nodeBuilder = new NodeOperatorVector3ToFloatsBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_VECTOR3_TO_FLOATS, nodeBuilder);

  nodeBuilder = new NodeOperatorVector3DotBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_VECTOR3_DOT, nodeBuilder);

  nodeBuilder = new NodeOperatorVector3NormaliseBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_VECTOR3_NORMALISE, nodeBuilder);

  nodeBuilder = new NodeOperatorVector3DistanceBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_VECTOR3_DISTANCE, nodeBuilder);

  nodeBuilder = new NodeOperatorVector3CrossProductBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_VECTOR3_CROSSPRODUCT, nodeBuilder);

  nodeBuilder = new NodeOperatorRandomFloatBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_RANDOM_FLOAT, nodeBuilder);

  nodeBuilder = new NodeOperatorRayCastBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_RAY_CAST, nodeBuilder);

  nodeBuilder = new NodeOperatorTimeLagInputBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_CP_OP_TIME_LAG_CP, nodeBuilder);
  
  nodeBuilder = new NodeRetargetBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_RETARGET, nodeBuilder);

  nodeBuilder = new NodeScaleCharacterBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_SCALE_CHARACTER, nodeBuilder);
  
  nodeBuilder = new NodeCCOverrideBuilder();
  assetProc.registerNodeBuilder(NODE_TYPE_C_C_OVERRIDE, nodeBuilder);

  //-------------------------------------
  // Initialise transition condition builders.
  TransitConditionDefBuilder* conditionBuilder = new TransitConditionDefBuilderOnMessage();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_ON_MESSAGE_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderDiscreteEventTriggered();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_DISCRETE_EVENT_TRIGGERED_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderCrossedDurationFraction();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_CROSSED_DURATION_FRACTION_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderFalse();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_FALSE_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderControlParamTest();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_CONTROL_PARAM_TEST_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderControlParamInRange();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_CONTROL_PARAM_IN_RANGE_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderControlParamBoolSet();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_CONTROL_PARAM_BOOL_SET_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderCrossedCurveEventValueDecreasing();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_CROSSED_CURVE_EVENT_VALUE_DECREASING_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderInSyncEventRange();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_IN_SYNC_EVENT_RANGE_ID, conditionBuilder);
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_CROSSED_SYNC_EVENT_BOUNDARY_ID, conditionBuilder);
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_IN_SYNC_EVENT_SEGMENT_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderNodeActive();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_NODE_ACTIVE_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderInDurationEvent();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_IN_DURATION_EVENT_ID, conditionBuilder);

  conditionBuilder = new TransitConditionDefBuilderRayHit();
  assetProc.registerTransitConditionDefBuilder(TRANSCOND_RAY_HIT_ID, conditionBuilder);

  //-------------------------------------
  // Initialise transition deadblend builders.
  TransitDeadBlendDefBuilder* deadBlendBuilder = new TransitDeadBlendDefBuilderDefault();
  assetProc.registerTransitDeadBlendDefBuilder(TRANSDEADBLEND_DEFAULT_ID, deadBlendBuilder);

  //---------------------
  // PredictionModel builders.
  PredictionModelBuilder* predictionBuilder = new PredictionModelNDMeshBuilder();
  assetProc.registerPredictionBuilder(PREDICTMODELTYPE_ND_MESH, "PredictionModelNDMesh", predictionBuilder);
}

//----------------------------------------------------------------------------------------------------------------------
void initAnalysisAnalysers(AssetProcessor &assetProc)
{
  AnalysisProcessor *analyser;

  analyser = new ScatterBlendVertexSourceAnalyser();
  assetProc.registerAnalysisProcessor("ScatterBlendVertexSourceAnalyser", analyser);

  analyser = new ScatterBlendTriangleAnnotationAnalyser();
  assetProc.registerAnalysisProcessor("ScatterBlendTriangleAnnotationAnalyser", analyser);

  analyser = new ScatterBlend2DBSPTreeAnalyser();
  assetProc.registerAnalysisProcessor("ScatterBlend2DBSPTreeAnalyser", analyser);

  analyser = new DemoPredictionModel();
  assetProc.registerAnalysisProcessor("DemoPredictionModel", analyser);

  analyser = new PredictionModelNDMeshAnalyser();
  assetProc.registerAnalysisProcessor("PredictionModelNDMesh", analyser);

  analyser = new ScatterBlend1DTask();
  assetProc.registerAnalysisProcessor("ScatterBlend1DTask", analyser);
}

//----------------------------------------------------------------------------------------------------------------------
void initAnalysisConditions(AssetProcessor &assetProc)
{
  AnalysisConditionBuilder* condition;

  condition = new AnalysisConditionAtFrameBuilder;
  assetProc.registerAnalysisConditionBuilder("AnalysisConditionAtFrame", condition);

  condition = new AnalysisConditionCrossedDurationFractionBuilder;
  assetProc.registerAnalysisConditionBuilder("AnalysisConditionCrossedDurationFraction", condition);

  condition = new AnalysisConditionDiscreteEventTriggeredBuilder;
  assetProc.registerAnalysisConditionBuilder("AnalysisConditionDiscreteEventTriggered", condition);

  condition = new AnalysisConditionInSyncEventRangeBuilder;
  assetProc.registerAnalysisConditionBuilder("AnalysisConditionInSyncEventRange", condition);

  condition = new AnalysisConditionStateMachineInStateBuilder;
  assetProc.registerAnalysisConditionBuilder("AnalysisConditionStateMachineInState", condition);
}

//----------------------------------------------------------------------------------------------------------------------
void initAnalysisProperties(AssetProcessor &assetProc)
{
  AnalysisPropertyBuilder* property;

  property = new AnalysisPropertyPositionArcDistanceBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyPositionArcDistance", property);

  property = new AnalysisPropertyPositionDeltaDistanceBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyPositionDeltaDistance", property);

  property = new AnalysisPropertyPositionDeltaInAxisBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyPositionDeltaInAxis", property);

  property = new AnalysisPropertyVelocityInAxisBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyVelocityInAxis", property);

  property = new AnalysisPropertyTurningRateBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyTurningRate", property);

  property = new AnalysisPropertyRotationDeltaInAxisBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyRotationDeltaInAxis", property);

  property = new AnalysisPropertyRateOfTurnWithForwardTravelBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyRateOfTurnWithForwardTravel", property);

  property = new AnalysisPropertyTravelDirectionInPlaneBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyTravelDirectionInPlane", property);

  property = new AnalysisPropertySpeedForCutTurnsBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertySpeedForCutTurns", property);

  property = new AnalysisPropertySpeedInPlaneBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertySpeedInPlane", property);

  property = new AnalysisPropertySpeedBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertySpeed", property);

  property = new AnalysisPropertyTravelSlopeAngleBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyTravelSlopeAngle", property);

  property = new AnalysisPropertyTakeOffDirectionInPlaneBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyTakeOffDirectionInPlane", property);

  property = new AnalysisPropertyPositionValueInAxisBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyPositionValueInAxis", property);

  property = new AnalysisPropertyCustomBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyCustom", property);
  
  property = new AnalysisPropertyStrideLengthBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyStrideLength", property);

  property = new AnalysisPropertyEulerAngleComponentBuilder;
  assetProc.registerAnalysisPropertyBuilder("AnalysisPropertyEulerAngleComponent", property);
}

//----------------------------------------------------------------------------------------------------------------------
void initMessages(AssetProcessor &assetProc)
{
  MessageBuilder *messageBuilder = new MessageScaleCharacterBuilder();
  assetProc.registerMessageBuilder(MESSAGE_TYPE_SCALE_CHARACTER, messageBuilder);
}

//----------------------------------------------------------------------------------------------------------------------
void tidyMessages(AssetProcessor &assetProc)
{
  MessageBuilder *messageBuilder = assetProc.getMessageBuilder(MESSAGE_TYPE_SCALE_CHARACTER);
  if (messageBuilder)
    delete messageBuilder;
}

//----------------------------------------------------------------------------------------------------------------------
ACPluginCore::ACPluginCore(AP::AssetProcessor* assetProc, MR::Manager* manager) :
  AssetCompilerPlugin(assetProc, manager)
{
}

//----------------------------------------------------------------------------------------------------------------------
ACPluginCore::~ACPluginCore()
{
}

//----------------------------------------------------------------------------------------------------------------------
bool ACPluginCore::dislocateAsset(ProcessedAsset* asset)
{
  void* object = asset->assetMemory.ptr;

  switch (asset->assetType)
  {
    case MR::Manager::kAsset_NetworkDef:
      ((MR::NetworkDef*) object)->dislocate();
      break;

    case MR::Manager::kAsset_NetworkPredictionDef:
      ((MR::NetworkPredictionDef*) object)->dislocate();
      break;

    case MR::Manager::kAsset_EventTrackDiscrete:
      ((MR::EventTrackDefDiscrete*) object)->dislocate();
      break;

    case MR::Manager::kAsset_EventTrackDuration:
      ((MR::EventTrackDefDuration*) object)->dislocate();
      break;

    case MR::Manager::kAsset_EventTrackCurve:
      ((MR::EventTrackDefCurve*) object)->dislocate();
      break;

    case MR::Manager::kAsset_RigToAnimMap:
      ((MR::RigToAnimMap*) object)->dislocate();
      break;

    case MR::Manager::kAsset_Rig:
      ((MR::AnimRigDef*)object)->dislocate();
      break;

    case MR::Manager::kAsset_CharacterControllerDef:
      ((MR::CharacterControllerDef*)object)->dislocate();
      break;

    default:
      return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const ProcessedAsset* ACPluginCore::processAsset(
  ME::AssetExport* const sourceAsset,
  MR::Manager::AssetType assetTypeToGenerate)
{
  MR::Manager::AssetType  assetType = MR::Manager::kAsset_Invalid;
  const char*             assetName = 0;
  ME::GUID                guid = 0;
  NMP::Memory::Resource   assetMemory = { 0, NMP::Memory::Format(0, 0) };
  std::string             workingGuid;

  //------------------------------------------
  // Has this asset already been processed (Identified by a pointer to the source asset structure).
  const ProcessedAsset* processedAsset = m_assetProcessor->findProcessedAsset(sourceAsset->getGUID(), assetTypeToGenerate);
  if (processedAsset != 0)
  {
    // add additional dependency if needed
    uint32_t lastNewAsset = m_assetProcessor->getLastNewAssetStackValue();
    if (lastNewAsset)
      processedAsset->parendIDs.insert(lastNewAsset);

    return processedAsset;
  }

  m_assetProcessor->beginNewAssetProcessing();

  //------------------------------------------
  // Create a runtime asset from the source asset.
  // Determines the correct asset type to create by attempting to dynamically cast the source asset to one of the possible types.
  if (dynamic_cast<NetworkDefExport*>(sourceAsset) != 0)
  {
    NetworkDefExport* netDefSource = dynamic_cast<NetworkDefExport*>(sourceAsset);
    assetName = netDefSource->getName();

    // Source asset is a NetworkDef.
    if (assetTypeToGenerate == MR::Manager::kAsset_NetworkDef)
    {
      // Compile a NetworkDef from the source data.
      assetType = MR::Manager::kAsset_NetworkDef;
      guid = netDefSource->getGUID();
      assetMemory = createNetworkDef(netDefSource);
    }
    else
    {
      // Compile a NetworkPredictionDef from the source data.
      NMP_ASSERT(assetTypeToGenerate == MR::Manager::kAsset_Invalid); // We should come through this path prior to compiling the NetworkDef.
      assetType = MR::Manager::kAsset_NetworkPredictionDef;
      
      // We need to make a GUID because the source asset GUID (netDefSource->getGUID()) is the same as that for the NetworkDef.
      m_assetProcessor->makeGUID(workingGuid);
      guid = workingGuid.c_str();

      assetMemory = createNetworkPredictionDef(netDefSource);
      if (assetMemory.ptr == NULL)
        return NULL; // There wasn't any NetworkPrediction data in the source NetworkDef data so we haven't created a NetworkPredictionDef.
    }
  }
  else if (dynamic_cast<RigExport*>(sourceAsset) != 0)
  {
    // Source asset is a Rig.
    RigExport* rigSource = dynamic_cast<RigExport*>(sourceAsset);
    assetType = MR::Manager::kAsset_Rig;
    assetName = rigSource->getName();
    guid = rigSource->getGUID();
    assetMemory = createRig(rigSource);
  }
  else if (dynamic_cast<CharacterControllerExport*>(sourceAsset) != 0)
  {
    // Source asset is a CharacterController.
    CharacterControllerExport* controllerSource = dynamic_cast<CharacterControllerExport*>(sourceAsset);
    assetType = MR::Manager::kAsset_CharacterControllerDef;
    assetName = controllerSource->getName();
    guid = controllerSource->getGUID();
    assetMemory = createCharacterController(controllerSource);
  }
  else if (dynamic_cast<EventTrackExport*>(sourceAsset) != 0)
  {
    // Source asset is an EventTrack (discrete or float).
    EventTrackExport* eventTrackSource = dynamic_cast<EventTrackExport*>(sourceAsset);
    assetName = eventTrackSource->getName();
    guid = eventTrackSource->getGUID();

    switch (eventTrackSource->getEventTrackType())
    {
      case ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE:
        assetType = MR::Manager::kAsset_EventTrackDiscrete;
        assetMemory = createEventTrackDiscrete(eventTrackSource);
        break;

      case ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION:
        assetType = MR::Manager::kAsset_EventTrackDuration;
        assetMemory = createEventTrackDuration(eventTrackSource);
        break;

      case ME::EventTrackExport::EVENT_TRACK_TYPE_CURVE:
        assetType = MR::Manager::kAsset_EventTrackCurve;
        assetMemory = createEventTrackCurve(eventTrackSource);
        break;

      default:
        // Unrecognised event track type.
        return NULL;
    }
  }
  else if (dynamic_cast<AnimationLibraryExport*>(sourceAsset) != 0)
  {
    // Source asset is an AnimationLibrary (Anim viewer asset).
    AnimationLibraryExport* animLibrarySource = dynamic_cast<AnimationLibraryExport*>(sourceAsset);
    processAnimLibrary(animLibrarySource);

    // We don't actually add an asset to the processor in this case, we just want to get the anim.
    return NULL;
  }
  else if (dynamic_cast<AnalysisLibraryExport*>(sourceAsset) != 0)
  {
    // Process analysis library for return to caller.
    // Generally this is creating processed prediction model data that is returned to connect for caching.
    AnalysisLibraryExport* statsLibrary = dynamic_cast<AnalysisLibraryExport*>(sourceAsset);
    processAnalysisLibrary(statsLibrary);

    // We don't actually add an asset to the processor in this case, the data we have created is returned to connect.
    return NULL;
  }
  else
  {
    // Unrecognised asset type.
    m_assetProcessor->cancelNewAssetProcessing();
    return NULL;
  }

  return m_assetProcessor->addAsset(assetMemory, assetType, assetName, guid);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource ACPluginCore::createNetworkDef(NetworkDefExport* netDefExport)
{
  // Process everything that the networkDef is dependent on
  processNetworkDefDependencies(netDefExport);

  // Process the NetworkDef itself.
  NetworkDefBuilderBase* networkDefBuilder = m_assetProcessor->getNetworkDefBuilder();
  NMP_VERIFY(networkDefBuilder);
  NMP::Memory::Resource objectMemory = networkDefBuilder->init(netDefExport, m_assetProcessor);
  return objectMemory;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource ACPluginCore::createNetworkPredictionDef(NetworkDefExport* netDefExport)
{
  // Create the NetworkDef asset first as we may be dependent on it.
  const AP::ProcessedAsset* netDefAsset = m_assetProcessor->doProcessAsset(netDefExport, MR::Manager::kAsset_NetworkDef);

  // Process the NetworkPredictionDef itself.
  NMP::Memory::Resource objectMemory = { NULL, NMP::Memory::Format(0, 0) };
  if (netDefAsset) // If we haven't got a processed NetworkDef then we cant carry on.
  {
    MR::NetworkDef* netDef = static_cast<MR::NetworkDef*>(netDefAsset->assetMemory.ptr);
    NetworkPredictionDefBuilder netPredictionDefBuilder;
    objectMemory = netPredictionDefBuilder.init(m_assetProcessor, netDefExport, netDef);
  }  
  return objectMemory;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource ACPluginCore::createRig(RigExport* rigExport)
{
  NMP::Memory::Resource objectMemory = { NULL, NMP::Memory::Format(0, 0) };
  objectMemory = AnimRigDefBuilder::init(m_assetProcessor, rigExport);
  return objectMemory;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource ACPluginCore::createCharacterController(ME::CharacterControllerExport* characterControllerExport)
{
  NMP::Memory::Resource objectMemory = { NULL, NMP::Memory::Format(0, 0) };
  objectMemory = CharacterControllerDefBuilder::init(m_assetProcessor, characterControllerExport);
  return objectMemory;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource ACPluginCore::createEventTrackDiscrete(ME::EventTrackExport* eventTrackExport)
{
  NMP_VERIFY(eventTrackExport->getEventTrackType() == ME::EventTrackExport::EVENT_TRACK_TYPE_DISCRETE);
  NMP::Memory::Resource objectMemory = { NULL, NMP::Memory::Format(0, 0) };
  objectMemory = EventTrackDefDiscreteBuilder::init(m_assetProcessor, (ME::DiscreteEventTrackExport*)eventTrackExport);
  return objectMemory;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource ACPluginCore::createEventTrackDuration(ME::EventTrackExport* eventTrackExport)
{
  NMP_VERIFY(eventTrackExport->getEventTrackType() == ME::EventTrackExport::EVENT_TRACK_TYPE_DURATION);
  NMP::Memory::Resource objectMemory = { NULL, NMP::Memory::Format(0, 0) };
  objectMemory = EventTrackDefDurationBuilder::init(m_assetProcessor, (ME::DurationEventTrackExport*)eventTrackExport);
  return objectMemory;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource ACPluginCore::createEventTrackCurve(ME::EventTrackExport* eventTrackExport)
{
  NMP_VERIFY(eventTrackExport->getEventTrackType() == ME::EventTrackExport::EVENT_TRACK_TYPE_CURVE);
  NMP::Memory::Resource objectMemory = { NULL, NMP::Memory::Format(0, 0) };
  objectMemory = EventTrackDefCurveBuilder::init(m_assetProcessor, (ME::CurveEventTrackExport*)eventTrackExport);
  return objectMemory;
}

//----------------------------------------------------------------------------------------------------------------------
bool ACPluginCore::processNetworkDefDependencies(ME::NetworkDefExport* netDefExport)
{
  // Before we process the NetworkDef itself we need to make sure that everything
  //  it depends on is processed first.

  //-------------------------------------
  // Process the dependent Rigs, and event tracks from all the dependent AnimationSets first.
  AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  NMP_VERIFY(animLibraryExport);
  for (uint32_t n = 0; n < animLibraryExport->getNumAnimationSets(); ++n)
  {
    AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(n);

    //------------------
    // Physics rig.
    PhysicsRigDefExport* physicsRigDefExport = animSetExport->getPhysicsRigDef();
    if (physicsRigDefExport != 0)
    {
      m_assetProcessor->doProcessAsset(physicsRigDefExport);
    }

    //------------------
    // Character controller definition.
    MR::CharacterControllerDef* characterController = 0;
    CharacterControllerExport* characterControllerExport = animSetExport->getCharacterController();
    if (characterControllerExport != 0)
    {
      const AP::ProcessedAsset* characterControllerAsset = m_assetProcessor->doProcessAsset(characterControllerExport);
      if (characterControllerAsset)
      {
        characterController = (MR::CharacterControllerDef*) characterControllerAsset->assetMemory.ptr;
      }
    }

    //------------------
    // Animation rig.
    MR::AnimRigDef* rig = 0;
    RigExport* rigExport = animSetExport->getRig();
    if (rigExport != 0)
    {
      const AP::ProcessedAsset* rigAsset = m_assetProcessor->doProcessAsset(rigExport);
      rig = (MR::AnimRigDef*) rigAsset->assetMemory.ptr;
    }

    //------------------
    // Body.
    BodyDefExport* bodyDefExport = animSetExport->getBodyDef();
    if (bodyDefExport != 0)
    {
      m_assetProcessor->doProcessAsset(bodyDefExport);
    }

    //------------------
    // Interaction proxy definition.
    InteractionProxyExport* interactionProxy = animSetExport->getInteractionProxy();
    if (interactionProxy != 0)
    {
      m_assetProcessor->doProcessAsset(interactionProxy);
    }

    //------------------
    // Process all event tracks.
    uint32_t numAnims = animSetExport->getNumAnimationEntries();
    for (uint32_t i = 0; i < numAnims; ++i)
    {
      ME::AnimationEntryExport* animEntryExport = animSetExport->getAnimationEntry(i);
      if (!animEntryExport)
        m_assetProcessor->getErrorLogger()->output("Error: The animation entry set of an animation set may not be sparsely populated.\n");
      NMP_VERIFY(animEntryExport); // The animation entry set may not be sparsely populated.

      // We have introduced stripping out animation and their attrib data that are for animation sets 
      // that they cannot be in. This is primarily through the use of retarget nodes although custom nodes could do this.
      // We DON'T want to do this here. This code is used by the prediction which can have different valid animation sets.

      TakeExport* take = animEntryExport->getTakeExportData(m_assetProcessor->getRootDirectory());
      if (take)
      {
        uint32_t numEventTracks = take->getNumEventTracks();
        for (uint32_t k = 0; k < numEventTracks; ++k)
        {
          ME::EventTrackExport* eventTrackExport = take->getEventTrack(k);
          // We don't process empty event tracks, as we don't link them to the animation set anyway
          if (eventTrackExport->getNumEvents() > 0)
          {
            m_assetProcessor->doProcessAsset(eventTrackExport);
          }
        }
      }
    }
  }

  //-------------------------------------
  // Ensure the message presets library is loaded before calling init.
  netDefExport->getMessagePresetLibrary();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ACPluginCore::processAnimLibrary(AnimationLibraryExport* animLibraryExport)
{
  //-------------------------------------
  // Process the dependent Rigs, and event tracks from all the dependent AnimationSets first.
  for (uint32_t n = 0; n < animLibraryExport->getNumAnimationSets(); ++n)
  {
    AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(n);

    PhysicsRigDefExport* physicsRigDefExport = animSetExport->getPhysicsRigDef();
    if (physicsRigDefExport != 0)
    {
      m_assetProcessor->doProcessAsset(physicsRigDefExport);
    }

    MR::AnimRigDef* rig = 0;
    RigExport* rigExport = animSetExport->getRig();
    if (rigExport != 0)
    {
      const AP::ProcessedAsset* rigAsset = m_assetProcessor->doProcessAsset(rigExport);
      rig = (MR::AnimRigDef*) rigAsset->assetMemory.ptr;
    }

    NMP_VERIFY(rig); // An anim set needs a rig.

    // Process all event tracks before the animation set that depends on them.
    uint32_t numAnims = animSetExport->getNumAnimationEntries();
    for (uint32_t i = 0; i < numAnims; ++i)
    {
      ME::AnimationEntryExport* animEntryExport = animSetExport->getAnimationEntry(i);
      if (!animEntryExport)
        m_assetProcessor->getErrorLogger()->output("Error: The animation entry set of an animation set may not be sparsely populated.\n");
      NMP_VERIFY(animEntryExport); // The animation entry set may not be sparsely populated.

      // We have introduced stripping out animation and their attrib data that are for animation sets 
      // that they cannot be in. This is handled by the code :- 
      // networkDefBuilder->initCompileAnimations(animLibraryExport, m_assetProcessor); 
      // called from this function; as well as in the individual node builders.
      // No code is required here.

      TakeExport* take = animEntryExport->getTakeExportData(m_assetProcessor->getRootDirectory());
      if (take)
      {
        uint32_t numEventTracks = take->getNumEventTracks();
        for (uint32_t k = 0; k < numEventTracks; ++k)
        {
          ME::EventTrackExport* eventTrackExport = take->getEventTrack(k);
          // We don't process empty event tracks, as we don't link them to the animation set anyway
          if (eventTrackExport->getNumEvents() > 0)
          {
            m_assetProcessor->doProcessAsset(eventTrackExport);
          }
        }
      }
    }
  }

  //-------------------------
  // Compile all animations and rig to anim maps for this library.
  NetworkDefBuilderBase* networkDefBuilder = m_assetProcessor->getNetworkDefBuilder();
  NMP_VERIFY(networkDefBuilder);
  networkDefBuilder->initCompileAnimations(animLibraryExport, m_assetProcessor);
  networkDefBuilder->tidyCompileAnimations();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool ACPluginCore::processAnalysisLibrary(ME::AnalysisLibraryExport* analysisLibrary)
{
  // create a result analysis library, the file path is specified inside the analysisLibrary
  ME::AnalysisResultLibraryExport* result = analysisLibrary->createResultLibrary();
  bool status = true;

  uint32_t taskCount = analysisLibrary->getNumAnalysisTasks();
  for (uint32_t taskIndex = 0; taskIndex < taskCount; ++taskIndex)
  {
    ME::AnalysisTaskExport* task = analysisLibrary->getAnalysisTask(taskIndex);

    // create a return block for the analysis task to place its result data in
    ME::AnalysisNodeExport* resultAnalysisNode = result->createTaskResult(task->getName());
    ME::DataBlockExport* resultDataBlock = resultAnalysisNode->getDataBlock();

    // find the analyser specified in the task,
    const char* analysisType = task->getAnalysisType();
    AnalysisProcessor* analyser = m_assetProcessor->getAnalysisProcessorForType(analysisType);

    if (analyser)
    {
      if (!analyser->analyse(m_assetProcessor, task, resultDataBlock))
      {
        status = false;
      }
    }
    else
    {
      NMP_VERIFY_FAIL("Unknown analysis type '%s'", analysisType);

      analysisLibrary->destroyResultLibrary(result);
      return false;
    }
  }

  result->write();

  // there is no need to keep the result library around, it is only required by the caller.
  analysisLibrary->destroyResultLibrary(result);

  return status;
}

//----------------------------------------------------------------------------------------------------------------------
bool ACPluginCore::prepareAnimationMetaData(const std::vector<ME::AssetExport*>& sourceAssets, AnimationMetadata** metadata)
{
  // the whole in-progress-containers get cleant up afterwards - which might be messy. they might be either recycled
  // or bound into a seperate data type to do the allocation/clean up process automatically
  std::vector<RigInfo*>                                rigInfoVector;
  std::vector<TrajectoryComponentCompatibleRigsGroup*> compatibleRigGroupsVector;
  std::vector<AnimCompilationInfo*>                    animCompilationVector;
  std::vector<ConglomeratedRigBoneNames*>              conglomeratedRigBoneNamesVector;

  delete *metadata;
  *metadata = NULL;

  // first step: collect data
  for (std::vector<ME::AssetExport*>::const_iterator iteAsset = sourceAssets.begin();
       iteAsset != sourceAssets.end();
       ++iteAsset)
  {
    const ME::NetworkDefExportXML* networkDef = dynamic_cast<const ME::NetworkDefExportXML*>(*iteAsset);
    NMP_VERIFY(networkDef);
    const ME::AnimationLibraryExport* animLibraryExport = networkDef->getAnimationLibrary();
    // process the asset internally to make sure it is ready for operation later on
    // this can't be done from outside of the asset processor framework because it could cause nasty
    // side effects (like trying to write data where no data needs to be written)
    for (uint32_t i = 0; i < animLibraryExport->getNumAnimationSets(); ++i)
    {
      const AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(i);
      NMP_VERIFY(animSetExport);
      const RigExport* rigExport = animSetExport->getRig();
      NMP_VERIFY(rigExport);
      processAsset(const_cast<RigExport*>(rigExport));
    }

    NetworkDefBuilder::buildRigInfoVector(m_assetProcessor, animLibraryExport, rigInfoVector);
    NetworkDefBuilder::buildAnimCompilationInfoVector(m_assetProcessor, animLibraryExport, rigInfoVector, animCompilationVector);
  }

  if (animCompilationVector.size() > 0)
  {
    *metadata = new AnimationMetadata();
    AnimationMetadata* value = *metadata;
    NetworkDefBuilder::buildCompatibleRigGroupsVector(rigInfoVector, compatibleRigGroupsVector);
    NetworkDefBuilder::buildConglomeratedRigBoneNamesVector(animCompilationVector, rigInfoVector, conglomeratedRigBoneNamesVector);

    // create the meta-data
    for (std::vector<AnimCompilationInfo*>::iterator iteAnim = animCompilationVector.begin();
         iteAnim != animCompilationVector.end();
         ++iteAnim)
    {
      AnimCompilationInfo* animInfo = (*iteAnim);
      uint32_t rigBoneNamesIndex = animInfo->m_conglomeratedRigBoneNamesIndex;
      ConglomeratedRigBoneNames* rigBoneNamesEntry = conglomeratedRigBoneNamesVector[rigBoneNamesIndex];
      value->animFiles.push_back(animInfo->m_data);
      value->animRigdata.push_back(rigBoneNamesEntry->m_data);
    }
  }

  NetworkDefBuilder::tidyConglomeratedRigBoneNamesVector(conglomeratedRigBoneNamesVector);
  NetworkDefBuilder::tidyAnimCompilationInfoVector(animCompilationVector);
  NetworkDefBuilder::tidyCompatibleRigGroupsVector(compatibleRigGroupsVector);
  NetworkDefBuilder::tidyRigInfoVector(rigInfoVector);

  return (*metadata) != NULL;
}

//----------------------------------------------------------------------------------------------------------------------
AssetCompilerPlugin* g_pluginCore = NULL;

//----------------------------------------------------------------------------------------------------------------------
void loadPluginCore(const NMP::Memory::Config* config NMP_MEMORY_LOGGING_DECL)
{
#ifdef NMP_MEMORY_LOGGING
  if (config)
    NMP::Memory::init(*config, *NMP_MEMORY_LOGGING_CONFIG_NAME);
  else
    NMP::Memory::init(*NMP_MEMORY_LOGGING_CONFIG_NAME);
#else
  if (config)
    NMP::Memory::init(*config);
  else
    NMP::Memory::init();
#endif
}

//----------------------------------------------------------------------------------------------------------------------
AssetCompilerPlugin* initPluginCore(
  AssetProcessor*  assetProc,
  MR::Manager*     manager,
  uint32_t         NMP_UNUSED(numDispatchers),
  MR::Dispatcher** NMP_UNUSED(dispatchers))
{
  NMP_VERIFY_MSG(
    g_pluginCore == NULL,
    "The asset compiler plugin has already been initialised.");

  // Init the plugin. Note that semantic finalization and task function registration will be
  // performed later after all other plugins have been initialised.
  g_pluginCore = new ACPluginCore(assetProc, manager);

  initBuilders(*assetProc);
  initMessages(*assetProc);
  initAnalysisAnalysers(*assetProc);
  initAnalysisConditions(*assetProc);
  initAnalysisProperties(*assetProc);

  return g_pluginCore;
}

//----------------------------------------------------------------------------------------------------------------------
void deinitialisePluginCore()
{
  if (g_pluginCore)
  {
    delete g_pluginCore;
    g_pluginCore = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void unloadPluginCore()
{
  NMP::Memory::shutdown();
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
