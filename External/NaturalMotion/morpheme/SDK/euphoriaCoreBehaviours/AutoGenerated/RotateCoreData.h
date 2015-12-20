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

#ifndef NM_MDF_ROTATECORE_DATA_H
#define NM_MDF_ROTATECORE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/RotateCore.module"

// known types
#include "Types/RotationRequest.h"

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
 * \class RotateCoreInputs
 * \ingroup RotateCore
 * \brief Inputs simply reads requests for rotations of a core (chest or pelvis) and forwards them on to be used by swing/spine etc
 *
 * Data packet definition (164 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct RotateCoreInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(RotateCoreInputs));
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

protected:

  RotationRequest rotationRequest;  ///< Requested rotation from higher level modules

  // post-assignment stubs
  NM_INLINE void postAssignRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rotationRequestImportance;

  friend class RotateCore_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRotationRequestImportance() > 0.0f)
    {
      rotationRequest.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class RotateCoreOutputs
 * \ingroup RotateCore
 * \brief Outputs simply reads requests for rotations of a core (chest or pelvis) and forwards them on to be used by swing/spine etc
 *
 * Data packet definition (492 bytes, 512 aligned)
 */
NMP_ALIGN_PREFIX(32) struct RotateCoreOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(RotateCoreOutputs));
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

  NM_INLINE void setRotationUnsupportedRequest(const RotationRequest& _rotationUnsupportedRequest, float rotationUnsupportedRequest_importance = 1.f)
  {
    rotationUnsupportedRequest = _rotationUnsupportedRequest;
    postAssignRotationUnsupportedRequest(rotationUnsupportedRequest);
    NMP_ASSERT(rotationUnsupportedRequest_importance >= 0.0f && rotationUnsupportedRequest_importance <= 1.0f);
    m_rotationUnsupportedRequestImportance = rotationUnsupportedRequest_importance;
  }
  NM_INLINE RotationRequest& startRotationUnsupportedRequestModification()
  {
    m_rotationUnsupportedRequestImportance = -1.0f; // set invalid until stopRotationUnsupportedRequestModification()
    return rotationUnsupportedRequest;
  }
  NM_INLINE void stopRotationUnsupportedRequestModification(float rotationUnsupportedRequest_importance = 1.f)
  {
    postAssignRotationUnsupportedRequest(rotationUnsupportedRequest);
    NMP_ASSERT(rotationUnsupportedRequest_importance >= 0.0f && rotationUnsupportedRequest_importance <= 1.0f);
    m_rotationUnsupportedRequestImportance = rotationUnsupportedRequest_importance;
  }
  NM_INLINE float getRotationUnsupportedRequestImportance() const { return m_rotationUnsupportedRequestImportance; }
  NM_INLINE const float& getRotationUnsupportedRequestImportanceRef() const { return m_rotationUnsupportedRequestImportance; }
  NM_INLINE const RotationRequest& getRotationUnsupportedRequest() const { return rotationUnsupportedRequest; }

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

protected:

  RotationRequest rotationRequest;  ///< Rotation request forwarded on from lowerlevel modules
  RotationRequest rotationUnsupportedRequest;  ///< Separate request for when body is unsupported. Since you don't for instance rotate the chest with the arms when the spine is supported
  RotationRequest oneWayRotationRequest;  ///< Request forwarded from feedIn.oneWayTranslationRequest

  // post-assignment stubs
  NM_INLINE void postAssignRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignRotationUnsupportedRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
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

  // importance values
  float
     m_rotationRequestImportance, 
     m_rotationUnsupportedRequestImportance, 
     m_oneWayRotationRequestImportance;

  friend class RotateCore_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRotationRequestImportance() > 0.0f)
    {
      rotationRequest.validate();
    }
    if (getRotationUnsupportedRequestImportance() > 0.0f)
    {
      rotationUnsupportedRequest.validate();
    }
    if (getOneWayRotationRequestImportance() > 0.0f)
    {
      oneWayRotationRequest.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class RotateCoreFeedbackInputs
 * \ingroup RotateCore
 * \brief FeedbackInputs simply reads requests for rotations of a core (chest or pelvis) and forwards them on to be used by swing/spine etc
 *
 * Data packet definition (164 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct RotateCoreFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(RotateCoreFeedbackInputs));
  }

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

protected:

  RotationRequest oneWayRotationRequest;  ///< Requested rotation from lower level modules

  // post-assignment stubs
  NM_INLINE void postAssignOneWayRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_oneWayRotationRequestImportance;

  friend class RotateCore_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getOneWayRotationRequestImportance() > 0.0f)
    {
      oneWayRotationRequest.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class RotateCoreFeedbackOutputs
 * \ingroup RotateCore
 * \brief FeedbackOutputs simply reads requests for rotations of a core (chest or pelvis) and forwards them on to be used by swing/spine etc
 *
 * Data packet definition (164 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct RotateCoreFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(RotateCoreFeedbackOutputs));
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

protected:

  RotationRequest rotationRequest;  ///< Rotation request forwarded up from lowerlevel modules

  // post-assignment stubs
  NM_INLINE void postAssignRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rotationRequestImportance;

  friend class RotateCore_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRotationRequestImportance() > 0.0f)
    {
      rotationRequest.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ROTATECORE_DATA_H

