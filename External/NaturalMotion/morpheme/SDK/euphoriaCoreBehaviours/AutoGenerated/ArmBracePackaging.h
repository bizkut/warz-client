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

#ifndef NM_MDF_ARMBRACE_PKG_H
#define NM_MDF_ARMBRACE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmBrace.module"

#include "ArmBraceData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmBraceAPIBase
{
  ArmBraceAPIBase(
    const ArmBraceData* const _data, 
    const ArmBraceInputs* const _in, 
    const ArmBraceFeedbackInputs* const _feedIn, 
    const ArmAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const ArmBraceData* const data;
  const ArmBraceInputs* const in;
  const ArmBraceFeedbackInputs* const feedIn;

  const ArmAPIBase* const owner;

  ArmBraceAPIBase(const ArmBraceAPIBase& rhs);
  ArmBraceAPIBase& operator = (const ArmBraceAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmBraceUpdatePackage : public ArmBraceAPIBase
{
  ArmBraceUpdatePackage(
    ArmBraceData* const _data, 
    const ArmBraceInputs* const _in, 
    const ArmBraceFeedbackInputs* const _feedIn, 
    ArmBraceOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmBraceAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  ArmBraceData* const data;

  ArmBraceOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmBraceUpdatePackage(const ArmBraceUpdatePackage& rhs);
  ArmBraceUpdatePackage& operator = (const ArmBraceUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmBraceFeedbackPackage : public ArmBraceAPIBase
{
  ArmBraceFeedbackPackage(
    ArmBraceData* const _data, 
    const ArmBraceFeedbackInputs* const _feedIn, 
    const ArmBraceInputs* const _in, 
    ArmBraceFeedbackOutputs* const _feedOut, 
    const ArmAPIBase* const _owner  ) : ArmBraceAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ArmBraceData* const data;

  ArmBraceFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmBraceFeedbackPackage(const ArmBraceFeedbackPackage& rhs);
  ArmBraceFeedbackPackage& operator = (const ArmBraceFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMBRACE_PKG_H

