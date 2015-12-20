// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.
//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_SIMPLE_SPLINE_FITTING_UTILS_H
#define NM_SIMPLE_SPLINE_FITTING_UTILS_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Utility function class
class SimpleSplineFittingUtils
{
public:
  //---------------------------------------------------------------------
  /// \name   Utility functions
  /// \brief  Functions to provide information
  //---------------------------------------------------------------------
  //@{
  static bool isPosChannelUnchanging(const Vector3& pa, const Vector3& pb);
  static bool isPosChannelUnchanging(uint32_t numSamples, const Vector3* data);
  static bool isPosChannelUnchanging(const Vector3& pa, uint32_t numSamples, const Vector3* data);

  static bool isQuatChannelUnchanging(const Quat& qa, const Quat& qb);
  static bool isQuatChannelUnchanging(uint32_t numSamples, const Quat* quats);
  static bool isQuatChannelUnchanging(const Quat& qa, uint32_t numSamples, const Quat* quats);

  static void normaliseQuats(uint32_t numSamples, Quat* quats);
  static void conditionQuats(uint32_t numSamples, Quat* quats);

  static bool isKnotVectorValid(uint32_t numKnots, const float* knots);
  static bool isKnotVectorValid(uint32_t numKnots, const uint32_t* knots);

  static int32_t interval(float t, uint32_t& intv, uint32_t numSamples, const float* ts);
  static Vector3 posInterpolate(float t, uint32_t numSamples, const float* ts, const Vector3* data);
  static Quat quatInterpolate(float t, uint32_t numSamples, const float* ts, const Quat* quats);

  static void arcLengthVector(uint32_t numSamples, const Vector3* data, float* result);
  static void angleLengthVector(uint32_t numSamples, const Quat* quats, float* result);

  static int32_t findSampleSpan(float t, uint32_t& intv, uint32_t ia, uint32_t ib, uint32_t numSamples, const float* ts);
  static bool findSamplesExclusive(float ta, float tb, uint32_t& ia, uint32_t& ib, uint32_t numSamples, const float* ts);
  static bool findSamplesInclusive(float ta, float tb, uint32_t& ia, uint32_t& ib, uint32_t numSamples, const float* ts);
  static bool findSamplesInclusiveTa(float ta, float tb, uint32_t& ia, uint32_t& ib, uint32_t numSamples, const float* ts);
  static bool findSamplesInclusiveTb(float ta, float tb, uint32_t& ia, uint32_t& ib, uint32_t numSamples, const float* ts);
  //@}

protected:
  SimpleSplineFittingUtils() {} // Ensure we never construct an object
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_SIMPLE_SPLINE_FITTING_UTILS_H
