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

#ifndef NM_MDF_REACHFORBODY_DATA_H
#define NM_MDF_REACHFORBODY_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ReachForBody.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/BraceHazard.h"
#include "Types/ReachActionParams.h"
#include "Types/BodyHitInfo.h"
#include "Types/LimbControl.h"
#include "Types/ReachTarget.h"
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
 * \class ReachForBodyData
 * \ingroup ReachForBody
 * \brief Data *
 * Data packet definition (421 bytes, 448 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ReachForBodyData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ReachForBodyData));
  }

  ER::HandFootTransform endEffectorTMs[NetworkConstants::networkMaxNumArms];
  ReachTarget rt[NetworkConstants::networkMaxNumArms];
  NMP::Vector3 shoulderPositions[NetworkConstants::networkMaxNumArms];  ///< (Position)
  NMP::Vector3 weightedAverageEffectorPos;   ///< (Position)
  ReachActionParams reachActionParams[NetworkConstants::networkMaxNumArms];
  float reachTargetImportances[NetworkConstants::networkMaxNumArms];  ///< (Weight)
  float endEffectorDistanceSquaredToTargets[NetworkConstants::networkMaxNumArms];
  float armStrengthReduction[NetworkConstants::networkMaxNumArms];  ///< (Weight)
  float competingControlImportances[NetworkConstants::networkMaxNumArms];
  uint32_t numArmsReaching;
  float averageReachStrength;                ///< (Weight)
  bool haveReachTarget;
  bool armReachForBodyPartFlags[NetworkConstants::networkMaxNumArms];
  bool isReaching[NetworkConstants::networkMaxNumArms];

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    rt[0].validate();
    rt[1].validate();
    NM_VALIDATOR(Vector3Valid(shoulderPositions[0]), "shoulderPositions[0]");
    NM_VALIDATOR(Vector3Valid(shoulderPositions[1]), "shoulderPositions[1]");
    NM_VALIDATOR(Vector3Valid(weightedAverageEffectorPos), "weightedAverageEffectorPos");
    reachActionParams[0].validate();
    reachActionParams[1].validate();
    NM_VALIDATOR(FloatNonNegative(reachTargetImportances[0]), "reachTargetImportances[0]");
    NM_VALIDATOR(FloatNonNegative(reachTargetImportances[1]), "reachTargetImportances[1]");
    NM_VALIDATOR(FloatValid(endEffectorDistanceSquaredToTargets[0]), "endEffectorDistanceSquaredToTargets[0]");
    NM_VALIDATOR(FloatValid(endEffectorDistanceSquaredToTargets[1]), "endEffectorDistanceSquaredToTargets[1]");
    NM_VALIDATOR(FloatNonNegative(armStrengthReduction[0]), "armStrengthReduction[0]");
    NM_VALIDATOR(FloatNonNegative(armStrengthReduction[1]), "armStrengthReduction[1]");
    NM_VALIDATOR(FloatValid(competingControlImportances[0]), "competingControlImportances[0]");
    NM_VALIDATOR(FloatValid(competingControlImportances[1]), "competingControlImportances[1]");
    NM_VALIDATOR(FloatNonNegative(averageReachStrength), "averageReachStrength");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ReachForBodyInputs
 * \ingroup ReachForBody
 * \brief Inputs *
 * Data packet definition (414 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ReachForBodyInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ReachForBodyInputs));
  }

  NM_INLINE void setBraceHazard(const BraceHazard& _braceHazard, float braceHazard_importance = 1.f)
  {
    braceHazard = _braceHazard;
    postAssignBraceHazard(braceHazard);
    NMP_ASSERT(braceHazard_importance >= 0.0f && braceHazard_importance <= 1.0f);
    m_braceHazardImportance = braceHazard_importance;
  }
  NM_INLINE BraceHazard& startBraceHazardModification()
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
  NM_INLINE const BraceHazard& getBraceHazard() const { return braceHazard; }

  enum { maxReachActionParams = 2 };
  NM_INLINE unsigned int getMaxReachActionParams() const { return maxReachActionParams; }
  NM_INLINE void setReachActionParams(unsigned int number, const ReachActionParams* _reachActionParams, float reachActionParams_importance = 1.f)
  {
    NMP_ASSERT(number <= maxReachActionParams);
    NMP_ASSERT(reachActionParams_importance >= 0.0f && reachActionParams_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      reachActionParams[i] = _reachActionParams[i];
      postAssignReachActionParams(reachActionParams[i]);
      m_reachActionParamsImportance[i] = reachActionParams_importance;
    }
  }
  NM_INLINE void setReachActionParamsAt(unsigned int index, const ReachActionParams& _reachActionParams, float reachActionParams_importance = 1.f)
  {
    NMP_ASSERT(index < maxReachActionParams);
    reachActionParams[index] = _reachActionParams;
    NMP_ASSERT(reachActionParams_importance >= 0.0f && reachActionParams_importance <= 1.0f);
    postAssignReachActionParams(reachActionParams[index]);
    m_reachActionParamsImportance[index] = reachActionParams_importance;
  }
  NM_INLINE float getReachActionParamsImportance(int index) const { return m_reachActionParamsImportance[index]; }
  NM_INLINE const float& getReachActionParamsImportanceRef(int index) const { return m_reachActionParamsImportance[index]; }
  NM_INLINE unsigned int calculateNumReachActionParams() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_reachActionParamsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const ReachActionParams& getReachActionParams(unsigned int index) const { NMP_ASSERT(index <= maxReachActionParams); return reachActionParams[index]; }

  enum { maxArmReachForBodyPartFlags = 2 };
  NM_INLINE unsigned int getMaxArmReachForBodyPartFlags() const { return maxArmReachForBodyPartFlags; }
  NM_INLINE void setArmReachForBodyPartFlags(unsigned int number, const bool* _armReachForBodyPartFlags, float armReachForBodyPartFlags_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmReachForBodyPartFlags);
    NMP_ASSERT(armReachForBodyPartFlags_importance >= 0.0f && armReachForBodyPartFlags_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armReachForBodyPartFlags[i] = _armReachForBodyPartFlags[i];
      m_armReachForBodyPartFlagsImportance[i] = armReachForBodyPartFlags_importance;
    }
  }
  NM_INLINE void setArmReachForBodyPartFlagsAt(unsigned int index, const bool& _armReachForBodyPartFlags, float armReachForBodyPartFlags_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmReachForBodyPartFlags);
    armReachForBodyPartFlags[index] = _armReachForBodyPartFlags;
    NMP_ASSERT(armReachForBodyPartFlags_importance >= 0.0f && armReachForBodyPartFlags_importance <= 1.0f);
    m_armReachForBodyPartFlagsImportance[index] = armReachForBodyPartFlags_importance;
  }
  NM_INLINE float getArmReachForBodyPartFlagsImportance(int index) const { return m_armReachForBodyPartFlagsImportance[index]; }
  NM_INLINE const float& getArmReachForBodyPartFlagsImportanceRef(int index) const { return m_armReachForBodyPartFlagsImportance[index]; }
  NM_INLINE unsigned int calculateNumArmReachForBodyPartFlags() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armReachForBodyPartFlagsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const bool& getArmReachForBodyPartFlags(unsigned int index) const { NMP_ASSERT(index <= maxArmReachForBodyPartFlags); return armReachForBodyPartFlags[index]; }

protected:

  BraceHazard braceHazard;  ///< from hazardResponse: may yield an arm if importance is high
  ReachActionParams reachActionParams[NetworkConstants::networkMaxNumArms];
  bool armReachForBodyPartFlags[NetworkConstants::networkMaxNumArms];  ///< from reachForBodyBehaviourInterface: eg. game side requirement

  // post-assignment stubs
  NM_INLINE void postAssignBraceHazard(const BraceHazard& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignReachActionParams(const ReachActionParams& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_braceHazardImportance, 
     m_reachActionParamsImportance[NetworkConstants::networkMaxNumArms], 
     m_armReachForBodyPartFlagsImportance[NetworkConstants::networkMaxNumArms];

  friend class ReachForBody_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getBraceHazardImportance() > 0.0f)
    {
      braceHazard.validate();
    }
    {
      uint32_t numReachActionParams = calculateNumReachActionParams();
      for (uint32_t i=0; i<numReachActionParams; i++)
      {
        if (getReachActionParamsImportance(i) > 0.0f)
          reachActionParams[i].validate();
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ReachForBodyOutputs
 * \ingroup ReachForBody
 * \brief Outputs *
 * Data packet definition (462 bytes, 480 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ReachForBodyOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ReachForBodyOutputs));
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

  NM_INLINE void setReachTarget(const ReachTarget& _reachTarget, float reachTarget_importance = 1.f)
  {
    reachTarget = _reachTarget;
    postAssignReachTarget(reachTarget);
    NMP_ASSERT(reachTarget_importance >= 0.0f && reachTarget_importance <= 1.0f);
    m_reachTargetImportance = reachTarget_importance;
  }
  NM_INLINE float getReachTargetImportance() const { return m_reachTargetImportance; }
  NM_INLINE const float& getReachTargetImportanceRef() const { return m_reachTargetImportance; }
  NM_INLINE const ReachTarget& getReachTarget() const { return reachTarget; }

  enum { maxReachTargets = 2 };
  NM_INLINE unsigned int getMaxReachTargets() const { return maxReachTargets; }
  NM_INLINE void setReachTargets(unsigned int number, const ReachTarget* _reachTargets, float reachTargets_importance = 1.f)
  {
    NMP_ASSERT(number <= maxReachTargets);
    NMP_ASSERT(reachTargets_importance >= 0.0f && reachTargets_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      reachTargets[i] = _reachTargets[i];
      postAssignReachTargets(reachTargets[i]);
      m_reachTargetsImportance[i] = reachTargets_importance;
    }
  }
  NM_INLINE void setReachTargetsAt(unsigned int index, const ReachTarget& _reachTargets, float reachTargets_importance = 1.f)
  {
    NMP_ASSERT(index < maxReachTargets);
    reachTargets[index] = _reachTargets;
    NMP_ASSERT(reachTargets_importance >= 0.0f && reachTargets_importance <= 1.0f);
    postAssignReachTargets(reachTargets[index]);
    m_reachTargetsImportance[index] = reachTargets_importance;
  }
  NM_INLINE float getReachTargetsImportance(int index) const { return m_reachTargetsImportance[index]; }
  NM_INLINE const float& getReachTargetsImportanceRef(int index) const { return m_reachTargetsImportance[index]; }
  NM_INLINE unsigned int calculateNumReachTargets() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_reachTargetsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const ReachTarget& getReachTargets(unsigned int index) const { NMP_ASSERT(index <= maxReachTargets); return reachTargets[index]; }

  NM_INLINE void setErrorWeightedAverageEffectorPosition(const NMP::Vector3& _errorWeightedAverageEffectorPosition, float errorWeightedAverageEffectorPosition_importance = 1.f)
  {
    errorWeightedAverageEffectorPosition = _errorWeightedAverageEffectorPosition;
    postAssignErrorWeightedAverageEffectorPosition(errorWeightedAverageEffectorPosition);
    NMP_ASSERT(errorWeightedAverageEffectorPosition_importance >= 0.0f && errorWeightedAverageEffectorPosition_importance <= 1.0f);
    m_errorWeightedAverageEffectorPositionImportance = errorWeightedAverageEffectorPosition_importance;
  }
  NM_INLINE float getErrorWeightedAverageEffectorPositionImportance() const { return m_errorWeightedAverageEffectorPositionImportance; }
  NM_INLINE const float& getErrorWeightedAverageEffectorPositionImportanceRef() const { return m_errorWeightedAverageEffectorPositionImportance; }
  NM_INLINE const NMP::Vector3& getErrorWeightedAverageEffectorPosition() const { return errorWeightedAverageEffectorPosition; }

  enum { maxReachActionParams = 2 };
  NM_INLINE unsigned int getMaxReachActionParams() const { return maxReachActionParams; }
  NM_INLINE void setReachActionParams(unsigned int number, const ReachActionParams* _reachActionParams, float reachActionParams_importance = 1.f)
  {
    NMP_ASSERT(number <= maxReachActionParams);
    NMP_ASSERT(reachActionParams_importance >= 0.0f && reachActionParams_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      reachActionParams[i] = _reachActionParams[i];
      postAssignReachActionParams(reachActionParams[i]);
      m_reachActionParamsImportance[i] = reachActionParams_importance;
    }
  }
  NM_INLINE void setReachActionParamsAt(unsigned int index, const ReachActionParams& _reachActionParams, float reachActionParams_importance = 1.f)
  {
    NMP_ASSERT(index < maxReachActionParams);
    reachActionParams[index] = _reachActionParams;
    NMP_ASSERT(reachActionParams_importance >= 0.0f && reachActionParams_importance <= 1.0f);
    postAssignReachActionParams(reachActionParams[index]);
    m_reachActionParamsImportance[index] = reachActionParams_importance;
  }
  NM_INLINE float getReachActionParamsImportance(int index) const { return m_reachActionParamsImportance[index]; }
  NM_INLINE const float& getReachActionParamsImportanceRef(int index) const { return m_reachActionParamsImportance[index]; }
  NM_INLINE unsigned int calculateNumReachActionParams() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_reachActionParamsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const ReachActionParams& getReachActionParams(unsigned int index) const { NMP_ASSERT(index <= maxReachActionParams); return reachActionParams[index]; }

  enum { maxArmReachForBodyPartFlags = 2 };
  NM_INLINE unsigned int getMaxArmReachForBodyPartFlags() const { return maxArmReachForBodyPartFlags; }
  NM_INLINE void setArmReachForBodyPartFlags(unsigned int number, const bool* _armReachForBodyPartFlags, float armReachForBodyPartFlags_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmReachForBodyPartFlags);
    NMP_ASSERT(armReachForBodyPartFlags_importance >= 0.0f && armReachForBodyPartFlags_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armReachForBodyPartFlags[i] = _armReachForBodyPartFlags[i];
      m_armReachForBodyPartFlagsImportance[i] = armReachForBodyPartFlags_importance;
    }
  }
  NM_INLINE void setArmReachForBodyPartFlagsAt(unsigned int index, const bool& _armReachForBodyPartFlags, float armReachForBodyPartFlags_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmReachForBodyPartFlags);
    armReachForBodyPartFlags[index] = _armReachForBodyPartFlags;
    NMP_ASSERT(armReachForBodyPartFlags_importance >= 0.0f && armReachForBodyPartFlags_importance <= 1.0f);
    m_armReachForBodyPartFlagsImportance[index] = armReachForBodyPartFlags_importance;
  }
  NM_INLINE float getArmReachForBodyPartFlagsImportance(int index) const { return m_armReachForBodyPartFlagsImportance[index]; }
  NM_INLINE const float& getArmReachForBodyPartFlagsImportanceRef(int index) const { return m_armReachForBodyPartFlagsImportance[index]; }
  NM_INLINE unsigned int calculateNumArmReachForBodyPartFlags() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armReachForBodyPartFlagsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const bool& getArmReachForBodyPartFlags(unsigned int index) const { NMP_ASSERT(index <= maxArmReachForBodyPartFlags); return armReachForBodyPartFlags[index]; }

protected:

  TargetRequest lookTarget;
  ReachTarget reachTarget;
  ReachTarget reachTargets[NetworkConstants::networkMaxNumArms];
  NMP::Vector3 errorWeightedAverageEffectorPosition;  ///< (Position)
  ReachActionParams reachActionParams[NetworkConstants::networkMaxNumArms];
  bool armReachForBodyPartFlags[NetworkConstants::networkMaxNumArms];

  // post-assignment stubs
  NM_INLINE void postAssignLookTarget(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignReachTarget(const ReachTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignReachTargets(const ReachTarget& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignErrorWeightedAverageEffectorPosition(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "errorWeightedAverageEffectorPosition");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignReachActionParams(const ReachActionParams& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_lookTargetImportance, 
     m_reachTargetImportance, 
     m_reachTargetsImportance[NetworkConstants::networkMaxNumArms], 
     m_errorWeightedAverageEffectorPositionImportance, 
     m_reachActionParamsImportance[NetworkConstants::networkMaxNumArms], 
     m_armReachForBodyPartFlagsImportance[NetworkConstants::networkMaxNumArms];

  friend class ReachForBody_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getLookTargetImportance() > 0.0f)
    {
      lookTarget.validate();
    }
    if (getReachTargetImportance() > 0.0f)
    {
      reachTarget.validate();
    }
    {
      uint32_t numReachTargets = calculateNumReachTargets();
      for (uint32_t i=0; i<numReachTargets; i++)
      {
        if (getReachTargetsImportance(i) > 0.0f)
          reachTargets[i].validate();
      }
    }
    if (getErrorWeightedAverageEffectorPositionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(errorWeightedAverageEffectorPosition), "errorWeightedAverageEffectorPosition");
    }
    {
      uint32_t numReachActionParams = calculateNumReachActionParams();
      for (uint32_t i=0; i<numReachActionParams; i++)
      {
        if (getReachActionParamsImportance(i) > 0.0f)
          reachActionParams[i].validate();
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ReachForBodyFeedbackInputs
 * \ingroup ReachForBody
 * \brief FeedbackInputs *
 * Data packet definition (1032 bytes, 1056 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ReachForBodyFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ReachForBodyFeedbackInputs));
  }

  enum { maxCompetingArmControls = 2 };
  NM_INLINE unsigned int getMaxCompetingArmControls() const { return maxCompetingArmControls; }
  NM_INLINE void setCompetingArmControls(unsigned int number, const LimbControl* _competingArmControls, float competingArmControls_importance = 1.f)
  {
    NMP_ASSERT(number <= maxCompetingArmControls);
    NMP_ASSERT(competingArmControls_importance >= 0.0f && competingArmControls_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      competingArmControls[i] = _competingArmControls[i];
      postAssignCompetingArmControls(competingArmControls[i]);
      m_competingArmControlsImportance[i] = competingArmControls_importance;
    }
  }
  NM_INLINE void setCompetingArmControlsAt(unsigned int index, const LimbControl& _competingArmControls, float competingArmControls_importance = 1.f)
  {
    NMP_ASSERT(index < maxCompetingArmControls);
    competingArmControls[index] = _competingArmControls;
    NMP_ASSERT(competingArmControls_importance >= 0.0f && competingArmControls_importance <= 1.0f);
    postAssignCompetingArmControls(competingArmControls[index]);
    m_competingArmControlsImportance[index] = competingArmControls_importance;
  }
  NM_INLINE LimbControl& startCompetingArmControlsModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxCompetingArmControls);
    m_competingArmControlsImportance[index] = -1.0f; // set invalid until stopCompetingArmControlsModificationAt()
    return competingArmControls[index];
  }
  NM_INLINE void stopCompetingArmControlsModificationAt(unsigned int index, float competingArmControls_importance = 1.f)
  {
    postAssignCompetingArmControls(competingArmControls[index]);
    NMP_ASSERT(competingArmControls_importance >= 0.0f && competingArmControls_importance <= 1.0f);
    m_competingArmControlsImportance[index] = competingArmControls_importance;
  }
  NM_INLINE float getCompetingArmControlsImportance(int index) const { return m_competingArmControlsImportance[index]; }
  NM_INLINE const float& getCompetingArmControlsImportanceRef(int index) const { return m_competingArmControlsImportance[index]; }
  NM_INLINE unsigned int calculateNumCompetingArmControls() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_competingArmControlsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const LimbControl& getCompetingArmControls(unsigned int index) const { NMP_ASSERT(index <= maxCompetingArmControls); return competingArmControls[index]; }

  enum { maxHitInfo = 2 };
  NM_INLINE unsigned int getMaxHitInfo() const { return maxHitInfo; }
  NM_INLINE void setHitInfo(unsigned int number, const BodyHitInfo* _hitInfo, float hitInfo_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHitInfo);
    NMP_ASSERT(hitInfo_importance >= 0.0f && hitInfo_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      hitInfo[i] = _hitInfo[i];
      postAssignHitInfo(hitInfo[i]);
      m_hitInfoImportance[i] = hitInfo_importance;
    }
  }
  NM_INLINE void setHitInfoAt(unsigned int index, const BodyHitInfo& _hitInfo, float hitInfo_importance = 1.f)
  {
    NMP_ASSERT(index < maxHitInfo);
    hitInfo[index] = _hitInfo;
    NMP_ASSERT(hitInfo_importance >= 0.0f && hitInfo_importance <= 1.0f);
    postAssignHitInfo(hitInfo[index]);
    m_hitInfoImportance[index] = hitInfo_importance;
  }
  NM_INLINE BodyHitInfo& startHitInfoModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxHitInfo);
    m_hitInfoImportance[index] = -1.0f; // set invalid until stopHitInfoModificationAt()
    return hitInfo[index];
  }
  NM_INLINE void stopHitInfoModificationAt(unsigned int index, float hitInfo_importance = 1.f)
  {
    postAssignHitInfo(hitInfo[index]);
    NMP_ASSERT(hitInfo_importance >= 0.0f && hitInfo_importance <= 1.0f);
    m_hitInfoImportance[index] = hitInfo_importance;
  }
  NM_INLINE float getHitInfoImportance(int index) const { return m_hitInfoImportance[index]; }
  NM_INLINE const float& getHitInfoImportanceRef(int index) const { return m_hitInfoImportance[index]; }
  NM_INLINE unsigned int calculateNumHitInfo() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_hitInfoImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const BodyHitInfo& getHitInfo(unsigned int index) const { NMP_ASSERT(index <= maxHitInfo); return hitInfo[index]; }

  enum { maxShoulderPositions = 2 };
  NM_INLINE unsigned int getMaxShoulderPositions() const { return maxShoulderPositions; }
  NM_INLINE void setShoulderPositions(unsigned int number, const NMP::Vector3* _shoulderPositions, float shoulderPositions_importance = 1.f)
  {
    NMP_ASSERT(number <= maxShoulderPositions);
    NMP_ASSERT(shoulderPositions_importance >= 0.0f && shoulderPositions_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      shoulderPositions[i] = _shoulderPositions[i];
      postAssignShoulderPositions(shoulderPositions[i]);
      m_shoulderPositionsImportance[i] = shoulderPositions_importance;
    }
  }
  NM_INLINE void setShoulderPositionsAt(unsigned int index, const NMP::Vector3& _shoulderPositions, float shoulderPositions_importance = 1.f)
  {
    NMP_ASSERT(index < maxShoulderPositions);
    shoulderPositions[index] = _shoulderPositions;
    NMP_ASSERT(shoulderPositions_importance >= 0.0f && shoulderPositions_importance <= 1.0f);
    postAssignShoulderPositions(shoulderPositions[index]);
    m_shoulderPositionsImportance[index] = shoulderPositions_importance;
  }
  NM_INLINE float getShoulderPositionsImportance(int index) const { return m_shoulderPositionsImportance[index]; }
  NM_INLINE const float& getShoulderPositionsImportanceRef(int index) const { return m_shoulderPositionsImportance[index]; }
  NM_INLINE unsigned int calculateNumShoulderPositions() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_shoulderPositionsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Vector3& getShoulderPositions(unsigned int index) const { NMP_ASSERT(index <= maxShoulderPositions); return shoulderPositions[index]; }

  enum { maxEndEffectorDistanceSquaredToTargets = 2 };
  NM_INLINE unsigned int getMaxEndEffectorDistanceSquaredToTargets() const { return maxEndEffectorDistanceSquaredToTargets; }
  NM_INLINE void setEndEffectorDistanceSquaredToTargets(unsigned int number, const float* _endEffectorDistanceSquaredToTargets, float endEffectorDistanceSquaredToTargets_importance = 1.f)
  {
    NMP_ASSERT(number <= maxEndEffectorDistanceSquaredToTargets);
    NMP_ASSERT(endEffectorDistanceSquaredToTargets_importance >= 0.0f && endEffectorDistanceSquaredToTargets_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      endEffectorDistanceSquaredToTargets[i] = _endEffectorDistanceSquaredToTargets[i];
      postAssignEndEffectorDistanceSquaredToTargets(endEffectorDistanceSquaredToTargets[i]);
      m_endEffectorDistanceSquaredToTargetsImportance[i] = endEffectorDistanceSquaredToTargets_importance;
    }
  }
  NM_INLINE void setEndEffectorDistanceSquaredToTargetsAt(unsigned int index, const float& _endEffectorDistanceSquaredToTargets, float endEffectorDistanceSquaredToTargets_importance = 1.f)
  {
    NMP_ASSERT(index < maxEndEffectorDistanceSquaredToTargets);
    endEffectorDistanceSquaredToTargets[index] = _endEffectorDistanceSquaredToTargets;
    NMP_ASSERT(endEffectorDistanceSquaredToTargets_importance >= 0.0f && endEffectorDistanceSquaredToTargets_importance <= 1.0f);
    postAssignEndEffectorDistanceSquaredToTargets(endEffectorDistanceSquaredToTargets[index]);
    m_endEffectorDistanceSquaredToTargetsImportance[index] = endEffectorDistanceSquaredToTargets_importance;
  }
  NM_INLINE float getEndEffectorDistanceSquaredToTargetsImportance(int index) const { return m_endEffectorDistanceSquaredToTargetsImportance[index]; }
  NM_INLINE const float& getEndEffectorDistanceSquaredToTargetsImportanceRef(int index) const { return m_endEffectorDistanceSquaredToTargetsImportance[index]; }
  NM_INLINE unsigned int calculateNumEndEffectorDistanceSquaredToTargets() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_endEffectorDistanceSquaredToTargetsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getEndEffectorDistanceSquaredToTargets(unsigned int index) const { NMP_ASSERT(index <= maxEndEffectorDistanceSquaredToTargets); return endEffectorDistanceSquaredToTargets[index]; }

  enum { maxArmStrengthReduction = 2 };
  NM_INLINE unsigned int getMaxArmStrengthReduction() const { return maxArmStrengthReduction; }
  NM_INLINE void setArmStrengthReduction(unsigned int number, const float* _armStrengthReduction, float armStrengthReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmStrengthReduction);
    NMP_ASSERT(armStrengthReduction_importance >= 0.0f && armStrengthReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armStrengthReduction[i] = _armStrengthReduction[i];
      postAssignArmStrengthReduction(armStrengthReduction[i]);
      m_armStrengthReductionImportance[i] = armStrengthReduction_importance;
    }
  }
  NM_INLINE void setArmStrengthReductionAt(unsigned int index, const float& _armStrengthReduction, float armStrengthReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmStrengthReduction);
    armStrengthReduction[index] = _armStrengthReduction;
    NMP_ASSERT(armStrengthReduction_importance >= 0.0f && armStrengthReduction_importance <= 1.0f);
    postAssignArmStrengthReduction(armStrengthReduction[index]);
    m_armStrengthReductionImportance[index] = armStrengthReduction_importance;
  }
  NM_INLINE float getArmStrengthReductionImportance(int index) const { return m_armStrengthReductionImportance[index]; }
  NM_INLINE const float& getArmStrengthReductionImportanceRef(int index) const { return m_armStrengthReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumArmStrengthReduction() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armStrengthReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getArmStrengthReduction(unsigned int index) const { NMP_ASSERT(index <= maxArmStrengthReduction); return armStrengthReduction[index]; }

  enum { maxArmReachStates = 2 };
  NM_INLINE unsigned int getMaxArmReachStates() const { return maxArmReachStates; }
  NM_INLINE void setArmReachStates(unsigned int number, const uint32_t* _armReachStates, float armReachStates_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmReachStates);
    NMP_ASSERT(armReachStates_importance >= 0.0f && armReachStates_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armReachStates[i] = _armReachStates[i];
      m_armReachStatesImportance[i] = armReachStates_importance;
    }
  }
  NM_INLINE void setArmReachStatesAt(unsigned int index, const uint32_t& _armReachStates, float armReachStates_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmReachStates);
    armReachStates[index] = _armReachStates;
    NMP_ASSERT(armReachStates_importance >= 0.0f && armReachStates_importance <= 1.0f);
    m_armReachStatesImportance[index] = armReachStates_importance;
  }
  NM_INLINE float getArmReachStatesImportance(int index) const { return m_armReachStatesImportance[index]; }
  NM_INLINE const float& getArmReachStatesImportanceRef(int index) const { return m_armReachStatesImportance[index]; }
  NM_INLINE unsigned int calculateNumArmReachStates() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armReachStatesImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const uint32_t& getArmReachStates(unsigned int index) const { NMP_ASSERT(index <= maxArmReachStates); return armReachStates[index]; }

protected:

  LimbControl competingArmControls[NetworkConstants::networkMaxNumArms];  ///< look at brace arm controls so we can yield to brace
  BodyHitInfo hitInfo[NetworkConstants::networkMaxNumArms];  ///< a hit info for each arm
  NMP::Vector3 shoulderPositions[NetworkConstants::networkMaxNumArms];  ///< on yield (to brace) use arm with shouder closest to brace hazard  (Position)
  float endEffectorDistanceSquaredToTargets[NetworkConstants::networkMaxNumArms];
  float armStrengthReduction[NetworkConstants::networkMaxNumArms];  ///< for phasing out arms  (Weight)
  uint32_t armReachStates[NetworkConstants::networkMaxNumArms];  // reach state tells us something about how armReachForBodyPart is getting along and in particular when she's done

  // post-assignment stubs
  NM_INLINE void postAssignCompetingArmControls(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHitInfo(const BodyHitInfo& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignShoulderPositions(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "shoulderPositions");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignEndEffectorDistanceSquaredToTargets(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "endEffectorDistanceSquaredToTargets");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmStrengthReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "armStrengthReduction");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_competingArmControlsImportance[NetworkConstants::networkMaxNumArms], 
     m_hitInfoImportance[NetworkConstants::networkMaxNumArms], 
     m_shoulderPositionsImportance[NetworkConstants::networkMaxNumArms], 
     m_endEffectorDistanceSquaredToTargetsImportance[NetworkConstants::networkMaxNumArms], 
     m_armStrengthReductionImportance[NetworkConstants::networkMaxNumArms], 
     m_armReachStatesImportance[NetworkConstants::networkMaxNumArms];

  friend class ReachForBody_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numCompetingArmControls = calculateNumCompetingArmControls();
      for (uint32_t i=0; i<numCompetingArmControls; i++)
      {
        if (getCompetingArmControlsImportance(i) > 0.0f)
          competingArmControls[i].validate();
      }
    }
    {
      uint32_t numHitInfo = calculateNumHitInfo();
      for (uint32_t i=0; i<numHitInfo; i++)
      {
        if (getHitInfoImportance(i) > 0.0f)
          hitInfo[i].validate();
      }
    }
    {
      uint32_t numShoulderPositions = calculateNumShoulderPositions();
      for (uint32_t i=0; i<numShoulderPositions; i++)
      {
        if (getShoulderPositionsImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Vector3Valid(shoulderPositions[i]), "shoulderPositions");
        }
      }
    }
    {
      uint32_t numEndEffectorDistanceSquaredToTargets = calculateNumEndEffectorDistanceSquaredToTargets();
      for (uint32_t i=0; i<numEndEffectorDistanceSquaredToTargets; i++)
      {
        if (getEndEffectorDistanceSquaredToTargetsImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(endEffectorDistanceSquaredToTargets[i]), "endEffectorDistanceSquaredToTargets");
        }
      }
    }
    {
      uint32_t numArmStrengthReduction = calculateNumArmStrengthReduction();
      for (uint32_t i=0; i<numArmStrengthReduction; i++)
      {
        if (getArmStrengthReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatNonNegative(armStrengthReduction[i]), "armStrengthReduction");
        }
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ReachForBodyFeedbackOutputs
 * \ingroup ReachForBody
 * \brief FeedbackOutputs *
 * Data packet definition (5 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ReachForBodyFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ReachForBodyFeedbackOutputs));
  }

  NM_INLINE void setCompleted(const bool& _completed, float completed_importance = 1.f)
  {
    completed = _completed;
    NMP_ASSERT(completed_importance >= 0.0f && completed_importance <= 1.0f);
    m_completedImportance = completed_importance;
  }
  NM_INLINE float getCompletedImportance() const { return m_completedImportance; }
  NM_INLINE const float& getCompletedImportanceRef() const { return m_completedImportance; }
  NM_INLINE const bool& getCompleted() const { return completed; }

protected:

  bool completed;

  // importance values
  float
     m_completedImportance;

  friend class ReachForBody_Con;

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

#endif // NM_MDF_REACHFORBODY_DATA_H

