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
#include "physics/Nodes/mrPhysicsNodes.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void registerPhysicsAssets()
{
  Manager& manager = Manager::getInstance();

  NMP_USED_FOR_VERIFY(AssetLocateFn physicsRigDefLocate =)
    manager.getAssetLocateFn(MR::Manager::kAsset_PhysicsRigDef);
  NMP_VERIFY_MSG(physicsRigDefLocate, "Physics driver specific PhysicsRigDef locate function must be registered before calling this function.");
}

//----------------------------------------------------------------------------------------------------------------------
void registerPhysicsQueuingFnsAndOutputCPTasks()
{
  Manager& manager = Manager::getInstance();

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePhysicsQueueUpdateTrajectory));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePhysicsQueueUpdateAnimatedTrajectory));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePhysicsQueueUpdatePhysicalTrajectoryPostPhysics));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePhysicsQueueUpdateTransformsPrePhysics));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePhysicsQueueUpdateTransformsPostPhysics));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeSetNonPhysicsTransformsQueueSetNonPhysicsTransforms));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePhysicsGrouperQueueUpdateTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePhysicsGrouperQueueUpdateTrajectory));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePhysicsGrouperQueueGetOutputMask));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePhysicsGrouperQueueUpdateTransformsPrePhysics));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeApplyPhysicsJointLimitsQueueTransforms));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePhysicsTransitQueueTransformBuffs));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodePhysicsTransitSyncEventsQueueTransformBuffs));

  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeExpandLimitsQueueTransforms));
  manager.registerTaskQueuingFn(REG_FUNC_ARGS(nodeExpandLimitsQueueTrajectoryDeltaAndTransforms));

  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodePhysicsFindGeneratingNodeForSemantic));
  manager.registerFindGeneratingNodeForSemanticFn(REG_FUNC_ARGS(nodePhysicsGrouperFindGeneratingNodeForSemantic));

  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorPhysicsInfoOutputCPUpdate));
  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeControlParamPhysicsObjectPointerEmittedCPUpdatePhysicsObjectPointer));

  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodePhysicsUpdateConnections));
  manager.registerUpdateNodeConnectionsFn(REG_FUNC_ARGS(nodePhysicsGrouperUpdateConnections));

  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeOperatorPhysicsInfoInitInstance));
  manager.registerInitNodeInstanceFn(REG_FUNC_ARGS(nodeControlParamPhysicsObjectPointerInitInstance));

  manager.registerOutputCPTask(REG_FUNC_ARGS(nodeOperatorApplyImpulseOutputCPUpdate));

  manager.registerDeleteNodeInstanceFn(REG_FUNC_ARGS(nodePhysicsDeleteInstance));
}

//----------------------------------------------------------------------------------------------------------------------
void registerPhysicsTransitConditions()
{
  Manager& manager = Manager::getInstance();

  manager.registerTransitCondType(
    TRANSCOND_GROUND_CONTACT_ID,
    TransitConditionDefGroundContact::defLocate,
    TransitConditionDefGroundContact::defDislocate,
    NULL,
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefGroundContact::instanceInit,
    TransitConditionDefGroundContact::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);

  manager.registerTransitCondType(
    TRANSCOND_PHYSICS_AVAILABLE_ID,
    TransitConditionDefPhysicsAvailable::defLocate,
    TransitConditionDefPhysicsAvailable::defDislocate,
    NULL,
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefPhysicsAvailable::instanceInit,
    TransitConditionDefPhysicsAvailable::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);

  manager.registerTransitCondType(
    TRANSCOND_PHYSICS_IN_USE_ID,
    TransitConditionDefPhysicsInUse::defLocate,
    TransitConditionDefPhysicsInUse::defDislocate,
    NULL,
    TransitConditionDef::defaultInstanceGetMemoryRequirements,
    TransitConditionDefPhysicsInUse::instanceInit,
    TransitConditionDefPhysicsInUse::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDef::defaultInstanceReset);

  manager.registerTransitCondType(
    TRANSCOND_PHYSICS_MOVING_ID,
    TransitConditionDefPhysicsMoving::defLocate,
    TransitConditionDefPhysicsMoving::defDislocate,
    NULL,
    TransitConditionDefPhysicsMoving::instanceGetMemoryRequirements,
    TransitConditionDefPhysicsMoving::instanceInit,
    TransitConditionDefPhysicsMoving::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDefPhysicsMoving::instanceReset);

  manager.registerTransitCondType(
    TRANSCOND_SK_DEVIATION_ID,
    TransitConditionDefSKDeviation::defLocate,
    TransitConditionDefSKDeviation::defDislocate,
    NULL,
    TransitConditionDefSKDeviation::instanceGetMemoryRequirements,
    TransitConditionDefSKDeviation::instanceInit,
    TransitConditionDefSKDeviation::instanceUpdate,
    TransitConditionDef::defaultInstanceQueueDeps,
    TransitConditionDefSKDeviation::instanceReset);
}

//----------------------------------------------------------------------------------------------------------------------
void registerPhysicsAttribDataTypes()
{
  Manager& manager = Manager::getInstance();

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PHYSICS_RIG));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PHYSICS_RIG_DEF),
    NMP_NULL_ON_SPU(AttribDataPhysicsRigDef::locate),
    NMP_NULL_ON_SPU(AttribDataPhysicsRigDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_ANIM_TO_PHYSICS_MAP),
    NMP_NULL_ON_SPU(AttribDataAnimToPhysicsMap::locate),
    NMP_NULL_ON_SPU(AttribDataAnimToPhysicsMap::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_TRAJECTORY_OVERRIDE_DEF),
    NMP_NULL_ON_SPU(AttribDataTrajectoryOverrideDefAnimSet::locate),
    NMP_NULL_ON_SPU(AttribDataTrajectoryOverrideDefAnimSet::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_OPERATOR_APPLYIMPULSE_DEF),
    NMP_NULL_ON_SPU(MR::AttribDataOperatorApplyImpulseDef::locate),
    NMP_NULL_ON_SPU(MR::AttribDataOperatorApplyImpulseDef::dislocate));

  manager.registerAttrDataType(
    REG_FUNC_ARGS(ATTRIB_TYPE_PHYSICS_OBJECT_POINTER),
    NMP_NULL_ON_SPU(AttribDataPhysicsInfoDef::locate),
    NMP_NULL_ON_SPU(AttribDataPhysicsInfoDef::dislocate),
    NULL,
    NULL,
    NULL,
    MR_NULL_NO_OUTPUT_DEBUGGING(AttribDataPhysicsObjectPointer::serializeTx));
}

//----------------------------------------------------------------------------------------------------------------------
void initMorphemePhysics(uint32_t numDispatchers, Dispatcher** dispatchers)
{
  // Register the task queuing functions, attribute data types etc.
  MR::registerPhysicsQueuingFnsAndOutputCPTasks();
  MR::registerPhysicsAttribDataTypes();
  MR::registerPhysicsTransitConditions();
  MR::registerPhysicsAssets();

  // Register a callback so the physics core knows when a network's anim set changes.
  MR::Network::setActiveAnimSetChangedCallback(MR::physicsOnAnimSetChanged);

  // Register the task functions with the dispatchers
  for (uint32_t i = 0; i < numDispatchers; ++i)
    MR::CoreTaskIDs::registerNMPhysicsTasks(dispatchers[i]);

  // Accumulate any semantic registration requirements in the manager.
  MR::registerPhysicsAttribSemantics(true);
}

//----------------------------------------------------------------------------------------------------------------------
void finaliseInitMorphemePhysics()
{
  // Actually register each semantic with the manager.
  MR::registerPhysicsAttribSemantics(false);
}

}  // namespace MR

//----------------------------------------------------------------------------------------------------------------------
