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

#ifndef NM_MDF_SPINECONTROL_PKG_H
#define NM_MDF_SPINECONTROL_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/SpineControl.module"

#include "SpineControlData.h"

#include "SpinePackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct SpineControlAPIBase
{
  SpineControlAPIBase(
    const SpineControlInputs* const _in, 
    const SpineAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const SpineControlInputs* const in;

  const SpineAPIBase* const owner;

  SpineControlAPIBase(const SpineControlAPIBase& rhs);
  SpineControlAPIBase& operator = (const SpineControlAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct SpineControlUpdatePackage : public SpineControlAPIBase
{
  SpineControlUpdatePackage(
    const SpineControlInputs* const _in, 
    SpineControlOutputs* const _out, 
    const SpineAPIBase* const _owner  ) : SpineControlAPIBase(_in ,_owner ), out(_out) 
  {
  }


  SpineControlOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SpineControlUpdatePackage(const SpineControlUpdatePackage& rhs);
  SpineControlUpdatePackage& operator = (const SpineControlUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct SpineControlFeedbackPackage : public SpineControlAPIBase
{
  SpineControlFeedbackPackage(
    const SpineControlInputs* const _in, 
    const SpineAPIBase* const _owner  ) : SpineControlAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  SpineControlFeedbackPackage(const SpineControlFeedbackPackage& rhs);
  SpineControlFeedbackPackage& operator = (const SpineControlFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SPINECONTROL_PKG_H

