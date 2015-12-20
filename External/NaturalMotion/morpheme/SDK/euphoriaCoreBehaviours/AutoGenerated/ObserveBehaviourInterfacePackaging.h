#pragma once

/*
 * Copyright (c) 2013 NaturalMotion Ltd. All rights reserved.
 *
 * Not to be copied, adapted, modified, used, distributed, sold,
 * licensed or commercially exploited in any manner without the
 * written consent of NaturalMotion.
 *
 * All non public elements of this software are the confidential
 * information of NaturalMotion and may not be disclosed to any
 * person nor used for any purpose not expressly approved by
 * NaturalMotion in writing.
 *
 */

//----------------------------------------------------------------------------------------------------------------------
//                                  This file is auto-generated
//----------------------------------------------------------------------------------------------------------------------

#ifndef NM_MDF_OBSERVEBEHAVIOURINTERFACE_PKG_H
#define NM_MDF_OBSERVEBEHAVIOURINTERFACE_PKG_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ObserveBehaviourInterface.module"

#include "ObserveBehaviourInterfaceData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ObserveBehaviourInterfaceAPIBase
{
  ObserveBehaviourInterfaceAPIBase(
    const ObserveBehaviourInterfaceData* const _data, 
    const ObserveBehaviourInterfaceFeedbackInputs* const _feedIn, 
    const MyNetworkAPIBase* const _owner  ) :data(_data) ,feedIn(_feedIn) ,owner(_owner)  {}

  const ObserveBehaviourInterfaceData* const data;
  const ObserveBehaviourInterfaceFeedbackInputs* const feedIn;

  const MyNetworkAPIBase* const owner;

  ObserveBehaviourInterfaceAPIBase(const ObserveBehaviourInterfaceAPIBase& rhs);
  ObserveBehaviourInterfaceAPIBase& operator = (const ObserveBehaviourInterfaceAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_OBSERVEBEHAVIOURINTERFACE_PKG_H

