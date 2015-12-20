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
#ifndef NM_SOLVER_LU_H
#define NM_SOLVER_LU_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMVector.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Used to return the status of the decomposition
class SolverLUStatus
{
public:
  enum eType {
    Success,
    Singular,
    BadParams
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Linear equation solver via LU decomposition with pivoting
///
template <typename _T>
class SolverLU
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to compute or preallocate memory for a linear system
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the solution of a simple linear system of
  /// equations A X = B
  SolverLU(const Matrix<_T>& A, const Matrix<_T>& B);
  SolverLU(const Matrix<_T>& A, const Vector<_T>& b);
  SolverLU(uint32_t An, uint32_t Bc);

  //---------------------------------------------------------------------
  /// \brief  Constructors to compute or preallocate memory for matrix inversion
  SolverLU(const Matrix<_T>& A);
  SolverLU(uint32_t An);

  /// Destructor
  ~SolverLU();
  //@}

  //---------------------------------------------------------------------
  /// \name   Repeat Solve functions
  /// \brief  These functions provide a way to repeatedly solve similar tri-diagonal
  /// linear systems. i.e. the A, X and B matrices have already been preallocated by
  /// the corresponding constructor and will always have the same sizes.
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Function the decompose the matrix in to LU form
  SolverLUStatus::eType decompose(const Matrix<_T>& A);

  //---------------------------------------------------------------------
  /// \brief  Function the decompose the matrix in to LU form and solve
  /// the set of linear equations (L U) X = I
  SolverLUStatus::eType invert(const Matrix<_T>& A);

  //---------------------------------------------------------------------
  /// \brief  Function to solve the linear system of equations (L U) X = B
  /// where the left hand side has previously been decomposed into LU form
  void solve(const Matrix<_T>& B);

  //---------------------------------------------------------------------
  /// \brief  Function to solve the linear system of equations (L U) x = b
  /// where the left hand side has previously been decomposed into LU form.
  /// The solution vector x replaces the right hand side vector during the
  /// computation.
  void solve(Vector<_T>& b);
  //@}

  //---------------------------------------------------------------------
  /// \name   Solver Data
  /// \brief  Functions to get information or matrix data
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Determinant of the matrix
  _T determinant() const;

  //---------------------------------------------------------------------
  /// \brief  Get the solution matrix
  const Matrix<_T>& getX() const;
  Matrix<_T>& getX();

  //---------------------------------------------------------------------
  /// \brief  Get the status of the solve
  SolverLUStatus::eType getStatus() const;

  //---------------------------------------------------------------------
  /// \brief  Test if the solver failed
  bool fail() const;

  //---------------------------------------------------------------------
  /// \brief  Get the workspace decomposition matrix
  Matrix<_T>& getA();
  //@}

protected:
  //---------------------------------------------------------------------
  /// \brief  Performs the main LU decomposition routine
  /// that does not allocate memory
  SolverLUStatus::eType doDecomposition();

private:
  /// Data for LU decomposition
  Matrix<_T>        m_A;
  Vector<int32_t>   m_Pk; // Permutation matrices

  /// Data for solution
  Matrix<_T>        m_X;

  /// Computation status
  SolverLUStatus::eType   m_status;

private:
  /// Temporary workspace memory
  Vector<_T>        m_p;
  Vector<_T>        m_v;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_SOLVER_LU_H
