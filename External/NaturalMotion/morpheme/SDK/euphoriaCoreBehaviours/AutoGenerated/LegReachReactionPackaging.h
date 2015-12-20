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

#ifndef NM_MDF_LEGREACHREACTION_PKG_H
#define NM_MDF_LEGREACHREACTION_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/LegReachReaction.module"

#include "LegReachReactionData.h"

#include "LegPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct LegReachReactionAPIBase
{
  LegReachReactionAPIBase(
    const LegReachReactionInputs* const _in, 
    const LegAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const LegReachReactionInputs* const in;

  const LegAPIBase* const owner;

  LegReachReactionAPIBase(const LegReachReactionAPIBase& rhs);
  LegReachReactionAPIBase& operator = (const LegReachReactionAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct LegReachReactionUpdatePackage : public LegReachReactionAPIBase
{
  LegReachReactionUpdatePackage(
    const LegReachReactionInputs* const _in, 
    LegReachReactionOutputs* const _out, 
    const LegAPIBase* const _owner  ) : LegReachReactionAPIBase(_in ,_owner ), out(_out) 
  {
  }


  LegReachReactionOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegReachReactionUpdatePackage(const LegReachReactionUpdatePackage& rhs);
  LegReachReactionUpdatePackage& operator = (const LegReachReactionUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct LegReachReactionFeedbackPackage : public LegReachReactionAPIBase
{
  LegReachReactionFeedbackPackage(
    const LegReachReactionInputs* const _in, 
    const LegAPIBase* const _owner  ) : LegReachReactionAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  LegReachReactionFeedbackPackage(const LegReachReactionFeedbackPackage& rhs);
  LegReachReactionFeedbackPackage& operator = (const LegReachReactionFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEGREACHREACTION_PKG_H

