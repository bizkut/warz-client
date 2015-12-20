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

#ifndef NM_MDF_ARMPOSE_PKG_H
#define NM_MDF_ARMPOSE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmPose.module"

#include "ArmPoseData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmPoseAPIBase
{
  ArmPoseAPIBase(
    const ArmPoseInputs* const _in, 
    const ArmAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const ArmPoseInputs* const in;

  const ArmAPIBase* const owner;

  ArmPoseAPIBase(const ArmPoseAPIBase& rhs);
  ArmPoseAPIBase& operator = (const ArmPoseAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmPoseUpdatePackage : public ArmPoseAPIBase
{
  ArmPoseUpdatePackage(
    const ArmPoseInputs* const _in, 
    ArmPoseOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmPoseAPIBase(_in ,_owner ), out(_out) 
  {
  }


  ArmPoseOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmPoseUpdatePackage(const ArmPoseUpdatePackage& rhs);
  ArmPoseUpdatePackage& operator = (const ArmPoseUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmPoseFeedbackPackage : public ArmPoseAPIBase
{
  ArmPoseFeedbackPackage(
    const ArmPoseInputs* const _in, 
    const ArmAPIBase* const _owner  ) : ArmPoseAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  ArmPoseFeedbackPackage(const ArmPoseFeedbackPackage& rhs);
  ArmPoseFeedbackPackage& operator = (const ArmPoseFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMPOSE_PKG_H

