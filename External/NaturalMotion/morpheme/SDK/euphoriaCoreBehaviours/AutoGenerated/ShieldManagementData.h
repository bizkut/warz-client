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

#ifndef NM_MDF_SHIELDMANAGEMENT_DATA_H
#define NM_MDF_SHIELDMANAGEMENT_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/ShieldManagement.module"

// external types
#include "euphoria/erLimbTransforms.h"
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/RotationRequest.h"

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
 * \class ShieldManagementData
 * \ingroup ShieldManagement
 * \brief Data coordinates the shielding of the whole character
 *
 * Data packet definition (8 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ShieldManagementData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ShieldManagementData));
  }

  float yawAngle;                            ///< (Angle)
  float shieldTimer;                         ///< (TimePeriod)

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(yawAngle), "yawAngle");
    NM_VALIDATOR(FloatValid(shieldTimer), "shieldTimer");
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ShieldManagementInputs
 * \ingroup ShieldManagement
 * \brief Inputs coordinates the shielding of the whole character
 *
 * Data packet definition (36 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ShieldManagementInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ShieldManagementInputs));
  }

  NM_INLINE void setDirectionToHazard(const NMP::Vector3& _directionToHazard, float directionToHazard_importance = 1.f)
  {
    directionToHazard = _directionToHazard;
    postAssignDirectionToHazard(directionToHazard);
    NMP_ASSERT(directionToHazard_importance >= 0.0f && directionToHazard_importance <= 1.0f);
    m_directionToHazardImportance = directionToHazard_importance;
  }
  NM_INLINE float getDirectionToHazardImportance() const { return m_directionToHazardImportance; }
  NM_INLINE const float& getDirectionToHazardImportanceRef() const { return m_directionToHazardImportance; }
  NM_INLINE const NMP::Vector3& getDirectionToHazard() const { return directionToHazard; }

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

  NMP::Vector3 directionToHazard;  ///< In world space - from the character pelvis to the hazard  (Direction)
  float smoothReturnTP;  ///< Time character will need to stop completely to do a shield.   (TimePeriod)
  float turnAwayScale;  ///< Scale used to set the amount of rotation to turn away from the hazard (0 - 1).  (Multiplier)

  // post-assignment stubs
  NM_INLINE void postAssignDirectionToHazard(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Normalised(v), "directionToHazard");
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
     m_directionToHazardImportance, 
     m_smoothReturnTPImportance, 
     m_turnAwayScaleImportance;

  friend class ShieldManagement_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getDirectionToHazardImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Normalised(directionToHazard), "directionToHazard");
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
 * \class ShieldManagementOutputs
 * \ingroup ShieldManagement
 * \brief Outputs coordinates the shielding of the whole character
 *
 * Data packet definition (164 bytes, 192 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ShieldManagementOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ShieldManagementOutputs));
  }

  enum { maxSpineRotationRequest = 1 };
  NM_INLINE unsigned int getMaxSpineRotationRequest() const { return maxSpineRotationRequest; }
  NM_INLINE void setSpineRotationRequest(unsigned int number, const RotationRequest* _spineRotationRequest, float spineRotationRequest_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSpineRotationRequest);
    NMP_ASSERT(spineRotationRequest_importance >= 0.0f && spineRotationRequest_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      spineRotationRequest[i] = _spineRotationRequest[i];
      postAssignSpineRotationRequest(spineRotationRequest[i]);
      m_spineRotationRequestImportance[i] = spineRotationRequest_importance;
    }
  }
  NM_INLINE void setSpineRotationRequestAt(unsigned int index, const RotationRequest& _spineRotationRequest, float spineRotationRequest_importance = 1.f)
  {
    NMP_ASSERT(index < maxSpineRotationRequest);
    spineRotationRequest[index] = _spineRotationRequest;
    NMP_ASSERT(spineRotationRequest_importance >= 0.0f && spineRotationRequest_importance <= 1.0f);
    postAssignSpineRotationRequest(spineRotationRequest[index]);
    m_spineRotationRequestImportance[index] = spineRotationRequest_importance;
  }
  NM_INLINE RotationRequest& startSpineRotationRequestModificationAt(unsigned int index)
  {
    NMP_ASSERT(index <= maxSpineRotationRequest);
    m_spineRotationRequestImportance[index] = -1.0f; // set invalid until stopSpineRotationRequestModificationAt()
    return spineRotationRequest[index];
  }
  NM_INLINE void stopSpineRotationRequestModificationAt(unsigned int index, float spineRotationRequest_importance = 1.f)
  {
    postAssignSpineRotationRequest(spineRotationRequest[index]);
    NMP_ASSERT(spineRotationRequest_importance >= 0.0f && spineRotationRequest_importance <= 1.0f);
    m_spineRotationRequestImportance[index] = spineRotationRequest_importance;
  }
  NM_INLINE float getSpineRotationRequestImportance(int index) const { return m_spineRotationRequestImportance[index]; }
  NM_INLINE const float& getSpineRotationRequestImportanceRef(int index) const { return m_spineRotationRequestImportance[index]; }
  NM_INLINE unsigned int calculateNumSpineRotationRequest() const
  {
    for (int i=0; i>=0; --i)
    {
      if (m_spineRotationRequestImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const RotationRequest& getSpineRotationRequest(unsigned int index) const { NMP_ASSERT(index <= maxSpineRotationRequest); return spineRotationRequest[index]; }

protected:

  RotationRequest spineRotationRequest[NetworkConstants::networkMaxNumSpines];  ///< Angle to turn away from the hazard, imminence is 0.5f if danger is not max or 10 if it is.

  // post-assignment stubs
  NM_INLINE void postAssignSpineRotationRequest(const RotationRequest& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_spineRotationRequestImportance[NetworkConstants::networkMaxNumSpines];

  friend class ShieldManagement_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numSpineRotationRequest = calculateNumSpineRotationRequest();
      for (uint32_t i=0; i<numSpineRotationRequest; i++)
      {
        if (getSpineRotationRequestImportance(i) > 0.0f)
          spineRotationRequest[i].validate();
      }
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ShieldManagementFeedbackInputs
 * \ingroup ShieldManagement
 * \brief FeedbackInputs coordinates the shielding of the whole character
 *
 * Data packet definition (68 bytes, 96 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ShieldManagementFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ShieldManagementFeedbackInputs));
  }

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

protected:

  ER::LimbTransform supportTM;  ///< in support this is considered the "fixed root" of the system

  // importance values
  float
     m_supportTMImportance;

  friend class ShieldManagement_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class ShieldManagementFeedbackOutputs
 * \ingroup ShieldManagement
 * \brief FeedbackOutputs coordinates the shielding of the whole character
 *
 * Data packet definition (40 bytes, 64 aligned)
 */
NMP_ALIGN_PREFIX(32) struct ShieldManagementFeedbackOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(ShieldManagementFeedbackOutputs));
  }

  NM_INLINE void setShieldSpineYawAngle(const float& _shieldSpineYawAngle, float shieldSpineYawAngle_importance = 1.f)
  {
    shieldSpineYawAngle = _shieldSpineYawAngle;
    postAssignShieldSpineYawAngle(shieldSpineYawAngle);
    NMP_ASSERT(shieldSpineYawAngle_importance >= 0.0f && shieldSpineYawAngle_importance <= 1.0f);
    m_shieldSpineYawAngleImportance = shieldSpineYawAngle_importance;
  }
  NM_INLINE float getShieldSpineYawAngleImportance() const { return m_shieldSpineYawAngleImportance; }
  NM_INLINE const float& getShieldSpineYawAngleImportanceRef() const { return m_shieldSpineYawAngleImportance; }
  NM_INLINE const float& getShieldSpineYawAngle() const { return shieldSpineYawAngle; }

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

protected:

  float shieldSpineYawAngle;  ///< Yaw angle to turn away from the hazard.  (Angle)
  float hazardAngle;  ///< Relative to the support forwards direction  (Angle)
  float doShieldWithUpperBody;               ///< (Weight)
  float doShieldWithLowerBody;  ///< Will be sent only if character is not supported.  (Weight)
  float driveCompensationScale;  ///< Multiplier used to multiply the normalDriveCompensation of each limb.  (Multiplier)

  // post-assignment stubs
  NM_INLINE void postAssignShieldSpineYawAngle(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatValid(v), "shieldSpineYawAngle");
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
     m_shieldSpineYawAngleImportance, 
     m_hazardAngleImportance, 
     m_doShieldWithUpperBodyImportance, 
     m_doShieldWithLowerBodyImportance, 
     m_driveCompensationScaleImportance;

  friend class ShieldManagement_Con;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getShieldSpineYawAngleImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatValid(shieldSpineYawAngle), "shieldSpineYawAngle");
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

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SHIELDMANAGEMENT_DATA_H

