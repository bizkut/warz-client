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

#ifndef NM_MDF_LEGPOSE_PKG_H
#define NM_MDF_LEGPOSE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/LegPose.module"

#include "LegPoseData.h"

#include "LegPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct LegPoseAPIBase
{
  LegPoseAPIBase(
    const LegPoseInputs* const _in, 
    const LegAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const LegPoseInputs* const in;

  const LegAPIBase* const owner;

  LegPoseAPIBase(const LegPoseAPIBase& rhs);
  LegPoseAPIBase& operator = (const LegPoseAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct LegPoseUpdatePackage : public LegPoseAPIBase
{
  LegPoseUpdatePackage(
    const LegPoseInputs* const _in, 
    LegPoseOutputs* const _out, 
    const LegAPIBase* const _owner  ) : LegPoseAPIBase(_in ,_owner ), out(_out) 
  {
  }


  LegPoseOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegPoseUpdatePackage(const LegPoseUpdatePackage& rhs);
  LegPoseUpdatePackage& operator = (const LegPoseUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct LegPoseFeedbackPackage : public LegPoseAPIBase
{
  LegPoseFeedbackPackage(
    const LegPoseInputs* const _in, 
    const LegAPIBase* const _owner  ) : LegPoseAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  LegPoseFeedbackPackage(const LegPoseFeedbackPackage& rhs);
  LegPoseFeedbackPackage& operator = (const LegPoseFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEGPOSE_PKG_H

