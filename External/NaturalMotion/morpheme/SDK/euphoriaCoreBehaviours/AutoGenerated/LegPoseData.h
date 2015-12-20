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

#ifndef NM_MDF_LEGPOSE_DATA_H
#define NM_MDF_LEGPOSE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/LegPose.module"

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
 * \class LegPoseInputs
 * \ingroup LegPose
 * \brief Inputs *
 * Data packet definition (264 bytes, 288 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegPoseInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegPoseInputs));
  }

  NM_INLINE void setLegPoseLowImp(const PoseData& _legPoseLowImp, float legPoseLowImp_importance = 1.f)
  {
    legPoseLowImp = _legPoseLowImp;
    postAssignLegPoseLowImp(legPoseLowImp);
    NMP_ASSERT(legPoseLowImp_importance >= 0.0f && legPoseLowImp_importance <= 1.0f);
    m_legPoseLowImpImportance = legPoseLowImp_importance;
  }
  NM_INLINE PoseData& startLegPoseLowImpModification()
  {
    m_legPoseLowImpImportance = -1.0f; // set invalid until stopLegPoseLowImpModification()
    return legPoseLowImp;
  }
  NM_INLINE void stopLegPoseLowImpModification(float legPoseLowImp_importance = 1.f)
  {
    postAssignLegPoseLowImp(legPoseLowImp);
    NMP_ASSERT(legPoseLowImp_importance >= 0.0f && legPoseLowImp_importance <= 1.0f);
    m_legPoseLowImpImportance = legPoseLowImp_importance;
  }
  NM_INLINE float getLegPoseLowImpImportance() const { return m_legPoseLowImpImportance; }
  NM_INLINE const float& getLegPoseLowImpImportanceRef() const { return m_legPoseLowImpImportance; }
  NM_INLINE const PoseData& getLegPoseLowImp() const { return legPoseLowImp; }

  NM_INLINE void setLegPoseHighImp(const PoseData& _legPoseHighImp, float legPoseHighImp_importance = 1.f)
  {
    legPoseHighImp = _legPoseHighImp;
    postAssignLegPoseHighImp(legPoseHighImp);
    NMP_ASSERT(legPoseHighImp_importance >= 0.0f && legPoseHighImp_importance <= 1.0f);
    m_legPoseHighImpImportance = legPoseHighImp_importance;
  }
  NM_INLINE PoseData& startLegPoseHighImpModification()
  {
    m_legPoseHighImpImportance = -1.0f; // set invalid until stopLegPoseHighImpModification()
    return legPoseHighImp;
  }
  NM_INLINE void stopLegPoseHighImpModification(float legPoseHighImp_importance = 1.f)
  {
    postAssignLegPoseHighImp(legPoseHighImp);
    NMP_ASSERT(legPoseHighImp_importance >= 0.0f && legPoseHighImp_importance <= 1.0f);
    m_legPoseHighImpImportance = legPoseHighImp_importance;
  }
  NM_INLINE float getLegPoseHighImpImportance() const { return m_legPoseHighImpImportance; }
  NM_INLINE const float& getLegPoseHighImpImportanceRef() const { return m_legPoseHighImpImportance; }
  NM_INLINE const PoseData& getLegPoseHighImp() const { return legPoseHighImp; }

protected:

  PoseData legPoseLowImp;
  PoseData legPoseHighImp;

  // post-assignment stubs
  NM_INLINE void postAssignLegPoseLowImp(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegPoseHighImp(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_legPoseLowImpImportance, 
     m_legPoseHighImpImportance;

  friend class LegPose_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getLegPoseLowImpImportance() > 0.0f)
    {
      legPoseLowImp.validate();
    }
    if (getLegPoseHighImpImportance() > 0.0f)
    {
      legPoseHighImp.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class LegPoseOutputs
 * \ingroup LegPose
 * \brief Outputs *
 * Data packet definition (776 bytes, 800 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegPoseOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegPoseOutputs));
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

  friend class LegPose_Con;

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

#endif // NM_MDF_LEGPOSE_DATA_H

