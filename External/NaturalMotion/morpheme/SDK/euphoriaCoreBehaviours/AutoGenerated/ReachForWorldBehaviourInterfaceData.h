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

#ifndef NM_MDF_REACHFORWORLDBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_REACHFORWORLDBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ReachForWorldBehaviourInterface.module"

// known types
#include "Types/LimbControl.h"
#include "Types/ReachWorldParameters.h"
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
 * \class ReachForWorldBehaviourInterfaceOutputs
 * \ingroup ReachForWorldBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (1116 bytes, 1120 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ReachForWorldBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ReachForWorldBehaviourInterfaceOutputs));
  }

  enum { maxLimbControls = 2 };
  NM_INLINE unsigned int getMaxLimbControls() const { return maxLimbControls; }
  NM_INLINE void setLimbControls(unsigned int number, const LimbControl* _limbControls, float limbControls_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLimbControls);
    NMP_ASSERT(limbControls_importance >= 0.0f && limbControls_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      limbControls[i] = _limbControls[i];
      postAssignLimbControls(limbControls[i]);
      m_limbControlsImportance[i] = limbControls_importance;
    }
  }
  NM_INLINE void setLimbControlsAt(unsigned int index, const LimbControl& _limbControls, float limbControls_importance = 1.f)
  {
    NMP_ASSERT(index < maxLimbControls);
    limbControls[index] = _limbControls;
    NMP_ASSERT(limbControls_importance >= 0.0f && limbControls_importance <= 1.0f);
    postAssignLimbControls(limbControls[index]);
    m_limbControlsImportance[index] = limbControls_importance;
  }
  NM_INLINE LimbControl& startLimbControlsModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxLimbControls);
    m_limbControlsImportance[index] = -1.0f; // set invalid until stopLimbControlsModificationAt()
    return limbControls[index];
  }
  NM_INLINE void stopLimbControlsModificationAt(unsigned int index, float limbControls_importance = 1.f)
  {
    postAssignLimbControls(limbControls[index]);
    NMP_ASSERT(limbControls_importance >= 0.0f && limbControls_importance <= 1.0f);
    m_limbControlsImportance[index] = limbControls_importance;
  }
  NM_INLINE float getLimbControlsImportance(int index) const { return m_limbControlsImportance[index]; }
  NM_INLINE const float& getLimbControlsImportanceRef(int index) const { return m_limbControlsImportance[index]; }
  NM_INLINE unsigned int calculateNumLimbControls() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_limbControlsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const LimbControl& getLimbControls(unsigned int index) const { NMP_ASSERT(index <= maxLimbControls); return limbControls[index]; }

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

  enum { maxReachParams = 2 };
  NM_INLINE unsigned int getMaxReachParams() const { return maxReachParams; }
  NM_INLINE void setReachParams(unsigned int number, const ReachWorldParameters* _reachParams, float reachParams_importance = 1.f)
  {
    NMP_ASSERT(number <= maxReachParams);
    NMP_ASSERT(reachParams_importance >= 0.0f && reachParams_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      reachParams[i] = _reachParams[i];
      postAssignReachParams(reachParams[i]);
      m_reachParamsImportance[i] = reachParams_importance;
    }
  }
  NM_INLINE void setReachParamsAt(unsigned int index, const ReachWorldParameters& _reachParams, float reachParams_importance = 1.f)
  {
    NMP_ASSERT(index < maxReachParams);
    reachParams[index] = _reachParams;
    NMP_ASSERT(reachParams_importance >= 0.0f && reachParams_importance <= 1.0f);
    postAssignReachParams(reachParams[index]);
    m_reachParamsImportance[index] = reachParams_importance;
  }
  NM_INLINE ReachWorldParameters& startReachParamsModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxReachParams);
    m_reachParamsImportance[index] = -1.0f; // set invalid until stopReachParamsModificationAt()
    return reachParams[index];
  }
  NM_INLINE void stopReachParamsModificationAt(unsigned int index, float reachParams_importance = 1.f)
  {
    postAssignReachParams(reachParams[index]);
    NMP_ASSERT(reachParams_importance >= 0.0f && reachParams_importance <= 1.0f);
    m_reachParamsImportance[index] = reachParams_importance;
  }
  NM_INLINE float getReachParamsImportance(int index) const { return m_reachParamsImportance[index]; }
  NM_INLINE const float& getReachParamsImportanceRef(int index) const { return m_reachParamsImportance[index]; }
  NM_INLINE unsigned int calculateNumReachParams() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_reachParamsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const ReachWorldParameters& getReachParams(unsigned int index) const { NMP_ASSERT(index <= maxReachParams); return reachParams[index]; }

  NM_INLINE void setSpineDampingIncrease(const float& _spineDampingIncrease, float spineDampingIncrease_importance = 1.f)
  {
    spineDampingIncrease = _spineDampingIncrease;
    postAssignSpineDampingIncrease(spineDampingIncrease);
    NMP_ASSERT(spineDampingIncrease_importance >= 0.0f && spineDampingIncrease_importance <= 1.0f);
    m_spineDampingIncreaseImportance = spineDampingIncrease_importance;
  }
  NM_INLINE float getSpineDampingIncreaseImportance() const { return m_spineDampingIncreaseImportance; }
  NM_INLINE const float& getSpineDampingIncreaseImportanceRef() const { return m_spineDampingIncreaseImportance; }
  NM_INLINE const float& getSpineDampingIncrease() const { return spineDampingIncrease; }

protected:

  LimbControl limbControls[NetworkConstants::networkMaxNumArms];
  TargetRequest lookTarget;
  ReachWorldParameters reachParams[NetworkConstants::networkMaxNumArms];
  float spineDampingIncrease;                ///< (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignLimbControls(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLookTarget(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignReachParams(const ReachWorldParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpineDampingIncrease(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "spineDampingIncrease");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_limbControlsImportance[NetworkConstants::networkMaxNumArms], 
     m_lookTargetImportance, 
     m_reachParamsImportance[NetworkConstants::networkMaxNumArms], 
     m_spineDampingIncreaseImportance;

  friend class ReachForWorldBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numLimbControls = calculateNumLimbControls();
      for (uint32_t i=0; i<numLimbControls; i++)
      {
        if (getLimbControlsImportance(i) > 0.0f)
          limbControls[i].validate();
      }
    }
    if (getLookTargetImportance() > 0.0f)
    {
      lookTarget.validate();
    }
    {
      uint32_t numReachParams = calculateNumReachParams();
      for (uint32_t i=0; i<numReachParams; i++)
      {
        if (getReachParamsImportance(i) > 0.0f)
          reachParams[i].validate();
      }
    }
    if (getSpineDampingIncreaseImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(spineDampingIncrease), "spineDampingIncrease");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ReachForWorldBehaviourInterfaceFeedbackInputs
 * \ingroup ReachForWorldBehaviourInterface
 * \brief FeedbackInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (16 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ReachForWorldBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ReachForWorldBehaviourInterfaceFeedbackInputs));
  }

  enum { maxDistanceToTarget = 2 };
  NM_INLINE unsigned int getMaxDistanceToTarget() const { return maxDistanceToTarget; }
  NM_INLINE void setDistanceToTarget(unsigned int number, const float* _distanceToTarget, float distanceToTarget_importance = 1.f)
  {
    NMP_ASSERT(number <= maxDistanceToTarget);
    NMP_ASSERT(distanceToTarget_importance >= 0.0f && distanceToTarget_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      distanceToTarget[i] = _distanceToTarget[i];
      postAssignDistanceToTarget(distanceToTarget[i]);
      m_distanceToTargetImportance[i] = distanceToTarget_importance;
    }
  }
  NM_INLINE void setDistanceToTargetAt(unsigned int index, const float& _distanceToTarget, float distanceToTarget_importance = 1.f)
  {
    NMP_ASSERT(index < maxDistanceToTarget);
    distanceToTarget[index] = _distanceToTarget;
    NMP_ASSERT(distanceToTarget_importance >= 0.0f && distanceToTarget_importance <= 1.0f);
    postAssignDistanceToTarget(distanceToTarget[index]);
    m_distanceToTargetImportance[index] = distanceToTarget_importance;
  }
  NM_INLINE float getDistanceToTargetImportance(int index) const { return m_distanceToTargetImportance[index]; }
  NM_INLINE const float& getDistanceToTargetImportanceRef(int index) const { return m_distanceToTargetImportance[index]; }
  NM_INLINE unsigned int calculateNumDistanceToTarget() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_distanceToTargetImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getDistanceToTarget(unsigned int index) const { NMP_ASSERT(index <= maxDistanceToTarget); return distanceToTarget[index]; }

protected:

  float distanceToTarget[NetworkConstants::networkMaxNumArms];  ///< (Distance)

  // post-assignment stubs
  NM_INLINE void postAssignDistanceToTarget(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "distanceToTarget");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_distanceToTargetImportance[NetworkConstants::networkMaxNumArms];

  friend class ReachForWorldBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numDistanceToTarget = calculateNumDistanceToTarget();
      for (uint32_t i=0; i<numDistanceToTarget; i++)
      {
        if (getDistanceToTargetImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(distanceToTarget[i]), "distanceToTarget");
        }
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ReachForWorldBehaviourInterfaceFeedbackOutputs
 * \ingroup ReachForWorldBehaviourInterface
 * \brief FeedbackOutputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (16 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ReachForWorldBehaviourInterfaceFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ReachForWorldBehaviourInterfaceFeedbackOutputs));
  }

  enum { maxDistanceToTarget = 2 };
  NM_INLINE unsigned int getMaxDistanceToTarget() const { return maxDistanceToTarget; }
  NM_INLINE void setDistanceToTarget(unsigned int number, const float* _distanceToTarget, float distanceToTarget_importance = 1.f)
  {
    NMP_ASSERT(number <= maxDistanceToTarget);
    NMP_ASSERT(distanceToTarget_importance >= 0.0f && distanceToTarget_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      distanceToTarget[i] = _distanceToTarget[i];
      postAssignDistanceToTarget(distanceToTarget[i]);
      m_distanceToTargetImportance[i] = distanceToTarget_importance;
    }
  }
  NM_INLINE void setDistanceToTargetAt(unsigned int index, const float& _distanceToTarget, float distanceToTarget_importance = 1.f)
  {
    NMP_ASSERT(index < maxDistanceToTarget);
    distanceToTarget[index] = _distanceToTarget;
    NMP_ASSERT(distanceToTarget_importance >= 0.0f && distanceToTarget_importance <= 1.0f);
    postAssignDistanceToTarget(distanceToTarget[index]);
    m_distanceToTargetImportance[index] = distanceToTarget_importance;
  }
  NM_INLINE float getDistanceToTargetImportance(int index) const { return m_distanceToTargetImportance[index]; }
  NM_INLINE const float& getDistanceToTargetImportanceRef(int index) const { return m_distanceToTargetImportance[index]; }
  NM_INLINE unsigned int calculateNumDistanceToTarget() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_distanceToTargetImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getDistanceToTarget(unsigned int index) const { NMP_ASSERT(index <= maxDistanceToTarget); return distanceToTarget[index]; }

protected:

  float distanceToTarget[NetworkConstants::networkMaxNumArms];  ///< (Distance)

  // post-assignment stubs
  NM_INLINE void postAssignDistanceToTarget(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "distanceToTarget");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_distanceToTargetImportance[NetworkConstants::networkMaxNumArms];

  friend class ReachForWorldBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numDistanceToTarget = calculateNumDistanceToTarget();
      for (uint32_t i=0; i<numDistanceToTarget; i++)
      {
        if (getDistanceToTargetImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(distanceToTarget[i]), "distanceToTarget");
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

#endif // NM_MDF_REACHFORWORLDBEHAVIOURINTERFACE_DATA_H

