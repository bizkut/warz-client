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

#ifndef NM_MDF_SHIELDBEHAVIOURINTERFACE_DATA_H
#define NM_MDF_SHIELDBEHAVIOURINTERFACE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ShieldBehaviourInterface.module"

// known types
#include "Types/PoseData.h"
#include "Types/ShieldParameters.h"
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
 * \class ShieldBehaviourInterfaceData
 * \ingroup ShieldBehaviourInterface
 * \brief Data Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (792 bytes, 800 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ShieldBehaviourInterfaceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ShieldBehaviourInterfaceData));
  }

  PoseData shieldPose[NetworkConstants::networkMaxNumLimbs];  // Following are only used if the behaviour param UseSingleFrameForShieldPoses is set to true
  float shieldPoseImportance[NetworkConstants::networkMaxNumLimbs];  ///< (Weight)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    shieldPose[0].validate();
    shieldPose[1].validate();
    shieldPose[2].validate();
    shieldPose[3].validate();
    shieldPose[4].validate();
    shieldPose[5].validate();
    NM_VALIDATOR(FloatNonNegative(shieldPoseImportance[0]), "shieldPoseImportance[0]");
    NM_VALIDATOR(FloatNonNegative(shieldPoseImportance[1]), "shieldPoseImportance[1]");
    NM_VALIDATOR(FloatNonNegative(shieldPoseImportance[2]), "shieldPoseImportance[2]");
    NM_VALIDATOR(FloatNonNegative(shieldPoseImportance[3]), "shieldPoseImportance[3]");
    NM_VALIDATOR(FloatNonNegative(shieldPoseImportance[4]), "shieldPoseImportance[4]");
    NM_VALIDATOR(FloatNonNegative(shieldPoseImportance[5]), "shieldPoseImportance[5]");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ShieldBehaviourInterfaceOutputs
 * \ingroup ShieldBehaviourInterface
 * \brief Outputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (876 bytes, 896 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ShieldBehaviourInterfaceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ShieldBehaviourInterfaceOutputs));
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

  NM_INLINE void setShieldParameters(const ShieldParameters& _shieldParameters, float shieldParameters_importance = 1.f)
  {
    shieldParameters = _shieldParameters;
    postAssignShieldParameters(shieldParameters);
    NMP_ASSERT(shieldParameters_importance >= 0.0f && shieldParameters_importance <= 1.0f);
    m_shieldParametersImportance = shieldParameters_importance;
  }
  NM_INLINE float getShieldParametersImportance() const { return m_shieldParametersImportance; }
  NM_INLINE const float& getShieldParametersImportanceRef() const { return m_shieldParametersImportance; }
  NM_INLINE const ShieldParameters& getShieldParameters() const { return shieldParameters; }

  NM_INLINE void setSmoothReturnTP(const float& _smoothReturnTP, float smoothReturnTP_importance = 1.f)
  {
    smoothReturnTP = _smoothReturnTP;
    postAssignSmoothReturnTP(smoothReturnTP);
    NMP_ASSERT(smoothReturnTP_importance >= 0.0f && smoothReturnTP_importance <= 1.0f);
    m_smoothReturnTPImportance = smoothReturnTP_importance;
  }
  NM_INLINE float getSmoothReturnTPImportance() const { return m_smoothReturnTPImportance; }
  NM_INLINE const float& getSmoothReturnTPImportanceRef() const { return m_smoothReturnTPImportance; }
  NM_INLINE const float& getSmoothReturnTP() const { return smoothReturnTP; }

  NM_INLINE void setTurnAwayScale(const float& _turnAwayScale, float turnAwayScale_importance = 1.f)
  {
    turnAwayScale = _turnAwayScale;
    postAssignTurnAwayScale(turnAwayScale);
    NMP_ASSERT(turnAwayScale_importance >= 0.0f && turnAwayScale_importance <= 1.0f);
    m_turnAwayScaleImportance = turnAwayScale_importance;
  }
  NM_INLINE float getTurnAwayScaleImportance() const { return m_turnAwayScaleImportance; }
  NM_INLINE const float& getTurnAwayScaleImportanceRef() const { return m_turnAwayScaleImportance; }
  NM_INLINE const float& getTurnAwayScale() const { return turnAwayScale; }

protected:

  PoseData armPose[NetworkConstants::networkMaxNumArms];
  PoseData headPose[NetworkConstants::networkMaxNumHeads];
  PoseData spinePose[NetworkConstants::networkMaxNumSpines];
  PoseData legPose[NetworkConstants::networkMaxNumLegs];
  BalancePoseParameters poseParameters;
  ShieldParameters shieldParameters;
  float smoothReturnTP;  ///< Minimum time period character will do a shield.  (TimePeriod)
  float turnAwayScale;  ///< Scale used to set the amount of rotation to turn away from the hazard (0 - 1).  (Multiplier)

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

  NM_INLINE void postAssignSpinePose(const PoseData& NMP_VALIDATOR_ARG(v)) const
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

  NM_INLINE void postAssignPoseParameters(const BalancePoseParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignShieldParameters(const ShieldParameters& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSmoothReturnTP(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "smoothReturnTP");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignTurnAwayScale(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "turnAwayScale");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_armPoseImportance[NetworkConstants::networkMaxNumArms], 
     m_headPoseImportance[NetworkConstants::networkMaxNumHeads], 
     m_spinePoseImportance[NetworkConstants::networkMaxNumSpines], 
     m_legPoseImportance[NetworkConstants::networkMaxNumLegs], 
     m_poseParametersImportance, 
     m_shieldParametersImportance, 
     m_smoothReturnTPImportance, 
     m_turnAwayScaleImportance;

  friend class ShieldBehaviourInterface_Con;

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
      uint32_t numSpinePose = calculateNumSpinePose();
      for (uint32_t i=0; i<numSpinePose; i++)
      {
        if (getSpinePoseImportance(i) > 0.0f)
          spinePose[i].validate();
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
    if (getPoseParametersImportance() > 0.0f)
    {
      poseParameters.validate();
    }
    if (getShieldParametersImportance() > 0.0f)
    {
      shieldParameters.validate();
    }
    if (getSmoothReturnTPImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(smoothReturnTP), "smoothReturnTP");
    }
    if (getTurnAwayScaleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(turnAwayScale), "turnAwayScale");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ShieldBehaviourInterfaceFeedbackInputs
 * \ingroup ShieldBehaviourInterface
 * \brief FeedbackInputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (45 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ShieldBehaviourInterfaceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ShieldBehaviourInterfaceFeedbackInputs));
  }

  NM_INLINE void setYawAngle(const float& _yawAngle, float yawAngle_importance = 1.f)
  {
    yawAngle = _yawAngle;
    postAssignYawAngle(yawAngle);
    NMP_ASSERT(yawAngle_importance >= 0.0f && yawAngle_importance <= 1.0f);
    m_yawAngleImportance = yawAngle_importance;
  }
  NM_INLINE float getYawAngleImportance() const { return m_yawAngleImportance; }
  NM_INLINE const float& getYawAngleImportanceRef() const { return m_yawAngleImportance; }
  NM_INLINE const float& getYawAngle() const { return yawAngle; }

  NM_INLINE void setHazardAngle(const float& _hazardAngle, float hazardAngle_importance = 1.f)
  {
    hazardAngle = _hazardAngle;
    postAssignHazardAngle(hazardAngle);
    NMP_ASSERT(hazardAngle_importance >= 0.0f && hazardAngle_importance <= 1.0f);
    m_hazardAngleImportance = hazardAngle_importance;
  }
  NM_INLINE float getHazardAngleImportance() const { return m_hazardAngleImportance; }
  NM_INLINE const float& getHazardAngleImportanceRef() const { return m_hazardAngleImportance; }
  NM_INLINE const float& getHazardAngle() const { return hazardAngle; }

  NM_INLINE void setDoShieldWithUpperBody(const float& _doShieldWithUpperBody, float doShieldWithUpperBody_importance = 1.f)
  {
    doShieldWithUpperBody = _doShieldWithUpperBody;
    postAssignDoShieldWithUpperBody(doShieldWithUpperBody);
    NMP_ASSERT(doShieldWithUpperBody_importance >= 0.0f && doShieldWithUpperBody_importance <= 1.0f);
    m_doShieldWithUpperBodyImportance = doShieldWithUpperBody_importance;
  }
  NM_INLINE float getDoShieldWithUpperBodyImportance() const { return m_doShieldWithUpperBodyImportance; }
  NM_INLINE const float& getDoShieldWithUpperBodyImportanceRef() const { return m_doShieldWithUpperBodyImportance; }
  NM_INLINE const float& getDoShieldWithUpperBody() const { return doShieldWithUpperBody; }

  NM_INLINE void setDoShieldWithLowerBody(const float& _doShieldWithLowerBody, float doShieldWithLowerBody_importance = 1.f)
  {
    doShieldWithLowerBody = _doShieldWithLowerBody;
    postAssignDoShieldWithLowerBody(doShieldWithLowerBody);
    NMP_ASSERT(doShieldWithLowerBody_importance >= 0.0f && doShieldWithLowerBody_importance <= 1.0f);
    m_doShieldWithLowerBodyImportance = doShieldWithLowerBody_importance;
  }
  NM_INLINE float getDoShieldWithLowerBodyImportance() const { return m_doShieldWithLowerBodyImportance; }
  NM_INLINE const float& getDoShieldWithLowerBodyImportanceRef() const { return m_doShieldWithLowerBodyImportance; }
  NM_INLINE const float& getDoShieldWithLowerBody() const { return doShieldWithLowerBody; }

  NM_INLINE void setDriveCompensationScale(const float& _driveCompensationScale, float driveCompensationScale_importance = 1.f)
  {
    driveCompensationScale = _driveCompensationScale;
    postAssignDriveCompensationScale(driveCompensationScale);
    NMP_ASSERT(driveCompensationScale_importance >= 0.0f && driveCompensationScale_importance <= 1.0f);
    m_driveCompensationScaleImportance = driveCompensationScale_importance;
  }
  NM_INLINE float getDriveCompensationScaleImportance() const { return m_driveCompensationScaleImportance; }
  NM_INLINE const float& getDriveCompensationScaleImportanceRef() const { return m_driveCompensationScaleImportance; }
  NM_INLINE const float& getDriveCompensationScale() const { return driveCompensationScale; }

  NM_INLINE void setSuppress(const bool& _suppress, float suppress_importance = 1.f)
  {
    suppress = _suppress;
    NMP_ASSERT(suppress_importance >= 0.0f && suppress_importance <= 1.0f);
    m_suppressImportance = suppress_importance;
  }
  NM_INLINE float getSuppressImportance() const { return m_suppressImportance; }
  NM_INLINE const float& getSuppressImportanceRef() const { return m_suppressImportance; }
  NM_INLINE const bool& getSuppress() const { return suppress; }

protected:

  float yawAngle;  ///< current yaw rotation required to turn away from the hazard  (Angle)
  float hazardAngle;  ///< Relative to the support forwards direction  (Angle)
  float doShieldWithUpperBody;               ///< (Weight)
  float doShieldWithLowerBody;  ///< Will be sent only if character is not supported.  (Weight)
  float driveCompensationScale;  ///< Multiplier used to multiply the normalDriveCompensation of each limb.  (Multiplier)
  bool suppress;

  // post-assignment stubs
  NM_INLINE void postAssignYawAngle(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "yawAngle");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHazardAngle(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "hazardAngle");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDoShieldWithUpperBody(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "doShieldWithUpperBody");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDoShieldWithLowerBody(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "doShieldWithLowerBody");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignDriveCompensationScale(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "driveCompensationScale");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_yawAngleImportance, 
     m_hazardAngleImportance, 
     m_doShieldWithUpperBodyImportance, 
     m_doShieldWithLowerBodyImportance, 
     m_driveCompensationScaleImportance, 
     m_suppressImportance;

  friend class ShieldBehaviourInterface_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getYawAngleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(yawAngle), "yawAngle");
    }
    if (getHazardAngleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(hazardAngle), "hazardAngle");
    }
    if (getDoShieldWithUpperBodyImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(doShieldWithUpperBody), "doShieldWithUpperBody");
    }
    if (getDoShieldWithLowerBodyImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(doShieldWithLowerBody), "doShieldWithLowerBody");
    }
    if (getDriveCompensationScaleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(driveCompensationScale), "driveCompensationScale");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ShieldBehaviourInterfaceFeedbackOutputs
 * \ingroup ShieldBehaviourInterface
 * \brief FeedbackOutputs Non-SPU so it can interface with the system to get data into an appropriate network format etc
 *
 * Data packet definition (5 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ShieldBehaviourInterfaceFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ShieldBehaviourInterfaceFeedbackOutputs));
  }

  NM_INLINE void setIsShielding(const bool& _isShielding, float isShielding_importance = 1.f)
  {
    isShielding = _isShielding;
    NMP_ASSERT(isShielding_importance >= 0.0f && isShielding_importance <= 1.0f);
    m_isShieldingImportance = isShielding_importance;
  }
  NM_INLINE float getIsShieldingImportance() const { return m_isShieldingImportance; }
  NM_INLINE const float& getIsShieldingImportanceRef() const { return m_isShieldingImportance; }
  NM_INLINE const bool& getIsShielding() const { return isShielding; }

protected:

  bool isShielding;

  // importance values
  float
     m_isShieldingImportance;

  friend class ShieldBehaviourInterface_Con;

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

#endif // NM_MDF_SHIELDBEHAVIOURINTERFACE_DATA_H

