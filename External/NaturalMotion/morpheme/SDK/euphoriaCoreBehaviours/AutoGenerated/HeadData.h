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

#ifndef NM_MDF_HEAD_DATA_H
#define NM_MDF_HEAD_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/Head.module"

// external types
#include "euphoria/erDimensionalScaling.h"
#include "NMPlatform/NMVector3.h"

// known types
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
 * \class HeadData
 * \ingroup Head
 * \brief Data All controllers and sensors that are specific to just the physical neck and head
 *
 * Data packet definition (50 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadData));
  }

  ER::DimensionalScaling dimensionalScaling;
  int32_t limbRigIndex;  ///< index into all the rig's limbs
  int32_t limbNetworkIndex;  ///< index into all the network's limbs
  int32_t childIndex;  ///< index into network's array of arms.
  float contactForceScalarSqr;  ///< TODO this is only used for a unit test
  float length;  ///< Distance from chest to centre of head  (Length)
  float normalStiffness;  ///< Usual stiffness, e.g. for holding the head up  (Stiffness)
  float normalDampingRatio;  ///< 1 is critical damping  (DampingRatio)
  float normalDriveCompensation;  ///< Larger is more controlled  (Compensation)
  bool headInContact;
  bool chestInContact;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(contactForceScalarSqr), "contactForceScalarSqr");
    NM_VALIDATOR(FloatNonNegative(length), "length");
    NM_VALIDATOR(FloatNonNegative(normalStiffness), "normalStiffness");
    NM_VALIDATOR(FloatNonNegative(normalDampingRatio), "normalDampingRatio");
    NM_VALIDATOR(FloatNonNegative(normalDriveCompensation), "normalDriveCompensation");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HeadInputs
 * \ingroup Head
 * \brief Inputs All controllers and sensors that are specific to just the physical neck and head
 *
 * Data packet definition (37 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadInputs));
  }

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

  NM_INLINE void setUpperBodyInContact(const bool& _upperBodyInContact, float upperBodyInContact_importance = 1.f)
  {
    upperBodyInContact = _upperBodyInContact;
    NMP_ASSERT(upperBodyInContact_importance >= 0.0f && upperBodyInContact_importance <= 1.0f);
    m_upperBodyInContactImportance = upperBodyInContact_importance;
  }
  NM_INLINE float getUpperBodyInContactImportance() const { return m_upperBodyInContactImportance; }
  NM_INLINE const float& getUpperBodyInContactImportanceRef() const { return m_upperBodyInContactImportance; }
  NM_INLINE const bool& getUpperBodyInContact() const { return upperBodyInContact; }

protected:

  float chestControlledAmount;  ///< How well controlled the chest is, if it is 1 then it should move with normalStiffness  (WeightWithClamp)
  float limbStiffnessReduction;  // 1 is full reduction so no stiffness / control / damping  (Multiplier)
  float limbControlCompensationReduction;    ///< (Multiplier)
  float limbDampingRatioReduction;           ///< (Multiplier)
  bool upperBodyInContact;  ///< Is any limb in the upper body contacting an external object

  // post-assignment stubs
  NM_INLINE void postAssignChestControlledAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "chestControlledAmount");
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
     m_chestControlledAmountImportance, 
     m_limbStiffnessReductionImportance, 
     m_limbControlCompensationReductionImportance, 
     m_limbDampingRatioReductionImportance, 
     m_upperBodyInContactImportance;

  friend class Head_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getChestControlledAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(chestControlledAmount), "chestControlledAmount");
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
 * \class HeadOutputs
 * \ingroup Head
 * \brief Outputs All controllers and sensors that are specific to just the physical neck and head
 *
 * Data packet definition (776 bytes, 800 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadOutputs));
  }

  NM_INLINE float getControlImportance() const { return m_controlImportance; }
  NM_INLINE const float& getControlImportanceRef() const { return m_controlImportance; }
  NM_INLINE const LimbControl& getControl() const { return control; }

  NM_INLINE float getAverageImportance() const { return m_averageImportance; }
  NM_INLINE const float& getAverageImportanceRef() const { return m_averageImportance; }
  NM_INLINE const LimbControl& getAverage() const { return average; }

protected:

  LimbControl control;  ///< IK control of the head
  LimbControl average;  ///< This is a temporary output to get rid of junctions feeding into junctions

  // post-assignment stubs
  NM_INLINE void postAssignControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverage(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_controlImportance, 
     m_averageImportance;

  friend class Head_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getControlImportance() > 0.0f)
    {
      control.validate();
    }
    if (getAverageImportance() > 0.0f)
    {
      average.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HeadFeedbackOutputs
 * \ingroup Head
 * \brief FeedbackOutputs All controllers and sensors that are specific to just the physical neck and head
 *
 * Data packet definition (25 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadFeedbackOutputs));
  }

  NM_INLINE void setCurrEndPartPosition(const NMP::Vector3& _currEndPartPosition, float currEndPartPosition_importance = 1.f)
  {
    currEndPartPosition = _currEndPartPosition;
    postAssignCurrEndPartPosition(currEndPartPosition);
    NMP_ASSERT(currEndPartPosition_importance >= 0.0f && currEndPartPosition_importance <= 1.0f);
    m_currEndPartPositionImportance = currEndPartPosition_importance;
  }
  NM_INLINE float getCurrEndPartPositionImportance() const { return m_currEndPartPositionImportance; }
  NM_INLINE const float& getCurrEndPartPositionImportanceRef() const { return m_currEndPartPositionImportance; }
  NM_INLINE const NMP::Vector3& getCurrEndPartPosition() const { return currEndPartPosition; }

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

  NMP::Vector3 currEndPartPosition;  ///< The centre of the head   (Position)
  bool inContact;

  // post-assignment stubs
  NM_INLINE void postAssignCurrEndPartPosition(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "currEndPartPosition");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_currEndPartPositionImportance, 
     m_inContactImportance;

  friend class Head_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getCurrEndPartPositionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(currEndPartPosition), "currEndPartPosition");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HEAD_DATA_H

