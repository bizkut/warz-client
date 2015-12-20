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

#ifndef NM_MDF_TYPE_SELECTIONINFO_H
#define NM_MDF_TYPE_SELECTIONINFO_H

// include definition file to create project dependency
#include "./Definition/Types/Externs.types"

// external types
#include "NMPlatform/NMVector3.h"

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
// Declaration from 'Externs.types'
// Data Payload: 64 Bytes
// Alignment: 16
struct SelectionInfo
{

  NMP::Vector3 point;  /// pos and normal in actor local coordinates  (Position)

  NMP::Vector3 normal;                       ///< (Direction)

  NMP::Vector3 hitDir;                       ///< (Direction)

  void* selectedActor;  /// pointer identifying the (physics system) object selected

  int32_t selectionCode;  /// code to indicate reason/context for selection

  bool newSelection;  /// set to true when a selection is first made and false otherwise
  /// signals a new selection



  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(point), "point");
    NM_VALIDATOR(Vector3Normalised(normal), "normal");
    NM_VALIDATOR(Vector3Normalised(hitDir), "hitDir");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postCombine()
  {
    PostCombiners::Vector3Normalise(normal);
    PostCombiners::Vector3Normalise(hitDir);
    validate();
  }

}; // struct SelectionInfo


} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_TYPE_SELECTIONINFO_H

