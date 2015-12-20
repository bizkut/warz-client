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

#ifndef NM_MDF_POSITIONCORE_DATA_H
#define NM_MDF_POSITIONCORE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/PositionCore.module"

// known types
#include "Types/TranslationRequest.h"

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
 * \class PositionCoreInputs
 * \ingroup PositionCore
 * \brief Inputs receives and transmits data related to positioning a core (which is a chest or a pelvis)
 *
 * Data packet definition (164 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct PositionCoreInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(PositionCoreInputs));
  }

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

  TranslationRequest translationRequest;  ///< Requested translation from higher level modules

  // post-assignment stubs
  NM_INLINE void postAssignTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_translationRequestImportance;

  friend class PositionCore_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTranslationRequestImportance() > 0.0f)
    {
      translationRequest.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class PositionCoreOutputs
 * \ingroup PositionCore
 * \brief Outputs receives and transmits data related to positioning a core (which is a chest or a pelvis)
 *
 * Data packet definition (492 bytes, 512 aligned)
 */
NMP_ALIGN_PREFIX(32) struct PositionCoreOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(PositionCoreOutputs));
  }

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

  NM_INLINE void setTranslationUnsupportedRequest(const TranslationRequest& _translationUnsupportedRequest, float translationUnsupportedRequest_importance = 1.f)
  {
    translationUnsupportedRequest = _translationUnsupportedRequest;
    postAssignTranslationUnsupportedRequest(translationUnsupportedRequest);
    NMP_ASSERT(translationUnsupportedRequest_importance >= 0.0f && translationUnsupportedRequest_importance <= 1.0f);
    m_translationUnsupportedRequestImportance = translationUnsupportedRequest_importance;
  }
  NM_INLINE TranslationRequest& startTranslationUnsupportedRequestModification()
  {
    m_translationUnsupportedRequestImportance = -1.0f; // set invalid until stopTranslationUnsupportedRequestModification()
    return translationUnsupportedRequest;
  }
  NM_INLINE void stopTranslationUnsupportedRequestModification(float translationUnsupportedRequest_importance = 1.f)
  {
    postAssignTranslationUnsupportedRequest(translationUnsupportedRequest);
    NMP_ASSERT(translationUnsupportedRequest_importance >= 0.0f && translationUnsupportedRequest_importance <= 1.0f);
    m_translationUnsupportedRequestImportance = translationUnsupportedRequest_importance;
  }
  NM_INLINE float getTranslationUnsupportedRequestImportance() const { return m_translationUnsupportedRequestImportance; }
  NM_INLINE const float& getTranslationUnsupportedRequestImportanceRef() const { return m_translationUnsupportedRequestImportance; }
  NM_INLINE const TranslationRequest& getTranslationUnsupportedRequest() const { return translationUnsupportedRequest; }

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

protected:

  TranslationRequest translationRequest;  ///< Translation request forwarded on from lowerlevel modules
  TranslationRequest translationUnsupportedRequest;  ///< Separate request for when body is unsupported. Since you don't for instance translate the chest with the arms when the spine is supported
  TranslationRequest oneWayTranslationRequest;  ///< Request forwarded from feedIn.oneWayTranslationRequest

  // post-assignment stubs
  NM_INLINE void postAssignTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTranslationUnsupportedRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
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

  // importance values
  float
     m_translationRequestImportance, 
     m_translationUnsupportedRequestImportance, 
     m_oneWayTranslationRequestImportance;

  friend class PositionCore_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTranslationRequestImportance() > 0.0f)
    {
      translationRequest.validate();
    }
    if (getTranslationUnsupportedRequestImportance() > 0.0f)
    {
      translationUnsupportedRequest.validate();
    }
    if (getOneWayTranslationRequestImportance() > 0.0f)
    {
      oneWayTranslationRequest.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class PositionCoreFeedbackInputs
 * \ingroup PositionCore
 * \brief FeedbackInputs receives and transmits data related to positioning a core (which is a chest or a pelvis)
 *
 * Data packet definition (164 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct PositionCoreFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(PositionCoreFeedbackInputs));
  }

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

protected:

  TranslationRequest oneWayTranslationRequest;  ///< Requested translation from lower level modules

  // post-assignment stubs
  NM_INLINE void postAssignOneWayTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_oneWayTranslationRequestImportance;

  friend class PositionCore_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getOneWayTranslationRequestImportance() > 0.0f)
    {
      oneWayTranslationRequest.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class PositionCoreFeedbackOutputs
 * \ingroup PositionCore
 * \brief FeedbackOutputs receives and transmits data related to positioning a core (which is a chest or a pelvis)
 *
 * Data packet definition (164 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct PositionCoreFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(PositionCoreFeedbackOutputs));
  }

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

  TranslationRequest translationRequest;  ///< Translation request forwarded up from lowerlevel modules

  // post-assignment stubs
  NM_INLINE void postAssignTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_translationRequestImportance;

  friend class PositionCore_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTranslationRequestImportance() > 0.0f)
    {
      translationRequest.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_POSITIONCORE_DATA_H

