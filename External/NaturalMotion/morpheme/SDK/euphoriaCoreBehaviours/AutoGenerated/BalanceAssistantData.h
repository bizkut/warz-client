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

#ifndef NM_MDF_BALANCEASSISTANT_DATA_H
#define NM_MDF_BALANCEASSISTANT_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/BalanceAssistant.module"

// external types
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/BalanceAssistanceParameters.h"

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
 * \class BalanceAssistantInputs
 * \ingroup BalanceAssistant
 * \brief Inputs *
 * Data packet definition (128 bytes, 160 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalanceAssistantInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalanceAssistantInputs));
  }

  NM_INLINE void setDesiredPelvisTM(const ER::LimbTransform& _desiredPelvisTM, float desiredPelvisTM_importance = 1.f)
  {
    desiredPelvisTM = _desiredPelvisTM;
    NMP_ASSERT(desiredPelvisTM_importance >= 0.0f && desiredPelvisTM_importance <= 1.0f);
    m_desiredPelvisTMImportance = desiredPelvisTM_importance;
  }
  NM_INLINE ER::LimbTransform& startDesiredPelvisTMModification()
  {
    m_desiredPelvisTMImportance = -1.0f; // set invalid until stopDesiredPelvisTMModification()
    return desiredPelvisTM;
  }
  NM_INLINE void stopDesiredPelvisTMModification(float desiredPelvisTM_importance = 1.f)
  {
    NMP_ASSERT(desiredPelvisTM_importance >= 0.0f && desiredPelvisTM_importance <= 1.0f);
    m_desiredPelvisTMImportance = desiredPelvisTM_importance;
  }
  NM_INLINE float getDesiredPelvisTMImportance() const { return m_desiredPelvisTMImportance; }
  NM_INLINE const float& getDesiredPelvisTMImportanceRef() const { return m_desiredPelvisTMImportance; }
  NM_INLINE const ER::LimbTransform& getDesiredPelvisTM() const { return desiredPelvisTM; }

  NM_INLINE void setBalanceAssistanceParameters(const BalanceAssistanceParameters& _balanceAssistanceParameters, float balanceAssistanceParameters_importance = 1.f)
  {
    balanceAssistanceParameters = _balanceAssistanceParameters;
    postAssignBalanceAssistanceParameters(balanceAssistanceParameters);
    NMP_ASSERT(balanceAssistanceParameters_importance >= 0.0f && balanceAssistanceParameters_importance <= 1.0f);
    m_balanceAssistanceParametersImportance = balanceAssistanceParameters_importance;
  }
  NM_INLINE float getBalanceAssistanceParametersImportance() const { return m_balanceAssistanceParametersImportance; }
  NM_INLINE const float& getBalanceAssistanceParametersImportanceRef() const { return m_balanceAssistanceParametersImportance; }
  NM_INLINE const BalanceAssistanceParameters& getBalanceAssistanceParameters() const { return balanceAssistanceParameters; }

  NM_INLINE void setBalanceAmount(const float& _balanceAmount, float balanceAmount_importance = 1.f)
  {
    balanceAmount = _balanceAmount;
    postAssignBalanceAmount(balanceAmount);
    NMP_ASSERT(balanceAmount_importance >= 0.0f && balanceAmount_importance <= 1.0f);
    m_balanceAmountImportance = balanceAmount_importance;
  }
  NM_INLINE float getBalanceAmountImportance() const { return m_balanceAmountImportance; }
  NM_INLINE const float& getBalanceAmountImportanceRef() const { return m_balanceAmountImportance; }
  NM_INLINE const float& getBalanceAmount() const { return balanceAmount; }

protected:

  ER::LimbTransform desiredPelvisTM;
  BalanceAssistanceParameters balanceAssistanceParameters;
  float balanceAmount;                       ///< (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignBalanceAssistanceParameters(const BalanceAssistanceParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "balanceAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_desiredPelvisTMImportance, 
     m_balanceAssistanceParametersImportance, 
     m_balanceAmountImportance;

  friend class BalanceAssistant_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getBalanceAssistanceParametersImportance() > 0.0f)
    {
      balanceAssistanceParameters.validate();
    }
    if (getBalanceAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(balanceAmount), "balanceAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BalanceAssistantFeedbackInputs
 * \ingroup BalanceAssistant
 * \brief FeedbackInputs *
 * Data packet definition (10 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalanceAssistantFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalanceAssistantFeedbackInputs));
  }

  enum { maxIsStepping = 2 };
  NM_INLINE unsigned int getMaxIsStepping() const { return maxIsStepping; }
  NM_INLINE void setIsStepping(unsigned int number, const bool* _isStepping, float isStepping_importance = 1.f)
  {
    NMP_ASSERT(number <= maxIsStepping);
    NMP_ASSERT(isStepping_importance >= 0.0f && isStepping_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      isStepping[i] = _isStepping[i];
      m_isSteppingImportance[i] = isStepping_importance;
    }
  }
  NM_INLINE void setIsSteppingAt(unsigned int index, const bool& _isStepping, float isStepping_importance = 1.f)
  {
    NMP_ASSERT(index < maxIsStepping);
    isStepping[index] = _isStepping;
    NMP_ASSERT(isStepping_importance >= 0.0f && isStepping_importance <= 1.0f);
    m_isSteppingImportance[index] = isStepping_importance;
  }
  NM_INLINE float getIsSteppingImportance(int index) const { return m_isSteppingImportance[index]; }
  NM_INLINE const float& getIsSteppingImportanceRef(int index) const { return m_isSteppingImportance[index]; }
  NM_INLINE unsigned int calculateNumIsStepping() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_isSteppingImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const bool& getIsStepping(unsigned int index) const { NMP_ASSERT(index <= maxIsStepping); return isStepping[index]; }

protected:

  bool isStepping[NetworkConstants::networkMaxNumLegs];

  // importance values
  float
     m_isSteppingImportance[NetworkConstants::networkMaxNumLegs];

  friend class BalanceAssistant_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BALANCEASSISTANT_DATA_H

