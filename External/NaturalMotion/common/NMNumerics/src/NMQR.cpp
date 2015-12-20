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
#include "NMNumerics/NMQR.h"

// Double precision
#define ScalarType double
#define NMQR_ETOL 1e-14
#include "NMQR_imp.cpp"
#undef NMQR_ETOL
#undef ScalarType

// Single precision
#define ScalarType float
#define NMQR_ETOL 1e-5f
#include "NMQR_imp.cpp"
#undef NMQR_ETOL
#undef ScalarType

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{
// Type-specific functions

} // namespace NMP
