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
#ifndef NM_SOLVER_TRIDIAG_H
#define NM_SOLVER_TRIDIAG_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMBandDiagMatrix.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Used to return the status of the solver
class SolverTriDiagStatus
{
public:
  enum eType {
    Success,
    Singular,
    BadParams
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Tri-diagonal linear equation solver
///
/// Performs fast LU elimination without pivoting on the tri-diagonals. The
/// process of forward elimination and backward substitution each take only
/// O(N) operations for the special tri-diagonal matrix form.
///
/// Note: in practice most tri-diagonal systems are diagonally dominant, thus
/// guaranteeing a stable solution without the need for pivoting.
///
template <typename _T>
class SolverTriDiag
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to compute or preallocate memory for a linear system
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the solution of a simple tri-diagonal
  /// linear system of equations. i.e. compute the solution vectors X for the
  /// linear system A X = B. The tri-diagonal matrix is specified by a
  /// band diagonal matrix with one sub and super diagonal bands.
  SolverTriDiag(const BandDiagMatrix<_T>& A, const Matrix<_T>& B);

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the solution of a simple tri-diagonal
  /// linear system of equations. i.e. compute the solution vectors X for the
  /// linear system A X = B. The tri-diagonal matrix is specified by three
  /// vectors: Adiag has length N, Asub and Asuper both have length N-1
  SolverTriDiag(const Vector<_T>& Asub, const Vector<_T>& Adiag,
                const Vector<_T>& Asuper, const Matrix<_T>& B);

  //---------------------------------------------------------------------
  /// \brief  Constructor to preallocate the memory for a linear system
  ///
  /// \param  An - size of the square matrix A
  /// \param  Bc - number of right hand sides (columns in X and B)
  SolverTriDiag(uint32_t An, uint32_t Bc);

  //---------------------------------------------------------------------
  /// Destructor
  ~SolverTriDiag();
  //@}

  //---------------------------------------------------------------------
  /// \name   Repeat Solve functions
  /// \brief  These functions provide a way to repeatedly solve similar tri-diagonal
  /// linear systems. i.e. the A, X and B matrices have already been preallocated by
  /// the corresponding constructor and will always have the same sizes.
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Function to compute the solution of a simple tri-diagonal
  /// linear system of equations. i.e. compute the solution vectors X for the
  /// linear system A X = B. The tri-diagonal matrix is specified by a
  /// band diagonal matrix with one sub and super diagonal bands.
  SolverTriDiagStatus::eType solve(
    const BandDiagMatrix<_T>& A, const Matrix<_T>& B);

  //---------------------------------------------------------------------
  /// \brief  Function to compute the solution of a simple tri-diagonal
  /// linear system of equations. i.e. compute the solution vectors X for the
  /// linear system A X = B. The tri-diagonal matrix is specified by three
  /// vectors: Adiag has length N, Asub and Asuper both have length N-1
  SolverTriDiagStatus::eType solve(
    const Vector<_T>& Asub, const Vector<_T>& Adiag, const Vector<_T>& Asuper,
    const Matrix<_T>& B);
  //@}

  //---------------------------------------------------------------------
  /// \name   Solver Data
  /// \brief  Functions to get information or matrix data
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Get the tri-diagonal matrix A
  const BandDiagMatrix<_T>& getA() const;
  BandDiagMatrix<_T>& getA();

  //---------------------------------------------------------------------
  /// \brief  Get the B block matrix
  const Matrix<_T>& getB() const;
  Matrix<_T>& getB();

  //---------------------------------------------------------------------
  /// \brief  Get the solution matrix
  const Matrix<_T>& getX() const;
  Matrix<_T>& getX();

  //---------------------------------------------------------------------
  /// \brief  Get the status of the solve
  SolverTriDiagStatus::eType getStatus() const;

  //---------------------------------------------------------------------
  /// \brief  Test if the solver failed
  bool fail() const;
  //@}

protected:
  //---------------------------------------------------------------------
  /// \brief  Performs tri-diagonal linear solve to compute X.
  /// (Main solve routine that does not allocate memory)
  SolverTriDiagStatus::eType doSolve();

protected:
  /// Data matrices
  BandDiagMatrix<_T>    m_A;
  Matrix<_T>            m_X;
  Matrix<_T>            m_B;

  /// Computation status
  SolverTriDiagStatus::eType   m_status;

protected:
  /// Temporary workspace memory required for the solve
  Vector<_T>            m_v;
  Vector<_T>            m_p;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_SOLVER_TRIDIAG_H
