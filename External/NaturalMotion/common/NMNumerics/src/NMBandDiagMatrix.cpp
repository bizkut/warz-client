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
#include <math.h>
#include "NMPlatform/NMMathUtils.h"
#include "NMNumerics/NMBandDiagMatrix.h"
#include "NMPlatform/NMStlUtils.h"

#define ScalarType float
#include "NMBandDiagMatrix_imp.cpp"
#undef ScalarType

#define ScalarType double
#include "NMBandDiagMatrix_imp.cpp"
#undef ScalarType

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{
// Specialized template implementation stuff goes here

} // namespace NMP
