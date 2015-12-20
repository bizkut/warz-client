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

#ifndef NM_MDF_HEADPOSE_PKG_H
#define NM_MDF_HEADPOSE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadPose.module"

#include "HeadPoseData.h"

#include "HeadPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct HeadPoseAPIBase
{
  HeadPoseAPIBase(
    const HeadPoseInputs* const _in, 
    const HeadAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const HeadPoseInputs* const in;

  const HeadAPIBase* const owner;

  HeadPoseAPIBase(const HeadPoseAPIBase& rhs);
  HeadPoseAPIBase& operator = (const HeadPoseAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct HeadPoseUpdatePackage : public HeadPoseAPIBase
{
  HeadPoseUpdatePackage(
    const HeadPoseInputs* const _in, 
    HeadPoseOutputs* const _out, 
    const HeadAPIBase* const _owner  ) : HeadPoseAPIBase(_in ,_owner ), out(_out) 
  {
  }


  HeadPoseOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadPoseUpdatePackage(const HeadPoseUpdatePackage& rhs);
  HeadPoseUpdatePackage& operator = (const HeadPoseUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct HeadPoseFeedbackPackage : public HeadPoseAPIBase
{
  HeadPoseFeedbackPackage(
    const HeadPoseInputs* const _in, 
    const HeadAPIBase* const _owner  ) : HeadPoseAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  HeadPoseFeedbackPackage(const HeadPoseFeedbackPackage& rhs);
  HeadPoseFeedbackPackage& operator = (const HeadPoseFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HEADPOSE_PKG_H

