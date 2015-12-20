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

#ifndef NM_MDF_GRAB_DATA_H
#define NM_MDF_GRAB_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/Grab.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/Edge.h"
#include "Types/GrabControl.h"
#include "Types/GrabEnableConditions.h"
#include "Types/GrabAliveParams.h"
#include "Types/ContactInfo.h"
#include "Types/Hazard.h"
#include "Types/EndConstraintControl.h"
#include "Types/HoldTimer.h"
#include "Types/TranslationRequest.h"
#include "Types/RotationRequest.h"
#include "Types/TargetRequest.h"

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
 * \class GrabData
 * \ingroup Grab
 * \brief Data try to hold onto environment objects for support (when falling e.g., or to help balancer)
 *
 * Data packet definition (140 bytes, 160 aligned)
 */
NMP_ALIGN_PREFIX(32) struct GrabData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(GrabData));
  }

  Edge cachedGrabbableEdgeLocal;  /// Caches the edge sent out, in local space. If a constraint is actually created, we can stop looking for grabbable edges
  /// (using the EA system), and just keep sending this.
  NMP::Vector3 desiredTorsoFwd;  ///< Ideal facing direction while hanging.  (Direction)
  float edgeBehindPeriod;  ///< How long the edge has been behind the character's back.  (TimePeriod)
  float balancedPeriod;  ///< How long the character's been balanced.  (TimePeriod)
  float onGroundPeriod;  ///< How long the character has been lying on the floor.  (TimePeriod)
  float holdDuration;  ///< How long the character has been constrained.  (TimePeriod)
  float maxArmLength;  ///< Length of the longest arm.  (Distance)
  uint16_t numConstrainedArms;  ///< Number of currently constrained hands.
  bool grabAllowed;  ///< Whether to try and grab currently. Used for hysteresis of some internal parameters.
  bool isHanging;  ///< Is the character stably hanging (rather than still falling).
  bool rayCastEdge;  /// Perform edge ray-cast on first entry, then only when edge has changed provided it's a non-action edge.
  bool edgeIsObstructed;
  bool useEnvironmentAwareness;
  bool reEvaluateGrabPoint;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    cachedGrabbableEdgeLocal.validate();
    NM_VALIDATOR(Vector3Normalised(desiredTorsoFwd), "desiredTorsoFwd");
    NM_VALIDATOR(FloatValid(edgeBehindPeriod), "edgeBehindPeriod");
    NM_VALIDATOR(FloatValid(balancedPeriod), "balancedPeriod");
    NM_VALIDATOR(FloatValid(onGroundPeriod), "onGroundPeriod");
    NM_VALIDATOR(FloatValid(holdDuration), "holdDuration");
    NM_VALIDATOR(FloatValid(maxArmLength), "maxArmLength");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class GrabInputs
 * \ingroup Grab
 * \brief Inputs try to hold onto environment objects for support (when falling e.g., or to help balancer)
 *
 * Data packet definition (1463 bytes, 1472 aligned)
 */
NMP_ALIGN_PREFIX(32) struct GrabInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(GrabInputs));
  }

  NM_INLINE void setGrabAbilityControl(const GrabControl& _grabAbilityControl, float grabAbilityControl_importance = 1.f)
  {
    grabAbilityControl = _grabAbilityControl;
    NMP_ASSERT(grabAbilityControl_importance >= 0.0f && grabAbilityControl_importance <= 1.0f);
    m_grabAbilityControlImportance = grabAbilityControl_importance;
  }
  NM_INLINE GrabControl& startGrabAbilityControlModification()
  {
    m_grabAbilityControlImportance = -1.0f; // set invalid until stopGrabAbilityControlModification()
    return grabAbilityControl;
  }
  NM_INLINE void stopGrabAbilityControlModification(float grabAbilityControl_importance = 1.f)
  {
    NMP_ASSERT(grabAbilityControl_importance >= 0.0f && grabAbilityControl_importance <= 1.0f);
    m_grabAbilityControlImportance = grabAbilityControl_importance;
  }
  NM_INLINE float getGrabAbilityControlImportance() const { return m_grabAbilityControlImportance; }
  NM_INLINE const float& getGrabAbilityControlImportanceRef() const { return m_grabAbilityControlImportance; }
  NM_INLINE const GrabControl& getGrabAbilityControl() const { return grabAbilityControl; }

  NM_INLINE void setGrabActionControl(const GrabControl& _grabActionControl, float grabActionControl_importance = 1.f)
  {
    grabActionControl = _grabActionControl;
    NMP_ASSERT(grabActionControl_importance >= 0.0f && grabActionControl_importance <= 1.0f);
    m_grabActionControlImportance = grabActionControl_importance;
  }
  NM_INLINE GrabControl& startGrabActionControlModification()
  {
    m_grabActionControlImportance = -1.0f; // set invalid until stopGrabActionControlModification()
    return grabActionControl;
  }
  NM_INLINE void stopGrabActionControlModification(float grabActionControl_importance = 1.f)
  {
    NMP_ASSERT(grabActionControl_importance >= 0.0f && grabActionControl_importance <= 1.0f);
    m_grabActionControlImportance = grabActionControl_importance;
  }
  NM_INLINE float getGrabActionControlImportance() const { return m_grabActionControlImportance; }
  NM_INLINE const float& getGrabActionControlImportanceRef() const { return m_grabActionControlImportance; }
  NM_INLINE const GrabControl& getGrabActionControl() const { return grabActionControl; }

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

  NM_INLINE void setEnableConditions(const GrabEnableConditions& _enableConditions, float enableConditions_importance = 1.f)
  {
    enableConditions = _enableConditions;
    postAssignEnableConditions(enableConditions);
    NMP_ASSERT(enableConditions_importance >= 0.0f && enableConditions_importance <= 1.0f);
    m_enableConditionsImportance = enableConditions_importance;
  }
  NM_INLINE float getEnableConditionsImportance() const { return m_enableConditionsImportance; }
  NM_INLINE const float& getEnableConditionsImportanceRef() const { return m_enableConditionsImportance; }
  NM_INLINE const GrabEnableConditions& getEnableConditions() const { return enableConditions; }

  NM_INLINE void setGrabAliveParams(const GrabAliveParams& _grabAliveParams, float grabAliveParams_importance = 1.f)
  {
    grabAliveParams = _grabAliveParams;
    postAssignGrabAliveParams(grabAliveParams);
    NMP_ASSERT(grabAliveParams_importance >= 0.0f && grabAliveParams_importance <= 1.0f);
    m_grabAliveParamsImportance = grabAliveParams_importance;
  }
  NM_INLINE float getGrabAliveParamsImportance() const { return m_grabAliveParamsImportance; }
  NM_INLINE const float& getGrabAliveParamsImportanceRef() const { return m_grabAliveParamsImportance; }
  NM_INLINE const GrabAliveParams& getGrabAliveParams() const { return grabAliveParams; }

  NM_INLINE void setMaxTwistBwdPeriod(const float& _maxTwistBwdPeriod, float maxTwistBwdPeriod_importance = 1.f)
  {
    maxTwistBwdPeriod = _maxTwistBwdPeriod;
    postAssignMaxTwistBwdPeriod(maxTwistBwdPeriod);
    NMP_ASSERT(maxTwistBwdPeriod_importance >= 0.0f && maxTwistBwdPeriod_importance <= 1.0f);
    m_maxTwistBwdPeriodImportance = maxTwistBwdPeriod_importance;
  }
  NM_INLINE float getMaxTwistBwdPeriodImportance() const { return m_maxTwistBwdPeriodImportance; }
  NM_INLINE const float& getMaxTwistBwdPeriodImportanceRef() const { return m_maxTwistBwdPeriodImportance; }
  NM_INLINE const float& getMaxTwistBwdPeriod() const { return maxTwistBwdPeriod; }

  NM_INLINE void setTimeRatioBeforeLookingDown(const float& _timeRatioBeforeLookingDown, float timeRatioBeforeLookingDown_importance = 1.f)
  {
    timeRatioBeforeLookingDown = _timeRatioBeforeLookingDown;
    postAssignTimeRatioBeforeLookingDown(timeRatioBeforeLookingDown);
    NMP_ASSERT(timeRatioBeforeLookingDown_importance >= 0.0f && timeRatioBeforeLookingDown_importance <= 1.0f);
    m_timeRatioBeforeLookingDownImportance = timeRatioBeforeLookingDown_importance;
  }
  NM_INLINE float getTimeRatioBeforeLookingDownImportance() const { return m_timeRatioBeforeLookingDownImportance; }
  NM_INLINE const float& getTimeRatioBeforeLookingDownImportanceRef() const { return m_timeRatioBeforeLookingDownImportance; }
  NM_INLINE const float& getTimeRatioBeforeLookingDown() const { return timeRatioBeforeLookingDown; }

  NM_INLINE void setHasForcedEdge(const bool& _hasForcedEdge, float hasForcedEdge_importance = 1.f)
  {
    hasForcedEdge = _hasForcedEdge;
    NMP_ASSERT(hasForcedEdge_importance >= 0.0f && hasForcedEdge_importance <= 1.0f);
    m_hasForcedEdgeImportance = hasForcedEdge_importance;
  }
  NM_INLINE float getHasForcedEdgeImportance() const { return m_hasForcedEdgeImportance; }
  NM_INLINE const float& getHasForcedEdgeImportanceRef() const { return m_hasForcedEdgeImportance; }
  NM_INLINE const bool& getHasForcedEdge() const { return hasForcedEdge; }

  NM_INLINE void setLetGoEdge(const bool& _letGoEdge, float letGoEdge_importance = 1.f)
  {
    letGoEdge = _letGoEdge;
    NMP_ASSERT(letGoEdge_importance >= 0.0f && letGoEdge_importance <= 1.0f);
    m_letGoEdgeImportance = letGoEdge_importance;
  }
  NM_INLINE float getLetGoEdgeImportance() const { return m_letGoEdgeImportance; }
  NM_INLINE const float& getLetGoEdgeImportanceRef() const { return m_letGoEdgeImportance; }
  NM_INLINE const bool& getLetGoEdge() const { return letGoEdge; }

  NM_INLINE void setIgnoreOvershotEdges(const bool& _ignoreOvershotEdges, float ignoreOvershotEdges_importance = 1.f)
  {
    ignoreOvershotEdges = _ignoreOvershotEdges;
    NMP_ASSERT(ignoreOvershotEdges_importance >= 0.0f && ignoreOvershotEdges_importance <= 1.0f);
    m_ignoreOvershotEdgesImportance = ignoreOvershotEdges_importance;
  }
  NM_INLINE float getIgnoreOvershotEdgesImportance() const { return m_ignoreOvershotEdgesImportance; }
  NM_INLINE const float& getIgnoreOvershotEdgesImportanceRef() const { return m_ignoreOvershotEdgesImportance; }
  NM_INLINE const bool& getIgnoreOvershotEdges() const { return ignoreOvershotEdges; }

protected:

  GrabControl grabAbilityControl;  ///< Hold parameters as specified by the Hold (-ability) behaviour interface
  GrabControl grabActionControl;  ///< Hold parameters as specified by the HoldAction behaviour interface
  Edge gameEdges[10];  ///< Edges passed in directly by the game.
  Edge grabbableEdge;  ///< Best edge to grab as returned by GrabDetection.
  GrabEnableConditions enableConditions;  ///< Configures the conditions for starting a hold attempt.
  GrabAliveParams grabAliveParams;  ///< Configures behaviour while hanging (e.g. "pullUps")
  float maxTwistBwdPeriod;  ///< If character is not facing the edge for this long he will give up  (TimePeriod)
  float timeRatioBeforeLookingDown;  /// The proportion of maxHoldTime at which the character will switch from looking at the edge to looking down.  (Weight)
  bool hasForcedEdge;  ///< If true, would overwrite any EA patches, therefore no need to use EA.
  bool letGoEdge;
  bool ignoreOvershotEdges;  ///< Ignore edge when chest is likely to land on the surface above that edge.

  // post-assignment stubs
  NM_INLINE void postAssignGameEdges(const Edge& NMP_VALIDATOR_ARG(v)) const
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

  NM_INLINE void postAssignEnableConditions(const GrabEnableConditions& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignGrabAliveParams(const GrabAliveParams& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignMaxTwistBwdPeriod(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "maxTwistBwdPeriod");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTimeRatioBeforeLookingDown(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "timeRatioBeforeLookingDown");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_grabAbilityControlImportance, 
     m_grabActionControlImportance, 
     m_gameEdgesImportance[10], 
     m_grabbableEdgeImportance, 
     m_enableConditionsImportance, 
     m_grabAliveParamsImportance, 
     m_maxTwistBwdPeriodImportance, 
     m_timeRatioBeforeLookingDownImportance, 
     m_hasForcedEdgeImportance, 
     m_letGoEdgeImportance, 
     m_ignoreOvershotEdgesImportance;

  friend class Grab_Con;

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
    if (getGrabbableEdgeImportance() > 0.0f)
    {
      grabbableEdge.validate();
    }
    if (getEnableConditionsImportance() > 0.0f)
    {
      enableConditions.validate();
    }
    if (getGrabAliveParamsImportance() > 0.0f)
    {
      grabAliveParams.validate();
    }
    if (getMaxTwistBwdPeriodImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(maxTwistBwdPeriod), "maxTwistBwdPeriod");
    }
    if (getTimeRatioBeforeLookingDownImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(timeRatioBeforeLookingDown), "timeRatioBeforeLookingDown");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class GrabOutputs
 * \ingroup Grab
 * \brief Outputs try to hold onto environment objects for support (when falling e.g., or to help balancer)
 *
 * Data packet definition (770 bytes, 800 aligned)
 */
NMP_ALIGN_PREFIX(32) struct GrabOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(GrabOutputs));
  }

  NM_INLINE void setTranslationRequest(const TranslationRequest& _translationRequest, float translationRequest_importance = 1.f)
  {
    translationRequest = _translationRequest;
    postAssignTranslationRequest(translationRequest);
    NMP_ASSERT(translationRequest_importance >= 0.0f && translationRequest_importance <= 1.0f);
    m_translationRequestImportance = translationRequest_importance;
  }
  NM_INLINE TranslationRequest& startTranslationRequestModification()
  {
    m_translationRequestImportance = -1.0f; // set invalid until stopTranslationRequestModification()
    return translationRequest;
  }
  NM_INLINE void stopTranslationRequestModification(float translationRequest_importance = 1.f)
  {
    postAssignTranslationRequest(translationRequest);
    NMP_ASSERT(translationRequest_importance >= 0.0f && translationRequest_importance <= 1.0f);
    m_translationRequestImportance = translationRequest_importance;
  }
  NM_INLINE float getTranslationRequestImportance() const { return m_translationRequestImportance; }
  NM_INLINE const float& getTranslationRequestImportanceRef() const { return m_translationRequestImportance; }
  NM_INLINE const TranslationRequest& getTranslationRequest() const { return translationRequest; }

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

  NM_INLINE void setEdgeToGrab(const Edge& _edgeToGrab, float edgeToGrab_importance = 1.f)
  {
    edgeToGrab = _edgeToGrab;
    postAssignEdgeToGrab(edgeToGrab);
    NMP_ASSERT(edgeToGrab_importance >= 0.0f && edgeToGrab_importance <= 1.0f);
    m_edgeToGrabImportance = edgeToGrab_importance;
  }
  NM_INLINE Edge& startEdgeToGrabModification()
  {
    m_edgeToGrabImportance = -1.0f; // set invalid until stopEdgeToGrabModification()
    return edgeToGrab;
  }
  NM_INLINE void stopEdgeToGrabModification(float edgeToGrab_importance = 1.f)
  {
    postAssignEdgeToGrab(edgeToGrab);
    NMP_ASSERT(edgeToGrab_importance >= 0.0f && edgeToGrab_importance <= 1.0f);
    m_edgeToGrabImportance = edgeToGrab_importance;
  }
  NM_INLINE float getEdgeToGrabImportance() const { return m_edgeToGrabImportance; }
  NM_INLINE const float& getEdgeToGrabImportanceRef() const { return m_edgeToGrabImportance; }
  NM_INLINE const Edge& getEdgeToGrab() const { return edgeToGrab; }

  NM_INLINE void setEndConstraintOptions(const EndConstraintControl& _endConstraintOptions, float endConstraintOptions_importance = 1.f)
  {
    endConstraintOptions = _endConstraintOptions;
    postAssignEndConstraintOptions(endConstraintOptions);
    NMP_ASSERT(endConstraintOptions_importance >= 0.0f && endConstraintOptions_importance <= 1.0f);
    m_endConstraintOptionsImportance = endConstraintOptions_importance;
  }
  NM_INLINE EndConstraintControl& startEndConstraintOptionsModification()
  {
    m_endConstraintOptionsImportance = -1.0f; // set invalid until stopEndConstraintOptionsModification()
    return endConstraintOptions;
  }
  NM_INLINE void stopEndConstraintOptionsModification(float endConstraintOptions_importance = 1.f)
  {
    postAssignEndConstraintOptions(endConstraintOptions);
    NMP_ASSERT(endConstraintOptions_importance >= 0.0f && endConstraintOptions_importance <= 1.0f);
    m_endConstraintOptionsImportance = endConstraintOptions_importance;
  }
  NM_INLINE float getEndConstraintOptionsImportance() const { return m_endConstraintOptionsImportance; }
  NM_INLINE const float& getEndConstraintOptionsImportanceRef() const { return m_endConstraintOptionsImportance; }
  NM_INLINE const EndConstraintControl& getEndConstraintOptions() const { return endConstraintOptions; }

  NM_INLINE void setUprightContactDir(const NMP::Vector3& _uprightContactDir, float uprightContactDir_importance = 1.f)
  {
    uprightContactDir = _uprightContactDir;
    postAssignUprightContactDir(uprightContactDir);
    NMP_ASSERT(uprightContactDir_importance >= 0.0f && uprightContactDir_importance <= 1.0f);
    m_uprightContactDirImportance = uprightContactDir_importance;
  }
  NM_INLINE float getUprightContactDirImportance() const { return m_uprightContactDirImportance; }
  NM_INLINE const float& getUprightContactDirImportanceRef() const { return m_uprightContactDirImportance; }
  NM_INLINE const NMP::Vector3& getUprightContactDir() const { return uprightContactDir; }

  NM_INLINE void setHoldTimer(const HoldTimer& _holdTimer, float holdTimer_importance = 1.f)
  {
    holdTimer = _holdTimer;
    postAssignHoldTimer(holdTimer);
    NMP_ASSERT(holdTimer_importance >= 0.0f && holdTimer_importance <= 1.0f);
    m_holdTimerImportance = holdTimer_importance;
  }
  NM_INLINE float getHoldTimerImportance() const { return m_holdTimerImportance; }
  NM_INLINE const float& getHoldTimerImportanceRef() const { return m_holdTimerImportance; }
  NM_INLINE const HoldTimer& getHoldTimer() const { return holdTimer; }

  NM_INLINE void setArmSwingStiffnessScale(const float& _armSwingStiffnessScale, float armSwingStiffnessScale_importance = 1.f)
  {
    armSwingStiffnessScale = _armSwingStiffnessScale;
    postAssignArmSwingStiffnessScale(armSwingStiffnessScale);
    NMP_ASSERT(armSwingStiffnessScale_importance >= 0.0f && armSwingStiffnessScale_importance <= 1.0f);
    m_armSwingStiffnessScaleImportance = armSwingStiffnessScale_importance;
  }
  NM_INLINE float getArmSwingStiffnessScaleImportance() const { return m_armSwingStiffnessScaleImportance; }
  NM_INLINE const float& getArmSwingStiffnessScaleImportanceRef() const { return m_armSwingStiffnessScaleImportance; }
  NM_INLINE const float& getArmSwingStiffnessScale() const { return armSwingStiffnessScale; }

  NM_INLINE void setSpineDampingIncrease(const float& _spineDampingIncrease, float spineDampingIncrease_importance = 1.f)
  {
    spineDampingIncrease = _spineDampingIncrease;
    postAssignSpineDampingIncrease(spineDampingIncrease);
    NMP_ASSERT(spineDampingIncrease_importance >= 0.0f && spineDampingIncrease_importance <= 1.0f);
    m_spineDampingIncreaseImportance = spineDampingIncrease_importance;
  }
  NM_INLINE float getSpineDampingIncreaseImportance() const { return m_spineDampingIncreaseImportance; }
  NM_INLINE const float& getSpineDampingIncreaseImportanceRef() const { return m_spineDampingIncreaseImportance; }
  NM_INLINE const float& getSpineDampingIncrease() const { return spineDampingIncrease; }

  NM_INLINE void setNumConstrainedArms(const uint16_t& _numConstrainedArms, float numConstrainedArms_importance = 1.f)
  {
    numConstrainedArms = _numConstrainedArms;
    NMP_ASSERT(numConstrainedArms_importance >= 0.0f && numConstrainedArms_importance <= 1.0f);
    m_numConstrainedArmsImportance = numConstrainedArms_importance;
  }
  NM_INLINE float getNumConstrainedArmsImportance() const { return m_numConstrainedArmsImportance; }
  NM_INLINE const float& getNumConstrainedArmsImportanceRef() const { return m_numConstrainedArmsImportance; }
  NM_INLINE const uint16_t& getNumConstrainedArms() const { return numConstrainedArms; }

  NM_INLINE void setEdgeIsObstructed(const bool& _edgeIsObstructed, float edgeIsObstructed_importance = 1.f)
  {
    edgeIsObstructed = _edgeIsObstructed;
    NMP_ASSERT(edgeIsObstructed_importance >= 0.0f && edgeIsObstructed_importance <= 1.0f);
    m_edgeIsObstructedImportance = edgeIsObstructed_importance;
  }
  NM_INLINE float getEdgeIsObstructedImportance() const { return m_edgeIsObstructedImportance; }
  NM_INLINE const float& getEdgeIsObstructedImportanceRef() const { return m_edgeIsObstructedImportance; }
  NM_INLINE const bool& getEdgeIsObstructed() const { return edgeIsObstructed; }

  NM_INLINE void setEdgeOvershot(const bool& _edgeOvershot, float edgeOvershot_importance = 1.f)
  {
    edgeOvershot = _edgeOvershot;
    NMP_ASSERT(edgeOvershot_importance >= 0.0f && edgeOvershot_importance <= 1.0f);
    m_edgeOvershotImportance = edgeOvershot_importance;
  }
  NM_INLINE float getEdgeOvershotImportance() const { return m_edgeOvershotImportance; }
  NM_INLINE const float& getEdgeOvershotImportanceRef() const { return m_edgeOvershotImportance; }
  NM_INLINE const bool& getEdgeOvershot() const { return edgeOvershot; }

  NM_INLINE void setHasTwistedAround(const bool& _hasTwistedAround, float hasTwistedAround_importance = 1.f)
  {
    hasTwistedAround = _hasTwistedAround;
    NMP_ASSERT(hasTwistedAround_importance >= 0.0f && hasTwistedAround_importance <= 1.0f);
    m_hasTwistedAroundImportance = hasTwistedAround_importance;
  }
  NM_INLINE float getHasTwistedAroundImportance() const { return m_hasTwistedAroundImportance; }
  NM_INLINE const float& getHasTwistedAroundImportanceRef() const { return m_hasTwistedAroundImportance; }
  NM_INLINE const bool& getHasTwistedAround() const { return hasTwistedAround; }

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

  TranslationRequest translationRequest;  ///< Controls desired chest position.
  RotationRequest rotationRequest;  ///< Controls desired chest orientation.
  TargetRequest lookTarget;  ///< Where to look at (e.g. average grab point)
  Edge edgeToGrab;  ///< Passes on preprocessed edge information to individual arms (ArmGrab).
  EndConstraintControl endConstraintOptions;  ///< Desired properties of the physical constraint.
  NMP::Vector3 uprightContactDir;  ///< Request leg collision normals that are closest to upright.  (Direction)
  HoldTimer holdTimer;  ///< Contains Minimum and maximum constraint duration, as well as recovery period.
  float armSwingStiffnessScale;  ///< Used to suppress arm swing whilst holding  (Multiplier)
  float spineDampingIncrease;  ///< Large damping passed down so chest doesn't easily counter-rotate when the arms reach quickly  (Weight)
  uint16_t numConstrainedArms;  ///< Number of currently constrained hands.
  bool edgeIsObstructed;  ///< Edge ray-cast result to assess whether edge is obstructed by geometry out and below the edge.
  bool edgeOvershot;  ///< Set if the behaviour ignored the best edge because the chest was going over the top.
  bool hasTwistedAround;  ///< True if character is grabbing an edge behind him with more than one arm.
  bool isHanging;  ///< Is the character stably hanging (rather than still falling).

  // post-assignment stubs
  NM_INLINE void postAssignTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLookTarget(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignEdgeToGrab(const Edge& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignEndConstraintOptions(const EndConstraintControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignUprightContactDir(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "uprightContactDir");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHoldTimer(const HoldTimer& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmSwingStiffnessScale(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "armSwingStiffnessScale");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpineDampingIncrease(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "spineDampingIncrease");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_translationRequestImportance, 
     m_rotationRequestImportance, 
     m_lookTargetImportance, 
     m_edgeToGrabImportance, 
     m_endConstraintOptionsImportance, 
     m_uprightContactDirImportance, 
     m_holdTimerImportance, 
     m_armSwingStiffnessScaleImportance, 
     m_spineDampingIncreaseImportance, 
     m_numConstrainedArmsImportance, 
     m_edgeIsObstructedImportance, 
     m_edgeOvershotImportance, 
     m_hasTwistedAroundImportance, 
     m_isHangingImportance;

  friend class Grab_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTranslationRequestImportance() > 0.0f)
    {
      translationRequest.validate();
    }
    if (getRotationRequestImportance() > 0.0f)
    {
      rotationRequest.validate();
    }
    if (getLookTargetImportance() > 0.0f)
    {
      lookTarget.validate();
    }
    if (getEdgeToGrabImportance() > 0.0f)
    {
      edgeToGrab.validate();
    }
    if (getEndConstraintOptionsImportance() > 0.0f)
    {
      endConstraintOptions.validate();
    }
    if (getUprightContactDirImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(uprightContactDir), "uprightContactDir");
    }
    if (getHoldTimerImportance() > 0.0f)
    {
      holdTimer.validate();
    }
    if (getArmSwingStiffnessScaleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(armSwingStiffnessScale), "armSwingStiffnessScale");
    }
    if (getSpineDampingIncreaseImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(spineDampingIncrease), "spineDampingIncrease");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class GrabFeedbackInputs
 * \ingroup Grab
 * \brief FeedbackInputs try to hold onto environment objects for support (when falling e.g., or to help balancer)
 *
 * Data packet definition (486 bytes, 512 aligned)
 */
NMP_ALIGN_PREFIX(32) struct GrabFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(GrabFeedbackInputs));
  }

  NM_INLINE void setBraceHazard(const Hazard& _braceHazard, float braceHazard_importance = 1.f)
  {
    braceHazard = _braceHazard;
    postAssignBraceHazard(braceHazard);
    NMP_ASSERT(braceHazard_importance >= 0.0f && braceHazard_importance <= 1.0f);
    m_braceHazardImportance = braceHazard_importance;
  }
  NM_INLINE Hazard& startBraceHazardModification()
  {
    m_braceHazardImportance = -1.0f; // set invalid until stopBraceHazardModification()
    return braceHazard;
  }
  NM_INLINE void stopBraceHazardModification(float braceHazard_importance = 1.f)
  {
    postAssignBraceHazard(braceHazard);
    NMP_ASSERT(braceHazard_importance >= 0.0f && braceHazard_importance <= 1.0f);
    m_braceHazardImportance = braceHazard_importance;
  }
  NM_INLINE float getBraceHazardImportance() const { return m_braceHazardImportance; }
  NM_INLINE const float& getBraceHazardImportanceRef() const { return m_braceHazardImportance; }
  NM_INLINE const Hazard& getBraceHazard() const { return braceHazard; }

  NM_INLINE void setMostUprightContact(const ContactInfo& _mostUprightContact, float mostUprightContact_importance = 1.f)
  {
    mostUprightContact = _mostUprightContact;
    postAssignMostUprightContact(mostUprightContact);
    NMP_ASSERT(mostUprightContact_importance >= 0.0f && mostUprightContact_importance <= 1.0f);
    m_mostUprightContactImportance = mostUprightContact_importance;
  }
  NM_INLINE ContactInfo& startMostUprightContactModification()
  {
    m_mostUprightContactImportance = -1.0f; // set invalid until stopMostUprightContactModification()
    return mostUprightContact;
  }
  NM_INLINE void stopMostUprightContactModification(float mostUprightContact_importance = 1.f)
  {
    postAssignMostUprightContact(mostUprightContact);
    NMP_ASSERT(mostUprightContact_importance >= 0.0f && mostUprightContact_importance <= 1.0f);
    m_mostUprightContactImportance = mostUprightContact_importance;
  }
  NM_INLINE float getMostUprightContactImportance() const { return m_mostUprightContactImportance; }
  NM_INLINE const float& getMostUprightContactImportanceRef() const { return m_mostUprightContactImportance; }
  NM_INLINE const ContactInfo& getMostUprightContact() const { return mostUprightContact; }

  NM_INLINE void setCentreOfShoulders(const NMP::Vector3& _centreOfShoulders, float centreOfShoulders_importance = 1.f)
  {
    centreOfShoulders = _centreOfShoulders;
    postAssignCentreOfShoulders(centreOfShoulders);
    NMP_ASSERT(centreOfShoulders_importance >= 0.0f && centreOfShoulders_importance <= 1.0f);
    m_centreOfShouldersImportance = centreOfShoulders_importance;
  }
  NM_INLINE float getCentreOfShouldersImportance() const { return m_centreOfShouldersImportance; }
  NM_INLINE const float& getCentreOfShouldersImportanceRef() const { return m_centreOfShouldersImportance; }
  NM_INLINE const NMP::Vector3& getCentreOfShoulders() const { return centreOfShoulders; }

  NM_INLINE void setSpineInContact(const bool& _spineInContact, float spineInContact_importance = 1.f)
  {
    spineInContact = _spineInContact;
    NMP_ASSERT(spineInContact_importance >= 0.0f && spineInContact_importance <= 1.0f);
    m_spineInContactImportance = spineInContact_importance;
  }
  NM_INLINE float getSpineInContactImportance() const { return m_spineInContactImportance; }
  NM_INLINE const float& getSpineInContactImportanceRef() const { return m_spineInContactImportance; }
  NM_INLINE const bool& getSpineInContact() const { return spineInContact; }

  NM_INLINE void setLegsInContact(const bool& _legsInContact, float legsInContact_importance = 1.f)
  {
    legsInContact = _legsInContact;
    NMP_ASSERT(legsInContact_importance >= 0.0f && legsInContact_importance <= 1.0f);
    m_legsInContactImportance = legsInContact_importance;
  }
  NM_INLINE float getLegsInContactImportance() const { return m_legsInContactImportance; }
  NM_INLINE const float& getLegsInContactImportanceRef() const { return m_legsInContactImportance; }
  NM_INLINE const bool& getLegsInContact() const { return legsInContact; }

protected:

  Hazard braceHazard;  ///< Brace's current target, to check whether brace is more important.
  ContactInfo mostUprightContact;  ///< Receives the most upright contact normal of current leg collisions
  NMP::Vector3 centreOfShoulders;  ///< Center of shoulders.  (Position)
  bool spineInContact;  ///< Is the spine currently in contact with the environment?
  bool legsInContact;  ///< Are the legs currently in contact with the environment?

  // post-assignment stubs
  NM_INLINE void postAssignBraceHazard(const Hazard& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignMostUprightContact(const ContactInfo& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignCentreOfShoulders(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "centreOfShoulders");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_braceHazardImportance, 
     m_mostUprightContactImportance, 
     m_centreOfShouldersImportance, 
     m_spineInContactImportance, 
     m_legsInContactImportance;

  friend class Grab_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getBraceHazardImportance() > 0.0f)
    {
      braceHazard.validate();
    }
    if (getMostUprightContactImportance() > 0.0f)
    {
      mostUprightContact.validate();
    }
    if (getCentreOfShouldersImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(centreOfShoulders), "centreOfShoulders");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class GrabFeedbackOutputs
 * \ingroup Grab
 * \brief FeedbackOutputs try to hold onto environment objects for support (when falling e.g., or to help balancer)
 *
 * Data packet definition (47 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct GrabFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(GrabFeedbackOutputs));
  }

  NM_INLINE void setEdgeForwardNormal(const NMP::Vector3& _edgeForwardNormal, float edgeForwardNormal_importance = 1.f)
  {
    edgeForwardNormal = _edgeForwardNormal;
    postAssignEdgeForwardNormal(edgeForwardNormal);
    NMP_ASSERT(edgeForwardNormal_importance >= 0.0f && edgeForwardNormal_importance <= 1.0f);
    m_edgeForwardNormalImportance = edgeForwardNormal_importance;
  }
  NM_INLINE float getEdgeForwardNormalImportance() const { return m_edgeForwardNormalImportance; }
  NM_INLINE const float& getEdgeForwardNormalImportanceRef() const { return m_edgeForwardNormalImportance; }
  NM_INLINE const NMP::Vector3& getEdgeForwardNormal() const { return edgeForwardNormal; }

  NM_INLINE void setHoldAttemptImportance(const float& _holdAttemptImportance, float holdAttemptImportance_importance = 1.f)
  {
    holdAttemptImportance = _holdAttemptImportance;
    postAssignHoldAttemptImportance(holdAttemptImportance);
    NMP_ASSERT(holdAttemptImportance_importance >= 0.0f && holdAttemptImportance_importance <= 1.0f);
    m_holdAttemptImportanceImportance = holdAttemptImportance_importance;
  }
  NM_INLINE float getHoldAttemptImportanceImportance() const { return m_holdAttemptImportanceImportance; }
  NM_INLINE const float& getHoldAttemptImportanceImportanceRef() const { return m_holdAttemptImportanceImportance; }
  NM_INLINE const float& getHoldAttemptImportance() const { return holdAttemptImportance; }

  NM_INLINE void setHoldDuration(const float& _holdDuration, float holdDuration_importance = 1.f)
  {
    holdDuration = _holdDuration;
    postAssignHoldDuration(holdDuration);
    NMP_ASSERT(holdDuration_importance >= 0.0f && holdDuration_importance <= 1.0f);
    m_holdDurationImportance = holdDuration_importance;
  }
  NM_INLINE float getHoldDurationImportance() const { return m_holdDurationImportance; }
  NM_INLINE const float& getHoldDurationImportanceRef() const { return m_holdDurationImportance; }
  NM_INLINE const float& getHoldDuration() const { return holdDuration; }

  NM_INLINE void setNumConstrainedArms(const uint16_t& _numConstrainedArms, float numConstrainedArms_importance = 1.f)
  {
    numConstrainedArms = _numConstrainedArms;
    NMP_ASSERT(numConstrainedArms_importance >= 0.0f && numConstrainedArms_importance <= 1.0f);
    m_numConstrainedArmsImportance = numConstrainedArms_importance;
  }
  NM_INLINE float getNumConstrainedArmsImportance() const { return m_numConstrainedArmsImportance; }
  NM_INLINE const float& getNumConstrainedArmsImportanceRef() const { return m_numConstrainedArmsImportance; }
  NM_INLINE const uint16_t& getNumConstrainedArms() const { return numConstrainedArms; }

  NM_INLINE void setUseEnvironmentAwareness(const bool& _useEnvironmentAwareness, float useEnvironmentAwareness_importance = 1.f)
  {
    useEnvironmentAwareness = _useEnvironmentAwareness;
    NMP_ASSERT(useEnvironmentAwareness_importance >= 0.0f && useEnvironmentAwareness_importance <= 1.0f);
    m_useEnvironmentAwarenessImportance = useEnvironmentAwareness_importance;
  }
  NM_INLINE float getUseEnvironmentAwarenessImportance() const { return m_useEnvironmentAwarenessImportance; }
  NM_INLINE const float& getUseEnvironmentAwarenessImportanceRef() const { return m_useEnvironmentAwarenessImportance; }
  NM_INLINE const bool& getUseEnvironmentAwareness() const { return useEnvironmentAwareness; }

protected:

  NMP::Vector3 edgeForwardNormal;  ///< The normal to the front surface of the edge (as opposed to the upper surface).  (Direction)
  float holdAttemptImportance;  ///< How important Hold thinks it is to grab the current edge.  (Weight)
  float holdDuration;  ///< How long the character has been constrained.  (TimePeriod)
  uint16_t numConstrainedArms;  ///< Number of currently constrained hands.
  bool useEnvironmentAwareness;  ///< Whether or not to look for grabbable edges.

  // post-assignment stubs
  NM_INLINE void postAssignEdgeForwardNormal(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "edgeForwardNormal");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHoldAttemptImportance(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "holdAttemptImportance");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHoldDuration(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "holdDuration");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_edgeForwardNormalImportance, 
     m_holdAttemptImportanceImportance, 
     m_holdDurationImportance, 
     m_numConstrainedArmsImportance, 
     m_useEnvironmentAwarenessImportance;

  friend class Grab_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getEdgeForwardNormalImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(edgeForwardNormal), "edgeForwardNormal");
    }
    if (getHoldAttemptImportanceImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(holdAttemptImportance), "holdAttemptImportance");
    }
    if (getHoldDurationImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(holdDuration), "holdDuration");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_GRAB_DATA_H

