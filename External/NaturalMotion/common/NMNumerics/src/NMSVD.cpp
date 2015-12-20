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
#include "NMNumerics/NMSVD.h"

// Double precision
#define ScalarType double
#define NMSVD_RTOL 1e-12
#include "NMSVD_imp.cpp"
#undef NMSVD_RTOL
#undef ScalarType

// Single precision
#define ScalarType float
#define NMSVD_RTOL 1e-6f
#include "NMSVD_imp.cpp"
#undef NMSVD_RTOL
#undef ScalarType

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{
// Type-specific functions

} // namespace NMP
