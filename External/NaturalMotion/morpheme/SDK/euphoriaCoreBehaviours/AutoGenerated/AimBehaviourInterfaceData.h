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

#ifndef NM_MDF_AIMBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_AIMBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/AimBehaviourInterface.module"

// external types
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
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
 * \class AimBehaviourInterfaceData
 * \ingroup AimBehaviourInterface
 * \brief Data Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (659 bytes, 672 aligned)
 */
NMP_ALIGN_PREFIX(32) struct AimBehaviourInterfaceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(AimBehaviourInterfaceData));
  }

  NMP::Matrix34 supportingRelAimingEndTM;  ///< Supporting hand's TM relative to aiming hand.  (Transform)
  NMP::Matrix34 poseSupportRelPelvisTM;  ///< support TM relative to the pelvis in the input pose.  (Transform)
  NMP::Matrix34 rigSupportRelPelvisTM;  ///< support TM relative to the pelvis n the current rig configuration.  (Transform)
  NMP::Matrix34 cachedPoseEndRelativeToRoots[NetworkConstants::networkMaxNumLimbs];  // Following are only used if the behaviour param UseSingleFrameForPose is set to true  (Transform)
  NMP::Vector3 targetPosition;  ///< Current target position in world space.  (Position)
  NMP::Vector3 windingDirection;  ///< Which way has the character twisted about its spine, determined by the aiming limb.  (Direction)
  float targetPitch;  ///< Angle between the support TM's forward direction and the target position about the pelvis' right direction (so +ve is up, -ve is down).  (Angle)
  float targetYaw;  ///< Angle between the support TM's forward direction and the target position about the pelvis' up directions (so +ve is left, -ve is right).  (Angle)
  float handSeparation;  ///< Distance between the aiming and supporting hands.  (Distance)
  float supportingLimbStrengthScale;  ///< Multiplier applied to the strength used in the supporting arm's limb control.  (Weight)
  float lastTargetPelvisRotationAngle;  ///< Used to prevent oscillations in target pelvis orientation.  (Angle)
  float cachedPoseWeights[NetworkConstants::networkMaxNumLimbs];  ///< (Weight)
  int32_t constrainedLimbIndex;  ///< Index of a non-aiming limb that has been constrained to the aiming one, -1 if no limbs constrained.
  bool isLyingOnGround;
  bool areFeetOnGround;  ///< Internal state determined by testing feetOnGroundAmount feedIn
  bool isUnwindingSupportingArm;  ///< Internal state used to drive hysterisis on supporting arm unwinding.

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(supportingRelAimingEndTM), "supportingRelAimingEndTM");
    NM_VALIDATOR(Matrix34Orthonormal(poseSupportRelPelvisTM), "poseSupportRelPelvisTM");
    NM_VALIDATOR(Matrix34Orthonormal(rigSupportRelPelvisTM), "rigSupportRelPelvisTM");
    NM_VALIDATOR(Matrix34Orthonormal(cachedPoseEndRelativeToRoots[0]), "cachedPoseEndRelativeToRoots[0]");
    NM_VALIDATOR(Matrix34Orthonormal(cachedPoseEndRelativeToRoots[1]), "cachedPoseEndRelativeToRoots[1]");
    NM_VALIDATOR(Matrix34Orthonormal(cachedPoseEndRelativeToRoots[2]), "cachedPoseEndRelativeToRoots[2]");
    NM_VALIDATOR(Matrix34Orthonormal(cachedPoseEndRelativeToRoots[3]), "cachedPoseEndRelativeToRoots[3]");
    NM_VALIDATOR(Matrix34Orthonormal(cachedPoseEndRelativeToRoots[4]), "cachedPoseEndRelativeToRoots[4]");
    NM_VALIDATOR(Matrix34Orthonormal(cachedPoseEndRelativeToRoots[5]), "cachedPoseEndRelativeToRoots[5]");
    NM_VALIDATOR(Vector3Valid(targetPosition), "targetPosition");
    NM_VALIDATOR(Vector3Normalised(windingDirection), "windingDirection");
    NM_VALIDATOR(FloatValid(targetPitch), "targetPitch");
    NM_VALIDATOR(FloatValid(targetYaw), "targetYaw");
    NM_VALIDATOR(FloatValid(handSeparation), "handSeparation");
    NM_VALIDATOR(FloatNonNegative(supportingLimbStrengthScale), "supportingLimbStrengthScale");
    NM_VALIDATOR(FloatValid(lastTargetPelvisRotationAngle), "lastTargetPelvisRotationAngle");
    NM_VALIDATOR(FloatNonNegative(cachedPoseWeights[0]), "cachedPoseWeights[0]");
    NM_VALIDATOR(FloatNonNegative(cachedPoseWeights[1]), "cachedPoseWeights[1]");
    NM_VALIDATOR(FloatNonNegative(cachedPoseWeights[2]), "cachedPoseWeights[2]");
    NM_VALIDATOR(FloatNonNegative(cachedPoseWeights[3]), "cachedPoseWeights[3]");
    NM_VALIDATOR(FloatNonNegative(cachedPoseWeights[4]), "cachedPoseWeights[4]");
    NM_VALIDATOR(FloatNonNegative(cachedPoseWeights[5]), "cachedPoseWeights[5]");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class AimBehaviourInterfaceOutputs
 * \ingroup AimBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (724 bytes, 736 aligned)
 */
NMP_ALIGN_PREFIX(32) struct AimBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(AimBehaviourInterfaceOutputs));
  }

  enum { maxArmAimInfo = 2 };
  NM_INLINE unsigned int getMaxArmAimInfo() const { return maxArmAimInfo; }
  NM_INLINE void setArmAimInfo(unsigned int number, const AimInfo* _armAimInfo, float armAimInfo_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmAimInfo);
    NMP_ASSERT(armAimInfo_importance >= 0.0f && armAimInfo_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armAimInfo[i] = _armAimInfo[i];
      postAssignArmAimInfo(armAimInfo[i]);
      m_armAimInfoImportance[i] = armAimInfo_importance;
    }
  }
  NM_INLINE void setArmAimInfoAt(unsigned int index, const AimInfo& _armAimInfo, float armAimInfo_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmAimInfo);
    armAimInfo[index] = _armAimInfo;
    NMP_ASSERT(armAimInfo_importance >= 0.0f && armAimInfo_importance <= 1.0f);
    postAssignArmAimInfo(armAimInfo[index]);
    m_armAimInfoImportance[index] = armAimInfo_importance;
  }
  NM_INLINE AimInfo& startArmAimInfoModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxArmAimInfo);
    m_armAimInfoImportance[index] = -1.0f; // set invalid until stopArmAimInfoModificationAt()
    return armAimInfo[index];
  }
  NM_INLINE void stopArmAimInfoModificationAt(unsigned int index, float armAimInfo_importance = 1.f)
  {
    postAssignArmAimInfo(armAimInfo[index]);
    NMP_ASSERT(armAimInfo_importance >= 0.0f && armAimInfo_importance <= 1.0f);
    m_armAimInfoImportance[index] = armAimInfo_importance;
  }
  NM_INLINE float getArmAimInfoImportance(int index) const { return m_armAimInfoImportance[index]; }
  NM_INLINE const float& getArmAimInfoImportanceRef(int index) const { return m_armAimInfoImportance[index]; }
  NM_INLINE unsigned int calculateNumArmAimInfo() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armAimInfoImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const AimInfo& getArmAimInfo(unsigned int index) const { NMP_ASSERT(index <= maxArmAimInfo); return armAimInfo[index]; }

  enum { maxHeadAimInfo = 1 };
  NM_INLINE unsigned int getMaxHeadAimInfo() const { return maxHeadAimInfo; }
  NM_INLINE void setHeadAimInfo(unsigned int number, const AimInfo* _headAimInfo, float headAimInfo_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHeadAimInfo);
    NMP_ASSERT(headAimInfo_importance >= 0.0f && headAimInfo_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      headAimInfo[i] = _headAimInfo[i];
      postAssignHeadAimInfo(headAimInfo[i]);
      m_headAimInfoImportance[i] = headAimInfo_importance;
    }
  }
  NM_INLINE void setHeadAimInfoAt(unsigned int index, const AimInfo& _headAimInfo, float headAimInfo_importance = 1.f)
  {
    NMP_ASSERT(index < maxHeadAimInfo);
    headAimInfo[index] = _headAimInfo;
    NMP_ASSERT(headAimInfo_importance >= 0.0f && headAimInfo_importance <= 1.0f);
    postAssignHeadAimInfo(headAimInfo[index]);
    m_headAimInfoImportance[index] = headAimInfo_importance;
  }
  NM_INLINE AimInfo& startHeadAimInfoModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxHeadAimInfo);
    m_headAimInfoImportance[index] = -1.0f; // set invalid until stopHeadAimInfoModificationAt()
    return headAimInfo[index];
  }
  NM_INLINE void stopHeadAimInfoModificationAt(unsigned int index, float headAimInfo_importance = 1.f)
  {
    postAssignHeadAimInfo(headAimInfo[index]);
    NMP_ASSERT(headAimInfo_importance >= 0.0f && headAimInfo_importance <= 1.0f);
    m_headAimInfoImportance[index] = headAimInfo_importance;
  }
  NM_INLINE float getHeadAimInfoImportance(int index) const { return m_headAimInfoImportance[index]; }
  NM_INLINE const float& getHeadAimInfoImportanceRef(int index) const { return m_headAimInfoImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadAimInfo() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headAimInfoImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const AimInfo& getHeadAimInfo(unsigned int index) const { NMP_ASSERT(index <= maxHeadAimInfo); return headAimInfo[index]; }

  enum { maxSpineAimInfo = 1 };
  NM_INLINE unsigned int getMaxSpineAimInfo() const { return maxSpineAimInfo; }
  NM_INLINE void setSpineAimInfo(unsigned int number, const AimInfo* _spineAimInfo, float spineAimInfo_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpineAimInfo);
    NMP_ASSERT(spineAimInfo_importance >= 0.0f && spineAimInfo_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spineAimInfo[i] = _spineAimInfo[i];
      postAssignSpineAimInfo(spineAimInfo[i]);
      m_spineAimInfoImportance[i] = spineAimInfo_importance;
    }
  }
  NM_INLINE void setSpineAimInfoAt(unsigned int index, const AimInfo& _spineAimInfo, float spineAimInfo_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpineAimInfo);
    spineAimInfo[index] = _spineAimInfo;
    NMP_ASSERT(spineAimInfo_importance >= 0.0f && spineAimInfo_importance <= 1.0f);
    postAssignSpineAimInfo(spineAimInfo[index]);
    m_spineAimInfoImportance[index] = spineAimInfo_importance;
  }
  NM_INLINE AimInfo& startSpineAimInfoModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxSpineAimInfo);
    m_spineAimInfoImportance[index] = -1.0f; // set invalid until stopSpineAimInfoModificationAt()
    return spineAimInfo[index];
  }
  NM_INLINE void stopSpineAimInfoModificationAt(unsigned int index, float spineAimInfo_importance = 1.f)
  {
    postAssignSpineAimInfo(spineAimInfo[index]);
    NMP_ASSERT(spineAimInfo_importance >= 0.0f && spineAimInfo_importance <= 1.0f);
    m_spineAimInfoImportance[index] = spineAimInfo_importance;
  }
  NM_INLINE float getSpineAimInfoImportance(int index) const { return m_spineAimInfoImportance[index]; }
  NM_INLINE const float& getSpineAimInfoImportanceRef(int index) const { return m_spineAimInfoImportance[index]; }
  NM_INLINE unsigned int calculateNumSpineAimInfo() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spineAimInfoImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const AimInfo& getSpineAimInfo(unsigned int index) const { NMP_ASSERT(index <= maxSpineAimInfo); return spineAimInfo[index]; }

  NM_INLINE void setTargetPelvisTM(const ER::LimbTransform& _targetPelvisTM, float targetPelvisTM_importance = 1.f)
  {
    targetPelvisTM = _targetPelvisTM;
    NMP_ASSERT(targetPelvisTM_importance >= 0.0f && targetPelvisTM_importance <= 1.0f);
    m_targetPelvisTMImportance = targetPelvisTM_importance;
  }
  NM_INLINE ER::LimbTransform& startTargetPelvisTMModification()
  {
    m_targetPelvisTMImportance = -1.0f; // set invalid until stopTargetPelvisTMModification()
    return targetPelvisTM;
  }
  NM_INLINE void stopTargetPelvisTMModification(float targetPelvisTM_importance = 1.f)
  {
    NMP_ASSERT(targetPelvisTM_importance >= 0.0f && targetPelvisTM_importance <= 1.0f);
    m_targetPelvisTMImportance = targetPelvisTM_importance;
  }
  NM_INLINE float getTargetPelvisTMImportance() const { return m_targetPelvisTMImportance; }
  NM_INLINE const float& getTargetPelvisTMImportanceRef() const { return m_targetPelvisTMImportance; }
  NM_INLINE const ER::LimbTransform& getTargetPelvisTM() const { return targetPelvisTM; }

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

  NM_INLINE void setTargetPositionForPose(const NMP::Vector3& _targetPositionForPose, float targetPositionForPose_importance = 1.f)
  {
    targetPositionForPose = _targetPositionForPose;
    postAssignTargetPositionForPose(targetPositionForPose);
    NMP_ASSERT(targetPositionForPose_importance >= 0.0f && targetPositionForPose_importance <= 1.0f);
    m_targetPositionForPoseImportance = targetPositionForPose_importance;
  }
  NM_INLINE float getTargetPositionForPoseImportance() const { return m_targetPositionForPoseImportance; }
  NM_INLINE const float& getTargetPositionForPoseImportanceRef() const { return m_targetPositionForPoseImportance; }
  NM_INLINE const NMP::Vector3& getTargetPositionForPose() const { return targetPositionForPose; }

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

  NM_INLINE void setPoseAimDirRelChest(const NMP::Vector3& _poseAimDirRelChest, float poseAimDirRelChest_importance = 1.f)
  {
    poseAimDirRelChest = _poseAimDirRelChest;
    postAssignPoseAimDirRelChest(poseAimDirRelChest);
    NMP_ASSERT(poseAimDirRelChest_importance >= 0.0f && poseAimDirRelChest_importance <= 1.0f);
    m_poseAimDirRelChestImportance = poseAimDirRelChest_importance;
  }
  NM_INLINE float getPoseAimDirRelChestImportance() const { return m_poseAimDirRelChestImportance; }
  NM_INLINE const float& getPoseAimDirRelChestImportanceRef() const { return m_poseAimDirRelChestImportance; }
  NM_INLINE const NMP::Vector3& getPoseAimDirRelChest() const { return poseAimDirRelChest; }

  NM_INLINE void setPoseAimDirRelPelvis(const NMP::Vector3& _poseAimDirRelPelvis, float poseAimDirRelPelvis_importance = 1.f)
  {
    poseAimDirRelPelvis = _poseAimDirRelPelvis;
    postAssignPoseAimDirRelPelvis(poseAimDirRelPelvis);
    NMP_ASSERT(poseAimDirRelPelvis_importance >= 0.0f && poseAimDirRelPelvis_importance <= 1.0f);
    m_poseAimDirRelPelvisImportance = poseAimDirRelPelvis_importance;
  }
  NM_INLINE float getPoseAimDirRelPelvisImportance() const { return m_poseAimDirRelPelvisImportance; }
  NM_INLINE const float& getPoseAimDirRelPelvisImportanceRef() const { return m_poseAimDirRelPelvisImportance; }
  NM_INLINE const NMP::Vector3& getPoseAimDirRelPelvis() const { return poseAimDirRelPelvis; }

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

  NM_INLINE void setAimingLimbIndex(const int32_t& _aimingLimbIndex, float aimingLimbIndex_importance = 1.f)
  {
    aimingLimbIndex = _aimingLimbIndex;
    NMP_ASSERT(aimingLimbIndex_importance >= 0.0f && aimingLimbIndex_importance <= 1.0f);
    m_aimingLimbIndexImportance = aimingLimbIndex_importance;
  }
  NM_INLINE float getAimingLimbIndexImportance() const { return m_aimingLimbIndexImportance; }
  NM_INLINE const float& getAimingLimbIndexImportanceRef() const { return m_aimingLimbIndexImportance; }
  NM_INLINE const int32_t& getAimingLimbIndex() const { return aimingLimbIndex; }

protected:

  AimInfo armAimInfo[NetworkConstants::networkMaxNumArms];  // Misc aiming data that must be passed to each limbs Aim module
  AimInfo headAimInfo[NetworkConstants::networkMaxNumHeads];
  AimInfo spineAimInfo[NetworkConstants::networkMaxNumSpines];
  ER::LimbTransform targetPelvisTM;
  NMP::Quat targetPelvisOrientation;         ///< (Orientation)
  NMP::Vector3 targetPositionForPose;  ///< Effectively the position the character is aiming at in the input pose (in character space)  (Position)
  NMP::Vector3 targetPositionInWorldSpace;  ///< The position the character should aim at now (in world space).  (Position)
  NMP::Vector3 poseAimDirRelChest;  ///< Aiming direction relative to the chest in the input pose.  (Direction)
  NMP::Vector3 poseAimDirRelPelvis;  ///< Aiming direction relative to the pelvis in the input pose.  (Direction)
  NMP::Vector3 windingDirection;  ///< Axis of rotation for the aiming arm about the chest. Used to ensure that all other limbs twist in the same direction.  (Direction)
  int32_t aimingLimbIndex;

  // post-assignment stubs
  NM_INLINE void postAssignArmAimInfo(const AimInfo& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadAimInfo(const AimInfo& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpineAimInfo(const AimInfo& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTargetPelvisOrientation(const NMP::Quat& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(QuatNormalised(v), "targetPelvisOrientation");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTargetPositionForPose(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "targetPositionForPose");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTargetPositionInWorldSpace(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "targetPositionInWorldSpace");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPoseAimDirRelChest(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "poseAimDirRelChest");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPoseAimDirRelPelvis(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "poseAimDirRelPelvis");
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
     m_armAimInfoImportance[NetworkConstants::networkMaxNumArms], 
     m_headAimInfoImportance[NetworkConstants::networkMaxNumHeads], 
     m_spineAimInfoImportance[NetworkConstants::networkMaxNumSpines], 
     m_targetPelvisTMImportance, 
     m_targetPelvisOrientationImportance, 
     m_targetPositionForPoseImportance, 
     m_targetPositionInWorldSpaceImportance, 
     m_poseAimDirRelChestImportance, 
     m_poseAimDirRelPelvisImportance, 
     m_windingDirectionImportance, 
     m_aimingLimbIndexImportance;

  friend class AimBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numArmAimInfo = calculateNumArmAimInfo();
      for (uint32_t i=0; i<numArmAimInfo; i++)
      {
        if (getArmAimInfoImportance(i) > 0.0f)
          armAimInfo[i].validate();
      }
    }
    {
      uint32_t numHeadAimInfo = calculateNumHeadAimInfo();
      for (uint32_t i=0; i<numHeadAimInfo; i++)
      {
        if (getHeadAimInfoImportance(i) > 0.0f)
          headAimInfo[i].validate();
      }
    }
    {
      uint32_t numSpineAimInfo = calculateNumSpineAimInfo();
      for (uint32_t i=0; i<numSpineAimInfo; i++)
      {
        if (getSpineAimInfoImportance(i) > 0.0f)
          spineAimInfo[i].validate();
      }
    }
    if (getTargetPelvisOrientationImportance() > 0.0f)
    {
      NM_VALIDATOR(QuatNormalised(targetPelvisOrientation), "targetPelvisOrientation");
    }
    if (getTargetPositionForPoseImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(targetPositionForPose), "targetPositionForPose");
    }
    if (getTargetPositionInWorldSpaceImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(targetPositionInWorldSpace), "targetPositionInWorldSpace");
    }
    if (getPoseAimDirRelChestImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(poseAimDirRelChest), "poseAimDirRelChest");
    }
    if (getPoseAimDirRelPelvisImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(poseAimDirRelPelvis), "poseAimDirRelPelvis");
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
 * \class AimBehaviourInterfaceFeedbackInputs
 * \ingroup AimBehaviourInterface
 * \brief FeedbackInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (1565 bytes, 1568 aligned)
 */
NMP_ALIGN_PREFIX(32) struct AimBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(AimBehaviourInterfaceFeedbackInputs));
  }

  enum { maxArmFinalLimbControl = 2 };
  NM_INLINE unsigned int getMaxArmFinalLimbControl() const { return maxArmFinalLimbControl; }
  NM_INLINE void setArmFinalLimbControl(unsigned int number, const LimbControl* _armFinalLimbControl, float armFinalLimbControl_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmFinalLimbControl);
    NMP_ASSERT(armFinalLimbControl_importance >= 0.0f && armFinalLimbControl_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armFinalLimbControl[i] = _armFinalLimbControl[i];
      postAssignArmFinalLimbControl(armFinalLimbControl[i]);
      m_armFinalLimbControlImportance[i] = armFinalLimbControl_importance;
    }
  }
  NM_INLINE void setArmFinalLimbControlAt(unsigned int index, const LimbControl& _armFinalLimbControl, float armFinalLimbControl_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmFinalLimbControl);
    armFinalLimbControl[index] = _armFinalLimbControl;
    NMP_ASSERT(armFinalLimbControl_importance >= 0.0f && armFinalLimbControl_importance <= 1.0f);
    postAssignArmFinalLimbControl(armFinalLimbControl[index]);
    m_armFinalLimbControlImportance[index] = armFinalLimbControl_importance;
  }
  NM_INLINE LimbControl& startArmFinalLimbControlModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxArmFinalLimbControl);
    m_armFinalLimbControlImportance[index] = -1.0f; // set invalid until stopArmFinalLimbControlModificationAt()
    return armFinalLimbControl[index];
  }
  NM_INLINE void stopArmFinalLimbControlModificationAt(unsigned int index, float armFinalLimbControl_importance = 1.f)
  {
    postAssignArmFinalLimbControl(armFinalLimbControl[index]);
    NMP_ASSERT(armFinalLimbControl_importance >= 0.0f && armFinalLimbControl_importance <= 1.0f);
    m_armFinalLimbControlImportance[index] = armFinalLimbControl_importance;
  }
  NM_INLINE float getArmFinalLimbControlImportance(int index) const { return m_armFinalLimbControlImportance[index]; }
  NM_INLINE const float& getArmFinalLimbControlImportanceRef(int index) const { return m_armFinalLimbControlImportance[index]; }
  NM_INLINE unsigned int calculateNumArmFinalLimbControl() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armFinalLimbControlImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const LimbControl& getArmFinalLimbControl(unsigned int index) const { NMP_ASSERT(index <= maxArmFinalLimbControl); return armFinalLimbControl[index]; }

  enum { maxArmAimLimbControl = 2 };
  NM_INLINE unsigned int getMaxArmAimLimbControl() const { return maxArmAimLimbControl; }
  NM_INLINE void setArmAimLimbControl(unsigned int number, const LimbControl* _armAimLimbControl, float armAimLimbControl_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmAimLimbControl);
    NMP_ASSERT(armAimLimbControl_importance >= 0.0f && armAimLimbControl_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armAimLimbControl[i] = _armAimLimbControl[i];
      postAssignArmAimLimbControl(armAimLimbControl[i]);
      m_armAimLimbControlImportance[i] = armAimLimbControl_importance;
    }
  }
  NM_INLINE void setArmAimLimbControlAt(unsigned int index, const LimbControl& _armAimLimbControl, float armAimLimbControl_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmAimLimbControl);
    armAimLimbControl[index] = _armAimLimbControl;
    NMP_ASSERT(armAimLimbControl_importance >= 0.0f && armAimLimbControl_importance <= 1.0f);
    postAssignArmAimLimbControl(armAimLimbControl[index]);
    m_armAimLimbControlImportance[index] = armAimLimbControl_importance;
  }
  NM_INLINE LimbControl& startArmAimLimbControlModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxArmAimLimbControl);
    m_armAimLimbControlImportance[index] = -1.0f; // set invalid until stopArmAimLimbControlModificationAt()
    return armAimLimbControl[index];
  }
  NM_INLINE void stopArmAimLimbControlModificationAt(unsigned int index, float armAimLimbControl_importance = 1.f)
  {
    postAssignArmAimLimbControl(armAimLimbControl[index]);
    NMP_ASSERT(armAimLimbControl_importance >= 0.0f && armAimLimbControl_importance <= 1.0f);
    m_armAimLimbControlImportance[index] = armAimLimbControl_importance;
  }
  NM_INLINE float getArmAimLimbControlImportance(int index) const { return m_armAimLimbControlImportance[index]; }
  NM_INLINE const float& getArmAimLimbControlImportanceRef(int index) const { return m_armAimLimbControlImportance[index]; }
  NM_INLINE unsigned int calculateNumArmAimLimbControl() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armAimLimbControlImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const LimbControl& getArmAimLimbControl(unsigned int index) const { NMP_ASSERT(index <= maxArmAimLimbControl); return armAimLimbControl[index]; }

  NM_INLINE void setFeetOnGroundAmount(const float& _feetOnGroundAmount, float feetOnGroundAmount_importance = 1.f)
  {
    feetOnGroundAmount = _feetOnGroundAmount;
    postAssignFeetOnGroundAmount(feetOnGroundAmount);
    NMP_ASSERT(feetOnGroundAmount_importance >= 0.0f && feetOnGroundAmount_importance <= 1.0f);
    m_feetOnGroundAmountImportance = feetOnGroundAmount_importance;
  }
  NM_INLINE float getFeetOnGroundAmountImportance() const { return m_feetOnGroundAmountImportance; }
  NM_INLINE const float& getFeetOnGroundAmountImportanceRef() const { return m_feetOnGroundAmountImportance; }
  NM_INLINE const float& getFeetOnGroundAmount() const { return feetOnGroundAmount; }

  NM_INLINE void setSpineInContact(const bool& _spineInContact, float spineInContact_importance = 1.f)
  {
    spineInContact = _spineInContact;
    NMP_ASSERT(spineInContact_importance >= 0.0f && spineInContact_importance <= 1.0f);
    m_spineInContactImportance = spineInContact_importance;
  }
  NM_INLINE float getSpineInContactImportance() const { return m_spineInContactImportance; }
  NM_INLINE const float& getSpineInContactImportanceRef() const { return m_spineInContactImportance; }
  NM_INLINE const bool& getSpineInContact() const { return spineInContact; }

protected:

  LimbControl armFinalLimbControl[NetworkConstants::networkMaxNumArms];
  LimbControl armAimLimbControl[NetworkConstants::networkMaxNumArms];
  float feetOnGroundAmount;                  ///< (Weight)
  bool spineInContact;

  // post-assignment stubs
  NM_INLINE void postAssignArmFinalLimbControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmAimLimbControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignFeetOnGroundAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "feetOnGroundAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_armFinalLimbControlImportance[NetworkConstants::networkMaxNumArms], 
     m_armAimLimbControlImportance[NetworkConstants::networkMaxNumArms], 
     m_feetOnGroundAmountImportance, 
     m_spineInContactImportance;

  friend class AimBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numArmFinalLimbControl = calculateNumArmFinalLimbControl();
      for (uint32_t i=0; i<numArmFinalLimbControl; i++)
      {
        if (getArmFinalLimbControlImportance(i) > 0.0f)
          armFinalLimbControl[i].validate();
      }
    }
    {
      uint32_t numArmAimLimbControl = calculateNumArmAimLimbControl();
      for (uint32_t i=0; i<numArmAimLimbControl; i++)
      {
        if (getArmAimLimbControlImportance(i) > 0.0f)
          armAimLimbControl[i].validate();
      }
    }
    if (getFeetOnGroundAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(feetOnGroundAmount), "feetOnGroundAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_AIMBEHAVIOURINTERFACE_DATA_H

