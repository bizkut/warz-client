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

#ifndef NM_MDF_BALANCEMANAGEMENT_PKG_H
#define NM_MDF_BALANCEMANAGEMENT_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/BalanceManagement.module"

#include "BalanceManagementData.h"
#include "StaticBalanceData.h"
#include "SteppingBalanceData.h"
#include "BalancePoserData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct StaticBalanceUpdatePackage;
struct StaticBalanceFeedbackPackage;
class StaticBalance_Con;
struct SteppingBalanceUpdatePackage;
struct SteppingBalanceFeedbackPackage;
class SteppingBalance_Con;
struct BalancePoserUpdatePackage;
struct BalancePoserFeedbackPackage;
class BalancePoser_Con;

//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct BalanceManagementAPIBase
{
  BalanceManagementAPIBase(
    const BalanceManagementInputs* const _in, 
    const BalanceManagementFeedbackInputs* const _feedIn, 
    const MyNetworkAPIBase* const _owner  ) :in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const BalanceManagementInputs* const in;
  const BalanceManagementFeedbackInputs* const feedIn;

  const MyNetworkAPIBase* const owner;

  BalanceManagementAPIBase(const BalanceManagementAPIBase& rhs);
  BalanceManagementAPIBase& operator = (const BalanceManagementAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct BalanceManagementUpdatePackage : public BalanceManagementAPIBase
{
  BalanceManagementUpdatePackage(
    const BalanceManagementInputs* const _in, 
    const BalanceManagementFeedbackInputs* const _feedIn, 
    BalanceManagementOutputs* const _out, 
    const MyNetworkAPIBase* const _owner  ) : BalanceManagementAPIBase(_in ,_feedIn ,_owner ), out(_out) 
  {
  }


  BalanceManagementOutputs* const out;


  // No update api required for this module!


  BalanceManagementUpdatePackage(const BalanceManagementUpdatePackage& rhs);
  BalanceManagementUpdatePackage& operator = (const BalanceManagementUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct BalanceManagementFeedbackPackage : public BalanceManagementAPIBase
{
  BalanceManagementFeedbackPackage(
    const BalanceManagementFeedbackInputs* const _feedIn, 
    const BalanceManagementInputs* const _in, 
    const MyNetworkAPIBase* const _owner  ) : BalanceManagementAPIBase(_in ,_feedIn ,_owner ) 
  {
  }




  // No update api required for this module!


  BalanceManagementFeedbackPackage(const BalanceManagementFeedbackPackage& rhs);
  BalanceManagementFeedbackPackage& operator = (const BalanceManagementFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BALANCEMANAGEMENT_PKG_H

