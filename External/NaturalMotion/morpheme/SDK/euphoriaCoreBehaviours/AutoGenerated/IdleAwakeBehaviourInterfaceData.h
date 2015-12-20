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

#ifndef NM_MDF_IDLEAWAKEBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_IDLEAWAKEBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/IdleAwakeBehaviourInterface.module"

// known types
#include "Types/ModuleRNG.h"
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
 * \class IdleAwakeBehaviourInterfaceData
 * \ingroup IdleAwakeBehaviourInterface
 * \brief Data IdleAwake attempts to make the character seem less uncounscious/robotic by adding some variation
 *  to existing capabilities. This includes:
 *  * Perturbing the balance slightly so the character shifts his weight
 *  * Choosing random, low importance look targets
 *  * If he's fallen over then attempt to get into a suitable on-ground pose (using animation
 *    inputs)
 *
 * Data packet definition (100 bytes, 128 aligned)
 */
NMP_ALIGN_PREFIX(32) struct IdleAwakeBehaviourInterfaceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(IdleAwakeBehaviourInterfaceData));
  }

  ModuleRNG rng;
  float balanceOffsetFwd;  /// For random drift of the balance  (Distance)
  float balanceOffsetRight;                  ///< (Distance)
  float balanceOffsetFwdRate;                ///< (Speed)
  float balanceOffsetRightRate;              ///< (Speed)
  float balanceOffsetFwdTarget;              ///< (Distance)
  float balanceOffsetRightTarget;            ///< (Distance)
  float timeSinceLastOffsetTarget;           ///< (TimePeriod)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(balanceOffsetFwd), "balanceOffsetFwd");
    NM_VALIDATOR(FloatValid(balanceOffsetRight), "balanceOffsetRight");
    NM_VALIDATOR(FloatValid(balanceOffsetFwdRate), "balanceOffsetFwdRate");
    NM_VALIDATOR(FloatValid(balanceOffsetRightRate), "balanceOffsetRightRate");
    NM_VALIDATOR(FloatValid(balanceOffsetFwdTarget), "balanceOffsetFwdTarget");
    NM_VALIDATOR(FloatValid(balanceOffsetRightTarget), "balanceOffsetRightTarget");
    NM_VALIDATOR(FloatValid(timeSinceLastOffsetTarget), "timeSinceLastOffsetTarget");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class IdleAwakeBehaviourInterfaceOutputs
 * \ingroup IdleAwakeBehaviourInterface
 * \brief Outputs IdleAwake attempts to make the character seem less uncounscious/robotic by adding some variation
 *  to existing capabilities. This includes:
 *  * Perturbing the balance slightly so the character shifts his weight
 *  * Choosing random, low importance look targets
 *  * If he's fallen over then attempt to get into a suitable on-ground pose (using animation
 *    inputs)
 *
 * Data packet definition (852 bytes, 864 aligned)
 */
NMP_ALIGN_PREFIX(32) struct IdleAwakeBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(IdleAwakeBehaviourInterfaceOutputs));
  }

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

  enum { maxLegPose = 2 };
  NM_INLINE unsigned int getMaxLegPose() const { return maxLegPose; }
  NM_INLINE void setLegPose(unsigned int number, const PoseData* _legPose, float legPose_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegPose);
    NMP_ASSERT(legPose_importance >= 0.0f && legPose_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legPose[i] = _legPose[i];
      postAssignLegPose(legPose[i]);
      m_legPoseImportance[i] = legPose_importance;
    }
  }
  NM_INLINE void setLegPoseAt(unsigned int index, const PoseData& _legPose, float legPose_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegPose);
    legPose[index] = _legPose;
    NMP_ASSERT(legPose_importance >= 0.0f && legPose_importance <= 1.0f);
    postAssignLegPose(legPose[index]);
    m_legPoseImportance[index] = legPose_importance;
  }
  NM_INLINE PoseData& startLegPoseModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxLegPose);
    m_legPoseImportance[index] = -1.0f; // set invalid until stopLegPoseModificationAt()
    return legPose[index];
  }
  NM_INLINE void stopLegPoseModificationAt(unsigned int index, float legPose_importance = 1.f)
  {
    postAssignLegPose(legPose[index]);
    NMP_ASSERT(legPose_importance >= 0.0f && legPose_importance <= 1.0f);
    m_legPoseImportance[index] = legPose_importance;
  }
  NM_INLINE float getLegPoseImportance(int index) const { return m_legPoseImportance[index]; }
  NM_INLINE const float& getLegPoseImportanceRef(int index) const { return m_legPoseImportance[index]; }
  NM_INLINE unsigned int calculateNumLegPose() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legPoseImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getLegPose(unsigned int index) const { NMP_ASSERT(index <= maxLegPose); return legPose[index]; }

  enum { maxSpinePose = 1 };
  NM_INLINE unsigned int getMaxSpinePose() const { return maxSpinePose; }
  NM_INLINE void setSpinePose(unsigned int number, const PoseData* _spinePose, float spinePose_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpinePose);
    NMP_ASSERT(spinePose_importance >= 0.0f && spinePose_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spinePose[i] = _spinePose[i];
      postAssignSpinePose(spinePose[i]);
      m_spinePoseImportance[i] = spinePose_importance;
    }
  }
  NM_INLINE void setSpinePoseAt(unsigned int index, const PoseData& _spinePose, float spinePose_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpinePose);
    spinePose[index] = _spinePose;
    NMP_ASSERT(spinePose_importance >= 0.0f && spinePose_importance <= 1.0f);
    postAssignSpinePose(spinePose[index]);
    m_spinePoseImportance[index] = spinePose_importance;
  }
  NM_INLINE PoseData& startSpinePoseModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxSpinePose);
    m_spinePoseImportance[index] = -1.0f; // set invalid until stopSpinePoseModificationAt()
    return spinePose[index];
  }
  NM_INLINE void stopSpinePoseModificationAt(unsigned int index, float spinePose_importance = 1.f)
  {
    postAssignSpinePose(spinePose[index]);
    NMP_ASSERT(spinePose_importance >= 0.0f && spinePose_importance <= 1.0f);
    m_spinePoseImportance[index] = spinePose_importance;
  }
  NM_INLINE float getSpinePoseImportance(int index) const { return m_spinePoseImportance[index]; }
  NM_INLINE const float& getSpinePoseImportanceRef(int index) const { return m_spinePoseImportance[index]; }
  NM_INLINE unsigned int calculateNumSpinePose() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spinePoseImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const PoseData& getSpinePose(unsigned int index) const { NMP_ASSERT(index <= maxSpinePose); return spinePose[index]; }

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

  NM_INLINE void setUseSpineDefaultPose(const float& _useSpineDefaultPose, float useSpineDefaultPose_importance = 1.f)
  {
    useSpineDefaultPose = _useSpineDefaultPose;
    postAssignUseSpineDefaultPose(useSpineDefaultPose);
    NMP_ASSERT(useSpineDefaultPose_importance >= 0.0f && useSpineDefaultPose_importance <= 1.0f);
    m_useSpineDefaultPoseImportance = useSpineDefaultPose_importance;
  }
  NM_INLINE float getUseSpineDefaultPoseImportance() const { return m_useSpineDefaultPoseImportance; }
  NM_INLINE const float& getUseSpineDefaultPoseImportanceRef() const { return m_useSpineDefaultPoseImportance; }
  NM_INLINE const float& getUseSpineDefaultPose() const { return useSpineDefaultPose; }

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

protected:

  PoseData armPose[NetworkConstants::networkMaxNumArms];  // Poses for each of the limb ends relative to their root
  PoseData headPose[NetworkConstants::networkMaxNumHeads];
  PoseData legPose[NetworkConstants::networkMaxNumLegs];
  PoseData spinePose[NetworkConstants::networkMaxNumSpines];
  RandomLookParameters randomLookParameters;
  float useSpineDefaultPose;                 ///< (Weight)
  float balanceOffsetFwd;  // Desired offset for the balance so it doesn't always balance perfectly over the CoM  (Distance)
  float balanceOffsetRight;                  ///< (Distance)

  // post-assignment stubs
  NM_INLINE void postAssignArmPose(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHeadPose(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegPose(const PoseData& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSpinePose(const PoseData& NMP_VALIDATOR_ARG(v)) const
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

  NM_INLINE void postAssignUseSpineDefaultPose(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "useSpineDefaultPose");
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

  // importance values
  float
     m_armPoseImportance[NetworkConstants::networkMaxNumArms], 
     m_headPoseImportance[NetworkConstants::networkMaxNumHeads], 
     m_legPoseImportance[NetworkConstants::networkMaxNumLegs], 
     m_spinePoseImportance[NetworkConstants::networkMaxNumSpines], 
     m_randomLookParametersImportance, 
     m_useSpineDefaultPoseImportance, 
     m_balanceOffsetFwdImportance, 
     m_balanceOffsetRightImportance;

  friend class IdleAwakeBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numArmPose = calculateNumArmPose();
      for (uint32_t i=0; i<numArmPose; i++)
      {
        if (getArmPoseImportance(i) > 0.0f)
          armPose[i].validate();
      }
    }
    {
      uint32_t numHeadPose = calculateNumHeadPose();
      for (uint32_t i=0; i<numHeadPose; i++)
      {
        if (getHeadPoseImportance(i) > 0.0f)
          headPose[i].validate();
      }
    }
    {
      uint32_t numLegPose = calculateNumLegPose();
      for (uint32_t i=0; i<numLegPose; i++)
      {
        if (getLegPoseImportance(i) > 0.0f)
          legPose[i].validate();
      }
    }
    {
      uint32_t numSpinePose = calculateNumSpinePose();
      for (uint32_t i=0; i<numSpinePose; i++)
      {
        if (getSpinePoseImportance(i) > 0.0f)
          spinePose[i].validate();
      }
    }
    if (getRandomLookParametersImportance() > 0.0f)
    {
      randomLookParameters.validate();
    }
    if (getUseSpineDefaultPoseImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(useSpineDefaultPose), "useSpineDefaultPose");
    }
    if (getBalanceOffsetFwdImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(balanceOffsetFwd), "balanceOffsetFwd");
    }
    if (getBalanceOffsetRightImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(balanceOffsetRight), "balanceOffsetRight");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class IdleAwakeBehaviourInterfaceFeedbackInputs
 * \ingroup IdleAwakeBehaviourInterface
 * \brief FeedbackInputs IdleAwake attempts to make the character seem less uncounscious/robotic by adding some variation
 *  to existing capabilities. This includes:
 *  * Perturbing the balance slightly so the character shifts his weight
 *  * Choosing random, low importance look targets
 *  * If he's fallen over then attempt to get into a suitable on-ground pose (using animation
 *    inputs)
 *
 * Data packet definition (41 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct IdleAwakeBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(IdleAwakeBehaviourInterfaceFeedbackInputs));
  }

  NM_INLINE void setSupportAmount(const float& _supportAmount, float supportAmount_importance = 1.f)
  {
    supportAmount = _supportAmount;
    postAssignSupportAmount(supportAmount);
    NMP_ASSERT(supportAmount_importance >= 0.0f && supportAmount_importance <= 1.0f);
    m_supportAmountImportance = supportAmount_importance;
  }
  NM_INLINE float getSupportAmountImportance() const { return m_supportAmountImportance; }
  NM_INLINE const float& getSupportAmountImportanceRef() const { return m_supportAmountImportance; }
  NM_INLINE const float& getSupportAmount() const { return supportAmount; }

  NM_INLINE void setArmOrLegSupportAmount(const float& _armOrLegSupportAmount, float armOrLegSupportAmount_importance = 1.f)
  {
    armOrLegSupportAmount = _armOrLegSupportAmount;
    postAssignArmOrLegSupportAmount(armOrLegSupportAmount);
    NMP_ASSERT(armOrLegSupportAmount_importance >= 0.0f && armOrLegSupportAmount_importance <= 1.0f);
    m_armOrLegSupportAmountImportance = armOrLegSupportAmount_importance;
  }
  NM_INLINE float getArmOrLegSupportAmountImportance() const { return m_armOrLegSupportAmountImportance; }
  NM_INLINE const float& getArmOrLegSupportAmountImportanceRef() const { return m_armOrLegSupportAmountImportance; }
  NM_INLINE const float& getArmOrLegSupportAmount() const { return armOrLegSupportAmount; }

  NM_INLINE void setSpineInContact(const bool& _spineInContact, float spineInContact_importance = 1.f)
  {
    spineInContact = _spineInContact;
    NMP_ASSERT(spineInContact_importance >= 0.0f && spineInContact_importance <= 1.0f);
    m_spineInContactImportance = spineInContact_importance;
  }
  NM_INLINE float getSpineInContactImportance() const { return m_spineInContactImportance; }
  NM_INLINE const float& getSpineInContactImportanceRef() const { return m_spineInContactImportance; }
  NM_INLINE const bool& getSpineInContact() const { return spineInContact; }

  NM_INLINE void setLegsInContact(const bool& _legsInContact, float legsInContact_importance = 1.f)
  {
    legsInContact = _legsInContact;
    NMP_ASSERT(legsInContact_importance >= 0.0f && legsInContact_importance <= 1.0f);
    m_legsInContactImportance = legsInContact_importance;
  }
  NM_INLINE float getLegsInContactImportance() const { return m_legsInContactImportance; }
  NM_INLINE const float& getLegsInContactImportanceRef() const { return m_legsInContactImportance; }
  NM_INLINE const bool& getLegsInContact() const { return legsInContact; }

  NM_INLINE void setHeadsInContact(const bool& _headsInContact, float headsInContact_importance = 1.f)
  {
    headsInContact = _headsInContact;
    NMP_ASSERT(headsInContact_importance >= 0.0f && headsInContact_importance <= 1.0f);
    m_headsInContactImportance = headsInContact_importance;
  }
  NM_INLINE float getHeadsInContactImportance() const { return m_headsInContactImportance; }
  NM_INLINE const float& getHeadsInContactImportanceRef() const { return m_headsInContactImportance; }
  NM_INLINE const bool& getHeadsInContact() const { return headsInContact; }

  NM_INLINE void setArmsInContact(const bool& _armsInContact, float armsInContact_importance = 1.f)
  {
    armsInContact = _armsInContact;
    NMP_ASSERT(armsInContact_importance >= 0.0f && armsInContact_importance <= 1.0f);
    m_armsInContactImportance = armsInContact_importance;
  }
  NM_INLINE float getArmsInContactImportance() const { return m_armsInContactImportance; }
  NM_INLINE const float& getArmsInContactImportanceRef() const { return m_armsInContactImportance; }
  NM_INLINE const bool& getArmsInContact() const { return armsInContact; }

  NM_INLINE void setIsStepping(const bool& _isStepping, float isStepping_importance = 1.f)
  {
    isStepping = _isStepping;
    NMP_ASSERT(isStepping_importance >= 0.0f && isStepping_importance <= 1.0f);
    m_isSteppingImportance = isStepping_importance;
  }
  NM_INLINE float getIsSteppingImportance() const { return m_isSteppingImportance; }
  NM_INLINE const float& getIsSteppingImportanceRef() const { return m_isSteppingImportance; }
  NM_INLINE const bool& getIsStepping() const { return isStepping; }

protected:

  float supportAmount;                       ///< (Weight)
  float armOrLegSupportAmount;               ///< (Weight)
  bool spineInContact;
  bool legsInContact;
  bool headsInContact;
  bool armsInContact;
  bool isStepping;

  // post-assignment stubs
  NM_INLINE void postAssignSupportAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "supportAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmOrLegSupportAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "armOrLegSupportAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_supportAmountImportance, 
     m_armOrLegSupportAmountImportance, 
     m_spineInContactImportance, 
     m_legsInContactImportance, 
     m_headsInContactImportance, 
     m_armsInContactImportance, 
     m_isSteppingImportance;

  friend class IdleAwakeBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(supportAmount), "supportAmount");
    }
    if (getArmOrLegSupportAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(armOrLegSupportAmount), "armOrLegSupportAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_IDLEAWAKEBEHAVIOURINTERFACE_DATA_H

