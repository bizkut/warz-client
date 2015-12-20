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

#ifndef NM_MDF_SHIELDMANAGEMENT_PKG_H
#define NM_MDF_SHIELDMANAGEMENT_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ShieldManagement.module"

#include "ShieldManagementData.h"

#include "HazardManagementPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ShieldManagementAPIBase
{
  ShieldManagementAPIBase(
    const ShieldManagementData* const _data, 
    const ShieldManagementInputs* const _in, 
    const ShieldManagementFeedbackInputs* const _feedIn, 
    const HazardManagementAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const ShieldManagementData* const data;
  const ShieldManagementInputs* const in;
  const ShieldManagementFeedbackInputs* const feedIn;

  const HazardManagementAPIBase* const owner;

  ShieldManagementAPIBase(const ShieldManagementAPIBase& rhs);
  ShieldManagementAPIBase& operator = (const ShieldManagementAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ShieldManagementUpdatePackage : public ShieldManagementAPIBase
{
  ShieldManagementUpdatePackage(
    ShieldManagementData* const _data, 
    const ShieldManagementInputs* const _in, 
    const ShieldManagementFeedbackInputs* const _feedIn, 
    ShieldManagementOutputs* const _out, 
    const HazardManagementAPIBase* const _owner  ) : ShieldManagementAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  ShieldManagementData* const data;

  ShieldManagementOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ShieldManagementUpdatePackage(const ShieldManagementUpdatePackage& rhs);
  ShieldManagementUpdatePackage& operator = (const ShieldManagementUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ShieldManagementFeedbackPackage : public ShieldManagementAPIBase
{
  ShieldManagementFeedbackPackage(
    ShieldManagementData* const _data, 
    const ShieldManagementFeedbackInputs* const _feedIn, 
    const ShieldManagementInputs* const _in, 
    ShieldManagementFeedbackOutputs* const _feedOut, 
    const HazardManagementAPIBase* const _owner  ) : ShieldManagementAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ShieldManagementData* const data;

  ShieldManagementFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ShieldManagementFeedbackPackage(const ShieldManagementFeedbackPackage& rhs);
  ShieldManagementFeedbackPackage& operator = (const ShieldManagementFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SHIELDMANAGEMENT_PKG_H

