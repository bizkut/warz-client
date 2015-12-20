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

#ifndef NM_MDF_HEADSUPPORT_DATA_H
#define NM_MDF_HEADSUPPORT_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/HeadSupport.module"

// external types
#include "NMPlatform/NMQuat.h"

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
 * \class HeadSupportInputs
 * \ingroup HeadSupport
 * \brief Inputs keep the head supported and lifted above the chest in a comfortable posture
 *
 * Data packet definition (20 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadSupportInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadSupportInputs));
  }

  NM_INLINE void setDesiredHeadRelChestOrientation(const NMP::Quat& _desiredHeadRelChestOrientation, float desiredHeadRelChestOrientation_importance = 1.f)
  {
    desiredHeadRelChestOrientation = _desiredHeadRelChestOrientation;
    postAssignDesiredHeadRelChestOrientation(desiredHeadRelChestOrientation);
    NMP_ASSERT(desiredHeadRelChestOrientation_importance >= 0.0f && desiredHeadRelChestOrientation_importance <= 1.0f);
    m_desiredHeadRelChestOrientationImportance = desiredHeadRelChestOrientation_importance;
  }
  NM_INLINE float getDesiredHeadRelChestOrientationImportance() const { return m_desiredHeadRelChestOrientationImportance; }
  NM_INLINE const float& getDesiredHeadRelChestOrientationImportanceRef() const { return m_desiredHeadRelChestOrientationImportance; }
  NM_INLINE const NMP::Quat& getDesiredHeadRelChestOrientation() const { return desiredHeadRelChestOrientation; }

protected:

  NMP::Quat desiredHeadRelChestOrientation;  /// Desired rotation of the end of the head relative to the root (in root's space)  (OrientationDelta)

  // post-assignment stubs
  NM_INLINE void postAssignDesiredHeadRelChestOrientation(const NMP::Quat& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(QuatNormalised(v), "desiredHeadRelChestOrientation");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_desiredHeadRelChestOrientationImportance;

  friend class HeadSupport_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getDesiredHeadRelChestOrientationImportance() > 0.0f)
    {
      NM_VALIDATOR(QuatNormalised(desiredHeadRelChestOrientation), "desiredHeadRelChestOrientation");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HeadSupportOutputs
 * \ingroup HeadSupport
 * \brief Outputs keep the head supported and lifted above the chest in a comfortable posture
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadSupportOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadSupportOutputs));
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

  friend class HeadSupport_Con;

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

#endif // NM_MDF_HEADSUPPORT_DATA_H

