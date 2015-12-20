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

#ifndef NM_MDF_TYPE_ENVIRONMENT_OBJECT_H
#define NM_MDF_TYPE_ENVIRONMENT_OBJECT_H

// include definition file to create project dependency
#include "./Definition/Types/Environment.types"

// external types
#include "NMPlatform/NMMatrix34.h"

// known types
#include "Types/Environment_State.h"

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

namespace Environment { 

//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Environment.types'
// Data Payload: 208 Bytes
// Alignment: 16
struct Object
{

  Environment::State state;
 // 128 Bytes 
  NMP::Matrix34 matrix;

  int32_t dataIndex;  ///< This is ER::PhysXPerShapeData::dataIndex and is the index into the array of EA objects

  bool isStill;

  bool isACharacter;



}; // struct Object

} // namespace Environment

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_ENVIRONMENT_OBJECT_H

