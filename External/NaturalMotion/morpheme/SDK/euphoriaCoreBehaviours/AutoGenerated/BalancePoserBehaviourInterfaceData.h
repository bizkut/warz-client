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

#ifndef NM_MDF_BALANCEPOSERBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_BALANCEPOSERBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/BalancePoserBehaviourInterface.module"

// external types
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/BalancePoseParameters.h"

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
 * \class BalancePoserBehaviourInterfaceOutputs
 * \ingroup BalancePoserBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (52 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalancePoserBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalancePoserBehaviourInterfaceOutputs));
  }

  NM_INLINE void setPoseParameters(const BalancePoseParameters& _poseParameters, float poseParameters_importance = 1.f)
  {
    poseParameters = _poseParameters;
    postAssignPoseParameters(poseParameters);
    NMP_ASSERT(poseParameters_importance >= 0.0f && poseParameters_importance <= 1.0f);
    m_poseParametersImportance = poseParameters_importance;
  }
  NM_INLINE float getPoseParametersImportance() const { return m_poseParametersImportance; }
  NM_INLINE const float& getPoseParametersImportanceRef() const { return m_poseParametersImportance; }
  NM_INLINE const BalancePoseParameters& getPoseParameters() const { return poseParameters; }

protected:

  BalancePoseParameters poseParameters;

  // post-assignment stubs
  NM_INLINE void postAssignPoseParameters(const BalancePoseParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_poseParametersImportance;

  friend class BalancePoserBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getPoseParametersImportance() > 0.0f)
    {
      poseParameters.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BalancePoserBehaviourInterfaceFeedbackInputs
 * \ingroup BalancePoserBehaviourInterface
 * \brief FeedbackInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (68 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalancePoserBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalancePoserBehaviourInterfaceFeedbackInputs));
  }

  NM_INLINE void setSupportTM(const ER::LimbTransform& _supportTM, float supportTM_importance = 1.f)
  {
    supportTM = _supportTM;
    NMP_ASSERT(supportTM_importance >= 0.0f && supportTM_importance <= 1.0f);
    m_supportTMImportance = supportTM_importance;
  }
  NM_INLINE ER::LimbTransform& startSupportTMModification()
  {
    m_supportTMImportance = -1.0f; // set invalid until stopSupportTMModification()
    return supportTM;
  }
  NM_INLINE void stopSupportTMModification(float supportTM_importance = 1.f)
  {
    NMP_ASSERT(supportTM_importance >= 0.0f && supportTM_importance <= 1.0f);
    m_supportTMImportance = supportTM_importance;
  }
  NM_INLINE float getSupportTMImportance() const { return m_supportTMImportance; }
  NM_INLINE const float& getSupportTMImportanceRef() const { return m_supportTMImportance; }
  NM_INLINE const ER::LimbTransform& getSupportTM() const { return supportTM; }

protected:

  ER::LimbTransform supportTM;

  // importance values
  float
     m_supportTMImportance;

  friend class BalancePoserBehaviourInterface_Con;

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

#endif // NM_MDF_BALANCEPOSERBEHAVIOURINTERFACE_DATA_H

