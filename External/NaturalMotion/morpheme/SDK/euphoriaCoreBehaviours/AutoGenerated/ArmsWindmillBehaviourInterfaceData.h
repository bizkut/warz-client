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

#ifndef NM_MDF_ARMSWINDMILLBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_ARMSWINDMILLBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ArmsWindmillBehaviourInterface.module"

// known types
#include "Types/SpinParameters.h"
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
 * \class ArmsWindmillBehaviourInterfaceOutputs
 * \ingroup ArmsWindmillBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (419 bytes, 448 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmsWindmillBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmsWindmillBehaviourInterfaceOutputs));
  }

  enum { maxTargetRequests = 2 };
  NM_INLINE unsigned int getMaxTargetRequests() const { return maxTargetRequests; }
  NM_INLINE void setTargetRequests(unsigned int number, const RotationRequest* _targetRequests, float targetRequests_importance = 1.f)
  {
    NMP_ASSERT(number <= maxTargetRequests);
    NMP_ASSERT(targetRequests_importance >= 0.0f && targetRequests_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      targetRequests[i] = _targetRequests[i];
      postAssignTargetRequests(targetRequests[i]);
      m_targetRequestsImportance[i] = targetRequests_importance;
    }
  }
  NM_INLINE void setTargetRequestsAt(unsigned int index, const RotationRequest& _targetRequests, float targetRequests_importance = 1.f)
  {
    NMP_ASSERT(index < maxTargetRequests);
    targetRequests[index] = _targetRequests;
    NMP_ASSERT(targetRequests_importance >= 0.0f && targetRequests_importance <= 1.0f);
    postAssignTargetRequests(targetRequests[index]);
    m_targetRequestsImportance[index] = targetRequests_importance;
  }
  NM_INLINE RotationRequest& startTargetRequestsModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxTargetRequests);
    m_targetRequestsImportance[index] = -1.0f; // set invalid until stopTargetRequestsModificationAt()
    return targetRequests[index];
  }
  NM_INLINE void stopTargetRequestsModificationAt(unsigned int index, float targetRequests_importance = 1.f)
  {
    postAssignTargetRequests(targetRequests[index]);
    NMP_ASSERT(targetRequests_importance >= 0.0f && targetRequests_importance <= 1.0f);
    m_targetRequestsImportance[index] = targetRequests_importance;
  }
  NM_INLINE float getTargetRequestsImportance(int index) const { return m_targetRequestsImportance[index]; }
  NM_INLINE const float& getTargetRequestsImportanceRef(int index) const { return m_targetRequestsImportance[index]; }
  NM_INLINE unsigned int calculateNumTargetRequests() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_targetRequestsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const RotationRequest& getTargetRequests(unsigned int index) const { NMP_ASSERT(index <= maxTargetRequests); return targetRequests[index]; }

  NM_INLINE void setArmsSpinParams(const SpinParameters& _armsSpinParams, float armsSpinParams_importance = 1.f)
  {
    armsSpinParams = _armsSpinParams;
    postAssignArmsSpinParams(armsSpinParams);
    NMP_ASSERT(armsSpinParams_importance >= 0.0f && armsSpinParams_importance <= 1.0f);
    m_armsSpinParamsImportance = armsSpinParams_importance;
  }
  NM_INLINE float getArmsSpinParamsImportance() const { return m_armsSpinParamsImportance; }
  NM_INLINE const float& getArmsSpinParamsImportanceRef() const { return m_armsSpinParamsImportance; }
  NM_INLINE const SpinParameters& getArmsSpinParams() const { return armsSpinParams; }

  enum { maxSpinStiffnessScales = 2 };
  NM_INLINE unsigned int getMaxSpinStiffnessScales() const { return maxSpinStiffnessScales; }
  NM_INLINE void setSpinStiffnessScales(unsigned int number, const float* _spinStiffnessScales, float spinStiffnessScales_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpinStiffnessScales);
    NMP_ASSERT(spinStiffnessScales_importance >= 0.0f && spinStiffnessScales_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spinStiffnessScales[i] = _spinStiffnessScales[i];
      postAssignSpinStiffnessScales(spinStiffnessScales[i]);
      m_spinStiffnessScalesImportance[i] = spinStiffnessScales_importance;
    }
  }
  NM_INLINE void setSpinStiffnessScalesAt(unsigned int index, const float& _spinStiffnessScales, float spinStiffnessScales_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpinStiffnessScales);
    spinStiffnessScales[index] = _spinStiffnessScales;
    NMP_ASSERT(spinStiffnessScales_importance >= 0.0f && spinStiffnessScales_importance <= 1.0f);
    postAssignSpinStiffnessScales(spinStiffnessScales[index]);
    m_spinStiffnessScalesImportance[index] = spinStiffnessScales_importance;
  }
  NM_INLINE float getSpinStiffnessScalesImportance(int index) const { return m_spinStiffnessScalesImportance[index]; }
  NM_INLINE const float& getSpinStiffnessScalesImportanceRef(int index) const { return m_spinStiffnessScalesImportance[index]; }
  NM_INLINE unsigned int calculateNumSpinStiffnessScales() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_spinStiffnessScalesImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getSpinStiffnessScales(unsigned int index) const { NMP_ASSERT(index <= maxSpinStiffnessScales); return spinStiffnessScales[index]; }

  enum { maxSwingStiffnessScales = 2 };
  NM_INLINE unsigned int getMaxSwingStiffnessScales() const { return maxSwingStiffnessScales; }
  NM_INLINE void setSwingStiffnessScales(unsigned int number, const float* _swingStiffnessScales, float swingStiffnessScales_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSwingStiffnessScales);
    NMP_ASSERT(swingStiffnessScales_importance >= 0.0f && swingStiffnessScales_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      swingStiffnessScales[i] = _swingStiffnessScales[i];
      postAssignSwingStiffnessScales(swingStiffnessScales[i]);
      m_swingStiffnessScalesImportance[i] = swingStiffnessScales_importance;
    }
  }
  NM_INLINE void setSwingStiffnessScalesAt(unsigned int index, const float& _swingStiffnessScales, float swingStiffnessScales_importance = 1.f)
  {
    NMP_ASSERT(index < maxSwingStiffnessScales);
    swingStiffnessScales[index] = _swingStiffnessScales;
    NMP_ASSERT(swingStiffnessScales_importance >= 0.0f && swingStiffnessScales_importance <= 1.0f);
    postAssignSwingStiffnessScales(swingStiffnessScales[index]);
    m_swingStiffnessScalesImportance[index] = swingStiffnessScales_importance;
  }
  NM_INLINE float getSwingStiffnessScalesImportance(int index) const { return m_swingStiffnessScalesImportance[index]; }
  NM_INLINE const float& getSwingStiffnessScalesImportanceRef(int index) const { return m_swingStiffnessScalesImportance[index]; }
  NM_INLINE unsigned int calculateNumSwingStiffnessScales() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_swingStiffnessScalesImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getSwingStiffnessScales(unsigned int index) const { NMP_ASSERT(index <= maxSwingStiffnessScales); return swingStiffnessScales[index]; }

  NM_INLINE void setArmsSpinInPhase(const bool& _armsSpinInPhase, float armsSpinInPhase_importance = 1.f)
  {
    armsSpinInPhase = _armsSpinInPhase;
    NMP_ASSERT(armsSpinInPhase_importance >= 0.0f && armsSpinInPhase_importance <= 1.0f);
    m_armsSpinInPhaseImportance = armsSpinInPhase_importance;
  }
  NM_INLINE float getArmsSpinInPhaseImportance() const { return m_armsSpinInPhaseImportance; }
  NM_INLINE const float& getArmsSpinInPhaseImportanceRef() const { return m_armsSpinInPhaseImportance; }
  NM_INLINE const bool& getArmsSpinInPhase() const { return armsSpinInPhase; }

  NM_INLINE void setArmsOutwardsOnly(const bool& _armsOutwardsOnly, float armsOutwardsOnly_importance = 1.f)
  {
    armsOutwardsOnly = _armsOutwardsOnly;
    NMP_ASSERT(armsOutwardsOnly_importance >= 0.0f && armsOutwardsOnly_importance <= 1.0f);
    m_armsOutwardsOnlyImportance = armsOutwardsOnly_importance;
  }
  NM_INLINE float getArmsOutwardsOnlyImportance() const { return m_armsOutwardsOnlyImportance; }
  NM_INLINE const float& getArmsOutwardsOnlyImportanceRef() const { return m_armsOutwardsOnlyImportance; }
  NM_INLINE const bool& getArmsOutwardsOnly() const { return armsOutwardsOnly; }

protected:

  RotationRequest targetRequests[NetworkConstants::networkMaxNumArms];  ///< Request generated to cause arm spin, it is requesting a pelvis rotation
  SpinParameters armsSpinParams;  /// Poses for each of the limb ends relative to their root
  float spinStiffnessScales[NetworkConstants::networkMaxNumArms];  ///< 1 is normal stiffness  (Multiplier)
  float swingStiffnessScales[NetworkConstants::networkMaxNumArms];  ///< 1 is normal stiffness  (Multiplier)
  bool armsSpinInPhase;
  bool armsOutwardsOnly;  ///< True means arms will swing outwards only, not inwards, it looks more natural

  // post-assignment stubs
  NM_INLINE void postAssignTargetRequests(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmsSpinParams(const SpinParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinStiffnessScales(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "spinStiffnessScales");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSwingStiffnessScales(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "swingStiffnessScales");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_targetRequestsImportance[NetworkConstants::networkMaxNumArms], 
     m_armsSpinParamsImportance, 
     m_spinStiffnessScalesImportance[NetworkConstants::networkMaxNumArms], 
     m_swingStiffnessScalesImportance[NetworkConstants::networkMaxNumArms], 
     m_armsSpinInPhaseImportance, 
     m_armsOutwardsOnlyImportance;


public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numTargetRequests = calculateNumTargetRequests();
      for (uint32_t i=0; i<numTargetRequests; i++)
      {
        if (getTargetRequestsImportance(i) > 0.0f)
          targetRequests[i].validate();
      }
    }
    if (getArmsSpinParamsImportance() > 0.0f)
    {
      armsSpinParams.validate();
    }
    {
      uint32_t numSpinStiffnessScales = calculateNumSpinStiffnessScales();
      for (uint32_t i=0; i<numSpinStiffnessScales; i++)
      {
        if (getSpinStiffnessScalesImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(spinStiffnessScales[i]), "spinStiffnessScales");
        }
      }
    }
    {
      uint32_t numSwingStiffnessScales = calculateNumSwingStiffnessScales();
      for (uint32_t i=0; i<numSwingStiffnessScales; i++)
      {
        if (getSwingStiffnessScalesImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(swingStiffnessScales[i]), "swingStiffnessScales");
        }
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMSWINDMILLBEHAVIOURINTERFACE_DATA_H

