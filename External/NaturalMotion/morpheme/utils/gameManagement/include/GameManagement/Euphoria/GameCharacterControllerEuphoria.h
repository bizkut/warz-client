// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef GAME_CHARACTER_CONTROLLER_EUPHORIA
#define GAME_CHARACTER_CONTROLLER_EUPHORIA
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMapContainer.h"

#include "morpheme/mrNetwork.h"
#include "physics/mrPhysics.h"
#include "physics/mrPhysicsRigDef.h"
#include "physics/PhysX3/mrCharacterControllerInterfacePhysX3.h"
#include "physics/PhysX3/mrPhysX3.h"
#include "physics/PhysX3/mrPhysicsRigPhysX3.h"
#include "physics/PhysX3/mrPhysicsScenePhysX3.h"
#include "physics/PhysX3/mrPhysX3Includes.h"

#include <map>
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

#ifdef NM_COMPILER_MSVC
#pragma warning (push)
#pragma warning (disable : 4324)
#endif

typedef std::map<physx::PxActor*, float> TouchTimes;

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::CharacterControllerPhysX3
/// \brief Wrapper to a PhysX character controller, implementing all functionality required for it to be updated
///  as a morpheme character controller.
///
/// Inherits from CharacterControllerInterfacePhysX3 which implements an interface that is required by a morpheme
/// Network in order to communicate with a PhysX character controller.
//----------------------------------------------------------------------------------------------------------------------
class CharacterControllerPhysX3 : public MR::CharacterControllerInterfacePhysX3
{
public:

  ~CharacterControllerPhysX3();

  /// \brief Allocate and initialise a CharacterControllerPhysX3 instance.
  static CharacterControllerPhysX3* create(
    MR::Network*                net,
    const NMP::Vector3&         initialPosition,    ///< Starting position.
    const NMP::Quat&            initialOrientation, ///< Starting orientation.
    MR::PhysicsScenePhysX3*     physicsScene,       ///< 
    physx::PxControllerManager* controllerManager,  ///< Reference to the shared controller manager.
    physx::PxMaterial*          characterControllerMaterial, ///< The material to use for this character controller
                                                             ///<  (a default will be created if one is not provided).
    MR::PhysicsRigPhysX3::Type  physicsRigType = MR::PhysicsRigPhysX3::TYPE_ARTICULATED); ///< The type of physics rig to create,
                                                                                          ///<  MR::PhysicsRigPhysX3::TYPE_ARTICULATED or MR::PhysicsRigPhysX3::TYPE_JOINTED.

  /// \brief Release a CharacterControllerPhysX3 instance.
  static void destroy(CharacterControllerPhysX3* characterController);  


  /// \brief Initialise the character controller.
  void init(
    MR::Network*                net,
    const NMP::Vector3&         initialPosition,    ///< Starting position.
    const NMP::Quat&            initialOrientation, ///< Starting orientation.
    MR::PhysicsScenePhysX3*     physicsScene,       ///< 
    physx::PxControllerManager* controllerManager,  ///< Reference to the shared controller manager.
    physx::PxMaterial*          characterControllerMaterial, ///< The material to use for this character controller
                                                             ///<  (a default will be created if one is not provided).
    MR::PhysicsRigPhysX3::Type  physicsRigType = MR::PhysicsRigPhysX3::TYPE_ARTICULATED); ///< The type of physics rig to create,
                                                                                          ///<  MR::PhysicsRigPhysX3::TYPE_ARTICULATED or MR::PhysicsRigPhysX3::TYPE_JOINTED. 

  /// \brief Release any memory allocated by this class.
  void term();
  
  //----------------------------
  // Controller update stages.
  //----------------------------

  // Update order:
  //  1. updateControllerMove
  //  2. updateControllerMoveAdjustingStepping
  //  3. CharacterControllerPhysX3::updateCachedTransformsFromCharacterControllerPosition
  //  4. global PhysX simulation update
  //  5. CharacterControllerPhysX3::updateCachedTransformsFromPhysicsRigPosition

  /// \brief Set up for and move the PhysX character controller with a frames delta transform.
  ///
  /// Enable or disable the PhysX controller based on whether the character root is controlled by animation movement or physics body position.
  /// If the network root pos is being controlled by animation update the requested delta transform and apply gravity acceleration.
  /// NOTE: if requestedDeltaWorldTranslation & requestedDeltaWorldOrientation are not provided then 
  /// the delta transform calculated and stored in the Network is used directly.
  /// If the PhysX character controller is enabled (character root is controlled by animation)
  /// apply a vertical stepping distance to the cached requested transform delta to attempt to
  /// keep the controller in ground contact, then apply the cached requested transform delta to
  /// the physics systems character controller.
  void updateControllerMove(
    float               deltaTime,
    const NMP::Vector3* requestedDeltaWorldTranslation,
    const NMP::Quat*    requestedDeltaWorldOrientation);

  /// \brief If necessary undo additional vertical movement which was added to maintain ground contact.
  /// 
  /// If we were on the ground, but are no longer on it, undo the extra vertical movement 
  /// that we added to keep our controller in ground contact (e.g. when going down hill).
  void updateControllerMoveAdjustingStepping(float deltaTime);

  /// \brief If the Network root position is being controlled by animation, update our cached transforms 
  ///  from the physics systems character controller transform.
  ///
  /// The PxController will have changed position according to the desired change from animation 
  /// filtered through the PxController semi-physical collision response.
  /// So we now need to update our cached transform to reflect this new value,
  /// and also set the root position for the network's animation data.
  void updateCachedTransformsFromCharacterControllerPosition();

  /// \brief If the Network root position is being controlled by the physical rig position (e.g. rag doll)
  /// update our cached transforms from the rigs position (obtained from the Network).
  ///
  /// Sets the world position of the character controller from the Networks character root transform change
  /// (driven from the physics rig position).
  /// To be called after the full network update.
  void updateCachedTransformsFromPhysicsRigPosition(float deltaTime);
  
  
  //----------------------------
  // Accessors
  NMP::Vector3 getPosition() const { return m_positionWorld; }
  NMP::Quat getOrientation() const { return m_orientationWorld; }
  
  /// \brief Set the controllers position.
  ///
  /// This teleports the controller to a new location.
  void setPosition(NMP::Vector3& position); ///< A position offset from the controller object centre (usually at its base).

  /// \brief Set the controllers orientation.
  void setOrientation(NMP::Quat& orientation);

  /// \brief Set the controllers transform.
  ///
  /// This teleports the controller to a new location and applies a new orientation..
  void setTransform(
    const NMP::Vector3& position,           ///< A position offset from the controller object centre (usually at its base).
    const NMP::Quat&    orientation);

  /// \brief 
  NMP::Vector3 computeWorldSpaceCCOriginOffset();

protected:

  CharacterControllerPhysX3();

  /// \brief Enable or disable collision with the physical character controller object.
  ///
  /// The controller object is enabled when the character is not in full physical simulation
  /// and disabled when the character is in full physical simulation.
  void enablePhysXController(bool enable);

  /// \brief Calculate the offset from the origin of the character controller shape.
  ///
  /// In setting the position of the character controller in PhysX you are setting the center of the capsule or box,
  /// but when placing a character in game we will generally use a ground position.
  /// So in order to place the controller correctly in the world we need to know how
  /// we should offset the character controller position from the game characters position.
  ///
  /// With this code we are assuming that the origin of the capsule is at its center and that we want to offset the origin to its base.
  /// Note: We have to calculate this value dynamically rather than using an offset value generated by connect when using a capsule.
  /// This is because if we dynamically scale the height of the character controller we cannot just scale an offset. 
  /// The offset would include the radius of the bottom sphere of the capsule, but this is not actually scaled when scaling height.
  NMP::Vector3 calculateOriginOffset(
    const NMP::Vector3& worldUpDirection);
  
  /// \brief Create the physics rig for this Network and attaches it to the Network.
  void createPhysicsRig(
    MR::Network*               net,               ///< The Network that we are creating this rig for.
    MR::PhysicsScenePhysX3*    physicsScene,      ///< The scene to create this rig in.
    MR::PhysicsRigPhysX3::Type physicsRigType,    ///< The type of physics rig to create, articulated or jointed.
    const NMP::Vector3&        initialPosition);  ///< Starting position for the rig.

  /// \brief Destroys any physics rigs attached to this Network.
  ///
  /// Set the Networks active physics rig to NULL.
  void destroyPhysicsRig(MR::Network *net);

  /// \brief Create a kinematic and dynamic PhysX character controller objects from the contents of the controller definition.
  ///
  /// Before calling this function m_positionWorld && m_orientationWorld must have been initialised.
  void createPhysXControllerObjects(
    MR::PhysicsScenePhysX3* physicsScene,
    physx::PxMaterial*      controllerMaterial); ///< Providing a material allows controllers to share the same material,
                                                 ///< if one is not provided a new default material will be created.

  /// \brief Destroy any created kinematic and dynamic PhysX character controller objects.
  void destroyPhysXControllerObjects();

  /// \brief Set the position of the kinematic controller and any bound dynamic object.
  void setControllerPosition(
    physx::PxController*   controller, 
    physx::PxRigidDynamic* dynamicActor,
    const NMP::Vector3&    position); ///< The true position of the controller object (usually at its center).

  /// \brief Generate requested delta transform values.
  ///
  /// Updates internal values m_requestedDeltaWorldTranslation & m_requestedDeltaWorldOrientation.
  /// If we have been supplied with a delta transform just use it otherwise use the values calculated and stored in the Network.
  void initRequestedWorldDeltaTransform(
    const NMP::Vector3* requestedDeltaWorldTranslation,
    const NMP::Quat*    requestedDeltaWorldOrientation);

  /// \brief Add affect of gravity to the requested delta translation.
  void addAccelerationDueToGravity(float deltaTime);

  /// \brief Try to resolve any penetration of the kinematic Actor with moving dynamic objects (e.g. standing on moving platforms).
  ///
  /// Query PhysX to check if the kinematic character controllers Actor interpenetrates the environment at all,
  ///  if it does: move the kinematic Actor to the dynamic Actor position in order to attempt to resolve the collision,
  ///  if it does not: move the dynamic Actor to the kinematic Actor position so that it tracks the character movement.
  void resolveKinematicActorPenetration(MR::PhysicsRigPhysX3* physicsRig);

  /// \brief Add the movement of any object that we are stood on to this frames requested delta movement of our character.
  /// 
  /// This is so we stay in contact with things like moving platforms.
  void addMovementOfStoodOnObject(float deltaTime);

  /// \brief Calculate the component of the requested translation (m_requestedDeltaWorldTranslation) that is in the horizontal plane.
  ///
  /// Compared against actual translation to determine if the requested translation has been achieved. 
  void calculateRequestedHorizontalDeltaTranslation();

  /// \brief If the controller was in ground contact last frame try and keep it in contact this frame by adding
  ///  a vertical step down to this frames requested delta movement of our character.
  void addStickToGroundMovement();

  /// \brief After moving the character controller object determine whether character is on the ground or in the air, 
  ///  and whether it has just transitioned between the two. Also update how long the character has been in ground contact.
  void updateGroundContactState(
    float        deltaTime,
    physx::PxU32 controllerCollisionFlags); ///< Generated by PhysX after moving the controller object.

protected:

  NMP::Vector3 m_positionWorld;                    ///< The characters current world position from this frames update.
  NMP::Quat    m_orientationWorld;                 ///< The characters current world orientation from this frames update.

  NMP::Vector3 m_positionWorldPrevFrame;           ///< The characters world position from the last frames update.
  NMP::Quat    m_orientationWorldPrevFrame;        ///< The characters world orientation from the last frames update.


  /// Requested movement : This can come from a user requested movement or it can be calculated directly from the Networks 
  ///  suggested delta values after updating its animation phase (up to MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER).
  NMP::Vector3 m_requestedDeltaWorldTranslation;   ///< The desired world space delta translation of this character for this frame.
                                                   ///<  The physical character controller will try to achieve this translation when it is updated,
                                                   ///<  but it may be resisted by solid objects or altered by falling under gravity.
  NMP::Vector3 m_requestedHorizontalDeltaTranslation; ///< The component of the requested delta world translation that is in the horizontal plane.
  NMP::Quat    m_requestedDeltaWorldOrientation;   ///< The desired world space orientation of this character for this frame.
                                                   ///<  Most physical character controller implementations will not restrict rotation that does not
                                                   ///<  change the world profile of the collision capsule (i.e. rotation about the capsule vertical axis).

  /// Actual movement: After the final stage of controller update, the transformation that was actually achieved is stored here.
  ///  During controller update working values are stored here.
  NMP::Vector3 m_actualDeltaWorldTranslation;      ///< The delta translation that the character archived this frame.
  NMP::Quat    m_actualDeltaWorldOrientation;      ///< The delta orientation that the character archived this frame.
  
  class ControllerHitReport*  m_hitReport;         ///< This PxUserControllerHitReport is used to maintain the list of "touch times" which tell
                                                   ///<  us how long the character controller has been in contact with any other physics bodies.
                                                   ///<  It is used to ramp up forces gradually when contacting objects.        

  bool                   m_physXControllerEnabled; ///< Is collision with the physical character controller object enabled.
  physx::PxController*   m_physXController;        ///< This is the PhysX character controller itself. This object is used by the physics
                                                   ///< engine during the character controller update step to simulate the character 
                                                   ///< moving through the physics world.
  physx::PxMaterial*    m_physXDefaultControllerMaterial; ///< An internally created and managed material for use by this character controller
                                                          ///< Note that it is only used if we are not provided with an external alternative to use.

  physx::PxRigidDynamic* m_physXRigidDynamicActor; ///< An additional actor that is constrained to the kinematic one that is used for
                                                   ///<  recovering from penetration, and can also be used for pushing other objects. 
  physx::PxD6Joint*      m_physXDynamicJoint;      ///< Joint used to constrain the dynamic actor to the kinematic one.
  
  physx::PxControllerManager* m_physXControllerManager; ///< Reference to the externally managed PhysX3 global controller manager.

  MR::CharacterControllerDef* m_activeControllerDef;    ///< The definition of the physical character controllers properties.
                                                        ///<  For example shape, size, friction etc.

  MR::Network*    m_net;                       ///< That this character controller is associated with.
  
  bool            m_gravityEnabled;            ///< Is gravity switched on.
  
  bool            m_onGround;                  ///< Is the character controller currently on the ground.
  bool            m_onGroundPrevFrame;         ///< Was the character controller on the ground on the last update.
  NMP::Vector3    m_verticalStepVector;        ///< Distance to move the character vertically down in order to try and 
                                               ///<  deal with stepping down without falling (down slopes or small steps etc).
  
  NMP::Vector3    m_originOffset;              ///< Offset between the physics systems root position of the character controller
                                               ///<  (possibly at it centre) and the morpheme networks idea of the controllers
                                               ///<  root position(at its base).
                                               ///< Expressed in character local space.
  NMP::Vector3 m_velocityInGravityDirection;   ///< The characters falling speed in the gravity direction.
  TouchTimes   m_touchTimes;                   ///< How long we've touched different objects, for ramping up forces.
};

#ifdef NM_COMPILER_MSVC
#pragma warning (pop)
#endif

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_CONTROLLER_EUPHORIA
//----------------------------------------------------------------------------------------------------------------------
