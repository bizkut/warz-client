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
#include <limits>
#include "NMNumerics/NMNonLinearOptimiserBase.h"

// Disable warnings constant conditionals
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning(disable : 4127)
#endif

// Double precision
#define ScalarType double
#define NMNONLINEAR_TOLP 1e-6
#define NMNONLINEAR_TOLFUNC 1e-6
#define NMNONLINEAR_FDEPS 1e-7
#define NMNONLINEAR_FDMIN 1e-8
#define NMNONLINEAR_FDMAX 1e-1
#define NMNONLINEAR_LAMBDA_INIT 1e-4
#include "NMNonLinearOptimiserBase_imp.cpp"
#undef NMNONLINEAR_TOLP
#undef NMNONLINEAR_TOLFUNC
#undef NMNONLINEAR_FDEPS
#undef NMNONLINEAR_FDMIN
#undef NMNONLINEAR_FDMAX
#undef NMNONLINEAR_LAMBDA_INIT
#undef ScalarType

// Single precision
#define ScalarType float
#define NMNONLINEAR_TOLP 1e-5f
#define NMNONLINEAR_TOLFUNC 1e-5f
#define NMNONLINEAR_FDEPS 1e-6f
#define NMNONLINEAR_FDMIN 1e-7f
#define NMNONLINEAR_FDMAX 1e-1f
#define NMNONLINEAR_LAMBDA_INIT 1e-4f
#include "NMNonLinearOptimiserBase_imp.cpp"
#undef NMNONLINEAR_TOLP
#undef NMNONLINEAR_TOLFUNC
#undef NMNONLINEAR_FDEPS
#undef NMNONLINEAR_FDMIN
#undef NMNONLINEAR_FDMAX
#undef NMNONLINEAR_LAMBDA_INIT
#undef ScalarType

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// NonLinearLeastSquaresFuncBase
//----------------------------------------------------------------------------------------------------------------------
NonLinearLeastSquaresFuncBase::
NonLinearLeastSquaresFuncBase(NonLinearOptimiserFuncType::eType funcType, uint32_t numUnknowns, uint32_t maxNumResiduals)
  : m_funcType(funcType), m_numUnknowns(numUnknowns),
    m_maxNumResiduals(maxNumResiduals), m_numResiduals(maxNumResiduals)
{
}

//----------------------------------------------------------------------------------------------------------------------
NonLinearLeastSquaresFuncBase::~NonLinearLeastSquaresFuncBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
