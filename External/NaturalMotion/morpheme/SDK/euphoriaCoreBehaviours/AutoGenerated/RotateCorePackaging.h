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

#ifndef NM_MDF_ROTATECORE_PKG_H
#define NM_MDF_ROTATECORE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/RotateCore.module"

#include "RotateCoreData.h"

#include "BodySectionPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct RotateCoreAPIBase
{
  RotateCoreAPIBase(
    const RotateCoreInputs* const _in, 
    const RotateCoreFeedbackInputs* const _feedIn, 
    const BodySectionAPIBase* const _owner  ) :in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const RotateCoreInputs* const in;
  const RotateCoreFeedbackInputs* const feedIn;

  const BodySectionAPIBase* const owner;

  RotateCoreAPIBase(const RotateCoreAPIBase& rhs);
  RotateCoreAPIBase& operator = (const RotateCoreAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct RotateCoreUpdatePackage : public RotateCoreAPIBase
{
  RotateCoreUpdatePackage(
    const RotateCoreInputs* const _in, 
    const RotateCoreFeedbackInputs* const _feedIn, 
    RotateCoreOutputs* const _out, 
    const BodySectionAPIBase* const _owner  ) : RotateCoreAPIBase(_in ,_feedIn ,_owner ), out(_out) 
  {
  }


  RotateCoreOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  RotateCoreUpdatePackage(const RotateCoreUpdatePackage& rhs);
  RotateCoreUpdatePackage& operator = (const RotateCoreUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct RotateCoreFeedbackPackage : public RotateCoreAPIBase
{
  RotateCoreFeedbackPackage(
    const RotateCoreFeedbackInputs* const _feedIn, 
    const RotateCoreInputs* const _in, 
    RotateCoreFeedbackOutputs* const _feedOut, 
    const BodySectionAPIBase* const _owner  ) : RotateCoreAPIBase(_in ,_feedIn ,_owner ), feedOut(_feedOut) 
  {
  }


  RotateCoreFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  RotateCoreFeedbackPackage(const RotateCoreFeedbackPackage& rhs);
  RotateCoreFeedbackPackage& operator = (const RotateCoreFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ROTATECORE_PKG_H

