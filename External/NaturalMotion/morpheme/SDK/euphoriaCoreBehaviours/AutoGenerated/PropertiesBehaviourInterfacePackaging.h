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

#ifndef NM_MDF_PROPERTIESBEHAVIOURINTERFACE_PKG_H
#define NM_MDF_PROPERTIESBEHAVIOURINTERFACE_PKG_H

// include definition file to create project dependency
#include "./Definition/Behaviours/PropertiesBehaviourInterface.module"

#include "PropertiesBehaviourInterfaceData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct PropertiesBehaviourInterfaceAPIBase
{
  PropertiesBehaviourInterfaceAPIBase(
    const MyNetworkAPIBase* const _owner  ) :owner(_owner)  {}


  const MyNetworkAPIBase* const owner;

  PropertiesBehaviourInterfaceAPIBase(const PropertiesBehaviourInterfaceAPIBase& rhs);
  PropertiesBehaviourInterfaceAPIBase& operator = (const PropertiesBehaviourInterfaceAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_PROPERTIESBEHAVIOURINTERFACE_PKG_H

