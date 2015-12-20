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

#ifndef NM_MDF_REACHFORBODY_PKG_H
#define NM_MDF_REACHFORBODY_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ReachForBody.module"

#include "ReachForBodyData.h"

#include "BodyFramePackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ReachForBodyAPIBase
{
  ReachForBodyAPIBase(
    const ReachForBodyData* const _data, 
    const ReachForBodyInputs* const _in, 
    const ReachForBodyFeedbackInputs* const _feedIn, 
    const BodyFrameAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const ReachForBodyData* const data;
  const ReachForBodyInputs* const in;
  const ReachForBodyFeedbackInputs* const feedIn;

  const BodyFrameAPIBase* const owner;

  ReachForBodyAPIBase(const ReachForBodyAPIBase& rhs);
  ReachForBodyAPIBase& operator = (const ReachForBodyAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ReachForBodyUpdatePackage : public ReachForBodyAPIBase
{
  ReachForBodyUpdatePackage(
    ReachForBodyData* const _data, 
    const ReachForBodyInputs* const _in, 
    const ReachForBodyFeedbackInputs* const _feedIn, 
    ReachForBodyOutputs* const _out, 
    const BodyFrameAPIBase* const _owner  ) : ReachForBodyAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  ReachForBodyData* const data;

  ReachForBodyOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ReachForBodyUpdatePackage(const ReachForBodyUpdatePackage& rhs);
  ReachForBodyUpdatePackage& operator = (const ReachForBodyUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ReachForBodyFeedbackPackage : public ReachForBodyAPIBase
{
  ReachForBodyFeedbackPackage(
    ReachForBodyData* const _data, 
    const ReachForBodyFeedbackInputs* const _feedIn, 
    const ReachForBodyInputs* const _in, 
    ReachForBodyFeedbackOutputs* const _feedOut, 
    const BodyFrameAPIBase* const _owner  ) : ReachForBodyAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ReachForBodyData* const data;

  ReachForBodyFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ReachForBodyFeedbackPackage(const ReachForBodyFeedbackPackage& rhs);
  ReachForBodyFeedbackPackage& operator = (const ReachForBodyFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_REACHFORBODY_PKG_H

