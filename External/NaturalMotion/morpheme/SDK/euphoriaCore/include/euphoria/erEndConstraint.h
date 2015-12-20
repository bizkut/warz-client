// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_ENDCONSTRAINT_H
#define NM_ENDCONSTRAINT_H

#include "erDebugDraw.h"
#include "mrPhysX3.h"
#include "mrPhysicsRigPhysX3Articulation.h"
#include "mrPhysX3Includes.h"
#include "erLimbTransforms.h"
#include "erEuphoriaUserData.h"
#include "mrPhysicsScenePhysX3.h"

namespace ER
{

class Limb;
class DimensionalScaling;

//----------------------------------------------------------------------------------------------------------------------
/// Constrains a hand or foot to another object or the world.
//----------------------------------------------------------------------------------------------------------------------
class EndConstraint
{
  friend class Limb;

public:

  enum GrabBlendMode
  {
    GC_BLEND_TIMED,       // Always blend over a fixed amount of time.
    GC_BLEND_PROPORTION   // Always blend halfway towards target.
  };

  void update(float dt, MR::InstanceDebugInterface* pDebugDrawInst);
  void disable();

  // Configuring the constraint.
  // shape id >=  0: specific dynamic or static actor
  // shape id <= -1: constrain with "virtual" edge, i.e. world.
  void setDesiredTransformWs(const ER::HandFootTransform& desTM, int64_t shapeID);
  void setSeparationThresholds(float posMin, float posMax, float angMin, float angMax);
  void setLockedDofs(uint16_t linearDofs, uint16_t angularDofs) { m_lockedLinearDofs = linearDofs; m_lockedAngularDofs = angularDofs; };
  void disableCollisions(bool collide) { m_disableCollisions = collide; };
  void doConstrain(bool constrain) { m_doConstrain = constrain; };
  void constrainOnContact(bool constrain) { m_constrainOnContact = constrain; };
  void useCheatForces(bool doUse) { m_useCheatForces = doUse; };

  // Getters.
  bool isConstrained() const { return m_constraint != NULL; };

protected:

  void initialize(ER::Limb* limb);

  // These are for a more coherent interface to different PhysX versions.
  void createConstraint(
    physx::PxRigidActor* endPartActor,
    const NMP::Matrix34& constraintInEndPartFrame,
    physx::PxRigidActor* objectActor,
    const NMP::Matrix34& constraintInObjectFrame);

  physx::PxD6Joint* createJoint(
    physx::PxRigidActor* actor1,
    physx::PxRigidActor* actor2,
    const NMP::Matrix34& pose1,
    const NMP::Matrix34& pose2,
    uint16_t lockedLinearDofs,
    uint16_t lockedAngularDofs);

  void lockJointAngularDofs(uint16_t lockedAngularDofs);

  void removeConstraint();
  void disableCollisions();
  void resetCollisions();

  physx::PxRigidActor* getContactedActor();
  MR::PhysicsRig::Part* getRootPart();
  MR::PhysicsRig::Part* getEndPart();

  // Return positional and rotational error between end part and desired TM (meters and radians respectively).
  void getConstraintDistance(
    float& positionErr,
    float& rotationErr,
    bool   objectActorIsDynamic,
    float  timeDelta,
    NMP::Vector3& closestPointOffset);

  // Checks whether target actor is in range for making constraint.
  bool checkConstrainability(float positionErr);

  // For blending constraint TM from current pose to target pose.
  void blendToDesiredOrientation(float dt, NMP::Matrix34& blendTM);
  void blendToDesiredPosition(float dt, NMP::Matrix34& blendTM);

  void applyHelperForce(
    bool  desiredInActorSpace,
    float angErr,
    MR::InstanceDebugInterface* MR_OUTPUT_DEBUG_ARG(pDebugDrawInst));

#if defined(MR_OUTPUT_DEBUGGING)
  void debugDraw(
    const NMP::Matrix34& controlledTM,
    const NMP::Matrix34& currentDesired,
    physx::PxRigidActor* otherActor,
    MR::InstanceDebugInterface* pDebugDrawInst,
    const ER::DimensionalScaling& MR_OUTPUT_DEBUG_ARG(dimensionalScaling)
    );
#endif

  // Put Vector3 etc etc for alignment.
  NMP::Vector3 m_up;                        // Opposite to gravity.
  ER::HandFootTransform m_desiredHandTM;    // Joint will enforce this hand orientation (assuming other actor is "fixed").

  int64_t m_targetShapeID;                  // A shape to try connecting to.
  physx::PxD6Joint* m_constraint;           // The actual underlying PhysX constraint.

  bool m_collisionsWereModified;            // For resetting the collisions if they were disabled at joint creation.
  bool m_originalCollisionFlag;
  uint32_t m_originalCollisionPairFlag;     // 0 means there was no explicit pairFlag defined when collisions were disabled.

  physx::PxRigidActor* m_constrainedActor;  // The other actor when joint has been created  (NULL otherwise).
  Limb* m_limb;                             // Stored pointer to limb, wants access to private members, hence declared friend in limb.

  float m_posConstrainedDuration;
  float m_rotConstrainedDuration;
  float m_moveToJointPosFraction;

  // Constraint parameters, passed in through API.
  float m_createDistance;
  float m_destroyDistance;
  float m_startOrientationAngle;
  float m_stopOrientationAngle;
  bool m_constrainOnContact;
  uint16_t m_lockedLinearDofs;   // Bitmask toggling individual dofs (corresponding flags defined in enum PhysXDofFlag (physX30.h).
  uint16_t m_lockedAngularDofs;  // Bitmask toggling individual dofs
  bool m_doConstrain;            // Toggle constraint creation.
  bool m_disableCollisions;      // Collisions between constrained actors.
  bool m_useCheatForces;
}; // EndConstraint

//----------------------------------------------------------------------------------------------------------------------
// Inline implementations
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EndConstraint::setSeparationThresholds(float posMin, float posMax, float angMin, float angMax)
{
  m_createDistance = posMin;
  m_destroyDistance = posMax;
  m_startOrientationAngle = angMin;
  m_stopOrientationAngle = angMax;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void EndConstraint::disable()
{
  if (m_constraint != NULL)
  {
    removeConstraint();
    resetCollisions();
  }
};

} // namespace ER

#endif // ifndef NM_ENDCONSTRAINT_H
