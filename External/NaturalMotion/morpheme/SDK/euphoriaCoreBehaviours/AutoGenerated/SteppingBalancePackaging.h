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

#ifndef NM_MDF_STEPPINGBALANCE_PKG_H
#define NM_MDF_STEPPINGBALANCE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/SteppingBalance.module"

#include "SteppingBalanceData.h"

#include "BalanceManagementPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct SteppingBalanceAPIBase
{
  SteppingBalanceAPIBase(
    const SteppingBalanceData* const _data, 
    const SteppingBalanceInputs* const _in, 
    const SteppingBalanceFeedbackInputs* const _feedIn, 
    const BalanceManagementAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const SteppingBalanceData* const data;
  const SteppingBalanceInputs* const in;
  const SteppingBalanceFeedbackInputs* const feedIn;

  const BalanceManagementAPIBase* const owner;

  SteppingBalanceAPIBase(const SteppingBalanceAPIBase& rhs);
  SteppingBalanceAPIBase& operator = (const SteppingBalanceAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct SteppingBalanceUpdatePackage : public SteppingBalanceAPIBase
{
  SteppingBalanceUpdatePackage(
    SteppingBalanceData* const _data, 
    const SteppingBalanceInputs* const _in, 
    const SteppingBalanceFeedbackInputs* const _feedIn, 
    SteppingBalanceOutputs* const _out, 
    const BalanceManagementAPIBase* const _owner  ) : SteppingBalanceAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  SteppingBalanceData* const data;

  SteppingBalanceOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SteppingBalanceUpdatePackage(const SteppingBalanceUpdatePackage& rhs);
  SteppingBalanceUpdatePackage& operator = (const SteppingBalanceUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct SteppingBalanceFeedbackPackage : public SteppingBalanceAPIBase
{
  SteppingBalanceFeedbackPackage(
    SteppingBalanceData* const _data, 
    const SteppingBalanceFeedbackInputs* const _feedIn, 
    const SteppingBalanceInputs* const _in, 
    SteppingBalanceFeedbackOutputs* const _feedOut, 
    const BalanceManagementAPIBase* const _owner  ) : SteppingBalanceAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  SteppingBalanceData* const data;

  SteppingBalanceFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SteppingBalanceFeedbackPackage(const SteppingBalanceFeedbackPackage& rhs);
  SteppingBalanceFeedbackPackage& operator = (const SteppingBalanceFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_STEPPINGBALANCE_PKG_H

