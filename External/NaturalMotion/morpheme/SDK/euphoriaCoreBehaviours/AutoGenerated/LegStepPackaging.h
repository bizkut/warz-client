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

#ifndef NM_MDF_LEGSTEP_PKG_H
#define NM_MDF_LEGSTEP_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/LegStep.module"

#include "LegStepData.h"

#include "LegPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct LegStepAPIBase
{
  LegStepAPIBase(
    const LegStepData* const _data, 
    const LegStepInputs* const _in, 
    const LegAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const LegStepData* const data;
  const LegStepInputs* const in;

  const LegAPIBase* const owner;

  LegStepAPIBase(const LegStepAPIBase& rhs);
  LegStepAPIBase& operator = (const LegStepAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct LegStepUpdatePackage : public LegStepAPIBase
{
  LegStepUpdatePackage(
    LegStepData* const _data, 
    const LegStepInputs* const _in, 
    LegStepOutputs* const _out, 
    const LegAPIBase* const _owner  ) : LegStepAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  LegStepData* const data;

  LegStepOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegStepUpdatePackage(const LegStepUpdatePackage& rhs);
  LegStepUpdatePackage& operator = (const LegStepUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct LegStepFeedbackPackage : public LegStepAPIBase
{
  LegStepFeedbackPackage(
    LegStepData* const _data, 
    const LegStepInputs* const _in, 
    LegStepFeedbackOutputs* const _feedOut, 
    const LegAPIBase* const _owner  ) : LegStepAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  LegStepData* const data;

  LegStepFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegStepFeedbackPackage(const LegStepFeedbackPackage& rhs);
  LegStepFeedbackPackage& operator = (const LegStepFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEGSTEP_PKG_H

