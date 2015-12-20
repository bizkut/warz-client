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

#ifndef NM_MDF_ANIMATEBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_ANIMATEBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/AnimateBehaviourInterface.module"

// external types
#include "NMPlatform/NMMatrix34.h"

// known types
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
 * \class AnimateBehaviourInterfaceData
 * \ingroup AnimateBehaviourInterface
 * \brief Data Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (1176 bytes, 1184 aligned)
 */
NMP_ALIGN_PREFIX(32) struct AnimateBehaviourInterfaceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(AnimateBehaviourInterfaceData));
  }

  PoseData cachedPose[NetworkConstants::networkMaxNumLimbs];  // Pose caching is only used if the behaviour param UseSingleFrameForBalancePose is set
  NMP::Matrix34 previousPoseInputs[NetworkConstants::networkMaxNumLimbs];  ///< (Transform)
  float previousPoseImportances[NetworkConstants::networkMaxNumLimbs];  ///< (Weight)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    cachedPose[0].validate();
    cachedPose[1].validate();
    cachedPose[2].validate();
    cachedPose[3].validate();
    cachedPose[4].validate();
    cachedPose[5].validate();
    NM_VALIDATOR(Matrix34Orthonormal(previousPoseInputs[0]), "previousPoseInputs[0]");
    NM_VALIDATOR(Matrix34Orthonormal(previousPoseInputs[1]), "previousPoseInputs[1]");
    NM_VALIDATOR(Matrix34Orthonormal(previousPoseInputs[2]), "previousPoseInputs[2]");
    NM_VALIDATOR(Matrix34Orthonormal(previousPoseInputs[3]), "previousPoseInputs[3]");
    NM_VALIDATOR(Matrix34Orthonormal(previousPoseInputs[4]), "previousPoseInputs[4]");
    NM_VALIDATOR(Matrix34Orthonormal(previousPoseInputs[5]), "previousPoseInputs[5]");
    NM_VALIDATOR(FloatNonNegative(previousPoseImportances[0]), "previousPoseImportances[0]");
    NM_VALIDATOR(FloatNonNegative(previousPoseImportances[1]), "previousPoseImportances[1]");
    NM_VALIDATOR(FloatNonNegative(previousPoseImportances[2]), "previousPoseImportances[2]");
    NM_VALIDATOR(FloatNonNegative(previousPoseImportances[3]), "previousPoseImportances[3]");
    NM_VALIDATOR(FloatNonNegative(previousPoseImportances[4]), "previousPoseImportances[4]");
    NM_VALIDATOR(FloatNonNegative(previousPoseImportances[5]), "previousPoseImportances[5]");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class AnimateBehaviourInterfaceOutputs
 * \ingroup AnimateBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (1589 bytes, 1600 aligned)
 */
NMP_ALIGN_PREFIX(32) struct AnimateBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(AnimateBehaviourInterfaceOutputs));
  }

  enum { maxArmPoseHighPriority = 2 };
  NM_INLINE unsigned int getMaxArmPoseHighPriority() const { return maxArmPoseHighPriority; }
  NM_INLINE void setArmPoseHighPriority(unsigned int number, const PoseData* _armPoseHighPriority, float armPoseHighPriority_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmPoseHighPriority);
    NMP_ASSERT(armPoseHighPriority_importance >= 0.0f && armPoseHighPriority_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armPoseHighPriority[i] = _armPoseHighPriority[i];
      postAssignArmPoseHighPriority(armPoseHighPriority[i]);
      m_armPoseHighPriorityImportance[i] = armPoseHighPriority_importance;
    }
  }
  NM_INLINE void setArmPoseHighPriorityAt(unsigned int index, const PoseData& _armPoseHighPriority, float armPoseHighPriority_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmPoseHighPriority);
    armPoseHighPriority[index] = _armPoseHighPriority;
    NMP_ASSERT(armPoseHighPriority_importance >= 0.0f && armPoseHighPriority_importance <= 1.0f);
    postAssignArmPoseHighPriority(armPoseHighPriority[index]);
    m_armPoseHighPriorityImportance[index] = armPoseHighPriority_importance;
  }
  NM_INLINE PoseData& startArmPoseHighPriorityModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxArmPoseHighPriority);
    m_armPoseHighPriorityImportance[index] = -1.0f; // set invalid until stopArmPoseHighPriorityModificationAt()
    return armPoseHighPriority[index];
  }
  NM_INLINE void stopArmPoseHighPriorityModificationAt(unsigned int index, float armPoseHighPriority_importance = 1.f)
  {
    postAssignArmPoseHighPriority(armPoseHighPriority[index]);
    NMP_ASSERT(armPoseHighPriority_importance >= 0.0f && armPoseHighPriority_importance <= 1.0f);
    m_armPoseHighPriorityImportance[index] = armPoseHighPriority_importance;
  }
  NM_INLINE float getArmPoseHighPriorityImportance(int index) const { return m_armPoseHighPriorityImportance[index]; }
  NM_INLINE const float& getArmPoseHighPriorityImportanceRef(int index) const { return m_armPoseHighPriorityImportance[index]; }
  NM_INLINE unsigned int calculateNumArmPoseHighPriority() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armPoseHighPriorityImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getArmPoseHighPriority(unsigned int index) const { NMP_ASSERT(index <= maxArmPoseHighPriority); return armPoseHighPriority[index]; }

  enum { maxHeadPoseHighPriority = 1 };
  NM_INLINE unsigned int getMaxHeadPoseHighPriority() const { return maxHeadPoseHighPriority; }
  NM_INLINE void setHeadPoseHighPriority(unsigned int number, const PoseData* _headPoseHighPriority, float headPoseHighPriority_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHeadPoseHighPriority);
    NMP_ASSERT(headPoseHighPriority_importance >= 0.0f && headPoseHighPriority_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      headPoseHighPriority[i] = _headPoseHighPriority[i];
      postAssignHeadPoseHighPriority(headPoseHighPriority[i]);
      m_headPoseHighPriorityImportance[i] = headPoseHighPriority_importance;
    }
  }
  NM_INLINE void setHeadPoseHighPriorityAt(unsigned int index, const PoseData& _headPoseHighPriority, float headPoseHighPriority_importance = 1.f)
  {
    NMP_ASSERT(index < maxHeadPoseHighPriority);
    headPoseHighPriority[index] = _headPoseHighPriority;
    NMP_ASSERT(headPoseHighPriority_importance >= 0.0f && headPoseHighPriority_importance <= 1.0f);
    postAssignHeadPoseHighPriority(headPoseHighPriority[index]);
    m_headPoseHighPriorityImportance[index] = headPoseHighPriority_importance;
  }
  NM_INLINE PoseData& startHeadPoseHighPriorityModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxHeadPoseHighPriority);
    m_headPoseHighPriorityImportance[index] = -1.0f; // set invalid until stopHeadPoseHighPriorityModificationAt()
    return headPoseHighPriority[index];
  }
  NM_INLINE void stopHeadPoseHighPriorityModificationAt(unsigned int index, float headPoseHighPriority_importance = 1.f)
  {
    postAssignHeadPoseHighPriority(headPoseHighPriority[index]);
    NMP_ASSERT(headPoseHighPriority_importance >= 0.0f && headPoseHighPriority_importance <= 1.0f);
    m_headPoseHighPriorityImportance[index] = headPoseHighPriority_importance;
  }
  NM_INLINE float getHeadPoseHighPriorityImportance(int index) const { return m_headPoseHighPriorityImportance[index]; }
  NM_INLINE const float& getHeadPoseHighPriorityImportanceRef(int index) const { return m_headPoseHighPriorityImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadPoseHighPriority() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headPoseHighPriorityImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getHeadPoseHighPriority(unsigned int index) const { NMP_ASSERT(index <= maxHeadPoseHighPriority); return headPoseHighPriority[index]; }

  enum { maxSpinePoseHighPriority = 1 };
  NM_INLINE unsigned int getMaxSpinePoseHighPriority() const { return maxSpinePoseHighPriority; }
  NM_INLINE void setSpinePoseHighPriority(unsigned int number, const PoseData* _spinePoseHighPriority, float spinePoseHighPriority_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpinePoseHighPriority);
    NMP_ASSERT(spinePoseHighPriority_importance >= 0.0f && spinePoseHighPriority_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spinePoseHighPriority[i] = _spinePoseHighPriority[i];
      postAssignSpinePoseHighPriority(spinePoseHighPriority[i]);
      m_spinePoseHighPriorityImportance[i] = spinePoseHighPriority_importance;
    }
  }
  NM_INLINE void setSpinePoseHighPriorityAt(unsigned int index, const PoseData& _spinePoseHighPriority, float spinePoseHighPriority_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpinePoseHighPriority);
    spinePoseHighPriority[index] = _spinePoseHighPriority;
    NMP_ASSERT(spinePoseHighPriority_importance >= 0.0f && spinePoseHighPriority_importance <= 1.0f);
    postAssignSpinePoseHighPriority(spinePoseHighPriority[index]);
    m_spinePoseHighPriorityImportance[index] = spinePoseHighPriority_importance;
  }
  NM_INLINE PoseData& startSpinePoseHighPriorityModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxSpinePoseHighPriority);
    m_spinePoseHighPriorityImportance[index] = -1.0f; // set invalid until stopSpinePoseHighPriorityModificationAt()
    return spinePoseHighPriority[index];
  }
  NM_INLINE void stopSpinePoseHighPriorityModificationAt(unsigned int index, float spinePoseHighPriority_importance = 1.f)
  {
    postAssignSpinePoseHighPriority(spinePoseHighPriority[index]);
    NMP_ASSERT(spinePoseHighPriority_importance >= 0.0f && spinePoseHighPriority_importance <= 1.0f);
    m_spinePoseHighPriorityImportance[index] = spinePoseHighPriority_importance;
  }
  NM_INLINE float getSpinePoseHighPriorityImportance(int index) const { return m_spinePoseHighPriorityImportance[index]; }
  NM_INLINE const float& getSpinePoseHighPriorityImportanceRef(int index) const { return m_spinePoseHighPriorityImportance[index]; }
  NM_INLINE unsigned int calculateNumSpinePoseHighPriority() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spinePoseHighPriorityImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getSpinePoseHighPriority(unsigned int index) const { NMP_ASSERT(index <= maxSpinePoseHighPriority); return spinePoseHighPriority[index]; }

  enum { maxLegPoseHighPriority = 2 };
  NM_INLINE unsigned int getMaxLegPoseHighPriority() const { return maxLegPoseHighPriority; }
  NM_INLINE void setLegPoseHighPriority(unsigned int number, const PoseData* _legPoseHighPriority, float legPoseHighPriority_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegPoseHighPriority);
    NMP_ASSERT(legPoseHighPriority_importance >= 0.0f && legPoseHighPriority_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legPoseHighPriority[i] = _legPoseHighPriority[i];
      postAssignLegPoseHighPriority(legPoseHighPriority[i]);
      m_legPoseHighPriorityImportance[i] = legPoseHighPriority_importance;
    }
  }
  NM_INLINE void setLegPoseHighPriorityAt(unsigned int index, const PoseData& _legPoseHighPriority, float legPoseHighPriority_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegPoseHighPriority);
    legPoseHighPriority[index] = _legPoseHighPriority;
    NMP_ASSERT(legPoseHighPriority_importance >= 0.0f && legPoseHighPriority_importance <= 1.0f);
    postAssignLegPoseHighPriority(legPoseHighPriority[index]);
    m_legPoseHighPriorityImportance[index] = legPoseHighPriority_importance;
  }
  NM_INLINE PoseData& startLegPoseHighPriorityModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxLegPoseHighPriority);
    m_legPoseHighPriorityImportance[index] = -1.0f; // set invalid until stopLegPoseHighPriorityModificationAt()
    return legPoseHighPriority[index];
  }
  NM_INLINE void stopLegPoseHighPriorityModificationAt(unsigned int index, float legPoseHighPriority_importance = 1.f)
  {
    postAssignLegPoseHighPriority(legPoseHighPriority[index]);
    NMP_ASSERT(legPoseHighPriority_importance >= 0.0f && legPoseHighPriority_importance <= 1.0f);
    m_legPoseHighPriorityImportance[index] = legPoseHighPriority_importance;
  }
  NM_INLINE float getLegPoseHighPriorityImportance(int index) const { return m_legPoseHighPriorityImportance[index]; }
  NM_INLINE const float& getLegPoseHighPriorityImportanceRef(int index) const { return m_legPoseHighPriorityImportance[index]; }
  NM_INLINE unsigned int calculateNumLegPoseHighPriority() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legPoseHighPriorityImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getLegPoseHighPriority(unsigned int index) const { NMP_ASSERT(index <= maxLegPoseHighPriority); return legPoseHighPriority[index]; }

  enum { maxArmPoseLowPriority = 2 };
  NM_INLINE unsigned int getMaxArmPoseLowPriority() const { return maxArmPoseLowPriority; }
  NM_INLINE void setArmPoseLowPriority(unsigned int number, const PoseData* _armPoseLowPriority, float armPoseLowPriority_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmPoseLowPriority);
    NMP_ASSERT(armPoseLowPriority_importance >= 0.0f && armPoseLowPriority_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armPoseLowPriority[i] = _armPoseLowPriority[i];
      postAssignArmPoseLowPriority(armPoseLowPriority[i]);
      m_armPoseLowPriorityImportance[i] = armPoseLowPriority_importance;
    }
  }
  NM_INLINE void setArmPoseLowPriorityAt(unsigned int index, const PoseData& _armPoseLowPriority, float armPoseLowPriority_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmPoseLowPriority);
    armPoseLowPriority[index] = _armPoseLowPriority;
    NMP_ASSERT(armPoseLowPriority_importance >= 0.0f && armPoseLowPriority_importance <= 1.0f);
    postAssignArmPoseLowPriority(armPoseLowPriority[index]);
    m_armPoseLowPriorityImportance[index] = armPoseLowPriority_importance;
  }
  NM_INLINE PoseData& startArmPoseLowPriorityModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxArmPoseLowPriority);
    m_armPoseLowPriorityImportance[index] = -1.0f; // set invalid until stopArmPoseLowPriorityModificationAt()
    return armPoseLowPriority[index];
  }
  NM_INLINE void stopArmPoseLowPriorityModificationAt(unsigned int index, float armPoseLowPriority_importance = 1.f)
  {
    postAssignArmPoseLowPriority(armPoseLowPriority[index]);
    NMP_ASSERT(armPoseLowPriority_importance >= 0.0f && armPoseLowPriority_importance <= 1.0f);
    m_armPoseLowPriorityImportance[index] = armPoseLowPriority_importance;
  }
  NM_INLINE float getArmPoseLowPriorityImportance(int index) const { return m_armPoseLowPriorityImportance[index]; }
  NM_INLINE const float& getArmPoseLowPriorityImportanceRef(int index) const { return m_armPoseLowPriorityImportance[index]; }
  NM_INLINE unsigned int calculateNumArmPoseLowPriority() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armPoseLowPriorityImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getArmPoseLowPriority(unsigned int index) const { NMP_ASSERT(index <= maxArmPoseLowPriority); return armPoseLowPriority[index]; }

  enum { maxHeadPoseLowPriority = 1 };
  NM_INLINE unsigned int getMaxHeadPoseLowPriority() const { return maxHeadPoseLowPriority; }
  NM_INLINE void setHeadPoseLowPriority(unsigned int number, const PoseData* _headPoseLowPriority, float headPoseLowPriority_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHeadPoseLowPriority);
    NMP_ASSERT(headPoseLowPriority_importance >= 0.0f && headPoseLowPriority_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      headPoseLowPriority[i] = _headPoseLowPriority[i];
      postAssignHeadPoseLowPriority(headPoseLowPriority[i]);
      m_headPoseLowPriorityImportance[i] = headPoseLowPriority_importance;
    }
  }
  NM_INLINE void setHeadPoseLowPriorityAt(unsigned int index, const PoseData& _headPoseLowPriority, float headPoseLowPriority_importance = 1.f)
  {
    NMP_ASSERT(index < maxHeadPoseLowPriority);
    headPoseLowPriority[index] = _headPoseLowPriority;
    NMP_ASSERT(headPoseLowPriority_importance >= 0.0f && headPoseLowPriority_importance <= 1.0f);
    postAssignHeadPoseLowPriority(headPoseLowPriority[index]);
    m_headPoseLowPriorityImportance[index] = headPoseLowPriority_importance;
  }
  NM_INLINE PoseData& startHeadPoseLowPriorityModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxHeadPoseLowPriority);
    m_headPoseLowPriorityImportance[index] = -1.0f; // set invalid until stopHeadPoseLowPriorityModificationAt()
    return headPoseLowPriority[index];
  }
  NM_INLINE void stopHeadPoseLowPriorityModificationAt(unsigned int index, float headPoseLowPriority_importance = 1.f)
  {
    postAssignHeadPoseLowPriority(headPoseLowPriority[index]);
    NMP_ASSERT(headPoseLowPriority_importance >= 0.0f && headPoseLowPriority_importance <= 1.0f);
    m_headPoseLowPriorityImportance[index] = headPoseLowPriority_importance;
  }
  NM_INLINE float getHeadPoseLowPriorityImportance(int index) const { return m_headPoseLowPriorityImportance[index]; }
  NM_INLINE const float& getHeadPoseLowPriorityImportanceRef(int index) const { return m_headPoseLowPriorityImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadPoseLowPriority() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headPoseLowPriorityImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getHeadPoseLowPriority(unsigned int index) const { NMP_ASSERT(index <= maxHeadPoseLowPriority); return headPoseLowPriority[index]; }

  enum { maxSpinePoseLowPriority = 1 };
  NM_INLINE unsigned int getMaxSpinePoseLowPriority() const { return maxSpinePoseLowPriority; }
  NM_INLINE void setSpinePoseLowPriority(unsigned int number, const PoseData* _spinePoseLowPriority, float spinePoseLowPriority_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpinePoseLowPriority);
    NMP_ASSERT(spinePoseLowPriority_importance >= 0.0f && spinePoseLowPriority_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spinePoseLowPriority[i] = _spinePoseLowPriority[i];
      postAssignSpinePoseLowPriority(spinePoseLowPriority[i]);
      m_spinePoseLowPriorityImportance[i] = spinePoseLowPriority_importance;
    }
  }
  NM_INLINE void setSpinePoseLowPriorityAt(unsigned int index, const PoseData& _spinePoseLowPriority, float spinePoseLowPriority_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpinePoseLowPriority);
    spinePoseLowPriority[index] = _spinePoseLowPriority;
    NMP_ASSERT(spinePoseLowPriority_importance >= 0.0f && spinePoseLowPriority_importance <= 1.0f);
    postAssignSpinePoseLowPriority(spinePoseLowPriority[index]);
    m_spinePoseLowPriorityImportance[index] = spinePoseLowPriority_importance;
  }
  NM_INLINE PoseData& startSpinePoseLowPriorityModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxSpinePoseLowPriority);
    m_spinePoseLowPriorityImportance[index] = -1.0f; // set invalid until stopSpinePoseLowPriorityModificationAt()
    return spinePoseLowPriority[index];
  }
  NM_INLINE void stopSpinePoseLowPriorityModificationAt(unsigned int index, float spinePoseLowPriority_importance = 1.f)
  {
    postAssignSpinePoseLowPriority(spinePoseLowPriority[index]);
    NMP_ASSERT(spinePoseLowPriority_importance >= 0.0f && spinePoseLowPriority_importance <= 1.0f);
    m_spinePoseLowPriorityImportance[index] = spinePoseLowPriority_importance;
  }
  NM_INLINE float getSpinePoseLowPriorityImportance(int index) const { return m_spinePoseLowPriorityImportance[index]; }
  NM_INLINE const float& getSpinePoseLowPriorityImportanceRef(int index) const { return m_spinePoseLowPriorityImportance[index]; }
  NM_INLINE unsigned int calculateNumSpinePoseLowPriority() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spinePoseLowPriorityImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getSpinePoseLowPriority(unsigned int index) const { NMP_ASSERT(index <= maxSpinePoseLowPriority); return spinePoseLowPriority[index]; }

  enum { maxLegPoseLowPriority = 2 };
  NM_INLINE unsigned int getMaxLegPoseLowPriority() const { return maxLegPoseLowPriority; }
  NM_INLINE void setLegPoseLowPriority(unsigned int number, const PoseData* _legPoseLowPriority, float legPoseLowPriority_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegPoseLowPriority);
    NMP_ASSERT(legPoseLowPriority_importance >= 0.0f && legPoseLowPriority_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legPoseLowPriority[i] = _legPoseLowPriority[i];
      postAssignLegPoseLowPriority(legPoseLowPriority[i]);
      m_legPoseLowPriorityImportance[i] = legPoseLowPriority_importance;
    }
  }
  NM_INLINE void setLegPoseLowPriorityAt(unsigned int index, const PoseData& _legPoseLowPriority, float legPoseLowPriority_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegPoseLowPriority);
    legPoseLowPriority[index] = _legPoseLowPriority;
    NMP_ASSERT(legPoseLowPriority_importance >= 0.0f && legPoseLowPriority_importance <= 1.0f);
    postAssignLegPoseLowPriority(legPoseLowPriority[index]);
    m_legPoseLowPriorityImportance[index] = legPoseLowPriority_importance;
  }
  NM_INLINE PoseData& startLegPoseLowPriorityModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxLegPoseLowPriority);
    m_legPoseLowPriorityImportance[index] = -1.0f; // set invalid until stopLegPoseLowPriorityModificationAt()
    return legPoseLowPriority[index];
  }
  NM_INLINE void stopLegPoseLowPriorityModificationAt(unsigned int index, float legPoseLowPriority_importance = 1.f)
  {
    postAssignLegPoseLowPriority(legPoseLowPriority[index]);
    NMP_ASSERT(legPoseLowPriority_importance >= 0.0f && legPoseLowPriority_importance <= 1.0f);
    m_legPoseLowPriorityImportance[index] = legPoseLowPriority_importance;
  }
  NM_INLINE float getLegPoseLowPriorityImportance(int index) const { return m_legPoseLowPriorityImportance[index]; }
  NM_INLINE const float& getLegPoseLowPriorityImportanceRef(int index) const { return m_legPoseLowPriorityImportance[index]; }
  NM_INLINE unsigned int calculateNumLegPoseLowPriority() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legPoseLowPriorityImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getLegPoseLowPriority(unsigned int index) const { NMP_ASSERT(index <= maxLegPoseLowPriority); return legPoseLowPriority[index]; }

  NM_INLINE void setSleepingDisabled(const bool& _sleepingDisabled, float sleepingDisabled_importance = 1.f)
  {
    sleepingDisabled = _sleepingDisabled;
    NMP_ASSERT(sleepingDisabled_importance >= 0.0f && sleepingDisabled_importance <= 1.0f);
    m_sleepingDisabledImportance = sleepingDisabled_importance;
  }
  NM_INLINE float getSleepingDisabledImportance() const { return m_sleepingDisabledImportance; }
  NM_INLINE const float& getSleepingDisabledImportanceRef() const { return m_sleepingDisabledImportance; }
  NM_INLINE const bool& getSleepingDisabled() const { return sleepingDisabled; }

protected:

  PoseData armPoseHighPriority[NetworkConstants::networkMaxNumArms];  // Poses for each of the limb ends relative to their root
  PoseData headPoseHighPriority[NetworkConstants::networkMaxNumHeads];
  PoseData spinePoseHighPriority[NetworkConstants::networkMaxNumSpines];
  PoseData legPoseHighPriority[NetworkConstants::networkMaxNumLegs];
  PoseData armPoseLowPriority[NetworkConstants::networkMaxNumArms];
  PoseData headPoseLowPriority[NetworkConstants::networkMaxNumHeads];
  PoseData spinePoseLowPriority[NetworkConstants::networkMaxNumSpines];
  PoseData legPoseLowPriority[NetworkConstants::networkMaxNumLegs];
  bool sleepingDisabled;  ///< Is sleeping disabled when stationary

  // post-assignment stubs
  NM_INLINE void postAssignArmPoseHighPriority(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadPoseHighPriority(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinePoseHighPriority(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegPoseHighPriority(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmPoseLowPriority(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadPoseLowPriority(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinePoseLowPriority(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegPoseLowPriority(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_armPoseHighPriorityImportance[NetworkConstants::networkMaxNumArms], 
     m_headPoseHighPriorityImportance[NetworkConstants::networkMaxNumHeads], 
     m_spinePoseHighPriorityImportance[NetworkConstants::networkMaxNumSpines], 
     m_legPoseHighPriorityImportance[NetworkConstants::networkMaxNumLegs], 
     m_armPoseLowPriorityImportance[NetworkConstants::networkMaxNumArms], 
     m_headPoseLowPriorityImportance[NetworkConstants::networkMaxNumHeads], 
     m_spinePoseLowPriorityImportance[NetworkConstants::networkMaxNumSpines], 
     m_legPoseLowPriorityImportance[NetworkConstants::networkMaxNumLegs], 
     m_sleepingDisabledImportance;


public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numArmPoseHighPriority = calculateNumArmPoseHighPriority();
      for (uint32_t i=0; i<numArmPoseHighPriority; i++)
      {
        if (getArmPoseHighPriorityImportance(i) > 0.0f)
          armPoseHighPriority[i].validate();
      }
    }
    {
      uint32_t numHeadPoseHighPriority = calculateNumHeadPoseHighPriority();
      for (uint32_t i=0; i<numHeadPoseHighPriority; i++)
      {
        if (getHeadPoseHighPriorityImportance(i) > 0.0f)
          headPoseHighPriority[i].validate();
      }
    }
    {
      uint32_t numSpinePoseHighPriority = calculateNumSpinePoseHighPriority();
      for (uint32_t i=0; i<numSpinePoseHighPriority; i++)
      {
        if (getSpinePoseHighPriorityImportance(i) > 0.0f)
          spinePoseHighPriority[i].validate();
      }
    }
    {
      uint32_t numLegPoseHighPriority = calculateNumLegPoseHighPriority();
      for (uint32_t i=0; i<numLegPoseHighPriority; i++)
      {
        if (getLegPoseHighPriorityImportance(i) > 0.0f)
          legPoseHighPriority[i].validate();
      }
    }
    {
      uint32_t numArmPoseLowPriority = calculateNumArmPoseLowPriority();
      for (uint32_t i=0; i<numArmPoseLowPriority; i++)
      {
        if (getArmPoseLowPriorityImportance(i) > 0.0f)
          armPoseLowPriority[i].validate();
      }
    }
    {
      uint32_t numHeadPoseLowPriority = calculateNumHeadPoseLowPriority();
      for (uint32_t i=0; i<numHeadPoseLowPriority; i++)
      {
        if (getHeadPoseLowPriorityImportance(i) > 0.0f)
          headPoseLowPriority[i].validate();
      }
    }
    {
      uint32_t numSpinePoseLowPriority = calculateNumSpinePoseLowPriority();
      for (uint32_t i=0; i<numSpinePoseLowPriority; i++)
      {
        if (getSpinePoseLowPriorityImportance(i) > 0.0f)
          spinePoseLowPriority[i].validate();
      }
    }
    {
      uint32_t numLegPoseLowPriority = calculateNumLegPoseLowPriority();
      for (uint32_t i=0; i<numLegPoseLowPriority; i++)
      {
        if (getLegPoseLowPriorityImportance(i) > 0.0f)
          legPoseLowPriority[i].validate();
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

#endif // NM_MDF_ANIMATEBEHAVIOURINTERFACE_DATA_H

