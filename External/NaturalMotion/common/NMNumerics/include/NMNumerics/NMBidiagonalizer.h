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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NM_BIDIAGONALIZER_H
#define NM_BIDIAGONALIZER_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMNumerics/NMHouseholder.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Class for reducing a matrix A(m,n) with m >= n to bidiagonal form via
/// householder transforms: U^T A V = B
template <typename _T>
class Bidiagonalizer
{
public:
  //---------------------------------------------------------------------
  // Method of bi-diagonal reduction: R_BiDiag is more
  // efficient whenever m >= 5n/3
  enum eMethod {
    BiDiag,  // left - right transforms on A(m,n): flops = 4mn^2 - 4n^3/3
    R_BiDiag // reduce A(m,n) to R then left - right on R(n,n): flops = 2mn^2 + 2n^3
  }; 

public:
  //---------------------------------------------------------------------
  // Constructors / Destructor
  Bidiagonalizer(const Matrix<_T>& A);
  Bidiagonalizer(uint32_t m, uint32_t n);
  ~Bidiagonalizer();

  // Bi-diagonalization of matrix A
  void compute(const Matrix<_T>& A);

  // Get the bidiagonal matrix B
  void getB(Matrix<_T>& B);
  Matrix<_T> getB();

  // Get the left orthogonal matrix U
  void getU(Matrix<_T>& U);
  Matrix<_T> getU();

  // Get the right orthogonal matrix V
  void getV(Matrix<_T>& V);
  Matrix<_T> getV();

  // Get the workspace matrix Q
  const Matrix<_T>& getQ() const;
  Matrix<_T>& getQ();

  // Get the H Householder transform
  Householder<_T>& getTransformH();

  // Get the Hr Householder transform
  Householder<_T>& getTransformHr();

protected:
  // Householder decomposition (m by n) for reducing A to upper triangular form
  Householder<_T>       m_H;

  // Householder decomposition (n by n) for applying left and right transforms
  // to reduce the first upper triangular matrix to bidiagonal form
  Householder<_T>       m_Hr;

  // Method of bi-diagonal reduction
  eMethod               m_method;

protected:
  // Workspace matrix for the R-Bidiagonalization (n by n)
  Matrix<_T>            m_Q;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_BIDIAGONALIZER_H
