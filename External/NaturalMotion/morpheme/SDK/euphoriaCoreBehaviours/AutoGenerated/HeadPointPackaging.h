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

#ifndef NM_MDF_HEADPOINT_PKG_H
#define NM_MDF_HEADPOINT_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadPoint.module"

#include "HeadPointData.h"

#include "HeadPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct HeadPointAPIBase
{
  HeadPointAPIBase(
    const HeadPointData* const _data, 
    const HeadPointInputs* const _in, 
    const HeadAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const HeadPointData* const data;
  const HeadPointInputs* const in;

  const HeadAPIBase* const owner;

  HeadPointAPIBase(const HeadPointAPIBase& rhs);
  HeadPointAPIBase& operator = (const HeadPointAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct HeadPointUpdatePackage : public HeadPointAPIBase
{
  HeadPointUpdatePackage(
    HeadPointData* const _data, 
    const HeadPointInputs* const _in, 
    HeadPointOutputs* const _out, 
    const HeadAPIBase* const _owner  ) : HeadPointAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  HeadPointData* const data;

  HeadPointOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadPointUpdatePackage(const HeadPointUpdatePackage& rhs);
  HeadPointUpdatePackage& operator = (const HeadPointUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct HeadPointFeedbackPackage : public HeadPointAPIBase
{
  HeadPointFeedbackPackage(
    HeadPointData* const _data, 
    const HeadPointInputs* const _in, 
    HeadPointFeedbackOutputs* const _feedOut, 
    const HeadAPIBase* const _owner  ) : HeadPointAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  HeadPointData* const data;

  HeadPointFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadPointFeedbackPackage(const HeadPointFeedbackPackage& rhs);
  HeadPointFeedbackPackage& operator = (const HeadPointFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HEADPOINT_PKG_H

