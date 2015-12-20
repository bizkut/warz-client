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

#ifndef NM_MDF_ARMPLACEMENT_PKG_H
#define NM_MDF_ARMPLACEMENT_PKG_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmPlacement.module"

#include "ArmPlacementData.h"

#include "ArmPackaging.h"



namespace MR
{
  class InstanceDebugInterface;
}

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// API Packaging
struct ArmPlacementAPIBase
{
  ArmPlacementAPIBase(
    const ArmPlacementData* const _data, 
    const ArmPlacementInputs* const _in, 
    const ArmAPIBase* const _owner  ) :data(_data) ,in(_in) ,owner(_owner)  {}

  const ArmPlacementData* const data;
  const ArmPlacementInputs* const in;

  const ArmAPIBase* const owner;

  ArmPlacementAPIBase(const ArmPlacementAPIBase& rhs);
  ArmPlacementAPIBase& operator = (const ArmPlacementAPIBase& rhs);
};

//----------------------------------------------------------------------------------------------------------------------
// Update Packaging
struct ArmPlacementUpdatePackage : public ArmPlacementAPIBase
{
  ArmPlacementUpdatePackage(
    ArmPlacementData* const _data, 
    const ArmPlacementInputs* const _in, 
    ArmPlacementOutputs* const _out, 
    const ArmAPIBase* const _owner  ) : ArmPlacementAPIBase(_data ,_in ,_owner ), data(_data), out(_out) 
  {
  }

  ArmPlacementData* const data;

  ArmPlacementOutputs* const out;


  // module update entrypoint
  void update(float timeStep, MR::InstanceDebugInterface* pDebugDrawInst);


  ArmPlacementUpdatePackage(const ArmPlacementUpdatePackage& rhs);
  ArmPlacementUpdatePackage& operator = (const ArmPlacementUpdatePackage& rhs);
};
//----------------------------------------------------------------------------------------------------------------------
// Feedback Packaging
struct ArmPlacementFeedbackPackage : public ArmPlacementAPIBase
{
  ArmPlacementFeedbackPackage(
    ArmPlacementData* const _data, 
    const ArmPlacementInputs* const _in, 
    const ArmAPIBase* const _owner  ) : ArmPlacementAPIBase(_data ,_in ,_owner ), data(_data) 
  {
  }

  ArmPlacementData* const data;



  // No update api required for this module!


  ArmPlacementFeedbackPackage(const ArmPlacementFeedbackPackage& rhs);
  ArmPlacementFeedbackPackage& operator = (const ArmPlacementFeedbackPackage& rhs);
};

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMPLACEMENT_PKG_H

