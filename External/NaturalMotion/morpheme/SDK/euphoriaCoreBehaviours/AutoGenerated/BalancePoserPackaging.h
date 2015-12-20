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

#ifndef NM_MDF_BALANCEPOSER_PKG_H
#define NM_MDF_BALANCEPOSER_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/BalancePoser.module"

#include "BalancePoserData.h"

#include "BalanceManagementPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct BalancePoserAPIBase
{
  BalancePoserAPIBase(
    const BalancePoserInputs* const _in, 
    const BalanceManagementAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const BalancePoserInputs* const in;

  const BalanceManagementAPIBase* const owner;

  BalancePoserAPIBase(const BalancePoserAPIBase& rhs);
  BalancePoserAPIBase& operator = (const BalancePoserAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct BalancePoserUpdatePackage : public BalancePoserAPIBase
{
  BalancePoserUpdatePackage(
    const BalancePoserInputs* const _in, 
    BalancePoserOutputs* const _out, 
    const BalanceManagementAPIBase* const _owner  ) : BalancePoserAPIBase(_in ,_owner ), out(_out) 
  {
  }


  BalancePoserOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  BalancePoserUpdatePackage(const BalancePoserUpdatePackage& rhs);
  BalancePoserUpdatePackage& operator = (const BalancePoserUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct BalancePoserFeedbackPackage : public BalancePoserAPIBase
{
  BalancePoserFeedbackPackage(
    const BalancePoserInputs* const _in, 
    const BalanceManagementAPIBase* const _owner  ) : BalancePoserAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  BalancePoserFeedbackPackage(const BalancePoserFeedbackPackage& rhs);
  BalancePoserFeedbackPackage& operator = (const BalancePoserFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BALANCEPOSER_PKG_H

