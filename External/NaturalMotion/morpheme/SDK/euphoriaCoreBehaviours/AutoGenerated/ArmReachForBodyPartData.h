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

#ifndef NM_MDF_ARMREACHFORBODYPART_DATA_H
#define NM_MDF_ARMREACHFORBODYPART_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmReachForBodyPart.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/ReachTarget.h"
#include "Types/ReachActionParams.h"
#include "Types/LimbControl.h"
#include "Types/RotationRequest.h"
#include "Types/DirectionRequest.h"
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
 * \class ArmReachForBodyPartData
 * \ingroup ArmReachForBodyPart
 * \brief Data reach to a target (point + normal)
 *
 * Data packet definition (196 bytes, 224 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmReachForBodyPartData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmReachForBodyPartData));
  }

  enum State
  {
    /*  0 */ State_Waiting,  
    /*  1 */ State_Reaching,  
    /*  2 */ State_Holding,  
    /*  3 */ State_FailedRampdown,  
    /*  4 */ State_CompletedRampdown,  
    /*  5 */ State_Done,  
    /*  6 */ State_numStates,  
  };

  ReachTarget target;
  ReachTarget referenceTargetRelSupport;
  NMP::Vector3 cachedTargetNormalLocal;  ///< target reach normal stored in chest local frame  (Direction)
  ReachActionParams reachActionParams;  // Initial target position relative and  wrt the support tm, only referenced when in support.
  float targetImportance;                    ///< (Importance)
  float supportAmount;                       ///< (Weight)
  float elapsedTime;                         ///< (TimePeriod)
  float elapsedTimeToTarget;                 ///< (TimePeriod)
  float elapsedTimeOnTarget;                 ///< (TimePeriod)
  uint32_t state;
  float armStrengthReduction;                ///< (Weight)
  float maxArmExtension;                     ///< (Length)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    target.validate();
    referenceTargetRelSupport.validate();
    NM_VALIDATOR(Vector3Normalised(cachedTargetNormalLocal), "cachedTargetNormalLocal");
    reachActionParams.validate();
    NM_VALIDATOR(Float0to1(targetImportance), "targetImportance");
    NM_VALIDATOR(FloatNonNegative(supportAmount), "supportAmount");
    NM_VALIDATOR(FloatValid(elapsedTime), "elapsedTime");
    NM_VALIDATOR(FloatValid(elapsedTimeToTarget), "elapsedTimeToTarget");
    NM_VALIDATOR(FloatValid(elapsedTimeOnTarget), "elapsedTimeOnTarget");
    NM_VALIDATOR(FloatNonNegative(armStrengthReduction), "armStrengthReduction");
    NM_VALIDATOR(FloatNonNegative(maxArmExtension), "maxArmExtension");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmReachForBodyPartInputs
 * \ingroup ArmReachForBodyPart
 * \brief Inputs reach to a target (point + normal)
 *
 * Data packet definition (194 bytes, 224 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmReachForBodyPartInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmReachForBodyPartInputs));
  }

  NM_INLINE void setSupportTM(const ER::LimbTransform& _supportTM, float supportTM_importance = 1.f)
  {
    supportTM = _supportTM;
    NMP_ASSERT(supportTM_importance >= 0.0f && supportTM_importance <= 1.0f);
    m_supportTMImportance = supportTM_importance;
  }
  NM_INLINE ER::LimbTransform& startSupportTMModification()
  {
    m_supportTMImportance = -1.0f; // set invalid until stopSupportTMModification()
    return supportTM;
  }
  NM_INLINE void stopSupportTMModification(float supportTM_importance = 1.f)
  {
    NMP_ASSERT(supportTM_importance >= 0.0f && supportTM_importance <= 1.0f);
    m_supportTMImportance = supportTM_importance;
  }
  NM_INLINE float getSupportTMImportance() const { return m_supportTMImportance; }
  NM_INLINE const float& getSupportTMImportanceRef() const { return m_supportTMImportance; }
  NM_INLINE const ER::LimbTransform& getSupportTM() const { return supportTM; }

  NM_INLINE void setTarget(const ReachTarget& _target, float target_importance = 1.f)
  {
    target = _target;
    postAssignTarget(target);
    NMP_ASSERT(target_importance >= 0.0f && target_importance <= 1.0f);
    m_targetImportance = target_importance;
  }
  NM_INLINE float getTargetImportance() const { return m_targetImportance; }
  NM_INLINE const float& getTargetImportanceRef() const { return m_targetImportance; }
  NM_INLINE const ReachTarget& getTarget() const { return target; }

  NM_INLINE void setReachActionParams(const ReachActionParams& _reachActionParams, float reachActionParams_importance = 1.f)
  {
    reachActionParams = _reachActionParams;
    postAssignReachActionParams(reachActionParams);
    NMP_ASSERT(reachActionParams_importance >= 0.0f && reachActionParams_importance <= 1.0f);
    m_reachActionParamsImportance = reachActionParams_importance;
  }
  NM_INLINE float getReachActionParamsImportance() const { return m_reachActionParamsImportance; }
  NM_INLINE const float& getReachActionParamsImportanceRef() const { return m_reachActionParamsImportance; }
  NM_INLINE const ReachActionParams& getReachActionParams() const { return reachActionParams; }

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

  NM_INLINE void setBeenHit(const bool& _beenHit, float beenHit_importance = 1.f)
  {
    beenHit = _beenHit;
    NMP_ASSERT(beenHit_importance >= 0.0f && beenHit_importance <= 1.0f);
    m_beenHitImportance = beenHit_importance;
  }
  NM_INLINE float getBeenHitImportance() const { return m_beenHitImportance; }
  NM_INLINE const float& getBeenHitImportanceRef() const { return m_beenHitImportance; }
  NM_INLINE const bool& getBeenHit() const { return beenHit; }

  NM_INLINE void setReachEnabled(const bool& _reachEnabled, float reachEnabled_importance = 1.f)
  {
    reachEnabled = _reachEnabled;
    NMP_ASSERT(reachEnabled_importance >= 0.0f && reachEnabled_importance <= 1.0f);
    m_reachEnabledImportance = reachEnabled_importance;
  }
  NM_INLINE float getReachEnabledImportance() const { return m_reachEnabledImportance; }
  NM_INLINE const float& getReachEnabledImportanceRef() const { return m_reachEnabledImportance; }
  NM_INLINE const bool& getReachEnabled() const { return reachEnabled; }

protected:

  ER::LimbTransform supportTM;  ///< in support this is considered the "fixed root" of the system
  ReachTarget target;
  ReachActionParams reachActionParams;
  float supportAmount;  /// If the body as a whole is supported (includes standing or stepping)  (Weight)
  bool beenHit;
  bool reachEnabled;

  // post-assignment stubs
  NM_INLINE void postAssignTarget(const ReachTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignReachActionParams(const ReachActionParams& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
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
     m_supportTMImportance, 
     m_targetImportance, 
     m_reachActionParamsImportance, 
     m_supportAmountImportance, 
     m_beenHitImportance, 
     m_reachEnabledImportance;

  friend class ArmReachForBodyPart_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTargetImportance() > 0.0f)
    {
      target.validate();
    }
    if (getReachActionParamsImportance() > 0.0f)
    {
      reachActionParams.validate();
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
 * \class ArmReachForBodyPartOutputs
 * \ingroup ArmReachForBodyPart
 * \brief Outputs reach to a target (point + normal)
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmReachForBodyPartOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmReachForBodyPartOutputs));
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

protected:

  LimbControl control;

  // post-assignment stubs
  NM_INLINE void postAssignControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_controlImportance;

  friend class ArmReachForBodyPart_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getControlImportance() > 0.0f)
    {
      control.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmReachForBodyPartFeedbackInputs
 * \ingroup ArmReachForBodyPart
 * \brief FeedbackInputs reach to a target (point + normal)
 *
 * Data packet definition (8 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmReachForBodyPartFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmReachForBodyPartFeedbackInputs));
  }

  NM_INLINE void setArmReachReactionDistanceFromTarget(const float& _armReachReactionDistanceFromTarget, float armReachReactionDistanceFromTarget_importance = 1.f)
  {
    armReachReactionDistanceFromTarget = _armReachReactionDistanceFromTarget;
    postAssignArmReachReactionDistanceFromTarget(armReachReactionDistanceFromTarget);
    NMP_ASSERT(armReachReactionDistanceFromTarget_importance >= 0.0f && armReachReactionDistanceFromTarget_importance <= 1.0f);
    m_armReachReactionDistanceFromTargetImportance = armReachReactionDistanceFromTarget_importance;
  }
  NM_INLINE float getArmReachReactionDistanceFromTargetImportance() const { return m_armReachReactionDistanceFromTargetImportance; }
  NM_INLINE const float& getArmReachReactionDistanceFromTargetImportanceRef() const { return m_armReachReactionDistanceFromTargetImportance; }
  NM_INLINE const float& getArmReachReactionDistanceFromTarget() const { return armReachReactionDistanceFromTarget; }

protected:

  float armReachReactionDistanceFromTarget;  // data used to locate the body and compute full body reach responses

  // post-assignment stubs
  NM_INLINE void postAssignArmReachReactionDistanceFromTarget(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "armReachReactionDistanceFromTarget");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_armReachReactionDistanceFromTargetImportance;

  friend class ArmReachForBodyPart_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getArmReachReactionDistanceFromTargetImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(armReachReactionDistanceFromTarget), "armReachReactionDistanceFromTarget");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmReachForBodyPartFeedbackOutputs
 * \ingroup ArmReachForBodyPart
 * \brief FeedbackOutputs reach to a target (point + normal)
 *
 * Data packet definition (928 bytes, 960 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmReachForBodyPartFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmReachForBodyPartFeedbackOutputs));
  }

  NM_INLINE void setPelvisDirectionRequest(const DirectionRequest& _pelvisDirectionRequest, float pelvisDirectionRequest_importance = 1.f)
  {
    pelvisDirectionRequest = _pelvisDirectionRequest;
    postAssignPelvisDirectionRequest(pelvisDirectionRequest);
    NMP_ASSERT(pelvisDirectionRequest_importance >= 0.0f && pelvisDirectionRequest_importance <= 1.0f);
    m_pelvisDirectionRequestImportance = pelvisDirectionRequest_importance;
  }
  NM_INLINE DirectionRequest& startPelvisDirectionRequestModification()
  {
    m_pelvisDirectionRequestImportance = -1.0f; // set invalid until stopPelvisDirectionRequestModification()
    return pelvisDirectionRequest;
  }
  NM_INLINE void stopPelvisDirectionRequestModification(float pelvisDirectionRequest_importance = 1.f)
  {
    postAssignPelvisDirectionRequest(pelvisDirectionRequest);
    NMP_ASSERT(pelvisDirectionRequest_importance >= 0.0f && pelvisDirectionRequest_importance <= 1.0f);
    m_pelvisDirectionRequestImportance = pelvisDirectionRequest_importance;
  }
  NM_INLINE float getPelvisDirectionRequestImportance() const { return m_pelvisDirectionRequestImportance; }
  NM_INLINE const float& getPelvisDirectionRequestImportanceRef() const { return m_pelvisDirectionRequestImportance; }
  NM_INLINE const DirectionRequest& getPelvisDirectionRequest() const { return pelvisDirectionRequest; }

  NM_INLINE void setChestDirectionRequest(const DirectionRequest& _chestDirectionRequest, float chestDirectionRequest_importance = 1.f)
  {
    chestDirectionRequest = _chestDirectionRequest;
    postAssignChestDirectionRequest(chestDirectionRequest);
    NMP_ASSERT(chestDirectionRequest_importance >= 0.0f && chestDirectionRequest_importance <= 1.0f);
    m_chestDirectionRequestImportance = chestDirectionRequest_importance;
  }
  NM_INLINE DirectionRequest& startChestDirectionRequestModification()
  {
    m_chestDirectionRequestImportance = -1.0f; // set invalid until stopChestDirectionRequestModification()
    return chestDirectionRequest;
  }
  NM_INLINE void stopChestDirectionRequestModification(float chestDirectionRequest_importance = 1.f)
  {
    postAssignChestDirectionRequest(chestDirectionRequest);
    NMP_ASSERT(chestDirectionRequest_importance >= 0.0f && chestDirectionRequest_importance <= 1.0f);
    m_chestDirectionRequestImportance = chestDirectionRequest_importance;
  }
  NM_INLINE float getChestDirectionRequestImportance() const { return m_chestDirectionRequestImportance; }
  NM_INLINE const float& getChestDirectionRequestImportanceRef() const { return m_chestDirectionRequestImportance; }
  NM_INLINE const DirectionRequest& getChestDirectionRequest() const { return chestDirectionRequest; }

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

  NM_INLINE void setPelvisTranslationRequest(const TranslationRequest& _pelvisTranslationRequest, float pelvisTranslationRequest_importance = 1.f)
  {
    pelvisTranslationRequest = _pelvisTranslationRequest;
    postAssignPelvisTranslationRequest(pelvisTranslationRequest);
    NMP_ASSERT(pelvisTranslationRequest_importance >= 0.0f && pelvisTranslationRequest_importance <= 1.0f);
    m_pelvisTranslationRequestImportance = pelvisTranslationRequest_importance;
  }
  NM_INLINE TranslationRequest& startPelvisTranslationRequestModification()
  {
    m_pelvisTranslationRequestImportance = -1.0f; // set invalid until stopPelvisTranslationRequestModification()
    return pelvisTranslationRequest;
  }
  NM_INLINE void stopPelvisTranslationRequestModification(float pelvisTranslationRequest_importance = 1.f)
  {
    postAssignPelvisTranslationRequest(pelvisTranslationRequest);
    NMP_ASSERT(pelvisTranslationRequest_importance >= 0.0f && pelvisTranslationRequest_importance <= 1.0f);
    m_pelvisTranslationRequestImportance = pelvisTranslationRequest_importance;
  }
  NM_INLINE float getPelvisTranslationRequestImportance() const { return m_pelvisTranslationRequestImportance; }
  NM_INLINE const float& getPelvisTranslationRequestImportanceRef() const { return m_pelvisTranslationRequestImportance; }
  NM_INLINE const TranslationRequest& getPelvisTranslationRequest() const { return pelvisTranslationRequest; }

  NM_INLINE void setChestTranslationRequest(const TranslationRequest& _chestTranslationRequest, float chestTranslationRequest_importance = 1.f)
  {
    chestTranslationRequest = _chestTranslationRequest;
    postAssignChestTranslationRequest(chestTranslationRequest);
    NMP_ASSERT(chestTranslationRequest_importance >= 0.0f && chestTranslationRequest_importance <= 1.0f);
    m_chestTranslationRequestImportance = chestTranslationRequest_importance;
  }
  NM_INLINE TranslationRequest& startChestTranslationRequestModification()
  {
    m_chestTranslationRequestImportance = -1.0f; // set invalid until stopChestTranslationRequestModification()
    return chestTranslationRequest;
  }
  NM_INLINE void stopChestTranslationRequestModification(float chestTranslationRequest_importance = 1.f)
  {
    postAssignChestTranslationRequest(chestTranslationRequest);
    NMP_ASSERT(chestTranslationRequest_importance >= 0.0f && chestTranslationRequest_importance <= 1.0f);
    m_chestTranslationRequestImportance = chestTranslationRequest_importance;
  }
  NM_INLINE float getChestTranslationRequestImportance() const { return m_chestTranslationRequestImportance; }
  NM_INLINE const float& getChestTranslationRequestImportanceRef() const { return m_chestTranslationRequestImportance; }
  NM_INLINE const TranslationRequest& getChestTranslationRequest() const { return chestTranslationRequest; }

  NM_INLINE void setCurrentReachTarget(const ReachTarget& _currentReachTarget, float currentReachTarget_importance = 1.f)
  {
    currentReachTarget = _currentReachTarget;
    postAssignCurrentReachTarget(currentReachTarget);
    NMP_ASSERT(currentReachTarget_importance >= 0.0f && currentReachTarget_importance <= 1.0f);
    m_currentReachTargetImportance = currentReachTarget_importance;
  }
  NM_INLINE float getCurrentReachTargetImportance() const { return m_currentReachTargetImportance; }
  NM_INLINE const float& getCurrentReachTargetImportanceRef() const { return m_currentReachTargetImportance; }
  NM_INLINE const ReachTarget& getCurrentReachTarget() const { return currentReachTarget; }

  NM_INLINE void setDistanceSquaredToTarget(const float& _distanceSquaredToTarget, float distanceSquaredToTarget_importance = 1.f)
  {
    distanceSquaredToTarget = _distanceSquaredToTarget;
    postAssignDistanceSquaredToTarget(distanceSquaredToTarget);
    NMP_ASSERT(distanceSquaredToTarget_importance >= 0.0f && distanceSquaredToTarget_importance <= 1.0f);
    m_distanceSquaredToTargetImportance = distanceSquaredToTarget_importance;
  }
  NM_INLINE float getDistanceSquaredToTargetImportance() const { return m_distanceSquaredToTargetImportance; }
  NM_INLINE const float& getDistanceSquaredToTargetImportanceRef() const { return m_distanceSquaredToTargetImportance; }
  NM_INLINE const float& getDistanceSquaredToTarget() const { return distanceSquaredToTarget; }

  NM_INLINE void setArmStrengthReduction(const float& _armStrengthReduction, float armStrengthReduction_importance = 1.f)
  {
    armStrengthReduction = _armStrengthReduction;
    postAssignArmStrengthReduction(armStrengthReduction);
    NMP_ASSERT(armStrengthReduction_importance >= 0.0f && armStrengthReduction_importance <= 1.0f);
    m_armStrengthReductionImportance = armStrengthReduction_importance;
  }
  NM_INLINE float getArmStrengthReductionImportance() const { return m_armStrengthReductionImportance; }
  NM_INLINE const float& getArmStrengthReductionImportanceRef() const { return m_armStrengthReductionImportance; }
  NM_INLINE const float& getArmStrengthReduction() const { return armStrengthReduction; }

  NM_INLINE void setState(const uint32_t& _state, float state_importance = 1.f)
  {
    state = _state;
    NMP_ASSERT(state_importance >= 0.0f && state_importance <= 1.0f);
    m_stateImportance = state_importance;
  }
  NM_INLINE float getStateImportance() const { return m_stateImportance; }
  NM_INLINE const float& getStateImportanceRef() const { return m_stateImportance; }
  NM_INLINE const uint32_t& getState() const { return state; }

protected:

  DirectionRequest pelvisDirectionRequest;
  DirectionRequest chestDirectionRequest;
  RotationRequest rotationRequest;
  TranslationRequest pelvisTranslationRequest;
  TranslationRequest chestTranslationRequest;
  ReachTarget currentReachTarget;  ///< broadcast if arm has a current reach target
  float distanceSquaredToTarget;
  float armStrengthReduction;                ///< (Weight)
  uint32_t state;

  // post-assignment stubs
  NM_INLINE void postAssignPelvisDirectionRequest(const DirectionRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignChestDirectionRequest(const DirectionRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPelvisTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignChestTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignCurrentReachTarget(const ReachTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDistanceSquaredToTarget(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "distanceSquaredToTarget");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmStrengthReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "armStrengthReduction");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_pelvisDirectionRequestImportance, 
     m_chestDirectionRequestImportance, 
     m_rotationRequestImportance, 
     m_pelvisTranslationRequestImportance, 
     m_chestTranslationRequestImportance, 
     m_currentReachTargetImportance, 
     m_distanceSquaredToTargetImportance, 
     m_armStrengthReductionImportance, 
     m_stateImportance;

  friend class ArmReachForBodyPart_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getPelvisDirectionRequestImportance() > 0.0f)
    {
      pelvisDirectionRequest.validate();
    }
    if (getChestDirectionRequestImportance() > 0.0f)
    {
      chestDirectionRequest.validate();
    }
    if (getRotationRequestImportance() > 0.0f)
    {
      rotationRequest.validate();
    }
    if (getPelvisTranslationRequestImportance() > 0.0f)
    {
      pelvisTranslationRequest.validate();
    }
    if (getChestTranslationRequestImportance() > 0.0f)
    {
      chestTranslationRequest.validate();
    }
    if (getCurrentReachTargetImportance() > 0.0f)
    {
      currentReachTarget.validate();
    }
    if (getDistanceSquaredToTargetImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(distanceSquaredToTarget), "distanceSquaredToTarget");
    }
    if (getArmStrengthReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(armStrengthReduction), "armStrengthReduction");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMREACHFORBODYPART_DATA_H

