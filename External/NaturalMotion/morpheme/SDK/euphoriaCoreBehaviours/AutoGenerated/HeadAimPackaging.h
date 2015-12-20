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

#ifndef NM_MDF_HEADAIM_PKG_H
#define NM_MDF_HEADAIM_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadAim.module"

#include "HeadAimData.h"

#include "HeadPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct HeadAimAPIBase
{
  HeadAimAPIBase(
    const HeadAimInputs* const _in, 
    const HeadAimFeedbackInputs* const _feedIn, 
    const HeadAPIBase* const _owner  ) :in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const HeadAimInputs* const in;
  const HeadAimFeedbackInputs* const feedIn;

  const HeadAPIBase* const owner;

  HeadAimAPIBase(const HeadAimAPIBase& rhs);
  HeadAimAPIBase& operator = (const HeadAimAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct HeadAimUpdatePackage : public HeadAimAPIBase
{
  HeadAimUpdatePackage(
    const HeadAimInputs* const _in, 
    const HeadAimFeedbackInputs* const _feedIn, 
    HeadAimOutputs* const _out, 
    const HeadAPIBase* const _owner  ) : HeadAimAPIBase(_in ,_feedIn ,_owner ), out(_out) 
  {
  }


  HeadAimOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadAimUpdatePackage(const HeadAimUpdatePackage& rhs);
  HeadAimUpdatePackage& operator = (const HeadAimUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct HeadAimFeedbackPackage : public HeadAimAPIBase
{
  HeadAimFeedbackPackage(
    const HeadAimFeedbackInputs* const _feedIn, 
    const HeadAimInputs* const _in, 
    const HeadAPIBase* const _owner  ) : HeadAimAPIBase(_in ,_feedIn ,_owner ) 
  {
  }




  // No update api required for this module!


  HeadAimFeedbackPackage(const HeadAimFeedbackPackage& rhs);
  HeadAimFeedbackPackage& operator = (const HeadAimFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HEADAIM_PKG_H

