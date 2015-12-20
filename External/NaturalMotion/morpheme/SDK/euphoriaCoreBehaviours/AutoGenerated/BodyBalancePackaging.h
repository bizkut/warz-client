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

#ifndef NM_MDF_BODYBALANCE_PKG_H
#define NM_MDF_BODYBALANCE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/BodyBalance.module"

#include "BodyBalanceData.h"

#include "BodyFramePackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct BodyBalanceAPIBase
{
  BodyBalanceAPIBase(
    const BodyBalanceData* const _data, 
    const BodyBalanceInputs* const _in, 
    const BodyBalanceFeedbackInputs* const _feedIn, 
    const BodyFrameAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const BodyBalanceData* const data;
  const BodyBalanceInputs* const in;
  const BodyBalanceFeedbackInputs* const feedIn;

  const BodyFrameAPIBase* const owner;

  BodyBalanceAPIBase(const BodyBalanceAPIBase& rhs);
  BodyBalanceAPIBase& operator = (const BodyBalanceAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct BodyBalanceUpdatePackage : public BodyBalanceAPIBase
{
  BodyBalanceUpdatePackage(
    BodyBalanceData* const _data, 
    const BodyBalanceInputs* const _in, 
    const BodyBalanceFeedbackInputs* const _feedIn, 
    BodyBalanceOutputs* const _out, 
    const BodyFrameAPIBase* const _owner  ) : BodyBalanceAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  BodyBalanceData* const data;

  BodyBalanceOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  BodyBalanceUpdatePackage(const BodyBalanceUpdatePackage& rhs);
  BodyBalanceUpdatePackage& operator = (const BodyBalanceUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct BodyBalanceFeedbackPackage : public BodyBalanceAPIBase
{
  BodyBalanceFeedbackPackage(
    BodyBalanceData* const _data, 
    const BodyBalanceFeedbackInputs* const _feedIn, 
    const BodyBalanceInputs* const _in, 
    BodyBalanceFeedbackOutputs* const _feedOut, 
    const BodyFrameAPIBase* const _owner  ) : BodyBalanceAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  BodyBalanceData* const data;

  BodyBalanceFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  BodyBalanceFeedbackPackage(const BodyBalanceFeedbackPackage& rhs);
  BodyBalanceFeedbackPackage& operator = (const BodyBalanceFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BODYBALANCE_PKG_H

