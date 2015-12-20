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
#ifndef NM_NONLINEAR_OPTIMISER_SPARSE_BAND_DIAG_H
#define NM_NONLINEAR_OPTIMISER_SPARSE_BAND_DIAG_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMBandDiagMatrix.h"
#include "NMNumerics/NMBandDiagSolverCholesky.h"
#include "NMNumerics/NMNonLinearOptimiserBase.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief This class represents the row and column offset information for
/// a data block within the Jacobian matrix
class JacobianBlockDiagInfo
{
public:
  JacobianBlockDiagInfo();
  ~JacobianBlockDiagInfo();

  void clear();

public:
  uint32_t m_jRow;        ///< Row offset within the full Jacobian matrix
  uint32_t m_jColumn;     ///< Column offset within the full Jacobian matrix
  uint32_t m_jSize;       ///< Row size of the Jacobian block
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief This class represents a sparsely structured Jacobian matrix which have constant
/// column width blocks in an almost block diagonal form. The set of blocks are staggered
/// beneath each other with only a single block in each row set. The column offsets of
/// each consecutive data block within the Jacobian is assumed to be strictly increasing.
template <typename _T>
class JacobianBlockDiag : public JacobianBase<_T>
{
public:
  JacobianBlockDiag(uint32_t numRows, uint32_t numColumns, uint32_t numBlocks, uint32_t blockWidth);
  ~JacobianBlockDiag();

  const NMP::Matrix<_T>& getMatrix() const { return m_matrix; }
  NMP::Matrix<_T>& getMatrix() { return m_matrix; }

  void setBlockInfo(
    uint32_t i,
    uint32_t jRow,
    uint32_t jColumn,
    uint32_t jSize);

  uint32_t getNumBlocks() const { return m_numBlocks; }
  uint32_t getBlockWidth() const { return m_matrix.numColumns(); }
  const JacobianBlockDiagInfo* getBlockInfo() const { return m_blockInfo; }

  void getBlocksInColumn(uint32_t col, uint32_t& startBlock, uint32_t& endBlock) const;

  void makeFull(NMP::Matrix<_T>& result) const;

protected:
  NMP::Matrix<_T> m_matrix; ///< Jacobian blocks are stored as a (numRows by blockWidth) matrix

  uint32_t               m_numBlocks;
  JacobianBlockDiagInfo* m_blockInfo;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief This class represents a band-diagonal Hessian matrix which stores only the
/// upper triangular part of the matrix.
template <typename _T>
class HessianBandDiag : public HessianBase<_T>
{
public:
  HessianBandDiag(uint32_t size, uint32_t super);
  ~HessianBandDiag();

  const NMP::BandDiagMatrix<_T>& getMatrix() const { return m_matrix; }
  NMP::BandDiagMatrix<_T>& getMatrix() { return m_matrix; }

protected:
  NMP::BandDiagMatrix<_T> m_matrix;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Sparse Levenberg-Marquardt method for non-linear optimisation using
/// stepsize control with a cubic polynomial. The Levenberg-Marquardt
/// algorithm solves the non-linear least squares problem:
///
/// fResNorm = sum( f(x : p) - y )^2
///
/// to find the parameter variables p that minimises the sum of squares
///
/// Least Squares Function:
/// The user supplies a least squares function that computes the vector of
/// residuals r = f(x : p) - y, and the Jacobian of the objective function
/// J = D( f(x : p) ) / Dp
///
/// THE USER FUNCTION SHOULD NOT SQUARE THE RESIDUALS SINCE THIS IS
/// IMPLICITLY APPLIED WITHIN THE MINIMISATION ALGORITHM ITSELF
///
/// The least squares user function must be derived by computing an analytical
/// Jacobian with NonLinearLeastSquaresFuncJac. This optimiser does not support
/// building the sparse Jacobian by finite difference approximation.
template <typename _T>
class NonLinearOptimiserBandDiag : public NonLinearOptimiserBase<_T>
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory and register the user defined
  /// cost function with the optimiser
  //---------------------------------------------------------------------
  //@{
  NonLinearOptimiserBandDiag(
    uint32_t                          numBlocks,
    uint32_t                          blockWidth,
    NonLinearLeastSquaresFuncJac<_T>& funcJac);

  // Destructor
  ~NonLinearOptimiserBandDiag();
  //@}

protected:
  //---------------------------------------------------------------------
  /// \name   Core computation functions
  /// \brief  Functions to compute and solve the Hessian matrix to recover
  /// the search direction. These functions can be overridden to implement
  /// a sparse version of the optimisation algorithm.
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  virtual bool computeJByFiniteDifference(
    const NMP::Vector<_T>&         P,
    const NMP::Vector<_T>&         fdDeltas,
    const NMP::Vector<_T>&         fRes,
    NonLinearLeastSquaresFunc<_T>& lsqFn,
    NMP::JacobianBase<_T>*         J); // Does not support this

  //---------------------------------------------------------------------
  virtual void getJabsRowSums(
    uint32_t                     numResiduals,
    const NMP::JacobianBase<_T>* J,
    NMP::Vector<_T>&             result); // Does not support this

  //---------------------------------------------------------------------
  virtual void computeJtr(
    uint32_t                     numResiduals,
    const NMP::JacobianBase<_T>* J,
    const NMP::Vector<_T>&       r,
    NMP::Vector<_T>&             result);

  //---------------------------------------------------------------------
  virtual void computeJb(
    uint32_t                     numResiduals,
    const NMP::JacobianBase<_T>* J,
    const NMP::Vector<_T>&       b,
    NMP::Vector<_T>&             result);

  //---------------------------------------------------------------------
  virtual void computeJtJ(
    uint32_t                     numResiduals,
    const NMP::JacobianBase<_T>* J,
    NMP::HessianBase<_T>*        H);

  //---------------------------------------------------------------------
  virtual void augmentJtJ(
    NMP::HessianBase<_T>* H,
    _T                    lambda);

  //---------------------------------------------------------------------
  virtual void solve(
    const NMP::HessianBase<_T>* H,
    const NMP::Vector<_T>&      b,
    NMP::Vector<_T>&            searchDir);
  //@}

protected:
  // Band-diagonal symmetric linear system solver: A x = b
  NMP::BandDiagSolverCholesky<_T> m_solver;

  // Temporary workspace vector
  NMP::Vector<_T> m_temp;
  NMP::Vector<_T> m_temp2;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_NONLINEAR_OPTIMISER_SPARSE_BAND_DIAG_H
