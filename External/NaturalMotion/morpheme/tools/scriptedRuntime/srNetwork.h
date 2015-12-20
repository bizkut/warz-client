#pragma once
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
#include "NMPlatform/NMLoggingMemoryAllocator.h"

// ---------------------------------------------------------------------------------------------------------------------
namespace Network
{

// ---------------------------------------------------------------------------------------------------------------------
MR::NetworkDef *loadDefinition(const char *fileName, size_t& defDataFootprint);

// ---------------------------------------------------------------------------------------------------------------------
MR::Network *createInstance(
  MR::NetworkDef* networkDef,
  MR::InstanceDebugInterface* debugInterface,
  NMP::LoggingMemoryAllocator* persistantAllocator,
  MR::PhysicsScenePhysX3* physicsScene,
  const NMP::Matrix34& initialTM,
  size_t& networkInstFootprint,
  ER::RootModule** euphoriaRootModule);

}
