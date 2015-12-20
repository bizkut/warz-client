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
#ifndef NM_STL_UTILS_H
#define NM_STL_UTILS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMVector.h"
#include "NMPlatform/NMMatrix.h"
#include <ostream>

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

// Streaming an NMP::Vector
template <typename T>
std::ostream& operator<<(std::ostream& os, const Vector<T>& vec);

// Streaming an NMP::Matrix
template <typename T>
std::ostream& operator<<(std::ostream& os, const Matrix<T>& mat);

} // namespace NMP

#include "NMPlatform/NMStlUtils.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_STL_UTILS_H
