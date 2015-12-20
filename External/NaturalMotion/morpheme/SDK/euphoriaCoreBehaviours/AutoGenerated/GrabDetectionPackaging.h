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

#ifndef NM_MDF_GRABDETECTION_PKG_H
#define NM_MDF_GRABDETECTION_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/GrabDetection.module"

#include "GrabDetectionData.h"

#include "HazardManagementPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct GrabDetectionAPIBase
{
  GrabDetectionAPIBase(
    const GrabDetectionData* const _data, 
    const GrabDetectionInputs* const _in, 
    const GrabDetectionFeedbackInputs* const _feedIn, 
    const HazardManagementAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const GrabDetectionData* const data;
  const GrabDetectionInputs* const in;
  const GrabDetectionFeedbackInputs* const feedIn;

  const HazardManagementAPIBase* const owner;

  GrabDetectionAPIBase(const GrabDetectionAPIBase& rhs);
  GrabDetectionAPIBase& operator = (const GrabDetectionAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_GRABDETECTION_PKG_H

