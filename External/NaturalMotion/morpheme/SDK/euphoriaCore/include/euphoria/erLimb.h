// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_LIMB_H
#define NM_LIMB_H

#include "erLimbInterface.h"
#include "erEndConstraint.h"
#include "erLimbDef.h"
#include "erLimbIK.h"

namespace NMP
{
class BitArray;
}

namespace MR
{
  class PhysicsRig;
  struct PhysicsSerialisationBuffer;
}

namespace ER
{

class BodyDef;
class Body;

//----------------------------------------------------------------------------------------------------------------------
/// Limb controller: provides minimal interface needed to control (IK) limbs
//----------------------------------------------------------------------------------------------------------------------
class Limb : public LimbInterface
{

public:

  friend struct EffectorControlParams;

  //--------------------------------------------------------------------------------------------------------------------
  // LimbInterface API
  //--------------------------------------------------------------------------------------------------------------------

  char* getName() const NM_OVERRIDE;
  LimbIK& getIK() NM_OVERRIDE { return m_limbIK; }
  const LimbIK& getIK() const NM_OVERRIDE { return m_limbIK; }
  Body* getBody() const NM_OVERRIDE { return m_body; };
  MR::PhysicsRig* getPhysicsRig() const NM_OVERRIDE { return m_physicsRig; };
  LimbDef* getDefinition() const NM_OVERRIDE { return m_definition; };

  ReachLimit getReachLimit() { return m_definition->m_reachLimit; };
  uint32_t getNumPartsInChain() const NM_OVERRIDE;
  uint32_t getTotalNumParts() const NM_OVERRIDE;
  uint32_t getNumJointsInChain() const NM_OVERRIDE;
  uint32_t getRootIndex() const NM_OVERRIDE;
  uint32_t getEndIndex() const NM_OVERRIDE;
  uint32_t getBaseIndex() const NM_OVERRIDE;
  uint32_t getMidIndex() const NM_OVERRIDE;
  uint32_t getNumPartsBeforeBase() const NM_OVERRIDE;
  int getPhysicsRigPartIndex(int limbPartIndex) const NM_OVERRIDE;
  int getPhysicsRigJointIndex(int limbJointIndex) const NM_OVERRIDE;
  MR::PhysicsRig::Part* getPart(int limbPartIndex) NM_OVERRIDE;
  const MR::PhysicsRig::Part* getPart(int limbPartIndex) const NM_OVERRIDE;
  MR::PhysicsRig::Joint* getJoint(int physRigJointIndex) NM_OVERRIDE;

  bool getIsRootLimb() const NM_OVERRIDE { return m_isRootLimb; }
  LimbTypeEnum::Type getType() const NM_OVERRIDE { return m_definition->m_type; }

  const NMP::Vector3& getCentreOfMass() const NM_OVERRIDE { return m_COMPosition; }
  const NMP::Vector3& getCOMVelocity() const NM_OVERRIDE { return m_COMVelocity; }
  const NMP::Vector3& getCOMAngularVelocity() const NM_OVERRIDE { return m_COMAngularVelocity; }
  float getMass() const NM_OVERRIDE { return m_mass; }

  void getLatestJointAngles(NMP::Vector3* angles) const NM_OVERRIDE;
  void setLatestJointAngles(const NMP::Vector3* angles) NM_OVERRIDE;
  void getLatestJointQuats(NMP::Quat* quats) const NM_OVERRIDE;
  void setLatestJointQuats(const NMP::Quat* quats) NM_OVERRIDE;

#if defined(MR_OUTPUT_DEBUGGING)
  bool getDebugDrawFlag(DebugDrawId id) const NM_OVERRIDE { return m_debugDrawFlags->isBitSet(id) != 0; };
  void setDebugDrawFlag(DebugDrawId id, bool flag) NM_OVERRIDE { setBitFlag(m_debugDrawFlags, id, flag); };
  bool getFeatureFlag(FeatureId id) const NM_OVERRIDE { return m_featureFlags->isBitSet(id) != 0; };
  void setFeatureFlag(FeatureId id, bool flag) NM_OVERRIDE { setBitFlag(m_featureFlags, id, flag); };
#else
  bool getDebugDrawFlag(DebugDrawId NMP_UNUSED(id)) const NM_OVERRIDE { return false; }
  void setDebugDrawFlag(DebugDrawId NMP_UNUSED(id), bool NMP_UNUSED(flag)) NM_OVERRIDE {  }
  bool getFeatureFlag(FeatureId NMP_UNUSED(id)) const NM_OVERRIDE { return true; }
  void setFeatureFlag(FeatureId NMP_UNUSED(id), bool NMP_UNUSED(flag)) NM_OVERRIDE { }
#endif

#if defined(MR_OUTPUT_DEBUGGING)
  MR::LimbIndex findLimbIndex() const;
  void setDebugControlAmounts(const LimbControlAmounts& controlAmounts, float stiffnessFraction) NM_OVERRIDE {
    m_controlAmounts = controlAmounts; m_stiffnessFraction = stiffnessFraction;}
  const LimbControlAmounts& getDebugControlAmounts(float& stiffnessFraction) const NM_OVERRIDE {
    stiffnessFraction = m_stiffnessFraction; return m_controlAmounts;}
#endif // defined(MR_OUTPUT_DEBUGGING)

  //--------------------------------------------------------------------------------------------------------------------

  // API's for driving the class creation, init, update, serialisation etc.
  //
  // Allocate memory, but you don't have an actual physicalBody to work on
  void create(LimbDef* limbDef, class Body* body);
  // you now know the physical character, but no memory allocating here, this is why we don't need a deinitialise
  void initialise();
  void destroy();
  void prePhysicsStep(float timeDelta, MR::InstanceDebugInterface* pDebugDrawInst);
  void postPhysicsStep(float timeDelta);
  void disable();

  bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  bool restoreState(MR::PhysicsSerialisationBuffer& savedState);

  //--------------------------------------------------------------------------------------------------------------------

  /// Calculates distance from the base part's parent actor position (which will be its parent
  /// joint) to the end locator
  float calculateBaseToEndLength();

  /// Gets the end relative to the root (including the locator) offsets when in the default pose
  const NMP::Matrix34& getDefaultPoseEndRelativeToRoot() const { return m_definition->m_defaultPoseEndRelativeToRoot; }
  /// Gets the end relative to the root (including the locator) offsets when in the default pose
  const NMP::Quat& getDefaultPoseEndQuatRelativeToRoot() const { return m_definition->m_defaultPoseEndQuatRelativeToRoot; }
  float getDefaultPoseLimbLength() const { return m_definition->m_defaultPoseLimbLength; }
  /// Gets the end relative to the root (including the locator) offsets when in the zero pose
  const NMP::Matrix34& getZeroPoseEndRelativeToRoot() const { return m_definition->m_zeroPoseEndRelativeToRoot; }

  // Access to changing state
  //
  /// Get the end transform after applying the end transform offset
  const NMP::Matrix34& getEndTransform() const { return m_endTransform; }
  /// Get the root transform after applying the root transform offset
  const NMP::Matrix34& getRootTransform() const { return m_rootTransform; }
  /// get the base transform
  NMP::Matrix34 getBaseTransform() const;

  // Interactions between the limb and the environment
  //
  /// Finds the deepest contact in a rough direction
  EuphoriaRigPartUserData* getClosestContactInDirection(const NMP::Vector3& direction, float& distance);

  /// Returns true if at least one physics part is in contact with a physics object.
  bool isInContact() const { return m_isInContact; }

  /// Access to whether the limb is deemed to be supported by an external constraint or SK/HK
  bool getRootIsExternallySupported() const { return m_isRootExternallySupported; }
  bool getEndIsExternallySupported() const { return m_isEndExternallySupported; }
  bool getIsAtAllExternallySupported() const { return m_isRootExternallySupported || m_isEndExternallySupported; }

  /// Access to set whether the limb is deemed to be supported by an external constraint or SK/HK
  void setIsRootExternallySupported(bool b) { m_isRootExternallySupported = b; }
  void setIsEndExternallySupported(bool b) { m_isEndExternallySupported = b; }

  /// Access to the current end effector constraint
  EndConstraint& getEndConstraint() { return m_endConstraint; };
  const EndConstraint& getEndConstraint() const { return m_endConstraint; };

  /// Access to whether the limb is live (i.e. under Euphoria control)
  bool getIsOutputEnabledOnAnyJoint() { return m_isOutputEnabledOnAnyJoint; }
  /// Access set to whether the limb is live (i.e. under Euphoria control)
  void setIsOutputEnabledOnAnyJoint(bool b) { m_isOutputEnabledOnAnyJoint = b; }

  //--------------------------------------------------------------------------------------------------------------------

  // Access to "Effector Control Parameters": input for IK mostly from the behaviour network updated each frame.
  //
  // wholesale version of the various settor/gettor's below
  void setEffectorControlParams(const EffectorControlParams& ecp) { m_ECP = ecp; }

  /// Returns the errors between the IK target and the last IK solution
  const ER::LimbIK::SolverErrorData& getIKSolverErrorData() const { return m_IKSolveData; }

  //--------------------------------------------------------------------------------------------------------------------

  void setForceMultiplier(int index, float forceMultiplier) { m_forceMultiplier[index] = forceMultiplier; }
  float getForceMultiplier(int index) const { return m_forceMultiplier[index]; }

  /// Adjustment must be >= 0 and <= 1 - see ER::Body::m_softLimitAdjustment
  void setSoftLimitAdjustment(float adjustment, float stiffnessScale);

  /// Scale the external compliance requested by behaviours - applied before the implicit stiffness
  /// contribution is applied.
  void setExternalComplianceScale(float compliance) { m_externalCompliance = compliance; }

  /// Scale the friction on the physics part at the end of this limb. The friction value is set to this scale times the
  /// value in the physics rig def during the pre physics update. The friction value is set back to the one stored in
  /// the rig def in the post physics update.
  void setEndFrictionScale(float scale) { m_endFrictionScale = scale; }
  float getEndFrictionScale() const { return m_endFrictionScale; }

  /// Enables the collision set index (i.e. disables collision between all objects that have this
  /// collision set, which would normally be inactive by default.
  void activateCollisionGroupIndex(int index) { m_collisionGroupIndexToActivate = index; }
  int  getCollisionGroupIndexToActivate() const { return m_collisionGroupIndexToActivate; }

  /// Increases the skin width above the authored value on the parts associated with this limb. They
  /// will get reset after the physics update.
  void applySkinWidthIncrease(float skinWidthIncrease);

  //--------------------------------------------------------------------------------------------------------------------

  // API's relating to procedural swivel/guide pose config
  //
  
  /// Returns true if a left arm or leg
  bool isLeftLimb() const NM_OVERRIDE;
  /// Returns true if a right arm or leg
  bool isRightLimb() const NM_OVERRIDE;

  /// Returns 1.0 for left arm or leg -1.0 for right and zero otherwise
  // Notes:
  // Swivel sense (determines which way round a limb swivels away from neutral pose, given
  // a certain swivel value in [-1, 1], left and right limbs have opposite swivel sense so
  // that the same swivel value applied for e.g. to both left and right arms will either
  // swivel both elbows in or both elbows out).
  // Heads and Spines don't need "swivel" so swivel-sense is set to 0
  // Arms/Legs ("left-/right-ness" determines +/-ve swivel, which is here
  // determined by left/rightness of limb end wrt it's root in the "default pose")
  // The arm/leg calc depends on a suitable default pose, in particular, one which has the
  // end effectors of "right" limbs to right of root. Also depends on the convention of spine
  // end and roots being setup with z pointing to right.
  float getSwivelSense() const;

  // Swivel scale multipliers appear in the guide pose calculation and potentially provide a means 
  // to restrict the effective range of motion (not currently exposed to the connect user).
  float getPositiveSwivelScale() const { return m_positiveSwivelScale; }
  void setPositiveSwivelScale(float f) { m_positiveSwivelScale = f; }
  float getNegativeSwivelScale() const { return m_negativeSwivelScale; }
  void setNegativeSwivelScale(float f) { m_negativeSwivelScale = f; }

  void setLeafLimitScale(float scale) { m_leafLimitScale = scale; }

  // Returns the sum of the twist in each joint in the limb. The twist of each joint is measured as the rotation between
  // the joint frames about the vector from the previous to the current joint and the vector from the current to the
  // next joint.
  float calculateLimbTwist(MR::InstanceDebugInterface* pDebugDrawInst) const;

  // Set the max IK iterations used by default (usually 1).
  void setIncrementalIKIterations(const uint32_t iterationCount) { m_incrementalIKIterations = iterationCount; }

  // Set the number of IK iterations to use when incremental IK is disabled.
  void setNonIncrementalIKIterations(const uint32_t iterations) { m_nonIncrementalIKIterations = iterations; }

  //--------------------------------------------------------------------------------------------------------------------
  // Default Pose

private:
//----------------------------------------------------------------------------------------------------------------------
  /// Writes procedurally generated guide pose into the IK.
  void calculateGuidePose(MR::InstanceDebugInterface* pDebugDrawInst);

  /// Blends poses according to swivel amount.
  /// Specified result is fully neg for -1, fully pos for +1 or somewhere in between.
  void blendPoses(float swivelAmount, const NMP::Quat* neg, const NMP::Quat* zero, const NMP::Quat* pos, NMP::Quat* result) const;

  // Advanced IK helper
  void calculateIKSubstepTarget(
    NMP::Vector3& targetPos, NMP::Quat& targetQuat, NMP::Vector3& targetNormal,
    const NMP::Vector3& localNormal, const NMP::Quat& rootQuat, const NMP::Vector3& rootPos,
    const NMP::Quat* currentJointQuats, float subStep, MR::InstanceDebugInterface* pDebugDrawInst);

  // aux for COM state data update
  void updateCentreOfMassState();
  // update the isInContact flag.
  void updateContactState();

  /// Auxiliary function to simplify setting of bit in NMP::BitArray
  void setBitFlag(NMP::BitArray* bitArray, uint32_t bit, bool val)
  {
    if (val) bitArray->setBit(bit);
    else bitArray->clearBit(bit);
  };

#if defined(MR_OUTPUT_DEBUGGING)
  void debugDraw(MR::InstanceDebugInterface* pDebugDrawInst);
#endif // defined(MR_OUTPUT_DEBUGGING)

  //--------------------------------------------------------------------------------------------------------------------

  /// World space transform of the root locator
  NMP::Matrix34 m_rootTransform;
  /// World space transform of the end locator
  NMP::Matrix34 m_endTransform;

  // limb's dynamic state:
  NMP::Vector3 m_COMPosition, m_COMVelocity, m_COMAngularVelocity;

#if defined(MR_OUTPUT_DEBUGGING)
  LimbControlAmounts m_controlAmounts;
  float m_stiffnessFraction;
#endif // defined(MR_OUTPUT_DEBUGGING)

  // grab constraint
  EndConstraint m_endConstraint;
  friend class EndConstraint;

  /// Flag to say if the limb owns it's root (eg. the spine) or not (eg. it's a leg, head, arm, etc)
  bool m_isRootLimb;

  /// Flag indicating that the end part friction scale should be reset in post physics update
  bool m_shouldResetEndFrictionScale;

  /// The mass of the limb (excluding root part if !m_isRootLimb) - calculated at creation time
  float m_mass;

  //--------------------------------------------------------------------------------------------------------------------

  LimbDef* m_definition;
  Body* m_body;
  MR::PhysicsRig* m_physicsRig;
  LimbIK m_limbIK;

  // Joint angles from physics and for physics used in substepping or for initialising the IK when
  // behaviour control is resumed.
  uint32_t   m_numJointsInChain;
  NMP::Quat* m_physicsJointQuats; ///< Used for IK sub stepping and initialising from physics
  NMP::Quat* m_targetPhysicsJointQuats; ///< The final target angles that get passed to physics
  NMP::Quat* m_IKJointQuats; ///< The joints coming out of IK (which may be modified due to IK substep)

  // procedural guide pose swivel
  NMP::Quat* m_guidePose;    ///< The swivel-amount blend between zero and appropriate extreme
  float m_lastSwivelAmount;  ///< Cache the last swivel amount so we can avoid calculating the guide pose when possible

  // Temporary class data: procedural guide pose setup vars
  // (here while procedural pose gen is still implemented this way within erLimb, but soon to be removed
  // after pose gen is implemented as part of the asset pipeline)
  //
  /// m_swivelSense defines which way is positive or negative to cope with mirrored joints
  float m_swivelSense;
  /// Swivel "limits" in [0, 1] specifying extent of maximum swivel available in each dir/sense
  float m_positiveSwivelScale;
  float m_negativeSwivelScale;

  bool m_isRootExternallySupported; ///< root part under external keyframe control (SK or HK, but not AA etc)
  bool m_isEndExternallySupported;  ///< end part under external keyframe control (SK or HK, but not AA etc)
  bool m_isOutputEnabledOnAnyJoint; ///< True if at least one joint in this limb is under Euphoria
  bool m_isInContact;               ///< True if at least one physics part is in contact with a physics object.

#if defined(MR_OUTPUT_DEBUGGING)
  NMP::BitArray* m_debugDrawFlags;
  NMP::BitArray* m_featureFlags;
#endif
  //--------------------------------------------------------------------------------------------------------------------

  // effector control object, corresponding class members and "override flags"
  EffectorControlParams m_ECP;
  EffectorControlParams m_ECPOverride;

  // value of m_strength in previous frame signals when behaviour control is about to be resumed
  float m_strengthPrev;

  // some per joint force and damping properties
  float* m_forceMultiplier;

  float m_externalCompliance;

  float m_endFrictionScale;

  int m_collisionGroupIndexToActivate;

  uint32_t m_incrementalIKIterations;         // The number of IK iterations used when incremental IK is enabled.
  uint32_t m_nonIncrementalIKIterations;  // The number of IK iterations used when incremental IK is disabled.
  
  ER::LimbIK::SolverErrorData m_IKSolveData;

  float m_leafLimitScale; // foot/hand limit scale
};

//----------------------------------------------------------------------------------------------------------------------
// Simple inline functions
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
inline char* Limb::getName() const
{
  return m_definition->m_name;
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t Limb::getNumPartsInChain() const
{
  return m_definition->m_numPartsInChain;
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t Limb::getTotalNumParts() const
{
  return m_definition->m_totalNumParts;
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t Limb::getEndIndex() const
{
  return m_definition->m_endIndex;
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t Limb::getRootIndex() const
{
  return m_definition->m_rootIndex;
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t Limb::getBaseIndex() const
{
  return m_definition->m_baseIndex;
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t Limb::getMidIndex() const
{
  return m_definition->m_midIndex > 0 ? m_definition->m_midIndex : ER::PART_INDEX_UNSPECIFIED;
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t Limb::getNumPartsBeforeBase() const
{
  return m_definition->m_numPartsBeforeBase;
}

//----------------------------------------------------------------------------------------------------------------------
inline int Limb::getPhysicsRigPartIndex(int limbPartIndex) const
{
  return m_definition->getPhysicsRigPartIndex(limbPartIndex);
}

//----------------------------------------------------------------------------------------------------------------------
inline uint32_t Limb::getNumJointsInChain() const
{
  return m_definition->m_numJointsInChain;
}

//----------------------------------------------------------------------------------------------------------------------
inline int Limb::getPhysicsRigJointIndex(int limbJointIndex) const
{
  return m_definition->getPhysicsRigJointIndex(limbJointIndex);
}

//----------------------------------------------------------------------------------------------------------------------
inline MR::PhysicsRig::Part* Limb::getPart(int limbPartIndex)
{
  return m_physicsRig->getPart(getPhysicsRigPartIndex(limbPartIndex));
}

//----------------------------------------------------------------------------------------------------------------------
inline const MR::PhysicsRig::Part* Limb::getPart(int limbPartIndex) const
{
  return m_physicsRig->getPart(getPhysicsRigPartIndex(limbPartIndex));
}

//----------------------------------------------------------------------------------------------------------------------
inline MR::PhysicsRig::Joint* Limb::getJoint(int limbJointIndex)
{
  return m_physicsRig->getJoint(getPhysicsRigJointIndex(limbJointIndex));
}

}
#endif // NM_LIMB_H
