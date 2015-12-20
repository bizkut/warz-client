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
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrNetworkLogger.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "physics/mrPhysicsAttribData.h"

#include "physics/mrPhysics.h"
#include "physics/mrPhysicsScene.h"
#include "physics/mrPhysicsTasks.h"

#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/mrCharacterControllerInterface.h"
#include "euphoria/erAttribData.h"
#include "euphoria/erBehaviour.h"
#include "euphoria/Nodes/erNodes.h"
#include "euphoria/erBody.h"

#include "NMPlatform/NMProfiler.h"
//----------------------------------------------------------------------------------------------------------------------

using namespace MR;

namespace ER
{

#ifdef NM_PROFILING
static void StartProfiling(int behaviourID)
{
  // Profiling macro requires a literal string, since it's a preprocessor macro
  // see NetworkManifest::Behaviours in Network/NetworkDescriptor.h
  switch (behaviourID)
  {
  case  1: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics  1 Aim"); break;
  case  2: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics  2 Animate"); break;
  case  3: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics  3 ArmBrace"); break;
  case  4: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics  4 ArmsPlacement"); break;
  case  5: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics  5 ArmsWindmill"); break;
  case  6: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics  6 Balance"); break;
  case  7: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics  7 BalancePoser"); break;
  case  8: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics  8 Characteristics"); break;
  case  9: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics  9 EuphoriaRagdoll"); break;
  case 10: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 10 Eyes"); break;
  case 11: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 11 FreeFall"); break;
  case 12: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 12 HazardAwareness"); break;
  case 13: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 13 HeadAvoid"); break;
  case 14: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 14 HeadDodge"); break;
  case 15: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 15 Hold"); break;
  case 16: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 16 HoldAction"); break;
  case 17: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 17 IdleAwake"); break;
  case 18: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 18 LegsPedal"); break;
  case 19: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 19 Look"); break;
  case 20: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 20 Observe"); break;
  case 21: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 21 Properties"); break;
  case 22: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 22 Protect"); break;
  case 23: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 23 ReachForBody"); break;
  case 24: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 24 ReachForWorld"); break;
  case 25: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 25 Shield"); break;
  case 26: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 26 ShieldAction"); break;
  case 27: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 27 Sit"); break;
  case 28: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 28 UserHazard"); break;
  case 29: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics 29 Writhe"); break;
  default: NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics ???"); break;
  }
}
#endif

// Timings in comments below are for XBOX running the EuphoriaRagdoll state only
#ifdef NM_PROFILING
  #define SHOW_BEHAVIOUR_ID_IN_PROFILE_OUTPUT
#endif

//----------------------------------------------------------------------------------------------------------------------
void TaskBehaviourUpdateTransformsPrePhysics(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
#ifndef SHOW_BEHAVIOUR_ID_IN_PROFILE_OUTPUT
  NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPrePhysics");
#endif
  uint32_t taskIndex = 0;
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(taskIndex++, ATTRIB_SEMANTIC_RIG);
  AttribDataPhysicsRig* physicsRigAttrib = parameters->getInputAttrib<AttribDataPhysicsRig>(taskIndex++, ATTRIB_SEMANTIC_PHYSICS_RIG);
  AttribDataCharacterProperties* characterController = parameters->getInputAttrib<AttribDataCharacterProperties>(taskIndex++, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  AttribDataCharacter* characterAttrib = parameters->getInputAttrib<AttribDataCharacter>(taskIndex++, ATTRIB_SEMANTIC_CHARACTER);
  Character* character = characterAttrib ? characterAttrib->m_character : 0;
  // Get the node ID of this behaviour node from the output attrib.
  AttribDataBool* outputUpdated = parameters->createOutputAttrib<AttribDataBool>(taskIndex++, ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS);
  (void)outputUpdated; // not actually interested in it

  AttribDataBehaviourSetup* behaviourSetup = parameters->getInputAttrib<AttribDataBehaviourSetup>(taskIndex++, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
#ifdef SHOW_BEHAVIOUR_ID_IN_PROFILE_OUTPUT
  StartProfiling(behaviourSetup->m_behaviourID);
#endif

  AttribDataPhysicsInitialisation* physicsInitialisation = parameters->getInputAttrib<AttribDataPhysicsInitialisation>(taskIndex++, ATTRIB_SEMANTIC_PHYSICS_INITIALISATION);
  AttribDataBoolArray* outputMask = parameters->getInputAttrib<AttribDataBoolArray>(taskIndex++, ATTRIB_SEMANTIC_OUTPUT_MASK);
  AttribDataBehaviourParameters* behaviourParameters = parameters->getInputAttrib<AttribDataBehaviourParameters>(taskIndex++, ATTRIB_SEMANTIC_BEHAVIOUR_PARAMETERS);

  const uint32_t numRigJoints = rigAttribData->m_rig->getNumBones();
  AttribDataPhysicsStateCreateDesc physicsDesc(numRigJoints);
  AttribDataPhysicsState* physicsState = parameters->createOutputAttribReplace<AttribDataPhysicsState>(taskIndex++, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, &physicsDesc);
  NMP_ASSERT(physicsState->m_previousChildTransforms->m_transformBuffer->getLength() == numRigJoints);

  AttribDataBehaviourState* behaviourState = parameters->getInputAttrib<AttribDataBehaviourState>(taskIndex++, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE_2);
  AttribDataAnimToPhysicsMap* animToPhysics = parameters->getInputAttrib<AttribDataAnimToPhysicsMap>(taskIndex++, ATTRIB_SEMANTIC_ANIM_TO_PHYSICS_MAP);
  AttribDataCharacterControllerDef* characterControllerDef = parameters->getInputAttrib<AttribDataCharacterControllerDef>(taskIndex++, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF);

  AttribDataInt* priorityAttrib = parameters->getInputAttrib<AttribDataInt>(taskIndex++, ATTRIB_SEMANTIC_CP_INT);
  int32_t priorityNodeBehaviour = priorityAttrib->m_value;

  ++physicsState->m_updateCounter;

  PhysicsRig* physicsRig = physicsRigAttrib->m_physicsRig;
  if (!physicsRig)
  {
    // Make sure that if we get a physics rig in the future then (a) it gets initialised and (b) we add a reference to it.
    physicsState->m_haveAddedPhysicsRigRef = false;
    NM_END_PROFILING();
    return;
  }
  NMP_ASSERT(physicsRig->getPhysicsRigDef());

  if (!physicsState->m_haveAddedPhysicsRigRef)
  {
    // Note that this should only get called if the physics rig wasn't assigned to the network
    // during update connections, but now it is.
    PhysicsInitialisation pi;
    pi.m_transforms = physicsInitialisation->m_transforms->m_transformBuffer;
    pi.m_prevTransforms = physicsInitialisation->m_prevTransforms->m_transformBuffer;
    pi.m_worldRoot = physicsInitialisation->m_worldRoot;
    pi.m_prevWorldRoot = physicsInitialisation->m_prevWorldRoot;
    pi.m_deltaTime = physicsInitialisation->m_deltaTime;

    bool originallyReferenced = physicsRig->isReferenced();
    initialisePhysics(pi, physicsRig, physicsState, parameters->m_dispatcher);

    if (character && !originallyReferenced)
    {
      character->getBody().postPhysicsStep(physicsInitialisation->m_deltaTime);
    }
  }

  if (physicsState->m_needToInitialiseRoot)
  {
    physicsState->m_needToInitialiseRoot = false;
    NMP::Matrix34 originalRoot = characterController->m_worldRootTransform;
    // Re-initialise the blend between the current and the physics roots transforms
    (void) physicsRig->getRoot(&originalRoot, 0.0f, characterControllerDef->m_characterControllerDef);
  }

  NMP_ASSERT(behaviourSetup->m_behaviourID >= 0);
  for (uint32_t i = 0; i < physicsRig->getNumParts(); ++i)
  {
    // Enable dynamics on all parts enabled by the partial body/output mask
    if (outputMask->m_values[animToPhysics->m_animToPhysicsMap->getAnimIndexFromPhysicsIndex(i)] == true)
    {
      physicsRig->getPart(i)->makeKinematic(false, 1.0f, false);
    }
  }

  //NM_END_PROFILING(); // EuphoriaRagdoll -> 0.015

  // call ER::Character interface
  if (character && !character->isBehaviourUpdateDisabled())
  {
    if (priorityNodeBehaviour < character->getHighestPriority(behaviourSetup->m_behaviourID))
    {
      NM_END_PROFILING();
      return;
    }

    Behaviour* const behaviour = character->getBehaviour(behaviourSetup->m_behaviourID);

    behaviour->interpretControlParams(behaviourState);
    behaviour->interpretMessageData(behaviourParameters);

    // Pass in any input animations. Note that the setup data contains all the animation inputs,
    // even if they're not connected.
    int numAnimationInputs = behaviourSetup->m_childNodeIDs->m_numValues;

    BehaviourAnimationData behaviourAnimationData;
    behaviourAnimationData.m_numTMs = physicsRig->getNumParts();
    behaviourAnimationData.m_TMs = 0;

    for (
      behaviourAnimationData.m_animationID = 0; 
      behaviourAnimationData.m_animationID != numAnimationInputs; 
      ++behaviourAnimationData.m_animationID
      )
    {
      if (character->isAnimationRequiredByBehaviour(behaviourSetup->m_behaviourID, behaviourAnimationData.m_animationID))
      {
        NMP::DataBuffer* poseAnimDataBuffer;
        if ((MR::NodeID)behaviourSetup->m_childNodeIDs->m_values[behaviourAnimationData.m_animationID] == MR::INVALID_NODE_ID)
        {
          // There was no input connected anyway. Use the default pose
          behaviourAnimationData.m_isSet = false;
          NMP_ASSERT(character->getBody().m_definition->m_defaultPoseAnimData);
          poseAnimDataBuffer = character->getBody().m_definition->m_defaultPoseAnimData;
        }
        else
        {
          behaviourAnimationData.m_isSet = true;
          AttribDataTransformBuffer* poseAnimDataBufferAttribData = parameters->getInputAttrib<AttribDataTransformBuffer>(
            taskIndex++, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
          poseAnimDataBuffer = poseAnimDataBufferAttribData->m_transformBuffer;
        }

        // Apple LLVM bug - if we use an if () here to only allocate when we first need it the the compiler encounters an error
        behaviourAnimationData.m_TMs = 
          behaviourAnimationData.m_TMs ? 
          behaviourAnimationData.m_TMs : (NMP::Matrix34*) alloca(behaviourAnimationData.m_numTMs * sizeof(NMP::Matrix34));

        // Pass default pose animation data so it could be used whenever there is no channel in pose animation data buffer.
        NMP::DataBuffer* defaultPoseAnimDataBuffer = character->getBody().m_definition->m_defaultPoseAnimData;
        NMP_ASSERT(defaultPoseAnimDataBuffer);

        physicsRig->calculatePartTMsRelativeToRoot(
          behaviourAnimationData.m_TMs,
          *poseAnimDataBuffer,
          *defaultPoseAnimDataBuffer);

        behaviour->interpretAnimationMessage(&behaviourAnimationData, &character->getBody());
      }
      else if ((MR::NodeID)behaviourSetup->m_childNodeIDs->m_values[behaviourAnimationData.m_animationID] != MR::INVALID_NODE_ID)
      {
        // Just skip this input animation as it's not required by the behaviour at the moment.
        taskIndex++;
      }
    }

    NMP_ASSERT(parameters->m_numParameters >= taskIndex);

    PhysicsRig::JointChooser jointChooser(outputMask, physicsRig->getPhysicsRigDef(), animToPhysics->m_animToPhysicsMap);
    character->handlePhysicsJointMask(behaviourSetup->m_behaviourID, jointChooser);
  }

  NM_END_PROFILING(); // EuphoriaRagdoll -> 0.029
}

//----------------------------------------------------------------------------------------------------------------------
void nodeBehaviourDeleteInstance(const NodeDef* node, Network* net)
{
  NET_LOG_MESSAGE(99, "Deleting behaviour node %s\n", node->getName());
  AttribDataHandle* handle = net->getNetworkDef()->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
  NMP_ASSERT(handle);
  AttribDataBehaviourSetup* behaviourSetup = (AttribDataBehaviourSetup*) handle->m_attribData;
  NMP_ASSERT(behaviourSetup);

  // We don't need to restore any properties because next time a physics node takes over it will
  // call addReference which will restore default properties on the whole character.
  // All the PhysicsRig::apply* functions should set all the properties they need to each

  // remove the body reference if we can find it
  NodeBinEntry *attribEntry = net->getAttribDataNodeBinEntry(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    node->getNodeID(),
    INVALID_NODE_ID,
    VALID_FRAME_ANY_FRAME);
  // Check if the attrib still exists.
  if (attribEntry)
  {
    AttribDataPhysicsState *physicsState = (AttribDataPhysicsState*)attribEntry->m_attribDataHandle.m_attribData;
    NMP_ASSERT(physicsState);

    // Stop the character behavior which will only have been started if the node created a physics state (See 
    // nodeBehaviourUpdateConnections in erNodeBehaviour.cpp).
    Character *character = networkGetCharacter(net);
    if (character)
    {
      character->stopBehaviour(behaviourSetup->m_behaviourID);
    }

    PhysicsRig *physicsRig = getPhysicsRig(net);
    if (physicsRig && physicsState->m_haveAddedPhysicsRigRef)
    {
      physicsRig->removeReference();
    }
  }
  
  nodeShareDeleteInstanceWithChildren(node, net);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBehaviourUpdatePhysicalTrajectoryPostPhysics(MR::Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  AttribDataPhysicsRig* physicsRigAttrib = parameters->getInputAttrib<AttribDataPhysicsRig>(0, ATTRIB_SEMANTIC_PHYSICS_RIG);
  AttribDataCharacterProperties* characterController = parameters->getInputAttrib<AttribDataCharacterProperties>(1, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  AttribDataCharacterControllerDef* characterControllerDef = parameters->getInputAttrib<AttribDataCharacterControllerDef>(2, ATTRIB_SEMANTIC_CHARACTER_CONTROLLER_DEF);
  AttribDataTrajectoryDeltaTransform* trajOutput = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(3, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataBlendWeights* initialisationFractionAttrib = parameters->getOptionalInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);

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
  NMP::Quat prevRot = previousWorldRoot.toQuat();
  NMP::Quat prevInvRot(prevRot);
  prevInvRot.conjugate();
  NMP::Quat newRot = worldRoot.toQuat();
  trajOutput->m_deltaPos = prevInvRot.rotateVector(worldTrajDelta);

  trajOutput->m_deltaAtt.multiply(prevInvRot, newRot);
  trajOutput->m_deltaAtt.normalise();
  trajOutput->m_filteredOut = false;
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBehaviourUpdateAnimatedTrajectory(MR::Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();
  AttribDataTrajectoryDeltaTransform* outputDeltaTransform =
    parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);
  outputDeltaTransform->m_filteredOut = true;
  (void)outputDeltaTransform;
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBehaviourUpdateTransformsPostPhysics(MR::Dispatcher::TaskParameters* parameters)
{
  NM_BEGIN_PROFILING("TaskBehaviourUpdateTransformsPostPhysics");
  NET_LOG_ENTER_FUNC();
  AttribDataRig* rigAttribData = parameters->getInputAttrib<AttribDataRig>(0, ATTRIB_SEMANTIC_RIG);
  AttribDataPhysicsRig* physicsRigAttrib = parameters->getInputAttrib<AttribDataPhysicsRig>(1, ATTRIB_SEMANTIC_PHYSICS_RIG);
  AttribDataCharacterProperties* characterController = parameters->getInputAttrib<AttribDataCharacterProperties>(2, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);
  const uint32_t numRigJoints = rigAttribData->m_rig->getNumBones();
  AttribDataTransformBuffer* outputTransforms = parameters->createOutputAttribTransformBuffer(3, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // copy the non-physical transforms from the bind pose
  AttribDataTransformBuffer* inputTransforms = rigAttribData->m_rig->getBindPose();
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
  NMP::Matrix34 worldRoot = characterController->m_worldRootTransform;
  physicsRig->toTransformBuffer(*outputTransforms->m_transformBuffer, worldRoot, 0);

  NM_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
static void trajectoryOverrideSetTrajectoryDeltaTransform(
  const NMP::Matrix34& worldRoot,
  const NMP::Matrix34& previousWorldRoot,
  AttribDataTrajectoryDeltaTransform* const trajOutput)
{
  NMP_ASSERT(previousWorldRoot.isValidTM(0.001f));

  NMP::Vector3 worldTrajDelta = worldRoot.translation() - previousWorldRoot.translation();
  // Note that the translation is in the frame of the previous timestep.

  // Translation
  NMP::Quat prevRot = previousWorldRoot.toQuat();
  NMP::Quat prevInvRot = ~prevRot;
  NMP::Quat newRot = worldRoot.toQuat();
  trajOutput->m_deltaPos = prevInvRot.rotateVector(worldTrajDelta);

  // Rotation
  trajOutput->m_deltaAtt.multiply(prevInvRot, newRot);
  trajOutput->m_deltaAtt.normalise();
  trajOutput->m_filteredOut = false;
}

//----------------------------------------------------------------------------------------------------------------------
static NMP::Quat trajectoryOverrideFindCharacterOrientation(
  const AttribDataTrajectoryOverrideDefAnimSet* const setupAttrib,
  const AttribDataVector3* const                      directionAttrib,
  const NMP::Vector3&                                 worldUpDirection)
{
  NMP_ASSERT(setupAttrib);      // Null check.
  NMP_ASSERT(directionAttrib);  // Null check.

  NMP::Vector3 direction = directionAttrib->m_value;

  // Ensure CC is aligned with world up direction by making edge forwards direction perpendicular to world up direction.
  direction -= NMP::vDot(direction, worldUpDirection) * worldUpDirection;
  direction.fastNormalise();

  NMP_ASSERT(NMP::nmfabs(NMP::vDot(direction, worldUpDirection)) < 0.001f); // Assume edge fwd is perp to up.

  // Find transform from current to target.
  NMP::Quat worldToTarget;
  worldToTarget.forRotation(setupAttrib->m_directionInCharacterSpace, directionAttrib->m_value);

  return worldToTarget;
}

//----------------------------------------------------------------------------------------------------------------------
static NMP::Vector3 trajectoryOverrideFindWorldSpaceTrajectoryPosition(
  const AttribDataTrajectoryOverrideDefAnimSet* const setupAttrib,
  const AttribDataTransformBuffer* const              poseAttrib,
  const AttribDataPhysicsRig* const                   physicsRigAttrib,
  const NMP::Quat&                                    characterOrientation
  )
{
  const MR::PhysicsRig* const physicsRig(physicsRigAttrib->m_physicsRig);

  // Add world space position of physics part.
  const int32_t physicsPartIndex(setupAttrib->m_physicsPartIndex);

  // Find position of reference part relative to trajectory in pose.
  const NMP::Matrix34 worldRoot(NMP::Matrix34::kIdentity);
  NMP::Matrix34 offsetTM(NMP::Matrix34::kIdentity);

  physicsRig->calculateWorldSpacePartTM(
    offsetTM,
    physicsPartIndex,
    *poseAttrib->m_transformBuffer,
    *physicsRig->getAnimRigDef()->getBindPose()->m_transformBuffer,
    worldRoot,
    false);

  NMP::Vector3 result(-offsetTM.translation());

  // Transform offset by character orientation.
  result = characterOrientation.rotateVector(result);

  NMP_ASSERT(physicsPartIndex >= 0);
  NMP_ASSERT(physicsPartIndex < static_cast< int32_t >(physicsRig->getNumParts()));

  const MR::PhysicsRig::Part* const physicsPart(physicsRig->getPart(physicsPartIndex));

  NMP_ASSERT(physicsPart);

  if (physicsPart)
  {
    // Add extracted offset to world space position of reference part.
    result += physicsPart->getPosition();
  }

  // Optionally move trajectory to local floor position.
  if (setupAttrib->m_projectOntoGround)
  {
    const PhysicsScene* const characterPhysicsScene(physicsRig->getPhysicsScene());

    NMP_ASSERT(characterPhysicsScene);

    if (characterPhysicsScene)
    {
      result = characterPhysicsScene->getFloorPositionBelow(
        result, 
        physicsRig, 
        setupAttrib->m_projectOntoGroundDistance);
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
static const NMP::Quat trajectoryOverrideFindWorldSpaceTrajectoryOrientation(
  const AttribDataTransformBuffer* const poseAttrib,
  const AttribDataRig* const             animRigAttrib,
  const NMP::Quat&                       characterOrientation
  )
{
  NMP_ASSERT(poseAttrib);
  NMP_ASSERT(animRigAttrib);

  // Extract orientation of trajectory from pose.
  NMP::DataBuffer* const transform(poseAttrib->m_transformBuffer);
  MR::AnimRigDef* const  animRig(animRigAttrib->m_rig);

  NMP_ASSERT(animRig);
  NMP_ASSERT(transform);

  NMP::Quat result(*transform->getChannelQuatFast(animRig->getTrajectoryBoneIndex()));

  // Concatenate with character orientation.
  result *= characterOrientation;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTrajectoryOverrideUpdateTrajectoryDelta(Dispatcher::TaskParameters* parameters)
{
  NET_LOG_ENTER_FUNC();

  // Pose from which to extract the trajectory position (relative to the reference part) and orientation.
  const AttribDataTransformBuffer* const poseAttrib
    = parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  const AttribDataRig* const animRigAttrib
    = parameters->getInputAttrib<AttribDataRig>(1, ATTRIB_SEMANTIC_RIG);

  const AttribDataPhysicsRig* const physicsRigAttrib
    = parameters->getInputAttrib<AttribDataPhysicsRig>(2, ATTRIB_SEMANTIC_PHYSICS_RIG);

  const AttribDataCharacterProperties* const characterControllerAttrib
    = parameters->getInputAttrib<AttribDataCharacterProperties>(3, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);

  // Node specific attribute data.
  const AttribDataTrajectoryOverrideDefAnimSet* const setupAttrib
    = parameters->getInputAttrib<AttribDataTrajectoryOverrideDefAnimSet>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // Forward direction of the edge the character is holding onto in world space.
  const AttribDataVector3* const directionAttrib
    = parameters->getInputAttrib<AttribDataVector3>(5, ATTRIB_SEMANTIC_CP_VECTOR3);

  // Output.
  AttribDataTrajectoryDeltaTransform* const trajOutputAttrib
    = parameters->createOutputAttrib<AttribDataTrajectoryDeltaTransform>(6, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  PhysicsRig* const physicsRig(physicsRigAttrib->m_physicsRig);

  if (!physicsRig)
  {
    return;
  }

  // Find orientation of character in world space. 
  const NMP::Quat characterOrientation = trajectoryOverrideFindCharacterOrientation(
    setupAttrib,
    directionAttrib,
    physicsRig->getPhysicsScene()->getWorldUpDirection());

  // Find world space position of trajectory.
  const NMP::Vector3 trajectoryPosition = trajectoryOverrideFindWorldSpaceTrajectoryPosition( 
    setupAttrib,
    poseAttrib,
    physicsRigAttrib,
    characterOrientation);

  // Find world space orientation of trajectory.
  const NMP::Quat trajectoryOrientation = trajectoryOverrideFindWorldSpaceTrajectoryOrientation(
    poseAttrib,
    animRigAttrib,
    characterOrientation);

  // Create desired trajectory TM.
  const NMP::Matrix34 worldRoot(trajectoryOrientation, trajectoryPosition);

  NMP_ASSERT(characterControllerAttrib);

  // Determine previous frames trajectory TM
  NMP::Matrix34 previousWorldRoot = characterControllerAttrib->m_worldRootTransform;
  NMP_ASSERT(previousWorldRoot.isValidTM(0.001f));

  // Write difference between previous frame's trajectory TM and the desired TM to trajectory delta output.
  trajectoryOverrideSetTrajectoryDeltaTransform(worldRoot, previousWorldRoot, trajOutputAttrib);
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
