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

#ifndef NM_MDF_TYPE_HAZARD_H
#define NM_MDF_TYPE_HAZARD_H

// include definition file to create project dependency
#include "./Definition/Types/Environment.types"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/Environment_Patch.h"
#include "Types/SphereTrajectory.h"

// for combiners
#include "euphoria/erJunction.h"

// constants
#include "NetworkConstants.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

namespace MR { class InstanceDebugInterface; } 

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Environment.types'
// Data Payload: 368 Bytes
// Alignment: 16
struct Hazard
{

  NMP::Vector3 impactPosition;               ///< (Position)

  NMP::Vector3 contactNormal;                ///< (Direction)

  Environment::Patch object;
 // 240 Bytes 
  SphereTrajectory protectPath;
 // 64 Bytes 
  float protectMass;                         ///< (Mass)

  float impactTime;                          ///< (TimePeriod)

  float impactSpeed;                         ///< (Speed)

  float objectDistance;                      ///< (Length)

  bool isDodgeable;


  // functions

  void debugDraw(float radius, MR::InstanceDebugInterface* pDebugDrawInst);


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(impactPosition), "impactPosition");
    NM_VALIDATOR(Vector3Normalised(contactNormal), "contactNormal");
    object.validate();
    protectPath.validate();
    NM_VALIDATOR(FloatNonNegative(protectMass), "protectMass");
    NM_VALIDATOR(FloatValid(impactTime), "impactTime");
    NM_VALIDATOR(FloatValid(impactSpeed), "impactSpeed");
    NM_VALIDATOR(FloatNonNegative(objectDistance), "objectDistance");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Vector3Normalise(contactNormal);
    validate();
  }

}; // struct Hazard


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_HAZARD_H

