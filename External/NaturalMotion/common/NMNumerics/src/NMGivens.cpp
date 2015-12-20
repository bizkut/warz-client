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
#include <cmath>
#include "NMPlatform/NMNorm.h"
#include "NMNumerics/NMGivens.h"

// Double precision
#define ScalarType double
#define NMGIVENS_ETOL 2.2205e-16
#include "NMGivens_imp.cpp"
#undef NMGIVENS_ETOL
#undef ScalarType

// Single precision
#define ScalarType float
#define NMGIVENS_ETOL 1.1921e-7f
#include "NMGivens_imp.cpp"
#undef NMGIVENS_ETOL
#undef ScalarType

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{
// Type-specific functions

} // namespace NMP
