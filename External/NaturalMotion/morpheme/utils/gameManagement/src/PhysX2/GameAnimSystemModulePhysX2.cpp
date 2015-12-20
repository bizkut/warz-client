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
#include "gameManagement/GameAnimLoader.h"
#include "GameManagement/PhysX2/GameAnimSystemModulePhysX2.h"

#include "physics/Nodes/mrPhysicsNodes.h"
#include "physics/PhysX2/mrPhysX2.h"
#include "physics/PhysX2/mrPhysicsRigPhysX2.h"

#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/mrDispatcher.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

//----------------------------------------------------------------------------------------------------------------------
void AnimSystemModulePhysX2::init(
#ifdef NM_HOST_CELL_PPU
  uint32_t           numSpus, // = NMP::SPUManager::MAX_SPUS,
  CellSpursTaskset2* spursTaskset,
  CellSpurs*         spursObject,
  bool               initSPUPrintf
#endif // NM_HOST_CELL_PPU
  )
{
  // Only startup if we haven't already been initialised.
  if (!m_initialised)
  {
    // Initialise the morpheme memory system.
    NMP_ASSERT(!NMP::Memory::isInitialised());
    NMP::Memory::init();

    // Initialise morpheme library
    //  Registers all core node types, queuing functions, tasks, conditions, formats, prediction model types and attrib data types.
    MR::Manager::initMorphemeLib();

    // Provide morpheme with animation handling function.
    MR::Manager::getInstance().setAnimFileHandlingFunctions(AnimLoader::requestAnim, AnimLoader::releaseAnim);

#ifdef NM_HOST_CELL_PPU
    // Init Spurs if the app hasn't already done it
    if (!NMP::SPUManager::initialised())
    {
      // If we haven't already initialised the dispatcher, we do it here with MAX_SPUS
      NMP::SPUManager::init(numSpus, spursTaskset, spursObject, initSPUPrintf);
    }
#endif // NM_HOST_CELL_PPU
    
    // Create one of each type of dispatcher used at runtime.
    uint32_t numDispatchers = 0;
    MR::Dispatcher* dispatchers[2];
    dispatchers[numDispatchers++] = MR::DispatcherBasic::createAndInit(); // Common use basic dispatcher.

#if defined(NM_HOST_CELL_PPU) && defined(NM_USE_SPU_DISPATCHER_ON_PS3)
    dispatchers[numDispatchers++] = MR::DispatcherPS3::createAndInit();   // PS3 specific dispatcher 
#endif // NM_HOST_CELL_PPU

    // Register PhysX2 specific physics rig asset type with the manager.
    MR::Manager::getInstance().registerAsset(MR::Manager::kAsset_PhysicsRigDef, MR::locatePhysicsRigDefPhysX2);

    // Initialise the physics core.
    //  Registers the task queuing functions, attribute data and the task functions with the dispatchers.
    MR::initMorphemePhysics(numDispatchers, dispatchers);

    // Allocate Manager registry arrays (now that we have told it about which semantics and tasks we want to register).
    MR::Manager::getInstance().allocateRegistry();

    // Register all core and plugin tasks with all dispatchers (initialises static data in the dispatchers).
    for (uint32_t i = 0; i < numDispatchers; ++i)
      MR::CoreTaskIDs::registerNMCoreTasks(dispatchers[i]);

    // Tidy up the temporary dispatcher instances since we have finished initialising the static data.
    for (uint32_t i = 0; i < numDispatchers; ++i)
      dispatchers[i]->releaseAndDestroy();

    // Finalise semantic and core task registration.
    MR::Manager::getInstance().finaliseInitMorphemeLib();
    MR::finaliseInitMorphemePhysics();

    m_initialised = true;
  }
}

} // namespace Game

//----------------------------------------------------------------------------------------------------------------------
