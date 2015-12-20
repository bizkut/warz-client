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

#ifndef NM_MDF_LEG_PKG_H
#define NM_MDF_LEG_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/Leg.module"

#include "LegData.h"
#include "LegBraceData.h"
#include "LegStandingSupportData.h"
#include "LegSittingSupportData.h"
#include "LegStepData.h"
#include "LegSwingData.h"
#include "LegPoseData.h"
#include "LegSpinData.h"
#include "LegReachReactionData.h"
#include "LegWritheData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct LegBraceUpdatePackage;
struct LegBraceFeedbackPackage;
class LegBrace_Con;
struct LegStandingSupportUpdatePackage;
struct LegStandingSupportFeedbackPackage;
class LegStandingSupport_Con;
struct LegSittingSupportUpdatePackage;
struct LegSittingSupportFeedbackPackage;
class LegSittingSupport_Con;
struct LegStepUpdatePackage;
struct LegStepFeedbackPackage;
class LegStep_Con;
struct LegSwingUpdatePackage;
struct LegSwingFeedbackPackage;
class LegSwing_Con;
struct LegPoseUpdatePackage;
struct LegPoseFeedbackPackage;
class LegPose_Con;
struct LegSpinUpdatePackage;
struct LegSpinFeedbackPackage;
class LegSpin_Con;
struct LegReachReactionUpdatePackage;
struct LegReachReactionFeedbackPackage;
class LegReachReaction_Con;
struct LegWritheUpdatePackage;
struct LegWritheFeedbackPackage;
class LegWrithe_Con;

//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct LegAPIBase
{
  LegAPIBase(
    const LegData* const _data, 
    const LegInputs* const _in, 
    const MyNetworkAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const LegData* const data;
  const LegInputs* const in;

  const MyNetworkAPIBase* const owner;

  LegAPIBase(const LegAPIBase& rhs);
  LegAPIBase& operator = (const LegAPIBase& rhs);

  // public API functions declared in module definition

  bool canReachPoint(const NMP::Vector3& position, bool& canReach) const;

};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct LegUpdatePackage : public LegAPIBase
{
  LegUpdatePackage(
    LegData* const _data, 
    const LegInputs* const _in, 
    LegOutputs* const _out, 
    const MyNetworkAPIBase* const _owner  ) : LegAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  LegData* const data;

  LegOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegUpdatePackage(const LegUpdatePackage& rhs);
  LegUpdatePackage& operator = (const LegUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct LegFeedbackPackage : public LegAPIBase
{
  LegFeedbackPackage(
    LegData* const _data, 
    const LegInputs* const _in, 
    LegFeedbackOutputs* const _feedOut, 
    const MyNetworkAPIBase* const _owner  ) : LegAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  LegData* const data;

  LegFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegFeedbackPackage(const LegFeedbackPackage& rhs);
  LegFeedbackPackage& operator = (const LegFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEG_PKG_H

