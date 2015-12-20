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

#ifndef NM_MDF_STEPPINGBALANCE_DATA_H
#define NM_MDF_STEPPINGBALANCE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/SteppingBalance.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"

// known types
#include "Types/SteppingBalanceParameters.h"
#include "Types/BodyState.h"
#include "Types/TargetRequest.h"
#include "Types/PoseData.h"

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
 * \class SteppingBalanceData
 * \ingroup SteppingBalance
 * \brief Data Stepping balance control/management
 *
 * Data packet definition (41 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SteppingBalanceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SteppingBalanceData));
  }

  enum BackwardsLookDir
  {
    /*  0 */ NONE,  
    /*  1 */ LEFT,  
    /*  2 */ RIGHT,  
  };

  NMP::Vector3 horLookDir;                   ///< (Direction)
  float timeSinceLastStep;                   ///< (TimePeriod)
  float steppingTime;                        ///< (TimePeriod)
  float armPoseAmount;                       ///< (Weight)
  int32_t stepCount;
  int32_t stepLegIndex;
  uint32_t backwardsLookDir;
  bool wasStepping;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(horLookDir), "horLookDir");
    NM_VALIDATOR(FloatValid(timeSinceLastStep), "timeSinceLastStep");
    NM_VALIDATOR(FloatValid(steppingTime), "steppingTime");
    NM_VALIDATOR(FloatNonNegative(armPoseAmount), "armPoseAmount");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SteppingBalanceInputs
 * \ingroup SteppingBalance
 * \brief Inputs Stepping balance control/management
 *
 * Data packet definition (313 bytes, 320 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SteppingBalanceInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SteppingBalanceInputs));
  }

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

protected:

  NMP::Matrix34 armReadyPoseEndRelativeToRoot[NetworkConstants::networkMaxNumArms];  ///< (Transform)
  NMP::Matrix34 armBalancePoseEndRelativeToRoot[NetworkConstants::networkMaxNumArms];  ///< (Transform)
  SteppingBalanceParameters steppingBalanceParameters;

  // post-assignment stubs
  NM_INLINE void postAssignArmReadyPoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "armReadyPoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmBalancePoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "armBalancePoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSteppingBalanceParameters(const SteppingBalanceParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_armReadyPoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumArms], 
     m_armBalancePoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumArms], 
     m_steppingBalanceParametersImportance;

  friend class SteppingBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
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
    if (getSteppingBalanceParametersImportance() > 0.0f)
    {
      steppingBalanceParameters.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SteppingBalanceOutputs
 * \ingroup SteppingBalance
 * \brief Outputs Stepping balance control/management
 *
 * Data packet definition (428 bytes, 448 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SteppingBalanceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SteppingBalanceOutputs));
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

  enum { maxArmPose = 2 };
  NM_INLINE unsigned int getMaxArmPose() const { return maxArmPose; }
  NM_INLINE void setArmPose(unsigned int number, const PoseData* _armPose, float armPose_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmPose);
    NMP_ASSERT(armPose_importance >= 0.0f && armPose_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armPose[i] = _armPose[i];
      postAssignArmPose(armPose[i]);
      m_armPoseImportance[i] = armPose_importance;
    }
  }
  NM_INLINE void setArmPoseAt(unsigned int index, const PoseData& _armPose, float armPose_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmPose);
    armPose[index] = _armPose;
    NMP_ASSERT(armPose_importance >= 0.0f && armPose_importance <= 1.0f);
    postAssignArmPose(armPose[index]);
    m_armPoseImportance[index] = armPose_importance;
  }
  NM_INLINE PoseData& startArmPoseModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxArmPose);
    m_armPoseImportance[index] = -1.0f; // set invalid until stopArmPoseModificationAt()
    return armPose[index];
  }
  NM_INLINE void stopArmPoseModificationAt(unsigned int index, float armPose_importance = 1.f)
  {
    postAssignArmPose(armPose[index]);
    NMP_ASSERT(armPose_importance >= 0.0f && armPose_importance <= 1.0f);
    m_armPoseImportance[index] = armPose_importance;
  }
  NM_INLINE float getArmPoseImportance(int index) const { return m_armPoseImportance[index]; }
  NM_INLINE const float& getArmPoseImportanceRef(int index) const { return m_armPoseImportance[index]; }
  NM_INLINE unsigned int calculateNumArmPose() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armPoseImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getArmPose(unsigned int index) const { NMP_ASSERT(index <= maxArmPose); return armPose[index]; }

protected:

  TargetRequest lookTarget;  // When stepping, it's normal to look where you're going!
  PoseData armPose[NetworkConstants::networkMaxNumArms];  // These will only be output when we're stepping.

  // post-assignment stubs
  NM_INLINE void postAssignLookTarget(const TargetRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmPose(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_lookTargetImportance, 
     m_armPoseImportance[NetworkConstants::networkMaxNumArms];

  friend class SteppingBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getLookTargetImportance() > 0.0f)
    {
      lookTarget.validate();
    }
    {
      uint32_t numArmPose = calculateNumArmPose();
      for (uint32_t i=0; i<numArmPose; i++)
      {
        if (getArmPoseImportance(i) > 0.0f)
          armPose[i].validate();
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SteppingBalanceFeedbackInputs
 * \ingroup SteppingBalance
 * \brief FeedbackInputs Stepping balance control/management
 *
 * Data packet definition (143 bytes, 160 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SteppingBalanceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SteppingBalanceFeedbackInputs));
  }

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

  enum { maxStepDir = 2 };
  NM_INLINE unsigned int getMaxStepDir() const { return maxStepDir; }
  NM_INLINE void setStepDir(unsigned int number, const NMP::Vector3* _stepDir, float stepDir_importance = 1.f)
  {
    NMP_ASSERT(number <= maxStepDir);
    NMP_ASSERT(stepDir_importance >= 0.0f && stepDir_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      stepDir[i] = _stepDir[i];
      postAssignStepDir(stepDir[i]);
      m_stepDirImportance[i] = stepDir_importance;
    }
  }
  NM_INLINE void setStepDirAt(unsigned int index, const NMP::Vector3& _stepDir, float stepDir_importance = 1.f)
  {
    NMP_ASSERT(index < maxStepDir);
    stepDir[index] = _stepDir;
    NMP_ASSERT(stepDir_importance >= 0.0f && stepDir_importance <= 1.0f);
    postAssignStepDir(stepDir[index]);
    m_stepDirImportance[index] = stepDir_importance;
  }
  NM_INLINE float getStepDirImportance(int index) const { return m_stepDirImportance[index]; }
  NM_INLINE const float& getStepDirImportanceRef(int index) const { return m_stepDirImportance[index]; }
  NM_INLINE unsigned int calculateNumStepDir() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_stepDirImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Vector3& getStepDir(unsigned int index) const { NMP_ASSERT(index <= maxStepDir); return stepDir[index]; }

  NM_INLINE void setGroundVel(const NMP::Vector3& _groundVel, float groundVel_importance = 1.f)
  {
    groundVel = _groundVel;
    postAssignGroundVel(groundVel);
    NMP_ASSERT(groundVel_importance >= 0.0f && groundVel_importance <= 1.0f);
    m_groundVelImportance = groundVel_importance;
  }
  NM_INLINE float getGroundVelImportance() const { return m_groundVelImportance; }
  NM_INLINE const float& getGroundVelImportanceRef() const { return m_groundVelImportance; }
  NM_INLINE const NMP::Vector3& getGroundVel() const { return groundVel; }

  NM_INLINE void setIsDoingRecoveryStep(const bool& _isDoingRecoveryStep, float isDoingRecoveryStep_importance = 1.f)
  {
    isDoingRecoveryStep = _isDoingRecoveryStep;
    NMP_ASSERT(isDoingRecoveryStep_importance >= 0.0f && isDoingRecoveryStep_importance <= 1.0f);
    m_isDoingRecoveryStepImportance = isDoingRecoveryStep_importance;
  }
  NM_INLINE float getIsDoingRecoveryStepImportance() const { return m_isDoingRecoveryStepImportance; }
  NM_INLINE const float& getIsDoingRecoveryStepImportanceRef() const { return m_isDoingRecoveryStepImportance; }
  NM_INLINE const bool& getIsDoingRecoveryStep() const { return isDoingRecoveryStep; }

  enum { maxIsLegStepping = 2 };
  NM_INLINE unsigned int getMaxIsLegStepping() const { return maxIsLegStepping; }
  NM_INLINE void setIsLegStepping(unsigned int number, const bool* _isLegStepping, float isLegStepping_importance = 1.f)
  {
    NMP_ASSERT(number <= maxIsLegStepping);
    NMP_ASSERT(isLegStepping_importance >= 0.0f && isLegStepping_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      isLegStepping[i] = _isLegStepping[i];
      m_isLegSteppingImportance[i] = isLegStepping_importance;
    }
  }
  NM_INLINE void setIsLegSteppingAt(unsigned int index, const bool& _isLegStepping, float isLegStepping_importance = 1.f)
  {
    NMP_ASSERT(index < maxIsLegStepping);
    isLegStepping[index] = _isLegStepping;
    NMP_ASSERT(isLegStepping_importance >= 0.0f && isLegStepping_importance <= 1.0f);
    m_isLegSteppingImportance[index] = isLegStepping_importance;
  }
  NM_INLINE float getIsLegSteppingImportance(int index) const { return m_isLegSteppingImportance[index]; }
  NM_INLINE const float& getIsLegSteppingImportanceRef(int index) const { return m_isLegSteppingImportance[index]; }
  NM_INLINE unsigned int calculateNumIsLegStepping() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_isLegSteppingImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const bool& getIsLegStepping(unsigned int index) const { NMP_ASSERT(index <= maxIsLegStepping); return isLegStepping[index]; }

protected:

  BodyState centreOfMassBodyState;
  NMP::Vector3 stepDir[NetworkConstants::networkMaxNumLegs];  ///< (Direction)
  NMP::Vector3 groundVel;                    ///< (Velocity)
  bool isDoingRecoveryStep;
  bool isLegStepping[NetworkConstants::networkMaxNumLegs];

  // post-assignment stubs
  NM_INLINE void postAssignCentreOfMassBodyState(const BodyState& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStepDir(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "stepDir");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignGroundVel(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "groundVel");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_centreOfMassBodyStateImportance, 
     m_stepDirImportance[NetworkConstants::networkMaxNumLegs], 
     m_groundVelImportance, 
     m_isDoingRecoveryStepImportance, 
     m_isLegSteppingImportance[NetworkConstants::networkMaxNumLegs];

  friend class SteppingBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getCentreOfMassBodyStateImportance() > 0.0f)
    {
      centreOfMassBodyState.validate();
    }
    {
      uint32_t numStepDir = calculateNumStepDir();
      for (uint32_t i=0; i<numStepDir; i++)
      {
        if (getStepDirImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Vector3Normalised(stepDir[i]), "stepDir");
        }
      }
    }
    if (getGroundVelImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(groundVel), "groundVel");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SteppingBalanceFeedbackOutputs
 * \ingroup SteppingBalance
 * \brief FeedbackOutputs Stepping balance control/management
 *
 * Data packet definition (24 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SteppingBalanceFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SteppingBalanceFeedbackOutputs));
  }

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

protected:

  float timeSinceLastStep;                   ///< (TimePeriod)
  float steppingTime;                        ///< (TimePeriod)
  int32_t stepCount;

  // post-assignment stubs
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

  // importance values
  float
     m_timeSinceLastStepImportance, 
     m_steppingTimeImportance, 
     m_stepCountImportance;

  friend class SteppingBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getTimeSinceLastStepImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(timeSinceLastStep), "timeSinceLastStep");
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

#endif // NM_MDF_STEPPINGBALANCE_DATA_H

