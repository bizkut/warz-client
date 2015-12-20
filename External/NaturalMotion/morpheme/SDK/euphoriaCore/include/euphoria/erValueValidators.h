  #pragma once

/*
 * Copyright (c) 2011 NaturalMotion Ltd. All rights reserved.
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
#ifndef NM_VALUEVALIDATORFNS_H
#define NM_VALUEVALIDATORFNS_H
//----------------------------------------------------------------------------------------------------------------------

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"

// can change the conditions for enabling NM_CALL_VALIDATORS, this just means that we avoid their
// overhead on PPC platforms where they cause plenty of fcmp and LHS performance problems
#ifndef NM_CALL_VALIDATORS
  #if (NM_HOST_BIGENDIAN && defined(_DEBUG)) || defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
    #define NM_CALL_VALIDATORS  1
    #define NMP_VALIDATOR_ARG(x) x
  #else
    #define NM_CALL_VALIDATORS  0
    #define NMP_VALIDATOR_ARG(x) NMP_UNUSED(x)
  #endif
#endif // NM_CALL_VALIDATORS

//----------------------------------------------------------------------------------------------------------------------
#if defined(NM_DEBUG)
#define NM_VALIDATOR(_fn, _st) if (Validators::_fn == false) { NMP_DEBUG_MSG("%s(%i): Value validation failure - %s - %s", __FILE__, __LINE__, _st, #_fn); NM_BREAK(); }
#else
#define NM_VALIDATOR(_fn, _st) if (Validators::_fn == false) { NM_BREAK(); }
#endif // NM_DEBUG

/// This limit is intended to catch gross deviations from normal due to either simulation
/// instability or incorrect maths - not small deviations due to numerical drift.
#define EUPHORIA_QUAT_ERROR_LIMIT 0.01f

namespace Validators
{

/**
 * Value constraint functions can be assigned to types and variables in the MDF
 * definitions. They must be inline and return a bool, true if the validation passed.
 *
 */

NM_INLINE bool FloatValid(const float& f)
{
  return (f == f);
}

NM_INLINE bool Float0to1(const float& f)
{
  return FloatValid(f) && (f >= 0.0f && f <= 1.0f);
}

NM_INLINE bool Float0to2PI(const float& f)
{
  return FloatValid(f) && (f >= 0.0f && f <= (2.0f * NM_PI));
}

NM_INLINE bool FloatNegPItoPI(const float& f)
{
  return FloatValid(f) && (f >= (-NM_PI) && f <= NM_PI);
}

NM_INLINE bool FloatNonNegative(const float& f)
{
  return FloatValid(f) && (f >= 0.0f);
}

NM_INLINE bool Vector3Valid(const NMP::Vector3& v)
{
  return FloatValid(v.x) && FloatValid(v.y) && FloatValid(v.z);
}

NM_INLINE bool Vector3Normalised(const NMP::Vector3& v)
{
  float magSqr = v.magnitudeSquared();
  // MORPH-14154 this tolerance may need some tweaking + commenting.
  return Vector3Valid(v) && (fabs(magSqr - 1.0f) < 0.02f || magSqr == 0.0f);
}

NM_INLINE bool Vector3NonNegative(const NMP::Vector3& v)
{
  return FloatNonNegative(v.x) && FloatNonNegative(v.y) && FloatNonNegative(v.z);
}

NM_INLINE bool QuatValid(const NMP::Quat& q)
{
  return FloatValid(q.x) && FloatValid(q.y) && FloatValid(q.z) && FloatValid(q.w);
}

NM_INLINE bool QuatNormalised(const NMP::Quat& q)
{
  return QuatValid(q) && q.isNormal(EUPHORIA_QUAT_ERROR_LIMIT);
}

NM_INLINE bool Matrix34Valid(const NMP::Matrix34& m)
{
  return Vector3Valid(m.r[0]) && Vector3Valid(m.r[1]) && Vector3Valid(m.r[2]) && Vector3Valid(m.r[3]);
}

NM_INLINE bool Matrix34Orthonormal(const NMP::Matrix34& m)
{
  // MORPH-14154 this tolerance may need some tweaking + commenting.
  return Matrix34Valid(m) && m.isValidTM(0.01f);
}

// Validation for generic types
inline void validateValue(float value)
{
  NM_VALIDATOR(FloatValid(value), "value");
}
inline void validateValue(const NMP::Vector3& value)
{
  NM_VALIDATOR(Vector3Valid(value), "value");
}
inline void validateValue(const NMP::Matrix34& value)
{
  NM_VALIDATOR(Matrix34Valid(value), "value");
}
template<typename T>
void validateValue(T NMP_UNUSED(value))
{
}

}

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_VALUEVALIDATORFNS_H
//----------------------------------------------------------------------------------------------------------------------

