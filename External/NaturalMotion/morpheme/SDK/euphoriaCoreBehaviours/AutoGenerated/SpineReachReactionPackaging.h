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

#ifndef NM_MDF_SPINEREACHREACTION_PKG_H
#define NM_MDF_SPINEREACHREACTION_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/SpineReachReaction.module"

#include "SpineReachReactionData.h"

#include "SpinePackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct SpineReachReactionAPIBase
{
  SpineReachReactionAPIBase(
    const SpineReachReactionInputs* const _in, 
    const SpineAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const SpineReachReactionInputs* const in;

  const SpineAPIBase* const owner;

  SpineReachReactionAPIBase(const SpineReachReactionAPIBase& rhs);
  SpineReachReactionAPIBase& operator = (const SpineReachReactionAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct SpineReachReactionUpdatePackage : public SpineReachReactionAPIBase
{
  SpineReachReactionUpdatePackage(
    const SpineReachReactionInputs* const _in, 
    SpineReachReactionOutputs* const _out, 
    const SpineAPIBase* const _owner  ) : SpineReachReactionAPIBase(_in ,_owner ), out(_out) 
  {
  }


  SpineReachReactionOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SpineReachReactionUpdatePackage(const SpineReachReactionUpdatePackage& rhs);
  SpineReachReactionUpdatePackage& operator = (const SpineReachReactionUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct SpineReachReactionFeedbackPackage : public SpineReachReactionAPIBase
{
  SpineReachReactionFeedbackPackage(
    const SpineReachReactionInputs* const _in, 
    const SpineAPIBase* const _owner  ) : SpineReachReactionAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  SpineReachReactionFeedbackPackage(const SpineReachReactionFeedbackPackage& rhs);
  SpineReachReactionFeedbackPackage& operator = (const SpineReachReactionFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SPINEREACHREACTION_PKG_H

