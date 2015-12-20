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

#ifndef NM_MDF_BALANCEPOSER_DATA_H
#define NM_MDF_BALANCEPOSER_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/BalancePoser.module"

// external types
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/BalancePoseParameters.h"

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
 * \class BalancePoserInputs
 * \ingroup BalancePoser
 * \brief Inputs Static balance control/management
 *
 * Data packet definition (180 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalancePoserInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalancePoserInputs));
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

  NM_INLINE void setPoseParameters(const BalancePoseParameters& _poseParameters, float poseParameters_importance = 1.f)
  {
    poseParameters = _poseParameters;
    postAssignPoseParameters(poseParameters);
    NMP_ASSERT(poseParameters_importance >= 0.0f && poseParameters_importance <= 1.0f);
    m_poseParametersImportance = poseParameters_importance;
  }
  NM_INLINE float getPoseParametersImportance() const { return m_poseParametersImportance; }
  NM_INLINE const float& getPoseParametersImportanceRef() const { return m_poseParametersImportance; }
  NM_INLINE const BalancePoseParameters& getPoseParameters() const { return poseParameters; }

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
  /// The support coordinate from has the same convention as the spine etc - i.e. y up, x fwd  (Transform)
  BalancePoseParameters poseParameters;
  NMP::Quat chestRelPelvisOrientation[NetworkConstants::networkMaxNumSpines];  /// The desired rotation of the spine relative to the pelvis (sent to SpinePose)  (will only set the first)  (OrientationDelta)
  NMP::Vector3 chestRelPelvisTranslation[NetworkConstants::networkMaxNumSpines];  /// The desired translation of the spine relative to the pelvis (sent to SpinePose)  (will only set the first)  (PositionDelta)
  NMP::Quat headRelChestOrientation[NetworkConstants::networkMaxNumHeads];  /// The desired rotation of the head relative to the spine  (OrientationDelta)

  // post-assignment stubs
  NM_INLINE void postAssignPelvisRelSupport(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "pelvisRelSupport");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPoseParameters(const BalancePoseParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
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
     m_poseParametersImportance, 
     m_chestRelPelvisOrientationImportance[NetworkConstants::networkMaxNumSpines], 
     m_chestRelPelvisTranslationImportance[NetworkConstants::networkMaxNumSpines], 
     m_headRelChestOrientationImportance[NetworkConstants::networkMaxNumHeads];

  friend class BalancePoser_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getPelvisRelSupportImportance() > 0.0f)
    {
      NM_VALIDATOR(Matrix34Orthonormal(pelvisRelSupport), "pelvisRelSupport");
    }
    if (getPoseParametersImportance() > 0.0f)
    {
      poseParameters.validate();
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
 * \class BalancePoserOutputs
 * \ingroup BalancePoser
 * \brief Outputs Static balance control/management
 *
 * Data packet definition (128 bytes, 160 aligned)
 */
NMP_ALIGN_PREFIX(32) struct BalancePoserOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(BalancePoserOutputs));
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
  /// The support coordinate from has the same convention as the spine etc - i.e. y up, x fwd  (Transform)
  NMP::Quat chestRelPelvisOrientation[NetworkConstants::networkMaxNumSpines];  /// The desired rotation of the spine relative to the pelvis (sent to SpinePose)  (will only set the first)  (OrientationDelta)
  NMP::Vector3 chestRelPelvisTranslation[NetworkConstants::networkMaxNumSpines];  /// The desired translation of the spine relative to the pelvis (sent to SpinePose)  (will only set the first)  (PositionDelta)
  NMP::Quat headRelChestOrientation[NetworkConstants::networkMaxNumHeads];  /// The desired rotation of the head relative to the spine  (OrientationDelta)

  // post-assignment stubs
  NM_INLINE void postAssignPelvisRelSupport(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "pelvisRelSupport");
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
     m_chestRelPelvisOrientationImportance[NetworkConstants::networkMaxNumSpines], 
     m_chestRelPelvisTranslationImportance[NetworkConstants::networkMaxNumSpines], 
     m_headRelChestOrientationImportance[NetworkConstants::networkMaxNumHeads];

  friend class BalancePoser_Con;

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

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BALANCEPOSER_DATA_H

