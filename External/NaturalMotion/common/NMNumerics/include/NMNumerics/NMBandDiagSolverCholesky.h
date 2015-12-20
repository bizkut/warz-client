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
#ifndef NM_BAND_DIAG_SOLVER_CHOLESKY_H
#define NM_BAND_DIAG_SOLVER_CHOLESKY_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMBandDiagMatrix.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Used to return the status of the decomposition
class BandDiagSolverCholeskyStatus
{
public:
  enum eType {
    Success,
    NotPosDef,
    BadParams
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Band diagonal linear equation solver via Cholesky decomposition
///
template <typename _T>
class BandDiagSolverCholesky
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory or solve a linear system
  //---------------------------------------------------------------------
  //@{
  BandDiagSolverCholesky(const BandDiagMatrix<_T>& A, const Matrix<_T>& B);
  BandDiagSolverCholesky(const BandDiagMatrix<_T>& A, const Vector<_T>& b);
  BandDiagSolverCholesky(uint32_t An, uint32_t sub, uint32_t Bc);

  // Constructors to compute or preallocate memory for matrix inversion
  BandDiagSolverCholesky(const BandDiagMatrix<_T>& A);
  BandDiagSolverCholesky(uint32_t An, uint32_t sub);

  // Destructor
  ~BandDiagSolverCholesky();
  //@}

  //---------------------------------------------------------------------
  /// \name   Repeat solve functions
  /// \brief  Functions to decompose or solve linear systems using the
  /// preallocated memory
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief Function to decompose the symmetric band diagonal matrix A into
  /// banded lower triangular form L * L^T. Only the main and upper super diagonal
  /// band of matrix A is needed to compute the decomposition. User applications
  /// may therefore wish to implement the appropriate reduced storage scheme.
  BandDiagSolverCholeskyStatus::eType decompose(const BandDiagMatrix<_T>& A);

  //---------------------------------------------------------------------
  /// \brief Function to invert the symmetric band diagonal matrix A. Only
  /// the main and upper super diagonal band of matrix A is needed to compute
  /// the decomposition L * L^T and the. User applications may therefore wish
  /// to implement the appropriate reduced storage scheme.
  BandDiagSolverCholeskyStatus::eType invert(const BandDiagMatrix<_T>& A);

  //---------------------------------------------------------------------
  /// \brief Function to solve the banded symmetric linear system of equations
  /// (L * L^T) X = B. Only the main and upper super diagonal band of matrix A
  /// is needed to compute the decomposition. User applications may therefore
  /// wish to implement the appropriate reduced storage scheme.
  BandDiagSolverCholeskyStatus::eType solve(const BandDiagMatrix<_T>& A, const Matrix<_T>& B);

  //---------------------------------------------------------------------
  /// \brief Function to solve the banded symmetric linear system of equations
  /// (L * L^T) x = b. Only the main and upper super diagonal band of matrix A
  /// is needed to compute the decomposition. User applications may therefore
  /// wish to implement the appropriate reduced storage scheme.
  BandDiagSolverCholeskyStatus::eType solve(const BandDiagMatrix<_T>& A, const Vector<_T>& b);

  //---------------------------------------------------------------------
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
  const BandDiagMatrix<_T>& getL() const;
  BandDiagMatrix<_T>& getL();

  //---------------------------------------------------------------------
  /// \brief  Get the solution matrix
  const Matrix<_T>& getX() const;
  Matrix<_T>& getX();
  void getX(Vector<_T>& x) const;

  //---------------------------------------------------------------------
  /// \brief  Get the status of the solve
  BandDiagSolverCholeskyStatus::eType getStatus() const;

  //---------------------------------------------------------------------
  /// \brief  Test if the solver failed to decompose the matrix
  bool fail() const;
  //@}

protected:
  //---------------------------------------------------------------------
  /// \brief  Performs the main Cholesky decomposition routine
  /// that does not allocate memory
  BandDiagSolverCholeskyStatus::eType doDecomposition();

  //---------------------------------------------------------------------
  /// \brief  Set the lower triangle of L with the upper triangle of A
  void setLower(const BandDiagMatrix<_T>& A);

private:
  // Data for Cholesky decomposition
  BandDiagMatrix<_T>      m_L; ///< n by n lower triangular matrix with sub-band

  // Data for solution
  Matrix<_T>              m_X;

  // Computation status
  BandDiagSolverCholeskyStatus::eType   m_status;

private:
  // Temporary workspace memory
  Vector<_T>    m_p; ///< vector of size L.rows for solves
  Vector<_T>    m_u; ///< vector of size L.sub + 1
  Vector<_T>    m_v; ///< vector of size L.sub + 1
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_BAND_DIAG_SOLVER_CHOLESKY_H
