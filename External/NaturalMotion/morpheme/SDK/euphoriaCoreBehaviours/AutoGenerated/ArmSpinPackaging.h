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

#ifndef NM_MDF_ARMSPIN_PKG_H
#define NM_MDF_ARMSPIN_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmSpin.module"

#include "ArmSpinData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmSpinAPIBase
{
  ArmSpinAPIBase(
    const ArmSpinData* const _data, 
    const ArmSpinInputs* const _in, 
    const ArmAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const ArmSpinData* const data;
  const ArmSpinInputs* const in;

  const ArmAPIBase* const owner;

  ArmSpinAPIBase(const ArmSpinAPIBase& rhs);
  ArmSpinAPIBase& operator = (const ArmSpinAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmSpinUpdatePackage : public ArmSpinAPIBase
{
  ArmSpinUpdatePackage(
    ArmSpinData* const _data, 
    const ArmSpinInputs* const _in, 
    ArmSpinOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmSpinAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  ArmSpinData* const data;

  ArmSpinOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmSpinUpdatePackage(const ArmSpinUpdatePackage& rhs);
  ArmSpinUpdatePackage& operator = (const ArmSpinUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmSpinFeedbackPackage : public ArmSpinAPIBase
{
  ArmSpinFeedbackPackage(
    ArmSpinData* const _data, 
    const ArmSpinInputs* const _in, 
    ArmSpinFeedbackOutputs* const _feedOut, 
    const ArmAPIBase* const _owner  ) : ArmSpinAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ArmSpinData* const data;

  ArmSpinFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmSpinFeedbackPackage(const ArmSpinFeedbackPackage& rhs);
  ArmSpinFeedbackPackage& operator = (const ArmSpinFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMSPIN_PKG_H

