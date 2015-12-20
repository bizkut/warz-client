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
#ifndef NM_HOUSEHOLDER_H
#define NM_HOUSEHOLDER_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMVector.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// Base class for doing low level Householder transformation stuff
template <typename _T>
class Householder
{
public:
  //---------------------------------------------------------------------
  // Constructors / Destructor
  Householder(const Matrix<_T>& A);
  Householder(uint32_t m, uint32_t n);
  ~Householder();

  //---------------------------------------------------------------------
  /// \brief  Function computes the Householder transform vector. If x is a vector whose
  ///         elements x(2:n) we wish to zero by applying an orthogonal transformation y = P*x,
  ///         then P = I - beta*(v * v^T) is such a transform. where beta = 2 / (v^T * v)
  ///
  /// \param  n - The number of elements in the vector x
  /// \param  x - The input vector of length n
  /// \param  v - The Householder vector of length n. Note that the vector is normalised so
  ///             that its first element is always 1, with beta absorbing the scaling. This
  ///             then allows inplace storage within a vector\matrix that you have applied
  ///             the transform to.
  /// \return  The scalar factor beta
  static _T house(uint32_t n, const _T* x, _T* v);

  //---------------------------------------------------------------------
  /// \brief  Function computes the full Householder matrix P = I - beta*(v * v^T),
  ///         where beta = 2 / (v^T * v)
  static void house(Matrix<_T>& P, _T beta, const _T* v);

  //---------------------------------------------------------------------
  /// \brief  Function applies a Householder transform to eliminate elements in column i
  ///         below the sub-diagonal of size sub, placing the essential part of the
  ///         Householder vector back in the zeroed portion of the matrix and the beta
  ///         parameter in the betaL vector at position i
  void transformLeft(uint32_t i, uint32_t sub);

  //---------------------------------------------------------------------
  /// \brief  Function reduces the matrix to upper triangular form using Householder
  ///         transformations. The essential parts of the Householder vectors are placed
  ///         in the lower triangle of the decomposed matrix and the beta parameters
  ///         within the betaL vector
  void transformLeft(uint32_t sub);

  //---------------------------------------------------------------------
  /// \brief  Function applies a Householder transform to eliminate elements in row i
  ///         to the right of the super-diagonal of size super, placing the essential
  ///         part of the Householder vector back in the zeroed portion of the matrix
  ///         and the beta parameter in the betaR vector at position i
  void transformRight(uint32_t i, uint32_t super);

  //---------------------------------------------------------------------
  /// \brief  Function reduces the matrix to lower triangular form using Householder
  ///         transformations. The essential parts of the Householder vectors are placed
  ///         in the upper triangle of the decomposed matrix and the beta parameters
  ///         within the betaR vector
  void transformRight(uint32_t super);

  //---------------------------------------------------------------------
  /// \brief  Function applies symmetric Householder transforms P^T A P to eliminate
  ///         elements in both the i^th rows and columns with a specified band diagonal
  ///         size (A is symmetric). The essential parts of the Householder vector are
  ///         placed back in the zeroed portion of the matrix and the beta parameters
  ///         in the betaL, betaR vectors at position i
  void transformSym(uint32_t i, uint32_t band);

  //---------------------------------------------------------------------
  /// \brief  Function reduces the symmetric matrix A to Band-diagonal form using
  ///         Householder transformations. The essential parts of the Householder
  ///         vectors are placed in the zeroed portions of the decomposed matrix and
  ///         the beta parameters within the betaL, betaR vectors
  void transformSym(uint32_t band);

  //---------------------------------------------------------------------
  /// \brief  Function to recover the left orthogonal transformation by unrolling the
  ///         Householder transform vectors. i.e. if U^T A = R then the function recovers
  ///         the orthogonal matrix U.
  ///
  /// \param  U - The matrix to place the recovered orthogonal transformation into. The
  ///             matrix must have the same number of rows as A, though the number of
  ///             columns can vary between 1:n depending on the region you are interested
  ///             in. Computations within the dead zone to the right of a thin U are not
  ///             performed.
  ///
  /// \param  sub - The size of the matrix sub-diagonal band, below which the Householder
  ///               transform vectors are found.
  void left(Matrix<_T>& U, uint32_t sub);

  //---------------------------------------------------------------------
  /// \brief  Function to recover the full left orthogonal transformation by unrolling
  ///         the Householder transform vectors. i.e. if U^T A = R then the function
  ///         recovers the matrix U.
  ///
  /// \param  sub - The size of the matrix sub-diagonal band, below which the Householder
  ///               transform vectors are found.
  Matrix<_T> left(uint32_t sub);

  //---------------------------------------------------------------------
  /// \brief  Function to recover the right orthogonal transformation by unrolling the
  ///         Householder transform vectors. i.e. if A V = L then the function recovers
  ///         the matrix V.
  ///
  /// \param  V - The matrix to place the recovered orthogonal transformation into. The
  ///             matrix must have the same number of columns as A, though the number of
  ///             rows can vary between 1:m depending on the region you are interested
  ///             in. Computations within the dead zone below a thin V are not performed.
  ///
  /// \param  super - The size of the matrix super-diagonal band, to the right of which
  ///                 the Householder transform vectors are found.
  void right(Matrix<_T>& V, uint32_t super);

  //---------------------------------------------------------------------
  /// \brief  Function to recover the full right orthogonal transformation by unrolling
  ///         the Householder transform vectors. i.e. if A V = L then the function
  ///         recovers the matrix V.
  ///
  /// \param  super - The size of the matrix super-diagonal band, to the right of which
  ///                 the Householder transform vectors are found.
  Matrix<_T> right(uint32_t super);

  //---------------------------------------------------------------------
  /// \brief  Get the i^th left Householder transform vector
  void vectorLeft(Vector<_T>& v, uint32_t i, uint32_t sub) const;
  Vector<_T> vectorLeft(uint32_t i, uint32_t sub) const;

  //---------------------------------------------------------------------
  /// \brief  Get the i^th right Householder transform vector
  void vectorRight(Vector<_T>& v, uint32_t i, uint32_t super) const;
  Vector<_T> vectorRight(uint32_t i, uint32_t super) const;

  //---------------------------------------------------------------------
  /// \brief  Get the i^th left Householder beta scalar
  _T betaLeft(uint32_t i) const;

  //---------------------------------------------------------------------
  /// \brief  Get the i^th right Householder beta scalar
  _T betaRight(uint32_t i) const;

  //---------------------------------------------------------------------
  /// \brief  Clears the left and right orthogonal transform parameters
  void clearBetaLeft();
  void clearBetaRight();

  //---------------------------------------------------------------------
  /// \brief  Get the i^th left full Householder transform
  void PLeft(Matrix<_T>& P, uint32_t i, uint32_t sub);
  Matrix<_T> PLeft(uint32_t i, uint32_t sub);

  //---------------------------------------------------------------------
  /// \brief  Get the i^th right full Householder transform
  void PRight(Matrix<_T>& P, uint32_t i, uint32_t super);
  Matrix<_T> PRight(uint32_t i, uint32_t super);

  //---------------------------------------------------------------------
  /// \brief  Get the storage matrix
  Matrix<_T>& getA();
  const Matrix<_T>& getA() const;

  //---------------------------------------------------------------------
  /// \brief  Get the workspace memory
  Vector<_T>& getX();
  Vector<_T>& getV();
  Vector<_T>& getW();

protected:
  // Manipulation matrix
  Matrix<_T>      m_A;

  // Vector of stored beta parameters
  Vector<_T>      m_betaL;
  Vector<_T>      m_betaR;

protected:
  // Workspace memory: length max(m,n)
  Vector<_T>      m_x;
  Vector<_T>      m_v;
  Vector<_T>      m_w;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_HOUSEHOLDER_H
