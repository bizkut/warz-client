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
#include "NMNumerics/NMScrew.h"
#include "NMPlatform/NMNorm.h"

// Double precision
#define ScalarType double
#include "NMScrew_imp.cpp"
#undef ScalarType

// Single precision
#define ScalarType float
#include "NMScrew_imp.cpp"
#undef ScalarType

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{
// Type-specific functions

} // namespace NMP
