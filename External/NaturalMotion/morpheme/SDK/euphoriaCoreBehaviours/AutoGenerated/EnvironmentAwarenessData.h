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

#ifndef NM_MDF_ENVIRONMENTAWARENESS_DATA_H
#define NM_MDF_ENVIRONMENTAWARENESS_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/EnvironmentAwareness.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erCollisionProbes.h"

// known types
#include "Types/PatchStore.h"
#include "Types/PatchSet.h"
#include "Types/Environment_ObjectData.h"
#include "Types/Environment_Object.h"
#include "Types/ModuleRNG.h"
#include "Types/SphereTrajectory.h"
#include "Types/ObjectMetric.h"
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
 * \class EnvironmentAwarenessData
 * \ingroup EnvironmentAwareness
 * \brief Data module for retrieving useful information from the collision geometry of the environment (based on requested sphere projection probes) and passing the
 *  results up as environment patches (small pieces of geometry that approximate the neighbourhood of the probe hit point)
 *
 * Data packet definition (6299 bytes, 6304 aligned)
 */
NMP_ALIGN_PREFIX(32) struct EnvironmentAwarenessData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(EnvironmentAwarenessData));
  }

  enum Constants
  {
    /*  1 */ numImpactProjections = 1,  
    /*  2 */ numLegProjections = (NetworkConstants::networkMaxNumLegs),  
    /*  2 */ numArmProjections = (NetworkConstants::networkMaxNumArms),  
    /*  1 */ numProjections = (EnvironmentAwarenessData::numImpactProjections),  //numProjections = numImpactProjections + numLegProjections + numArmProjections,

    /* 16 */ maxObjectsInRange = 16,  
  };

  PatchSet patchSets[EnvironmentAwarenessData::numProjections];  ///< The resulting patches after being applied to the latest shape's transform
  PatchStore patchStores[EnvironmentAwarenessData::numProjections];  ///< Stores the state of each patch relative to its shape
  Environment::Object dynamicObjectsInRange[EnvironmentAwarenessData::maxObjectsInRange];  ///< This is the latest state of the dynamic objects in view, e.g. TMs, velocities
  Environment::Object staticSweepResultObject;  ///< The static object's state
  Environment::Object staticContactObject;  ///< The state of the latest contacting object
  Environment::ObjectData objectData[EnvironmentAwarenessData::maxObjectsInRange];  ///< Persistent data for objects that come in and out of view, e.g. for calcualting smooth velocities
  NMP::Vector3 lastDynamicContactPoint;      ///< (Position)
  NMP::Vector3 lastStaticContactPoint;       ///< (Position)
  ModuleRNG rng;  ///< Used to do a random nearby sweep when no other known dangers
  int32_t probeID;  ///< Which of the input probes to use
  int32_t staticProbeID;  ///< Which of the input probes to use for static objects
  int32_t numDynamicObjectsInRange;  ///< size of dynamicObjectsInRange array
  float lastMetric;  ///< Used for time-based hysteresis of observed objects
  bool foundPotentialDynamicLastFrame;  ///< Whether a dynamic object's bounding box collided with the projection last frame
  bool dynamicCollidedLastTest;  // Used to decide whether to do lower priority 'nearby' sweeps
  bool staticCollidedLastTest;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    objectData[0].validate();
    objectData[1].validate();
    objectData[2].validate();
    objectData[3].validate();
    objectData[4].validate();
    objectData[5].validate();
    objectData[6].validate();
    objectData[7].validate();
    objectData[8].validate();
    objectData[9].validate();
    objectData[10].validate();
    objectData[11].validate();
    objectData[12].validate();
    objectData[13].validate();
    objectData[14].validate();
    objectData[15].validate();
    NM_VALIDATOR(Vector3Valid(lastDynamicContactPoint), "lastDynamicContactPoint");
    NM_VALIDATOR(Vector3Valid(lastStaticContactPoint), "lastStaticContactPoint");
    NM_VALIDATOR(FloatValid(lastMetric), "lastMetric");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class EnvironmentAwarenessInputs
 * \ingroup EnvironmentAwareness
 * \brief Inputs module for retrieving useful information from the collision geometry of the environment (based on requested sphere projection probes) and passing the
 *  results up as environment patches (small pieces of geometry that approximate the neighbourhood of the probe hit point)
 *
 * Data packet definition (152 bytes, 160 aligned)
 */
NMP_ALIGN_PREFIX(32) struct EnvironmentAwarenessInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(EnvironmentAwarenessInputs));
  }

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

  enum { maxSphereProjectionsForImpactPredictor = 1 };
  NM_INLINE unsigned int getMaxSphereProjectionsForImpactPredictor() const { return maxSphereProjectionsForImpactPredictor; }
  NM_INLINE void setSphereProjectionsForImpactPredictor(unsigned int number, const SphereTrajectory* _sphereProjectionsForImpactPredictor, float sphereProjectionsForImpactPredictor_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSphereProjectionsForImpactPredictor);
    NMP_ASSERT(sphereProjectionsForImpactPredictor_importance >= 0.0f && sphereProjectionsForImpactPredictor_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      sphereProjectionsForImpactPredictor[i] = _sphereProjectionsForImpactPredictor[i];
      postAssignSphereProjectionsForImpactPredictor(sphereProjectionsForImpactPredictor[i]);
      m_sphereProjectionsForImpactPredictorImportance[i] = sphereProjectionsForImpactPredictor_importance;
    }
  }
  NM_INLINE void setSphereProjectionsForImpactPredictorAt(unsigned int index, const SphereTrajectory& _sphereProjectionsForImpactPredictor, float sphereProjectionsForImpactPredictor_importance = 1.f)
  {
    NMP_ASSERT(index < maxSphereProjectionsForImpactPredictor);
    sphereProjectionsForImpactPredictor[index] = _sphereProjectionsForImpactPredictor;
    NMP_ASSERT(sphereProjectionsForImpactPredictor_importance >= 0.0f && sphereProjectionsForImpactPredictor_importance <= 1.0f);
    postAssignSphereProjectionsForImpactPredictor(sphereProjectionsForImpactPredictor[index]);
    m_sphereProjectionsForImpactPredictorImportance[index] = sphereProjectionsForImpactPredictor_importance;
  }
  NM_INLINE SphereTrajectory& startSphereProjectionsForImpactPredictorModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxSphereProjectionsForImpactPredictor);
    m_sphereProjectionsForImpactPredictorImportance[index] = -1.0f; // set invalid until stopSphereProjectionsForImpactPredictorModificationAt()
    return sphereProjectionsForImpactPredictor[index];
  }
  NM_INLINE void stopSphereProjectionsForImpactPredictorModificationAt(unsigned int index, float sphereProjectionsForImpactPredictor_importance = 1.f)
  {
    postAssignSphereProjectionsForImpactPredictor(sphereProjectionsForImpactPredictor[index]);
    NMP_ASSERT(sphereProjectionsForImpactPredictor_importance >= 0.0f && sphereProjectionsForImpactPredictor_importance <= 1.0f);
    m_sphereProjectionsForImpactPredictorImportance[index] = sphereProjectionsForImpactPredictor_importance;
  }
  NM_INLINE float getSphereProjectionsForImpactPredictorImportance(int index) const { return m_sphereProjectionsForImpactPredictorImportance[index]; }
  NM_INLINE const float& getSphereProjectionsForImpactPredictorImportanceRef(int index) const { return m_sphereProjectionsForImpactPredictorImportance[index]; }
  NM_INLINE unsigned int calculateNumSphereProjectionsForImpactPredictor() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_sphereProjectionsForImpactPredictorImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const SphereTrajectory& getSphereProjectionsForImpactPredictor(unsigned int index) const { NMP_ASSERT(index <= maxSphereProjectionsForImpactPredictor); return sphereProjectionsForImpactPredictor[index]; }

protected:

  ObjectMetric findObject;  ///< Metric for picking the 'best' object in view, e.g. the fastest within a range. Used by Observe
  SphereTrajectory sphereProjectionsForImpactPredictor[EnvironmentAwarenessData::numImpactProjections];  // TODO MORPH-19578 Use network constants here - but need arithmetic to do this

  // post-assignment stubs
  NM_INLINE void postAssignFindObject(const ObjectMetric& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSphereProjectionsForImpactPredictor(const SphereTrajectory& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_findObjectImportance, 
     m_sphereProjectionsForImpactPredictorImportance[EnvironmentAwarenessData::numImpactProjections];

  friend class EnvironmentAwareness_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getFindObjectImportance() > 0.0f)
    {
      findObject.validate();
    }
    {
      uint32_t numSphereProjectionsForImpactPredictor = calculateNumSphereProjectionsForImpactPredictor();
      for (uint32_t i=0; i<numSphereProjectionsForImpactPredictor; i++)
      {
        if (getSphereProjectionsForImpactPredictorImportance(i) > 0.0f)
          sphereProjectionsForImpactPredictor[i].validate();
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class EnvironmentAwarenessOutputs
 * \ingroup EnvironmentAwareness
 * \brief Outputs module for retrieving useful information from the collision geometry of the environment (based on requested sphere projection probes) and passing the
 *  results up as environment patches (small pieces of geometry that approximate the neighbourhood of the probe hit point)
 *
 * Data packet definition (72 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct EnvironmentAwarenessOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(EnvironmentAwarenessOutputs));
  }

  NM_INLINE void setSphereSweepDynamic(const ER::SphereSweep& _sphereSweepDynamic, float sphereSweepDynamic_importance = 1.f)
  {
    sphereSweepDynamic = _sphereSweepDynamic;
    NMP_ASSERT(sphereSweepDynamic_importance >= 0.0f && sphereSweepDynamic_importance <= 1.0f);
    m_sphereSweepDynamicImportance = sphereSweepDynamic_importance;
  }
  NM_INLINE float getSphereSweepDynamicImportance() const { return m_sphereSweepDynamicImportance; }
  NM_INLINE const float& getSphereSweepDynamicImportanceRef() const { return m_sphereSweepDynamicImportance; }
  NM_INLINE const ER::SphereSweep& getSphereSweepDynamic() const { return sphereSweepDynamic; }

  NM_INLINE void setSphereSweepStatic(const ER::SphereSweep& _sphereSweepStatic, float sphereSweepStatic_importance = 1.f)
  {
    sphereSweepStatic = _sphereSweepStatic;
    NMP_ASSERT(sphereSweepStatic_importance >= 0.0f && sphereSweepStatic_importance <= 1.0f);
    m_sphereSweepStaticImportance = sphereSweepStatic_importance;
  }
  NM_INLINE float getSphereSweepStaticImportance() const { return m_sphereSweepStaticImportance; }
  NM_INLINE const float& getSphereSweepStaticImportanceRef() const { return m_sphereSweepStaticImportance; }
  NM_INLINE const ER::SphereSweep& getSphereSweepStatic() const { return sphereSweepStatic; }

protected:

  ER::SphereSweep sphereSweepDynamic;  // Requesting linear sphere sweeps, not output every frame
  ER::SphereSweep sphereSweepStatic;

  // importance values
  float
     m_sphereSweepDynamicImportance, 
     m_sphereSweepStaticImportance;

  friend class EnvironmentAwareness_Con;

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
 * \class EnvironmentAwarenessFeedbackInputs
 * \ingroup EnvironmentAwareness
 * \brief FeedbackInputs module for retrieving useful information from the collision geometry of the environment (based on requested sphere projection probes) and passing the
 *  results up as environment patches (small pieces of geometry that approximate the neighbourhood of the probe hit point)
 *
 * Data packet definition (141 bytes, 160 aligned)
 */
NMP_ALIGN_PREFIX(32) struct EnvironmentAwarenessFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(EnvironmentAwarenessFeedbackInputs));
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

  NM_INLINE void setSweepResultDynamic(const ER::SweepResult& _sweepResultDynamic, float sweepResultDynamic_importance = 1.f)
  {
    sweepResultDynamic = _sweepResultDynamic;
    NMP_ASSERT(sweepResultDynamic_importance >= 0.0f && sweepResultDynamic_importance <= 1.0f);
    m_sweepResultDynamicImportance = sweepResultDynamic_importance;
  }
  NM_INLINE float getSweepResultDynamicImportance() const { return m_sweepResultDynamicImportance; }
  NM_INLINE const float& getSweepResultDynamicImportanceRef() const { return m_sweepResultDynamicImportance; }
  NM_INLINE const ER::SweepResult& getSweepResultDynamic() const { return sweepResultDynamic; }

  NM_INLINE void setSweepResultStatic(const ER::SweepResult& _sweepResultStatic, float sweepResultStatic_importance = 1.f)
  {
    sweepResultStatic = _sweepResultStatic;
    NMP_ASSERT(sweepResultStatic_importance >= 0.0f && sweepResultStatic_importance <= 1.0f);
    m_sweepResultStaticImportance = sweepResultStatic_importance;
  }
  NM_INLINE float getSweepResultStaticImportance() const { return m_sweepResultStaticImportance; }
  NM_INLINE const float& getSweepResultStaticImportanceRef() const { return m_sweepResultStaticImportance; }
  NM_INLINE const ER::SweepResult& getSweepResultStatic() const { return sweepResultStatic; }

  NM_INLINE void setContactResult(const ER::SweepResult& _contactResult, float contactResult_importance = 1.f)
  {
    contactResult = _contactResult;
    NMP_ASSERT(contactResult_importance >= 0.0f && contactResult_importance <= 1.0f);
    m_contactResultImportance = contactResult_importance;
  }
  NM_INLINE float getContactResultImportance() const { return m_contactResultImportance; }
  NM_INLINE const float& getContactResultImportanceRef() const { return m_contactResultImportance; }
  NM_INLINE const ER::SweepResult& getContactResult() const { return contactResult; }

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

  NMP::Vector3 focalCentre;  ///< Where the character actually ended up looking  (Position)
  ER::SweepResult sweepResultDynamic;  // Results of linear sphere sweeps, these don't all arrive every frame
  ER::SweepResult sweepResultStatic;
  ER::SweepResult contactResult;  ///< Result of contact information, e.g. a ground plane from the foot contact. Allows awareness when not looking
  float focalRadius;  ///< Width of sphere of vision  (Length)
  bool requestNextProbe;  ///< Request to receive a sphere sweep next frame

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
     m_sweepResultDynamicImportance, 
     m_sweepResultStaticImportance, 
     m_contactResultImportance, 
     m_focalRadiusImportance, 
     m_requestNextProbeImportance;

  friend class EnvironmentAwareness_Con;

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
 * \class EnvironmentAwarenessFeedbackOutputs
 * \ingroup EnvironmentAwareness
 * \brief FeedbackOutputs module for retrieving useful information from the collision geometry of the environment (based on requested sphere projection probes) and passing the
 *  results up as environment patches (small pieces of geometry that approximate the neighbourhood of the probe hit point)
 *
 * Data packet definition (880 bytes, 896 aligned)
 */
NMP_ALIGN_PREFIX(32) struct EnvironmentAwarenessFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(EnvironmentAwarenessFeedbackOutputs));
  }

  enum { maxPotentialHazardsForImpactPredictor = 1 };
  NM_INLINE unsigned int getMaxPotentialHazardsForImpactPredictor() const { return maxPotentialHazardsForImpactPredictor; }
  NM_INLINE void setPotentialHazardsForImpactPredictor(unsigned int number, const PatchSet* _potentialHazardsForImpactPredictor, float potentialHazardsForImpactPredictor_importance = 1.f)
  {
    NMP_ASSERT(number <= maxPotentialHazardsForImpactPredictor);
    NMP_ASSERT(potentialHazardsForImpactPredictor_importance >= 0.0f && potentialHazardsForImpactPredictor_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      potentialHazardsForImpactPredictor[i] = _potentialHazardsForImpactPredictor[i];
      m_potentialHazardsForImpactPredictorImportance[i] = potentialHazardsForImpactPredictor_importance;
    }
  }
  NM_INLINE void setPotentialHazardsForImpactPredictorAt(unsigned int index, const PatchSet& _potentialHazardsForImpactPredictor, float potentialHazardsForImpactPredictor_importance = 1.f)
  {
    NMP_ASSERT(index < maxPotentialHazardsForImpactPredictor);
    potentialHazardsForImpactPredictor[index] = _potentialHazardsForImpactPredictor;
    NMP_ASSERT(potentialHazardsForImpactPredictor_importance >= 0.0f && potentialHazardsForImpactPredictor_importance <= 1.0f);
    m_potentialHazardsForImpactPredictorImportance[index] = potentialHazardsForImpactPredictor_importance;
  }
  NM_INLINE PatchSet& startPotentialHazardsForImpactPredictorModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxPotentialHazardsForImpactPredictor);
    m_potentialHazardsForImpactPredictorImportance[index] = -1.0f; // set invalid until stopPotentialHazardsForImpactPredictorModificationAt()
    return potentialHazardsForImpactPredictor[index];
  }
  NM_INLINE void stopPotentialHazardsForImpactPredictorModificationAt(unsigned int index, float potentialHazardsForImpactPredictor_importance = 1.f)
  {
    NMP_ASSERT(potentialHazardsForImpactPredictor_importance >= 0.0f && potentialHazardsForImpactPredictor_importance <= 1.0f);
    m_potentialHazardsForImpactPredictorImportance[index] = potentialHazardsForImpactPredictor_importance;
  }
  NM_INLINE float getPotentialHazardsForImpactPredictorImportance(int index) const { return m_potentialHazardsForImpactPredictorImportance[index]; }
  NM_INLINE const float& getPotentialHazardsForImpactPredictorImportanceRef(int index) const { return m_potentialHazardsForImpactPredictorImportance[index]; }
  NM_INLINE unsigned int calculateNumPotentialHazardsForImpactPredictor() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_potentialHazardsForImpactPredictorImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PatchSet& getPotentialHazardsForImpactPredictor(unsigned int index) const { NMP_ASSERT(index <= maxPotentialHazardsForImpactPredictor); return potentialHazardsForImpactPredictor[index]; }

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

  PatchSet potentialHazardsForImpactPredictor[EnvironmentAwarenessData::numImpactProjections];  ///< Returns a set of potential hazards for each projection that has been passed down
  Environment::State foundObject;  ///< Result of the in.findObject metric
  float foundObjectMetric;  ///< Value of the in.findObject metric

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
     m_potentialHazardsForImpactPredictorImportance[EnvironmentAwarenessData::numImpactProjections], 
     m_foundObjectImportance, 
     m_foundObjectMetricImportance;

  friend class EnvironmentAwareness_Con;

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

#endif // NM_MDF_ENVIRONMENTAWARENESS_DATA_H

