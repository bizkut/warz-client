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
#ifndef NM_NUMERIC_UTILS_H
#define NM_NUMERIC_UTILS_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMMatrix.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Convert a right side multiply transformation matrix H into separate
/// left side transformations R, t.
template <typename _T>
void convertToLeftTransform(const Matrix34& H, Matrix<_T>& R, Vector<_T>& t);

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Convert separate left side transformations R, t into a right side
/// multiply transformation matrix H.
template <typename _T>
void convertFromLeftTransform(Matrix34& H, const Matrix<_T>& R, const Vector<_T>& t);

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Modify the angle so that it lies within the [-pi:pi) range by adding
/// an integer multiple of 2*pi. The function returns the corresponding cycle number
/// that the original angle lies within.
template <typename _T>
int32_t cycleAngle(_T& theta);

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Modify the angle by adding an integer multiple of 2*pi so that it
/// lies closest to the base angle. The function returns the cycle number of the
/// new angle.
template <typename _T>
int32_t closestAngle(const _T baseAng, _T& ang);

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_NUMERIC_UTILS_H
