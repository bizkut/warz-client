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

#ifndef NM_MDF_HAZARDAWARENESSBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_HAZARDAWARENESSBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/HazardAwarenessBehaviourInterface.module"

// known types
#include "Types/Hazard.h"

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
 * \class HazardAwarenessBehaviourInterfaceData
 * \ingroup HazardAwarenessBehaviourInterface
 * \brief Data Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (4 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HazardAwarenessBehaviourInterfaceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HazardAwarenessBehaviourInterfaceData));
  }

  float hazardFreeTime;                      ///< (TimePeriod)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(hazardFreeTime), "hazardFreeTime");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HazardAwarenessBehaviourInterfaceUserInputs
 * \ingroup HazardAwarenessBehaviourInterface
 * \brief UserInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (5 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HazardAwarenessBehaviourInterfaceUserInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HazardAwarenessBehaviourInterfaceUserInputs));
  }

  NM_INLINE void setUseControlledVelocity(const bool& _useControlledVelocity, float useControlledVelocity_importance = 1.f)
  {
    useControlledVelocity = _useControlledVelocity;
    NMP_ASSERT(useControlledVelocity_importance >= 0.0f && useControlledVelocity_importance <= 1.0f);
    m_useControlledVelocityImportance = useControlledVelocity_importance;
  }
  NM_INLINE float getUseControlledVelocityImportance() const { return m_useControlledVelocityImportance; }
  NM_INLINE const float& getUseControlledVelocityImportanceRef() const { return m_useControlledVelocityImportance; }
  NM_INLINE const bool& getUseControlledVelocity() const { return useControlledVelocity; }

protected:

  bool useControlledVelocity;

  // importance values
  float
     m_useControlledVelocityImportance;

  friend class HazardAwarenessBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HazardAwarenessBehaviourInterfaceOutputs
 * \ingroup HazardAwarenessBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (21 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HazardAwarenessBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HazardAwarenessBehaviourInterfaceOutputs));
  }

  NM_INLINE void setIgnoreVerticalPredictionAmount(const float& _ignoreVerticalPredictionAmount, float ignoreVerticalPredictionAmount_importance = 1.f)
  {
    ignoreVerticalPredictionAmount = _ignoreVerticalPredictionAmount;
    postAssignIgnoreVerticalPredictionAmount(ignoreVerticalPredictionAmount);
    NMP_ASSERT(ignoreVerticalPredictionAmount_importance >= 0.0f && ignoreVerticalPredictionAmount_importance <= 1.0f);
    m_ignoreVerticalPredictionAmountImportance = ignoreVerticalPredictionAmount_importance;
  }
  NM_INLINE float getIgnoreVerticalPredictionAmountImportance() const { return m_ignoreVerticalPredictionAmountImportance; }
  NM_INLINE const float& getIgnoreVerticalPredictionAmountImportanceRef() const { return m_ignoreVerticalPredictionAmountImportance; }
  NM_INLINE const float& getIgnoreVerticalPredictionAmount() const { return ignoreVerticalPredictionAmount; }

  NM_INLINE void setProbeRadius(const float& _probeRadius, float probeRadius_importance = 1.f)
  {
    probeRadius = _probeRadius;
    postAssignProbeRadius(probeRadius);
    NMP_ASSERT(probeRadius_importance >= 0.0f && probeRadius_importance <= 1.0f);
    m_probeRadiusImportance = probeRadius_importance;
  }
  NM_INLINE float getProbeRadiusImportance() const { return m_probeRadiusImportance; }
  NM_INLINE const float& getProbeRadiusImportanceRef() const { return m_probeRadiusImportance; }
  NM_INLINE const float& getProbeRadius() const { return probeRadius; }

  NM_INLINE void setVelocityIsControlled(const bool& _velocityIsControlled, float velocityIsControlled_importance = 1.f)
  {
    velocityIsControlled = _velocityIsControlled;
    NMP_ASSERT(velocityIsControlled_importance >= 0.0f && velocityIsControlled_importance <= 1.0f);
    m_velocityIsControlledImportance = velocityIsControlled_importance;
  }
  NM_INLINE float getVelocityIsControlledImportance() const { return m_velocityIsControlledImportance; }
  NM_INLINE const float& getVelocityIsControlledImportanceRef() const { return m_velocityIsControlledImportance; }
  NM_INLINE const bool& getVelocityIsControlled() const { return velocityIsControlled; }

protected:

  float ignoreVerticalPredictionAmount;  /// 0  is default, 1 means chest assumes no vertical prediction of velocity or acceleration  (Weight)
  float probeRadius;                         ///< (Length)
  bool velocityIsControlled;

  // post-assignment stubs
  NM_INLINE void postAssignIgnoreVerticalPredictionAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "ignoreVerticalPredictionAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignProbeRadius(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "probeRadius");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_ignoreVerticalPredictionAmountImportance, 
     m_probeRadiusImportance, 
     m_velocityIsControlledImportance;

  friend class HazardAwarenessBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getIgnoreVerticalPredictionAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(ignoreVerticalPredictionAmount), "ignoreVerticalPredictionAmount");
    }
    if (getProbeRadiusImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(probeRadius), "probeRadius");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HazardAwarenessBehaviourInterfaceFeedbackInputs
 * \ingroup HazardAwarenessBehaviourInterface
 * \brief FeedbackInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (372 bytes, 384 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HazardAwarenessBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HazardAwarenessBehaviourInterfaceFeedbackInputs));
  }

  NM_INLINE void setHazard(const Hazard& _hazard, float hazard_importance = 1.f)
  {
    hazard = _hazard;
    postAssignHazard(hazard);
    NMP_ASSERT(hazard_importance >= 0.0f && hazard_importance <= 1.0f);
    m_hazardImportance = hazard_importance;
  }
  NM_INLINE Hazard& startHazardModification()
  {
    m_hazardImportance = -1.0f; // set invalid until stopHazardModification()
    return hazard;
  }
  NM_INLINE void stopHazardModification(float hazard_importance = 1.f)
  {
    postAssignHazard(hazard);
    NMP_ASSERT(hazard_importance >= 0.0f && hazard_importance <= 1.0f);
    m_hazardImportance = hazard_importance;
  }
  NM_INLINE float getHazardImportance() const { return m_hazardImportance; }
  NM_INLINE const float& getHazardImportanceRef() const { return m_hazardImportance; }
  NM_INLINE const Hazard& getHazard() const { return hazard; }

protected:

  Hazard hazard;

  // post-assignment stubs
  NM_INLINE void postAssignHazard(const Hazard& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_hazardImportance;

  friend class HazardAwarenessBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getHazardImportance() > 0.0f)
    {
      hazard.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HazardAwarenessBehaviourInterfaceFeedbackOutputs
 * \ingroup HazardAwarenessBehaviourInterface
 * \brief FeedbackOutputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HazardAwarenessBehaviourInterfaceFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HazardAwarenessBehaviourInterfaceFeedbackOutputs));
  }

  NM_INLINE float getHazardImportance() const { return m_hazardImportance; }
  NM_INLINE const float& getHazardImportanceRef() const { return m_hazardImportance; }
  NM_INLINE const Hazard& getHazard() const { return hazard; }

  NM_INLINE void setHazardFreeTime(const float& _hazardFreeTime, float hazardFreeTime_importance = 1.f)
  {
    hazardFreeTime = _hazardFreeTime;
    postAssignHazardFreeTime(hazardFreeTime);
    NMP_ASSERT(hazardFreeTime_importance >= 0.0f && hazardFreeTime_importance <= 1.0f);
    m_hazardFreeTimeImportance = hazardFreeTime_importance;
  }
  NM_INLINE float getHazardFreeTimeImportance() const { return m_hazardFreeTimeImportance; }
  NM_INLINE const float& getHazardFreeTimeImportanceRef() const { return m_hazardFreeTimeImportance; }
  NM_INLINE const float& getHazardFreeTime() const { return hazardFreeTime; }

  NM_INLINE void setHazardousness(const float& _hazardousness, float hazardousness_importance = 1.f)
  {
    hazardousness = _hazardousness;
    postAssignHazardousness(hazardousness);
    NMP_ASSERT(hazardousness_importance >= 0.0f && hazardousness_importance <= 1.0f);
    m_hazardousnessImportance = hazardousness_importance;
  }
  NM_INLINE float getHazardousnessImportance() const { return m_hazardousnessImportance; }
  NM_INLINE const float& getHazardousnessImportanceRef() const { return m_hazardousnessImportance; }
  NM_INLINE const float& getHazardousness() const { return hazardousness; }

protected:

  Hazard hazard;
  float hazardFreeTime;                      ///< (TimePeriod)
  float hazardousness;  ///< unitless 0-1 value

  // post-assignment stubs
  NM_INLINE void postAssignHazard(const Hazard& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHazardFreeTime(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "hazardFreeTime");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHazardousness(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "hazardousness");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_hazardImportance, 
     m_hazardFreeTimeImportance, 
     m_hazardousnessImportance;

  friend class HazardAwarenessBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getHazardImportance() > 0.0f)
    {
      hazard.validate();
    }
    if (getHazardFreeTimeImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(hazardFreeTime), "hazardFreeTime");
    }
    if (getHazardousnessImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(hazardousness), "hazardousness");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HAZARDAWARENESSBEHAVIOURINTERFACE_DATA_H

