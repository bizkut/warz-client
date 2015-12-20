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
#include "physics/mrPhysicsTasks.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrNetworkLogger.h"
#include "physics/mrPhysicsAttribData.h"

#include "physics/Nodes/mrNodePhysics.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysics.h"
#include "physics/mrPhysicsScene.h"

#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/mrCharacterControllerInterface.h"

#include "NMGeomUtils/NMGeomUtils.h"

#include "NMPlatform/NMProfiler.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Physics TODO: It would be better to just set up the pass through at the stage of queuing the task
void TaskPhysicsUpdateAnimatedTrajectory(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  AttribDataPhysicsSetup* physicsSetup = parameters->getInputAttrib<AttribDataPhysicsSetup>(0, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  if (physicsSetup->m_hasAnimationChild)
  {
    // will only have child deltas if we are not ragdoll
    AttribDataTrajectoryDeltaTransform* childDeltaTransform = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
    AttribDataTrajectoryDeltaTransform* outputDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
    outputDeltaTransform->m_deltaAtt = childDeltaTransform->m_deltaAtt;
    outputDeltaTransform->m_deltaPos = childDeltaTransform->m_deltaPos;
    outputDeltaTransform->m_filteredOut = childDeltaTransform->m_filteredOut;
  }
  else
  {
    AttribDataTrajectoryDeltaTransform* outputDeltaTransform = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(2, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
    outputDeltaTransform->m_filteredOut = true;
    (void)outputDeltaTransform;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPhysicsUpdatePhysicalTrajectoryPostPhysics(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  AttribDataPhysicsRig* physicsRigAttrib = parameters->getInputAttrib<AttribDataPhysicsRig>(0, ATTRIB_SEMANTIC_PHYSICS_RIG);
  AttribDataCharacterProperties* characterController = parameters->getInputAttrib<AttribDataCharacterProperties>(1, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  AttribDataCharacterControllerDef* characterControllerDef = parameters->getInputAttrib<AttribDataCharacterControllerDef>(2, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF);
  AttribDataTrajectoryDeltaTransform* trajOutput = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  // Transition blend weighting (Optional)
  AttribDataBlendWeights* initialisationFractionAttrib = parameters->getInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  AttribDataPhysicsSetup* physicsSetup = parameters->getInputAttrib<AttribDataPhysicsSetup>(5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  PhysicsRig* physicsRig = physicsRigAttrib->m_physicsRig;
  if (!physicsRig)
  {
    return;
  }
  NMP_ASSERT(physicsRig->getPhysicsRigDef());

  NMP::Matrix34 previousWorldRoot = characterController->m_worldRootTransform;
  NMP_ASSERT(previousWorldRoot.isValidTM(0.001f));

  float initialisationFraction = 1.0f;
  if (initialisationFractionAttrib)
  {
    NMP_ASSERT(initialisationFractionAttrib->m_trajectoryAndTransformsNumWeights == 1);
    initialisationFraction = initialisationFractionAttrib->m_trajectoryAndTransformsWeights[0];
  }

  // Since our root offset can change, don't expect the root and previous root position
  // to change in a necessarily "obvious" way
  NMP::Matrix34 worldRoot = physicsRig->getRoot(
    0, initialisationFraction, characterControllerDef->m_characterControllerDef);
  NMP_ASSERT(worldRoot.isValidTM(0.001f));

  NMP::Vector3 worldTrajDelta = worldRoot.translation() - previousWorldRoot.translation();
  // Note that the translation is in the frame of the previous timestep.

  // Translation
  const NMP::Quat prevRot = previousWorldRoot.toQuat();
  const NMP::Quat prevInvRot = ~prevRot;
  trajOutput->m_deltaPos = prevInvRot.rotateVector(worldTrajDelta);

  // If using LocalSK override the trajectory orientation with animation since using the physics
  // orientation leads to feedback.
  if (physicsSetup->m_method == AttribDataPhysicsSetup::METHOD_LOCAL_SK)
  {
    if (physicsSetup->m_hasAnimationChild)
    {
      // Rotation from animation
      AttribDataTrajectoryDeltaTransform* childDeltaTransform = 
        parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(7, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
      trajOutput->m_deltaAtt = childDeltaTransform->m_deltaAtt;
      trajOutput->m_filteredOut = childDeltaTransform->m_filteredOut;
    }
    else
    {
      // This shouldn't ever happen, since the manifest ensures we have an input.
      NMP_ASSERT_FAIL();
      trajOutput->m_filteredOut = true;
    }

  }
  else
  {
    NMP::Quat newRot = worldRoot.toQuat();
    trajOutput->m_deltaAtt.multiply(prevInvRot, newRot);
    trajOutput->m_deltaAtt.normalise();
    trajOutput->m_filteredOut = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void initialisePhysics(
  const PhysicsInitialisation& physicsInitialisation,
  PhysicsRig*                  physicsRig,
  AttribDataPhysicsState*      physicsState,
  Dispatcher*                  dispatcher)
{
  NET_LOG_ENTER_FUNC();

  const NMP::DataBuffer* savedTransforms = physicsInitialisation.m_transforms;
  const NMP::DataBuffer* prevSavedTransforms = physicsInitialisation.m_prevTransforms;

  // enable the rig
  bool bodyOriginallyReferenced = physicsRig->isReferenced();
  if (!physicsState->m_haveAddedPhysicsRigRef)
  {
    physicsRig->addReference();
    physicsState->m_haveAddedPhysicsRigRef = true;
  }

  NMP::Matrix34 worldRoot = physicsInitialisation.m_worldRoot;
  NMP::Matrix34 prevWorldRoot = physicsInitialisation.m_prevWorldRoot;
  // setup the actors.
  // We don't always initialise those bones that we control (taken from our mask) - otherwise if the body was
  // originally ragdoll, and we transition to a weak soft keyframing, there would be snapping of our parts to the input
  // animation. We assume that if some of the body is already controller, it all is being used for the output.
  if (!bodyOriginallyReferenced)
  {
    physicsRig->fromTransformBuffer(
      *savedTransforms,
      *prevSavedTransforms,
      worldRoot,
      prevWorldRoot,
      physicsInitialisation.m_deltaTime,
      dispatcher);
  }
}

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
void subTaskPhysicsUpdateTransformsPrePhysics(
  MR::Dispatcher* dispatcher,
  AttribDataPhysicsRig* physicsRigAttrib,
  AttribDataCharacterProperties* characterController,
  AttribDataPhysicsSetup* physicsSetup,
  AttribDataPhysicsSetupPerAnimSet* physicsSetupPerAnimSet,
  AttribDataPhysicsState* oldPhysicsState,
  AttribDataPhysicsState* physicsState,
  AttribDataFloat* SKWeightInput,
  AttribDataFloat* AAWeightInput,
  AttribDataPhysicsInitialisation* physicsInitialisation,
  AttribDataBoolArray* outputMask,
  AttribDataAnimToPhysicsMap* animToPhysics,
  AttribDataCharacterControllerDef* characterControllerDef,
  AttribDataTransformBuffer* inputTransforms,
  AttribDataTrajectoryDeltaTransform* trajectoryDeltaTransforms)
{
  NET_LOG_ENTER_FUNC();

  PhysicsRig* physicsRig = physicsRigAttrib->m_physicsRig;

  if (oldPhysicsState)
  {
    if (oldPhysicsState->m_previousChildTransforms->m_transformBuffer->getLength() !=
      physicsState->m_previousChildTransforms->m_transformBuffer->getLength())
    {
      // This section takes care of a possible animation set switch. In this case the oldPhysicsState transform buffer could
      // be completely unrelated with the new one, so the safest thing to do is to just use the input transforms.
      inputTransforms->m_transformBuffer->copyTo(physicsState->m_previousChildTransforms->m_transformBuffer);
    }
  }

  if (!physicsRig)
  {
    // Make sure that if we get a physics rig in the future then (a) it gets initialised and (b) we add a reference to it.
    physicsState->m_haveAddedPhysicsRigRef = false;
    return;
  }
  NMP_ASSERT(physicsRig->getPhysicsRigDef());
  
  // This allows behaviour network (which runs before this) to also set sleeping to false
  if (physicsSetupPerAnimSet->m_disableSleeping)
  {
    physicsRig->disableSleeping();
  }

  // On the first update we won't have access to the previous child transforms - needed for
  // soft keyframing and AA - so just fake them
  if (physicsState->m_updateCounter == 0)
  {
    inputTransforms->m_transformBuffer->copyTo(physicsState->m_previousChildTransforms->m_transformBuffer);
  }
  ++physicsState->m_updateCounter;

  if (!physicsState->m_haveAddedPhysicsRigRef)
  {
    // reference added in here
    PhysicsInitialisation pi;
    pi.m_transforms = physicsInitialisation->m_transforms->m_transformBuffer;
    pi.m_prevTransforms = physicsInitialisation->m_prevTransforms->m_transformBuffer;
    pi.m_worldRoot = physicsInitialisation->m_worldRoot;
    pi.m_prevWorldRoot = physicsInitialisation->m_prevWorldRoot;
    pi.m_deltaTime = physicsInitialisation->m_deltaTime;
    initialisePhysics(pi, physicsRig, physicsState, dispatcher);
  }

  if (physicsState->m_needToInitialiseRoot)
  {
    physicsState->m_needToInitialiseRoot = false;
    NMP::Matrix34 originalRoot = characterController->m_worldRootTransform;
    // TODO: Should only do this if we're going to end up controlling the root, investigate furter.
    (void) physicsRig->getRoot(&originalRoot, 0.0f, characterControllerDef->m_characterControllerDef);
  }

  float limitClampFraction = 1.0f;
  float AAWeight = AAWeightInput->m_value;
  float SKWeight = SKWeightInput->m_value;

  PhysicsScene* ps = physicsRig->getPhysicsScene();
  float timeStep = ps ? ps->getNextPhysicsTimeStep() : 1.0f;

  NMP::Matrix34 worldRoot = characterController->m_worldRootTransform;
  NMP::Matrix34 prevWorldRoot = characterController->m_prevWorldRootTransform;

  // Note that if the CC is updated with physics then it won't have been updated yet. In this case
  // we need to extrapolate to estimate the desired CC position. We'll only have been passed
  // trajectoryDeltaTransforms if we're supposed to use them in order to do some form of prediction
  // (i.e. keyframed and using a single physics/character controller update).
  if (
    characterController->m_physicsAndCharacterControllerUpdate != PHYSICS_AND_CC_UPDATE_SEPARATE &&
    characterController->m_physicsAndCharacterControllerUpdate != PHYSICS_AND_CC_UPDATE_COMBINED_NO_PREDICTION
    )
  {
    // Predict the position before orientation since the trajectory delta position is in the space
    // of the original orientation.
    float prevTimeStep = ps->getLastPhysicsTimeStep();
    if (
      characterController->m_physicsAndCharacterControllerUpdate == PHYSICS_AND_CC_UPDATE_COMBINED_USE_NETWORK_TRAJ_DELTA ||
      prevTimeStep == 0.0f)
    {
      // Use the network translation
      const NMP::Vector3 trajDeltaPos = trajectoryDeltaTransforms->m_deltaPos;
      worldRoot.translation() += worldRoot.getRotatedVector(trajDeltaPos);
    }
    else
    {
      // Use an extrapolated position since this copes better with slopes and running into walls etc.
      worldRoot.translation() += (worldRoot.translation() - prevWorldRoot.translation()) * (timeStep / prevTimeStep);
    }

    // Use the animated rotation
    const NMP::Quat trajDeltaRot = trajectoryDeltaTransforms->m_deltaAtt;
    worldRoot.fromQuat(worldRoot.toQuat() * trajDeltaRot);
  }

  PhysicsRig::JointChooser jointChooser(outputMask, physicsRig->getPhysicsRigDef(), animToPhysics->m_animToPhysicsMap);
  PhysicsRig::PartChooser partChooser(outputMask, animToPhysics->m_animToPhysicsMap);

  if (physicsSetup->m_method == AttribDataPhysicsSetup::METHOD_RAGDOLL)
  {
    // ragdoll, so apply some strength to the mid-limit pose
    limitClampFraction = 0.0f;
    AAWeight = 1.0f;
  }
  else if (!physicsSetupPerAnimSet->m_enableJointLimits)
  {
    // Disable clamping of the target to joint limits if they're disabled.
    limitClampFraction = -1.0f;
  }

  physicsRig->requestJointProjectionParameters(
    physicsSetupPerAnimSet->m_jointProjectionIterations, 
    physicsSetupPerAnimSet->m_jointProjectionLinearTolerance, 
    physicsSetupPerAnimSet->m_jointProjectionAngularTolerance);

  if (
    physicsSetup->m_method == AttribDataPhysicsSetup::METHOD_RAGDOLL ||
    physicsSetup->m_method == AttribDataPhysicsSetup::METHOD_AA  ||
    physicsSetup->m_method == AttribDataPhysicsSetup::METHOD_SKAA)
  {
    // Internal compliance is a weighted average of the requested value and the default, which is 1
    float internalCompliance = AAWeight * physicsSetupPerAnimSet->m_activeAnimationInternalCompliance + (1.0f - AAWeight);
    float externalCompliance = physicsSetupPerAnimSet->m_activeAnimationExternalCompliance;

    physicsRig->applyActiveAnimation(
      *inputTransforms->m_transformBuffer,
      *physicsRig->getAnimRigDef()->getBindPose()->m_transformBuffer,
      physicsSetupPerAnimSet->m_activeAnimationStrengthMultiplier * AAWeight,
      physicsSetupPerAnimSet->m_activeAnimationDampingMultiplier,
      internalCompliance,
      externalCompliance,
      physicsSetupPerAnimSet->m_enableJointLimits,
      jointChooser,
      limitClampFraction);
  }

  if (timeStep > 0.0f && (
    physicsSetup->m_method == AttribDataPhysicsSetup::METHOD_SK || 
    physicsSetup->m_method == AttribDataPhysicsSetup::METHOD_LOCAL_SK || 
    physicsSetup->m_method == AttribDataPhysicsSetup::METHOD_SKAA))
  {
    // Always use the authored external compliance even if the weight is zero
    float externalCompliance = physicsSetupPerAnimSet->m_activeAnimationExternalCompliance;

    physicsRig->applySoftKeyframing(
      *inputTransforms->m_transformBuffer,
      *physicsState->m_previousChildTransforms->m_transformBuffer,
      *physicsRig->getAnimRigDef()->getBindPose()->m_transformBuffer,
      worldRoot,
      prevWorldRoot,
      physicsSetup->m_enableCollision,
      physicsSetupPerAnimSet->m_enableJointLimits,
      physicsSetup->m_preserveMomentum,
      externalCompliance,
      physicsSetupPerAnimSet->m_gravityCompensation,
      timeStep,
      SKWeight,
      physicsSetupPerAnimSet->m_softKeyFramingMaxAccel,
      physicsSetupPerAnimSet->m_softKeyFramingMaxAngAccel,
      partChooser);
  }
  if (physicsSetup->m_method == AttribDataPhysicsSetup::METHOD_HK)
  {
    physicsRig->applyHardKeyframing(
      *inputTransforms->m_transformBuffer,
      physicsState->m_previousChildTransforms->m_transformBuffer,
      *physicsRig->getAnimRigDef()->getBindPose()->m_transformBuffer,
      worldRoot,
      &prevWorldRoot,
      physicsSetup->m_enableCollision,
      physicsSetupPerAnimSet->m_massMultiplier,
      physicsSetupPerAnimSet->m_hardKeyframingEnableConstraint,
      timeStep,
      partChooser);
  }

  inputTransforms->m_transformBuffer->copyTo(physicsState->m_previousChildTransforms->m_transformBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace 

//----------------------------------------------------------------------------------------------------------------------
static void updatePhysicalEffectInputs(NodeDef* nodeDef, Network* net)
{
  // If there's a physical effect, it is always pin 2 (specified in the AC)
  const CPConnection* inputCPConnection =  nodeDef->getInputCPConnection(2);
  // We should only get called if there is actually an input
  NMP_ASSERT(inputCPConnection);
  if (inputCPConnection->m_sourceNodeID != INVALID_NODE_ID)
  {
    MR::AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());

    net->updateInputCPConnection(inputCPConnection, animSet);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPhysicsUpdateTransformsPrePhysics(Dispatcher::TaskParameters* parameters)
{
  NM_BEGIN_PROFILING("TaskPhysicsUpdateTransformsPrePhysics");

  NET_LOG_ENTER_FUNC();
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(0, ATTRIB_SEMANTIC_RIG);
  AttribDataPhysicsRig* physicsRigAttrib = parameters->getInputAttrib<AttribDataPhysicsRig>(1, ATTRIB_SEMANTIC_PHYSICS_RIG);
  AttribDataCharacterProperties* characterController = parameters->getInputAttrib<AttribDataCharacterProperties>(2, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  AttribDataBool* outputUpdated = parameters->createOutputAttrib<AttribDataBool>(3, ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS);
  (void)outputUpdated;
  
  AttribDataPhysicsSetup* physicsSetup = parameters->getInputAttrib<AttribDataPhysicsSetup>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  AttribDataPhysicsSetupPerAnimSet* physicsSetupPerAnimSet = parameters->getInputAttrib<AttribDataPhysicsSetupPerAnimSet>(5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);
  AttribDataPhysicsState* oldPhysicsState = parameters->getOptionalInputAttrib<AttribDataPhysicsState>(6, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);
  const uint32_t numRigJoints = rigAttribData->m_rig->getNumBones();
  AttribDataPhysicsStateCreateDesc desc(numRigJoints);
  AttribDataPhysicsState* physicsState = parameters->createOutputAttribReplace<AttribDataPhysicsState>(6, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, &desc);
  NMP_ASSERT(physicsState->m_previousChildTransforms->m_transformBuffer->getLength() == numRigJoints);

  AttribDataPhysicalEffectData* physicalEffectData = 
    parameters->getOptionalInputAttrib<AttribDataPhysicalEffectData>(7, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2);

  AttribDataFloat* SKWeightInput = parameters->getInputAttrib<AttribDataFloat>(8, ATTRIB_SEMANTIC_CP_FLOAT);
  AttribDataFloat* AAWeightInput = parameters->getInputAttrib<AttribDataFloat>(9, ATTRIB_SEMANTIC_CP_FLOAT);
  AttribDataPhysicsInitialisation* physicsInitialisation = parameters->getInputAttrib<AttribDataPhysicsInitialisation>(10, ATTRIB_SEMANTIC_PHYSICS_INITIALISATION);
  AttribDataBoolArray* outputMask = parameters->getInputAttrib<AttribDataBoolArray>(11, ATTRIB_SEMANTIC_OUTPUT_MASK);

  AttribDataAnimToPhysicsMap* animToPhysics = parameters->getInputAttrib<AttribDataAnimToPhysicsMap>(12, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP);

  AttribDataCharacterControllerDef* characterControllerDef = parameters->getInputAttrib<AttribDataCharacterControllerDef>(13, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF);

  AttribDataTransformBuffer* inputTransforms;
  AttribDataTrajectoryDeltaTransform* trajectoryDeltaTransforms = 0;
  int taskIndex = 14;
  if (physicsSetup->m_hasAnimationChild)
  {
    AttribDataTransformBuffer* childTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(taskIndex++, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
    AttribDataTransformBuffer* mergedChildTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(taskIndex++, ATTRIB_SEMANTIC_MERGED_PHYSICS_RIG_TRANSFORM_BUFFER);
    inputTransforms = mergedChildTransforms;
    if (physicsSetup->m_rootControlMethod == Network::ROOT_CONTROL_PHYSICS || physicsSetup->m_useRootAsAnchor)
    {
      inputTransforms = childTransforms;
    }
  }
  else
  {
    inputTransforms = rigAttribData->m_rig->getBindPose();
  }

  if (
    physicsSetup->m_rootControlMethod != Network::ROOT_CONTROL_PHYSICS && 
    characterController->m_physicsAndCharacterControllerUpdate != PHYSICS_AND_CC_UPDATE_SEPARATE
    )
  {
    trajectoryDeltaTransforms = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(taskIndex++, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  }

  subTaskPhysicsUpdateTransformsPrePhysics(
    parameters->m_dispatcher,
    physicsRigAttrib,
    characterController,
    physicsSetup,
    physicsSetupPerAnimSet,
    oldPhysicsState,
    physicsState,
    SKWeightInput,
    AAWeightInput,
    physicsInitialisation,
    outputMask,
    animToPhysics,
    characterControllerDef,
    inputTransforms,
    trajectoryDeltaTransforms);

  // Now call the performance functions
  if (physicalEffectData)
    updatePhysicalEffectInputs(physicalEffectData->m_nodeDef, physicalEffectData->m_network);

  NM_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void TaskPhysicsUpdateTransformsPostPhysics(Dispatcher::TaskParameters* parameters)
{
  NM_BEGIN_PROFILING("TaskPhysicsUpdateTransformsPostPhysics");
  NET_LOG_ENTER_FUNC();
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(0, ATTRIB_SEMANTIC_RIG);
  AttribDataPhysicsRig* physicsRigAttrib = parameters->getInputAttrib<AttribDataPhysicsRig>(1, ATTRIB_SEMANTIC_PHYSICS_RIG);
  AttribDataCharacterProperties* characterController = parameters->getInputAttrib<AttribDataCharacterProperties>(2, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  const uint32_t numRigJoints = rigAttribData->m_rig->getNumBones();
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(3, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataPhysicsSetup* physicsSetup = parameters->getInputAttrib<AttribDataPhysicsSetup>(6, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // copy the non-physical transforms - from the input if there is one
  AttribDataTransformBuffer* inputTransforms;
  if (physicsSetup->m_hasAnimationChild)
    inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(8, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  else
    inputTransforms = rigAttribData->m_rig->getBindPose();
  inputTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);

  PhysicsRig* physicsRig = physicsRigAttrib->m_physicsRig;
  if (!physicsRig)
  {
    return;
  }
  NMP_ASSERT(physicsRig->getPhysicsRigDef());

  // Overwrite the physical transforms.
  // Always use the same root for going from world to local space as the application will use going
  // the other way.
  if (!physicsSetup->m_outputSourceAnimation)
  {
    NMP::Matrix34 worldRoot = characterController->m_worldRootTransform;
    physicsRig->toTransformBuffer(*outputTransforms->m_transformBuffer, worldRoot, 0);
  }
  NM_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void nodePhysicsDeleteInstance(const NodeDef* node, Network* net)
{
  NET_LOG_MESSAGE(99, "Deleting physics node %s\n", node->getName());

  PhysicsRig* physicsRig = getPhysicsRig(net);
  if (physicsRig)
  {
    // We don't need to restore any properties because next time a physics node takes over it will
    // call addReference which will restore default properties on the whole character.
    // All the PhysicsRig::apply* functions should set all the properties they need to each
    // update - i.e. not rely on them getting maintained from update to update.

    // remove the body reference if we can find it
    NodeBinEntry* attribEntry = net->getAttribDataNodeBinEntry(
                                  ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                                  node->getNodeID(),
                                  INVALID_NODE_ID,
                                  VALID_FRAME_ANY_FRAME);
    // Check if the attrib still exists.
    if (attribEntry)
    {
      AttribDataPhysicsState* physicsState = attribEntry->getAttribData<AttribDataPhysicsState>();
      NMP_ASSERT(physicsState);

      if (physicsState->m_haveAddedPhysicsRigRef)
        physicsRig->removeReference();
    }
  }
  nodeShareDeleteInstanceWithChildren(node, net);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskApplyPhysicsJointLimitsTransforms(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  AttribDataTransformBuffer* inputTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataPhysicsRig* physicsRigAttrib = parameters->getInputAttrib<AttribDataPhysicsRig>(1, ATTRIB_SEMANTIC_PHYSICS_RIG);

  const uint32_t numRigJoints = inputTransforms->m_transformBuffer->getLength();
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(2, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  inputTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);

  if (!physicsRigAttrib->m_physicsRig)
    return;

  NMP::Quat* qOut = outputTransforms->m_transformBuffer->getChannelQuat(0);

  for (uint32_t i = 0; i < numRigJoints; ++i)
  {
    if (outputTransforms->m_transformBuffer->getUsedFlags()->isBitSet(i))
      qOut[i] = physicsRigAttrib->m_physicsRig->getLimitedJointQuat(i, qOut[i], 1.0f);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskSetNonPhysicsTransforms(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  AttribDataAnimToPhysicsMap* animToPhysics = parameters->getInputAttrib<AttribDataAnimToPhysicsMap>(0, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP);
  AttribDataTransformBuffer* inputPhysicsTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* inputNonPhysicsTransforms = parameters->getInputAttrib<AttribDataTransformBuffer>(2, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  NMP_ASSERT(inputPhysicsTransforms->m_transformBuffer->getLength() == inputNonPhysicsTransforms->m_transformBuffer->getLength());
  const uint32_t numRigJoints = inputNonPhysicsTransforms->m_transformBuffer->getLength();
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(3, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // First copy all the physics channels
  inputPhysicsTransforms->m_transformBuffer->copyTo(outputTransforms->m_transformBuffer);

  // Then replace any non-physics channels with non-physics input, so long as it
  // exists for that channel.
  for (uint32_t channel = 0; channel < numRigJoints; ++channel)
  {
    int32_t iPhysicsJoint = animToPhysics->m_animToPhysicsMap->getPhysicsIndexFromAnimIndex(channel);
    bool hasNonPhysicsChannel = inputNonPhysicsTransforms->m_transformBuffer->hasChannel(channel);
    if (iPhysicsJoint == -1 && hasNonPhysicsChannel)
    {
      outputTransforms->m_transformBuffer->setChannelPos(
        channel, *inputNonPhysicsTransforms->m_transformBuffer->getChannelPos(channel));
      outputTransforms->m_transformBuffer->setChannelQuat(
        channel, *inputNonPhysicsTransforms->m_transformBuffer->getChannelQuat(channel));
      outputTransforms->m_transformBuffer->setChannelUsed(channel);
    }
  }

  outputTransforms->m_transformBuffer->calculateFullFlag(); 
}

//----------------------------------------------------------------------------------------------------------------------
static void subTaskExpandLimitsToAccomodateTransform(
  Dispatcher::TaskParameters* parameters,
  MR::AttribDataSemantic      transformSemantic)
{
  NET_LOG_ENTER_FUNC();

  AttribDataTransformBuffer* const transformBufferAttrib =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, transformSemantic);
  NMP_ASSERT(transformBufferAttrib);

  const AttribDataBoolArray* outputMask =
    parameters->getInputAttrib<AttribDataBoolArray>(1, ATTRIB_SEMANTIC_OUTPUT_MASK);
  NMP_ASSERT(outputMask);

  const AttribDataPhysicsRig* const physicsRigAttrib =
    parameters->getInputAttrib<AttribDataPhysicsRig>(2, ATTRIB_SEMANTIC_PHYSICS_RIG);
  NMP_ASSERT(physicsRigAttrib);

  AttribDataAnimToPhysicsMap* animToPhysics =
    parameters->getInputAttrib<AttribDataAnimToPhysicsMap>(3, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP);

  const uint32_t numOutputTransforms = transformBufferAttrib->m_transformBuffer->getLength();
  AttribDataTransformBuffer* outputTransformBufferAttrib = parameters->createOutputAttribTransformBuffer(4, numOutputTransforms, transformSemantic);
  transformBufferAttrib->m_transformBuffer->copyTo(outputTransformBufferAttrib->m_transformBuffer);

  if (animToPhysics)
  {
    MR::PhysicsRig* const physicsRig = physicsRigAttrib->m_physicsRig;
    MR::AnimToPhysicsMap* const animToPhysicsMap = animToPhysics->m_animToPhysicsMap;

    if (physicsRig)
    {
      const uint32_t numPhysJoints = physicsRig->getNumJoints();

      for (uint32_t physJointIndex = 0; physJointIndex < numPhysJoints; ++physJointIndex)
      {
        // Get physics joint
        const PhysicsJointDef* jointDef = physicsRig->getPhysicsRigDef()->m_joints[physJointIndex];

        // Find relative orientation of the child anim part to the parent anim part.
        const int32_t animPartIndex = animToPhysicsMap->getAnimIndexFromPhysicsIndex(jointDef->m_childPartIndex);

        NMP_ASSERT(animPartIndex < static_cast< int32_t >(outputMask->m_numValues));

        // Ignore joints whose child part is not in the output mask.
        if ((animPartIndex < static_cast< int32_t >(outputMask->m_numValues)) && (outputMask->m_values[animPartIndex]))
        {
          const NMP::Quat* const animPartOri = transformBufferAttrib->m_transformBuffer->getChannelQuatFast(animPartIndex);
          NMP_ASSERT(animPartOri); // null check

          // Expand physics joint to accommodate transform.
          physicsRig->expandJointLimits(physJointIndex, *animPartOri);
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskExpandLimitsTransforms(Dispatcher::TaskParameters* parameters)
{
  subTaskExpandLimitsToAccomodateTransform(parameters, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskExpandLimitsDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  subTaskExpandLimitsToAccomodateTransform(parameters, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
