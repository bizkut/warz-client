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

#ifndef NM_MDF_SPINE_DATA_H
#define NM_MDF_SPINE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/Spine.module"

// external types
#include "euphoria/erDimensionalScaling.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/ProcessRequest.h"
#include "Types/RotationRequest.h"
#include "Types/TranslationRequest.h"
#include "Types/LimbControl.h"
#include "Types/State.h"

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
 * \class SpineData
 * \ingroup Spine
 * \brief Data All controllers and sensors that are specific to just the spine.
 *
 * Data packet definition (772 bytes, 800 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineData));
  }

  RotationRequest upperRotationRequest;  // Stored so update and feedback can use the same data
  RotationRequest lowerRotationRequest;
  TranslationRequest upperTranslationRequest;
  TranslationRequest lowerTranslationRequest;
  ER::DimensionalScaling dimensionalScaling;
  ProcessRequest processEndRotation;  // Objects for processing a request and storing the resulting stiffness/importance in affecting the request
  ProcessRequest processRootRotation;
  ProcessRequest processEndTranslation;
  ProcessRequest processRootTranslation;
  int32_t childIndex;  ///< index into the root module's spines array
  int32_t limbRigIndex;  ///< index into all the rig's limbs
  int32_t limbNetworkIndex;  ///< index into all the network's limbs
  float spineLength;  ///< From chest to pelvis  (Length)
  float timeSinceStep;  ///< Affects drive compensation  (TimePeriod)
  float collidingTime;  /// +ve for time we've been colliding, -ve for time we've been not colliding  (TimePeriod)
  float endGroundCollidingTime;  /// +ve for time the chest has been colliding with ground, -ve for time we've been not colliding  (TimePeriod)
  float rootGroundCollidingTime;  /// +ve for time the pelvis has been colliding with ground, -ve for time we've been not colliding  (TimePeriod)
  float endOnGroundAmount;  /// Amount the chest is on the ground - stays positive for a short while after the chest leaves the ground  (Weight)
  float rootOnGroundAmount;  /// Amount the pelvis is on the ground - stays positive for a short while after the pelvis leaves the ground  (Weight)
  float normalStiffness;  ///< Usual stiffness, e.g. for supporting or turning to look  (Stiffness)
  float normalDampingRatio;  ///< 1 = critical damping  (DampingRatio)
  float normalDriveCompensation;  ///< Larger is more controlled  (Compensation)
  float upperRotationRequestImp;  // Importances stored as we can't write directly to a child anymore  (Weight)
  float lowerRotationRequestImp;             ///< (Weight)
  float upperTranslationRequestImp;          ///< (Weight)
  float lowerTranslationRequestImp;          ///< (Weight)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    upperRotationRequest.validate();
    lowerRotationRequest.validate();
    upperTranslationRequest.validate();
    lowerTranslationRequest.validate();
    processEndRotation.validate();
    processRootRotation.validate();
    processEndTranslation.validate();
    processRootTranslation.validate();
    NM_VALIDATOR(FloatNonNegative(spineLength), "spineLength");
    NM_VALIDATOR(FloatValid(timeSinceStep), "timeSinceStep");
    NM_VALIDATOR(FloatValid(collidingTime), "collidingTime");
    NM_VALIDATOR(FloatValid(endGroundCollidingTime), "endGroundCollidingTime");
    NM_VALIDATOR(FloatValid(rootGroundCollidingTime), "rootGroundCollidingTime");
    NM_VALIDATOR(FloatNonNegative(endOnGroundAmount), "endOnGroundAmount");
    NM_VALIDATOR(FloatNonNegative(rootOnGroundAmount), "rootOnGroundAmount");
    NM_VALIDATOR(FloatNonNegative(normalStiffness), "normalStiffness");
    NM_VALIDATOR(FloatNonNegative(normalDampingRatio), "normalDampingRatio");
    NM_VALIDATOR(FloatNonNegative(normalDriveCompensation), "normalDriveCompensation");
    NM_VALIDATOR(FloatNonNegative(upperRotationRequestImp), "upperRotationRequestImp");
    NM_VALIDATOR(FloatNonNegative(lowerRotationRequestImp), "lowerRotationRequestImp");
    NM_VALIDATOR(FloatNonNegative(upperTranslationRequestImp), "upperTranslationRequestImp");
    NM_VALIDATOR(FloatNonNegative(lowerTranslationRequestImp), "lowerTranslationRequestImp");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SpineInputs
 * \ingroup Spine
 * \brief Inputs All controllers and sensors that are specific to just the spine.
 *
 * Data packet definition (709 bytes, 736 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineInputs));
  }

  NM_INLINE void setEndRotationRequest(const RotationRequest& _endRotationRequest, float endRotationRequest_importance = 1.f)
  {
    endRotationRequest = _endRotationRequest;
    postAssignEndRotationRequest(endRotationRequest);
    NMP_ASSERT(endRotationRequest_importance >= 0.0f && endRotationRequest_importance <= 1.0f);
    m_endRotationRequestImportance = endRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startEndRotationRequestModification()
  {
    m_endRotationRequestImportance = -1.0f; // set invalid until stopEndRotationRequestModification()
    return endRotationRequest;
  }
  NM_INLINE void stopEndRotationRequestModification(float endRotationRequest_importance = 1.f)
  {
    postAssignEndRotationRequest(endRotationRequest);
    NMP_ASSERT(endRotationRequest_importance >= 0.0f && endRotationRequest_importance <= 1.0f);
    m_endRotationRequestImportance = endRotationRequest_importance;
  }
  NM_INLINE float getEndRotationRequestImportance() const { return m_endRotationRequestImportance; }
  NM_INLINE const float& getEndRotationRequestImportanceRef() const { return m_endRotationRequestImportance; }
  NM_INLINE const RotationRequest& getEndRotationRequest() const { return endRotationRequest; }

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

  NM_INLINE void setEndTranslationRequest(const TranslationRequest& _endTranslationRequest, float endTranslationRequest_importance = 1.f)
  {
    endTranslationRequest = _endTranslationRequest;
    postAssignEndTranslationRequest(endTranslationRequest);
    NMP_ASSERT(endTranslationRequest_importance >= 0.0f && endTranslationRequest_importance <= 1.0f);
    m_endTranslationRequestImportance = endTranslationRequest_importance;
  }
  NM_INLINE TranslationRequest& startEndTranslationRequestModification()
  {
    m_endTranslationRequestImportance = -1.0f; // set invalid until stopEndTranslationRequestModification()
    return endTranslationRequest;
  }
  NM_INLINE void stopEndTranslationRequestModification(float endTranslationRequest_importance = 1.f)
  {
    postAssignEndTranslationRequest(endTranslationRequest);
    NMP_ASSERT(endTranslationRequest_importance >= 0.0f && endTranslationRequest_importance <= 1.0f);
    m_endTranslationRequestImportance = endTranslationRequest_importance;
  }
  NM_INLINE float getEndTranslationRequestImportance() const { return m_endTranslationRequestImportance; }
  NM_INLINE const float& getEndTranslationRequestImportanceRef() const { return m_endTranslationRequestImportance; }
  NM_INLINE const TranslationRequest& getEndTranslationRequest() const { return endTranslationRequest; }

  NM_INLINE void setRootTranslationRequest(const TranslationRequest& _rootTranslationRequest, float rootTranslationRequest_importance = 1.f)
  {
    rootTranslationRequest = _rootTranslationRequest;
    postAssignRootTranslationRequest(rootTranslationRequest);
    NMP_ASSERT(rootTranslationRequest_importance >= 0.0f && rootTranslationRequest_importance <= 1.0f);
    m_rootTranslationRequestImportance = rootTranslationRequest_importance;
  }
  NM_INLINE TranslationRequest& startRootTranslationRequestModification()
  {
    m_rootTranslationRequestImportance = -1.0f; // set invalid until stopRootTranslationRequestModification()
    return rootTranslationRequest;
  }
  NM_INLINE void stopRootTranslationRequestModification(float rootTranslationRequest_importance = 1.f)
  {
    postAssignRootTranslationRequest(rootTranslationRequest);
    NMP_ASSERT(rootTranslationRequest_importance >= 0.0f && rootTranslationRequest_importance <= 1.0f);
    m_rootTranslationRequestImportance = rootTranslationRequest_importance;
  }
  NM_INLINE float getRootTranslationRequestImportance() const { return m_rootTranslationRequestImportance; }
  NM_INLINE const float& getRootTranslationRequestImportanceRef() const { return m_rootTranslationRequestImportance; }
  NM_INLINE const TranslationRequest& getRootTranslationRequest() const { return rootTranslationRequest; }

  NM_INLINE void setDefaultPoseWeight(const float& _defaultPoseWeight, float defaultPoseWeight_importance = 1.f)
  {
    defaultPoseWeight = _defaultPoseWeight;
    postAssignDefaultPoseWeight(defaultPoseWeight);
    NMP_ASSERT(defaultPoseWeight_importance >= 0.0f && defaultPoseWeight_importance <= 1.0f);
    m_defaultPoseWeightImportance = defaultPoseWeight_importance;
  }
  NM_INLINE float getDefaultPoseWeightImportance() const { return m_defaultPoseWeightImportance; }
  NM_INLINE const float& getDefaultPoseWeightImportanceRef() const { return m_defaultPoseWeightImportance; }
  NM_INLINE const float& getDefaultPoseWeight() const { return defaultPoseWeight; }

  NM_INLINE void setChestControlledAmount(const float& _chestControlledAmount, float chestControlledAmount_importance = 1.f)
  {
    chestControlledAmount = _chestControlledAmount;
    postAssignChestControlledAmount(chestControlledAmount);
    NMP_ASSERT(chestControlledAmount_importance >= 0.0f && chestControlledAmount_importance <= 1.0f);
    m_chestControlledAmountImportance = chestControlledAmount_importance;
  }
  NM_INLINE float getChestControlledAmountImportance() const { return m_chestControlledAmountImportance; }
  NM_INLINE const float& getChestControlledAmountImportanceRef() const { return m_chestControlledAmountImportance; }
  NM_INLINE const float& getChestControlledAmount() const { return chestControlledAmount; }

  NM_INLINE void setPelvisControlledAmount(const float& _pelvisControlledAmount, float pelvisControlledAmount_importance = 1.f)
  {
    pelvisControlledAmount = _pelvisControlledAmount;
    postAssignPelvisControlledAmount(pelvisControlledAmount);
    NMP_ASSERT(pelvisControlledAmount_importance >= 0.0f && pelvisControlledAmount_importance <= 1.0f);
    m_pelvisControlledAmountImportance = pelvisControlledAmount_importance;
  }
  NM_INLINE float getPelvisControlledAmountImportance() const { return m_pelvisControlledAmountImportance; }
  NM_INLINE const float& getPelvisControlledAmountImportanceRef() const { return m_pelvisControlledAmountImportance; }
  NM_INLINE const float& getPelvisControlledAmount() const { return pelvisControlledAmount; }

  NM_INLINE void setLimbStiffnessReduction(const float& _limbStiffnessReduction, float limbStiffnessReduction_importance = 1.f)
  {
    limbStiffnessReduction = _limbStiffnessReduction;
    postAssignLimbStiffnessReduction(limbStiffnessReduction);
    NMP_ASSERT(limbStiffnessReduction_importance >= 0.0f && limbStiffnessReduction_importance <= 1.0f);
    m_limbStiffnessReductionImportance = limbStiffnessReduction_importance;
  }
  NM_INLINE float getLimbStiffnessReductionImportance() const { return m_limbStiffnessReductionImportance; }
  NM_INLINE const float& getLimbStiffnessReductionImportanceRef() const { return m_limbStiffnessReductionImportance; }
  NM_INLINE const float& getLimbStiffnessReduction() const { return limbStiffnessReduction; }

  NM_INLINE void setLimbControlCompensationReduction(const float& _limbControlCompensationReduction, float limbControlCompensationReduction_importance = 1.f)
  {
    limbControlCompensationReduction = _limbControlCompensationReduction;
    postAssignLimbControlCompensationReduction(limbControlCompensationReduction);
    NMP_ASSERT(limbControlCompensationReduction_importance >= 0.0f && limbControlCompensationReduction_importance <= 1.0f);
    m_limbControlCompensationReductionImportance = limbControlCompensationReduction_importance;
  }
  NM_INLINE float getLimbControlCompensationReductionImportance() const { return m_limbControlCompensationReductionImportance; }
  NM_INLINE const float& getLimbControlCompensationReductionImportanceRef() const { return m_limbControlCompensationReductionImportance; }
  NM_INLINE const float& getLimbControlCompensationReduction() const { return limbControlCompensationReduction; }

  NM_INLINE void setLimbDampingRatioReduction(const float& _limbDampingRatioReduction, float limbDampingRatioReduction_importance = 1.f)
  {
    limbDampingRatioReduction = _limbDampingRatioReduction;
    postAssignLimbDampingRatioReduction(limbDampingRatioReduction);
    NMP_ASSERT(limbDampingRatioReduction_importance >= 0.0f && limbDampingRatioReduction_importance <= 1.0f);
    m_limbDampingRatioReductionImportance = limbDampingRatioReduction_importance;
  }
  NM_INLINE float getLimbDampingRatioReductionImportance() const { return m_limbDampingRatioReductionImportance; }
  NM_INLINE const float& getLimbDampingRatioReductionImportanceRef() const { return m_limbDampingRatioReductionImportance; }
  NM_INLINE const float& getLimbDampingRatioReduction() const { return limbDampingRatioReduction; }

  NM_INLINE void setIsStepping(const bool& _isStepping, float isStepping_importance = 1.f)
  {
    isStepping = _isStepping;
    NMP_ASSERT(isStepping_importance >= 0.0f && isStepping_importance <= 1.0f);
    m_isSteppingImportance = isStepping_importance;
  }
  NM_INLINE float getIsSteppingImportance() const { return m_isSteppingImportance; }
  NM_INLINE const float& getIsSteppingImportanceRef() const { return m_isSteppingImportance; }
  NM_INLINE const bool& getIsStepping() const { return isStepping; }

protected:

  RotationRequest endRotationRequest;  // Desired rotation and offset of the end of the spine relative to the root (in root's space)
  RotationRequest rootRotationRequest;
  TranslationRequest endTranslationRequest;
  TranslationRequest rootTranslationRequest;
  float defaultPoseWeight;  ///< Desired amount to use the default pose if there's no "real" input pose.   (Weight)
  float chestControlledAmount;  ///< 1 means chest can move with normalStiffness  (WeightWithClamp)
  float pelvisControlledAmount;  ///< 1 means pelvis can move with normalStiffness  (WeightWithClamp)
  float limbStiffnessReduction;  // 1 is full reduction so no strength  (Multiplier)
  float limbControlCompensationReduction;    ///< (Multiplier)
  float limbDampingRatioReduction;           ///< (Multiplier)
  bool isStepping;

  // post-assignment stubs
  NM_INLINE void postAssignEndRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
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

  NM_INLINE void postAssignEndTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignRootTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDefaultPoseWeight(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "defaultPoseWeight");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignChestControlledAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "chestControlledAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPelvisControlledAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "pelvisControlledAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLimbStiffnessReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "limbStiffnessReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLimbControlCompensationReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "limbControlCompensationReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLimbDampingRatioReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "limbDampingRatioReduction");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_endRotationRequestImportance, 
     m_rootRotationRequestImportance, 
     m_endTranslationRequestImportance, 
     m_rootTranslationRequestImportance, 
     m_defaultPoseWeightImportance, 
     m_chestControlledAmountImportance, 
     m_pelvisControlledAmountImportance, 
     m_limbStiffnessReductionImportance, 
     m_limbControlCompensationReductionImportance, 
     m_limbDampingRatioReductionImportance, 
     m_isSteppingImportance;

  friend class Spine_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getEndRotationRequestImportance() > 0.0f)
    {
      endRotationRequest.validate();
    }
    if (getRootRotationRequestImportance() > 0.0f)
    {
      rootRotationRequest.validate();
    }
    if (getEndTranslationRequestImportance() > 0.0f)
    {
      endTranslationRequest.validate();
    }
    if (getRootTranslationRequestImportance() > 0.0f)
    {
      rootTranslationRequest.validate();
    }
    if (getDefaultPoseWeightImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(defaultPoseWeight), "defaultPoseWeight");
    }
    if (getChestControlledAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(chestControlledAmount), "chestControlledAmount");
    }
    if (getPelvisControlledAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(pelvisControlledAmount), "pelvisControlledAmount");
    }
    if (getLimbStiffnessReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(limbStiffnessReduction), "limbStiffnessReduction");
    }
    if (getLimbControlCompensationReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(limbControlCompensationReduction), "limbControlCompensationReduction");
    }
    if (getLimbDampingRatioReductionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(limbDampingRatioReduction), "limbDampingRatioReduction");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SpineOutputs
 * \ingroup Spine
 * \brief Outputs All controllers and sensors that are specific to just the spine.
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineOutputs));
  }

  NM_INLINE float getControlImportance() const { return m_controlImportance; }
  NM_INLINE const float& getControlImportanceRef() const { return m_controlImportance; }
  NM_INLINE const LimbControl& getControl() const { return control; }

protected:

  LimbControl control;  ///< IK control of spine

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

  friend class Spine_Con;

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
 * \class SpineFeedbackOutputs
 * \ingroup Spine
 * \brief FeedbackOutputs All controllers and sensors that are specific to just the spine.
 *
 * Data packet definition (841 bytes, 864 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineFeedbackOutputs));
  }

  NM_INLINE void setUpstreamOnlyRotationRequest(const RotationRequest& _upstreamOnlyRotationRequest, float upstreamOnlyRotationRequest_importance = 1.f)
  {
    upstreamOnlyRotationRequest = _upstreamOnlyRotationRequest;
    postAssignUpstreamOnlyRotationRequest(upstreamOnlyRotationRequest);
    NMP_ASSERT(upstreamOnlyRotationRequest_importance >= 0.0f && upstreamOnlyRotationRequest_importance <= 1.0f);
    m_upstreamOnlyRotationRequestImportance = upstreamOnlyRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startUpstreamOnlyRotationRequestModification()
  {
    m_upstreamOnlyRotationRequestImportance = -1.0f; // set invalid until stopUpstreamOnlyRotationRequestModification()
    return upstreamOnlyRotationRequest;
  }
  NM_INLINE void stopUpstreamOnlyRotationRequestModification(float upstreamOnlyRotationRequest_importance = 1.f)
  {
    postAssignUpstreamOnlyRotationRequest(upstreamOnlyRotationRequest);
    NMP_ASSERT(upstreamOnlyRotationRequest_importance >= 0.0f && upstreamOnlyRotationRequest_importance <= 1.0f);
    m_upstreamOnlyRotationRequestImportance = upstreamOnlyRotationRequest_importance;
  }
  NM_INLINE float getUpstreamOnlyRotationRequestImportance() const { return m_upstreamOnlyRotationRequestImportance; }
  NM_INLINE const float& getUpstreamOnlyRotationRequestImportanceRef() const { return m_upstreamOnlyRotationRequestImportance; }
  NM_INLINE const RotationRequest& getUpstreamOnlyRotationRequest() const { return upstreamOnlyRotationRequest; }

  NM_INLINE void setDownstreamOnlyRotationRequest(const RotationRequest& _downstreamOnlyRotationRequest, float downstreamOnlyRotationRequest_importance = 1.f)
  {
    downstreamOnlyRotationRequest = _downstreamOnlyRotationRequest;
    postAssignDownstreamOnlyRotationRequest(downstreamOnlyRotationRequest);
    NMP_ASSERT(downstreamOnlyRotationRequest_importance >= 0.0f && downstreamOnlyRotationRequest_importance <= 1.0f);
    m_downstreamOnlyRotationRequestImportance = downstreamOnlyRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startDownstreamOnlyRotationRequestModification()
  {
    m_downstreamOnlyRotationRequestImportance = -1.0f; // set invalid until stopDownstreamOnlyRotationRequestModification()
    return downstreamOnlyRotationRequest;
  }
  NM_INLINE void stopDownstreamOnlyRotationRequestModification(float downstreamOnlyRotationRequest_importance = 1.f)
  {
    postAssignDownstreamOnlyRotationRequest(downstreamOnlyRotationRequest);
    NMP_ASSERT(downstreamOnlyRotationRequest_importance >= 0.0f && downstreamOnlyRotationRequest_importance <= 1.0f);
    m_downstreamOnlyRotationRequestImportance = downstreamOnlyRotationRequest_importance;
  }
  NM_INLINE float getDownstreamOnlyRotationRequestImportance() const { return m_downstreamOnlyRotationRequestImportance; }
  NM_INLINE const float& getDownstreamOnlyRotationRequestImportanceRef() const { return m_downstreamOnlyRotationRequestImportance; }
  NM_INLINE const RotationRequest& getDownstreamOnlyRotationRequest() const { return downstreamOnlyRotationRequest; }

  NM_INLINE void setUpstreamOnlyTranslationRequest(const TranslationRequest& _upstreamOnlyTranslationRequest, float upstreamOnlyTranslationRequest_importance = 1.f)
  {
    upstreamOnlyTranslationRequest = _upstreamOnlyTranslationRequest;
    postAssignUpstreamOnlyTranslationRequest(upstreamOnlyTranslationRequest);
    NMP_ASSERT(upstreamOnlyTranslationRequest_importance >= 0.0f && upstreamOnlyTranslationRequest_importance <= 1.0f);
    m_upstreamOnlyTranslationRequestImportance = upstreamOnlyTranslationRequest_importance;
  }
  NM_INLINE TranslationRequest& startUpstreamOnlyTranslationRequestModification()
  {
    m_upstreamOnlyTranslationRequestImportance = -1.0f; // set invalid until stopUpstreamOnlyTranslationRequestModification()
    return upstreamOnlyTranslationRequest;
  }
  NM_INLINE void stopUpstreamOnlyTranslationRequestModification(float upstreamOnlyTranslationRequest_importance = 1.f)
  {
    postAssignUpstreamOnlyTranslationRequest(upstreamOnlyTranslationRequest);
    NMP_ASSERT(upstreamOnlyTranslationRequest_importance >= 0.0f && upstreamOnlyTranslationRequest_importance <= 1.0f);
    m_upstreamOnlyTranslationRequestImportance = upstreamOnlyTranslationRequest_importance;
  }
  NM_INLINE float getUpstreamOnlyTranslationRequestImportance() const { return m_upstreamOnlyTranslationRequestImportance; }
  NM_INLINE const float& getUpstreamOnlyTranslationRequestImportanceRef() const { return m_upstreamOnlyTranslationRequestImportance; }
  NM_INLINE const TranslationRequest& getUpstreamOnlyTranslationRequest() const { return upstreamOnlyTranslationRequest; }

  NM_INLINE void setDownstreamOnlyTranslationRequest(const TranslationRequest& _downstreamOnlyTranslationRequest, float downstreamOnlyTranslationRequest_importance = 1.f)
  {
    downstreamOnlyTranslationRequest = _downstreamOnlyTranslationRequest;
    postAssignDownstreamOnlyTranslationRequest(downstreamOnlyTranslationRequest);
    NMP_ASSERT(downstreamOnlyTranslationRequest_importance >= 0.0f && downstreamOnlyTranslationRequest_importance <= 1.0f);
    m_downstreamOnlyTranslationRequestImportance = downstreamOnlyTranslationRequest_importance;
  }
  NM_INLINE TranslationRequest& startDownstreamOnlyTranslationRequestModification()
  {
    m_downstreamOnlyTranslationRequestImportance = -1.0f; // set invalid until stopDownstreamOnlyTranslationRequestModification()
    return downstreamOnlyTranslationRequest;
  }
  NM_INLINE void stopDownstreamOnlyTranslationRequestModification(float downstreamOnlyTranslationRequest_importance = 1.f)
  {
    postAssignDownstreamOnlyTranslationRequest(downstreamOnlyTranslationRequest);
    NMP_ASSERT(downstreamOnlyTranslationRequest_importance >= 0.0f && downstreamOnlyTranslationRequest_importance <= 1.0f);
    m_downstreamOnlyTranslationRequestImportance = downstreamOnlyTranslationRequest_importance;
  }
  NM_INLINE float getDownstreamOnlyTranslationRequestImportance() const { return m_downstreamOnlyTranslationRequestImportance; }
  NM_INLINE const float& getDownstreamOnlyTranslationRequestImportanceRef() const { return m_downstreamOnlyTranslationRequestImportance; }
  NM_INLINE const TranslationRequest& getDownstreamOnlyTranslationRequest() const { return downstreamOnlyTranslationRequest; }

  NM_INLINE void setChestState(const State& _chestState, float chestState_importance = 1.f)
  {
    chestState = _chestState;
    postAssignChestState(chestState);
    NMP_ASSERT(chestState_importance >= 0.0f && chestState_importance <= 1.0f);
    m_chestStateImportance = chestState_importance;
  }
  NM_INLINE State& startChestStateModification()
  {
    m_chestStateImportance = -1.0f; // set invalid until stopChestStateModification()
    return chestState;
  }
  NM_INLINE void stopChestStateModification(float chestState_importance = 1.f)
  {
    postAssignChestState(chestState);
    NMP_ASSERT(chestState_importance >= 0.0f && chestState_importance <= 1.0f);
    m_chestStateImportance = chestState_importance;
  }
  NM_INLINE float getChestStateImportance() const { return m_chestStateImportance; }
  NM_INLINE const float& getChestStateImportanceRef() const { return m_chestStateImportance; }
  NM_INLINE const State& getChestState() const { return chestState; }

  NM_INLINE void setDesiredEndRotationRelRoot(const NMP::Quat& _desiredEndRotationRelRoot, float desiredEndRotationRelRoot_importance = 1.f)
  {
    desiredEndRotationRelRoot = _desiredEndRotationRelRoot;
    postAssignDesiredEndRotationRelRoot(desiredEndRotationRelRoot);
    NMP_ASSERT(desiredEndRotationRelRoot_importance >= 0.0f && desiredEndRotationRelRoot_importance <= 1.0f);
    m_desiredEndRotationRelRootImportance = desiredEndRotationRelRoot_importance;
  }
  NM_INLINE float getDesiredEndRotationRelRootImportance() const { return m_desiredEndRotationRelRootImportance; }
  NM_INLINE const float& getDesiredEndRotationRelRootImportanceRef() const { return m_desiredEndRotationRelRootImportance; }
  NM_INLINE const NMP::Quat& getDesiredEndRotationRelRoot() const { return desiredEndRotationRelRoot; }

  NM_INLINE void setDesiredEndTranslationRelRoot(const NMP::Vector3& _desiredEndTranslationRelRoot, float desiredEndTranslationRelRoot_importance = 1.f)
  {
    desiredEndTranslationRelRoot = _desiredEndTranslationRelRoot;
    postAssignDesiredEndTranslationRelRoot(desiredEndTranslationRelRoot);
    NMP_ASSERT(desiredEndTranslationRelRoot_importance >= 0.0f && desiredEndTranslationRelRoot_importance <= 1.0f);
    m_desiredEndTranslationRelRootImportance = desiredEndTranslationRelRoot_importance;
  }
  NM_INLINE float getDesiredEndTranslationRelRootImportance() const { return m_desiredEndTranslationRelRootImportance; }
  NM_INLINE const float& getDesiredEndTranslationRelRootImportanceRef() const { return m_desiredEndTranslationRelRootImportance; }
  NM_INLINE const NMP::Vector3& getDesiredEndTranslationRelRoot() const { return desiredEndTranslationRelRoot; }

  NM_INLINE void setPelvisControlledAmount(const float& _pelvisControlledAmount, float pelvisControlledAmount_importance = 1.f)
  {
    pelvisControlledAmount = _pelvisControlledAmount;
    postAssignPelvisControlledAmount(pelvisControlledAmount);
    NMP_ASSERT(pelvisControlledAmount_importance >= 0.0f && pelvisControlledAmount_importance <= 1.0f);
    m_pelvisControlledAmountImportance = pelvisControlledAmount_importance;
  }
  NM_INLINE float getPelvisControlledAmountImportance() const { return m_pelvisControlledAmountImportance; }
  NM_INLINE const float& getPelvisControlledAmountImportanceRef() const { return m_pelvisControlledAmountImportance; }
  NM_INLINE const float& getPelvisControlledAmount() const { return pelvisControlledAmount; }

  NM_INLINE void setChestControlledAmount(const float& _chestControlledAmount, float chestControlledAmount_importance = 1.f)
  {
    chestControlledAmount = _chestControlledAmount;
    postAssignChestControlledAmount(chestControlledAmount);
    NMP_ASSERT(chestControlledAmount_importance >= 0.0f && chestControlledAmount_importance <= 1.0f);
    m_chestControlledAmountImportance = chestControlledAmount_importance;
  }
  NM_INLINE float getChestControlledAmountImportance() const { return m_chestControlledAmountImportance; }
  NM_INLINE const float& getChestControlledAmountImportanceRef() const { return m_chestControlledAmountImportance; }
  NM_INLINE const float& getChestControlledAmount() const { return chestControlledAmount; }

  NM_INLINE void setEndOnGroundAmount(const float& _endOnGroundAmount, float endOnGroundAmount_importance = 1.f)
  {
    endOnGroundAmount = _endOnGroundAmount;
    postAssignEndOnGroundAmount(endOnGroundAmount);
    NMP_ASSERT(endOnGroundAmount_importance >= 0.0f && endOnGroundAmount_importance <= 1.0f);
    m_endOnGroundAmountImportance = endOnGroundAmount_importance;
  }
  NM_INLINE float getEndOnGroundAmountImportance() const { return m_endOnGroundAmountImportance; }
  NM_INLINE const float& getEndOnGroundAmountImportanceRef() const { return m_endOnGroundAmountImportance; }
  NM_INLINE const float& getEndOnGroundAmount() const { return endOnGroundAmount; }

  NM_INLINE void setRootOnGroundAmount(const float& _rootOnGroundAmount, float rootOnGroundAmount_importance = 1.f)
  {
    rootOnGroundAmount = _rootOnGroundAmount;
    postAssignRootOnGroundAmount(rootOnGroundAmount);
    NMP_ASSERT(rootOnGroundAmount_importance >= 0.0f && rootOnGroundAmount_importance <= 1.0f);
    m_rootOnGroundAmountImportance = rootOnGroundAmount_importance;
  }
  NM_INLINE float getRootOnGroundAmountImportance() const { return m_rootOnGroundAmountImportance; }
  NM_INLINE const float& getRootOnGroundAmountImportanceRef() const { return m_rootOnGroundAmountImportance; }
  NM_INLINE const float& getRootOnGroundAmount() const { return rootOnGroundAmount; }

  NM_INLINE void setExternalSupportAmount(const float& _externalSupportAmount, float externalSupportAmount_importance = 1.f)
  {
    externalSupportAmount = _externalSupportAmount;
    postAssignExternalSupportAmount(externalSupportAmount);
    NMP_ASSERT(externalSupportAmount_importance >= 0.0f && externalSupportAmount_importance <= 1.0f);
    m_externalSupportAmountImportance = externalSupportAmount_importance;
  }
  NM_INLINE float getExternalSupportAmountImportance() const { return m_externalSupportAmountImportance; }
  NM_INLINE const float& getExternalSupportAmountImportanceRef() const { return m_externalSupportAmountImportance; }
  NM_INLINE const float& getExternalSupportAmount() const { return externalSupportAmount; }

  NM_INLINE void setInContact(const bool& _inContact, float inContact_importance = 1.f)
  {
    inContact = _inContact;
    NMP_ASSERT(inContact_importance >= 0.0f && inContact_importance <= 1.0f);
    m_inContactImportance = inContact_importance;
  }
  NM_INLINE float getInContactImportance() const { return m_inContactImportance; }
  NM_INLINE const float& getInContactImportanceRef() const { return m_inContactImportance; }
  NM_INLINE const bool& getInContact() const { return inContact; }

protected:

  RotationRequest upstreamOnlyRotationRequest;  // Data forwarded on from inputs
  RotationRequest downstreamOnlyRotationRequest;
  TranslationRequest upstreamOnlyTranslationRequest;
  TranslationRequest downstreamOnlyTranslationRequest;
  State chestState;  ///< Pos, vel etc of the chest
  NMP::Quat desiredEndRotationRelRoot;  // These are the default relative TMs for the bind pose  (OrientationDelta)
  NMP::Vector3 desiredEndTranslationRelRoot;  ///< (PositionDelta)
  float pelvisControlledAmount;  // Calculated from the feedIns  (WeightWithClamp)
  float chestControlledAmount;               ///< (WeightWithClamp)
  float endOnGroundAmount;                   ///< (Weight)
  float rootOnGroundAmount;                  ///< (Weight)
  float externalSupportAmount;               ///< (Weight)
  bool inContact;  ///< Spine contacting an external object

  // post-assignment stubs
  NM_INLINE void postAssignUpstreamOnlyRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDownstreamOnlyRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignUpstreamOnlyTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDownstreamOnlyTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignChestState(const State& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDesiredEndRotationRelRoot(const NMP::Quat& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(QuatNormalised(v), "desiredEndRotationRelRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDesiredEndTranslationRelRoot(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "desiredEndTranslationRelRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPelvisControlledAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "pelvisControlledAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignChestControlledAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "chestControlledAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignEndOnGroundAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "endOnGroundAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignRootOnGroundAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "rootOnGroundAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignExternalSupportAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "externalSupportAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_upstreamOnlyRotationRequestImportance, 
     m_downstreamOnlyRotationRequestImportance, 
     m_upstreamOnlyTranslationRequestImportance, 
     m_downstreamOnlyTranslationRequestImportance, 
     m_chestStateImportance, 
     m_desiredEndRotationRelRootImportance, 
     m_desiredEndTranslationRelRootImportance, 
     m_pelvisControlledAmountImportance, 
     m_chestControlledAmountImportance, 
     m_endOnGroundAmountImportance, 
     m_rootOnGroundAmountImportance, 
     m_externalSupportAmountImportance, 
     m_inContactImportance;

  friend class Spine_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getUpstreamOnlyRotationRequestImportance() > 0.0f)
    {
      upstreamOnlyRotationRequest.validate();
    }
    if (getDownstreamOnlyRotationRequestImportance() > 0.0f)
    {
      downstreamOnlyRotationRequest.validate();
    }
    if (getUpstreamOnlyTranslationRequestImportance() > 0.0f)
    {
      upstreamOnlyTranslationRequest.validate();
    }
    if (getDownstreamOnlyTranslationRequestImportance() > 0.0f)
    {
      downstreamOnlyTranslationRequest.validate();
    }
    if (getChestStateImportance() > 0.0f)
    {
      chestState.validate();
    }
    if (getDesiredEndRotationRelRootImportance() > 0.0f)
    {
      NM_VALIDATOR(QuatNormalised(desiredEndRotationRelRoot), "desiredEndRotationRelRoot");
    }
    if (getDesiredEndTranslationRelRootImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(desiredEndTranslationRelRoot), "desiredEndTranslationRelRoot");
    }
    if (getPelvisControlledAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(pelvisControlledAmount), "pelvisControlledAmount");
    }
    if (getChestControlledAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(chestControlledAmount), "chestControlledAmount");
    }
    if (getEndOnGroundAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(endOnGroundAmount), "endOnGroundAmount");
    }
    if (getRootOnGroundAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(rootOnGroundAmount), "rootOnGroundAmount");
    }
    if (getExternalSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(externalSupportAmount), "externalSupportAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SPINE_DATA_H

