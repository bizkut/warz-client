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

#ifndef NM_MDF_HAZARDMANAGEMENT_PKG_H
#define NM_MDF_HAZARDMANAGEMENT_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/HazardManagement.module"

#include "HazardManagementData.h"
#include "GrabDetectionData.h"
#include "GrabData.h"
#include "HazardResponseData.h"
#include "FreeFallManagementData.h"
#include "ImpactPredictorData.h"
#include "ShieldManagementData.h"

#include "MyNetworkPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct GrabDetectionUpdatePackage;
struct GrabDetectionFeedbackPackage;
class GrabDetection_Con;
struct GrabUpdatePackage;
struct GrabFeedbackPackage;
class Grab_Con;
struct HazardResponseUpdatePackage;
struct HazardResponseFeedbackPackage;
class HazardResponse_Con;
struct FreeFallManagementUpdatePackage;
struct FreeFallManagementFeedbackPackage;
class FreeFallManagement_Con;
struct ImpactPredictorUpdatePackage;
struct ImpactPredictorFeedbackPackage;
class ImpactPredictor_chestCon;
struct ShieldManagementUpdatePackage;
struct ShieldManagementFeedbackPackage;
class ShieldManagement_Con;

//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct HazardManagementAPIBase
{
  HazardManagementAPIBase(
    const HazardManagementFeedbackInputs* const _feedIn, 
    const MyNetworkAPIBase* const _owner  ) :feedIn(_feedIn) ,owner(_owner)  {}

  const HazardManagementFeedbackInputs* const feedIn;

  const MyNetworkAPIBase* const owner;

  HazardManagementAPIBase(const HazardManagementAPIBase& rhs);
  HazardManagementAPIBase& operator = (const HazardManagementAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct HazardManagementUpdatePackage : public HazardManagementAPIBase
{
  HazardManagementUpdatePackage(
    const HazardManagementFeedbackInputs* const _feedIn, 
    HazardManagementOutputs* const _out, 
    const MyNetworkAPIBase* const _owner  ) : HazardManagementAPIBase(_feedIn ,_owner ), out(_out) 
  {
  }


  HazardManagementOutputs* const out;


  // No update api required for this module!


  HazardManagementUpdatePackage(const HazardManagementUpdatePackage& rhs);
  HazardManagementUpdatePackage& operator = (const HazardManagementUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct HazardManagementFeedbackPackage : public HazardManagementAPIBase
{
  HazardManagementFeedbackPackage(
    const HazardManagementFeedbackInputs* const _feedIn, 
    const MyNetworkAPIBase* const _owner  ) : HazardManagementAPIBase(_feedIn ,_owner ) 
  {
  }




  // No update api required for this module!


  HazardManagementFeedbackPackage(const HazardManagementFeedbackPackage& rhs);
  HazardManagementFeedbackPackage& operator = (const HazardManagementFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HAZARDMANAGEMENT_PKG_H

