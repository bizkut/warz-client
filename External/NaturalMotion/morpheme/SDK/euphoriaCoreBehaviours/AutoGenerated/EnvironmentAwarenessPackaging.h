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

#ifndef NM_MDF_ENVIRONMENTAWARENESS_PKG_H
#define NM_MDF_ENVIRONMENTAWARENESS_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/EnvironmentAwareness.module"

#include "EnvironmentAwarenessData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct EnvironmentAwarenessAPIBase
{
  EnvironmentAwarenessAPIBase(
    const EnvironmentAwarenessData* const _data, 
    const EnvironmentAwarenessInputs* const _in, 
    const EnvironmentAwarenessFeedbackInputs* const _feedIn, 
    const MyNetworkAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const EnvironmentAwarenessData* const data;
  const EnvironmentAwarenessInputs* const in;
  const EnvironmentAwarenessFeedbackInputs* const feedIn;

  const MyNetworkAPIBase* const owner;

  EnvironmentAwarenessAPIBase(const EnvironmentAwarenessAPIBase& rhs);
  EnvironmentAwarenessAPIBase& operator = (const EnvironmentAwarenessAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct EnvironmentAwarenessUpdatePackage : public EnvironmentAwarenessAPIBase
{
  EnvironmentAwarenessUpdatePackage(
    EnvironmentAwarenessData* const _data, 
    const EnvironmentAwarenessInputs* const _in, 
    const EnvironmentAwarenessFeedbackInputs* const _feedIn, 
    EnvironmentAwarenessOutputs* const _out, 
    const MyNetworkAPIBase* const _owner  ) : EnvironmentAwarenessAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  EnvironmentAwarenessData* const data;

  EnvironmentAwarenessOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  EnvironmentAwarenessUpdatePackage(const EnvironmentAwarenessUpdatePackage& rhs);
  EnvironmentAwarenessUpdatePackage& operator = (const EnvironmentAwarenessUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct EnvironmentAwarenessFeedbackPackage : public EnvironmentAwarenessAPIBase
{
  EnvironmentAwarenessFeedbackPackage(
    EnvironmentAwarenessData* const _data, 
    const EnvironmentAwarenessFeedbackInputs* const _feedIn, 
    const EnvironmentAwarenessInputs* const _in, 
    EnvironmentAwarenessFeedbackOutputs* const _feedOut, 
    const MyNetworkAPIBase* const _owner  ) : EnvironmentAwarenessAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  EnvironmentAwarenessData* const data;

  EnvironmentAwarenessFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  EnvironmentAwarenessFeedbackPackage(const EnvironmentAwarenessFeedbackPackage& rhs);
  EnvironmentAwarenessFeedbackPackage& operator = (const EnvironmentAwarenessFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ENVIRONMENTAWARENESS_PKG_H

