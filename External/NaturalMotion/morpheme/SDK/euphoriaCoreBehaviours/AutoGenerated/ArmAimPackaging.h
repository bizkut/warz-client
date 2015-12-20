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

#ifndef NM_MDF_ARMAIM_PKG_H
#define NM_MDF_ARMAIM_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmAim.module"

#include "ArmAimData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmAimAPIBase
{
  ArmAimAPIBase(
    const ArmAimInputs* const _in, 
    const ArmAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const ArmAimInputs* const in;

  const ArmAPIBase* const owner;

  ArmAimAPIBase(const ArmAimAPIBase& rhs);
  ArmAimAPIBase& operator = (const ArmAimAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmAimUpdatePackage : public ArmAimAPIBase
{
  ArmAimUpdatePackage(
    const ArmAimInputs* const _in, 
    ArmAimOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmAimAPIBase(_in ,_owner ), out(_out) 
  {
  }


  ArmAimOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmAimUpdatePackage(const ArmAimUpdatePackage& rhs);
  ArmAimUpdatePackage& operator = (const ArmAimUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmAimFeedbackPackage : public ArmAimAPIBase
{
  ArmAimFeedbackPackage(
    const ArmAimInputs* const _in, 
    const ArmAPIBase* const _owner  ) : ArmAimAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  ArmAimFeedbackPackage(const ArmAimFeedbackPackage& rhs);
  ArmAimFeedbackPackage& operator = (const ArmAimFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMAIM_PKG_H

