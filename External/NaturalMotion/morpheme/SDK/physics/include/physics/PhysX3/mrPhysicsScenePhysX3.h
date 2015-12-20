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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_PHYSICS_SCENE_PHYSX_H
#define MR_PHYSICS_SCENE_PHYSX_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMHashMap.h"
#include "physics/mrPhysicsScene.h"
#include "physics/PhysX3/mrPhysX3.h"
#include "mrPhysX3Includes.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

/// user data attached to PhysX actors etc must be of this type - see the mrPhysicsRigPhysX.h for the declaration.
struct PhysicsRigPhysX3ActorData;

class CharacterControllerInterface;
struct PhysicsSerialisationBuffer;
class PhysicsRigPhysX3;

physx::PxFilterFlags morphemePhysX3FilterShader(
  physx::PxFilterObjectAttributes attributes0,
  physx::PxFilterData             filterData0,
  physx::PxFilterObjectAttributes attributes1,
  physx::PxFilterData             filterData1,
  physx::PxPairFlags&             pairFlags,
  const void*                     constantBlock,
  physx::PxU32                    constantBlockSize);

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemePhysX3ContactModifyCallback
///
/// Register this with PhysX if it's desirable to reduce the suddenness of penetration resolution in character
/// self-contacts. Specify a maximum speed at which objects will be separated (e.g. 1 m/s), and also
/// the size of your simulation timestep. These can be subsequently modified.
//----------------------------------------------------------------------------------------------------------------------
class MorphemePhysX3ContactModifyCallback : public physx::PxContactModifyCallback
{
public:
  MorphemePhysX3ContactModifyCallback()
    : m_maxSeparationSpeed(0.0f), m_timeStep(0.0f) {}
  MorphemePhysX3ContactModifyCallback(float maxSeparationSpeed, float timeStep) 
    : m_maxSeparationSpeed(maxSeparationSpeed), m_timeStep(timeStep) {}

  virtual void onContactModify(physx::PxContactModifyPair *const pairs, physx::PxU32 count);

  void setMaxSeparationSpeed(float maxSeparationSpeed) {m_maxSeparationSpeed = maxSeparationSpeed;}
  void setTimeStep(float timeStep) {m_timeStep = timeStep;}
private:
  float m_maxSeparationSpeed;
  float m_timeStep;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MorphemePhysX3QueryFilterCallback
//----------------------------------------------------------------------------------------------------------------------
class MorphemePhysX3QueryFilterCallback : public physx::PxSceneQueryFilterCallback
{
public:
  MorphemePhysX3QueryFilterCallback() :
    m_ignoreID(-1) {}
  MorphemePhysX3QueryFilterCallback(const physx::PxFilterData& queryFilterData, int32_t ignoreID=-1) :
    m_queryFilterData(queryFilterData),
    m_ignoreID(ignoreID) {}
  virtual ~MorphemePhysX3QueryFilterCallback() {}
protected:
  physx::PxSceneQueryHitType::Enum preFilter(const physx::PxFilterData& filterData, physx::PxShape* shape, physx::PxSceneQueryFilterFlags& filterFlags) NM_OVERRIDE;
  physx::PxSceneQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxSceneQueryHit& hit) NM_OVERRIDE;
  physx::PxFilterData m_queryFilterData;
  int32_t m_ignoreID;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class PhysicsScenePhysX3
///
/// Interface used to access the physics scene by morpheme runtime.
//----------------------------------------------------------------------------------------------------------------------
class PhysicsScenePhysX3 : public PhysicsScene
{
public:
  //----------------------------------------------------------------------
  // Functions that the application should call at the appropriate times
  //----------------------------------------------------------------------
  PhysicsScenePhysX3(physx::PxScene* physXScene = 0);

  virtual ~PhysicsScenePhysX3();

  /// Allow setting the PhysX scene in case it's not possible to pass it in in the constructor
  void setPhysXScene(physx::PxScene* physXScene);

  //----------------------------------------------------------------------
  // Functions that the application implement unless the default implementation is OK
  //----------------------------------------------------------------------

  /// Cast a ray. Returns true/false to indicate a hit, and if there is a hit then hitDist etc will
  /// be set (hit Velocity is the velocity of the point on the object hit). Can pass in objects to
  /// ignore.
  bool castRay(
    const NMP::Vector3&                 start,
    const NMP::Vector3&                 delta,
    const PhysicsRig*                   skipChar,
    const CharacterControllerInterface* skipCharController,
    float&                              hitDist,
    NMP::Vector3&                       hitPosition,
    NMP::Vector3&                       hitNormal,
    NMP::Vector3&                       hitVelocity) const NM_OVERRIDE;

  /// This will return the floor position below pos.
  /// Default implementation simply ray casts. The application may wish to improve on this.
  /// skipChar indicates the character that needs to be skipped form the tests
  NMP::Vector3 getFloorPositionBelow(
    const NMP::Vector3& pos,
    const PhysicsRig*   skipChar,
    float               distToCheck) const NM_OVERRIDE;

  /// Returns the gravity in the physics simulation.
  /// Default implementation returns the PhysX scene gravity
  NMP::Vector3 getGravity() NM_OVERRIDE;

  /// Sets the gravity used in the physics simulation
  /// Default implementation sets the PhysX scene gravity
  void setGravity(const NMP::Vector3& gravity) NM_OVERRIDE;

  /// Collides a ray against the environment. Returns true if there was a hit, and fills in
  /// raycastHit.
  /// See PhysX documentation for information on the ownerClientID and clientBehaviourBits
  virtual bool rayCollide(
    physx::PxRaycastHit& raycastHit,
    const NMP::Vector3&  position, 
    const NMP::Vector3&  direction, 
    float                distance, 
    uint32_t             collisionIgnoreMask,
    physx::PxClientID    ownerClientID) const;

public:
  //----------------------------------------------------------------------
  // Functions below here are primarily for use of the morpheme runtime
  //----------------------------------------------------------------------

  /// Allow access to the PhysX scene for situations where it's really awkward to avoid it.
  physx::PxScene* getPhysXScene() { return m_physXScene; };
  const physx::PxScene* getPhysXScene() const { return m_physXScene; };

private:
  physx::PxScene* m_physXScene;

#ifdef NM_HOST_64_BIT
  uint32_t pad[2];
#endif // NM_HOST_64_BIT

};


#define INITIAL_SHAPEMAP_SIZE 256
//----------------------------------------------------------------------------------------------------------------------
/// \struct PhysXPerShapeData
///
/// Per-shape data that is needed on physics objects if Euphoria is being used
//----------------------------------------------------------------------------------------------------------------------
struct PhysXPerShapeData
{
  static void initialiseMap()
  {
    NMP_ASSERT_MSG(s_mapAllocator == NULL && s_shapeToDataMap == NULL, "PhysXPerShapeData map already initialised.");

    NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(NMP::HeapAllocator::getMemoryRequirements());
    s_mapAllocator = NMP::HeapAllocator::init(resource);

    NMP::Memory::Format mapFormat(sizeof(ShapeToDataMap));
    NMP::Memory::Resource mapResource = NMPMemoryAllocateFromFormat(mapFormat);
    s_shapeToDataMap = ShapeToDataMap::init(mapResource, INITIAL_SHAPEMAP_SIZE, s_mapAllocator);
    NMP_ASSERT(mapResource.format.size == 0);
  }

  static void destroyMap()
  {
    if (s_shapeToDataMap)
    {
      s_shapeToDataMap->~ShapeToDataMap();
      NMP::Memory::memFree(s_shapeToDataMap);
      s_shapeToDataMap = NULL;
    }

    if (s_mapAllocator)
    {
      s_mapAllocator->term();
      s_mapAllocator->~HeapAllocator();
      NMP::Memory::memFree(s_mapAllocator);
      s_mapAllocator = NULL;
    }
  }

  static PhysXPerShapeData* create(physx::PxShape* shape)
  {
    if (!s_shapeToDataMap)
      return 0;
    PhysXPerShapeData* data = (PhysXPerShapeData*) NMPMemoryAlloc(sizeof(PhysXPerShapeData));
    NMP_ASSERT(data);
    new (data) PhysXPerShapeData(shape);
    return data;
  }

  static void destroy(PhysXPerShapeData* data, physx::PxShape* shape)
  {
    if (!s_shapeToDataMap)
      return;
    if (!data)
      return;
    if (shape)
    {
      s_shapeToDataMap->erase(shape);
    }
    NMP::Memory::memFree(data);
  }

  static PhysXPerShapeData* getFromShape(physx::PxShape* shape)
  {
    if (!s_shapeToDataMap)
      return 0;
    PhysXPerShapeData* pVal = 0;
    s_shapeToDataMap->find(shape, &pVal);
    return pVal;
  }

  // Data
#if defined(MR_OUTPUT_DEBUGGING)
  NMP::Vector3 m_debugColour;
#endif
  int32_t m_dataIndex; // Index into the array of EA objects

private:
  typedef NMP::hash_map<physx::PxShape*, PhysXPerShapeData*>  ShapeToDataMap;

  PhysXPerShapeData(physx::PxShape* shape) : m_dataIndex(0)
  {
    NMP_ASSERT(getFromShape(shape) == 0);
    s_shapeToDataMap->insert(shape, this);
#if defined(MR_OUTPUT_DEBUGGING)
    m_debugColour.setToZero();
#endif
  }

  static ShapeToDataMap*      s_shapeToDataMap;
  static NMP::HeapAllocator*  s_mapAllocator;
};


/// returns the actor tm
void getActorGlobalPoseTM(const physx::PxActor& actor, NMP::Matrix34& actorToWorld);
// sets the actor TM - This method instantaneously changes the actor space to world space transformation.
void setActorGlobalPoseTM(physx::PxActor& actor, const NMP::Matrix34& actorToWorld);
/// applies an impulse to the point on the actor: impulse and position coordinates are relative and wrt global frame
void addImpulseToActor(physx::PxActor& actor, const NMP::Vector3 &impulse, const NMP::Vector3 &position, float torqueMultiplier = 1.0f);
/// applies an impulse to the point on the actor: impulse and position coordinates are relative and wrt actor frame
void addLocalImpulseAtLocalPosToActor(physx::PxActor& actor, const NMP::Vector3 &impulse, const NMP::Vector3 &position, float torqueMultiplier = 1.0f);
/// applies a force to the point on the actor: force and position coordinates are wrt global frame
void addForceToActor(physx::PxActor& actor, const NMP::Vector3 &force, const NMP::Vector3 &position);
/// applies a force to the actor: force and is in the global frame
void addForceToActor(physx::PxActor& actor, const NMP::Vector3 &force);
/// applies pure linear impulse (i.e. as if to actor centre of mass): impulse coordinates wrt global frame
void addImpulseToActor(physx::PxActor& actor, const NMP::Vector3& impulse);
/// applies a pure torque (i.e. as if via a couple): torque coordinates wrt global frame
void addTorqueToActor(physx::PxActor& actor, const NMP::Vector3& torque);
/// applies a pure torqueImpulse (i.e. as if via a couple): torqueImpulse coordinates wrt global frame
void addTorqueImpulseToActor(physx::PxActor& actor, const NMP::Vector3& torqueImpulse);

/// applies an acceleration to the actor - accel is in the global frame
void addAccelerationToActor(physx::PxActor& actor, const NMP::Vector3 &accel);

/// Applies a velocity change at the world space point, with an optional multiplier for the angular amount
void addVelocityChangeToActor(physx::PxActor& actor, const NMP::Vector3& velChange, const NMP::Vector3& worldPos, float angularMultiplier = 1.0f);
/// Applies a linear velocity change
void addLinearVelocityChangeToActor(physx::PxActor& actor, const NMP::Vector3& velChange);
/// Applies an angular velocity change
void addAngularVelocityChangeToActor(physx::PxActor& actor, const NMP::Vector3& angularVelChange);
/// Applies an angular acceleration
void addAngularAccelerationToActor(physx::PxActor& actor, const NMP::Vector3& angularAccel);

/// aux used to convert position from actorlocal to world frame
void actorToWorldPoint(const physx::PxActor * actor, const NMP::Vector3& xLocal, NMP::Vector3& xWorld);
/// aux used to convert normal from actorlocal to world frame
void actorToWorldNormal(const physx::PxActor * actor, const NMP::Vector3& nLocal, NMP::Vector3& nWorld);
/// aux to convert actor local direction and point of application eg. of an impulse
void actorToWorldImpulse(physx::PxActor* actor, const NMP::Vector3 &dirL, const NMP::Vector3 &posL, NMP::Vector3 &dirW, NMP::Vector3 &posW);
// aux used to convert normal from world to actorlocal frame
void worldToActorNormal(const physx::PxActor * actor, const NMP::Vector3& nWorld, NMP::Vector3& nLocal);
// aux used to convert position from world to actorlocal frame
void worldToActorPoint(const physx::PxActor * actor, const NMP::Vector3& xWorld, NMP::Vector3& xLocal);

/// returns the mass frame of the actor
NMP::Matrix34 getActorCOMTM(const physx::PxActor* actor);
/// returns the centre of mass position of the actor
NMP::Vector3 getActorCOMPos(const physx::PxActor* actor);
/// returns the angular velocity of the actor
NMP::Vector3 getActorAngVelW(const physx::PxActor* actor);
/// returns the linear velocity of the actor
NMP::Vector3 getActorLinVelW(const physx::PxActor* actor);
/// returns world frame actor inertia matrix
NMP::Matrix34 getActorInertiaWorld(const physx::PxActor* actor);
/// returns the actor mass, or 0 if it's not dynamic
float getActorMass(const physx::PxActor *actor);

/// zeros the linear velocity of the actor
void setActorLinVelW(physx::PxActor& actor, const NMP::Vector3 &accel);
/// zeros the angular velocity of the actor
void setActorAngVelW(physx::PxActor& actor, const NMP::Vector3 &accel);

//----------------------------------------------------------------------------------------------------------------------
// inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void setActorLinVelW(physx::PxActor& actor, const NMP::Vector3 &v)
{
  physx::PxRigidBody* rigidBody = actor.isRigidBody();
  rigidBody->setLinearVelocity(MR::nmVector3ToPxVec3(v));
}
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void setActorAngVelW(physx::PxActor& actor, const NMP::Vector3 &v)
{
  physx::PxRigidBody* rigidBody = actor.isRigidBody();
  rigidBody->setAngularVelocity(MR::nmVector3ToPxVec3(v));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Matrix34 getActorCOMTM(const physx::PxActor* actor)
{
  const physx::PxRigidBody* rigidBody = actor->isRigidBody();
  return MR::nmPxTransformToNmMatrix34(rigidBody->getGlobalPose().transform(rigidBody->getCMassLocalPose()));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 getActorCOMPos(const physx::PxActor* actor)
{
  const physx::PxRigidBody* rigidBody = actor->isRigidBody();
  return MR::nmPxVec3ToVector3(rigidBody->getGlobalPose().transform(rigidBody->getCMassLocalPose().p));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void getActorGlobalPoseTM(const physx::PxActor& actor, NMP::Matrix34& actorToWorld) 
{ 
  if (actor.isRigidActor())
    actorToWorld = nmPxTransformToNmMatrix34(actor.isRigidActor()->getGlobalPose());
} 

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void setActorGlobalPoseTM(physx::PxActor& actor, const NMP::Matrix34& newActorToWorld) 
{ 
  if (actor.isRigidActor())
    actor.isRigidActor()->setGlobalPose(nmMatrix34ToPxTransform(newActorToWorld), true);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 getActorAngVelW(const physx::PxActor* actor)
{
  const physx::PxRigidBody* rigidBody = actor->isRigidBody();
  return MR::nmPxVec3ToVector3(rigidBody->getAngularVelocity());
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 getActorLinVelW(const physx::PxActor* actor)
{
  const physx::PxRigidBody* rigidBody = actor->isRigidBody();
  return MR::nmPxVec3ToVector3(rigidBody->getLinearVelocity());
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void addLinearVelocityChangeToActor(physx::PxActor& actor, const NMP::Vector3& velChange)
{
  if (actor.isRigidBody())
    actor.isRigidBody()->addForce(nmVector3ToPxVec3(velChange), physx::PxForceMode::eVELOCITY_CHANGE);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void addAngularVelocityChangeToActor(physx::PxActor& actor, const NMP::Vector3& angularVelChange)
{
  if (actor.isRigidBody())
    actor.isRigidBody()->addTorque(nmVector3ToPxVec3(angularVelChange), physx::PxForceMode::eVELOCITY_CHANGE);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void addAngularAccelerationToActor(physx::PxActor& actor, const NMP::Vector3& angularAccel)
{
  if (actor.isRigidBody())
    actor.isRigidBody()->addTorque(nmVector3ToPxVec3(angularAccel), physx::PxForceMode::eACCELERATION);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void addForceToActor(physx::PxActor& actor, const NMP::Vector3 &force)
{
  if (actor.isRigidBody())
    actor.isRigidBody()->addForce(nmVector3ToPxVec3(force), physx::PxForceMode::eFORCE);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void addAccelerationToActor(physx::PxActor& actor, const NMP::Vector3 &accel)
{
  if (actor.isRigidBody())
    actor.isRigidBody()->addForce(nmVector3ToPxVec3(accel), physx::PxForceMode::eACCELERATION);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void addImpulseToActor(physx::PxActor& actor, const NMP::Vector3 &impulse)
{
  if (actor.isRigidBody())
    actor.isRigidBody()->addForce(nmVector3ToPxVec3(impulse), physx::PxForceMode::eIMPULSE);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void addTorqueToActor(physx::PxActor& actor, const NMP::Vector3 &torque)
{
  if (actor.isRigidBody())
    actor.isRigidBody()->addTorque(nmVector3ToPxVec3(torque), physx::PxForceMode::eFORCE);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void addTorqueImpulseToActor(physx::PxActor& actor, const NMP::Vector3 &torqueImpulse)
{
  if (actor.isRigidBody())
    actor.isRigidBody()->addTorque(nmVector3ToPxVec3(torqueImpulse), physx::PxForceMode::eIMPULSE);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 PhysicsScenePhysX3::getGravity() 
{
  return nmPxVec3ToVector3(getPhysXScene()->getGravity());
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float getActorMass(const physx::PxActor *actor)
{
  if (actor->isRigidBody())
    return actor->isRigidBody()->getMass();
  else
    return 0.0f;
}

} // namespace

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_SCENE_PHYSX_H
//----------------------------------------------------------------------------------------------------------------------
