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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_ERNODES_H
#define NM_ERNODES_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrManager.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "mrPhysicsScenePhysX3.h"
#include "euphoria/Nodes/erNodeBehaviour.h"
#include "euphoria/Nodes/erNodeLimbInfo.h"
#include "euphoria/Nodes/erNodeBehaviourGrouper.h"
#include "euphoria/Nodes/erNodeBehaviour.h"
#include "euphoria/Nodes/erNodePerformanceBehaviour.h"
#include "euphoria/Nodes/erNodeOperatorHit.h"
#include "euphoria/Nodes/erNodeOperatorContactReporter.h"
#include "euphoria/Nodes/erNodeOperatorOrientationInFreeFall.h"
#include "euphoria/Nodes/erNodeOperatorRollDownStairs.h"
#include "euphoria/Nodes/erNodeOperatorPhysicalConstraint.h"
#include "euphoria/Nodes/erNodeTrajectoryOverride.h"
#include "euphoria/Nodes/erNodeOperatorFallOverWall.h"
#include "euphoria/erBodyDef.h"
#include "euphoria/erContactFeedback.h"
#include "euphoria/erEuphoriaUserData.h"
#ifdef NM_HOST_CELL_PPU
  #include "euphoria/erSPU.h"
#endif
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
void TaskBehaviourUpdateTransformsPrePhysics(MR::Dispatcher::TaskParameters* parameters);
void TaskBehaviourUpdatePhysicalTrajectoryPostPhysics(MR::Dispatcher::TaskParameters* parameters);
void TaskBehaviourUpdateAnimatedTrajectory(MR::Dispatcher::TaskParameters* parameters);
void TaskBehaviourUpdateTransformsPostPhysics(MR::Dispatcher::TaskParameters* parameters);
void TaskLimbInfoUpdateTrajectoryDeltaAndTransforms(MR::Dispatcher::TaskParameters* parameters);
void TaskLimbInfoUpdateTransforms(MR::Dispatcher::TaskParameters* parameters);

void TaskPerformanceBehaviourUpdateTransformsPrePhysics(MR::Dispatcher::TaskParameters* parameters);

void TaskTrajectoryOverrideUpdateTrajectoryDelta(MR::Dispatcher::TaskParameters* parameters);

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void registerEuphoriaAssets()
{
  MR::Manager& manager = MR::Manager::getInstance();

  manager.registerAsset(MR::Manager::kAsset_BodyDef, locateBodyDef);
  manager.registerAsset(MR::Manager::kAsset_InteractionProxyDef, locateInteractionProxyDef);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void registerEuphoriaQueuingFnsAndOutputCPTasks()
{
  MR::Manager& manager = MR::Manager::getInstance();

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBehaviourQueueUpdateTransformsPrePhysics));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBehaviourQueueUpdateTransformsPostPhysics));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeBehaviourQueueUpdateTrajectory));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePerformanceBehaviourQueueUpdateTransformsPrePhysics));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeLimbInfoQueueTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeLimbInfoQueueTrajectoryDeltaAndTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeTrajectoryOverrideQueueUpdate));

  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeBehaviourFindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeBehaviourGrouperFindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodeTrajectoryOverrideFindGeneratingNodeForSemantic));

  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeBehaviourUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodePerformanceBehaviourUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodeTrajectoryOverrideUpdateConnections));

  manager.registerDeleteNodeInstanceFn(REG_FUNC_ARGS(nodeBehaviourDeleteInstance));
  manager.registerDeleteNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorPhysicalConstraintOutputDeleteInstance));

  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeBehaviorOutputCPUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeBehaviourEmitMessageUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorHitOutputCPUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorFallOverWallEmitMessageUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorContactReporterOutputCPUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorOrientationInFreeFallOutputCPUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorRollDownStairsOutputCPUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorPhysicalConstraintOutputCPUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorFallOverWallOutputCPUpdate));

  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorHitInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorContactReporterInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorOrientationInFreeFallInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorFallOverWallInitInstance));

  //-------------
  // Message Handlers
  manager.registerMessageDataType(MESSAGE_TYPE_HIT, &HitMessageData::locate, &HitMessageData::dislocate);
  manager.registerMessageHandlerFn(REG_FUNC_ARGS(nodeOperatorHitMessageHandler));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool registerNMBehaviourTasks(MR::Dispatcher* dispatcher)
{
#ifndef NM_HOST_CELL_SPU
  dispatcher->registerTask(TASK_NAME_ARG(TaskBehaviourUpdateTransformsPrePhysics),
    MR::CoreTaskIDs::MR_TASKID_BEHAVIOURUPDATETRANSFORMSPREPHYSICS);
  dispatcher->registerTask(TASK_NAME_ARG(TaskBehaviourUpdatePhysicalTrajectoryPostPhysics),
    MR::CoreTaskIDs::MR_TASKID_BEHAVIOURUPDATEPHYSICALTRAJECTORYPOSTPHYSICS);
  dispatcher->registerTask(TASK_NAME_ARG(TaskBehaviourUpdateAnimatedTrajectory),
    MR::CoreTaskIDs::MR_TASKID_BEHAVIOURUPDATEANIMATEDTRAJECTORY);
  dispatcher->registerTask(TASK_NAME_ARG(TaskBehaviourUpdateTransformsPostPhysics),
    MR::CoreTaskIDs::MR_TASKID_BEHAVIOURUPDATETRANSFORMSPOSTPHYSICS);
  dispatcher->registerTask(TASK_NAME_ARG(TaskPerformanceBehaviourUpdateTransformsPrePhysics),
    MR::CoreTaskIDs::MR_TASKID_PERFORMANCEBEHAVIOURUPDATETRANSFORMSPREPHYSICS);
  dispatcher->registerTask(TASK_NAME_ARG(TaskLimbInfoUpdateTrajectoryDeltaAndTransforms),
    MR::CoreTaskIDs::MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_LIMBINFO);
  dispatcher->registerTask(TASK_NAME_ARG(TaskLimbInfoUpdateTransforms),
    MR::CoreTaskIDs::MR_TASKID_PASSTHROUGHTRANSFORMS_LIMBINFO);
  dispatcher->registerTask(TASK_NAME_ARG(TaskTrajectoryOverrideUpdateTrajectoryDelta),
    MR::CoreTaskIDs::MR_TASKID_TRAJECTORYOVERRIDE);
#endif // NM_HOST_CELL_SPU

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void registerEuphoriaAttribDataTypes()
{
  MR::Manager& manager = MR::Manager::getInstance();

  // AttribDataBehaviourSetup
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BEHAVIOUR_SETUP),
    NMP_NULL_ON_SPU(AttribDataBehaviourSetup::locate),
    NMP_NULL_ON_SPU(AttribDataBehaviourSetup::dislocate));

  // BodyDef
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BODY_DEF),
    NMP_NULL_ON_SPU(AttribDataBodyDef::locate),
    NMP_NULL_ON_SPU(AttribDataBodyDef::dislocate));

  // Interaction Proxy
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_INTERACTION_PROXY_DEF),
    NMP_NULL_ON_SPU(AttribDataInteractionProxyDef::locate),
    NMP_NULL_ON_SPU(AttribDataInteractionProxyDef::dislocate));

  // Custom Physics
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_CHARACTER),
    NMP_NULL_ON_SPU(AttribDataCharacter::locate),
    NMP_NULL_ON_SPU(AttribDataCharacter::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_BEHAVIOUR_PARAMETERS),
    NMP_NULL_ON_SPU(AttribDataBehaviourParameters::locate),
    NMP_NULL_ON_SPU(AttribDataBehaviourParameters::dislocate),
    NULL,
    AttribDataBehaviourParameters::relocate);

  manager.registerAttrDataType(REG_FUNC_ARGS(ATTRIB_TYPE_OPERATOR_HIT_STATE));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_OPERATOR_HIT_DEF),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorHitDef::locate),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorHitDef::dislocate));

  // hit node state data
  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_OPERATOR_CONTACTREPORTER_DEF),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorContactReporterDef::locate),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorContactReporterDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_OPERATOR_ROLLDOWNSTAIRS_DEF),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorRollDownStairsDef::locate),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorRollDownStairsDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_OPERATOR_FALLOVERWALL_DEF),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorFallOverWallDef::locate),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorFallOverWallDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_OPERATOR_FALLOVERWALL_STATE),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorFallOverWallState::locate),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorFallOverWallState::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_OPERATOR_PHYSICALCONSTRAINT_DEF),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorPhysicalConstraintDef::locate),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorPhysicalConstraintDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_OPERATOR_PHYSICALCONSTRAINT_STATE));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_OPERATOR_ORIENTATIONINFREEFALL_DEF),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorOrientationInFreeFallDef::locate),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorOrientationInFreeFallDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_OPERATOR_ORIENTATIONINFREEFALL_STATE),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorOrientationInFreeFallState::locate),
    NMP_NULL_ON_SPU(ER::AttribDataOperatorOrientationInFreeFallState::dislocate));
}

//----------------------------------------------------------------------------------------------------------------------
/// Register the euphoria attribute semantics.
NM_INLINE void registerEuphoriaAttribSemantics(
  bool computeRegistryRequirements ///< true:  Determines required array sizes for storage of registered functions, data types etc.
                                   ///< false: Registers functions, data types etc.
  )
{
  MR::Manager& manager = MR::Manager::getInstance();

  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_BODY_DEF), MR::ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_INTERACTION_PROXY_DEF), MR::ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_CHARACTER), MR::ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_BEHAVIOUR_PARAMETERS), MR::ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_DEFAULT_POSE), MR::ATTRIB_SENSE_NONE);
  manager.registerAttributeSemanticID(REG_FUNC_ARGS_COMPUTE(ATTRIB_SEMANTIC_GUIDE_POSE), MR::ATTRIB_SENSE_NONE);
}

//----------------------------------------------------------------------------------------------------------------------
/// Helper functions providing simple initialization of the euphoria module. All the API used is
/// public so the contents of this function can be called directly for finer control over initialization.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \brief Registers task queuing functions, attribute data types etc with Manager. Accumulates
///        manager registry requirements etc.
/// manager is an optional parameter to be used if bringing a manager in across a dll boundary
NM_INLINE void initEuphoria(uint32_t numDispatchers, MR::Dispatcher** dispatchers)
{
  // Register the task queuing functions and attribute data types etc.
  ER::registerEuphoriaQueuingFnsAndOutputCPTasks();
  ER::registerEuphoriaAttribDataTypes();
  ER::registerEuphoriaAssets();

#ifdef NM_HOST_CELL_PPU
  ER::SPU::init();
#endif // NM_HOST_CELL_PPU

  for (uint32_t i = 0; i < numDispatchers; ++i)
    ER::registerNMBehaviourTasks(dispatchers[i]);

  MR::PhysXPerShapeData::initialiseMap();
  ER::EuphoriaRigPartUserData::initialiseMap();

  // Accumulate any semantic registration requirements in the manager.
  ER::registerEuphoriaAttribSemantics(true);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Registration of semantics etc with the manager, post Manager::allocateRegistry().
NM_INLINE void finaliseInitEuphoria()
{
  // Actually register each semantic with the manager.
  ER::registerEuphoriaAttribSemantics(false);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Destroy static structures used by euphoria
NM_INLINE void destroyEuphoria()
{
  MR::PhysXPerShapeData::destroyMap();
  ER::EuphoriaRigPartUserData::destroyMap();
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_ERNODES_H
//----------------------------------------------------------------------------------------------------------------------
