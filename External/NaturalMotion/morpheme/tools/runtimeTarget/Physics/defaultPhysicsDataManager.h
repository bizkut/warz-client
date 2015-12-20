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
#ifndef NM_DEFAULTPHYSICSDATAMANAGER_H
#define NM_DEFAULTPHYSICSDATAMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/physicsRuntimeTargetSimple.h"
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
  public MCOMMS::SimplePhysicsDataManager
{
public:

  DefaultPhysicsDataManager(RuntimeTargetContext* context, DefaultPhysicsMgr* physicsMgr);

  virtual ~DefaultPhysicsDataManager() {}

  /// \brief This custom assignment operator is required as the compiler cannot generate the default assignment operator
  ///  due to const members.
  DefaultPhysicsDataManager & operator= (const DefaultPhysicsDataManager & other) {*this = other; return *this;}

  //--------------------------------------------------------------------------------------------------------------------
  // [DataManagementInterface]
public:

  virtual MR::Network* findNetworkByInstanceID(MCOMMS::InstanceID id) const NM_OVERRIDE;
  virtual MR::NetworkDef* findNetworkDefByGuid(const MCOMMS::GUID& networkGUID) const NM_OVERRIDE;
  virtual MR::PhysicsObjectID findPhysicsObjectIDForPart(const MR::PhysicsRig::Part* part) const NM_OVERRIDE;

  virtual void getPhysicsEngineID(char* buffer, uint32_t bufferLength) const NM_OVERRIDE;

  virtual void onEnvironmentVariableChange(const MCOMMS::Attribute* attribute) NM_OVERRIDE;

protected:
  DefaultPhysicsMgr*                   m_physicsManager;
  NetworkDefRecordManager* const       m_networkDefManager;
  NetworkInstanceRecordManager* const  m_networkInstanceManager;
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_DEFAULTDATAMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
