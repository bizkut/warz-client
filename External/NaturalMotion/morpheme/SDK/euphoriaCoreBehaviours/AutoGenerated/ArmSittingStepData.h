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

#ifndef NM_MDF_ARMSITTINGSTEP_DATA_H
#define NM_MDF_ARMSITTINGSTEP_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmSittingStep.module"

// external types
#include "NMPlatform/NMMatrix34.h"

// known types
#include "Types/SitParameters.h"
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
 * \class ArmSittingStepInputs
 * \ingroup ArmSittingStep
 * \brief Inputs Make the arm move to prepare to support the chest when the pelvis is supported
 *
 * Data packet definition (92 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmSittingStepInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmSittingStepInputs));
  }

  NM_INLINE void setArmPoseEndRelativeToRoot(const NMP::Matrix34& _armPoseEndRelativeToRoot, float armPoseEndRelativeToRoot_importance = 1.f)
  {
    armPoseEndRelativeToRoot = _armPoseEndRelativeToRoot;
    postAssignArmPoseEndRelativeToRoot(armPoseEndRelativeToRoot);
    NMP_ASSERT(armPoseEndRelativeToRoot_importance >= 0.0f && armPoseEndRelativeToRoot_importance <= 1.0f);
    m_armPoseEndRelativeToRootImportance = armPoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getArmPoseEndRelativeToRootImportance() const { return m_armPoseEndRelativeToRootImportance; }
  NM_INLINE const float& getArmPoseEndRelativeToRootImportanceRef() const { return m_armPoseEndRelativeToRootImportance; }
  NM_INLINE const NMP::Matrix34& getArmPoseEndRelativeToRoot() const { return armPoseEndRelativeToRoot; }

  NM_INLINE void setSitParameters(const SitParameters& _sitParameters, float sitParameters_importance = 1.f)
  {
    sitParameters = _sitParameters;
    postAssignSitParameters(sitParameters);
    NMP_ASSERT(sitParameters_importance >= 0.0f && sitParameters_importance <= 1.0f);
    m_sitParametersImportance = sitParameters_importance;
  }
  NM_INLINE float getSitParametersImportance() const { return m_sitParametersImportance; }
  NM_INLINE const float& getSitParametersImportanceRef() const { return m_sitParametersImportance; }
  NM_INLINE const SitParameters& getSitParameters() const { return sitParameters; }

protected:

  NMP::Matrix34 armPoseEndRelativeToRoot;    ///< (Transform)
  SitParameters sitParameters;  ///< Parameters defined by sit behaviour interface.

  // post-assignment stubs
  NM_INLINE void postAssignArmPoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "armPoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSitParameters(const SitParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_armPoseEndRelativeToRootImportance, 
     m_sitParametersImportance;

  friend class ArmSittingStep_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getArmPoseEndRelativeToRootImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(armPoseEndRelativeToRoot), "armPoseEndRelativeToRoot");
    }
    if (getSitParametersImportance() > 0.0f)
    {
      sitParameters.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmSittingStepOutputs
 * \ingroup ArmSittingStep
 * \brief Outputs Make the arm move to prepare to support the chest when the pelvis is supported
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmSittingStepOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmSittingStepOutputs));
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

  friend class ArmSittingStep_Con;

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
 * \class ArmSittingStepFeedbackInputs
 * \ingroup ArmSittingStep
 * \brief FeedbackInputs Make the arm move to prepare to support the chest when the pelvis is supported
 *
 * Data packet definition (8 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmSittingStepFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmSittingStepFeedbackInputs));
  }

  NM_INLINE void setSupportImportance(const float& _supportImportance, float supportImportance_importance = 1.f)
  {
    supportImportance = _supportImportance;
    postAssignSupportImportance(supportImportance);
    NMP_ASSERT(supportImportance_importance >= 0.0f && supportImportance_importance <= 1.0f);
    m_supportImportanceImportance = supportImportance_importance;
  }
  NM_INLINE float getSupportImportanceImportance() const { return m_supportImportanceImportance; }
  NM_INLINE const float& getSupportImportanceImportanceRef() const { return m_supportImportanceImportance; }
  NM_INLINE const float& getSupportImportance() const { return supportImportance; }

protected:

  float supportImportance;  ///< Overall importance of the arms in maintaining a sitting balance.  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignSupportImportance(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "supportImportance");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_supportImportanceImportance;

  friend class ArmSittingStep_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getSupportImportanceImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(supportImportance), "supportImportance");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmSittingStepFeedbackOutputs
 * \ingroup ArmSittingStep
 * \brief FeedbackOutputs Make the arm move to prepare to support the chest when the pelvis is supported
 *
 * Data packet definition (5 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmSittingStepFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmSittingStepFeedbackOutputs));
  }

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

  bool isStepping;

  // importance values
  float
     m_isSteppingImportance;

  friend class ArmSittingStep_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMSITTINGSTEP_DATA_H

