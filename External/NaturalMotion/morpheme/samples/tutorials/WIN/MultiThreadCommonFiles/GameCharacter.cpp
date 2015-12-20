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
#include "GameCharacter.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
GameCharacterBase::GameCharacterBase(
  const char*                   name,
  GameAnimNetwork*              network,
  GameCharacterControllerBase*  characterController) :
  m_name(name),
  m_network(network),
  m_characterController(characterController)
{
  NMP_ASSERT(name && (name[0] != '\0') && network && characterController);
}

//----------------------------------------------------------------------------------------------------------------------
GameCharacterBase::~GameCharacterBase()
{
  delete m_characterController;
  delete m_network;
}

//----------------------------------------------------------------------------------------------------------------------
GameCharacter* GameCharacter::create(
  const char*                  name,
  const NMP::Vector3&          initialTranslation,
  const NMP::Quat&             initialOrientation,
  GameAnimNetworkDef*          networkDef,
  MR::AnimSetIndex             initialAnimSetIndex,
  NMP::FastHeapAllocator*      tempAllocator)
{
  if (!name || (name[0] == '\0') || !networkDef)
  {
    NMP_DEBUG_MSG("error: Valid name and network definition expected!");
    return NULL;
  }

  GameCharacterController* gameCharacterController = GameCharacterController::create(initialTranslation, initialOrientation);
  if (!gameCharacterController)
  {
    return NULL;
  }

  // Create a network instance.
  GameAnimNetwork* network = GameAnimNetwork::create(networkDef, initialAnimSetIndex, gameCharacterController, tempAllocator);
  if (!network)
  {
    return NULL;
  }

  // init
  gameCharacterController->init(network->getNetwork());

  GameCharacter* const instance = new GameCharacter(name, network, gameCharacterController);
  network->runInitializingUpdateStep();

  // ...
  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void GameCharacter::update(float deltaTime)
{
  MR::Network* const network = getNetwork()->getNetwork();

  //--------------------------------------------------------------------------------------------------------------------
  // Run animation only network update.
  {
    network->startUpdate(deltaTime);

    MR::Task* task = NULL;
    MR::ExecuteResult execResult = MR::EXECUTE_RESULT_IN_PROGRESS;

    // Keep in mind this update is designed to work only with no-physics networks and if you are looking for a full physics
    // compatible update loop please consult the physics specific tutorials. The most notable difference in how the update
    // is different is that for physics network a pre-physics and post-physics update is required which runs before and
    // after the physics simulation update. The character controller update in physics networks is also interleaved with the
    // morpheme update while for animation-only network it can safely be updated after the morpheme update has completed.
    while (execResult != MR::EXECUTE_RESULT_COMPLETE)
    {
      execResult = network->update(task);
    }

    GameCharacterController* cc = static_cast<GameCharacterController*>(getCharacterController());
    cc->updateController(getNetwork()->getTranslationChange(), getNetwork()->getOrientationChange());

    network->getTempMemoryAllocator()->reset();
  }

  //--------------------------------------------------------------------------------------------------------------------
  // Update world-space output.
  getNetwork()->updatePose(network->getCharacterPropertiesWorldRootTransform());

#ifdef THIS_IS_WHERE_YOU_WOULD_MAP_THE_OUTPUT_TO_THE_GAME_CHARACTERS_SKELETON
  const MR::AnimRigDef* animRig = NULL;
  const NMP::DataBuffer* const pose = getNetwork()->getPose(animRig);
#endif // THIS_IS_WHERE_YOU_WOULD_MAP_THE_OUTPUT_TO_THE_GAME_CHARACTERS_SKELETON
}
