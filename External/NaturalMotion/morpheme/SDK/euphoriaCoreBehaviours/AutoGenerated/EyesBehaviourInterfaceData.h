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

#ifndef NM_MDF_EYESBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_EYESBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/EyesBehaviourInterface.module"

// external types
#include "NMPlatform/NMVector3.h"

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
 * \class EyesBehaviourInterfaceFeedbackInputs
 * \ingroup EyesBehaviourInterface
 * \brief FeedbackInputs This behaviour currently is just used to retrieve the eye vector or position, which can then be set in the animation
 *
 * Data packet definition (48 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct EyesBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(EyesBehaviourInterfaceFeedbackInputs));
  }

  NM_INLINE void setFocalDirection(const NMP::Vector3& _focalDirection, float focalDirection_importance = 1.f)
  {
    focalDirection = _focalDirection;
    postAssignFocalDirection(focalDirection);
    NMP_ASSERT(focalDirection_importance >= 0.0f && focalDirection_importance <= 1.0f);
    m_focalDirectionImportance = focalDirection_importance;
  }
  NM_INLINE float getFocalDirectionImportance() const { return m_focalDirectionImportance; }
  NM_INLINE const float& getFocalDirectionImportanceRef() const { return m_focalDirectionImportance; }
  NM_INLINE const NMP::Vector3& getFocalDirection() const { return focalDirection; }

  NM_INLINE void setFocalCentre(const NMP::Vector3& _focalCentre, float focalCentre_importance = 1.f)
  {
    focalCentre = _focalCentre;
    postAssignFocalCentre(focalCentre);
    NMP_ASSERT(focalCentre_importance >= 0.0f && focalCentre_importance <= 1.0f);
    m_focalCentreImportance = focalCentre_importance;
  }
  NM_INLINE float getFocalCentreImportance() const { return m_focalCentreImportance; }
  NM_INLINE const float& getFocalCentreImportanceRef() const { return m_focalCentreImportance; }
  NM_INLINE const NMP::Vector3& getFocalCentre() const { return focalCentre; }

  NM_INLINE void setFocalRadius(const float& _focalRadius, float focalRadius_importance = 1.f)
  {
    focalRadius = _focalRadius;
    postAssignFocalRadius(focalRadius);
    NMP_ASSERT(focalRadius_importance >= 0.0f && focalRadius_importance <= 1.0f);
    m_focalRadiusImportance = focalRadius_importance;
  }
  NM_INLINE float getFocalRadiusImportance() const { return m_focalRadiusImportance; }
  NM_INLINE const float& getFocalRadiusImportanceRef() const { return m_focalRadiusImportance; }
  NM_INLINE const float& getFocalRadius() const { return focalRadius; }

protected:

  NMP::Vector3 focalDirection;  ///< Direction character is looking in  (Direction)
  NMP::Vector3 focalCentre;  ///< Position character is looking at  (Position)
  float focalRadius;  ///< Radius of vision  (Length)

  // post-assignment stubs
  NM_INLINE void postAssignFocalDirection(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "focalDirection");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignFocalCentre(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "focalCentre");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignFocalRadius(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "focalRadius");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_focalDirectionImportance, 
     m_focalCentreImportance, 
     m_focalRadiusImportance;

  friend class EyesBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getFocalDirectionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(focalDirection), "focalDirection");
    }
    if (getFocalCentreImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(focalCentre), "focalCentre");
    }
    if (getFocalRadiusImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(focalRadius), "focalRadius");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_EYESBEHAVIOURINTERFACE_DATA_H

