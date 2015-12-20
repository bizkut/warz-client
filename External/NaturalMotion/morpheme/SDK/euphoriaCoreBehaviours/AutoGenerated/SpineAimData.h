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

#ifndef NM_MDF_SPINEAIM_DATA_H
#define NM_MDF_SPINEAIM_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/SpineAim.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/AimInfo.h"
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
 * \class SpineAimInputs
 * \ingroup SpineAim
 * \brief Inputs Module responsible for driving the character to aim at a supplied target using a supplied pose.
 *
 * Data packet definition (328 bytes, 352 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineAimInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineAimInputs));
  }

  NM_INLINE void setAimInfo(const AimInfo& _aimInfo, float aimInfo_importance = 1.f)
  {
    aimInfo = _aimInfo;
    postAssignAimInfo(aimInfo);
    NMP_ASSERT(aimInfo_importance >= 0.0f && aimInfo_importance <= 1.0f);
    m_aimInfoImportance = aimInfo_importance;
  }
  NM_INLINE AimInfo& startAimInfoModification()
  {
    m_aimInfoImportance = -1.0f; // set invalid until stopAimInfoModification()
    return aimInfo;
  }
  NM_INLINE void stopAimInfoModification(float aimInfo_importance = 1.f)
  {
    postAssignAimInfo(aimInfo);
    NMP_ASSERT(aimInfo_importance >= 0.0f && aimInfo_importance <= 1.0f);
    m_aimInfoImportance = aimInfo_importance;
  }
  NM_INLINE float getAimInfoImportance() const { return m_aimInfoImportance; }
  NM_INLINE const float& getAimInfoImportanceRef() const { return m_aimInfoImportance; }
  NM_INLINE const AimInfo& getAimInfo() const { return aimInfo; }

  NM_INLINE void setCurrentRootTM(const ER::LimbTransform& _currentRootTM, float currentRootTM_importance = 1.f)
  {
    currentRootTM = _currentRootTM;
    NMP_ASSERT(currentRootTM_importance >= 0.0f && currentRootTM_importance <= 1.0f);
    m_currentRootTMImportance = currentRootTM_importance;
  }
  NM_INLINE ER::LimbTransform& startCurrentRootTMModification()
  {
    m_currentRootTMImportance = -1.0f; // set invalid until stopCurrentRootTMModification()
    return currentRootTM;
  }
  NM_INLINE void stopCurrentRootTMModification(float currentRootTM_importance = 1.f)
  {
    NMP_ASSERT(currentRootTM_importance >= 0.0f && currentRootTM_importance <= 1.0f);
    m_currentRootTMImportance = currentRootTM_importance;
  }
  NM_INLINE float getCurrentRootTMImportance() const { return m_currentRootTMImportance; }
  NM_INLINE const float& getCurrentRootTMImportanceRef() const { return m_currentRootTMImportance; }
  NM_INLINE const ER::LimbTransform& getCurrentRootTM() const { return currentRootTM; }

  NM_INLINE void setTargetRootTM(const ER::LimbTransform& _targetRootTM, float targetRootTM_importance = 1.f)
  {
    targetRootTM = _targetRootTM;
    NMP_ASSERT(targetRootTM_importance >= 0.0f && targetRootTM_importance <= 1.0f);
    m_targetRootTMImportance = targetRootTM_importance;
  }
  NM_INLINE ER::LimbTransform& startTargetRootTMModification()
  {
    m_targetRootTMImportance = -1.0f; // set invalid until stopTargetRootTMModification()
    return targetRootTM;
  }
  NM_INLINE void stopTargetRootTMModification(float targetRootTM_importance = 1.f)
  {
    NMP_ASSERT(targetRootTM_importance >= 0.0f && targetRootTM_importance <= 1.0f);
    m_targetRootTMImportance = targetRootTM_importance;
  }
  NM_INLINE float getTargetRootTMImportance() const { return m_targetRootTMImportance; }
  NM_INLINE const float& getTargetRootTMImportanceRef() const { return m_targetRootTMImportance; }
  NM_INLINE const ER::LimbTransform& getTargetRootTM() const { return targetRootTM; }

  NM_INLINE void setTargetPositionInWorldSpace(const NMP::Vector3& _targetPositionInWorldSpace, float targetPositionInWorldSpace_importance = 1.f)
  {
    targetPositionInWorldSpace = _targetPositionInWorldSpace;
    postAssignTargetPositionInWorldSpace(targetPositionInWorldSpace);
    NMP_ASSERT(targetPositionInWorldSpace_importance >= 0.0f && targetPositionInWorldSpace_importance <= 1.0f);
    m_targetPositionInWorldSpaceImportance = targetPositionInWorldSpace_importance;
  }
  NM_INLINE float getTargetPositionInWorldSpaceImportance() const { return m_targetPositionInWorldSpaceImportance; }
  NM_INLINE const float& getTargetPositionInWorldSpaceImportanceRef() const { return m_targetPositionInWorldSpaceImportance; }
  NM_INLINE const NMP::Vector3& getTargetPositionInWorldSpace() const { return targetPositionInWorldSpace; }

  NM_INLINE void setPoseAimDirRelRoot(const NMP::Vector3& _poseAimDirRelRoot, float poseAimDirRelRoot_importance = 1.f)
  {
    poseAimDirRelRoot = _poseAimDirRelRoot;
    postAssignPoseAimDirRelRoot(poseAimDirRelRoot);
    NMP_ASSERT(poseAimDirRelRoot_importance >= 0.0f && poseAimDirRelRoot_importance <= 1.0f);
    m_poseAimDirRelRootImportance = poseAimDirRelRoot_importance;
  }
  NM_INLINE float getPoseAimDirRelRootImportance() const { return m_poseAimDirRelRootImportance; }
  NM_INLINE const float& getPoseAimDirRelRootImportanceRef() const { return m_poseAimDirRelRootImportance; }
  NM_INLINE const NMP::Vector3& getPoseAimDirRelRoot() const { return poseAimDirRelRoot; }

  NM_INLINE void setWindingDirection(const NMP::Vector3& _windingDirection, float windingDirection_importance = 1.f)
  {
    windingDirection = _windingDirection;
    postAssignWindingDirection(windingDirection);
    NMP_ASSERT(windingDirection_importance >= 0.0f && windingDirection_importance <= 1.0f);
    m_windingDirectionImportance = windingDirection_importance;
  }
  NM_INLINE float getWindingDirectionImportance() const { return m_windingDirectionImportance; }
  NM_INLINE const float& getWindingDirectionImportanceRef() const { return m_windingDirectionImportance; }
  NM_INLINE const NMP::Vector3& getWindingDirection() const { return windingDirection; }

protected:

  AimInfo aimInfo;
  ER::LimbTransform currentRootTM;
  ER::LimbTransform targetRootTM;
  NMP::Vector3 targetPositionInWorldSpace;  ///< The position the character should aim at now (in world space).  (Position)
  NMP::Vector3 poseAimDirRelRoot;            ///< (Direction)
  NMP::Vector3 windingDirection;             ///< (Direction)

  // post-assignment stubs
  NM_INLINE void postAssignAimInfo(const AimInfo& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTargetPositionInWorldSpace(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "targetPositionInWorldSpace");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPoseAimDirRelRoot(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "poseAimDirRelRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignWindingDirection(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "windingDirection");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_aimInfoImportance, 
     m_currentRootTMImportance, 
     m_targetRootTMImportance, 
     m_targetPositionInWorldSpaceImportance, 
     m_poseAimDirRelRootImportance, 
     m_windingDirectionImportance;

  friend class SpineAim_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getAimInfoImportance() > 0.0f)
    {
      aimInfo.validate();
    }
    if (getTargetPositionInWorldSpaceImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(targetPositionInWorldSpace), "targetPositionInWorldSpace");
    }
    if (getPoseAimDirRelRootImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(poseAimDirRelRoot), "poseAimDirRelRoot");
    }
    if (getWindingDirectionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(windingDirection), "windingDirection");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SpineAimOutputs
 * \ingroup SpineAim
 * \brief Outputs Module responsible for driving the character to aim at a supplied target using a supplied pose.
 *
 * Data packet definition (456 bytes, 480 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SpineAimOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SpineAimOutputs));
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

  NM_INLINE void setTargetEndTM(const ER::LimbTransform& _targetEndTM, float targetEndTM_importance = 1.f)
  {
    targetEndTM = _targetEndTM;
    NMP_ASSERT(targetEndTM_importance >= 0.0f && targetEndTM_importance <= 1.0f);
    m_targetEndTMImportance = targetEndTM_importance;
  }
  NM_INLINE ER::LimbTransform& startTargetEndTMModification()
  {
    m_targetEndTMImportance = -1.0f; // set invalid until stopTargetEndTMModification()
    return targetEndTM;
  }
  NM_INLINE void stopTargetEndTMModification(float targetEndTM_importance = 1.f)
  {
    NMP_ASSERT(targetEndTM_importance >= 0.0f && targetEndTM_importance <= 1.0f);
    m_targetEndTMImportance = targetEndTM_importance;
  }
  NM_INLINE float getTargetEndTMImportance() const { return m_targetEndTMImportance; }
  NM_INLINE const float& getTargetEndTMImportanceRef() const { return m_targetEndTMImportance; }
  NM_INLINE const ER::LimbTransform& getTargetEndTM() const { return targetEndTM; }

protected:

  LimbControl control;
  ER::LimbTransform targetEndTM;

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
     m_targetEndTMImportance;

  friend class SpineAim_Con;

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

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SPINEAIM_DATA_H

