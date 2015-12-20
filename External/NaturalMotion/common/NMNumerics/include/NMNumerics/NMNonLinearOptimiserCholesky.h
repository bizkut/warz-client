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
#ifndef NM_NONLINEAR_OPTIMISER_CHOLESKY_H
#define NM_NONLINEAR_OPTIMISER_CHOLESKY_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMNumerics/NMSolverCholesky.h"
#include "NMNumerics/NMNonLinearOptimiser.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Levenberg-Marquardt (full rank Cholesky method) for non-linear
/// optimisation using stepsize control with a cubic polynomial. The
/// Levenberg-Marquardt algorithm solves the non-linear least squares problem:
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
/// The least squares user function must be derived from one of the function
/// computation classes NonLinearLeastSquaresFunc, NonLinearLeastSquaresFuncJac
/// depending on whether your function computes an analytical Jacobian or if
/// you wish the Jacobian to be built by the algorithm by finite difference
/// approximation.
template <typename _T>
class NonLinearOptimiserCholesky : public NonLinearOptimiserBase<_T>
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory and register the user defined
  /// cost function with the optimiser
  //---------------------------------------------------------------------
  //@{
  NonLinearOptimiserCholesky(NonLinearLeastSquaresFunc<_T>& func);
  NonLinearOptimiserCholesky(NonLinearLeastSquaresFuncJac<_T>& funcJac);

  // Destructor
  ~NonLinearOptimiserCholesky();
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
    NMP::JacobianBase<_T>*         J);

  //---------------------------------------------------------------------
  virtual void getJabsRowSums(
    uint32_t                     numResiduals,
    const NMP::JacobianBase<_T>* J,
    NMP::Vector<_T>&             result);

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
  // Symmetric linear system solver: A x = b
  NMP::SolverCholesky<_T> m_solver;

  // Workspace vector for modifying the parameter vector
  NMP::Vector<_T> m_Pfd;
  NMP::Vector<_T> m_fRes_;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_NONLINEAR_OPTIMISER_CHOLESKY_H
