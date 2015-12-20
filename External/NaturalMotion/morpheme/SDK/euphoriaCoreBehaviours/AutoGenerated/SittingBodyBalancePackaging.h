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

#ifndef NM_MDF_SITTINGBODYBALANCE_PKG_H
#define NM_MDF_SITTINGBODYBALANCE_PKG_H

// include definition file to create project dependency
#include "./Definition/Behaviours/SittingBodyBalance.module"

#include "SittingBodyBalanceData.h"

#include "BodyFramePackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct SittingBodyBalanceAPIBase
{
  SittingBodyBalanceAPIBase(
    const SittingBodyBalanceData* const _data, 
    const SittingBodyBalanceInputs* const _in, 
    const SittingBodyBalanceFeedbackInputs* const _feedIn, 
    const BodyFrameAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const SittingBodyBalanceData* const data;
  const SittingBodyBalanceInputs* const in;
  const SittingBodyBalanceFeedbackInputs* const feedIn;

  const BodyFrameAPIBase* const owner;

  SittingBodyBalanceAPIBase(const SittingBodyBalanceAPIBase& rhs);
  SittingBodyBalanceAPIBase& operator = (const SittingBodyBalanceAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct SittingBodyBalanceUpdatePackage : public SittingBodyBalanceAPIBase
{
  SittingBodyBalanceUpdatePackage(
    SittingBodyBalanceData* const _data, 
    const SittingBodyBalanceInputs* const _in, 
    const SittingBodyBalanceFeedbackInputs* const _feedIn, 
    SittingBodyBalanceOutputs* const _out, 
    const BodyFrameAPIBase* const _owner  ) : SittingBodyBalanceAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  SittingBodyBalanceData* const data;

  SittingBodyBalanceOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SittingBodyBalanceUpdatePackage(const SittingBodyBalanceUpdatePackage& rhs);
  SittingBodyBalanceUpdatePackage& operator = (const SittingBodyBalanceUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct SittingBodyBalanceFeedbackPackage : public SittingBodyBalanceAPIBase
{
  SittingBodyBalanceFeedbackPackage(
    SittingBodyBalanceData* const _data, 
    const SittingBodyBalanceFeedbackInputs* const _feedIn, 
    const SittingBodyBalanceInputs* const _in, 
    SittingBodyBalanceFeedbackOutputs* const _feedOut, 
    const BodyFrameAPIBase* const _owner  ) : SittingBodyBalanceAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  SittingBodyBalanceData* const data;

  SittingBodyBalanceFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SittingBodyBalanceFeedbackPackage(const SittingBodyBalanceFeedbackPackage& rhs);
  SittingBodyBalanceFeedbackPackage& operator = (const SittingBodyBalanceFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SITTINGBODYBALANCE_PKG_H

