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

#ifndef NM_MDF_SPINE_PKG_H
#define NM_MDF_SPINE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/Spine.module"

#include "SpineData.h"
#include "SpineSupportData.h"
#include "SpinePoseData.h"
#include "SpineControlData.h"
#include "SpineReachReactionData.h"
#include "SpineWritheData.h"
#include "SpineAimData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct SpineSupportUpdatePackage;
struct SpineSupportFeedbackPackage;
class SpineSupport_Con;
struct SpinePoseUpdatePackage;
struct SpinePoseFeedbackPackage;
class SpinePose_Con;
struct SpineControlUpdatePackage;
struct SpineControlFeedbackPackage;
class SpineControl_Con;
struct SpineReachReactionUpdatePackage;
struct SpineReachReactionFeedbackPackage;
class SpineReachReaction_Con;
struct SpineWritheUpdatePackage;
struct SpineWritheFeedbackPackage;
class SpineWrithe_Con;
struct SpineAimUpdatePackage;
struct SpineAimFeedbackPackage;
class SpineAim_Con;

//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct SpineAPIBase
{
  SpineAPIBase(
    const SpineData* const _data, 
    const SpineInputs* const _in, 
    const MyNetworkAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const SpineData* const data;
  const SpineInputs* const in;

  const MyNetworkAPIBase* const owner;

  SpineAPIBase(const SpineAPIBase& rhs);
  SpineAPIBase& operator = (const SpineAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct SpineUpdatePackage : public SpineAPIBase
{
  SpineUpdatePackage(
    SpineData* const _data, 
    const SpineInputs* const _in, 
    SpineOutputs* const _out, 
    const MyNetworkAPIBase* const _owner  ) : SpineAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  SpineData* const data;

  SpineOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SpineUpdatePackage(const SpineUpdatePackage& rhs);
  SpineUpdatePackage& operator = (const SpineUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct SpineFeedbackPackage : public SpineAPIBase
{
  SpineFeedbackPackage(
    SpineData* const _data, 
    const SpineInputs* const _in, 
    SpineFeedbackOutputs* const _feedOut, 
    const MyNetworkAPIBase* const _owner  ) : SpineAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  SpineData* const data;

  SpineFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SpineFeedbackPackage(const SpineFeedbackPackage& rhs);
  SpineFeedbackPackage& operator = (const SpineFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SPINE_PKG_H

