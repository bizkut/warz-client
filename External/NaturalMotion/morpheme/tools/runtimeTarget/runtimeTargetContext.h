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
#ifndef NM_RTT_RUNTIMETARGETCONTEXT_H
#define NM_RTT_RUNTIMETARGETCONTEXT_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

class NetworkDefRecordManager;
class NetworkInstanceRecordManager;
class SceneObjectRecordManager;

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class RuntimeTargetContext
/// \brief Utility class that holds instances of the data managers.
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class RuntimeTargetContext
{
public:

  RuntimeTargetContext(
    NetworkDefRecordManager* const      networkDefManager,
    NetworkInstanceRecordManager* const networkInstanceManager,
    SceneObjectRecordManager* const     sceneObjectManager);

  NetworkDefRecordManager* getNetworkDefManager() const { return m_networkDefManager; }
  NetworkInstanceRecordManager* getNetworkInstanceManager() const { return m_networkInstanceManager; }
  SceneObjectRecordManager* getSceneObjectManager() const { return m_sceneObjectManager; }

private:

  NetworkDefRecordManager*         const m_networkDefManager;
  NetworkInstanceRecordManager*    const m_networkInstanceManager;
  SceneObjectRecordManager*        const m_sceneObjectManager;

  RuntimeTargetContext(const RuntimeTargetContext&);
  RuntimeTargetContext& operator=(const RuntimeTargetContext&);
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_RTT_RUNTIMETARGETCONTEXT_H
//----------------------------------------------------------------------------------------------------------------------
