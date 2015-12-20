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
#ifndef NM_SOLVER_CHOLESKY_H
#define NM_SOLVER_CHOLESKY_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMVector.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Used to return the status of the decomposition
class SolverCholeskyStatus
{
public:
  enum eType {
    Success,
    NotPosDef,
    BadParams
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  linear equation solver A X = B via Cholesky decomposition,
/// where A = L L^T is a symmetric, positive definite matrix.

template <typename _T>
class SolverCholesky
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory or solve a linear system
  //---------------------------------------------------------------------
  //@{
  SolverCholesky(const Matrix<_T>& A, const Matrix<_T>& B);
  SolverCholesky(const Matrix<_T>& A, const Vector<_T>& b);
  SolverCholesky(uint32_t An, uint32_t Bc);

  // Constructors to compute or preallocate memory for matrix inversion
  SolverCholesky(const Matrix<_T>& A);
  SolverCholesky(uint32_t An);

  // Destructor
  ~SolverCholesky();
  //@}

  //---------------------------------------------------------------------
  /// \name   Repeat solve functions
  /// \brief  Functions to decompose or solve linear systems using the
  /// preallocated memory
  //---------------------------------------------------------------------
  //@{
  SolverCholeskyStatus::eType decompose(const Matrix<_T>& A);

  SolverCholeskyStatus::eType invert(const Matrix<_T>& A);

  SolverCholeskyStatus::eType solve(const Matrix<_T>& A, const Matrix<_T>& B);
  SolverCholeskyStatus::eType solve(const Matrix<_T>& A, const Vector<_T>& b);
  void solve(const Matrix<_T>& B);
  void solve(Vector<_T>& b);
  //@}

  //---------------------------------------------------------------------
  /// \name   Information
  /// \brief  Functions to get information or matrix data
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Determinant of the matrix
  _T determinant();

  //---------------------------------------------------------------------
  /// \brief  Get the lower triangular matrix
  const Matrix<_T>& getL() const;
  Matrix<_T>& getL();

  //---------------------------------------------------------------------
  /// \brief  Get the solution matrix
  const Matrix<_T>& getX() const;
  Matrix<_T>& getX();
  void getX(Vector<_T>& x) const;

  //---------------------------------------------------------------------
  /// \brief  Get the status of the solve
  SolverCholeskyStatus::eType getStatus() const;

  //---------------------------------------------------------------------
  /// \brief  Test if the solver failed to decompose the matrix
  bool fail() const;
  //@}

protected:
  //---------------------------------------------------------------------
  /// \brief  Performs the main Cholesky decomposition routine
  /// that does not allocate memory
  SolverCholeskyStatus::eType doDecomposition();

  //---------------------------------------------------------------------
  /// \brief  Set the lower triangle of L with the upper triangle of A
  void setLower(const Matrix<_T>& A);

private:
  // Lower triangle for Cholesky decomposition
  Matrix<_T>    m_L;

  // Data for solution
  Matrix<_T>    m_X;

  // Computation status
  SolverCholeskyStatus::eType   m_status;

private:
  // Temporary workspace memory
  Vector<_T>    m_p;
  Vector<_T>    m_v;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_SOLVER_CHOLESKY_H
