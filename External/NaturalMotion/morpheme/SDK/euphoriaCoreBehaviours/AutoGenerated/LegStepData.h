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

#ifndef NM_MDF_LEGSTEP_DATA_H
#define NM_MDF_LEGSTEP_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/LegStep.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/StepParameters.h"
#include "Types/LimbControl.h"

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
 * \class LegStepData
 * \ingroup LegStep
 * \brief Data Make the leg move to prepare to support the pelvis
 *
 * Data packet definition (139 bytes, 160 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegStepData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegStepData));
  }

  NMP::Matrix34 recoveryStepTarget;          ///< (Transform)
  NMP::Vector3 averageOtherEndPos;  // These are calculated during the feedback  (Position)
  NMP::Vector3 stepTargetPos;                ///< (Position)
  NMP::Vector3 stepStartDelta;  // These are calculated just once at the start of the step and then don't change  (PositionDelta)
  NMP::Vector3 stepDir;                      ///< (Direction)
  float stepTime;                            ///< (TimePeriod)
  float stepLength;                          ///< (Length)
  bool isStepping;  // These are calculated during the update
  bool isDoingRecoveryStep;
  bool hasEndLeftGround;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(recoveryStepTarget), "recoveryStepTarget");
    NM_VALIDATOR(Vector3Valid(averageOtherEndPos), "averageOtherEndPos");
    NM_VALIDATOR(Vector3Valid(stepTargetPos), "stepTargetPos");
    NM_VALIDATOR(Vector3Valid(stepStartDelta), "stepStartDelta");
    NM_VALIDATOR(Vector3Normalised(stepDir), "stepDir");
    NM_VALIDATOR(FloatValid(stepTime), "stepTime");
    NM_VALIDATOR(FloatNonNegative(stepLength), "stepLength");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class LegStepInputs
 * \ingroup LegStep
 * \brief Inputs Make the leg move to prepare to support the pelvis
 *
 * Data packet definition (527 bytes, 544 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegStepInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegStepInputs));
  }

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

  NM_INLINE void setRecoveryStepTarget(const NMP::Matrix34& _recoveryStepTarget, float recoveryStepTarget_importance = 1.f)
  {
    recoveryStepTarget = _recoveryStepTarget;
    postAssignRecoveryStepTarget(recoveryStepTarget);
    NMP_ASSERT(recoveryStepTarget_importance >= 0.0f && recoveryStepTarget_importance <= 1.0f);
    m_recoveryStepTargetImportance = recoveryStepTarget_importance;
  }
  NM_INLINE float getRecoveryStepTargetImportance() const { return m_recoveryStepTargetImportance; }
  NM_INLINE const float& getRecoveryStepTargetImportanceRef() const { return m_recoveryStepTargetImportance; }
  NM_INLINE const NMP::Matrix34& getRecoveryStepTarget() const { return recoveryStepTarget; }

  enum { maxStandingPoseEndRelativeToSupport = 2 };
  NM_INLINE unsigned int getMaxStandingPoseEndRelativeToSupport() const { return maxStandingPoseEndRelativeToSupport; }
  NM_INLINE void setStandingPoseEndRelativeToSupport(unsigned int number, const NMP::Matrix34* _standingPoseEndRelativeToSupport, float standingPoseEndRelativeToSupport_importance = 1.f)
  {
    NMP_ASSERT(number <= maxStandingPoseEndRelativeToSupport);
    NMP_ASSERT(standingPoseEndRelativeToSupport_importance >= 0.0f && standingPoseEndRelativeToSupport_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      standingPoseEndRelativeToSupport[i] = _standingPoseEndRelativeToSupport[i];
      postAssignStandingPoseEndRelativeToSupport(standingPoseEndRelativeToSupport[i]);
      m_standingPoseEndRelativeToSupportImportance[i] = standingPoseEndRelativeToSupport_importance;
    }
  }
  NM_INLINE void setStandingPoseEndRelativeToSupportAt(unsigned int index, const NMP::Matrix34& _standingPoseEndRelativeToSupport, float standingPoseEndRelativeToSupport_importance = 1.f)
  {
    NMP_ASSERT(index < maxStandingPoseEndRelativeToSupport);
    standingPoseEndRelativeToSupport[index] = _standingPoseEndRelativeToSupport;
    NMP_ASSERT(standingPoseEndRelativeToSupport_importance >= 0.0f && standingPoseEndRelativeToSupport_importance <= 1.0f);
    postAssignStandingPoseEndRelativeToSupport(standingPoseEndRelativeToSupport[index]);
    m_standingPoseEndRelativeToSupportImportance[index] = standingPoseEndRelativeToSupport_importance;
  }
  NM_INLINE float getStandingPoseEndRelativeToSupportImportance(int index) const { return m_standingPoseEndRelativeToSupportImportance[index]; }
  NM_INLINE const float& getStandingPoseEndRelativeToSupportImportanceRef(int index) const { return m_standingPoseEndRelativeToSupportImportance[index]; }
  NM_INLINE unsigned int calculateNumStandingPoseEndRelativeToSupport() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_standingPoseEndRelativeToSupportImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getStandingPoseEndRelativeToSupport(unsigned int index) const { NMP_ASSERT(index <= maxStandingPoseEndRelativeToSupport); return standingPoseEndRelativeToSupport[index]; }

  NM_INLINE void setAverageStandingPoseEndRelativeToSupport(const NMP::Matrix34& _averageStandingPoseEndRelativeToSupport, float averageStandingPoseEndRelativeToSupport_importance = 1.f)
  {
    averageStandingPoseEndRelativeToSupport = _averageStandingPoseEndRelativeToSupport;
    postAssignAverageStandingPoseEndRelativeToSupport(averageStandingPoseEndRelativeToSupport);
    NMP_ASSERT(averageStandingPoseEndRelativeToSupport_importance >= 0.0f && averageStandingPoseEndRelativeToSupport_importance <= 1.0f);
    m_averageStandingPoseEndRelativeToSupportImportance = averageStandingPoseEndRelativeToSupport_importance;
  }
  NM_INLINE float getAverageStandingPoseEndRelativeToSupportImportance() const { return m_averageStandingPoseEndRelativeToSupportImportance; }
  NM_INLINE const float& getAverageStandingPoseEndRelativeToSupportImportanceRef() const { return m_averageStandingPoseEndRelativeToSupportImportance; }
  NM_INLINE const NMP::Matrix34& getAverageStandingPoseEndRelativeToSupport() const { return averageStandingPoseEndRelativeToSupport; }

  NM_INLINE void setAverageEndContactNormal(const NMP::Vector3& _averageEndContactNormal, float averageEndContactNormal_importance = 1.f)
  {
    averageEndContactNormal = _averageEndContactNormal;
    postAssignAverageEndContactNormal(averageEndContactNormal);
    NMP_ASSERT(averageEndContactNormal_importance >= 0.0f && averageEndContactNormal_importance <= 1.0f);
    m_averageEndContactNormalImportance = averageEndContactNormal_importance;
  }
  NM_INLINE float getAverageEndContactNormalImportance() const { return m_averageEndContactNormalImportance; }
  NM_INLINE const float& getAverageEndContactNormalImportanceRef() const { return m_averageEndContactNormalImportance; }
  NM_INLINE const NMP::Vector3& getAverageEndContactNormal() const { return averageEndContactNormal; }

  NM_INLINE void setGroundVelocity(const NMP::Vector3& _groundVelocity, float groundVelocity_importance = 1.f)
  {
    groundVelocity = _groundVelocity;
    postAssignGroundVelocity(groundVelocity);
    NMP_ASSERT(groundVelocity_importance >= 0.0f && groundVelocity_importance <= 1.0f);
    m_groundVelocityImportance = groundVelocity_importance;
  }
  NM_INLINE float getGroundVelocityImportance() const { return m_groundVelocityImportance; }
  NM_INLINE const float& getGroundVelocityImportanceRef() const { return m_groundVelocityImportance; }
  NM_INLINE const NMP::Vector3& getGroundVelocity() const { return groundVelocity; }

  NM_INLINE void setAllowStep(const bool& _allowStep, float allowStep_importance = 1.f)
  {
    allowStep = _allowStep;
    NMP_ASSERT(allowStep_importance >= 0.0f && allowStep_importance <= 1.0f);
    m_allowStepImportance = allowStep_importance;
  }
  NM_INLINE float getAllowStepImportance() const { return m_allowStepImportance; }
  NM_INLINE const float& getAllowStepImportanceRef() const { return m_allowStepImportance; }
  NM_INLINE const bool& getAllowStep() const { return allowStep; }

  NM_INLINE void setEnableStep(const bool& _enableStep, float enableStep_importance = 1.f)
  {
    enableStep = _enableStep;
    NMP_ASSERT(enableStep_importance >= 0.0f && enableStep_importance <= 1.0f);
    m_enableStepImportance = enableStep_importance;
  }
  NM_INLINE float getEnableStepImportance() const { return m_enableStepImportance; }
  NM_INLINE const float& getEnableStepImportanceRef() const { return m_enableStepImportance; }
  NM_INLINE const bool& getEnableStep() const { return enableStep; }

  NM_INLINE void setEvaluateStep(const bool& _evaluateStep, float evaluateStep_importance = 1.f)
  {
    evaluateStep = _evaluateStep;
    NMP_ASSERT(evaluateStep_importance >= 0.0f && evaluateStep_importance <= 1.0f);
    m_evaluateStepImportance = evaluateStep_importance;
  }
  NM_INLINE float getEvaluateStepImportance() const { return m_evaluateStepImportance; }
  NM_INLINE const float& getEvaluateStepImportanceRef() const { return m_evaluateStepImportance; }
  NM_INLINE const bool& getEvaluateStep() const { return evaluateStep; }

protected:

  StepParameters stepParameters;
  ER::LimbTransform supportTM;
  NMP::Matrix34 recoveryStepTarget;          ///< (Transform)
  NMP::Matrix34 standingPoseEndRelativeToSupport[NetworkConstants::networkMaxNumLegs];  ///< (Transform)
  NMP::Matrix34 averageStandingPoseEndRelativeToSupport;  ///< (Transform)
  NMP::Vector3 averageEndContactNormal;      ///< (Direction)
  NMP::Vector3 groundVelocity;               ///< (Velocity)
  bool allowStep;
  bool enableStep;
  bool evaluateStep;

  // post-assignment stubs
  NM_INLINE void postAssignStepParameters(const StepParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignRecoveryStepTarget(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "recoveryStepTarget");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStandingPoseEndRelativeToSupport(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "standingPoseEndRelativeToSupport");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverageStandingPoseEndRelativeToSupport(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "averageStandingPoseEndRelativeToSupport");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverageEndContactNormal(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "averageEndContactNormal");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignGroundVelocity(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "groundVelocity");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_stepParametersImportance, 
     m_supportTMImportance, 
     m_recoveryStepTargetImportance, 
     m_standingPoseEndRelativeToSupportImportance[NetworkConstants::networkMaxNumLegs], 
     m_averageStandingPoseEndRelativeToSupportImportance, 
     m_averageEndContactNormalImportance, 
     m_groundVelocityImportance, 
     m_allowStepImportance, 
     m_enableStepImportance, 
     m_evaluateStepImportance;

  friend class LegStep_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getStepParametersImportance() > 0.0f)
    {
      stepParameters.validate();
    }
    if (getRecoveryStepTargetImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(recoveryStepTarget), "recoveryStepTarget");
    }
    {
      uint32_t numStandingPoseEndRelativeToSupport = calculateNumStandingPoseEndRelativeToSupport();
      for (uint32_t i=0; i<numStandingPoseEndRelativeToSupport; i++)
      {
        if (getStandingPoseEndRelativeToSupportImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(standingPoseEndRelativeToSupport[i]), "standingPoseEndRelativeToSupport");
        }
      }
    }
    if (getAverageStandingPoseEndRelativeToSupportImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(averageStandingPoseEndRelativeToSupport), "averageStandingPoseEndRelativeToSupport");
    }
    if (getAverageEndContactNormalImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(averageEndContactNormal), "averageEndContactNormal");
    }
    if (getGroundVelocityImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(groundVelocity), "groundVelocity");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class LegStepOutputs
 * \ingroup LegStep
 * \brief Outputs Make the leg move to prepare to support the pelvis
 *
 * Data packet definition (393 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegStepOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegStepOutputs));
  }

  NM_INLINE void setControl(const LimbControl& _control, float control_importance = 1.f)
  {
    control = _control;
    postAssignControl(control);
    NMP_ASSERT(control_importance >= 0.0f && control_importance <= 1.0f);
    m_controlImportance = control_importance;
  }
  NM_INLINE LimbControl& startControlModification()
  {
    m_controlImportance = -1.0f; // set invalid until stopControlModification()
    return control;
  }
  NM_INLINE void stopControlModification(float control_importance = 1.f)
  {
    postAssignControl(control);
    NMP_ASSERT(control_importance >= 0.0f && control_importance <= 1.0f);
    m_controlImportance = control_importance;
  }
  NM_INLINE float getControlImportance() const { return m_controlImportance; }
  NM_INLINE const float& getControlImportanceRef() const { return m_controlImportance; }
  NM_INLINE const LimbControl& getControl() const { return control; }

  NM_INLINE void setUseFullEndJointRange(const bool& _useFullEndJointRange, float useFullEndJointRange_importance = 1.f)
  {
    useFullEndJointRange = _useFullEndJointRange;
    NMP_ASSERT(useFullEndJointRange_importance >= 0.0f && useFullEndJointRange_importance <= 1.0f);
    m_useFullEndJointRangeImportance = useFullEndJointRange_importance;
  }
  NM_INLINE float getUseFullEndJointRangeImportance() const { return m_useFullEndJointRangeImportance; }
  NM_INLINE const float& getUseFullEndJointRangeImportanceRef() const { return m_useFullEndJointRangeImportance; }
  NM_INLINE const bool& getUseFullEndJointRange() const { return useFullEndJointRange; }

protected:

  LimbControl control;
  bool useFullEndJointRange;

  // post-assignment stubs
  NM_INLINE void postAssignControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_controlImportance, 
     m_useFullEndJointRangeImportance;

  friend class LegStep_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getControlImportance() > 0.0f)
    {
      control.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class LegStepFeedbackOutputs
 * \ingroup LegStep
 * \brief FeedbackOutputs Make the leg move to prepare to support the pelvis
 *
 * Data packet definition (59 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct LegStepFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(LegStepFeedbackOutputs));
  }

  NM_INLINE void setStepDir(const NMP::Vector3& _stepDir, float stepDir_importance = 1.f)
  {
    stepDir = _stepDir;
    postAssignStepDir(stepDir);
    NMP_ASSERT(stepDir_importance >= 0.0f && stepDir_importance <= 1.0f);
    m_stepDirImportance = stepDir_importance;
  }
  NM_INLINE float getStepDirImportance() const { return m_stepDirImportance; }
  NM_INLINE const float& getStepDirImportanceRef() const { return m_stepDirImportance; }
  NM_INLINE const NMP::Vector3& getStepDir() const { return stepDir; }

  NM_INLINE void setStepLength(const float& _stepLength, float stepLength_importance = 1.f)
  {
    stepLength = _stepLength;
    postAssignStepLength(stepLength);
    NMP_ASSERT(stepLength_importance >= 0.0f && stepLength_importance <= 1.0f);
    m_stepLengthImportance = stepLength_importance;
  }
  NM_INLINE float getStepLengthImportance() const { return m_stepLengthImportance; }
  NM_INLINE const float& getStepLengthImportanceRef() const { return m_stepLengthImportance; }
  NM_INLINE const float& getStepLength() const { return stepLength; }

  NM_INLINE void setLowerRootDistance(const float& _lowerRootDistance, float lowerRootDistance_importance = 1.f)
  {
    lowerRootDistance = _lowerRootDistance;
    postAssignLowerRootDistance(lowerRootDistance);
    NMP_ASSERT(lowerRootDistance_importance >= 0.0f && lowerRootDistance_importance <= 1.0f);
    m_lowerRootDistanceImportance = lowerRootDistance_importance;
  }
  NM_INLINE float getLowerRootDistanceImportance() const { return m_lowerRootDistanceImportance; }
  NM_INLINE const float& getLowerRootDistanceImportanceRef() const { return m_lowerRootDistanceImportance; }
  NM_INLINE const float& getLowerRootDistance() const { return lowerRootDistance; }

  NM_INLINE void setStepFraction(const float& _stepFraction, float stepFraction_importance = 1.f)
  {
    stepFraction = _stepFraction;
    postAssignStepFraction(stepFraction);
    NMP_ASSERT(stepFraction_importance >= 0.0f && stepFraction_importance <= 1.0f);
    m_stepFractionImportance = stepFraction_importance;
  }
  NM_INLINE float getStepFractionImportance() const { return m_stepFractionImportance; }
  NM_INLINE const float& getStepFractionImportanceRef() const { return m_stepFractionImportance; }
  NM_INLINE const float& getStepFraction() const { return stepFraction; }

  NM_INLINE void setIsStepping(const bool& _isStepping, float isStepping_importance = 1.f)
  {
    isStepping = _isStepping;
    NMP_ASSERT(isStepping_importance >= 0.0f && isStepping_importance <= 1.0f);
    m_isSteppingImportance = isStepping_importance;
  }
  NM_INLINE float getIsSteppingImportance() const { return m_isSteppingImportance; }
  NM_INLINE const float& getIsSteppingImportanceRef() const { return m_isSteppingImportance; }
  NM_INLINE const bool& getIsStepping() const { return isStepping; }

  NM_INLINE void setIsSteppingBeingSuppressed(const bool& _isSteppingBeingSuppressed, float isSteppingBeingSuppressed_importance = 1.f)
  {
    isSteppingBeingSuppressed = _isSteppingBeingSuppressed;
    NMP_ASSERT(isSteppingBeingSuppressed_importance >= 0.0f && isSteppingBeingSuppressed_importance <= 1.0f);
    m_isSteppingBeingSuppressedImportance = isSteppingBeingSuppressed_importance;
  }
  NM_INLINE float getIsSteppingBeingSuppressedImportance() const { return m_isSteppingBeingSuppressedImportance; }
  NM_INLINE const float& getIsSteppingBeingSuppressedImportanceRef() const { return m_isSteppingBeingSuppressedImportance; }
  NM_INLINE const bool& getIsSteppingBeingSuppressed() const { return isSteppingBeingSuppressed; }

  NM_INLINE void setIsDoingRecoveryStep(const bool& _isDoingRecoveryStep, float isDoingRecoveryStep_importance = 1.f)
  {
    isDoingRecoveryStep = _isDoingRecoveryStep;
    NMP_ASSERT(isDoingRecoveryStep_importance >= 0.0f && isDoingRecoveryStep_importance <= 1.0f);
    m_isDoingRecoveryStepImportance = isDoingRecoveryStep_importance;
  }
  NM_INLINE float getIsDoingRecoveryStepImportance() const { return m_isDoingRecoveryStepImportance; }
  NM_INLINE const float& getIsDoingRecoveryStepImportanceRef() const { return m_isDoingRecoveryStepImportance; }
  NM_INLINE const bool& getIsDoingRecoveryStep() const { return isDoingRecoveryStep; }

protected:

  NMP::Vector3 stepDir;                      ///< (Direction)
  float stepLength;                          ///< (Length)
  float lowerRootDistance;                   ///< (Distance)
  float stepFraction;  ///< Approximately how far through the step (will be 0 if not stepping)  (Multiplier)
  bool isStepping;
  bool isSteppingBeingSuppressed;
  bool isDoingRecoveryStep;

  // post-assignment stubs
  NM_INLINE void postAssignStepDir(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "stepDir");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStepLength(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "stepLength");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLowerRootDistance(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "lowerRootDistance");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStepFraction(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "stepFraction");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_stepDirImportance, 
     m_stepLengthImportance, 
     m_lowerRootDistanceImportance, 
     m_stepFractionImportance, 
     m_isSteppingImportance, 
     m_isSteppingBeingSuppressedImportance, 
     m_isDoingRecoveryStepImportance;

  friend class LegStep_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getStepDirImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(stepDir), "stepDir");
    }
    if (getStepLengthImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(stepLength), "stepLength");
    }
    if (getLowerRootDistanceImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(lowerRootDistance), "lowerRootDistance");
    }
    if (getStepFractionImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(stepFraction), "stepFraction");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_LEGSTEP_DATA_H

