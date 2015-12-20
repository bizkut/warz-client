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

#ifndef NM_MDF_IMPACTPREDICTOR_PKG_H
#define NM_MDF_IMPACTPREDICTOR_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ImpactPredictor.module"

#include "ImpactPredictorData.h"

#include "HazardManagementPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ImpactPredictorAPIBase
{
  ImpactPredictorAPIBase(
    const ImpactPredictorData* const _data, 
    const ImpactPredictorInputs* const _in, 
    const ImpactPredictorFeedbackInputs* const _feedIn, 
    const HazardManagementAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const ImpactPredictorData* const data;
  const ImpactPredictorInputs* const in;
  const ImpactPredictorFeedbackInputs* const feedIn;

  const HazardManagementAPIBase* const owner;

  ImpactPredictorAPIBase(const ImpactPredictorAPIBase& rhs);
  ImpactPredictorAPIBase& operator = (const ImpactPredictorAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ImpactPredictorUpdatePackage : public ImpactPredictorAPIBase
{
  ImpactPredictorUpdatePackage(
    ImpactPredictorData* const _data, 
    const ImpactPredictorInputs* const _in, 
    const ImpactPredictorFeedbackInputs* const _feedIn, 
    ImpactPredictorOutputs* const _out, 
    const HazardManagementAPIBase* const _owner  ) : ImpactPredictorAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  ImpactPredictorData* const data;

  ImpactPredictorOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ImpactPredictorUpdatePackage(const ImpactPredictorUpdatePackage& rhs);
  ImpactPredictorUpdatePackage& operator = (const ImpactPredictorUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ImpactPredictorFeedbackPackage : public ImpactPredictorAPIBase
{
  ImpactPredictorFeedbackPackage(
    ImpactPredictorData* const _data, 
    const ImpactPredictorFeedbackInputs* const _feedIn, 
    const ImpactPredictorInputs* const _in, 
    ImpactPredictorFeedbackOutputs* const _feedOut, 
    const HazardManagementAPIBase* const _owner  ) : ImpactPredictorAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ImpactPredictorData* const data;

  ImpactPredictorFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ImpactPredictorFeedbackPackage(const ImpactPredictorFeedbackPackage& rhs);
  ImpactPredictorFeedbackPackage& operator = (const ImpactPredictorFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_IMPACTPREDICTOR_PKG_H

