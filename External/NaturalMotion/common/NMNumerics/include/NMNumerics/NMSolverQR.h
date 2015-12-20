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
#ifndef NM_SOLVER_QR_H
#define NM_SOLVER_QR_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMVector.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Used to return the status of the solver
class SolverQRStatus
{
public:
  enum eType {
    Success,
    Singular,
    BadParams
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Block sparse linear least squares solver.
///
/// Can be used to solve simple linear systems A X = B, where the system
/// is either square, over determined (rows > cols) or fixed width
/// block sparse (one block per row segment in A, each block may have
/// a different row segment depth). In the sparse case, since all blocks have
/// fixed column width, then A can be stored as a single matrix corresponding
/// to the set of stacked block segments. The block row depths are encoded
/// by an integer vector 'sizes' and the column offsets of the blocks within
/// A are encoded by an integer vector 'offsets'. It is assumed that the
/// blocks stored in A have been stacked, sorted by increasing column offset.
///
/// The residual r(X) of the over determined linear system A X = B
/// has the form: r(X) = B - A X. The least squares solution can be found
/// via orthogonal transformation Q so that the transformed matrix is upper
/// triangular. Q r(X) = Q B - Q A X  =  [u; v] - [R; 0] X
/// An orthogonal transformation does not change the 2-norm:
/// || r(X) ||^2 = || Q r(X) ||^2 = || u - R X ||^2 + || v - 0 ||^2
/// This is minimum when u - R X = 0, so the minimizer X~ can be found by
/// back substitution in the upper triangular system R X~ = u
///
/// The orthogonal transformations are done by performing Householder
/// reflections to both sides of the system A, B. We therefore do not explicitly
/// need to store Q, which could potentially be very large, thus the memory
/// footprint required for the solve is small and can be preallocated up
/// front for repeated solves.
///
template <typename _T>
class SolverQR
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to compute or preallocate memory for a linear system
  ///
  ///         Preallocation:
  ///         Ar - rows in A, Ac - columns in A,
  ///         Bc = columns in B
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the solution of a simple linear
  /// system of equations A X = I. i.e. compute the inverse of the
  /// input matrix A. If matrix A is not square then the pseudo-inverse
  /// X = (A^T A)^-1 A^T is computed
  SolverQR(const Matrix<_T>& A);
  SolverQR(uint32_t Ar, uint32_t Ac);

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the solution of a simple linear
  /// system of equations. i.e. compute the solution vectors X for the
  /// possibly over determined linear system A X = B
  SolverQR(const Matrix<_T>& A, const Matrix<_T>& B);
  SolverQR(uint32_t Ar, uint32_t Ac, uint32_t Bc);

  //---------------------------------------------------------------------
  /// \brief  Constructor to compute the solution of a sparse linear
  /// system of equations. i.e. compute the solution vectors X for the
  /// possibly over determined linear system A X = B, where A is input
  /// as a stacked set of matrix blocks (each with same width and one
  /// block per segment). The number of rows in each block is specified
  /// by the vector sizes, and the corresponding column offset of each
  /// block by the vector offsets. The offset vector is assumed to be
  /// sorted with increasing indices within matrix A
  SolverQR(const Matrix<_T>& A, const Matrix<_T>& B,
           const Vector<int32_t>& sizes, const Vector<int32_t>& offsets);

  //---------------------------------------------------------------------
  /// \brief  Constructors to preallocate memory for a linear system given
  /// the known block offset indices or known solution size Xr
  ///
  /// \param  Ar - the number of rows in the design matrix A
  /// \param  Ac - the fixed width block size of each block segment
  /// \param  Bc - the number of dimensions of the sample vectors
  /// \param  numBlocks - the number of sparse data segment blocks in A
  /// \param  offsets - the set of column index offsets for each sparse block
  /// \param  Xr - rows in the solution X, i.e. actual full column size of matrix A
  SolverQR(uint32_t Ar, uint32_t Ac, uint32_t Bc, const Vector<int32_t>& offsets);
  SolverQR(uint32_t Ar, uint32_t Ac, uint32_t Bc, uint32_t numBlocks, uint32_t Xr);

  //---------------------------------------------------------------------
  /// Destructor
  ~SolverQR();
  //@}

  //---------------------------------------------------------------------
  /// \name   Repeat Solve functions
  /// \brief  These functions provide a way to repeatedly solve similar tri-diagonal
  /// linear systems. i.e. the A, X and B matrices have already been preallocated by
  /// the corresponding constructor and will always have the same sizes.
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Function to compute the solution of a simple linear
  /// system of equations A X = I. i.e. compute the inverse of the
  /// input matrix A. If matrix A is not square then the pseudo-inverse
  /// X = (A^T A)^-1 A^T is computed
  SolverQRStatus::eType solve(const Matrix<_T>& A);

  //---------------------------------------------------------------------
  /// \brief  Function to compute the solution of a simple linear
  /// system of equations. i.e. compute the solution vectors X for the
  /// possibly over determined linear system A X = B
  SolverQRStatus::eType solve(const Matrix<_T>& A, const Matrix<_T>& B);

  //---------------------------------------------------------------------
  /// \brief  Function to compute the solution of a sparse linear
  /// system of equations. i.e. compute the solution vectors X for the
  /// possibly over determined linear system A X = B, where A is input
  /// as a stacked set of matrix blocks (each with same width and one
  /// block per segment). The number of rows in each block is specified
  /// by the vector sizes, and the corresponding column offset of each
  /// block by the vector offsets. The offset vector is assumed to be
  /// sorted with increasing indices within matrix A
  SolverQRStatus::eType solve(
    const Matrix<_T>& A, const Matrix<_T>& B,
    const Vector<int32_t>& sizes, const Vector<int32_t>& offsets);
  //@}

  //---------------------------------------------------------------------
  /// \name   Solver Data
  /// \brief  Functions to get information or matrix data
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Get the A block matrix
  const Matrix<_T>& getA() const;
  Matrix<_T>& getA();

  //---------------------------------------------------------------------
  /// \brief  Get the B block matrix
  const Matrix<_T>& getB() const;
  Matrix<_T>& getB();

  //---------------------------------------------------------------------
  /// \brief  Get the solution matrix
  const Matrix<_T>& getX() const;
  Matrix<_T>& getX();

  //---------------------------------------------------------------------
  /// \brief  Get the vector of block sizes of A
  const Vector<int32_t>& getSizes() const;
  Vector<int32_t>& getSizes();

  //---------------------------------------------------------------------
  /// \brief  Get the vector of index offsets of A
  const Vector<int32_t>& getOffsets() const;
  Vector<int32_t>& getOffsets();

  //---------------------------------------------------------------------
  /// \brief  Get the rank of the matrix A
  uint32_t getRank() const;

  //---------------------------------------------------------------------
  /// \brief  Get the status of the solve
  SolverQRStatus::eType getStatus() const;

  //---------------------------------------------------------------------
  /// \brief  Test if the solver failed
  bool fail() const;

  //---------------------------------------------------------------------
  /// \brief  Machine precision tolerance for singularity
  _T getETol() const;
  void setETol(_T eps);
  //@}

protected:
  //---------------------------------------------------------------------
  /// \brief  Performs a sparse linear solve to compute X.
  /// (Main solve routine that does not allocate memory)
  SolverQRStatus::eType doSolve(Matrix<_T>& X);

  //---------------------------------------------------------------------
  /// \brief  Initialize default values for the solver
  void init();

private:
  /// Data matrices
  Matrix<_T>        m_A;
  Matrix<_T>        m_X;
  Matrix<_T>        m_B;

  /// Block size and offset vectors
  Vector<int32_t>      m_sizes;
  Vector<int32_t>      m_offsets;

  /// Matrix property info
  uint32_t              m_rank;

  /// Machine precision tolerance for singular matrix
  _T                m_eTol;

  /// Computation status
  SolverQRStatus::eType   m_status;

private:
  /// Temporary workspace memory required for the solve
  Vector<_T>        m_v;
  Vector<_T>        m_vc;
  Vector<_T>        m_p;
  Vector<int32_t>   m_diffs;

  /// Temporary workspace memory required for pseudo-inverse
  Matrix<_T>        m_At;
  Matrix<_T>        m_iAtA;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_SOLVER_QR_H
