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

#ifndef NM_MDF_BALANCEMANAGEMENT_DATA_H
#define NM_MDF_BALANCEMANAGEMENT_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/BalanceManagement.module"

// external types
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMQuat.h"

// known types
#include "Types/RotationRequest.h"
#include "Types/TranslationRequest.h"
#include "Types/DirectionRequest.h"

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
 * \class BalanceManagementInputs
 * \ingroup BalanceManagement
 * \brief Inputs High-level balance control/management
 *
 * Data packet definition (10 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalanceManagementInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalanceManagementInputs));
  }

  NM_INLINE void setSupportWithLegs(const bool& _supportWithLegs, float supportWithLegs_importance = 1.f)
  {
    supportWithLegs = _supportWithLegs;
    NMP_ASSERT(supportWithLegs_importance >= 0.0f && supportWithLegs_importance <= 1.0f);
    m_supportWithLegsImportance = supportWithLegs_importance;
  }
  NM_INLINE float getSupportWithLegsImportance() const { return m_supportWithLegsImportance; }
  NM_INLINE const float& getSupportWithLegsImportanceRef() const { return m_supportWithLegsImportance; }
  NM_INLINE const bool& getSupportWithLegs() const { return supportWithLegs; }

  NM_INLINE void setSupportWithArms(const bool& _supportWithArms, float supportWithArms_importance = 1.f)
  {
    supportWithArms = _supportWithArms;
    NMP_ASSERT(supportWithArms_importance >= 0.0f && supportWithArms_importance <= 1.0f);
    m_supportWithArmsImportance = supportWithArms_importance;
  }
  NM_INLINE float getSupportWithArmsImportance() const { return m_supportWithArmsImportance; }
  NM_INLINE const float& getSupportWithArmsImportanceRef() const { return m_supportWithArmsImportance; }
  NM_INLINE const bool& getSupportWithArms() const { return supportWithArms; }

protected:

  bool supportWithLegs;
  bool supportWithArms;

  // importance values
  float
     m_supportWithLegsImportance, 
     m_supportWithArmsImportance;

  friend class BalanceManagement_Con;

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
 * \class BalanceManagementOutputs
 * \ingroup BalanceManagement
 * \brief Outputs High-level balance control/management
 *
 * Data packet definition (88 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalanceManagementOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalanceManagementOutputs));
  }

  NM_INLINE float getPelvisRelSupportImportance() const { return m_pelvisRelSupportImportance; }
  NM_INLINE const float& getPelvisRelSupportImportanceRef() const { return m_pelvisRelSupportImportance; }
  NM_INLINE const NMP::Matrix34& getPelvisRelSupport() const { return pelvisRelSupport; }

  enum { maxHeadRelChestOrientation = 1 };
  NM_INLINE unsigned int getMaxHeadRelChestOrientation() const { return maxHeadRelChestOrientation; }
  NM_INLINE float getHeadRelChestOrientationImportance(int index) const { return m_headRelChestOrientationImportance[index]; }
  NM_INLINE const float& getHeadRelChestOrientationImportanceRef(int index) const { return m_headRelChestOrientationImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadRelChestOrientation() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headRelChestOrientationImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Quat& getHeadRelChestOrientation(unsigned int index) const { NMP_ASSERT(index <= maxHeadRelChestOrientation); return headRelChestOrientation[index]; }

protected:

  NMP::Matrix34 pelvisRelSupport;            ///< (Transform)
  NMP::Quat headRelChestOrientation[NetworkConstants::networkMaxNumHeads];  ///< (OrientationDelta)

  // post-assignment stubs
  NM_INLINE void postAssignPelvisRelSupport(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "pelvisRelSupport");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadRelChestOrientation(const NMP::Quat& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(QuatNormalised(v), "headRelChestOrientation");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_pelvisRelSupportImportance, 
     m_headRelChestOrientationImportance[NetworkConstants::networkMaxNumHeads];

  friend class BalanceManagement_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getPelvisRelSupportImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(pelvisRelSupport), "pelvisRelSupport");
    }
    {
      uint32_t numHeadRelChestOrientation = calculateNumHeadRelChestOrientation();
      for (uint32_t i=0; i<numHeadRelChestOrientation; i++)
      {
        if (getHeadRelChestOrientationImportance(i) > 0.0f)
        {
          NM_VALIDATOR(QuatNormalised(headRelChestOrientation[i]), "headRelChestOrientation");
        }
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BalanceManagementFeedbackInputs
 * \ingroup BalanceManagement
 * \brief FeedbackInputs High-level balance control/management
 *
 * Data packet definition (1757 bytes, 1760 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalanceManagementFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalanceManagementFeedbackInputs));
  }

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

  NM_INLINE void setAvgReachForBodyPartCDR(const DirectionRequest& _avgReachForBodyPartCDR, float avgReachForBodyPartCDR_importance = 1.f)
  {
    avgReachForBodyPartCDR = _avgReachForBodyPartCDR;
    postAssignAvgReachForBodyPartCDR(avgReachForBodyPartCDR);
    NMP_ASSERT(avgReachForBodyPartCDR_importance >= 0.0f && avgReachForBodyPartCDR_importance <= 1.0f);
    m_avgReachForBodyPartCDRImportance = avgReachForBodyPartCDR_importance;
  }
  NM_INLINE DirectionRequest& startAvgReachForBodyPartCDRModification()
  {
    m_avgReachForBodyPartCDRImportance = -1.0f; // set invalid until stopAvgReachForBodyPartCDRModification()
    return avgReachForBodyPartCDR;
  }
  NM_INLINE void stopAvgReachForBodyPartCDRModification(float avgReachForBodyPartCDR_importance = 1.f)
  {
    postAssignAvgReachForBodyPartCDR(avgReachForBodyPartCDR);
    NMP_ASSERT(avgReachForBodyPartCDR_importance >= 0.0f && avgReachForBodyPartCDR_importance <= 1.0f);
    m_avgReachForBodyPartCDRImportance = avgReachForBodyPartCDR_importance;
  }
  NM_INLINE float getAvgReachForBodyPartCDRImportance() const { return m_avgReachForBodyPartCDRImportance; }
  NM_INLINE const float& getAvgReachForBodyPartCDRImportanceRef() const { return m_avgReachForBodyPartCDRImportance; }
  NM_INLINE const DirectionRequest& getAvgReachForBodyPartCDR() const { return avgReachForBodyPartCDR; }

  NM_INLINE void setAvgReachForWorldCDR(const DirectionRequest& _avgReachForWorldCDR, float avgReachForWorldCDR_importance = 1.f)
  {
    avgReachForWorldCDR = _avgReachForWorldCDR;
    postAssignAvgReachForWorldCDR(avgReachForWorldCDR);
    NMP_ASSERT(avgReachForWorldCDR_importance >= 0.0f && avgReachForWorldCDR_importance <= 1.0f);
    m_avgReachForWorldCDRImportance = avgReachForWorldCDR_importance;
  }
  NM_INLINE DirectionRequest& startAvgReachForWorldCDRModification()
  {
    m_avgReachForWorldCDRImportance = -1.0f; // set invalid until stopAvgReachForWorldCDRModification()
    return avgReachForWorldCDR;
  }
  NM_INLINE void stopAvgReachForWorldCDRModification(float avgReachForWorldCDR_importance = 1.f)
  {
    postAssignAvgReachForWorldCDR(avgReachForWorldCDR);
    NMP_ASSERT(avgReachForWorldCDR_importance >= 0.0f && avgReachForWorldCDR_importance <= 1.0f);
    m_avgReachForWorldCDRImportance = avgReachForWorldCDR_importance;
  }
  NM_INLINE float getAvgReachForWorldCDRImportance() const { return m_avgReachForWorldCDRImportance; }
  NM_INLINE const float& getAvgReachForWorldCDRImportanceRef() const { return m_avgReachForWorldCDRImportance; }
  NM_INLINE const DirectionRequest& getAvgReachForWorldCDR() const { return avgReachForWorldCDR; }

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

  NM_INLINE void setAvgReachForBodyPartPDR(const DirectionRequest& _avgReachForBodyPartPDR, float avgReachForBodyPartPDR_importance = 1.f)
  {
    avgReachForBodyPartPDR = _avgReachForBodyPartPDR;
    postAssignAvgReachForBodyPartPDR(avgReachForBodyPartPDR);
    NMP_ASSERT(avgReachForBodyPartPDR_importance >= 0.0f && avgReachForBodyPartPDR_importance <= 1.0f);
    m_avgReachForBodyPartPDRImportance = avgReachForBodyPartPDR_importance;
  }
  NM_INLINE DirectionRequest& startAvgReachForBodyPartPDRModification()
  {
    m_avgReachForBodyPartPDRImportance = -1.0f; // set invalid until stopAvgReachForBodyPartPDRModification()
    return avgReachForBodyPartPDR;
  }
  NM_INLINE void stopAvgReachForBodyPartPDRModification(float avgReachForBodyPartPDR_importance = 1.f)
  {
    postAssignAvgReachForBodyPartPDR(avgReachForBodyPartPDR);
    NMP_ASSERT(avgReachForBodyPartPDR_importance >= 0.0f && avgReachForBodyPartPDR_importance <= 1.0f);
    m_avgReachForBodyPartPDRImportance = avgReachForBodyPartPDR_importance;
  }
  NM_INLINE float getAvgReachForBodyPartPDRImportance() const { return m_avgReachForBodyPartPDRImportance; }
  NM_INLINE const float& getAvgReachForBodyPartPDRImportanceRef() const { return m_avgReachForBodyPartPDRImportance; }
  NM_INLINE const DirectionRequest& getAvgReachForBodyPartPDR() const { return avgReachForBodyPartPDR; }

  NM_INLINE void setAvgReachForWorldPDR(const DirectionRequest& _avgReachForWorldPDR, float avgReachForWorldPDR_importance = 1.f)
  {
    avgReachForWorldPDR = _avgReachForWorldPDR;
    postAssignAvgReachForWorldPDR(avgReachForWorldPDR);
    NMP_ASSERT(avgReachForWorldPDR_importance >= 0.0f && avgReachForWorldPDR_importance <= 1.0f);
    m_avgReachForWorldPDRImportance = avgReachForWorldPDR_importance;
  }
  NM_INLINE DirectionRequest& startAvgReachForWorldPDRModification()
  {
    m_avgReachForWorldPDRImportance = -1.0f; // set invalid until stopAvgReachForWorldPDRModification()
    return avgReachForWorldPDR;
  }
  NM_INLINE void stopAvgReachForWorldPDRModification(float avgReachForWorldPDR_importance = 1.f)
  {
    postAssignAvgReachForWorldPDR(avgReachForWorldPDR);
    NMP_ASSERT(avgReachForWorldPDR_importance >= 0.0f && avgReachForWorldPDR_importance <= 1.0f);
    m_avgReachForWorldPDRImportance = avgReachForWorldPDR_importance;
  }
  NM_INLINE float getAvgReachForWorldPDRImportance() const { return m_avgReachForWorldPDRImportance; }
  NM_INLINE const float& getAvgReachForWorldPDRImportanceRef() const { return m_avgReachForWorldPDRImportance; }
  NM_INLINE const DirectionRequest& getAvgReachForWorldPDR() const { return avgReachForWorldPDR; }

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

  NM_INLINE void setAvgReachForBodyPartPTR(const TranslationRequest& _avgReachForBodyPartPTR, float avgReachForBodyPartPTR_importance = 1.f)
  {
    avgReachForBodyPartPTR = _avgReachForBodyPartPTR;
    postAssignAvgReachForBodyPartPTR(avgReachForBodyPartPTR);
    NMP_ASSERT(avgReachForBodyPartPTR_importance >= 0.0f && avgReachForBodyPartPTR_importance <= 1.0f);
    m_avgReachForBodyPartPTRImportance = avgReachForBodyPartPTR_importance;
  }
  NM_INLINE TranslationRequest& startAvgReachForBodyPartPTRModification()
  {
    m_avgReachForBodyPartPTRImportance = -1.0f; // set invalid until stopAvgReachForBodyPartPTRModification()
    return avgReachForBodyPartPTR;
  }
  NM_INLINE void stopAvgReachForBodyPartPTRModification(float avgReachForBodyPartPTR_importance = 1.f)
  {
    postAssignAvgReachForBodyPartPTR(avgReachForBodyPartPTR);
    NMP_ASSERT(avgReachForBodyPartPTR_importance >= 0.0f && avgReachForBodyPartPTR_importance <= 1.0f);
    m_avgReachForBodyPartPTRImportance = avgReachForBodyPartPTR_importance;
  }
  NM_INLINE float getAvgReachForBodyPartPTRImportance() const { return m_avgReachForBodyPartPTRImportance; }
  NM_INLINE const float& getAvgReachForBodyPartPTRImportanceRef() const { return m_avgReachForBodyPartPTRImportance; }
  NM_INLINE const TranslationRequest& getAvgReachForBodyPartPTR() const { return avgReachForBodyPartPTR; }

  NM_INLINE void setAvgReachForWorldPTR(const TranslationRequest& _avgReachForWorldPTR, float avgReachForWorldPTR_importance = 1.f)
  {
    avgReachForWorldPTR = _avgReachForWorldPTR;
    postAssignAvgReachForWorldPTR(avgReachForWorldPTR);
    NMP_ASSERT(avgReachForWorldPTR_importance >= 0.0f && avgReachForWorldPTR_importance <= 1.0f);
    m_avgReachForWorldPTRImportance = avgReachForWorldPTR_importance;
  }
  NM_INLINE TranslationRequest& startAvgReachForWorldPTRModification()
  {
    m_avgReachForWorldPTRImportance = -1.0f; // set invalid until stopAvgReachForWorldPTRModification()
    return avgReachForWorldPTR;
  }
  NM_INLINE void stopAvgReachForWorldPTRModification(float avgReachForWorldPTR_importance = 1.f)
  {
    postAssignAvgReachForWorldPTR(avgReachForWorldPTR);
    NMP_ASSERT(avgReachForWorldPTR_importance >= 0.0f && avgReachForWorldPTR_importance <= 1.0f);
    m_avgReachForWorldPTRImportance = avgReachForWorldPTR_importance;
  }
  NM_INLINE float getAvgReachForWorldPTRImportance() const { return m_avgReachForWorldPTRImportance; }
  NM_INLINE const float& getAvgReachForWorldPTRImportanceRef() const { return m_avgReachForWorldPTRImportance; }
  NM_INLINE const TranslationRequest& getAvgReachForWorldPTR() const { return avgReachForWorldPTR; }

  NM_INLINE void setImpendingInstability(const float& _impendingInstability, float impendingInstability_importance = 1.f)
  {
    impendingInstability = _impendingInstability;
    postAssignImpendingInstability(impendingInstability);
    NMP_ASSERT(impendingInstability_importance >= 0.0f && impendingInstability_importance <= 1.0f);
    m_impendingInstabilityImportance = impendingInstability_importance;
  }
  NM_INLINE float getImpendingInstabilityImportance() const { return m_impendingInstabilityImportance; }
  NM_INLINE const float& getImpendingInstabilityImportanceRef() const { return m_impendingInstabilityImportance; }
  NM_INLINE const float& getImpendingInstability() const { return impendingInstability; }

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

  DirectionRequest chestDirectionRequest;
  DirectionRequest avgReachForBodyPartCDR;
  DirectionRequest avgReachForWorldCDR;
  DirectionRequest pelvisDirectionRequest;
  DirectionRequest avgReachForBodyPartPDR;
  DirectionRequest avgReachForWorldPDR;
  RotationRequest pelvisRotationRequest;
  TranslationRequest pelvisTranslationRequest;
  TranslationRequest avgReachForBodyPartPTR;
  TranslationRequest avgReachForWorldPTR;
  float impendingInstability;  ///< this value is the magnitude of the instability, and the importance is 1/(1+ time to instability)  (Weight)
  float supportAmount;                       ///< (Weight)
  bool isStepping;

  // post-assignment stubs
  NM_INLINE void postAssignChestDirectionRequest(const DirectionRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAvgReachForBodyPartCDR(const DirectionRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAvgReachForWorldCDR(const DirectionRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPelvisDirectionRequest(const DirectionRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAvgReachForBodyPartPDR(const DirectionRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAvgReachForWorldPDR(const DirectionRequest& NMP_VALIDATOR_ARG(v)) const
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

  NM_INLINE void postAssignPelvisTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAvgReachForBodyPartPTR(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAvgReachForWorldPTR(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignImpendingInstability(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "impendingInstability");
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
     m_chestDirectionRequestImportance, 
     m_avgReachForBodyPartCDRImportance, 
     m_avgReachForWorldCDRImportance, 
     m_pelvisDirectionRequestImportance, 
     m_avgReachForBodyPartPDRImportance, 
     m_avgReachForWorldPDRImportance, 
     m_pelvisRotationRequestImportance, 
     m_pelvisTranslationRequestImportance, 
     m_avgReachForBodyPartPTRImportance, 
     m_avgReachForWorldPTRImportance, 
     m_impendingInstabilityImportance, 
     m_supportAmountImportance, 
     m_isSteppingImportance;

  friend class BalanceManagement_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getChestDirectionRequestImportance() > 0.0f)
    {
      chestDirectionRequest.validate();
    }
    if (getAvgReachForBodyPartCDRImportance() > 0.0f)
    {
      avgReachForBodyPartCDR.validate();
    }
    if (getAvgReachForWorldCDRImportance() > 0.0f)
    {
      avgReachForWorldCDR.validate();
    }
    if (getPelvisDirectionRequestImportance() > 0.0f)
    {
      pelvisDirectionRequest.validate();
    }
    if (getAvgReachForBodyPartPDRImportance() > 0.0f)
    {
      avgReachForBodyPartPDR.validate();
    }
    if (getAvgReachForWorldPDRImportance() > 0.0f)
    {
      avgReachForWorldPDR.validate();
    }
    if (getPelvisRotationRequestImportance() > 0.0f)
    {
      pelvisRotationRequest.validate();
    }
    if (getPelvisTranslationRequestImportance() > 0.0f)
    {
      pelvisTranslationRequest.validate();
    }
    if (getAvgReachForBodyPartPTRImportance() > 0.0f)
    {
      avgReachForBodyPartPTR.validate();
    }
    if (getAvgReachForWorldPTRImportance() > 0.0f)
    {
      avgReachForWorldPTR.validate();
    }
    if (getImpendingInstabilityImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(impendingInstability), "impendingInstability");
    }
    if (getSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(supportAmount), "supportAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BALANCEMANAGEMENT_DATA_H

