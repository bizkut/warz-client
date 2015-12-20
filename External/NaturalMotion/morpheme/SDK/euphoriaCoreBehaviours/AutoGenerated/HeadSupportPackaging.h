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

#ifndef NM_MDF_HEADSUPPORT_PKG_H
#define NM_MDF_HEADSUPPORT_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadSupport.module"

#include "HeadSupportData.h"

#include "HeadPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct HeadSupportAPIBase
{
  HeadSupportAPIBase(
    const HeadSupportInputs* const _in, 
    const HeadAPIBase* const _owner  ) :in(_in) ,owner(_owner)  {}

  const HeadSupportInputs* const in;

  const HeadAPIBase* const owner;

  HeadSupportAPIBase(const HeadSupportAPIBase& rhs);
  HeadSupportAPIBase& operator = (const HeadSupportAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct HeadSupportUpdatePackage : public HeadSupportAPIBase
{
  HeadSupportUpdatePackage(
    const HeadSupportInputs* const _in, 
    HeadSupportOutputs* const _out, 
    const HeadAPIBase* const _owner  ) : HeadSupportAPIBase(_in ,_owner ), out(_out) 
  {
  }


  HeadSupportOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HeadSupportUpdatePackage(const HeadSupportUpdatePackage& rhs);
  HeadSupportUpdatePackage& operator = (const HeadSupportUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct HeadSupportFeedbackPackage : public HeadSupportAPIBase
{
  HeadSupportFeedbackPackage(
    const HeadSupportInputs* const _in, 
    const HeadAPIBase* const _owner  ) : HeadSupportAPIBase(_in ,_owner ) 
  {
  }




  // No update api required for this module!


  HeadSupportFeedbackPackage(const HeadSupportFeedbackPackage& rhs);
  HeadSupportFeedbackPackage& operator = (const HeadSupportFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HEADSUPPORT_PKG_H

