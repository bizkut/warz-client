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

#ifndef NM_MDF_RANDOMLOOK_PKG_H
#define NM_MDF_RANDOMLOOK_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/RandomLook.module"

#include "RandomLookData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct RandomLookAPIBase
{
  RandomLookAPIBase(
    const RandomLookData* const _data, 
    const RandomLookInputs* const _in, 
    const RandomLookFeedbackInputs* const _feedIn, 
    const MyNetworkAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const RandomLookData* const data;
  const RandomLookInputs* const in;
  const RandomLookFeedbackInputs* const feedIn;

  const MyNetworkAPIBase* const owner;

  RandomLookAPIBase(const RandomLookAPIBase& rhs);
  RandomLookAPIBase& operator = (const RandomLookAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct RandomLookUpdatePackage : public RandomLookAPIBase
{
  RandomLookUpdatePackage(
    RandomLookData* const _data, 
    const RandomLookInputs* const _in, 
    const RandomLookFeedbackInputs* const _feedIn, 
    RandomLookOutputs* const _out, 
    const MyNetworkAPIBase* const _owner  ) : RandomLookAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  RandomLookData* const data;

  RandomLookOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  RandomLookUpdatePackage(const RandomLookUpdatePackage& rhs);
  RandomLookUpdatePackage& operator = (const RandomLookUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct RandomLookFeedbackPackage : public RandomLookAPIBase
{
  RandomLookFeedbackPackage(
    RandomLookData* const _data, 
    const RandomLookFeedbackInputs* const _feedIn, 
    const RandomLookInputs* const _in, 
    const MyNetworkAPIBase* const _owner  ) : RandomLookAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data) 
  {
  }

  RandomLookData* const data;



  // No update api required for this module!


  RandomLookFeedbackPackage(const RandomLookFeedbackPackage& rhs);
  RandomLookFeedbackPackage& operator = (const RandomLookFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_RANDOMLOOK_PKG_H

