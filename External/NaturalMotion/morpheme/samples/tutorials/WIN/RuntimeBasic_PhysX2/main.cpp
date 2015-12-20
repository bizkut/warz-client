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
#include "GameCharacterManagerPhysX2.h"
#include "GameCharacterPhysX2.h"
#include "GameCharacterDef.h"
#include "GameCharacterControllerPhysX2.h"

#define ASSETLOCATION "./ProjectData/" PLATFORMDIR
#define NETWORK_DEF_BUNDLE_NAME ASSETLOCATION "/Network_RuntimeBasic_PhysX.nmb"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Initialise PhysX libraries
void initPhysics()
{
  NMP_STDOUT("Initialising PhysX SDK");

  //----------------------------
  // Initialize PhysX sdk
  NxPhysicsSDKDesc sdkDesc;
  sdkDesc.flags |= NX_SDKF_NO_HARDWARE;
  NxPhysicsSDK* physicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, 0, 0, sdkDesc);

  NMP_ASSERT_MSG(physicsSDK, "Unable to create the NVIDIA PhysX SDK. "
                 "The NVIDIA PhysX System software is required to run this app.");

  //----------------------------
  // Connect to the remote debugger, if there is one.
  if (!physicsSDK->getFoundationSDK().getRemoteDebugger()->isConnected())
  {
    physicsSDK->getFoundationSDK().getRemoteDebugger()->connect("localhost", 5425);
  }

  //----------------------------
  // Set some default values on this instance of the SDK.
  physicsSDK->setParameter(NX_SKIN_WIDTH, 0.001f);
  physicsSDK->setParameter(NX_VISUALIZATION_SCALE, 0.25f);
  physicsSDK->setParameter(NX_ADAPTIVE_FORCE, 0);
  physicsSDK->setParameter(NX_IMPROVED_SPRING_SOLVER, 1);
}

//----------------------------------------------------------------------------------------------------------------------
// Destroy the physics scene and shutdown PhysX
void termPhysics(NxScene* physXScene)
{
  NxPhysicsSDK* physXSDK = NxGetPhysicsSDK();

  if (physXScene)
  {
    physXSDK->releaseScene(*physXScene);
    physXScene = NULL;
  }
  
  if (physXSDK)
  {
    NxReleasePhysicsSDK(physXSDK);
    NMP_ASSERT(!NxGetPhysicsSDK());
    physXSDK = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Create a PhysX scene
NxScene* CreatePhysXScene()
{
  //----------------------------
  // Initialize the scene description.
  NxSceneDesc sceneDesc;
  sceneDesc.gravity         = NxVec3(0.0f, -9.81f, 0.0f);
  sceneDesc.timeStepMethod  = NX_TIMESTEP_FIXED;
  sceneDesc.maxTimestep     = 1.0f / 240.0f;
  sceneDesc.maxIter         = 300;

  //----------------------------------
  // Create the scene.
  NxScene* nxScene = NxGetPhysicsSDK()->createScene(sceneDesc);
  NMP_ASSERT(nxScene);
  if (nxScene)
  {
    //----------------------------------
    // Initialise the default material for the scene.
    //  We use this as a basis for creating materials for actors in the scene.
    NxMaterial* material = nxScene->getMaterialFromIndex(0);
    material->setDynamicFriction(1);
    material->setDynamicFriction(1);
    material->setRestitution(1);
  }

  return nxScene;
}

//----------------------------------------------------------------------------------------------------------------------
// Create a physics environment for our character to interact with
void createPhysicsEnvironment(NxScene* physXScene)
{
  //----------------------------
  // Add a box for the ground.
  NxBoxShapeDesc groundBoxDesc;
  groundBoxDesc.dimensions = NxVec3(100, 1, 100);
  groundBoxDesc.group =  (NxCollisionGroup)MR::GROUP_COLLIDABLE_NON_PUSHABLE;

  NxBodyDesc groundBodyDesc;
  groundBodyDesc.mass = 10;
  groundBodyDesc.flags |= NX_BF_KINEMATIC;

  NxActorDesc groundActorDesc;
  groundActorDesc.body = &groundBodyDesc;
  groundActorDesc.shapes.push_back(&groundBoxDesc);

  NxActor* groundActor = physXScene->createActor(groundActorDesc);
  groundActor->setGlobalPosition(NxVec3(0, -1, 0));
}

//----------------------------------------------------------------------------------------------------------------------
// Update the physics simulation
void simulate(NxScene* physXScene, const float timeDelta)
{
  static const float maxTimeStep  = 1.0f / 240.0f;

  // Simulate physics world using a stepping scheme similar to the one used in the runtime target.
  int nSteps = 1 + (int)(timeDelta / maxTimeStep);
  NxReal timeStep = timeDelta / nSteps;

  physXScene->setTiming(timeStep, nSteps, NX_TIMESTEP_FIXED);
  physXScene->simulate(timeDelta);
  physXScene->flushStream();
  physXScene->fetchResults(NX_RIGID_BODY_FINISHED, true);
}

//----------------------------------------------------------------------------------------------------------------------
// RuntimeBasic_PhysX: This tutorial adds physics to the RuntimeBasic3 tutorial. A new physics character controller 
// class is added to maintain all the physics based character data. This will allow you to load and run networks
// that have been exported and contain physics nodes. Physical objects are also created in the scene for
// the character to stand on.
int main(int NMP_UNUSED(argc), char** NMP_UNUSED(argv))
{
  //----------------------------
  // This program defaults to loading the "Network" from the ProjectData folder. The animation files will 
  // also be loaded from this location; see Game::CharacterDef::loadAnimationsFromDisk() for more details.
  NMP_STDOUT("Defaulting <network filename> to : '%s'", NETWORK_DEF_BUNDLE_NAME);

  //----------------------------
  // Initialise the PhysX libraries
  NMP_STDOUT("\nInitialising Physics:");
  initPhysics();

  //----------------------------
  // Create a NxScene pointer used to initialise a physics environment
  NMP_STDOUT("Creating MR::PhysicsScenePhysX object");
  NxScene* nxScene = CreatePhysXScene();

  //----------------------------
  // Create a physics environment for our character to interact with
  NMP_STDOUT("Creating Physics environment");
  createPhysicsEnvironment(nxScene);

  //----------------------------
  // Create a physics controller manager. used by the GameCharacters character controller
  NMP_STDOUT("Creating PhysX Controller Manager");
  NxControllerManager* controllerManager = NxCreateControllerManager(NxGetPhysicsSDKAllocator());

  NMP_STDOUT("Physics initialisation complete");

  //----------------------------
  // Game::CharacterManagerNoPhysics which will act as a manager for all the morpheme network definitions 
  // and network instances. Initialise will set up the morpheme libraries and load all the default node types
  // and other default functions.
  Game::CharacterManagerPhysX2Basic characterManager;

  //---------------------------
  // Initialize morpheme.
  characterManager.initMorpheme();
  characterManager.attachToPhysXScene(nxScene);

  //----------------------------
  // We also need an instance of a Game::CharacterDef that will be initialised with the binary asset data. Using the
  // Game::AnimModule manager class we will initialise our Game::CharacterDef. This will load the file found at 
  // ASSETLOCATION and use that information to apply to the character definition instance.
  NMP_STDOUT("\nCreating Game Character Definition 1:"); 
  Game::CharacterDefBasic* gameCharacterDef = characterManager.createCharacterDef(NETWORK_DEF_BUNDLE_NAME);
  if(!gameCharacterDef)
  {
    NMP_STDOUT("\nError: Failed to create Game Character Definition 1");
    return NULL;
  }

  //----------------------------
  // Load the animations listed in the GameCharacterDef loaded from the exported network asset file. We will just
  // load the tutorials into memory in this tutorial.
  NMP_STDOUT("\nLoading Animation files:");
  if (!gameCharacterDef->loadAnimations())
  {
    NMP_STDOUT("\nload Animations for Game Character Definition returned error");
    return NULL;
  }

  //----------------------------
  // Once we have a valid character definition we can print out information about the animation rig such as the number
  // of bones and bone names. Now that we have loaded all the data within the asset file we can also print out the
  // number of nodes in the network and other information such as the number of state machines.
  if (gameCharacterDef->isLoaded())
  {
    NMP_STDOUT("\nPrinting data from GameCharacterDef:");
    NMP_STDOUT("Number of Node Definitions: %d", gameCharacterDef->getNetworkDef()->getNumNodeDefs());
    NMP_STDOUT("Number of State Machines: %d", gameCharacterDef->getNetworkDef()->getNumStateMachines());
    NMP_STDOUT("Number of Control Parameters: %d", gameCharacterDef->getNetworkDef()->getNumControlParameterNodes());
    NMP_STDOUT("\nList of bones in rig:");

    const MR::AnimRigDef* rig = gameCharacterDef->getNetworkDef()->getRig(0);
    for (uint32_t i = 0; i < rig->getNumBones(); i++)
    {
      NMP_STDOUT("%s", rig->getBoneName(i));
    }
  }

  //----------------------------
  // We need an instance of a Game::Character class that represents and wraps the runtime instance of a CharacterDef
  // class. We initialise the Character instance data using the static Character Create function.
  // This class contains the runtime network instance for a character that can be updated and run. Each character in a
  // game would have a Character class instance for updating its unique instance of a character definition.
  NMP_STDOUT("\nCreating Character from CharacterDef:");
  Game::CharacterPhysX2Basic* gameCharacter = Game::CharacterPhysX2Basic::create(
                                                                            gameCharacterDef,
                                                                            characterManager.getPhysicsScene(),
                                                                            controllerManager);

  if(!gameCharacter)
  {
    NMP_STDOUT("\nError: Failed to create Game Character 1");
    return NULL;
  }

  //----------------------------
  // Hand off management of the gameCharacter we just created to the Game::WorldPhysX2.
  characterManager.registerCharacter(gameCharacter);

  NMP_STDOUT("\nPerforming Updates:");

  //----------------------------
  // Our time difference between updates
  static const float deltaTime = 1.0f / 60.0f;

  //----------------------------
  // Update i number of times to test the network update. We also need to update the physics scene as well
  for (int i = 0; i <= 1000; ++i)
  {
    //----------------------------
    // Morpheme updates work on a taskQueue based system. This queue is built at the beginning of each update
    // with the Network::startUpdate() method.  Tasks need to be updated in a specific order as some
    // tasks are reliant on other tasks being complete or certain data being available. The prePhysicsUpdate()
    // function will update the character controller and then update any non physics tasks. physicsUpdate()
    // is then called to update the Physics simulation before running the post physics tasks.
    // postPhysicsUpdate() then calls all the remaining tasks that are dependent on the physics simulation.
    // See the following section of the user guide for more details:
    // Understanding Morpheme > Networks > Runtime handling > Tasks

    //----------------------------
    // Pre Physics update for the gameCharacter instance
    characterManager.updateCharacterController(deltaTime);
    characterManager.updatePostCharacterController();

    //----------------------------
    // Write our cached values to the physics rig
    characterManager.updatePrePhysics();

    //----------------------------
    // Update the physics simulation
    simulate(nxScene, deltaTime);

    //----------------------------
    // Regenerate our cached physics rig values post physics update
    characterManager.updatePostPhysics();

    //----------------------------
    // Post physics updates for the gameCharacter instance
    characterManager.updateFinalise(deltaTime);
  }

  NMP_STDOUT("Updates Complete");

  //----------------------------
  // When we are ready to exit we can terminate the Game::WorldPhysX2 to free any classes/memory it has stored.
  // Since Game::WorldPhysX2 stores a pointer to gameCharacterDef and gameCharacter instances it will
  // manage their termination as well.
  NMP_STDOUT("\nReleasing data:");
  characterManager.termMorpheme();
  NMP_STDOUT("Release complete");

  //----------------------------
  // Finally shut down PhysX.
  termPhysics(nxScene);

  NMP_STDOUT("\nPress Enter to Exit");
  getchar();

  return 0;
}
//----------------------------------------------------------------------------------------------------------------------
