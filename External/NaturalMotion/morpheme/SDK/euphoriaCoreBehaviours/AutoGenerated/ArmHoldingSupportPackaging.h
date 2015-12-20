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

#ifndef NM_MDF_ARMHOLDINGSUPPORT_PKG_H
#define NM_MDF_ARMHOLDINGSUPPORT_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmHoldingSupport.module"

#include "ArmHoldingSupportData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmHoldingSupportAPIBase
{
  ArmHoldingSupportAPIBase(
    const ArmHoldingSupportData* const _data, 
    const ArmHoldingSupportInputs* const _in, 
    const ArmAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const ArmHoldingSupportData* const data;
  const ArmHoldingSupportInputs* const in;

  const ArmAPIBase* const owner;

  ArmHoldingSupportAPIBase(const ArmHoldingSupportAPIBase& rhs);
  ArmHoldingSupportAPIBase& operator = (const ArmHoldingSupportAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmHoldingSupportUpdatePackage : public ArmHoldingSupportAPIBase
{
  ArmHoldingSupportUpdatePackage(
    ArmHoldingSupportData* const _data, 
    const ArmHoldingSupportInputs* const _in, 
    ArmHoldingSupportOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmHoldingSupportAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  ArmHoldingSupportData* const data;

  ArmHoldingSupportOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmHoldingSupportUpdatePackage(const ArmHoldingSupportUpdatePackage& rhs);
  ArmHoldingSupportUpdatePackage& operator = (const ArmHoldingSupportUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmHoldingSupportFeedbackPackage : public ArmHoldingSupportAPIBase
{
  ArmHoldingSupportFeedbackPackage(
    ArmHoldingSupportData* const _data, 
    const ArmHoldingSupportInputs* const _in, 
    ArmHoldingSupportFeedbackOutputs* const _feedOut, 
    const ArmAPIBase* const _owner  ) : ArmHoldingSupportAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ArmHoldingSupportData* const data;

  ArmHoldingSupportFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmHoldingSupportFeedbackPackage(const ArmHoldingSupportFeedbackPackage& rhs);
  ArmHoldingSupportFeedbackPackage& operator = (const ArmHoldingSupportFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMHOLDINGSUPPORT_PKG_H

