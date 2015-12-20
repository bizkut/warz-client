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

#ifndef NM_MDF_HEADDODGE_PKG_H
#define NM_MDF_HEADDODGE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadDodge.module"

#include "HeadDodgeData.h"

#include "HeadPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct HeadDodgeAPIBase
{
  HeadDodgeAPIBase(
    const HeadDodgeData* const _data, 
    const HeadDodgeInputs* const _in, 
    const HeadAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const HeadDodgeData* const data;
  const HeadDodgeInputs* const in;

  const HeadAPIBase* const owner;

  HeadDodgeAPIBase(const HeadDodgeAPIBase& rhs);
  HeadDodgeAPIBase& operator = (const HeadDodgeAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct HeadDodgeUpdatePackage : public HeadDodgeAPIBase
{
  HeadDodgeUpdatePackage(
    HeadDodgeData* const _data, 
    const HeadDodgeInputs* const _in, 
    HeadDodgeOutputs* const _out, 
    const HeadAPIBase* const _owner  ) : HeadDodgeAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  HeadDodgeData* const data;

  HeadDodgeOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadDodgeUpdatePackage(const HeadDodgeUpdatePackage& rhs);
  HeadDodgeUpdatePackage& operator = (const HeadDodgeUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct HeadDodgeFeedbackPackage : public HeadDodgeAPIBase
{
  HeadDodgeFeedbackPackage(
    HeadDodgeData* const _data, 
    const HeadDodgeInputs* const _in, 
    HeadDodgeFeedbackOutputs* const _feedOut, 
    const HeadAPIBase* const _owner  ) : HeadDodgeAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  HeadDodgeData* const data;

  HeadDodgeFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadDodgeFeedbackPackage(const HeadDodgeFeedbackPackage& rhs);
  HeadDodgeFeedbackPackage& operator = (const HeadDodgeFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HEADDODGE_PKG_H

