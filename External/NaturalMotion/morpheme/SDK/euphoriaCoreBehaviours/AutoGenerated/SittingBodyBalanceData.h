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

#ifndef NM_MDF_SITTINGBODYBALANCE_DATA_H
#define NM_MDF_SITTINGBODYBALANCE_DATA_H

// include definition file to create project dependency
#include "./Definition/Behaviours/SittingBodyBalance.module"

// external types
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
#include "euphoria/erLimbTransforms.h"
#include "NMPlatform/NMQuat.h"

// known types
#include "Types/SitParameters.h"
#include "Types/Environment_SupportPoly.h"
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
 * \class SittingBodyBalanceData
 * \ingroup SittingBodyBalance
 * \brief Data Drives the character into a sitting pose using the legs and arms to support the spine when necessary.
 *
 * Data packet definition (117 bytes, 128 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SittingBodyBalanceData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SittingBodyBalanceData));
  }

  enum Constants
  {
    /*  5 */ rigMaxNumSupports = (NetworkConstants::networkMaxNumLegs + NetworkConstants::networkMaxNumArms + 1),  
  };

  NMP::Vector3 effectiveGravityDir;  /// Direction of the effective gravity (adjusted if the floor is accelerating)  (Direction)
  NMP::Vector3 groundVel;  /// The current (smoothed) ground velocity (averaged over all supporting limbs)  (Velocity)
  NMP::Vector3 groundVelRate;  /// Rate of change of the ground velocity so we can smooth it  (Acceleration)
  NMP::Vector3 lastGroundVel;  /// Store the old ground velocity so we can estimate the acceleration  (Velocity)
  NMP::Vector3 groundAccel;  /// (Smoothed) Acceleration of the feet/standing surface  (Acceleration)
  NMP::Vector3 groundAccelRate;  /// Rate of change of the acceleration of the standing surface, used for smoothing  (Jerk)
  NMP::Vector3 COMVel;  /// velocity of the CoM  (Velocity)
  float sitAmount;                           ///< (Weight)
  bool needToInitialiseSupportingSurface;  /// True when after module entry and before supporting surface properties (effective gravity etc) have been updated.

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(effectiveGravityDir), "effectiveGravityDir");
    NM_VALIDATOR(Vector3Valid(groundVel), "groundVel");
    NM_VALIDATOR(Vector3Valid(groundVelRate), "groundVelRate");
    NM_VALIDATOR(Vector3Valid(lastGroundVel), "lastGroundVel");
    NM_VALIDATOR(Vector3Valid(groundAccel), "groundAccel");
    NM_VALIDATOR(Vector3Valid(groundAccelRate), "groundAccelRate");
    NM_VALIDATOR(Vector3Valid(COMVel), "COMVel");
    NM_VALIDATOR(FloatNonNegative(sitAmount), "sitAmount");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SittingBodyBalanceInputs
 * \ingroup SittingBodyBalance
 * \brief Inputs Drives the character into a sitting pose using the legs and arms to support the spine when necessary.
 *
 * Data packet definition (974 bytes, 992 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SittingBodyBalanceInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SittingBodyBalanceInputs));
  }

  NM_INLINE void setSupportPolygon(const Environment::SupportPoly& _supportPolygon, float supportPolygon_importance = 1.f)
  {
    supportPolygon = _supportPolygon;
    postAssignSupportPolygon(supportPolygon);
    NMP_ASSERT(supportPolygon_importance >= 0.0f && supportPolygon_importance <= 1.0f);
    m_supportPolygonImportance = supportPolygon_importance;
  }
  NM_INLINE Environment::SupportPoly& startSupportPolygonModification()
  {
    m_supportPolygonImportance = -1.0f; // set invalid until stopSupportPolygonModification()
    return supportPolygon;
  }
  NM_INLINE void stopSupportPolygonModification(float supportPolygon_importance = 1.f)
  {
    postAssignSupportPolygon(supportPolygon);
    NMP_ASSERT(supportPolygon_importance >= 0.0f && supportPolygon_importance <= 1.0f);
    m_supportPolygonImportance = supportPolygon_importance;
  }
  NM_INLINE float getSupportPolygonImportance() const { return m_supportPolygonImportance; }
  NM_INLINE const float& getSupportPolygonImportanceRef() const { return m_supportPolygonImportance; }
  NM_INLINE const Environment::SupportPoly& getSupportPolygon() const { return supportPolygon; }

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

protected:

  Environment::SupportPoly supportPolygon;
  NMP::Matrix34 armPoseEndRelativeToRoot[NetworkConstants::networkMaxNumArms];  ///< Arm balance pose  (Transform)
  NMP::Matrix34 legPoseEndRelativeToRoot[NetworkConstants::networkMaxNumLegs];  ///< Leg balance pose  (Transform)
  NMP::Matrix34 spinePoseEndRelativeToRoot[NetworkConstants::networkMaxNumSpines];  ///< Spine balance pose  (Transform)
  NMP::Matrix34 headPoseEndRelativeToRoot[NetworkConstants::networkMaxNumHeads];  ///< Head balance pose  (Transform)
  SitParameters sitParameters;  ///< Parameters defined by sit behaviour interface.
  bool supportWithArms;
  bool supportWithLegs;

  // post-assignment stubs
  NM_INLINE void postAssignSupportPolygon(const Environment::SupportPoly& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

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

  // importance values
  float
     m_supportPolygonImportance, 
     m_armPoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumArms], 
     m_legPoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumLegs], 
     m_spinePoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumSpines], 
     m_headPoseEndRelativeToRootImportance[NetworkConstants::networkMaxNumHeads], 
     m_sitParametersImportance, 
     m_supportWithArmsImportance, 
     m_supportWithLegsImportance;

  friend class SittingBodyBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getSupportPolygonImportance() > 0.0f)
    {
      supportPolygon.validate();
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
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SittingBodyBalanceOutputs
 * \ingroup SittingBodyBalance
 * \brief Outputs Drives the character into a sitting pose using the legs and arms to support the spine when necessary.
 *
 * Data packet definition (1168 bytes, 1184 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SittingBodyBalanceOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SittingBodyBalanceOutputs));
  }

  enum { maxArmControl = 2 };
  NM_INLINE unsigned int getMaxArmControl() const { return maxArmControl; }
  NM_INLINE void setArmControl(unsigned int number, const LimbControl* _armControl, float armControl_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmControl);
    NMP_ASSERT(armControl_importance >= 0.0f && armControl_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armControl[i] = _armControl[i];
      postAssignArmControl(armControl[i]);
      m_armControlImportance[i] = armControl_importance;
    }
  }
  NM_INLINE void setArmControlAt(unsigned int index, const LimbControl& _armControl, float armControl_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmControl);
    armControl[index] = _armControl;
    NMP_ASSERT(armControl_importance >= 0.0f && armControl_importance <= 1.0f);
    postAssignArmControl(armControl[index]);
    m_armControlImportance[index] = armControl_importance;
  }
  NM_INLINE LimbControl& startArmControlModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxArmControl);
    m_armControlImportance[index] = -1.0f; // set invalid until stopArmControlModificationAt()
    return armControl[index];
  }
  NM_INLINE void stopArmControlModificationAt(unsigned int index, float armControl_importance = 1.f)
  {
    postAssignArmControl(armControl[index]);
    NMP_ASSERT(armControl_importance >= 0.0f && armControl_importance <= 1.0f);
    m_armControlImportance[index] = armControl_importance;
  }
  NM_INLINE float getArmControlImportance(int index) const { return m_armControlImportance[index]; }
  NM_INLINE const float& getArmControlImportanceRef(int index) const { return m_armControlImportance[index]; }
  NM_INLINE unsigned int calculateNumArmControl() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armControlImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const LimbControl& getArmControl(unsigned int index) const { NMP_ASSERT(index <= maxArmControl); return armControl[index]; }

  NM_INLINE void setDesiredPelvisTM(const ER::LimbTransform& _desiredPelvisTM, float desiredPelvisTM_importance = 1.f)
  {
    desiredPelvisTM = _desiredPelvisTM;
    NMP_ASSERT(desiredPelvisTM_importance >= 0.0f && desiredPelvisTM_importance <= 1.0f);
    m_desiredPelvisTMImportance = desiredPelvisTM_importance;
  }
  NM_INLINE ER::LimbTransform& startDesiredPelvisTMModification()
  {
    m_desiredPelvisTMImportance = -1.0f; // set invalid until stopDesiredPelvisTMModification()
    return desiredPelvisTM;
  }
  NM_INLINE void stopDesiredPelvisTMModification(float desiredPelvisTM_importance = 1.f)
  {
    NMP_ASSERT(desiredPelvisTM_importance >= 0.0f && desiredPelvisTM_importance <= 1.0f);
    m_desiredPelvisTMImportance = desiredPelvisTM_importance;
  }
  NM_INLINE float getDesiredPelvisTMImportance() const { return m_desiredPelvisTMImportance; }
  NM_INLINE const float& getDesiredPelvisTMImportanceRef() const { return m_desiredPelvisTMImportance; }
  NM_INLINE const ER::LimbTransform& getDesiredPelvisTM() const { return desiredPelvisTM; }

  NM_INLINE void setDesiredChestTM(const ER::LimbTransform& _desiredChestTM, float desiredChestTM_importance = 1.f)
  {
    desiredChestTM = _desiredChestTM;
    NMP_ASSERT(desiredChestTM_importance >= 0.0f && desiredChestTM_importance <= 1.0f);
    m_desiredChestTMImportance = desiredChestTM_importance;
  }
  NM_INLINE ER::LimbTransform& startDesiredChestTMModification()
  {
    m_desiredChestTMImportance = -1.0f; // set invalid until stopDesiredChestTMModification()
    return desiredChestTM;
  }
  NM_INLINE void stopDesiredChestTMModification(float desiredChestTM_importance = 1.f)
  {
    NMP_ASSERT(desiredChestTM_importance >= 0.0f && desiredChestTM_importance <= 1.0f);
    m_desiredChestTMImportance = desiredChestTM_importance;
  }
  NM_INLINE float getDesiredChestTMImportance() const { return m_desiredChestTMImportance; }
  NM_INLINE const float& getDesiredChestTMImportanceRef() const { return m_desiredChestTMImportance; }
  NM_INLINE const ER::LimbTransform& getDesiredChestTM() const { return desiredChestTM; }

  enum { maxDesiredArmEndTM = 2 };
  NM_INLINE unsigned int getMaxDesiredArmEndTM() const { return maxDesiredArmEndTM; }
  NM_INLINE void setDesiredArmEndTM(unsigned int number, const ER::HandFootTransform* _desiredArmEndTM, float desiredArmEndTM_importance = 1.f)
  {
    NMP_ASSERT(number <= maxDesiredArmEndTM);
    NMP_ASSERT(desiredArmEndTM_importance >= 0.0f && desiredArmEndTM_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      desiredArmEndTM[i] = _desiredArmEndTM[i];
      m_desiredArmEndTMImportance[i] = desiredArmEndTM_importance;
    }
  }
  NM_INLINE void setDesiredArmEndTMAt(unsigned int index, const ER::HandFootTransform& _desiredArmEndTM, float desiredArmEndTM_importance = 1.f)
  {
    NMP_ASSERT(index < maxDesiredArmEndTM);
    desiredArmEndTM[index] = _desiredArmEndTM;
    NMP_ASSERT(desiredArmEndTM_importance >= 0.0f && desiredArmEndTM_importance <= 1.0f);
    m_desiredArmEndTMImportance[index] = desiredArmEndTM_importance;
  }
  NM_INLINE ER::HandFootTransform& startDesiredArmEndTMModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxDesiredArmEndTM);
    m_desiredArmEndTMImportance[index] = -1.0f; // set invalid until stopDesiredArmEndTMModificationAt()
    return desiredArmEndTM[index];
  }
  NM_INLINE void stopDesiredArmEndTMModificationAt(unsigned int index, float desiredArmEndTM_importance = 1.f)
  {
    NMP_ASSERT(desiredArmEndTM_importance >= 0.0f && desiredArmEndTM_importance <= 1.0f);
    m_desiredArmEndTMImportance[index] = desiredArmEndTM_importance;
  }
  NM_INLINE float getDesiredArmEndTMImportance(int index) const { return m_desiredArmEndTMImportance[index]; }
  NM_INLINE const float& getDesiredArmEndTMImportanceRef(int index) const { return m_desiredArmEndTMImportance[index]; }
  NM_INLINE unsigned int calculateNumDesiredArmEndTM() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_desiredArmEndTMImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const ER::HandFootTransform& getDesiredArmEndTM(unsigned int index) const { NMP_ASSERT(index <= maxDesiredArmEndTM); return desiredArmEndTM[index]; }

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

  NM_INLINE void setArmSupportImportance(const float& _armSupportImportance, float armSupportImportance_importance = 1.f)
  {
    armSupportImportance = _armSupportImportance;
    postAssignArmSupportImportance(armSupportImportance);
    NMP_ASSERT(armSupportImportance_importance >= 0.0f && armSupportImportance_importance <= 1.0f);
    m_armSupportImportanceImportance = armSupportImportance_importance;
  }
  NM_INLINE float getArmSupportImportanceImportance() const { return m_armSupportImportanceImportance; }
  NM_INLINE const float& getArmSupportImportanceImportanceRef() const { return m_armSupportImportanceImportance; }
  NM_INLINE const float& getArmSupportImportance() const { return armSupportImportance; }

  enum { maxArmSupportStrengthScales = 2 };
  NM_INLINE unsigned int getMaxArmSupportStrengthScales() const { return maxArmSupportStrengthScales; }
  NM_INLINE void setArmSupportStrengthScales(unsigned int number, const float* _armSupportStrengthScales, float armSupportStrengthScales_importance = 1.f)
  {
    NMP_ASSERT(number <= maxArmSupportStrengthScales);
    NMP_ASSERT(armSupportStrengthScales_importance >= 0.0f && armSupportStrengthScales_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      armSupportStrengthScales[i] = _armSupportStrengthScales[i];
      postAssignArmSupportStrengthScales(armSupportStrengthScales[i]);
      m_armSupportStrengthScalesImportance[i] = armSupportStrengthScales_importance;
    }
  }
  NM_INLINE void setArmSupportStrengthScalesAt(unsigned int index, const float& _armSupportStrengthScales, float armSupportStrengthScales_importance = 1.f)
  {
    NMP_ASSERT(index < maxArmSupportStrengthScales);
    armSupportStrengthScales[index] = _armSupportStrengthScales;
    NMP_ASSERT(armSupportStrengthScales_importance >= 0.0f && armSupportStrengthScales_importance <= 1.0f);
    postAssignArmSupportStrengthScales(armSupportStrengthScales[index]);
    m_armSupportStrengthScalesImportance[index] = armSupportStrengthScales_importance;
  }
  NM_INLINE float getArmSupportStrengthScalesImportance(int index) const { return m_armSupportStrengthScalesImportance[index]; }
  NM_INLINE const float& getArmSupportStrengthScalesImportanceRef(int index) const { return m_armSupportStrengthScalesImportance[index]; }
  NM_INLINE unsigned int calculateNumArmSupportStrengthScales() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_armSupportStrengthScalesImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getArmSupportStrengthScales(unsigned int index) const { NMP_ASSERT(index <= maxArmSupportStrengthScales); return armSupportStrengthScales[index]; }

  enum { maxLegSupportStrengthScales = 2 };
  NM_INLINE unsigned int getMaxLegSupportStrengthScales() const { return maxLegSupportStrengthScales; }
  NM_INLINE void setLegSupportStrengthScales(unsigned int number, const float* _legSupportStrengthScales, float legSupportStrengthScales_importance = 1.f)
  {
    NMP_ASSERT(number <= maxLegSupportStrengthScales);
    NMP_ASSERT(legSupportStrengthScales_importance >= 0.0f && legSupportStrengthScales_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      legSupportStrengthScales[i] = _legSupportStrengthScales[i];
      postAssignLegSupportStrengthScales(legSupportStrengthScales[i]);
      m_legSupportStrengthScalesImportance[i] = legSupportStrengthScales_importance;
    }
  }
  NM_INLINE void setLegSupportStrengthScalesAt(unsigned int index, const float& _legSupportStrengthScales, float legSupportStrengthScales_importance = 1.f)
  {
    NMP_ASSERT(index < maxLegSupportStrengthScales);
    legSupportStrengthScales[index] = _legSupportStrengthScales;
    NMP_ASSERT(legSupportStrengthScales_importance >= 0.0f && legSupportStrengthScales_importance <= 1.0f);
    postAssignLegSupportStrengthScales(legSupportStrengthScales[index]);
    m_legSupportStrengthScalesImportance[index] = legSupportStrengthScales_importance;
  }
  NM_INLINE float getLegSupportStrengthScalesImportance(int index) const { return m_legSupportStrengthScalesImportance[index]; }
  NM_INLINE const float& getLegSupportStrengthScalesImportanceRef(int index) const { return m_legSupportStrengthScalesImportance[index]; }
  NM_INLINE unsigned int calculateNumLegSupportStrengthScales() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_legSupportStrengthScalesImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getLegSupportStrengthScales(unsigned int index) const { NMP_ASSERT(index <= maxLegSupportStrengthScales); return legSupportStrengthScales[index]; }

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

  NM_INLINE void setSupportWithSpine(const bool& _supportWithSpine, float supportWithSpine_importance = 1.f)
  {
    supportWithSpine = _supportWithSpine;
    NMP_ASSERT(supportWithSpine_importance >= 0.0f && supportWithSpine_importance <= 1.0f);
    m_supportWithSpineImportance = supportWithSpine_importance;
  }
  NM_INLINE float getSupportWithSpineImportance() const { return m_supportWithSpineImportance; }
  NM_INLINE const float& getSupportWithSpineImportanceRef() const { return m_supportWithSpineImportance; }
  NM_INLINE const bool& getSupportWithSpine() const { return supportWithSpine; }

  NM_INLINE void setAllowLegStep(const bool& _allowLegStep, float allowLegStep_importance = 1.f)
  {
    allowLegStep = _allowLegStep;
    NMP_ASSERT(allowLegStep_importance >= 0.0f && allowLegStep_importance <= 1.0f);
    m_allowLegStepImportance = allowLegStep_importance;
  }
  NM_INLINE float getAllowLegStepImportance() const { return m_allowLegStepImportance; }
  NM_INLINE const float& getAllowLegStepImportanceRef() const { return m_allowLegStepImportance; }
  NM_INLINE const bool& getAllowLegStep() const { return allowLegStep; }

protected:

  LimbControl armControl[NetworkConstants::networkMaxNumArms];  ///< Limb control used for arm stepping.
  ER::LimbTransform desiredPelvisTM;  ///< Desired spine root transform that support modules may try to reach for balance
  ER::LimbTransform desiredChestTM;  ///< Desired spine end transform that support modules may try to reach for balance
  ER::HandFootTransform desiredArmEndTM[NetworkConstants::networkMaxNumArms];  ///< Desired hand transform for support module
  NMP::Quat chestRelPelvisOrientation[NetworkConstants::networkMaxNumSpines];  // Output for spine support module.  (OrientationDelta)
  NMP::Vector3 chestRelPelvisTranslation[NetworkConstants::networkMaxNumSpines];  ///< (PositionDelta)
  NMP::Quat headRelChestOrientation[NetworkConstants::networkMaxNumHeads];  /// The desired rotation of the head relative to the spine  (OrientationDelta)
  float armSupportImportance;  ///< Importance of the arms in keeping the spine upright.  (Weight)
  float armSupportStrengthScales[NetworkConstants::networkMaxNumArms];  ///< Strength scales for the arms that are supporting  (Weight)
  float legSupportStrengthScales[NetworkConstants::networkMaxNumLegs];  ///< Strength scales for the legs that are supporting  (Weight)
  bool supportWithArms;
  bool supportWithLegs;
  bool supportWithSpine;
  bool allowLegStep;

  // post-assignment stubs
  NM_INLINE void postAssignArmControl(const LimbControl& NMP_VALIDATOR_ARG(v)) const
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

  NM_INLINE void postAssignArmSupportImportance(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "armSupportImportance");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignArmSupportStrengthScales(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "armSupportStrengthScales");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignLegSupportStrengthScales(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "legSupportStrengthScales");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_armControlImportance[NetworkConstants::networkMaxNumArms], 
     m_desiredPelvisTMImportance, 
     m_desiredChestTMImportance, 
     m_desiredArmEndTMImportance[NetworkConstants::networkMaxNumArms], 
     m_chestRelPelvisOrientationImportance[NetworkConstants::networkMaxNumSpines], 
     m_chestRelPelvisTranslationImportance[NetworkConstants::networkMaxNumSpines], 
     m_headRelChestOrientationImportance[NetworkConstants::networkMaxNumHeads], 
     m_armSupportImportanceImportance, 
     m_armSupportStrengthScalesImportance[NetworkConstants::networkMaxNumArms], 
     m_legSupportStrengthScalesImportance[NetworkConstants::networkMaxNumLegs], 
     m_supportWithArmsImportance, 
     m_supportWithLegsImportance, 
     m_supportWithSpineImportance, 
     m_allowLegStepImportance;

  friend class SittingBodyBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numArmControl = calculateNumArmControl();
      for (uint32_t i=0; i<numArmControl; i++)
      {
        if (getArmControlImportance(i) > 0.0f)
          armControl[i].validate();
      }
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
    if (getArmSupportImportanceImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(armSupportImportance), "armSupportImportance");
    }
    {
      uint32_t numArmSupportStrengthScales = calculateNumArmSupportStrengthScales();
      for (uint32_t i=0; i<numArmSupportStrengthScales; i++)
      {
        if (getArmSupportStrengthScalesImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatNonNegative(armSupportStrengthScales[i]), "armSupportStrengthScales");
        }
      }
    }
    {
      uint32_t numLegSupportStrengthScales = calculateNumLegSupportStrengthScales();
      for (uint32_t i=0; i<numLegSupportStrengthScales; i++)
      {
        if (getLegSupportStrengthScalesImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatNonNegative(legSupportStrengthScales[i]), "legSupportStrengthScales");
        }
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SittingBodyBalanceFeedbackInputs
 * \ingroup SittingBodyBalance
 * \brief FeedbackInputs Drives the character into a sitting pose using the legs and arms to support the spine when necessary.
 *
 * Data packet definition (56 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SittingBodyBalanceFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SittingBodyBalanceFeedbackInputs));
  }

  NM_INLINE void setChestOnGroundAmount(const float& _chestOnGroundAmount, float chestOnGroundAmount_importance = 1.f)
  {
    chestOnGroundAmount = _chestOnGroundAmount;
    postAssignChestOnGroundAmount(chestOnGroundAmount);
    NMP_ASSERT(chestOnGroundAmount_importance >= 0.0f && chestOnGroundAmount_importance <= 1.0f);
    m_chestOnGroundAmountImportance = chestOnGroundAmount_importance;
  }
  NM_INLINE float getChestOnGroundAmountImportance() const { return m_chestOnGroundAmountImportance; }
  NM_INLINE const float& getChestOnGroundAmountImportanceRef() const { return m_chestOnGroundAmountImportance; }
  NM_INLINE const float& getChestOnGroundAmount() const { return chestOnGroundAmount; }

  NM_INLINE void setPelvisOnGroundAmount(const float& _pelvisOnGroundAmount, float pelvisOnGroundAmount_importance = 1.f)
  {
    pelvisOnGroundAmount = _pelvisOnGroundAmount;
    postAssignPelvisOnGroundAmount(pelvisOnGroundAmount);
    NMP_ASSERT(pelvisOnGroundAmount_importance >= 0.0f && pelvisOnGroundAmount_importance <= 1.0f);
    m_pelvisOnGroundAmountImportance = pelvisOnGroundAmount_importance;
  }
  NM_INLINE float getPelvisOnGroundAmountImportance() const { return m_pelvisOnGroundAmountImportance; }
  NM_INLINE const float& getPelvisOnGroundAmountImportanceRef() const { return m_pelvisOnGroundAmountImportance; }
  NM_INLINE const float& getPelvisOnGroundAmount() const { return pelvisOnGroundAmount; }

  enum { maxHandOnGroundAmount = 2 };
  NM_INLINE unsigned int getMaxHandOnGroundAmount() const { return maxHandOnGroundAmount; }
  NM_INLINE void setHandOnGroundAmount(unsigned int number, const float* _handOnGroundAmount, float handOnGroundAmount_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHandOnGroundAmount);
    NMP_ASSERT(handOnGroundAmount_importance >= 0.0f && handOnGroundAmount_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      handOnGroundAmount[i] = _handOnGroundAmount[i];
      postAssignHandOnGroundAmount(handOnGroundAmount[i]);
      m_handOnGroundAmountImportance[i] = handOnGroundAmount_importance;
    }
  }
  NM_INLINE void setHandOnGroundAmountAt(unsigned int index, const float& _handOnGroundAmount, float handOnGroundAmount_importance = 1.f)
  {
    NMP_ASSERT(index < maxHandOnGroundAmount);
    handOnGroundAmount[index] = _handOnGroundAmount;
    NMP_ASSERT(handOnGroundAmount_importance >= 0.0f && handOnGroundAmount_importance <= 1.0f);
    postAssignHandOnGroundAmount(handOnGroundAmount[index]);
    m_handOnGroundAmountImportance[index] = handOnGroundAmount_importance;
  }
  NM_INLINE float getHandOnGroundAmountImportance(int index) const { return m_handOnGroundAmountImportance[index]; }
  NM_INLINE const float& getHandOnGroundAmountImportanceRef(int index) const { return m_handOnGroundAmountImportance[index]; }
  NM_INLINE unsigned int calculateNumHandOnGroundAmount() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_handOnGroundAmountImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getHandOnGroundAmount(unsigned int index) const { NMP_ASSERT(index <= maxHandOnGroundAmount); return handOnGroundAmount[index]; }

  enum { maxHandOnGroundCollidingTime = 2 };
  NM_INLINE unsigned int getMaxHandOnGroundCollidingTime() const { return maxHandOnGroundCollidingTime; }
  NM_INLINE void setHandOnGroundCollidingTime(unsigned int number, const float* _handOnGroundCollidingTime, float handOnGroundCollidingTime_importance = 1.f)
  {
    NMP_ASSERT(number <= maxHandOnGroundCollidingTime);
    NMP_ASSERT(handOnGroundCollidingTime_importance >= 0.0f && handOnGroundCollidingTime_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      handOnGroundCollidingTime[i] = _handOnGroundCollidingTime[i];
      postAssignHandOnGroundCollidingTime(handOnGroundCollidingTime[i]);
      m_handOnGroundCollidingTimeImportance[i] = handOnGroundCollidingTime_importance;
    }
  }
  NM_INLINE void setHandOnGroundCollidingTimeAt(unsigned int index, const float& _handOnGroundCollidingTime, float handOnGroundCollidingTime_importance = 1.f)
  {
    NMP_ASSERT(index < maxHandOnGroundCollidingTime);
    handOnGroundCollidingTime[index] = _handOnGroundCollidingTime;
    NMP_ASSERT(handOnGroundCollidingTime_importance >= 0.0f && handOnGroundCollidingTime_importance <= 1.0f);
    postAssignHandOnGroundCollidingTime(handOnGroundCollidingTime[index]);
    m_handOnGroundCollidingTimeImportance[index] = handOnGroundCollidingTime_importance;
  }
  NM_INLINE float getHandOnGroundCollidingTimeImportance(int index) const { return m_handOnGroundCollidingTimeImportance[index]; }
  NM_INLINE const float& getHandOnGroundCollidingTimeImportanceRef(int index) const { return m_handOnGroundCollidingTimeImportance[index]; }
  NM_INLINE unsigned int calculateNumHandOnGroundCollidingTime() const
  {
    for (int i=1; i>=0; --i)
    {
      if (m_handOnGroundCollidingTimeImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const float& getHandOnGroundCollidingTime(unsigned int index) const { NMP_ASSERT(index <= maxHandOnGroundCollidingTime); return handOnGroundCollidingTime[index]; }

  NM_INLINE void setStandingBalanceAmount(const float& _standingBalanceAmount, float standingBalanceAmount_importance = 1.f)
  {
    standingBalanceAmount = _standingBalanceAmount;
    postAssignStandingBalanceAmount(standingBalanceAmount);
    NMP_ASSERT(standingBalanceAmount_importance >= 0.0f && standingBalanceAmount_importance <= 1.0f);
    m_standingBalanceAmountImportance = standingBalanceAmount_importance;
  }
  NM_INLINE float getStandingBalanceAmountImportance() const { return m_standingBalanceAmountImportance; }
  NM_INLINE const float& getStandingBalanceAmountImportanceRef() const { return m_standingBalanceAmountImportance; }
  NM_INLINE const float& getStandingBalanceAmount() const { return standingBalanceAmount; }

protected:

  float chestOnGroundAmount;                 ///< (Weight)
  float pelvisOnGroundAmount;                ///< (Weight)
  float handOnGroundAmount[NetworkConstants::networkMaxNumArms];  ///< (Weight)
  float handOnGroundCollidingTime[NetworkConstants::networkMaxNumArms];  ///< (TimePeriod)
  float standingBalanceAmount;               ///< (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignChestOnGroundAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "chestOnGroundAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignPelvisOnGroundAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "pelvisOnGroundAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHandOnGroundAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "handOnGroundAmount");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignHandOnGroundCollidingTime(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "handOnGroundCollidingTime");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignStandingBalanceAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "standingBalanceAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_chestOnGroundAmountImportance, 
     m_pelvisOnGroundAmountImportance, 
     m_handOnGroundAmountImportance[NetworkConstants::networkMaxNumArms], 
     m_handOnGroundCollidingTimeImportance[NetworkConstants::networkMaxNumArms], 
     m_standingBalanceAmountImportance;

  friend class SittingBodyBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getChestOnGroundAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(chestOnGroundAmount), "chestOnGroundAmount");
    }
    if (getPelvisOnGroundAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(pelvisOnGroundAmount), "pelvisOnGroundAmount");
    }
    {
      uint32_t numHandOnGroundAmount = calculateNumHandOnGroundAmount();
      for (uint32_t i=0; i<numHandOnGroundAmount; i++)
      {
        if (getHandOnGroundAmountImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatNonNegative(handOnGroundAmount[i]), "handOnGroundAmount");
        }
      }
    }
    {
      uint32_t numHandOnGroundCollidingTime = calculateNumHandOnGroundCollidingTime();
      for (uint32_t i=0; i<numHandOnGroundCollidingTime; i++)
      {
        if (getHandOnGroundCollidingTimeImportance(i) > 0.0f)
        {
          NM_VALIDATOR(FloatValid(handOnGroundCollidingTime[i]), "handOnGroundCollidingTime");
        }
      }
    }
    if (getStandingBalanceAmountImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(standingBalanceAmount), "standingBalanceAmount");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SittingBodyBalanceFeedbackOutputs
 * \ingroup SittingBodyBalance
 * \brief FeedbackOutputs Drives the character into a sitting pose using the legs and arms to support the spine when necessary.
 *
 * Data packet definition (28 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SittingBodyBalanceFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SittingBodyBalanceFeedbackOutputs));
  }

  NM_INLINE void setLeanVector(const NMP::Vector3& _leanVector, float leanVector_importance = 1.f)
  {
    leanVector = _leanVector;
    postAssignLeanVector(leanVector);
    NMP_ASSERT(leanVector_importance >= 0.0f && leanVector_importance <= 1.0f);
    m_leanVectorImportance = leanVector_importance;
  }
  NM_INLINE float getLeanVectorImportance() const { return m_leanVectorImportance; }
  NM_INLINE const float& getLeanVectorImportanceRef() const { return m_leanVectorImportance; }
  NM_INLINE const NMP::Vector3& getLeanVector() const { return leanVector; }

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

  NMP::Vector3 leanVector;  ///< Normalised direction from the pelvis to the chest.  (PositionDelta)
  float sitAmount;                           ///< (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignLeanVector(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "leanVector");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignSitAmount(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "sitAmount");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_leanVectorImportance, 
     m_sitAmountImportance;

  friend class SittingBodyBalance_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getLeanVectorImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(leanVector), "leanVector");
    }
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

#endif // NM_MDF_SITTINGBODYBALANCE_DATA_H

