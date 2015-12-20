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

#ifndef NM_MDF_GRABDETECTION_DATA_H
#define NM_MDF_GRABDETECTION_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/GrabDetection.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/EdgeBuffer.h"
#include "Types/Edge.h"
#include "Types/ObjectFiltering.h"
#include "Types/GrabDetectionParams.h"
#include "Types/TargetRequest.h"
#include "Types/PatchSet.h"
#include "Types/BodyState.h"

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
 * \class GrabDetectionData
 * \ingroup GrabDetection
 * \brief Data identifies geometry in the environment appropriate for grabbing and passes the best patch to Grab
 *
 * Data packet definition (429 bytes, 448 aligned)
 */
NMP_ALIGN_PREFIX(32) struct GrabDetectionData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(GrabDetectionData));
  }

  EdgeBuffer edgeBuffer;  ///< Keep local memory of encountered edges (ring buffer).
  NMP::Vector3 smoothedChestVel;  ///< Makes chest predictions less erratic.  (Velocity)
  int64_t bestEdgeShapeID;
  float maxArmLength;  ///< Length of longest arm, calculated in entry.  (Length)
  bool isFirstUpdate;  ///< True only right at the start of the update (and then cleared).

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(smoothedChestVel), "smoothedChestVel");
    NM_VALIDATOR(FloatNonNegative(maxArmLength), "maxArmLength");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class GrabDetectionInputs
 * \ingroup GrabDetection
 * \brief Inputs identifies geometry in the environment appropriate for grabbing and passes the best patch to Grab
 *
 * Data packet definition (1052 bytes, 1056 aligned)
 */
NMP_ALIGN_PREFIX(32) struct GrabDetectionInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(GrabDetectionInputs));
  }

  enum { maxGameEdges = 10 };
  NM_INLINE unsigned int getMaxGameEdges() const { return maxGameEdges; }
  NM_INLINE void setGameEdges(unsigned int number, const Edge* _gameEdges, float gameEdges_importance = 1.f)
  {
    NMP_ASSERT(number <= maxGameEdges);
    NMP_ASSERT(gameEdges_importance >= 0.0f && gameEdges_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      gameEdges[i] = _gameEdges[i];
      postAssignGameEdges(gameEdges[i]);
      m_gameEdgesImportance[i] = gameEdges_importance;
    }
  }
  NM_INLINE void setGameEdgesAt(unsigned int index, const Edge& _gameEdges, float gameEdges_importance = 1.f)
  {
    NMP_ASSERT(index < maxGameEdges);
    gameEdges[index] = _gameEdges;
    NMP_ASSERT(gameEdges_importance >= 0.0f && gameEdges_importance <= 1.0f);
    postAssignGameEdges(gameEdges[index]);
    m_gameEdgesImportance[index] = gameEdges_importance;
  }
  NM_INLINE Edge& startGameEdgesModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxGameEdges);
    m_gameEdgesImportance[index] = -1.0f; // set invalid until stopGameEdgesModificationAt()
    return gameEdges[index];
  }
  NM_INLINE void stopGameEdgesModificationAt(unsigned int index, float gameEdges_importance = 1.f)
  {
    postAssignGameEdges(gameEdges[index]);
    NMP_ASSERT(gameEdges_importance >= 0.0f && gameEdges_importance <= 1.0f);
    m_gameEdgesImportance[index] = gameEdges_importance;
  }
  NM_INLINE float getGameEdgesImportance(int index) const { return m_gameEdgesImportance[index]; }
  NM_INLINE const float& getGameEdgesImportanceRef(int index) const { return m_gameEdgesImportance[index]; }
  NM_INLINE unsigned int calculateNumGameEdges() const
  {
    for (int i=9; i>=0; --i)
    {
      if (m_gameEdgesImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const Edge& getGameEdges(unsigned int index) const { NMP_ASSERT(index <= maxGameEdges); return gameEdges[index]; }

  NM_INLINE void setParams(const GrabDetectionParams& _params, float params_importance = 1.f)
  {
    params = _params;
    postAssignParams(params);
    NMP_ASSERT(params_importance >= 0.0f && params_importance <= 1.0f);
    m_paramsImportance = params_importance;
  }
  NM_INLINE float getParamsImportance() const { return m_paramsImportance; }
  NM_INLINE const float& getParamsImportanceRef() const { return m_paramsImportance; }
  NM_INLINE const GrabDetectionParams& getParams() const { return params; }

  NM_INLINE void setGrabFiltering(const ObjectFiltering& _grabFiltering, float grabFiltering_importance = 1.f)
  {
    grabFiltering = _grabFiltering;
    NMP_ASSERT(grabFiltering_importance >= 0.0f && grabFiltering_importance <= 1.0f);
    m_grabFilteringImportance = grabFiltering_importance;
  }
  NM_INLINE float getGrabFilteringImportance() const { return m_grabFilteringImportance; }
  NM_INLINE const float& getGrabFilteringImportanceRef() const { return m_grabFilteringImportance; }
  NM_INLINE const ObjectFiltering& getGrabFiltering() const { return grabFiltering; }

protected:

  Edge gameEdges[10];  ///< Edges passed in directly by the game.
  GrabDetectionParams params;  ///< What kind of edges too look for.
  ObjectFiltering grabFiltering;

  // post-assignment stubs
  NM_INLINE void postAssignGameEdges(const Edge& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignParams(const GrabDetectionParams& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_gameEdgesImportance[10], 
     m_paramsImportance, 
     m_grabFilteringImportance;

  friend class GrabDetection_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numGameEdges = calculateNumGameEdges();
      for (uint32_t i=0; i<numGameEdges; i++)
      {
        if (getGameEdgesImportance(i) > 0.0f)
          gameEdges[i].validate();
      }
    }
    if (getParamsImportance() > 0.0f)
    {
      params.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class GrabDetectionOutputs
 * \ingroup GrabDetection
 * \brief Outputs identifies geometry in the environment appropriate for grabbing and passes the best patch to Grab
 *
 * Data packet definition (269 bytes, 288 aligned)
 */
NMP_ALIGN_PREFIX(32) struct GrabDetectionOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(GrabDetectionOutputs));
  }

  NM_INLINE void setLookTarget(const TargetRequest& _lookTarget, float lookTarget_importance = 1.f)
  {
    lookTarget = _lookTarget;
    postAssignLookTarget(lookTarget);
    NMP_ASSERT(lookTarget_importance >= 0.0f && lookTarget_importance <= 1.0f);
    m_lookTargetImportance = lookTarget_importance;
  }
  NM_INLINE TargetRequest& startLookTargetModification()
  {
    m_lookTargetImportance = -1.0f; // set invalid until stopLookTargetModification()
    return lookTarget;
  }
  NM_INLINE void stopLookTargetModification(float lookTarget_importance = 1.f)
  {
    postAssignLookTarget(lookTarget);
    NMP_ASSERT(lookTarget_importance >= 0.0f && lookTarget_importance <= 1.0f);
    m_lookTargetImportance = lookTarget_importance;
  }
  NM_INLINE float getLookTargetImportance() const { return m_lookTargetImportance; }
  NM_INLINE const float& getLookTargetImportanceRef() const { return m_lookTargetImportance; }
  NM_INLINE const TargetRequest& getLookTarget() const { return lookTarget; }

  NM_INLINE void setGrabbableEdge(const Edge& _grabbableEdge, float grabbableEdge_importance = 1.f)
  {
    grabbableEdge = _grabbableEdge;
    postAssignGrabbableEdge(grabbableEdge);
    NMP_ASSERT(grabbableEdge_importance >= 0.0f && grabbableEdge_importance <= 1.0f);
    m_grabbableEdgeImportance = grabbableEdge_importance;
  }
  NM_INLINE Edge& startGrabbableEdgeModification()
  {
    m_grabbableEdgeImportance = -1.0f; // set invalid until stopGrabbableEdgeModification()
    return grabbableEdge;
  }
  NM_INLINE void stopGrabbableEdgeModification(float grabbableEdge_importance = 1.f)
  {
    postAssignGrabbableEdge(grabbableEdge);
    NMP_ASSERT(grabbableEdge_importance >= 0.0f && grabbableEdge_importance <= 1.0f);
    m_grabbableEdgeImportance = grabbableEdge_importance;
  }
  NM_INLINE float getGrabbableEdgeImportance() const { return m_grabbableEdgeImportance; }
  NM_INLINE const float& getGrabbableEdgeImportanceRef() const { return m_grabbableEdgeImportance; }
  NM_INLINE const Edge& getGrabbableEdge() const { return grabbableEdge; }

  NM_INLINE void setLetGoEdge(const bool& _letGoEdge, float letGoEdge_importance = 1.f)
  {
    letGoEdge = _letGoEdge;
    NMP_ASSERT(letGoEdge_importance >= 0.0f && letGoEdge_importance <= 1.0f);
    m_letGoEdgeImportance = letGoEdge_importance;
  }
  NM_INLINE float getLetGoEdgeImportance() const { return m_letGoEdgeImportance; }
  NM_INLINE const float& getLetGoEdgeImportanceRef() const { return m_letGoEdgeImportance; }
  NM_INLINE const bool& getLetGoEdge() const { return letGoEdge; }

protected:

  TargetRequest lookTarget;  ///< Head look target, closest point on edge to character's chest.
  Edge grabbableEdge;  ///< Returns best edge found amongst EA patches and game-provided edges.
  bool letGoEdge;

  // post-assignment stubs
  NM_INLINE void postAssignLookTarget(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignGrabbableEdge(const Edge& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_lookTargetImportance, 
     m_grabbableEdgeImportance, 
     m_letGoEdgeImportance;

  friend class GrabDetection_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getLookTargetImportance() > 0.0f)
    {
      lookTarget.validate();
    }
    if (getGrabbableEdgeImportance() > 0.0f)
    {
      grabbableEdge.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class GrabDetectionFeedbackInputs
 * \ingroup GrabDetection
 * \brief FeedbackInputs identifies geometry in the environment appropriate for grabbing and passes the best patch to Grab
 *
 * Data packet definition (818 bytes, 832 aligned)
 */
NMP_ALIGN_PREFIX(32) struct GrabDetectionFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(GrabDetectionFeedbackInputs));
  }

  NM_INLINE void setPotentialGrabPatches(const PatchSet& _potentialGrabPatches, float potentialGrabPatches_importance = 1.f)
  {
    potentialGrabPatches = _potentialGrabPatches;
    NMP_ASSERT(potentialGrabPatches_importance >= 0.0f && potentialGrabPatches_importance <= 1.0f);
    m_potentialGrabPatchesImportance = potentialGrabPatches_importance;
  }
  NM_INLINE PatchSet& startPotentialGrabPatchesModification()
  {
    m_potentialGrabPatchesImportance = -1.0f; // set invalid until stopPotentialGrabPatchesModification()
    return potentialGrabPatches;
  }
  NM_INLINE void stopPotentialGrabPatchesModification(float potentialGrabPatches_importance = 1.f)
  {
    NMP_ASSERT(potentialGrabPatches_importance >= 0.0f && potentialGrabPatches_importance <= 1.0f);
    m_potentialGrabPatchesImportance = potentialGrabPatches_importance;
  }
  NM_INLINE float getPotentialGrabPatchesImportance() const { return m_potentialGrabPatchesImportance; }
  NM_INLINE const float& getPotentialGrabPatchesImportanceRef() const { return m_potentialGrabPatchesImportance; }
  NM_INLINE const PatchSet& getPotentialGrabPatches() const { return potentialGrabPatches; }

  NM_INLINE void setCentreOfMassBodyState(const BodyState& _centreOfMassBodyState, float centreOfMassBodyState_importance = 1.f)
  {
    centreOfMassBodyState = _centreOfMassBodyState;
    postAssignCentreOfMassBodyState(centreOfMassBodyState);
    NMP_ASSERT(centreOfMassBodyState_importance >= 0.0f && centreOfMassBodyState_importance <= 1.0f);
    m_centreOfMassBodyStateImportance = centreOfMassBodyState_importance;
  }
  NM_INLINE BodyState& startCentreOfMassBodyStateModification()
  {
    m_centreOfMassBodyStateImportance = -1.0f; // set invalid until stopCentreOfMassBodyStateModification()
    return centreOfMassBodyState;
  }
  NM_INLINE void stopCentreOfMassBodyStateModification(float centreOfMassBodyState_importance = 1.f)
  {
    postAssignCentreOfMassBodyState(centreOfMassBodyState);
    NMP_ASSERT(centreOfMassBodyState_importance >= 0.0f && centreOfMassBodyState_importance <= 1.0f);
    m_centreOfMassBodyStateImportance = centreOfMassBodyState_importance;
  }
  NM_INLINE float getCentreOfMassBodyStateImportance() const { return m_centreOfMassBodyStateImportance; }
  NM_INLINE const float& getCentreOfMassBodyStateImportanceRef() const { return m_centreOfMassBodyStateImportance; }
  NM_INLINE const BodyState& getCentreOfMassBodyState() const { return centreOfMassBodyState; }

  NM_INLINE void setUseEnvironmentAwareness(const bool& _useEnvironmentAwareness, float useEnvironmentAwareness_importance = 1.f)
  {
    useEnvironmentAwareness = _useEnvironmentAwareness;
    NMP_ASSERT(useEnvironmentAwareness_importance >= 0.0f && useEnvironmentAwareness_importance <= 1.0f);
    m_useEnvironmentAwarenessImportance = useEnvironmentAwareness_importance;
  }
  NM_INLINE float getUseEnvironmentAwarenessImportance() const { return m_useEnvironmentAwarenessImportance; }
  NM_INLINE const float& getUseEnvironmentAwarenessImportanceRef() const { return m_useEnvironmentAwarenessImportance; }
  NM_INLINE const bool& getUseEnvironmentAwareness() const { return useEnvironmentAwareness; }

  NM_INLINE void setIsHanging(const bool& _isHanging, float isHanging_importance = 1.f)
  {
    isHanging = _isHanging;
    NMP_ASSERT(isHanging_importance >= 0.0f && isHanging_importance <= 1.0f);
    m_isHangingImportance = isHanging_importance;
  }
  NM_INLINE float getIsHangingImportance() const { return m_isHangingImportance; }
  NM_INLINE const float& getIsHangingImportanceRef() const { return m_isHangingImportance; }
  NM_INLINE const bool& getIsHanging() const { return isHanging; }

protected:

  PatchSet potentialGrabPatches;  ///< Input from EA system: set of patches colliding with impactPredictor's sphere sweep
  BodyState centreOfMassBodyState;
  bool useEnvironmentAwareness;  ///< Allows Grab module to control when to use grabDetection (and therefore EA).
  bool isHanging;

  // post-assignment stubs
  NM_INLINE void postAssignCentreOfMassBodyState(const BodyState& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_potentialGrabPatchesImportance, 
     m_centreOfMassBodyStateImportance, 
     m_useEnvironmentAwarenessImportance, 
     m_isHangingImportance;

  friend class GrabDetection_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getCentreOfMassBodyStateImportance() > 0.0f)
    {
      centreOfMassBodyState.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_GRABDETECTION_DATA_H

