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
#ifndef NM_EIGEN_SYSTEM_SYM_H
#define NM_EIGEN_SYSTEM_SYM_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMHouseholder.h"
#include "NMNumerics/NMGivens.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Used to return the status of the decomposition
class EigenSystemStatus
{
public:
  enum eType {
    Success,
    MaxIts,
    BadParams
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Class to perform Eigen-decomposition of a symmetric matrix A
/// D = V^T * A * V
template <typename _T>
class EigenSystemSym
{
public:
  //---------------------------------------------------------------------
  // Constructors / Destructor
  EigenSystemSym(const Matrix<_T>& A, bool wantV = true);
  EigenSystemSym(uint32_t n);
  ~EigenSystemSym();

  //---------------------------------------------------------------------
  /// \brief  Main computation routine that does not allocate memory
  EigenSystemStatus::eType compute(const Matrix<_T>& A, bool wantV = true);

  //---------------------------------------------------------------------
  /// \brief  Solve the linear system A x = b (in least squares sense)
  void solve(Vector<_T>& x, const Vector<_T>& b);

  //---------------------------------------------------------------------
  /// \brief  Compute the pseudo-inverse of the matrix: A^+ = V D^+ V^T
  void pseudoInverse(Matrix<_T>& M);
  Matrix<_T> pseudoInverse();

  //---------------------------------------------------------------------
  /// \brief  Compute the rank of the matrix
  uint32_t rank() const;

  //---------------------------------------------------------------------
  /// \brief  Compute the norm of the matrix
  _T norm() const;

  //---------------------------------------------------------------------
  /// \brief  Compute the condition of the matrix
  _T cond() const;

  //---------------------------------------------------------------------
  /// \brief  Sorts the eigenvalues in D from highest to lowest,
  /// reflecting the changes in the V matrix
  void sort();

  //---------------------------------------------------------------------
  /// \brief Eigen-values
  const Vector<_T>& getD() const;
  Vector<_T>& getD();

  //---------------------------------------------------------------------
  /// \brief Eigen-vectors
  const Matrix<_T>& getV() const;
  Matrix<_T>& getV();

  //---------------------------------------------------------------------
  /// \brief Tolerance parameter functions for zeroing super-diagonal elements
  _T getETol() const;
  void setETol(_T tol);

  //---------------------------------------------------------------------
  /// \brief  Tolerance parameter functions for computing the rank of a matrix
  _T getRTol() const;
  void setRTol(_T tol);

  //---------------------------------------------------------------------
  /// \brief Maximum number of iterations (per singular value)
  uint32_t getMaxIts() const;
  void setMaxIts(uint32_t its);

  //---------------------------------------------------------------------
  /// \brief Status of the decomposition
  EigenSystemStatus::eType getStatus() const;

  //---------------------------------------------------------------------
  /// \brief  Test if the solver failed to decompose the matrix
  bool fail() const;

protected:
  // Householder processing class to reduce the matrix to tri-diagonal form
  Householder<_T>             m_House;

  // Givens processing class to diagonalize the tri-diagonal matrix
  Givens<_T>                  m_Givens; // Eigen-Values

  Matrix<_T>                  m_V; // Eigen-vectors

  // Maximum number of iterations
  uint32_t                        m_maxIts;

  /// Tolerance for the rank computation
  _T                          m_rTol;

  // Status of the decomposition
  EigenSystemStatus::eType    m_status;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_EIGEN_SYSTEM_SYM_H
