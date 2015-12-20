/*
* Copyright (c) 2011 NaturalMotion Ltd. All rights reserved.
*
* Not to be copied, adapted, modified, used, distributed, sold,
* licensed or commercially exploited in any manner without the
* written consent of NaturalMotion.
*
* All non public elements of this software are the confidential
* information of NaturalMotion and may not be disclosed to any
* person nor used for any purpose not expressly approved by
* NaturalMotion in writing.
*
*/

//----------------------------------------------------------------------------------------------------------------------
#include "MyNetwork.h"
#include "MyNetworkPackaging.h"
#include "FreeFallManagement.h"
#include "HazardManagement.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "NMPlatform/NMMatrix34.h"
#include "euphoria/erBody.h"
#include "Types/Environment_CollideResult.h"

#define SCALING_SOURCE freeFallManagement.owner->owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

enum CharacterAxis
{
  // The values of these enums is important and affects the logic of the 
  // computeTargetOrientation() fn.
  // The values should match the assignment of direction names to axis 
  // in the euphoria spine limb root i.e. front = 0, up = 1, right = 2

  CHARACTER_AXIS_FRONT  = 0,
  CHARACTER_AXIS_UP     = 1,
  CHARACTER_AXIS_RIGHT  = 2,

  CHARACTER_AXIS_BACK   = 3,
  CHARACTER_AXIS_DOWN   = 4,
  CHARACTER_AXIS_LEFT   = 5,

  CHARACTER_AXIS_NONE   = 6,

  CHARACTER_AXIS_MAX
};

enum TargetAxis
{
  TARGET_AXIS_VELOCITY,
  TARGET_AXIS_WORLD_UP,
  TARGET_AXIS_LANDING_UP,

  TARGET_AXIS_NONE,

  TARGET_AXIS_MAX
};

//----------------------------------------------------------------------------------------------------------------------
// groundDetected : Here we check if the character goes down during NUM_FRAMES_BETWEEN_PROBES frames
// at least (as the module impactPredictor requests a sweep in the direction of the COM velocity
// this often). So if there is nothing detected for these 4 frames, it means the character will be
// in a freefall for quite a long time as there is no ground below him. Return true if there is
// ground detected below the character.
//----------------------------------------------------------------------------------------------------------------------
static bool groundDetected(const FreeFallManagement& freeFallManagement, bool disableWhenInContact)
{
  NMP::Vector3 COMVel = freeFallManagement.owner->owner->data->bodyLimbSharedState.m_dynamicState.getVelocity();
  bool disabledByContact = freeFallManagement.feedIn->getIsInContact() & disableWhenInContact;
  
  bool characterGoesUp = COMVel.dot(freeFallManagement.owner->owner->data->gravity) < 0.0f;
  bool dangerDetected = freeFallManagement.feedIn->getChestHazardImportance() > 0.0f;

  if (disabledByContact || dangerDetected || characterGoesUp)
  {
    // If the character is in contact, doesn't go down or detects any danger, reset the counter to 0.
    freeFallManagement.data->noHazardDetected = 0;
  }
  else
  {
    freeFallManagement.data->noHazardDetected++;
    if (freeFallManagement.data->noHazardDetected >= SceneProbesData::NUM_FRAMES_BETWEEN_PROBES)
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Compute and return the time before the next impact if a hazard is detected.
static float computeTimeToImpact(const FreeFallManagement& freeFallManagement)
{
  // By default, he is considered in freefall if there is no hazard detected.
  float nextImpactTime(FLT_MAX);

  if (freeFallManagement.feedIn->getChestHazardImportance() != 0.0f)
  {
    nextImpactTime = freeFallManagement.feedIn->getChestHazard().impactTime;
  }

  return nextImpactTime;
}

//----------------------------------------------------------------------------------------------------------------------
// Return the rotation vector required to have the character in the desired orientation at the landing.
static NMP::Vector3 computeLandingUpAxis(const FreeFallManagement& freeFallManagement)
{
  const DynamicState& dsBody = freeFallManagement.owner->owner->data->bodyLimbSharedState.m_dynamicState;
  const FreefallParameters& freefallParams = freeFallManagement.in->getFreefallParams();

  // This function knows when the impact will happen, so it can calculate the velocity at impact.
  // The best angle to cushion a particular velocity is one where an inverted pendulum would be
  // knocked up into a vertical position this is not the same as the velocity vector at landing
  const float nextImpactTime(NMP::minimum(computeTimeToImpact(freeFallManagement), SCALE_TIME(1.0f)));

  // TODO, velocity of the chest would be better as it takes into account angular vel
  NMP::Vector3 velocityAtImpact = dsBody.getVelocity() + freeFallManagement.owner->owner->data->gravity * nextImpactTime;
  // Due to lack of data here and simplicity we assume the ground is flat
  float downwardsSpeed = velocityAtImpact.dot(freeFallManagement.owner->owner->data->down);
  NMP::Vector3 levelledVelocity = velocityAtImpact - downwardsSpeed * freeFallManagement.owner->owner->data->down;
  levelledVelocity *= freefallParams.angleLandingAmount;
  NMP::Vector3 targetUp = -(levelledVelocity + freeFallManagement.owner->owner->data->down * downwardsSpeed);
  targetUp.normalise();

  return targetUp;
}


//----------------------------------------------------------------------------------------------------------------------
// Return the rotation vector required to drive the character to the desired orientation.
//----------------------------------------------------------------------------------------------------------------------
static NMP::Vector3 computeRotationRequiredForOrientation(
  const float MR_OUTPUT_DEBUG_ARG(timeStep),
  const FreeFallManagement& freeFallManagement,
  const FreefallParameters& params,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  NMP_ASSERT(params.characterAxis[0] < CHARACTER_AXIS_MAX);
  NMP_ASSERT(params.characterAxis[1] < CHARACTER_AXIS_MAX);

  NMP_ASSERT(params.targetAxis[0] < TARGET_AXIS_MAX);
  NMP_ASSERT(params.targetAxis[1] < TARGET_AXIS_MAX);

  // Build table of possible target axis.
  const DynamicState& dsBody(freeFallManagement.owner->owner->data->bodyLimbSharedState.m_dynamicState);

  NMP::Vector3 targetAxisTable[TARGET_AXIS_MAX];

  targetAxisTable[TARGET_AXIS_NONE].setToZero();
  targetAxisTable[TARGET_AXIS_WORLD_UP] = freeFallManagement.owner->owner->data->up;

  if ((params.targetAxis[0] == TARGET_AXIS_VELOCITY) || (params.targetAxis[1] == TARGET_AXIS_VELOCITY))
  {
    targetAxisTable[TARGET_AXIS_VELOCITY] = dsBody.getVelocity();
    targetAxisTable[TARGET_AXIS_VELOCITY].fastNormalise();
  }

  if ((params.targetAxis[0] == TARGET_AXIS_LANDING_UP) || (params.targetAxis[1] == TARGET_AXIS_LANDING_UP))
  {
    targetAxisTable[TARGET_AXIS_LANDING_UP] = computeLandingUpAxis(freeFallManagement);
  }

  // Determine how many of the characters axis should be aligned with world space axis.
  const bool alignPrimaryAxis((params.characterAxis[0] != CHARACTER_AXIS_NONE)
    && (params.targetAxis[0] != TARGET_AXIS_NONE));
  bool alignSecondaryAxis((params.characterAxis[1] != CHARACTER_AXIS_NONE)
    && (params.targetAxis[1] != TARGET_AXIS_NONE));

  // can only align the secondary axis if the targets are not parallel
  if (alignSecondaryAxis)
  {
    float d = targetAxisTable[params.targetAxis[0]].dot(targetAxisTable[params.targetAxis[1]]);
    float criticalD = params.secondaryDirectionThreshold;
    if (fabsf(d) > criticalD)
      alignSecondaryAxis = false;
  }

  const ER::LimbTransform& rootSpine(freeFallManagement.owner->owner->data->spineLimbSharedStates[0].m_rootTM);

  // Retrieve and condition input orientation.
  NMP::Quat inputOrientation(params.orientation);
  inputOrientation.normalise();

  NMP::Quat characterToTarget(NMP::Quat::kIdentity);

  if (alignSecondaryAxis)
  {
    // Initialise output (target orientation in world space).
    NMP::Matrix34 targetTM(NMP::Matrix34::kIdentity);

    // Two of the character's axis need to be aligned with world space axis.
    const uint32_t primaryAxis(params.characterAxis[0] % 3);
    const uint32_t secondaryAxis(params.characterAxis[1] % 3);

    targetTM.r[primaryAxis] = targetAxisTable[params.targetAxis[0]];

    // Find second target axis which must be orthogonal to the first.
    targetTM.r[secondaryAxis] = NMRU::GeomUtils::calculateOrthogonalPlanarVector(
      targetAxisTable[params.targetAxis[0]], targetAxisTable[params.targetAxis[1]]).getNormalised();

    // Invert primary target axis if required.
    if (params.characterAxis[0] > CHARACTER_AXIS_RIGHT)
    {
      targetTM.r[primaryAxis] = -targetTM.r[primaryAxis];
    }

    // Invert secondary target axis if required.
    if (params.characterAxis[1] > CHARACTER_AXIS_RIGHT)
    {
      targetTM.r[secondaryAxis] = -targetTM.r[secondaryAxis];
    }

    // Determine which axis hasn't been specified.
    // - this is 3 minus the sum of the two specified axis indexes i.e. if axis 1 & 2 are specified then axis 0 is
    // missing and needs to be calculated.
    const uint32_t unspecifiedAxis(3 - params.characterAxis[0] - params.characterAxis[1]);

    // Calculate third orthogonal target axis from the first two axis.
    // - x ^ y = z can be written as 0 ^ 1 = 2 + cyclic permutations.
    // - calculate missing axis m as axis[m] = axis[(1 + m) % 3] ^ axis[(2 + m) % 3], the modulo generates the cyclic
    //   permutations.
    targetTM.r[unspecifiedAxis] =
      NMP::vCross(targetTM.r[(1 + unspecifiedAxis) % 3], targetTM.r[(2 + unspecifiedAxis) % 3]);

    NMP_ASSERT(targetTM.isValidTM3x3(0.01f)); // Ensure orientation matrix is orthonormal.

    characterToTarget = targetTM.toQuat();

    // Transform target orientation by input orientation.
    characterToTarget = characterToTarget * inputOrientation;

    // Calculate rotation from character to target orientation.
    NMP::Quat characterConjugate(rootSpine.toQuat());
    characterConjugate.conjugate();
    characterToTarget = targetTM.toQuat() * characterConjugate;
  }
  else if (alignPrimaryAxis)
  {
    // Only one character axis needs to be aligned with a world space target.
    const NMP::Matrix34& currentOrientation(rootSpine.matrix());

    // Determine the index of the specified axis.
    const uint32_t specifiedAxis(params.characterAxis[0] % 3);

    // Transform target axis by input orientation.
    NMP::Vector3 targetAxis(inputOrientation.rotateVector(targetAxisTable[params.targetAxis[0]]));

    // Invert target axis if required.
    if (params.characterAxis[0] > CHARACTER_AXIS_RIGHT)
    {
      targetAxis = -targetAxis;
    }

    // Find rotation from specified character axis to target axis.
    characterToTarget.forRotation(currentOrientation.r[specifiedAxis], targetAxis);
  }

#if defined(MR_OUTPUT_DEBUGGING)
  // Debug draw.
  const NMP::Vector3 projectedCoM(dsBody.getPosition() + (dsBody.getVelocity() * timeStep));

  // Debug render for final target axis
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, projectedCoM,
    characterToTarget.rotateVector(rootSpine. frontDirection()), NMP::Colour::RED);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, projectedCoM,
    characterToTarget.rotateVector(rootSpine.upDirection()), NMP::Colour::GREEN);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, projectedCoM,
    characterToTarget.rotateVector(rootSpine.rightDirection()), NMP::Colour::BLUE);

  // Debug render for current character axis
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, projectedCoM,
    rootSpine.frontDirection(), NMP::Colour::LIGHT_RED);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, projectedCoM,
    rootSpine.upDirection(), NMP::Colour::LIGHT_GREEN);
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, projectedCoM,
    rootSpine.rightDirection(), NMP::Colour::LIGHT_BLUE);

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngleDelta, projectedCoM,
    characterToTarget.toRotationVector(), NMP::Colour::WHITE);
#endif // defined(MR_OUTPUT_DEBUGGING)

  return characterToTarget.toRotationVector();
}


//----------------------------------------------------------------------------------------------------------------------
// Determine whether or not the character is in freefall and update the time in freefall accordingly.
//----------------------------------------------------------------------------------------------------------------------
static void updateTimeInFreefall(const FreeFallManagement& freeFallManagement, const float dt, bool disableWhenInContact)
{
  // By default, he is considered in freefall if there is no hazard detected.
  bool isSupported = freeFallManagement.feedIn->getSupportAmount() > 0.01f;
  bool isTotallySupported = freeFallManagement.feedIn->getSupportAmount() > 0.99f;
  bool detectedGround = groundDetected(freeFallManagement, disableWhenInContact);
  bool disabledByContact = freeFallManagement.feedIn->getIsInContact() & disableWhenInContact;
  
  if (disabledByContact || isTotallySupported || (isSupported && detectedGround && !freeFallManagement.data->isInFreefall) )
  {
    freeFallManagement.data->isInFreefall = false;
    freeFallManagement.data->isInFreefallTimer = 0.0f;
  }
  else
  {
    freeFallManagement.data->isInFreefall = true;
    freeFallManagement.data->isInFreefallTimer += dt;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Return true if the behaviour should act to change the orientation of the character now.
static bool shouldDriveOrientation(const FreeFallManagement& freeFallManagement)
{
  bool result(false);

  if (freeFallManagement.data->isInFreefall)
  {
    const float timeInFreefall(freeFallManagement.data->isInFreefallTimer);
    const float timeToImpact(computeTimeToImpact(freeFallManagement));

    const FreefallParameters& freefallParam(freeFallManagement.in->getFreefallParams());

    // 1 - Calculate the time (in seconds) since the we began driving the character's orientation. This can be negative,
    //     indicating that we have not yet started.
    const float timeAfterStart =
      (freefallParam.startOrientationAtTimeBeforeImpact) 
      ? freefallParam.startOrientationTime - timeToImpact
      : timeInFreefall - freefallParam.startOrientationTime;

    // 2 - Calculate the time (in seconds) before we should finished driving the character's orientation. This can be 
    //     negative, indicating that we have passed the point where we should stop.
    const float timeBeforeStop =
      (freefallParam.stopOrientationAtTimeBeforeImpact) 
      ? timeToImpact - freefallParam.stopOrientationTime
      : freefallParam.stopOrientationTime - timeInFreefall;

    result = (timeAfterStart > 0.0f) && (timeBeforeStop > 0.0f);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Set and return the target to look target : look in the direction of the velocity, but bias it
// towards forwards otherwise it isn't very good at low speeds.
//----------------------------------------------------------------------------------------------------------------------
static NMP::Vector3 getLookTarget(const FreeFallManagement& freeFallManagement)
{
  const float predictionTime = NMP::minimum(freeFallManagement.feedIn->getChestHazard().impactTime, SCALE_TIME(1.0f));
  NMP::Vector3 position = freeFallManagement.owner->owner->data->spineLimbSharedStates[0].m_endTM.translation() + 
    freeFallManagement.owner->owner->data->spineLimbSharedStates[0].m_endTM.frontDirection() * SCALE_DIST(1.0f);
  NMP::Vector3 velocity = freeFallManagement.owner->owner->data->bodyLimbSharedState.m_dynamicState.getVelocity();
  NMP::Vector3 acceleration = freeFallManagement.owner->owner->data->gravity;
  NMP::Vector3 lookTarget = position + velocity * predictionTime + 0.5f * acceleration * NMP::sqr(predictionTime);

  return lookTarget;
}

#undef SCALING_SOURCE
#define SCALING_SOURCE owner->owner->data->dimensionalScaling


//----------------------------------------------------------------------------------------------------------------------
// FreefallManagementUpdatePackage
//----------------------------------------------------------------------------------------------------------------------
void FreeFallManagement::update(float timeStep)
{
  MR::InstanceDebugInterface* pDebugDrawInst = getRootModule()->getDebugInterface();
  MR_DEBUG_MODULE_ENTER(pDebugDrawInst, getClassName());

  const float paramsImportance(in->getFreefallParamsImportance());

  if (paramsImportance == 0.0f)
  {
    return;
  }

  const FreefallParameters& freefallParams = in->getFreefallParams();
  updateTimeInFreefall(*this, timeStep, freefallParams.disableWhenInContact);

  if (shouldDriveOrientation(*this))
  {
    // Update last CoM angular velocity.
    const DynamicState& dsBody = owner->owner->data->bodyLimbSharedState.m_dynamicState;

    // Orientate character according to preferred axis specified in attributes.
    const NMP::Vector3 rotRequired(
      computeRotationRequiredForOrientation(timeStep, *this, freefallParams, pDebugDrawInst));

    const float rotationTime(NMP::maximum(freefallParams.rotationTime, SCALE_TIME(0.0001f)));
    const float imminence(1.0f / rotationTime);
    const float importance(NMP::clampValue(freefallParams.weight, 0.0f, 1.0f));

    // Set arm windmill request.
    RotationRequest armsRequest(rotRequired * freefallParams.armsAmount, SCALING_SOURCE);
    armsRequest.imminence = imminence;
#if defined(MR_OUTPUT_DEBUGGING)
    armsRequest.debugControlAmounts.setControlAmount(ER::freefallControl, 1.0f);
#endif
    out->setArmsSpinRotationRequest(armsRequest, importance);

    // Set leg spin request.
    RotationRequest legsRequest(rotRequired * freefallParams.legsAmount, SCALING_SOURCE);
    legsRequest.imminence = imminence;
#if defined(MR_OUTPUT_DEBUGGING)
    legsRequest.debugControlAmounts.setControlAmount(ER::freefallControl, 1.0f);
#endif
    out->setLegsSpinRotationRequest(legsRequest, importance);

    // Dual vector of force and torque
    data->wrench.force.setToZero();
    data->wrench.torque.setToZero();

    PoseData spinePose;
    spinePose.poseEndRelativeToRoot = 
      owner->owner->data->defaultPoseEndsRelativeToRoot[owner->owner->data->firstSpineNetworkIndex];
    spinePose.stiffnessScale = 1.0f;
    spinePose.driveCompensationScale = 1.0f;
    out->setSpinePose(spinePose, importance);

    // Build Cheating torque
    NMP::Vector3 angularAcceleration(NMP::Vector3::InitTypeZero);

    if (freefallParams.assistanceAmount > 0.0f)
    {
      // Since chest forces should always be minimal, we don't worry about the end angular velocity here
      // we just try and get to the velocity necessary to reach the correct angle at the correct time
      // we shouldn't be seeing any oscillations with this

      const float strength(freefallParams.assistanceAmount * freefallParams.weight * SCALE_STRENGTH(1.0f));
      const float damping(2.0f * NMP::fastSqrt(strength));

      // Only damp angular velocity about axis of rotation.
      const NMP::Vector3 dirRequired(rotRequired.getNormalised());
      const NMP::Vector3 angularVel( NMP::vDot(dsBody.getAngularVelocity(), dirRequired) * dirRequired);

      angularAcceleration = (rotRequired * strength) - (angularVel * damping);
    }

    // Apply damping to angular velocity.
    if (freefallParams.dampingAmount > 0.0f)
    {
      const NMP::Vector3 angularVel(dsBody.getAngularVelocity());

      angularAcceleration -= angularVel * freefallParams.dampingAmount;
    }

    // Apply any required torque to character.
    if (angularAcceleration.magnitudeSquared() > 0.0f)
    {
      const float approxCharacterInertia(SCALE_INERTIA(16.0f));

      data->wrench.torque = angularAcceleration * approxCharacterInertia;

      owner->owner->getCharacter()->getBody().getPhysicsRig()->receiveWrenchImpulse(
        data->wrench.force * timeStep, data->wrench.torque * timeStep);

      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Torque,
        owner->owner->data->spineLimbSharedStates[0].m_rootTM.translation(), rotRequired, NMP::Colour::DARK_GREEN);
    }

    // Build HeadLook target request.
    NMP::Vector3 lookTarget = getLookTarget(*this);
    TargetRequest lookRequest(lookTarget, SCALING_SOURCE);
    lookRequest.passOnAmount = 0.0f; // Look at only with eyes and head
#if defined(MR_OUTPUT_DEBUGGING)
    lookRequest.debugControlAmounts.setControlAmount(ER::freefallControl, 1.0f);
#endif

    // don't try and look out of range so much when falling through air, it doesn't look as good
    lookRequest.lookOutOfRangeWeight = 0.5f;
    out->setFocalCentre(lookRequest, importance);

    // Compute error with target orientation
    data->orientationError = rotRequired.magnitude();
    out->setOrientationError(data->orientationError);

    if (data->isInFreefall)
    {
      out->setIsInFreefall(data->isInFreefall, importance);
    }
  }

}

//----------------------------------------------------------------------------------------------------------------------
void FreeFallManagement::entry()
{
  data->noHazardDetected = 0;
  data->isInFreefallTimer = 0.0f;
  data->isInFreefall = false;
  data->orientationError = 0.0f;
}

} //namespace NM_BEHAVIOUR_LIB_NAMESPACE
