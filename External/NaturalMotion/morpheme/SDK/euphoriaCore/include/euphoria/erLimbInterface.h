// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_LIMB_INTERFACE_H
#define NM_LIMB_INTERFACE_H

#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"

#include "physics/mrPhysicsRig.h"

#include "erLimbIK.h"
#include "erSharedEnums.h"

namespace MR
{
struct PhysicsSerialisationBuffer;
}

namespace ER
{

const uint32_t PART_INDEX_UNSPECIFIED = 0xFFFFFFFF;

class Body;
class Limb;
class LimbDef;
struct ReachLimit;
class EndConstraint;
struct EuphoriaRigPartUserData;

//----------------------------------------------------------------------------------------------------------------------
/// Limb controller: provides minimal interface needed to control (VM + IK) limbs
//----------------------------------------------------------------------------------------------------------------------
class LimbInterface
{
public:

  virtual ~LimbInterface() {}

  //--------------------------------------------------------------------------------------------------------------------
  /// Bundles data set by behaviours
  //--------------------------------------------------------------------------------------------------------------------
  struct EffectorControlParams
  {
    // target and weightings
    NMP::Vector3 targetPos;
    NMP::Vector3 targetNormal;
    NMP::Vector3 localNormal;
    NMP::Quat targetOrient;
    NMP::Vector3 targetVelocity;
    NMP::Vector3 targetAngularVelocity;

    NMP::Vector3 rootTargetPos;
    NMP::Quat rootTargetOrient;
    NMP::Vector3 rootVelocity;
    NMP::Vector3 rootAngularVelocity;

    float targetPositionWeight;
    float targetNormalWeight;
    float targetOrientationWeight;
    float rootTargetPositionWeight;
    float rootTargetOrientationWeight;

    // control
    float strength;
    float damping;
    float driveCompensation;
    float externalComplianceScale;
    float implicitStiffness;
    float maxDriveVelocityOffset;

    // IK substepping used to bias the path towards straight lines
    float ikSubStep;
    float ikSwivelWeight;
    // Used to limit control to the root end of the limb
    float strengthReductionTowardsEnd;
    bool  useIncrementalIK;

    // Limb::init friendly data zeroing
    void initData();
  };

  /// Specifies parameters that can be accessed through get/setDebugDrawFlag(id)
  enum DebugDrawId
  {
    kDrawEnabled,
    kDrawHamstrings,
    kDrawLimbState,
    kDrawIKSubStepping,
    kDrawLimits,
    kDrawDials,
    kDrawSoftLimits,
    kDrawGuidePoseExtremes,
    kDrawGuidePoseRange,
    kDrawEndConstraint,
    kDrawEndConstraintForces,
    kNumDebugDrawFlags
  };

  /// Specifies parameters that can be accessed through get/setFeatureFlag(id)
  enum FeatureId
  {
    kFeatureDriveCompensation,
    kFeatureAutoGuidePose,
    kFeatureAutoGuidePoseBoneSwivel,
    kFeatureAutoGuidePoseBoneReverseSwivel,
    kFeatureAutoGuidePoseReachSwivel,
    kFeatureHamstrings,
    kNumFeatureFlags
  };

public:
  /// return the name of the limb
  virtual char* getName() const = 0;

  // Access to ik solver, body, rig, parts/joints etc
  //
  /// Returns the limb's ik solver
  virtual LimbIK& getIK() = 0;
  virtual const LimbIK& getIK() const = 0;
  /// Returns a pointer to the body this limb is part of
  virtual Body* getBody() const = 0;
  /// Returns a pointer to the physics rig this limb is part of
  virtual MR::PhysicsRig* getPhysicsRig() const = 0;
  /// Returns a pointer to the definition of this limb
  virtual ER::LimbDef* getDefinition() const = 0;

  /// Returns the number of parts in the limb's IK chain
  virtual uint32_t getNumPartsInChain() const = 0;
  /// Returns the number of parts in the limb, the sum of chain parts and parts beyond the end effector
  virtual uint32_t getTotalNumParts() const = 0;
  /// Returns the number of joints in the IK chain
  virtual uint32_t getNumJointsInChain() const = 0;
  /// index into the physics rig list of parts
  virtual uint32_t getRootIndex() const = 0;
  /// index into the physics rig list of parts
  virtual uint32_t getEndIndex() const = 0;
  /// index into the physics rig list of parts
  virtual uint32_t getBaseIndex() const = 0;
  /// returns an physics rig part index if set in the limb definition, or PART_INDEX_UNSPECIFIED otherwise
  virtual uint32_t getMidIndex() const = 0;
  virtual uint32_t getNumPartsBeforeBase() const = 0;
  /// Returns the physics rig index given the limb index
  virtual int getPhysicsRigPartIndex(int limbPartIndex) const = 0;
  /// Returns the physics rig index given the limb index
  virtual int getPhysicsRigJointIndex(int limbJointIndex) const = 0;
  /// Returns a pointer to the indexed part
  virtual MR::PhysicsRig::Part* getPart(int limbPartIndex) = 0;
  /// Returns a pointer to the indexed joint
  virtual const MR::PhysicsRig::Part* getPart(int limbPartIndex) const = 0;
  /// Returns a pointer to the indexed joint
  virtual MR::PhysicsRig::Joint* getJoint(int physRigJointIndex) = 0;

  /// The spine is the root limb in that it contains the root parts of all other limbs
  virtual bool getIsRootLimb() const = 0;
  virtual ER::LimbTypeEnum::Type getType() const = 0;

  // Accessors to COM state data
  //
  /// Access to limb COM position
  virtual const NMP::Vector3&  getCentreOfMass()        const = 0;
  /// Access to limb COM velocity
  virtual const NMP::Vector3&  getCOMVelocity()         const = 0;
  /// Access to limb COM angular velocity
  virtual const NMP::Vector3&  getCOMAngularVelocity()  const = 0;
  /// Access to total mass of the limb
  virtual float               getMass()                 const = 0;

  /// Copies the guide-pose joint angles from the IK into an array of twist/swing
  /// caller must ensure that angles is large enough
  virtual void getLatestJointAngles(NMP::Vector3* angles) const = 0;
  virtual void setLatestJointAngles(const NMP::Vector3* angles) = 0;
  /// Quat replacement for getLatestJointAngles()
  virtual void getLatestJointQuats(NMP::Quat* quats) const = 0;
  /// Quat replacement for setLatestJointAngles()
  virtual void setLatestJointQuats(const NMP::Quat* quats) = 0;

  /// Returns true if a left arm or leg
  virtual bool isLeftLimb() const = 0;
  /// Returns true if a right arm or leg
  virtual bool isRightLimb() const = 0;

  // Access to flags indexed by constants defined in enum above
  virtual bool getDebugDrawFlag(DebugDrawId id) const = 0;
  virtual void setDebugDrawFlag(DebugDrawId id, bool flag) = 0;
  virtual bool getFeatureFlag(FeatureId id) const = 0;
  virtual void setFeatureFlag(FeatureId id, bool flag) = 0;
#if defined(MR_OUTPUT_DEBUGGING)
  virtual void setDebugControlAmounts(const LimbControlAmounts& limbControlAmounts, float stiffnessFraction) = 0;
  virtual const LimbControlAmounts& getDebugControlAmounts(float& stiffnessFraction) const = 0;
#endif // defined(MR_OUTPUT_DEBUGGING)
};

}
#endif // NM_LIMB_H
