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

#ifndef NM_MDF_HEADEYES_PKG_H
#define NM_MDF_HEADEYES_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadEyes.module"

#include "HeadEyesData.h"

#include "HeadPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct HeadEyesAPIBase
{
  HeadEyesAPIBase(
    const HeadEyesData* const _data, 
    const HeadEyesInputs* const _in, 
    const HeadAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const HeadEyesData* const data;
  const HeadEyesInputs* const in;

  const HeadAPIBase* const owner;

  HeadEyesAPIBase(const HeadEyesAPIBase& rhs);
  HeadEyesAPIBase& operator = (const HeadEyesAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct HeadEyesUpdatePackage : public HeadEyesAPIBase
{
  HeadEyesUpdatePackage(
    HeadEyesData* const _data, 
    const HeadEyesInputs* const _in, 
    HeadEyesOutputs* const _out, 
    const HeadAPIBase* const _owner  ) : HeadEyesAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  HeadEyesData* const data;

  HeadEyesOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadEyesUpdatePackage(const HeadEyesUpdatePackage& rhs);
  HeadEyesUpdatePackage& operator = (const HeadEyesUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct HeadEyesFeedbackPackage : public HeadEyesAPIBase
{
  HeadEyesFeedbackPackage(
    HeadEyesData* const _data, 
    const HeadEyesInputs* const _in, 
    HeadEyesFeedbackOutputs* const _feedOut, 
    const HeadAPIBase* const _owner  ) : HeadEyesAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  HeadEyesData* const data;

  HeadEyesFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadEyesFeedbackPackage(const HeadEyesFeedbackPackage& rhs);
  HeadEyesFeedbackPackage& operator = (const HeadEyesFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HEADEYES_PKG_H

