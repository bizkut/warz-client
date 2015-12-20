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

#ifndef NM_MDF_LEGSWING_PKG_H
#define NM_MDF_LEGSWING_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/LegSwing.module"

#include "LegSwingData.h"

#include "LegPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct LegSwingAPIBase
{
  LegSwingAPIBase(
    const LegSwingData* const _data, 
    const LegSwingInputs* const _in, 
    const LegAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const LegSwingData* const data;
  const LegSwingInputs* const in;

  const LegAPIBase* const owner;

  LegSwingAPIBase(const LegSwingAPIBase& rhs);
  LegSwingAPIBase& operator = (const LegSwingAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct LegSwingUpdatePackage : public LegSwingAPIBase
{
  LegSwingUpdatePackage(
    LegSwingData* const _data, 
    const LegSwingInputs* const _in, 
    LegSwingOutputs* const _out, 
    const LegAPIBase* const _owner  ) : LegSwingAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  LegSwingData* const data;

  LegSwingOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegSwingUpdatePackage(const LegSwingUpdatePackage& rhs);
  LegSwingUpdatePackage& operator = (const LegSwingUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct LegSwingFeedbackPackage : public LegSwingAPIBase
{
  LegSwingFeedbackPackage(
    LegSwingData* const _data, 
    const LegSwingInputs* const _in, 
    const LegAPIBase* const _owner  ) : LegSwingAPIBase(_data ,_in ,_owner ), data(_data) 
  {
  }

  LegSwingData* const data;



  // No update api required for this module!


  LegSwingFeedbackPackage(const LegSwingFeedbackPackage& rhs);
  LegSwingFeedbackPackage& operator = (const LegSwingFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEGSWING_PKG_H

