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

#ifndef NM_MDF_TYPE_PATCHSTORE_H
#define NM_MDF_TYPE_PATCHSTORE_H

// include definition file to create project dependency
#include "./Definition/Types/Environment.types"

// known types
#include "Types/Environment_LocalShape.h"

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
// Data Payload: 416 Bytes
// Alignment: 16
// Modifiers: __no_combiner__  
struct PatchStore
{

  Environment::LocalShape localShapes[4];
 // 384 Bytes 
  int64_t lastObjectShapeID;

  int32_t patchesHead;

  int32_t lastObjectIndex;

  int32_t sweepSegment;

  int32_t staticSweepSegment;



}; // struct PatchStore


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_PATCHSTORE_H

