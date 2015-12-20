// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.  
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

// ---------------------------------------------------------------------------------------------------------------------

#include "scriptedRuntimeApp.h"
#include "srScripting.h"
#include "srScene.h"
#include "srEnvironment.h"

static char g_currentDir[MAX_PATH_LENGTH] = "";

// ---------------------------------------------------------------------------------------------------------------------
void setCurrentDirectory(const char *path)
{
  strncpy_s(g_currentDir, path, MAX_PATH_LENGTH);
}

// ---------------------------------------------------------------------------------------------------------------------
void getFullPath(char *fullPath, size_t size, const char *filename)
{
  sprintf_s(fullPath, size, "%s\\%s", g_currentDir, filename);
}

// ---------------------------------------------------------------------------------------------------------------------
MR::AnimSourceBase *loadAnimation(const MR::RuntimeAnimAssetID animAssetID, void *userData)
{
  MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup *fileNameLookup = (MR::UTILS::SimpleAnimRuntimeIDtoFilenameLookup*)userData;

  // Get the animation file name
  const char *filename = fileNameLookup->getFilename(animAssetID);

  MR::AnimSourceBase *animSource = 0;
  static char fullPath[MAX_PATH_LENGTH];
  getFullPath(fullPath, MAX_PATH_LENGTH, filename);
  int64_t fileSize = 0;
#ifdef NMP_ENABLE_ASSERTS
  int64_t bytesRead = 
#endif // NMP_ENABLE_ASSERTS
    NMP::NMFile::allocAndLoad(fullPath, &(void*&)animSource, &fileSize, NMP_VECTOR_ALIGNMENT);

  NMP_ASSERT(bytesRead > 0);

  return animSource;
}

// ---------------------------------------------------------------------------------------------------------------------
void initMorphemeAndEuphoria()
{
  //----------------------------
  // Initialise morpheme library
  MR::Manager::initMorphemeLib();
  MR::Manager::getInstance().setAnimFileHandlingFunctions(&loadAnimation, NULL);

  // Dispatcher initialisation
  MR::Dispatcher* dispatcher = MR::DispatcherBasic::createAndInit();

  // Engine specific assets
  MR::Manager::getInstance().registerAsset(MR::Manager::kAsset_PhysicsRigDef, MR::locatePhysicsRigDefPhysX3);

  // Initialise the physics core by registering the task queuing functions,
  // attribute data and the task functions with the dispatchers.
  MR::initMorphemePhysics(1, &dispatcher);
  ER::initEuphoria(1, &dispatcher);

  // Finalize semantic and core task registration
  MR::Manager::getInstance().allocateRegistry();
  MR::CoreTaskIDs::registerNMCoreTasks(dispatcher);

  MR::Manager::getInstance().finaliseInitMorphemeLib();
  MR::finaliseInitMorphemePhysics();
  ER::finaliseInitEuphoria();

  // Tidy up the dispatcher instance since we have finished initialising the static data
  dispatcher->releaseAndDestroy();
}


// ---------------------------------------------------------------------------------------------------------------------
int main(int argc, const char *argv[])
{
  // we need to try and take priority over anything else that might be running
  SetPriorityClass( GetCurrentProcess(), HIGH_PRIORITY_CLASS );

  // fire up system services
  NMP::Memory::init();
  {
    // process inputs, create local path data
    Environment env(argc, argv);

    NMP::NetworkInitialize();

    // run the scenario
    ScriptedScenario scn(&env);
    if (!scn.run())
    {
      printf("Failed to successfully execute squirrel script!\n");
      return -1;
    }

    NMP::NetworkShutdown();
  }
  NMP::Memory::shutdown();

  return ScriptedScenario::m_returnValue;
}
