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
#ifndef MR_CHARACTER_CONTROLLER_PHYSX_H
#define MR_CHARACTER_CONTROLLER_PHYSX_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrCharacterControllerInterface.h"
#include "physics/mrCCOverrideBasics.h"
#include "physics/PhysX2/mrPhysicsScenePhysX2.h"
//----------------------------------------------------------------------------------------------------------------------

class NxActor;

namespace MR
{

class PhysicsRig;
class PhysicsScene;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::CharacterControllerInterfacePhysX2
/// \brief This specialization of the MR::CharacterControllerInterface class adds support for the PhysX 2 engine.
//----------------------------------------------------------------------------------------------------------------------
class CharacterControllerInterfacePhysX2 : public CharacterControllerInterface
{
public:
  CharacterControllerInterfacePhysX2();
  virtual ~CharacterControllerInterfacePhysX2() {}

  /// Set the underlying physics engine object which is a representation of this character controller.  This is used
  /// for such tasks as ignoring collisions against the character controller's volume when
  /// ray-casting into the world. It can be left as zero if not actually used.
  void setActor(NxActor* actor) { m_actor = actor; }

  /// Get the underlying physics engine object which is a representation of this character controller.
  NxActor* getActor() { return m_actor; }
  const NxActor* getActor() const { return m_actor; }

  /// Physics scene access.
  void setPhysicsScene(PhysicsScenePhysX2* physicsScene) { m_physicsScene = physicsScene; }
  /// Get the physics scene
  PhysicsScenePhysX2* getPhysicsScene() { return m_physicsScene; }

  bool castRayIntoCollisionWorld(
    const NMP::Vector3& start,
    const NMP::Vector3& delta,
    float&              hitDist,
    NMP::Vector3&       hitPosition,
    NMP::Vector3&       hitNormal,
    NMP::Vector3&       hitVelocity,
    MR::Network*        network) const NM_OVERRIDE;

  /// \brief Store a requested property change on the character controller.
  /// \return true if property was actually set, 
  ///         false if set did not happen (not high enough priority, property type not handled).
  virtual bool setRequestedPropertyOverride(
    CCPropertyType     propertyType,
    AttribData*        property,
    CCOverridePriority priority,
    FrameCount         frameIndex) NM_OVERRIDE;
  
  /// \brief Get a requested property change on the character controller.
  /// \return NULL if there is no cached requested override of this property type for this frame.
  virtual const AttribData* getRequestedPropertyOverride(
    CCPropertyType propertyType,
    FrameCount     frameIndex) const NM_OVERRIDE;

public:
  //--------------------------------
  // Cached override property values.
  CCOverrideBasics  m_overrideBasics;
    
private:
  /// Associated physics engine representation
  NxActor*            m_actor;
  PhysicsScenePhysX2* m_physicsScene;
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_CHARACTER_CONTROLLER_PHYSX_H
//----------------------------------------------------------------------------------------------------------------------
