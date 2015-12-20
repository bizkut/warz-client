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

#ifndef NM_MDF_HEADPOSE_DATA_H
#define NM_MDF_HEADPOSE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadPose.module"

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
 * \class HeadPoseInputs
 * \ingroup HeadPose
 * \brief Inputs *
 * Data packet definition (264 bytes, 288 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadPoseInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadPoseInputs));
  }

  NM_INLINE void setHeadPoseLowImp(const PoseData& _headPoseLowImp, float headPoseLowImp_importance = 1.f)
  {
    headPoseLowImp = _headPoseLowImp;
    postAssignHeadPoseLowImp(headPoseLowImp);
    NMP_ASSERT(headPoseLowImp_importance >= 0.0f && headPoseLowImp_importance <= 1.0f);
    m_headPoseLowImpImportance = headPoseLowImp_importance;
  }
  NM_INLINE PoseData& startHeadPoseLowImpModification()
  {
    m_headPoseLowImpImportance = -1.0f; // set invalid until stopHeadPoseLowImpModification()
    return headPoseLowImp;
  }
  NM_INLINE void stopHeadPoseLowImpModification(float headPoseLowImp_importance = 1.f)
  {
    postAssignHeadPoseLowImp(headPoseLowImp);
    NMP_ASSERT(headPoseLowImp_importance >= 0.0f && headPoseLowImp_importance <= 1.0f);
    m_headPoseLowImpImportance = headPoseLowImp_importance;
  }
  NM_INLINE float getHeadPoseLowImpImportance() const { return m_headPoseLowImpImportance; }
  NM_INLINE const float& getHeadPoseLowImpImportanceRef() const { return m_headPoseLowImpImportance; }
  NM_INLINE const PoseData& getHeadPoseLowImp() const { return headPoseLowImp; }

  NM_INLINE void setHeadPoseHighImp(const PoseData& _headPoseHighImp, float headPoseHighImp_importance = 1.f)
  {
    headPoseHighImp = _headPoseHighImp;
    postAssignHeadPoseHighImp(headPoseHighImp);
    NMP_ASSERT(headPoseHighImp_importance >= 0.0f && headPoseHighImp_importance <= 1.0f);
    m_headPoseHighImpImportance = headPoseHighImp_importance;
  }
  NM_INLINE PoseData& startHeadPoseHighImpModification()
  {
    m_headPoseHighImpImportance = -1.0f; // set invalid until stopHeadPoseHighImpModification()
    return headPoseHighImp;
  }
  NM_INLINE void stopHeadPoseHighImpModification(float headPoseHighImp_importance = 1.f)
  {
    postAssignHeadPoseHighImp(headPoseHighImp);
    NMP_ASSERT(headPoseHighImp_importance >= 0.0f && headPoseHighImp_importance <= 1.0f);
    m_headPoseHighImpImportance = headPoseHighImp_importance;
  }
  NM_INLINE float getHeadPoseHighImpImportance() const { return m_headPoseHighImpImportance; }
  NM_INLINE const float& getHeadPoseHighImpImportanceRef() const { return m_headPoseHighImpImportance; }
  NM_INLINE const PoseData& getHeadPoseHighImp() const { return headPoseHighImp; }

protected:

  PoseData headPoseLowImp;
  PoseData headPoseHighImp;

  // post-assignment stubs
  NM_INLINE void postAssignHeadPoseLowImp(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadPoseHighImp(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_headPoseLowImpImportance, 
     m_headPoseHighImpImportance;

  friend class HeadPose_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getHeadPoseLowImpImportance() > 0.0f)
    {
      headPoseLowImp.validate();
    }
    if (getHeadPoseHighImpImportance() > 0.0f)
    {
      headPoseHighImp.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HeadPoseOutputs
 * \ingroup HeadPose
 * \brief Outputs *
 * Data packet definition (776 bytes, 800 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadPoseOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadPoseOutputs));
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

  friend class HeadPose_Con;

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

#endif // NM_MDF_HEADPOSE_DATA_H

