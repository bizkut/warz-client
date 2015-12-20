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
#include "GameAnimModule.h"
#include "GameAnimLoader.h"
//----------------------------------------------------------------------------------------------------------------------

#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/mrDispatcher.h"
#include "morpheme/mrManager.h"
#include "morpheme/AnimSource/mrAnimSourceNSA.h"
#include "morpheme/AnimSource/mrAnimSourceQSA.h"
#include "morpheme/Nodes/mrNodeAnimSyncEvents.h"

//----------------------------------------------------------------------------------------------------------------------

namespace {

template<typename T>
void locateDefaultAnimFormatFn(MR::AnimSourceBase* animSource)
{
  NMP_ASSERT(animSource);
  T* const animFormat = static_cast<T*>(animSource);
  animFormat->locate();
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
void dislocateDefaultAnimFormatFn(MR::AnimSourceBase* animSource)
{
  NMP_ASSERT(animSource);
  T* const animFormat = static_cast<T*>(animSource);
  animFormat->dislocate();
}

} // anonymous namespace

//----------------------------------------------------------------------------------------------------------------------
void GameAnimModule::initMorpheme(const InitModuleFn initModule /* = NULL */)
{
  NMP::Memory::init();

  //----------------------------
  // Initialise morpheme library
  MR::Manager::initMorphemeLib();
  
  MR::Manager::getInstance().setAnimFileHandlingFunctions(
    GameAnimLoader::requestAnim,
    GameAnimLoader::releaseAnim);

  // Create one of each type of dispatchers used at runtime and register the core and plugin tasks with them.
  uint32_t numDispatchers = 0;
  MR::Dispatcher* dispatchers[2];

  dispatchers[numDispatchers++] = MR::DispatcherBasic::createAndInit();

#if defined(NM_HOST_CELL_PPU)
  dispatchers[numDispatchers++] = MR::DispatcherPS3::createAndInit();
#endif // NM_HOST_CELL_PPU

  if (initModule)
  {
    // initialise additional modules, if required.
    initModule(dispatchers, numDispatchers);
  }

  // Finalize semantic and core task registration
  MR::Manager::getInstance().allocateRegistry();
 
  for (uint32_t i = 0; i < numDispatchers; ++i)
  {
    MR::CoreTaskIDs::registerNMCoreTasks(dispatchers[i]);

    // Tidy up the dispatcher instance since we have finished initialising the static data
    dispatchers[i]->releaseAndDestroy();
    dispatchers[i] = NULL;
  }

  MR::Manager::getInstance().finaliseInitMorphemeLib();
}


//----------------------------------------------------------------------------------------------------------------------
void GameAnimModule::termMorpheme(const TermtModuleFn termModule /* = NULL */)
{
  if (termModule)
  {
    termModule();
  }

#if defined(NM_HOST_CELL_PPU)
  NMP::SPUManager::term();
#endif

  MR::DispatcherBasic::term();
#ifdef NM_HOST_CELL_PPU
  MR::DispatcherPS3::term();
#endif // NM_HOST_CELL_PPU

  MR::Manager::termMorphemeLib();
  NMP::Memory::shutdown();
}
