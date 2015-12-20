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

#ifndef NM_MDF_LEGWRITHE_PKG_H
#define NM_MDF_LEGWRITHE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/LegWrithe.module"

#include "LegWritheData.h"

#include "LegPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct LegWritheAPIBase
{
  LegWritheAPIBase(
    const LegWritheData* const _data, 
    const LegWritheInputs* const _in, 
    const LegAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const LegWritheData* const data;
  const LegWritheInputs* const in;

  const LegAPIBase* const owner;

  LegWritheAPIBase(const LegWritheAPIBase& rhs);
  LegWritheAPIBase& operator = (const LegWritheAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct LegWritheUpdatePackage : public LegWritheAPIBase
{
  LegWritheUpdatePackage(
    LegWritheData* const _data, 
    const LegWritheInputs* const _in, 
    LegWritheOutputs* const _out, 
    const LegAPIBase* const _owner  ) : LegWritheAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  LegWritheData* const data;

  LegWritheOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  LegWritheUpdatePackage(const LegWritheUpdatePackage& rhs);
  LegWritheUpdatePackage& operator = (const LegWritheUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct LegWritheFeedbackPackage : public LegWritheAPIBase
{
  LegWritheFeedbackPackage(
    LegWritheData* const _data, 
    const LegWritheInputs* const _in, 
    const LegAPIBase* const _owner  ) : LegWritheAPIBase(_data ,_in ,_owner ), data(_data) 
  {
  }

  LegWritheData* const data;



  // No update api required for this module!


  LegWritheFeedbackPackage(const LegWritheFeedbackPackage& rhs);
  LegWritheFeedbackPackage& operator = (const LegWritheFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEGWRITHE_PKG_H

