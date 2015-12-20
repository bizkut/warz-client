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

#ifndef NM_MDF_SPINEAIM_PKG_H
#define NM_MDF_SPINEAIM_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/SpineAim.module"

#include "SpineAimData.h"

#include "SpinePackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct SpineAimAPIBase
{
  SpineAimAPIBase(
    const SpineAimInputs* const _in, 
    const SpineAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const SpineAimInputs* const in;

  const SpineAPIBase* const owner;

  SpineAimAPIBase(const SpineAimAPIBase& rhs);
  SpineAimAPIBase& operator = (const SpineAimAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct SpineAimUpdatePackage : public SpineAimAPIBase
{
  SpineAimUpdatePackage(
    const SpineAimInputs* const _in, 
    SpineAimOutputs* const _out, 
    const SpineAPIBase* const _owner  ) : SpineAimAPIBase(_in ,_owner ), out(_out) 
  {
  }


  SpineAimOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SpineAimUpdatePackage(const SpineAimUpdatePackage& rhs);
  SpineAimUpdatePackage& operator = (const SpineAimUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct SpineAimFeedbackPackage : public SpineAimAPIBase
{
  SpineAimFeedbackPackage(
    const SpineAimInputs* const _in, 
    const SpineAPIBase* const _owner  ) : SpineAimAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  SpineAimFeedbackPackage(const SpineAimFeedbackPackage& rhs);
  SpineAimFeedbackPackage& operator = (const SpineAimFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SPINEAIM_PKG_H

