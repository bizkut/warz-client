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

#ifndef NM_MDF_SPINESUPPORT_PKG_H
#define NM_MDF_SPINESUPPORT_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/SpineSupport.module"

#include "SpineSupportData.h"

#include "SpinePackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct SpineSupportAPIBase
{
  SpineSupportAPIBase(
    const SpineSupportInputs* const _in, 
    const SpineSupportFeedbackInputs* const _feedIn, 
    const SpineAPIBase* const _owner  ) :in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const SpineSupportInputs* const in;
  const SpineSupportFeedbackInputs* const feedIn;

  const SpineAPIBase* const owner;

  SpineSupportAPIBase(const SpineSupportAPIBase& rhs);
  SpineSupportAPIBase& operator = (const SpineSupportAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct SpineSupportUpdatePackage : public SpineSupportAPIBase
{
  SpineSupportUpdatePackage(
    const SpineSupportInputs* const _in, 
    const SpineSupportFeedbackInputs* const _feedIn, 
    SpineSupportOutputs* const _out, 
    const SpineAPIBase* const _owner  ) : SpineSupportAPIBase(_in ,_feedIn ,_owner ), out(_out) 
  {
  }


  SpineSupportOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SpineSupportUpdatePackage(const SpineSupportUpdatePackage& rhs);
  SpineSupportUpdatePackage& operator = (const SpineSupportUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct SpineSupportFeedbackPackage : public SpineSupportAPIBase
{
  SpineSupportFeedbackPackage(
    const SpineSupportFeedbackInputs* const _feedIn, 
    const SpineSupportInputs* const _in, 
    SpineSupportFeedbackOutputs* const _feedOut, 
    const SpineAPIBase* const _owner  ) : SpineSupportAPIBase(_in ,_feedIn ,_owner ), feedOut(_feedOut) 
  {
  }


  SpineSupportFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SpineSupportFeedbackPackage(const SpineSupportFeedbackPackage& rhs);
  SpineSupportFeedbackPackage& operator = (const SpineSupportFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SPINESUPPORT_PKG_H

