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
#include "morpheme/mrUnevenTerrainUtilities.h"
#include "morpheme/mrJointControlUtilities.h"
#include "morpheme/mrDebugMacros.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
//----------------------------------------------------------------------------------------------------------------------
void unevenTerrainIKSetup(
  UnevenTerrainHipsIK*                 hipsIKSolver,
  UnevenTerrainLegIK*                  legIKSolvers,
  UnevenTerrainFootIK*                 footIKSolvers,
  AttribDataBasicUnevenTerrainIKSetup* outputIKSetupAttrib,
  const AttribDataBasicUnevenTerrainSetup* setupAttrib,
  const AttribDataCharacterProperties* characterControllerAttrib,
  const AttribDataBasicUnevenTerrainChain* chainAttrib,
  const AttribDataRig*                 rigAttrib,
  const AttribDataTransformBuffer*     inputTransformsAttrib,
  const NMP::Vector3&                  inputTrajectoryDeltaPos,
  const NMP::Quat&                     inputTrajectoryDeltaAtt)
{
  NMP_ASSERT(hipsIKSolver);
  NMP_ASSERT(legIKSolvers);
  NMP_ASSERT(footIKSolvers);
  NMP_ASSERT(outputIKSetupAttrib);
  NMP_ASSERT(setupAttrib);
  NMP_ASSERT(characterControllerAttrib);
  NMP_ASSERT(chainAttrib);
  NMP_ASSERT(rigAttrib);
  NMP_ASSERT(inputTransformsAttrib);
  NMP_ASSERT(
    chainAttrib->m_footAlignToSurfaceMaxSlopeAngle >= 0.0f &&
    chainAttrib->m_footAlignToSurfaceMaxSlopeAngle <= 1.0f); // Cosine of the angle limit

  NM_UT_BEGIN_PROFILING("BASIC_UT_IK_SETUP");

  const AnimRigDef* rig = rigAttrib->m_rig;
  NMP_ASSERT(rig);

  uint32_t numLimbs = chainAttrib->m_numLimbs;
  NMP_ASSERT(numLimbs > 0);
  float recipNumLimbs = 1.0f / numLimbs;

  //---------------------------
  // Check if the character controller achieved its requested motion. If not then we will be
  // blending the foot motion back to the source animation so there is no need to perform any
  // ray-casting with the physics scene
#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "groundContactTime = %f\n", 
    characterControllerAttrib->m_groundContactTime);
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "achievedRequestedMovement = %d\n", 
    characterControllerAttrib->m_achievedRequestedMovement);
#endif
  outputIKSetupAttrib->m_enableIK = false;
  if (characterControllerAttrib->m_groundContactTime > -0.1f && characterControllerAttrib->m_achievedRequestedMovement)
  {
    outputIKSetupAttrib->m_enableIK = true;
  }

  // Set the character controller world root position
  const NMP::Matrix34& curWorldTM = characterControllerAttrib->m_worldRootTransform;
  outputIKSetupAttrib->m_characterRootWorldTM = curWorldTM;

  // Compute the up axis direction for foot lifting in the frame of the world
  NMP::Vector3 upAxisWS;
  curWorldTM.rotateVector(setupAttrib->m_upAxis, upAxisWS);

  //------------------------
  // FK to the hips joint
  hipsIKSolver->init(
    rigAttrib->m_rig,
    inputTransformsAttrib->m_transformBuffer,
    chainAttrib->m_hipsChannelID);

  hipsIKSolver->fkHipsParentJointTM(curWorldTM);
  hipsIKSolver->fkHipsJointTM();
  const NMP::Matrix34& hipsTM = hipsIKSolver->getHipsJointTM();

  //---------------------------
  // Initialise the leg IK solvers (Anim source pose)
  outputIKSetupAttrib->m_rootToHipsHeight = 0.0f;
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    const AttribDataBasicUnevenTerrainChain::ChainData* chainData = chainAttrib->m_chainInfo[limbIndex];
    NMP_ASSERT(chainData);

    // Initialise the Leg IK solver
    UnevenTerrainLegIK& legIK = legIKSolvers[limbIndex];
    legIK.init(
      rig,
      inputTransformsAttrib->m_transformBuffer,
      chainData->m_channelIDs[0],
      chainData->m_channelIDs[1],
      chainData->m_channelIDs[2],
      chainData->m_kneeRotationAxis,
      chainAttrib->m_straightestLegFactor);

    // FK the leg IK joints
    legIK.fkRootParentJointTM(hipsTM, chainAttrib->m_hipsChannelID);
    legIK.fkEndJointTM();
    const NMP::Matrix34& endJointTM = legIK.getEndJointTM();

    // Compute the vertical distance from the character root to the IK root joint
    const NMP::Matrix34& rootJointTM = legIK.getRootJointTM();
    NMP::Vector3 charRootToIKRootV = rootJointTM.translation() - curWorldTM.translation();
    float lambda = charRootToIKRootV.dot(upAxisWS);
    outputIKSetupAttrib->m_rootToHipsHeight += lambda;

    // FK the straightest leg length
    legIK.fkStraightestLegLength();

    // Initialise the Foot IK solver
    UnevenTerrainFootIK& footIK = footIKSolvers[limbIndex];
    footIK.init(
      rig,
      inputTransformsAttrib->m_transformBuffer,
      chainData->m_numFootJoints,
      &chainData->m_channelIDs[2]);

    // FK the foot IK joints
    footIK.setAnkleJointTM(endJointTM);
    footIK.fkFootJointTMs(false);

    // Compute the target world foot pivot position, i.e. the point about which the
    // foot pivots in order to align the footbase with the terrain surface.
    // Note: After tilting the foot to the correct attitude, is will then be lifted
    // vertically onto the terrain surface (actually it is lifted so that the relative
    // pivot to canonical ground plane distance is preserved with the new surface).
    NMP::Vector3 worldFootPivotPos;
    footIK.computeWorldFootCentroid(worldFootPivotPos);

    // Initialise the target worldspace foot pivot position and recover the
    // corresponding point in the frame of the ankle joint. The ankle relative
    // point will be used later after vel/accel limit clamping in order to recover
    // a new world pivot position.
    footIK.initFootPivotPos(worldFootPivotPos);
  }

  // Average hip height from character root
  outputIKSetupAttrib->m_rootToHipsHeight *= recipNumLimbs;

  //---------------------------
  // Initialise the Hips IK setup state
  outputIKSetupAttrib->m_hipsParentWorldJointTM = hipsIKSolver->getHipsParentJointTM();
  outputIKSetupAttrib->m_hipsWorldJointTM = hipsIKSolver->getHipsJointTM();

  //---------------------------
  // Compute the canonical ground plane normal (in the world frame) whose slope
  // is in the direction of the animation trajectory delta. This will be used to
  // compute the relative height and orientation of the foot above the assumed
  // ground plane of animation trajectory motion.
  NMP::Quat footPreAlignOffsetQuat;
  NMP::Vector3 trajectoryPlaneNormal;
  float dotD;
  if (chainAttrib->m_useTrajectorySlopeAlignment)
  {
    // Direction vector (in the world frame) corresponding to the inverse of the
    // *animation* trajectory position component. i.e. compute the position of last
    // frame's character controller using the animation trajectory delta (not the
    // actual post physics CC trajectory delta) and recover the delta translation
    // between the current CC to the computed last frame CC. 
    NMP::Vector3 invTrajectoryDeltaPos;
    // Inverse anim trajectory position component in the frame of the current CC
    inputTrajectoryDeltaAtt.inverseRotateVector(
      inputTrajectoryDeltaPos,
      invTrajectoryDeltaPos);
    invTrajectoryDeltaPos *= -1.0f;
    // Inverse anim trajectory position component in the world frame
    curWorldTM.rotateVector(invTrajectoryDeltaPos);

    // Compute the orthogonal vector that is in the trajectory motion plane
    NMP::Vector3 v2;
    v2.cross(invTrajectoryDeltaPos, upAxisWS);

    // Compute the trajectory motion plane normal (not normalised)
    trajectoryPlaneNormal.cross(v2, invTrajectoryDeltaPos);

    // Compute rotation that orients the trajectory plane normal back with the world up axis
    footPreAlignOffsetQuat.forRotation(trajectoryPlaneNormal, upAxisWS);

    // Check if the inclined trajectory motion plane is within the maximum slope limit.
    // Use the half angle formula for this: cos(theta/2) = sqrt(0.5*(1 + cos(theta)))
    float cosTheta = 2.0f * footPreAlignOffsetQuat.w * footPreAlignOffsetQuat.w - 1.0f;
    if (cosTheta < chainAttrib->m_footAlignToSurfaceMaxSlopeAngle)
    {
      // The inclined plane is too steep (some sort of jump motion?) so align with the
      // canonical ground plane
      footPreAlignOffsetQuat.identity();
    }

    // Recover the clamped trajectory plane normal (normalised)
    footPreAlignOffsetQuat.inverseRotateVector(upAxisWS, trajectoryPlaneNormal);
    dotD = trajectoryPlaneNormal.dot(upAxisWS);
  }
  else
  {
    // Use the canonical ground plane
    footPreAlignOffsetQuat.identity();
    trajectoryPlaneNormal = upAxisWS;
    dotD = 1.0f;
  }

  // Set the foot pre-alignment offset rotation
  outputIKSetupAttrib->m_footPreAlignOffsetQuat = footPreAlignOffsetQuat;

  //---------------------------
  // Initialise the IK setup limb state
  float averageStraightestLegLength = 0.0f;
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    AttribDataBasicUnevenTerrainIKSetup::LimbState* ikSetupLimbState = outputIKSetupAttrib->m_limbStates[limbIndex];
    const UnevenTerrainLegIK& legIK = legIKSolvers[limbIndex];
    const UnevenTerrainFootIK& footIK = footIKSolvers[limbIndex];
    NMP_ASSERT(ikSetupLimbState);

    // Save the Leg IK transform state
    ikSetupLimbState->m_rootParentWorldJointTM = legIK.getRootParentJointTM();
    ikSetupLimbState->m_rootWorldJointTM = legIK.getRootJointTM();
    ikSetupLimbState->m_midWorldJointTM = legIK.getMidJointTM();
    ikSetupLimbState->m_endWorldJointTM = legIK.getEndJointTM();
    ikSetupLimbState->m_straightestLegLength = legIK.getStraightestLegLength();

    // Save the Foot IK transform state
    ikSetupLimbState->m_localFootPivotPos = footIK.getLocalFootPivotPos();

    // Update the average straightest leg length
    averageStraightestLegLength += legIK.getStraightestLegLength();

    //---------------------------
    // Compute the relative distance between the foot pivot position and the trajectory
    // motion plane that the uneven terrain lifting step will try to preserve when
    // lifting onto the new surface.
    const NMP::Vector3& worldFootPivotPos = footIK.getWorldFootPivotPos();
    NMP::Vector3 charRootToFootPivotV = worldFootPivotPos - curWorldTM.translation();

    // The relative distance
    float dotN = trajectoryPlaneNormal.dot(charRootToFootPivotV);
    ikSetupLimbState->m_footbaseDistFromPivotPos = dotN / dotD;

    // The projected footbase position in the world on the trajectory motion plane
    ikSetupLimbState->m_initWorldFootbasePos = worldFootPivotPos - (upAxisWS * ikSetupLimbState->m_footbaseDistFromPivotPos);
  }

  // Update the average straightest leg length
  outputIKSetupAttrib->m_straightestLegLength = averageStraightestLegLength * recipNumLimbs;

  NM_UT_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
void unevenTerrainInitSolve(
  UnevenTerrainHipsIK*                 hipsIKSolver,
  UnevenTerrainLegIK*                  legIKSolvers,
  UnevenTerrainFootIK*                 footIKSolvers,
  AttribDataTransformBuffer*           outputTransformsAttrib,
  const AttribDataRig*                 rigAttrib,
  const AttribDataBasicUnevenTerrainChain* chainAttrib,
  const AttribDataBasicUnevenTerrainIKSetup* inputIKSetupAttrib)
{
  NMP_ASSERT(hipsIKSolver);
  NMP_ASSERT(legIKSolvers);
  NMP_ASSERT(footIKSolvers);
  NMP_ASSERT(outputTransformsAttrib);
  NMP_ASSERT(rigAttrib);
  NMP_ASSERT(chainAttrib);
  NMP_ASSERT(inputIKSetupAttrib);

  NM_UT_BEGIN_PROFILING("BASIC_UT_INIT_SOLVE");

  NMP::DataBuffer* outputTransforms = outputTransformsAttrib->m_transformBuffer;
  uint32_t numLimbs = inputIKSetupAttrib->m_numLimbs;

  // Hips IK state
  hipsIKSolver->init(
    rigAttrib->m_rig,
    outputTransforms,
    chainAttrib->m_hipsChannelID);

  hipsIKSolver->setHipsParentJointTM(inputIKSetupAttrib->m_hipsParentWorldJointTM);
  hipsIKSolver->setHipsJointTM(inputIKSetupAttrib->m_hipsWorldJointTM);

  //---------------------------
  // Initialise the current state end joint transform (anim source pose)
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    const AttribDataBasicUnevenTerrainChain::ChainData* chainData = chainAttrib->m_chainInfo[limbIndex];
    const AttribDataBasicUnevenTerrainIKSetup::LimbState* curIKSetupLimbState = inputIKSetupAttrib->m_limbStates[limbIndex];
    NMP_ASSERT(chainData);
    NMP_ASSERT(curIKSetupLimbState);
    UnevenTerrainLegIK& legIK = legIKSolvers[limbIndex];
    UnevenTerrainFootIK& footIK = footIKSolvers[limbIndex];

    // Leg IK state
    legIK.init(
      rigAttrib->m_rig,
      outputTransforms,
      chainData->m_channelIDs[0],
      chainData->m_channelIDs[1],
      chainData->m_channelIDs[2],
      chainData->m_kneeRotationAxis,
      chainAttrib->m_straightestLegFactor);

    legIK.setRootParentJointTM(curIKSetupLimbState->m_rootParentWorldJointTM);
    legIK.setRootJointTM(curIKSetupLimbState->m_rootWorldJointTM);
    legIK.setMidJointTM(curIKSetupLimbState->m_midWorldJointTM);
    legIK.setEndJointTM(curIKSetupLimbState->m_endWorldJointTM);
    legIK.setStraightestLegLength(curIKSetupLimbState->m_straightestLegLength);

    // Foot IK state
    footIK.init(
      rigAttrib->m_rig,
      outputTransforms,
      chainData->m_numFootJoints,
      &chainData->m_channelIDs[2]);

    footIK.setAnkleParentJointTM(curIKSetupLimbState->m_midWorldJointTM);
    footIK.setAnkleJointTM(curIKSetupLimbState->m_endWorldJointTM);
    footIK.setLocalFootPivotPos(curIKSetupLimbState->m_localFootPivotPos);
  }

  NM_UT_END_PROFILING();
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void unevenTerrainRayCastFromCurrentFootbasePos(
  const NMP::Vector3&                            upAxisWS,
  Network*                                       net,
  const CharacterControllerInterface*            cc,
  const AttribDataBasicUnevenTerrainChain*       chainAttrib,
  const AttribDataBasicUnevenTerrainIKSetup*     inputIKSetupAttrib,
  AttribDataBasicUnevenTerrainFootLiftingTarget* outputFootLiftingTargetAttrib)
{
  NMP_ASSERT(net);
  NMP_ASSERT(cc);
  NMP_ASSERT(chainAttrib);
  NMP_ASSERT(inputIKSetupAttrib);
  NMP_ASSERT(outputFootLiftingTargetAttrib);

  NM_UT_BEGIN_PROFILING("BASIC_UT_RAY_CAST_CUR_FOOTBASE");

  // Compute the ray-casts to recover the terrain hit positions
  uint32_t numLimbs = inputIKSetupAttrib->m_numLimbs;
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "limbIndex = %d\n", limbIndex);
#endif
    const AttribDataBasicUnevenTerrainIKSetup::LimbState* curIKSetupLimbState = inputIKSetupAttrib->m_limbStates[limbIndex];    
    AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData* curFootLiftingLimbData = outputFootLiftingTargetAttrib->m_limbInfo[limbIndex];
    NMP_ASSERT(curIKSetupLimbState);
    NMP_ASSERT(curFootLiftingLimbData);

    // Lifting limits
    float footLiftingUpDist = NMP::maximum(inputIKSetupAttrib->m_rootToHipsHeight * chainAttrib->m_footLiftingHeightLimit, 0.0f);
    float footLiftingDownDist = inputIKSetupAttrib->m_straightestLegLength * chainAttrib->m_footLiftingHeightLimit;

    // Compute the ray start position
    NMP::Vector3 rayStart = curIKSetupLimbState->m_initWorldFootbasePos + (upAxisWS * footLiftingUpDist);

    // Compute the ray cast direction
    float footLiftingDeltaDist = footLiftingUpDist + footLiftingDownDist;
    NMP::Vector3 rayDelta = upAxisWS * -footLiftingDeltaDist;

    // Compute the ray-cast information
    NMP::Vector3 raycastHitPos;
    NMP::Vector3 raycastHitNormal;
    NMP::Vector3 raycastHitVelocity;
    float raycastHitDistance;

    if (cc->castRayIntoCollisionWorld(
          rayStart,
          rayDelta,
          raycastHitDistance,
          raycastHitPos,
          raycastHitNormal,
          raycastHitVelocity,
          net))
    {
      // Set the grounded footbase position
      curFootLiftingLimbData->m_targetWorldFootbaseTerrainPos = raycastHitPos;
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingPos = raycastHitPos;
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingNormal = raycastHitNormal;
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingValid = true;
    }
    else
    {
      // Set the default foot lifting target
      curFootLiftingLimbData->m_targetWorldFootbaseTerrainPos.setToZero();
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingPos = curIKSetupLimbState->m_initWorldFootbasePos;
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingNormal = upAxisWS;
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingValid = false;
    }

#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "CUR POS: valid = %d\n",
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingValid);
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "targetWorldFootbaseLiftingPos = %f, %f, %f\n",
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingPos.x,
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingPos.y,
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingPos.z);
    NET_LOG_MESSAGE(
      NMP::LOG_PRIORITY_ALWAYS,
      "targetWorldFootbaseLiftingNormal = %f, %f, %f\n",
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingNormal.x,
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingNormal.y,
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingNormal.z);
#endif

#ifdef TEST_UT_TASK_DEBUG_DRAW
    MR_DEBUG_DRAW_LINE_GLOBAL(
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingPos, 
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingPos+curFootLiftingLimbData->m_targetWorldFootbaseLiftingNormal, 
      NMP::Colour::GREEN);
#endif
  }

  NM_UT_END_PROFILING();
}
#endif // !NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void unevenTerrainDefaultFootLiftingTargets(
  const NMP::Vector3&                            upAxisWS,
  const AttribDataBasicUnevenTerrainIKSetup*     inputIKSetupAttrib,
  AttribDataBasicUnevenTerrainFootLiftingTarget* outputFootLiftingTargetAttrib)
{
  uint32_t numLimbs = inputIKSetupAttrib->m_numLimbs;
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    const AttribDataBasicUnevenTerrainIKSetup::LimbState* curIKSetupLimbState = inputIKSetupAttrib->m_limbStates[limbIndex];
    AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData* curFootLiftingLimbData = outputFootLiftingTargetAttrib->m_limbInfo[limbIndex];
    NMP_ASSERT(curIKSetupLimbState);
    NMP_ASSERT(curFootLiftingLimbData);

    // Set the default foot lifting target
    curFootLiftingLimbData->m_targetWorldFootbaseTerrainPos.setToZero();
    curFootLiftingLimbData->m_targetWorldFootbaseLiftingPos = curIKSetupLimbState->m_initWorldFootbasePos;
    curFootLiftingLimbData->m_targetWorldFootbaseLiftingNormal = upAxisWS;
    curFootLiftingLimbData->m_targetWorldFootbaseLiftingValid = false;
  }
}
#endif // !NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
namespace
{

//----------------------------------------------------------------------------------------------------------------------
float hipHeightLiftingDelta(
  float               straightestLegLength,
  const NMP::Vector3& worldUpAxis,
  const NMP::Vector3& rootJointPos,
  const NMP::Vector3& endJointPos)
{
  // Assumes that the up axis is a unit vector
  NMP_ASSERT(fabs(1.0f - worldUpAxis.magnitude()) < 1e-4f);

  // Compute the vector from the desired end joint lifting position to the
  // leg IK root joint (source anim pose)
  NMP::Vector3 targetV = rootJointPos - endJointPos;

  // Check if the root joint is within reach of the end joint lifting position
  float hipsDelta = 0.0f;
  float distAdjustedSq = targetV.magnitudeSquared();
  float distTolSq = straightestLegLength * straightestLegLength;
  float c = distAdjustedSq - distTolSq;

  if (c > 1e-4f)
  {
    // Keep the end joint lifting position fixed and allow the leg IK root joint to move vertically
    // (via hip height control). Find the delta in the vertical direction that gives the required
    // straightest leg lifting distance between the IK root joint and the end joint lifting position.
    // ||(x + lambda*v)||^2 = d^2
    // where x is the root position relative to the end joint, lambda is the translation scalar in the
    // world up axis direction v and d is the straightest leg length limit.
    float b = 2.0f * targetV.dot(worldUpAxis);
    float disc = b * b - 4 * c;
    if (disc > 0)
    {
      // Compute the two roots
      float lambda[2];
      float signVal = NMP::floatSelect(b, 1.0f, -1.0f);
      lambda[0] = -0.5f * (b + signVal * sqrtf(disc));
      lambda[1] = c / lambda[0];

      // Find the smaller of the two roots
      float d = NMP::nmfabs(lambda[0]) - NMP::nmfabs(lambda[1]);
      hipsDelta = NMP::floatSelect(d, lambda[1], lambda[0]);
    }
  }

  return hipsDelta;
}

//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void unevenTerrainMotionSolve(
  const NMP::Vector3&                            upAxisWS,
  UnevenTerrainHipsIK*                           hipsIKSolver,
  UnevenTerrainLegIK*                            legIKSolvers,
  UnevenTerrainFootIK*                           footIKSolvers,
  const AttribDataTransformBuffer*               inputTransformsAttrib,
  const AttribDataUpdatePlaybackPos*             networkUpdateTimeAttrib,
  const AttribDataBasicUnevenTerrainSetup*       setupAttrib,
  const AttribDataBasicUnevenTerrainChain*       chainAttrib,
  const AttribDataBasicUnevenTerrainIKSetup*     inputIKSetupAttrib,
  const AttribDataBasicUnevenTerrainFootLiftingTarget* inputFootLiftingTargetAttrib,
  const AttribDataFloat*                         ikHipsWeightAttrib,
  AttribDataBasicUnevenTerrainIKState*           ikStateAttrib)
{
  NMP_ASSERT(hipsIKSolver);
  NMP_ASSERT(legIKSolvers);
  NMP_ASSERT(footIKSolvers);
  NMP_ASSERT(inputTransformsAttrib);
  NMP_ASSERT(networkUpdateTimeAttrib);
  NMP_ASSERT(setupAttrib);
  NMP_ASSERT(chainAttrib);
  NMP_ASSERT(inputIKSetupAttrib);
  NMP_ASSERT(inputFootLiftingTargetAttrib);
  NMP_ASSERT(ikHipsWeightAttrib);
  NMP_ASSERT(ikStateAttrib);

  NM_UT_BEGIN_PROFILING("BASIC_UT_MOTION_SOLVE");

  uint32_t numLimbs = inputIKSetupAttrib->m_numLimbs;
  bool isValidInputIKState = ikStateAttrib->m_isValidData;
  const NMP::Matrix34& curWorldTM = inputIKSetupAttrib->m_characterRootWorldTM;

  // Network update time
  float deltaTime = 0.0f;
  float recipDeltaTime = 0.0f;
  if (!networkUpdateTimeAttrib->m_isAbs)
  {
    NMP_ASSERT(!networkUpdateTimeAttrib->m_isFraction);
    deltaTime = networkUpdateTimeAttrib->m_value;
    NMP_ASSERT(deltaTime > 0.0f);
    recipDeltaTime = 1.0f / deltaTime;
  }

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "deltaTime = %f, recipDeltaTime = %f\n", deltaTime, recipDeltaTime);
#endif

  // Note that the velocity and acceleration controller updates the position of the ankle based on the
  // discretized approximation for the velocity and acceleration between frame positions. Because of
  // this discrete approximation, no two motion paths will be the same if you change the delta time update
  // step of the network. Although a quadratic approximation is used to model the motion between frame
  // samples there is still a noticeable difference in the responses between the frame rate extremes.
  // To account for this we generate some bias factors for the limits depending on the current frame rate.
  float bias = 1.0f;
  if (recipDeltaTime > 60.0f)
  {
    // Only apply bias for frame rates in excess of 60 fps
    float u = (recipDeltaTime - 60.0f) / (300.0f - 60.0f); // Linear interpolant between 60 and 300 fps
    bias = (1 - u) + 3.0f * u; // Linearly interpolated bias factor between 1 and 3
  }

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "bias = %f\n", bias);
#endif

  //---------------------------
  // Old hip control state data
  NMP::Vector3 drivenHipsPosCSOld = ikStateAttrib->m_drivenHipsPosCS;
  NMP::Vector3 drivenHipsPosVelCSOld = ikStateAttrib->m_drivenHipsPosVelCS;
  NMP::Vector3 targetHipsPosCSOld = ikStateAttrib->m_targetHipsPosCS;
  NMP::Vector3 targetHipsPosVelCSOld = ikStateAttrib->m_targetHipsPosVelCS;

  //---------------------------
  // Compute an estimate for the hips height control delta
  float hipsDelta[2];
  float hipDeltaMin = 0.0f;
  if (isValidInputIKState && chainAttrib->m_hipsHeightControlEnable)
  {
    NMP::Vector3 drivenWorldHipsPosEstimate;
    curWorldTM.transformVector(drivenHipsPosCSOld, drivenWorldHipsPosEstimate);
    NMP::Vector3 deltaV = drivenWorldHipsPosEstimate - inputIKSetupAttrib->m_hipsWorldJointTM.translation();
    hipDeltaMin = upAxisWS.dot(deltaV);
  }
  NMP::Vector3 hipsDeltaPos = upAxisWS * hipDeltaMin; // delta in world frame

  //---------------------------------------------------------------------------------
  // Foot position and alignment to lifting target
  //---------------------------------------------------------------------------------
  bool groundPenetrationFixupFlags[2] = {false, false};

  NM_UT_BEGIN_PROFILING("BASIC_UT_LIFTING_END_JOINT_TARGET");
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    const AttribDataBasicUnevenTerrainIKSetup::LimbState* curIKSetupLimbState = inputIKSetupAttrib->m_limbStates[limbIndex];
    const AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData* curFootLiftingLimbData = inputFootLiftingTargetAttrib->m_limbInfo[limbIndex];
    AttribDataBasicUnevenTerrainIKState::LimbState* curIKStateLimbState = ikStateAttrib->m_limbStates[limbIndex];    
    NMP_ASSERT(curIKSetupLimbState);
    NMP_ASSERT(curFootLiftingLimbData);
    NMP_ASSERT(curIKStateLimbState);
    UnevenTerrainFootIK& footIK = footIKSolvers[limbIndex];

    // Set the output state target foot lifting target
    if (curFootLiftingLimbData->m_targetWorldFootbaseLiftingValid)
    {
      // Set the IK foot lifting target to the calculated target foot lifting position.
      curIKStateLimbState->m_targetWorldFootbaseLiftingPos = curFootLiftingLimbData->m_targetWorldFootbaseLiftingPos;
    }
    else
    {
      // Set the IK foot lifting target to the initial footbase pose but modified by the
      // estimated hips height delta.
      curIKStateLimbState->m_targetWorldFootbaseLiftingPos = curIKSetupLimbState->m_initWorldFootbasePos + hipsDeltaPos;
    }

    // Compute the transform that positions and orients the footbase with the terrain surface.
    // This updates the ankle joint TM with the surface alignment transformation.
    footIK.terrainSurfaceAlignmentTransform(
      upAxisWS,
      inputIKSetupAttrib->m_footPreAlignOffsetQuat,
      curIKStateLimbState->m_targetWorldFootbaseLiftingPos,
      curFootLiftingLimbData->m_targetWorldFootbaseLiftingNormal,
      curIKSetupLimbState->m_initWorldFootbasePos,
      chainAttrib->m_footAlignToSurfaceAngleLimit,
      chainAttrib->m_footAlignToSurfaceMaxSlopeAngle);

    // Cosine of the angle limit
    NMP_ASSERT(
      chainAttrib->m_footAlignToSurfaceMaxSlopeAngle >= 0.0f &&
      chainAttrib->m_footAlignToSurfaceMaxSlopeAngle <= 1.0f);

    // Update the world foot pivot position
    footIK.fkWorldFootPivotPos();
  }
  NM_UT_END_PROFILING(); // BASIC_UT_LIFTING_END_JOINT_TARGET

  //---------------------------
  // End joint positional motion clamping in the vertical direction
  NM_UT_BEGIN_PROFILING("BASIC_UT_LIFTING_END_JOINT_MOTION_CLAMP");
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    const AttribDataBasicUnevenTerrainIKSetup::LimbState* curIKSetupLimbState = inputIKSetupAttrib->m_limbStates[limbIndex];
    const AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData* curFootLiftingLimbData = inputFootLiftingTargetAttrib->m_limbInfo[limbIndex];
    AttribDataBasicUnevenTerrainIKState::LimbState* curIKStateLimbState = ikStateAttrib->m_limbStates[limbIndex];
    NMP_ASSERT(curIKSetupLimbState);
    NMP_ASSERT(curFootLiftingLimbData);
    NMP_ASSERT(curIKStateLimbState);
    UnevenTerrainLegIK& legIK = legIKSolvers[limbIndex];
    UnevenTerrainFootIK& footIK = footIKSolvers[limbIndex];
    NMP::Matrix34& endJointTM = footIK.getAnkleJointTM(); // footIK end joint gets modified directly

    //---------------------------
    // Old end joint state data
    NMP::Vector3 drivenEndJointPosWSOld = curIKStateLimbState->m_drivenWorldAnkleJointTM.translation();
    NMP::Vector3 targetEndJointPosWSOld = curIKStateLimbState->m_targetWorldAnkleJointTM.translation();
    NMP::Vector3 drivenEndJointPosCSOld = curIKStateLimbState->m_drivenEndJointPosCS;
    NMP::Vector3 drivenEndJointPosVelCSOld = curIKStateLimbState->m_drivenEndJointPosVelCS;
    NMP::Vector3 targetEndJointPosCSOld = curIKStateLimbState->m_targetEndJointPosCS;
    NMP::Vector3 targetEndJointPosVelCSOld = curIKStateLimbState->m_targetEndJointPosVelCS;
    NMP::Vector3 drivenWorldFootbaseLiftingPosOld = curIKStateLimbState->m_drivenWorldFootbaseLiftingPos;

    //---------------------------
    float liftingUpDist = NMP::maximum(inputIKSetupAttrib->m_rootToHipsHeight * chainAttrib->m_footLiftingHeightLimit, 0.0f);

    // Pass 1: Clamp the foot lifting of the end joint to the maximum lifting height
    NMP::Vector3 rootToEndJointV = endJointTM.translation() - curWorldTM.translation();
    // Vertical component in the direction of lifting
    float lambda = rootToEndJointV.dot(upAxisWS);
    // Clamp
    if (lambda > liftingUpDist)
    {
#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "limbIndex = %d, Pass1: Clamping end joint to the maximum lifting height.\n",
        limbIndex);
#endif
      NMP::Vector3 deltaV = upAxisWS * (liftingUpDist - lambda);
      endJointTM.translation() += deltaV;
    }

    //---------------------------
    // End joint positional motion clamping
    if (isValidInputIKState)
    {
      // During a foot contact phase apply vertical joint control in worldspace if possible
      bool jointControlInWorld = curFootLiftingLimbData->m_isFootInContactPhase && inputIKSetupAttrib->m_enableIK;

      // Check if the vector between the last frames end joint position and this frames
      // lifted position is within the position delta maximum
      NMP::Vector3 endJointPrevToCurPosV = endJointTM.translation() - drivenEndJointPosWSOld;
      float posDeltaTol = chainAttrib->m_footLiftingHeightLimit * legIK.getStraightestLegLength();
      if (endJointPrevToCurPosV.magnitudeSquared() > posDeltaTol * posDeltaTol)
      {
#ifdef TEST_UT_TASK_LOGGING
        NET_LOG_MESSAGE(
          NMP::LOG_PRIORITY_ALWAYS,
          "limbIndex = %d, Vector between the last frames end joint position and this frames position is too large. Resetting end joint to animation pose.\n",
          limbIndex);
#endif
        // Position delta is too large. Reset the end joint orientation to the source anim configuration.
        endJointTM = curIKSetupLimbState->m_endWorldJointTM;

        // Since we are blending back to the animation source pose, ensure that we apply
        // joint control in character space.
        jointControlInWorld = false;
      }

      // Set the target ankle joint transform for foot lifting
      curIKStateLimbState->m_targetWorldAnkleJointTM = endJointTM;

      // Compute the target IK end joint position in character space
      curWorldTM.inverseTransformVector(endJointTM.translation(), curIKStateLimbState->m_targetEndJointPosCS);

      NM_UT_BEGIN_PROFILING("BASIC_UT_JOINT_POS_CTRL");
      const float weightBetweenPosAndVel = 0.4f;
      const float jointPosVelMaxInAxis = chainAttrib->m_endJointPosVelLimit * inputIKSetupAttrib->m_straightestLegLength;
      const float jointPosAccelMaxInAxis = bias * chainAttrib->m_endJointPosAccelLimit * inputIKSetupAttrib->m_straightestLegLength;

      if (jointControlInWorld)
      {
        // Clamp the positional velocity and acceleration of the end joint. We do this in worldspace during a foot
        // contact phase so that any sudden movements of the character controller are not considered when updating
        // the driven position of the end joint.
        //
        // During contact the foot should appear to remain stationary at the target position. If we apply
        // control in character space while the foot is grounded, and the character controller suddenly
        // moves up a step then the complementary motion of the foot target appears to move suddenly down.
        // This is likely to be motion clamped, resulting in the foot rising off the floor.
        //
        // We assume that the previous driven foot velocity is zero (a fair assumption during a foot plant)
        // to avoid the problem whenever the ankle joint motion is clamped due to violation of the leg reach
        // limits. Should the character controller suddenly jump upwards when moving between steps then it
        // is likely that the leg reach limit is exceeded, resulting in this global upward velocity being
        // transfered to the end joint controller. This upward motion of the ankle joint would continue
        // until the controller can recover the tracking of the target motion.
        NMP::Vector3 curTargetEndJointPosVelWorld;
        NMP::Vector3 curDrivenEndJointPosWorld;
        NMP::Vector3 curDrivenEndJointPosVelWorld;
        NMP::Vector3 vZero(NMP::Vector3::InitZero);

        // Compute the target velocity. Apply velocity and acceleration motion clamping to
        // limit the target velocity to the specified limits. Since this has been marked up
        // as a foot plant event, we assume zero previous velocity to bias the target towards
        // stationary motion.
        jointPosVelControlInAxis(
          deltaTime,
          chainAttrib->m_endJointPosVelLimitEnable,
          jointPosVelMaxInAxis,
          chainAttrib->m_endJointPosAccelLimitEnable,
          jointPosAccelMaxInAxis,
          upAxisWS,
          targetEndJointPosWSOld,         // IN: Previous target position
          vZero,                          // IN: Assume zero previous target velocity
          endJointTM.translation(),       // IN: Current target joint position
          curTargetEndJointPosVelWorld);  // OUT: Current target joint velocity (motion clamped)

        // Compute the driven joint motion. Apply velocity and acceleration motion clamping to
        // limit the driven joint motion. Assume zero previous driven joint velocity to bias the
        // driven joint towards stationary motion.
        jointPosControlInAxis(
          deltaTime,
          chainAttrib->m_endJointPosVelLimitEnable,
          jointPosVelMaxInAxis,
          chainAttrib->m_endJointPosAccelLimitEnable,
          jointPosAccelMaxInAxis,
          weightBetweenPosAndVel,
          upAxisWS,
          targetEndJointPosWSOld,         // IN: Previous target position
          drivenEndJointPosWSOld,         // IN: Previous driven position
          vZero,                          // IN: Assume zero previous driven velocity
          endJointTM.translation(),       // IN: Current target joint position
          curTargetEndJointPosVelWorld,   // IN: Current target joint velocity
          curDrivenEndJointPosWorld,      // OUT: Current driven joint position
          curDrivenEndJointPosVelWorld);  // OUT: Current driven joint velocity

#ifdef TEST_UT_TASK_LOGGING
        NET_LOG_MESSAGE(
          NMP::LOG_PRIORITY_ALWAYS,
          "limbIndex = %d, Applying vertical joint position control in character space.\n",
          limbIndex);

        NET_LOG_MESSAGE(
          NMP::LOG_PRIORITY_ALWAYS,
          "limbIndex = %d, prevDrivenEndJointPosWorld = %f %f %f, curDrivenEndJointPosWorld = %f %f %f\n",
          limbIndex,
          drivenEndJointPosWSOld.x,
          drivenEndJointPosWSOld.y,
          drivenEndJointPosWSOld.z,
          curDrivenEndJointPosWorld.x,
          curDrivenEndJointPosWorld.y,
          curDrivenEndJointPosWorld.z);
#endif

        // Update the worldspace end joint position
        endJointTM.translation() = curDrivenEndJointPosWorld;

        // Target IK end joint position velocity in character space
        curIKStateLimbState->m_targetEndJointPosVelCS.setToZero();
      }
      else
      {
        // Clamp the positional velocity and acceleration of the end joint. We do this in character space
        // so that the movement of the character controller is not considered while limiting the limb velocities:
        //
        // - The character could have walked over the edge of a cliff and is falling through the air. In which case
        //   the global motion would be accelerating downward very fast, thus clamping in worldspace would mean the
        //   legs would be dragged behind as the character falls.
        //
        // - Other artifacts of clamping in worldspace are apparent whenever the ankle joint motion is clamped
        //   due to violation of the leg reach limits. Should the character controller suddenly jump upwards when
        //   moving between steps then it is likely that the leg reach limit is exceeded, resulting in this global
        //   upward velocity being transfered to the end joint controller. This upward motion of the ankle joint
        //   continues until the controller can recover the tracking of the target motion.
        NMP::Vector3 drivenEndJointPosCS;
        NMP::Vector3 drivenEndJointPosVelCS;

        // Compute the target velocity. Apply velocity and acceleration motion clamping to
        // limit the target velocity to the specified limits.
        jointPosVelControlInAxis(
          deltaTime,
          chainAttrib->m_endJointPosVelLimitEnable,
          jointPosVelMaxInAxis,
          chainAttrib->m_endJointPosAccelLimitEnable,
          jointPosAccelMaxInAxis,
          upAxisWS,
          targetEndJointPosCSOld,                         // IN: Previous target position
          targetEndJointPosVelCSOld,                      // IN: Previous target velocity
          curIKStateLimbState->m_targetEndJointPosCS,      // IN: Current target joint position
          curIKStateLimbState->m_targetEndJointPosVelCS);  // OUT: Current target joint velocity (motion clamped)

        // Compute the smoothed target velocity estimate. It can be quite important to provide
        // some form of smoothing for non-uniform time updates. The target velocities can vary
        // considerably for similar displacement step responses with different delta time updates.
        NMP::Vector3 targetEndJointPosVelCS = (curIKStateLimbState->m_targetEndJointPosVelCS + targetEndJointPosVelCSOld) * 0.5f;

        // Compute the driven joint motion. Apply velocity and acceleration motion clamping to
        // limit the driven joint motion. Overshoot detection is also used to prevent the
        // driven joint motion looking under-damped or becoming simple harmonic.
        jointPosControlInAxis(
          deltaTime,
          chainAttrib->m_endJointPosVelLimitEnable,
          jointPosVelMaxInAxis,
          chainAttrib->m_endJointPosAccelLimitEnable,
          jointPosAccelMaxInAxis,
          weightBetweenPosAndVel,
          setupAttrib->m_upAxis,
          targetEndJointPosCSOld,                       // IN: Previous target joint position.
          drivenEndJointPosCSOld,                       // IN: Previous driven joint position.
          drivenEndJointPosVelCSOld,                    // IN: Previous driven joint velocity.
          curIKStateLimbState->m_targetEndJointPosCS,    // IN: Current target joint position
          targetEndJointPosVelCS,                       // IN: Current target joint velocity estimate
          drivenEndJointPosCS,                          // OUT: Current driven joint position
          drivenEndJointPosVelCS);                      // OUT: Current driven joint velocity

#ifdef TEST_UT_TASK_LOGGING
        NET_LOG_MESSAGE(
          NMP::LOG_PRIORITY_ALWAYS,
          "limbIndex = %d, Applying vertical joint position control in character space.\n",
          limbIndex);

        NET_LOG_MESSAGE(
          NMP::LOG_PRIORITY_ALWAYS,
          "limbIndex = %d, prevTargetEndJointPosCS = %f %f %f, curTargetEndJointPosCS = %f %f %f\n",
          limbIndex,
          targetEndJointPosCSOld.x,
          targetEndJointPosCSOld.y,
          targetEndJointPosCSOld.z,
          curIKStateLimbState->m_targetEndJointPosCS.x,
          curIKStateLimbState->m_targetEndJointPosCS.y,
          curIKStateLimbState->m_targetEndJointPosCS.z);

        NET_LOG_MESSAGE(
          NMP::LOG_PRIORITY_ALWAYS,
          "limbIndex = %d, prevDrivenEndJointPosCS = %f %f %f, curDrivenEndJointPosCS = %f %f %f\n",
          limbIndex,
          drivenEndJointPosCSOld.x,
          drivenEndJointPosCSOld.y,
          drivenEndJointPosCSOld.z,
          drivenEndJointPosCS.x,
          drivenEndJointPosCS.y,
          drivenEndJointPosCS.z);
#endif

        // Update the worldspace end joint position
        curWorldTM.transformVector(drivenEndJointPosCS, endJointTM.translation());
      }
      NM_UT_END_PROFILING();

      //---------------------------
      // Pass 2: Clamp the foot lifting of the end joint to the maximum lifting height
      rootToEndJointV = endJointTM.translation() - curWorldTM.translation();
      // Vertical component in the direction of lifting
      lambda = rootToEndJointV.dot(upAxisWS);
      // Clamp
      if (lambda > liftingUpDist)
      {
#ifdef TEST_UT_TASK_LOGGING
        NET_LOG_MESSAGE(
          NMP::LOG_PRIORITY_ALWAYS,
          "limbIndex = %d, Pass2: Clamping end joint to the maximum lifting height.\n",
          limbIndex);
#endif
        // Clamp the maximum height of the end joint. Note there is no need to clamp the
        // corresponding end joint velocity since this will be recomputed again after
        // performing the leg IK.
        NMP::Vector3 deltaV = upAxisWS * (liftingUpDist - lambda);
        endJointTM.translation() += deltaV;
      }

      //---------------------------
      // Ground penetration fix-up
      if (chainAttrib->m_useGroundPenetrationFixup && curFootLiftingLimbData->m_targetWorldFootbaseLiftingValid)
      {
        footIK.fkWorldFootPivotPos();

        // Compute the driven footbase lifting point by projecting the foot pivot point
        // vertically onto the footbase plane.
        footIK.computeFootbaseLiftingPos(
          upAxisWS,
          curIKSetupLimbState->m_endWorldJointTM, // The initial animation pose ankle joint TM
          curIKSetupLimbState->m_footbaseDistFromPivotPos, // The distance of the footbase from the pivot point
          curIKStateLimbState->m_drivenWorldFootbaseLiftingPos,  // OUT
          curIKStateLimbState->m_drivenWorldFootbaseLiftingNormal); // OUT

        // Compute the previous and current footbase lifting positions relative to the current
        // terrain ray-cast hit position.
        NMP::Vector3 terrainHitPosToFootbasePos = curIKStateLimbState->m_drivenWorldFootbaseLiftingPos - curFootLiftingLimbData->m_targetWorldFootbaseTerrainPos;
        NMP::Vector3 terrainHitPosToFootbasePosOld = drivenWorldFootbaseLiftingPosOld - curFootLiftingLimbData->m_targetWorldFootbaseTerrainPos;

        // Check if the foot has transitioned from above the terrain surface to below it.
        float curFootbaseDeltaInAxis = terrainHitPosToFootbasePos.dot(upAxisWS);
        float prevFootbaseDeltaInAxis = terrainHitPosToFootbasePosOld.dot(upAxisWS);
        if (prevFootbaseDeltaInAxis > 1e-3f && curFootbaseDeltaInAxis < -1e-3f)
        {
          // Clamp the foot back onto the terrain surface
          endJointTM.translation() -= upAxisWS * curFootbaseDeltaInAxis;

          // Zero the vertical component of the driven end joint velocity
          // after performing leg IK. We need to do this since clamping the
          // foot back onto the surface in worldspace can lead to significant
          // ankle velocity in character space, especially if the character
          // root has made a sudden vertical movement.
          groundPenetrationFixupFlags[limbIndex] = true;

#ifdef TEST_UT_TASK_LOGGING
          NET_LOG_MESSAGE(
            NMP::LOG_PRIORITY_ALWAYS,
            "limbIndex = %d, Ground penetration fix-up: Clamping foot back onto terrain surface.\n",
            limbIndex);
#endif
        }
      }
    }
    else
    {
      // Set the target ankle joint transform for foot lifting
      curIKStateLimbState->m_targetWorldAnkleJointTM = endJointTM;

      // Compute the target IK end joint position in character space
      curWorldTM.inverseTransformVector(endJointTM.translation(), curIKStateLimbState->m_targetEndJointPosCS);

      // Target IK end joint position velocity in character space
      curIKStateLimbState->m_targetEndJointPosVelCS.setToZero();
    }

    // Update the Leg IK end joint transform for foot lifting
    legIK.setEndJointTM(endJointTM);
  }
  NM_UT_END_PROFILING(); // BASIC_UT_LIFTING_END_JOINT_MOTION_CLAMP

  //---------------------------------------------------------------------------------
  // Hip height control
  //---------------------------------------------------------------------------------
  if (chainAttrib->m_hipsHeightControlEnable)
  {
    NM_UT_BEGIN_PROFILING("BASIC_UT_HIP_HEIGHT_CTRL");
#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "Hip height control\n");
#endif

    NMP::Matrix34& hipsTM = hipsIKSolver->getHipsJointTM(); // Source pose hips TM

    for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
    {
      AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData* curFootLiftingLimbData = inputFootLiftingTargetAttrib->m_limbInfo[limbIndex];

#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "limbIndex = %d\n", limbIndex);
#endif
      // Check if the character controller is still grounded and has achieved
      // its requested motion. Also check if the foot lifting target is valid.
      // i.e. if the foot lifting target is out of reach then do not let its
      // lifted position affect the hip height control.
      if (inputIKSetupAttrib->m_enableIK && curFootLiftingLimbData->m_targetWorldFootbaseLiftingValid)
      {
        const UnevenTerrainLegIK& legIK = legIKSolvers[limbIndex];
        const NMP::Matrix34& rootJointTM = legIK.getRootJointTM();
        const NMP::Matrix34& endJointTM = legIK.getEndJointTM();

        // Tolerances
        const float straightestAllowableLegLength = legIK.getStraightestAllowableLegLength();
        const float maxHipDelta = 0.5f * straightestAllowableLegLength;

#ifdef TEST_UT_TASK_LOGGING
        NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "straightestAllowableLegLength = %f\n", straightestAllowableLegLength);
        NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "maxHipDelta = %f\n", maxHipDelta);
#endif
        // Compute the hips upward vertical lifting delta for this leg
        hipsDelta[limbIndex] = hipHeightLiftingDelta(
          straightestAllowableLegLength,
          upAxisWS,
          rootJointTM.translation(),
          endJointTM.translation());

#ifdef TEST_UT_TASK_LOGGING
        NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "hipsDelta[%d] = %f\n", limbIndex, hipsDelta[limbIndex]);
#endif
        // Ensure that the hip delta is within a suitable range
        if (fabs(hipsDelta[limbIndex]) > maxHipDelta)
        {
#ifdef TEST_UT_TASK_LOGGING
          NET_LOG_MESSAGE(
            NMP::LOG_PRIORITY_ALWAYS,
            "hipDelta[%d] = %f > maxHipDelta = %f : Clamping to max\n",
            limbIndex,
            hipsDelta[limbIndex],
            maxHipDelta);
#endif
          // Note: we clamp to the maximum hip lifting delta rather than resetting to zero since
          // resetting can cause high frequency oscillation when the lifting delta bounces between
          // the maximum lifting limit and zero. If the foot plant is truly out of reach then
          // the ray-cast step will reset the foot lifting target back to the animation position.
          hipsDelta[limbIndex] = -maxHipDelta;
        }
      }
      else
      {
        // Blend back towards the source animation hip position
        hipsDelta[limbIndex] = 0.0f;
      }
    }

    // Compute the largest hip lifting delta
    hipDeltaMin = NMP::minimum(hipsDelta[0], hipsDelta[1]);

#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\nhipsDeltaMin = %f\n", hipDeltaMin);
#endif

    // Update the worldspace hips position
    hipsDeltaPos = upAxisWS * hipDeltaMin; // delta in world frame
    hipsTM.translation() = inputIKSetupAttrib->m_hipsWorldJointTM.translation() + hipsDeltaPos;
    // Compute the target IK hips position in character space
    curWorldTM.inverseTransformVector(hipsTM.translation(), ikStateAttrib->m_targetHipsPosCS);

    // Hips positional motion clamping
    if (isValidInputIKState)
    {
      // Clamp the positional acceleration of the hips joint. We do this in character space so that the
      // movement of the character controller is not considered while limiting the limb velocities.
      // i.e. the character could have walked over the edge of a cliff and is falling through the air.
      const float weightBetweenPosAndVel = 0.4f;
      const float hipsPosVelMaxInAxis = chainAttrib->m_hipsPosVelLimit * inputIKSetupAttrib->m_straightestLegLength;
      const float hipsPosAccelMaxInAxis = bias * chainAttrib->m_hipsPosAccelLimit * inputIKSetupAttrib->m_straightestLegLength;

      NM_UT_BEGIN_PROFILING("BASIC_UT_HIP_HEIGHT_POS_CTRL");

      // Compute the target velocity. Apply velocity and acceleration motion clamping to
      // limit the target velocity to the specified limits.
      jointPosVelControlInAxis(
        deltaTime,
        chainAttrib->m_hipsPosVelLimitEnable,
        hipsPosVelMaxInAxis,
        chainAttrib->m_hipsPosAccelLimitEnable,
        hipsPosAccelMaxInAxis,
        upAxisWS,
        targetHipsPosCSOld,                   // IN: Previous target position
        targetHipsPosVelCSOld,                // IN: Previous target velocity
        ikStateAttrib->m_targetHipsPosCS,     // IN: Current target joint position
        ikStateAttrib->m_targetHipsPosVelCS); // OUT: Current target joint velocity (motion clamped)

      // Compute the smoothed target velocity estimate. It can be quite important to provide
      // some form of smoothing for non-uniform time updates. The target velocities can vary
      // considerably for similar displacement step responses with different delta time updates.
      NMP::Vector3 targetHipsPosVelCS = (ikStateAttrib->m_targetHipsPosVelCS + targetHipsPosVelCSOld) * 0.5f;

      // Compute the driven joint motion. Apply velocity and acceleration motion clamping to
      // limit the driven joint motion. Overshoot detection is also used to prevent the
      // driven joint motion looking under-damped or becoming simple harmonic.
      jointPosControlInAxis(
        deltaTime,
        chainAttrib->m_hipsPosVelLimitEnable,
        hipsPosVelMaxInAxis,
        chainAttrib->m_hipsPosAccelLimitEnable,
        hipsPosAccelMaxInAxis,
        weightBetweenPosAndVel,
        setupAttrib->m_upAxis,
        targetHipsPosCSOld,                   // IN: Previous target joint position.
        drivenHipsPosCSOld,                   // IN: Previous driven joint position.
        drivenHipsPosVelCSOld,                // IN: Previous driven joint velocity.
        ikStateAttrib->m_targetHipsPosCS,     // IN: Current target joint position
        targetHipsPosVelCS,                   // IN: Current target joint velocity estimate
        ikStateAttrib->m_drivenHipsPosCS,     // OUT: Current driven joint position
        ikStateAttrib->m_drivenHipsPosVelCS); // OUT: Current driven joint velocity

      NM_UT_END_PROFILING();

#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "m_drivenHipsPosCS = %f %f %f\n", ikStateAttrib->m_drivenHipsPosCS.x, ikStateAttrib->m_drivenHipsPosCS.y, ikStateAttrib->m_drivenHipsPosCS.z);
      NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "m_drivenHipsPosVelCS = %f %f %f\n", ikStateAttrib->m_drivenHipsPosVelCS.x, ikStateAttrib->m_drivenHipsPosVelCS.y, ikStateAttrib->m_drivenHipsPosVelCS.z);
#endif
    }
    else
    {
      ikStateAttrib->m_drivenHipsPosCS = ikStateAttrib->m_targetHipsPosCS;
      ikStateAttrib->m_drivenHipsPosVelCS.setToZero();
      ikStateAttrib->m_targetHipsPosVelCS.setToZero();
    }

    // Update the worldspace hips position
    curWorldTM.transformVector(ikStateAttrib->m_drivenHipsPosCS, hipsTM.translation());

    // Set the output local-space hip position
    hipsIKSolver->solve();

    // Apply the IK Hips blend weight
    float weight = NMP::clampValue(ikHipsWeightAttrib->m_value, 0.0f, 1.0f);
    if (weight < (1.0f - 1e-4f))
    {
      hipsIKSolver->blendWithBuffer(inputTransformsAttrib->m_transformBuffer, weight);

      // Update the Hips IK transform
      hipsIKSolver->fkHipsJointTM();
    }

    // Update the Leg IK root transforms
    for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
    {
      UnevenTerrainLegIK& legIK = legIKSolvers[limbIndex];
      legIK.fkRootParentJointTM(hipsTM, chainAttrib->m_hipsChannelID);
    }

    NM_UT_END_PROFILING(); // BASIC_UT_HIP_HEIGHT_CTRL
  }

  //---------------------------------------------------------------------------------
  // Leg IK
  //---------------------------------------------------------------------------------
  NM_UT_BEGIN_PROFILING("BASIC_UT_LEG_IK");
#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "Leg IK\n");
#endif
  for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
  {
    const AttribDataBasicUnevenTerrainIKSetup::LimbState* curIKSetupLimbState = inputIKSetupAttrib->m_limbStates[limbIndex];
    const AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData* curFootLiftingLimbData = inputFootLiftingTargetAttrib->m_limbInfo[limbIndex];
    AttribDataBasicUnevenTerrainIKState::LimbState* curIKStateLimbState = ikStateAttrib->m_limbStates[limbIndex];
    NMP_ASSERT(curIKSetupLimbState);
    NMP_ASSERT(curFootLiftingLimbData);
    NMP_ASSERT(curIKStateLimbState);
    UnevenTerrainLegIK& legIK = legIKSolvers[limbIndex];
    UnevenTerrainFootIK& footIK = footIKSolvers[limbIndex];

    //---------------------------
    // Old end joint state data
    NMP::Quat drivenEndJointQuatOld = curIKStateLimbState->m_drivenEndJointQuat;
    NMP::Vector3 drivenEndJointAngVelOld = curIKStateLimbState->m_drivenEndJointAngVel;
    NMP::Quat targetEndJointQuatOld = curIKStateLimbState->m_targetEndJointQuat;
    NMP::Vector3 targetEndJointAngVelOld = curIKStateLimbState->m_targetEndJointAngVel;
    // Pos
    NMP::Vector3 drivenEndJointPosCSOld = curIKStateLimbState->m_drivenEndJointPosCS;

    //---------------------------
    // If the end joint target is out of reach then adjust this target by lifting vertically
    // until it is within reach again. Note that the behaviour of the leg IK solve will align
    // the root to end joint vector with the target when out of reach. We adjust the target
    // vertically to prevent foot jitter due to the changing alignment of this root to end
    // joint vector. This sort of jitter is most noticeable when updating with with variable
    // update delta times.
    NMP::Matrix34& endJointTM = legIK.getEndJointTM();
    NMP::Vector3 worldRootPos = legIK.fkRootJointPos();

    const float straightestAllowableLegLength = legIK.getStraightestAllowableLegLength();
    const float legLiftingDelta = hipHeightLiftingDelta(
      straightestAllowableLegLength,
      upAxisWS,
      worldRootPos,
      endJointTM.translation());

#ifdef TEST_UT_TASK_LOGGING
    NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "limbIndex %d: End joint target adjustment. legLiftingDelta = %f\n", limbIndex, legLiftingDelta);
#endif
    endJointTM.translation() -= (upAxisWS * legLiftingDelta);

    //---------------------------
    // Perform the Leg IK solve to get the end joint into the target position
    NM_UT_BEGIN_PROFILING("BASIC_UT_LEG_IK_SOLVE1");
    legIK.solve();
    NM_UT_END_PROFILING();

    //---------------------------
    // Local end joint angular motion clamping
    curIKStateLimbState->m_targetEndJointQuat = legIK.getEndChannelQuat();
    curIKStateLimbState->m_targetEndJointQuat.normalise();

    if (isValidInputIKState)
    {
      // Perform the end joint angular acceleration clamping
      NM_UT_BEGIN_PROFILING("BASIC_UT_JOINT_ANG_CTRL");

      const float weightBetweenAngAndVel = 0.4f;
      const float endJointAngVelLimit = chainAttrib->m_endJointAngVelLimit;
      const float endJointAngAccelLimit = bias * chainAttrib->m_endJointAngAccelLimit;

      // Compute the target angular velocity. Apply velocity and acceleration motion clamping to
      // limit the target velocity to the specified limits.
      jointAngVelControl(
        deltaTime,
        chainAttrib->m_endJointAngVelLimitEnable,
        endJointAngVelLimit,
        chainAttrib->m_endJointAngAccelLimitEnable,
        endJointAngAccelLimit,
        targetEndJointQuatOld,                        // IN: Previous target orientation
        targetEndJointAngVelOld,                      // IN: Previous target velocity
        curIKStateLimbState->m_targetEndJointQuat,     // IN: Current target joint orientation
        curIKStateLimbState->m_targetEndJointAngVel);  // OUT: Current target joint angular velocity (motion clamped)

      // Compute the smoothed target angular velocity estimate. It can be quite important to provide
      // some form of smoothing for non-uniform time updates. The target velocities can vary
      // considerably for similar displacement step responses with different delta time updates.
      NMP::Vector3 targetEndJointAngVel = (curIKStateLimbState->m_targetEndJointAngVel + targetEndJointAngVelOld) * 0.5f;      

      // Compute the driven joint motion. Apply angular velocity and acceleration motion clamping
      // to limit the driven joint motion. Overshoot detection is also used to prevent the
      // driven joint motion looking under-damped or becoming simple harmonic.
      jointAngControl(
        deltaTime,
        chainAttrib->m_endJointAngVelLimitEnable,
        endJointAngVelLimit,
        chainAttrib->m_endJointAngAccelLimitEnable,
        endJointAngAccelLimit,
        weightBetweenAngAndVel,
        targetEndJointQuatOld,                        // IN: Previous target joint orientation.
        drivenEndJointQuatOld,                        // IN: Previous driven joint orientation.
        drivenEndJointAngVelOld,                      // IN: Previous driven joint angular velocity.
        curIKStateLimbState->m_targetEndJointQuat,     // IN: Current target joint orientation
        targetEndJointAngVel,                         // IN: Current target joint angular velocity estimate
        curIKStateLimbState->m_drivenEndJointQuat,     // OUT: Current driven joint orientation
        curIKStateLimbState->m_drivenEndJointAngVel);  // OUT: Current driven joint angular velocity

      NM_UT_END_PROFILING();

      // Update the end joint orientation
      legIK.setEndChannelQuat(curIKStateLimbState->m_drivenEndJointQuat);

      if (curFootLiftingLimbData->m_targetWorldFootbaseLiftingValid)
      {
        // Check if the end joint was clamped
        float fromDotTo = curIKStateLimbState->m_drivenEndJointQuat.dot(curIKStateLimbState->m_targetEndJointQuat);
        float qErr = NMP::nmfabs(1.0f - NMP::nmfabs(fromDotTo));
        if (qErr > 1e-3f)
        {
          // IK fixup to prevent issues with snapping due to clamping the end joint angular motion.
          // Clamping the angular motion of the end joint moves the foot pivot point away from
          // its desired position. i.e. the vertical projection through the foot pivot no longer
          // intersects the terrain lifting target. We FK the leg joints and recover the corresponding
          // worldspace foot pivot position then apply the horizontal translation that vertically
          // re-aligns the foot pivot with the terrain lifting target.
          legIK.fkEndJointTM();

#ifdef TEST_UT_TASK_LOGGING
          NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "limbIndex %d: Performing IK Fixup\n", limbIndex);
#endif
          // Compute the new worldspace foot pivot position
          footIK.setAnkleJointTM(endJointTM);
          footIK.fkWorldFootPivotPos();
          NMP::Vector3 worldFootPivotPos = footIK.getWorldFootPivotPos();
          // Compute the vector between the new foot pivot position and the target lifting position
          NMP::Vector3 deltaV = curIKStateLimbState->m_targetWorldFootbaseLiftingPos - worldFootPivotPos;
          // Compute the component in the vertical direction
          float lambda = deltaV.dot(upAxisWS);
          // Compute the component in the horizontal direction
          deltaV -= (upAxisWS * lambda);
          // Apply the horizontal correction to align the new foot pivot with the target lifting position
          endJointTM.translation() += deltaV;

          // Recompute the IK solve
          NM_UT_BEGIN_PROFILING("BASIC_UT_LEG_IK_SOLVE2");
          legIK.solve();
          NM_UT_END_PROFILING();
        }
      }
    }
    else
    {
      curIKStateLimbState->m_drivenEndJointQuat = legIK.getEndChannelQuat();
      curIKStateLimbState->m_drivenEndJointAngVel.setToZero();
      curIKStateLimbState->m_targetEndJointAngVel.setToZero();
    }

    //---------------------------
    // Update the foot lifting IK state
    legIK.fkEndJointTM();
    footIK.setAnkleJointTM(endJointTM);
    footIK.fkWorldFootPivotPos();
    curIKStateLimbState->m_drivenWorldAnkleJointTM = endJointTM;

    // Current end joint state
    curWorldTM.inverseTransformVector(endJointTM.translation(), curIKStateLimbState->m_drivenEndJointPosCS);
    if (isValidInputIKState)
    {
      // Compute the velocity using a linear approximation between the current and previous end joint
      // positions. This works better than using a quadratic approximation that involves velocity terms
      // from the previous step. i.e. the end joint motion needn't be smooth, especially if the motion
      // was clamped due to the leg maximum reach limit being violated.
      curIKStateLimbState->m_drivenEndJointPosVelCS = (curIKStateLimbState->m_drivenEndJointPosCS - drivenEndJointPosCSOld) * recipDeltaTime;

      // Check if we need to zero the vertical component of the driven end joint motion for
      // ground penetration fixup.
      if (groundPenetrationFixupFlags[limbIndex])
      {
        float lambda = curIKStateLimbState->m_drivenEndJointPosVelCS.dot(setupAttrib->m_upAxis);
        curIKStateLimbState->m_drivenEndJointPosVelCS -= setupAttrib->m_upAxis * lambda;
      }

#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "limbIndex = %d (final pos in CS), prevDrivenEndJointPosCS = %f %f %f, curDrivenEndJointPosCS = %f %f %f\n",
        limbIndex,
        drivenEndJointPosCSOld.x,
        drivenEndJointPosCSOld.y,
        drivenEndJointPosCSOld.z,
        curIKStateLimbState->m_drivenEndJointPosCS.x,
        curIKStateLimbState->m_drivenEndJointPosCS.y,
        curIKStateLimbState->m_drivenEndJointPosCS.z);
#endif
    }
    else
    {
      curIKStateLimbState->m_drivenEndJointPosVelCS.setToZero();
      
#ifdef TEST_UT_TASK_LOGGING
      NET_LOG_MESSAGE(
        NMP::LOG_PRIORITY_ALWAYS,
        "limbIndex = %d (final pos in CS), curDrivenEndJointPosCS = %f %f %f\n",
        limbIndex,
        curIKStateLimbState->m_drivenEndJointPosCS.x,
        curIKStateLimbState->m_drivenEndJointPosCS.y,
        curIKStateLimbState->m_drivenEndJointPosCS.z);
#endif
    }

    // Compute the driven footbase lifting point by projecting the foot pivot point
    // vertically onto the footbase plane.
    footIK.computeFootbaseLiftingPos(
      upAxisWS,
      curIKSetupLimbState->m_endWorldJointTM, // The initial animation pose ankle joint TM
      curIKSetupLimbState->m_footbaseDistFromPivotPos, // The distance of the footbase from the pivot point
      curIKStateLimbState->m_drivenWorldFootbaseLiftingPos, // OUT
      curIKStateLimbState->m_drivenWorldFootbaseLiftingNormal); // OUT
  }

  // Set the output IK state as having valid data
  ikStateAttrib->m_isValidData = true;
  NM_UT_END_PROFILING(); // BASIC_UT_LEG_IK

  NM_UT_END_PROFILING(); // BASIC_UT_MOTION_SOLVE
}

//----------------------------------------------------------------------------------------------------------------------
void unevenTerrainIKFKBlend(
  UnevenTerrainHipsIK*                 hipsIKSolver,
  UnevenTerrainLegIK*                  legIKSolvers,
  const AttribDataTransformBuffer*     inputTransformsAttrib,
  AttribDataTransformBuffer*           outputTransformsAttrib,
  const AttribDataBasicUnevenTerrainIKState* inputIKStateAttrib,
  const AttribDataFloat*               ikFkBlendWeightAttrib)
{
  NMP_ASSERT(hipsIKSolver);
  NMP_ASSERT(legIKSolvers);
  NMP_ASSERT(inputTransformsAttrib);
  NMP_ASSERT(inputIKStateAttrib);
  NMP_ASSERT(outputTransformsAttrib);
  NMP_ASSERT(ikFkBlendWeightAttrib);

  NM_UT_BEGIN_PROFILING("BASIC_UT_IK_FK_BLENDING");

  const NMP::DataBuffer* inputTransforms = inputTransformsAttrib->m_transformBuffer;
  NMP::DataBuffer* outputTransforms = outputTransformsAttrib->m_transformBuffer;
  uint32_t numLimbs = inputIKStateAttrib->m_numLimbs;

#ifdef TEST_UT_TASK_LOGGING
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "\n----------------------------------------------------\n");
  NET_LOG_MESSAGE(NMP::LOG_PRIORITY_ALWAYS, "IK / FK blending\n");
#endif

  float weight = NMP::clampValue(ikFkBlendWeightAttrib->m_value, 0.0f, 1.0f);
  if (weight < (1.0f - 1e-4f))
  {
    // Hips
    hipsIKSolver->blendWithBuffer(inputTransforms, weight);

    // Legs
    for (uint32_t limbIndex = 0; limbIndex < numLimbs; ++limbIndex)
    {
      UnevenTerrainLegIK& legIK = legIKSolvers[limbIndex];
      legIK.blendWithBuffer(inputTransforms, weight);
    }
  }

  // This node can add missing channels, so recalculate the full flag
  outputTransforms->setFullFlag(outputTransforms->getUsedFlags()->calculateAreAllSet());

  NM_UT_END_PROFILING();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
