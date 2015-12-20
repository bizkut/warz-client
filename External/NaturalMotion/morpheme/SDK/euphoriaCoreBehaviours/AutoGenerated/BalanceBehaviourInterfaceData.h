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

#ifndef NM_MDF_BALANCEBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_BALANCEBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/BalanceBehaviourInterface.module"

// external types
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/PoseData.h"
#include "Types/BalanceParameters.h"
#include "Types/BalancePoseClamping.h"
#include "Types/StepParameters.h"
#include "Types/SteppingBalanceParameters.h"
#include "Types/StepRecoveryParameters.h"
#include "Types/BalanceAssistanceParameters.h"
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
 * \class BalanceBehaviourInterfaceData
 * \ingroup BalanceBehaviourInterface
 * \brief Data Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (920 bytes, 928 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalanceBehaviourInterfaceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalanceBehaviourInterfaceData));
  }

  PoseData cachedBalancePose[NetworkConstants::networkMaxNumLimbs];  /// Cached pose is only used if the behaviour param UseSingleFrameForBalancePose is set
  NMP::Matrix34 cachedReadyPoseArmEndRelativeToRoot[NetworkConstants::networkMaxNumArms];  // stepping  (Transform)
  float standingStillTime;                   ///< (TimePeriod)
  float fallenTime;                          ///< (TimePeriod)
  float runningTime;                         ///< (TimePeriod)
  float onGroundTime;                        ///< (TimePeriod)
  float cachedReadyPoseArmEndRelativeToRootWeights[NetworkConstants::networkMaxNumArms];  ///< (Weight)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    cachedBalancePose[0].validate();
    cachedBalancePose[1].validate();
    cachedBalancePose[2].validate();
    cachedBalancePose[3].validate();
    cachedBalancePose[4].validate();
    cachedBalancePose[5].validate();
    NM_VALIDATOR(Matrix34Orthonormal(cachedReadyPoseArmEndRelativeToRoot[0]), "cachedReadyPoseArmEndRelativeToRoot[0]");
    NM_VALIDATOR(Matrix34Orthonormal(cachedReadyPoseArmEndRelativeToRoot[1]), "cachedReadyPoseArmEndRelativeToRoot[1]");
    NM_VALIDATOR(FloatValid(standingStillTime), "standingStillTime");
    NM_VALIDATOR(FloatValid(fallenTime), "fallenTime");
    NM_VALIDATOR(FloatValid(runningTime), "runningTime");
    NM_VALIDATOR(FloatValid(onGroundTime), "onGroundTime");
    NM_VALIDATOR(FloatNonNegative(cachedReadyPoseArmEndRelativeToRootWeights[0]), "cachedReadyPoseArmEndRelativeToRootWeights[0]");
    NM_VALIDATOR(FloatNonNegative(cachedReadyPoseArmEndRelativeToRootWeights[1]), "cachedReadyPoseArmEndRelativeToRootWeights[1]");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BalanceBehaviourInterfaceInputs
 * \ingroup BalanceBehaviourInterface
 * \brief Inputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (32 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalanceBehaviourInterfaceInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalanceBehaviourInterfaceInputs));
  }

  enum { maxLegStrengthReduction = 2 };
  NM_INLINE unsigned int getMaxLegStrengthReduction() const { return maxLegStrengthReduction; }
  NM_INLINE void setLegStrengthReduction(unsigned int number, const float* _legStrengthReduction, float legStrengthReduction_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegStrengthReduction);
    NMP_ASSERT(legStrengthReduction_importance >= 0.0f && legStrengthReduction_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legStrengthReduction[i] = _legStrengthReduction[i];
      postAssignLegStrengthReduction(legStrengthReduction[i]);
      m_legStrengthReductionImportance[i] = legStrengthReduction_importance;
    }
  }
  NM_INLINE void setLegStrengthReductionAt(unsigned int index, const float& _legStrengthReduction, float legStrengthReduction_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegStrengthReduction);
    legStrengthReduction[index] = _legStrengthReduction;
    NMP_ASSERT(legStrengthReduction_importance >= 0.0f && legStrengthReduction_importance <= 1.0f);
    postAssignLegStrengthReduction(legStrengthReduction[index]);
    m_legStrengthReductionImportance[index] = legStrengthReduction_importance;
  }
  NM_INLINE float getLegStrengthReductionImportance(int index) const { return m_legStrengthReductionImportance[index]; }
  NM_INLINE const float& getLegStrengthReductionImportanceRef(int index) const { return m_legStrengthReductionImportance[index]; }
  NM_INLINE unsigned int calculateNumLegStrengthReduction() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legStrengthReductionImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getLegStrengthReduction(unsigned int index) const { NMP_ASSERT(index <= maxLegStrengthReduction); return legStrengthReduction[index]; }

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

protected:

  float legStrengthReduction[NetworkConstants::networkMaxNumLegs];  ///< (Multiplier)
  float armStrengthReduction[NetworkConstants::networkMaxNumArms];  ///< (Multiplier)

  // post-assignment stubs
  NM_INLINE void postAssignLegStrengthReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "legStrengthReduction");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmStrengthReduction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "armStrengthReduction");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_legStrengthReductionImportance[NetworkConstants::networkMaxNumLegs], 
     m_armStrengthReductionImportance[NetworkConstants::networkMaxNumArms];

  friend class BalanceBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numLegStrengthReduction = calculateNumLegStrengthReduction();
      for (uint32_t i=0; i<numLegStrengthReduction; i++)
      {
        if (getLegStrengthReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(legStrengthReduction[i]), "legStrengthReduction");
        }
      }
    }
    {
      uint32_t numArmStrengthReduction = calculateNumArmStrengthReduction();
      for (uint32_t i=0; i<numArmStrengthReduction; i++)
      {
        if (getArmStrengthReductionImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(armStrengthReduction[i]), "armStrengthReduction");
        }
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BalanceBehaviourInterfaceOutputs
 * \ingroup BalanceBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (1380 bytes, 1408 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalanceBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalanceBehaviourInterfaceOutputs));
  }

  enum { maxArmBalancePose = 2 };
  NM_INLINE unsigned int getMaxArmBalancePose() const { return maxArmBalancePose; }
  NM_INLINE void setArmBalancePose(unsigned int number, const PoseData* _armBalancePose, float armBalancePose_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmBalancePose);
    NMP_ASSERT(armBalancePose_importance >= 0.0f && armBalancePose_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armBalancePose[i] = _armBalancePose[i];
      postAssignArmBalancePose(armBalancePose[i]);
      m_armBalancePoseImportance[i] = armBalancePose_importance;
    }
  }
  NM_INLINE void setArmBalancePoseAt(unsigned int index, const PoseData& _armBalancePose, float armBalancePose_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmBalancePose);
    armBalancePose[index] = _armBalancePose;
    NMP_ASSERT(armBalancePose_importance >= 0.0f && armBalancePose_importance <= 1.0f);
    postAssignArmBalancePose(armBalancePose[index]);
    m_armBalancePoseImportance[index] = armBalancePose_importance;
  }
  NM_INLINE PoseData& startArmBalancePoseModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxArmBalancePose);
    m_armBalancePoseImportance[index] = -1.0f; // set invalid until stopArmBalancePoseModificationAt()
    return armBalancePose[index];
  }
  NM_INLINE void stopArmBalancePoseModificationAt(unsigned int index, float armBalancePose_importance = 1.f)
  {
    postAssignArmBalancePose(armBalancePose[index]);
    NMP_ASSERT(armBalancePose_importance >= 0.0f && armBalancePose_importance <= 1.0f);
    m_armBalancePoseImportance[index] = armBalancePose_importance;
  }
  NM_INLINE float getArmBalancePoseImportance(int index) const { return m_armBalancePoseImportance[index]; }
  NM_INLINE const float& getArmBalancePoseImportanceRef(int index) const { return m_armBalancePoseImportance[index]; }
  NM_INLINE unsigned int calculateNumArmBalancePose() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armBalancePoseImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getArmBalancePose(unsigned int index) const { NMP_ASSERT(index <= maxArmBalancePose); return armBalancePose[index]; }

  NM_INLINE void setStepParameters(const StepParameters& _stepParameters, float stepParameters_importance = 1.f)
  {
    stepParameters = _stepParameters;
    postAssignStepParameters(stepParameters);
    NMP_ASSERT(stepParameters_importance >= 0.0f && stepParameters_importance <= 1.0f);
    m_stepParametersImportance = stepParameters_importance;
  }
  NM_INLINE StepParameters& startStepParametersModification()
  {
    m_stepParametersImportance = -1.0f; // set invalid until stopStepParametersModification()
    return stepParameters;
  }
  NM_INLINE void stopStepParametersModification(float stepParameters_importance = 1.f)
  {
    postAssignStepParameters(stepParameters);
    NMP_ASSERT(stepParameters_importance >= 0.0f && stepParameters_importance <= 1.0f);
    m_stepParametersImportance = stepParameters_importance;
  }
  NM_INLINE float getStepParametersImportance() const { return m_stepParametersImportance; }
  NM_INLINE const float& getStepParametersImportanceRef() const { return m_stepParametersImportance; }
  NM_INLINE const StepParameters& getStepParameters() const { return stepParameters; }

  enum { maxArmBalancePoseEndRelativeToRoot = 2 };
  NM_INLINE unsigned int getMaxArmBalancePoseEndRelativeToRoot() const { return maxArmBalancePoseEndRelativeToRoot; }
  NM_INLINE void setArmBalancePoseEndRelativeToRoot(unsigned int number, const NMP::Matrix34* _armBalancePoseEndRelativeToRoot, float armBalancePoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmBalancePoseEndRelativeToRoot);
    NMP_ASSERT(armBalancePoseEndRelativeToRoot_importance >= 0.0f && armBalancePoseEndRelativeToRoot_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armBalancePoseEndRelativeToRoot[i] = _armBalancePoseEndRelativeToRoot[i];
      postAssignArmBalancePoseEndRelativeToRoot(armBalancePoseEndRelativeToRoot[i]);
      m_armBalancePoseEndRelativeToRootImportance[i] = armBalancePoseEndRelativeToRoot_importance;
    }
  }
  NM_INLINE void setArmBalancePoseEndRelativeToRootAt(unsigned int index, const NMP::Matrix34& _armBalancePoseEndRelativeToRoot, float armBalancePoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmBalancePoseEndRelativeToRoot);
    armBalancePoseEndRelativeToRoot[index] = _armBalancePoseEndRelativeToRoot;
    NMP_ASSERT(armBalancePoseEndRelativeToRoot_importance >= 0.0f && armBalancePoseEndRelativeToRoot_importance <= 1.0f);
    postAssignArmBalancePoseEndRelativeToRoot(armBalancePoseEndRelativeToRoot[index]);
    m_armBalancePoseEndRelativeToRootImportance[index] = armBalancePoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getArmBalancePoseEndRelativeToRootImportance(int index) const { return m_armBalancePoseEndRelativeToRootImportance[index]; }
  NM_INLINE const float& getArmBalancePoseEndRelativeToRootImportanceRef(int index) const { return m_armBalancePoseEndRelativeToRootImportance[index]; }
  NM_INLINE unsigned int calculateNumArmBalancePoseEndRelativeToRoot() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armBalancePoseEndRelativeToRootImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getArmBalancePoseEndRelativeToRoot(unsigned int index) const { NMP_ASSERT(index <= maxArmBalancePoseEndRelativeToRoot); return armBalancePoseEndRelativeToRoot[index]; }

  enum { maxHeadBalancePoseEndRelativeToRoot = 1 };
  NM_INLINE unsigned int getMaxHeadBalancePoseEndRelativeToRoot() const { return maxHeadBalancePoseEndRelativeToRoot; }
  NM_INLINE void setHeadBalancePoseEndRelativeToRoot(unsigned int number, const NMP::Matrix34* _headBalancePoseEndRelativeToRoot, float headBalancePoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHeadBalancePoseEndRelativeToRoot);
    NMP_ASSERT(headBalancePoseEndRelativeToRoot_importance >= 0.0f && headBalancePoseEndRelativeToRoot_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      headBalancePoseEndRelativeToRoot[i] = _headBalancePoseEndRelativeToRoot[i];
      postAssignHeadBalancePoseEndRelativeToRoot(headBalancePoseEndRelativeToRoot[i]);
      m_headBalancePoseEndRelativeToRootImportance[i] = headBalancePoseEndRelativeToRoot_importance;
    }
  }
  NM_INLINE void setHeadBalancePoseEndRelativeToRootAt(unsigned int index, const NMP::Matrix34& _headBalancePoseEndRelativeToRoot, float headBalancePoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(index < maxHeadBalancePoseEndRelativeToRoot);
    headBalancePoseEndRelativeToRoot[index] = _headBalancePoseEndRelativeToRoot;
    NMP_ASSERT(headBalancePoseEndRelativeToRoot_importance >= 0.0f && headBalancePoseEndRelativeToRoot_importance <= 1.0f);
    postAssignHeadBalancePoseEndRelativeToRoot(headBalancePoseEndRelativeToRoot[index]);
    m_headBalancePoseEndRelativeToRootImportance[index] = headBalancePoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getHeadBalancePoseEndRelativeToRootImportance(int index) const { return m_headBalancePoseEndRelativeToRootImportance[index]; }
  NM_INLINE const float& getHeadBalancePoseEndRelativeToRootImportanceRef(int index) const { return m_headBalancePoseEndRelativeToRootImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadBalancePoseEndRelativeToRoot() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headBalancePoseEndRelativeToRootImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getHeadBalancePoseEndRelativeToRoot(unsigned int index) const { NMP_ASSERT(index <= maxHeadBalancePoseEndRelativeToRoot); return headBalancePoseEndRelativeToRoot[index]; }

  enum { maxSpineBalancePoseEndRelativeToRoot = 1 };
  NM_INLINE unsigned int getMaxSpineBalancePoseEndRelativeToRoot() const { return maxSpineBalancePoseEndRelativeToRoot; }
  NM_INLINE void setSpineBalancePoseEndRelativeToRoot(unsigned int number, const NMP::Matrix34* _spineBalancePoseEndRelativeToRoot, float spineBalancePoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpineBalancePoseEndRelativeToRoot);
    NMP_ASSERT(spineBalancePoseEndRelativeToRoot_importance >= 0.0f && spineBalancePoseEndRelativeToRoot_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spineBalancePoseEndRelativeToRoot[i] = _spineBalancePoseEndRelativeToRoot[i];
      postAssignSpineBalancePoseEndRelativeToRoot(spineBalancePoseEndRelativeToRoot[i]);
      m_spineBalancePoseEndRelativeToRootImportance[i] = spineBalancePoseEndRelativeToRoot_importance;
    }
  }
  NM_INLINE void setSpineBalancePoseEndRelativeToRootAt(unsigned int index, const NMP::Matrix34& _spineBalancePoseEndRelativeToRoot, float spineBalancePoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpineBalancePoseEndRelativeToRoot);
    spineBalancePoseEndRelativeToRoot[index] = _spineBalancePoseEndRelativeToRoot;
    NMP_ASSERT(spineBalancePoseEndRelativeToRoot_importance >= 0.0f && spineBalancePoseEndRelativeToRoot_importance <= 1.0f);
    postAssignSpineBalancePoseEndRelativeToRoot(spineBalancePoseEndRelativeToRoot[index]);
    m_spineBalancePoseEndRelativeToRootImportance[index] = spineBalancePoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getSpineBalancePoseEndRelativeToRootImportance(int index) const { return m_spineBalancePoseEndRelativeToRootImportance[index]; }
  NM_INLINE const float& getSpineBalancePoseEndRelativeToRootImportanceRef(int index) const { return m_spineBalancePoseEndRelativeToRootImportance[index]; }
  NM_INLINE unsigned int calculateNumSpineBalancePoseEndRelativeToRoot() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spineBalancePoseEndRelativeToRootImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getSpineBalancePoseEndRelativeToRoot(unsigned int index) const { NMP_ASSERT(index <= maxSpineBalancePoseEndRelativeToRoot); return spineBalancePoseEndRelativeToRoot[index]; }

  enum { maxLegBalancePoseEndRelativeToRoot = 2 };
  NM_INLINE unsigned int getMaxLegBalancePoseEndRelativeToRoot() const { return maxLegBalancePoseEndRelativeToRoot; }
  NM_INLINE void setLegBalancePoseEndRelativeToRoot(unsigned int number, const NMP::Matrix34* _legBalancePoseEndRelativeToRoot, float legBalancePoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegBalancePoseEndRelativeToRoot);
    NMP_ASSERT(legBalancePoseEndRelativeToRoot_importance >= 0.0f && legBalancePoseEndRelativeToRoot_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legBalancePoseEndRelativeToRoot[i] = _legBalancePoseEndRelativeToRoot[i];
      postAssignLegBalancePoseEndRelativeToRoot(legBalancePoseEndRelativeToRoot[i]);
      m_legBalancePoseEndRelativeToRootImportance[i] = legBalancePoseEndRelativeToRoot_importance;
    }
  }
  NM_INLINE void setLegBalancePoseEndRelativeToRootAt(unsigned int index, const NMP::Matrix34& _legBalancePoseEndRelativeToRoot, float legBalancePoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegBalancePoseEndRelativeToRoot);
    legBalancePoseEndRelativeToRoot[index] = _legBalancePoseEndRelativeToRoot;
    NMP_ASSERT(legBalancePoseEndRelativeToRoot_importance >= 0.0f && legBalancePoseEndRelativeToRoot_importance <= 1.0f);
    postAssignLegBalancePoseEndRelativeToRoot(legBalancePoseEndRelativeToRoot[index]);
    m_legBalancePoseEndRelativeToRootImportance[index] = legBalancePoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getLegBalancePoseEndRelativeToRootImportance(int index) const { return m_legBalancePoseEndRelativeToRootImportance[index]; }
  NM_INLINE const float& getLegBalancePoseEndRelativeToRootImportanceRef(int index) const { return m_legBalancePoseEndRelativeToRootImportance[index]; }
  NM_INLINE unsigned int calculateNumLegBalancePoseEndRelativeToRoot() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legBalancePoseEndRelativeToRootImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getLegBalancePoseEndRelativeToRoot(unsigned int index) const { NMP_ASSERT(index <= maxLegBalancePoseEndRelativeToRoot); return legBalancePoseEndRelativeToRoot[index]; }

  NM_INLINE float getAverageBalancePoseArmEndRelativeToRootImportance() const { return m_averageBalancePoseArmEndRelativeToRootImportance; }
  NM_INLINE const float& getAverageBalancePoseArmEndRelativeToRootImportanceRef() const { return m_averageBalancePoseArmEndRelativeToRootImportance; }
  NM_INLINE const NMP::Matrix34& getAverageBalancePoseArmEndRelativeToRoot() const { return averageBalancePoseArmEndRelativeToRoot; }

  NM_INLINE float getAverageBalancePoseLegEndRelativeToRootImportance() const { return m_averageBalancePoseLegEndRelativeToRootImportance; }
  NM_INLINE const float& getAverageBalancePoseLegEndRelativeToRootImportanceRef() const { return m_averageBalancePoseLegEndRelativeToRootImportance; }
  NM_INLINE const NMP::Matrix34& getAverageBalancePoseLegEndRelativeToRoot() const { return averageBalancePoseLegEndRelativeToRoot; }

  enum { maxArmReadyPoseEndRelativeToRoot = 2 };
  NM_INLINE unsigned int getMaxArmReadyPoseEndRelativeToRoot() const { return maxArmReadyPoseEndRelativeToRoot; }
  NM_INLINE void setArmReadyPoseEndRelativeToRoot(unsigned int number, const NMP::Matrix34* _armReadyPoseEndRelativeToRoot, float armReadyPoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmReadyPoseEndRelativeToRoot);
    NMP_ASSERT(armReadyPoseEndRelativeToRoot_importance >= 0.0f && armReadyPoseEndRelativeToRoot_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armReadyPoseEndRelativeToRoot[i] = _armReadyPoseEndRelativeToRoot[i];
      postAssignArmReadyPoseEndRelativeToRoot(armReadyPoseEndRelativeToRoot[i]);
      m_armReadyPoseEndRelativeToRootImportance[i] = armReadyPoseEndRelativeToRoot_importance;
    }
  }
  NM_INLINE void setArmReadyPoseEndRelativeToRootAt(unsigned int index, const NMP::Matrix34& _armReadyPoseEndRelativeToRoot, float armReadyPoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmReadyPoseEndRelativeToRoot);
    armReadyPoseEndRelativeToRoot[index] = _armReadyPoseEndRelativeToRoot;
    NMP_ASSERT(armReadyPoseEndRelativeToRoot_importance >= 0.0f && armReadyPoseEndRelativeToRoot_importance <= 1.0f);
    postAssignArmReadyPoseEndRelativeToRoot(armReadyPoseEndRelativeToRoot[index]);
    m_armReadyPoseEndRelativeToRootImportance[index] = armReadyPoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getArmReadyPoseEndRelativeToRootImportance(int index) const { return m_armReadyPoseEndRelativeToRootImportance[index]; }
  NM_INLINE const float& getArmReadyPoseEndRelativeToRootImportanceRef(int index) const { return m_armReadyPoseEndRelativeToRootImportance[index]; }
  NM_INLINE unsigned int calculateNumArmReadyPoseEndRelativeToRoot() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armReadyPoseEndRelativeToRootImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getArmReadyPoseEndRelativeToRoot(unsigned int index) const { NMP_ASSERT(index <= maxArmReadyPoseEndRelativeToRoot); return armReadyPoseEndRelativeToRoot[index]; }

  NM_INLINE void setBalanceAssistanceParameters(const BalanceAssistanceParameters& _balanceAssistanceParameters, float balanceAssistanceParameters_importance = 1.f)
  {
    balanceAssistanceParameters = _balanceAssistanceParameters;
    postAssignBalanceAssistanceParameters(balanceAssistanceParameters);
    NMP_ASSERT(balanceAssistanceParameters_importance >= 0.0f && balanceAssistanceParameters_importance <= 1.0f);
    m_balanceAssistanceParametersImportance = balanceAssistanceParameters_importance;
  }
  NM_INLINE float getBalanceAssistanceParametersImportance() const { return m_balanceAssistanceParametersImportance; }
  NM_INLINE const float& getBalanceAssistanceParametersImportanceRef() const { return m_balanceAssistanceParametersImportance; }
  NM_INLINE const BalanceAssistanceParameters& getBalanceAssistanceParameters() const { return balanceAssistanceParameters; }

  NM_INLINE void setTargetPelvisDirection(const NMP::Vector3& _targetPelvisDirection, float targetPelvisDirection_importance = 1.f)
  {
    targetPelvisDirection = _targetPelvisDirection;
    postAssignTargetPelvisDirection(targetPelvisDirection);
    NMP_ASSERT(targetPelvisDirection_importance >= 0.0f && targetPelvisDirection_importance <= 1.0f);
    m_targetPelvisDirectionImportance = targetPelvisDirection_importance;
  }
  NM_INLINE float getTargetPelvisDirectionImportance() const { return m_targetPelvisDirectionImportance; }
  NM_INLINE const float& getTargetPelvisDirectionImportanceRef() const { return m_targetPelvisDirectionImportance; }
  NM_INLINE const NMP::Vector3& getTargetPelvisDirection() const { return targetPelvisDirection; }

  NM_INLINE void setBalanceParameters(const BalanceParameters& _balanceParameters, float balanceParameters_importance = 1.f)
  {
    balanceParameters = _balanceParameters;
    postAssignBalanceParameters(balanceParameters);
    NMP_ASSERT(balanceParameters_importance >= 0.0f && balanceParameters_importance <= 1.0f);
    m_balanceParametersImportance = balanceParameters_importance;
  }
  NM_INLINE BalanceParameters& startBalanceParametersModification()
  {
    m_balanceParametersImportance = -1.0f; // set invalid until stopBalanceParametersModification()
    return balanceParameters;
  }
  NM_INLINE void stopBalanceParametersModification(float balanceParameters_importance = 1.f)
  {
    postAssignBalanceParameters(balanceParameters);
    NMP_ASSERT(balanceParameters_importance >= 0.0f && balanceParameters_importance <= 1.0f);
    m_balanceParametersImportance = balanceParameters_importance;
  }
  NM_INLINE float getBalanceParametersImportance() const { return m_balanceParametersImportance; }
  NM_INLINE const float& getBalanceParametersImportanceRef() const { return m_balanceParametersImportance; }
  NM_INLINE const BalanceParameters& getBalanceParameters() const { return balanceParameters; }

  NM_INLINE void setBalancePoseClamping(const BalancePoseClamping& _balancePoseClamping, float balancePoseClamping_importance = 1.f)
  {
    balancePoseClamping = _balancePoseClamping;
    postAssignBalancePoseClamping(balancePoseClamping);
    NMP_ASSERT(balancePoseClamping_importance >= 0.0f && balancePoseClamping_importance <= 1.0f);
    m_balancePoseClampingImportance = balancePoseClamping_importance;
  }
  NM_INLINE float getBalancePoseClampingImportance() const { return m_balancePoseClampingImportance; }
  NM_INLINE const float& getBalancePoseClampingImportanceRef() const { return m_balancePoseClampingImportance; }
  NM_INLINE const BalancePoseClamping& getBalancePoseClamping() const { return balancePoseClamping; }

  NM_INLINE void setSteppingBalanceParameters(const SteppingBalanceParameters& _steppingBalanceParameters, float steppingBalanceParameters_importance = 1.f)
  {
    steppingBalanceParameters = _steppingBalanceParameters;
    postAssignSteppingBalanceParameters(steppingBalanceParameters);
    NMP_ASSERT(steppingBalanceParameters_importance >= 0.0f && steppingBalanceParameters_importance <= 1.0f);
    m_steppingBalanceParametersImportance = steppingBalanceParameters_importance;
  }
  NM_INLINE float getSteppingBalanceParametersImportance() const { return m_steppingBalanceParametersImportance; }
  NM_INLINE const float& getSteppingBalanceParametersImportanceRef() const { return m_steppingBalanceParametersImportance; }
  NM_INLINE const SteppingBalanceParameters& getSteppingBalanceParameters() const { return steppingBalanceParameters; }

  NM_INLINE void setStepRecoveryParameters(const StepRecoveryParameters& _stepRecoveryParameters, float stepRecoveryParameters_importance = 1.f)
  {
    stepRecoveryParameters = _stepRecoveryParameters;
    postAssignStepRecoveryParameters(stepRecoveryParameters);
    NMP_ASSERT(stepRecoveryParameters_importance >= 0.0f && stepRecoveryParameters_importance <= 1.0f);
    m_stepRecoveryParametersImportance = stepRecoveryParameters_importance;
  }
  NM_INLINE float getStepRecoveryParametersImportance() const { return m_stepRecoveryParametersImportance; }
  NM_INLINE const float& getStepRecoveryParametersImportanceRef() const { return m_stepRecoveryParametersImportance; }
  NM_INLINE const StepRecoveryParameters& getStepRecoveryParameters() const { return stepRecoveryParameters; }

  NM_INLINE void setBalanceUsingAnimationHorizontalPelvisOffsetAmount(const float& _balanceUsingAnimationHorizontalPelvisOffsetAmount, float balanceUsingAnimationHorizontalPelvisOffsetAmount_importance = 1.f)
  {
    balanceUsingAnimationHorizontalPelvisOffsetAmount = _balanceUsingAnimationHorizontalPelvisOffsetAmount;
    postAssignBalanceUsingAnimationHorizontalPelvisOffsetAmount(balanceUsingAnimationHorizontalPelvisOffsetAmount);
    NMP_ASSERT(balanceUsingAnimationHorizontalPelvisOffsetAmount_importance >= 0.0f && balanceUsingAnimationHorizontalPelvisOffsetAmount_importance <= 1.0f);
    m_balanceUsingAnimationHorizontalPelvisOffsetAmountImportance = balanceUsingAnimationHorizontalPelvisOffsetAmount_importance;
  }
  NM_INLINE float getBalanceUsingAnimationHorizontalPelvisOffsetAmountImportance() const { return m_balanceUsingAnimationHorizontalPelvisOffsetAmountImportance; }
  NM_INLINE const float& getBalanceUsingAnimationHorizontalPelvisOffsetAmountImportanceRef() const { return m_balanceUsingAnimationHorizontalPelvisOffsetAmountImportance; }
  NM_INLINE const float& getBalanceUsingAnimationHorizontalPelvisOffsetAmount() const { return balanceUsingAnimationHorizontalPelvisOffsetAmount; }

  NM_INLINE void setArmPoseStrengthWhenStepping(const float& _armPoseStrengthWhenStepping, float armPoseStrengthWhenStepping_importance = 1.f)
  {
    armPoseStrengthWhenStepping = _armPoseStrengthWhenStepping;
    postAssignArmPoseStrengthWhenStepping(armPoseStrengthWhenStepping);
    NMP_ASSERT(armPoseStrengthWhenStepping_importance >= 0.0f && armPoseStrengthWhenStepping_importance <= 1.0f);
    m_armPoseStrengthWhenSteppingImportance = armPoseStrengthWhenStepping_importance;
  }
  NM_INLINE float getArmPoseStrengthWhenSteppingImportance() const { return m_armPoseStrengthWhenSteppingImportance; }
  NM_INLINE const float& getArmPoseStrengthWhenSteppingImportanceRef() const { return m_armPoseStrengthWhenSteppingImportance; }
  NM_INLINE const float& getArmPoseStrengthWhenStepping() const { return armPoseStrengthWhenStepping; }

  NM_INLINE void setSupportWithLegs(const bool& _supportWithLegs, float supportWithLegs_importance = 1.f)
  {
    supportWithLegs = _supportWithLegs;
    NMP_ASSERT(supportWithLegs_importance >= 0.0f && supportWithLegs_importance <= 1.0f);
    m_supportWithLegsImportance = supportWithLegs_importance;
  }
  NM_INLINE float getSupportWithLegsImportance() const { return m_supportWithLegsImportance; }
  NM_INLINE const float& getSupportWithLegsImportanceRef() const { return m_supportWithLegsImportance; }
  NM_INLINE const bool& getSupportWithLegs() const { return supportWithLegs; }

  NM_INLINE void setSupportWithArms(const bool& _supportWithArms, float supportWithArms_importance = 1.f)
  {
    supportWithArms = _supportWithArms;
    NMP_ASSERT(supportWithArms_importance >= 0.0f && supportWithArms_importance <= 1.0f);
    m_supportWithArmsImportance = supportWithArms_importance;
  }
  NM_INLINE float getSupportWithArmsImportance() const { return m_supportWithArmsImportance; }
  NM_INLINE const float& getSupportWithArmsImportanceRef() const { return m_supportWithArmsImportance; }
  NM_INLINE const bool& getSupportWithArms() const { return supportWithArms; }

  NM_INLINE void setStepWithLegs(const bool& _stepWithLegs, float stepWithLegs_importance = 1.f)
  {
    stepWithLegs = _stepWithLegs;
    NMP_ASSERT(stepWithLegs_importance >= 0.0f && stepWithLegs_importance <= 1.0f);
    m_stepWithLegsImportance = stepWithLegs_importance;
  }
  NM_INLINE float getStepWithLegsImportance() const { return m_stepWithLegsImportance; }
  NM_INLINE const float& getStepWithLegsImportanceRef() const { return m_stepWithLegsImportance; }
  NM_INLINE const bool& getStepWithLegs() const { return stepWithLegs; }

  NM_INLINE void setStepWithArms(const bool& _stepWithArms, float stepWithArms_importance = 1.f)
  {
    stepWithArms = _stepWithArms;
    NMP_ASSERT(stepWithArms_importance >= 0.0f && stepWithArms_importance <= 1.0f);
    m_stepWithArmsImportance = stepWithArms_importance;
  }
  NM_INLINE float getStepWithArmsImportance() const { return m_stepWithArmsImportance; }
  NM_INLINE const float& getStepWithArmsImportanceRef() const { return m_stepWithArmsImportance; }
  NM_INLINE const bool& getStepWithArms() const { return stepWithArms; }

protected:

  PoseData armBalancePose[NetworkConstants::networkMaxNumArms];  /// These will only be output if there was really an incoming pose and we're balanced.
  StepParameters stepParameters;  // stepping
  NMP::Matrix34 armBalancePoseEndRelativeToRoot[NetworkConstants::networkMaxNumArms];  // These poses will always be set, if we're running, even if the importances are almost (but will
  // never be exactly) 0! If there was no animation input, it will default to the default pose.  (Transform)
  NMP::Matrix34 headBalancePoseEndRelativeToRoot[NetworkConstants::networkMaxNumHeads];  ///< (Transform)
  NMP::Matrix34 spineBalancePoseEndRelativeToRoot[NetworkConstants::networkMaxNumSpines];  ///< (Transform)
  NMP::Matrix34 legBalancePoseEndRelativeToRoot[NetworkConstants::networkMaxNumLegs];  ///< (Transform)
  NMP::Matrix34 averageBalancePoseArmEndRelativeToRoot;  ///< (Transform)
  NMP::Matrix34 averageBalancePoseLegEndRelativeToRoot;  ///< (Transform)
  NMP::Matrix34 armReadyPoseEndRelativeToRoot[NetworkConstants::networkMaxNumArms];  ///< (Transform)
  BalanceAssistanceParameters balanceAssistanceParameters;  // assist
  NMP::Vector3 targetPelvisDirection;  /// The requested world-space pelvis direction. Importance indicates how much of this to use.  (Direction)
  BalanceParameters balanceParameters;
  BalancePoseClamping balancePoseClamping;
  SteppingBalanceParameters steppingBalanceParameters;
  StepRecoveryParameters stepRecoveryParameters;
  float balanceUsingAnimationHorizontalPelvisOffsetAmount;  /// If 1 then the incoming horizontal pelvis offset from support should be kept. If 0 then the CoM will be used
  /// for balance.   (ClampedWeight)
  float armPoseStrengthWhenStepping;         ///< (Weight)
  bool supportWithLegs;
  bool supportWithArms;
  bool stepWithLegs;
  bool stepWithArms;

  // post-assignment stubs
  NM_INLINE void postAssignArmBalancePose(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStepParameters(const StepParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmBalancePoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "armBalancePoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadBalancePoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "headBalancePoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpineBalancePoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "spineBalancePoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegBalancePoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "legBalancePoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverageBalancePoseArmEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "averageBalancePoseArmEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverageBalancePoseLegEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "averageBalancePoseLegEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmReadyPoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "armReadyPoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceAssistanceParameters(const BalanceAssistanceParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTargetPelvisDirection(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "targetPelvisDirection");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceParameters(const BalanceParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalancePoseClamping(const BalancePoseClamping& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSteppingBalanceParameters(const SteppingBalanceParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStepRecoveryParameters(const StepRecoveryParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceUsingAnimationHorizontalPelvisOffsetAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "balanceUsingAnimationHorizontalPelvisOffsetAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmPoseStrengthWhenStepping(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "armPoseStrengthWhenStepping");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_armBalancePoseImportance[NetworkConstants::networkMaxNumArms], 
     m_stepParametersImportance, 
     m_armBalancePoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumArms], 
     m_headBalancePoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumHeads], 
     m_spineBalancePoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumSpines], 
     m_legBalancePoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumLegs], 
     m_averageBalancePoseArmEndRelativeToRootImportance, 
     m_averageBalancePoseLegEndRelativeToRootImportance, 
     m_armReadyPoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumArms], 
     m_balanceAssistanceParametersImportance, 
     m_targetPelvisDirectionImportance, 
     m_balanceParametersImportance, 
     m_balancePoseClampingImportance, 
     m_steppingBalanceParametersImportance, 
     m_stepRecoveryParametersImportance, 
     m_balanceUsingAnimationHorizontalPelvisOffsetAmountImportance, 
     m_armPoseStrengthWhenSteppingImportance, 
     m_supportWithLegsImportance, 
     m_supportWithArmsImportance, 
     m_stepWithLegsImportance, 
     m_stepWithArmsImportance;

  friend class BalanceBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numArmBalancePose = calculateNumArmBalancePose();
      for (uint32_t i=0; i<numArmBalancePose; i++)
      {
        if (getArmBalancePoseImportance(i) > 0.0f)
          armBalancePose[i].validate();
      }
    }
    if (getStepParametersImportance() > 0.0f)
    {
      stepParameters.validate();
    }
    {
      uint32_t numArmBalancePoseEndRelativeToRoot = calculateNumArmBalancePoseEndRelativeToRoot();
      for (uint32_t i=0; i<numArmBalancePoseEndRelativeToRoot; i++)
      {
        if (getArmBalancePoseEndRelativeToRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(armBalancePoseEndRelativeToRoot[i]), "armBalancePoseEndRelativeToRoot");
        }
      }
    }
    {
      uint32_t numHeadBalancePoseEndRelativeToRoot = calculateNumHeadBalancePoseEndRelativeToRoot();
      for (uint32_t i=0; i<numHeadBalancePoseEndRelativeToRoot; i++)
      {
        if (getHeadBalancePoseEndRelativeToRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(headBalancePoseEndRelativeToRoot[i]), "headBalancePoseEndRelativeToRoot");
        }
      }
    }
    {
      uint32_t numSpineBalancePoseEndRelativeToRoot = calculateNumSpineBalancePoseEndRelativeToRoot();
      for (uint32_t i=0; i<numSpineBalancePoseEndRelativeToRoot; i++)
      {
        if (getSpineBalancePoseEndRelativeToRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(spineBalancePoseEndRelativeToRoot[i]), "spineBalancePoseEndRelativeToRoot");
        }
      }
    }
    {
      uint32_t numLegBalancePoseEndRelativeToRoot = calculateNumLegBalancePoseEndRelativeToRoot();
      for (uint32_t i=0; i<numLegBalancePoseEndRelativeToRoot; i++)
      {
        if (getLegBalancePoseEndRelativeToRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(legBalancePoseEndRelativeToRoot[i]), "legBalancePoseEndRelativeToRoot");
        }
      }
    }
    if (getAverageBalancePoseArmEndRelativeToRootImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(averageBalancePoseArmEndRelativeToRoot), "averageBalancePoseArmEndRelativeToRoot");
    }
    if (getAverageBalancePoseLegEndRelativeToRootImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(averageBalancePoseLegEndRelativeToRoot), "averageBalancePoseLegEndRelativeToRoot");
    }
    {
      uint32_t numArmReadyPoseEndRelativeToRoot = calculateNumArmReadyPoseEndRelativeToRoot();
      for (uint32_t i=0; i<numArmReadyPoseEndRelativeToRoot; i++)
      {
        if (getArmReadyPoseEndRelativeToRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(armReadyPoseEndRelativeToRoot[i]), "armReadyPoseEndRelativeToRoot");
        }
      }
    }
    if (getBalanceAssistanceParametersImportance() > 0.0f)
    {
      balanceAssistanceParameters.validate();
    }
    if (getTargetPelvisDirectionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(targetPelvisDirection), "targetPelvisDirection");
    }
    if (getBalanceParametersImportance() > 0.0f)
    {
      balanceParameters.validate();
    }
    if (getBalancePoseClampingImportance() > 0.0f)
    {
      balancePoseClamping.validate();
    }
    if (getSteppingBalanceParametersImportance() > 0.0f)
    {
      steppingBalanceParameters.validate();
    }
    if (getStepRecoveryParametersImportance() > 0.0f)
    {
      stepRecoveryParameters.validate();
    }
    if (getBalanceUsingAnimationHorizontalPelvisOffsetAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(balanceUsingAnimationHorizontalPelvisOffsetAmount), "balanceUsingAnimationHorizontalPelvisOffsetAmount");
    }
    if (getArmPoseStrengthWhenSteppingImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(armPoseStrengthWhenStepping), "armPoseStrengthWhenStepping");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BalanceBehaviourInterfaceFeedbackInputs
 * \ingroup BalanceBehaviourInterface
 * \brief FeedbackInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (212 bytes, 224 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalanceBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalanceBehaviourInterfaceFeedbackInputs));
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

  NM_INLINE void setCoMBodyState(const BodyState& _CoMBodyState, float CoMBodyState_importance = 1.f)
  {
    CoMBodyState = _CoMBodyState;
    postAssignCoMBodyState(CoMBodyState);
    NMP_ASSERT(CoMBodyState_importance >= 0.0f && CoMBodyState_importance <= 1.0f);
    m_CoMBodyStateImportance = CoMBodyState_importance;
  }
  NM_INLINE BodyState& startCoMBodyStateModification()
  {
    m_CoMBodyStateImportance = -1.0f; // set invalid until stopCoMBodyStateModification()
    return CoMBodyState;
  }
  NM_INLINE void stopCoMBodyStateModification(float CoMBodyState_importance = 1.f)
  {
    postAssignCoMBodyState(CoMBodyState);
    NMP_ASSERT(CoMBodyState_importance >= 0.0f && CoMBodyState_importance <= 1.0f);
    m_CoMBodyStateImportance = CoMBodyState_importance;
  }
  NM_INLINE float getCoMBodyStateImportance() const { return m_CoMBodyStateImportance; }
  NM_INLINE const float& getCoMBodyStateImportanceRef() const { return m_CoMBodyStateImportance; }
  NM_INLINE const BodyState& getCoMBodyState() const { return CoMBodyState; }

  NM_INLINE void setSupportVelocity(const NMP::Vector3& _supportVelocity, float supportVelocity_importance = 1.f)
  {
    supportVelocity = _supportVelocity;
    postAssignSupportVelocity(supportVelocity);
    NMP_ASSERT(supportVelocity_importance >= 0.0f && supportVelocity_importance <= 1.0f);
    m_supportVelocityImportance = supportVelocity_importance;
  }
  NM_INLINE float getSupportVelocityImportance() const { return m_supportVelocityImportance; }
  NM_INLINE const float& getSupportVelocityImportanceRef() const { return m_supportVelocityImportance; }
  NM_INLINE const NMP::Vector3& getSupportVelocity() const { return supportVelocity; }

  NM_INLINE void setBalanceAmount(const float& _balanceAmount, float balanceAmount_importance = 1.f)
  {
    balanceAmount = _balanceAmount;
    postAssignBalanceAmount(balanceAmount);
    NMP_ASSERT(balanceAmount_importance >= 0.0f && balanceAmount_importance <= 1.0f);
    m_balanceAmountImportance = balanceAmount_importance;
  }
  NM_INLINE float getBalanceAmountImportance() const { return m_balanceAmountImportance; }
  NM_INLINE const float& getBalanceAmountImportanceRef() const { return m_balanceAmountImportance; }
  NM_INLINE const float& getBalanceAmount() const { return balanceAmount; }

  NM_INLINE void setMaxLegCollidingTime(const float& _maxLegCollidingTime, float maxLegCollidingTime_importance = 1.f)
  {
    maxLegCollidingTime = _maxLegCollidingTime;
    postAssignMaxLegCollidingTime(maxLegCollidingTime);
    NMP_ASSERT(maxLegCollidingTime_importance >= 0.0f && maxLegCollidingTime_importance <= 1.0f);
    m_maxLegCollidingTimeImportance = maxLegCollidingTime_importance;
  }
  NM_INLINE float getMaxLegCollidingTimeImportance() const { return m_maxLegCollidingTimeImportance; }
  NM_INLINE const float& getMaxLegCollidingTimeImportanceRef() const { return m_maxLegCollidingTimeImportance; }
  NM_INLINE const float& getMaxLegCollidingTime() const { return maxLegCollidingTime; }

  NM_INLINE void setTimeSinceLastStep(const float& _timeSinceLastStep, float timeSinceLastStep_importance = 1.f)
  {
    timeSinceLastStep = _timeSinceLastStep;
    postAssignTimeSinceLastStep(timeSinceLastStep);
    NMP_ASSERT(timeSinceLastStep_importance >= 0.0f && timeSinceLastStep_importance <= 1.0f);
    m_timeSinceLastStepImportance = timeSinceLastStep_importance;
  }
  NM_INLINE float getTimeSinceLastStepImportance() const { return m_timeSinceLastStepImportance; }
  NM_INLINE const float& getTimeSinceLastStepImportanceRef() const { return m_timeSinceLastStepImportance; }
  NM_INLINE const float& getTimeSinceLastStep() const { return timeSinceLastStep; }

  NM_INLINE void setSteppingTime(const float& _steppingTime, float steppingTime_importance = 1.f)
  {
    steppingTime = _steppingTime;
    postAssignSteppingTime(steppingTime);
    NMP_ASSERT(steppingTime_importance >= 0.0f && steppingTime_importance <= 1.0f);
    m_steppingTimeImportance = steppingTime_importance;
  }
  NM_INLINE float getSteppingTimeImportance() const { return m_steppingTimeImportance; }
  NM_INLINE const float& getSteppingTimeImportanceRef() const { return m_steppingTimeImportance; }
  NM_INLINE const float& getSteppingTime() const { return steppingTime; }

  NM_INLINE void setStepCount(const int32_t& _stepCount, float stepCount_importance = 1.f)
  {
    stepCount = _stepCount;
    NMP_ASSERT(stepCount_importance >= 0.0f && stepCount_importance <= 1.0f);
    m_stepCountImportance = stepCount_importance;
  }
  NM_INLINE float getStepCountImportance() const { return m_stepCountImportance; }
  NM_INLINE const float& getStepCountImportanceRef() const { return m_stepCountImportance; }
  NM_INLINE const int32_t& getStepCount() const { return stepCount; }

  enum { maxLegStepFractions = 2 };
  NM_INLINE unsigned int getMaxLegStepFractions() const { return maxLegStepFractions; }
  NM_INLINE void setLegStepFractions(unsigned int number, const float* _legStepFractions, float legStepFractions_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegStepFractions);
    NMP_ASSERT(legStepFractions_importance >= 0.0f && legStepFractions_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legStepFractions[i] = _legStepFractions[i];
      postAssignLegStepFractions(legStepFractions[i]);
      m_legStepFractionsImportance[i] = legStepFractions_importance;
    }
  }
  NM_INLINE void setLegStepFractionsAt(unsigned int index, const float& _legStepFractions, float legStepFractions_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegStepFractions);
    legStepFractions[index] = _legStepFractions;
    NMP_ASSERT(legStepFractions_importance >= 0.0f && legStepFractions_importance <= 1.0f);
    postAssignLegStepFractions(legStepFractions[index]);
    m_legStepFractionsImportance[index] = legStepFractions_importance;
  }
  NM_INLINE float getLegStepFractionsImportance(int index) const { return m_legStepFractionsImportance[index]; }
  NM_INLINE const float& getLegStepFractionsImportanceRef(int index) const { return m_legStepFractionsImportance[index]; }
  NM_INLINE unsigned int calculateNumLegStepFractions() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legStepFractionsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getLegStepFractions(unsigned int index) const { NMP_ASSERT(index <= maxLegStepFractions); return legStepFractions[index]; }

protected:

  ER::LimbTransform supportTM;
  BodyState CoMBodyState;
  NMP::Vector3 supportVelocity;              ///< (Velocity)
  float balanceAmount;                       ///< (Weight)
  float maxLegCollidingTime;                 ///< (TimePeriod)
  float timeSinceLastStep;  // stepping  (TimePeriod)
  float steppingTime;                        ///< (TimePeriod)
  int32_t stepCount;
  float legStepFractions[NetworkConstants::networkMaxNumLegs];  ///< Approximately how far through the step (will be 0 if not stepping)  (Multiplier)

  // post-assignment stubs
  NM_INLINE void postAssignCoMBodyState(const BodyState& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSupportVelocity(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "supportVelocity");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "balanceAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignMaxLegCollidingTime(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "maxLegCollidingTime");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTimeSinceLastStep(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "timeSinceLastStep");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSteppingTime(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "steppingTime");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegStepFractions(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "legStepFractions");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_supportTMImportance, 
     m_CoMBodyStateImportance, 
     m_supportVelocityImportance, 
     m_balanceAmountImportance, 
     m_maxLegCollidingTimeImportance, 
     m_timeSinceLastStepImportance, 
     m_steppingTimeImportance, 
     m_stepCountImportance, 
     m_legStepFractionsImportance[NetworkConstants::networkMaxNumLegs];

  friend class BalanceBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getCoMBodyStateImportance() > 0.0f)
    {
      CoMBodyState.validate();
    }
    if (getSupportVelocityImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(supportVelocity), "supportVelocity");
    }
    if (getBalanceAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(balanceAmount), "balanceAmount");
    }
    if (getMaxLegCollidingTimeImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(maxLegCollidingTime), "maxLegCollidingTime");
    }
    if (getTimeSinceLastStepImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(timeSinceLastStep), "timeSinceLastStep");
    }
    if (getSteppingTimeImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(steppingTime), "steppingTime");
    }
    {
      uint32_t numLegStepFractions = calculateNumLegStepFractions();
      for (uint32_t i=0; i<numLegStepFractions; i++)
      {
        if (getLegStepFractionsImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(legStepFractions[i]), "legStepFractions");
        }
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class BalanceBehaviourInterfaceFeedbackOutputs
 * \ingroup BalanceBehaviourInterface
 * \brief FeedbackOutputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (116 bytes, 128 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalanceBehaviourInterfaceFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalanceBehaviourInterfaceFeedbackOutputs));
  }

  NM_INLINE void setCoMBodyState(const BodyState& _CoMBodyState, float CoMBodyState_importance = 1.f)
  {
    CoMBodyState = _CoMBodyState;
    postAssignCoMBodyState(CoMBodyState);
    NMP_ASSERT(CoMBodyState_importance >= 0.0f && CoMBodyState_importance <= 1.0f);
    m_CoMBodyStateImportance = CoMBodyState_importance;
  }
  NM_INLINE BodyState& startCoMBodyStateModification()
  {
    m_CoMBodyStateImportance = -1.0f; // set invalid until stopCoMBodyStateModification()
    return CoMBodyState;
  }
  NM_INLINE void stopCoMBodyStateModification(float CoMBodyState_importance = 1.f)
  {
    postAssignCoMBodyState(CoMBodyState);
    NMP_ASSERT(CoMBodyState_importance >= 0.0f && CoMBodyState_importance <= 1.0f);
    m_CoMBodyStateImportance = CoMBodyState_importance;
  }
  NM_INLINE float getCoMBodyStateImportance() const { return m_CoMBodyStateImportance; }
  NM_INLINE const float& getCoMBodyStateImportanceRef() const { return m_CoMBodyStateImportance; }
  NM_INLINE const BodyState& getCoMBodyState() const { return CoMBodyState; }

  NM_INLINE void setStandingStillTime(const float& _standingStillTime, float standingStillTime_importance = 1.f)
  {
    standingStillTime = _standingStillTime;
    postAssignStandingStillTime(standingStillTime);
    NMP_ASSERT(standingStillTime_importance >= 0.0f && standingStillTime_importance <= 1.0f);
    m_standingStillTimeImportance = standingStillTime_importance;
  }
  NM_INLINE float getStandingStillTimeImportance() const { return m_standingStillTimeImportance; }
  NM_INLINE const float& getStandingStillTimeImportanceRef() const { return m_standingStillTimeImportance; }
  NM_INLINE const float& getStandingStillTime() const { return standingStillTime; }

  NM_INLINE void setFallenTime(const float& _fallenTime, float fallenTime_importance = 1.f)
  {
    fallenTime = _fallenTime;
    postAssignFallenTime(fallenTime);
    NMP_ASSERT(fallenTime_importance >= 0.0f && fallenTime_importance <= 1.0f);
    m_fallenTimeImportance = fallenTime_importance;
  }
  NM_INLINE float getFallenTimeImportance() const { return m_fallenTimeImportance; }
  NM_INLINE const float& getFallenTimeImportanceRef() const { return m_fallenTimeImportance; }
  NM_INLINE const float& getFallenTime() const { return fallenTime; }

  NM_INLINE void setOnGroundTime(const float& _onGroundTime, float onGroundTime_importance = 1.f)
  {
    onGroundTime = _onGroundTime;
    postAssignOnGroundTime(onGroundTime);
    NMP_ASSERT(onGroundTime_importance >= 0.0f && onGroundTime_importance <= 1.0f);
    m_onGroundTimeImportance = onGroundTime_importance;
  }
  NM_INLINE float getOnGroundTimeImportance() const { return m_onGroundTimeImportance; }
  NM_INLINE const float& getOnGroundTimeImportanceRef() const { return m_onGroundTimeImportance; }
  NM_INLINE const float& getOnGroundTime() const { return onGroundTime; }

  NM_INLINE void setBalanceAmount(const float& _balanceAmount, float balanceAmount_importance = 1.f)
  {
    balanceAmount = _balanceAmount;
    postAssignBalanceAmount(balanceAmount);
    NMP_ASSERT(balanceAmount_importance >= 0.0f && balanceAmount_importance <= 1.0f);
    m_balanceAmountImportance = balanceAmount_importance;
  }
  NM_INLINE float getBalanceAmountImportance() const { return m_balanceAmountImportance; }
  NM_INLINE const float& getBalanceAmountImportanceRef() const { return m_balanceAmountImportance; }
  NM_INLINE const float& getBalanceAmount() const { return balanceAmount; }

  NM_INLINE void setSteppingTime(const float& _steppingTime, float steppingTime_importance = 1.f)
  {
    steppingTime = _steppingTime;
    postAssignSteppingTime(steppingTime);
    NMP_ASSERT(steppingTime_importance >= 0.0f && steppingTime_importance <= 1.0f);
    m_steppingTimeImportance = steppingTime_importance;
  }
  NM_INLINE float getSteppingTimeImportance() const { return m_steppingTimeImportance; }
  NM_INLINE const float& getSteppingTimeImportanceRef() const { return m_steppingTimeImportance; }
  NM_INLINE const float& getSteppingTime() const { return steppingTime; }

  NM_INLINE void setStepCount(const int32_t& _stepCount, float stepCount_importance = 1.f)
  {
    stepCount = _stepCount;
    NMP_ASSERT(stepCount_importance >= 0.0f && stepCount_importance <= 1.0f);
    m_stepCountImportance = stepCount_importance;
  }
  NM_INLINE float getStepCountImportance() const { return m_stepCountImportance; }
  NM_INLINE const float& getStepCountImportanceRef() const { return m_stepCountImportance; }
  NM_INLINE const int32_t& getStepCount() const { return stepCount; }

protected:

  BodyState CoMBodyState;
  float standingStillTime;  ///< Time that the character has been standing still - will be zero if stepping or fallen.  (TimePeriod)
  float fallenTime;  ///< Time that the character has been fallen.   (TimePeriod)
  float onGroundTime;  ///< Time that the character has been touching something that appears to be "ground"  (TimePeriod)
  float balanceAmount;  ///< Indicator of how balanced the character is from 0 to 1  (Weight)
  float steppingTime;  // stepping  (TimePeriod)
  int32_t stepCount;

  // post-assignment stubs
  NM_INLINE void postAssignCoMBodyState(const BodyState& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStandingStillTime(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "standingStillTime");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignFallenTime(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "fallenTime");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignOnGroundTime(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "onGroundTime");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "balanceAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSteppingTime(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "steppingTime");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_CoMBodyStateImportance, 
     m_standingStillTimeImportance, 
     m_fallenTimeImportance, 
     m_onGroundTimeImportance, 
     m_balanceAmountImportance, 
     m_steppingTimeImportance, 
     m_stepCountImportance;

  friend class BalanceBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getCoMBodyStateImportance() > 0.0f)
    {
      CoMBodyState.validate();
    }
    if (getStandingStillTimeImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(standingStillTime), "standingStillTime");
    }
    if (getFallenTimeImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(fallenTime), "fallenTime");
    }
    if (getOnGroundTimeImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(onGroundTime), "onGroundTime");
    }
    if (getBalanceAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(balanceAmount), "balanceAmount");
    }
    if (getSteppingTimeImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(steppingTime), "steppingTime");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BALANCEBEHAVIOURINTERFACE_DATA_H

