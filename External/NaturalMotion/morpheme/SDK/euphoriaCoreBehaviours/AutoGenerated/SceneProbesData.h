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

#ifndef NM_MDF_SCENEPROBES_DATA_H
#define NM_MDF_SCENEPROBES_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/SceneProbes.module"

// external types
#include "NMPlatform/NMVector3.h"

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
 * \class SceneProbesData
 * \ingroup SceneProbes
 * \brief Data Takes requests from the EnvironmentAwareness and probes the environment using physics engine level calls, then passes this information
 *  back to the environment awareness module.
 *
 * Data packet definition (1 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SceneProbesData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SceneProbesData));
  }

  enum ProbeFrequency
  {
    /*  4 */ NUM_FRAMES_BETWEEN_PROBES = 4,  
  };

  bool justStarted;  ///< Used for optimising the timing of collision probes

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SceneProbesInputs
 * \ingroup SceneProbes
 * \brief Inputs Takes requests from the EnvironmentAwareness and probes the environment using physics engine level calls, then passes this information
 *  back to the environment awareness module.
 *
 * Data packet definition (28 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SceneProbesInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SceneProbesInputs));
  }

  NM_INLINE void setFocalCentre(const NMP::Vector3& _focalCentre, float focalCentre_importance = 1.f)
  {
    focalCentre = _focalCentre;
    postAssignFocalCentre(focalCentre);
    NMP_ASSERT(focalCentre_importance >= 0.0f && focalCentre_importance <= 1.0f);
    m_focalCentreImportance = focalCentre_importance;
  }
  NM_INLINE float getFocalCentreImportance() const { return m_focalCentreImportance; }
  NM_INLINE const float& getFocalCentreImportanceRef() const { return m_focalCentreImportance; }
  NM_INLINE const NMP::Vector3& getFocalCentre() const { return focalCentre; }

  NM_INLINE void setFocalRadius(const float& _focalRadius, float focalRadius_importance = 1.f)
  {
    focalRadius = _focalRadius;
    postAssignFocalRadius(focalRadius);
    NMP_ASSERT(focalRadius_importance >= 0.0f && focalRadius_importance <= 1.0f);
    m_focalRadiusImportance = focalRadius_importance;
  }
  NM_INLINE float getFocalRadiusImportance() const { return m_focalRadiusImportance; }
  NM_INLINE const float& getFocalRadiusImportanceRef() const { return m_focalRadiusImportance; }
  NM_INLINE const float& getFocalRadius() const { return focalRadius; }

protected:

  NMP::Vector3 focalCentre;                  ///< (Position)
  float focalRadius;                         ///< (Length)

  // post-assignment stubs
  NM_INLINE void postAssignFocalCentre(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "focalCentre");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignFocalRadius(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "focalRadius");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_focalCentreImportance, 
     m_focalRadiusImportance;

  friend class SceneProbes_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getFocalCentreImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(focalCentre), "focalCentre");
    }
    if (getFocalRadiusImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(focalRadius), "focalRadius");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SceneProbesFeedbackOutputs
 * \ingroup SceneProbes
 * \brief FeedbackOutputs Takes requests from the EnvironmentAwareness and probes the environment using physics engine level calls, then passes this information
 *  back to the environment awareness module.
 *
 * Data packet definition (5 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SceneProbesFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SceneProbesFeedbackOutputs));
  }

  NM_INLINE void setRequestNextProbe(const bool& _requestNextProbe, float requestNextProbe_importance = 1.f)
  {
    requestNextProbe = _requestNextProbe;
    NMP_ASSERT(requestNextProbe_importance >= 0.0f && requestNextProbe_importance <= 1.0f);
    m_requestNextProbeImportance = requestNextProbe_importance;
  }
  NM_INLINE float getRequestNextProbeImportance() const { return m_requestNextProbeImportance; }
  NM_INLINE const float& getRequestNextProbeImportanceRef() const { return m_requestNextProbeImportance; }
  NM_INLINE const bool& getRequestNextProbe() const { return requestNextProbe; }

protected:

  bool requestNextProbe;

  // importance values
  float
     m_requestNextProbeImportance;

  friend class SceneProbes_Con;

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

#endif // NM_MDF_SCENEPROBES_DATA_H

