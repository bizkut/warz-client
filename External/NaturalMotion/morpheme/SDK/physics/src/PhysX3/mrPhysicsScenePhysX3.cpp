// Copyright (c) 2009 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.  
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "mrPhysicsScenePhysX3.h"
#include "mrPhysX3.h"
#include "mrPhysicsRigPhysX3.h"
#include "mrCharacterControllerInterfacePhysX3.h"
#include "physics/mrPhysicsSerialisationBuffer.h"
#include "mrPhysX3Includes.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

PhysXPerShapeData::ShapeToDataMap* PhysXPerShapeData::s_shapeToDataMap = 0;
NMP::HeapAllocator* PhysXPerShapeData::s_mapAllocator = 0;

//----------------------------------------------------------------------------------------------------------------------
physx::PxFilterFlags morphemePhysX3FilterShader(
  physx::PxFilterObjectAttributes attributes0,
  physx::PxFilterData filterData0,
  physx::PxFilterObjectAttributes attributes1,
  physx::PxFilterData filterData1,
  physx::PxPairFlags &pairFlags, 
  const void *NMP_UNUSED(constantBlock),
  physx::PxU32 NMP_UNUSED(constantBlockSize))
{
  bool kinematic0 = physx::PxFilterObjectIsKinematic(attributes0);
  bool kinematic1 = physx::PxFilterObjectIsKinematic(attributes1);
  if (kinematic0 && kinematic1)
  {
    pairFlags = physx::PxPairFlags(); 
    return physx::PxFilterFlag::eSUPPRESS;
  }

  // Support the idea of this being called from a user's filter shader - i.e. don't trample on existing flags
  pairFlags |= physx::PxPairFlag::eRESOLVE_CONTACTS;

  // We use the flag to see if it's associated with an articulation so that we pick up both the
  // dynamic and the kinematic parts. 
  bool art0 = (filterData0.word0 & 1 << MR::GROUP_CHARACTER_PART) != 0;
  bool art1 = (filterData1.word0 & 1 << MR::GROUP_CHARACTER_PART) != 0;

  if (art0 || art1) // only get info for contact info if a character articulation is involved
  {
    pairFlags |= 
      physx::PxPairFlag::eNOTIFY_CONTACT_POINTS |
      physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
      physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS;
#ifdef USE_PHYSX_SWEEPS_FOR_CHARACTER
    pairFlags |= physx::PxPairFlag::eSWEPT_INTEGRATION_LINEAR;
#endif
  }
  if (art0 && art1) // Two articulations
  {
    if (filterData0.word2 == filterData1.word2) // if they are parts on the same articulation
    {
      // if they share any of the same collision groups, then they should not collide
      if (filterData0.word3 & filterData1.word3) 
      {
        pairFlags = physx::PxPairFlags(); // disable all dynamic collisions
        return physx::PxFilterFlag::eSUPPRESS; // Suppress, not Kill, as the filter data can change
      }
    }
    // Enable contact modification for self collision
    pairFlags |= physx::PxPairFlag::eMODIFY_CONTACTS;
    return physx::PxFilterFlag::eDEFAULT;
  }
  else // non-articulation is involved
  {
    // now test groups (word0) against ignore groups (word1)
    if ((filterData0.word0 & filterData1.word1) || (filterData1.word0 & filterData0.word1)) 
    {
      pairFlags = physx::PxPairFlags(); // disable all dynamic collisions
      return physx::PxFilterFlag::eSUPPRESS; // this stops the pair being actually passed into ray probe onObjectQuery
    }
    else
    {
      return physx::PxFilterFlag::eDEFAULT;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void MorphemePhysX3ContactModifyCallback::onContactModify(
  physx::PxContactModifyPair *const pairs, physx::PxU32 count)
{
  // Limit the reported penetration to a very low value - this prevents parts "pinging" out of each
  // other, which can happen when collision groups are enabled/disabled, or could happen when
  // transitioning from animation to physics.
  const float maxPenetration = m_maxSeparationSpeed * m_timeStep;
  for (physx::PxU32 i = 0 ; i != count ; ++i)
  {
    physx::PxContactModifyPair& pair = pairs[i];
    
    physx::PxU32 numPts = pair.contacts.size();
    for (physx::PxU32 iPt = 0 ; iPt != numPts ; ++iPt)
    {
      float penetration = -pair.contacts.getSeparation(iPt);
      if (penetration > maxPenetration)
      {
        pair.contacts.setSeparation(iPt, -maxPenetration);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
physx::PxSceneQueryHitType::Enum MorphemePhysX3QueryFilterCallback::preFilter(
  const physx::PxFilterData& NMP_UNUSED(filterData), 
  physx::PxShape* shape, 
  physx::PxSceneQueryFilterFlags& filterFlags)
{
  physx::PxFilterData shapeFilterData = shape->getQueryFilterData();
  (void) filterFlags; // this is the filterFlags passed into raycastSingle
  
  // Ignore the specified ignore ID, e.g. character probes can ignore the character's parts
  if (m_ignoreID == (int)shapeFilterData.word2)
  {
    return physx::PxSceneQueryHitType::eNONE;
  }

  // now test groups (word0) against ignore groups (word1)
  if ((shapeFilterData.word0 & m_queryFilterData.word1) || (m_queryFilterData.word0 & shapeFilterData.word1)) 
  {
    return physx::PxSceneQueryHitType::eNONE;
  }
  else
  {
    return physx::PxSceneQueryHitType::eBLOCK;
  }
}

//----------------------------------------------------------------------------------------------------------------------
physx::PxSceneQueryHitType::Enum MorphemePhysX3QueryFilterCallback::postFilter(
  const physx::PxFilterData& NMP_UNUSED(filterData), 
  const physx::PxSceneQueryHit& NMP_UNUSED(hit))
{
  NMP_ASSERT_FAIL();
  return physx::PxSceneQueryHitType::eNONE;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsScenePhysX3::rayCollide(physx::PxRaycastHit &raycastHit,
                                    const NMP::Vector3 &position, 
                                    const NMP::Vector3 &direction, 
                                    float distance, 
                                    uint32_t collisionIgnoreMask,
                                    physx::PxClientID ownerClientID) const
{
  // Use default PxFilterData here to bypass the internal filtering, and we pass the real flags
  // into the callback.
  physx::PxSceneQueryFilterData filterData; // by default hits all static & dynamic - requires masks to be all zero(?!)

  filterData.flags = physx::PxSceneQueryFilterFlags(
    physx::PxSceneQueryFilterFlag::eSTATIC | 
    physx::PxSceneQueryFilterFlag::eDYNAMIC | 
    physx::PxSceneQueryFilterFlag::ePREFILTER);

  physx::PxSceneQueryFlags flags(
    physx::PxSceneQueryFlag::eIMPACT | 
    physx::PxSceneQueryFlag::eNORMAL | 
    physx::PxSceneQueryFlag::eDISTANCE);

  MorphemePhysX3QueryFilterCallback morphemePhysX3QueryFilterCallback(physx::PxFilterData(
    0,
    collisionIgnoreMask,
    0,
    0));

  return m_physXScene->raycastSingle(
    nmVector3ToPxVec3(position), 
    nmVector3ToPxVec3(direction), 
    distance, 
    flags,
    raycastHit, 
    filterData,
    &morphemePhysX3QueryFilterCallback,
    NULL,
    ownerClientID);
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsScenePhysX3::PhysicsScenePhysX3(physx::PxScene *physXScene) :
  m_physXScene(physXScene)
{
  PhysicsRigPhysX3ActorData::init();

  if (physXScene)
  {
    setPhysXScene(physXScene);
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsScenePhysX3::~PhysicsScenePhysX3()
{
  PhysicsRigPhysX3ActorData::term();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsScenePhysX3::setPhysXScene(physx::PxScene *physXScene) 
{ 
  m_physXScene = physXScene;   
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsScenePhysX3::getFloorPositionBelow(
  const NMP::Vector3 &pos, 
  const PhysicsRig *skipChar, 
  float distToCheck) const
{
  // Rather than skipping the character, skip all characters - that means we won't detect another
  // ragdoll etc as the ground...
  uint32_t ignore = 1<<MR::GROUP_NON_COLLIDABLE | 1<<MR::GROUP_INTERACTION_PROXY | 1<<MR::GROUP_CHARACTER_CONTROLLER;
  if (skipChar)
  {
    ignore |= 1<<MR::GROUP_CHARACTER_PART;
  }
  NMP::Vector3 rayDirection = m_worldUpDirection * -1.0f;
  physx::PxRaycastHit hit;
  const PhysicsRigPhysX3* physicsRigPhysX3 = (const PhysicsRigPhysX3*) skipChar;
  physx::PxClientID clientID = physicsRigPhysX3 ? physicsRigPhysX3->getClientID() : physx::PX_DEFAULT_CLIENT;
  if (rayCollide(hit, pos, rayDirection, distToCheck, ignore, clientID))
  {
    return nmPxVec3ToVector3(hit.impact);
  }
  else
  {
    return pos + (rayDirection * distToCheck);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsScenePhysX3::castRay(
    const NMP::Vector3& start,
    const NMP::Vector3& delta,
    const PhysicsRig* skipChar,
    const CharacterControllerInterface* skipCharController,
    float& hitDistance,
    NMP::Vector3& hitPosition,
    NMP::Vector3& hitNormal,
    NMP::Vector3& hitVelocity) const
{
  NMP::Vector3 dir = delta;
  float len = dir.normaliseGetLength();
  // Rather than skipping the character, skip all characters - that means we won't detect another
  // ragdoll etc as the ground...
  uint32_t ignore = 1<<MR::GROUP_NON_COLLIDABLE | 1<<MR::GROUP_INTERACTION_PROXY;
  if (skipChar)
  {
    ignore |= 1<<MR::GROUP_CHARACTER_PART;
  }
  if (skipCharController)
  {
    ignore |= 1<<MR::GROUP_CHARACTER_CONTROLLER;
  }

  physx::PxRaycastHit hit;

  const PhysicsRigPhysX3* physicsRigPhysX3 = (const PhysicsRigPhysX3*) skipChar;
  physx::PxClientID clientID = physicsRigPhysX3 ? physicsRigPhysX3->getClientID() : physx::PX_DEFAULT_CLIENT;
  if (rayCollide(hit, start, dir, len, ignore, clientID))
  {
    hitPosition = nmPxVec3ToVector3(hit.impact);
    hitNormal = nmPxVec3ToVector3(hit.normal);
    hitDistance = hit.distance;
    hitVelocity.setToZero();

    if (hit.shape)
    {
      const physx::PxRigidBody* rigidBody = hit.shape->getActor().isRigidBody();
      if (rigidBody)
      {
        hitVelocity = MR::nmPxVec3ToVector3(physx::PxRigidBodyExt::getVelocityAtPos(*rigidBody, hit.impact));
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsScenePhysX3::setGravity(const NMP::Vector3& gravity)
{
  physx::PxVec3 nxGravity(gravity.x, gravity.y, gravity.z);
  getPhysXScene()->setGravity(nxGravity);
}

//----------------------------------------------------------------------------------------------------------------------
// aux used to convert position from actorlocal to world frame
void actorToWorldPoint(const physx::PxActor * actor, const NMP::Vector3& xLocal, NMP::Vector3& xWorld)
{
  NMP::Matrix34 actorToWorld(NMP::Matrix34::kIdentity);
  MR::getActorGlobalPoseTM(*actor, actorToWorld);
  actorToWorld.transformVector(xLocal, xWorld);
}

//----------------------------------------------------------------------------------------------------------------------
// aux used to convert normal from actorlocal to world frame
void actorToWorldNormal(const physx::PxActor * actor, const NMP::Vector3& nLocal, NMP::Vector3& nWorld)
{
  NMP::Matrix34 actorToWorld;
  MR::getActorGlobalPoseTM(*actor, actorToWorld);
  actorToWorld.rotateVector(nLocal, nWorld);
}

//----------------------------------------------------------------------------------------------------------------------
// aux used to convert position from world to actorlocal frame
void worldToActorPoint(const physx::PxActor* actor, const NMP::Vector3& xWorld, NMP::Vector3& xLocal)
{
  NMP::Matrix34 worldToActor;
  MR::getActorGlobalPoseTM(*actor, worldToActor);
  worldToActor.invertFast();
  worldToActor.transformVector(xWorld, xLocal);
}

//----------------------------------------------------------------------------------------------------------------------
// aux used to convert normal from world to actorlocal frame
void worldToActorNormal(const physx::PxActor* actor, const NMP::Vector3& nWorld, NMP::Vector3& nLocal)
{
  NMP::Matrix34 worldToActor;
  MR::getActorGlobalPoseTM(*actor, worldToActor);
  worldToActor.invertFast();
  worldToActor.rotateVector(nWorld, nLocal);
}

void actorToWorldImpulse(physx::PxActor* actor, const NMP::Vector3 &dirL, const NMP::Vector3 &posL, NMP::Vector3 &dirW, NMP::Vector3 &posW)
{
  NMP::Matrix34 actorToWorld;
  getActorGlobalPoseTM(*actor, actorToWorld);  
  actorToWorld.rotateVector(dirL, dirW);
  actorToWorld.transformVector(posL, posW);
}

//----------------------------------------------------------------------------------------------------------------------
void addLocalImpulseAtLocalPosToActor(physx::PxActor& actor, const NMP::Vector3 &impulse, const NMP::Vector3 &position, float torqueMultiplier /*= 1.0f*/)
{
  NMP::Matrix34 actorToWorld;
  getActorGlobalPoseTM(actor, actorToWorld);
  NMP::Vector3 impulseW, positionW;
  actorToWorld.rotateVector(impulse, impulseW);
  actorToWorld.transformVector(position, positionW);

  addImpulseToActor(actor, impulseW, positionW, torqueMultiplier);
}

//----------------------------------------------------------------------------------------------------------------------
void addImpulseToActor(physx::PxActor& actor, const NMP::Vector3 &impulse, const NMP::Vector3 &position, float torqueMultiplier)
{
  if (actor.isRigidBody())
  {
    NMP::Vector3 actorCOM = getActorCOMPos(&actor);
    NMP::Vector3 torque = NMP::vCross(position - actorCOM, impulse) * torqueMultiplier;
    actor.isRigidBody()->addForce(nmVector3ToPxVec3(impulse), physx::PxForceMode::eIMPULSE);
    actor.isRigidBody()->addTorque(nmVector3ToPxVec3(torque), physx::PxForceMode::eIMPULSE);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void addVelocityChangeToActor(physx::PxActor& actor, const NMP::Vector3& velChange, const NMP::Vector3& worldPos, float torqueMultiplier)
{
  if (velChange.magnitude() == 0.0f)
    return;
  physx::PxRigidBody* rigidBody = actor.isRigidBody();
  if (!rigidBody)
    return;

  // Note that physx::PxRigidBodyExt::addForceAtPos doesn't work with velocity changes, so do the
  // maths ourselves.

  // calculate global inertia tensor
  NMP::Vector3 t = nmPxVec3ToVector3(rigidBody->getMassSpaceInertiaTensor());
  NMP::Matrix34 massSpaceInertiaTensor(NMP::Matrix34::kIdentity);
  massSpaceInertiaTensor.scale3x3(t);
  NMP::Matrix34 localOffset = nmPxTransformToNmMatrix34(rigidBody->getCMassLocalPose());
  NMP::Matrix34 globalInertiaTensor = massSpaceInertiaTensor * 
    localOffset * nmPxTransformToNmMatrix34(rigidBody->getGlobalPose());

  // Calculate inv inertia
  NMP::Matrix34 invInertiaWorld = globalInertiaTensor;
  invInertiaWorld.invert3x3();
  NMP::Vector3 COMPositionWorld = MR::getActorCOMPos(rigidBody);

  NMP::Vector3 localImpulsePos = worldPos - COMPositionWorld;
  NMP::Vector3 directionWorld = velChange.getNormalised();

  // Calculate impulse using the same equations as found in collision response code - i.e. relating
  // a required change in velocity in a certain direction to an impulse.
   float velChangePerUnitImpulse =  
    (1.0f / rigidBody->getMass()) + 
    NMP::vDot(directionWorld, 
    NMP::vCross(invInertiaWorld.getRotatedVector(NMP::vCross(localImpulsePos, 
    directionWorld)), 
    localImpulsePos));

  NMP::Vector3 impulse = velChange / velChangePerUnitImpulse;

  addImpulseToActor(actor, impulse, worldPos, 1.0f);

  if (torqueMultiplier != 1.0f)
  {
    // Apply a rotational boost by applying the scaled velocity change either side of the CoM
    NMP::Vector3 actorCOMW = MR::getActorCOMPos(&actor);
    NMP::Vector3 offset = worldPos - actorCOMW;

    MR::addVelocityChangeToActor(actor, velChange * (torqueMultiplier - 1.0f) * 0.5f, worldPos + offset);
    MR::addVelocityChangeToActor(actor, velChange * -(torqueMultiplier - 1.0f) * 0.5f, worldPos - offset);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void addForceToActor(physx::PxActor& actor, const NMP::Vector3 &force, const NMP::Vector3 &position)
{
  NMP::Vector3 actorCOM = getActorCOMPos(&actor);
  NMP::Vector3 torque = NMP::vCross(position - actorCOM, force);
  if (actor.isRigidBody())
  {
    actor.isRigidBody()->addForce(nmVector3ToPxVec3(force), physx::PxForceMode::eFORCE);
    actor.isRigidBody()->addTorque(nmVector3ToPxVec3(torque), physx::PxForceMode::eFORCE);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// a (sub-optimal) implementation of a function to get the inertia matrix of an actor in world frame
// todo: investigate loosing some conversions to and from physX types
NMP::Matrix34 getActorInertiaWorld(const physx::PxActor* actor)
{
  // get the (mass-aligned) body frame and it's inverse
  NMP::Matrix34 bodyTM = getActorCOMTM(actor);
  bodyTM.setTranslation(NMP::Vector3Zero());
  NMP::Matrix34 bodyTMInv(bodyTM);

  bodyTMInv.invertFast3x3();
  
  // get the body frame inertia matrix
  NMP::Matrix34 inertiaBody;
  inertiaBody.identity();
  const physx::PxRigidBody* rigidBody = actor->isRigidBody();
  physx::PxVec3 I = rigidBody->getMassSpaceInertiaTensor();
  inertiaBody.setXAxis(NMP::Vector3(I.x, 0, 0));
  inertiaBody.setYAxis(NMP::Vector3(0, I.y, 0));
  inertiaBody.setZAxis(NMP::Vector3(0, 0, I.z));
  
  // return world frame inertia
  return bodyTMInv * inertiaBody * bodyTM;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
