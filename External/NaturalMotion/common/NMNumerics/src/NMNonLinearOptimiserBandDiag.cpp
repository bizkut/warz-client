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
#include "NMNumerics/NMNonLinearOptimiserBandDiag.h"

// Disable warnings constant conditionals
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4127)
#endif

// Double precision
#define ScalarType double
#include "NMNonLinearOptimiserBandDiag_imp.cpp"
#undef ScalarType

// Single precision
#define ScalarType float
#include "NMNonLinearOptimiserBandDiag_imp.cpp"
#undef ScalarType

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// JacobianBlockDiagInfo
//----------------------------------------------------------------------------------------------------------------------
JacobianBlockDiagInfo::JacobianBlockDiagInfo()
{
}

//----------------------------------------------------------------------------------------------------------------------
JacobianBlockDiagInfo::~JacobianBlockDiagInfo()
{
}

//----------------------------------------------------------------------------------------------------------------------
void JacobianBlockDiagInfo::clear()
{
  m_jRow = 0;
  m_jColumn = 0;
  m_jSize = 0;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
