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
#ifndef NM_NORM_H
#define NM_NORM_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

/// \brief Function computes pythagoras' formula sqrt(a^2 + b^2) without destructive overflow.
/// \ingroup NaturalMotionPlatformMath
template <typename T, typename R>
R pythag(T a, T b);

/// \brief Function computes pythagoras' formula sqrt(a^2 + b^2 + c^2) without destructive overflow.
/// \ingroup NaturalMotionPlatformMath
template <typename T, typename R>
R pythag(T a, T b, T c);

/// \brief Function computes the Euclidean norm (L2-norm) of the vector x
/// || x ||_2 = sqrt(\ sum_i x_i^2)
/// \ingroup NaturalMotionPlatformMath
/// \param  n - The number of elements in the vector.
/// \param  x - The vector.
/// \return The Euclidean norm.
template <typename T, typename R>
R enorm(uint32_t n, const T* x);

template <typename T, typename R>
R enorm(Vector<T>& v);

/// \brief Function computes the L1 norm of the vector x
/// || x ||_1 = \ sum_i |x_i|
/// \ingroup NaturalMotionPlatformMath
/// \param  n - The number of elements in the vector.
/// \param  x - The vector.
/// \return The L1 norm.
template <typename T, typename R>
R norm1(uint32_t n, const T* x);

template <typename T, typename R>
R norm1(Vector<T>& v);

/// \brief Function computes the L-infinity norm of the vector x
/// || x ||_\ infty = max_i | x_i |
/// \ingroup NaturalMotionPlatformMath
/// \param  n - The number of elements in the vector.
/// \param  x - The vector.
/// \return The L-infinity norm.
template <typename T, typename R>
R inftynorm(uint32_t n, const T* x);

template <typename T, typename R>
R inftynorm(Vector<T>& v);

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_NORM_H
//----------------------------------------------------------------------------------------------------------------------
