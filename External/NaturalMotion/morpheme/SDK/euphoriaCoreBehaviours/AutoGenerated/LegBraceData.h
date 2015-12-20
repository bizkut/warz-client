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

#ifndef NM_MDF_LEGBRACE_DATA_H
#define NM_MDF_LEGBRACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/LegBrace.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/BodyState.h"
#include "Types/BraceHazard.h"
#include "Types/BraceState.h"
#include "Types/RotationRequest.h"
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
 * \class LegBraceData
 * \ingroup LegBrace
 * \brief Data reach out towards hazard in order to get legs into the best position to cushion after impact
 *  then cushions the impact
 *
 * Data packet definition (65 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegBraceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegBraceData));
  }

  NMP::Vector3 reachDirection;  ///< Direction to brace in  (Direction)
  NMP::Vector3 placePos;  ///< Where exactly to place the feet  (Position)
  NMP::Vector3 protectPoint;  ///< The point to be protected, roughly the stomach, depending on protectChestAmount  (Position)
  int64_t patchShapeID;  ///< The shape to test is contacted
  float importance;  ///< Importance of the resulting limb control  (Weight)
  float imminence;  ///< How soon the impact is  (Imminence)
  bool doingBrace;  ///< Used for hysteresis

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(reachDirection), "reachDirection");
    NM_VALIDATOR(Vector3Valid(placePos), "placePos");
    NM_VALIDATOR(Vector3Valid(protectPoint), "protectPoint");
    NM_VALIDATOR(FloatNonNegative(importance), "importance");
    NM_VALIDATOR(FloatValid(imminence), "imminence");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class LegBraceInputs
 * \ingroup LegBrace
 * \brief Inputs reach out towards hazard in order to get legs into the best position to cushion after impact
 *  then cushions the impact
 *
 * Data packet definition (393 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegBraceInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegBraceInputs));
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

  BraceHazard braceHazard;  ///< The description of the incoming hazard
  BodyState protectState;  ///< Pos, vel etc of point to be protected
  NMP::Vector3 cushionPoint;  ///< Centre of support when several legs are cushioning  (Position)
  float swivelAmount;  ///< Refer to the guide poses for the posture at each swivel amount, usually -1 to 1  (Multiplier)
  bool shouldBrace;  ///< Flag so only the closest legs will brace

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
     m_swivelAmountImportance, 
     m_shouldBraceImportance;

  friend class LegBrace_Con;

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
    if (getSwivelAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(swivelAmount), "swivelAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class LegBraceOutputs
 * \ingroup LegBrace
 * \brief Outputs reach out towards hazard in order to get legs into the best position to cushion after impact
 *  then cushions the impact
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegBraceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegBraceOutputs));
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

  LimbControl control;  ///< IK control of the leg

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

  friend class LegBrace_Con;

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
 * \class LegBraceFeedbackInputs
 * \ingroup LegBrace
 * \brief FeedbackInputs reach out towards hazard in order to get legs into the best position to cushion after impact
 *  then cushions the impact
 *
 * Data packet definition (10 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegBraceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegBraceFeedbackInputs));
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

  friend class LegBrace_Con;

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
 * \class LegBraceFeedbackOutputs
 * \ingroup LegBrace
 * \brief FeedbackOutputs reach out towards hazard in order to get legs into the best position to cushion after impact
 *  then cushions the impact
 *
 * Data packet definition (220 bytes, 224 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegBraceFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegBraceFeedbackOutputs));
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

  RotationRequest rootRotationRequest;  ///< To help with bracing, a rotation of the pelvis is requested
  BraceState braceState;  ///< Roughly the proximity of foot to hazard, used to determine which legs to brace with
  NMP::Vector3 endCushionPoint;  ///< Individual foot position, used to calculated averaged cushion point (centre of support)  (Position)

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

  friend class LegBrace_Con;

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

#endif // NM_MDF_LEGBRACE_DATA_H

