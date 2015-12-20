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

#ifndef NM_MDF_LEGSITTINGSUPPORT_DATA_H
#define NM_MDF_LEGSITTINGSUPPORT_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/LegSittingSupport.module"

// external types
#include "euphoria/erLimbTransforms.h"
#include "NMPlatform/NMMatrix34.h"
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
 * \class LegSittingSupportData
 * \ingroup LegSittingSupport
 * \brief Data Drive the orientation of the pelvis with the legs without attempting to maintain the pelvis position. The position of
 *  the feet relative to the pelvis is driven gently toward the supplied pose but the priority is to drive the feet 
 *  position in whatever way is most useful for maintaining the desired pelvis orientation. This module assumes that the 
 *  pelvis is intended to be in contact with a supporting surface.
 *
 * Data packet definition (5 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegSittingSupportData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegSittingSupportData));
  }

  float endOffGroundAmount;  ///< Continuously varying parameter indicating time out of contact.
  bool hasFeedbackRun;  /// Used to make support more keen to claim that it is supporting on the first update/feedback - so 
  /// that there isn't one frame of unsupported (which would result in other behaviours trying to run)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(endOffGroundAmount), "endOffGroundAmount");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class LegSittingSupportInputs
 * \ingroup LegSittingSupport
 * \brief Inputs Drive the orientation of the pelvis with the legs without attempting to maintain the pelvis position. The position of
 *  the feet relative to the pelvis is driven gently toward the supplied pose but the priority is to drive the feet 
 *  position in whatever way is most useful for maintaining the desired pelvis orientation. This module assumes that the 
 *  pelvis is intended to be in contact with a supporting surface.
 *
 * Data packet definition (144 bytes, 160 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegSittingSupportInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegSittingSupportInputs));
  }

  NM_INLINE void setRootDesiredTM(const ER::LimbTransform& _rootDesiredTM, float rootDesiredTM_importance = 1.f)
  {
    rootDesiredTM = _rootDesiredTM;
    NMP_ASSERT(rootDesiredTM_importance >= 0.0f && rootDesiredTM_importance <= 1.0f);
    m_rootDesiredTMImportance = rootDesiredTM_importance;
  }
  NM_INLINE ER::LimbTransform& startRootDesiredTMModification()
  {
    m_rootDesiredTMImportance = -1.0f; // set invalid until stopRootDesiredTMModification()
    return rootDesiredTM;
  }
  NM_INLINE void stopRootDesiredTMModification(float rootDesiredTM_importance = 1.f)
  {
    NMP_ASSERT(rootDesiredTM_importance >= 0.0f && rootDesiredTM_importance <= 1.0f);
    m_rootDesiredTMImportance = rootDesiredTM_importance;
  }
  NM_INLINE float getRootDesiredTMImportance() const { return m_rootDesiredTMImportance; }
  NM_INLINE const float& getRootDesiredTMImportanceRef() const { return m_rootDesiredTMImportance; }
  NM_INLINE const ER::LimbTransform& getRootDesiredTM() const { return rootDesiredTM; }

  NM_INLINE void setBalancePoseEndRelativeToRoot(const NMP::Matrix34& _balancePoseEndRelativeToRoot, float balancePoseEndRelativeToRoot_importance = 1.f)
  {
    balancePoseEndRelativeToRoot = _balancePoseEndRelativeToRoot;
    postAssignBalancePoseEndRelativeToRoot(balancePoseEndRelativeToRoot);
    NMP_ASSERT(balancePoseEndRelativeToRoot_importance >= 0.0f && balancePoseEndRelativeToRoot_importance <= 1.0f);
    m_balancePoseEndRelativeToRootImportance = balancePoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getBalancePoseEndRelativeToRootImportance() const { return m_balancePoseEndRelativeToRootImportance; }
  NM_INLINE const float& getBalancePoseEndRelativeToRootImportanceRef() const { return m_balancePoseEndRelativeToRootImportance; }
  NM_INLINE const NMP::Matrix34& getBalancePoseEndRelativeToRoot() const { return balancePoseEndRelativeToRoot; }

  NM_INLINE void setSupportStrengthScale(const float& _supportStrengthScale, float supportStrengthScale_importance = 1.f)
  {
    supportStrengthScale = _supportStrengthScale;
    postAssignSupportStrengthScale(supportStrengthScale);
    NMP_ASSERT(supportStrengthScale_importance >= 0.0f && supportStrengthScale_importance <= 1.0f);
    m_supportStrengthScaleImportance = supportStrengthScale_importance;
  }
  NM_INLINE float getSupportStrengthScaleImportance() const { return m_supportStrengthScaleImportance; }
  NM_INLINE const float& getSupportStrengthScaleImportanceRef() const { return m_supportStrengthScaleImportance; }
  NM_INLINE const float& getSupportStrengthScale() const { return supportStrengthScale; }

protected:

  ER::LimbTransform rootDesiredTM;  ///< Desired transform (world space) for the root of the limb
  NMP::Matrix34 balancePoseEndRelativeToRoot;  ///< Balance pose  (Transform)
  float supportStrengthScale;  /// Scaling on the strengths so that modules coordinating support etc can indicate if the 
  /// actions need to be stronger/weaker depending on how many limbs are acting.  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignBalancePoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "balancePoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSupportStrengthScale(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "supportStrengthScale");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rootDesiredTMImportance, 
     m_balancePoseEndRelativeToRootImportance, 
     m_supportStrengthScaleImportance;

  friend class LegSittingSupport_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getBalancePoseEndRelativeToRootImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(balancePoseEndRelativeToRoot), "balancePoseEndRelativeToRoot");
    }
    if (getSupportStrengthScaleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(supportStrengthScale), "supportStrengthScale");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class LegSittingSupportOutputs
 * \ingroup LegSittingSupport
 * \brief Outputs Drive the orientation of the pelvis with the legs without attempting to maintain the pelvis position. The position of
 *  the feet relative to the pelvis is driven gently toward the supplied pose but the priority is to drive the feet 
 *  position in whatever way is most useful for maintaining the desired pelvis orientation. This module assumes that the 
 *  pelvis is intended to be in contact with a supporting surface.
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegSittingSupportOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegSittingSupportOutputs));
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

  friend class LegSittingSupport_Con;

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
 * \class LegSittingSupportFeedbackOutputs
 * \ingroup LegSittingSupport
 * \brief FeedbackOutputs Drive the orientation of the pelvis with the legs without attempting to maintain the pelvis position. The position of
 *  the feet relative to the pelvis is driven gently toward the supplied pose but the priority is to drive the feet 
 *  position in whatever way is most useful for maintaining the desired pelvis orientation. This module assumes that the 
 *  pelvis is intended to be in contact with a supporting surface.
 *
 * Data packet definition (28 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegSittingSupportFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegSittingSupportFeedbackOutputs));
  }

  NM_INLINE void setEndSupportPoint(const NMP::Vector3& _endSupportPoint, float endSupportPoint_importance = 1.f)
  {
    endSupportPoint = _endSupportPoint;
    postAssignEndSupportPoint(endSupportPoint);
    NMP_ASSERT(endSupportPoint_importance >= 0.0f && endSupportPoint_importance <= 1.0f);
    m_endSupportPointImportance = endSupportPoint_importance;
  }
  NM_INLINE float getEndSupportPointImportance() const { return m_endSupportPointImportance; }
  NM_INLINE const float& getEndSupportPointImportanceRef() const { return m_endSupportPointImportance; }
  NM_INLINE const NMP::Vector3& getEndSupportPoint() const { return endSupportPoint; }

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

  NMP::Vector3 endSupportPoint;  ///< Position of the end effector.  (Position)
  float supportAmount;  ///< How much the support is operating, 1 if support is operating, even if it has only loose control of the root.  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignEndSupportPoint(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "endSupportPoint");
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
     m_endSupportPointImportance, 
     m_supportAmountImportance;

  friend class LegSittingSupport_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getEndSupportPointImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(endSupportPoint), "endSupportPoint");
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

#endif // NM_MDF_LEGSITTINGSUPPORT_DATA_H

