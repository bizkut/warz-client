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

#ifndef NM_MDF_TYPE_PATCHSET_H
#define NM_MDF_TYPE_PATCHSET_H

// include definition file to create project dependency
#include "./Definition/Types/Environment.types"

// known types
#include "Types/Environment_Patch.h"

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
// Declaration from 'Environment.types'
// Data Payload: 736 Bytes
// Alignment: 16
struct PatchSet
{

  enum NumPatches
  {
    /*  3 */ MAX_NUM_PATCHES = 3,  
  };

  Environment::Patch patches[PatchSet::MAX_NUM_PATCHES];
 // 720 Bytes 
  int32_t numPatches;



}; // struct PatchSet


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_PATCHSET_H

