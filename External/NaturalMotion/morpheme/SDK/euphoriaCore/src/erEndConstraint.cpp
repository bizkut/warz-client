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
#include "euphoria/erEndConstraint.h"

#include "euphoria/erLimb.h"
#include "euphoria/erBody.h"
#include "euphoria/erBodyDef.h"
#include "euphoria/erEuphoriaUserData.h"
#include "euphoria/erEuphoriaLogger.h"
#include "euphoria/erLimbTransforms.h"
#include "mrPhysX3Includes.h"
#include "mrPhysicsScenePhysX3.h"

#define SCALING_SOURCE Body::getFromPhysicsRig(m_limb->m_physicsRig)->getDimensionalScaling()
#include "euphoria/erDimensionalScalingHelpers.h"
//----------------------------------------------------------------------------------------------------------------------
namespace ER
{

#define HOLD_INERTIA_MULTIPLIER 10.0f

#define NM_END_CONSTRAINT_DEBUG_DRAW_CONSTRAINT 1
#define NM_END_CONSTRAINT_DEBUG_DRAW_FORCES 0

//----------------------------------------------------------------------------------------------------------------------
static NMP::Matrix34 getActorTransform(physx::PxRigidActor* actor)
{
  NMP_ASSERT(actor);
  return MR::nmPxTransformToNmMatrix34(actor->getGlobalPose());
}

//----------------------------------------------------------------------------------------------------------------------
static physx::PxRigidActor* getActorFromShapeId(int64_t shapeID)
{
  physx::PxShape* shape = (physx::PxShape*) (size_t) shapeID;
  if (shapeID > 0 && MR::PhysXPerShapeData::getFromShape(shape))
    return &shape->getActor();

  return NULL;
};

//----------------------------------------------------------------------------------------------------------------------
static bool actorIsDynamic(physx::PxRigidActor* actor)
{
  return actor && actor->isRigidDynamic();
};

// Transform world space TM to local.
//----------------------------------------------------------------------------------------------------------------------
static ER::HandFootTransform getLocalTM(const NMP::Matrix34& wsTM, physx::PxRigidActor* actor)
{
  if (actor)
  {
    NMP::Matrix34 actorInvTM = getActorTransform(actor);
    actorInvTM.invert();
    return wsTM * actorInvTM;
  }
  else
  {
    return wsTM;
  }
}


//----------------------------------------------------------------------------------------------------------------------
void EndConstraint::initialize(ER::Limb* limb)
{
  m_limb = limb;
  m_up = -NMP::vNormalise(m_limb->m_body->m_physicsScene->getGravity());
  m_constraint = NULL;
  m_constrainedActor = NULL;
  m_targetShapeID = 0;
  m_doConstrain = false;
  m_constrainOnContact = false;
  m_desiredHandTM = NMP::Matrix34Identity();
  m_disableCollisions = true;
  m_useCheatForces = false;
  m_createDistance = SCALE_DIST(0.1f);
  m_destroyDistance = SCALE_DIST(0.2f);
  m_startOrientationAngle = NMP::degreesToRadians(120.0f);
  m_stopOrientationAngle = NMP::degreesToRadians(150.0f);
  m_posConstrainedDuration = 0.0f;
  m_rotConstrainedDuration = 0.0f;
  m_collisionsWereModified = false;
  m_originalCollisionFlag = true;
  m_originalCollisionPairFlag = 0;

  // Can possibly exposed to the outside.
  m_moveToJointPosFraction = 0.5f;

  m_lockedLinearDofs = 1 | 1<<1 | 1<<2; // All dofs locked.
  m_lockedAngularDofs = 0;              // No dofs locked.
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsRig::Part* EndConstraint::getRootPart()
{
  return m_limb->getPart(0);
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsRig::Part* EndConstraint::getEndPart()
{
  return m_limb->getPart(m_limb->getNumPartsInChain() - 1);
}

//----------------------------------------------------------------------------------------------------------------------
void EndConstraint::setDesiredTransformWs(const ER::HandFootTransform& desiredTM, int64_t shapeID)
{
  m_targetShapeID = shapeID;

  // Check if desired TM is local to an explicitly specified actor. If the actor is a dynamic one,
  // make TM local, so that constraint is on the body rather than in world space
  physx::PxRigidActor* objectActor = getActorFromShapeId(m_targetShapeID);
  if (actorIsDynamic(objectActor))
  {
    // We don't allow changing position if already constrained. Hence overwrite with previous
    // position. TODO Understand what is going on here - however, without it, the constraint can't
    // keep hold of dynamic objects, because the desired hand position stays constant whilst the
    // constraint moves. This seems to be because the incoming position is always in world space, yet
    // sometimes it comes from a cached desired transform (e.g. when holding onto an edge that is no
    // longer suitable for grabbing). This means we can't distinguish between real changes in the
    // position or orientation.
    NMP::Vector3 prevPos(NMP::Vector3::InitZero);
    if (m_constraint)
    {
      prevPos = m_desiredHandTM.translation();
    }

    m_desiredHandTM = getLocalTM(desiredTM, objectActor);

    if (m_constraint)
    {
      m_desiredHandTM.translation() = prevPos;
    }
  }
  else
  {
    m_desiredHandTM = desiredTM;
  }
}

// Disabling end part collisions and resetting to original state.
//----------------------------------------------------------------------------------------------------------------------
void EndConstraint::disableCollisions()
{
  // Early out if we already disabled collisions.
  // MORPH-21380: doesn't work as desired.  The character must be coming out of this function
  // without collisions being disabled, And with the early out he won't try disabling again.
  if (m_collisionsWereModified)
  {
    return;
  }

  // All collisions with end part.
  MR::PhysicsRig::Part* endPart = getEndPart();
  m_originalCollisionFlag = endPart->getCollisionEnabled();
  endPart->enableCollision(false);
#ifdef USING_EUPHORIA_LOGGING
  EUPHORIA_LOG_MESSAGE("GrabConstraint: disabling collisions now (%s).\n", m_limb->m_definition->m_name);
#endif

  m_collisionsWereModified = true;
}

// Reset collision flag to original state.
//----------------------------------------------------------------------------------------------------------------------
void EndConstraint::resetCollisions()
{
  // Don't change when not necessary.
  if (m_collisionsWereModified)
  {
    getEndPart()->enableCollision(m_originalCollisionFlag);
    m_collisionsWereModified = false;
#ifdef USING_EUPHORIA_LOGGING
    EUPHORIA_LOG_MESSAGE("GrabConstraint: restoring collisions now (%s).\n", m_limb->m_definition->m_name);
#endif
  }
}

// Wrapper for hiding dynamic/static objects and PhysX versions from update function.
// PhysX is more stable when static Actor is the first Actor passed to joint creation function.
//----------------------------------------------------------------------------------------------------------------------
void EndConstraint::createConstraint(
  physx::PxRigidActor* endPartActor,
  const NMP::Matrix34& constraintInEndPartFrame,
  physx::PxRigidActor* objectActor,
  const NMP::Matrix34& constraintInObjectFrame)
{
  // For static objects, constrain with world.
  physx::PxRigidActor* constraineeActor = actorIsDynamic(objectActor) ? objectActor : NULL;
  m_constraint = createJoint(constraineeActor, endPartActor, constraintInObjectFrame, constraintInEndPartFrame,
    m_lockedLinearDofs, m_lockedAngularDofs);

  // Store pointer to constrainee.
  m_constrainedActor = objectActor;
}

//----------------------------------------------------------------------------------------------------------------------
void EndConstraint::removeConstraint()
{
  NMP_ASSERT(m_constraint != NULL);

  m_constraint->release();
  m_constraint = NULL;
  m_posConstrainedDuration = 0.0f;
  m_rotConstrainedDuration = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
physx::PxRigidActor* EndConstraint::getContactedActor()
{
  MR::PhysicsRig::Part* endPart = getEndPart();
  ER::EuphoriaRigPartUserData* endPartUserData = ER::EuphoriaRigPartUserData::getFromPart(endPart);
  bool inContact = endPartUserData->getNumContacts() != 0;

  if (inContact)
  {
    // Just return the first contact
    return &endPartUserData->getContactedShape(0).getActor();
  }
  return NULL;
}

// Returns the shortest arc distance in radians between the current end effector TM and the desired TM This is achieved
// by representing the end effector TM in the local space of the desired. The resulting TM, in effect a rotational
// offset, is then converted into a rotationVector whose magnitude gives the desired rotation error.
//----------------------------------------------------------------------------------------------------------------------
void EndConstraint::getConstraintDistance(
  float& positionErr,
  float& rotationErr,
  bool   objectActorIsDynamic,
  float  timeDelta,
  NMP::Vector3& closestPointOffset)
{
  // Get end effector in desired TM local space.
  NMP::Matrix34 desInv = m_desiredHandTM;
  NMP::Vector3 positionDelta = m_limb->getPart(m_limb->getDefinition()->m_numPartsInChain - 1)->getVel() * timeDelta;
  // If dynamic object, convert to world space first. Note that if this flag is true there really
  // will be an actor. There will already have been a check to see if it's a deleted shape, so can
  // safely assume that if dynamic, we can get the actor.
  if (objectActorIsDynamic)
  {
    physx::PxRigidActor *actor = getActorFromShapeId(m_targetShapeID);
    NMP_ASSERT(actor);
    desInv *= getActorTransform(actor);
    positionDelta -= MR::nmPxVec3ToVector3(MR::getPointVelocity(*actor, MR::nmVector3ToPxVec3(desInv.translation()))) * timeDelta;
  }
  const NMP::Vector3 toEnd = m_limb->getEndTransform().translation() - desInv.translation();

  float t = -positionDelta.dot(toEnd) / (1e-10f + positionDelta.magnitudeSquared());
  if (t > 1.0f) // The thinking here is that if t = 1 then we don't have to constrain yet, the next frame it will get closer, so constrain then.
  {
    t = 0.0f;
  }
  closestPointOffset = positionDelta * NMP::maximum(t, 0.0f);
  const NMP::Vector3 closestPointRelativeToConstraint = toEnd + closestPointOffset;

  // Positional error finds distance to closest point of the path covered by the hand relative to the constraint in the timeDelta.
  positionErr = closestPointRelativeToConstraint.magnitude();
  desInv.invert();
  NMP::Matrix34 endInDesTM = m_limb->getEndTransform() * desInv;

  // Rotational error is magnitude of rotation vector.
  NMP::Vector3 rotVec = endInDesTM.toRotationVector();
  rotationErr = rotVec.magnitude();
}

//----------------------------------------------------------------------------------------------------------------------
// Applies a force that helps the limb end part reach the desired position and orientation.
// Applies a torque that helps orienting limb root part towards the edge.
void EndConstraint::applyHelperForce(
  bool isInActorSpace,
  float angErr,
  MR::InstanceDebugInterface* pDebugDrawInst)
{
  if (m_constraint && m_useCheatForces)
  {
    MR::PhysicsRig::Part* limbRootPart = getRootPart();
    MR::PhysicsRig::Part* limbEndPart = getEndPart();

    {
      // The following cheat forces are applied to the limb root, calculated so that they will help to
      // orientate the end parts with the desired end orientation.
      const ER::LimbTransform limbRootTM = m_limb->getRootTransform();
      const NMP::Vector3 limbRootAngVel = getRootPart()->getAngVel();

      // AddTorque function is used in it's acceleration based (not force based) configuration
      // so function parameter has to have a unit of [angle/time^2], i.e. an angular acceleration
      // not [distance * [mass * distance / time^2]].
      const float strength = SCALE_STRENGTH(20.0f); // [1/time^2]
      const float damping = SCALE_DAMPING(1.0f);  // [1/time]

      NMP::Vector3 desLimbRootFwd = -m_desiredHandTM.normalDirection();
      desLimbRootFwd -= ((desLimbRootFwd.dot(m_up)) * m_up);
      NMP::Quat q; q.forRotation(limbRootTM.frontDirection(), desLimbRootFwd);
      // Only around vertical axis.
      NMP::Vector3 yawSpringAccel = (q.toRotationVector().dot(limbRootTM.upDirection())) * limbRootTM.upDirection();
      yawSpringAccel *= strength; // [angle/time^2] = [angle] * [1/time^2]
      // Velocity around vertical axis.
      NMP::Vector3 yawDampingAccel = -(limbRootAngVel.dot(limbRootTM.upDirection())) * limbRootTM.upDirection(); // Angular velocity [angle/time].
      yawDampingAccel *= damping; // [angle/time^2] = [angle/time] * [1/time]

      // Apply acceleration based torques [angle/time^2].
      limbRootPart->addAngularAcceleration(yawSpringAccel + yawDampingAccel);

      NMP::Vector3 limbRootSidewaysDirection = -limbRootTM.rightDirection();
      NMP::Vector3 desiredLimbRootSidewaysDirection = limbRootSidewaysDirection;
      desiredLimbRootSidewaysDirection -= ((desiredLimbRootSidewaysDirection.dot(m_up)) * m_up);
      q.forRotation(limbRootSidewaysDirection, desiredLimbRootSidewaysDirection);
      // Only around forward axis.
      NMP::Vector3 rollSpringAccel = (q.toRotationVector().dot(limbRootTM.frontDirection())) * limbRootTM.frontDirection();
      rollSpringAccel *= strength;
      // Velocity around forward axis.
      NMP::Vector3 rollDampingAccel = -(limbRootAngVel.dot(limbRootTM.frontDirection())) * limbRootTM.frontDirection();
      rollDampingAccel *= damping;

      // Apply acceleration based torques [angle/time^2].
      limbRootPart->addAngularAcceleration(rollSpringAccel + rollDampingAccel);

#if NM_END_CONSTRAINT_DEBUG_DRAW_FORCES
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngularAcceleration, limbRootTM.translation(), yawSpringAccel, NMP::Colour::RED);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngularAcceleration, limbRootTM.translation(), yawDampingAccel, NMP::Colour::GREEN);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngularAcceleration, limbRootTM.translation(), rollSpringAccel, NMP::Colour::RED);
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, MR::VT_AngularAcceleration, limbRootTM.translation(), rollDampingAccel, NMP::Colour::GREEN);
#else
      (void) pDebugDrawInst;
#endif
    }

    // Only use cheat forces on the limb end part if the angular error is above a threshold. The
    // angular constraint is only enabled (elsewhere) if the angle error is less than some other
    // threshold. It's easier here to just always drive with an explicit spring.
    const float handAngErrThreshold = NMP::degreesToRadians(10.0f);
    if (angErr > handAngErrThreshold)
    {
      // Use an angular acceleration based spring/damper
      const float P = SCALE_STRENGTH(4.1f);
      const float D = SCALE_DAMPING(0.02f);
      ER::HandFootTransform desInv = m_desiredHandTM;
      if (isInActorSpace)
      {
        // If dynamic object, convert to world space first.
        desInv *= getActorTransform(getActorFromShapeId(m_targetShapeID));
      }
      desInv.invert();
      NMP::Matrix34 endInDesTM = desInv * m_limb->getEndTransform();
      NMP::Vector3 rotVec = -endInDesTM.toRotationVector(); 
      NMP::Vector3 helperAngAccel = (P * rotVec) - (D * getEndPart()->getAngVel()); 

      // Apply acceleration based torque 
      limbEndPart->addAngularAcceleration(helperAngAccel);
#if NM_END_CONSTRAINT_DEBUG_DRAW_FORCES
      MR_DEBUG_DRAW_VECTOR(pDebugDrawInst, 
        MR::VT_AngularAcceleration, m_limb->getEndTransform().translation(), helperAngAccel, NMP::Colour::WHITE);
#endif
    }
  } // End for test if the constraint has been enabled and we should use cheat forces
}

//----------------------------------------------------------------------------------------------------------------------
bool EndConstraint::checkConstrainability(float posErr)
{
  // Check whether in sufficient proximity.
  const float maxLinErr = isConstrained() ? m_destroyDistance : m_createDistance;
  if (posErr > maxLinErr)
  {
#ifdef USING_EUPHORIA_LOGGING
    if (isConstrained())
    {
      EUPHORIA_LOG_MESSAGE("GrabConstraint: breaking constraint. Hand position too far from target (%s).\n", 
        m_limb->m_definition->m_name)
    }
#endif //USING_EUPHORIA_LOGGING
    return false;
  }

  // Check orientation is approx. right (don't constrain palm up e.g.).
  const ER::HandFootTransform &limbEndTM = m_limb->getEndTransform();
  const NMP::Vector3 currentNormal = limbEndTM.normalDirection();
  const NMP::Vector3 desiredNormal = m_desiredHandTM.normalDirection();
  const float currentDotDesired = NMP::clampValue(currentNormal.dot(desiredNormal), -1.0f, 1.0f);
  const float angError = NMP::fastArccos(currentDotDesired);
  if (angError > m_stopOrientationAngle)
  {
#ifdef USING_EUPHORIA_LOGGING
    if (isConstrained() && !(angError < m_stopOrientationAngle))
    {
      EUPHORIA_LOG_MESSAGE("GrabConstraint: breaking constraint. Hand orientation too far from target (%s).\n", 
        m_limb->m_definition->m_name)
    }
#endif //USING_EUPHORIA_LOGGING
    return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void EndConstraint::blendToDesiredOrientation(float dt, NMP::Matrix34& blendTM)
{
  // Update timer for time-based blending of target constraint orientation.
  m_rotConstrainedDuration += dt;

  // Blend based on time.
  const float blendPeriod = SCALE_TIME(0.5f); // Seconds.
  float blendWeight = m_rotConstrainedDuration / blendPeriod;
  blendWeight = NMP::minimum(blendWeight, 1.0f);

  // 2nd: do the actual blend. At this point both m_desiredHandTM and objectJointFrame are either both in world space (for
  // static objects or world space constraints) or both in the local space of the other actor.
  // We don't need to check whether blendWeight == 1 for efficiency, interpolate does that inernally.
  blendTM.interpolate(m_desiredHandTM, blendWeight);
}

//----------------------------------------------------------------------------------------------------------------------
void EndConstraint::blendToDesiredPosition(float dt, NMP::Matrix34& blendTM)
{
  m_posConstrainedDuration += dt;
  const float blendPeriod = SCALE_TIME(0.5f); // Seconds
  float blendWeight = m_posConstrainedDuration / blendPeriod;
  blendWeight = NMP::minimum(blendWeight, 1.0f);
  // Could check whether blendWeight == 1, but probably as expensive as the following operation itself.
  blendTM.r[3] += blendWeight * (m_desiredHandTM.r[3] - blendTM.r[3]);
}

//----------------------------------------------------------------------------------------------------------------------
void EndConstraint::update(float dt, MR::InstanceDebugInterface* pDebugDrawInst)
{
  NMP_ASSERT(m_limb != NULL);

  // Check whether we're allowed to create constraint.
  // MORPH-21381: Overwrite used in debug, code needs to be removed.
  bool doConstrain = m_doConstrain || m_constrainOnContact;
  bool constrained = isConstrained();

  // Early out if nothing to do here.
  if (!doConstrain)
  {
    if (constrained)
    {
      removeConstraint();
      // Re-enable collisions if not colliding. 
      resetCollisions();
    }
    return;
  }

  // From here on we know we're allowed to constrain.
  // -----------------------------------------------

  // Turn off limb end part collisions when trying to grab. But not when grabbing on contact, as contacts are not
  // reported when collisions are off.
  if (m_disableCollisions && !m_constrainOnContact)
  {
    disableCollisions();
  }

  // Decide who to try constraining to:
  // targetShape id >=  0: specific dynamic or static actor
  // targetShape id == -1: constrain with "virtual" edge, i.e. world. 
  // First see if specified target (actor) is constrainable (in reach).
  // NULL if m_targetShape < 0.
  physx::PxRigidActor* objectActor = getActorFromShapeId(m_targetShapeID);

  // The following will be false if objectActor == NULL or static object.
  bool objectActorIsDynamic = actorIsDynamic(objectActor);

  // Check for a deleted shape
  if (objectActorIsDynamic && !getActorFromShapeId(m_targetShapeID))
  {
    if (constrained)
    {
      removeConstraint();
      resetCollisions();
    }
    return;
  }

  // Check whether chosen actor is within range.
  float posErr = FLT_MAX;
  float rotErr = FLT_MAX; // in radians
  NMP::Vector3 closestPointOffset(0,0,0);
  getConstraintDistance(posErr, rotErr, objectActorIsDynamic, dt, closestPointOffset);
  bool constrainable = checkConstrainability(posErr);

  // If we can't constrain to the target, check if we can constrain to any contacted object.
  // Needs to be world constraint, as contacts currently don't return static actor IDs.
  if (!constrainable && m_constrainOnContact)
  {
    ER::EuphoriaRigPartUserData* data = ER::EuphoriaRigPartUserData::getFromPart(getEndPart());
    constrainable = data->getNumContacts() > 0;
    if (constrainable)
    {
      // Artificial errors that lead to no blending of position or orientation.
      posErr = 0.0f;
      rotErr = 3.0f;
      setDesiredTransformWs(m_limb->getEndTransform(), -1);
    }
  }

  // Cheat forces to help limb end reach targets and limb root to orient appropriately.
  applyHelperForce(objectActorIsDynamic, rotErr, pDebugDrawInst);

  // Break joint when separation reaches a threshold.
  if (constrained && (!constrainable))
  {
    removeConstraint();
    resetCollisions();
    return;
  }

  // Create or update already existing constraint.
  //-------------------------------------------------------------------------------------------------------
  if (constrained || constrainable)
  {
    // Build up necessary matrices for constraint: parent frame first. The constraint needs the parent joint frame in
    // the end part's local space. We want to control the end transform, therefore we calculate the end transform in the
    // endPart's local space.
    physx::PxRigidActor* endPartActor = ((MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation*)getEndPart())->getArticulationLink();

    NMP::Matrix34 constraintTM = m_limb->getEndTransform();
    if (m_moveToJointPosFraction > 0.0f)
    {
      // For stability reasons, move the attachment point towards the joint position by a certain fraction.
      // see MORPH-14060
      NMP::Vector3 jointPos = getActorTransform(endPartActor).translation(); // By convention same as joint position.
      constraintTM.translation() += m_moveToJointPosFraction * (jointPos - constraintTM.translation());
    }

    // To local space.
    NMP::Matrix34 endPartJointFrameInv = getActorTransform(endPartActor);
    endPartJointFrameInv.invert();
    NMP::Matrix34 constraintInEndPartFrame = constraintTM * endPartJointFrameInv;

    // Now the object frame (desired): we always start from current and may later blend to desired.
    // Move constraintTM to the closestPoint first.
    constraintTM.translation() += closestPointOffset;
    NMP::Matrix34 constraintInObjectFrame = objectActorIsDynamic ? getLocalTM(constraintTM, objectActor) : constraintTM;

    // Remove existing constraint first when switching from one grabbed object to another.
    if (constrained && (objectActor != m_constrainedActor))
    {
      removeConstraint();
      constrained = false;
    }

    // Either create a new constraint...
    //---------------------------------------------------
    if (!constrained)
    {
      // No blending needed at first frame of constraint. We start constraining at initial pose.
      constraintInEndPartFrame.orthonormalise();
      constraintInObjectFrame.orthonormalise();
      createConstraint(endPartActor, constraintInEndPartFrame, objectActor, constraintInObjectFrame);
    }
    // ... or update an already existing constraint.
    //---------------------------------------------------
    else
    {
      // We start and stop the orientation constraint at different levels of angular error (hysteresis).
      float angThreshold = m_rotConstrainedDuration > 0.0f ? m_stopOrientationAngle : m_startOrientationAngle;
      bool constrainOrientation = (rotErr < angThreshold) && (m_lockedAngularDofs > 0);
      if (constrainOrientation)
      {
#ifdef USING_EUPHORIA_LOGGING
        if (m_rotConstrainedDuration == 0.0f) EUPHORIA_LOG_MESSAGE("GrabConstraint: started constraining orientation (%s).\n", m_limb->m_definition->m_name);
#endif
        // Set rotational dofs from bitmask (will have been unconstrained initially).
        blendToDesiredOrientation(dt, constraintInObjectFrame);
        lockJointAngularDofs(m_lockedAngularDofs);
      }
      else
      {
        // Free up rotation if orientation error is too large.
#ifdef USING_EUPHORIA_LOGGING
        if (m_rotConstrainedDuration > 0.0f) 
        {
          EUPHORIA_LOG_MESSAGE("GrabConstraint: stopped constraining orientation (%s).\n", m_limb->m_definition->m_name);
        }
#endif
        lockJointAngularDofs(0);
        m_rotConstrainedDuration = 0.0f; // Reset timer for smooth orientation blend.
      }

      // Blend position always, right from the beginning.
      blendToDesiredPosition(dt, constraintInObjectFrame);

      // Update desired transform (i.e. the object we're constraining to).
      constraintInObjectFrame.orthonormalise();
      m_constraint->setLocalPose(physx::PxJointActorIndex::eACTOR0, MR::nmMatrix34ToPxTransform(constraintInObjectFrame));
    }

    // Reduce jitter coming from joint limits by increasing inertia tensor of limb end part.
    // MORPH-12878: Identify what it is about the inertia distribution that causes instability and fix up the
    // inertia on all parts that are likely to exhibit instability. Note that this does
    // little more than just setting the data on the PhysX side, so should be pretty fast.
    MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation* endPart = (MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation*)getEndPart();
    endPart->setMassSpaceInertia(endPart->getOriginalMassSpaceInertia() * HOLD_INERTIA_MULTIPLIER);
    // If the limb has more than 1 part then also increase the inertia on the 2nd last part in the limb.
    if (m_limb->getNumPartsInChain() >= 2)
    {
      MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation* secondLastPart = 
        (MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation*)(m_limb->getPart(m_limb->getNumPartsInChain() - 2));
      secondLastPart->setMassSpaceInertia(secondLastPart->getOriginalMassSpaceInertia() * HOLD_INERTIA_MULTIPLIER);
    }

#if NM_END_CONSTRAINT_DEBUG_DRAW_CONSTRAINT && defined(MR_OUTPUT_DEBUGGING)
    if (pDebugDrawInst)
    {
      debugDraw(
        constraintTM, 
        constraintInObjectFrame, 
        objectActorIsDynamic ? objectActor : NULL, 
        pDebugDrawInst, 
        Body::getFromPhysicsRig(m_limb->m_physicsRig)->getDimensionalScaling());
    }
#endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
void EndConstraint::debugDraw(
  const NMP::Matrix34& constraintTM, 
  const NMP::Matrix34& objectJointFrame, 
  physx::PxRigidActor* objectActor, 
  MR::InstanceDebugInterface* pDebugDrawInst,
  const DimensionalScaling& MR_OUTPUT_DEBUG_ARG(dimensionalScaling)
  )
{
  // Early out if the instance interface is null
  if (pDebugDrawInst == 0)
  { 
    return;
  }
  MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, constraintTM, dimensionalScaling.scaleDist(0.1f));
  if (objectActor)
  {
    // Transform desired back to world for drawing.
    NMP::Matrix34 otherTM = getActorTransform(objectActor);
    NMP::Matrix34 currentDesiredWs = objectJointFrame * otherTM;
    NMP::Matrix34 desiredWs = m_desiredHandTM * otherTM;
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, currentDesiredWs, dimensionalScaling.scaleDist(0.5f));
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, desiredWs, dimensionalScaling.scaleDist(0.25f));
  }
  else
  {
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, m_desiredHandTM, dimensionalScaling.scaleDist(0.5f));
    MR_DEBUG_DRAW_MATRIX(pDebugDrawInst, objectJointFrame, dimensionalScaling.scaleDist(0.25f));
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
physx::PxD6Joint* EndConstraint::createJoint(
  physx::PxRigidActor* actor1,
  physx::PxRigidActor* actor2,
  const NMP::Matrix34& pose1,
  const NMP::Matrix34& pose2,
  uint16_t lockedLinearDofs,
  uint16_t lockedAngularDofs)
{
  // PhysX is more stable when static Actor is the first Actor passed to joint creation function.
  physx::PxD6Joint* joint = (physx::PxD6Joint*)PxD6JointCreate(
    PxGetPhysics(),
    actor1,
    MR::nmMatrix34ToPxTransform(pose1),
    actor2,
    MR::nmMatrix34ToPxTransform(pose2));

  for (int i = 0; i < 3; i++)
  {
    // Translational and rotational dofs.
    joint->setMotion((physx::PxD6Axis::Enum) (physx::PxD6Axis::eX + i), lockedLinearDofs & (1 << i) ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
    joint->setMotion((physx::PxD6Axis::Enum) (physx::PxD6Axis::eTWIST + i), lockedAngularDofs & (1 << i) ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
  }

  return joint;
}

// Set rotational dofs from bitmask (will have been unconstrained initially).
//----------------------------------------------------------------------------------------------------------------------
void EndConstraint::lockJointAngularDofs(uint16_t angDofs)
{
  m_constraint->setMotion(physx::PxD6Axis::eTWIST,  angDofs & (1 << 0) ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
  m_constraint->setMotion(physx::PxD6Axis::eSWING1,  angDofs & (1 << 1) ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
  m_constraint->setMotion(physx::PxD6Axis::eSWING2,  angDofs & (1 << 2) ? physx::PxD6Motion::eLOCKED : physx::PxD6Motion::eFREE);
}

} // namespace ER

