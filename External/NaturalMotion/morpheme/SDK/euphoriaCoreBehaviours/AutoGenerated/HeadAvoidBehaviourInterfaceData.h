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

#ifndef NM_MDF_HEADAVOIDBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_HEADAVOIDBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/HeadAvoidBehaviourInterface.module"

// known types
#include "Types/SpatialTarget.h"

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
 * \class HeadAvoidBehaviourInterfaceOutputs
 * \ingroup HeadAvoidBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (65 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HeadAvoidBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HeadAvoidBehaviourInterfaceOutputs));
  }

  NM_INLINE void setAvoidTarget(const SpatialTarget& _avoidTarget, float avoidTarget_importance = 1.f)
  {
    avoidTarget = _avoidTarget;
    postAssignAvoidTarget(avoidTarget);
    NMP_ASSERT(avoidTarget_importance >= 0.0f && avoidTarget_importance <= 1.0f);
    m_avoidTargetImportance = avoidTarget_importance;
  }
  NM_INLINE float getAvoidTargetImportance() const { return m_avoidTargetImportance; }
  NM_INLINE const float& getAvoidTargetImportanceRef() const { return m_avoidTargetImportance; }
  NM_INLINE const SpatialTarget& getAvoidTarget() const { return avoidTarget; }

  NM_INLINE void setAvoidRadius(const float& _avoidRadius, float avoidRadius_importance = 1.f)
  {
    avoidRadius = _avoidRadius;
    postAssignAvoidRadius(avoidRadius);
    NMP_ASSERT(avoidRadius_importance >= 0.0f && avoidRadius_importance <= 1.0f);
    m_avoidRadiusImportance = avoidRadius_importance;
  }
  NM_INLINE float getAvoidRadiusImportance() const { return m_avoidRadiusImportance; }
  NM_INLINE const float& getAvoidRadiusImportanceRef() const { return m_avoidRadiusImportance; }
  NM_INLINE const float& getAvoidRadius() const { return avoidRadius; }

  NM_INLINE void setTwoSidedTarget(const bool& _twoSidedTarget, float twoSidedTarget_importance = 1.f)
  {
    twoSidedTarget = _twoSidedTarget;
    NMP_ASSERT(twoSidedTarget_importance >= 0.0f && twoSidedTarget_importance <= 1.0f);
    m_twoSidedTargetImportance = twoSidedTarget_importance;
  }
  NM_INLINE float getTwoSidedTargetImportance() const { return m_twoSidedTargetImportance; }
  NM_INLINE const float& getTwoSidedTargetImportanceRef() const { return m_twoSidedTargetImportance; }
  NM_INLINE const bool& getTwoSidedTarget() const { return twoSidedTarget; }

protected:

  SpatialTarget avoidTarget;  ///< choice of output defines whether it has priority or not when competing
  float avoidRadius;  ///< Max radius at which to avoid, equilibrium will be less than this radius   (Length)
  bool twoSidedTarget;  ///< Since we avoid a plane, the plane can optionally be two-sided

  // post-assignment stubs
  NM_INLINE void postAssignAvoidTarget(const SpatialTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAvoidRadius(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "avoidRadius");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_avoidTargetImportance, 
     m_avoidRadiusImportance, 
     m_twoSidedTargetImportance;


public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getAvoidTargetImportance() > 0.0f)
    {
      avoidTarget.validate();
    }
    if (getAvoidRadiusImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(avoidRadius), "avoidRadius");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_HEADAVOIDBEHAVIOURINTERFACE_DATA_H

