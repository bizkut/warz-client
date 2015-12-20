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
#ifndef GAME_CHARACTER_PHYSX2_H
#define GAME_CHARACTER_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/GameCharacter.h"
#include "physics/PhysX2/mrPhysicsScenePhysX2.h"
#include "GameCharacterControllerPhysX2.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
  class FastHeapAllocator;
}

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::CharacterPhysX2
///
/// A simplistic PhysX2 game character. This class expands on Game::Character by adding the functionality required by
/// a PhysX2 morpheme network. In a game scenario a Game::PhysX2Character class could also store data for a character
/// instance (current health/ammo etc).
//----------------------------------------------------------------------------------------------------------------------
class CharacterPhysX2 : public Character
{
public:

  virtual ~CharacterPhysX2()  {};

  /// \brief Allocate and initialise a CharacterPhysX2 instance.
  ///
  /// This does an initialising update step of the characters Network
  ///  in order to get it in to proper starting state.
  static CharacterPhysX2* create(
    CharacterDef*             characterDef,
    MR::PhysicsScenePhysX2*   physicsScene,
    NxControllerManager*      controllerManager,
    const NMP::Vector3&       initialPosition = NMP::Vector3::InitZero,
    const NMP::Quat&          initialOrientation = NMP::Quat::kIdentity,
    const NMP::Vector3&       graveyardPosition = NMP::Vector3(0.0f, -1000.0f, 0.0f), ///< Where to put the PhysX character controller object when we want to
                                                                                    ///<  temporarily disable collision with it.
    MR::AnimSetIndex          initialAnimSetIndex = 0,
    const char*               name = "",
    NMP::TempMemoryAllocator* temporaryMemoryAllocator = NULL,
    uint32_t                  externalIdentifier = 0xFFFFFFFF);

  /// \brief deinitialise and deallocate a CharacterPhysX2 instance.
  virtual void destroy() NM_OVERRIDE;

  //----------------------------
  // Character update stages.
  //----------------------------

  // Update order:
  //  1. CharacterPhysX2::updateToPreCharacterController
  //  2. CharacterPhysX2::updateCharacterControllerMove
  //  3. global PhysX character controller update
  //  4. CharacterPhysX2::updateControllerMoveAdjustingStepping
  //  5. global PhysX character controller update
  //  6. CharacterPhysX2::updatePostCharacterController
  //  7. CharacterPhysX2::updatePhysicsEngineFromCacheValues
  //  8. global PhysX simulation update
  //  9. CharacterPhysX2::updateCacheValuesFromPhysicsEngine
  // 10. CharacterPhysX2::updateFinalise

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
    float deltaTime ///< Frame update period.
    );

  /// \brief If necessary undo additional vertical movement which was added to maintain ground contact.
  ///
  /// This is not a thread safe step.
  void updateControllerMoveAdjustingStepping();
  
  /// \brief Execute the Network queue up to the point where a global physics update is required.
  ///
  /// Called after the PhysX character controller has been updated.  
  /// If the Network root position is being controlled by animation update our cached transforms 
  /// from the physics systems character controller transform,
  /// then execute the queue up to the point where a global physics update is required.
  /// This is a thread safe step.
  void updatePostCharacterController(
    float deltaTime ///< Frame update period.
    );

  /// \brief Write our cached PhysX channel data to our rig in preparation of the physics step.
  ///
  /// This is not a thread safe step.
  void updatePrePhysics();
  
  /// \brief Re-generate our physics cache data from the PhysX rig after the physics step which should have just taken place.
  ///
  /// This is not a thread safe step.
  void updatePostPhysics();

  /// \brief Complete network task execution and compute final rig world transforms.
  ///
  ///  The final update stage, to be called after physics simulation has been updated.
  ///  + Execute any remaining tasks on the queue.
  ///  + If the character root is being updated via physics e.g. when its in rag doll,
  ///    then update the character controller position from the physics rigs position.   
  ///  + Compute the characters world transforms.
  /// This is a thread safe step.
  void updateFinalise(
    float deltaTime ///< Frame update period.
    );

  //----------------------------
  // Accessors
  CharacterControllerPhysX2* getCharacterController() { return static_cast<CharacterControllerPhysX2*>(m_net->getCharacterController()); }
  
protected:

  CharacterPhysX2() {};
  
  /// \brief Regenerate the physics caching data from the most recent physics step.
  void readPhysicsRigFromPhysX();

  /// \brief Write the cached physics data into the PhysX rig in preparation for the physics step.
  void writePhysicsRigToPhysX();
};

} // namespace game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
