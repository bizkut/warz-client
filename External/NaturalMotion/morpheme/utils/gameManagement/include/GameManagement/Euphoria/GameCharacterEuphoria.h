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
#ifndef GAME_CHARACTER_EUPHORIA_H
#define GAME_CHARACTER_EUPHORIA_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/GameCharacter.h"
#include "physics/PhysX3/mrPhysicsScenePhysX3.h"
#include "GameCharacterControllerEuphoria.h"
#include "GameCharacterDefEuphoria.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
  class FastHeapAllocator;
}

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::CharacterEuphoria
///
/// A simplistic PhysX3 game character. This class expands on Game::Character by adding the functionality required by
/// a PhysX3 morpheme network. In a game scenario a Game::PhysX3Character class could also store data for a character
/// instance (current health/ammo etc).
//----------------------------------------------------------------------------------------------------------------------
class CharacterEuphoria : public Character
{
public:

  virtual ~CharacterEuphoria()  {};

  /// \brief Allocate and initialise a CharacterEuphoria instance.
  ///
  /// This does an initialising update step of the characters Network
  ///  in order to get it in to proper starting state.
  static CharacterEuphoria* create(
    CharacterDefEuphoria*       characterEuphoriaDef,
    MR::PhysicsScenePhysX3*     physicsScene,
    physx::PxControllerManager* controllerManager,
    physx::PxMaterial*          characterControllerMaterial = NULL, ///< The material to use for the character controller we create
                                                                    ///<  (a default will be created if one is not provided).
    MR::PhysicsRigPhysX3::Type  physicsRigType = MR::PhysicsRigPhysX3::TYPE_ARTICULATED, ///< The type of physics rig to create,
                                                                                          ///<  MR::PhysicsRigPhysX3::TYPE_ARTICULATED or 
                                                                                          ///<  MR::PhysicsRigPhysX3::TYPE_JOINTED.
    const NMP::Vector3&         initialPosition = NMP::Vector3::InitZero,
    const NMP::Quat&            initialOrientation = NMP::Quat::kIdentity,
    MR::AnimSetIndex            initialAnimSetIndex = 0,
    const char*                 name = "",
    NMP::TempMemoryAllocator*   temporaryMemoryAllocator = NULL);

  /// \brief deinitialise and deallocate a CharacterEuphoria instance.
  virtual void destroy() NM_OVERRIDE;

  /// \brief Initialise the debug interface with the module names and controls.
  ///
  /// This gets the list of Euphoria module names and the debug controls and passes them to the
  /// debug interface registered with the dispatcher. If there's no character, dispatcher or
  /// registered debug interface it will do nothing, so it needs to be called towards the final
  /// stages of initialisation.
  void initDebugInterfaceModuleNamesAndControls();

  //----------------------------
  // Character update stages.
  //----------------------------

  // Update order:
  //  1. CharacterEuphoria::updateToPreCharacterController
  //  2. CharacterEuphoria::updateCharacterControllerMove
  //  3. CharacterEuphoria::updateControllerMoveAdjustingStepping
  //  4. CharacterEuphoria::updatePostCharacterController
  //  5. CharacterEuphoria::updatePrePhysics
  //  6. global PhysX simulation update
  //  7. CharacterEuphoria::updatePostPhysics
  //  8. CharacterEuphoria::updateFinalise

  /// \brief Update the morpheme Network to the point where we need to update the PhysX character controller.
  ///
  /// Starts the Network update by updating its connections and building the task queue.
  /// The task queue is then executed until an external character controller update is required.
  /// This is a thread safe step.
  void updateToPreCharacterController(
    float deltaTime ///< Frame update period.
    );

  /// \brief Move the character controller based on the requested delta transform.
  ///
  /// If a requested delta transform is not supplied here the delta calculated from the Network update will be used.
  /// This is not a thread safe step.
  void updateCharacterControllerMove(
    float               deltaTime,                             ///< Frame update period.
    const NMP::Vector3* requestedDeltaWorldTranslation = NULL, ///< Allows the caller to apply a delta translation other than the one generated by the Network update.
                                                               ///<  If not supplied then the delta value generated by the Network will be used.
    const NMP::Quat*    requestedDeltaWorldOrientation = NULL  ///< Allows the caller to apply a delta orientation other than the one generated by the Network update.
                                                               ///<  If not supplied then the delta value generated by the Network will be used.
    );

  /// \brief If necessary undo additional vertical movement which was added to maintain ground contact.
  ///
  /// This is not a thread safe step.
  void updateControllerMoveAdjustingStepping(float deltaTime);
  
  /// \brief Execute the Network queue up to the point where a global physics update is required.
  ///
  /// Called after the PhysX character controller has been updated.  
  /// If the Network root position is being controlled by animation update our cached transforms 
  /// from the physics systems character controller transform,
  /// then execute the queue up to the point where a global physics update is required.
  /// This is a thread safe step.
  void updatePostCharacterController(float deltaTime);

  /// \brief Write our cached PhysX channel data to our rig in preparation of the physics step.
  ///
  /// This is not a thread safe step.
  void updatePrePhysics(float deltaTime);
  
  /// \brief Re-generate our physics cache data from the PhysX rig after the physics step which should have just taken place.
  ///
  /// This is not a thread safe step.
  void updatePostPhysics(float deltaTime);

  /// \brief Complete network task execution and compute final rig world transforms.
  ///
  ///  The final update stage, to be called after physics simulation has been updated.
  ///  + Execute any remaining tasks on the queue.
  ///  + If the character root is being updated via physics e.g. when its in rag doll,
  ///    then update the character controller position from the physics rigs position.   
  ///  + Compute the characters world transforms.
  /// This is a thread safe step.
  void updateFinalise(
    float deltaTime   ///< Frame update period.
    );

  //----------------------------
  // Accessors
  CharacterControllerPhysX3* getCharacterController() { return static_cast<CharacterControllerPhysX3*>(m_net->getCharacterController()); }
  
protected:

  CharacterEuphoria() {};
  
  /// \brief Regenerate the physics caching data from the most recent physics step.
  void readPhysicsRigFromPhysX();

  /// \brief Write the cached physics data into the PhysX rig in preparation for the physics step.
  void writePhysicsRigToPhysX();

  ER::Character* m_euphoriaCharacter;
};

} // namespace game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_EUPHORIA_H
//----------------------------------------------------------------------------------------------------------------------
