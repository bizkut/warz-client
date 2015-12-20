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
#include "euphoria/erEuphoriaUserData.h"
#include "euphoria/erLimb.h"
#include "euphoria/erLimbIK.h"
#include "euphoria/erBody.h"
#include "euphoria/erDebugDraw.h"
#define SCALING_SOURCE ((const DimensionalScaling&)(Body::getFromPhysicsRig(m_physicsRig)->getDimensionalScaling()))
#include "euphoria/erDimensionalScalingHelpers.h"

#include "euphoria/erValueValidators.h"
#include "euphoria/erDebugControls.h"
#include "physics/mrPhysicsSerialisationBuffer.h"
#include "NMPlatform/NMProfiler.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "NMGeomUtils/NMJointLimits.h"
#include "physics/mrPhysicsScene.h"

#define IK_DEBUGGING_DRAW_SETTINGSx
#define ALLOW_EFFECTOR_CONTROL_OVERRIDES

#if defined(MR_OUTPUT_DEBUGGING)
// only for debug draw
static const float drawMatrixSizeNormal = 0.2f;
#endif

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
// Limb implementation starts here
//----------------------------------------------------------------------------------------------------------------------
void Limb::create(LimbDef* limbDef, class Body* body)
{
  m_definition = limbDef;
  m_body = body;
  m_isRootLimb = limbDef->m_isRootLimb;

  setPositiveSwivelScale(1.0f);
  setNegativeSwivelScale(1.0f);

  m_leafLimitScale = 1.0f;
  m_lastSwivelAmount = 0.0f; // Note that the guide pose is initialised to the zero swivel pose
  m_incrementalIKIterations = 1;
  m_nonIncrementalIKIterations = 16;

  m_numJointsInChain = getNumJointsInChain();

  m_physicsJointQuats    = (NMP::Quat*) NMPMemoryAlloc(sizeof(NMP::Quat) * m_numJointsInChain);
  m_targetPhysicsJointQuats = (NMP::Quat*) NMPMemoryAlloc(sizeof(NMP::Quat) * m_numJointsInChain);

  MR::PhysicsRigDef* physicsRigDef = (MR::PhysicsRigDef*) m_definition->m_bodyDef->getPhysicsRigDef();

  m_IKJointQuats
    = (NMP::Quat*) NMPMemoryAlloc(sizeof(NMP::Quat) * m_numJointsInChain);
  NMP_ASSERT(m_IKJointQuats);
  m_guidePose
    = (NMP::Quat*) NMPMemoryAlloc(sizeof(NMP::Quat) * m_numJointsInChain);
  NMP_ASSERT(m_guidePose);
  m_forceMultiplier
  = (float*) NMPMemoryAlloc(sizeof(float) * m_numJointsInChain);
  NMP_ASSERT(m_forceMultiplier);

  for (uint32_t i = 0; i < m_numJointsInChain; i++)
  {
    m_IKJointQuats[i].identity();
    m_physicsJointQuats[i].identity();
    m_targetPhysicsJointQuats[i].identity();
    m_guidePose[i] = m_definition->m_zeroSwivelPoseQuats[i];
    m_forceMultiplier[i] = 1.0f;
  }

  // setup bitArrays for debug-draw, feature, and override flags
#if defined(MR_OUTPUT_DEBUGGING)
  NMP::Memory::Resource debugFlagMem =
    NMPMemoryAllocateFromFormat(NMP::BitArray::getMemoryRequirements(kNumDebugDrawFlags));
  m_debugDrawFlags = NMP::BitArray::init(debugFlagMem, kNumDebugDrawFlags);

  NMP::Memory::Resource featureFlagMem =
    NMPMemoryAllocateFromFormat(NMP::BitArray::getMemoryRequirements(kNumFeatureFlags));
  m_featureFlags = NMP::BitArray::init(featureFlagMem, kNumFeatureFlags);
#endif

  // initialise the IK solver
  m_limbIK.init(physicsRigDef, m_definition);
#if defined(MR_OUTPUT_DEBUGGING)
  m_limbIK.setLimbIndex(findLimbIndex());
#endif
  m_limbIK.setGuidePose(m_guidePose);

  // debug flags
#ifdef IK_DEBUGGING_DRAW_SETTINGS
  m_limbIK.setDebugDrawFlag(LimbIK::kDrawTarget, true);
  m_limbIK.setDebugDrawFlag(LimbIK::kDrawFinalResult, true);
  m_limbIK.setDebugDrawFlag(LimbIK::kDrawGuidePose, true);
  m_limbIK.setDebugDrawFlag(LimbIK::kDrawEachIteration, true);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// Copies last twist swing angles held for this limb into angles. Note that it is the responsibility of the caller to
// ensure that angles be an array of length >= number of joints in the chain.
//----------------------------------------------------------------------------------------------------------------------
void Limb::getLatestJointAngles(NMP::Vector3* angles) const
{
  NMP_ASSERT(angles);
  int numJointsInChain = getNumJointsInChain();
  for (int i = 0; i < numJointsInChain; ++i)
  {
    NMRU::GeomUtils::quatToTwistSwing(m_IKJointQuats[i], angles[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Limb::setLatestJointAngles(const NMP::Vector3* angles)
{
  int numJointsInChain = getNumJointsInChain();
  for (int i = 0; i < numJointsInChain; ++i)
  {
    NMRU::GeomUtils::twistSwingToQuat(angles[i], m_IKJointQuats[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Limb::getLatestJointQuats(NMP::Quat* quats) const
{
  int numJointsInChain = getNumJointsInChain();
  for (int i = 0; i < numJointsInChain; ++i)
  {
    quats[i] = m_IKJointQuats[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Limb::setLatestJointQuats(const NMP::Quat* quats)
{
  int numJointsInChain = getNumJointsInChain();
  for (int i = 0; i < numJointsInChain; ++i)
  {
    m_IKJointQuats[i] = quats[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Combined updateCentreOfMass, updateCOMVelocity, updateCOMAngularVelocity into a single loop
//----------------------------------------------------------------------------------------------------------------------
void Limb::updateCentreOfMassState()
{
  // reset components to zero, ready for summation
  m_COMPosition.setToZero();
  m_COMVelocity.setToZero();

  m_mass = 0.0f;

  uint32_t numParts = getTotalNumParts();
  for (uint32_t i = m_isRootLimb ? 0 : 1; i < numParts; ++i)
  {
    const MR::PhysicsRig::Part* part = getPart(i);
    const float mass = part->getAugmentedMass();
    m_COMPosition += mass * part->getAugmentedCOMPosition(); // mass weighted position
    m_COMVelocity +=  mass * part->getVel();
    m_mass += mass;
  }
  m_COMPosition /= m_mass;
  m_COMVelocity /= m_mass;
  m_COMAngularVelocity.setToZero();
  float totalInertia = 0.0f;
  // This is a decent approximation of the angular velocity of the whole limb, you have to take into
  // account the linear velocity around the centre of mass, it makes for a more stable angular vel than
  // just taking the individual angular vels which can get large values from sharp joint movements.
  for (uint32_t i = m_isRootLimb ? 0 : 1; i < numParts; ++i)
  {
    const MR::PhysicsRig::Part* part = getPart(i);
    const float mass = part->getAugmentedMass();
    NMP::Vector3 massSpaceInertia = part->getMassSpaceInertiaTensor();
    // below approximates the inertia, correct for a sphere
    float approxInertia = (massSpaceInertia.x + massSpaceInertia.y + massSpaceInertia.z) / 3.0f;
    m_COMAngularVelocity += approxInertia * part->getAngVel();
    NMP::Vector3 toPart = part->getAugmentedCOMPosition() - m_COMPosition;
    m_COMAngularVelocity += mass * NMP::vCross(toPart, part->getVel());
    totalInertia += mass * toPart.magnitudeSquared() + approxInertia;
  }

  // this is an approximation of the angular vel, in most cases it's accurate.
  m_COMAngularVelocity /= totalInertia;
}

//----------------------------------------------------------------------------------------------------------------------
void Limb::updateContactState()
{
  m_isInContact = false;

  const uint32_t numParts(getTotalNumParts());

  for (uint32_t partIndex = m_isRootLimb ? 0 : 1; !m_isInContact && (partIndex < numParts); ++partIndex)
  {
    const ER::EuphoriaRigPartUserData* const partUserData =
      ER::EuphoriaRigPartUserData::getFromPart(getPart(partIndex));

    NMP_ASSERT(partUserData); // Null check.

    m_isInContact |= (partUserData->getNumContacts() > 0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Limb::setSoftLimitAdjustment(float adjustment, float stiffnessScale)
{
  NMP_ASSERT(adjustment >= 0.0f && adjustment <= 1.0f);
  NMP_ASSERT(stiffnessScale >= 0.0f && stiffnessScale <= 10.0f); // 10 is just a sanity check as it can really go > 1 
  // Note that we only adjust the SL on the joints in the chain - joints connecting additional parts
  // (e.g. toes) are not affected - they should have full SL.
  for (uint32_t i = 0; i < getNumJointsInChain(); ++i)
  {
    m_body->m_softLimitAdjustment[getPhysicsRigJointIndex(i)] = adjustment;
    m_body->m_softLimitStiffnessScale[getPhysicsRigJointIndex(i)] = stiffnessScale;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Limb::applySkinWidthIncrease(float skinWidthIncrease)
{
  uint32_t numParts = getTotalNumParts();
  for (uint32_t limbPartIndex = m_isRootLimb ? 0 : 1; limbPartIndex < numParts; ++limbPartIndex)
  {
    int physicsRigPartIndex = getPhysicsRigPartIndex(limbPartIndex);
    m_physicsRig->setSkinWidthIncrease(physicsRigPartIndex, skinWidthIncrease);
  }
}

//----------------------------------------------------------------------------------------------------------------------
float Limb::calculateBaseToEndLength()
{
  float length = 0;
  const int first = getNumPartsBeforeBase();
  const int last = getNumPartsInChain() - 1;
  for (int i = first; i < last; ++i)
  {
    // By convention the actor transform is in the same position as the joint
    NMP::Vector3 jointPos1 = getPart(i)->getTransform().translation();
    NMP::Vector3 jointPos2 = getPart(i + 1)->getTransform().translation();
    length += (jointPos2 - jointPos1).magnitude();
  }
  length += m_definition->m_endOffset.translation().magnitude();
  return length;
}

//----------------------------------------------------------------------------------------------------------------------
void Limb::initialise()
{
  m_physicsRig = m_body->getPhysicsRig();

  setIsRootExternallySupported(true);
  setIsEndExternallySupported(true);
  setIsOutputEnabledOnAnyJoint(true);

  // Initialise the root/end locators and the CoM state
  postPhysicsStep(0.0f);

  m_IKSolveData.zeroData();
  m_ECP.initData();
  m_ECPOverride.initData();

  m_strengthPrev = 0.0f;
  m_positiveSwivelScale = 1.0f;
  m_negativeSwivelScale = 1.0f;
  m_swivelSense = 0.0f;

#if defined(MR_OUTPUT_DEBUGGING)
  m_controlAmounts.setToZero();
  m_stiffnessFraction = 0.0f;
#endif

  // grab constraint
  m_endConstraint.initialize(this);

  m_externalCompliance = 1.0f;
  m_endFrictionScale = 1.0f;
  m_shouldResetEndFrictionScale = false;

  m_collisionGroupIndexToActivate = -1;

  // Default values for features, overrides and debug-draw flags.
#if defined(MR_OUTPUT_DEBUGGING)
  m_featureFlags->setAll();
  m_debugDrawFlags->clearAll();
#endif
  getIK().setDebugDrawFlag(LimbIK::kDrawRelativeToRoot, true);

  assertIsInitialised(this);
}

//----------------------------------------------------------------------------------------------------------------------
// aux for Limb::init()
//----------------------------------------------------------------------------------------------------------------------
void Limb::EffectorControlParams::initData()
{
  // target and root data (geometry + weights)
  targetPos.setToZero();
  targetNormal.set(0, 1, 0);
  localNormal.set(1, 0, 0);
  targetOrient.identity();
  targetVelocity.setToZero();
  targetAngularVelocity.setToZero();

  rootTargetPos.setToZero();
  rootTargetOrient.identity();
  rootVelocity.setToZero();
  rootAngularVelocity.setToZero();

  targetPositionWeight = 1.0f;
  targetNormalWeight = 1.0f;
  targetOrientationWeight = 0.0f;
  rootTargetPositionWeight = 0.0f;
  rootTargetOrientationWeight = 0.0f;

  strength = 1000.0f;
  damping = 100.0f;
  driveCompensation = 19.0f;
  externalComplianceScale = 1.0f;
  implicitStiffness = 0.0f;
  maxDriveVelocityOffset = 1e10f;
  ikSubStep = 1.0f;
  ikSwivelWeight = 2.0f;
  strengthReductionTowardsEnd = 0.0f;
  useIncrementalIK = 1.0f;

  assertIsInitialised(this);
}

//----------------------------------------------------------------------------------------------------------------------
void Limb::destroy()
{
  // deinit and dealloc IK system
  m_limbIK.deinit();

  NMP::Memory::memFree(m_IKJointQuats);

  NMP::Memory::memFree(m_physicsJointQuats);
  NMP::Memory::memFree(m_targetPhysicsJointQuats);

  NMP::Memory::memFree(m_guidePose);

  NMP::Memory::memFree(m_forceMultiplier);
#if defined(MR_OUTPUT_DEBUGGING)
  NMP::Memory::memFree(m_debugDrawFlags);
  NMP::Memory::memFree(m_featureFlags);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void Limb::calculateIKSubstepTarget(
  NMP::Vector3& targetPos, NMP::Quat& targetQuat, NMP::Vector3& targetNormal,
  const NMP::Vector3& localNormal, const NMP::Quat& rootQuat, const NMP::Vector3& rootPos,
  const NMP::Quat* currentJointQuats, float subStep, MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{
  NMP_ASSERT(1e-6f < subStep && subStep < 1 - 1e-6f);

  // get the current end effector TM and associated normal
  NMP::Matrix34 endEffectorTM = getIK().calcEndEffectorTMFromRootAndJointQuats(
    rootQuat, rootPos, currentJointQuats);
  NMP::Vector3 endEffectorNormal = endEffectorTM.getRotatedVector(localNormal);

  // store results for return to caller
  targetPos.lerp(endEffectorTM.translation(), targetPos, subStep);
  targetQuat.slerp(endEffectorTM.toQuat(), targetQuat, subStep);
  targetNormal.slerp(endEffectorNormal, targetNormal, subStep);

#if defined(MR_OUTPUT_DEBUGGING)
  if (getDebugDrawFlag(kDrawIKSubStepping))
  {
    NMP::Matrix34 advancedTargetTM(targetQuat, targetPos);
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, endEffectorTM, 
      SCALE_DIST(drawMatrixSizeNormal));
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, advancedTargetTM, 
      SCALE_DIST(drawMatrixSizeNormal));
    MR_DEBUG_DRAW_VECTOR(
      pDebugDrawInst,
      MR::VT_Normal,
      advancedTargetTM.translation(),
      SCALE_DIST(0.2f) * targetNormal,
      NMP::Colour::ORANGE);
    MR_DEBUG_DRAW_VECTOR(
      pDebugDrawInst,
      MR::VT_Normal,
      advancedTargetTM.translation(),
      SCALE_DIST(0.2f) * endEffectorNormal,
      NMP::Colour::RED);
  }
#endif // defined(MR_OUTPUT_DEBUGGING)
}

// API's relating to procedural swivel/guide pose config and left/right-handedness
//
//----------------------------------------------------------------------------------------------------------------------
float Limb::getSwivelSense() const
{
  if (isLeftLimb())
  {
    return 1.0f;
  }
  else if (isRightLimb())
  {
    return -1.0f;
  }
  return 0.0f;
}


//----------------------------------------------------------------------------------------------------------------------
float Limb::calculateLimbTwist(MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst)) const
{
  float limbTwist = 0.0f;

  const MR::PhysicsRigPhysX3Articulation* const physicsRig =
    static_cast<MR::PhysicsRigPhysX3Articulation*>(m_body->getPhysicsRig());

  for (int32_t i = 0, jointCount = getNumJointsInChain(); i < jointCount; ++i)
  {
    const int32_t previousJointIndex = i - 1;
    const int32_t currentJointIndex  = i;
    const int32_t nextJointIndex     = i + 1;

    const MR::PhysicsJointDef* const currentJointDef =
      physicsRig->getPhysicsRigDef()->m_joints[getPhysicsRigJointIndex(currentJointIndex)];

    // Find parent and child part transforms.
    const NMP::Matrix34 childPartTM  = 
      m_body->getPhysicsRig()->getPart(currentJointDef->m_childPartIndex)->getTransform();
    const NMP::Matrix34 parentPartTM = 
      m_body->getPhysicsRig()->getPart(currentJointDef->m_parentPartIndex)->getTransform();

    NMP::Matrix34 jointTMChild  = currentJointDef->m_childPartFrame * childPartTM;
    NMP::Matrix34 jointTMParent = currentJointDef->m_parentPartFrame * parentPartTM;

    // Find transform between the joint frames on each part, in world space.
    NMP::Matrix34 jointTMParentInv = jointTMParent;
    jointTMParentInv.invertFast();
    NMP::Matrix34 jointTM = jointTMParentInv * jointTMChild;
    NMP::Vector3 jointRotation = jointTM.toRotationVector();

    // Find twist about parent and child part axis.
    float parentJointTwist = 0.0f;
    float childJointTwist = 0.0f;

    NMP::Vector3 parentTwistAxis(NMP::Vector3::InitZero);

    // Calculate twist in joint about the line from the previous to the current joint.
    if (previousJointIndex >= 0)
    {
      const MR::PhysicsJointDef* const previousJointDef =
        physicsRig->getPhysicsRigDef()->m_joints[getPhysicsRigJointIndex(previousJointIndex)];
      parentTwistAxis =
        (currentJointDef->m_parentPartFrame.translation() - previousJointDef->m_childPartFrame.translation());
      parentTwistAxis.fastNormalise();
      parentPartTM.rotateVector(parentTwistAxis); // Transform into world space.
      parentJointTwist = jointRotation.dot(parentTwistAxis);
    }

    // Calculate twist in joint about the line from the current to the next joint.
    if (nextJointIndex < jointCount)
    {
      const MR::PhysicsJointDef* const nextJointDef =
        physicsRig->getPhysicsRigDef()->m_joints[getPhysicsRigJointIndex(nextJointIndex)];
      NMP::Vector3 childTwistAxis =
        (nextJointDef->m_parentPartFrame.translation() - currentJointDef->m_childPartFrame.translation());
      childTwistAxis.fastNormalise();
      childPartTM.rotateVector(childTwistAxis); // Transform into world space

      if (parentTwistAxis.magnitudeSquared() > 0.0f)
      {
        // only consider component of twist that is orthogonal to parent twist axis to avoid double counting.
        childTwistAxis = childTwistAxis.getComponentOrthogonalToDir(parentTwistAxis);
      }

      childJointTwist = jointRotation.dot(childTwistAxis);
    }

    limbTwist += parentJointTwist + childJointTwist;

#if defined(MR_OUTPUT_DEBUGGING)
    char dbgCharBfr[32];
    NMP_SPRINTF(dbgCharBfr, sizeof(dbgCharBfr), "%.2f", parentJointTwist + childJointTwist);
    MR_DEBUG_DRAW_TEXT(pDebugDrawInst, jointTMChild.translation() + (NMP::Vector3YAxis() * 0.02f), dbgCharBfr,
      NMP::Colour::YELLOW);
    NMP_SPRINTF(dbgCharBfr, sizeof(dbgCharBfr), "%.2f", limbTwist);
    MR_DEBUG_DRAW_TEXT(pDebugDrawInst, jointTMChild.translation() + (NMP::Vector3YAxis() * 0.04f), dbgCharBfr,
      NMP::Colour::RED);
#endif
  }

  return limbTwist;
}


//----------------------------------------------------------------------------------------------------------------------
bool Limb::isLeftLimb() const 
{ 
  if ((m_definition->m_type == ER::LimbTypeEnum::L_leg)
    || (m_definition->m_type == ER::LimbTypeEnum::L_arm))
  {
    return (m_definition->m_defaultPoseEndRelativeToRoot.translation().z < 0.0f);
  }
  return false;
}
//----------------------------------------------------------------------------------------------------------------------
bool Limb::isRightLimb() const 
{ 
  if ((m_definition->m_type == ER::LimbTypeEnum::L_leg)
    || (m_definition->m_type == ER::LimbTypeEnum::L_arm))
  {
    return (m_definition->m_defaultPoseEndRelativeToRoot.translation().z > 0.0f);
  }
  return false;
}


//----------------------------------------------------------------------------------------------------------------------
void Limb::prePhysicsStep(float timeDelta, MR::InstanceDebugInterface* pDebugDrawInst)
{
  NM_BEGIN_PROFILING("Pre-IK");
  calculateGuidePose(pDebugDrawInst);

#if defined(MR_OUTPUT_DEBUGGING)
  if (getDebugDrawFlag(kDrawEndConstraint))
  {
    MR_DEBUG_CONTROL_WITH_PART_ENTER(pDebugDrawInst, ER::kDrawEndContraintControlID, findLimbIndex());
    m_endConstraint.update(timeDelta, pDebugDrawInst);
  }
  else
  {
    m_endConstraint.update(timeDelta, 0);
  }
#else
  m_endConstraint.update(timeDelta, 0);
#endif

  LimbIK::OperatingParams& OPs = getIK().m_OP;

  NMP_ASSERT(getIsOutputEnabledOnAnyJoint());

  for (uint32_t j = 0; j < m_numJointsInChain; ++j)
  {
    const uint32_t iJoint = getPhysicsRigJointIndex(j);
    if (!m_body->getOutputEnabledOnJoint(iJoint))
    {
      continue;
    }

    MR::PhysicsRig::Joint* joint = m_body->getPhysicsRig()->getJoint(iJoint);

    // Increase the damping/decrease compliance for wrists and ankles. Evaluate whether it's really
    // necessary to do this, and if so how the data should be authored. See MORPH-11266. It is the
    // case, though that unless these values are adjusted like this, then the balance quality
    // decreases. Without the multipliers, the ankles tend to be too jittery. When the damping value
    // is too high the ankle doesn't make the foot clear the ground.
    float dampingMult = 1.0f;
    float extComplianceMult = 1.0f;
    if (j == (m_numJointsInChain - 1) && (getType() == ER::LimbTypeEnum::L_leg || getType() == ER::LimbTypeEnum::L_arm))
    {
      dampingMult = 3.0f;
      extComplianceMult = 0.75f;
    }

    // ForceMultiplier is computed in GC and used for example on the ankles to reach the required
    // acceleration spring given the ground resistance.
    float jointStrength = m_forceMultiplier[j] * m_ECP.strength * joint->getDriveStrengthScale();

    // Reduce the strength, damping and external compliance towards the limb end, if requested
    float strengthReductionMultiplier = 1.0f;
    // Note that if the base part is the same as the root part then the base joint is the same as if the base part were
    // the root+1 part.
    const uint32_t numJointsBeforeBase = NMP::maximum(getNumPartsBeforeBase(), (uint32_t)1) - 1;
    const uint32_t numJointsFromBase = m_numJointsInChain - numJointsBeforeBase;
    if (numJointsFromBase > 1 && j > numJointsBeforeBase)
    {
      const float limbFraction = (j - numJointsBeforeBase) / (numJointsFromBase - 1.0f);
      strengthReductionMultiplier = NMP::clampValue(1.0f - limbFraction * m_ECP.strengthReductionTowardsEnd, 0.0f, 1.0f);
      jointStrength *= strengthReductionMultiplier;
    }

    // Really forceMultiplier should be sqrted here, but that results in very jittery feet.
    float jointDamping = m_forceMultiplier[j] * m_ECP.damping * joint->getDriveDampingScale();

    // Write out the strength and damping
    NMP_ASSERT(Validators::FloatValid(jointStrength));
    joint->setStrength(jointStrength);
    NMP_ASSERT(Validators::FloatValid(jointDamping));
    joint->setDamping(jointDamping * dampingMult * strengthReductionMultiplier);

    // allow scaling of the default external compliance.. used in get up, may not be needed if can
    // improve get up.
    float extCompliance = extComplianceMult * m_externalCompliance * m_ECP.externalComplianceScale;
    if (m_ECP.implicitStiffness < 1.0f && extCompliance > 0.0f)
    {
      // convert compensation to compliance
      float s = joint->getStrength();
      float d = joint->getDamping();

      // Recalculate the external compliance so that we get the ISF value that we want.
      // To do this:
      //
      // origISF = (1 + d * timeDelta + s * timeDelta * timeDelta) / extCompliance;
      //
      // We want to reduce the implicit contributions, so do that by reducing the timestep:
      //
      float dt = timeDelta * m_ECP.implicitStiffness;
      float desiredISF = (1 + d * dt + s * dt * dt) / extCompliance;
      //
      // Now solve for modifiedCompliance so that
      //
      // modifiedISF = (1 + d * timeDelta + s * timeDelta * timeDelta) / modifiedCompliance = desiredISF;
      //
      // i.e. origISF * extCompliance / modifiedCompliance = desiredISF
      //
      // and set extCompliance = modifiedCompliance
      NMP_ASSERT(desiredISF > 0.0f);
      extCompliance = (1 + d * timeDelta + s * timeDelta * timeDelta) / desiredISF;
    }
    NMP_ASSERT(Validators::FloatValid(extCompliance));
    joint->setExternalCompliance(extCompliance);

#if defined(MR_OUTPUT_DEBUGGING)
    float jointDriveCompensation = getFeatureFlag(kFeatureDriveCompensation) ?
      m_ECP.driveCompensation * joint->getDriveCompensationScale() : 0.0f;
#else
    float jointDriveCompensation = m_ECP.driveCompensation * joint->getDriveCompensationScale();
#endif
    jointDriveCompensation *= strengthReductionMultiplier;
    NMP_ASSERT(Validators::FloatValid(jointDriveCompensation));
    joint->setDriveCompensation(jointDriveCompensation);
  } // for m_numJointsInChain

  if (m_endFrictionScale != 1.0f)
  {
    m_body->getPhysicsRig()->scaleFrictionOnPart(getEndIndex(), m_endFrictionScale);
    m_shouldResetEndFrictionScale = true;
  }

  // Run IK solver?
  float summedTargetWeights = m_ECP.targetPositionWeight + m_ECP.targetOrientationWeight + m_ECP.targetNormalWeight;
  bool performIK = summedTargetWeights > 1e-6f;

  // Run IK solver.
  if (performIK)
  {
    bool initIKFromPhysics = (m_strengthPrev < 1e-4f) && (m_ECP.strength > 1e-5f);
    NMP_ASSERT(1e-5f < m_ECP.ikSubStep);
    bool usingIKSubStep = (1e-5f < m_ECP.ikSubStep) && (m_ECP.ikSubStep < (1 - 1e-5f));

    // Only calculate the following if it's actually used, i.e. if an IK is performed.
    if (usingIKSubStep || initIKFromPhysics)
    {
      // Grab the the current physics angles if we are sub-stepping.
      for (uint32_t i = 0; i < m_numJointsInChain; i++)
      {
        m_physicsJointQuats[i] = getPhysicsRig()->getJointQuat(getPhysicsRigJointIndex(i));
      }
    }

    // IK root and target inputs to be adjusted wrt the setting of a root target and associated weight
    //
    NMP::Quat rootQuat = m_rootTransform.toQuat();
    NMP_ASSERT(rootQuat.isNormal(EUPHORIA_QUAT_ERROR_LIMIT));
    NMP::Vector3 rootPos = m_rootTransform.translation();
    NMP::Quat targetQuat = m_ECP.targetOrient;
    NMP::Vector3 targetNormal = m_ECP.targetNormal;
    NMP::Vector3 targetPos;

    // 1 root: (rootQuat, rootPos) adjustments
    //
    // Make adjustments implied by the setting of a root target and associated weight
    float timeLag = timeDelta + m_ECP.damping / (m_ECP.strength + 1e-10f);
    if (m_ECP.rootTargetOrientationWeight)
    {
      NMP::Quat rootOffset(m_ECP.rootAngularVelocity * timeLag, false);
      NMP::Quat adjustedRootTarget = rootOffset * m_ECP.rootTargetOrient;
      rootQuat.lerp(
        adjustedRootTarget,
        NMP::minimum(m_ECP.rootTargetOrientationWeight, 1.0f),
        adjustedRootTarget.dot(rootQuat));
    }
    rootPos += (m_ECP.rootTargetPos + m_ECP.rootVelocity * timeLag - rootPos) *
      NMP::minimum(m_ECP.rootTargetPositionWeight, 1.0f);

    // 2 target (targetQuat, targetPos, targetNormal)
    //
    // Make adjustments to account for both requested effector velocity and current root velocity
    // target position and orientation are both displaced to account for the requested target and root velocities tho
    // the position displacement is clamped (to maxDriveVelocityOffset) for stability
    //
    NMP::Vector3 velDriveOffset = (m_ECP.targetVelocity - m_ECP.rootVelocity * m_ECP.rootTargetPositionWeight) * timeLag;
    velDriveOffset.clampMagnitude(m_ECP.maxDriveVelocityOffset);
    velDriveOffset += m_ECP.rootVelocity * m_ECP.rootTargetPositionWeight * timeLag;
    targetPos = m_ECP.targetPos + velDriveOffset;
    NMP::Quat angVel(m_ECP.targetAngularVelocity * timeLag, false);
    targetQuat = angVel * targetQuat;
    targetNormal = angVel.rotateVector(targetNormal);

    NMP_ASSERT(targetQuat.isNormal(EUPHORIA_QUAT_ERROR_LIMIT));
    NMP_ASSERT(rootQuat.isNormal(EUPHORIA_QUAT_ERROR_LIMIT));
    NM_END_PROFILING(); // pre-IK

    NM_BEGIN_PROFILING("Do IK");
    OPs.m_positionWeight = m_ECP.targetPositionWeight;
    OPs.m_orientationWeight = m_ECP.targetOrientationWeight;
    OPs.m_normalWeight = m_ECP.targetNormalWeight;

    if (initIKFromPhysics)
    {
      setLatestJointQuats(m_physicsJointQuats);
      m_limbIK.setInitialJointQuats(m_IKJointQuats);
    }

    OPs.m_maxIterations = m_incrementalIKIterations;

    if (!m_ECP.useIncrementalIK)
    {
      // Overwrite joint quats with guide pose.
      for (uint32_t i = 0; i < m_numJointsInChain; ++i)
      {
        m_IKJointQuats[i] = m_definition->m_zeroSwivelPoseQuats[i];
      }

      m_limbIK.setInitialJointQuats(m_IKJointQuats);
      OPs.m_maxIterations = m_nonIncrementalIKIterations;
    }
    else if (OPs.m_useGuidePose)
    {
      // Blend guide pose into the initial input joint angles.
      for (uint32_t i = 0; i < m_numJointsInChain; ++i)
      {
        if (OPs.m_guidePoseWeights[i] > 0.0f)
        {
          NMRU::GeomUtils::shortestArcFastSlerp(
            m_IKJointQuats[i], m_definition->m_zeroSwivelPoseQuats[i], OPs.m_guidePoseWeights[i]);
        }
      }
      m_limbIK.setInitialJointQuats(m_IKJointQuats);
    }

    // Pre-solve substep fixup: modulate target based on requested and current physics state
    if (usingIKSubStep)
    {
      calculateIKSubstepTarget(
        targetPos,
        targetQuat,
        targetNormal,
        m_ECP.localNormal,
        rootQuat,
        rootPos,
        m_physicsJointQuats,
        m_ECP.ikSubStep,
        pDebugDrawInst);
    }

    // Update the IK joint limits from the current physics rig joints.
    for (uint32_t i = 0; i < m_numJointsInChain; ++i)
    {
      const MR::PhysicsRigPhysX3::JointPhysX3* const joint =
        static_cast< const MR::PhysicsRigPhysX3::JointPhysX3* >(getPhysicsRig()->getJoint(getPhysicsRigJointIndex(i)));

      const NMP::Vector3 limits(
        joint->getModifiableLimits().getTwistLimitLow(),
        joint->getModifiableLimits().getSwing2Limit(),
        joint->getModifiableLimits().getSwing1Limit(),
        joint->getModifiableLimits().getTwistLimitHigh());

      m_limbIK.setSingleJointLimits(i, limits);
    }

    m_limbIK.setSingleJointLimitScale(m_numJointsInChain - 1, m_leafLimitScale);

    NM_BEGIN_PROFILING("IK solve");
    m_limbIK.solve(
      rootQuat,
      rootPos,
      targetQuat,
      targetNormal,
      m_ECP.localNormal,
      targetPos,
      &m_IKSolveData,
      timeDelta,
      pDebugDrawInst,
      getRootTransform(),
      Body::getFromPhysicsRig(m_physicsRig)->getDimensionalScaling());
    NM_END_PROFILING();

    // Use the IK results and apply them to the physics rig
    for (uint32_t i = 0; i != m_numJointsInChain; ++i)
    {
      m_limbIK.getFinalQuat(i, m_IKJointQuats[i]);
      m_targetPhysicsJointQuats[i] = m_IKJointQuats[i];

      uint32_t iJoint = getPhysicsRigJointIndex(i);
      m_physicsRig->getJoint(iJoint)->enableLimit(m_body->getFeatureFlag(Body::kFeatureJointLimits));

      if (!m_body->getOutputEnabledOnJoint(iJoint))
      {
        continue;
      }

      // Set the target angle
      MR::PhysicsRig::Joint* joint = m_body->getPhysicsRig()->getJoint(iJoint);
      NMP_ASSERT(m_targetPhysicsJointQuats[i].isNormal(EUPHORIA_QUAT_ERROR_LIMIT));
      joint->setTargetOrientation(m_targetPhysicsJointQuats[i]);

      // Post-solve substep fixup: Adjust for the reduced angles by increasing the strength (but not
      // damping), since the drive is linear (approximately) in angle.
      if (usingIKSubStep)
      {
        float strength = joint->getStrength();
        strength /= m_ECP.ikSubStep;
        joint->setStrength(strength);
      }
    }
  } // if use IK and strength > 0

  NM_END_PROFILING(); // either pre-IK or

#if defined(MR_OUTPUT_DEBUGGING)
  if (getDebugDrawFlag(kDrawEnabled))
  {
    debugDraw(pDebugDrawInst);
  }
#endif // defined(MR_OUTPUT_DEBUGGING)

  // value of m_strength in previous frame signals when behaviour control is about to be resumed
  m_strengthPrev = m_ECP.strength;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 Limb::getBaseTransform() const
{
  return m_physicsRig->getPart(m_definition->m_baseIndex)->getTransform();
}

//----------------------------------------------------------------------------------------------------------------------
void Limb::postPhysicsStep(float NMP_UNUSED(timeDelta))
{
  m_rootTransform.multiply(m_definition->m_rootOffset, 
    m_physicsRig->getPart(m_definition->m_rootIndex)->getTransform());
  m_endTransform.multiply(m_definition->m_endOffset, 
    m_physicsRig->getPart(m_definition->m_endIndex)->getTransform());

  m_rootTransform.orthonormalise();
  m_endTransform.orthonormalise();

  // if asserts fire here then we know that there isn't enough precision in the physics transforms
  NMP_ASSERT(m_rootTransform.toQuat().isNormal(EUPHORIA_QUAT_ERROR_LIMIT));
  NMP_ASSERT(m_endTransform.toQuat().isNormal(EUPHORIA_QUAT_ERROR_LIMIT));

  updateCentreOfMassState();
  updateContactState();

  // Reset end part friction if it was altered in the pre physics step
  if (m_shouldResetEndFrictionScale)
  {
    m_body->getPhysicsRig()->scaleFrictionOnPart(getEndIndex(), 1.0f);
    m_shouldResetEndFrictionScale = false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Limb::disable()
{
  // Set limb strength and previous limb strength to zero, the previous limb strength determines when behaviour control
  // of the limb is being resumed.
  m_ECP.strength = m_strengthPrev = 0;

  for (uint32_t i = 0, N = getNumJointsInChain() ; i < N; ++i)
  {
    // Only reset the things specific to behaviours here - as otherwise there's a danger that we'll
    // overwrite values set by non-behaviour physics nodes.
    MR::PhysicsRig::Joint* joint = m_body->getPhysicsRig()->getJoint(getPhysicsRigJointIndex(i));
    joint->setDriveCompensation(0.0f);
  }
  // This wipes down various things including the guide pose.
  m_limbIK.resetInternalState();
  // Set the "last swivel amount" to something way out of operating range to ensure guide pose refresh when the limb is
  // re-enabled.
  m_lastSwivelAmount = -FLT_MAX;

  m_endConstraint.disable();

#if defined(MR_OUTPUT_DEBUGGING)
  m_controlAmounts.setToZero();
  m_stiffnessFraction = 0.0f;
#endif // defined(MR_OUTPUT_DEBUGGING)

}

//----------------------------------------------------------------------------------------------------------------------
ER::EuphoriaRigPartUserData* Limb::getClosestContactInDirection(const NMP::Vector3& direction, float& distance)
{
  const int numParts = getNumPartsInChain();
  float closestDist = 1e10f;
  ER::EuphoriaRigPartUserData* closestData = NULL;
  for (int i = 0; i < numParts; i++)
  {
    ER::EuphoriaRigPartUserData* data = ER::EuphoriaRigPartUserData::getFromPart(getPart(i));
    if (data->getNumContacts() > 0)
    {
      NMP::Vector3 normal = data->getLastAverageNormal();
      float dist = (normal - direction).magnitudeSquared();
      if (dist < closestDist)
      {
        closestDist = dist;
        closestData = data;
      }
    }
  }
  if (closestData)
  {
    distance = closestDist;
    return closestData;
  }
  distance = 0.0f;
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void Limb::blendPoses(
  float swivelAmount, const NMP::Quat* neg, const NMP::Quat* zero, const NMP::Quat* pos,
  NMP::Quat* result) const
{
  // Form the weighted sum of either zero and pos or zero and neg poses depending on whether
  // swivelAmount is positive or negative
  float weight = NMP::clampValue(fabsf(swivelAmount), 0.0f, 1.0f);
  const NMP::Quat* to = swivelAmount > 0.0f ? pos : neg;
  const uint32_t numJointsInChain = getNumJointsInChain();
// OPTIMISE: For reachSwivel, only 1 joint seems to swivel so we could do fewer quickSlerps.
  for (uint32_t j = 0; j < numJointsInChain; ++j)
  {
    result[j].quickSlerp(zero[j], to[j], weight);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Computes a blend of the -ve, zero and +ve swivel extremes based on the current swivel amount and stores the result
// as the current guide pose.
//----------------------------------------------------------------------------------------------------------------------
void Limb::calculateGuidePose(MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst))
{

#if defined(MR_OUTPUT_DEBUGGING)

  // The swivel pose extremes.
  //
  if (getDebugDrawFlag(kDrawGuidePoseExtremes))
  {
    MR_DEBUG_CONTROL_WITH_PART_ENTER(pDebugDrawInst, ER::kDrawGuidePoseExtremesControlID, findLimbIndex());

    m_limbIK.drawLimb(pDebugDrawInst, m_definition->m_negativeSwivelPoseQuats,
      NMP::Matrix34::kIdentity, NMP::Vector3(1, 1, 1, 1), true, false, SCALING_SOURCE);
    m_limbIK.drawLimb(pDebugDrawInst, m_definition->m_zeroSwivelPoseQuats, 
      NMP::Matrix34::kIdentity, NMP::Vector3(1, 1, 1, 1), true, false, SCALING_SOURCE);
    m_limbIK.drawLimb(pDebugDrawInst, m_definition->m_positiveSwivelPoseQuats, 
      NMP::Matrix34::kIdentity, NMP::Vector3(1, 1, 1, 1), true, false, SCALING_SOURCE);
  }

  // The swivel pose range.
  //
  if (getDebugDrawFlag(kDrawGuidePoseRange))
  {
    const int numSteps = 20;
    MR_DEBUG_CONTROL_WITH_PART_ENTER(pDebugDrawInst, ER::kDrawGuidePoseRangeControlID, findLimbIndex());
    for (int i = 0; i < numSteps; ++i)
    {
      float swivel = -1.0f + (2.0f * i / (float)numSteps);
      blendPoses(
        swivel,
        m_definition->m_negativeSwivelPoseQuats,
        m_definition->m_zeroSwivelPoseQuats,
        m_definition->m_positiveSwivelPoseQuats,
        m_guidePose);
      m_limbIK.drawLimb(pDebugDrawInst, m_guidePose, 
        NMP::Matrix34::kIdentity, NMP::Vector3(0.5f, 0.5f, 0.5f, 0.5f), false, false, SCALING_SOURCE);
    }
  }
#endif // defined(MR_OUTPUT_DEBUGGING)

  // Compute a blend of the extremals based on the current swivel amount
  //

  // Read the current swivel amount, clamp to range [-1, 1], scale and make into a weight by shift to [0, 1]
  float swivelAmount = NMP::clampValue(m_ECP.ikSwivelWeight, -getNegativeSwivelScale(), getPositiveSwivelScale());

  if (swivelAmount != m_lastSwivelAmount)
  {
    // Blend
    blendPoses(
      swivelAmount,
      m_definition->m_negativeSwivelPoseQuats,
      m_definition->m_zeroSwivelPoseQuats,
      m_definition->m_positiveSwivelPoseQuats, 
      m_guidePose);

    // Result is the guide pose for this frame
    m_limbIK.setGuidePose(m_guidePose);
    m_lastSwivelAmount = swivelAmount;
  }
}

#if defined(MR_OUTPUT_DEBUGGING)
//----------------------------------------------------------------------------------------------------------------------
void Limb::debugDraw(MR::InstanceDebugInterface* pDebugDrawInst)
{
  // If the position weight is zero, the target position is not specified or significant
  // in this case the IK Target is displayed at the current end effector position
  NMP::Vector3 targ = m_ECP.targetPositionWeight ? m_ECP.targetPos : getEndTransform().r[3];
  NMP::Matrix34 tempTarget(m_ECP.targetOrient, targ);
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, tempTarget, 
    m_ECP.targetOrientationWeight * SCALE_DIST(drawMatrixSizeNormal));
  MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Normal, targ, 
    m_ECP.targetNormal * SCALE_DIST(0.2f) * m_ECP.targetNormalWeight, NMP::Colour::LIGHT_ORANGE);
  MR_DEBUG_DRAW_POINT(pDebugDrawInst, targ, 
    SCALE_DIST(0.02f) * m_ECP.targetPositionWeight, NMP::Colour::RED);
  if (m_ECP.rootTargetPositionWeight)
  {
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity,
      m_ECP.rootTargetPos, m_ECP.rootVelocity, NMP::Colour::TURQUOISE);
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, m_ECP.rootTargetPos, 
      SCALE_DIST(0.1f) * m_ECP.rootTargetPositionWeight, NMP::Colour::YELLOW);
  }

  if (m_ECP.rootTargetOrientationWeight)
  {
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngularVelocity, getRootTransform().r[3], 
      m_ECP.rootAngularVelocity, NMP::Colour::PURPLE);
  }

  if (m_ECP.targetPositionWeight)
  {
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_Velocity, m_ECP.targetPos, m_ECP.targetVelocity, NMP::Colour::TURQUOISE);
    float timeDelta = getPhysicsRig()->getPhysicsScene()->getNextPhysicsTimeStep();
    float timeLag = timeDelta + m_ECP.damping / (m_ECP.strength + 1e-10f);
    MR_DEBUG_DRAW_POINT(pDebugDrawInst, m_ECP.targetPos + m_ECP.targetVelocity * timeLag, 
      SCALE_DIST(0.2f), NMP::Colour::BLUE);
  }

  if (m_ECP.targetOrientationWeight > 0.0f || m_ECP.targetNormalWeight > 0.0f)
  {
    MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngularVelocity, m_ECP.targetPos,
      m_ECP.targetAngularVelocity, NMP::Colour::PURPLE);
  }
}
#endif // defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
bool Limb::storeState(MR::PhysicsSerialisationBuffer& savedState)
{
  savedState.addValue(*this);
  savedState.addRawData(m_IKJointQuats, getNumJointsInChain() * sizeof(*m_IKJointQuats));
  //savedState.addRawData(m_guidePose, getNumJointsInChain() * sizeof(*m_guidePose));
  m_limbIK.storeState(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Limb::restoreState(MR::PhysicsSerialisationBuffer& savedState)
{
  // Make sure we're not telling EndConstraint that it isn't constrained anymore by overwriting its m_isConstrained
  // flag, when it actually is constrained. It won't remove the constraint if it thinks it isn't constrained.
  // One option is to disable the constraint explicitly here.
  // m_endConstraint.disable(); // easy version
  // But this doesn't allow to store a state with the character being constrained and restoring it. Hence, instead
  // don't overwrite the constraint, but reset to it's current state. This way, if the character is restored to a non-
  // constrained state, it will destroy itself in the next frame. And if it is restored to a constrained state, then it
  // will continue to be constrained. If this has unforeseen consequences, we can revert to the safer version of
  // simply disabling the constraint.
  physx::PxD6Joint* constraint = m_endConstraint.m_constraint;

  *this = savedState.getValue<Limb>();

  m_endConstraint.m_constraint = constraint;

  savedState.getRawData(m_IKJointQuats);
  //savedState.getRawData(m_guidePose);
  m_limbIK.restoreState(savedState);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
MR::LimbIndex Limb::findLimbIndex() const
{
  MR::LimbIndex numLimbs = (MR::LimbIndex)m_body->getNumLimbs();
  for (MR::LimbIndex i = 0 ; i < numLimbs; ++i)
  {
    if (m_body->getLimb(i).getName() == getName())
      return i;
  }

  return MR::INVALID_LIMB_INDEX;
}
#endif

}
