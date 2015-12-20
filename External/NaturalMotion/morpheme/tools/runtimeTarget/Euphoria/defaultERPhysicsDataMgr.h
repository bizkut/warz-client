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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_DEFAULTERPHYSICSDATAMGR_H
#define NM_DEFAULTERPHYSICSDATAMGR_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/euphoriaRuntimeTargetSimple.h"
//----------------------------------------------------------------------------------------------------------------------

class NetworkDefRecordManager;
class NetworkInstanceRecordManager;
class RuntimeTargetContext;
class DefaultPhysicsMgr;

//----------------------------------------------------------------------------------------------------------------------
/// \class DefaultDataManager
/// \brief Implements MCOMMS::DataManagementInterface.
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class DefaultPhysicsDataManager :
  public MCOMMS::SimpleEuphoriaPhysicsDataManager
{
public:

  DefaultPhysicsDataManager(RuntimeTargetContext* context, DefaultPhysicsMgr* physicsMgr);

  virtual ~DefaultPhysicsDataManager() {}

  //--------------------------------------------------------------------------------------------------------------------
  // [DataManagementInterface]
public:

  virtual MR::Network* findNetworkByInstanceID(MCOMMS::InstanceID id) const NM_OVERRIDE;
  virtual MR::NetworkDef* findNetworkDefByGuid(const MCOMMS::GUID& networkGUID) const NM_OVERRIDE;
  virtual MR::PhysicsObjectID findPhysicsObjectIDForPart(const MR::PhysicsRig::Part* part) const NM_OVERRIDE;

  virtual void getPhysicsEngineID(char* buffer, uint32_t bufferLength) const NM_OVERRIDE;

  /// \brief If the asset scale changes this will reset the PhysX3 SDK with a new scale tolerance.
  virtual void onEnvironmentVariableChange(const MCOMMS::Attribute* attribute) NM_OVERRIDE;

protected:
  DefaultPhysicsMgr*                   m_physicsManager;
  NetworkDefRecordManager* const       m_networkDefManager;
  NetworkInstanceRecordManager* const  m_networkInstanceManager;

private:
  // disabled assignment operator to disable warning C4512
  const DefaultPhysicsDataManager& operator = (const DefaultPhysicsDataManager& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_DEFAULTDATAMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
