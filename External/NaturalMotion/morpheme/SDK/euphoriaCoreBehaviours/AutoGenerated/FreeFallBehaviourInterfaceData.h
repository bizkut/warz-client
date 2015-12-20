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

#ifndef NM_MDF_FREEFALLBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_FREEFALLBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/FreeFallBehaviourInterface.module"

// known types
#include "Types/FreefallParameters.h"

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
 * \class FreeFallBehaviourInterfaceOutputs
 * \ingroup FreeFallBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (121 bytes, 128 aligned)
 */
NMP_ALIGN_PREFIX(32) struct FreeFallBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(FreeFallBehaviourInterfaceOutputs));
  }

  NM_INLINE void setFreefallParams(const FreefallParameters& _freefallParams, float freefallParams_importance = 1.f)
  {
    freefallParams = _freefallParams;
    postAssignFreefallParams(freefallParams);
    NMP_ASSERT(freefallParams_importance >= 0.0f && freefallParams_importance <= 1.0f);
    m_freefallParamsImportance = freefallParams_importance;
  }
  NM_INLINE FreefallParameters& startFreefallParamsModification()
  {
    m_freefallParamsImportance = -1.0f; // set invalid until stopFreefallParamsModification()
    return freefallParams;
  }
  NM_INLINE void stopFreefallParamsModification(float freefallParams_importance = 1.f)
  {
    postAssignFreefallParams(freefallParams);
    NMP_ASSERT(freefallParams_importance >= 0.0f && freefallParams_importance <= 1.0f);
    m_freefallParamsImportance = freefallParams_importance;
  }
  NM_INLINE float getFreefallParamsImportance() const { return m_freefallParamsImportance; }
  NM_INLINE const float& getFreefallParamsImportanceRef() const { return m_freefallParamsImportance; }
  NM_INLINE const FreefallParameters& getFreefallParams() const { return freefallParams; }

  enum { maxArmsSpinStiffnessScales = 2 };
  NM_INLINE unsigned int getMaxArmsSpinStiffnessScales() const { return maxArmsSpinStiffnessScales; }
  NM_INLINE void setArmsSpinStiffnessScales(unsigned int number, const float* _armsSpinStiffnessScales, float armsSpinStiffnessScales_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmsSpinStiffnessScales);
    NMP_ASSERT(armsSpinStiffnessScales_importance >= 0.0f && armsSpinStiffnessScales_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armsSpinStiffnessScales[i] = _armsSpinStiffnessScales[i];
      postAssignArmsSpinStiffnessScales(armsSpinStiffnessScales[i]);
      m_armsSpinStiffnessScalesImportance[i] = armsSpinStiffnessScales_importance;
    }
  }
  NM_INLINE void setArmsSpinStiffnessScalesAt(unsigned int index, const float& _armsSpinStiffnessScales, float armsSpinStiffnessScales_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmsSpinStiffnessScales);
    armsSpinStiffnessScales[index] = _armsSpinStiffnessScales;
    NMP_ASSERT(armsSpinStiffnessScales_importance >= 0.0f && armsSpinStiffnessScales_importance <= 1.0f);
    postAssignArmsSpinStiffnessScales(armsSpinStiffnessScales[index]);
    m_armsSpinStiffnessScalesImportance[index] = armsSpinStiffnessScales_importance;
  }
  NM_INLINE float getArmsSpinStiffnessScalesImportance(int index) const { return m_armsSpinStiffnessScalesImportance[index]; }
  NM_INLINE const float& getArmsSpinStiffnessScalesImportanceRef(int index) const { return m_armsSpinStiffnessScalesImportance[index]; }
  NM_INLINE unsigned int calculateNumArmsSpinStiffnessScales() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armsSpinStiffnessScalesImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getArmsSpinStiffnessScales(unsigned int index) const { NMP_ASSERT(index <= maxArmsSpinStiffnessScales); return armsSpinStiffnessScales[index]; }

  enum { maxLegsSpinStiffnessScales = 2 };
  NM_INLINE unsigned int getMaxLegsSpinStiffnessScales() const { return maxLegsSpinStiffnessScales; }
  NM_INLINE void setLegsSpinStiffnessScales(unsigned int number, const float* _legsSpinStiffnessScales, float legsSpinStiffnessScales_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegsSpinStiffnessScales);
    NMP_ASSERT(legsSpinStiffnessScales_importance >= 0.0f && legsSpinStiffnessScales_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legsSpinStiffnessScales[i] = _legsSpinStiffnessScales[i];
      postAssignLegsSpinStiffnessScales(legsSpinStiffnessScales[i]);
      m_legsSpinStiffnessScalesImportance[i] = legsSpinStiffnessScales_importance;
    }
  }
  NM_INLINE void setLegsSpinStiffnessScalesAt(unsigned int index, const float& _legsSpinStiffnessScales, float legsSpinStiffnessScales_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegsSpinStiffnessScales);
    legsSpinStiffnessScales[index] = _legsSpinStiffnessScales;
    NMP_ASSERT(legsSpinStiffnessScales_importance >= 0.0f && legsSpinStiffnessScales_importance <= 1.0f);
    postAssignLegsSpinStiffnessScales(legsSpinStiffnessScales[index]);
    m_legsSpinStiffnessScalesImportance[index] = legsSpinStiffnessScales_importance;
  }
  NM_INLINE float getLegsSpinStiffnessScalesImportance(int index) const { return m_legsSpinStiffnessScalesImportance[index]; }
  NM_INLINE const float& getLegsSpinStiffnessScalesImportanceRef(int index) const { return m_legsSpinStiffnessScalesImportance[index]; }
  NM_INLINE unsigned int calculateNumLegsSpinStiffnessScales() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legsSpinStiffnessScalesImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getLegsSpinStiffnessScales(unsigned int index) const { NMP_ASSERT(index <= maxLegsSpinStiffnessScales); return legsSpinStiffnessScales[index]; }

  NM_INLINE void setArmsSpinInPhase(const bool& _armsSpinInPhase, float armsSpinInPhase_importance = 1.f)
  {
    armsSpinInPhase = _armsSpinInPhase;
    NMP_ASSERT(armsSpinInPhase_importance >= 0.0f && armsSpinInPhase_importance <= 1.0f);
    m_armsSpinInPhaseImportance = armsSpinInPhase_importance;
  }
  NM_INLINE float getArmsSpinInPhaseImportance() const { return m_armsSpinInPhaseImportance; }
  NM_INLINE const float& getArmsSpinInPhaseImportanceRef() const { return m_armsSpinInPhaseImportance; }
  NM_INLINE const bool& getArmsSpinInPhase() const { return armsSpinInPhase; }

protected:

  FreefallParameters freefallParams;
  float armsSpinStiffnessScales[NetworkConstants::networkMaxNumArms];  // Poses for each of the limb ends relative to their root  (Multiplier)
  float legsSpinStiffnessScales[NetworkConstants::networkMaxNumLegs];  ///< (Multiplier)
  bool armsSpinInPhase;

  // post-assignment stubs
  NM_INLINE void postAssignFreefallParams(const FreefallParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmsSpinStiffnessScales(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "armsSpinStiffnessScales");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegsSpinStiffnessScales(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "legsSpinStiffnessScales");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_freefallParamsImportance, 
     m_armsSpinStiffnessScalesImportance[NetworkConstants::networkMaxNumArms], 
     m_legsSpinStiffnessScalesImportance[NetworkConstants::networkMaxNumLegs], 
     m_armsSpinInPhaseImportance;

  friend class FreeFallBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getFreefallParamsImportance() > 0.0f)
    {
      freefallParams.validate();
    }
    {
      uint32_t numArmsSpinStiffnessScales = calculateNumArmsSpinStiffnessScales();
      for (uint32_t i=0; i<numArmsSpinStiffnessScales; i++)
      {
        if (getArmsSpinStiffnessScalesImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(armsSpinStiffnessScales[i]), "armsSpinStiffnessScales");
        }
      }
    }
    {
      uint32_t numLegsSpinStiffnessScales = calculateNumLegsSpinStiffnessScales();
      for (uint32_t i=0; i<numLegsSpinStiffnessScales; i++)
      {
        if (getLegsSpinStiffnessScalesImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(legsSpinStiffnessScales[i]), "legsSpinStiffnessScales");
        }
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class FreeFallBehaviourInterfaceFeedbackInputs
 * \ingroup FreeFallBehaviourInterface
 * \brief FeedbackInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (13 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct FreeFallBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(FreeFallBehaviourInterfaceFeedbackInputs));
  }

  NM_INLINE void setOrientationError(const float& _orientationError, float orientationError_importance = 1.f)
  {
    orientationError = _orientationError;
    postAssignOrientationError(orientationError);
    NMP_ASSERT(orientationError_importance >= 0.0f && orientationError_importance <= 1.0f);
    m_orientationErrorImportance = orientationError_importance;
  }
  NM_INLINE float getOrientationErrorImportance() const { return m_orientationErrorImportance; }
  NM_INLINE const float& getOrientationErrorImportanceRef() const { return m_orientationErrorImportance; }
  NM_INLINE const float& getOrientationError() const { return orientationError; }

  NM_INLINE void setIsInFreefall(const bool& _isInFreefall, float isInFreefall_importance = 1.f)
  {
    isInFreefall = _isInFreefall;
    NMP_ASSERT(isInFreefall_importance >= 0.0f && isInFreefall_importance <= 1.0f);
    m_isInFreefallImportance = isInFreefall_importance;
  }
  NM_INLINE float getIsInFreefallImportance() const { return m_isInFreefallImportance; }
  NM_INLINE const float& getIsInFreefallImportanceRef() const { return m_isInFreefallImportance; }
  NM_INLINE const bool& getIsInFreefall() const { return isInFreefall; }

protected:

  float orientationError;                    ///< (Angle)
  bool isInFreefall;

  // post-assignment stubs
  NM_INLINE void postAssignOrientationError(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "orientationError");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_orientationErrorImportance, 
     m_isInFreefallImportance;

  friend class FreeFallBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getOrientationErrorImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(orientationError), "orientationError");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class FreeFallBehaviourInterfaceFeedbackOutputs
 * \ingroup FreeFallBehaviourInterface
 * \brief FeedbackOutputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (13 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct FreeFallBehaviourInterfaceFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(FreeFallBehaviourInterfaceFeedbackOutputs));
  }

  NM_INLINE void setOrientationError(const float& _orientationError, float orientationError_importance = 1.f)
  {
    orientationError = _orientationError;
    postAssignOrientationError(orientationError);
    NMP_ASSERT(orientationError_importance >= 0.0f && orientationError_importance <= 1.0f);
    m_orientationErrorImportance = orientationError_importance;
  }
  NM_INLINE float getOrientationErrorImportance() const { return m_orientationErrorImportance; }
  NM_INLINE const float& getOrientationErrorImportanceRef() const { return m_orientationErrorImportance; }
  NM_INLINE const float& getOrientationError() const { return orientationError; }

  NM_INLINE void setIsInFreefall(const bool& _isInFreefall, float isInFreefall_importance = 1.f)
  {
    isInFreefall = _isInFreefall;
    NMP_ASSERT(isInFreefall_importance >= 0.0f && isInFreefall_importance <= 1.0f);
    m_isInFreefallImportance = isInFreefall_importance;
  }
  NM_INLINE float getIsInFreefallImportance() const { return m_isInFreefallImportance; }
  NM_INLINE const float& getIsInFreefallImportanceRef() const { return m_isInFreefallImportance; }
  NM_INLINE const bool& getIsInFreefall() const { return isInFreefall; }

protected:

  float orientationError;                    ///< (Angle)
  bool isInFreefall;

  // post-assignment stubs
  NM_INLINE void postAssignOrientationError(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "orientationError");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_orientationErrorImportance, 
     m_isInFreefallImportance;

  friend class FreeFallBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getOrientationErrorImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(orientationError), "orientationError");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_FREEFALLBEHAVIOURINTERFACE_DATA_H

