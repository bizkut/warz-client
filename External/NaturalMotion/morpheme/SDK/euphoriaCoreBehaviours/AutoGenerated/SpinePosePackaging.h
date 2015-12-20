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

#ifndef NM_MDF_SPINEPOSE_PKG_H
#define NM_MDF_SPINEPOSE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/SpinePose.module"

#include "SpinePoseData.h"

#include "SpinePackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct SpinePoseAPIBase
{
  SpinePoseAPIBase(
    const SpinePoseInputs* const _in, 
    const SpineAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const SpinePoseInputs* const in;

  const SpineAPIBase* const owner;

  SpinePoseAPIBase(const SpinePoseAPIBase& rhs);
  SpinePoseAPIBase& operator = (const SpinePoseAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct SpinePoseUpdatePackage : public SpinePoseAPIBase
{
  SpinePoseUpdatePackage(
    const SpinePoseInputs* const _in, 
    SpinePoseOutputs* const _out, 
    const SpineAPIBase* const _owner  ) : SpinePoseAPIBase(_in ,_owner ), out(_out) 
  {
  }


  SpinePoseOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SpinePoseUpdatePackage(const SpinePoseUpdatePackage& rhs);
  SpinePoseUpdatePackage& operator = (const SpinePoseUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct SpinePoseFeedbackPackage : public SpinePoseAPIBase
{
  SpinePoseFeedbackPackage(
    const SpinePoseInputs* const _in, 
    const SpineAPIBase* const _owner  ) : SpinePoseAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  SpinePoseFeedbackPackage(const SpinePoseFeedbackPackage& rhs);
  SpinePoseFeedbackPackage& operator = (const SpinePoseFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SPINEPOSE_PKG_H

