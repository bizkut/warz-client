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

#ifndef NM_MDF_SPINEPOSE_DATA_H
#define NM_MDF_SPINEPOSE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/SpinePose.module"

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
 * \class SpinePoseInputs
 * \ingroup SpinePose
 * \brief Inputs *
 * Data packet definition (264 bytes, 288 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpinePoseInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpinePoseInputs));
  }

  NM_INLINE void setSpinePoseLowImp(const PoseData& _spinePoseLowImp, float spinePoseLowImp_importance = 1.f)
  {
    spinePoseLowImp = _spinePoseLowImp;
    postAssignSpinePoseLowImp(spinePoseLowImp);
    NMP_ASSERT(spinePoseLowImp_importance >= 0.0f && spinePoseLowImp_importance <= 1.0f);
    m_spinePoseLowImpImportance = spinePoseLowImp_importance;
  }
  NM_INLINE PoseData& startSpinePoseLowImpModification()
  {
    m_spinePoseLowImpImportance = -1.0f; // set invalid until stopSpinePoseLowImpModification()
    return spinePoseLowImp;
  }
  NM_INLINE void stopSpinePoseLowImpModification(float spinePoseLowImp_importance = 1.f)
  {
    postAssignSpinePoseLowImp(spinePoseLowImp);
    NMP_ASSERT(spinePoseLowImp_importance >= 0.0f && spinePoseLowImp_importance <= 1.0f);
    m_spinePoseLowImpImportance = spinePoseLowImp_importance;
  }
  NM_INLINE float getSpinePoseLowImpImportance() const { return m_spinePoseLowImpImportance; }
  NM_INLINE const float& getSpinePoseLowImpImportanceRef() const { return m_spinePoseLowImpImportance; }
  NM_INLINE const PoseData& getSpinePoseLowImp() const { return spinePoseLowImp; }

  NM_INLINE void setSpinePoseHighImp(const PoseData& _spinePoseHighImp, float spinePoseHighImp_importance = 1.f)
  {
    spinePoseHighImp = _spinePoseHighImp;
    postAssignSpinePoseHighImp(spinePoseHighImp);
    NMP_ASSERT(spinePoseHighImp_importance >= 0.0f && spinePoseHighImp_importance <= 1.0f);
    m_spinePoseHighImpImportance = spinePoseHighImp_importance;
  }
  NM_INLINE PoseData& startSpinePoseHighImpModification()
  {
    m_spinePoseHighImpImportance = -1.0f; // set invalid until stopSpinePoseHighImpModification()
    return spinePoseHighImp;
  }
  NM_INLINE void stopSpinePoseHighImpModification(float spinePoseHighImp_importance = 1.f)
  {
    postAssignSpinePoseHighImp(spinePoseHighImp);
    NMP_ASSERT(spinePoseHighImp_importance >= 0.0f && spinePoseHighImp_importance <= 1.0f);
    m_spinePoseHighImpImportance = spinePoseHighImp_importance;
  }
  NM_INLINE float getSpinePoseHighImpImportance() const { return m_spinePoseHighImpImportance; }
  NM_INLINE const float& getSpinePoseHighImpImportanceRef() const { return m_spinePoseHighImpImportance; }
  NM_INLINE const PoseData& getSpinePoseHighImp() const { return spinePoseHighImp; }

protected:

  PoseData spinePoseLowImp;
  PoseData spinePoseHighImp;

  // post-assignment stubs
  NM_INLINE void postAssignSpinePoseLowImp(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinePoseHighImp(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_spinePoseLowImpImportance, 
     m_spinePoseHighImpImportance;

  friend class SpinePose_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getSpinePoseLowImpImportance() > 0.0f)
    {
      spinePoseLowImp.validate();
    }
    if (getSpinePoseHighImpImportance() > 0.0f)
    {
      spinePoseHighImp.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SpinePoseOutputs
 * \ingroup SpinePose
 * \brief Outputs *
 * Data packet definition (776 bytes, 800 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpinePoseOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpinePoseOutputs));
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

  friend class SpinePose_Con;

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

#endif // NM_MDF_SPINEPOSE_DATA_H

