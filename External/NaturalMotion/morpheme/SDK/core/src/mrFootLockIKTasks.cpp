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
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrNetwork.h"
#include "sharedDefines/mCoreDebugInterface.h"
#include "morpheme/mrCharacterControllerAttribData.h"
//----------------------------------------------------------------------------------------------------------------------
#include "NMGeomUtils/NMGeomUtils.h"
#include "NMIK/NMTwoBoneIK.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
namespace 
{
    
  
//----------------------------------------------------------------------------------------------------------------------
/// A very simple lookup table implementation of 1 - exp(-x), for positive x
float fastStepResponse(float x)
{
  static const float interpPoints[6] = { 0, 0.632f, 0.86466f, 0.9502f, 0.98168f, 0.99326f };
  uint32_t indexLow = NMP::minimum((uint32_t)floorf(x), (uint32_t)4);
  float interpX = x - indexLow;
  return NMP::minimum(interpPoints[indexLow] * (1.0f - interpX) + interpPoints[indexLow+1] * interpX, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
void subTaskLockFootTransforms(
  const AttribDataTransformBuffer* localBufferAttrib,
  const NMP::Vector3& deltaTransformPos,
  const NMP::Quat& deltaTransformAtt,
  const AttribDataFloat* ikFkBlendWeightAttrib,
  const AttribDataFloat* swivelContributionToOrientationAttrib,
  const AttribDataLockFootSetup* lockFootSetupAttrib,
  const AttribDataLockFootChain* lockFootChainAttrib,
  const AttribDataRig* rigAttrib,
  const AttribDataSampledEvents* sampleEventsBufferAttrib,
  const AttribDataUpdatePlaybackPos* timePosAttrib,
  AttribDataLockFootState* lockFootState,
  const AttribDataLockFootState* lastLockFootState,
  AttribDataTransformBuffer* outputBufferAttrib)
{
  const AnimRigDef* rig = rigAttrib->m_rig;
  NMP_USED_FOR_ASSERTS(const uint32_t numRigJoints = rig->getNumBones());

  // Get the blend weight control parameter
  float ikFkBlendWeight = NMP::clampValue(ikFkBlendWeightAttrib->m_value, 0.0f, 1.0f);

  // Get the swivel orientation control parameter, or a default if none is connected
  float swivelContributionToOrientation = swivelContributionToOrientationAttrib->m_value;

  // Get some other parameters or aliases of them
  const int32_t* joints = lockFootChainAttrib->m_jointIndex;
  const int32_t& footfallEventID = lockFootChainAttrib->m_footfallEventID;
  //
  // The index of the up axis - this assumes that up is a canonical direction
  NMP_ASSERT(lockFootSetupAttrib->m_upAxis == 0 || lockFootSetupAttrib->m_upAxis == 1 ||
    lockFootSetupAttrib->m_upAxis == 2);
  const uint32_t& upAxis = lockFootSetupAttrib->m_upAxis;
  //
  NMP::DataBuffer* outputBuffer = outputBufferAttrib->m_transformBuffer;
  const NMP::DataBuffer* localBuffer = localBufferAttrib->m_transformBuffer;

  //--------------------------------------------
  // Validations

  // Validate joint indices
  NMP_ASSERT(joints[0] > 0 && joints[0] < (signed)numRigJoints);
  NMP_ASSERT(joints[1] > 0 && joints[1] < (signed)numRigJoints);
  NMP_ASSERT(joints[2] > 0 && joints[2] < (signed)numRigJoints);
  NMP_ASSERT(!lockFootChainAttrib->m_useBallJoint || (joints[3] > 0 && joints[3] < (signed)numRigJoints));
  NMP_ASSERT(!lockFootChainAttrib->m_fixToeGroundPenetration || (joints[4] > 0 && joints[4] < (signed)numRigJoints));

  // If we are fixing the toe ground penetration then to avoid confusion we must be locking the
  // ball joint and fixing ground penetration on it.
  NMP_ASSERT(!lockFootChainAttrib->m_fixToeGroundPenetration ||
    (lockFootChainAttrib->m_useBallJoint && lockFootChainAttrib->m_fixGroundPenetration));
  //   If we need a valid toe index, it must be the child of the ball joint, at the moment
  NMP_ASSERT(!lockFootChainAttrib->m_fixToeGroundPenetration ||
    joints[3] == rig->getParentBoneIndex(joints[4]));

  //--------------------------------------------

  // Determine whether we're in a footfall event
  bool inFootStep = false;
  SampledCurveEventsBuffer* curveEvents = sampleEventsBufferAttrib->m_curveBuffer;
  for (uint32_t i = 0; i < curveEvents->getNumSampledEvents(); ++i)
  {
    const SampledCurveEvent* ev = curveEvents->getSampledEvent(i);
    if (ev->getSourceEventUserData() == (uint32_t)footfallEventID)
    {
      inFootStep = true;
      break;
    }
  }

  //--------------------------------------------
  // Retrieve or initialise state

  // Get aliases for state contents
  NMP::Vector3& previousEndEffectorPosition = lockFootState->m_previousEndEffectorPosition;
  NMP::Quat& previousEndJointOrientation = lockFootState->m_previousEndJointOrientation;
  NMP::Vector3& lockPosition = lockFootState->m_lockPosition;
  NMP::Quat& lockOrientation = lockFootState->m_lockOrientation;
  bool& tracking = lockFootState->m_tracking;
  bool& firstFootStep = lockFootState->m_firstFootStep;
  float& footRollTime = lockFootState->m_footRollTime;
  float& footCatchUpTime = lockFootState->m_footCatchUpTime;
  float& footPivotTime = lockFootState->m_footPivotTime;
  bool& pivotIsLocked = lockFootState->m_pivotIsLocked;

  // We get the trajectory transform from the previous frame, then update it
  AttribDataTransform trajectoryTransform = lockFootState->m_trajectoryTransform;
  // Take a copy of deltaTime so we can set it to zero on the first footfall
  float deltaTime = timePosAttrib->m_value;
  // Update the first foot step
  firstFootStep = inFootStep && !lockFootState->m_previouslyInFootStep;

  // If this is the first update (or we have an abs time set), initialise the state
  if (!lastLockFootState || timePosAttrib->m_isAbs)
  {
    trajectoryTransform.m_att = NMP::Quat(NMP::Quat::kIdentity);
    trajectoryTransform.m_pos = NMP::Vector3Zero();
    previousEndEffectorPosition.setToZero();
    previousEndJointOrientation.identity();
    lockPosition.setToZero();
    lockOrientation.identity();
    tracking = true;
    firstFootStep = inFootStep;
    footRollTime = 0;
    footCatchUpTime = 0;
    footPivotTime = 0;
    pivotIsLocked = false;
    deltaTime = 0.0;
  }

  // Update the trajectory transform accumulator. This will be reset when a new lockfoot cycle begins (if
  // we are not tracking the character controller).
  // - Position
  trajectoryTransform.m_pos += trajectoryTransform.m_att.rotateVector(deltaTransformPos);
  // - Rotation
  trajectoryTransform.m_att *= deltaTransformAtt;
  trajectoryTransform.m_att.fastNormalise(); // To minimise error accumulation.

  //--------------------------------------------
  // Geometry setup

  // Copy input buffer to output buffer.  I will then be modifying the output buffer at the
  // joints of the chain.
  localBuffer->copyTo(outputBuffer);

  // On first footfall, reset the 'global' TM to the identity, essentially realigning
  // the animation to the origin at the start of each footfall.  This eliminates the
  // concern of numerical drift.
  if (!lockFootSetupAttrib->m_trackCharacterController && firstFootStep)
  {
    // We have stored a couple of vectors from the previous update that are in global
    // coordinates, and need to be moved as a result of this.
    trajectoryTransform.m_att.inverseRotateVector(
      previousEndEffectorPosition - trajectoryTransform.m_pos, previousEndEffectorPosition);
    previousEndJointOrientation = ~trajectoryTransform.m_att * previousEndJointOrientation;
    trajectoryTransform.m_att.inverseRotateVector(
      lockPosition - trajectoryTransform.m_pos, lockPosition);
    lockOrientation = ~trajectoryTransform.m_att * lockOrientation;

    trajectoryTransform.m_pos.setToZero();
    trajectoryTransform.m_att.identity();
  }

  // Get the world transform of the parent of the root joint, to relate the IK chain coordinate frame
  // to the world coordinate frame.
  NMRU::GeomUtils::PosQuat rootParentT = NMRU::GeomUtils::identityPosQuat();
  int32_t rootJoint = joints[0];
  while ((rootJoint = rig->getParentBoneIndex(rootJoint)) > 0)
  {
    const NMP::Vector3* p = outputBuffer->getPosQuatChannelPos(rootJoint);
    const NMP::Quat* q = outputBuffer->getPosQuatChannelQuat(rootJoint);
    if (!outputBuffer->hasChannel(rootJoint))
    {
      p = rig->getBindPoseBonePos(rootJoint);
      q = rig->getBindPoseBoneQuat(rootJoint);
    }
    NMRU::GeomUtils::PosQuat jointT = {*p, *q};
    NMRU::GeomUtils::multiplyPosQuat(rootParentT, jointT);
  }
  // Tack on global TM (actually...relative to where we were at the start of the footfall)
  NMRU::GeomUtils::PosQuat trajectoryT = {trajectoryTransform.m_pos, trajectoryTransform.m_att};
  NMRU::GeomUtils::multiplyPosQuat(rootParentT, trajectoryT);

  // Set up the IK geometry, adding missing channels from the bind pose and activating them in the
  // transform buffer, since we are modifying them.
  // While doing this, retrieve the current end effector joint TM (the end effector /joint/ being
  // the ankle, regardless of whether we're using the ball joint as the end effector or not - we are
  // not moving the ball joint for the locking).
  NMRU::GeomUtils::PosQuat ikGeometry[4];
  NMRU::GeomUtils::PosQuat endEffectorTransform = rootParentT;
  for (uint32_t i = 0; i < 3; ++i)
  {
    int32_t j = joints[i];
    if (!outputBuffer->hasChannel(j))
    {
      ikGeometry[i].t = *rig->getBindPoseBonePos(j);
      ikGeometry[i].q = *rig->getBindPoseBoneQuat(j);
      outputBuffer->setChannelUsed(j);
    }
    else
    {
      ikGeometry[i].t = *outputBuffer->getPosQuatChannelPos(j);
      ikGeometry[i].q = *outputBuffer->getPosQuatChannelQuat(j);
    }
    NMRU::GeomUtils::premultiplyPosQuat(ikGeometry[i], endEffectorTransform);
  }
  // The fourth 'joint' is the end effector, which has the ankle's orientation but the ball joint's
  // position - if we are using the ball joint.
  ikGeometry[3] = NMRU::GeomUtils::identityPosQuat();
  if (lockFootChainAttrib->m_useBallJoint)
  {
    if (outputBuffer->hasChannel(joints[3]))
    {
      ikGeometry[3].t = *outputBuffer->getPosQuatChannelPos(joints[3]);
    }
    else
    {
      ikGeometry[3].t = *rig->getBindPoseBonePos(joints[3]);
    }
  }
  NMRU::GeomUtils::premultiplyPosQuat(ikGeometry[3], endEffectorTransform);
  // Aliases
  const NMP::Vector3& endEffectorPosition = endEffectorTransform.t;
  NMP::Quat& endJointQuat = endEffectorTransform.q;
  const NMP::Vector3& ballChannelPos = ikGeometry[3].t;

  // Override previous frame values of start position and orientation on startup, for valid maths.
  if (!lastLockFootState)
  {
    previousEndEffectorPosition = endEffectorPosition;
    previousEndJointOrientation = endJointQuat;
    lockPosition = endEffectorPosition;
    lockOrientation = endJointQuat;
  }

  //--------------------------------------------
  // Determine the location to lock the foot

  // Set the locking position, the place we want the end effector to be.
  // In between footsteps we want the target position to 'catch up' with where the end effector
  // is in the source animation.  If we have caught up, then we switch on the 'm_tracking' flag
  // which sets the two to the same thing, and allows us to skip doing any IK until the next
  // footstep.
  if ((!inFootStep || firstFootStep) && !tracking)
  {
    // Control rule to bring the target back onto source position
    // ----------------------------------------------------------
    //
    // The essential law is Gain*(position error) + (1-Gain)*(target velocity), this is very simple and
    // essentially means that if the Gain is 1, the target will immediately snap to the source position
    // on this update.  Unfortunately on its own this does not adapt to variable step size (delta time).
    //
    // To remove this dependence I use a slightly complicated way of calculating Gain from the input
    // control parameter m_catchupSpeedFactor (a): Gain = 1 - exp(-60*(delta time)*a/(1-a*a)).  The main
    // reason for the complication is to keep the input parameter between 0 and 1 - 0 means no position
    // gain, and 1 means an instantaneous step to the target.
    //
    // This encapsulates a 2nd-order critically damped system which essentially means that if the time
    // step is divided by some factor f, say, then in f steps the target will be in the same position
    // as before.  Of course, the target is moving in this time and so this analysis isn't exactly true -
    // but this essential analogue/digital problem is inevitable regardless of the control law.
    //
    // The factor of 60 is simply to place the useful range of input parameters in a sensible bracket.
    // It means that a value of 0.5 of m_catchupSpeedFactor will result in a Gain of approximately
    // 0.5 if the frame rate is 60Hz, so in this time step we will move halfway towards the target.
    // But if the frame rate changes to, say, 120Hz, then the Gain will decrease (not by half) such that
    // (were everything static) it will take two steps to go this distance, because the step size
    // has halved.
    //
    // Another problem with this control rule is that is does not /guarantee/ that the catch-up period
    // will be of any particular duration.  This could only be done with a simple blend, which can take
    // no account of smoothness and may result in a strange path to the target.  It may be that such a
    // system should be placed on top of the 2nd order rule to provide some guarantees, but only with
    // enhanced events that allow us to define desired blend periods.
    float displacementGain = 1.0f;
    if (lockFootChainAttrib->m_catchUpSpeedFactor < 1.0f)
    {
      float k = deltaTime * lockFootChainAttrib->m_catchUpSpeedFactor /
        (1.0f - (lockFootChainAttrib->m_catchUpSpeedFactor * lockFootChainAttrib->m_catchUpSpeedFactor));
      displacementGain = fastStepResponse(60.0f * k);
    }
    NMP::Vector3 previousVelocity = endEffectorPosition - previousEndEffectorPosition;
    float velocityGain = (1 - displacementGain);
    lockPosition += (endEffectorPosition - lockPosition) * displacementGain + previousVelocity * velocityGain;
    //
    // The same basic control rule for orientation
    // OPTIMISE - would it be faster to surround this with if (lockFootChainAttrib->m_fixFootOrientation) ?
    //  Apply displacement gain
    NMRU::GeomUtils::shortestArcFastSlerp(lockOrientation, endJointQuat, displacementGain);
    //  Apply velocity gain
    NMP::Quat velocityControlRotation = endJointQuat * ~previousEndJointOrientation;
    NMP::Quat identityQuat(NMP::Quat::kIdentity);
    NMRU::GeomUtils::shortestArcFastSlerp(velocityControlRotation, identityQuat, velocityGain);
    lockOrientation = velocityControlRotation * lockOrientation;
    //
    // If the control rule puts us within snapping distance of the source position, switch on tracking.
    // This assumes orientations match up so no check for orientation snapping is required.
    if ((endEffectorPosition - lockPosition).magnitudeSquared() <
      lockFootChainAttrib->m_snapToSourceDistance * lockFootChainAttrib->m_snapToSourceDistance)
    {
      tracking = true;
    }

    // Increment the time spent catching up, which we use for the orientation control rule
    footCatchUpTime += deltaTime;
  }
  else
  {
    // A catch-up time of zero is the indicator that we're not in a catch-up period
    footCatchUpTime = 0;
  }

  // Either this is the first footstep frame, or between footsteps, and I want the target and source
  // animation to match.
  if (tracking)
  {
    lockPosition = endEffectorPosition;
    lockOrientation = endJointQuat;
  }
  // Inside a footstep, I leave the locking position the same as the previous update, except for
  // having the height track the source.  If you don't do this you can get artifacts should the
  // footfall events not match the footstep correctly; however it can be switched off.
  if (inFootStep)
  {
    tracking = false;
    // Discard vertical component of lock position, so track height but lock skate
    if (!lockFootSetupAttrib->m_lockVerticalMotion)
    {
      lockPosition[upAxis] = endEffectorPosition[upAxis];
    }
  }
  // If I'm not allowed to adjust the foot orientation, it should be locked to the input orientation
  // regardless of where I am in the step cycle.
  if (!lockFootChainAttrib->m_fixFootOrientation)
  {
    lockOrientation = endJointQuat;
  }

  // Correct height of foot to prevent ground penetration, if required.
  // We may also adjust the desired foot orientation, in which case, record this as a separate
  // quat.
  NMP::Quat targetEndJointQuat = endJointQuat;
  if (lockFootChainAttrib->m_fixGroundPenetration)
  {
    if (lockPosition[upAxis] < lockFootChainAttrib->m_lowerHeightBound)
    {
      lockPosition[upAxis] = lockFootChainAttrib->m_lowerHeightBound;
      tracking = false; // Because I need to IK to correct the height of the foot
      // If ground plane penetration has triggered locking, we cannot be in a 'catch-up' period any more
      footCatchUpTime = 0;
    }
    // It is possible that the ball joint is not penetrating the ground but the ankle is, so
    // correct for this.
    if (lockFootChainAttrib->m_useBallJoint)
    {
      // Would the ankle be penetrating the ground, with the ball joint in this position?
      NMP::Vector3 relativePositionWorld = endJointQuat.rotateVector(ballChannelPos);
      NMP::Vector3 anklePosition = lockPosition - relativePositionWorld;
      if (anklePosition[upAxis] < lockFootChainAttrib->m_ankleLowerHeightBound)
      {
        tracking = false;
        if (lockFootChainAttrib->m_fixFootOrientation)
        {
          anklePosition[upAxis] = lockFootChainAttrib->m_ankleLowerHeightBound;
          NMP::Vector3 newRelativePositionWorld = lockPosition - anklePosition;
          NMP::Quat correctionQuat;
          correctionQuat.forRotation(relativePositionWorld, newRelativePositionWorld);
          targetEndJointQuat = correctionQuat * endJointQuat;
        }
        else
        {
          lockPosition[upAxis] += lockFootChainAttrib->m_ankleLowerHeightBound - anklePosition[upAxis];
        }

        // If ground plane penetration has triggered locking, we cannot be in a 'catch-up' period any more
        footCatchUpTime = 0;
      }
    }
  }

  //--------------------------------------------
  // Move the foot to the lock position and orientation using IK

  // If we are tracking then all IK would do is returned an unaltered pose, hence we skip the
  // unnecessary computation.
  if (!tracking)
  {

    //--------------------------------------------
    // Target foot orientation fixup

    // If we are fixing up the orientation of the foot, adjust the end joint TM's rotation
    // component to roll the foot level (so the foot level vector is horizontal), then
    // adjust it to prevent swivel, depending on the allowed pivot offset.
    if (lockFootChainAttrib->m_fixFootOrientation)
    {
      // Increment the time we've been fixing foot orientation, for blend-in control
      footRollTime += deltaTime;

      // Supposed to have been normalised by the asset compiler, so save time by only asserting
      NMP_ASSERT(NMP::nmfabs(lockFootChainAttrib->m_footLevelVector.magnitudeSquared() - 1.0f) < 1e-6f);
      NMP::Vector3 footWorldLevelVector = targetEndJointQuat.rotateVector(lockFootChainAttrib->m_footLevelVector);
      NMP::Vector3 upVector(0, 0, 0);
      upVector[upAxis] = 1.0f;
      NMP::Vector3 desiredFootWorldLevelVector = footWorldLevelVector - (upVector * footWorldLevelVector.dot(upVector));
      desiredFootWorldLevelVector.normalise();
      NMP::Quat rollToLevel;
      rollToLevel.forRotation(footWorldLevelVector, desiredFootWorldLevelVector);
      NMP::Quat desiredEndJointQuat = rollToLevel * targetEndJointQuat;

      // Time constants for smoothing profiles
      static const float xTimeToGet99PercentOfTheWay = 0.5f; // Time in seconds
      static const float xInvTimeConstant = 5.0f / xTimeToGet99PercentOfTheWay; // 5 is from standard step response

      // Blend the desired orientation back towards the current depending on the catch-up time.
      // A value of zero means we are not in a catch-up period
      if (footCatchUpTime > 0)
      {
        float normalisedTime = xInvTimeConstant * footCatchUpTime;
        float weight = fastStepResponse(normalisedTime);
        NMRU::GeomUtils::shortestArcFastSlerp(desiredEndJointQuat, targetEndJointQuat, weight);
      }

      // Blend into the desired orientation according to a control rule that prevents snapping at the start of
      // a period of tracking
      float normalisedTime = xInvTimeConstant * footRollTime;
      float weight = fastStepResponse(normalisedTime);
      NMRU::GeomUtils::shortestArcFastSlerp(targetEndJointQuat, desiredEndJointQuat, weight);

      // Apply pivot rotation to stop the foot pivoting around the lock position.
      //   Calculate the pivot rotation to bring the foot back to as close as possible to the lock orientation,
      //   pivoting around the upVector.
      NMP::Quat pivotToLockOrientation =
        NMRU::GeomUtils::closestRotationAroundAxis(lockOrientation * ~targetEndJointQuat, upVector);
      desiredEndJointQuat = pivotToLockOrientation * targetEndJointQuat;
      //
      //   Determine whether this rotation is above the offset threshold
      //   Note that the offsets are in cos-half-angle, so when amountOfPivot is 1 there is no pivot, and
      //   when it is 0 there is 180 degrees of pivot.
      float amountOfPivot = NMP::nmfabs(pivotToLockOrientation.w);
      if (pivotIsLocked && amountOfPivot > lockFootChainAttrib->m_footPivotOffsetThreshold)
      {
        // Correct pivot because the offset is too low to force it (i.e. too high, in cos-half-angle terms).
        // By pivoting back from the source to the lock orientation what is happening to the character is
        // that the foot is staying locked.  So inside this clause, the foot stays fixed.

        // Foot is not pivoting from the lock orientation (it is locked), so reset the pivot control
        // time to zero
        footPivotTime = 0;

        targetEndJointQuat = desiredEndJointQuat;
      }
      // Ensure there is no lag if the threshold has been set to 1 (meaning always pivot to source)
      else if (lockFootChainAttrib->m_footPivotOffsetThreshold < 1.0f)
      {
        // Pivot has been forced, which means we will keep the source orientation, which looks to
        // the user like we are pivoting to the source.

        // Advance time for our step response blend-in controller
        footPivotTime += deltaTime;

        // Reduce pivot based on step response control rule, to prevent snapping
        normalisedTime = xInvTimeConstant * footPivotTime;
        weight = 1.0f - fastStepResponse(normalisedTime);
        NMRU::GeomUtils::shortestArcFastSlerp(targetEndJointQuat, desiredEndJointQuat, weight);

        // During pivoting we unlock pivot so that the orientation is allowed to come to match the
        // input
        pivotIsLocked = footPivotTime > xTimeToGet99PercentOfTheWay ? true : false;
      }

      lockOrientation = targetEndJointQuat;
    }

    //--------------------------------------------
    // FOOT IK

    // Get the TwoBoneIK parameters
    NMRU::TwoBoneIK::Params ikParams;
    ikParams.hingeAxis = lockFootChainAttrib->m_kneeRotationAxis;
    ikParams.endJointOrientationWeight = 1.0f;
    ikParams.swivelOrientationWeight = swivelContributionToOrientation;
    ikParams.preventBendingBackwards = true;
    ikParams.maximumReachFraction = lockFootChainAttrib->m_straightestLegFactor;

    // Get the foot transform in the coordinate frame of the root parent
    NMRU::GeomUtils::PosQuat target = {lockPosition, lockOrientation};
    NMRU::GeomUtils::multiplyInversePosQuat(target, rootParentT);

    // SOLVE
    NMRU::TwoBoneIK::solveWithEndJoint(&ikParams, ikGeometry, target);

    //--------------------------------------------

    // Set results in output buffer as weighted blends.  Modify the ikGeometry array because
    // we may be using it again in the toe penetration section.
    for (uint32_t i = 0; i < 3; ++i)
    {
      uint32_t j = joints[i];
      NMP_ASSERT(outputBuffer->hasChannel(j));
      NMP::Quat* outputQuat = outputBuffer->getPosQuatChannelQuat(j);
      NMRU::GeomUtils::shortestArcFastSlerp(*outputQuat, ikGeometry[i].q, ikFkBlendWeight);
      ikGeometry[i].q = *outputQuat;
    }

  } // if (!tracking)

  //--------------------------------------------
  // Adjust ball joint to fix toe penetration

  if (lockFootChainAttrib->m_fixToeGroundPenetration)
  {
    // Alias - so we make use of the existing posquat for the ball joint
    NMP::Quat& ballIKQuat = ikGeometry[3].q;

    // Get the rotation - we didn't put the ball orientation into the original IK chain.
    ballIKQuat = *outputBuffer->getPosQuatChannelQuat(joints[3]);
    // Be sure to activate the ball joint, since we will now be modifying it
    if (!outputBuffer->hasChannel(joints[3]))
    {
      ballIKQuat = *rig->getBindPoseBoneQuat(joints[3]);
      outputBuffer->setPosQuatChannelQuat(joints[3], ballIKQuat);
      outputBuffer->setChannelUsed(joints[3]);
    }

    // FK out to the toe 'joint' to get its height following IK above.
    NMRU::GeomUtils::PosQuat ankleJointTransform = rootParentT;
    for (uint32_t i = 0; i < 3; ++i)
    {
      NMRU::GeomUtils::premultiplyPosQuat(ikGeometry[i], ankleJointTransform);
    }
    NMRU::GeomUtils::PosQuat ballTransform = ankleJointTransform;
    NMRU::GeomUtils::premultiplyPosQuat(ikGeometry[3], ballTransform);
    NMP::Vector3 toePosLocal = *outputBuffer->getPosQuatChannelPos(joints[4]);
    if (!outputBuffer->hasChannel(joints[4]))
    {
      toePosLocal = *rig->getBindPoseBonePos(joints[4]);
    }
    NMP::Vector3 toePosGlobal = toePosLocal;
    NMRU::GeomUtils::transformVectorPosQuat(ballTransform, toePosGlobal);
    // Actually we want the local toe vector in the ankle orientation frame (ball joint fixed frame)
    toePosLocal = ballIKQuat.rotateVector(toePosLocal);

    // If we need to lift the toe, do the calculation
    if (lockFootChainAttrib->m_toeLowerHeightBound > toePosGlobal[upAxis])
    {
      // This is a very similar calculation for that of the knee joint.  Again, we have
      // a bone rotating around a fixed axis passing through a joint.  This time, however,
      // we are interested in the amount that the toe joint is moved upwards during
      // the rotation.
      //
      // Everything is put into the reference frame of the ball joint's fixed frame.  upV
      // is the unit vector pointing upwards in this frame.  hdesired is the amount we
      // need to move the toe joint in this direction to reach the minimum height bound.
      //
      // cV is the vector along the rotation axis to the centre of the locus of rotation of
      // the toe joint (a circle); then z and y are, like before, unit vectors from cV
      // towards the toe (in its current configuration) and perpendicular to it.  theta is
      // the angle of rotation and toePosLocal is the current position of the toe joint.  l
      // is the radius of the circle of rotation, l = ||toePosLocal - cV||.
      //
      // Thus we get the new position of the toe joint, tprime as:
      //    tprime = cV + lcos(theta)z + lsin(theta)y
      //
      // We want to know the amount, h, that the toe moves upwards during the rotation:
      //    h = (tprime - toePosLocal).upV = hdesired
      //
      // Once again we substitute away the trig functions using t = tan(theta/2), giving
      //    At^2 + Bt + C = 0, where
      //    A = hdesired + l(z.upV)
      //    B = -2l(y.upV)
      //    C = hdesired - cV.upV - l(z.upV) - toePosLocal.upV
      // and thus we can solve a quadratic.  This time we just take the smallest magnitude
      // result for t (and therefore theta) because that will always be correct.

      // Get desired offset from ball joint along vertical axis
      float hdesired = lockFootChainAttrib->m_toeLowerHeightBound - toePosGlobal[upAxis];
      NMP::Vector3 worldUpAxis(0, 0, 0);
      worldUpAxis[upAxis] = 1.0f;
      NMP::Vector3 upV = ankleJointTransform.q.inverseRotateVector(worldUpAxis);

      // Prepare geometry
      NMP::Vector3 cV =
        lockFootChainAttrib->m_ballRotationAxis * lockFootChainAttrib->m_ballRotationAxis.dot(toePosLocal);
      NMP::Vector3 z = toePosLocal - cV;
      float l2 = z.magnitudeSquared();
      float l = NMP::fastSqrt(l2);
      z.fastNormalise();
      NMP::Vector3 y;
      y.cross(lockFootChainAttrib->m_ballRotationAxis, z);

      // Set up quadratic equation
      float lzdotu = l * z.dot(upV);
      float lydotu = l * y.dot(upV);
      //
      // Clamp the change in height to the maximum possible.  This ensures the quadratic below
      // always has a solution.
      float maxH = cV.dot(upV) + NMP::fastSqrt(lzdotu * lzdotu + lydotu * lydotu) - toePosLocal.dot(upV);
      hdesired = NMP::minimum(hdesired, maxH);
      //
      float A = hdesired + 2.0f * lzdotu;
      float B = -2.0f * lydotu;
      float C = hdesired;
      // Solve quadratic for t = tan(theta/2).
      float t1, t2;
      NMRU::GeomUtils::solveQuadratic(A, B, C, t1, t2);
      // Take the smallest magnitude solution
      float t = NMP::floatSelect(NMP::nmfabs(t2) - NMP::nmfabs(t1), t1, t2);

      // Generate ball rotation (on top of current orientation)
      NMP::Quat ballRotation(t * lockFootChainAttrib->m_ballRotationAxis);
      ballIKQuat = ballRotation * ballIKQuat;

      // Adjust weight for blend-back towards the end of the footstep
      NMP_ASSERT(outputBuffer->hasChannel(joints[3]));
      NMRU::GeomUtils::shortestArcFastSlerp(
        *outputBuffer->getPosQuatChannelQuat(joints[3]), ballIKQuat, ikFkBlendWeight);
    }
  }

  //--------------------------------------------

  // Update any remaining state variables for the next step
  lockFootState->m_trajectoryTransform = trajectoryTransform;
  lockFootState->m_previousEndEffectorPosition = endEffectorPosition;
  lockFootState->m_previousEndJointOrientation = endJointQuat;
  lockFootState->m_previouslyInFootStep = inFootStep;

  // Reset the 'full' flag, since this node may have added required missing channels
  outputBuffer->setFullFlag(outputBuffer->getUsedFlags()->calculateAreAllSet());
}
  
//----------------------------------------------------------------------------------------------------------------------
}  // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void TaskLockFootTransforms(Dispatcher::TaskParameters* parameters)
{
  //--------------------------------------------
  // Task parameters

  // Local buffer - the input transforms
  const AttribDataTransformBuffer* localBufferAttrib =
    parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  // Trajectory delta for this frame
  const AttribDataTrajectoryDeltaTransform* deltaTransformAttrib =
    parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(1, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM);

  // Weight for blending between the input and the IK solution
  const AttribDataFloat* ikFkBlendWeightAttrib =
    parameters->getInputAttrib<AttribDataFloat>(2, ATTRIB_SEMANTIC_CP_FLOAT);

  // Weight to control how much swivel is used to help achieve the foot orientation, as opposed to
  // doing it all with rotation at the ankle
  const AttribDataFloat* swivelContributionToOrientationAttrib =
    parameters->getInputAttrib<AttribDataFloat>(3, ATTRIB_SEMANTIC_CP_FLOAT);

  // Animation set independent node attributes
  const AttribDataLockFootSetup* lockFootSetupAttrib =
    parameters->getInputAttrib<AttribDataLockFootSetup>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Animation set dependent node attributes
  const AttribDataLockFootChain* lockFootChainAttrib =
    parameters->getInputAttrib<AttribDataLockFootChain>(5, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // The rig, with hierarchy and bind pose
  const AttribDataRig* rigAttrib = parameters->getInputAttrib<AttribDataRig>(6, ATTRIB_SEMANTIC_RIG);
  const AnimRigDef* rig = rigAttrib->m_rig;
  const uint32_t numRigJoints = rig->getNumBones();

  // Sampled events for this frame, to determine whether or not the foot should be locked
  const AttribDataSampledEvents* sampleEventsBufferAttrib =
    parameters->getInputAttrib<AttribDataSampledEvents>(7, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);

  // The time delta for controlling speed of catch-up after a period of being locked
  const AttribDataUpdatePlaybackPos* timePosAttrib =
    parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(8, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  // State data from the previous frame
  const AttribDataLockFootState* lastLockFootState =
    parameters->getOptionalInputAttrib<AttribDataLockFootState>(9, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  // State data for this frame, to be persisted as input/output
  AttribDataLockFootState* lockFootState =
    parameters->createOutputAttribReplace<AttribDataLockFootState>(9, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  // The output transforms
  AttribDataTransformBuffer* outputBufferAttrib =
    parameters->createOutputAttribTransformBuffer(10, numRigJoints, ATTRIB_SEMANTIC_TRANSFORM_BUFFER);

  subTaskLockFootTransforms(
    localBufferAttrib,
    deltaTransformAttrib->m_deltaPos,
    deltaTransformAttrib->m_deltaAtt,
    ikFkBlendWeightAttrib,
    swivelContributionToOrientationAttrib,
    lockFootSetupAttrib,
    lockFootChainAttrib,
    rigAttrib,
    sampleEventsBufferAttrib,
    timePosAttrib,
    lockFootState,
    lastLockFootState,
    outputBufferAttrib);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskLockFootTrajectoryDeltaAndTransforms(Dispatcher::TaskParameters* parameters)
{
  // Local buffer - the input transforms
  const AttribDataTransformBuffer* localBufferAttrib =
  parameters->getInputAttrib<AttribDataTransformBuffer>(0, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  // Weight for blending between the input and the IK solution
  const AttribDataFloat* ikFkBlendWeightAttrib =
  parameters->getInputAttrib<AttribDataFloat>(1, ATTRIB_SEMANTIC_CP_FLOAT);

  // Weight to control how much swivel is used to help achieve the foot orientation, as opposed to
  // doing it all with rotation at the ankle
  const AttribDataFloat* swivelContributionToOrientationAttrib =
  parameters->getInputAttrib<AttribDataFloat>(2, ATTRIB_SEMANTIC_CP_FLOAT);

  // Animation set independent node attributes
  const AttribDataLockFootSetup* lockFootSetupAttrib =
  parameters->getInputAttrib<AttribDataLockFootSetup>(3, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Animation set dependent node attributes
  const AttribDataLockFootChain* lockFootChainAttrib =
  parameters->getInputAttrib<AttribDataLockFootChain>(4, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  // The rig, with hierarchy and bind pose
  const AttribDataRig* rigAttrib = parameters->getInputAttrib<AttribDataRig>(5, ATTRIB_SEMANTIC_RIG);
  const AnimRigDef* rig = rigAttrib->m_rig;
  const uint32_t numRigJoints = rig->getNumBones();

  // Sampled events for this frame, to determine whether or not the foot should be locked
  const AttribDataSampledEvents* sampleEventsBufferAttrib =
  parameters->getInputAttrib<AttribDataSampledEvents>(6, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);

  // The time delta for controlling speed of catch-up after a period of being locked
  const AttribDataUpdatePlaybackPos* timePosAttrib =
  parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(7, ATTRIB_SEMANTIC_UPDATE_TIME_POS);

  // State data from the previous frame
  const AttribDataLockFootState* lastLockFootState =
    parameters->getOptionalInputAttrib<AttribDataLockFootState>(8, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  // State data for this frame, to be persisted as input/output
  AttribDataLockFootState* lockFootState =
    parameters->createOutputAttribReplace<AttribDataLockFootState>(8, ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE);

  // The output transforms
  AttribDataTransformBuffer* outputBufferAttrib =
  parameters->createOutputAttribTransformBuffer(9, numRigJoints, ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER);

  subTaskLockFootTransforms(
    localBufferAttrib,
    *localBufferAttrib->m_transformBuffer->getPosQuatChannelPos(0),
    *localBufferAttrib->m_transformBuffer->getPosQuatChannelQuat(0),
    ikFkBlendWeightAttrib,
    swivelContributionToOrientationAttrib,
    lockFootSetupAttrib,
    lockFootChainAttrib,
    rigAttrib,
    sampleEventsBufferAttrib,
    timePosAttrib,
    lockFootState,
    lastLockFootState,
    outputBufferAttrib);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataLockFootSetup functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataLockFootSetup::locate(AttribData* target)
{
  AttribDataLockFootSetup* result = (AttribDataLockFootSetup*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_upAxis);
  NMP::endianSwap(result->m_lockVerticalMotion);
  NMP::endianSwap(result->m_assumeSimpleHierarchy);
  NMP::endianSwap(result->m_trackCharacterController);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataLockFootSetup::dislocate(AttribData* target)
{
  AttribDataLockFootSetup* result = (AttribDataLockFootSetup*)target;

  NMP::endianSwap(result->m_upAxis);
  NMP::endianSwap(result->m_lockVerticalMotion);
  NMP::endianSwap(result->m_assumeSimpleHierarchy);
  NMP::endianSwap(result->m_trackCharacterController);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataLockFootSetup::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataLockFootSetup), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataLockFootSetup* AttribDataLockFootSetup::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataLockFootSetup), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataLockFootSetup* result = (AttribDataLockFootSetup*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_LOCK_FOOT_SETUP);
  result->setRefCount(refCount);

  result->m_upAxis = 1;
  result->m_lockVerticalMotion = false;
  result->m_assumeSimpleHierarchy = false;
  result->m_trackCharacterController = false;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataLockFootChain functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataLockFootChain::locate(AttribData* target)
{
  AttribDataLockFootChain* result = (AttribDataLockFootChain*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_kneeRotationAxis);
  NMP::endianSwap(result->m_ballRotationAxis);
  NMP::endianSwap(result->m_footLevelVector);
  for (uint32_t j = 0; j < 5; ++j)
  {
    NMP::endianSwap(result->m_jointIndex[j]);
  }
  NMP::endianSwap(result->m_footfallEventID);
  NMP::endianSwap(result->m_footPivotOffsetThreshold);
  NMP::endianSwap(result->m_catchUpSpeedFactor);
  NMP::endianSwap(result->m_snapToSourceDistance);
  NMP::endianSwap(result->m_straightestLegFactor);
  NMP::endianSwap(result->m_ankleLowerHeightBound);
  NMP::endianSwap(result->m_lowerHeightBound);
  NMP::endianSwap(result->m_toeLowerHeightBound);
  NMP::endianSwap(result->m_fixToeGroundPenetration);
  NMP::endianSwap(result->m_useBallJoint);
  NMP::endianSwap(result->m_fixGroundPenetration);
  NMP::endianSwap(result->m_fixFootOrientation);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataLockFootChain::dislocate(AttribData* target)
{
  AttribDataLockFootChain* result = (AttribDataLockFootChain*)target;

  NMP::endianSwap(result->m_kneeRotationAxis);
  NMP::endianSwap(result->m_ballRotationAxis);
  NMP::endianSwap(result->m_footLevelVector);
  for (uint32_t j = 0; j < 5; ++j)
  {
    NMP::endianSwap(result->m_jointIndex[j]);
  }
  NMP::endianSwap(result->m_footfallEventID);
  NMP::endianSwap(result->m_footPivotOffsetThreshold);
  NMP::endianSwap(result->m_catchUpSpeedFactor);
  NMP::endianSwap(result->m_snapToSourceDistance);
  NMP::endianSwap(result->m_straightestLegFactor);
  NMP::endianSwap(result->m_ankleLowerHeightBound);
  NMP::endianSwap(result->m_lowerHeightBound);
  NMP::endianSwap(result->m_toeLowerHeightBound);
  NMP::endianSwap(result->m_fixToeGroundPenetration);
  NMP::endianSwap(result->m_useBallJoint);
  NMP::endianSwap(result->m_fixGroundPenetration);
  NMP::endianSwap(result->m_fixFootOrientation);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataLockFootChain::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataLockFootChain), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataLockFootChain* AttribDataLockFootChain::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  resource.align(format);

  AttribDataLockFootChain* result = (AttribDataLockFootChain*) resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_LOCK_FOOT_CHAIN);
  result->setRefCount(refCount);

  result->m_kneeRotationAxis.setToZero();
  result->m_ballRotationAxis.setToZero();
  result->m_footLevelVector.setToZero();
  result->m_catchUpSpeedFactor = 0.0f;
  result->m_snapToSourceDistance = 0.0f;
  result->m_straightestLegFactor = 0.0f;
  result->m_ankleLowerHeightBound = 0.0f;
  result->m_lowerHeightBound = 0.0f;
  result->m_toeLowerHeightBound = 0.0f;
  result->m_fixToeGroundPenetration = false;
  result->m_fixGroundPenetration = false;
  result->m_useBallJoint = false;
  result->m_fixFootOrientation = false;
  result->m_footPivotOffsetThreshold = 0;
  for (uint32_t j = 0; j < 5; ++j)
  {
    result->m_jointIndex[j] = 0xFFFFFFFF;
  }
  result->m_footfallEventID = -1;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataLockFootState functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataLockFootState::locate(AttribData* target)
{
  AttribDataLockFootState* result = (AttribDataLockFootState*)target;
  AttribData::locate(target);

  AttribDataTransform::locate(&result->m_trajectoryTransform);
  NMP::endianSwap(result->m_previousEndEffectorPosition);
  NMP::endianSwap(result->m_previousEndJointOrientation);
  NMP::endianSwap(result->m_lockPosition);
  NMP::endianSwap(result->m_lockOrientation);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataLockFootState::dislocate(AttribData* target)
{
  AttribDataLockFootState* result = (AttribDataLockFootState*)target;

  AttribDataTransform::dislocate(&result->m_trajectoryTransform);
  NMP::endianSwap(result->m_previousEndEffectorPosition);
  NMP::endianSwap(result->m_previousEndJointOrientation);
  NMP::endianSwap(result->m_lockPosition);
  NMP::endianSwap(result->m_lockOrientation);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataLockFootState::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataLockFootState), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataLockFootState* AttribDataLockFootState::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  resource.align(format);

  AttribDataLockFootState* result = (AttribDataLockFootState*) resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_LOCK_FOOT_STATE);
  result->setRefCount(refCount);

  result->m_trajectoryTransform.setType(ATTRIB_TYPE_TRANSFORM);
  result->m_trajectoryTransform.m_att = NMP::Quat(NMP::Quat::kIdentity);
  result->m_trajectoryTransform.m_pos = NMP::Vector3Zero();
  result->m_previousEndEffectorPosition = NMP::Vector3Zero();
  result->m_previousEndJointOrientation.identity();
  result->m_lockPosition = NMP::Vector3Zero();
  result->m_lockOrientation.identity();
  result->m_tracking = true;
  result->m_previouslyInFootStep = false;
  result->m_firstFootStep = false;
  result->m_pivotIsLocked = false;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataLockFootState::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc* NMP_UNUSED(desc))
{
  AttribDataHandle result;
  
  // Defaults
  uint16_t refCount = 0;

  result.m_format = getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, refCount);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataLockFootState::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataLockFootState::copy(AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_LOCK_FOOT_STATE && dest->getType() == ATTRIB_TYPE_LOCK_FOOT_STATE);
  *((AttribDataLockFootState*) dest) = *((AttribDataLockFootState*) source);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataLockFootState::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);

  uint32_t dataSize = (uint32_t)sizeof(AttribLockFootStateOutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribLockFootStateOutputData* output = (AttribLockFootStateOutputData*) outputBuffer;
    AttribDataLockFootState* sourceAttrib = (AttribDataLockFootState*)toOutput;

    output->m_trajectoryPos = sourceAttrib->m_trajectoryTransform.m_pos;
    output->m_trajectoryAtt = sourceAttrib->m_trajectoryTransform.m_att;
    output->m_previousEndEffectorPosition = sourceAttrib->m_previousEndEffectorPosition;
    output->m_lockPosition = sourceAttrib->m_lockPosition;
    output->m_tracking = sourceAttrib->m_tracking;
    output->m_previouslyInFootStep = sourceAttrib->m_previouslyInFootStep;
    output->m_firstFootStep = sourceAttrib->m_firstFootStep;
    AttribLockFootStateOutputData::endianSwap(outputBuffer);
  }
  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
