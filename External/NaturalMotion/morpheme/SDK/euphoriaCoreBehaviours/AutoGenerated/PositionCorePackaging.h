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

#ifndef NM_MDF_POSITIONCORE_PKG_H
#define NM_MDF_POSITIONCORE_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/PositionCore.module"

#include "PositionCoreData.h"

#include "BodySectionPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct PositionCoreAPIBase
{
  PositionCoreAPIBase(
    const PositionCoreInputs* const _in, 
    const PositionCoreFeedbackInputs* const _feedIn, 
    const BodySectionAPIBase* const _owner  ) :in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const PositionCoreInputs* const in;
  const PositionCoreFeedbackInputs* const feedIn;

  const BodySectionAPIBase* const owner;

  PositionCoreAPIBase(const PositionCoreAPIBase& rhs);
  PositionCoreAPIBase& operator = (const PositionCoreAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct PositionCoreUpdatePackage : public PositionCoreAPIBase
{
  PositionCoreUpdatePackage(
    const PositionCoreInputs* const _in, 
    const PositionCoreFeedbackInputs* const _feedIn, 
    PositionCoreOutputs* const _out, 
    const BodySectionAPIBase* const _owner  ) : PositionCoreAPIBase(_in ,_feedIn ,_owner ), out(_out) 
  {
  }


  PositionCoreOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  PositionCoreUpdatePackage(const PositionCoreUpdatePackage& rhs);
  PositionCoreUpdatePackage& operator = (const PositionCoreUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct PositionCoreFeedbackPackage : public PositionCoreAPIBase
{
  PositionCoreFeedbackPackage(
    const PositionCoreFeedbackInputs* const _feedIn, 
    const PositionCoreInputs* const _in, 
    PositionCoreFeedbackOutputs* const _feedOut, 
    const BodySectionAPIBase* const _owner  ) : PositionCoreAPIBase(_in ,_feedIn ,_owner ), feedOut(_feedOut) 
  {
  }


  PositionCoreFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  PositionCoreFeedbackPackage(const PositionCoreFeedbackPackage& rhs);
  PositionCoreFeedbackPackage& operator = (const PositionCoreFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_POSITIONCORE_PKG_H

