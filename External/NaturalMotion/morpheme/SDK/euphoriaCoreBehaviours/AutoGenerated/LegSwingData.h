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

#ifndef NM_MDF_LEGSWING_DATA_H
#define NM_MDF_LEGSWING_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/LegSwing.module"

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
 * \class LegSwingData
 * \ingroup LegSwing
 * \brief Data attempt to adjust the angle of the body by accelerating the swing of the legs in the opposite angular direction
 *
 * Data packet definition (5 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegSwingData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegSwingData));
  }

  float timeLegsOrSpineCollided;  ///< Used to turn swing off after having collided too long.  (TimePeriod)
  uint8_t chestContactInfoHysteresis;  /// Buffer for frame-by-frame chest collision registration (0-no collision,1-collision).

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(timeLegsOrSpineCollided), "timeLegsOrSpineCollided");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class LegSwingInputs
 * \ingroup LegSwing
 * \brief Inputs attempt to adjust the angle of the body by accelerating the swing of the legs in the opposite angular direction
 *
 * Data packet definition (185 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegSwingInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegSwingInputs));
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

  NM_INLINE void setMaxSwingTimeOnGround(const float& _maxSwingTimeOnGround, float maxSwingTimeOnGround_importance = 1.f)
  {
    maxSwingTimeOnGround = _maxSwingTimeOnGround;
    postAssignMaxSwingTimeOnGround(maxSwingTimeOnGround);
    NMP_ASSERT(maxSwingTimeOnGround_importance >= 0.0f && maxSwingTimeOnGround_importance <= 1.0f);
    m_maxSwingTimeOnGroundImportance = maxSwingTimeOnGround_importance;
  }
  NM_INLINE float getMaxSwingTimeOnGroundImportance() const { return m_maxSwingTimeOnGroundImportance; }
  NM_INLINE const float& getMaxSwingTimeOnGroundImportanceRef() const { return m_maxSwingTimeOnGroundImportance; }
  NM_INLINE const float& getMaxSwingTimeOnGround() const { return maxSwingTimeOnGround; }

  NM_INLINE void setSpineInContact(const bool& _spineInContact, float spineInContact_importance = 1.f)
  {
    spineInContact = _spineInContact;
    NMP_ASSERT(spineInContact_importance >= 0.0f && spineInContact_importance <= 1.0f);
    m_spineInContactImportance = spineInContact_importance;
  }
  NM_INLINE float getSpineInContactImportance() const { return m_spineInContactImportance; }
  NM_INLINE const float& getSpineInContactImportanceRef() const { return m_spineInContactImportance; }
  NM_INLINE const bool& getSpineInContact() const { return spineInContact; }

protected:

  RotationRequest rotationRequest;  ///< Specifies how to try and rotate the pelvis, and how urgently.
  float swingStiffnessScale;  ///< 1 is normal stiffness.  (Multiplier)
  float maxSwingTimeOnGround;  ///< Max time before disabling the swing when on ground.  (TimePeriod)
  bool spineInContact;  ///< Spine contacting external object.

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

  NM_INLINE void postAssignMaxSwingTimeOnGround(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "maxSwingTimeOnGround");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_rotationRequestImportance, 
     m_swingStiffnessScaleImportance, 
     m_maxSwingTimeOnGroundImportance, 
     m_spineInContactImportance;

  friend class LegSwing_Con;

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
    if (getMaxSwingTimeOnGroundImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(maxSwingTimeOnGround), "maxSwingTimeOnGround");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class LegSwingOutputs
 * \ingroup LegSwing
 * \brief Outputs attempt to adjust the angle of the body by accelerating the swing of the legs in the opposite angular direction
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegSwingOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegSwingOutputs));
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

  LimbControl control;  ///< IK control of leg.

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

  friend class LegSwing_Con;

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

#endif // NM_MDF_LEGSWING_DATA_H

