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

#ifndef NM_MDF_WRITHEBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_WRITHEBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/WritheBehaviourInterface.module"

// known types
#include "Types/WritheParameters.h"
#include "Types/PoseData.h"
#include "Types/RandomLookParameters.h"

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
 * \class WritheBehaviourInterfaceOutputs
 * \ingroup WritheBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (668 bytes, 672 aligned)
 */
NMP_ALIGN_PREFIX(32) struct WritheBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(WritheBehaviourInterfaceOutputs));
  }

  enum { maxHeadPose = 1 };
  NM_INLINE unsigned int getMaxHeadPose() const { return maxHeadPose; }
  NM_INLINE void setHeadPose(unsigned int number, const PoseData* _headPose, float headPose_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHeadPose);
    NMP_ASSERT(headPose_importance >= 0.0f && headPose_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      headPose[i] = _headPose[i];
      postAssignHeadPose(headPose[i]);
      m_headPoseImportance[i] = headPose_importance;
    }
  }
  NM_INLINE void setHeadPoseAt(unsigned int index, const PoseData& _headPose, float headPose_importance = 1.f)
  {
    NMP_ASSERT(index < maxHeadPose);
    headPose[index] = _headPose;
    NMP_ASSERT(headPose_importance >= 0.0f && headPose_importance <= 1.0f);
    postAssignHeadPose(headPose[index]);
    m_headPoseImportance[index] = headPose_importance;
  }
  NM_INLINE PoseData& startHeadPoseModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxHeadPose);
    m_headPoseImportance[index] = -1.0f; // set invalid until stopHeadPoseModificationAt()
    return headPose[index];
  }
  NM_INLINE void stopHeadPoseModificationAt(unsigned int index, float headPose_importance = 1.f)
  {
    postAssignHeadPose(headPose[index]);
    NMP_ASSERT(headPose_importance >= 0.0f && headPose_importance <= 1.0f);
    m_headPoseImportance[index] = headPose_importance;
  }
  NM_INLINE float getHeadPoseImportance(int index) const { return m_headPoseImportance[index]; }
  NM_INLINE const float& getHeadPoseImportanceRef(int index) const { return m_headPoseImportance[index]; }
  NM_INLINE unsigned int calculateNumHeadPose() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_headPoseImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getHeadPose(unsigned int index) const { NMP_ASSERT(index <= maxHeadPose); return headPose[index]; }

  enum { maxArmsParams = 2 };
  NM_INLINE unsigned int getMaxArmsParams() const { return maxArmsParams; }
  NM_INLINE void setArmsParams(unsigned int number, const WritheParameters* _armsParams, float armsParams_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmsParams);
    NMP_ASSERT(armsParams_importance >= 0.0f && armsParams_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armsParams[i] = _armsParams[i];
      postAssignArmsParams(armsParams[i]);
      m_armsParamsImportance[i] = armsParams_importance;
    }
  }
  NM_INLINE void setArmsParamsAt(unsigned int index, const WritheParameters& _armsParams, float armsParams_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmsParams);
    armsParams[index] = _armsParams;
    NMP_ASSERT(armsParams_importance >= 0.0f && armsParams_importance <= 1.0f);
    postAssignArmsParams(armsParams[index]);
    m_armsParamsImportance[index] = armsParams_importance;
  }
  NM_INLINE WritheParameters& startArmsParamsModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxArmsParams);
    m_armsParamsImportance[index] = -1.0f; // set invalid until stopArmsParamsModificationAt()
    return armsParams[index];
  }
  NM_INLINE void stopArmsParamsModificationAt(unsigned int index, float armsParams_importance = 1.f)
  {
    postAssignArmsParams(armsParams[index]);
    NMP_ASSERT(armsParams_importance >= 0.0f && armsParams_importance <= 1.0f);
    m_armsParamsImportance[index] = armsParams_importance;
  }
  NM_INLINE float getArmsParamsImportance(int index) const { return m_armsParamsImportance[index]; }
  NM_INLINE const float& getArmsParamsImportanceRef(int index) const { return m_armsParamsImportance[index]; }
  NM_INLINE unsigned int calculateNumArmsParams() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armsParamsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const WritheParameters& getArmsParams(unsigned int index) const { NMP_ASSERT(index <= maxArmsParams); return armsParams[index]; }

  enum { maxLegsParams = 2 };
  NM_INLINE unsigned int getMaxLegsParams() const { return maxLegsParams; }
  NM_INLINE void setLegsParams(unsigned int number, const WritheParameters* _legsParams, float legsParams_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegsParams);
    NMP_ASSERT(legsParams_importance >= 0.0f && legsParams_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legsParams[i] = _legsParams[i];
      postAssignLegsParams(legsParams[i]);
      m_legsParamsImportance[i] = legsParams_importance;
    }
  }
  NM_INLINE void setLegsParamsAt(unsigned int index, const WritheParameters& _legsParams, float legsParams_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegsParams);
    legsParams[index] = _legsParams;
    NMP_ASSERT(legsParams_importance >= 0.0f && legsParams_importance <= 1.0f);
    postAssignLegsParams(legsParams[index]);
    m_legsParamsImportance[index] = legsParams_importance;
  }
  NM_INLINE WritheParameters& startLegsParamsModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxLegsParams);
    m_legsParamsImportance[index] = -1.0f; // set invalid until stopLegsParamsModificationAt()
    return legsParams[index];
  }
  NM_INLINE void stopLegsParamsModificationAt(unsigned int index, float legsParams_importance = 1.f)
  {
    postAssignLegsParams(legsParams[index]);
    NMP_ASSERT(legsParams_importance >= 0.0f && legsParams_importance <= 1.0f);
    m_legsParamsImportance[index] = legsParams_importance;
  }
  NM_INLINE float getLegsParamsImportance(int index) const { return m_legsParamsImportance[index]; }
  NM_INLINE const float& getLegsParamsImportanceRef(int index) const { return m_legsParamsImportance[index]; }
  NM_INLINE unsigned int calculateNumLegsParams() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legsParamsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const WritheParameters& getLegsParams(unsigned int index) const { NMP_ASSERT(index <= maxLegsParams); return legsParams[index]; }

  enum { maxSpinesParams = 1 };
  NM_INLINE unsigned int getMaxSpinesParams() const { return maxSpinesParams; }
  NM_INLINE void setSpinesParams(unsigned int number, const WritheParameters* _spinesParams, float spinesParams_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpinesParams);
    NMP_ASSERT(spinesParams_importance >= 0.0f && spinesParams_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spinesParams[i] = _spinesParams[i];
      postAssignSpinesParams(spinesParams[i]);
      m_spinesParamsImportance[i] = spinesParams_importance;
    }
  }
  NM_INLINE void setSpinesParamsAt(unsigned int index, const WritheParameters& _spinesParams, float spinesParams_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpinesParams);
    spinesParams[index] = _spinesParams;
    NMP_ASSERT(spinesParams_importance >= 0.0f && spinesParams_importance <= 1.0f);
    postAssignSpinesParams(spinesParams[index]);
    m_spinesParamsImportance[index] = spinesParams_importance;
  }
  NM_INLINE WritheParameters& startSpinesParamsModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxSpinesParams);
    m_spinesParamsImportance[index] = -1.0f; // set invalid until stopSpinesParamsModificationAt()
    return spinesParams[index];
  }
  NM_INLINE void stopSpinesParamsModificationAt(unsigned int index, float spinesParams_importance = 1.f)
  {
    postAssignSpinesParams(spinesParams[index]);
    NMP_ASSERT(spinesParams_importance >= 0.0f && spinesParams_importance <= 1.0f);
    m_spinesParamsImportance[index] = spinesParams_importance;
  }
  NM_INLINE float getSpinesParamsImportance(int index) const { return m_spinesParamsImportance[index]; }
  NM_INLINE const float& getSpinesParamsImportanceRef(int index) const { return m_spinesParamsImportance[index]; }
  NM_INLINE unsigned int calculateNumSpinesParams() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spinesParamsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const WritheParameters& getSpinesParams(unsigned int index) const { NMP_ASSERT(index <= maxSpinesParams); return spinesParams[index]; }

  NM_INLINE void setRandomLookParameters(const RandomLookParameters& _randomLookParameters, float randomLookParameters_importance = 1.f)
  {
    randomLookParameters = _randomLookParameters;
    postAssignRandomLookParameters(randomLookParameters);
    NMP_ASSERT(randomLookParameters_importance >= 0.0f && randomLookParameters_importance <= 1.0f);
    m_randomLookParametersImportance = randomLookParameters_importance;
  }
  NM_INLINE float getRandomLookParametersImportance() const { return m_randomLookParametersImportance; }
  NM_INLINE const float& getRandomLookParametersImportanceRef() const { return m_randomLookParametersImportance; }
  NM_INLINE const RandomLookParameters& getRandomLookParameters() const { return randomLookParameters; }

protected:

  PoseData headPose[NetworkConstants::networkMaxNumHeads];
  WritheParameters armsParams[NetworkConstants::networkMaxNumArms];
  WritheParameters legsParams[NetworkConstants::networkMaxNumLegs];
  WritheParameters spinesParams[NetworkConstants::networkMaxNumSpines];
  RandomLookParameters randomLookParameters;

  // post-assignment stubs
  NM_INLINE void postAssignHeadPose(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmsParams(const WritheParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegsParams(const WritheParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinesParams(const WritheParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignRandomLookParameters(const RandomLookParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_headPoseImportance[NetworkConstants::networkMaxNumHeads], 
     m_armsParamsImportance[NetworkConstants::networkMaxNumArms], 
     m_legsParamsImportance[NetworkConstants::networkMaxNumLegs], 
     m_spinesParamsImportance[NetworkConstants::networkMaxNumSpines], 
     m_randomLookParametersImportance;


public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numHeadPose = calculateNumHeadPose();
      for (uint32_t i=0; i<numHeadPose; i++)
      {
        if (getHeadPoseImportance(i) > 0.0f)
          headPose[i].validate();
      }
    }
    {
      uint32_t numArmsParams = calculateNumArmsParams();
      for (uint32_t i=0; i<numArmsParams; i++)
      {
        if (getArmsParamsImportance(i) > 0.0f)
          armsParams[i].validate();
      }
    }
    {
      uint32_t numLegsParams = calculateNumLegsParams();
      for (uint32_t i=0; i<numLegsParams; i++)
      {
        if (getLegsParamsImportance(i) > 0.0f)
          legsParams[i].validate();
      }
    }
    {
      uint32_t numSpinesParams = calculateNumSpinesParams();
      for (uint32_t i=0; i<numSpinesParams; i++)
      {
        if (getSpinesParamsImportance(i) > 0.0f)
          spinesParams[i].validate();
      }
    }
    if (getRandomLookParametersImportance() > 0.0f)
    {
      randomLookParameters.validate();
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_WRITHEBEHAVIOURINTERFACE_DATA_H

