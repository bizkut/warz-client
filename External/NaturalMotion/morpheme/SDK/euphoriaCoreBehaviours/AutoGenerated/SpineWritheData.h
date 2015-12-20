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

#ifndef NM_MDF_SPINEWRITHE_DATA_H
#define NM_MDF_SPINEWRITHE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/SpineWrithe.module"

// external types
#include "NMPlatform/NMMatrix34.h"

// known types
#include "Types/ModuleRNG.h"
#include "Types/WritheParameters.h"
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
 * \class SpineWritheData
 * \ingroup SpineWrithe
 * \brief Data Will generate some random movements based on writheParams.
 *
 * Data packet definition (140 bytes, 160 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineWritheData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineWritheData));
  }

  NMP::Matrix34 targetTM;                    ///< (Transform)
  ModuleRNG rng;
  float lastResetTimer;                      ///< (TimePeriod)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(targetTM), "targetTM");
    NM_VALIDATOR(FloatValid(lastResetTimer), "lastResetTimer");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SpineWritheInputs
 * \ingroup SpineWrithe
 * \brief Inputs Will generate some random movements based on writheParams.
 *
 * Data packet definition (100 bytes, 128 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineWritheInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineWritheInputs));
  }

  NM_INLINE void setWritheParams(const WritheParameters& _writheParams, float writheParams_importance = 1.f)
  {
    writheParams = _writheParams;
    postAssignWritheParams(writheParams);
    NMP_ASSERT(writheParams_importance >= 0.0f && writheParams_importance <= 1.0f);
    m_writheParamsImportance = writheParams_importance;
  }
  NM_INLINE WritheParameters& startWritheParamsModification()
  {
    m_writheParamsImportance = -1.0f; // set invalid until stopWritheParamsModification()
    return writheParams;
  }
  NM_INLINE void stopWritheParamsModification(float writheParams_importance = 1.f)
  {
    postAssignWritheParams(writheParams);
    NMP_ASSERT(writheParams_importance >= 0.0f && writheParams_importance <= 1.0f);
    m_writheParamsImportance = writheParams_importance;
  }
  NM_INLINE float getWritheParamsImportance() const { return m_writheParamsImportance; }
  NM_INLINE const float& getWritheParamsImportanceRef() const { return m_writheParamsImportance; }
  NM_INLINE const WritheParameters& getWritheParams() const { return writheParams; }

protected:

  WritheParameters writheParams;

  // post-assignment stubs
  NM_INLINE void postAssignWritheParams(const WritheParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_writheParamsImportance;

  friend class SpineWrithe_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getWritheParamsImportance() > 0.0f)
    {
      writheParams.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SpineWritheOutputs
 * \ingroup SpineWrithe
 * \brief Outputs Will generate some random movements based on writheParams.
 *
 * Data packet definition (388 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineWritheOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineWritheOutputs));
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

  friend class SpineWrithe_Con;

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

#endif // NM_MDF_SPINEWRITHE_DATA_H

