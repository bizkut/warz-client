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

#ifndef NM_MDF_HOLDACTIONBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_HOLDACTIONBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/HoldActionBehaviourInterface.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/Edge.h"
#include "Types/GrabControl.h"
#include "Types/GrabDetectionParams.h"
#include "Types/GrabEnableConditions.h"
#include "Types/GrabFailureConditions.h"
#include "Types/GrabAliveParams.h"

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
 * \class HoldActionBehaviourInterfaceUserInputs
 * \ingroup HoldActionBehaviourInterface
 * \brief UserInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (1000 bytes, 1024 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HoldActionBehaviourInterfaceUserInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HoldActionBehaviourInterfaceUserInputs));
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

protected:

  Edge gameEdges[10];

  // post-assignment stubs
  NM_INLINE void postAssignGameEdges(const Edge& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_gameEdgesImportance[10];

  friend class HoldActionBehaviourInterface_Con;

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
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HoldActionBehaviourInterfaceOutputs
 * \ingroup HoldActionBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (269 bytes, 288 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HoldActionBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HoldActionBehaviourInterfaceOutputs));
  }

  NM_INLINE void setGrabControl(const GrabControl& _grabControl, float grabControl_importance = 1.f)
  {
    grabControl = _grabControl;
    NMP_ASSERT(grabControl_importance >= 0.0f && grabControl_importance <= 1.0f);
    m_grabControlImportance = grabControl_importance;
  }
  NM_INLINE GrabControl& startGrabControlModification()
  {
    m_grabControlImportance = -1.0f; // set invalid until stopGrabControlModification()
    return grabControl;
  }
  NM_INLINE void stopGrabControlModification(float grabControl_importance = 1.f)
  {
    NMP_ASSERT(grabControl_importance >= 0.0f && grabControl_importance <= 1.0f);
    m_grabControlImportance = grabControl_importance;
  }
  NM_INLINE float getGrabControlImportance() const { return m_grabControlImportance; }
  NM_INLINE const float& getGrabControlImportanceRef() const { return m_grabControlImportance; }
  NM_INLINE const GrabControl& getGrabControl() const { return grabControl; }

  NM_INLINE void setGrabDetectionParams(const GrabDetectionParams& _grabDetectionParams, float grabDetectionParams_importance = 1.f)
  {
    grabDetectionParams = _grabDetectionParams;
    postAssignGrabDetectionParams(grabDetectionParams);
    NMP_ASSERT(grabDetectionParams_importance >= 0.0f && grabDetectionParams_importance <= 1.0f);
    m_grabDetectionParamsImportance = grabDetectionParams_importance;
  }
  NM_INLINE float getGrabDetectionParamsImportance() const { return m_grabDetectionParamsImportance; }
  NM_INLINE const float& getGrabDetectionParamsImportanceRef() const { return m_grabDetectionParamsImportance; }
  NM_INLINE const GrabDetectionParams& getGrabDetectionParams() const { return grabDetectionParams; }

  NM_INLINE void setGrabEnableConditions(const GrabEnableConditions& _grabEnableConditions, float grabEnableConditions_importance = 1.f)
  {
    grabEnableConditions = _grabEnableConditions;
    postAssignGrabEnableConditions(grabEnableConditions);
    NMP_ASSERT(grabEnableConditions_importance >= 0.0f && grabEnableConditions_importance <= 1.0f);
    m_grabEnableConditionsImportance = grabEnableConditions_importance;
  }
  NM_INLINE float getGrabEnableConditionsImportance() const { return m_grabEnableConditionsImportance; }
  NM_INLINE const float& getGrabEnableConditionsImportanceRef() const { return m_grabEnableConditionsImportance; }
  NM_INLINE const GrabEnableConditions& getGrabEnableConditions() const { return grabEnableConditions; }

  NM_INLINE void setGrabFailureConditions(const GrabFailureConditions& _grabFailureConditions, float grabFailureConditions_importance = 1.f)
  {
    grabFailureConditions = _grabFailureConditions;
    postAssignGrabFailureConditions(grabFailureConditions);
    NMP_ASSERT(grabFailureConditions_importance >= 0.0f && grabFailureConditions_importance <= 1.0f);
    m_grabFailureConditionsImportance = grabFailureConditions_importance;
  }
  NM_INLINE float getGrabFailureConditionsImportance() const { return m_grabFailureConditionsImportance; }
  NM_INLINE const float& getGrabFailureConditionsImportanceRef() const { return m_grabFailureConditionsImportance; }
  NM_INLINE const GrabFailureConditions& getGrabFailureConditions() const { return grabFailureConditions; }

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

  enum { maxHoldPerArmImportance = 2 };
  NM_INLINE unsigned int getMaxHoldPerArmImportance() const { return maxHoldPerArmImportance; }
  NM_INLINE void setHoldPerArmImportance(unsigned int number, const float* _holdPerArmImportance, float holdPerArmImportance_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHoldPerArmImportance);
    NMP_ASSERT(holdPerArmImportance_importance >= 0.0f && holdPerArmImportance_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      holdPerArmImportance[i] = _holdPerArmImportance[i];
      postAssignHoldPerArmImportance(holdPerArmImportance[i]);
      m_holdPerArmImportanceImportance[i] = holdPerArmImportance_importance;
    }
  }
  NM_INLINE void setHoldPerArmImportanceAt(unsigned int index, const float& _holdPerArmImportance, float holdPerArmImportance_importance = 1.f)
  {
    NMP_ASSERT(index < maxHoldPerArmImportance);
    holdPerArmImportance[index] = _holdPerArmImportance;
    NMP_ASSERT(holdPerArmImportance_importance >= 0.0f && holdPerArmImportance_importance <= 1.0f);
    postAssignHoldPerArmImportance(holdPerArmImportance[index]);
    m_holdPerArmImportanceImportance[index] = holdPerArmImportance_importance;
  }
  NM_INLINE float getHoldPerArmImportanceImportance(int index) const { return m_holdPerArmImportanceImportance[index]; }
  NM_INLINE const float& getHoldPerArmImportanceImportanceRef(int index) const { return m_holdPerArmImportanceImportance[index]; }
  NM_INLINE unsigned int calculateNumHoldPerArmImportance() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_holdPerArmImportanceImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getHoldPerArmImportance(unsigned int index) const { NMP_ASSERT(index <= maxHoldPerArmImportance); return holdPerArmImportance[index]; }

  NM_INLINE void setHasForcedEdge(const bool& _hasForcedEdge, float hasForcedEdge_importance = 1.f)
  {
    hasForcedEdge = _hasForcedEdge;
    NMP_ASSERT(hasForcedEdge_importance >= 0.0f && hasForcedEdge_importance <= 1.0f);
    m_hasForcedEdgeImportance = hasForcedEdge_importance;
  }
  NM_INLINE float getHasForcedEdgeImportance() const { return m_hasForcedEdgeImportance; }
  NM_INLINE const float& getHasForcedEdgeImportanceRef() const { return m_hasForcedEdgeImportance; }
  NM_INLINE const bool& getHasForcedEdge() const { return hasForcedEdge; }

protected:

  GrabControl grabControl;  ///< Wraps EndConstraint parameters and timers into a single structure.
  GrabDetectionParams grabDetectionParams;  ///< Configures what kind of edges to look for. 
  GrabEnableConditions grabEnableConditions;  ///< Configures the conditions for starting a hold attempt.
  GrabFailureConditions grabFailureConditions;  ///< Configures the conditions for aborting a hold attempt.
  GrabAliveParams grabAliveParams;  ///< Configures behaviour while hanging (e.g. "pullUps")
  float timeRatioBeforeLookingDown;  ///< Ratio of maxHoldTime at which the character starts looking at ground  (Weight)
  float maxTwistBwdPeriod;  ///< If character is not facing the edge for this long he will give up  (TimePeriod)
  float holdPerArmImportance[NetworkConstants::networkMaxNumArms];  ///< Toggle grab per limb  
  bool hasForcedEdge;  ///< If true, would overwrite any EA patches, therefore no need to use EA.

  // post-assignment stubs
  NM_INLINE void postAssignGrabDetectionParams(const GrabDetectionParams& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignGrabEnableConditions(const GrabEnableConditions& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignGrabFailureConditions(const GrabFailureConditions& NMP_VALIDATOR_ARG(v)) const
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

  NM_INLINE void postAssignTimeRatioBeforeLookingDown(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "timeRatioBeforeLookingDown");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignMaxTwistBwdPeriod(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "maxTwistBwdPeriod");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHoldPerArmImportance(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "holdPerArmImportance");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_grabControlImportance, 
     m_grabDetectionParamsImportance, 
     m_grabEnableConditionsImportance, 
     m_grabFailureConditionsImportance, 
     m_grabAliveParamsImportance, 
     m_timeRatioBeforeLookingDownImportance, 
     m_maxTwistBwdPeriodImportance, 
     m_holdPerArmImportanceImportance[NetworkConstants::networkMaxNumArms], 
     m_hasForcedEdgeImportance;

  friend class HoldActionBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getGrabDetectionParamsImportance() > 0.0f)
    {
      grabDetectionParams.validate();
    }
    if (getGrabEnableConditionsImportance() > 0.0f)
    {
      grabEnableConditions.validate();
    }
    if (getGrabFailureConditionsImportance() > 0.0f)
    {
      grabFailureConditions.validate();
    }
    if (getGrabAliveParamsImportance() > 0.0f)
    {
      grabAliveParams.validate();
    }
    if (getTimeRatioBeforeLookingDownImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(timeRatioBeforeLookingDown), "timeRatioBeforeLookingDown");
    }
    if (getMaxTwistBwdPeriodImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(maxTwistBwdPeriod), "maxTwistBwdPeriod");
    }
    {
      uint32_t numHoldPerArmImportance = calculateNumHoldPerArmImportance();
      for (uint32_t i=0; i<numHoldPerArmImportance; i++)
      {
        if (getHoldPerArmImportanceImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(holdPerArmImportance[i]), "holdPerArmImportance");
        }
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class HoldActionBehaviourInterfaceFeedbackInputs
 * \ingroup HoldActionBehaviourInterface
 * \brief FeedbackInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (42 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HoldActionBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HoldActionBehaviourInterfaceFeedbackInputs));
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

protected:

  NMP::Vector3 edgeForwardNormal;  ///< The normal to the front surface of the edge (as opposed to the upper surface).  (Direction)
  float holdAttemptImportance;  ///< How important Hold thinks it is to grab the current edge.  (Weight)
  float holdDuration;  ///< How long the character has been constrained  (TimePeriod)
  uint16_t numConstrainedArms;  ///< Number of currently constrained hands.

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
     m_numConstrainedArmsImportance;

  friend class HoldActionBehaviourInterface_Con;

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
/** 
 * \class HoldActionBehaviourInterfaceFeedbackOutputs
 * \ingroup HoldActionBehaviourInterface
 * \brief FeedbackOutputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (6 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct HoldActionBehaviourInterfaceFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(HoldActionBehaviourInterfaceFeedbackOutputs));
  }

  NM_INLINE void setNumConstrainedArms(const uint16_t& _numConstrainedArms, float numConstrainedArms_importance = 1.f)
  {
    numConstrainedArms = _numConstrainedArms;
    NMP_ASSERT(numConstrainedArms_importance >= 0.0f && numConstrainedArms_importance <= 1.0f);
    m_numConstrainedArmsImportance = numConstrainedArms_importance;
  }
  NM_INLINE float getNumConstrainedArmsImportance() const { return m_numConstrainedArmsImportance; }
  NM_INLINE const float& getNumConstrainedArmsImportanceRef() const { return m_numConstrainedArmsImportance; }
  NM_INLINE const uint16_t& getNumConstrainedArms() const { return numConstrainedArms; }

protected:

  uint16_t numConstrainedArms;  ///< Number of currently constrained hands. Simply passed on.

  // importance values
  float
     m_numConstrainedArmsImportance;

  friend class HoldActionBehaviourInterface_Con;

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

#endif // NM_MDF_HOLDACTIONBEHAVIOURINTERFACE_DATA_H

