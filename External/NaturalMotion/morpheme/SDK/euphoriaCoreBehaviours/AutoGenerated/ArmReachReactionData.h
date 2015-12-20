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

#ifndef NM_MDF_ARMREACHREACTION_DATA_H
#define NM_MDF_ARMREACHREACTION_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmReachReaction.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"

// known types
#include "Types/ReachTarget.h"
#include "Types/LimbControl.h"

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
 * \class ArmReachReactionInputs
 * \ingroup ArmReachReaction
 * \brief Inputs looks at the specified target
 *
 * Data packet definition (200 bytes, 224 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmReachReactionInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmReachReactionInputs));
  }

  NM_INLINE void setArmHitPoseEndRelRoot(const NMP::Matrix34& _armHitPoseEndRelRoot, float armHitPoseEndRelRoot_importance = 1.f)
  {
    armHitPoseEndRelRoot = _armHitPoseEndRelRoot;
    postAssignArmHitPoseEndRelRoot(armHitPoseEndRelRoot);
    NMP_ASSERT(armHitPoseEndRelRoot_importance >= 0.0f && armHitPoseEndRelRoot_importance <= 1.0f);
    m_armHitPoseEndRelRootImportance = armHitPoseEndRelRoot_importance;
  }
  NM_INLINE float getArmHitPoseEndRelRootImportance() const { return m_armHitPoseEndRelRootImportance; }
  NM_INLINE const float& getArmHitPoseEndRelRootImportanceRef() const { return m_armHitPoseEndRelRootImportance; }
  NM_INLINE const NMP::Matrix34& getArmHitPoseEndRelRoot() const { return armHitPoseEndRelRoot; }

  enum { maxCurrentReachTargets = 2 };
  NM_INLINE unsigned int getMaxCurrentReachTargets() const { return maxCurrentReachTargets; }
  NM_INLINE void setCurrentReachTargets(unsigned int number, const ReachTarget* _currentReachTargets, float currentReachTargets_importance = 1.f)
  {
    NMP_ASSERT(number <= maxCurrentReachTargets);
    NMP_ASSERT(currentReachTargets_importance >= 0.0f && currentReachTargets_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      currentReachTargets[i] = _currentReachTargets[i];
      postAssignCurrentReachTargets(currentReachTargets[i]);
      m_currentReachTargetsImportance[i] = currentReachTargets_importance;
    }
  }
  NM_INLINE void setCurrentReachTargetsAt(unsigned int index, const ReachTarget& _currentReachTargets, float currentReachTargets_importance = 1.f)
  {
    NMP_ASSERT(index < maxCurrentReachTargets);
    currentReachTargets[index] = _currentReachTargets;
    NMP_ASSERT(currentReachTargets_importance >= 0.0f && currentReachTargets_importance <= 1.0f);
    postAssignCurrentReachTargets(currentReachTargets[index]);
    m_currentReachTargetsImportance[index] = currentReachTargets_importance;
  }
  NM_INLINE float getCurrentReachTargetsImportance(int index) const { return m_currentReachTargetsImportance[index]; }
  NM_INLINE const float& getCurrentReachTargetsImportanceRef(int index) const { return m_currentReachTargetsImportance[index]; }
  NM_INLINE unsigned int calculateNumCurrentReachTargets() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_currentReachTargetsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const ReachTarget& getCurrentReachTargets(unsigned int index) const { NMP_ASSERT(index <= maxCurrentReachTargets); return currentReachTargets[index]; }

  NM_INLINE void setAvgReachingHandPos(const NMP::Vector3& _avgReachingHandPos, float avgReachingHandPos_importance = 1.f)
  {
    avgReachingHandPos = _avgReachingHandPos;
    postAssignAvgReachingHandPos(avgReachingHandPos);
    NMP_ASSERT(avgReachingHandPos_importance >= 0.0f && avgReachingHandPos_importance <= 1.0f);
    m_avgReachingHandPosImportance = avgReachingHandPos_importance;
  }
  NM_INLINE float getAvgReachingHandPosImportance() const { return m_avgReachingHandPosImportance; }
  NM_INLINE const float& getAvgReachingHandPosImportanceRef() const { return m_avgReachingHandPosImportance; }
  NM_INLINE const NMP::Vector3& getAvgReachingHandPos() const { return avgReachingHandPos; }

  NM_INLINE void setSupportAmount(const float& _supportAmount, float supportAmount_importance = 1.f)
  {
    supportAmount = _supportAmount;
    postAssignSupportAmount(supportAmount);
    NMP_ASSERT(supportAmount_importance >= 0.0f && supportAmount_importance <= 1.0f);
    m_supportAmountImportance = supportAmount_importance;
  }
  NM_INLINE float getSupportAmountImportance() const { return m_supportAmountImportance; }
  NM_INLINE const float& getSupportAmountImportanceRef() const { return m_supportAmountImportance; }
  NM_INLINE const float& getSupportAmount() const { return supportAmount; }

protected:

  NMP::Matrix34 armHitPoseEndRelRoot;        ///< (Transform)
  ReachTarget currentReachTargets[NetworkConstants::networkMaxNumArms];
  NMP::Vector3 avgReachingHandPos;           ///< (Position)
  float supportAmount;                       ///< (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignArmHitPoseEndRelRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "armHitPoseEndRelRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignCurrentReachTargets(const ReachTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAvgReachingHandPos(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "avgReachingHandPos");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSupportAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "supportAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_armHitPoseEndRelRootImportance, 
     m_currentReachTargetsImportance[NetworkConstants::networkMaxNumArms], 
     m_avgReachingHandPosImportance, 
     m_supportAmountImportance;

  friend class ArmReachReaction_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getArmHitPoseEndRelRootImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(armHitPoseEndRelRoot), "armHitPoseEndRelRoot");
    }
    {
      uint32_t numCurrentReachTargets = calculateNumCurrentReachTargets();
      for (uint32_t i=0; i<numCurrentReachTargets; i++)
      {
        if (getCurrentReachTargetsImportance(i) > 0.0f)
          currentReachTargets[i].validate();
      }
    }
    if (getAvgReachingHandPosImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(avgReachingHandPos), "avgReachingHandPos");
    }
    if (getSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(supportAmount), "supportAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmReachReactionOutputs
 * \ingroup ArmReachReaction
 * \brief Outputs looks at the specified target
 *
 * Data packet definition (396 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmReachReactionOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmReachReactionOutputs));
  }

  NM_INLINE void setControl(const LimbControl& _control, float control_importance = 1.f)
  {
    control = _control;
    postAssignControl(control);
    NMP_ASSERT(control_importance >= 0.0f && control_importance <= 1.0f);
    m_controlImportance = control_importance;
  }
  NM_INLINE LimbControl& startControlModification()
  {
    m_controlImportance = -1.0f; // set invalid until stopControlModification()
    return control;
  }
  NM_INLINE void stopControlModification(float control_importance = 1.f)
  {
    postAssignControl(control);
    NMP_ASSERT(control_importance >= 0.0f && control_importance <= 1.0f);
    m_controlImportance = control_importance;
  }
  NM_INLINE float getControlImportance() const { return m_controlImportance; }
  NM_INLINE const float& getControlImportanceRef() const { return m_controlImportance; }
  NM_INLINE const LimbControl& getControl() const { return control; }

  NM_INLINE void setDistanceToTarget(const float& _distanceToTarget, float distanceToTarget_importance = 1.f)
  {
    distanceToTarget = _distanceToTarget;
    postAssignDistanceToTarget(distanceToTarget);
    NMP_ASSERT(distanceToTarget_importance >= 0.0f && distanceToTarget_importance <= 1.0f);
    m_distanceToTargetImportance = distanceToTarget_importance;
  }
  NM_INLINE float getDistanceToTargetImportance() const { return m_distanceToTargetImportance; }
  NM_INLINE const float& getDistanceToTargetImportanceRef() const { return m_distanceToTargetImportance; }
  NM_INLINE const float& getDistanceToTarget() const { return distanceToTarget; }

protected:

  LimbControl control;
  float distanceToTarget;

  // post-assignment stubs
  NM_INLINE void postAssignControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDistanceToTarget(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "distanceToTarget");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_controlImportance, 
     m_distanceToTargetImportance;

  friend class ArmReachReaction_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getControlImportance() > 0.0f)
    {
      control.validate();
    }
    if (getDistanceToTargetImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(distanceToTarget), "distanceToTarget");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMREACHREACTION_DATA_H

