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

#ifndef NM_MDF_RANDOMLOOK_DATA_H
#define NM_MDF_RANDOMLOOK_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/RandomLook.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/ModuleRNG.h"
#include "Types/RandomLookParameters.h"
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
 * \class RandomLookData
 * \ingroup RandomLook
 * \brief Data *
 * Data packet definition (125 bytes, 128 aligned)
 */
NMP_ALIGN_PREFIX(32) struct RandomLookData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(RandomLookData));
  }

  NMP::Vector3 lookTarget;                   ///< (Position)
  NMP::Vector3 lastLookTarget;               ///< (Position)
  NMP::Vector3 lastLookTargetRate;           ///< (Velocity)
  ModuleRNG rng;
  float lookTargetTimer;                     ///< (TimePeriod)
  bool needToInitialise;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(lookTarget), "lookTarget");
    NM_VALIDATOR(Vector3Valid(lastLookTarget), "lastLookTarget");
    NM_VALIDATOR(Vector3Valid(lastLookTargetRate), "lastLookTargetRate");
    NM_VALIDATOR(FloatValid(lookTargetTimer), "lookTargetTimer");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class RandomLookInputs
 * \ingroup RandomLook
 * \brief Inputs *
 * Data packet definition (36 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct RandomLookInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(RandomLookInputs));
  }

  NM_INLINE void setRandomLookParameters(const RandomLookParameters& _randomLookParameters, float randomLookParameters_importance = 1.f)
  {
    randomLookParameters = _randomLookParameters;
    postAssignRandomLookParameters(randomLookParameters);
    NMP_ASSERT(randomLookParameters_importance >= 0.0f && randomLookParameters_importance <= 1.0f);
    m_randomLookParametersImportance = randomLookParameters_importance;
  }
  NM_INLINE float getRandomLookParametersImportance() const { return m_randomLookParametersImportance; }
  NM_INLINE const float& getRandomLookParametersImportanceRef() const { return m_randomLookParametersImportance; }
  NM_INLINE const RandomLookParameters& getRandomLookParameters() const { return randomLookParameters; }

protected:

  RandomLookParameters randomLookParameters;

  // post-assignment stubs
  NM_INLINE void postAssignRandomLookParameters(const RandomLookParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_randomLookParametersImportance;

  friend class RandomLook_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRandomLookParametersImportance() > 0.0f)
    {
      randomLookParameters.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class RandomLookOutputs
 * \ingroup RandomLook
 * \brief Outputs *
 * Data packet definition (164 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct RandomLookOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(RandomLookOutputs));
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

protected:

  TargetRequest lookTarget;

  // post-assignment stubs
  NM_INLINE void postAssignLookTarget(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_lookTargetImportance;

  friend class RandomLook_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getLookTargetImportance() > 0.0f)
    {
      lookTarget.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class RandomLookFeedbackInputs
 * \ingroup RandomLook
 * \brief FeedbackInputs *
 * Data packet definition (20 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct RandomLookFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(RandomLookFeedbackInputs));
  }

  NM_INLINE void setHeadPosition(const NMP::Vector3& _headPosition, float headPosition_importance = 1.f)
  {
    headPosition = _headPosition;
    postAssignHeadPosition(headPosition);
    NMP_ASSERT(headPosition_importance >= 0.0f && headPosition_importance <= 1.0f);
    m_headPositionImportance = headPosition_importance;
  }
  NM_INLINE float getHeadPositionImportance() const { return m_headPositionImportance; }
  NM_INLINE const float& getHeadPositionImportanceRef() const { return m_headPositionImportance; }
  NM_INLINE const NMP::Vector3& getHeadPosition() const { return headPosition; }

protected:

  NMP::Vector3 headPosition;                 ///< (Position)

  // post-assignment stubs
  NM_INLINE void postAssignHeadPosition(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "headPosition");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_headPositionImportance;

  friend class RandomLook_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getHeadPositionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(headPosition), "headPosition");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_RANDOMLOOK_DATA_H

