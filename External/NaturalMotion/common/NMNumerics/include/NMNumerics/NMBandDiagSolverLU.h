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
#ifndef NM_BAND_DIAG_SOLVER_LU_H
#define NM_BAND_DIAG_SOLVER_LU_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMBandDiagMatrix.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Used to return the status of the decomposition
class BandDiagSolverLUStatus
{
public:
  enum eType {
    Success,
    Singular,
    BadParams
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Band diagonal linear equation solver via LU decomposition
///

template <typename _T>
class BandDiagSolverLU
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory or solve a linear system
  //---------------------------------------------------------------------
  //@{
  BandDiagSolverLU(const BandDiagMatrix<_T>& A, const Matrix<_T>& B);
  BandDiagSolverLU(const BandDiagMatrix<_T>& A, const Vector<_T>& b);
  BandDiagSolverLU(uint32_t An, uint32_t sub, uint32_t super, uint32_t Bc);

  // Invert the matrix
  BandDiagSolverLU(const BandDiagMatrix<_T>& A);
  BandDiagSolverLU(uint32_t An, uint32_t sub, uint32_t super);

  // Destructor
  ~BandDiagSolverLU();
  //@}

  //---------------------------------------------------------------------
  /// \name   Repeat solve functions
  /// \brief  Functions to decompose or solve linear systems using the
  /// preallocated memory
  //---------------------------------------------------------------------
  //@{
  BandDiagSolverLUStatus::eType decompose(const BandDiagMatrix<_T>& A);

  BandDiagSolverLUStatus::eType invert(const BandDiagMatrix<_T>& A);

  BandDiagSolverLUStatus::eType solve(const BandDiagMatrix<_T>& A, const Matrix<_T>& B);
  void solve(const Matrix<_T>& B);
  void solve(Vector<_T>& b);
  //@}

  //---------------------------------------------------------------------
  /// \name   Information
  /// \brief  Functions to get information or matrix data
  //---------------------------------------------------------------------
  //@{

  /// \brief  Determinant of the matrix
  _T determinant() const;

  /// \brief  Get the solution matrix
  const Matrix<_T>& getX() const;
  Matrix<_T>& getX();

  /// \brief  Get the status of the solve
  BandDiagSolverLUStatus::eType getStatus() const;
  //@}

protected:
  /// \brief  Performs the main LU decomposition routine
  /// that does not allocate memory
  BandDiagSolverLUStatus::eType doDecomposition();

  /// \brief  Flattens the band diagonal matrix B into a B.rows by B.bandWidth matrix
  /// by pushing all its rows to the left. This is the format required for the
  /// LU decomposition process
  void flattenRows(const BandDiagMatrix<_T>& B);

private:
  // Data for LU decomposition
  Matrix<_T>       m_L; // sub by rows matrix containing the gauss vectors
  Matrix<_T>       m_U; // rows by bandwidth matrix of U
  Vector<int32_t>  m_Pk; // Permutation matrices

  // Data for solution
  Matrix<_T>    m_X;

  // Computation status
  BandDiagSolverLUStatus::eType   m_status;

private:
  // Temporary workspace memory
  Vector<_T>    m_p; ///< vector of size A.rows for solves
  Vector<_T>    m_u; ///< vector of size A.band
  Vector<_T>    m_v; ///< vector of size A.band
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_BAND_DIAG_SOLVER_LU_H
