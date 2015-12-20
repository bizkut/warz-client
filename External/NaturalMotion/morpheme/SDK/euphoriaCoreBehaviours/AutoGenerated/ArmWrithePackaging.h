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

#ifndef NM_MDF_ARMWRITHE_PKG_H
#define NM_MDF_ARMWRITHE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmWrithe.module"

#include "ArmWritheData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmWritheAPIBase
{
  ArmWritheAPIBase(
    const ArmWritheData* const _data, 
    const ArmWritheInputs* const _in, 
    const ArmAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const ArmWritheData* const data;
  const ArmWritheInputs* const in;

  const ArmAPIBase* const owner;

  ArmWritheAPIBase(const ArmWritheAPIBase& rhs);
  ArmWritheAPIBase& operator = (const ArmWritheAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmWritheUpdatePackage : public ArmWritheAPIBase
{
  ArmWritheUpdatePackage(
    ArmWritheData* const _data, 
    const ArmWritheInputs* const _in, 
    ArmWritheOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmWritheAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  ArmWritheData* const data;

  ArmWritheOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmWritheUpdatePackage(const ArmWritheUpdatePackage& rhs);
  ArmWritheUpdatePackage& operator = (const ArmWritheUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmWritheFeedbackPackage : public ArmWritheAPIBase
{
  ArmWritheFeedbackPackage(
    ArmWritheData* const _data, 
    const ArmWritheInputs* const _in, 
    const ArmAPIBase* const _owner  ) : ArmWritheAPIBase(_data ,_in ,_owner ), data(_data) 
  {
  }

  ArmWritheData* const data;



  // No update api required for this module!


  ArmWritheFeedbackPackage(const ArmWritheFeedbackPackage& rhs);
  ArmWritheFeedbackPackage& operator = (const ArmWritheFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMWRITHE_PKG_H

