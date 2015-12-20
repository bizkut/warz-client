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

#ifndef NM_MDF_LEGBRACE_PKG_H
#define NM_MDF_LEGBRACE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/LegBrace.module"

#include "LegBraceData.h"

#include "LegPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct LegBraceAPIBase
{
  LegBraceAPIBase(
    const LegBraceData* const _data, 
    const LegBraceInputs* const _in, 
    const LegBraceFeedbackInputs* const _feedIn, 
    const LegAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const LegBraceData* const data;
  const LegBraceInputs* const in;
  const LegBraceFeedbackInputs* const feedIn;

  const LegAPIBase* const owner;

  LegBraceAPIBase(const LegBraceAPIBase& rhs);
  LegBraceAPIBase& operator = (const LegBraceAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct LegBraceUpdatePackage : public LegBraceAPIBase
{
  LegBraceUpdatePackage(
    LegBraceData* const _data, 
    const LegBraceInputs* const _in, 
    const LegBraceFeedbackInputs* const _feedIn, 
    LegBraceOutputs* const _out, 
    const LegAPIBase* const _owner  ) : LegBraceAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  LegBraceData* const data;

  LegBraceOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegBraceUpdatePackage(const LegBraceUpdatePackage& rhs);
  LegBraceUpdatePackage& operator = (const LegBraceUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct LegBraceFeedbackPackage : public LegBraceAPIBase
{
  LegBraceFeedbackPackage(
    LegBraceData* const _data, 
    const LegBraceFeedbackInputs* const _feedIn, 
    const LegBraceInputs* const _in, 
    LegBraceFeedbackOutputs* const _feedOut, 
    const LegAPIBase* const _owner  ) : LegBraceAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  LegBraceData* const data;

  LegBraceFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegBraceFeedbackPackage(const LegBraceFeedbackPackage& rhs);
  LegBraceFeedbackPackage& operator = (const LegBraceFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEGBRACE_PKG_H

