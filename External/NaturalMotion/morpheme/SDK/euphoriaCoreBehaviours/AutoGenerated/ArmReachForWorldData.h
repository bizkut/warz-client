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

#ifndef NM_MDF_ARMREACHFORWORLD_DATA_H
#define NM_MDF_ARMREACHFORWORLD_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmReachForWorld.module"

// external types
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/LimbControl.h"
#include "Types/ReachWorldParameters.h"
#include "Types/DirectionRequest.h"
#include "Types/TranslationRequest.h"
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
 * \class ArmReachForWorldData
 * \ingroup ArmReachForWorld
 * \brief Data reach to a target (point + normal)
 *
 * Data packet definition (12 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmReachForWorldData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmReachForWorldData));
  }

  float effectorToTargetAngle;
  float reachImportance;  ///< A measure of how much apparent effort should be made to reach the current target.  (Importance)
  float maxReach;  ///< This is the arm length scaled by the user-specified maximum reach scale parameter.  (Length)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(effectorToTargetAngle), "effectorToTargetAngle");
    NM_VALIDATOR(Float0to1(reachImportance), "reachImportance");
    NM_VALIDATOR(FloatNonNegative(maxReach), "maxReach");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmReachForWorldInputs
 * \ingroup ArmReachForWorld
 * \brief Inputs reach to a target (point + normal)
 *
 * Data packet definition (548 bytes, 576 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmReachForWorldInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmReachForWorldInputs));
  }

  NM_INLINE void setLimbControl(const LimbControl& _limbControl, float limbControl_importance = 1.f)
  {
    limbControl = _limbControl;
    postAssignLimbControl(limbControl);
    NMP_ASSERT(limbControl_importance >= 0.0f && limbControl_importance <= 1.0f);
    m_limbControlImportance = limbControl_importance;
  }
  NM_INLINE LimbControl& startLimbControlModification()
  {
    m_limbControlImportance = -1.0f; // set invalid until stopLimbControlModification()
    return limbControl;
  }
  NM_INLINE void stopLimbControlModification(float limbControl_importance = 1.f)
  {
    postAssignLimbControl(limbControl);
    NMP_ASSERT(limbControl_importance >= 0.0f && limbControl_importance <= 1.0f);
    m_limbControlImportance = limbControl_importance;
  }
  NM_INLINE float getLimbControlImportance() const { return m_limbControlImportance; }
  NM_INLINE const float& getLimbControlImportanceRef() const { return m_limbControlImportance; }
  NM_INLINE const LimbControl& getLimbControl() const { return limbControl; }

  NM_INLINE void setReachParams(const ReachWorldParameters& _reachParams, float reachParams_importance = 1.f)
  {
    reachParams = _reachParams;
    postAssignReachParams(reachParams);
    NMP_ASSERT(reachParams_importance >= 0.0f && reachParams_importance <= 1.0f);
    m_reachParamsImportance = reachParams_importance;
  }
  NM_INLINE ReachWorldParameters& startReachParamsModification()
  {
    m_reachParamsImportance = -1.0f; // set invalid until stopReachParamsModification()
    return reachParams;
  }
  NM_INLINE void stopReachParamsModification(float reachParams_importance = 1.f)
  {
    postAssignReachParams(reachParams);
    NMP_ASSERT(reachParams_importance >= 0.0f && reachParams_importance <= 1.0f);
    m_reachParamsImportance = reachParams_importance;
  }
  NM_INLINE float getReachParamsImportance() const { return m_reachParamsImportance; }
  NM_INLINE const float& getReachParamsImportanceRef() const { return m_reachParamsImportance; }
  NM_INLINE const ReachWorldParameters& getReachParams() const { return reachParams; }

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

  LimbControl limbControl;
  ReachWorldParameters reachParams;
  ER::LimbTransform supportTM;  ///< in support this is considered the "fixed root" of the system
  float supportAmount;                       ///< (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignLimbControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
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

  NM_INLINE void postAssignSupportAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "supportAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_limbControlImportance, 
     m_reachParamsImportance, 
     m_supportTMImportance, 
     m_supportAmountImportance;

  friend class ArmReachForWorld_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getLimbControlImportance() > 0.0f)
    {
      limbControl.validate();
    }
    if (getReachParamsImportance() > 0.0f)
    {
      reachParams.validate();
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
 * \class ArmReachForWorldOutputs
 * \ingroup ArmReachForWorld
 * \brief Outputs reach to a target (point + normal)
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmReachForWorldOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmReachForWorldOutputs));
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

  friend class ArmReachForWorld_Con;

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
 * \class ArmReachForWorldFeedbackOutputs
 * \ingroup ArmReachForWorld
 * \brief FeedbackOutputs reach to a target (point + normal)
 *
 * Data packet definition (1024 bytes, 1056 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmReachForWorldFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmReachForWorldFeedbackOutputs));
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

  NM_INLINE void setPelvisRotationRequest(const RotationRequest& _pelvisRotationRequest, float pelvisRotationRequest_importance = 1.f)
  {
    pelvisRotationRequest = _pelvisRotationRequest;
    postAssignPelvisRotationRequest(pelvisRotationRequest);
    NMP_ASSERT(pelvisRotationRequest_importance >= 0.0f && pelvisRotationRequest_importance <= 1.0f);
    m_pelvisRotationRequestImportance = pelvisRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startPelvisRotationRequestModification()
  {
    m_pelvisRotationRequestImportance = -1.0f; // set invalid until stopPelvisRotationRequestModification()
    return pelvisRotationRequest;
  }
  NM_INLINE void stopPelvisRotationRequestModification(float pelvisRotationRequest_importance = 1.f)
  {
    postAssignPelvisRotationRequest(pelvisRotationRequest);
    NMP_ASSERT(pelvisRotationRequest_importance >= 0.0f && pelvisRotationRequest_importance <= 1.0f);
    m_pelvisRotationRequestImportance = pelvisRotationRequest_importance;
  }
  NM_INLINE float getPelvisRotationRequestImportance() const { return m_pelvisRotationRequestImportance; }
  NM_INLINE const float& getPelvisRotationRequestImportanceRef() const { return m_pelvisRotationRequestImportance; }
  NM_INLINE const RotationRequest& getPelvisRotationRequest() const { return pelvisRotationRequest; }

  NM_INLINE void setRootRotationRequest(const RotationRequest& _rootRotationRequest, float rootRotationRequest_importance = 1.f)
  {
    rootRotationRequest = _rootRotationRequest;
    postAssignRootRotationRequest(rootRotationRequest);
    NMP_ASSERT(rootRotationRequest_importance >= 0.0f && rootRotationRequest_importance <= 1.0f);
    m_rootRotationRequestImportance = rootRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startRootRotationRequestModification()
  {
    m_rootRotationRequestImportance = -1.0f; // set invalid until stopRootRotationRequestModification()
    return rootRotationRequest;
  }
  NM_INLINE void stopRootRotationRequestModification(float rootRotationRequest_importance = 1.f)
  {
    postAssignRootRotationRequest(rootRotationRequest);
    NMP_ASSERT(rootRotationRequest_importance >= 0.0f && rootRotationRequest_importance <= 1.0f);
    m_rootRotationRequestImportance = rootRotationRequest_importance;
  }
  NM_INLINE float getRootRotationRequestImportance() const { return m_rootRotationRequestImportance; }
  NM_INLINE const float& getRootRotationRequestImportanceRef() const { return m_rootRotationRequestImportance; }
  NM_INLINE const RotationRequest& getRootRotationRequest() const { return rootRotationRequest; }

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

  DirectionRequest pelvisDirectionRequest;
  DirectionRequest chestDirectionRequest;  ///< Will be sent when he is supported
  TranslationRequest pelvisTranslationRequest;
  RotationRequest pelvisRotationRequest;
  RotationRequest rootRotationRequest;  ///< Will be sent when he is not supported
  TranslationRequest chestTranslationRequest;
  float distanceToTarget;                    ///< (Distance)

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

  NM_INLINE void postAssignPelvisTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPelvisRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignRootRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
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

  NM_INLINE void postAssignDistanceToTarget(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "distanceToTarget");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_pelvisDirectionRequestImportance, 
     m_chestDirectionRequestImportance, 
     m_pelvisTranslationRequestImportance, 
     m_pelvisRotationRequestImportance, 
     m_rootRotationRequestImportance, 
     m_chestTranslationRequestImportance, 
     m_distanceToTargetImportance;

  friend class ArmReachForWorld_Con;

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
    if (getPelvisTranslationRequestImportance() > 0.0f)
    {
      pelvisTranslationRequest.validate();
    }
    if (getPelvisRotationRequestImportance() > 0.0f)
    {
      pelvisRotationRequest.validate();
    }
    if (getRootRotationRequestImportance() > 0.0f)
    {
      rootRotationRequest.validate();
    }
    if (getChestTranslationRequestImportance() > 0.0f)
    {
      chestTranslationRequest.validate();
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

#endif // NM_MDF_ARMREACHFORWORLD_DATA_H

