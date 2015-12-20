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

#ifndef NM_MDF_ARMREACHFORBODYPART_PKG_H
#define NM_MDF_ARMREACHFORBODYPART_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmReachForBodyPart.module"

#include "ArmReachForBodyPartData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmReachForBodyPartAPIBase
{
  ArmReachForBodyPartAPIBase(
    const ArmReachForBodyPartData* const _data, 
    const ArmReachForBodyPartInputs* const _in, 
    const ArmReachForBodyPartFeedbackInputs* const _feedIn, 
    const ArmAPIBase* const _owner  ) :data(_data) ,in(_in) ,feedIn(_feedIn) ,owner(_owner)  {}

  const ArmReachForBodyPartData* const data;
  const ArmReachForBodyPartInputs* const in;
  const ArmReachForBodyPartFeedbackInputs* const feedIn;

  const ArmAPIBase* const owner;

  ArmReachForBodyPartAPIBase(const ArmReachForBodyPartAPIBase& rhs);
  ArmReachForBodyPartAPIBase& operator = (const ArmReachForBodyPartAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmReachForBodyPartUpdatePackage : public ArmReachForBodyPartAPIBase
{
  ArmReachForBodyPartUpdatePackage(
    ArmReachForBodyPartData* const _data, 
    const ArmReachForBodyPartInputs* const _in, 
    const ArmReachForBodyPartFeedbackInputs* const _feedIn, 
    ArmReachForBodyPartOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmReachForBodyPartAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), out(_out) 
  {
  }

  ArmReachForBodyPartData* const data;

  ArmReachForBodyPartOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmReachForBodyPartUpdatePackage(const ArmReachForBodyPartUpdatePackage& rhs);
  ArmReachForBodyPartUpdatePackage& operator = (const ArmReachForBodyPartUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmReachForBodyPartFeedbackPackage : public ArmReachForBodyPartAPIBase
{
  ArmReachForBodyPartFeedbackPackage(
    ArmReachForBodyPartData* const _data, 
    const ArmReachForBodyPartFeedbackInputs* const _feedIn, 
    const ArmReachForBodyPartInputs* const _in, 
    ArmReachForBodyPartFeedbackOutputs* const _feedOut, 
    const ArmAPIBase* const _owner  ) : ArmReachForBodyPartAPIBase(_data ,_in ,_feedIn ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ArmReachForBodyPartData* const data;

  ArmReachForBodyPartFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmReachForBodyPartFeedbackPackage(const ArmReachForBodyPartFeedbackPackage& rhs);
  ArmReachForBodyPartFeedbackPackage& operator = (const ArmReachForBodyPartFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMREACHFORBODYPART_PKG_H

