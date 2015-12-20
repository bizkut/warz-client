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

#ifndef NM_MDF_ARMSTANDINGSUPPORT_PKG_H
#define NM_MDF_ARMSTANDINGSUPPORT_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmStandingSupport.module"

#include "ArmStandingSupportData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmStandingSupportAPIBase
{
  ArmStandingSupportAPIBase(
    const ArmStandingSupportData* const _data, 
    const ArmStandingSupportInputs* const _in, 
    const ArmAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const ArmStandingSupportData* const data;
  const ArmStandingSupportInputs* const in;

  const ArmAPIBase* const owner;

  ArmStandingSupportAPIBase(const ArmStandingSupportAPIBase& rhs);
  ArmStandingSupportAPIBase& operator = (const ArmStandingSupportAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmStandingSupportUpdatePackage : public ArmStandingSupportAPIBase
{
  ArmStandingSupportUpdatePackage(
    ArmStandingSupportData* const _data, 
    const ArmStandingSupportInputs* const _in, 
    ArmStandingSupportOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmStandingSupportAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  ArmStandingSupportData* const data;

  ArmStandingSupportOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmStandingSupportUpdatePackage(const ArmStandingSupportUpdatePackage& rhs);
  ArmStandingSupportUpdatePackage& operator = (const ArmStandingSupportUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmStandingSupportFeedbackPackage : public ArmStandingSupportAPIBase
{
  ArmStandingSupportFeedbackPackage(
    ArmStandingSupportData* const _data, 
    const ArmStandingSupportInputs* const _in, 
    ArmStandingSupportFeedbackOutputs* const _feedOut, 
    const ArmAPIBase* const _owner  ) : ArmStandingSupportAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ArmStandingSupportData* const data;

  ArmStandingSupportFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmStandingSupportFeedbackPackage(const ArmStandingSupportFeedbackPackage& rhs);
  ArmStandingSupportFeedbackPackage& operator = (const ArmStandingSupportFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMSTANDINGSUPPORT_PKG_H

