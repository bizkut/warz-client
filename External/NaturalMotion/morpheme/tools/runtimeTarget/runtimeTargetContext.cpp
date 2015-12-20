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
#include "runtimeTargetContext.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
RuntimeTargetContext::RuntimeTargetContext(
  NetworkDefRecordManager*      const networkDefManager,
  NetworkInstanceRecordManager* const networkInstanceManager,
  SceneObjectRecordManager*     const sceneObjectManager) :
  m_networkDefManager(networkDefManager),
  m_networkInstanceManager(networkInstanceManager),
  m_sceneObjectManager(sceneObjectManager)
{
  NMP_ASSERT(networkDefManager && networkInstanceManager);
  NMP_ASSERT(sceneObjectManager);
}
