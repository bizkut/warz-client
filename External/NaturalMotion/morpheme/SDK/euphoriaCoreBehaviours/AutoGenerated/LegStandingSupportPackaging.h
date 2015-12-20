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

#ifndef NM_MDF_LEGSTANDINGSUPPORT_PKG_H
#define NM_MDF_LEGSTANDINGSUPPORT_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/LegStandingSupport.module"

#include "LegStandingSupportData.h"

#include "LegPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct LegStandingSupportAPIBase
{
  LegStandingSupportAPIBase(
    const LegStandingSupportData* const _data, 
    const LegStandingSupportInputs* const _in, 
    const LegAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const LegStandingSupportData* const data;
  const LegStandingSupportInputs* const in;

  const LegAPIBase* const owner;

  LegStandingSupportAPIBase(const LegStandingSupportAPIBase& rhs);
  LegStandingSupportAPIBase& operator = (const LegStandingSupportAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct LegStandingSupportUpdatePackage : public LegStandingSupportAPIBase
{
  LegStandingSupportUpdatePackage(
    LegStandingSupportData* const _data, 
    const LegStandingSupportInputs* const _in, 
    LegStandingSupportOutputs* const _out, 
    const LegAPIBase* const _owner  ) : LegStandingSupportAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  LegStandingSupportData* const data;

  LegStandingSupportOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegStandingSupportUpdatePackage(const LegStandingSupportUpdatePackage& rhs);
  LegStandingSupportUpdatePackage& operator = (const LegStandingSupportUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct LegStandingSupportFeedbackPackage : public LegStandingSupportAPIBase
{
  LegStandingSupportFeedbackPackage(
    LegStandingSupportData* const _data, 
    const LegStandingSupportInputs* const _in, 
    LegStandingSupportFeedbackOutputs* const _feedOut, 
    const LegAPIBase* const _owner  ) : LegStandingSupportAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  LegStandingSupportData* const data;

  LegStandingSupportFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegStandingSupportFeedbackPackage(const LegStandingSupportFeedbackPackage& rhs);
  LegStandingSupportFeedbackPackage& operator = (const LegStandingSupportFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEGSTANDINGSUPPORT_PKG_H

