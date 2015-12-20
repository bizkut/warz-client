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

#ifndef NM_MDF_SPINEWRITHE_PKG_H
#define NM_MDF_SPINEWRITHE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/SpineWrithe.module"

#include "SpineWritheData.h"

#include "SpinePackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct SpineWritheAPIBase
{
  SpineWritheAPIBase(
    const SpineWritheData* const _data, 
    const SpineWritheInputs* const _in, 
    const SpineAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const SpineWritheData* const data;
  const SpineWritheInputs* const in;

  const SpineAPIBase* const owner;

  SpineWritheAPIBase(const SpineWritheAPIBase& rhs);
  SpineWritheAPIBase& operator = (const SpineWritheAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct SpineWritheUpdatePackage : public SpineWritheAPIBase
{
  SpineWritheUpdatePackage(
    SpineWritheData* const _data, 
    const SpineWritheInputs* const _in, 
    SpineWritheOutputs* const _out, 
    const SpineAPIBase* const _owner  ) : SpineWritheAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  SpineWritheData* const data;

  SpineWritheOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  SpineWritheUpdatePackage(const SpineWritheUpdatePackage& rhs);
  SpineWritheUpdatePackage& operator = (const SpineWritheUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct SpineWritheFeedbackPackage : public SpineWritheAPIBase
{
  SpineWritheFeedbackPackage(
    SpineWritheData* const _data, 
    const SpineWritheInputs* const _in, 
    const SpineAPIBase* const _owner  ) : SpineWritheAPIBase(_data ,_in ,_owner ), data(_data) 
  {
  }

  SpineWritheData* const data;



  // No update api required for this module!


  SpineWritheFeedbackPackage(const SpineWritheFeedbackPackage& rhs);
  SpineWritheFeedbackPackage& operator = (const SpineWritheFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SPINEWRITHE_PKG_H

