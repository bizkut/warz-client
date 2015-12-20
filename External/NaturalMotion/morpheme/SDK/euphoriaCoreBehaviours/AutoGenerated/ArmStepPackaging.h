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

#ifndef NM_MDF_ARMSTEP_PKG_H
#define NM_MDF_ARMSTEP_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmStep.module"

#include "ArmStepData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmStepAPIBase
{
  ArmStepAPIBase(
    const ArmStepData* const _data, 
    const ArmStepInputs* const _in, 
    const ArmAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const ArmStepData* const data;
  const ArmStepInputs* const in;

  const ArmAPIBase* const owner;

  ArmStepAPIBase(const ArmStepAPIBase& rhs);
  ArmStepAPIBase& operator = (const ArmStepAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmStepUpdatePackage : public ArmStepAPIBase
{
  ArmStepUpdatePackage(
    ArmStepData* const _data, 
    const ArmStepInputs* const _in, 
    ArmStepOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmStepAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  ArmStepData* const data;

  ArmStepOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmStepUpdatePackage(const ArmStepUpdatePackage& rhs);
  ArmStepUpdatePackage& operator = (const ArmStepUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmStepFeedbackPackage : public ArmStepAPIBase
{
  ArmStepFeedbackPackage(
    ArmStepData* const _data, 
    const ArmStepInputs* const _in, 
    ArmStepFeedbackOutputs* const _feedOut, 
    const ArmAPIBase* const _owner  ) : ArmStepAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ArmStepData* const data;

  ArmStepFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmStepFeedbackPackage(const ArmStepFeedbackPackage& rhs);
  ArmStepFeedbackPackage& operator = (const ArmStepFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMSTEP_PKG_H

