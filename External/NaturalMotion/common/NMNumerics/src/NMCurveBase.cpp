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
#include <limits> // infinity
#include "NMPlatform/NMNorm.h"
#include "NMNumerics/NMCurveBase.h"
#include "NMPlatform/NMStlUtils.h"

// Disable warnings constant conditionals
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4127)
#endif

#define ScalarType float
// Set up tolerances for point projection onto the curve
#define NMCURVEBASE_CTOL 1e-5f
#define NMCURVEBASE_XTOL 1e-5f
#define NMCURVEBASE_DTOL 1e-5f
#include "NMCurveBase_imp.cpp"
#undef NMCURVEBASE_DTOL
#undef NMCURVEBASE_XTOL
#undef NMCURVEBASE_CTOL
#undef ScalarType

#define ScalarType double
// Set up tolerances for point projection onto the curve
#define NMCURVEBASE_CTOL 1e-10
#define NMCURVEBASE_XTOL 1e-10
#define NMCURVEBASE_DTOL 1e-10
#include "NMCurveBase_imp.cpp"
#undef NMCURVEBASE_DTOL
#undef NMCURVEBASE_XTOL
#undef NMCURVEBASE_CTOL
#undef ScalarType

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
KnotProperties::KnotProperties(int32_t rgn, uint32_t intv, uint32_t mult)
  : m_region(rgn), m_interval(intv), m_multiplicity(mult)
{
}

//----------------------------------------------------------------------------------------------------------------------
KnotProperties::KnotProperties() : m_region(-1), m_interval(0), m_multiplicity(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
KnotProperties::~KnotProperties()
{
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
