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
#ifndef NM_LL_DEFAULTERASSETMANAGER_H
#define NM_LL_DEFAULTERASSETMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "defaultAssetMgr.h"

//----------------------------------------------------------------------------------------------------------------------
/// \class DefaultERAssetMgr
/// \brief Specialization of DefaultAssetMrg to add support for Euphoria assets
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class DefaultERAssetMgr : public DefaultAssetMgr
{
public:

  DefaultERAssetMgr(RuntimeTargetContext* const context, MCOMMS::IPluginValidator* validatePluginList);

protected:

  virtual bool loadNetworkDefinitionFromBuffer(
    void* filebuffer, 
    size_t fileSize, 
    NetworkDefRecord* networkDefRecord,
    MCOMMS::GUID& guid,
    const char* networkDefName) NM_OVERRIDE;

  virtual MCOMMS::InstanceID createNetworkInstance(
    const MCOMMS::GUID& guid,
    const char* instanceName,
    uint32_t startingAnimSetIndex,
    MCOMMS::Connection* connection,
    const NMP::Vector3& characterStartPosition,
    const NMP::Quat& characterStartRotation) NM_OVERRIDE;

  virtual bool destroyNetworkInstance(MCOMMS::InstanceID instance) NM_OVERRIDE;

}; // class DefaultERAssetMgr

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_LL_DEFAULTERASSETMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
