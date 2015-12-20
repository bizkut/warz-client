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

#ifndef NM_MDF_HAZARDRESPONSE_PKG_H
#define NM_MDF_HAZARDRESPONSE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/HazardResponse.module"

#include "HazardResponseData.h"

#include "HazardManagementPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct HazardResponseAPIBase
{
  HazardResponseAPIBase(
    const HazardResponseData* const _data, 
    const HazardResponseInputs* const _in, 
    const HazardResponseFeedbackInputs* const _feedIn, 
    const HazardManagementAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const HazardResponseData* const data;
  const HazardResponseInputs* const in;
  const HazardResponseFeedbackInputs* const feedIn;

  const HazardManagementAPIBase* const owner;

  HazardResponseAPIBase(const HazardResponseAPIBase& rhs);
  HazardResponseAPIBase& operator = (const HazardResponseAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct HazardResponseUpdatePackage : public HazardResponseAPIBase
{
  HazardResponseUpdatePackage(
    HazardResponseData* const _data, 
    const HazardResponseInputs* const _in, 
    const HazardResponseFeedbackInputs* const _feedIn, 
    HazardResponseOutputs* const _out, 
    const HazardManagementAPIBase* const _owner  ) : HazardResponseAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  HazardResponseData* const data;

  HazardResponseOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  HazardResponseUpdatePackage(const HazardResponseUpdatePackage& rhs);
  HazardResponseUpdatePackage& operator = (const HazardResponseUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct HazardResponseFeedbackPackage : public HazardResponseAPIBase
{
  HazardResponseFeedbackPackage(
    HazardResponseData* const _data, 
    const HazardResponseFeedbackInputs* const _feedIn, 
    const HazardResponseInputs* const _in, 
    const HazardManagementAPIBase* const _owner  ) : HazardResponseAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data) 
  {
  }

  HazardResponseData* const data;



  // No update api required for this module!


  HazardResponseFeedbackPackage(const HazardResponseFeedbackPackage& rhs);
  HazardResponseFeedbackPackage& operator = (const HazardResponseFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HAZARDRESPONSE_PKG_H

