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

#ifndef NM_MDF_HEADAVOID_PKG_H
#define NM_MDF_HEADAVOID_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadAvoid.module"

#include "HeadAvoidData.h"

#include "HeadPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct HeadAvoidAPIBase
{
  HeadAvoidAPIBase(
    const HeadAvoidData* const _data, 
    const HeadAvoidInputs* const _in, 
    const HeadAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const HeadAvoidData* const data;
  const HeadAvoidInputs* const in;

  const HeadAPIBase* const owner;

  HeadAvoidAPIBase(const HeadAvoidAPIBase& rhs);
  HeadAvoidAPIBase& operator = (const HeadAvoidAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct HeadAvoidUpdatePackage : public HeadAvoidAPIBase
{
  HeadAvoidUpdatePackage(
    HeadAvoidData* const _data, 
    const HeadAvoidInputs* const _in, 
    HeadAvoidOutputs* const _out, 
    const HeadAPIBase* const _owner  ) : HeadAvoidAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  HeadAvoidData* const data;

  HeadAvoidOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadAvoidUpdatePackage(const HeadAvoidUpdatePackage& rhs);
  HeadAvoidUpdatePackage& operator = (const HeadAvoidUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct HeadAvoidFeedbackPackage : public HeadAvoidAPIBase
{
  HeadAvoidFeedbackPackage(
    HeadAvoidData* const _data, 
    const HeadAvoidInputs* const _in, 
    HeadAvoidFeedbackOutputs* const _feedOut, 
    const HeadAPIBase* const _owner  ) : HeadAvoidAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  HeadAvoidData* const data;

  HeadAvoidFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadAvoidFeedbackPackage(const HeadAvoidFeedbackPackage& rhs);
  HeadAvoidFeedbackPackage& operator = (const HeadAvoidFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HEADAVOID_PKG_H

