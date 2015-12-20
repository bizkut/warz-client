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

#ifndef NM_MDF_USERHAZARDBEHAVIOURINTERFACE_PKG_H
#define NM_MDF_USERHAZARDBEHAVIOURINTERFACE_PKG_H

// include definition file to create project dependency
#include "./Definition/Behaviours/UserHazardBehaviourInterface.module"

#include "UserHazardBehaviourInterfaceData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct UserHazardBehaviourInterfaceAPIBase
{
  UserHazardBehaviourInterfaceAPIBase(
    const UserHazardBehaviourInterfaceUserInputs* const _userIn, 
    const UserHazardBehaviourInterfaceFeedbackInputs* const _feedIn, 
    const MyNetworkAPIBase* const _owner  ) :userIn(_userIn) ,feedIn(_feedIn) ,owner(_owner)  {}

  const UserHazardBehaviourInterfaceUserInputs* const userIn;
  const UserHazardBehaviourInterfaceFeedbackInputs* const feedIn;

  const MyNetworkAPIBase* const owner;

  UserHazardBehaviourInterfaceAPIBase(const UserHazardBehaviourInterfaceAPIBase& rhs);
  UserHazardBehaviourInterfaceAPIBase& operator = (const UserHazardBehaviourInterfaceAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_USERHAZARDBEHAVIOURINTERFACE_PKG_H

