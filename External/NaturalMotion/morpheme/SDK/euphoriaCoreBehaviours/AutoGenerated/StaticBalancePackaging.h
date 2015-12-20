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

#ifndef NM_MDF_STATICBALANCE_PKG_H
#define NM_MDF_STATICBALANCE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/StaticBalance.module"

#include "StaticBalanceData.h"

#include "BalanceManagementPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct StaticBalanceAPIBase
{
  StaticBalanceAPIBase(
    const StaticBalanceInputs* const _in, 
    const StaticBalanceFeedbackInputs* const _feedIn, 
    const BalanceManagementAPIBase* const _owner  ) :in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const StaticBalanceInputs* const in;
  const StaticBalanceFeedbackInputs* const feedIn;

  const BalanceManagementAPIBase* const owner;

  StaticBalanceAPIBase(const StaticBalanceAPIBase& rhs);
  StaticBalanceAPIBase& operator = (const StaticBalanceAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct StaticBalanceUpdatePackage : public StaticBalanceAPIBase
{
  StaticBalanceUpdatePackage(
    const StaticBalanceInputs* const _in, 
    const StaticBalanceFeedbackInputs* const _feedIn, 
    StaticBalanceOutputs* const _out, 
    const BalanceManagementAPIBase* const _owner  ) : StaticBalanceAPIBase(_in ,_feedIn ,_owner ), out(_out) 
  {
  }


  StaticBalanceOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  StaticBalanceUpdatePackage(const StaticBalanceUpdatePackage& rhs);
  StaticBalanceUpdatePackage& operator = (const StaticBalanceUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct StaticBalanceFeedbackPackage : public StaticBalanceAPIBase
{
  StaticBalanceFeedbackPackage(
    const StaticBalanceFeedbackInputs* const _feedIn, 
    const StaticBalanceInputs* const _in, 
    const BalanceManagementAPIBase* const _owner  ) : StaticBalanceAPIBase(_in ,_feedIn ,_owner ) 
  {
  }




  // No update api required for this module!


  StaticBalanceFeedbackPackage(const StaticBalanceFeedbackPackage& rhs);
  StaticBalanceFeedbackPackage& operator = (const StaticBalanceFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_STATICBALANCE_PKG_H

