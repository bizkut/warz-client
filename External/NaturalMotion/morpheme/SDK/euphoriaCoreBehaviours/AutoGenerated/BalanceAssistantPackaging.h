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

#ifndef NM_MDF_BALANCEASSISTANT_PKG_H
#define NM_MDF_BALANCEASSISTANT_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/BalanceAssistant.module"

#include "BalanceAssistantData.h"

#include "BodyFramePackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct BalanceAssistantAPIBase
{
  BalanceAssistantAPIBase(
    const BalanceAssistantInputs* const _in, 
    const BalanceAssistantFeedbackInputs* const _feedIn, 
    const BodyFrameAPIBase* const _owner  ) :in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const BalanceAssistantInputs* const in;
  const BalanceAssistantFeedbackInputs* const feedIn;

  const BodyFrameAPIBase* const owner;

  BalanceAssistantAPIBase(const BalanceAssistantAPIBase& rhs);
  BalanceAssistantAPIBase& operator = (const BalanceAssistantAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BALANCEASSISTANT_PKG_H

