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

#ifndef NM_MDF_HEADDODGEBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_HEADDODGEBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/HeadDodgeBehaviourInterface.module"

// known types
#include "Types/DodgeHazard.h"

// hook functions
#include "euphoria/erValueValidators.h"
#include "euphoria/erValuePostCombiners.h"

// constants
#include "NetworkConstants.h"

#include <string.h>

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable: 4324) // structure was padded due to __declspec(align()) 
#endif // _MSC_VER

/** 
 * \class HeadDodgeBehaviourInterfaceOutputs
 * \ingroup HeadDodgeBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (52 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadDodgeBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadDodgeBehaviourInterfaceOutputs));
  }

  NM_INLINE void setDodgeHazard(const DodgeHazard& _dodgeHazard, float dodgeHazard_importance = 1.f)
  {
    dodgeHazard = _dodgeHazard;
    postAssignDodgeHazard(dodgeHazard);
    NMP_ASSERT(dodgeHazard_importance >= 0.0f && dodgeHazard_importance <= 1.0f);
    m_dodgeHazardImportance = dodgeHazard_importance;
  }
  NM_INLINE float getDodgeHazardImportance() const { return m_dodgeHazardImportance; }
  NM_INLINE const float& getDodgeHazardImportanceRef() const { return m_dodgeHazardImportance; }
  NM_INLINE const DodgeHazard& getDodgeHazard() const { return dodgeHazard; }

protected:

  DodgeHazard dodgeHazard;  ///< Structure representing what needs to be dodged

  // post-assignment stubs
  NM_INLINE void postAssignDodgeHazard(const DodgeHazard& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_dodgeHazardImportance;


public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getDodgeHazardImportance() > 0.0f)
    {
      dodgeHazard.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HEADDODGEBEHAVIOURINTERFACE_DATA_H

