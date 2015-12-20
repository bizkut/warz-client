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
#ifndef MR_PHYSICS_SCENE_H
#define MR_PHYSICS_SCENE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// Properties of physical objects.
/// These may be used in different physics engines in slightly different ways, but in each case it must be possible to
/// tag objects with any number of these flags. Objects may of course be tagged with other flags as well.
//----------------------------------------------------------------------------------------------------------------------
enum GameGroup
{
  GROUP_NON_COLLIDABLE            = 0, ///< 1 Object does not interact
  GROUP_COLLIDABLE_NON_PUSHABLE   = 1, ///< 2 Object is (effectively) static/kinematic and can be interacted with
  GROUP_COLLIDABLE_PUSHABLE       = 2, ///< 4 Object is dynamic and can be interacted with
  GROUP_CHARACTER_CONTROLLER      = 3, ///< 8 Object is used as a character controller
  GROUP_CHARACTER_PART            = 4, ///< 16 Object is part of a morpheme physics rig
  GROUP_INTERACTION_PROXY         = 5, ///< 32 Interaction proxy object for character probes.
  GROUP_CHARACTER_PART_WITH_PROXY = 6, ///< 64 Object is part of a physics rig that has an interaction proxy
};

class PhysicsRig;
class CharacterControllerInterface;

// This class is raising "structure was padded due to __declspec(align())" in error.
#if defined(NM_HOST_64_BIT) && defined(NM_COMPILER_MSVC) 
  #pragma warning (push)
  #pragma warning (disable : 4324)
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsScene
/// \brief Interface used to access the physics scene by morpheme runtime.
///
/// The application should instantiate this and make it available to the runtime by passing it to the PhysicsRig.
/// Almost all access to the underlying physics SDK should go through this to ease porting to other physics libraries.
//----------------------------------------------------------------------------------------------------------------------
class PhysicsScene
{
protected:
  PhysicsScene() :
    m_worldUpDirection(0.0f, 1.0f, 0.0f),
    m_nextPhysicsTimeStep(1.0f),
    m_lastPhysicsTimeStep(1.0f) {}
  virtual ~PhysicsScene() {}

public:
  //--------------------------------------------------------------------------------------------------------------------
  // Functions that the application should call at the appropriate times
  //--------------------------------------------------------------------------------------------------------------------

  /// stores the duration of the next physics timestep. Should be called at the start of the network update.
  void setNextPhysicsTimeStep(float dt) { m_nextPhysicsTimeStep = dt; }

  /// stores the duration of the last physics timestep. Should be called immediately before/after the
  /// physics engine is stepped.
  void setLastPhysicsTimeStep(float dt) { m_lastPhysicsTimeStep = dt; }

  /// \brief Return a vector describing world upward direction
  virtual NMP::Vector3 getWorldUpDirection() const;

  /// \brief Set the internal vector describing world upward direction
  void setWorldUpDirection(const NMP::Vector3& worldUpDirection);

  //--------------------------------------------------------------------------------------------------------------------
  // Functions below here are primarily for use of the morpheme runtime
  //--------------------------------------------------------------------------------------------------------------------

  /// This will return the duration of the next physics simulation timestep. It will be called by
  /// the runtime during the network update
  float getNextPhysicsTimeStep() const;

  /// This will return the duration of the previous physics simulation timestep - so that the runtime
  /// can access the world time between the current and previous updates.
  float getLastPhysicsTimeStep() const;

  //--------------------------------------------------------------------------------------------------------------------
  // Functions below here should be implemented by engine-specific implementations.
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
    NMP::Vector3&                       hitVelocity) const = 0;

  /// This will return the floor position below pos.
  virtual NMP::Vector3 getFloorPositionBelow(
    const NMP::Vector3& pos,        ///< Position to check below.
    const PhysicsRig*   skipChar,   ///< Indicates a character that needs to be skipped from the tests.
    float               distToCheck ///< How far to look.
    ) const = 0;

  /// \brief Returns the gravity in the physics simulation.
  ///
  /// Default implementation returns the scene gravity.
  virtual NMP::Vector3 getGravity() = 0;

  /// \brief Sets the gravity used in the physics simulation.
  ///
  /// Default implementation sets the scene gravity.
  virtual void setGravity(const NMP::Vector3& gravity) = 0;

protected:
  /// direction defining upwards
  NMP::Vector3 m_worldUpDirection;

  /// durations of the next and last physics timesteps.
  float        m_nextPhysicsTimeStep;
  float        m_lastPhysicsTimeStep;
};

#if defined(NM_HOST_64_BIT) && defined(NM_COMPILER_MSVC) 
  #pragma warning (pop)
#endif

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float PhysicsScene::getNextPhysicsTimeStep() const
{
  return m_nextPhysicsTimeStep;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float PhysicsScene::getLastPhysicsTimeStep() const
{
  return m_lastPhysicsTimeStep;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::Vector3 PhysicsScene::getWorldUpDirection() const
{
  return m_worldUpDirection;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void PhysicsScene::setWorldUpDirection(const NMP::Vector3& worldUpDirection)
{
  m_worldUpDirection = worldUpDirection;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_SCENE_H
//----------------------------------------------------------------------------------------------------------------------
