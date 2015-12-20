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

#ifndef NM_MDF_ARMSWING_DATA_H
#define NM_MDF_ARMSWING_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ArmSwing.module"

// known types
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
 * \class ArmSwingInputs
 * \ingroup ArmSwing
 * \brief Inputs attempt to adjust the angle of the body by accelerating the swing of the arms in the opposite angular direction
 *
 * Data packet definition (183 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmSwingInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmSwingInputs));
  }

  NM_INLINE void setRotationRequest(const RotationRequest& _rotationRequest, float rotationRequest_importance = 1.f)
  {
    rotationRequest = _rotationRequest;
    postAssignRotationRequest(rotationRequest);
    NMP_ASSERT(rotationRequest_importance >= 0.0f && rotationRequest_importance <= 1.0f);
    m_rotationRequestImportance = rotationRequest_importance;
  }
  NM_INLINE RotationRequest& startRotationRequestModification()
  {
    m_rotationRequestImportance = -1.0f; // set invalid until stopRotationRequestModification()
    return rotationRequest;
  }
  NM_INLINE void stopRotationRequestModification(float rotationRequest_importance = 1.f)
  {
    postAssignRotationRequest(rotationRequest);
    NMP_ASSERT(rotationRequest_importance >= 0.0f && rotationRequest_importance <= 1.0f);
    m_rotationRequestImportance = rotationRequest_importance;
  }
  NM_INLINE float getRotationRequestImportance() const { return m_rotationRequestImportance; }
  NM_INLINE const float& getRotationRequestImportanceRef() const { return m_rotationRequestImportance; }
  NM_INLINE const RotationRequest& getRotationRequest() const { return rotationRequest; }

  NM_INLINE void setSwingStiffnessScale(const float& _swingStiffnessScale, float swingStiffnessScale_importance = 1.f)
  {
    swingStiffnessScale = _swingStiffnessScale;
    postAssignSwingStiffnessScale(swingStiffnessScale);
    NMP_ASSERT(swingStiffnessScale_importance >= 0.0f && swingStiffnessScale_importance <= 1.0f);
    m_swingStiffnessScaleImportance = swingStiffnessScale_importance;
  }
  NM_INLINE float getSwingStiffnessScaleImportance() const { return m_swingStiffnessScaleImportance; }
  NM_INLINE const float& getSwingStiffnessScaleImportanceRef() const { return m_swingStiffnessScaleImportance; }
  NM_INLINE const float& getSwingStiffnessScale() const { return swingStiffnessScale; }

  NM_INLINE void setNumConstrainedArms(const uint16_t& _numConstrainedArms, float numConstrainedArms_importance = 1.f)
  {
    numConstrainedArms = _numConstrainedArms;
    NMP_ASSERT(numConstrainedArms_importance >= 0.0f && numConstrainedArms_importance <= 1.0f);
    m_numConstrainedArmsImportance = numConstrainedArms_importance;
  }
  NM_INLINE float getNumConstrainedArmsImportance() const { return m_numConstrainedArmsImportance; }
  NM_INLINE const float& getNumConstrainedArmsImportanceRef() const { return m_numConstrainedArmsImportance; }
  NM_INLINE const uint16_t& getNumConstrainedArms() const { return numConstrainedArms; }

  NM_INLINE void setArmsOutwardsOnly(const bool& _armsOutwardsOnly, float armsOutwardsOnly_importance = 1.f)
  {
    armsOutwardsOnly = _armsOutwardsOnly;
    NMP_ASSERT(armsOutwardsOnly_importance >= 0.0f && armsOutwardsOnly_importance <= 1.0f);
    m_armsOutwardsOnlyImportance = armsOutwardsOnly_importance;
  }
  NM_INLINE float getArmsOutwardsOnlyImportance() const { return m_armsOutwardsOnlyImportance; }
  NM_INLINE const float& getArmsOutwardsOnlyImportanceRef() const { return m_armsOutwardsOnlyImportance; }
  NM_INLINE const bool& getArmsOutwardsOnly() const { return armsOutwardsOnly; }

protected:

  RotationRequest rotationRequest;  ///< Requests rotation from the chest
  float swingStiffnessScale;  ///< Makes the arms stiffer / faster  (Multiplier)
  uint16_t numConstrainedArms;  ///< Number of arms with pin constraints. Prevents swinging when arms are constrained
  bool armsOutwardsOnly;  ///< Doesn't try and windmill arms to cross to inside of body

  // post-assignment stubs
  NM_INLINE void postAssignRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSwingStiffnessScale(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "swingStiffnessScale");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rotationRequestImportance, 
     m_swingStiffnessScaleImportance, 
     m_numConstrainedArmsImportance, 
     m_armsOutwardsOnlyImportance;

  friend class ArmSwing_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getRotationRequestImportance() > 0.0f)
    {
      rotationRequest.validate();
    }
    if (getSwingStiffnessScaleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(swingStiffnessScale), "swingStiffnessScale");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ArmSwingOutputs
 * \ingroup ArmSwing
 * \brief Outputs attempt to adjust the angle of the body by accelerating the swing of the arms in the opposite angular direction
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ArmSwingOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ArmSwingOutputs));
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

  LimbControl control;  ///< IK control of arm

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

  friend class ArmSwing_Con;

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

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_ARMSWING_DATA_H

