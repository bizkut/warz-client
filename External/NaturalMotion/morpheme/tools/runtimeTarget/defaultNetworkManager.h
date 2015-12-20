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
#ifndef NM_DEFAULTNETWORKMANAGER_H
#define NM_DEFAULTNETWORKMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/runtimeTargetInterface.h"
//----------------------------------------------------------------------------------------------------------------------

class RuntimeTargetContext;

//----------------------------------------------------------------------------------------------------------------------
class DefaultNetworkManager :
  public MCOMMS::NetworkManagementInterface
{
  // Construction/destruction
public:

  DefaultNetworkManager(RuntimeTargetContext* context);
  virtual ~DefaultNetworkManager();

  //--------------------------------------------------------------------------------------------------------------------
  // [NetworkManagementInterface]
public:

  virtual uint32_t getNetworkDefCount() const NM_OVERRIDE;
  virtual uint32_t getNetworkDefGUIDs(MCOMMS::GUID* guids, uint32_t maxGuids) const NM_OVERRIDE;
  virtual bool isNetworkDefinitionLoaded(const MCOMMS::GUID& guid) const NM_OVERRIDE;

  virtual uint32_t getNetworkInstanceCount() const NM_OVERRIDE;
  virtual uint32_t getNetworkInstanceIDs(MCOMMS::InstanceID* instanceIDs, uint32_t maxInstanceIDs) const NM_OVERRIDE;
  virtual const char* getNetworkInstanceName(MCOMMS::InstanceID id) const NM_OVERRIDE;
  virtual bool getNetworkInstanceDefinitionGUID(MCOMMS::InstanceID id, MCOMMS::GUID& guid) const NM_OVERRIDE;

private:

  RuntimeTargetContext*   const m_context;

  DefaultNetworkManager(const DefaultNetworkManager&);
  DefaultNetworkManager& operator=(const DefaultNetworkManager&);
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_DEFAULTNETWORKMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
