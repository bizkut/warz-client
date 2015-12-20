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
#ifndef GAME_WORLD_PHYSX2_H
#define GAME_WORLD_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameCharacterDef.h"
#include "GameCharacterPhysX2.h"

#include "NMPlatform/NMPlatform.h"
#include "simpleBundle/simpleAnimRuntimeIDtoFilenameLookup.h"
//----------------------------------------------------------------------------------------------------------------------

#ifdef _WIN64
#define PLATFORMDIR "x64"
#else
#define PLATFORMDIR "win32"
#endif

namespace Game
{


//----------------------------------------------------------------------------------------------------------------------
// The Game::CharacterManagerPhysX2Basic class is an example of how you could manage the morpheme animation system within 
// your application. It would hold responsibility for managing all the functions required to run a morpheme network by 
// wrapping many of morpheme api calls into conceptually simplified methods.
class CharacterManagerPhysX2Basic
{
public:

  CharacterManagerPhysX2Basic() {};
  virtual ~CharacterManagerPhysX2Basic()  {};

  //----------------------------
  // Allocate required memory and initialise
  void initMorpheme();

  //----------------------------
  // Unload any data stored in this CharacterManager, including CharacterDef that have been loaded and unload the memory
  // they store.
  void termMorpheme();

  //----------------------------
  // Create and initialise a CharacterDef; loading its assets a specified simple bundle source file.
  //
  // CharacterDefs that are created via the Manager are automatically registered with the manager
  //  and the memory management of the Character remains the responsibility of the Manager.
  CharacterDefBasic* createCharacterDef(const char* filename );  //< Name of simple bundle file where we should try and load this characters assets from.

  //----------------------------
  // Stores a pointer to a characterDef. This allows the CharacterManager to manage this character definition.
  void registerCharacterDef(CharacterDefBasic* characterDef);

  //----------------------------
  // Stores a pointer to a Character. This allows the CharacterManager to manage this Character.
  void registerCharacter(CharacterPhysX2Basic* characterDef);

  //----------------------------
  // Create a morpheme PhysicsScenePhysX
  void attachToPhysXScene(NxScene* nxScene);

  //----------------------------
  // Simple update functions...
  //----------------------------
  // For a more complete implementation see update functions in Game Management libraries CharacterManagerPhysX2.

  //----------------------------
  // Update step 1: Builds queues and processes them up to the point where the global character controller update is required.
  //  Then updates the character controller - including the global physx character controller update.
  void updateCharacterController(float deltaTime);

  //----------------------------
  // Update step 2: Updates the network to the point where a Physics update is required.
  void updatePostCharacterController();

  //----------------------------
  // Update step 3: Write our cached PhysX channel data to our rig in preparation of the physics step.
  void updatePrePhysics();

  //----------------------------
  // Update step 4: Re-generate our physics cache data from the PhysX rig after the physics step which should have just taken place.
  void updatePostPhysics();

  //----------------------------
  // Update step 5: For physics update loops.
  //  Calls the final update step on all the character instance.
  //  To be called post the physics simulation.
  //  Does any remaining network execution, does final amendments to the character controller position
  //   and computes world transforms.
  void updateFinalise(float deltaTime);

  //----------------------------
  // Accessors.
  MR::PhysicsScenePhysX2* getPhysicsScene() { return m_physicsScene; }

protected:

  CharacterDefBasic*      m_characterDef;   // Store a pointer to a morpheme character definition
  CharacterPhysX2Basic*   m_characterData;  // Store a pointer to a morpheme character

  MR::PhysicsScenePhysX2*  m_physicsScene;  /// Store a pointer to the NxScene which provides a communication interface for morpheme.
};


} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_WORLD_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
