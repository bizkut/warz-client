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

#ifndef NM_MDF_ARMSITTINGSTEP_PKG_H
#define NM_MDF_ARMSITTINGSTEP_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmSittingStep.module"

#include "ArmSittingStepData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmSittingStepAPIBase
{
  ArmSittingStepAPIBase(
    const ArmSittingStepInputs* const _in, 
    const ArmSittingStepFeedbackInputs* const _feedIn, 
    const ArmAPIBase* const _owner  ) :in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const ArmSittingStepInputs* const in;
  const ArmSittingStepFeedbackInputs* const feedIn;

  const ArmAPIBase* const owner;

  ArmSittingStepAPIBase(const ArmSittingStepAPIBase& rhs);
  ArmSittingStepAPIBase& operator = (const ArmSittingStepAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmSittingStepUpdatePackage : public ArmSittingStepAPIBase
{
  ArmSittingStepUpdatePackage(
    const ArmSittingStepInputs* const _in, 
    const ArmSittingStepFeedbackInputs* const _feedIn, 
    ArmSittingStepOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmSittingStepAPIBase(_in ,_feedIn ,_owner ), out(_out) 
  {
  }


  ArmSittingStepOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmSittingStepUpdatePackage(const ArmSittingStepUpdatePackage& rhs);
  ArmSittingStepUpdatePackage& operator = (const ArmSittingStepUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmSittingStepFeedbackPackage : public ArmSittingStepAPIBase
{
  ArmSittingStepFeedbackPackage(
    const ArmSittingStepFeedbackInputs* const _feedIn, 
    const ArmSittingStepInputs* const _in, 
    ArmSittingStepFeedbackOutputs* const _feedOut, 
    const ArmAPIBase* const _owner  ) : ArmSittingStepAPIBase(_in ,_feedIn ,_owner ), feedOut(_feedOut) 
  {
  }


  ArmSittingStepFeedbackOutputs* const feedOut;


  // No update api required for this module!


  ArmSittingStepFeedbackPackage(const ArmSittingStepFeedbackPackage& rhs);
  ArmSittingStepFeedbackPackage& operator = (const ArmSittingStepFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMSITTINGSTEP_PKG_H

