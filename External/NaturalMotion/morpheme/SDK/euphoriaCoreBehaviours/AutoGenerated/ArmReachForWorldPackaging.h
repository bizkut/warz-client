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

#ifndef NM_MDF_ARMREACHFORWORLD_PKG_H
#define NM_MDF_ARMREACHFORWORLD_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmReachForWorld.module"

#include "ArmReachForWorldData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmReachForWorldAPIBase
{
  ArmReachForWorldAPIBase(
    const ArmReachForWorldData* const _data, 
    const ArmReachForWorldInputs* const _in, 
    const ArmAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const ArmReachForWorldData* const data;
  const ArmReachForWorldInputs* const in;

  const ArmAPIBase* const owner;

  ArmReachForWorldAPIBase(const ArmReachForWorldAPIBase& rhs);
  ArmReachForWorldAPIBase& operator = (const ArmReachForWorldAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmReachForWorldUpdatePackage : public ArmReachForWorldAPIBase
{
  ArmReachForWorldUpdatePackage(
    ArmReachForWorldData* const _data, 
    const ArmReachForWorldInputs* const _in, 
    ArmReachForWorldOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmReachForWorldAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  ArmReachForWorldData* const data;

  ArmReachForWorldOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmReachForWorldUpdatePackage(const ArmReachForWorldUpdatePackage& rhs);
  ArmReachForWorldUpdatePackage& operator = (const ArmReachForWorldUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmReachForWorldFeedbackPackage : public ArmReachForWorldAPIBase
{
  ArmReachForWorldFeedbackPackage(
    ArmReachForWorldData* const _data, 
    const ArmReachForWorldInputs* const _in, 
    ArmReachForWorldFeedbackOutputs* const _feedOut, 
    const ArmAPIBase* const _owner  ) : ArmReachForWorldAPIBase(_data ,_in ,_owner ), data(_data), feedOut(_feedOut) 
  {
  }

  ArmReachForWorldData* const data;

  ArmReachForWorldFeedbackOutputs* const feedOut;


  // module feedback entrypoint
  void feedback(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmReachForWorldFeedbackPackage(const ArmReachForWorldFeedbackPackage& rhs);
  ArmReachForWorldFeedbackPackage& operator = (const ArmReachForWorldFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMREACHFORWORLD_PKG_H

