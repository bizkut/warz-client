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

#ifndef NM_MDF_ARMREACHREACTION_PKG_H
#define NM_MDF_ARMREACHREACTION_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmReachReaction.module"

#include "ArmReachReactionData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmReachReactionAPIBase
{
  ArmReachReactionAPIBase(
    const ArmReachReactionInputs* const _in, 
    const ArmAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const ArmReachReactionInputs* const in;

  const ArmAPIBase* const owner;

  ArmReachReactionAPIBase(const ArmReachReactionAPIBase& rhs);
  ArmReachReactionAPIBase& operator = (const ArmReachReactionAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmReachReactionUpdatePackage : public ArmReachReactionAPIBase
{
  ArmReachReactionUpdatePackage(
    const ArmReachReactionInputs* const _in, 
    ArmReachReactionOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmReachReactionAPIBase(_in ,_owner ), out(_out) 
  {
  }


  ArmReachReactionOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmReachReactionUpdatePackage(const ArmReachReactionUpdatePackage& rhs);
  ArmReachReactionUpdatePackage& operator = (const ArmReachReactionUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmReachReactionFeedbackPackage : public ArmReachReactionAPIBase
{
  ArmReachReactionFeedbackPackage(
    const ArmReachReactionInputs* const _in, 
    const ArmAPIBase* const _owner  ) : ArmReachReactionAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  ArmReachReactionFeedbackPackage(const ArmReachReactionFeedbackPackage& rhs);
  ArmReachReactionFeedbackPackage& operator = (const ArmReachReactionFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMREACHREACTION_PKG_H

