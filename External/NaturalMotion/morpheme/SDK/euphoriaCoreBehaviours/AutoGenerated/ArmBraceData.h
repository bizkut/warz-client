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

#ifndef NM_MDF_ARMBRACE_DATA_H
#define NM_MDF_ARMBRACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmBrace.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/BodyState.h"
#include "Types/BraceHazard.h"
#include "Types/RotationRequest.h"
#include "Types/BraceState.h"
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
 * \class ArmBraceData
 * \ingroup ArmBrace
 * \brief Data Reach out towards hazard in order to get arms into the best position to cushion after impact
 *  then cushion the impact
 *
 * Data packet definition (49 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmBraceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmBraceData));
  }

  NMP::Vector3 reachDirection;  ///< Direction to hazard  (Direction)
  NMP::Vector3 placePos;  ///< Position to place hands  (Position)
  int64_t patchShapeID;  ///< Used to tell whether the hand is in contact with the passed in hazard
  float importance;  ///< Importance of the resulting limb control  (Weight)
  float imminence;  ///< 1/time to impact  (Imminence)
  bool doingBrace;  ///< Used for hysteresis

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(reachDirection), "reachDirection");
    NM_VALIDATOR(Vector3Valid(placePos), "placePos");
    NM_VALIDATOR(FloatNonNegative(importance), "importance");
    NM_VALIDATOR(FloatValid(imminence), "imminence");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmBraceInputs
 * \ingroup ArmBrace
 * \brief Inputs Reach out towards hazard in order to get arms into the best position to cushion after impact
 *  then cushion the impact
 *
 * Data packet definition (409 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmBraceInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmBraceInputs));
  }

  NM_INLINE void setBraceHazard(const BraceHazard& _braceHazard, float braceHazard_importance = 1.f)
  {
    braceHazard = _braceHazard;
    postAssignBraceHazard(braceHazard);
    NMP_ASSERT(braceHazard_importance >= 0.0f && braceHazard_importance <= 1.0f);
    m_braceHazardImportance = braceHazard_importance;
  }
  NM_INLINE BraceHazard& startBraceHazardModification()
  {
    m_braceHazardImportance = -1.0f; // set invalid until stopBraceHazardModification()
    return braceHazard;
  }
  NM_INLINE void stopBraceHazardModification(float braceHazard_importance = 1.f)
  {
    postAssignBraceHazard(braceHazard);
    NMP_ASSERT(braceHazard_importance >= 0.0f && braceHazard_importance <= 1.0f);
    m_braceHazardImportance = braceHazard_importance;
  }
  NM_INLINE float getBraceHazardImportance() const { return m_braceHazardImportance; }
  NM_INLINE const float& getBraceHazardImportanceRef() const { return m_braceHazardImportance; }
  NM_INLINE const BraceHazard& getBraceHazard() const { return braceHazard; }

  NM_INLINE void setProtectState(const BodyState& _protectState, float protectState_importance = 1.f)
  {
    protectState = _protectState;
    postAssignProtectState(protectState);
    NMP_ASSERT(protectState_importance >= 0.0f && protectState_importance <= 1.0f);
    m_protectStateImportance = protectState_importance;
  }
  NM_INLINE BodyState& startProtectStateModification()
  {
    m_protectStateImportance = -1.0f; // set invalid until stopProtectStateModification()
    return protectState;
  }
  NM_INLINE void stopProtectStateModification(float protectState_importance = 1.f)
  {
    postAssignProtectState(protectState);
    NMP_ASSERT(protectState_importance >= 0.0f && protectState_importance <= 1.0f);
    m_protectStateImportance = protectState_importance;
  }
  NM_INLINE float getProtectStateImportance() const { return m_protectStateImportance; }
  NM_INLINE const float& getProtectStateImportanceRef() const { return m_protectStateImportance; }
  NM_INLINE const BodyState& getProtectState() const { return protectState; }

  NM_INLINE void setCushionPoint(const NMP::Vector3& _cushionPoint, float cushionPoint_importance = 1.f)
  {
    cushionPoint = _cushionPoint;
    postAssignCushionPoint(cushionPoint);
    NMP_ASSERT(cushionPoint_importance >= 0.0f && cushionPoint_importance <= 1.0f);
    m_cushionPointImportance = cushionPoint_importance;
  }
  NM_INLINE float getCushionPointImportance() const { return m_cushionPointImportance; }
  NM_INLINE const float& getCushionPointImportanceRef() const { return m_cushionPointImportance; }
  NM_INLINE const NMP::Vector3& getCushionPoint() const { return cushionPoint; }

  NM_INLINE void setBraceScale(const float& _braceScale, float braceScale_importance = 1.f)
  {
    braceScale = _braceScale;
    postAssignBraceScale(braceScale);
    NMP_ASSERT(braceScale_importance >= 0.0f && braceScale_importance <= 1.0f);
    m_braceScaleImportance = braceScale_importance;
  }
  NM_INLINE float getBraceScaleImportance() const { return m_braceScaleImportance; }
  NM_INLINE const float& getBraceScaleImportanceRef() const { return m_braceScaleImportance; }
  NM_INLINE const float& getBraceScale() const { return braceScale; }

  NM_INLINE void setMaxArmExtensionScale(const float& _maxArmExtensionScale, float maxArmExtensionScale_importance = 1.f)
  {
    maxArmExtensionScale = _maxArmExtensionScale;
    postAssignMaxArmExtensionScale(maxArmExtensionScale);
    NMP_ASSERT(maxArmExtensionScale_importance >= 0.0f && maxArmExtensionScale_importance <= 1.0f);
    m_maxArmExtensionScaleImportance = maxArmExtensionScale_importance;
  }
  NM_INLINE float getMaxArmExtensionScaleImportance() const { return m_maxArmExtensionScaleImportance; }
  NM_INLINE const float& getMaxArmExtensionScaleImportanceRef() const { return m_maxArmExtensionScaleImportance; }
  NM_INLINE const float& getMaxArmExtensionScale() const { return maxArmExtensionScale; }

  NM_INLINE void setSwivelAmount(const float& _swivelAmount, float swivelAmount_importance = 1.f)
  {
    swivelAmount = _swivelAmount;
    postAssignSwivelAmount(swivelAmount);
    NMP_ASSERT(swivelAmount_importance >= 0.0f && swivelAmount_importance <= 1.0f);
    m_swivelAmountImportance = swivelAmount_importance;
  }
  NM_INLINE float getSwivelAmountImportance() const { return m_swivelAmountImportance; }
  NM_INLINE const float& getSwivelAmountImportanceRef() const { return m_swivelAmountImportance; }
  NM_INLINE const float& getSwivelAmount() const { return swivelAmount; }

  NM_INLINE void setShouldBrace(const bool& _shouldBrace, float shouldBrace_importance = 1.f)
  {
    shouldBrace = _shouldBrace;
    NMP_ASSERT(shouldBrace_importance >= 0.0f && shouldBrace_importance <= 1.0f);
    m_shouldBraceImportance = shouldBrace_importance;
  }
  NM_INLINE float getShouldBraceImportance() const { return m_shouldBraceImportance; }
  NM_INLINE const float& getShouldBraceImportanceRef() const { return m_shouldBraceImportance; }
  NM_INLINE const bool& getShouldBrace() const { return shouldBrace; }

protected:

  BraceHazard braceHazard;  ///< The main object used for determining the brace
  BodyState protectState;  ///< pos, vel etc of the protect area, which is around the chest
  NMP::Vector3 cushionPoint;  ///< Middle of the polygon of support for when cushioning  (Position)
  float braceScale;  ///< Controls how much brace is requested from lower body, since the arm brace is already scaled by braceScale in the braceHazardImportance  (Multiplier)
  float maxArmExtensionScale;  ///< 1 is full arm extension   (Weight)
  float swivelAmount;  ///< refer to the guide poses for the posture at each swivel amount, usually -1 to 1  (Multiplier)
  bool shouldBrace;  ///< Based on how close that limb is to being able to reach the hazard

  // post-assignment stubs
  NM_INLINE void postAssignBraceHazard(const BraceHazard& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignProtectState(const BodyState& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignCushionPoint(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "cushionPoint");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBraceScale(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "braceScale");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignMaxArmExtensionScale(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "maxArmExtensionScale");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSwivelAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "swivelAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_braceHazardImportance, 
     m_protectStateImportance, 
     m_cushionPointImportance, 
     m_braceScaleImportance, 
     m_maxArmExtensionScaleImportance, 
     m_swivelAmountImportance, 
     m_shouldBraceImportance;

  friend class ArmBrace_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getBraceHazardImportance() > 0.0f)
    {
      braceHazard.validate();
    }
    if (getProtectStateImportance() > 0.0f)
    {
      protectState.validate();
    }
    if (getCushionPointImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(cushionPoint), "cushionPoint");
    }
    if (getBraceScaleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(braceScale), "braceScale");
    }
    if (getMaxArmExtensionScaleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(maxArmExtensionScale), "maxArmExtensionScale");
    }
    if (getSwivelAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(swivelAmount), "swivelAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmBraceOutputs
 * \ingroup ArmBrace
 * \brief Outputs Reach out towards hazard in order to get arms into the best position to cushion after impact
 *  then cushion the impact
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmBraceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmBraceOutputs));
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

  LimbControl control;  ///< IK Control of arm

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

  friend class ArmBrace_Con;

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
 * \class ArmBraceFeedbackInputs
 * \ingroup ArmBrace
 * \brief FeedbackInputs Reach out towards hazard in order to get arms into the best position to cushion after impact
 *  then cushion the impact
 *
 * Data packet definition (10 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmBraceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmBraceFeedbackInputs));
  }

  NM_INLINE void setLimbInContactWithPatchShapeID(const bool& _limbInContactWithPatchShapeID, float limbInContactWithPatchShapeID_importance = 1.f)
  {
    limbInContactWithPatchShapeID = _limbInContactWithPatchShapeID;
    NMP_ASSERT(limbInContactWithPatchShapeID_importance >= 0.0f && limbInContactWithPatchShapeID_importance <= 1.0f);
    m_limbInContactWithPatchShapeIDImportance = limbInContactWithPatchShapeID_importance;
  }
  NM_INLINE float getLimbInContactWithPatchShapeIDImportance() const { return m_limbInContactWithPatchShapeIDImportance; }
  NM_INLINE const float& getLimbInContactWithPatchShapeIDImportanceRef() const { return m_limbInContactWithPatchShapeIDImportance; }
  NM_INLINE const bool& getLimbInContactWithPatchShapeID() const { return limbInContactWithPatchShapeID; }

  NM_INLINE void setRootInContactWithPatchShapeID(const bool& _rootInContactWithPatchShapeID, float rootInContactWithPatchShapeID_importance = 1.f)
  {
    rootInContactWithPatchShapeID = _rootInContactWithPatchShapeID;
    NMP_ASSERT(rootInContactWithPatchShapeID_importance >= 0.0f && rootInContactWithPatchShapeID_importance <= 1.0f);
    m_rootInContactWithPatchShapeIDImportance = rootInContactWithPatchShapeID_importance;
  }
  NM_INLINE float getRootInContactWithPatchShapeIDImportance() const { return m_rootInContactWithPatchShapeIDImportance; }
  NM_INLINE const float& getRootInContactWithPatchShapeIDImportanceRef() const { return m_rootInContactWithPatchShapeIDImportance; }
  NM_INLINE const bool& getRootInContactWithPatchShapeID() const { return rootInContactWithPatchShapeID; }

protected:

  bool limbInContactWithPatchShapeID;  ///< Result of contact test, is any part of the limb in contact with the incoming hazard
  bool rootInContactWithPatchShapeID;  ///< Result of contact test, is the limb's root part in contact with the incoming hazard

  // importance values
  float
     m_limbInContactWithPatchShapeIDImportance, 
     m_rootInContactWithPatchShapeIDImportance;

  friend class ArmBrace_Con;

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
 * \class ArmBraceFeedbackOutputs
 * \ingroup ArmBrace
 * \brief FeedbackOutputs Reach out towards hazard in order to get arms into the best position to cushion after impact
 *  then cushion the impact
 *
 * Data packet definition (220 bytes, 224 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmBraceFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmBraceFeedbackOutputs));
  }

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

  NM_INLINE void setBraceState(const BraceState& _braceState, float braceState_importance = 1.f)
  {
    braceState = _braceState;
    postAssignBraceState(braceState);
    NMP_ASSERT(braceState_importance >= 0.0f && braceState_importance <= 1.0f);
    m_braceStateImportance = braceState_importance;
  }
  NM_INLINE float getBraceStateImportance() const { return m_braceStateImportance; }
  NM_INLINE const float& getBraceStateImportanceRef() const { return m_braceStateImportance; }
  NM_INLINE const BraceState& getBraceState() const { return braceState; }

  NM_INLINE void setEndCushionPoint(const NMP::Vector3& _endCushionPoint, float endCushionPoint_importance = 1.f)
  {
    endCushionPoint = _endCushionPoint;
    postAssignEndCushionPoint(endCushionPoint);
    NMP_ASSERT(endCushionPoint_importance >= 0.0f && endCushionPoint_importance <= 1.0f);
    m_endCushionPointImportance = endCushionPoint_importance;
  }
  NM_INLINE float getEndCushionPointImportance() const { return m_endCushionPointImportance; }
  NM_INLINE const float& getEndCushionPointImportanceRef() const { return m_endCushionPointImportance; }
  NM_INLINE const NMP::Vector3& getEndCushionPoint() const { return endCushionPoint; }

protected:

  RotationRequest rootRotationRequest;  ///< Requests a rotation of the chest
  BraceState braceState;  ///< Used by the BraceChooser to decide if the arm is in a good position to brace compared to the others
  NMP::Vector3 endCushionPoint;  ///< Passes up hand position to generate averaged cushionPoint as input  (Position)

  // post-assignment stubs
  NM_INLINE void postAssignRootRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBraceState(const BraceState& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignEndCushionPoint(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "endCushionPoint");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rootRotationRequestImportance, 
     m_braceStateImportance, 
     m_endCushionPointImportance;

  friend class ArmBrace_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRootRotationRequestImportance() > 0.0f)
    {
      rootRotationRequest.validate();
    }
    if (getBraceStateImportance() > 0.0f)
    {
      braceState.validate();
    }
    if (getEndCushionPointImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(endCushionPoint), "endCushionPoint");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMBRACE_DATA_H

