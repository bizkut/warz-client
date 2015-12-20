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
#ifndef NM_GIVENS_H
#define NM_GIVENS_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMatrix.h"
#include "NMPlatform/NMVector.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// Base class for doing low level Givens transformation stuff on
// bidiagonal or symmetric tridiagonal matrices. Useful for Symmetric
// Eigen-decomposition or singular value decomposition.
template <typename _T>
class Givens
{
public:
  //---------------------------------------------------------------------
  // Constructors / Destructor
  Givens(const Matrix<_T>& A);
  Givens(const Vector<_T>& d, const Vector<_T>& s);
  Givens(uint32_t n);
  ~Givens();

  //---------------------------------------------------------------------
  /// \brief  Compute the parameters of a 2 by 2 Givens rotation U that zeros
  ///         the last element of the specified column vector [a; b].
  ///         U^T [a; b] = [r; 0]  where U = [cos(t) sin(t); -sin(t) cos(t)]
  static void givens(_T a, _T b, _T& c, _T& s);

  //---------------------------------------------------------------------
  /// \brief  Compute the parameters of a 2 by 2 Jacobi transformation D = V^T * A * V
  ///         that diagonalizes the symmetric matrix A. The resulting columns of V
  ///         contain the Eigen-vectors of the system and the elements of the diagonal
  ///         matrix correspond to the Eigen-values. V = [cos(t) sin(t); -sin(t) cos(t)]
  ///
  /// \param  aij - The elements A_{i,j} of the upper triagle of matrix A
  static void jacobi(_T a11, _T a12, _T a22, _T& c, _T& s);

  //---------------------------------------------------------------------
  /// \brief  Compute the implicit symmetric QR step with a Wilkinson shift
  ///         T' = V^T * T * V, where V = G_1 * ... * Gn is a product of Givens
  ///         rotations. Iterative calls to this function diagonalizes the
  ///         symmetric tri-diagonal matrix T. This reduction must be carried
  ///         on an unreduced tri-diagonal matrix, i.e. if any of the super-diagonal
  ///         elements are zero then the problem can be split into reducing two
  ///         smaller problems. If T = diag(T_11, T_22, D_33), where T_11 is the
  ///         largest tridiagonal matrix of reduced form, T_22 is unreduced and
  ///         D_33 is already diagonalized then this function makes a step to
  ///         diagonalize block T_22 specified by the indices p:q
  ///
  /// \param  p - Start index of the unreduced block T_22
  /// \param  q - Start index of the diagonalized block D_33
  ///
  /// \param  pV - Pointer to the orthogonal transform matrix to *update*
  ///              i.e. apply transforms to elements alreay in V. This
  ///              parameter can be NULL in which case V is not updated.
  void implicitQRStep(uint32_t p, uint32_t q, Matrix<_T>* pV = 0);

  //---------------------------------------------------------------------
  /// \brief  Compute the implicit QR step with a Wilkinson shift on a bi-diagonal
  ///         matrix B such that B' = U^T B V is a shifted bi-diagonal matrix
  ///         closer to being diagonal. U and V are products of Givens rotations
  ///         and V is essentially the orthogonal matrix that would be obtained
  ///         by applying implicitQRStep() to the tridiagonal matrix T = B^T B.
  ///         B must be unreduced since any zero super-diagonal element in B
  ///         decouples into two smaller sub-problems. This procedure is useful
  ///         for computing the SVD, since explicit formulation of T = B^T B is
  ///         unwise from a numerical standpoint.
  ///
  /// \param  p - Start index of the unreduced block B_22
  /// \param  q - Start index of the diagonalized block D_33
  ///
  /// \param  pU - Pointer to the left orthogonal transform matrix to *update*
  ///              i.e. apply transforms to elements alreay in U. This
  ///              parameter can be NULL in which case U is not updated.
  ///
  /// \param  pV - Pointer to the right orthogonal transform matrix to *update*
  ///              i.e. apply transforms to elements alreay in V. This
  ///              parameter can be NULL in which case V is not updated.
  void implicitSVDStep(uint32_t p, uint32_t q, Matrix<_T>* pU = 0, Matrix<_T>* pV = 0);

  //---------------------------------------------------------------------
  /// \brief  Function for chasing the specified super-diagonal element that has a
  ///         negligible corresponding diagonal element towards the right of the
  ///         matrix. This zeros out the entire specified row via a product of Givens
  ///         rotations to the left of the matrix: diag(B_11, 0, B'_33) = U^T * B.
  ///         Note that this operation modifies the diagonal and super-diagonal
  ///         elements in the next row to the one specified. This function is then
  ///         *only* useful for chasing elements within the body of an unreduced block
  ///         since it does not modify the already diagonalized matrix block.
  ///
  /// \param  i - The specified row to eliminate
  /// \param  q - Start index of the diagonalized block
  ///
  /// \param  pU - Pointer to the left orthogonal transform matrix to *update*
  ///              i.e. apply transforms to elements alreay in U. This
  ///              parameter can be NULL in which case U is not updated.
  void chaseLeft(uint32_t i, uint32_t q, Matrix<_T>* pU = 0);

  //---------------------------------------------------------------------
  /// \brief  Function for chasing the specified super-diagonal element that has a
  ///         negligible corresponding diagonal element towards the top of the
  ///         matrix. This zeros out the entire specified row via a product of Givens
  ///         rotations to the right of the matrix: diag(B'_11, 0, B_33) = B * V.
  ///         Note that this operation modifies the super-diagonal element
  ///         in the previous row to the one specified. This function is then
  ///         useful for chasing the tail element within an unreduced block
  ///         since it does not modify the already diagonalized matrix block.
  ///
  /// \param  i - The specified row to eliminate
  /// \param  p - Start index of the unreduced block
  ///
  /// \param  pU - Pointer to the left orthogonal transform matrix to *update*
  ///              i.e. apply transforms to elements alreay in U. This
  ///              parameter can be NULL in which case U is not updated.
  void chaseRight(uint32_t i, uint32_t p, Matrix<_T>* pV = 0);

  //---------------------------------------------------------------------
  /// \brief Diagonalize the symmetric tri-diagonal matrix: D = V^T * T * V by
  /// iteratively applying Givens rotations. The function essentially computes
  /// a symmetric Eigen-system.
  ///
  /// \param  max_its - The maximum number of iterations to allow for convergence
  ///                   of an Eigen-value.
  ///
  /// \param  pV - Pointer to the orthogonal transform matrix to *update*
  ///              i.e. apply transforms to elements alreay in V. This
  ///              parameter can be NULL in which case V is not updated.
  ///
  /// \return  true if all Eigen-values and Eigen-vectors were computed
  ///          successfully, false if any of the Eigen-values failed to
  ///          converge within the maximum number of iterations. In this
  ///          case the Eigen-values will be very close to correct, however
  ///          the Eigen-vectors may differ substantially.
  bool diagonalizeT(uint32_t max_its, Matrix<_T>* pV = 0);

  //---------------------------------------------------------------------
  /// \brief Diagonalize the bi-diagonal matrix: D = U^T * B * V by
  /// iteratively applying Givens rotations. This function essentially computes
  /// the singular value decomposition.
  ///
  /// \param  max_its - The maximum number of iterations to allow for convergence
  ///                   of a singular value.
  ///
  /// \param  pU - Pointer to the left orthogonal transform matrix to *update*
  ///              i.e. apply transforms to elements alreay in U. This
  ///              parameter can be NULL in which case U is not updated.
  ///
  /// \param  pV - Pointer to the right orthogonal transform matrix to *update*
  ///              i.e. apply transforms to elements alreay in V. This
  ///              parameter can be NULL in which case V is not updated.
  ///
  /// \return  true if all singular values and vectors were computed
  ///          successfully, false if any of the singular values failed to
  ///          converge within the maximum number of iterations. In this
  ///          case the singular values will be very close to correct, however
  ///          the singular vectors may differ substantially.
  bool diagonalizeB(uint32_t max_its, Matrix<_T>* pU = 0, Matrix<_T>* pV = 0);

  //---------------------------------------------------------------------
  // Get the matrix diagonal vectors
  Vector<_T>& getD();
  const Vector<_T>& getD() const;

  Vector<_T>& getS();
  const Vector<_T>& getS() const;

  //---------------------------------------------------------------------
  // Get and set diagonal vectors from a matrix
  void getT(Matrix<_T>& T) const;
  void setT(const Matrix<_T>& T);
  void setT(const Vector<_T>& d, const Vector<_T>& s);

  //---------------------------------------------------------------------
  // Tolerance parameter functions for zeroing super-diagonal elements
  _T getETol() const;
  void setETol(_T tol);

protected:
  // Main and super-diagonal matrix elements as vectors
  Vector<_T>      m_d; // main
  Vector<_T>      m_s; // super

  // Tolerance factor for zeroing super-diagonal elements
  _T              m_eTol;

protected:
  // Workspace memory: length n
  Vector<_T>      m_x;
  Vector<_T>      m_v;
  Vector<_T>      m_z;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_GIVENS_H
