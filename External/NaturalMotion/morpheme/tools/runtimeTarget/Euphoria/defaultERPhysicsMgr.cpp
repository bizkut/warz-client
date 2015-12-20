// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.
//----------------------------------------------------------------------------------------------------------------------
#include "defaultERPhysicsMgr.h"
#include "NMPlatform/NMProfiler.h"
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"
#include "comms/mcomms.h"
#include "comms/packet.h"
#include "iPhysicsMgr.h"
#include "iControllerMgr.h"
#include "mrPhysX3.h"
#include "physics/mrPhysicsRigDef.h"
#include "mrPhysicsRigPhysX3Articulation.h"
#include "mrPhysicsScenePhysX3.h"
#include "morpheme/mrCoreTaskIDs.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsRigDef.h"
#include "../../defaultSceneObjectMgr.h"
#include "defaultControllerMgr.h"

#include "morpheme/mrManager.h"
#include "physics/mrPhysics.h"
#include "euphoria/Nodes/erNodes.h"
#include "euphoria/erAttribData.h"
#include "euphoria/erEuphoriaUserData.h"
#include "euphoria/erContactFeedback.h"

#include "erLiveLinkData.h"

#include "morpheme/mrDispatcher.h"
#ifdef NM_HOST_CELL_PPU
  #include "morpheme/mrDispatcherPS3.h"
  #include "euphoria/erSPU.h"
#endif // NM_HOST_CELL_PPU

//----------------------------------------------------------------------------------------------------------------------
// Default ER physics manager
//----------------------------------------------------------------------------------------------------------------------
DefaultERPhysicsMgr::~DefaultERPhysicsMgr()
{
  ER::destroyEuphoria();
  MR::PhysXPerShapeData::destroyMap();
}


//----------------------------------------------------------------------------------------------------------------------
void DefaultERPhysicsMgr::initializePhysicsCore(uint32_t numDispatchers, MR::Dispatcher** dispatchers)
{
  DefaultPhysicsMgr::initializePhysicsCore(numDispatchers, dispatchers);
  ER::initEuphoria(numDispatchers, dispatchers);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultERPhysicsMgr::finaliseInitPhysicsCore()
{
  DefaultPhysicsMgr::finaliseInitPhysicsCore();
  ER::finaliseInitEuphoria();
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultERPhysicsMgr::updateInteractionProxy(MCOMMS::InstanceID instanceID, float deltaTime)
{
  NetworkInstanceRecord* netInstanceRecord = m_context->getNetworkInstanceManager()->findInstanceRecord(instanceID);
  NMP_ASSERT(netInstanceRecord);
  ERNetworkInstanceRecord* euphoriaInstanceRecord = static_cast<ERNetworkInstanceRecord*>(netInstanceRecord);

  euphoriaInstanceRecord->updateInteractionProxy(deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultERPhysicsMgr::updatePrePhysics(MCOMMS::InstanceID instanceID, float deltaTime, bool NMP_UNUSED(updatePhysicsRig))
{
  NetworkInstanceRecord* netInstanceRecord = m_context->getNetworkInstanceManager()->findInstanceRecord(instanceID);
  NMP_ASSERT(netInstanceRecord);
  ERNetworkInstanceRecord* euphoriaInstanceRecord = static_cast<ERNetworkInstanceRecord*>(netInstanceRecord);

  bool updatePhysicsRig = !euphoriaInstanceRecord->getCharacter();
  DefaultPhysicsMgr::updatePrePhysics(instanceID, deltaTime, updatePhysicsRig);

  euphoriaInstanceRecord->updatePrePhysics(deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultERPhysicsMgr::updatePostPhysics(MCOMMS::InstanceID instanceID, float deltaTime, bool NMP_UNUSED(updatePhysicsRig))
{
  NetworkInstanceRecord* netInstanceRecord = m_context->getNetworkInstanceManager()->findInstanceRecord(instanceID);
  NMP_ASSERT(netInstanceRecord);
  ERNetworkInstanceRecord* euphoriaInstanceRecord = static_cast<ERNetworkInstanceRecord*>(netInstanceRecord);

  // Note that the physics rig has already been updated post-physics in
  // DefaultPhysicsMgr::updatePostPhysicsInit
  euphoriaInstanceRecord->updatePostPhysics(deltaTime);

  // Post physics on the morpheme network.
  DefaultPhysicsMgr::updatePostPhysics(instanceID, deltaTime, false);
}

//----------------------------------------------------------------------------------------------------------------------
