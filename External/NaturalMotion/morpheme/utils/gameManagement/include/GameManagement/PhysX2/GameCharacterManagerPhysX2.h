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
#ifndef GAME_CHARACTER_MANAGER_PHYSX2_H
#define GAME_CHARACTER_MANAGER_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
#include "physics/PhysX2/mrPhysicsScenePhysX2.h"
#include "mrPhysX2Includes.h"
#include "GameManagement/GameCharacterManager.h"
#include "GameManagement/PhysX2/GameCharacterPhysX2.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
/// \class Game::CharacterManagerPhysX2
/// \brief Provides a simplified interface to managing and updating a set of PhysX2 morpheme characters.
///
/// It is an example of how you could manage the morpheme animation system within your application.
/// It manages lists of characters and character definitions and wraps many morpheme API calls into conceptually simplified methods.
/// It includes the registration of character definitions and characters as well as management of their subsequent use.
//----------------------------------------------------------------------------------------------------------------------
class CharacterManagerPhysX2 : public CharacterManager
{
public:

  CharacterManagerPhysX2();
  virtual ~CharacterManagerPhysX2();
 
  /// \brief Allocate required memory and initialise.
  virtual void init() NM_OVERRIDE;

  /// \brief Free allocated memory and shutdown.
  virtual void term() NM_OVERRIDE;
  
  /// \brief Create and initialise a CharacterDef; loading its assets a specified simple bundle source file.
  ///
  /// CharacterDefs that are created via the Manager are automatically registered with the manager
  ///  and the memory management of the Character remains the responsibility of the Manager.
  CharacterDef* createCharacterDef(
    const char* filename); ///< Name of simple bundle file where we should try and load this characters assets from.
    
  /// \brief Create CharacterDef and initialise it from the simple bundle assets that have been loaded in to the specified memory block.
  ///
  /// CharacterDefs that are created via the Manager are automatically registered with the manager
  ///  and the memory management of the Character remains the responsibility of the Manager.
  CharacterDef* createCharacterDef(
    void*  bundle,      ///< Block of memory that holds this characters simple bundle assets.
    size_t bundleSize); ///< Memory block size.
 
  /// \brief Create a CharacterPhysX2 and register it with this manager.
  ///
  /// Characters that are created via the Manager are automatically registered with the manager
  ///  and the memory management of the Character remains the responsibility of the Manager.
  /// The manager must already have been attached to a PhysX scene.
  CharacterPhysX2* createCharacter(
    CharacterDef*           characterDef,       // Must have been registered with this manager.
    const NMP::Vector3&     initialPosition = NMP::Vector3::InitZero,
    const NMP::Quat&        initialOrientation = NMP::Quat::kIdentity,
    const NMP::Vector3&     graveyardPosition = NMP::Vector3(0.0f, -1000.0f, 0.0f),
    MR::AnimSetIndex        initialAnimSetIndex = 0,
    const char*             name = "",
    NMP::FastHeapAllocator* temporaryMemoryAllocator = NULL,
    uint32_t                externalIdentifier = 0xFFFFFFFF);

  /// \brief Tell the CharacterManagerPhysX2 which NxScene and NxControllerManager it should use
  ///  when creating and destroying characters.
  void attachToPhysXScene(
    NxScene*             physXScene,                       ///< 
    NxControllerManager* physXCharacterControllerManager); ///< 
  
  /// \brief Destroy our communication interface to the NxScene and our reference to the NxControllerManager.
  void detachFromPhysXScene();

  /// \brief Utility function for spacing inactive physical character controller objects in a graveyard.
  NMP::Vector3 calculateNextCharacterGraveyardPosition(float horizontalSpacingFactor);

  //----------------------------
  // Accessors.
  NM_INLINE MR::PhysicsScenePhysX2* getPhysicsScene() { return m_physicsScene; }
  NM_INLINE NxControllerManager* getPhysXCharacterControllerManager() { return m_physXCharacterControllerManager; }

  //----------------------------
  // Update stages.
  //----------------------------

  // Update order:
  //  1. CharacterManagerPhysX2::updatePreCharacterController
  //  2. CharacterManagerPhysX2::updateCharacterController 
  //  3. CharacterManagerPhysX2::updatePostCharacterController
  //  4. CharacterManagerPhysX2::updatePrePhysics
  //  5. global PhysX simulation update
  //  6. CharacterManagerPhysX2::updatePostPhysics
  //  7. CharacterManagerPhysX2::updateFinalise

  /// \brief Update each characters morpheme Network to the point where we need to update the PhysX character controller.
  ///
  ///  Calls updateToPreCharacterController on all active character instances.
  ///  Builds queues and processes them up to the point where the global character controller update is required.
  void updatePreCharacterController(float deltaTime);

  /// \brief Update each characters PhysX2 character controller.
  ///
  /// Calls the global PhysX character controller update two times as part of updating each
  /// characters controller position.
  ///  If you want to override the Networks suggested delta trajectory replace this function with your own implementation.
  void updateCharacterController(float deltaTime);
  
  /// \brief Execute each characters Network queue up to the point where the global physics update is required.
  void updatePostCharacterController(float deltaTime);
  
  /// \brief Write each characters cached PhysX channel data to its rig in preparation of the physics simulation step.
  ///
  /// Possibly not a thread safe step because we are communicating directly with the physics system.
  void updatePrePhysics();
  
  /// \brief Re-generate each characters physics cache data from the PhysX rig after the physics simulation step
  ///  which should have just taken place.
  ///
  ///  Possibly not a thread safe step because we are communicating directly with the physics system.
  void updatePostPhysics();

  /// \brief Complete each characters network task execution and compute final rig world transforms.
  void updateFinalise(float deltaTime);
  
protected:

  /// Holds a reference to the NxScene that was created externally.
  ///  Provides a communication interface to the NxScene for morpheme.
  MR::PhysicsScenePhysX2* m_physicsScene;

  /// Holds a reference to the NxControllerManager that was created externally.
  ///  Allows us to create and destroy physical character controller objects for our characters.
  NxControllerManager*    m_physXCharacterControllerManager;

  /// Value that is incremented every time a new character is created.
  ///  Ensures that the graveyard position for each character is unique.
  int32_t                 m_graveYardSeed;
};

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_MANAGER_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------


