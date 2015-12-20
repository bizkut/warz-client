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

#ifndef NM_MDF_SUPPORTPOLYGON_DATA_H
#define NM_MDF_SUPPORTPOLYGON_DATA_H

// include definition file to create project dependency
#include "./Definition/Modules/SupportPolygon.module"

// external types
#include "NMPlatform/NMVector3.h"

// known types
#include "Types/Environment_SupportPoly.h"

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
 * \class SupportPolygonData
 * \ingroup SupportPolygon
 * \brief Data Collects support points and builds them into a polygon structure.
 *
 * Data packet definition (528 bytes, 544 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SupportPolygonData
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SupportPolygonData));
  }

  enum Constants
  {
    /*  5 */ rigMaxNumSupports = (NetworkConstants::networkMaxNumLegs + NetworkConstants::networkMaxNumArms + NetworkConstants::networkMaxNumSpines),  
  };

  Environment::SupportPoly polygon;  ///< set of 3d points.

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    polygon.validate();
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SupportPolygonInputs
 * \ingroup SupportPolygon
 * \brief Inputs Collects support points and builds them into a polygon structure.
 *
 * Data packet definition (15 bytes, 32 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SupportPolygonInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SupportPolygonInputs));
  }

  NM_INLINE void setIncludeLegSupportPoints(const bool& _includeLegSupportPoints, float includeLegSupportPoints_importance = 1.f)
  {
    includeLegSupportPoints = _includeLegSupportPoints;
    NMP_ASSERT(includeLegSupportPoints_importance >= 0.0f && includeLegSupportPoints_importance <= 1.0f);
    m_includeLegSupportPointsImportance = includeLegSupportPoints_importance;
  }
  NM_INLINE float getIncludeLegSupportPointsImportance() const { return m_includeLegSupportPointsImportance; }
  NM_INLINE const float& getIncludeLegSupportPointsImportanceRef() const { return m_includeLegSupportPointsImportance; }
  NM_INLINE const bool& getIncludeLegSupportPoints() const { return includeLegSupportPoints; }

  NM_INLINE void setIncludeArmSupportPoints(const bool& _includeArmSupportPoints, float includeArmSupportPoints_importance = 1.f)
  {
    includeArmSupportPoints = _includeArmSupportPoints;
    NMP_ASSERT(includeArmSupportPoints_importance >= 0.0f && includeArmSupportPoints_importance <= 1.0f);
    m_includeArmSupportPointsImportance = includeArmSupportPoints_importance;
  }
  NM_INLINE float getIncludeArmSupportPointsImportance() const { return m_includeArmSupportPointsImportance; }
  NM_INLINE const float& getIncludeArmSupportPointsImportanceRef() const { return m_includeArmSupportPointsImportance; }
  NM_INLINE const bool& getIncludeArmSupportPoints() const { return includeArmSupportPoints; }

  NM_INLINE void setIncludeSpineSupportPoints(const bool& _includeSpineSupportPoints, float includeSpineSupportPoints_importance = 1.f)
  {
    includeSpineSupportPoints = _includeSpineSupportPoints;
    NMP_ASSERT(includeSpineSupportPoints_importance >= 0.0f && includeSpineSupportPoints_importance <= 1.0f);
    m_includeSpineSupportPointsImportance = includeSpineSupportPoints_importance;
  }
  NM_INLINE float getIncludeSpineSupportPointsImportance() const { return m_includeSpineSupportPointsImportance; }
  NM_INLINE const float& getIncludeSpineSupportPointsImportanceRef() const { return m_includeSpineSupportPointsImportance; }
  NM_INLINE const bool& getIncludeSpineSupportPoints() const { return includeSpineSupportPoints; }

protected:

  bool includeLegSupportPoints;
  bool includeArmSupportPoints;
  bool includeSpineSupportPoints;

  // importance values
  float
     m_includeLegSupportPointsImportance, 
     m_includeArmSupportPointsImportance, 
     m_includeSpineSupportPointsImportance;

  friend class SupportPolygon_Con;
  friend class SupportPolygon_StandingCon;
  friend class SupportPolygon_SittingCon;

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
 * \class SupportPolygonOutputs
 * \ingroup SupportPolygon
 * \brief Outputs Collects support points and builds them into a polygon structure.
 *
 * Data packet definition (552 bytes, 576 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SupportPolygonOutputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SupportPolygonOutputs));
  }

  NM_INLINE void setPolygon(const Environment::SupportPoly& _polygon, float polygon_importance = 1.f)
  {
    polygon = _polygon;
    postAssignPolygon(polygon);
    NMP_ASSERT(polygon_importance >= 0.0f && polygon_importance <= 1.0f);
    m_polygonImportance = polygon_importance;
  }
  NM_INLINE Environment::SupportPoly& startPolygonModification()
  {
    m_polygonImportance = -1.0f; // set invalid until stopPolygonModification()
    return polygon;
  }
  NM_INLINE void stopPolygonModification(float polygon_importance = 1.f)
  {
    postAssignPolygon(polygon);
    NMP_ASSERT(polygon_importance >= 0.0f && polygon_importance <= 1.0f);
    m_polygonImportance = polygon_importance;
  }
  NM_INLINE float getPolygonImportance() const { return m_polygonImportance; }
  NM_INLINE const float& getPolygonImportanceRef() const { return m_polygonImportance; }
  NM_INLINE const Environment::SupportPoly& getPolygon() const { return polygon; }

  NM_INLINE void setOffsetFromPolygon(const NMP::Vector3& _offsetFromPolygon, float offsetFromPolygon_importance = 1.f)
  {
    offsetFromPolygon = _offsetFromPolygon;
    postAssignOffsetFromPolygon(offsetFromPolygon);
    NMP_ASSERT(offsetFromPolygon_importance >= 0.0f && offsetFromPolygon_importance <= 1.0f);
    m_offsetFromPolygonImportance = offsetFromPolygon_importance;
  }
  NM_INLINE float getOffsetFromPolygonImportance() const { return m_offsetFromPolygonImportance; }
  NM_INLINE const float& getOffsetFromPolygonImportanceRef() const { return m_offsetFromPolygonImportance; }
  NM_INLINE const NMP::Vector3& getOffsetFromPolygon() const { return offsetFromPolygon; }

protected:

  Environment::SupportPoly polygon;  ///< The calculated polygon of support.
  NMP::Vector3 offsetFromPolygon;  ///< Offset of test position from polygon.  (PositionDelta)

  // post-assignment stubs
  NM_INLINE void postAssignPolygon(const Environment::SupportPoly& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    v.validate();
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignOffsetFromPolygon(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "offsetFromPolygon");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_polygonImportance, 
     m_offsetFromPolygonImportance;

  friend class SupportPolygon_Con;
  friend class SupportPolygon_StandingCon;
  friend class SupportPolygon_SittingCon;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    if (getPolygonImportance() > 0.0f)
    {
      polygon.validate();
    }
    if (getOffsetFromPolygonImportance() > 0.0f)
    {
      NM_VALIDATOR(Vector3Valid(offsetFromPolygon), "offsetFromPolygon");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------
/** 
 * \class SupportPolygonFeedbackInputs
 * \ingroup SupportPolygon
 * \brief FeedbackInputs Collects support points and builds them into a polygon structure.
 *
 * Data packet definition (108 bytes, 128 aligned)
 */
NMP_ALIGN_PREFIX(32) struct SupportPolygonFeedbackInputs
{
  NM_INLINE void clear()
  {
    memset(this, 0, sizeof(SupportPolygonFeedbackInputs));
  }

  enum { maxSupportPoints = 5 };
  NM_INLINE unsigned int getMaxSupportPoints() const { return maxSupportPoints; }
  NM_INLINE void setSupportPoints(unsigned int number, const NMP::Vector3* _supportPoints, float supportPoints_importance = 1.f)
  {
    NMP_ASSERT(number <= maxSupportPoints);
    NMP_ASSERT(supportPoints_importance >= 0.0f && supportPoints_importance <= 1.0f);
    for (unsigned int i = 0; i<number; i++)
    {
      supportPoints[i] = _supportPoints[i];
      postAssignSupportPoints(supportPoints[i]);
      m_supportPointsImportance[i] = supportPoints_importance;
    }
  }
  NM_INLINE void setSupportPointsAt(unsigned int index, const NMP::Vector3& _supportPoints, float supportPoints_importance = 1.f)
  {
    NMP_ASSERT(index < maxSupportPoints);
    supportPoints[index] = _supportPoints;
    NMP_ASSERT(supportPoints_importance >= 0.0f && supportPoints_importance <= 1.0f);
    postAssignSupportPoints(supportPoints[index]);
    m_supportPointsImportance[index] = supportPoints_importance;
  }
  NM_INLINE float getSupportPointsImportance(int index) const { return m_supportPointsImportance[index]; }
  NM_INLINE const float& getSupportPointsImportanceRef(int index) const { return m_supportPointsImportance[index]; }
  NM_INLINE unsigned int calculateNumSupportPoints() const
  {
    for (int i=4; i>=0; --i)
    {
      if (m_supportPointsImportance[i])
        return ((unsigned int)(i+1));
    }
    return 0;
  }
  NM_INLINE const NMP::Vector3& getSupportPoints(unsigned int index) const { NMP_ASSERT(index <= maxSupportPoints); return supportPoints[index]; }

  NM_INLINE void setFullySupported(const float& _fullySupported, float fullySupported_importance = 1.f)
  {
    fullySupported = _fullySupported;
    postAssignFullySupported(fullySupported);
    NMP_ASSERT(fullySupported_importance >= 0.0f && fullySupported_importance <= 1.0f);
    m_fullySupportedImportance = fullySupported_importance;
  }
  NM_INLINE float getFullySupportedImportance() const { return m_fullySupportedImportance; }
  NM_INLINE const float& getFullySupportedImportanceRef() const { return m_fullySupportedImportance; }
  NM_INLINE const float& getFullySupported() const { return fullySupported; }

protected:

  NMP::Vector3 supportPoints[SupportPolygonData::rigMaxNumSupports];  ///< Incoming points which define the support.  (Position)
  float fullySupported;  ///< Affects lean vector and makes inside-support-polygon return true.  (Weight)

  // post-assignment stubs
  NM_INLINE void postAssignSupportPoints(const NMP::Vector3& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(Vector3Valid(v), "supportPoints");
#endif // NM_CALL_VALIDATORS
  }

  NM_INLINE void postAssignFullySupported(const float& NMP_VALIDATOR_ARG(v)) const
  {
#if NM_CALL_VALIDATORS
    NM_VALIDATOR(FloatNonNegative(v), "fullySupported");
#endif // NM_CALL_VALIDATORS
  }

  // importance values
  float
     m_supportPointsImportance[SupportPolygonData::rigMaxNumSupports], 
     m_fullySupportedImportance;

  friend class SupportPolygon_Con;
  friend class SupportPolygon_StandingCon;
  friend class SupportPolygon_SittingCon;

public:

  // manually call any assigned validators
  NM_INLINE void validate() const
  {
#if NM_CALL_VALIDATORS
    {
      uint32_t numSupportPoints = calculateNumSupportPoints();
      for (uint32_t i=0; i<numSupportPoints; i++)
      {
        if (getSupportPointsImportance(i) > 0.0f)
        {
          NM_VALIDATOR(Vector3Valid(supportPoints[i]), "supportPoints");
        }
      }
    }
    if (getFullySupportedImportance() > 0.0f)
    {
      NM_VALIDATOR(FloatNonNegative(fullySupported), "fullySupported");
    }
#endif // NM_CALL_VALIDATORS
  }

} NMP_ALIGN_SUFFIX(32);

//----------------------------------------------------------------------------------------------------------------------

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER
} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_SUPPORTPOLYGON_DATA_H

