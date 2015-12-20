/*
* Copyright (c) 2012 NaturalMotion Ltd. All rights reserved.
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
#include "AimBehaviourInterface.h"
#include "MyNetwork.h"
#include "Behaviours/AimBehaviour.h"
#include "NetworkDescriptor.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "euphoria/erBody.h"
#include "euphoria/erRigConstraint.h"
#include "Helpers/Aim.h"
#include "Helpers/Helpers.h"

#define SCALING_SOURCE module.owner->data->dimensionalScaling
#include "euphoria/erDimensionalScalingHelpers.h"

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{
//----------------------------------------------------------------------------------------------------------------------
// Creates and maintains the constraint between the aiming and supporting hands.
// 
static void maintainConstraint(
  const AimBehaviourData& params, 
  const NMP::Matrix34& supportingRelAimingInPoseTM,
  AimBehaviourInterface& module,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  // Create constraint if one is required and we haven't already got one.
  if (params.getEnableConstraint() && (params.getSupportingLimbIndex() >= 0))
  {
    const uint32_t aimingLimbIndex = params.getAimingLimbIndex();
    const uint32_t constrainedLimbIndex = params.getSupportingLimbIndex();

    if (aimingLimbIndex == constrainedLimbIndex)
    {
      // Avoid constraining a part to itself.
      return;
    }

    ER::Body& body = module.owner->getCharacter()->getBody();
    ER::Limb& aimingLimb = body.getLimb(params.getAimingLimbIndex());
    ER::Limb& constrainedLimb = body.getLimb(constrainedLimbIndex);
    const uint32_t aimingLimbEndPartIndex = aimingLimb.getEndIndex();
    const uint32_t constrainedLimbEndPartIndex = constrainedLimb.getEndIndex();

    NMP::Matrix34 constrainedEndPartRelAimingEndPartTM = constrainedLimb.getDefinition()->m_endOffset;
    constrainedEndPartRelAimingEndPartTM.invertFast();
    constrainedEndPartRelAimingEndPartTM.multiply(supportingRelAimingInPoseTM);
    constrainedEndPartRelAimingEndPartTM.multiply(aimingLimb.getDefinition()->m_endOffset);

    constrainedEndPartRelAimingEndPartTM.invertFast();

#if defined(MR_OUTPUT_DEBUGGING)
    // Debug render constraint TM for each end part in world space. They should be the same.
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, body.getPhysicsRig()->getPart(aimingLimbEndPartIndex)->getTransform(), 0.2f);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, constrainedEndPartRelAimingEndPartTM *
      body.getPhysicsRig()->getPart(constrainedLimbEndPartIndex)->getTransform(), 0.2f);
#endif

    MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation* const aimingLimbEndPart =
      static_cast<MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation* const>(
      aimingLimb.getPart(aimingLimb.getNumPartsInChain() - 1));
    MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation* const constrainedLimbEndPart =
      static_cast<MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation* const>(
      constrainedLimb.getPart(constrainedLimb.getNumPartsInChain() - 1));
    NMP_ASSERT(aimingLimbEndPart);
    NMP_ASSERT(constrainedLimbEndPart);

    ER::RigConstraint* constraint = NULL;

    if (module.data->constrainedLimbIndex != MR::INVALID_LIMB_INDEX)
    {
      constraint = body.getRigConstraintManager()->find(aimingLimbEndPartIndex, constrainedLimbEndPartIndex);
    }
    else
    {
      // Make sure constrained limb has a higher dominance group than aiming limb
      const physx::PxDominanceGroup aimingPartDominanceGroup =
        aimingLimbEndPart->getArticulationLink()->getDominanceGroup();
      constrainedLimbEndPart->getArticulationLink()->setDominanceGroup(aimingPartDominanceGroup + 1);

      NMP_ASSERT(body.getRigConstraintManager()->find(aimingLimbEndPartIndex, constrainedLimbEndPartIndex) == NULL);
      constraint = body.createRigConstraint(
        aimingLimbEndPartIndex,
        NMP::Matrix34Identity(),
        constrainedLimbEndPartIndex,
        constrainedEndPartRelAimingEndPartTM);
      NMP_ASSERT(constraint);

      if (constraint)
      {
        constraint->setLinearDof(0, false);
        constraint->setLinearDof(1, false);
        constraint->setLinearDof(2, false);
        constraint->setAngularDof(0, false);
        constraint->setAngularDof(1, false);
        constraint->setAngularDof(2, false);
      }
      module.data->constrainedLimbIndex = constrainedLimbIndex;
      NMP_ASSERT(body.getRigConstraintManager()->find(aimingLimbEndPartIndex, constrainedLimbEndPartIndex) != NULL);
    }

    NMP_ASSERT(constraint);

    // Increase inertia of aiming end part to reduce movement under influence of constraint
    if (constraint &&
      aimingLimbEndPart &&
      constrainedLimbEndPart &&
      (module.data->handSeparation > 0.0f))
    {
      aimingLimbEndPart->setMassSpaceInertia(
        aimingLimbEndPart->getOriginalMassSpaceInertia() * params.getAimingLimbInertiaScale());
      float forceLimit = SCALE_ACCEL(params.getConstraintAccelerationLimit()) *
        constrainedLimbEndPart->getMass() * SCALE_AREA(1.0f) / NMP::sqr(module.data->handSeparation);
      forceLimit *= module.data->supportingLimbStrengthScale;

      const physx::PxD6JointDrive drive(SCALE_STRENGTH(1000.0f), SCALE_DAMPING(1.0f), forceLimit, true);
      constraint->setDrive(physx::PxD6Drive::eX, drive);
      constraint->setDrive(physx::PxD6Drive::eY, drive);
      constraint->setDrive(physx::PxD6Drive::eZ, drive);

      if (params.getEnableOrientationConstraint())
      {
        constraint->setDrive(physx::PxD6Drive::eSLERP, drive);
      }
      else
      {
        const physx::PxD6JointDrive noDrive(0.0f, 0.0f, PX_MAX_F32);
        constraint->setDrive(physx::PxD6Drive::eSLERP, noDrive);
        constraint->setDrive(physx::PxD6Drive::eSWING, noDrive);
        constraint->setDrive(physx::PxD6Drive::eTWIST, noDrive);
      }

      constraint->setLocalPoseForPartB(constrainedEndPartRelAimingEndPartTM); // Note: part A TM is always the identity.
      constraint->keep();
    }
    NMP_ASSERT(body.getRigConstraintManager()->find(aimingLimbEndPartIndex, constrainedLimbEndPartIndex) != NULL);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Destroys the constraint between the aiming and supporting hands.
// 
static void destroyConstraint(
  const AimBehaviourData& params, 
  AimBehaviourInterface& module)
{
  if (module.data->constrainedLimbIndex != MR::INVALID_LIMB_INDEX)
  {
    ER::Body& body  = module.owner->getCharacter()->getBody();
    ER::Limb& aimingLimb  = body.getLimb(params.getAimingLimbIndex());
    ER::Limb& currentLimb = body.getLimb(module.data->constrainedLimbIndex);

    const uint32_t aimingLimbEndPartIndex  = aimingLimb.getEndIndex();
    const uint32_t currentLimbEndPartIndex = currentLimb.getEndIndex();

    body.destroyRigConstraint(aimingLimbEndPartIndex, currentLimbEndPartIndex);

    module.data->constrainedLimbIndex = MR::INVALID_LIMB_INDEX;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Updates the constraint between the aiming and supporting hands
// 
// This function determines when the constraint between aiming and supporting hands should be activated as well as the
// constraints strength and position. This function calls the maintainConstraint or destryConstraint functions as
// appropriate.
// 
static void updateConstaint(
  const bool isActive,
  const AimBehaviourData& params,
  const NMP::Matrix34* const poseEndRelativeToRoots,
  const NMP::Matrix34& supportingRelAimingInPoseTM,
  AimBehaviourInterface& module,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  const uint32_t aimingLimbIndex = params.getAimingLimbIndex();
  const uint32_t constrainedLimbIndex = params.getSupportingLimbIndex();

  bool constraintActive = 
    isActive &&
    params.getEnableConstraint() &&
    params.getAllowConstraintCP() &&
    !module.data->isUnwindingSupportingArm;

  // Disable constraint if separation in pose exceeds threshold.
  if (constraintActive && params.getShouldDisableConstraintOnSeparationInPose())
  {
    const NMP::Vector3& aimingLimbEndRelativePos = poseEndRelativeToRoots[aimingLimbIndex].translation();
    const NMP::Vector3& constrainedLimbEndRelativePos = poseEndRelativeToRoots[constrainedLimbIndex].translation();
    const float separationInPoseSq = (aimingLimbEndRelativePos - constrainedLimbEndRelativePos).magnitudeSquared();
    constraintActive = separationInPoseSq < NMP::sqr(SCALE_DIST(params.getDisableConstraintOnSeparationInPoseMax()));
  }

  // Disable constraint if linear or angular separation in between current and target end TM exceeds threshold.
  if (constraintActive && params.getShouldDisableConstraintOnSeparationInRig())
  {
    const ER::Body& body            = module.owner->getCharacter()->getBody();
    const ER::Limb& aimingLimb      = body.getLimb(aimingLimbIndex);
    const ER::Limb& constrainedLimb = body.getLimb(constrainedLimbIndex);

    // Calculate linear and angular separation between current and target positions for supporting hand.
    NMP::Matrix34 currentToTargetEndTM = constrainedLimb.getEndTransform();
    currentToTargetEndTM.invertFast();
    currentToTargetEndTM = aimingLimb.getEndTransform() * currentToTargetEndTM; 
    currentToTargetEndTM = supportingRelAimingInPoseTM * currentToTargetEndTM;

    const float linearDistanceToTargetSq  = currentToTargetEndTM.translation().magnitudeSquared();
    const float angularDistanceToTargetSq = currentToTargetEndTM.toRotationVector().magnitudeSquared();

    constraintActive =
      (linearDistanceToTargetSq < NMP::sqr(SCALE_DIST(params.getDisableConstraintOnSeparationInRigMax()))) &&
      (angularDistanceToTargetSq < NMP::sqr(NM_PI_OVER_FOUR));
  }

  // Disable constraint if separation between constrained limb final IK constrained position and IK constrained position
  // generated by aim behaviour is greater than threshold. When the separation is large it is assumed that the influence
  // that the aim behaviour currently has over the limb is small and the constraint should be disabled to prevent it
  // interfering with other behaviours.
  if (constraintActive)
  {
    const bool constrainedLimbTypeIsArm = NMP::valueInRange<int>(constrainedLimbIndex,
      module.owner->data->firstArmNetworkIndex,
      module.owner->data->firstArmNetworkIndex + NetworkConstants::networkMaxNumArms - 1);

    if (constrainedLimbTypeIsArm)
    {
      const uint32_t constrainedArmIndex = constrainedLimbIndex - module.owner->data->firstArmNetworkIndex;

      const NMP::Vector3 aimTargetPosition =
        module.feedIn->getArmAimLimbControl(constrainedArmIndex).getTargetPos();
      const NMP::Vector3 finalTargetPosition =
        module.feedIn->getArmFinalLimbControl(constrainedArmIndex).getTargetPos();
      const float separationSq = (aimTargetPosition - finalTargetPosition).magnitudeSquared();

      constraintActive = separationSq < SCALE_AREA(0.1f);
    }
  }

  if (constraintActive)
  {
     maintainConstraint(params, supportingRelAimingInPoseTM, module, pDebugDrawInst);
  }
  else
  {
    destroyConstraint(params, module);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// updateSupportingArm
//
// Calculates the IK target for the supporting hand and calls constraint update functions if appropriate. This function
// attempts to guide the supporting hand toward its position relative to the aiming hand whilst avoiding getting caught
// on or tangled up with the aiming arm.
// 
static void updateSupportingArm(
  const AimBehaviourData& params,
  const NMP::Matrix34* const poseEndRelativeToRoots,
  AimBehaviourInterface& module,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  const uint32_t aimingLimbIndex = params.getAimingLimbIndex();
  const uint32_t supportingLimbIndex = params.getSupportingLimbIndex();

  if (params.getEnableSupportingArm() && (supportingLimbIndex != aimingLimbIndex))
  {
    const ER::Body& body           = module.owner->getCharacter()->getBody();
    const ER::Limb& aimingLimb     = body.getLimb(aimingLimbIndex);
    const ER::Limb& supportingLimb = body.getLimb(supportingLimbIndex);

    // Find separation between supporting and aiming hands in rig.
    const NMP::Vector3 supportingToAimingEndOffsetOnRig = 
      aimingLimb.getEndTransform().translation() - supportingLimb.getEndTransform().translation();
    const float supportingToAimingEndSeparationOnRig =
      NMP::fastSqrt(supportingToAimingEndOffsetOnRig.magnitudeSquared());
    module.data->handSeparation = supportingToAimingEndSeparationOnRig;

    // Find transform from aiming to supporting hand in input pose.
    NMP::Matrix34 supportingRelAimingInPoseTM = poseEndRelativeToRoots[aimingLimbIndex];
    supportingRelAimingInPoseTM.invertFast();
    supportingRelAimingInPoseTM = poseEndRelativeToRoots[supportingLimbIndex] * supportingRelAimingInPoseTM;

    // Find transform from aiming to supporting hand in current rig configuration.
    NMP::Matrix34 supportingRelAimingOnRigTM = aimingLimb.getEndTransform();
    supportingRelAimingOnRigTM.invertFast();
    supportingRelAimingOnRigTM = supportingLimb.getEndTransform() * supportingRelAimingOnRigTM;

    bool constraintActive = true;

    if (params.getShouldDisableHandsOnSeparationInPose() &&
      (supportingRelAimingInPoseTM.translation().magnitudeSquared() >
      NMP::sqr(SCALE_DIST(params.getDisableHandsSeparationInPoseMax()))))
    {
      // Supporting limb end is too far from aiming limb end. Set importance to 0.
      module.data->supportingLimbStrengthScale = 0.0f;
      constraintActive = false;

      // Draw a box to indicate that supporting hand is deactivated.
      MR_DEBUG_DRAW_BOX(pDebugDrawInst,
        supportingLimb.getEndTransform().translation(), 
        SCALE_DIST(NMP::Vector3(0.1f,0.1f,0.1f)), NMP::Colour::LIGHT_GREY);
    }
    else
    {
      module.data->supportingLimbStrengthScale = 1.0f;

      // Determine whether supporting arm is twisted more than pi away from the ideal twist (as specified in attributes).
      module.data->isUnwindingSupportingArm = false;

      const float supportingLimbTwist = supportingLimb.calculateLimbTwist(pDebugDrawInst);
      const NMP::Vector3 limbAxis =
        aimingLimb.getEndTransform().getRotatedVector((supportingLimb.getEndTransform().translation() - 
        supportingLimb.getRootTransform().translation()).getNormalised());

      NMP::Matrix34 rotationFromRigToPose = supportingRelAimingOnRigTM;
      rotationFromRigToPose.invertFast3x3();
      rotationFromRigToPose = supportingRelAimingInPoseTM * rotationFromRigToPose;
      NMP::Vector3 rotationVector = rotationFromRigToPose.toRotationVector();

      const float estimatedTwistForEndTarget = supportingLimbTwist + limbAxis.dot(rotationVector);
      const float idealTwist = NMP::degreesToRadians(params.getSupportingArmTwist());

      if ((estimatedTwistForEndTarget < (idealTwist - NM_PI)) ||
          (estimatedTwistForEndTarget > (idealTwist + NM_PI)))
      {
        // Setting this flag will cause the supporting arm to use non-incremental IK (see updatePerLimbOutputs) which 
        // should unwind it.
        module.data->isUnwindingSupportingArm = true;

        // Deactivate constraint whilst unwinding.
        constraintActive = false;

        // Draw a red box to indicate that unwinding behaviour is active.
        MR_DEBUG_DRAW_BOX(pDebugDrawInst,
          supportingLimb.getEndTransform().translation(), SCALE_DIST(NMP::Vector3(0.1f,0.1f,0.1f)), NMP::Colour::RED);
      }
    }

    updateConstaint(
      constraintActive,
      params, 
      poseEndRelativeToRoots,
      supportingRelAimingInPoseTM,
      module,
      pDebugDrawInst);

      module.data->supportingRelAimingEndTM = supportingRelAimingInPoseTM;
    }
}

//----------------------------------------------------------------------------------------------------------------------
// cacheLimbTargets
//
// Cache pose data received on first frame. This is only called if the Aim behaviour's "UseSingleFrameForPose" attribute 
// is set to true.
// 
static void cacheLimbTargets(
  NMP::Matrix34* const poseEndRelativeToRoots,
  float* const poseWeights,
  const BodyPoseData& bodyPose)
{
  for (uint32_t i = 0 ; i < NetworkMetrics::numLimbs; ++i)
  {
    poseEndRelativeToRoots[i] = bodyPose.m_poseEndRelativeToRoots[i];
    poseWeights[i] = bodyPose.m_poseWeights[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
// updatePerLimbOutputs
// 
// Update pose outputs and aim info structs for each limb.
// 
static void updatePerLimbOutputs(
  const AimBehaviourData& params,
  const NMP::Matrix34* const poseEndRelativeToRoots,
  const float* const poseWeights,
  AimBehaviourInterface& module)
{
  // Here we assume that the incoming data is in the order arms, heads, spine,
  // legs arms. Also... note that the poses and the weights use the network indexing.
  const uint32_t aimingLimbIndex = params.getAimingLimbIndex();
  const uint32_t supportingLimbIndex =
    (params.getEnableSupportingArm())? params.getSupportingLimbIndex() : NetworkConstants::networkMaxNumArms;

  const int aimingArmIndex = aimingLimbIndex - module.owner->data->firstArmNetworkIndex;

  AimInfo aimInfo;

#if defined(MR_OUTPUT_DEBUGGING)
  aimInfo.aimingLimbIndex = static_cast<float>(params.getAimingLimbIndex());

  aimInfo.aimDirection[0] = params.getBarrelDirectionCP().x;
  aimInfo.aimDirection[1] = params.getBarrelDirectionCP().y;
  aimInfo.aimDirection[2] = params.getBarrelDirectionCP().z;
#endif

  // Disable aim on all non-aiming limbs if aim behaviour doesn't have sufficient control over aiming limb.
  // - This should prevent the situation where the behaviour is driving the spine, head, supporting hands
  //   etc whilst the aiming hand is otherwise engaged.
  //   note: This assumes the aiming limb is an arm.
  float nonAimingLimbStrengthScale = 1.0f;

  const bool aimingLimbTypeIsArm = NMP::valueInRange<int>(aimingLimbIndex, 
    module.owner->data->firstArmNetworkIndex,
    module.owner->data->firstArmNetworkIndex + NetworkConstants::networkMaxNumArms - 1);

  if (aimingLimbTypeIsArm)
  {
    // Determine whether aim has a stronger influence over the aiming arm than any other behaviour by requiring that the
    // difference in the final hand target position and the hand target position generated by aim is small.
    const NMP::Vector3 aimTargetPosition =
      module.feedIn->getArmAimLimbControl(aimingArmIndex).getTargetPos();
    const NMP::Vector3 finalTargetPosition =
      module.feedIn->getArmFinalLimbControl(aimingArmIndex).getTargetPos();
    const float separationSq = (aimTargetPosition - finalTargetPosition).magnitudeSquared();

    if (separationSq > SCALE_AREA(0.1f))
    {
      nonAimingLimbStrengthScale = 0.0f;
    }
  }

  // arms
  for (uint32_t i = 0; i < module.owner->data->numArms; ++i)
  {
    const uint32_t index = module.owner->data->firstArmNetworkIndex + i;
    float importance = poseWeights[index] * params.getArmStrengthCP(i);

    float limbControlStrengthScale     = params.getArmStrengthCP(i);
    float limbControlPositionWeight    = 1.0f;
    float limbControlOrientationWeight = 0.8f;
    float poseModificationWeight       = 1.0f;

    aimInfo.useIncrementalIK = true;

    if (index == aimingLimbIndex)
    {
      // The orientation of the aiming limb is paramount as it must point in the right direction.
      limbControlPositionWeight    = 0.8f;
      limbControlOrientationWeight = 1.0f;
    }
    else
    {
      // Reduce control over non-aiming limbs if aiming limb is controlled by another behaviour.
      importance *= nonAimingLimbStrengthScale;
    }

    if (params.getEnableSupportingArm() && (index == supportingLimbIndex))
    {
      limbControlStrengthScale *= module.data->supportingLimbStrengthScale;

      // Set supporting hand target relative to current aiming hand IK solution.
      ER::Body& body = module.owner->getCharacter()->getBody();
      ER::Limb& aimingLimb = body.getLimb(params.getAimingLimbIndex());

      aimInfo.poseEndRelativeToRoot = aimingLimb.getRootTransform();
      aimInfo.poseEndRelativeToRoot.invert();
      aimInfo.poseEndRelativeToRoot = aimingLimb.getIK().m_lastSolutionEndEffectorTM * aimInfo.poseEndRelativeToRoot;
      aimInfo.poseEndRelativeToRoot = module.data->supportingRelAimingEndTM * aimInfo.poseEndRelativeToRoot;

      // Reduce supporting arm strength when aiming arms solution is far from the target to avoid glitchy arm movement.
      limbControlStrengthScale *=
        NMP::clampValue(1.0f - aimingLimb.getIKSolverErrorData().m_positionError / SCALE_DIST(1.0f), 0.0f, 1.0f);

      // Don't modify this pose in Aim Update, just pass it straight to the limb control.
      poseModificationWeight = 0.0f;

      // The position of the supporting limb is paramount as it must be in contact with the aiming limb (if thats whats 
      // its doing in the pose anyway).
      limbControlPositionWeight    = 1.0f;
      limbControlOrientationWeight = 0.8f;

      aimInfo.useIncrementalIK = !module.data->isUnwindingSupportingArm;
    }
    else
    {
      aimInfo.poseEndRelativeToRoot = poseEndRelativeToRoots[index];
    }

    aimInfo.gravityCompensation    = params.getArmGravityCompensation(i);
    aimInfo.dampingScale           = params.getArmDampingScaleCP(i);
    aimInfo.strengthScale          = limbControlStrengthScale;
    aimInfo.swivelAmount           = params.getArmSwivelCP(i);
    aimInfo.limbControlDeltaStep   = params.getArmIKSubStepCP(i);

    aimInfo.limbControlPositionWeight     = limbControlPositionWeight;
    aimInfo.limbControlOrientationWeight  = limbControlOrientationWeight;
    aimInfo.rootRotationCompensation      = params.getArmRootRotationCompensationCP(i);
    aimInfo.poseTwistWeight               = poseModificationWeight;
    aimInfo.poseSwingWeight               = poseModificationWeight;

    module.out->setArmAimInfoAt(i, aimInfo, NMP::clampValue(importance, 0.0f, 1.0f));
  }

  // spine
  for (uint32_t i = 0; i < module.owner->data->numSpines; ++i)
  {
    const uint32_t index = module.owner->data->firstSpineNetworkIndex + i;
    float importance = poseWeights[index] * params.getSpineStrengthCP(i);

    if (index != aimingLimbIndex)
    {
      importance *= nonAimingLimbStrengthScale;
    }

    aimInfo.poseEndRelativeToRoot  = poseEndRelativeToRoots[index];
    aimInfo.gravityCompensation    = params.getSpineGravityCompensation(i);
    aimInfo.dampingScale           = params.getSpineDampingScaleCP(i);
    aimInfo.strengthScale          = params.getSpineStrengthCP(i);
    aimInfo.swivelAmount           = 0.0f;
    aimInfo.limbControlDeltaStep   = 1.0f;
    aimInfo.useIncrementalIK       = true;

    aimInfo.limbControlPositionWeight    = 0.8f;
    aimInfo.limbControlOrientationWeight = 1.0f;
    aimInfo.rootRotationCompensation     = params.getSpineRootRotationCompensationCP(i);

    // Reduce spine modification amounts when feet are off the ground as spine control is poor when not supported and 
    // introduces a lot of jitter to the character's movement.
    aimInfo.poseTwistWeight              = params.getTwistBodyAmount() * module.feedIn->getFeetOnGroundAmount();
    aimInfo.poseSwingWeight              = params.getSwingBodyAmount() * module.feedIn->getFeetOnGroundAmount();

    module.out->setSpineAimInfoAt(i, aimInfo, NMP::clampValue(importance, 0.0f, 1.0f));
  }

  // head
  for (uint32_t i = 0; i < module.owner->data->numHeads; ++i)
  {
    const uint32_t index = module.owner->data->firstHeadNetworkIndex + i;
    float importance = poseWeights[index] * params.getHeadStrengthCP(i);

    if (index != aimingLimbIndex)
    {
      importance *= nonAimingLimbStrengthScale;
    }

    aimInfo.poseEndRelativeToRoot  = poseEndRelativeToRoots[index];
    aimInfo.gravityCompensation    = params.getHeadGravityCompensation(i);
    aimInfo.dampingScale           = params.getHeadDampingScaleCP(i);
    aimInfo.strengthScale          = params.getHeadStrengthCP(i);
    aimInfo.swivelAmount           = 0.0f;
    aimInfo.limbControlDeltaStep   = 1.0f;
    aimInfo.useIncrementalIK       = true;

    aimInfo.limbControlPositionWeight     = 0.0f;
    aimInfo.limbControlOrientationWeight  = 1.0f;
    aimInfo.rootRotationCompensation      = params.getHeadRootRotationCompensationCP(i);
    aimInfo.poseTwistWeight               = params.getTwistHeadAmount();
    aimInfo.poseSwingWeight               = params.getSwingHeadAmount();

    module.out->setHeadAimInfoAt(i, aimInfo, NMP::clampValue(importance, 0.0f, 1.0f));
  }
}

//----------------------------------------------------------------------------------------------------------------------
// updateEffectiveTargetPosition
// 
// Modify target position s.t. it passes around the character, rather than going straight through its body. When the
// target position passes within a sphere of radius equal to the aiming arm's length then it is projected onto the
// surface of that sphere.
static void updateEffectiveTargetPosition(
  const AimBehaviourData& params,
  AimBehaviourInterface& module,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  module.data->targetPosition = params.getTargetPositionInWorldSpaceCP();

  const SpineLimbSharedState& spineLimbState = module.owner->data->spineLimbSharedStates[0];
  NMP::Vector3 targetRelChestDelta = module.data->targetPosition - spineLimbState.m_endTM.translation();
  const float targetRelChestSeperationSq = targetRelChestDelta.magnitudeSquared();
  const float minDistanceToTarget = module.owner->data->defaultPoseLimbLengths[params.getAimingLimbIndex()];

  if (targetRelChestSeperationSq < NMP::sqr(minDistanceToTarget))
  {
    targetRelChestDelta.fastNormaliseOrDef(module.owner->data->up);
    targetRelChestDelta *= minDistanceToTarget;
    module.data->targetPosition = spineLimbState.m_endTM.translation() + targetRelChestDelta;

    MR_DEBUG_DRAW_POINT(pDebugDrawInst, module.data->targetPosition, SCALE_DIST(0.4f), NMP::Colour::LIGHT_BLUE);
  }

  MR_DEBUG_DRAW_POINT(pDebugDrawInst, params.getTargetPositionInWorldSpaceCP(), SCALE_DIST(0.4f), NMP::Colour::DARK_RED);
}

//----------------------------------------------------------------------------------------------------------------------
// updateRigSupportTransform
// 
// Calculate character's current support transform relative to the current pelvis transform by effectively taking the
// mean of the feet transforms. The support TM is then aligned with the ground plane.
// 
static void updateRigSupportTransform(
  AimBehaviourInterface& module, 
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  if (module.data->areFeetOnGround)
  {
    // Find support TM for current rig configuration.
    NMP::Matrix34 pelvisTMInv = module.owner->data->spineLimbSharedStates[0].m_rootTM;
    pelvisTMInv.invertFast();
    NMP::Matrix34 legEndRelRootTM[NetworkConstants::networkMaxNumLegs];
    NMP::Matrix34 legEndTM[NetworkConstants::networkMaxNumLegs];
    NMP::Vector3  meanFootFwdDir(NMP::Vector3::InitZero);

    for (uint32_t i=0; i < module.owner->data->numLegs; ++i)
    {
      const NMP::Matrix34& footTM = module.owner->data->legLimbSharedStates[i].m_endTM;
      legEndRelRootTM[i] = footTM * pelvisTMInv;
      legEndTM[i]        = footTM;
      meanFootFwdDir    += footTM.yAxis();
    }

    // Calculate support TM rel pelvis by finding the mean of the feet rel pelvis TMs.
    module.data->rigSupportRelPelvisTM = getSupportTransformRelRoot(legEndRelRootTM, module.owner->data->numLegs);

    // Find mean world space TM of feet.
    NMP::Matrix34 meanFootTM = getSupportTransformRelRoot(legEndTM, module.owner->data->numLegs);

    // Find mean world space TM of feet rotated into the ground plane (y-axis is aligned with world up).
    meanFootFwdDir.fastNormalise();
    NMP::Matrix34 alignedFootTM(NMP::Matrix34::kIdentity);
    alignedFootTM.yAxis() = module.owner->data->up;
    alignedFootTM.xAxis() =
      NMRU::GeomUtils::calculateOrthogonalPlanarVector(module.owner->data->up, meanFootFwdDir);
    alignedFootTM.xAxis().fastNormalise();
    alignedFootTM.zAxis() =
      NMRU::GeomUtils::calculateOrthogonalPerpendicularVector(meanFootFwdDir, module.owner->data->up);
    alignedFootTM.zAxis().fastNormalise();

    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, meanFootTM, 0.4f);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, alignedFootTM, 0.8f);

    // Find rotation from mean to aligned feet TM
    meanFootTM.invert3x3();
    NMP::Matrix34 rotationFromMeanToAlignedFeetTM = alignedFootTM * meanFootTM;
    rotationFromMeanToAlignedFeetTM.setTranslation(NMP::Vector3Zero());

    // Use rotation from mean to aligned feet TM to align support TM with ground plane.
    module.data->rigSupportRelPelvisTM = rotationFromMeanToAlignedFeetTM * module.data->rigSupportRelPelvisTM;
  }
  else
  {
    module.data->rigSupportRelPelvisTM.identity();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// updatePitchAndYaw
// 
// Calculates the pitch and yaw from the center of the character's support to the target position. These values are 
// written to the behaviour node's output pins and can be used, for example, to drive an animation blend tree of aiming
// poses.
// 
static void updatePitchAndYaw(AimBehaviourInterface& module)
{
  NMP::Vector3 targetDirection = module.data->targetPosition;
  module.owner->data->spineLimbSharedStates[0].m_rootTM.inverseTransformVector(targetDirection);

  // Target bearing is relative to support TM when character is balanced, relative to pelvis when it is not.
  if (module.data->areFeetOnGround)
  {
    module.data->rigSupportRelPelvisTM.inverseTransformVector(targetDirection);
  }

  NMP::Quat rotationFromForwardToTarget;
  rotationFromForwardToTarget.forRotation(NMP::Vector3XAxis(), targetDirection);
  NMP::Vector3 eulerXYZ = rotationFromForwardToTarget.toEulerXYZ();

  module.data->targetPitch = eulerXYZ.z;
  module.data->targetYaw   = eulerXYZ.y;
}

//----------------------------------------------------------------------------------------------------------------------
// shouldDisable
// 
// Returns true if the behaviour should be deactivated after evaluating the deactivate conditions set in the behaviour's
// attributes.
//
static bool shouldDisable(const AimBehaviourData& params, AimBehaviourInterface& module)
{
  bool disable = false;

  if(params.getDisableWhenLyingOnGround() && !disable)
  {
    disable = module.data->isLyingOnGround;
  }

  if(params.getDisableWhenLyingOnFront() && !disable)
  {
    disable = module.feedIn->getSpineInContact() &&
      NMP::vDot(module.owner->data->spineLimbSharedStates[0].m_endTM.frontDirection(), module.owner->data->down) > 0.5f;
  }

  if(params.getDisableWhenTargetOutsideRange() && !disable)
  {
    const float yawMin   = NMP::degreesToRadians(params.getTargetYawRight());
    const float yawMax   = NMP::degreesToRadians(params.getTargetYawLeft());
    const float pitchMin = NMP::degreesToRadians(params.getTargetPitchDown());
    const float pitchMax = NMP::degreesToRadians(params.getTargetPitchUp());

    bool targetInRange = true;
    targetInRange &= NMP::valueInRange(module.data->targetYaw, yawMin, yawMax);
    targetInRange &= NMP::valueInRange(module.data->targetPitch, pitchMin, pitchMax);

    disable = !targetInRange;
  }

  return disable;
}

//----------------------------------------------------------------------------------------------------------------------
// updateTargetPelvisTransform
// 
// Calculates the target pelvis transform which is passed to the balancer.
//
static void updateTargetPelvisTransform(
  const AimBehaviourData& params,
  AimBehaviourInterface& module,
  const NMP::Vector3& aimingDirectionRelPelvis,
  MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  const SpineLimbSharedState& spineLimbState = module.owner->data->spineLimbSharedStates[0];
  NMP::Vector3 rigTargetRelPelvis = module.data->targetPosition;
  spineLimbState.m_rootTM.inverseTransformVector(rigTargetRelPelvis);

  if (module.data->areFeetOnGround &&
      !module.data->isLyingOnGround &&
      (rigTargetRelPelvis.getComponentOrthogonalToDir(NMP::Vector3YAxis()).magnitudeSquared() > SCALE_DIST(0.1f)))
  {
    // Find appropriate pelvis transform from input leg pose.
    NMP::Matrix34 posePelvisTransformRelSupport = module.data->poseSupportRelPelvisTM;
    posePelvisTransformRelSupport.invertFast();

    // Find direction to target relative to world and pose pelvis.
    NMP::Vector3 poseTargetRelPelvis = aimingDirectionRelPelvis;

    // Only rotate the pelvis about the line from the support to pelvis in the input pose to avoid any odd leaning.
    const NMP::Vector3 supportToPelvisDelta = posePelvisTransformRelSupport.translation();
    rigTargetRelPelvis  = rigTargetRelPelvis.getComponentOrthogonalToDir(supportToPelvisDelta);
    poseTargetRelPelvis = poseTargetRelPelvis.getComponentOrthogonalToDir(supportToPelvisDelta);

    NMP::Quat pelvisRotation;
    pelvisRotation.forRotation(poseTargetRelPelvis, rigTargetRelPelvis);

    // Scale pelvis rotation toward target as dictated by "move body amount" parameter.
    NMP::Vector3 axis(NMP::Vector3::InitTypeZero);
    float        angle = 0.0f;
    pelvisRotation.toAxisAngle(axis, angle);

    // Ensure pelvis rotation axis is always pointing in character up direction.
    if(axis.dot(module.owner->data->spineLimbSharedStates[0].m_rootTM.upDirection()) < 0.0f)
    {
      axis  = -axis;
      angle = -angle;
    }

    // Hysteresis on pelvis rotation.
    if ((module.data->lastTargetPelvisRotationAngle * angle < 0.0f) && 
       (module.data->lastTargetPelvisRotationAngle + angle < NM_PI_OVER_FOUR))
    {
      // trying to rotate in opposite directions on successive frames could lead to oscillations.
      angle = module.data->lastTargetPelvisRotationAngle;
    }

    module.data->lastTargetPelvisRotationAngle = angle;

    angle *= NMP::sqr(params.getTwistBodyAmount());
    pelvisRotation.fromAxisAngle(axis, angle);

    // Calculate final pelvis target TM in world space
    const NMP::Matrix34 rigSupportTM = module.data->rigSupportRelPelvisTM
      * module.owner->data->spineLimbSharedStates[0].m_rootTM;
    NMP::Matrix34 targetPelvisTM(NMP::Matrix34::kIdentity);
    targetPelvisTM = posePelvisTransformRelSupport * pelvisRotation;
    targetPelvisTM *= rigSupportTM;
    targetPelvisTM.translation()
      = module.owner->data->spineLimbSharedStates[0].m_rootTM.translation(); // Don't drive pelvis position.

    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, targetPelvisTM, SCALE_DIST(0.6f));

    module.out->setTargetPelvisOrientation(targetPelvisTM.toQuat(), params.getTwistBodyAmount());
    module.out->setTargetPelvisTM(ER::LimbTransform(targetPelvisTM), params.getTwistBodyAmount());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AimBehaviourInterface::update(float NMP_UNUSED(timeStep))
{
  AimBehaviour* const behaviour = 
    static_cast<AimBehaviour*>(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Aim));
  NMP_ASSERT(behaviour);

  MR::InstanceDebugInterface* pDebugDrawInst = NULL;

#if defined(MR_OUTPUT_DEBUGGING)
  const ER::RootModule* rootModule = getRootModule();
  pDebugDrawInst = rootModule->getDebugInterface();
  MR_DEBUG_MODULE_ENTER(pDebugDrawInst, getClassName());
#endif

  const AimBehaviourData& params = behaviour->getParams();
  const SpineLimbSharedState& spineLimbState = owner->data->spineLimbSharedStates[0];

  data->areFeetOnGround = feedIn->getFeetOnGroundAmount() > 0.5f;
  updateRigSupportTransform(*this, pDebugDrawInst);

  // Determine whether character is lying on ground.
  NMP::Vector3 characterUpDir = spineLimbState.m_endTM.translation() - spineLimbState.m_rootTM.translation();
  characterUpDir.fastNormalise();
  data->isLyingOnGround = feedIn->getSpineInContact() && NMP::vDot(owner->data->up, characterUpDir) < 0.5f;

  updateEffectiveTargetPosition(params, *this, pDebugDrawInst);
  updatePitchAndYaw(*this);

  if (shouldDisable(params, *this))
  {
    destroyConstraint(params, *this);
    return;
  }

  const NMP::Matrix34* poseEndRelativeToRoots = NULL;
  const float*         poseWeights = NULL;

  // On the first frame only cache the pose if desired. The flags in poseRequirements tells
  // morpheme whether or not to process the incoming animations.
  if (params.getUseSingleFrameForPose())
  {
    AimPoseRequirements& poseRequirements = behaviour->getPoseRequirements();

    if (poseRequirements.enablePoseInput_Pose)
    {
      cacheLimbTargets(data->cachedPoseEndRelativeToRoots, data->cachedPoseWeights, params.getPose());
      poseRequirements.enablePoseInput_Pose = false;
    }

    poseEndRelativeToRoots = data->cachedPoseEndRelativeToRoots;
    poseWeights            = data->cachedPoseWeights;
  }
  else
  {
    poseEndRelativeToRoots = params.getPose().m_poseEndRelativeToRoots;
    poseWeights            = params.getPose().m_poseWeights;
  }

  // Find support TM for current pose.
  data->poseSupportRelPelvisTM =
    getSupportTransformRelRoot(poseEndRelativeToRoots + owner->data->firstLegNetworkIndex, owner->data->numLegs);

  const int aimingLimbIndex = params.getAimingLimbIndex();
  const bool aimingLimbTypeIsArm  = 
    NMP::valueInRange<int>(aimingLimbIndex, owner->data->firstArmNetworkIndex,
    owner->data->firstArmNetworkIndex + NetworkConstants::networkMaxNumArms - 1);
  const bool aimingLimbTypeIsHead = 
    NMP::valueInRange<int>(aimingLimbIndex, owner->data->firstHeadNetworkIndex,
    owner->data->firstHeadNetworkIndex + NetworkConstants::networkMaxNumHeads - 1);

  // Find target relative to pelvis in input pose TM.
  const NMP::Matrix34& spinePoseEndRelativeToRoot = poseEndRelativeToRoots[owner->data->firstSpineNetworkIndex];
  NMP::Matrix34 aimingLimbPoseEndRelPelvis = poseEndRelativeToRoots[aimingLimbIndex];

  if (aimingLimbTypeIsArm || aimingLimbTypeIsHead)
  {
    // Aiming limb is an arm or head so use spine transform to find target pos relative to root.
    aimingLimbPoseEndRelPelvis =
      aimingLimbPoseEndRelPelvis * spinePoseEndRelativeToRoot;
  }

  // Output aiming limb winding
  NMP::Vector3 poseAimingLimbPos(poseEndRelativeToRoots[aimingLimbIndex].translation());
  NMP::Vector3 targetAimingLimbPos(feedIn->getArmAimLimbControl(aimingLimbIndex).getTargetPos());
  spineLimbState.m_endTM.inverseTransformVector(targetAimingLimbPos);

  data->windingDirection = NMP::vCross(targetAimingLimbPos, poseAimingLimbPos);
  data->windingDirection.fastNormalise();
  out->setWindingDirection(data->windingDirection);

  // Calculate aiming directions for the chest and pelvis in the input pose.
  NMP::Vector3 poseAimDirRelPelvis = params.getBarrelDirectionCP();
  aimingLimbPoseEndRelPelvis.rotateVector(poseAimDirRelPelvis);

  NMP::Vector3 poseAimDirRelChest(poseAimDirRelPelvis);
  spinePoseEndRelativeToRoot.inverseRotateVector(poseAimDirRelChest);
  poseAimDirRelChest.fastNormalise();
  out->setPoseAimDirRelChest(poseAimDirRelChest);
  out->setPoseAimDirRelPelvis(poseAimDirRelPelvis);

  updateSupportingArm(params, poseEndRelativeToRoots, *this, pDebugDrawInst);
  updatePerLimbOutputs(params, poseEndRelativeToRoots, poseWeights,*this);
  updateTargetPelvisTransform(params, *this, poseAimDirRelPelvis, pDebugDrawInst);

  // Output world space target position.
  out->setTargetPositionInWorldSpace(data->targetPosition);

  // Debug rendering
#if defined(MR_OUTPUT_DEBUGGING)
  const AimBehaviourInterface& module = *this; // used by dimensional scaling macros.

  // Support TMs
  const NMP::Matrix34 rigSupportTM = data->rigSupportRelPelvisTM * spineLimbState.m_rootTM;
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, rigSupportTM.translation(), SCALE_DIST(rigSupportTM.xAxis()),
    NMP::Colour::LIGHT_GREEN);

  const NMP::Matrix34 poseSupportTM = data->poseSupportRelPelvisTM * spineLimbState.m_rootTM;
  const NMP::Vector3 poseSupportFwdDir =
    poseSupportTM.xAxis().getComponentOrthogonalToDir(data->rigSupportRelPelvisTM.yAxis());

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, rigSupportTM.translation(), SCALE_DIST(poseSupportFwdDir),
    NMP::Colour::DARK_GREEN); // render at same position as rigSupportTM to illustrate angular separation.

  NMP::Vector3 worldPelvisToTargetDirection = data->targetPosition - spineLimbState.m_rootTM.translation();
  worldPelvisToTargetDirection.fastNormalise();

  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Delta, spineLimbState.m_rootTM.translation(),
    SCALE_DIST(worldPelvisToTargetDirection), NMP::Colour::DARK_GREEN);

  // Character's aiming direction
  const ER::Body& body = owner->getCharacter()->getBody();
  const ER::Limb& aimingLimb = body.getLimb(params.getAimingLimbIndex());
  const NMP::Matrix34& aimingEffectorTM = aimingLimb.getEndTransform();
  const float    lineLength = SCALE_DIST(32.0f);

  NMP::Vector3 aimingDirectionInWorldSpace = params.getBarrelDirectionCP();
  aimingEffectorTM.rotateVector(aimingDirectionInWorldSpace);

  // Draw line in character's aiming direction.
  MR_DEBUG_DRAW_VECTOR(
    pDebugDrawInst,
    MR::VT_Delta,
    aimingEffectorTM.translation(),
    aimingDirectionInWorldSpace * SCALE_DIST(lineLength),
    NMP::Colour::DARK_YELLOW);

  // Draw tick marks on line.
  const uint32_t lineTickCount = 16;
  for (uint32_t i = 0; i < lineTickCount; ++i)
  {
    const NMP::Vector3 tickPosition = aimingEffectorTM.translation() +
      aimingDirectionInWorldSpace * SCALE_DIST(lineLength * static_cast<float>(i + 1) / lineTickCount);
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, tickPosition, SCALE_DIST(0.1f), NMP::Colour::DARK_YELLOW);
  }
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void AimBehaviourInterface::feedback(float NMP_UNUSED(timeStep))
{
  // Set output control parameters.
  AimBehaviour* const behaviour =
    static_cast<AimBehaviour*>(owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Aim));
  NMP_ASSERT(behaviour); // Expect behaviour to exist.

  AimBehaviourData& params = behaviour->getParams();

  params.setTargetPitchOCP(NMP::radiansToDegrees(data->targetPitch));
  params.setTargetYawOCP(NMP::radiansToDegrees(data->targetYaw));
  params.setHandSeparationOCP(data->handSeparation);
}

//----------------------------------------------------------------------------------------------------------------------
void AimBehaviourInterface::entry()
{
  // Reset this in case it was previously set to false (doesn't get cleared)
  AimPoseRequirements& poseRequirements = static_cast<AimBehaviour*>
    (owner->getCharacter()->getBehaviour(NetworkManifest::BehaviourID_Aim))->getPoseRequirements();
  poseRequirements.enablePoseInput_Pose = true;

  data->supportingRelAimingEndTM.identity();
  data->poseSupportRelPelvisTM.identity();
  data->rigSupportRelPelvisTM.identity();
  data->targetPosition.setToZero();
  data->windingDirection.setToZero();

  data->targetPitch                 = 0.0f;
  data->targetYaw                   = 0.0f;
  data->handSeparation              = 0.0f;
  data->supportingLimbStrengthScale = 0.0f;

  data->constrainedLimbIndex = MR::INVALID_LIMB_INDEX;
  data->isUnwindingSupportingArm = false;

  for (uint32_t i = 0; i < NetworkConstants::networkMaxNumLimbs; ++i)
  {
    data->cachedPoseEndRelativeToRoots[i].identity();
    data->cachedPoseWeights[i] = 0.0f;
  }
}

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE