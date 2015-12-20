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

#ifndef NM_MDF_HEAD_PKG_H
#define NM_MDF_HEAD_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/Head.module"

#include "HeadData.h"
#include "HeadEyesData.h"
#include "HeadAvoidData.h"
#include "HeadDodgeData.h"
#include "HeadAimData.h"
#include "HeadPointData.h"
#include "HeadSupportData.h"
#include "HeadPoseData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct HeadEyesUpdatePackage;
struct HeadEyesFeedbackPackage;
class HeadEyes_Con;
struct HeadAvoidUpdatePackage;
struct HeadAvoidFeedbackPackage;
class HeadAvoid_Con;
struct HeadDodgeUpdatePackage;
struct HeadDodgeFeedbackPackage;
class HeadDodge_Con;
struct HeadAimUpdatePackage;
struct HeadAimFeedbackPackage;
class HeadAim_Con;
struct HeadPointUpdatePackage;
struct HeadPointFeedbackPackage;
class HeadPoint_Con;
struct HeadSupportUpdatePackage;
struct HeadSupportFeedbackPackage;
class HeadSupport_Con;
struct HeadPoseUpdatePackage;
struct HeadPoseFeedbackPackage;
class HeadPose_Con;

//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct HeadAPIBase
{
  HeadAPIBase(
    const HeadData* const _data, 
    const HeadInputs* const _in, 
    const MyNetworkAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const HeadData* const data;
  const HeadInputs* const in;

  const MyNetworkAPIBase* const owner;

  HeadAPIBase(const HeadAPIBase& rhs);
  HeadAPIBase& operator = (const HeadAPIBase& rhs);

  // public API functions declared in module definition

  bool canReachPointTarget(const NMP::Vector3& position, bool& canReach, float lookOutOfRangeWeight) const;

};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct HeadUpdatePackage : public HeadAPIBase
{
  HeadUpdatePackage(
    HeadData* const _data, 
    const HeadInputs* const _in, 
    HeadOutputs* const _out, 
    const MyNetworkAPIBase* const _owner  ) : HeadAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  HeadData* const data;

  HeadOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadUpdatePackage(const HeadUpdatePackage& rhs);
  HeadUpdatePackage& operator = (const HeadUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct HeadFeedbackPackage : public HeadAPIBase
{
  HeadFeedbackPackage(
    HeadData* const _data, 
    const HeadInputs* const _in, 
    HeadFeedbackOutputs* const _feedOut, 
    const MyNetworkAPIBase* const _owner  ) : HeadAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  HeadData* const data;

  HeadFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadFeedbackPackage(const HeadFeedbackPackage& rhs);
  HeadFeedbackPackage& operator = (const HeadFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HEAD_PKG_H

