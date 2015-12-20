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

#ifndef NM_MDF_ARMPOSE_DATA_H
#define NM_MDF_ARMPOSE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmPose.module"

// known types
#include "Types/PoseData.h"
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
 * \class ArmPoseInputs
 * \ingroup ArmPose
 * \brief Inputs *
 * Data packet definition (264 bytes, 288 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmPoseInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmPoseInputs));
  }

  NM_INLINE void setArmPoseLowImp(const PoseData& _armPoseLowImp, float armPoseLowImp_importance = 1.f)
  {
    armPoseLowImp = _armPoseLowImp;
    postAssignArmPoseLowImp(armPoseLowImp);
    NMP_ASSERT(armPoseLowImp_importance >= 0.0f && armPoseLowImp_importance <= 1.0f);
    m_armPoseLowImpImportance = armPoseLowImp_importance;
  }
  NM_INLINE PoseData& startArmPoseLowImpModification()
  {
    m_armPoseLowImpImportance = -1.0f; // set invalid until stopArmPoseLowImpModification()
    return armPoseLowImp;
  }
  NM_INLINE void stopArmPoseLowImpModification(float armPoseLowImp_importance = 1.f)
  {
    postAssignArmPoseLowImp(armPoseLowImp);
    NMP_ASSERT(armPoseLowImp_importance >= 0.0f && armPoseLowImp_importance <= 1.0f);
    m_armPoseLowImpImportance = armPoseLowImp_importance;
  }
  NM_INLINE float getArmPoseLowImpImportance() const { return m_armPoseLowImpImportance; }
  NM_INLINE const float& getArmPoseLowImpImportanceRef() const { return m_armPoseLowImpImportance; }
  NM_INLINE const PoseData& getArmPoseLowImp() const { return armPoseLowImp; }

  NM_INLINE void setArmPoseHighImp(const PoseData& _armPoseHighImp, float armPoseHighImp_importance = 1.f)
  {
    armPoseHighImp = _armPoseHighImp;
    postAssignArmPoseHighImp(armPoseHighImp);
    NMP_ASSERT(armPoseHighImp_importance >= 0.0f && armPoseHighImp_importance <= 1.0f);
    m_armPoseHighImpImportance = armPoseHighImp_importance;
  }
  NM_INLINE PoseData& startArmPoseHighImpModification()
  {
    m_armPoseHighImpImportance = -1.0f; // set invalid until stopArmPoseHighImpModification()
    return armPoseHighImp;
  }
  NM_INLINE void stopArmPoseHighImpModification(float armPoseHighImp_importance = 1.f)
  {
    postAssignArmPoseHighImp(armPoseHighImp);
    NMP_ASSERT(armPoseHighImp_importance >= 0.0f && armPoseHighImp_importance <= 1.0f);
    m_armPoseHighImpImportance = armPoseHighImp_importance;
  }
  NM_INLINE float getArmPoseHighImpImportance() const { return m_armPoseHighImpImportance; }
  NM_INLINE const float& getArmPoseHighImpImportanceRef() const { return m_armPoseHighImpImportance; }
  NM_INLINE const PoseData& getArmPoseHighImp() const { return armPoseHighImp; }

protected:

  PoseData armPoseLowImp;
  PoseData armPoseHighImp;

  // post-assignment stubs
  NM_INLINE void postAssignArmPoseLowImp(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmPoseHighImp(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_armPoseLowImpImportance, 
     m_armPoseHighImpImportance;

  friend class ArmPose_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getArmPoseLowImpImportance() > 0.0f)
    {
      armPoseLowImp.validate();
    }
    if (getArmPoseHighImpImportance() > 0.0f)
    {
      armPoseHighImp.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmPoseOutputs
 * \ingroup ArmPose
 * \brief Outputs *
 * Data packet definition (776 bytes, 800 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmPoseOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmPoseOutputs));
  }

  NM_INLINE void setControlLowImp(const LimbControl& _controlLowImp, float controlLowImp_importance = 1.f)
  {
    controlLowImp = _controlLowImp;
    postAssignControlLowImp(controlLowImp);
    NMP_ASSERT(controlLowImp_importance >= 0.0f && controlLowImp_importance <= 1.0f);
    m_controlLowImpImportance = controlLowImp_importance;
  }
  NM_INLINE LimbControl& startControlLowImpModification()
  {
    m_controlLowImpImportance = -1.0f; // set invalid until stopControlLowImpModification()
    return controlLowImp;
  }
  NM_INLINE void stopControlLowImpModification(float controlLowImp_importance = 1.f)
  {
    postAssignControlLowImp(controlLowImp);
    NMP_ASSERT(controlLowImp_importance >= 0.0f && controlLowImp_importance <= 1.0f);
    m_controlLowImpImportance = controlLowImp_importance;
  }
  NM_INLINE float getControlLowImpImportance() const { return m_controlLowImpImportance; }
  NM_INLINE const float& getControlLowImpImportanceRef() const { return m_controlLowImpImportance; }
  NM_INLINE const LimbControl& getControlLowImp() const { return controlLowImp; }

  NM_INLINE void setControlHighImp(const LimbControl& _controlHighImp, float controlHighImp_importance = 1.f)
  {
    controlHighImp = _controlHighImp;
    postAssignControlHighImp(controlHighImp);
    NMP_ASSERT(controlHighImp_importance >= 0.0f && controlHighImp_importance <= 1.0f);
    m_controlHighImpImportance = controlHighImp_importance;
  }
  NM_INLINE LimbControl& startControlHighImpModification()
  {
    m_controlHighImpImportance = -1.0f; // set invalid until stopControlHighImpModification()
    return controlHighImp;
  }
  NM_INLINE void stopControlHighImpModification(float controlHighImp_importance = 1.f)
  {
    postAssignControlHighImp(controlHighImp);
    NMP_ASSERT(controlHighImp_importance >= 0.0f && controlHighImp_importance <= 1.0f);
    m_controlHighImpImportance = controlHighImp_importance;
  }
  NM_INLINE float getControlHighImpImportance() const { return m_controlHighImpImportance; }
  NM_INLINE const float& getControlHighImpImportanceRef() const { return m_controlHighImpImportance; }
  NM_INLINE const LimbControl& getControlHighImp() const { return controlHighImp; }

protected:

  LimbControl controlLowImp;
  LimbControl controlHighImp;

  // post-assignment stubs
  NM_INLINE void postAssignControlLowImp(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignControlHighImp(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_controlLowImpImportance, 
     m_controlHighImpImportance;

  friend class ArmPose_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getControlLowImpImportance() > 0.0f)
    {
      controlLowImp.validate();
    }
    if (getControlHighImpImportance() > 0.0f)
    {
      controlHighImp.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMPOSE_DATA_H

