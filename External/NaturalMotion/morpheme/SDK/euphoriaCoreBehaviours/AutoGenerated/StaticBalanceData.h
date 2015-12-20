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

#ifndef NM_MDF_STATICBALANCE_DATA_H
#define NM_MDF_STATICBALANCE_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/StaticBalance.module"

// external types
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"
#include "euphoria/erLimbTransforms.h"

// known types
#include "Types/RotationRequest.h"
#include "Types/DirectionRequest.h"
#include "Types/TranslationRequest.h"

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
 * \class StaticBalanceInputs
 * \ingroup StaticBalance
 * \brief Inputs Static balance control/management
 *
 * Data packet definition (488 bytes, 512 aligned)
 */
NMP_ALIGN_PREFIX(32) struct StaticBalanceInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(StaticBalanceInputs));
  }

  enum { maxHeadPoseEndRelativeToRoot = 1 };
  NM_INLINE unsigned int getMaxHeadPoseEndRelativeToRoot() const { return maxHeadPoseEndRelativeToRoot; }
  NM_INLINE void setHeadPoseEndRelativeToRoot(unsigned int number, const NMP::Matrix34* _headPoseEndRelativeToRoot, float headPoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHeadPoseEndRelativeToRoot);
    NMP_ASSERT(headPoseEndRelativeToRoot_importance >= 0.0f && headPoseEndRelativeToRoot_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      headPoseEndRelativeToRoot[i] = _headPoseEndRelativeToRoot[i];
      postAssignHeadPoseEndRelativeToRoot(headPoseEndRelativeToRoot[i]);
      m_headPoseEndRelativeToRootImportance[i] = headPoseEndRelativeToRoot_importance;
    }
  }
  NM_INLINE void setHeadPoseEndRelativeToRootAt(unsigned int index, const NMP::Matrix34& _headPoseEndRelativeToRoot, float headPoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(index < maxHeadPoseEndRelativeToRoot);
    headPoseEndRelativeToRoot[index] = _headPoseEndRelativeToRoot;
    NMP_ASSERT(headPoseEndRelativeToRoot_importance >= 0.0f && headPoseEndRelativeToRoot_importance <= 1.0f);
    postAssignHeadPoseEndRelativeToRoot(headPoseEndRelativeToRoot[index]);
    m_headPoseEndRelativeToRootImportance[index] = headPoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getHeadPoseEndRelativeToRootImportance(int index) const { return m_headPoseEndRelativeToRootImportance[index]; }
  NM_INLINE const float& getHeadPoseEndRelativeToRootImportanceRef(int index) const { return m_headPoseEndRelativeToRootImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadPoseEndRelativeToRoot() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headPoseEndRelativeToRootImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getHeadPoseEndRelativeToRoot(unsigned int index) const { NMP_ASSERT(index <= maxHeadPoseEndRelativeToRoot); return headPoseEndRelativeToRoot[index]; }

  enum { maxSpinePoseEndRelativeToRoot = 1 };
  NM_INLINE unsigned int getMaxSpinePoseEndRelativeToRoot() const { return maxSpinePoseEndRelativeToRoot; }
  NM_INLINE void setSpinePoseEndRelativeToRoot(unsigned int number, const NMP::Matrix34* _spinePoseEndRelativeToRoot, float spinePoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpinePoseEndRelativeToRoot);
    NMP_ASSERT(spinePoseEndRelativeToRoot_importance >= 0.0f && spinePoseEndRelativeToRoot_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spinePoseEndRelativeToRoot[i] = _spinePoseEndRelativeToRoot[i];
      postAssignSpinePoseEndRelativeToRoot(spinePoseEndRelativeToRoot[i]);
      m_spinePoseEndRelativeToRootImportance[i] = spinePoseEndRelativeToRoot_importance;
    }
  }
  NM_INLINE void setSpinePoseEndRelativeToRootAt(unsigned int index, const NMP::Matrix34& _spinePoseEndRelativeToRoot, float spinePoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpinePoseEndRelativeToRoot);
    spinePoseEndRelativeToRoot[index] = _spinePoseEndRelativeToRoot;
    NMP_ASSERT(spinePoseEndRelativeToRoot_importance >= 0.0f && spinePoseEndRelativeToRoot_importance <= 1.0f);
    postAssignSpinePoseEndRelativeToRoot(spinePoseEndRelativeToRoot[index]);
    m_spinePoseEndRelativeToRootImportance[index] = spinePoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getSpinePoseEndRelativeToRootImportance(int index) const { return m_spinePoseEndRelativeToRootImportance[index]; }
  NM_INLINE const float& getSpinePoseEndRelativeToRootImportanceRef(int index) const { return m_spinePoseEndRelativeToRootImportance[index]; }
  NM_INLINE unsigned int calculateNumSpinePoseEndRelativeToRoot() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spinePoseEndRelativeToRootImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getSpinePoseEndRelativeToRoot(unsigned int index) const { NMP_ASSERT(index <= maxSpinePoseEndRelativeToRoot); return spinePoseEndRelativeToRoot[index]; }

  enum { maxLegPoseEndRelativeToRoot = 2 };
  NM_INLINE unsigned int getMaxLegPoseEndRelativeToRoot() const { return maxLegPoseEndRelativeToRoot; }
  NM_INLINE void setLegPoseEndRelativeToRoot(unsigned int number, const NMP::Matrix34* _legPoseEndRelativeToRoot, float legPoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegPoseEndRelativeToRoot);
    NMP_ASSERT(legPoseEndRelativeToRoot_importance >= 0.0f && legPoseEndRelativeToRoot_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legPoseEndRelativeToRoot[i] = _legPoseEndRelativeToRoot[i];
      postAssignLegPoseEndRelativeToRoot(legPoseEndRelativeToRoot[i]);
      m_legPoseEndRelativeToRootImportance[i] = legPoseEndRelativeToRoot_importance;
    }
  }
  NM_INLINE void setLegPoseEndRelativeToRootAt(unsigned int index, const NMP::Matrix34& _legPoseEndRelativeToRoot, float legPoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegPoseEndRelativeToRoot);
    legPoseEndRelativeToRoot[index] = _legPoseEndRelativeToRoot;
    NMP_ASSERT(legPoseEndRelativeToRoot_importance >= 0.0f && legPoseEndRelativeToRoot_importance <= 1.0f);
    postAssignLegPoseEndRelativeToRoot(legPoseEndRelativeToRoot[index]);
    m_legPoseEndRelativeToRootImportance[index] = legPoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getLegPoseEndRelativeToRootImportance(int index) const { return m_legPoseEndRelativeToRootImportance[index]; }
  NM_INLINE const float& getLegPoseEndRelativeToRootImportanceRef(int index) const { return m_legPoseEndRelativeToRootImportance[index]; }
  NM_INLINE unsigned int calculateNumLegPoseEndRelativeToRoot() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legPoseEndRelativeToRootImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getLegPoseEndRelativeToRoot(unsigned int index) const { NMP_ASSERT(index <= maxLegPoseEndRelativeToRoot); return legPoseEndRelativeToRoot[index]; }

  enum { maxArmPoseEndRelativeToRoot = 2 };
  NM_INLINE unsigned int getMaxArmPoseEndRelativeToRoot() const { return maxArmPoseEndRelativeToRoot; }
  NM_INLINE void setArmPoseEndRelativeToRoot(unsigned int number, const NMP::Matrix34* _armPoseEndRelativeToRoot, float armPoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmPoseEndRelativeToRoot);
    NMP_ASSERT(armPoseEndRelativeToRoot_importance >= 0.0f && armPoseEndRelativeToRoot_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armPoseEndRelativeToRoot[i] = _armPoseEndRelativeToRoot[i];
      postAssignArmPoseEndRelativeToRoot(armPoseEndRelativeToRoot[i]);
      m_armPoseEndRelativeToRootImportance[i] = armPoseEndRelativeToRoot_importance;
    }
  }
  NM_INLINE void setArmPoseEndRelativeToRootAt(unsigned int index, const NMP::Matrix34& _armPoseEndRelativeToRoot, float armPoseEndRelativeToRoot_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmPoseEndRelativeToRoot);
    armPoseEndRelativeToRoot[index] = _armPoseEndRelativeToRoot;
    NMP_ASSERT(armPoseEndRelativeToRoot_importance >= 0.0f && armPoseEndRelativeToRoot_importance <= 1.0f);
    postAssignArmPoseEndRelativeToRoot(armPoseEndRelativeToRoot[index]);
    m_armPoseEndRelativeToRootImportance[index] = armPoseEndRelativeToRoot_importance;
  }
  NM_INLINE float getArmPoseEndRelativeToRootImportance(int index) const { return m_armPoseEndRelativeToRootImportance[index]; }
  NM_INLINE const float& getArmPoseEndRelativeToRootImportanceRef(int index) const { return m_armPoseEndRelativeToRootImportance[index]; }
  NM_INLINE unsigned int calculateNumArmPoseEndRelativeToRoot() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armPoseEndRelativeToRootImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getArmPoseEndRelativeToRoot(unsigned int index) const { NMP_ASSERT(index <= maxArmPoseEndRelativeToRoot); return armPoseEndRelativeToRoot[index]; }

  NM_INLINE void setTargetPelvisOrientation(const NMP::Quat& _targetPelvisOrientation, float targetPelvisOrientation_importance = 1.f)
  {
    targetPelvisOrientation = _targetPelvisOrientation;
    postAssignTargetPelvisOrientation(targetPelvisOrientation);
    NMP_ASSERT(targetPelvisOrientation_importance >= 0.0f && targetPelvisOrientation_importance <= 1.0f);
    m_targetPelvisOrientationImportance = targetPelvisOrientation_importance;
  }
  NM_INLINE float getTargetPelvisOrientationImportance() const { return m_targetPelvisOrientationImportance; }
  NM_INLINE const float& getTargetPelvisOrientationImportanceRef() const { return m_targetPelvisOrientationImportance; }
  NM_INLINE const NMP::Quat& getTargetPelvisOrientation() const { return targetPelvisOrientation; }

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

  NM_INLINE void setBalanceOffsetFwd(const float& _balanceOffsetFwd, float balanceOffsetFwd_importance = 1.f)
  {
    balanceOffsetFwd = _balanceOffsetFwd;
    postAssignBalanceOffsetFwd(balanceOffsetFwd);
    NMP_ASSERT(balanceOffsetFwd_importance >= 0.0f && balanceOffsetFwd_importance <= 1.0f);
    m_balanceOffsetFwdImportance = balanceOffsetFwd_importance;
  }
  NM_INLINE float getBalanceOffsetFwdImportance() const { return m_balanceOffsetFwdImportance; }
  NM_INLINE const float& getBalanceOffsetFwdImportanceRef() const { return m_balanceOffsetFwdImportance; }
  NM_INLINE const float& getBalanceOffsetFwd() const { return balanceOffsetFwd; }

  NM_INLINE void setBalanceOffsetRight(const float& _balanceOffsetRight, float balanceOffsetRight_importance = 1.f)
  {
    balanceOffsetRight = _balanceOffsetRight;
    postAssignBalanceOffsetRight(balanceOffsetRight);
    NMP_ASSERT(balanceOffsetRight_importance >= 0.0f && balanceOffsetRight_importance <= 1.0f);
    m_balanceOffsetRightImportance = balanceOffsetRight_importance;
  }
  NM_INLINE float getBalanceOffsetRightImportance() const { return m_balanceOffsetRightImportance; }
  NM_INLINE const float& getBalanceOffsetRightImportanceRef() const { return m_balanceOffsetRightImportance; }
  NM_INLINE const float& getBalanceOffsetRight() const { return balanceOffsetRight; }

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

  NM_INLINE void setStabilisingCrouchDownAmount(const float& _stabilisingCrouchDownAmount, float stabilisingCrouchDownAmount_importance = 1.f)
  {
    stabilisingCrouchDownAmount = _stabilisingCrouchDownAmount;
    postAssignStabilisingCrouchDownAmount(stabilisingCrouchDownAmount);
    NMP_ASSERT(stabilisingCrouchDownAmount_importance >= 0.0f && stabilisingCrouchDownAmount_importance <= 1.0f);
    m_stabilisingCrouchDownAmountImportance = stabilisingCrouchDownAmount_importance;
  }
  NM_INLINE float getStabilisingCrouchDownAmountImportance() const { return m_stabilisingCrouchDownAmountImportance; }
  NM_INLINE const float& getStabilisingCrouchDownAmountImportanceRef() const { return m_stabilisingCrouchDownAmountImportance; }
  NM_INLINE const float& getStabilisingCrouchDownAmount() const { return stabilisingCrouchDownAmount; }

  NM_INLINE void setStabilisingCrouchPitchAmount(const float& _stabilisingCrouchPitchAmount, float stabilisingCrouchPitchAmount_importance = 1.f)
  {
    stabilisingCrouchPitchAmount = _stabilisingCrouchPitchAmount;
    postAssignStabilisingCrouchPitchAmount(stabilisingCrouchPitchAmount);
    NMP_ASSERT(stabilisingCrouchPitchAmount_importance >= 0.0f && stabilisingCrouchPitchAmount_importance <= 1.0f);
    m_stabilisingCrouchPitchAmountImportance = stabilisingCrouchPitchAmount_importance;
  }
  NM_INLINE float getStabilisingCrouchPitchAmountImportance() const { return m_stabilisingCrouchPitchAmountImportance; }
  NM_INLINE const float& getStabilisingCrouchPitchAmountImportanceRef() const { return m_stabilisingCrouchPitchAmountImportance; }
  NM_INLINE const float& getStabilisingCrouchPitchAmount() const { return stabilisingCrouchPitchAmount; }

protected:

  NMP::Matrix34 headPoseEndRelativeToRoot[NetworkConstants::networkMaxNumHeads];  // Note that the transforms will be set, even if the importances are 0, since we require a standing pose  (Transform)
  NMP::Matrix34 spinePoseEndRelativeToRoot[NetworkConstants::networkMaxNumSpines];  ///< (Transform)
  NMP::Matrix34 legPoseEndRelativeToRoot[NetworkConstants::networkMaxNumLegs];  ///< (Transform)
  NMP::Matrix34 armPoseEndRelativeToRoot[NetworkConstants::networkMaxNumArms];  ///< (Transform)
  NMP::Quat targetPelvisOrientation;         ///< (Orientation)
  NMP::Vector3 targetPelvisDirection;  // Desired direction for the pelvis  (Direction)
  float balanceOffsetFwd;  // Desired offset for the balance so it doesn't always balance perfectly over the CoM  (Distance)
  float balanceOffsetRight;                  ///< (Distance)
  float balanceUsingAnimationHorizontalPelvisOffsetAmount;  /// If 1 then the incoming horizontal pelvis offset from support should be kept. If 0 then the CoM will be used
  /// balance.   (ClampedWeight)
  float stabilisingCrouchDownAmount;         ///< (Multiplier)
  float stabilisingCrouchPitchAmount;        ///< (Multiplier)

  // post-assignment stubs
  NM_INLINE void postAssignHeadPoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "headPoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinePoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "spinePoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegPoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "legPoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmPoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "armPoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTargetPelvisOrientation(const NMP::Quat& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(QuatNormalised(v), "targetPelvisOrientation");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTargetPelvisDirection(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "targetPelvisDirection");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceOffsetFwd(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "balanceOffsetFwd");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceOffsetRight(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "balanceOffsetRight");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignBalanceUsingAnimationHorizontalPelvisOffsetAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Float0to1(v), "balanceUsingAnimationHorizontalPelvisOffsetAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStabilisingCrouchDownAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "stabilisingCrouchDownAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStabilisingCrouchPitchAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "stabilisingCrouchPitchAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_headPoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumHeads], 
     m_spinePoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumSpines], 
     m_legPoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumLegs], 
     m_armPoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumArms], 
     m_targetPelvisOrientationImportance, 
     m_targetPelvisDirectionImportance, 
     m_balanceOffsetFwdImportance, 
     m_balanceOffsetRightImportance, 
     m_balanceUsingAnimationHorizontalPelvisOffsetAmountImportance, 
     m_stabilisingCrouchDownAmountImportance, 
     m_stabilisingCrouchPitchAmountImportance;

  friend class StaticBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numHeadPoseEndRelativeToRoot = calculateNumHeadPoseEndRelativeToRoot();
      for (uint32_t i=0; i<numHeadPoseEndRelativeToRoot; i++)
      {
        if (getHeadPoseEndRelativeToRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(headPoseEndRelativeToRoot[i]), "headPoseEndRelativeToRoot");
        }
      }
    }
    {
      uint32_t numSpinePoseEndRelativeToRoot = calculateNumSpinePoseEndRelativeToRoot();
      for (uint32_t i=0; i<numSpinePoseEndRelativeToRoot; i++)
      {
        if (getSpinePoseEndRelativeToRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(spinePoseEndRelativeToRoot[i]), "spinePoseEndRelativeToRoot");
        }
      }
    }
    {
      uint32_t numLegPoseEndRelativeToRoot = calculateNumLegPoseEndRelativeToRoot();
      for (uint32_t i=0; i<numLegPoseEndRelativeToRoot; i++)
      {
        if (getLegPoseEndRelativeToRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(legPoseEndRelativeToRoot[i]), "legPoseEndRelativeToRoot");
        }
      }
    }
    {
      uint32_t numArmPoseEndRelativeToRoot = calculateNumArmPoseEndRelativeToRoot();
      for (uint32_t i=0; i<numArmPoseEndRelativeToRoot; i++)
      {
        if (getArmPoseEndRelativeToRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(armPoseEndRelativeToRoot[i]), "armPoseEndRelativeToRoot");
        }
      }
    }
    if (getTargetPelvisOrientationImportance() > 0.0f)
    {
      NM_VALIDATOR(QuatNormalised(targetPelvisOrientation), "targetPelvisOrientation");
    }
    if (getTargetPelvisDirectionImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(targetPelvisDirection), "targetPelvisDirection");
    }
    if (getBalanceOffsetFwdImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(balanceOffsetFwd), "balanceOffsetFwd");
    }
    if (getBalanceOffsetRightImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(balanceOffsetRight), "balanceOffsetRight");
    }
    if (getBalanceUsingAnimationHorizontalPelvisOffsetAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(Float0to1(balanceUsingAnimationHorizontalPelvisOffsetAmount), "balanceUsingAnimationHorizontalPelvisOffsetAmount");
    }
    if (getStabilisingCrouchDownAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(stabilisingCrouchDownAmount), "stabilisingCrouchDownAmount");
    }
    if (getStabilisingCrouchPitchAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(stabilisingCrouchPitchAmount), "stabilisingCrouchPitchAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class StaticBalanceOutputs
 * \ingroup StaticBalance
 * \brief Outputs Static balance control/management
 *
 * Data packet definition (536 bytes, 544 aligned)
 */
NMP_ALIGN_PREFIX(32) struct StaticBalanceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(StaticBalanceOutputs));
  }

  NM_INLINE void setPelvisRelSupport(const NMP::Matrix34& _pelvisRelSupport, float pelvisRelSupport_importance = 1.f)
  {
    pelvisRelSupport = _pelvisRelSupport;
    postAssignPelvisRelSupport(pelvisRelSupport);
    NMP_ASSERT(pelvisRelSupport_importance >= 0.0f && pelvisRelSupport_importance <= 1.0f);
    m_pelvisRelSupportImportance = pelvisRelSupport_importance;
  }
  NM_INLINE float getPelvisRelSupportImportance() const { return m_pelvisRelSupportImportance; }
  NM_INLINE const float& getPelvisRelSupportImportanceRef() const { return m_pelvisRelSupportImportance; }
  NM_INLINE const NMP::Matrix34& getPelvisRelSupport() const { return pelvisRelSupport; }

  enum { maxLegPoseEndRelSupport = 2 };
  NM_INLINE unsigned int getMaxLegPoseEndRelSupport() const { return maxLegPoseEndRelSupport; }
  NM_INLINE void setLegPoseEndRelSupport(unsigned int number, const NMP::Matrix34* _legPoseEndRelSupport, float legPoseEndRelSupport_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegPoseEndRelSupport);
    NMP_ASSERT(legPoseEndRelSupport_importance >= 0.0f && legPoseEndRelSupport_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legPoseEndRelSupport[i] = _legPoseEndRelSupport[i];
      postAssignLegPoseEndRelSupport(legPoseEndRelSupport[i]);
      m_legPoseEndRelSupportImportance[i] = legPoseEndRelSupport_importance;
    }
  }
  NM_INLINE void setLegPoseEndRelSupportAt(unsigned int index, const NMP::Matrix34& _legPoseEndRelSupport, float legPoseEndRelSupport_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegPoseEndRelSupport);
    legPoseEndRelSupport[index] = _legPoseEndRelSupport;
    NMP_ASSERT(legPoseEndRelSupport_importance >= 0.0f && legPoseEndRelSupport_importance <= 1.0f);
    postAssignLegPoseEndRelSupport(legPoseEndRelSupport[index]);
    m_legPoseEndRelSupportImportance[index] = legPoseEndRelSupport_importance;
  }
  NM_INLINE float getLegPoseEndRelSupportImportance(int index) const { return m_legPoseEndRelSupportImportance[index]; }
  NM_INLINE const float& getLegPoseEndRelSupportImportanceRef(int index) const { return m_legPoseEndRelSupportImportance[index]; }
  NM_INLINE unsigned int calculateNumLegPoseEndRelSupport() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legPoseEndRelSupportImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getLegPoseEndRelSupport(unsigned int index) const { NMP_ASSERT(index <= maxLegPoseEndRelSupport); return legPoseEndRelSupport[index]; }

  NM_INLINE float getAverageLegPoseEndRelSupportImportance() const { return m_averageLegPoseEndRelSupportImportance; }
  NM_INLINE const float& getAverageLegPoseEndRelSupportImportanceRef() const { return m_averageLegPoseEndRelSupportImportance; }
  NM_INLINE const NMP::Matrix34& getAverageLegPoseEndRelSupport() const { return averageLegPoseEndRelSupport; }

  enum { maxArmPoseEndRelSupport = 2 };
  NM_INLINE unsigned int getMaxArmPoseEndRelSupport() const { return maxArmPoseEndRelSupport; }
  NM_INLINE void setArmPoseEndRelSupport(unsigned int number, const NMP::Matrix34* _armPoseEndRelSupport, float armPoseEndRelSupport_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmPoseEndRelSupport);
    NMP_ASSERT(armPoseEndRelSupport_importance >= 0.0f && armPoseEndRelSupport_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armPoseEndRelSupport[i] = _armPoseEndRelSupport[i];
      postAssignArmPoseEndRelSupport(armPoseEndRelSupport[i]);
      m_armPoseEndRelSupportImportance[i] = armPoseEndRelSupport_importance;
    }
  }
  NM_INLINE void setArmPoseEndRelSupportAt(unsigned int index, const NMP::Matrix34& _armPoseEndRelSupport, float armPoseEndRelSupport_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmPoseEndRelSupport);
    armPoseEndRelSupport[index] = _armPoseEndRelSupport;
    NMP_ASSERT(armPoseEndRelSupport_importance >= 0.0f && armPoseEndRelSupport_importance <= 1.0f);
    postAssignArmPoseEndRelSupport(armPoseEndRelSupport[index]);
    m_armPoseEndRelSupportImportance[index] = armPoseEndRelSupport_importance;
  }
  NM_INLINE float getArmPoseEndRelSupportImportance(int index) const { return m_armPoseEndRelSupportImportance[index]; }
  NM_INLINE const float& getArmPoseEndRelSupportImportanceRef(int index) const { return m_armPoseEndRelSupportImportance[index]; }
  NM_INLINE unsigned int calculateNumArmPoseEndRelSupport() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armPoseEndRelSupportImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getArmPoseEndRelSupport(unsigned int index) const { NMP_ASSERT(index <= maxArmPoseEndRelSupport); return armPoseEndRelSupport[index]; }

  NM_INLINE void setAverageArmPoseEndRelSupport(const NMP::Matrix34& _averageArmPoseEndRelSupport, float averageArmPoseEndRelSupport_importance = 1.f)
  {
    averageArmPoseEndRelSupport = _averageArmPoseEndRelSupport;
    postAssignAverageArmPoseEndRelSupport(averageArmPoseEndRelSupport);
    NMP_ASSERT(averageArmPoseEndRelSupport_importance >= 0.0f && averageArmPoseEndRelSupport_importance <= 1.0f);
    m_averageArmPoseEndRelSupportImportance = averageArmPoseEndRelSupport_importance;
  }
  NM_INLINE float getAverageArmPoseEndRelSupportImportance() const { return m_averageArmPoseEndRelSupportImportance; }
  NM_INLINE const float& getAverageArmPoseEndRelSupportImportanceRef() const { return m_averageArmPoseEndRelSupportImportance; }
  NM_INLINE const NMP::Matrix34& getAverageArmPoseEndRelSupport() const { return averageArmPoseEndRelSupport; }

  enum { maxChestRelPelvisOrientation = 1 };
  NM_INLINE unsigned int getMaxChestRelPelvisOrientation() const { return maxChestRelPelvisOrientation; }
  NM_INLINE void setChestRelPelvisOrientation(unsigned int number, const NMP::Quat* _chestRelPelvisOrientation, float chestRelPelvisOrientation_importance = 1.f)
  {
    NMP_ASSERT(number <= maxChestRelPelvisOrientation);
    NMP_ASSERT(chestRelPelvisOrientation_importance >= 0.0f && chestRelPelvisOrientation_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      chestRelPelvisOrientation[i] = _chestRelPelvisOrientation[i];
      postAssignChestRelPelvisOrientation(chestRelPelvisOrientation[i]);
      m_chestRelPelvisOrientationImportance[i] = chestRelPelvisOrientation_importance;
    }
  }
  NM_INLINE void setChestRelPelvisOrientationAt(unsigned int index, const NMP::Quat& _chestRelPelvisOrientation, float chestRelPelvisOrientation_importance = 1.f)
  {
    NMP_ASSERT(index < maxChestRelPelvisOrientation);
    chestRelPelvisOrientation[index] = _chestRelPelvisOrientation;
    NMP_ASSERT(chestRelPelvisOrientation_importance >= 0.0f && chestRelPelvisOrientation_importance <= 1.0f);
    postAssignChestRelPelvisOrientation(chestRelPelvisOrientation[index]);
    m_chestRelPelvisOrientationImportance[index] = chestRelPelvisOrientation_importance;
  }
  NM_INLINE float getChestRelPelvisOrientationImportance(int index) const { return m_chestRelPelvisOrientationImportance[index]; }
  NM_INLINE const float& getChestRelPelvisOrientationImportanceRef(int index) const { return m_chestRelPelvisOrientationImportance[index]; }
  NM_INLINE unsigned int calculateNumChestRelPelvisOrientation() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_chestRelPelvisOrientationImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Quat& getChestRelPelvisOrientation(unsigned int index) const { NMP_ASSERT(index <= maxChestRelPelvisOrientation); return chestRelPelvisOrientation[index]; }

  enum { maxChestRelPelvisTranslation = 1 };
  NM_INLINE unsigned int getMaxChestRelPelvisTranslation() const { return maxChestRelPelvisTranslation; }
  NM_INLINE void setChestRelPelvisTranslation(unsigned int number, const NMP::Vector3* _chestRelPelvisTranslation, float chestRelPelvisTranslation_importance = 1.f)
  {
    NMP_ASSERT(number <= maxChestRelPelvisTranslation);
    NMP_ASSERT(chestRelPelvisTranslation_importance >= 0.0f && chestRelPelvisTranslation_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      chestRelPelvisTranslation[i] = _chestRelPelvisTranslation[i];
      postAssignChestRelPelvisTranslation(chestRelPelvisTranslation[i]);
      m_chestRelPelvisTranslationImportance[i] = chestRelPelvisTranslation_importance;
    }
  }
  NM_INLINE void setChestRelPelvisTranslationAt(unsigned int index, const NMP::Vector3& _chestRelPelvisTranslation, float chestRelPelvisTranslation_importance = 1.f)
  {
    NMP_ASSERT(index < maxChestRelPelvisTranslation);
    chestRelPelvisTranslation[index] = _chestRelPelvisTranslation;
    NMP_ASSERT(chestRelPelvisTranslation_importance >= 0.0f && chestRelPelvisTranslation_importance <= 1.0f);
    postAssignChestRelPelvisTranslation(chestRelPelvisTranslation[index]);
    m_chestRelPelvisTranslationImportance[index] = chestRelPelvisTranslation_importance;
  }
  NM_INLINE float getChestRelPelvisTranslationImportance(int index) const { return m_chestRelPelvisTranslationImportance[index]; }
  NM_INLINE const float& getChestRelPelvisTranslationImportanceRef(int index) const { return m_chestRelPelvisTranslationImportance[index]; }
  NM_INLINE unsigned int calculateNumChestRelPelvisTranslation() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_chestRelPelvisTranslationImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Vector3& getChestRelPelvisTranslation(unsigned int index) const { NMP_ASSERT(index <= maxChestRelPelvisTranslation); return chestRelPelvisTranslation[index]; }

  enum { maxHeadRelChestOrientation = 1 };
  NM_INLINE unsigned int getMaxHeadRelChestOrientation() const { return maxHeadRelChestOrientation; }
  NM_INLINE void setHeadRelChestOrientation(unsigned int number, const NMP::Quat* _headRelChestOrientation, float headRelChestOrientation_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHeadRelChestOrientation);
    NMP_ASSERT(headRelChestOrientation_importance >= 0.0f && headRelChestOrientation_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      headRelChestOrientation[i] = _headRelChestOrientation[i];
      postAssignHeadRelChestOrientation(headRelChestOrientation[i]);
      m_headRelChestOrientationImportance[i] = headRelChestOrientation_importance;
    }
  }
  NM_INLINE void setHeadRelChestOrientationAt(unsigned int index, const NMP::Quat& _headRelChestOrientation, float headRelChestOrientation_importance = 1.f)
  {
    NMP_ASSERT(index < maxHeadRelChestOrientation);
    headRelChestOrientation[index] = _headRelChestOrientation;
    NMP_ASSERT(headRelChestOrientation_importance >= 0.0f && headRelChestOrientation_importance <= 1.0f);
    postAssignHeadRelChestOrientation(headRelChestOrientation[index]);
    m_headRelChestOrientationImportance[index] = headRelChestOrientation_importance;
  }
  NM_INLINE float getHeadRelChestOrientationImportance(int index) const { return m_headRelChestOrientationImportance[index]; }
  NM_INLINE const float& getHeadRelChestOrientationImportanceRef(int index) const { return m_headRelChestOrientationImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadRelChestOrientation() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headRelChestOrientationImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Quat& getHeadRelChestOrientation(unsigned int index) const { NMP_ASSERT(index <= maxHeadRelChestOrientation); return headRelChestOrientation[index]; }

protected:

  NMP::Matrix34 pelvisRelSupport;  /// The desired rotation and offset of the pelvis relative to the centre and direction of support.
  /// The support coordinate frame has the same convention as the spine etc - i.e. y up, x fwd  (Transform)
  NMP::Matrix34 legPoseEndRelSupport[NetworkConstants::networkMaxNumLegs];  /// The desired end transforms relative to the support. Note that these are unaffected by any pose modifications.  (Transform)
  NMP::Matrix34 averageLegPoseEndRelSupport;  ///< (Transform)
  NMP::Matrix34 armPoseEndRelSupport[NetworkConstants::networkMaxNumArms];  ///< (Transform)
  NMP::Matrix34 averageArmPoseEndRelSupport;  ///< (Transform)
  NMP::Quat chestRelPelvisOrientation[NetworkConstants::networkMaxNumSpines];  /// The desired rotation of the spine relative to the pelvis (sent to SpineSupport)  (will only set the first)  (OrientationDelta)
  NMP::Vector3 chestRelPelvisTranslation[NetworkConstants::networkMaxNumSpines];  /// The desired translation of the spine relative to the pelvis (sent to SpineSupport)  (will only set the first)  (PositionDelta)
  NMP::Quat headRelChestOrientation[NetworkConstants::networkMaxNumHeads];  /// The desired rotation of the head relative to the spine  (OrientationDelta)

  // post-assignment stubs
  NM_INLINE void postAssignPelvisRelSupport(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "pelvisRelSupport");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegPoseEndRelSupport(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "legPoseEndRelSupport");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverageLegPoseEndRelSupport(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "averageLegPoseEndRelSupport");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmPoseEndRelSupport(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "armPoseEndRelSupport");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignAverageArmPoseEndRelSupport(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "averageArmPoseEndRelSupport");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignChestRelPelvisOrientation(const NMP::Quat& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(QuatNormalised(v), "chestRelPelvisOrientation");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignChestRelPelvisTranslation(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "chestRelPelvisTranslation");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadRelChestOrientation(const NMP::Quat& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(QuatNormalised(v), "headRelChestOrientation");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_pelvisRelSupportImportance, 
     m_legPoseEndRelSupportImportance[NetworkConstants::networkMaxNumLegs], 
     m_averageLegPoseEndRelSupportImportance, 
     m_armPoseEndRelSupportImportance[NetworkConstants::networkMaxNumArms], 
     m_averageArmPoseEndRelSupportImportance, 
     m_chestRelPelvisOrientationImportance[NetworkConstants::networkMaxNumSpines], 
     m_chestRelPelvisTranslationImportance[NetworkConstants::networkMaxNumSpines], 
     m_headRelChestOrientationImportance[NetworkConstants::networkMaxNumHeads];

  friend class StaticBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getPelvisRelSupportImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(pelvisRelSupport), "pelvisRelSupport");
    }
    {
      uint32_t numLegPoseEndRelSupport = calculateNumLegPoseEndRelSupport();
      for (uint32_t i=0; i<numLegPoseEndRelSupport; i++)
      {
        if (getLegPoseEndRelSupportImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(legPoseEndRelSupport[i]), "legPoseEndRelSupport");
        }
      }
    }
    if (getAverageLegPoseEndRelSupportImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(averageLegPoseEndRelSupport), "averageLegPoseEndRelSupport");
    }
    {
      uint32_t numArmPoseEndRelSupport = calculateNumArmPoseEndRelSupport();
      for (uint32_t i=0; i<numArmPoseEndRelSupport; i++)
      {
        if (getArmPoseEndRelSupportImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(armPoseEndRelSupport[i]), "armPoseEndRelSupport");
        }
      }
    }
    if (getAverageArmPoseEndRelSupportImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(averageArmPoseEndRelSupport), "averageArmPoseEndRelSupport");
    }
    {
      uint32_t numChestRelPelvisOrientation = calculateNumChestRelPelvisOrientation();
      for (uint32_t i=0; i<numChestRelPelvisOrientation; i++)
      {
        if (getChestRelPelvisOrientationImportance(i) > 0.0f)
        {
          NM_VALIDATOR(QuatNormalised(chestRelPelvisOrientation[i]), "chestRelPelvisOrientation");
        }
      }
    }
    {
      uint32_t numChestRelPelvisTranslation = calculateNumChestRelPelvisTranslation();
      for (uint32_t i=0; i<numChestRelPelvisTranslation; i++)
      {
        if (getChestRelPelvisTranslationImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Vector3Valid(chestRelPelvisTranslation[i]), "chestRelPelvisTranslation");
        }
      }
    }
    {
      uint32_t numHeadRelChestOrientation = calculateNumHeadRelChestOrientation();
      for (uint32_t i=0; i<numHeadRelChestOrientation; i++)
      {
        if (getHeadRelChestOrientationImportance(i) > 0.0f)
        {
          NM_VALIDATOR(QuatNormalised(headRelChestOrientation[i]), "headRelChestOrientation");
        }
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class StaticBalanceFeedbackInputs
 * \ingroup StaticBalance
 * \brief FeedbackInputs Static balance control/management
 *
 * Data packet definition (776 bytes, 800 aligned)
 */
NMP_ALIGN_PREFIX(32) struct StaticBalanceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(StaticBalanceFeedbackInputs));
  }

  NM_INLINE void setPelvisDirectionRequest(const DirectionRequest& _pelvisDirectionRequest, float pelvisDirectionRequest_importance = 1.f)
  {
    pelvisDirectionRequest = _pelvisDirectionRequest;
    postAssignPelvisDirectionRequest(pelvisDirectionRequest);
    NMP_ASSERT(pelvisDirectionRequest_importance >= 0.0f && pelvisDirectionRequest_importance <= 1.0f);
    m_pelvisDirectionRequestImportance = pelvisDirectionRequest_importance;
  }
  NM_INLINE DirectionRequest& startPelvisDirectionRequestModification()
  {
    m_pelvisDirectionRequestImportance = -1.0f; // set invalid until stopPelvisDirectionRequestModification()
    return pelvisDirectionRequest;
  }
  NM_INLINE void stopPelvisDirectionRequestModification(float pelvisDirectionRequest_importance = 1.f)
  {
    postAssignPelvisDirectionRequest(pelvisDirectionRequest);
    NMP_ASSERT(pelvisDirectionRequest_importance >= 0.0f && pelvisDirectionRequest_importance <= 1.0f);
    m_pelvisDirectionRequestImportance = pelvisDirectionRequest_importance;
  }
  NM_INLINE float getPelvisDirectionRequestImportance() const { return m_pelvisDirectionRequestImportance; }
  NM_INLINE const float& getPelvisDirectionRequestImportanceRef() const { return m_pelvisDirectionRequestImportance; }
  NM_INLINE const DirectionRequest& getPelvisDirectionRequest() const { return pelvisDirectionRequest; }

  NM_INLINE void setChestDirectionRequest(const DirectionRequest& _chestDirectionRequest, float chestDirectionRequest_importance = 1.f)
  {
    chestDirectionRequest = _chestDirectionRequest;
    postAssignChestDirectionRequest(chestDirectionRequest);
    NMP_ASSERT(chestDirectionRequest_importance >= 0.0f && chestDirectionRequest_importance <= 1.0f);
    m_chestDirectionRequestImportance = chestDirectionRequest_importance;
  }
  NM_INLINE DirectionRequest& startChestDirectionRequestModification()
  {
    m_chestDirectionRequestImportance = -1.0f; // set invalid until stopChestDirectionRequestModification()
    return chestDirectionRequest;
  }
  NM_INLINE void stopChestDirectionRequestModification(float chestDirectionRequest_importance = 1.f)
  {
    postAssignChestDirectionRequest(chestDirectionRequest);
    NMP_ASSERT(chestDirectionRequest_importance >= 0.0f && chestDirectionRequest_importance <= 1.0f);
    m_chestDirectionRequestImportance = chestDirectionRequest_importance;
  }
  NM_INLINE float getChestDirectionRequestImportance() const { return m_chestDirectionRequestImportance; }
  NM_INLINE const float& getChestDirectionRequestImportanceRef() const { return m_chestDirectionRequestImportance; }
  NM_INLINE const DirectionRequest& getChestDirectionRequest() const { return chestDirectionRequest; }

  NM_INLINE void setPelvisRotationRequest(const RotationRequest& _pelvisRotationRequest, float pelvisRotationRequest_importance = 1.f)
  {
    pelvisRotationRequest = _pelvisRotationRequest;
    postAssignPelvisRotationRequest(pelvisRotationRequest);
    NMP_ASSERT(pelvisRotationRequest_importance >= 0.0f && pelvisRotationRequest_importance <= 1.0f);
    m_pelvisRotationRequestImportance = pelvisRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startPelvisRotationRequestModification()
  {
    m_pelvisRotationRequestImportance = -1.0f; // set invalid until stopPelvisRotationRequestModification()
    return pelvisRotationRequest;
  }
  NM_INLINE void stopPelvisRotationRequestModification(float pelvisRotationRequest_importance = 1.f)
  {
    postAssignPelvisRotationRequest(pelvisRotationRequest);
    NMP_ASSERT(pelvisRotationRequest_importance >= 0.0f && pelvisRotationRequest_importance <= 1.0f);
    m_pelvisRotationRequestImportance = pelvisRotationRequest_importance;
  }
  NM_INLINE float getPelvisRotationRequestImportance() const { return m_pelvisRotationRequestImportance; }
  NM_INLINE const float& getPelvisRotationRequestImportanceRef() const { return m_pelvisRotationRequestImportance; }
  NM_INLINE const RotationRequest& getPelvisRotationRequest() const { return pelvisRotationRequest; }

  NM_INLINE void setPelvisTranslationRequest(const TranslationRequest& _pelvisTranslationRequest, float pelvisTranslationRequest_importance = 1.f)
  {
    pelvisTranslationRequest = _pelvisTranslationRequest;
    postAssignPelvisTranslationRequest(pelvisTranslationRequest);
    NMP_ASSERT(pelvisTranslationRequest_importance >= 0.0f && pelvisTranslationRequest_importance <= 1.0f);
    m_pelvisTranslationRequestImportance = pelvisTranslationRequest_importance;
  }
  NM_INLINE TranslationRequest& startPelvisTranslationRequestModification()
  {
    m_pelvisTranslationRequestImportance = -1.0f; // set invalid until stopPelvisTranslationRequestModification()
    return pelvisTranslationRequest;
  }
  NM_INLINE void stopPelvisTranslationRequestModification(float pelvisTranslationRequest_importance = 1.f)
  {
    postAssignPelvisTranslationRequest(pelvisTranslationRequest);
    NMP_ASSERT(pelvisTranslationRequest_importance >= 0.0f && pelvisTranslationRequest_importance <= 1.0f);
    m_pelvisTranslationRequestImportance = pelvisTranslationRequest_importance;
  }
  NM_INLINE float getPelvisTranslationRequestImportance() const { return m_pelvisTranslationRequestImportance; }
  NM_INLINE const float& getPelvisTranslationRequestImportanceRef() const { return m_pelvisTranslationRequestImportance; }
  NM_INLINE const TranslationRequest& getPelvisTranslationRequest() const { return pelvisTranslationRequest; }

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

  enum { maxCurrHeadEndPositions = 1 };
  NM_INLINE unsigned int getMaxCurrHeadEndPositions() const { return maxCurrHeadEndPositions; }
  NM_INLINE void setCurrHeadEndPositions(unsigned int number, const NMP::Vector3* _currHeadEndPositions, float currHeadEndPositions_importance = 1.f)
  {
    NMP_ASSERT(number <= maxCurrHeadEndPositions);
    NMP_ASSERT(currHeadEndPositions_importance >= 0.0f && currHeadEndPositions_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      currHeadEndPositions[i] = _currHeadEndPositions[i];
      postAssignCurrHeadEndPositions(currHeadEndPositions[i]);
      m_currHeadEndPositionsImportance[i] = currHeadEndPositions_importance;
    }
  }
  NM_INLINE void setCurrHeadEndPositionsAt(unsigned int index, const NMP::Vector3& _currHeadEndPositions, float currHeadEndPositions_importance = 1.f)
  {
    NMP_ASSERT(index < maxCurrHeadEndPositions);
    currHeadEndPositions[index] = _currHeadEndPositions;
    NMP_ASSERT(currHeadEndPositions_importance >= 0.0f && currHeadEndPositions_importance <= 1.0f);
    postAssignCurrHeadEndPositions(currHeadEndPositions[index]);
    m_currHeadEndPositionsImportance[index] = currHeadEndPositions_importance;
  }
  NM_INLINE float getCurrHeadEndPositionsImportance(int index) const { return m_currHeadEndPositionsImportance[index]; }
  NM_INLINE const float& getCurrHeadEndPositionsImportanceRef(int index) const { return m_currHeadEndPositionsImportance[index]; }
  NM_INLINE unsigned int calculateNumCurrHeadEndPositions() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_currHeadEndPositionsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Vector3& getCurrHeadEndPositions(unsigned int index) const { NMP_ASSERT(index <= maxCurrHeadEndPositions); return currHeadEndPositions[index]; }

protected:

  DirectionRequest pelvisDirectionRequest;
  DirectionRequest chestDirectionRequest;
  RotationRequest pelvisRotationRequest;
  TranslationRequest pelvisTranslationRequest;
  ER::LimbTransform supportTM;
  NMP::Vector3 currHeadEndPositions[NetworkConstants::networkMaxNumHeads];  ///< (Position)

  // post-assignment stubs
  NM_INLINE void postAssignPelvisDirectionRequest(const DirectionRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignChestDirectionRequest(const DirectionRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPelvisRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPelvisTranslationRequest(const TranslationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignCurrHeadEndPositions(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "currHeadEndPositions");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_pelvisDirectionRequestImportance, 
     m_chestDirectionRequestImportance, 
     m_pelvisRotationRequestImportance, 
     m_pelvisTranslationRequestImportance, 
     m_supportTMImportance, 
     m_currHeadEndPositionsImportance[NetworkConstants::networkMaxNumHeads];

  friend class StaticBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getPelvisDirectionRequestImportance() > 0.0f)
    {
      pelvisDirectionRequest.validate();
    }
    if (getChestDirectionRequestImportance() > 0.0f)
    {
      chestDirectionRequest.validate();
    }
    if (getPelvisRotationRequestImportance() > 0.0f)
    {
      pelvisRotationRequest.validate();
    }
    if (getPelvisTranslationRequestImportance() > 0.0f)
    {
      pelvisTranslationRequest.validate();
    }
    {
      uint32_t numCurrHeadEndPositions = calculateNumCurrHeadEndPositions();
      for (uint32_t i=0; i<numCurrHeadEndPositions; i++)
      {
        if (getCurrHeadEndPositionsImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Vector3Valid(currHeadEndPositions[i]), "currHeadEndPositions");
        }
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_STATICBALANCE_DATA_H

