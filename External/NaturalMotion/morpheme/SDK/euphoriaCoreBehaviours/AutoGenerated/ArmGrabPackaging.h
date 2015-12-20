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

#ifndef NM_MDF_ARMGRAB_PKG_H
#define NM_MDF_ARMGRAB_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmGrab.module"

#include "ArmGrabData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmGrabAPIBase
{
  ArmGrabAPIBase(
    const ArmGrabData* const _data, 
    const ArmGrabInputs* const _in, 
    const ArmGrabFeedbackInputs* const _feedIn, 
    const ArmAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const ArmGrabData* const data;
  const ArmGrabInputs* const in;
  const ArmGrabFeedbackInputs* const feedIn;

  const ArmAPIBase* const owner;

  ArmGrabAPIBase(const ArmGrabAPIBase& rhs);
  ArmGrabAPIBase& operator = (const ArmGrabAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmGrabUpdatePackage : public ArmGrabAPIBase
{
  ArmGrabUpdatePackage(
    ArmGrabData* const _data, 
    const ArmGrabInputs* const _in, 
    const ArmGrabFeedbackInputs* const _feedIn, 
    ArmGrabOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmGrabAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  ArmGrabData* const data;

  ArmGrabOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmGrabUpdatePackage(const ArmGrabUpdatePackage& rhs);
  ArmGrabUpdatePackage& operator = (const ArmGrabUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmGrabFeedbackPackage : public ArmGrabAPIBase
{
  ArmGrabFeedbackPackage(
    ArmGrabData* const _data, 
    const ArmGrabFeedbackInputs* const _feedIn, 
    const ArmGrabInputs* const _in, 
    ArmGrabFeedbackOutputs* const _feedOut, 
    const ArmAPIBase* const _owner  ) : ArmGrabAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ArmGrabData* const data;

  ArmGrabFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmGrabFeedbackPackage(const ArmGrabFeedbackPackage& rhs);
  ArmGrabFeedbackPackage& operator = (const ArmGrabFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMGRAB_PKG_H

