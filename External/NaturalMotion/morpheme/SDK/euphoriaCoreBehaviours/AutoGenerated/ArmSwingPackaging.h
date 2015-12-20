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

#ifndef NM_MDF_ARMSWING_PKG_H
#define NM_MDF_ARMSWING_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmSwing.module"

#include "ArmSwingData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmSwingAPIBase
{
  ArmSwingAPIBase(
    const ArmSwingInputs* const _in, 
    const ArmAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const ArmSwingInputs* const in;

  const ArmAPIBase* const owner;

  ArmSwingAPIBase(const ArmSwingAPIBase& rhs);
  ArmSwingAPIBase& operator = (const ArmSwingAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmSwingUpdatePackage : public ArmSwingAPIBase
{
  ArmSwingUpdatePackage(
    const ArmSwingInputs* const _in, 
    ArmSwingOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmSwingAPIBase(_in ,_owner ), out(_out) 
  {
  }


  ArmSwingOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmSwingUpdatePackage(const ArmSwingUpdatePackage& rhs);
  ArmSwingUpdatePackage& operator = (const ArmSwingUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmSwingFeedbackPackage : public ArmSwingAPIBase
{
  ArmSwingFeedbackPackage(
    const ArmSwingInputs* const _in, 
    const ArmAPIBase* const _owner  ) : ArmSwingAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  ArmSwingFeedbackPackage(const ArmSwingFeedbackPackage& rhs);
  ArmSwingFeedbackPackage& operator = (const ArmSwingFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMSWING_PKG_H

