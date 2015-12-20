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
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMProfiler.h"
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/mrUnevenTerrainIK.h"
#include "morpheme/mrUnevenTerrainUtilities.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief This task initialises the uneven terrain IK setup with the current pose of the character
//----------------------------------------------------------------------------------------------------------------------
void TaskBasicUnevenTerrainIKSetup(Dispatcher::TaskParameters* parameters)
{
  AttribDataBasicUnevenTerrainIKSetup* outputIKSetupAttrib;
  const AttribDataBasicUnevenTerrainSetup* setupAttrib;
  const AttribDataCharacterProperties* characterControllerAttrib;
  const AttribDataBasicUnevenTerrainChain* chainAttrib;
  const AttribDataRig* rigAttrib;
  const AttribDataTransformBuffer* inputTransformsAttrib;
  const AttribDataTrajectoryDeltaTransform* inputTrajectoryDeltaAttrib;

  // IK solver parameters
  UnevenTerrainHipsIK hipsIKSolver;
  UnevenTerrainLegIK legIKSolvers[2];
  UnevenTerrainFootIK footIKSolvers[2];

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "TaskBasicUnevenTerrainIKSetup()\n");
#endif

  // Uneven terrain const setup
  setupAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainSetup>(0, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // IK chain information
  chainAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainChain>(1, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // Character controller (2 - update bool flag)
  characterControllerAttrib = parameters->getInputAttrib<AttribDataCharacterProperties>(3, ATTRIB_SEMANTIC_CHARACTER_PROPERTIES);

  // Animation rig
  rigAttrib = parameters->getInputAttrib<AttribDataRig>(4, ATTRIB_SEMANTIC_RIG);

  // Input transforms buffer
  inputTransformsAttrib = parameters->getInputAttrib<AttribDataTransformBuffer>(5, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // Uneven terrain output IK setup information
  outputIKSetupAttrib = parameters->createOutputAttribReplace<AttribDataBasicUnevenTerrainIKSetup>(6, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_IK_SETUP);

  // Trajectory delta transform
  inputTrajectoryDeltaAttrib = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(7, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  //---------------------------
  unevenTerrainIKSetup(
    &hipsIKSolver,
    legIKSolvers,
    footIKSolvers,
    outputIKSetupAttrib,
    setupAttrib,
    characterControllerAttrib,
    chainAttrib,
    rigAttrib,
    inputTransformsAttrib,
    inputTrajectoryDeltaAttrib->m_deltaPos,
    inputTrajectoryDeltaAttrib->m_deltaAtt);
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
// \brief This task performs a ray-cast with the physics from the character hips height in the
// downward vertical direction. This task can not be performed externally on a SPU.
//----------------------------------------------------------------------------------------------------------------------
void TaskBasicUnevenTerrainFootLiftingTarget(Dispatcher::TaskParameters* parameters)
{
  NM_UT_BEGIN_PROFILING("BASIC_UT_FOOT_LIFTING_TARGET_TASK");

  const AttribDataBasicUnevenTerrainSetup* setupAttrib;
  const AttribDataBasicUnevenTerrainChain* chainAttrib;
  const AttribDataBasicUnevenTerrainIKSetup* inputIKSetupAttrib;
  AttribDataBasicUnevenTerrainFootLiftingTarget* outputFootLiftingTargetAttrib;

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "TaskBasicUnevenTerrainFootLiftingTarget()\n");
#endif

  // Uneven terrain const setup
  setupAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainSetup>(0, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Uneven terrain IK chain information
  chainAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainChain>(1, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // Uneven terrain input IK setup information
  inputIKSetupAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainIKSetup>(2, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_IK_SETUP);

  // Character controller (3 - update bool flag)

  // Output foot lifting target information
  outputFootLiftingTargetAttrib = parameters->createOutputAttribReplace<AttribDataBasicUnevenTerrainFootLiftingTarget>(4, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET);

  //---------------------------
  // Information
  Network* net = parameters->m_dispatcher->getTaskQueue()->m_owningNetwork;
  NMP_ASSERT(net);
  const CharacterControllerInterface* cc = net->getCharacterController();
  NMP_ASSERT(cc);

  // Compute the up axis direction for foot lifting in the frame of the world
  NMP::Vector3 upAxisWS;
  inputIKSetupAttrib->m_characterRootWorldTM.rotateVector(setupAttrib->m_upAxis, upAxisWS);

  // Check if the character controller achieved its requested motion
  if (net->getCharacterPropertiesAchievedRequestedMovement())
  {
    // Compute the ray-casts to recover the terrain hit positions
    unevenTerrainRayCastFromCurrentFootbasePos(
      upAxisWS,
      net,
      cc,
      chainAttrib,
      inputIKSetupAttrib,
      outputFootLiftingTargetAttrib);
  }
  else
  {
#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "Character controller did not achieve its requested motion\n");
#endif

    unevenTerrainDefaultFootLiftingTargets(
      upAxisWS,
      inputIKSetupAttrib,
      outputFootLiftingTargetAttrib);
  }

  NM_UT_END_PROFILING();
}
#endif // !NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// \brief This task determines the required foot lifting and computes the leg IK to obtain
// suitable transforms for the specified leg joints.
//----------------------------------------------------------------------------------------------------------------------
void TaskBasicUnevenTerrainTransforms(Dispatcher::TaskParameters* parameters)
{
  NM_UT_BEGIN_PROFILING("BASIC_UT_TRANSFORMS_TASK");

  // Input args
  const AttribDataUpdatePlaybackPos* networkUpdateTimeAttrib;
  const AttribDataTransformBuffer* inputTransformsAttrib;
  const AttribDataRig* rigAttrib;
  const AttribDataBasicUnevenTerrainSetup* setupAttrib;
  const AttribDataBasicUnevenTerrainChain* chainAttrib;
  const AttribDataBasicUnevenTerrainIKSetup* inputIKSetupAttrib;
  const AttribDataBasicUnevenTerrainFootLiftingTarget* inputFootLiftingTargetAttrib;
  const AttribDataFloat* ikHipsWeightAttrib;
  const AttribDataFloat* ikFkBlendWeightAttrib;
  // Output args
  AttribDataTransformBuffer* outputTransformsAttrib;
  // Input | Output args
  AttribDataBasicUnevenTerrainIKState* ikStateAttrib;

  // IK solver parameters
  UnevenTerrainHipsIK hipsIKSolver;
  UnevenTerrainLegIK legIKSolvers[2];
  UnevenTerrainFootIK footIKSolvers[2];

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "TaskBasicUnevenTerrainTransforms()\n");
#endif

  // Network update time
  networkUpdateTimeAttrib = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(0, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  // Input transforms buffer
  inputTransformsAttrib = parameters->getInputAttrib<AttribDataTransformBuffer>(1, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  const NMP::DataBuffer* inputTransforms = inputTransformsAttrib->m_transformBuffer;
  uint32_t numRigJoints = inputTransforms->getLength();
  NMP_ASSERT(numRigJoints >= 1);

  // Output transforms buffer
  outputTransformsAttrib = parameters->createOutputAttribTransformBuffer(2, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);
  NMP::DataBuffer* outputTransforms = outputTransformsAttrib->m_transformBuffer;

  // Animation rig
  rigAttrib = parameters->getInputAttrib<AttribDataRig>(3, ATTRIB_SEMANTIC_RIG);

  // Character controller (4 - update bool flag)

  // Uneven terrain node definition setup data
  setupAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainSetup>(5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Uneven terrain - anim set dependent
  chainAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainChain>(6, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // Uneven terrain input IK setup information
  inputIKSetupAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainIKSetup>(7, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_IK_SETUP);

  // Ray cast information
  inputFootLiftingTargetAttrib = parameters->getInputAttrib<AttribDataBasicUnevenTerrainFootLiftingTarget>(8, ATTRIB_SEMANTIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET);

  // Uneven terrain IK state (Input | Output)
  ikStateAttrib = parameters->createOutputAttribReplace<AttribDataBasicUnevenTerrainIKState>(9, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  // IkHipsWeight
  ikHipsWeightAttrib = parameters->getInputAttrib<AttribDataFloat>(10, ATTRIB_SEMANTIC_CP_FLOAT);

  // IkFkBlendWeight
  ikFkBlendWeightAttrib = parameters->getInputAttrib<AttribDataFloat>(11, ATTRIB_SEMANTIC_CP_FLOAT);

#ifdef TEST_UT_TASK_LOGGING
  float deltaTime = 0.0f;
  if (!networkUpdateTimeAttrib->m_isAbs)
  {
    deltaTime = networkUpdateTimeAttrib->m_value;
  }
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "deltaTime = %f\n", deltaTime);
#endif

  //---------------------------
  // Initialisation
  inputTransforms->copyTo(outputTransforms);

  // Compute the up axis direction for foot lifting in the frame of the world
  NMP::Vector3 upAxisWS;
  inputIKSetupAttrib->m_characterRootWorldTM.rotateVector(setupAttrib->m_upAxis, upAxisWS);

  unevenTerrainInitSolve(
    &hipsIKSolver,
    legIKSolvers,
    footIKSolvers,
    outputTransformsAttrib,
    rigAttrib,
    chainAttrib,
    inputIKSetupAttrib);

  //---------------------------
  // Motion solve
  unevenTerrainMotionSolve(
    upAxisWS,
    &hipsIKSolver,
    legIKSolvers,
    footIKSolvers,
    inputTransformsAttrib,
    networkUpdateTimeAttrib,
    setupAttrib,
    chainAttrib,
    inputIKSetupAttrib,
    inputFootLiftingTargetAttrib,
    ikHipsWeightAttrib,
    ikStateAttrib);

  //---------------------------
  // IK / FK blending
  unevenTerrainIKFKBlend(
    &hipsIKSolver,
    legIKSolvers,
    inputTransformsAttrib,
    outputTransformsAttrib,
    ikStateAttrib,
    ikFkBlendWeightAttrib);

  NM_UT_END_PROFILING();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
