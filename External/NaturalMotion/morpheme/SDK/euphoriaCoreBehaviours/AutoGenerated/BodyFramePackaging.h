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

#ifndef NM_MDF_BODYFRAME_PKG_H
#define NM_MDF_BODYFRAME_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/BodyFrame.module"

#include "BodyFrameData.h"
#include "SupportPolygonData.h"
#include "SupportPolygonData.h"
#include "BodyBalanceData.h"
#include "SittingBodyBalanceData.h"
#include "ReachForBodyData.h"
#include "BalanceAssistantData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct SupportPolygonUpdatePackage;
struct SupportPolygonFeedbackPackage;
class SupportPolygon_StandingCon;
struct SupportPolygonUpdatePackage;
struct SupportPolygonFeedbackPackage;
class SupportPolygon_SittingCon;
struct BodyBalanceUpdatePackage;
struct BodyBalanceFeedbackPackage;
class BodyBalance_Con;
struct SittingBodyBalanceUpdatePackage;
struct SittingBodyBalanceFeedbackPackage;
class SittingBodyBalance_Con;
struct ReachForBodyUpdatePackage;
struct ReachForBodyFeedbackPackage;
class ReachForBody_Con;
struct BalanceAssistantUpdatePackage;
struct BalanceAssistantFeedbackPackage;
class BalanceAssistant_Con;

//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct BodyFrameAPIBase
{
  BodyFrameAPIBase(
    const BodyFrameData* const _data, 
    const BodyFrameInputs* const _in, 
    const BodyFrameFeedbackInputs* const _feedIn, 
    const MyNetworkAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const BodyFrameData* const data;
  const BodyFrameInputs* const in;
  const BodyFrameFeedbackInputs* const feedIn;

  const MyNetworkAPIBase* const owner;

  BodyFrameAPIBase(const BodyFrameAPIBase& rhs);
  BodyFrameAPIBase& operator = (const BodyFrameAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct BodyFrameUpdatePackage : public BodyFrameAPIBase
{
  BodyFrameUpdatePackage(
    BodyFrameData* const _data, 
    const BodyFrameInputs* const _in, 
    const BodyFrameFeedbackInputs* const _feedIn, 
    BodyFrameOutputs* const _out, 
    const MyNetworkAPIBase* const _owner  ) : BodyFrameAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  BodyFrameData* const data;

  BodyFrameOutputs* const out;


  // No update api required for this module!


  BodyFrameUpdatePackage(const BodyFrameUpdatePackage& rhs);
  BodyFrameUpdatePackage& operator = (const BodyFrameUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct BodyFrameFeedbackPackage : public BodyFrameAPIBase
{
  BodyFrameFeedbackPackage(
    BodyFrameData* const _data, 
    const BodyFrameFeedbackInputs* const _feedIn, 
    const BodyFrameInputs* const _in, 
    BodyFrameFeedbackOutputs* const _feedOut, 
    const MyNetworkAPIBase* const _owner  ) : BodyFrameAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  BodyFrameData* const data;

  BodyFrameFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  BodyFrameFeedbackPackage(const BodyFrameFeedbackPackage& rhs);
  BodyFrameFeedbackPackage& operator = (const BodyFrameFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BODYFRAME_PKG_H

