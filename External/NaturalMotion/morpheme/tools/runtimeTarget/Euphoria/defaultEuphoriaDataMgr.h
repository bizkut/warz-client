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
#ifndef NM_DEFAULTEREUPHORIADATAMGR_H
#define NM_DEFAULTEREUPHORIADATAMGR_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/euphoriaRuntimeTargetSimple.h"
//----------------------------------------------------------------------------------------------------------------------

class NetworkDefRecordManager;
class NetworkInstanceRecordManager;
class RuntimeTargetContext;

//----------------------------------------------------------------------------------------------------------------------
/// \class DefaultEuphoriaDataManager
/// \brief Implements MCOMMS::SimpleEuphoriaDataManager.
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class DefaultEuphoriaDataManager :
  public MCOMMS::SimpleEuphoriaDataManager
{
public:

  DefaultEuphoriaDataManager(RuntimeTargetContext* context);

  virtual ~DefaultEuphoriaDataManager() {}

  //--------------------------------------------------------------------------------------------------------------------
  // [DataManagementInterface]
public:

  virtual MR::Network* findNetworkByInstanceID(MCOMMS::InstanceID id) const NM_OVERRIDE;
  virtual MR::NetworkDef* findNetworkDefByGuid(const MCOMMS::GUID& networkGUID) const NM_OVERRIDE;

protected:
  NetworkDefRecordManager* const       m_networkDefManager;
  NetworkInstanceRecordManager* const  m_networkInstanceManager;

private:
  // disabled assignment operator to disable warning C4512
  const DefaultEuphoriaDataManager& operator = (const DefaultEuphoriaDataManager& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_DEFAULTEREUPHORIADATAMGR_H
//----------------------------------------------------------------------------------------------------------------------
