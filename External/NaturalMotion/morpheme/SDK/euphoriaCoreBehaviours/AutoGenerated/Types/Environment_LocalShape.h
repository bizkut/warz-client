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

#ifndef NM_MDF_TYPE_ENVIRONMENT_LOCALSHAPE_H
#define NM_MDF_TYPE_ENVIRONMENT_LOCALSHAPE_H

// include definition file to create project dependency
#include "./Definition/Types/Environment.types"

// external types
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"

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

namespace Environment { struct Patch; } 
namespace Environment { struct LocalShape; } 

namespace Environment { 

//----------------------------------------------------------------------------------------------------------------------
// Declaration from 'Environment.types'
// Data Payload: 96 Bytes
// Alignment: 16
// Modifiers: __no_combiner__  
struct LocalShape
{

  NMP::Vector3 corner;  ///< this is local to the bound object  (Position)

  NMP::Vector3 faceNormals[3];               ///< (Direction)

  NMP::Vector3 knownContactPoint;            ///< (Position)

  int32_t type;


  // functions

  void fromWorldSpace(const Environment::Patch& patch, const NMP::Matrix34& matrix);
  void toWorldSpace(Environment::Patch& patch, const NMP::Matrix34& matrix);
  bool isConnectedTo(const Environment::LocalShape& object, float epsilon) const;


  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(corner), "corner");
    NM_VALIDATOR(Vector3Normalised(faceNormals[0]), "faceNormals[0]");
    NM_VALIDATOR(Vector3Normalised(faceNormals[1]), "faceNormals[1]");
    NM_VALIDATOR(Vector3Normalised(faceNormals[2]), "faceNormals[2]");
    NM_VALIDATOR(Vector3Valid(knownContactPoint), "knownContactPoint");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Vector3Normalise(faceNormals[0]);
    PostCombiners::Vector3Normalise(faceNormals[1]);
    PostCombiners::Vector3Normalise(faceNormals[2]);
    validate();
  }

}; // struct LocalShape

} // namespace Environment

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_ENVIRONMENT_LOCALSHAPE_H

