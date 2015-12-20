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

#ifndef NM_MDF_REACHFORBODYBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_REACHFORBODYBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ReachForBodyBehaviourInterface.module"

// external types
#include "NMPlatform/NMMatrix34.h"

// known types
#include "Types/BodyHitInfo.h"
#include "Types/ReachActionParams.h"

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
 * \class ReachForBodyBehaviourInterfaceData
 * \ingroup ReachForBodyBehaviourInterface
 * \brief Data Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (267 bytes, 288 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ReachForBodyBehaviourInterfaceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ReachForBodyBehaviourInterfaceData));
  }

  BodyHitInfo lastBodyHitInfo[NetworkConstants::networkMaxNumArms];  // hit info is stored, maintained by the module and made available as output for other modules as a BodyHitInfo
  ReachActionParams reachActionParams[NetworkConstants::networkMaxNumArms];
  bool armReachForBodyPartFlags[NetworkConstants::networkMaxNumArms];
  bool isReacting;  // behaviour state flags

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    lastBodyHitInfo[0].validate();
    lastBodyHitInfo[1].validate();
    reachActionParams[0].validate();
    reachActionParams[1].validate();
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ReachForBodyBehaviourInterfaceOutputs
 * \ingroup ReachForBodyBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (426 bytes, 448 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ReachForBodyBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ReachForBodyBehaviourInterfaceOutputs));
  }

  enum { maxBodyHitInfo = 2 };
  NM_INLINE unsigned int getMaxBodyHitInfo() const { return maxBodyHitInfo; }
  NM_INLINE void setBodyHitInfo(unsigned int number, const BodyHitInfo* _bodyHitInfo, float bodyHitInfo_importance = 1.f)
  {
    NMP_ASSERT(number <= maxBodyHitInfo);
    NMP_ASSERT(bodyHitInfo_importance >= 0.0f && bodyHitInfo_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      bodyHitInfo[i] = _bodyHitInfo[i];
      postAssignBodyHitInfo(bodyHitInfo[i]);
      m_bodyHitInfoImportance[i] = bodyHitInfo_importance;
    }
  }
  NM_INLINE void setBodyHitInfoAt(unsigned int index, const BodyHitInfo& _bodyHitInfo, float bodyHitInfo_importance = 1.f)
  {
    NMP_ASSERT(index < maxBodyHitInfo);
    bodyHitInfo[index] = _bodyHitInfo;
    NMP_ASSERT(bodyHitInfo_importance >= 0.0f && bodyHitInfo_importance <= 1.0f);
    postAssignBodyHitInfo(bodyHitInfo[index]);
    m_bodyHitInfoImportance[index] = bodyHitInfo_importance;
  }
  NM_INLINE BodyHitInfo& startBodyHitInfoModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxBodyHitInfo);
    m_bodyHitInfoImportance[index] = -1.0f; // set invalid until stopBodyHitInfoModificationAt()
    return bodyHitInfo[index];
  }
  NM_INLINE void stopBodyHitInfoModificationAt(unsigned int index, float bodyHitInfo_importance = 1.f)
  {
    postAssignBodyHitInfo(bodyHitInfo[index]);
    NMP_ASSERT(bodyHitInfo_importance >= 0.0f && bodyHitInfo_importance <= 1.0f);
    m_bodyHitInfoImportance[index] = bodyHitInfo_importance;
  }
  NM_INLINE float getBodyHitInfoImportance(int index) const { return m_bodyHitInfoImportance[index]; }
  NM_INLINE const float& getBodyHitInfoImportanceRef(int index) const { return m_bodyHitInfoImportance[index]; }
  NM_INLINE unsigned int calculateNumBodyHitInfo() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_bodyHitInfoImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const BodyHitInfo& getBodyHitInfo(unsigned int index) const { NMP_ASSERT(index <= maxBodyHitInfo); return bodyHitInfo[index]; }

  enum { maxArmHitPoseEndRelRoot = 2 };
  NM_INLINE unsigned int getMaxArmHitPoseEndRelRoot() const { return maxArmHitPoseEndRelRoot; }
  NM_INLINE void setArmHitPoseEndRelRoot(unsigned int number, const NMP::Matrix34* _armHitPoseEndRelRoot, float armHitPoseEndRelRoot_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmHitPoseEndRelRoot);
    NMP_ASSERT(armHitPoseEndRelRoot_importance >= 0.0f && armHitPoseEndRelRoot_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armHitPoseEndRelRoot[i] = _armHitPoseEndRelRoot[i];
      postAssignArmHitPoseEndRelRoot(armHitPoseEndRelRoot[i]);
      m_armHitPoseEndRelRootImportance[i] = armHitPoseEndRelRoot_importance;
    }
  }
  NM_INLINE void setArmHitPoseEndRelRootAt(unsigned int index, const NMP::Matrix34& _armHitPoseEndRelRoot, float armHitPoseEndRelRoot_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmHitPoseEndRelRoot);
    armHitPoseEndRelRoot[index] = _armHitPoseEndRelRoot;
    NMP_ASSERT(armHitPoseEndRelRoot_importance >= 0.0f && armHitPoseEndRelRoot_importance <= 1.0f);
    postAssignArmHitPoseEndRelRoot(armHitPoseEndRelRoot[index]);
    m_armHitPoseEndRelRootImportance[index] = armHitPoseEndRelRoot_importance;
  }
  NM_INLINE float getArmHitPoseEndRelRootImportance(int index) const { return m_armHitPoseEndRelRootImportance[index]; }
  NM_INLINE const float& getArmHitPoseEndRelRootImportanceRef(int index) const { return m_armHitPoseEndRelRootImportance[index]; }
  NM_INLINE unsigned int calculateNumArmHitPoseEndRelRoot() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armHitPoseEndRelRootImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Matrix34& getArmHitPoseEndRelRoot(unsigned int index) const { NMP_ASSERT(index <= maxArmHitPoseEndRelRoot); return armHitPoseEndRelRoot[index]; }

  enum { maxReachActionParams = 2 };
  NM_INLINE unsigned int getMaxReachActionParams() const { return maxReachActionParams; }
  NM_INLINE void setReachActionParams(unsigned int number, const ReachActionParams* _reachActionParams, float reachActionParams_importance = 1.f)
  {
    NMP_ASSERT(number <= maxReachActionParams);
    NMP_ASSERT(reachActionParams_importance >= 0.0f && reachActionParams_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      reachActionParams[i] = _reachActionParams[i];
      postAssignReachActionParams(reachActionParams[i]);
      m_reachActionParamsImportance[i] = reachActionParams_importance;
    }
  }
  NM_INLINE void setReachActionParamsAt(unsigned int index, const ReachActionParams& _reachActionParams, float reachActionParams_importance = 1.f)
  {
    NMP_ASSERT(index < maxReachActionParams);
    reachActionParams[index] = _reachActionParams;
    NMP_ASSERT(reachActionParams_importance >= 0.0f && reachActionParams_importance <= 1.0f);
    postAssignReachActionParams(reachActionParams[index]);
    m_reachActionParamsImportance[index] = reachActionParams_importance;
  }
  NM_INLINE float getReachActionParamsImportance(int index) const { return m_reachActionParamsImportance[index]; }
  NM_INLINE const float& getReachActionParamsImportanceRef(int index) const { return m_reachActionParamsImportance[index]; }
  NM_INLINE unsigned int calculateNumReachActionParams() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_reachActionParamsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const ReachActionParams& getReachActionParams(unsigned int index) const { NMP_ASSERT(index <= maxReachActionParams); return reachActionParams[index]; }

  enum { maxArmReachForBodyPartFlags = 2 };
  NM_INLINE unsigned int getMaxArmReachForBodyPartFlags() const { return maxArmReachForBodyPartFlags; }
  NM_INLINE void setArmReachForBodyPartFlags(unsigned int number, const bool* _armReachForBodyPartFlags, float armReachForBodyPartFlags_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmReachForBodyPartFlags);
    NMP_ASSERT(armReachForBodyPartFlags_importance >= 0.0f && armReachForBodyPartFlags_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armReachForBodyPartFlags[i] = _armReachForBodyPartFlags[i];
      m_armReachForBodyPartFlagsImportance[i] = armReachForBodyPartFlags_importance;
    }
  }
  NM_INLINE void setArmReachForBodyPartFlagsAt(unsigned int index, const bool& _armReachForBodyPartFlags, float armReachForBodyPartFlags_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmReachForBodyPartFlags);
    armReachForBodyPartFlags[index] = _armReachForBodyPartFlags;
    NMP_ASSERT(armReachForBodyPartFlags_importance >= 0.0f && armReachForBodyPartFlags_importance <= 1.0f);
    m_armReachForBodyPartFlagsImportance[index] = armReachForBodyPartFlags_importance;
  }
  NM_INLINE float getArmReachForBodyPartFlagsImportance(int index) const { return m_armReachForBodyPartFlagsImportance[index]; }
  NM_INLINE const float& getArmReachForBodyPartFlagsImportanceRef(int index) const { return m_armReachForBodyPartFlagsImportance[index]; }
  NM_INLINE unsigned int calculateNumArmReachForBodyPartFlags() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armReachForBodyPartFlagsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const bool& getArmReachForBodyPartFlags(unsigned int index) const { NMP_ASSERT(index <= maxArmReachForBodyPartFlags); return armReachForBodyPartFlags[index]; }

protected:

  BodyHitInfo bodyHitInfo[NetworkConstants::networkMaxNumArms];  // info required for reach reaction
  NMP::Matrix34 armHitPoseEndRelRoot[NetworkConstants::networkMaxNumArms];  // Transform matrix of the end of the limb relative to root space. Used to pose the arm when hit  (Transform)
  ReachActionParams reachActionParams[NetworkConstants::networkMaxNumArms];
  bool armReachForBodyPartFlags[NetworkConstants::networkMaxNumArms];  // set of flags used to tell the network which arms are available for reaching

  // post-assignment stubs
  NM_INLINE void postAssignBodyHitInfo(const BodyHitInfo& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmHitPoseEndRelRoot(const NMP::Matrix34& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Matrix34Orthonormal(v), "armHitPoseEndRelRoot");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignReachActionParams(const ReachActionParams& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_bodyHitInfoImportance[NetworkConstants::networkMaxNumArms], 
     m_armHitPoseEndRelRootImportance[NetworkConstants::networkMaxNumArms], 
     m_reachActionParamsImportance[NetworkConstants::networkMaxNumArms], 
     m_armReachForBodyPartFlagsImportance[NetworkConstants::networkMaxNumArms];

  friend class ReachForBodyBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numBodyHitInfo = calculateNumBodyHitInfo();
      for (uint32_t i=0; i<numBodyHitInfo; i++)
      {
        if (getBodyHitInfoImportance(i) > 0.0f)
          bodyHitInfo[i].validate();
      }
    }
    {
      uint32_t numArmHitPoseEndRelRoot = calculateNumArmHitPoseEndRelRoot();
      for (uint32_t i=0; i<numArmHitPoseEndRelRoot; i++)
      {
        if (getArmHitPoseEndRelRootImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Matrix34Orthonormal(armHitPoseEndRelRoot[i]), "armHitPoseEndRelRoot");
        }
      }
    }
    {
      uint32_t numReachActionParams = calculateNumReachActionParams();
      for (uint32_t i=0; i<numReachActionParams; i++)
      {
        if (getReachActionParamsImportance(i) > 0.0f)
          reachActionParams[i].validate();
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ReachForBodyBehaviourInterfaceFeedbackInputs
 * \ingroup ReachForBodyBehaviourInterface
 * \brief FeedbackInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (5 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ReachForBodyBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ReachForBodyBehaviourInterfaceFeedbackInputs));
  }

  NM_INLINE void setIsComplete(const bool& _isComplete, float isComplete_importance = 1.f)
  {
    isComplete = _isComplete;
    NMP_ASSERT(isComplete_importance >= 0.0f && isComplete_importance <= 1.0f);
    m_isCompleteImportance = isComplete_importance;
  }
  NM_INLINE float getIsCompleteImportance() const { return m_isCompleteImportance; }
  NM_INLINE const float& getIsCompleteImportanceRef() const { return m_isCompleteImportance; }
  NM_INLINE const bool& getIsComplete() const { return isComplete; }

protected:

  bool isComplete;

  // importance values
  float
     m_isCompleteImportance;

  friend class ReachForBodyBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_REACHFORBODYBEHAVIOURINTERFACE_DATA_H

