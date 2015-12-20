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
#include "NMPlatform/NMNorm.h"
//----------------------------------------------------------------------------------------------------------------------

// Double precision
#define ScalarType double
#define ResultType double
#define NMNORM_DWARF (3.834e-20)
#define NMNORM_GIANT (1.304e19)
#include "NMNorm_imp.cpp"
#undef NMNORM_DWARF
#undef NMNORM_GIANT
#undef ResultType
#undef ScalarType

// Single precision
#define ScalarType float
#define ResultType float
#define NMNORM_DWARF (3.834e-10f)
#define NMNORM_GIANT (1.304e9f)
#include "NMNorm_imp.cpp"
#undef NMNORM_DWARF
#undef NMNORM_GIANT
#undef ResultType
#undef ScalarType

// Integer
#define ScalarType int32_t
#define ResultType double
#define NMNORM_DWARF (3.834e-20)
#define NMNORM_GIANT (1.304e19)
#include "NMNorm_imp.cpp"
#undef NMNORM_DWARF
#undef NMNORM_GIANT
#undef ResultType
#undef ScalarType

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
template <typename T, typename R>
R enorm(Vector<T>& v)
{
  return NMP::enorm(v.numElements(), &v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T, typename R>
R inftynorm(Vector<T>& v)
{
  return NMP::inftynorm(v.numElements(), &v[0]);
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
