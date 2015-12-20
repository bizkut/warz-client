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
#ifndef GAME_CHARACTER_PHYSX2_H
#define GAME_CHARACTER_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------
#include "GameCharacterControllerPhysX2.h"

#include "morpheme/mrDispatcher.h"
#include "GameCharacterDef.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
// Game::CharacterPhysX2 represents an instance of a Game::CharacterDef.  This class expands on Game::Character by adding 
// the functionality required by a PhysX2 morpheme network. Essentially the class wraps up the instantiated morpheme
// network and the information required to update and run an individual character. In a game scenario a Character class
// could also store data for a character instance (current health/ammo etc).
//----------------------------------------------------------------------------------------------------------------------
class CharacterPhysX2Basic
{
public: 
  
  ~CharacterPhysX2Basic() {};

  //----------------------------
  // Create an instance of a PhysX2 morpheme network (Game::CharacterPhysX2)
  static CharacterPhysX2Basic* create(
    Game::CharacterDefBasic* gameCharacterDef,
    MR::PhysicsScenePhysX2* physicsScene,
    NxControllerManager* controllerManager);

  //----------------------------
  // Release an instance of Character
  static void destroy(CharacterPhysX2Basic* character);

  //----------------------------
  // Update the world transforms from the network transforms. Root position and root rotation are just taken from the
  // character root bone here as we have an empty character controller.
  void updateWorldTransforms(const NMP::Matrix34& rootTransform);

  //----------------------------
  // Runs an update step to initialise the network.
  // The initial update with a delta time of zero will initialize active connections etc. It is important that the
  // initial active animation set is set before the first update so the resulting transform output matches the active
  // rig. This should be called exactly once after the network was created.
  void runInitialisingUpdateStep();

  //----------------------------
  // Update stages.
  //----------------------------
  // What follows is a basic implementation of the update stages required to update a PhysX2 character.
  // For a more detailed implementation see the update functions of Game::CharacterPhysx2 in the Game Management library.
  //----------------------------

  //----------------------------
  // Update step 1: Update morpheme Network to the point where we need to update the PhysX character controller.
  void updatePreCharacterController(float deltaTime);

  //----------------------------
  // Update step 2: Update morpheme Network, the character controller and the PhysX character controller.
  void updateCharacterController(float deltaTime);

  //----------------------------
  // Update step 3: Called after the actual PhysX character controllers have been updated.  This call updates
  //  the network to the point where the global physics simulation step is required.
  void updatePostCharacterController();

  //----------------------------
  // Update step 4: Write our cached PhysX channel data to our rig in preparation of the physics step.
  //  Not a thread safe step.
  void updatePrePhysics();

  //----------------------------
  // Update step 5: Re-generate our physics cache data from the PhysX rig after the physics step which should have just taken place.
  //  Not a thread safe step.
  void updatePostPhysics();

  //----------------------------
  // Update step 6: The final update stage, to be called after physics simulation has been updated.
  //  + Execute any remaining tasks on the queue.
  //  + If the character root is being updated via physics e.g. when its in rag doll,
  //    then update the character controller position from the physics rigs position.   
  //  + Compute the characters world transforms.
  //  Note: This function is not required to call updateFinalise in base class.
  //  Thread safe step.
  void updateFinalise();

  //----------------------------
  // Accessors
  MR::Network*      getNetwork() const { return m_net; }
  NMP::DataBuffer*    getWorldTransforms() const { return m_worldTransforms; }

  CharacterControllerPhysX2Basic* getCharacterController()   { return static_cast<CharacterControllerPhysX2Basic*>(m_net->getCharacterController()); }

protected:

  CharacterPhysX2Basic(CharacterDefBasic* gameCharacterDef):
    m_characterDef(gameCharacterDef),
    m_net(NULL),
    m_worldTransforms(NULL),
    m_temporaryMemoryAllocator(NULL),
    m_persistentMemoryAllocator(NULL),
    m_physicsScene(NULL)
  {
  };

  //----------------------------
  // Initialise an instance of a morpheme network (Game::Character)
  bool init(CharacterDefBasic* characterDef);

  //----------------------------
  // Release any memory allocated by this class
  bool term();

protected:

  CharacterDefBasic*   m_characterDef;      // The character definition that this class is an instance of
  MR::Network*         m_net;               // The network instance for this character.

  NMP::DataBuffer* m_worldTransforms; // The transforms buffer (pose) of the character.

  NMP::FastHeapAllocator*   m_temporaryMemoryAllocator;
  NMP::MemoryAllocator*     m_persistentMemoryAllocator;

  MR::PhysicsScenePhysX2* m_physicsScene;

};


} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
#endif // GAME_CHARACTER_PHYSX2_H
//----------------------------------------------------------------------------------------------------------------------

