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

#ifndef NM_MDF_ANIMATEBEHAVIOURINTERFACE_PKG_H
#define NM_MDF_ANIMATEBEHAVIOURINTERFACE_PKG_H

// include definition file to create project dependency
#include "./Definition/Behaviours/AnimateBehaviourInterface.module"

#include "AnimateBehaviourInterfaceData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct AnimateBehaviourInterfaceAPIBase
{
  AnimateBehaviourInterfaceAPIBase(
    const AnimateBehaviourInterfaceData* const _data, 
    const MyNetworkAPIBase* const _owner  ) :data(_data) ,owner(_owner)  {}

  const AnimateBehaviourInterfaceData* const data;

  const MyNetworkAPIBase* const owner;

  AnimateBehaviourInterfaceAPIBase(const AnimateBehaviourInterfaceAPIBase& rhs);
  AnimateBehaviourInterfaceAPIBase& operator = (const AnimateBehaviourInterfaceAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ANIMATEBEHAVIOURINTERFACE_PKG_H

