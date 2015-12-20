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
#ifndef NM_VECTOR3_UTILS_H
#define NM_VECTOR3_UTILS_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"

//----------------------------------------------------------------------------------------------------------------------
/// \file NMVector3Utils.h
/// \ingroup NaturalMotionPlatform
/// \brief Defines a set of maths utility functions for manipulating Vector3s.
//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the min and max bounds of the set of 3-vector samples
void vecBounds(uint32_t numSamples, const Vector3* v, Vector3& minV, Vector3& maxV);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the mean of the set of 3-vector samples
void vecMean(uint32_t numSamples, const Vector3* v, Vector3& vbar);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the mean of the set of 3-vector samples
void vecMean(
  uint32_t              numArrays,
  const uint32_t*       vecSizes,
  const Vector3* const* vecArrays,
  Vector3&              vbar);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Function to compute the component variances of the set of 3-vector samples
void vecVar(uint32_t numSamples, const Vector3* v, Vector3& vvar, bool zeroMean = false);

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_VECTOR3_UTILS_H
