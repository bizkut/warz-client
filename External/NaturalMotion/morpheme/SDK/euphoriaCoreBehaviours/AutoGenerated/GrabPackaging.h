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

#ifndef NM_MDF_GRAB_PKG_H
#define NM_MDF_GRAB_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/Grab.module"

#include "GrabData.h"

#include "HazardManagementPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct GrabAPIBase
{
  GrabAPIBase(
    const GrabData* const _data, 
    const GrabInputs* const _in, 
    const GrabFeedbackInputs* const _feedIn, 
    const HazardManagementAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const GrabData* const data;
  const GrabInputs* const in;
  const GrabFeedbackInputs* const feedIn;

  const HazardManagementAPIBase* const owner;

  GrabAPIBase(const GrabAPIBase& rhs);
  GrabAPIBase& operator = (const GrabAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_GRAB_PKG_H

