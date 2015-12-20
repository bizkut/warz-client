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

#ifndef NM_MDF_BODYSECTION_DATA_H
#define NM_MDF_BODYSECTION_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/BodySection.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/RotationRequest.h"
#include "Types/TranslationRequest.h"
#include "Types/BraceState.h"
#include "Types/ContactInfo.h"

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
 * \class BodySectionInputs
 * \ingroup BodySection
 * \brief Inputs One for upper body, one for lower body, transports data which operates equivalently on upper and lower
 *
 * Data packet definition (328 bytes, 352 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodySectionInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodySectionInputs));
  }

  NM_INLINE void setRotationRequest(const RotationRequest& _rotationRequest, float rotationRequest_importance = 1.f)
  {
    rotationRequest = _rotationRequest;
    postAssignRotationRequest(rotationRequest);
    NMP_ASSERT(rotationRequest_importance >= 0.0f && rotationRequest_importance <= 1.0f);
    m_rotationRequestImportance = rotationRequest_importance;
  }
  NM_INLINE RotationRequest& startRotationRequestModification()
  {
    m_rotationRequestImportance = -1.0f; // set invalid until stopRotationRequestModification()
    return rotationRequest;
  }
  NM_INLINE void stopRotationRequestModification(float rotationRequest_importance = 1.f)
  {
    postAssignRotationRequest(rotationRequest);
    NMP_ASSERT(rotationRequest_importance >= 0.0f && rotationRequest_importance <= 1.0f);
    m_rotationRequestImportance = rotationRequest_importance;
  }
  NM_INLINE float getRotationRequestImportance() const { return m_rotationRequestImportance; }
  NM_INLINE const float& getRotationRequestImportanceRef() const { return m_rotationRequestImportance; }
  NM_INLINE const RotationRequest& getRotationRequest() const { return rotationRequest; }

  NM_INLINE void setTranslationRequest(const TranslationRequest& _translationRequest, float translationRequest_importance = 1.f)
  {
    translationRequest = _translationRequest;
    postAssignTranslationRequest(translationRequest);
    NMP_ASSERT(translationRequest_importance >= 0.0f && translationRequest_importance <= 1.0f);
    m_translationRequestImportance = translationRequest_importance;
  }
  NM_INLINE TranslationRequest& startTranslationRequestModification()
  {
    m_translationRequestImportance = -1.0f; // set invalid until stopTranslationRequestModification()
    return translationRequest;
  }
  NM_INLINE void stopTranslationRequestModification(float translationRequest_importance = 1.f)
  {
    postAssignTranslationRequest(translationRequest);
    NMP_ASSERT(translationRequest_importance >= 0.0f && translationRequest_importance <= 1.0f);
    m_translationRequestImportance = translationRequest_importance;
  }
  NM_INLINE float getTranslationRequestImportance() const { return m_translationRequestImportance; }
  NM_INLINE const float& getTranslationRequestImportanceRef() const { return m_translationRequestImportance; }
  NM_INLINE const TranslationRequest& getTranslationRequest() const { return translationRequest; }

protected:

  RotationRequest rotationRequest;  /// Request for rotation of chest or pelvis depending on if this is the upper or lower body section
  TranslationRequest translationRequest;  ///< Request for translation of chest or pelvis

  // post-assignment stubs
  NM_INLINE void postAssignRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rotationRequestImportance, 
     m_translationRequestImportance;

  friend class BodySection_Con;
  friend class BodySection_lowerCon;
  friend class BodySection_upperCon;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRotationRequestImportance() > 0.0f)
    {
      rotationRequest.validate();
    }
    if (getTranslationRequestImportance() > 0.0f)
    {
      translationRequest.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BodySectionOutputs
 * \ingroup BodySection
 * \brief Outputs One for upper body, one for lower body, transports data which operates equivalently on upper and lower
 *
 * Data packet definition (840 bytes, 864 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodySectionOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodySectionOutputs));
  }

  NM_INLINE float getRotationRequestImportance() const { return m_rotationRequestImportance; }
  NM_INLINE const float& getRotationRequestImportanceRef() const { return m_rotationRequestImportance; }
  NM_INLINE const RotationRequest& getRotationRequest() const { return rotationRequest; }

  NM_INLINE float getOneWayRotationRequestImportance() const { return m_oneWayRotationRequestImportance; }
  NM_INLINE const float& getOneWayRotationRequestImportanceRef() const { return m_oneWayRotationRequestImportance; }
  NM_INLINE const RotationRequest& getOneWayRotationRequest() const { return oneWayRotationRequest; }

  NM_INLINE float getTranslationRequestImportance() const { return m_translationRequestImportance; }
  NM_INLINE const float& getTranslationRequestImportanceRef() const { return m_translationRequestImportance; }
  NM_INLINE const TranslationRequest& getTranslationRequest() const { return translationRequest; }

  NM_INLINE float getOneWayTranslationRequestImportance() const { return m_oneWayTranslationRequestImportance; }
  NM_INLINE const float& getOneWayTranslationRequestImportanceRef() const { return m_oneWayTranslationRequestImportance; }
  NM_INLINE const TranslationRequest& getOneWayTranslationRequest() const { return oneWayTranslationRequest; }

  NM_INLINE float getIntermediateRRImportance() const { return m_intermediateRRImportance; }
  NM_INLINE const float& getIntermediateRRImportanceRef() const { return m_intermediateRRImportance; }
  NM_INLINE const RotationRequest& getIntermediateRR() const { return intermediateRR; }

  NM_INLINE float getContactDirectionRequestImportance() const { return m_contactDirectionRequestImportance; }
  NM_INLINE const float& getContactDirectionRequestImportanceRef() const { return m_contactDirectionRequestImportance; }
  NM_INLINE const NMP::Vector3& getContactDirectionRequest() const { return contactDirectionRequest; }

protected:

  RotationRequest rotationRequest;  // Passing on the incoming requests to the limbs of this body section
  RotationRequest oneWayRotationRequest;
  TranslationRequest translationRequest;
  TranslationRequest oneWayTranslationRequest;
  RotationRequest intermediateRR;  ///< just used as intermediate value in junction tree
  NMP::Vector3 contactDirectionRequest;      ///< (Direction)

  // post-assignment stubs
  NM_INLINE void postAssignRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignOneWayRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignOneWayTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignIntermediateRR(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignContactDirectionRequest(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "contactDirectionRequest");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rotationRequestImportance, 
     m_oneWayRotationRequestImportance, 
     m_translationRequestImportance, 
     m_oneWayTranslationRequestImportance, 
     m_intermediateRRImportance, 
     m_contactDirectionRequestImportance;

  friend class BodySection_Con;
  friend class BodySection_lowerCon;
  friend class BodySection_upperCon;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRotationRequestImportance() > 0.0f)
    {
      rotationRequest.validate();
    }
    if (getOneWayRotationRequestImportance() > 0.0f)
    {
      oneWayRotationRequest.validate();
    }
    if (getTranslationRequestImportance() > 0.0f)
    {
      translationRequest.validate();
    }
    if (getOneWayTranslationRequestImportance() > 0.0f)
    {
      oneWayTranslationRequest.validate();
    }
    if (getIntermediateRRImportance() > 0.0f)
    {
      intermediateRR.validate();
    }
    if (getContactDirectionRequestImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(contactDirectionRequest), "contactDirectionRequest");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BodySectionFeedbackInputs
 * \ingroup BodySection
 * \brief FeedbackInputs One for upper body, one for lower body, transports data which operates equivalently on upper and lower
 *
 * Data packet definition (873 bytes, 896 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodySectionFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodySectionFeedbackInputs));
  }

  NM_INLINE void setRotationRequest(const RotationRequest& _rotationRequest, float rotationRequest_importance = 1.f)
  {
    rotationRequest = _rotationRequest;
    postAssignRotationRequest(rotationRequest);
    NMP_ASSERT(rotationRequest_importance >= 0.0f && rotationRequest_importance <= 1.0f);
    m_rotationRequestImportance = rotationRequest_importance;
  }
  NM_INLINE RotationRequest& startRotationRequestModification()
  {
    m_rotationRequestImportance = -1.0f; // set invalid until stopRotationRequestModification()
    return rotationRequest;
  }
  NM_INLINE void stopRotationRequestModification(float rotationRequest_importance = 1.f)
  {
    postAssignRotationRequest(rotationRequest);
    NMP_ASSERT(rotationRequest_importance >= 0.0f && rotationRequest_importance <= 1.0f);
    m_rotationRequestImportance = rotationRequest_importance;
  }
  NM_INLINE float getRotationRequestImportance() const { return m_rotationRequestImportance; }
  NM_INLINE const float& getRotationRequestImportanceRef() const { return m_rotationRequestImportance; }
  NM_INLINE const RotationRequest& getRotationRequest() const { return rotationRequest; }

  NM_INLINE void setOneWayRotationRequest(const RotationRequest& _oneWayRotationRequest, float oneWayRotationRequest_importance = 1.f)
  {
    oneWayRotationRequest = _oneWayRotationRequest;
    postAssignOneWayRotationRequest(oneWayRotationRequest);
    NMP_ASSERT(oneWayRotationRequest_importance >= 0.0f && oneWayRotationRequest_importance <= 1.0f);
    m_oneWayRotationRequestImportance = oneWayRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startOneWayRotationRequestModification()
  {
    m_oneWayRotationRequestImportance = -1.0f; // set invalid until stopOneWayRotationRequestModification()
    return oneWayRotationRequest;
  }
  NM_INLINE void stopOneWayRotationRequestModification(float oneWayRotationRequest_importance = 1.f)
  {
    postAssignOneWayRotationRequest(oneWayRotationRequest);
    NMP_ASSERT(oneWayRotationRequest_importance >= 0.0f && oneWayRotationRequest_importance <= 1.0f);
    m_oneWayRotationRequestImportance = oneWayRotationRequest_importance;
  }
  NM_INLINE float getOneWayRotationRequestImportance() const { return m_oneWayRotationRequestImportance; }
  NM_INLINE const float& getOneWayRotationRequestImportanceRef() const { return m_oneWayRotationRequestImportance; }
  NM_INLINE const RotationRequest& getOneWayRotationRequest() const { return oneWayRotationRequest; }

  NM_INLINE void setTranslationRequest(const TranslationRequest& _translationRequest, float translationRequest_importance = 1.f)
  {
    translationRequest = _translationRequest;
    postAssignTranslationRequest(translationRequest);
    NMP_ASSERT(translationRequest_importance >= 0.0f && translationRequest_importance <= 1.0f);
    m_translationRequestImportance = translationRequest_importance;
  }
  NM_INLINE TranslationRequest& startTranslationRequestModification()
  {
    m_translationRequestImportance = -1.0f; // set invalid until stopTranslationRequestModification()
    return translationRequest;
  }
  NM_INLINE void stopTranslationRequestModification(float translationRequest_importance = 1.f)
  {
    postAssignTranslationRequest(translationRequest);
    NMP_ASSERT(translationRequest_importance >= 0.0f && translationRequest_importance <= 1.0f);
    m_translationRequestImportance = translationRequest_importance;
  }
  NM_INLINE float getTranslationRequestImportance() const { return m_translationRequestImportance; }
  NM_INLINE const float& getTranslationRequestImportanceRef() const { return m_translationRequestImportance; }
  NM_INLINE const TranslationRequest& getTranslationRequest() const { return translationRequest; }

  NM_INLINE void setOneWayTranslationRequest(const TranslationRequest& _oneWayTranslationRequest, float oneWayTranslationRequest_importance = 1.f)
  {
    oneWayTranslationRequest = _oneWayTranslationRequest;
    postAssignOneWayTranslationRequest(oneWayTranslationRequest);
    NMP_ASSERT(oneWayTranslationRequest_importance >= 0.0f && oneWayTranslationRequest_importance <= 1.0f);
    m_oneWayTranslationRequestImportance = oneWayTranslationRequest_importance;
  }
  NM_INLINE TranslationRequest& startOneWayTranslationRequestModification()
  {
    m_oneWayTranslationRequestImportance = -1.0f; // set invalid until stopOneWayTranslationRequestModification()
    return oneWayTranslationRequest;
  }
  NM_INLINE void stopOneWayTranslationRequestModification(float oneWayTranslationRequest_importance = 1.f)
  {
    postAssignOneWayTranslationRequest(oneWayTranslationRequest);
    NMP_ASSERT(oneWayTranslationRequest_importance >= 0.0f && oneWayTranslationRequest_importance <= 1.0f);
    m_oneWayTranslationRequestImportance = oneWayTranslationRequest_importance;
  }
  NM_INLINE float getOneWayTranslationRequestImportance() const { return m_oneWayTranslationRequestImportance; }
  NM_INLINE const float& getOneWayTranslationRequestImportanceRef() const { return m_oneWayTranslationRequestImportance; }
  NM_INLINE const TranslationRequest& getOneWayTranslationRequest() const { return oneWayTranslationRequest; }

  NM_INLINE void setClosestContact(const ContactInfo& _closestContact, float closestContact_importance = 1.f)
  {
    closestContact = _closestContact;
    postAssignClosestContact(closestContact);
    NMP_ASSERT(closestContact_importance >= 0.0f && closestContact_importance <= 1.0f);
    m_closestContactImportance = closestContact_importance;
  }
  NM_INLINE ContactInfo& startClosestContactModification()
  {
    m_closestContactImportance = -1.0f; // set invalid until stopClosestContactModification()
    return closestContact;
  }
  NM_INLINE void stopClosestContactModification(float closestContact_importance = 1.f)
  {
    postAssignClosestContact(closestContact);
    NMP_ASSERT(closestContact_importance >= 0.0f && closestContact_importance <= 1.0f);
    m_closestContactImportance = closestContact_importance;
  }
  NM_INLINE float getClosestContactImportance() const { return m_closestContactImportance; }
  NM_INLINE const float& getClosestContactImportanceRef() const { return m_closestContactImportance; }
  NM_INLINE const ContactInfo& getClosestContact() const { return closestContact; }

  enum { maxBraceStates = 2 };
  NM_INLINE unsigned int getMaxBraceStates() const { return maxBraceStates; }
  NM_INLINE void setBraceStates(unsigned int number, const BraceState* _braceStates, float braceStates_importance = 1.f)
  {
    NMP_ASSERT(number <= maxBraceStates);
    NMP_ASSERT(braceStates_importance >= 0.0f && braceStates_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      braceStates[i] = _braceStates[i];
      postAssignBraceStates(braceStates[i]);
      m_braceStatesImportance[i] = braceStates_importance;
    }
  }
  NM_INLINE void setBraceStatesAt(unsigned int index, const BraceState& _braceStates, float braceStates_importance = 1.f)
  {
    NMP_ASSERT(index < maxBraceStates);
    braceStates[index] = _braceStates;
    NMP_ASSERT(braceStates_importance >= 0.0f && braceStates_importance <= 1.0f);
    postAssignBraceStates(braceStates[index]);
    m_braceStatesImportance[index] = braceStates_importance;
  }
  NM_INLINE float getBraceStatesImportance(int index) const { return m_braceStatesImportance[index]; }
  NM_INLINE const float& getBraceStatesImportanceRef(int index) const { return m_braceStatesImportance[index]; }
  NM_INLINE unsigned int calculateNumBraceStates() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_braceStatesImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const BraceState& getBraceStates(unsigned int index) const { NMP_ASSERT(index <= maxBraceStates); return braceStates[index]; }

  NM_INLINE void setCushionPoint(const NMP::Vector3& _cushionPoint, float cushionPoint_importance = 1.f)
  {
    cushionPoint = _cushionPoint;
    postAssignCushionPoint(cushionPoint);
    NMP_ASSERT(cushionPoint_importance >= 0.0f && cushionPoint_importance <= 1.0f);
    m_cushionPointImportance = cushionPoint_importance;
  }
  NM_INLINE float getCushionPointImportance() const { return m_cushionPointImportance; }
  NM_INLINE const float& getCushionPointImportanceRef() const { return m_cushionPointImportance; }
  NM_INLINE const NMP::Vector3& getCushionPoint() const { return cushionPoint; }

  NM_INLINE void setSpinDirection(const NMP::Vector3& _spinDirection, float spinDirection_importance = 1.f)
  {
    spinDirection = _spinDirection;
    postAssignSpinDirection(spinDirection);
    NMP_ASSERT(spinDirection_importance >= 0.0f && spinDirection_importance <= 1.0f);
    m_spinDirectionImportance = spinDirection_importance;
  }
  NM_INLINE float getSpinDirectionImportance() const { return m_spinDirectionImportance; }
  NM_INLINE const float& getSpinDirectionImportanceRef() const { return m_spinDirectionImportance; }
  NM_INLINE const NMP::Vector3& getSpinDirection() const { return spinDirection; }

  NM_INLINE void setControlledAmount(const float& _controlledAmount, float controlledAmount_importance = 1.f)
  {
    controlledAmount = _controlledAmount;
    postAssignControlledAmount(controlledAmount);
    NMP_ASSERT(controlledAmount_importance >= 0.0f && controlledAmount_importance <= 1.0f);
    m_controlledAmountImportance = controlledAmount_importance;
  }
  NM_INLINE float getControlledAmountImportance() const { return m_controlledAmountImportance; }
  NM_INLINE const float& getControlledAmountImportanceRef() const { return m_controlledAmountImportance; }
  NM_INLINE const float& getControlledAmount() const { return controlledAmount; }

  NM_INLINE void setExternallyControlledAmount(const float& _externallyControlledAmount, float externallyControlledAmount_importance = 1.f)
  {
    externallyControlledAmount = _externallyControlledAmount;
    postAssignExternallyControlledAmount(externallyControlledAmount);
    NMP_ASSERT(externallyControlledAmount_importance >= 0.0f && externallyControlledAmount_importance <= 1.0f);
    m_externallyControlledAmountImportance = externallyControlledAmount_importance;
  }
  NM_INLINE float getExternallyControlledAmountImportance() const { return m_externallyControlledAmountImportance; }
  NM_INLINE const float& getExternallyControlledAmountImportanceRef() const { return m_externallyControlledAmountImportance; }
  NM_INLINE const float& getExternallyControlledAmount() const { return externallyControlledAmount; }

  NM_INLINE void setInContact(const bool& _inContact, float inContact_importance = 1.f)
  {
    inContact = _inContact;
    NMP_ASSERT(inContact_importance >= 0.0f && inContact_importance <= 1.0f);
    m_inContactImportance = inContact_importance;
  }
  NM_INLINE float getInContactImportance() const { return m_inContactImportance; }
  NM_INLINE const float& getInContactImportanceRef() const { return m_inContactImportance; }
  NM_INLINE const bool& getInContact() const { return inContact; }

protected:

  RotationRequest rotationRequest;  // Incoming requests to the limbs of this body section
  RotationRequest oneWayRotationRequest;  ///< Comes from the spine.
  TranslationRequest translationRequest;
  TranslationRequest oneWayTranslationRequest;  ///< Comes from the spine
  ContactInfo closestContact;  ///< Used by grab, to find the closest contact to a certain normal
  BraceState braceStates[NetworkConstants::networkMaxNumLegs];  ///< Used by BraceChooser to pick most appropriate limbs
  NMP::Vector3 cushionPoint;  ///< Averages the cushion position which is centre of cushioning hands  (Position)
  NMP::Vector3 spinDirection;  ///< Used to coordinate the spin between limbs of this body section  (Direction)
  float controlledAmount;  /// How much the chest of pelvis is controlled by the supporting limbs, 1 means it should move with normalStiffness.  (WeightWithClamp)
  float externallyControlledAmount;  ///< Means externally to that body section  (WeightWithClamp)
  bool inContact;  ///< Are any of the limbs in this body section in contact

  // post-assignment stubs
  NM_INLINE void postAssignRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignOneWayRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignOneWayTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignClosestContact(const ContactInfo& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBraceStates(const BraceState& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignCushionPoint(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "cushionPoint");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinDirection(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "spinDirection");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignControlledAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "controlledAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignExternallyControlledAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "externallyControlledAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rotationRequestImportance, 
     m_oneWayRotationRequestImportance, 
     m_translationRequestImportance, 
     m_oneWayTranslationRequestImportance, 
     m_closestContactImportance, 
     m_braceStatesImportance[NetworkConstants::networkMaxNumLegs], 
     m_cushionPointImportance, 
     m_spinDirectionImportance, 
     m_controlledAmountImportance, 
     m_externallyControlledAmountImportance, 
     m_inContactImportance;

  friend class BodySection_Con;
  friend class BodySection_lowerCon;
  friend class BodySection_upperCon;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRotationRequestImportance() > 0.0f)
    {
      rotationRequest.validate();
    }
    if (getOneWayRotationRequestImportance() > 0.0f)
    {
      oneWayRotationRequest.validate();
    }
    if (getTranslationRequestImportance() > 0.0f)
    {
      translationRequest.validate();
    }
    if (getOneWayTranslationRequestImportance() > 0.0f)
    {
      oneWayTranslationRequest.validate();
    }
    if (getClosestContactImportance() > 0.0f)
    {
      closestContact.validate();
    }
    {
      uint32_t numBraceStates = calculateNumBraceStates();
      for (uint32_t i=0; i<numBraceStates; i++)
      {
        if (getBraceStatesImportance(i) > 0.0f)
          braceStates[i].validate();
      }
    }
    if (getCushionPointImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(cushionPoint), "cushionPoint");
    }
    if (getSpinDirectionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(spinDirection), "spinDirection");
    }
    if (getControlledAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(controlledAmount), "controlledAmount");
    }
    if (getExternallyControlledAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(externallyControlledAmount), "externallyControlledAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BodySectionFeedbackOutputs
 * \ingroup BodySection
 * \brief FeedbackOutputs One for upper body, one for lower body, transports data which operates equivalently on upper and lower
 *
 * Data packet definition (8 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BodySectionFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BodySectionFeedbackOutputs));
  }

  NM_INLINE float getControlledAmountImportance() const { return m_controlledAmountImportance; }
  NM_INLINE const float& getControlledAmountImportanceRef() const { return m_controlledAmountImportance; }
  NM_INLINE const float& getControlledAmount() const { return controlledAmount; }

protected:

  float controlledAmount;  ///< Feeds out how controlled the chest/pelvis is  (WeightWithClamp)

  // post-assignment stubs
  NM_INLINE void postAssignControlledAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "controlledAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_controlledAmountImportance;

  friend class BodySection_Con;
  friend class BodySection_lowerCon;
  friend class BodySection_upperCon;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getControlledAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(controlledAmount), "controlledAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BODYSECTION_DATA_H

