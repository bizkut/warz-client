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

#ifndef NM_MDF_OBSERVEBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_OBSERVEBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ObserveBehaviourInterface.module"

// known types
#include "Types/ObjectMetric.h"
#include "Types/TargetRequest.h"
#include "Types/Environment_State.h"

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
 * \class ObserveBehaviourInterfaceData
 * \ingroup ObserveBehaviourInterface
 * \brief Data Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (80 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ObserveBehaviourInterfaceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ObserveBehaviourInterfaceData));
  }

  ObjectMetric findObject;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    findObject.validate();
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ObserveBehaviourInterfaceOutputs
 * \ingroup ObserveBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (248 bytes, 256 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ObserveBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ObserveBehaviourInterfaceOutputs));
  }

  NM_INLINE void setFocalCentre(const TargetRequest& _focalCentre, float focalCentre_importance = 1.f)
  {
    focalCentre = _focalCentre;
    postAssignFocalCentre(focalCentre);
    NMP_ASSERT(focalCentre_importance >= 0.0f && focalCentre_importance <= 1.0f);
    m_focalCentreImportance = focalCentre_importance;
  }
  NM_INLINE TargetRequest& startFocalCentreModification()
  {
    m_focalCentreImportance = -1.0f; // set invalid until stopFocalCentreModification()
    return focalCentre;
  }
  NM_INLINE void stopFocalCentreModification(float focalCentre_importance = 1.f)
  {
    postAssignFocalCentre(focalCentre);
    NMP_ASSERT(focalCentre_importance >= 0.0f && focalCentre_importance <= 1.0f);
    m_focalCentreImportance = focalCentre_importance;
  }
  NM_INLINE float getFocalCentreImportance() const { return m_focalCentreImportance; }
  NM_INLINE const float& getFocalCentreImportanceRef() const { return m_focalCentreImportance; }
  NM_INLINE const TargetRequest& getFocalCentre() const { return focalCentre; }

  NM_INLINE void setFindObject(const ObjectMetric& _findObject, float findObject_importance = 1.f)
  {
    findObject = _findObject;
    postAssignFindObject(findObject);
    NMP_ASSERT(findObject_importance >= 0.0f && findObject_importance <= 1.0f);
    m_findObjectImportance = findObject_importance;
  }
  NM_INLINE ObjectMetric& startFindObjectModification()
  {
    m_findObjectImportance = -1.0f; // set invalid until stopFindObjectModification()
    return findObject;
  }
  NM_INLINE void stopFindObjectModification(float findObject_importance = 1.f)
  {
    postAssignFindObject(findObject);
    NMP_ASSERT(findObject_importance >= 0.0f && findObject_importance <= 1.0f);
    m_findObjectImportance = findObject_importance;
  }
  NM_INLINE float getFindObjectImportance() const { return m_findObjectImportance; }
  NM_INLINE const float& getFindObjectImportanceRef() const { return m_findObjectImportance; }
  NM_INLINE const ObjectMetric& getFindObject() const { return findObject; }

protected:

  TargetRequest focalCentre;
  ObjectMetric findObject;

  // post-assignment stubs
  NM_INLINE void postAssignFocalCentre(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignFindObject(const ObjectMetric& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_focalCentreImportance, 
     m_findObjectImportance;

  friend class ObserveBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getFocalCentreImportance() > 0.0f)
    {
      focalCentre.validate();
    }
    if (getFindObjectImportance() > 0.0f)
    {
      findObject.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ObserveBehaviourInterfaceFeedbackInputs
 * \ingroup ObserveBehaviourInterface
 * \brief FeedbackInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (140 bytes, 160 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ObserveBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ObserveBehaviourInterfaceFeedbackInputs));
  }

  NM_INLINE void setFoundObject(const Environment::State& _foundObject, float foundObject_importance = 1.f)
  {
    foundObject = _foundObject;
    postAssignFoundObject(foundObject);
    NMP_ASSERT(foundObject_importance >= 0.0f && foundObject_importance <= 1.0f);
    m_foundObjectImportance = foundObject_importance;
  }
  NM_INLINE Environment::State& startFoundObjectModification()
  {
    m_foundObjectImportance = -1.0f; // set invalid until stopFoundObjectModification()
    return foundObject;
  }
  NM_INLINE void stopFoundObjectModification(float foundObject_importance = 1.f)
  {
    postAssignFoundObject(foundObject);
    NMP_ASSERT(foundObject_importance >= 0.0f && foundObject_importance <= 1.0f);
    m_foundObjectImportance = foundObject_importance;
  }
  NM_INLINE float getFoundObjectImportance() const { return m_foundObjectImportance; }
  NM_INLINE const float& getFoundObjectImportanceRef() const { return m_foundObjectImportance; }
  NM_INLINE const Environment::State& getFoundObject() const { return foundObject; }

  NM_INLINE void setFoundObjectMetric(const float& _foundObjectMetric, float foundObjectMetric_importance = 1.f)
  {
    foundObjectMetric = _foundObjectMetric;
    postAssignFoundObjectMetric(foundObjectMetric);
    NMP_ASSERT(foundObjectMetric_importance >= 0.0f && foundObjectMetric_importance <= 1.0f);
    m_foundObjectMetricImportance = foundObjectMetric_importance;
  }
  NM_INLINE float getFoundObjectMetricImportance() const { return m_foundObjectMetricImportance; }
  NM_INLINE const float& getFoundObjectMetricImportanceRef() const { return m_foundObjectMetricImportance; }
  NM_INLINE const float& getFoundObjectMetric() const { return foundObjectMetric; }

protected:

  Environment::State foundObject;
  float foundObjectMetric;

  // post-assignment stubs
  NM_INLINE void postAssignFoundObject(const Environment::State& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignFoundObjectMetric(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "foundObjectMetric");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_foundObjectImportance, 
     m_foundObjectMetricImportance;

  friend class ObserveBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getFoundObjectImportance() > 0.0f)
    {
      foundObject.validate();
    }
    if (getFoundObjectMetricImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(foundObjectMetric), "foundObjectMetric");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_OBSERVEBEHAVIOURINTERFACE_DATA_H

