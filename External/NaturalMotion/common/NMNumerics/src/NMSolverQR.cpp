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
#include "NMNumerics/NMSolverQR.h"
#include "NMPlatform/NMNorm.h"

#define ScalarType float
#define NMSOLVER_ETOL 1e-5f
#include "NMSolverQR_imp.cpp"
#undef NMSOLVER_ETOL
#undef ScalarType

#define ScalarType double
#define NMSOLVER_ETOL 1e-14
#include "NMSolverQR_imp.cpp"
#undef NMSOLVER_ETOL
#undef ScalarType

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// Helper function for determining the size of the solution vector
uint32_t getSizeX(uint32_t Ac, const Vector<int32_t>& offsets)
{
  uint32_t n = offsets.numElements();
  for (uint32_t i = 1; i < n; ++i)
  {
    Ac += (offsets[i] - offsets[i-1]);
  }
  return Ac;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
