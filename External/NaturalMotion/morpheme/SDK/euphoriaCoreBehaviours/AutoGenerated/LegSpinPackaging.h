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

#ifndef NM_MDF_LEGSPIN_PKG_H
#define NM_MDF_LEGSPIN_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/LegSpin.module"

#include "LegSpinData.h"

#include "LegPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct LegSpinAPIBase
{
  LegSpinAPIBase(
    const LegSpinData* const _data, 
    const LegSpinInputs* const _in, 
    const LegAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const LegSpinData* const data;
  const LegSpinInputs* const in;

  const LegAPIBase* const owner;

  LegSpinAPIBase(const LegSpinAPIBase& rhs);
  LegSpinAPIBase& operator = (const LegSpinAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct LegSpinUpdatePackage : public LegSpinAPIBase
{
  LegSpinUpdatePackage(
    LegSpinData* const _data, 
    const LegSpinInputs* const _in, 
    LegSpinOutputs* const _out, 
    const LegAPIBase* const _owner  ) : LegSpinAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  LegSpinData* const data;

  LegSpinOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegSpinUpdatePackage(const LegSpinUpdatePackage& rhs);
  LegSpinUpdatePackage& operator = (const LegSpinUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct LegSpinFeedbackPackage : public LegSpinAPIBase
{
  LegSpinFeedbackPackage(
    LegSpinData* const _data, 
    const LegSpinInputs* const _in, 
    LegSpinFeedbackOutputs* const _feedOut, 
    const LegAPIBase* const _owner  ) : LegSpinAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  LegSpinData* const data;

  LegSpinFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegSpinFeedbackPackage(const LegSpinFeedbackPackage& rhs);
  LegSpinFeedbackPackage& operator = (const LegSpinFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEGSPIN_PKG_H

