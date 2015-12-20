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
#ifndef GAME_CHARACTER_CONTROLLER_PHYSX2
#define GAME_CHARACTER_CONTROLLER_PHYSX2
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMapContainer.h"

#include "morpheme/mrNetwork.h"
#include "physics/mrPhysics.h"
#include "physics/mrPhysicsRigDef.h"
#include "physics/PhysX2/mrCharacterControllerInterfacePhysX2.h"
#include "physics/PhysX2/mrPhysX2.h"
#include "physics/PhysX2/mrPhysicsRigPhysX2.h"
#include "physics/PhysX2/mrPhysicsScenePhysX2.h"
#include "physics/PhysX2/mrPhysX2Includes.h"

#include <map>
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

typedef std::map<NxActor*, float> TouchTimes;

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::CharacterControllerPhysX2
/// \brief Wrapper to a PhysX character controller, implementing all functionality required for it to be updated
///  as a morpheme character controller.
///
/// Inherits from CharacterControllerInterfacePhysX2 which implements an interface that is required by a morpheme
/// Network in order to communicate with a PhysX character controller.
//----------------------------------------------------------------------------------------------------------------------
class CharacterControllerPhysX2 : public MR::CharacterControllerInterfacePhysX2
{
public:

  ~CharacterControllerPhysX2();

  /// \brief Allocate and initialise a CharacterControllerPhysX2 instance.
  static CharacterControllerPhysX2* create(
    MR::Network*            net,
    const NMP::Vector3&     initialPosition,    ///< Starting position.
    const NMP::Quat&        initialOrientation, ///< Starting orientation.
    const NMP::Vector3&     graveyardPosition,  ///< Where to put the PhysX character controller object when we want to
                                                ///<  temporarily disable collision with it.
    MR::PhysicsScenePhysX2* physicsScene,
    NxControllerManager*    controllerManager);

  /// \brief Release a CharacterControllerPhysX2 instance.
  static void destroy(CharacterControllerPhysX2* characterController);  


  /// \brief Initialise the character controller.
  void init(
    MR::Network*            net,
    const NMP::Vector3&     initialPosition,    ///< Starting position.
    const NMP::Quat&        initialOrientation, ///< Starting orientation.
    const NMP::Vector3&     graveyardPosition,  ///< Where to put the PhysX character controller object when we want to
                                                ///<  temporarily disable collision with it.
    MR::PhysicsScenePhysX2* physicsScene,
    NxControllerManager*    controllerManager);

  /// \brief Release any memory allocated by this class.
  void term();
  
  //----------------------------
  // Controller update stages.
  //----------------------------

  // Update order:
  //  1. CharacterControllerPhysX2::updateControllerMove
  //  2. global PhysX character controller update
  //  3. CharacterControllerPhysX2::updateControllerMoveAdjustingStepping
  //  4. global PhysX character controller update
  //  5. CharacterControllerPhysX2::updateCachedTransformsFromCharacterControllerPosition
  //  6. global PhysX simulation update
  //  7. CharacterControllerPhysX2::updateCachedTransformsFromPhysicsRigPosition


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
  /// To be called pre any global controller updates.
  void updateControllerMove(
    float               deltaTime,
    const NMP::Vector3* requestedDeltaWorldTranslation,
    const NMP::Quat*    requestedDeltaWorldOrientation);

  /// \brief If necessary undo additional vertical movement which was added to maintain ground contact.
  /// 
  /// If we were on the ground, but are no longer on it, undo the extra vertical movement 
  /// that we added to keep our controller in ground contact (e.g. when going down hill).
  /// This adjusted delta transform is then applied to the physics systems character controller.
  /// To be called between the two global controller updates.
  void updateControllerMoveAdjustingStepping();
  
  /// \brief If the Network root position is being controlled by animation update our cached transforms 
  ///  from the physics systems character controller transform.
  ///
  /// The NxController will have changed position according to the desired change from animation 
  /// filtered through the NxController's semi-physical collision detection,
  /// so we now need to update our cached transform to reflect this new value,
  ///  and also set the root position for the network's animation data.
  /// To be called after global controller update.
  void updateCachedTransformsFromCharacterControllerPosition(float deltaTime);

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
  void setPosition(NMP::Vector3& position);

  /// \brief Set the controllers orientation.
  void setOrientation(NMP::Quat& orientation);

  /// \brief Set the controllers transform.
  void setTransform(
    const NMP::Vector3& position,
    const NMP::Quat&    orientation);

  /// \brief 
  NMP::Vector3 computeWorldSpaceCCOriginOffset();

protected:

  CharacterControllerPhysX2();

  /// \brief Enable the physical character controller object and move it out of the graveyard.
  ///
  /// Set its position to that of its owning CharacterControllerPhysX2.
  /// The controller object is enabled when the character is not in full physical simulation.
  void enablePhysXController();

  /// \brief  Disable the physical character controller object and move it to the graveyard.
  ///
  /// Its owning CharacterControllerPhysX2 remains active at it's current location.
  /// The controller object is disabled when the character is in full physical simulation.
  void disablePhysXController();

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
  NMP::Vector3 calculateOriginOffset(const NMP::Vector3& worldUpDirection);

  /// \brief Create a physX character controller object from the contents of the controller definition.
  void createPhysXController(MR::PhysicsScenePhysX2* physicsScene);

  /// \brief Generate requested delta transform values.
  ///
  /// Updates internal values m_requestedDeltaWorldTranslation & m_requestedDeltaWorldOrientation.
  /// If we have been supplied with a delta transform just use it otherwise use the values calculated and stored in the Network.
  void initRequestedWorldDeltaTransform(
    const NMP::Vector3* requestedDeltaWorldTranslation,
    const NMP::Quat*    requestedDeltaWorldOrientation);

  /// \brief Add affect of gravity to the requested delta translation.
  void addAccelerationDueToGravity(float deltaTime);

  /// \brief If the controller was in ground contact last frame try and keep it in contact this frame by adding
  ///  a vertical step down to this frames requested delta movement of our character.
  void addStickToGroundMovement();

  /// \brief Calculate the component of the requested translation (m_requestedDeltaWorldTranslation) that is in the horizontal plane.
  ///
  /// Compared against actual translation to determine if the requested translation has been achieved. 
  void calculateRequestedHorizontalDeltaTranslation();

  /// \brief Determine whether the character achieved the requested delta movement this frame.
  bool didControllerAchieveRequestedMovement();

  /// \brief Record the vertical velocity of the character, but exclude any component that comes from 
  ///  the requested translation because this would artificially accelerate the character.
  ///
  ///  If on the ground then leave the velocity at zero so that we run off cliff edges starting horizontally
  ///  (even when we are stepping down).
  void computeVerticalVelocity(float deltaTime);
  
protected:

  NMP::Vector3 m_positionWorld;               ///< The characters current world position from this frames update.
  NMP::Quat    m_orientationWorld;            ///< The characters current world orientation from this frames update.

  NMP::Vector3 m_positionWorldPrevFrame;      ///< The characters world position from the last frames update.
  NMP::Quat    m_orientationWorldPrevFrame;   ///< The characters world orientation from the last frames update.


  /// Requested movement : This can come from a user requested movement or it can be calculated directly from the Networks 
  ///  suggested delta values after updating its animation phase (up to MR_TASKID_NETWORKUPDATECHARACTERCONTROLLER).
  NMP::Vector3 m_requestedDeltaWorldTranslation;      ///< The desired world space delta translation of this character for this frame.
                                                      ///<  The physical character controller will try to achieve this translation when it is updated,
                                                      ///<  but it may be resisted by solid objects or altered by falling under gravity.
  NMP::Vector3 m_requestedHorizontalDeltaTranslation; ///< The component of the requested delta world translation that is in the horizontal plane.
  NMP::Quat    m_requestedDeltaWorldOrientation;      ///< The desired world space orientation of this character for this frame.
                                                      ///<  Most physical character controller implementations will not restrict rotation that does not
                                                      ///<  change the world profile of the collision capsule (i.e. rotation about the capsule vertical axis).

  /// Actual movement: After the final stage of controller update, the transformation that was actually achieved is stored here.
  ///  During controller update working values are stored here.
  NMP::Vector3 m_actualDeltaWorldTranslation; ///< The delta translation that the character archived this frame.
  NMP::Quat    m_actualDeltaWorldOrientation; ///< The delta orientation that the character archived this frame.
  
  NMP::Vector3 m_graveyardPosition;           ///< When the nxController is disabled it is not removed from the world
                                              ///<  but rather sent to this location.

  NMP::Vector3 m_verticalStepVector;          ///< Distance to move the character vertically down in order to try and 
                                              ///<  deal with stepping down without falling (down slopes or small steps etc).
  
  NMP::Vector3 m_originOffset;                ///< Offset between the physics systems root position of the character controller
                                              ///<  (possibly at it centre) and the morpheme networks idea of the controllers
                                              ///<  root position(at its base).
                                              ///< Expressed in character local space.
  NMP::Vector3 m_velocityInGravityDirection;  ///< The characters falling speed in the gravity direction.

  class ControllerHitReport* m_hitReport;        ///< Used to apply forces to bodies that the character controller comes into contact with.
  bool                 m_physXControllerEnabled; ///< Is the physical character controller object enabled.
  NxController*        m_physXController;        ///< The physics systems collidable character controller object.
  NxControllerManager* m_physXControllerManager; ///< Reference to the PhysX2 global controller manager.
  

  MR::CharacterControllerDef* m_activeControllerDef; ///< The definition of the physical character controllers properties.
                                                     ///<  For example shape, size, friction etc.

  MR::Network*    m_net;                      ///< That this character controller is associated with.
  
  bool            m_gravityEnabled;           ///< Is gravity switched on.
  
  bool            m_onGround;                 ///< Is the character controller currently on the ground.
  bool            m_onGroundPrevFrame;        ///< Was the character controller on the ground on the last update.
 
  TouchTimes      m_touchTimes;               ///< How long we've touched different objects, for ramping up forces.
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_CONTROLLER_PHYSX2
//----------------------------------------------------------------------------------------------------------------------
