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
// inline included by NMHybridIK.h
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
// HybridIK::PerJointParams construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE HybridIK::PerJointParams::PerJointParams()
{
  init();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE HybridIK::PerJointParams* HybridIK::PerJointParams::init(NMP::Memory::Resource* memDesc)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(HybridIK::PerJointParams));

  return static_cast<HybridIK::PerJointParams*>(memDesc->ptr);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void HybridIK::PerJointParams::init()
{
  IKJointParams::setDefaults();
  bindPose.identity();
  directionHintQuat.setXYZW(0.0f, 0.0f, 0.0f, 2.0f);
  positionWeight = 1.0f;
  endEffectorPoleVectorWeight = 1.0f;
  orientationWeight = 1.0f;
  bindPoseWeight = 0;
  doTwoBoneSolve = false;
  twoBoneRootJointIndex = -1;
  secondaryOnly = false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format HybridIK::PerJointParams::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(HybridIK::PerJointParams), NMP_VECTOR_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
// HybridIK::PerJointParams serialisation

NM_INLINE void HybridIK::PerJointParams::endianSwap(PerJointParams* params)
{
  IKJointParams::endianSwap(params);
  NMP::endianSwap(params->bindPose);
  NMP::endianSwap(params->directionHintQuat);
  NMP::endianSwap(params->positionWeight);
  NMP::endianSwap(params->endEffectorPoleVectorWeight);
  NMP::endianSwap(params->orientationWeight);
  NMP::endianSwap(params->bindPoseWeight);
  NMP::endianSwap(params->doTwoBoneSolve);
  NMP::endianSwap(params->twoBoneRootJointIndex);
  NMP::endianSwap(params->secondaryOnly);
}

//----------------------------------------------------------------------------------------------------------------------
// HybridIK::Params construction and initialisation

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE HybridIK::Params::Params(uint32_t numJoints)
{
  init(numJoints);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE HybridIK::Params* HybridIK::Params::init(NMP::Memory::Resource* memDesc, uint32_t numJoints)
{
  NMP_ASSERT(memDesc);
  NMP_ASSERT(memDesc->format.size >= (int32_t)sizeof(HybridIK::Params));

  // Initialise shell
  Params* result = static_cast<Params*>(memDesc->ptr);
  NMP::Memory::Resource internalMemTrack = *memDesc;
  NMP::Memory::Format shellFormat(sizeof(Params), NMP_NATURAL_TYPE_ALIGNMENT);
  internalMemTrack.increment(shellFormat);

  // Initialise per-joint parameters
  NMP::Memory::Format perJointParamsFormat = PerJointParams::getMemoryRequirements();
  //  Need to call init on all params objects (in case one day it has memory to organise)
  NMP::Memory::Resource oneJointParamsResource;
  oneJointParamsResource.format = perJointParamsFormat;
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    internalMemTrack.align(perJointParamsFormat);
    if (j == 0)
    {
      result->perJointParams = (PerJointParams*)(internalMemTrack.ptr);
    }
    oneJointParamsResource.ptr = internalMemTrack.ptr;
    PerJointParams::init(&oneJointParamsResource);
    internalMemTrack.increment(perJointParamsFormat);
  }

  // Check size
  NMP_ASSERT(internalMemTrack.format.size == 0);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void HybridIK::Params::init(uint32_t numJoints)
{
  maxIterations = 10;
  endEffectorPoleVectorParams.init();
  constraints.clearThenSet(IKCONSTRAINT_PRIMARY_POSITION);
  reverseDirectionOfSolve = false;
  traverseOneWayOnly = false;
  directionHintsFactor = 0;
  targetPositionWeight = 1.0f;
  targetOrientationWeight = 1.0f;
  jointBindPoseWeight = 0;
  accuracy = 1e-5f;
  orientationAccuracy = 0.1f;
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    perJointParams[j].init();
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Memory::Format HybridIK::Params::getMemoryRequirements(uint32_t numJoints)
{
  // Shell
  NMP::Memory::Format result(sizeof(HybridIK::Params), NMP_NATURAL_TYPE_ALIGNMENT);

  // Per-joint params
  // Simply multiplying format size by numJoints would pad out to next alignment boundary,
  // whereas adding does not force the last joint to fill a whole number of alignment blocks,
  // so we use add for efficiency.
  NMP::Memory::Format perJointParamsFormat = PerJointParams::getMemoryRequirements();
  for (uint32_t j = 0; j < numJoints; ++j)
  {
    result += perJointParamsFormat;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// HybridIK solver functions and helpers

//----------------------------------------------------------------------------------------------------------------------
#ifdef NMIK_ENABLE_PRECLAMPING

NM_INLINE void HybridIK::preClampJoints(
  const HybridIK::PerJointParams* jointParams,
  GeomUtils::PosQuat* joints,
  uint32_t numJoints)
{
  for (uint32_t j = 0; j < numJoints; ++j)
    if (jointParams[j].isLimited && jointParams[j].applyHardLimits)
    {
      NMP::Quat qLimitFrame;
  #ifndef NMIK_JOINT_LIMITS_HAVE_IDENTITY_OFFSET_FRAME
      JointLimits::toInternal(jointParams[j].limits, joints[j].q, qLimitFrame);
  #else
      NMP_ASSERT(jointParams[j].limits.offset == NMP::Quat(NMP::Quat::kIdentity));
      qLimitFrame = ~jointParams[j].limits.frame * joints[j].q;
  #endif

      char clampedByWhichLimits = 0;
  #ifdef SMOOTH_LIMITS
      clampedByWhichLimits = JointLimits::clampSimple(jointParams[j].limits, joints[j].q, xLimitSmoothness);
  #else
      clampedByWhichLimits = JointLimits::clampSimple(jointParams[j].limits, joints[j].q);
  #endif
      if (clampedByWhichLimits != 0)
      {
  #ifndef NMIK_JOINT_LIMITS_HAVE_IDENTITY_OFFSET_FRAME
        JointLimits::toJoint(jointParams[j].limits, qLimitFrame, joints[j].q);
  #else
        joints[j].q = jointParams[j].limits.frame * qLimitFrame;
  #endif
      }
    }
}

#else

NM_INLINE void HybridIK::preClampJoints(
  const HybridIK::PerJointParams* NMP_UNUSED(jointParams),
  GeomUtils::PosQuat* NMP_UNUSED(joints),
  uint32_t NMP_UNUSED(numJoints))
{
  // Empty, should be optimised away
}

#endif

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void HybridIK::weightAndClamp(
  const HybridIK::PerJointParams& jointParams,
  const NMP::Quat& qPrev,
  NMP::Quat& qNext,
  bool
#if !(defined(DIRECT_CLAMP) && defined(DIRECT_CLAMP_EVEN_FOR_SWIVEL))
  overrideClampBetween
#endif
  )
{
  // WEIGHT
  GeomUtils::shortestArcFastSlerp(qNext, qPrev, 1.0f - jointParams.weight);

  // Contract limits as desired
  JointLimits::Params tightenedLimits;
  const JointLimits::Params* limits = &jointParams.limits;
  if (xClampTighten != 1.0f) // Optimised away
  {
    tightenedLimits = JointLimits::Params(jointParams.limits, xClampTighten);
    limits = &tightenedLimits;
  }

  // Convert to limit frame
  NMP::Quat qPrevLimitFrame;
  NMP::Quat qNextLimitFrame;
#ifndef NMIK_JOINT_LIMITS_HAVE_IDENTITY_OFFSET_FRAME
  JointLimits::toInternal(*limits, qPrev, qPrevLimitFrame);
  JointLimits::toInternal(*limits, qNext, qNextLimitFrame);
#else
  NMP_ASSERT(jointParams.limits.offset == NMP::Quat(NMP::Quat::kIdentity));
  qPrevLimitFrame = ~limits->frame* qPrev;
  qNextLimitFrame = ~limits->frame* qNext;
#endif

  bool needToConvertBack = false;

  // SOFT LIMIT INFLUENCE
  if (jointParams.applySoftLimits)
  {
    JointLimits::weightBetweenSimple(
      jointParams.limits, qPrevLimitFrame, qNextLimitFrame, jointParams.softLimitSoftness, xLimitSmoothness);
    needToConvertBack = true;
  }

  // HARD LIMIT CLAMP
  if (jointParams.applyHardLimits && jointParams.isLimited)
  {
    char clampedByWhichLimits = 0;

    // Ordinary clamp.  Faster, but potentially causes divergence.
#if !(defined(DIRECT_CLAMP) && defined(DIRECT_CLAMP_EVEN_FOR_SWIVEL)) // Avoid a branch for certain settings
    if (overrideClampBetween)
#endif
    {
#ifdef LIMIT_FLIP_SOFTENING
      // Eliminate flip between limit bounds as desired rotation crosses 180 degrees
      // from the centre of the limit frame.
      if (xFlipSoftness > GeomUtils::nTol)
      {
        needToConvertBack = needToConvertBack ||
          JointLimits::softenNearFlippingPointSimple(qPrevLimitFrame, qNextLimitFrame, xFlipSoftness);
      }
#endif
#ifdef SMOOTH_LIMITS
      clampedByWhichLimits = JointLimits::clampSimple(*limits, qNextLimitFrame, xLimitSmoothness);
#else
      clampedByWhichLimits = JointLimits::clampSimple(*limits, qNextLimitFrame);
#endif
    }
#if !(defined(DIRECT_CLAMP) && defined(DIRECT_CLAMP_EVEN_FOR_SWIVEL)) // Avoid a branch for certain settings
    else
    {
  #ifdef SMOOTH_LIMITS
      clampedByWhichLimits =
        JointLimits::clampBetweenWithBoundaryRepulsionSimple(
        *limits, qPrevLimitFrame, qNextLimitFrame, jointParams.hardLimitRepulsionFactor, xLimitSmoothness);
  #else
      // MORPH-21306: This doesn't work with the hard limit repulsion, i.e. that factor will be ignored.
      // If this is needed make the call above with a limit smoothness of 0 (but it will be less efficient).
      clampedByWhichLimits = JointLimits::clampBetweenSimple(*limits, qPrevLimitFrame, qNextLimitFrame);
  #endif
    }
#endif
    if (clampedByWhichLimits != 0)
    {
      needToConvertBack = true;
    }
  }

  if (needToConvertBack)
  {
#ifndef NMIK_JOINT_LIMITS_HAVE_IDENTITY_OFFSET_FRAME
    JointLimits::toJoint(*limits, qNextLimitFrame, qNext);
#else
    qNext = limits->frame * qNextLimitFrame;
#endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void HybridIK::errorTransform(
  const GeomUtils::PosQuat& joint,
  const GeomUtils::PosQuat& localEndEffector,
  const GeomUtils::PosQuat& localTargetFF,
  const GeomUtils::PosQuat& globalJointTM,
  GeomUtils::PosQuat* errorT)
{
  GeomUtils::PosQuat eeFFW = localEndEffector;
  GeomUtils::rotatePosQuat(joint.q, eeFFW);
  GeomUtils::multiplyPosQuat(eeFFW, globalJointTM);
  *errorT = localTargetFF;
  GeomUtils::multiplyPosQuat(*errorT, globalJointTM);
  GeomUtils::premultiplyInversePosQuat(eeFFW, *errorT);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float HybridIK::positionError(
  const GeomUtils::PosQuat& localEndEffectorFF,
  const GeomUtils::PosQuat& localTargetFF)
{
  return localEndEffectorFF.t.distanceTo(localTargetFF.t);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float HybridIK::orientationError(
  const GeomUtils::PosQuat& localEndEffectorFF,
  const GeomUtils::PosQuat& localTargetFF)
{
  return NMP::nmfabs(1.0f - NMP::nmfabs(localEndEffectorFF.q.dot(localTargetFF.q)));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float HybridIK::poleVectorError(
  const GeomUtils::PosQuat& localEndEffectorFF,
  const GeomUtils::PosQuat& localTargetFF,
  const PoleVectorControlParams& poleVectorParams)
{
  return NMP::nmfabs(1.0f -
    0.5f * (localEndEffectorFF.q.rotateVector(poleVectorParams.controlPoleVector).dot(
      localTargetFF.q.rotateVector(poleVectorParams.referencePoleVector)) + 1.0f));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool HybridIK::closeEnough(
  const GeomUtils::PosQuat& joint,
  const GeomUtils::PosQuat& localEndEffector,
  const GeomUtils::PosQuat& localTargetFF,
  const HybridIK::Params* params)
{
  // End effector orientation in fixed frame
  GeomUtils::PosQuat eeFF;
  GeomUtils::multiplyPosQuat(eeFF, localEndEffector, joint);

  // Position error
  bool closeEnough = true;
  if (closeEnough && (params->constraints & IKCONSTRAINT_POSITION) &&
      params->targetPositionWeight > 0)
  {
    closeEnough = positionError(eeFF, localTargetFF) <= params->accuracy;
  }

  // Orientation error
  if (closeEnough && (params->constraints & IKCONSTRAINT_ORIENTATION) &&
      params->targetOrientationWeight > 0)
  {
    closeEnough = orientationError(eeFF, localTargetFF) <= params->orientationAccuracy;
  }

  // Pole vector error
  if (closeEnough && (params->constraints & IKCONSTRAINT_POLEVECTOR) &&
      params->endEffectorPoleVectorParams.active &&
      params->endEffectorPoleVectorParams.weight > 0)
  {
    closeEnough = poleVectorError(eeFF, localTargetFF, params->endEffectorPoleVectorParams) <=
                  params->orientationAccuracy;
  }

  return closeEnough;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool HybridIK::divergence(
  const NMP::Quat& qPrev,
  const NMP::Quat& qNext,
  const GeomUtils::PosQuat& localEndEffector,
  const GeomUtils::PosQuat& localTargetFF,
  const HybridIK::Params* params)
{
  GeomUtils::PosQuat eePrev = localEndEffector;
  GeomUtils::rotatePosQuat(qPrev, eePrev);
  GeomUtils::PosQuat eeNext = localEndEffector;
  GeomUtils::rotatePosQuat(qNext, eeNext);

  // Position divergence
  if (params->constraints.isSet(IKCONSTRAINT_PRIMARY_POSITION) &&
      eeNext.t.distanceSquaredTo(localTargetFF.t) > (eePrev.t.distanceSquaredTo(localTargetFF.t) + 1e-3f))
  {
    return true;
  }

  // Orientation divergence
  if (params->constraints.isSet(IKCONSTRAINT_PRIMARY_ORIENTATION) &&
      NMP::nmfabs(eeNext.q.dot(localTargetFF.q)) < (NMP::nmfabs(eePrev.q.dot(localTargetFF.q)) - 1e-3f))
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void HybridIK::forRotationToTarget(
  const HybridIK::PerJointParams& jointParams,
  const NMP::Quat& qPrev,
  NMP::Quat& qOut,
  const IKSolverData& solverData)
{
  // Avoid singularities
  if (solverData.localEndEffector.t.magnitudeSquared() > GeomUtils::gTolSq &&
      solverData.localTarget.t.magnitudeSquared() > GeomUtils::gTolSq)
  {
    NMP::Vector3 pointerToEndEffector = NMP::vNormalise(qPrev.rotateVector(solverData.localEndEffector.t));
    NMP::Vector3 pointerToTarget = NMP::vNormalise(solverData.localTarget.t);
    if (jointParams.isHinge)
    {
      qOut =
        GeomUtils::forRotationAroundAxisSoft(
          pointerToEndEffector, pointerToTarget, jointParams.hingeAxis, xHingeSoftness, xFlipSoftness) * qPrev;
    }

    // If this isn't a hinge, then rotate around the shortest arc
    else
    {
      qOut = GeomUtils::forRotationSoft(pointerToEndEffector, pointerToTarget, xFlipSoftness);
      qOut *= qPrev;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void HybridIK::forRotationToTarget(
  const HybridIK::PerJointParams& jointParams,
  const NMP::Quat& qPrev,
  NMP::Quat& qOut,
  float directionHintsFactor,
  const IKSolverData& solverData)
{
  NMP::Vector3 pointerToEndEffectorFF = qPrev.rotateVector(solverData.localEndEffector.t);
  NMP::Vector3 pointerToTargetFF = solverData.localTarget.t;

  // Avoid singularities
  // This causes a discontinuity as the target passes through the joint, where it does nothing,
  // but this is unsolvable anyway and dealt with by flip smoothing.
  //  Save non-normalised versions of the pointer vectors
  NMP::Vector3 targetVec = pointerToTargetFF;
  float distToEndEffectorSquared = pointerToEndEffectorFF.magnitudeSquared();
  float distToTargetSquared = pointerToTargetFF.magnitudeSquared();
  if (distToEndEffectorSquared > GeomUtils::gTolSq &&
      distToTargetSquared > GeomUtils::gTolSq)
  {
    pointerToEndEffectorFF.normalise();
    pointerToTargetFF.normalise();
    if (jointParams.isHinge)
    {
      qOut =
        GeomUtils::forRotationAroundAxisSoft(
          pointerToEndEffectorFF, pointerToTargetFF, jointParams.hingeAxis, xHingeSoftness, xFlipSoftness);
    }

    // If this isn't a hinge, then rotate around the shortest arc
    else
    {
      qOut = GeomUtils::forRotationSoft(pointerToEndEffectorFF, pointerToTargetFF, xFlipSoftness);
    }
    qOut *= qPrev;
  }

  // DIRECTION HINT MODIFIERS
  // Increase contraction to prevent locking, increase extension to improve convergence speed.

  // OPTIMISE  Might be okay to use NMP::approxSqrt() here
  float distToTarget = NMP::fastSqrt(distToTargetSquared);
  float distToEndEffector = NMP::fastSqrt(distToEndEffectorSquared);

  // Blend towards the direction hint orientation if contracting, and increase the rotation
  // returned by the IK above (the call to forRotation) when extending the limb, controlled
  // by the user scale factor and the relative distance of end effector and target.
  // Once again, setting the direction hint quat's w-value to greater than 1 will disable the
  // direction hints system for this joint.
  if (jointParams.directionHintQuat.w <= 1.0f &&
      (distToEndEffector > GeomUtils::nTol || distToTarget < GeomUtils::nTol))
  {
    float distanceFactor;
    if (distToTarget < GeomUtils::nTol)
    {
      distanceFactor = 0;
    }
    else
    {
      distanceFactor = distToTarget / distToEndEffector;
    }

    // Measure the amount of swing motion since the beginning of the solve (to account for secondaries).
    // OPTIMISE if solverData.solveStartQuat equals qPrev, we can just use the current eeVec.
    float scale = distToTarget * distToEndEffector;
    NMP::Vector3 originalEEVec = solverData.solveStartQuat.rotateVector(solverData.localEndEffector.t);
    float originalRotationAmount = 1.0f - GeomUtils::safelyDivide(originalEEVec.dot(targetVec), scale);

    // LIMB CONTRACTING - distance factor between 0 and 1
    // When contracting we want to blend from where we are after IK towards the direction hint
    // direction, which presumably the user has set to be near the centre of the joint limits,
    // if they exist.
    NMP::Quat blendToQuat = jointParams.directionHintQuat;
    //
    // When contracting the blend weight is set so that we will increase the amount we move towards
    // the hint direction as the distance to the target goes down.
    float alpha = 1.0f - distanceFactor;
    float beta = 1.0f - originalRotationAmount;
    float directionHintImportance = alpha * alpha * beta * beta;

    // LIMB EXTENDING - distance factor greater than 1
    // When extending we are going to increase the amount of rotation calculated by the IK call to
    // forRotation above, i.e. overshoot the target - this helps the speed of limb extension, the rate of
    // convergence.  To do this, we simply do a lerp back to the input orientation with a negative blend
    // weight, that is equivalent to a linear extrapolation of the IK rotation.  The blend weight is set
    // equal to the user scale factor, so the user can control the amount of overshoot.  A factor of 1 would
    // typically cause oscillation, but less than 1 should converge correctly, balancing oscillations with
    // speed of convergence.
    if (distanceFactor > 1)
    {
      // We are going to blend back to the original orientation with a negative weight, hence overshoot
      blendToQuat = solverData.solveStartQuat;
      directionHintImportance = -directionHintsFactor;
    }

    // Do the blend using a linear interpolation, since this allows extrapolation as well
    float fromDotTo = qOut.dot(blendToQuat);
    if (fromDotTo < 0)
    {
      blendToQuat = -blendToQuat;
      fromDotTo = -fromDotTo;
    }
    qOut.lerp(blendToQuat, directionHintImportance, fromDotTo);
  }
}

} // end of namespace NMRU
