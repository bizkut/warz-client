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

#ifndef NM_MDF_HEADEYES_DATA_H
#define NM_MDF_HEADEYES_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadEyes.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/TargetRequest.h"

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
 * \class HeadEyesData
 * \ingroup HeadEyes
 * \brief Data controls the direction that the eyes look in, and forwards the information down to the headPoint
 *
 * Data packet definition (37 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadEyesData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadEyesData));
  }

  NMP::Vector3 eyeVector;  ///< Vector from head to look target  (PositionDelta)
  NMP::Vector3 focalCentre;  ///< Look target in world space  (Position)
  float focalRadius;  ///< Radius of sphere of vision  (Length)
  bool doingPoint;  ///< Used for hysteresis

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(eyeVector), "eyeVector");
    NM_VALIDATOR(Vector3Valid(focalCentre), "focalCentre");
    NM_VALIDATOR(FloatNonNegative(focalRadius), "focalRadius");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HeadEyesInputs
 * \ingroup HeadEyes
 * \brief Inputs controls the direction that the eyes look in, and forwards the information down to the headPoint
 *
 * Data packet definition (169 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadEyesInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadEyesInputs));
  }

  NM_INLINE void setTarget(const TargetRequest& _target, float target_importance = 1.f)
  {
    target = _target;
    postAssignTarget(target);
    NMP_ASSERT(target_importance >= 0.0f && target_importance <= 1.0f);
    m_targetImportance = target_importance;
  }
  NM_INLINE TargetRequest& startTargetModification()
  {
    m_targetImportance = -1.0f; // set invalid until stopTargetModification()
    return target;
  }
  NM_INLINE void stopTargetModification(float target_importance = 1.f)
  {
    postAssignTarget(target);
    NMP_ASSERT(target_importance >= 0.0f && target_importance <= 1.0f);
    m_targetImportance = target_importance;
  }
  NM_INLINE float getTargetImportance() const { return m_targetImportance; }
  NM_INLINE const float& getTargetImportanceRef() const { return m_targetImportance; }
  NM_INLINE const TargetRequest& getTarget() const { return target; }

  NM_INLINE void setShutEyes(const bool& _shutEyes, float shutEyes_importance = 1.f)
  {
    shutEyes = _shutEyes;
    NMP_ASSERT(shutEyes_importance >= 0.0f && shutEyes_importance <= 1.0f);
    m_shutEyesImportance = shutEyes_importance;
  }
  NM_INLINE float getShutEyesImportance() const { return m_shutEyesImportance; }
  NM_INLINE const float& getShutEyesImportanceRef() const { return m_shutEyesImportance; }
  NM_INLINE const bool& getShutEyes() const { return shutEyes; }

protected:

  TargetRequest target;  ///< Target to look at
  bool shutEyes;  ///< Choose whether to shut the character's eyes

  // post-assignment stubs
  NM_INLINE void postAssignTarget(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_targetImportance, 
     m_shutEyesImportance;

  friend class HeadEyes_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTargetImportance() > 0.0f)
    {
      target.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HeadEyesOutputs
 * \ingroup HeadEyes
 * \brief Outputs controls the direction that the eyes look in, and forwards the information down to the headPoint
 *
 * Data packet definition (192 bytes, 224 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadEyesOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadEyesOutputs));
  }

  NM_INLINE void setLookTarget(const TargetRequest& _lookTarget, float lookTarget_importance = 1.f)
  {
    lookTarget = _lookTarget;
    postAssignLookTarget(lookTarget);
    NMP_ASSERT(lookTarget_importance >= 0.0f && lookTarget_importance <= 1.0f);
    m_lookTargetImportance = lookTarget_importance;
  }
  NM_INLINE TargetRequest& startLookTargetModification()
  {
    m_lookTargetImportance = -1.0f; // set invalid until stopLookTargetModification()
    return lookTarget;
  }
  NM_INLINE void stopLookTargetModification(float lookTarget_importance = 1.f)
  {
    postAssignLookTarget(lookTarget);
    NMP_ASSERT(lookTarget_importance >= 0.0f && lookTarget_importance <= 1.0f);
    m_lookTargetImportance = lookTarget_importance;
  }
  NM_INLINE float getLookTargetImportance() const { return m_lookTargetImportance; }
  NM_INLINE const float& getLookTargetImportanceRef() const { return m_lookTargetImportance; }
  NM_INLINE const TargetRequest& getLookTarget() const { return lookTarget; }

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

  TargetRequest lookTarget;
  NMP::Vector3 focalCentre;                  ///< (Position)
  float focalRadius;                         ///< (Length)

  // post-assignment stubs
  NM_INLINE void postAssignLookTarget(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
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
     m_lookTargetImportance, 
     m_focalCentreImportance, 
     m_focalRadiusImportance;

  friend class HeadEyes_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getLookTargetImportance() > 0.0f)
    {
      lookTarget.validate();
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
/** 
 * \class HeadEyesFeedbackOutputs
 * \ingroup HeadEyes
 * \brief FeedbackOutputs controls the direction that the eyes look in, and forwards the information down to the headPoint
 *
 * Data packet definition (48 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadEyesFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadEyesFeedbackOutputs));
  }

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

  NMP::Vector3 focalCentre;  ///< Actual point that he can look at  (Position)
  NMP::Vector3 focalDirection;  ///< Actual direction that he is looking (with eyes, not head)  (Direction)
  float focalRadius;  ///< Actual radius of view used  (Length)

  // post-assignment stubs
  NM_INLINE void postAssignFocalCentre(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "focalCentre");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignFocalDirection(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "focalDirection");
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
     m_focalCentreImportance, 
     m_focalDirectionImportance, 
     m_focalRadiusImportance;

  friend class HeadEyes_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getFocalCentreImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(focalCentre), "focalCentre");
    }
    if (getFocalDirectionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(focalDirection), "focalDirection");
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

#endif // NM_MDF_HEADEYES_DATA_H

