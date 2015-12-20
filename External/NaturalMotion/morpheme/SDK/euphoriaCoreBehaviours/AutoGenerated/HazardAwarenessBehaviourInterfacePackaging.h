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

#ifndef NM_MDF_HAZARDAWARENESSBEHAVIOURINTERFACE_PKG_H
#define NM_MDF_HAZARDAWARENESSBEHAVIOURINTERFACE_PKG_H

// include definition file to create project dependency
#include "./Definition/Behaviours/HazardAwarenessBehaviourInterface.module"

#include "HazardAwarenessBehaviourInterfaceData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct HazardAwarenessBehaviourInterfaceAPIBase
{
  HazardAwarenessBehaviourInterfaceAPIBase(
    const HazardAwarenessBehaviourInterfaceData* const _data, 
    const HazardAwarenessBehaviourInterfaceUserInputs* const _userIn, 
    const HazardAwarenessBehaviourInterfaceFeedbackInputs* const _feedIn, 
    const MyNetworkAPIBase* const _owner  ) :data(_data) ,userIn(_userIn) ,feedIn(_feedIn) ,owner(_owner)  {}

  const HazardAwarenessBehaviourInterfaceData* const data;
  const HazardAwarenessBehaviourInterfaceUserInputs* const userIn;
  const HazardAwarenessBehaviourInterfaceFeedbackInputs* const feedIn;

  const MyNetworkAPIBase* const owner;

  HazardAwarenessBehaviourInterfaceAPIBase(const HazardAwarenessBehaviourInterfaceAPIBase& rhs);
  HazardAwarenessBehaviourInterfaceAPIBase& operator = (const HazardAwarenessBehaviourInterfaceAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HAZARDAWARENESSBEHAVIOURINTERFACE_PKG_H

