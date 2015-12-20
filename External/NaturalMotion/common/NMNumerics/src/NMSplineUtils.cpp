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
#include "NMNumerics/NMSplineUtils.h"

#define ScalarType float
#define NMSPLUTLS_EPS 1e-6
#include "NMSplineUtils_imp.cpp"
#undef NMSPLUTLS_EPS
#undef ScalarType

#define ScalarType double
#define NMSPLUTLS_EPS 1e-14
#include "NMSplineUtils_imp.cpp"
#undef NMSPLUTLS_EPS
#undef ScalarType

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{
// Specialized template implementation stuff goes here

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
