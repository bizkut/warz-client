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

#ifndef NM_MDF_ARMHOLD_PKG_H
#define NM_MDF_ARMHOLD_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmHold.module"

#include "ArmHoldData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmHoldAPIBase
{
  ArmHoldAPIBase(
    const ArmHoldData* const _data, 
    const ArmHoldInputs* const _in, 
    const ArmAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const ArmHoldData* const data;
  const ArmHoldInputs* const in;

  const ArmAPIBase* const owner;

  ArmHoldAPIBase(const ArmHoldAPIBase& rhs);
  ArmHoldAPIBase& operator = (const ArmHoldAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmHoldUpdatePackage : public ArmHoldAPIBase
{
  ArmHoldUpdatePackage(
    ArmHoldData* const _data, 
    const ArmHoldInputs* const _in, 
    ArmHoldOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmHoldAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  ArmHoldData* const data;

  ArmHoldOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmHoldUpdatePackage(const ArmHoldUpdatePackage& rhs);
  ArmHoldUpdatePackage& operator = (const ArmHoldUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmHoldFeedbackPackage : public ArmHoldAPIBase
{
  ArmHoldFeedbackPackage(
    ArmHoldData* const _data, 
    const ArmHoldInputs* const _in, 
    ArmHoldFeedbackOutputs* const _feedOut, 
    const ArmAPIBase* const _owner  ) : ArmHoldAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ArmHoldData* const data;

  ArmHoldFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmHoldFeedbackPackage(const ArmHoldFeedbackPackage& rhs);
  ArmHoldFeedbackPackage& operator = (const ArmHoldFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMHOLD_PKG_H

