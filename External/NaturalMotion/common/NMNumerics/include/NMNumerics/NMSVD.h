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
#ifndef NM_SVD_H
#define NM_SVD_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMHouseholder.h"
#include "NMNumerics/NMGivens.h"
#include "NMNumerics/NMBidiagonalizer.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Used to return the status of the decomposition
class SVDStatus
{
public:
  enum eType {
    Success,
    MaxIts,
    BadParams
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Class to perform Singular Value Decomposition of a matrix: D = U^T * A * V
/// Singular values are positive and stored with decreasing value within D.
template <typename _T>
class SVD
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory or decompose a matrix
  //---------------------------------------------------------------------
  //@{
  SVD(const Matrix<_T>& A, bool wantU = true, bool wantV = true);
  SVD(uint32_t m, uint32_t n);

  // Destuctor
  ~SVD();
  //@}

  //---------------------------------------------------------------------
  /// \name   Solve functions
  /// \brief  Functions to decompose / recompose a matrix, solve linear
  ///         systems of equations or compute a matrix inverse
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Main matrix decomposition routine that computes the singular
  /// values and optionally the left and right orthogonal transforms.
  SVDStatus::eType compute(const Matrix<_T>& A, bool wantU = true, bool wantV = true);

  //---------------------------------------------------------------------
  /// \brief  Solve the linear system A x = b (in least squares sense)
  void solve(Matrix<_T>& X, const Matrix<_T>& B);
  void solve(Vector<_T>& x, const Vector<_T>& b);

  //---------------------------------------------------------------------
  /// \brief  Compute the pseudo-inverse of the matrix: A^+ = V D^+ U^T
  void pseudoInverse(Matrix<_T>& M);
  Matrix<_T> pseudoInverse();

  //---------------------------------------------------------------------
  /// \brief  Recompose the matrix A = U * D * V^T
  void recompose(Matrix<_T>& A);
  Matrix<_T> recompose();

  //---------------------------------------------------------------------
  /// \brief  Recompose the matrix A = U * I * V^T
  void recomposeUVt(Matrix<_T>& A);
  Matrix<_T> recomposeUVt();
  //@}

  //---------------------------------------------------------------------
  /// \name   Singular Values
  /// \brief  Functions that work on the singular values
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Compute the rank of the matrix
  uint32_t rank() const;

  //---------------------------------------------------------------------
  /// \brief  Compute the 2-norm of the matrix: i.e. largest singular value.
  _T norm() const;

  //---------------------------------------------------------------------
  /// \brief  Compute the condition of the matrix. i.e the ratio of the largest
  /// singular value to the smallest.
  _T cond() const;

  //---------------------------------------------------------------------
  /// \brief  Compute the reciprocal of the matrix condition. i.e the ratio of the
  /// smallest singular value to the largest.
  _T rcond() const;

  //---------------------------------------------------------------------
  /// \brief  Zero the singular values below the given threshold
  void zeroAbsolute(_T tol);

  //---------------------------------------------------------------------
  /// \brief  Zero the singular values below the rank tolerance threshold
  void zeroAbsolute();

  //---------------------------------------------------------------------
  /// \brief  Zero the singular values below the specified threshold given by
  /// multiplying the tolerance factor with the largest singular value.
  void zeroRelative(_T tol);
  //@}

  //---------------------------------------------------------------------
  /// \name   Solver Data
  /// \brief  Functions get the decomposed matrix data, status of the solve
  ///         and the solver parameters.
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Function to get the placeholder for the initial A matrix.
  /// This can be passed to the compute routine though is destroyed by
  /// the bi-diagonalization process.
  Matrix<_T>& getA();

  //---------------------------------------------------------------------
  /// \brief  Left orthogonal transform
  const Matrix<_T>& getU() const;
  Matrix<_T>& getU();

  //---------------------------------------------------------------------
  /// \brief  Singular-values
  const Vector<_T>& getD() const;
  Vector<_T>& getD();

  //---------------------------------------------------------------------
  /// \brief  Right orthogonal transform
  const Matrix<_T>& getV() const;
  Matrix<_T>& getV();

  //---------------------------------------------------------------------
  /// \brief  Principal vectors in V
  Vector<_T> getV(uint32_t i) const;
  void getV(uint32_t i, Vector<_T>& v) const;

  //---------------------------------------------------------------------
  /// \brief  Get the vector in V corresponding to the smallest singular value
  void nullVector(Vector<_T>& v) const;
  Vector<_T> nullVector() const;

  //---------------------------------------------------------------------
  /// \brief  Tolerance parameter functions for zeroing super-diagonal elements
  _T getETol() const;
  void setETol(_T tol);

  //---------------------------------------------------------------------
  /// \brief  Tolerance parameter functions for computing the rank of a matrix
  _T getRTol() const;
  void setRTol(_T tol);

  //---------------------------------------------------------------------
  /// \brief  Maximum number of iterations (per singular value)
  uint32_t getMaxIts() const;
  void setMaxIts(uint32_t its);

  //---------------------------------------------------------------------
  /// \brief  Status of the decomposition
  SVDStatus::eType getStatus() const;

  //---------------------------------------------------------------------
  /// \brief  Test if the solver failed to decompose the matrix
  bool fail() const;
  //@}

protected:
  /// Reduction to bi-diagonal form
  Bidiagonalizer<_T>        m_Bidiag;

  /// Diagonalization
  Givens<_T>                m_Givens;

  /// Left orthogonal matrix U
  Matrix<_T>                m_U;

  /// Right orthogonal matrix V
  Matrix<_T>                m_V;

  /// Tolerance for the rank computation
  _T                        m_rTol;

  /// Maximum number of iterations
  uint32_t                      m_maxIts;

  /// Status of the decomposition
  SVDStatus::eType          m_status;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_SVD_H
