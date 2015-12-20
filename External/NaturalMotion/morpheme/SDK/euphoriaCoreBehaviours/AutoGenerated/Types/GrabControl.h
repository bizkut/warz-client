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

#ifndef NM_MDF_TYPE_GRABCONTROL_H
#define NM_MDF_TYPE_GRABCONTROL_H

// include definition file to create project dependency
#include "./Definition/Types/Hold.types"

// known types
#include "Types/EndConstraintControl.h"
#include "Types/HoldTimer.h"
#include "Types/EdgeRaycastParams.h"
#include "Types/GrabChestParams.h"

// for combiners
#include "euphoria/erJunction.h"

// constants
#include "NetworkConstants.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Hold.types'
// Data Payload: 144 Bytes
// Alignment: 16

//----------------------------------------------------------------------------------------------------------------------
// Aggregate for sending along together in BehaviourNetwork
//----------------------------------------------------------------------------------------------------------------------

struct GrabControl
{

  EndConstraintControl constraintParams;
 // 96 Bytes 
  HoldTimer holdTimer;
 // 12 Bytes 
  EdgeRaycastParams edgeRaycastParams;
 // 13 Bytes 
  GrabChestParams grabChestParams;
 // 12 Bytes 


}; // struct GrabControl


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_GRABCONTROL_H

