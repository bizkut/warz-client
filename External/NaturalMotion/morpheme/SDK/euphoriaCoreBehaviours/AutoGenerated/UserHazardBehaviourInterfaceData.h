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

#ifndef NM_MDF_USERHAZARDBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_USERHAZARDBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/UserHazardBehaviourInterface.module"

// known types
#include "Types/PatchSet.h"
#include "Types/Hazard.h"

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
 * \class UserHazardBehaviourInterfaceUserInputs
 * \ingroup UserHazardBehaviourInterface
 * \brief UserInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (740 bytes, 768 aligned)
 */
NMP_ALIGN_PREFIX(32) struct UserHazardBehaviourInterfaceUserInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(UserHazardBehaviourInterfaceUserInputs));
  }

  NM_INLINE void setGameHazards(const PatchSet& _gameHazards, float gameHazards_importance = 1.f)
  {
    gameHazards = _gameHazards;
    NMP_ASSERT(gameHazards_importance >= 0.0f && gameHazards_importance <= 1.0f);
    m_gameHazardsImportance = gameHazards_importance;
  }
  NM_INLINE PatchSet& startGameHazardsModification()
  {
    m_gameHazardsImportance = -1.0f; // set invalid until stopGameHazardsModification()
    return gameHazards;
  }
  NM_INLINE void stopGameHazardsModification(float gameHazards_importance = 1.f)
  {
    NMP_ASSERT(gameHazards_importance >= 0.0f && gameHazards_importance <= 1.0f);
    m_gameHazardsImportance = gameHazards_importance;
  }
  NM_INLINE float getGameHazardsImportance() const { return m_gameHazardsImportance; }
  NM_INLINE const float& getGameHazardsImportanceRef() const { return m_gameHazardsImportance; }
  NM_INLINE const PatchSet& getGameHazards() const { return gameHazards; }

protected:

  PatchSet gameHazards;  ///< Potential hazard can be written directly into this structure by client code.

  // importance values
  float
     m_gameHazardsImportance;

  friend class UserHazardBehaviourInterface_Con;

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
 * \class UserHazardBehaviourInterfaceOutputs
 * \ingroup UserHazardBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (745 bytes, 768 aligned)
 */
NMP_ALIGN_PREFIX(32) struct UserHazardBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(UserHazardBehaviourInterfaceOutputs));
  }

  NM_INLINE void setPotentialHazards(const PatchSet& _potentialHazards, float potentialHazards_importance = 1.f)
  {
    potentialHazards = _potentialHazards;
    NMP_ASSERT(potentialHazards_importance >= 0.0f && potentialHazards_importance <= 1.0f);
    m_potentialHazardsImportance = potentialHazards_importance;
  }
  NM_INLINE PatchSet& startPotentialHazardsModification()
  {
    m_potentialHazardsImportance = -1.0f; // set invalid until stopPotentialHazardsModification()
    return potentialHazards;
  }
  NM_INLINE void stopPotentialHazardsModification(float potentialHazards_importance = 1.f)
  {
    NMP_ASSERT(potentialHazards_importance >= 0.0f && potentialHazards_importance <= 1.0f);
    m_potentialHazardsImportance = potentialHazards_importance;
  }
  NM_INLINE float getPotentialHazardsImportance() const { return m_potentialHazardsImportance; }
  NM_INLINE const float& getPotentialHazardsImportanceRef() const { return m_potentialHazardsImportance; }
  NM_INLINE const PatchSet& getPotentialHazards() const { return potentialHazards; }

  NM_INLINE void setIgnoreNonUserHazards(const bool& _ignoreNonUserHazards, float ignoreNonUserHazards_importance = 1.f)
  {
    ignoreNonUserHazards = _ignoreNonUserHazards;
    NMP_ASSERT(ignoreNonUserHazards_importance >= 0.0f && ignoreNonUserHazards_importance <= 1.0f);
    m_ignoreNonUserHazardsImportance = ignoreNonUserHazards_importance;
  }
  NM_INLINE float getIgnoreNonUserHazardsImportance() const { return m_ignoreNonUserHazardsImportance; }
  NM_INLINE const float& getIgnoreNonUserHazardsImportanceRef() const { return m_ignoreNonUserHazardsImportance; }
  NM_INLINE const bool& getIgnoreNonUserHazards() const { return ignoreNonUserHazards; }

protected:

  PatchSet potentialHazards;
  bool ignoreNonUserHazards;

  // importance values
  float
     m_potentialHazardsImportance, 
     m_ignoreNonUserHazardsImportance;

  friend class UserHazardBehaviourInterface_Con;

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
 * \class UserHazardBehaviourInterfaceFeedbackInputs
 * \ingroup UserHazardBehaviourInterface
 * \brief FeedbackInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (372 bytes, 384 aligned)
 */
NMP_ALIGN_PREFIX(32) struct UserHazardBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(UserHazardBehaviourInterfaceFeedbackInputs));
  }

  NM_INLINE void setHazard(const Hazard& _hazard, float hazard_importance = 1.f)
  {
    hazard = _hazard;
    postAssignHazard(hazard);
    NMP_ASSERT(hazard_importance >= 0.0f && hazard_importance <= 1.0f);
    m_hazardImportance = hazard_importance;
  }
  NM_INLINE Hazard& startHazardModification()
  {
    m_hazardImportance = -1.0f; // set invalid until stopHazardModification()
    return hazard;
  }
  NM_INLINE void stopHazardModification(float hazard_importance = 1.f)
  {
    postAssignHazard(hazard);
    NMP_ASSERT(hazard_importance >= 0.0f && hazard_importance <= 1.0f);
    m_hazardImportance = hazard_importance;
  }
  NM_INLINE float getHazardImportance() const { return m_hazardImportance; }
  NM_INLINE const float& getHazardImportanceRef() const { return m_hazardImportance; }
  NM_INLINE const Hazard& getHazard() const { return hazard; }

protected:

  Hazard hazard;

  // post-assignment stubs
  NM_INLINE void postAssignHazard(const Hazard& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_hazardImportance;

  friend class UserHazardBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getHazardImportance() > 0.0f)
    {
      hazard.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_USERHAZARDBEHAVIOURINTERFACE_DATA_H

