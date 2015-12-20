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
#ifndef NM_SPLINE_UTILS_H
#define NM_SPLINE_UTILS_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector.h"
#include "NMPlatform/NMMatrix.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Removes repeated sites from the data samples. The function returns
/// the number of removed data sites.
template <typename _T>
uint32_t removeMultiplicities(Matrix<_T>& X, Vector<_T>& t);

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Remove all the repeated data samples by replacing repeated samples with the
/// weighted average of the corresponding Y data. The weight factor is replaced with
/// the corresponding weight sum. The function returns the number of removed data sites.
template <typename _T>
uint32_t removeMultiplicities(Matrix<_T>& Y_, Vector<_T>& x_, Vector<_T>& w_);

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_SPLINE_UTILS_H
