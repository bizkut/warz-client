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

#include "physics/mrPhysicsScene.h"
#include "physics/PhysX2/mrPhysX2.h"
//----------------------------------------------------------------------------------------------------------------------

class NxScene;

namespace MR
{

class CharacterControllerInterface;
struct PhysicsSerialisationBuffer;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsScenePhysX2
/// \brief Interface used to access the physics scene by morpheme runtime.
//----------------------------------------------------------------------------------------------------------------------
class PhysicsScenePhysX2 : public PhysicsScene
{
public:
  //--------------------------------------------------------------------------------------------------------------------
  // Functions that the application should call at the appropriate times
  //--------------------------------------------------------------------------------------------------------------------
  PhysicsScenePhysX2(NxScene* physXScene = 0) :
    m_physXScene(physXScene) {}
  virtual ~PhysicsScenePhysX2() {}

  /// Allow setting the PhysX scene in case it's not possible to pass it in in the constructor
  void setPhysXScene(NxScene* physXScene) { m_physXScene = physXScene; }

  //--------------------------------------------------------------------------------------------------------------------
  // Functions that the application implements unless the default implementation is OK
  //--------------------------------------------------------------------------------------------------------------------

  /// Cast a ray. Returns true/false to indicate a hit, and if there is a hit then hitDist etc
  /// will be set. Can pass in objects to ignore.
  virtual bool castRay(
    const NMP::Vector3&                 start,
    const NMP::Vector3&                 delta,
    const PhysicsRig*                   skipChar,
    const CharacterControllerInterface* skipCharController,
    float&                              hitDist,
    NMP::Vector3&                       hitPosition,
    NMP::Vector3&                       hitNormal,
    NMP::Vector3&                       hitVelocity) const;

  /// \brief This will return the floor position below pos.
  ///
  /// Default implementation simply ray casts. The application may wish to improve on this.
  virtual NMP::Vector3 getFloorPositionBelow(
    const NMP::Vector3& pos,        ///< Position to check below.
    const PhysicsRig*   skipChar,   ///< Indicates a character that needs to be skipped from the tests.
    float               distToCheck ///< How far to look.
  ) const NM_OVERRIDE;

  /// \brief Returns the gravity in the physics simulation.
  ///
  /// Default implementation returns the PhysX scene gravity.
  virtual NMP::Vector3 getGravity();

  /// \brief Sets the gravity used in the physics simulation.
  ///
  /// Default implementation sets the PhysX scene gravity.
  virtual void setGravity(const NMP::Vector3& gravity);

public:
  //--------------------------------------------------------------------------------------------------------------------
  // Functions below here are primarily for use of the morpheme runtime
  //--------------------------------------------------------------------------------------------------------------------

  /// Allow access to the PhysX scene for situations where it's really awkward to avoid it.
  NxScene* getPhysXScene() { return m_physXScene; }
  const NxScene* getPhysXScene() const { return m_physXScene; }

private:
  NxScene*     m_physXScene;
};

//----------------------------------------------------------------------------------------------------------------------
inline NMP::Vector3 getActorCOMPos(const NxActor* actor)
{
  return MR::nmNxVec3ToVector3(actor->getCMassGlobalPosition());
}

//----------------------------------------------------------------------------------------------------------------------
inline NMP::Matrix34 getActorGlobalPose(const NxActor* actor)
{
  return MR::nmNxMat34ToNmMatrix34(actor->getCMassGlobalPose());
}

//----------------------------------------------------------------------------------------------------------------------
inline  void setActorGlobalPose(NxActor& actor, const NMP::Matrix34& tm)
{
  return actor.setCMassGlobalPose(MR::nmMatrix34ToNxMat34(tm));
}

//----------------------------------------------------------------------------------------------------------------------
inline void addImpulseToActor(NxActor& actor, const NMP::Vector3 &impulse)
{
  actor.addForce(nmVector3ToNxVec3(impulse), NX_IMPULSE);
}

//----------------------------------------------------------------------------------------------------------------------
inline void setActorAngularVelocity(NxActor& actor, const NMP::Vector3 &v)
{
  actor.setAngularVelocity(nmVector3ToNxVec3(v));
}

//----------------------------------------------------------------------------------------------------------------------
inline void setActorLinearVelocity(NxActor& actor, const NMP::Vector3 &v)
{
  actor.setLinearVelocity(nmVector3ToNxVec3(v));
}

//----------------------------------------------------------------------------------------------------------------------
inline void addImpulseToActor(NxActor& actor, const NMP::Vector3 &impulse, const NMP::Vector3 &position, float torqueMultiplier)
{
  NMP::Vector3 actorCOM = getActorCOMPos(&actor);
  NMP::Vector3 torque = NMP::vCross(position - actorCOM, impulse) * torqueMultiplier;
  actor.addForce(nmVector3ToNxVec3(impulse), NX_IMPULSE);
  actor.addTorque(nmVector3ToNxVec3(torque), NX_IMPULSE);
}

//----------------------------------------------------------------------------------------------------------------------
inline void addLinearVelocityChangeToActor(NxActor& actor, const NMP::Vector3& velChange)
{
  actor.addForce(nmVector3ToNxVec3(velChange), NX_VELOCITY_CHANGE);
}

//----------------------------------------------------------------------------------------------------------------------
inline void addAngularVelocityChangeToActor(NxActor& actor, const NMP::Vector3& angularVelChange)
{
  actor.addTorque(nmVector3ToNxVec3(angularVelChange), NX_VELOCITY_CHANGE);
}


/// Applies a velocity change at the world space point, with an optional multiplier for the angular amount
void addVelocityChangeToActor(NxActor& actor, const NMP::Vector3& velChange, const NMP::Vector3& worldPos, float angularMultiplier = 1.0f);

}

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_SCENE_PHYSX_H
//----------------------------------------------------------------------------------------------------------------------
