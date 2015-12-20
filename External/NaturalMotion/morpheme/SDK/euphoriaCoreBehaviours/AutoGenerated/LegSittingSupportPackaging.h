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

#ifndef NM_MDF_LEGSITTINGSUPPORT_PKG_H
#define NM_MDF_LEGSITTINGSUPPORT_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/LegSittingSupport.module"

#include "LegSittingSupportData.h"

#include "LegPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct LegSittingSupportAPIBase
{
  LegSittingSupportAPIBase(
    const LegSittingSupportData* const _data, 
    const LegSittingSupportInputs* const _in, 
    const LegAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const LegSittingSupportData* const data;
  const LegSittingSupportInputs* const in;

  const LegAPIBase* const owner;

  LegSittingSupportAPIBase(const LegSittingSupportAPIBase& rhs);
  LegSittingSupportAPIBase& operator = (const LegSittingSupportAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct LegSittingSupportUpdatePackage : public LegSittingSupportAPIBase
{
  LegSittingSupportUpdatePackage(
    LegSittingSupportData* const _data, 
    const LegSittingSupportInputs* const _in, 
    LegSittingSupportOutputs* const _out, 
    const LegAPIBase* const _owner  ) : LegSittingSupportAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  LegSittingSupportData* const data;

  LegSittingSupportOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegSittingSupportUpdatePackage(const LegSittingSupportUpdatePackage& rhs);
  LegSittingSupportUpdatePackage& operator = (const LegSittingSupportUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct LegSittingSupportFeedbackPackage : public LegSittingSupportAPIBase
{
  LegSittingSupportFeedbackPackage(
    LegSittingSupportData* const _data, 
    const LegSittingSupportInputs* const _in, 
    LegSittingSupportFeedbackOutputs* const _feedOut, 
    const LegAPIBase* const _owner  ) : LegSittingSupportAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  LegSittingSupportData* const data;

  LegSittingSupportFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegSittingSupportFeedbackPackage(const LegSittingSupportFeedbackPackage& rhs);
  LegSittingSupportFeedbackPackage& operator = (const LegSittingSupportFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEGSITTINGSUPPORT_PKG_H

