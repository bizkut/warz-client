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

#ifndef NM_MDF_SITBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_SITBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/SitBehaviourInterface.module"

// external types
#include "NMPlatform/NMMatrix34.h"

// known types
#include "Types/SitParameters.h"

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
 * \class SitBehaviourInterfaceData
 * \ingroup SitBehaviourInterface
 * \brief Data Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (408 bytes, 416 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SitBehaviourInterfaceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SitBehaviourInterfaceData));
  }

  NMP::Matrix34 cachedSitPoseEndRelativeToRoots[NetworkConstants::networkMaxNumLimbs];  // Following are only used if the behaviour param UseSingleFrameForPose is set to true  (Transform)
  float cachedSitPoseWeights[NetworkConstants::networkMaxNumLimbs];

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(cachedSitPoseEndRelativeToRoots[0]), "cachedSitPoseEndRelativeToRoots[0]");
    NM_VALIDATOR(Matrix34Orthonormal(cachedSitPoseEndRelativeToRoots[1]), "cachedSitPoseEndRelativeToRoots[1]");
    NM_VALIDATOR(Matrix34Orthonormal(cachedSitPoseEndRelativeToRoots[2]), "cachedSitPoseEndRelativeToRoots[2]");
    NM_VALIDATOR(Matrix34Orthonormal(cachedSitPoseEndRelativeToRoots[3]), "cachedSitPoseEndRelativeToRoots[3]");
    NM_VALIDATOR(Matrix34Orthonormal(cachedSitPoseEndRelativeToRoots[4]), "cachedSitPoseEndRelativeToRoots[4]");
    NM_VALIDATOR(Matrix34Orthonormal(cachedSitPoseEndRelativeToRoots[5]), "cachedSitPoseEndRelativeToRoots[5]");
    NM_VALIDATOR(FloatValid(cachedSitPoseWeights[0]), "cachedSitPoseWeights[0]");
    NM_VALIDATOR(FloatValid(cachedSitPoseWeights[1]), "cachedSitPoseWeights[1]");
    NM_VALIDATOR(FloatValid(cachedSitPoseWeights[2]), "cachedSitPoseWeights[2]");
    NM_VALIDATOR(FloatValid(cachedSitPoseWeights[3]), "cachedSitPoseWeights[3]");
    NM_VALIDATOR(FloatValid(cachedSitPoseWeights[4]), "cachedSitPoseWeights[4]");
    NM_VALIDATOR(FloatValid(cachedSitPoseWeights[5]), "cachedSitPoseWeights[5]");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SitBehaviourInterfaceOutputs
 * \ingroup SitBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (455 bytes, 480 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SitBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SitBehaviourInterfaceOutputs));
  }

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

  NM_INLINE void setSitParameters(const SitParameters& _sitParameters, float sitParameters_importance = 1.f)
  {
    sitParameters = _sitParameters;
    postAssignSitParameters(sitParameters);
    NMP_ASSERT(sitParameters_importance >= 0.0f && sitParameters_importance <= 1.0f);
    m_sitParametersImportance = sitParameters_importance;
  }
  NM_INLINE float getSitParametersImportance() const { return m_sitParametersImportance; }
  NM_INLINE const float& getSitParametersImportanceRef() const { return m_sitParametersImportance; }
  NM_INLINE const SitParameters& getSitParameters() const { return sitParameters; }

  NM_INLINE void setLegsSwingWeight(const float& _legsSwingWeight, float legsSwingWeight_importance = 1.f)
  {
    legsSwingWeight = _legsSwingWeight;
    postAssignLegsSwingWeight(legsSwingWeight);
    NMP_ASSERT(legsSwingWeight_importance >= 0.0f && legsSwingWeight_importance <= 1.0f);
    m_legsSwingWeightImportance = legsSwingWeight_importance;
  }
  NM_INLINE float getLegsSwingWeightImportance() const { return m_legsSwingWeightImportance; }
  NM_INLINE const float& getLegsSwingWeightImportanceRef() const { return m_legsSwingWeightImportance; }
  NM_INLINE const float& getLegsSwingWeight() const { return legsSwingWeight; }

  NM_INLINE void setSupportWithArms(const bool& _supportWithArms, float supportWithArms_importance = 1.f)
  {
    supportWithArms = _supportWithArms;
    NMP_ASSERT(supportWithArms_importance >= 0.0f && supportWithArms_importance <= 1.0f);
    m_supportWithArmsImportance = supportWithArms_importance;
  }
  NM_INLINE float getSupportWithArmsImportance() const { return m_supportWithArmsImportance; }
  NM_INLINE const float& getSupportWithArmsImportanceRef() const { return m_supportWithArmsImportance; }
  NM_INLINE const bool& getSupportWithArms() const { return supportWithArms; }

  NM_INLINE void setSupportWithLegs(const bool& _supportWithLegs, float supportWithLegs_importance = 1.f)
  {
    supportWithLegs = _supportWithLegs;
    NMP_ASSERT(supportWithLegs_importance >= 0.0f && supportWithLegs_importance <= 1.0f);
    m_supportWithLegsImportance = supportWithLegs_importance;
  }
  NM_INLINE float getSupportWithLegsImportance() const { return m_supportWithLegsImportance; }
  NM_INLINE const float& getSupportWithLegsImportanceRef() const { return m_supportWithLegsImportance; }
  NM_INLINE const bool& getSupportWithLegs() const { return supportWithLegs; }

  NM_INLINE void setSupportWithSpines(const bool& _supportWithSpines, float supportWithSpines_importance = 1.f)
  {
    supportWithSpines = _supportWithSpines;
    NMP_ASSERT(supportWithSpines_importance >= 0.0f && supportWithSpines_importance <= 1.0f);
    m_supportWithSpinesImportance = supportWithSpines_importance;
  }
  NM_INLINE float getSupportWithSpinesImportance() const { return m_supportWithSpinesImportance; }
  NM_INLINE const float& getSupportWithSpinesImportanceRef() const { return m_supportWithSpinesImportance; }
  NM_INLINE const bool& getSupportWithSpines() const { return supportWithSpines; }

protected:

  NMP::Matrix34 armPoseEndRelativeToRoot[NetworkConstants::networkMaxNumArms];  ///< (Transform)
  NMP::Matrix34 legPoseEndRelativeToRoot[NetworkConstants::networkMaxNumLegs];  ///< (Transform)
  NMP::Matrix34 spinePoseEndRelativeToRoot[NetworkConstants::networkMaxNumSpines];  ///< (Transform)
  NMP::Matrix34 headPoseEndRelativeToRoot[NetworkConstants::networkMaxNumHeads];  ///< (Transform)
  SitParameters sitParameters;
  float legsSwingWeight;  ///< Used to disable leg swing when attempting to sit.  (Multiplier)
  bool supportWithArms;
  bool supportWithLegs;
  bool supportWithSpines;

  // post-assignment stubs
  NM_INLINE void postAssignArmPoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "armPoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegPoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "legPoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinePoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "spinePoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadPoseEndRelativeToRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "headPoseEndRelativeToRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSitParameters(const SitParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegsSwingWeight(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "legsSwingWeight");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_armPoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumArms], 
     m_legPoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumLegs], 
     m_spinePoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumSpines], 
     m_headPoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumHeads], 
     m_sitParametersImportance, 
     m_legsSwingWeightImportance, 
     m_supportWithArmsImportance, 
     m_supportWithLegsImportance, 
     m_supportWithSpinesImportance;

  friend class SitBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
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
      uint32_t numHeadPoseEndRelativeToRoot = calculateNumHeadPoseEndRelativeToRoot();
      for (uint32_t i=0; i<numHeadPoseEndRelativeToRoot; i++)
      {
        if (getHeadPoseEndRelativeToRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(headPoseEndRelativeToRoot[i]), "headPoseEndRelativeToRoot");
        }
      }
    }
    if (getSitParametersImportance() > 0.0f)
    {
      sitParameters.validate();
    }
    if (getLegsSwingWeightImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(legsSwingWeight), "legsSwingWeight");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SitBehaviourInterfaceFeedbackInputs
 * \ingroup SitBehaviourInterface
 * \brief FeedbackInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (8 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SitBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SitBehaviourInterfaceFeedbackInputs));
  }

  NM_INLINE void setSitAmount(const float& _sitAmount, float sitAmount_importance = 1.f)
  {
    sitAmount = _sitAmount;
    postAssignSitAmount(sitAmount);
    NMP_ASSERT(sitAmount_importance >= 0.0f && sitAmount_importance <= 1.0f);
    m_sitAmountImportance = sitAmount_importance;
  }
  NM_INLINE float getSitAmountImportance() const { return m_sitAmountImportance; }
  NM_INLINE const float& getSitAmountImportanceRef() const { return m_sitAmountImportance; }
  NM_INLINE const float& getSitAmount() const { return sitAmount; }

protected:

  float sitAmount;                           ///< (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignSitAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "sitAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_sitAmountImportance;

  friend class SitBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getSitAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(sitAmount), "sitAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SitBehaviourInterfaceFeedbackOutputs
 * \ingroup SitBehaviourInterface
 * \brief FeedbackOutputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (8 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SitBehaviourInterfaceFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SitBehaviourInterfaceFeedbackOutputs));
  }

  NM_INLINE float getSitAmountImportance() const { return m_sitAmountImportance; }
  NM_INLINE const float& getSitAmountImportanceRef() const { return m_sitAmountImportance; }
  NM_INLINE const float& getSitAmount() const { return sitAmount; }

protected:

  float sitAmount;                           ///< (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignSitAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "sitAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_sitAmountImportance;

  friend class SitBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getSitAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(sitAmount), "sitAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SITBEHAVIOURINTERFACE_DATA_H

