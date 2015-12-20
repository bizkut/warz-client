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
#ifndef NM_NONLINEAR_OPTIMISER_BASE_H
#define NM_NONLINEAR_OPTIMISER_BASE_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector.h"

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// Forward declaration
template <typename _T>
class NonLinearOptimiserBase;

//----------------------------------------------------------------------------------------------------------------------
/// \brief Base class that represents a Jacobian matrix. Sparse types of Jacobian
/// matrix can inherit from this base class an implement the appropriate storage.
template <typename _T>
class JacobianBase
{
public:
  JacobianBase(uint32_t numRows, uint32_t numColumns);
  virtual ~JacobianBase();

  uint32_t getNumRows() const { return m_numRows; }
  uint32_t getNumColumns() const { return m_numColumns; }

protected:
  uint32_t m_numRows;
  uint32_t m_numColumns;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Base class that represents the Hessian matrix J^T * J. Sparse types of
/// Jacobian lead to a sparsely structured Hessian. Sparse implementations of the
/// Hessian matrix can implement the appropriate storage.
template <typename _T>
class HessianBase
{
public:
  HessianBase(uint32_t size);
  virtual ~HessianBase();

  uint32_t getSize() const { return m_size; }

protected:
  uint32_t m_size;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Used to return the status of the optimisation
class NonLinearOptimiserStatus
{
public:
  enum eType {
    Converged,
    DidNotConverge,
    TerminatedOnMaxIts,
    TerminatedByUser,
    Invalid
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Used to specify how the Jacobian of the user function is computed
class NonLinearOptimiserFuncType
{
public:
  enum eType {
    Func,   // Computes Jacobian by finite difference
    FuncJac // Computes Jacobian analytically
  };
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Base class for user defined least squares functions for NonLinearOptimiser.
/// This base class manages the preallocated maximum and current sizes for the function
/// residuals. This allows you to preallocate a specified maximum of function residuals
/// and change the working number of residuals between successive optimisations
class NonLinearLeastSquaresFuncBase
{
public:
  // Constructor / Destructor
  NonLinearLeastSquaresFuncBase(NonLinearOptimiserFuncType::eType funcType, uint32_t numUnknowns, uint32_t maxNumResiduals);
  virtual ~NonLinearLeastSquaresFuncBase();

  // Information
  NonLinearOptimiserFuncType::eType getFuncType() const { return m_funcType; }
  uint32_t getNumUnknowns() const { return m_numUnknowns; }

  // Residuals functions
  uint32_t getMaxNumResiduals() const { return m_maxNumResiduals; }
  uint32_t getNumResiduals() const { return m_numResiduals; }
  void setNumResiduals(uint32_t numResiduals) { m_numResiduals = numResiduals; }

protected:
  NonLinearOptimiserFuncType::eType m_funcType;

  uint32_t m_numUnknowns;
  uint32_t m_maxNumResiduals;
  uint32_t m_numResiduals;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Base class for a user defined least squares function for
/// computing the Jacobian by using finite difference approximation.
template <typename _T>
class NonLinearLeastSquaresFunc : public NonLinearLeastSquaresFuncBase
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to initialise the number of parameter unknowns
  /// and the maximum number of function residual errors
  //---------------------------------------------------------------------
  //@{
  NonLinearLeastSquaresFunc(uint32_t numUnknowns, uint32_t maxNumResiduals);
  virtual ~NonLinearLeastSquaresFunc();
  //@}

  //---------------------------------------------------------------------
  /// \name   User defined function
  /// \brief  This user function is called by the Levenberg-Marquardt
  /// non-linear optimiser in order to evaluate the residual errors
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  User function for computing the residual function error vector.
  /// The Levenberg-Marquardt algorithm solves the non-linear least squares problem:
  ///
  /// fResNorm = sum( f(x : p) - y )^2
  ///
  /// to find the parameter variables p that minimises the sum of squares
  ///
  /// This computation function should compute the residual function errors:
  ///   fRes = f(x : p) - y
  /// i.e. The difference between user defined function with internal data samples x
  /// evaluated at the set of unknown parameters p, and the measured function data
  /// samples y. The data samples x, y are entirely dependent on the problem and may
  /// be empty or implied as zero. It is the responsibility of the user function to
  /// maintain the memory for these data samples.
  ///
  /// Note: THE USER FUNCTION SHOULD NOT SQUARE THE RESIDUAL ERRORS SINCE
  /// THIS IS IMPLICITLY APPLIED WITHIN THE MINIMISATION ALGORITHM ITSELF.
  ///
  /// The corresponding Jacobian of the function f(x : p) is computed by the optimiser
  /// by finite difference approximation
  ///
  /// \param  optimiser - A reference to the non-linear optimiser that invoked
  ///         the call to this function. This enables you to obtain information
  ///         about the current state of the optimiser or information such as
  ///         the current iteration.
  ///
  /// \param  P - The current unknowns parameter vector used to evaluate the function
  ///
  /// \param  fRes - The residual function error vector of size maxNumResiduals. The
  ///         user function should compute the numResiduals number of function
  ///         residual errors: fRes = f(x : p) - y
  virtual bool func(
    const NMP::NonLinearOptimiserBase<_T>& optimiser,
    const NMP::Vector<_T>&                 P,
    NMP::Vector<_T>&                       fRes) = 0;
  //@}
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Base class for user defined least squares function that
/// computes the Jacobian analytically
template <typename _T>
class NonLinearLeastSquaresFuncJac : public NonLinearLeastSquaresFuncBase
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to initialise the number of parameter unknowns
  /// and the maximum number of function residual errors
  //---------------------------------------------------------------------
  //@{
  NonLinearLeastSquaresFuncJac(uint32_t numUnknowns, uint32_t maxNumResiduals);
  virtual ~NonLinearLeastSquaresFuncJac();
  //@}

  //---------------------------------------------------------------------
  /// \name   User defined function
  /// \brief  This user function is called by the Levenberg-Marquardt
  /// non-linear optimiser in order to evaluate the residual errors and
  /// the function Jacobian
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  User function for computing the residual function error vector
  /// and the Jacobian matrix
  ///
  /// The Levenberg-Marquardt algorithm solves the non-linear least squares problem:
  ///
  /// fResNorm = sum( f(x : p) - y )^2
  ///
  /// to find the parameter variables p that minimises the sum of squares
  ///
  /// This computation function should compute the residual function errors:
  ///   fRes = f(x : p) - y
  /// and the Jacobian:
  ///   J = Df(x : p) / Dp
  /// i.e. The difference between user defined function with internal data samples x
  /// evaluated at the set of unknown parameters p, and the measured function data
  /// samples y. The data samples x, y are entirely dependent on the problem and may
  /// be empty or implied as zero. It is the responsibility of the user function to
  /// maintain the memory for these data samples.
  ///
  /// Note: THE USER FUNCTION SHOULD NOT SQUARE THE RESIDUAL ERRORS SINCE
  /// THIS IS IMPLICITLY APPLIED WITHIN THE MINIMISATION ALGORITHM ITSELF.
  ///
  /// \param  optimiser - A reference to the non-linear optimiser that invoked
  ///         the call to this function. This enables you to obtain information
  ///         about the current state of the optimiser or information such as
  ///         the current iteration.
  ///
  /// \param  P - The current unknowns parameter vector used to evaluate the function
  ///
  /// \param  fRes - The residual function error vector of size maxNumResiduals. The
  ///         user function should compute the numResiduals number of function
  ///         residual errors: fRes = f(x : p) - y
  ///
  /// \param  J - The Jacobian matrix of size (maxNumResiduals by numUnknowns). The
  ///         user function should compute the (numResiduals by numUnknowns) number
  ///         of function partial derivatives Df(x : p) / Dp
  virtual bool func(
    const NMP::NonLinearOptimiserBase<_T>& optimiser,
    const NMP::Vector<_T>&                 P,
    NMP::Vector<_T>&                       fRes,
    NMP::JacobianBase<_T>*                 J) = 0;
  //@}
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Levenberg-Marquardt method for non-linear optimisation using
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
/// The least squares user function must be derived from one of the function
/// computation classes NonLinearLeastSquaresFunc, NonLinearLeastSquaresFuncJac
/// depending on whether your function computes an analytical Jacobian or if
/// you wish the Jacobian to be built by the algorithm by finite difference
/// approximation.
///
/// Algorithm:
/// The Levenberg-Marquardt method computes an initial search direction by
/// solving the linear system:
///
/// ( J^T * J + lambda*I ) * dp = -(J^T * r)
///
///  J - Is the (numResiduals by numUnknowns) Jacobian matrix of the objective
///      function f(x : p)
///
///  r - Is the numResiduals vector of residual errors between the objective
///      function and the data: r = f(x) - y
///
/// lambda - Is the damping factor that heuristically biases the Hessian
///          (J^T * J) in order to solve the parameter update vector dp as a
///          blend between Newton iteration (dp is the vector that moves the
///          variables directly to the minimum of the quadratic approximation)
///          and steepest gradient descent
///
/// dp is used as the search direction along which we find the minimum. The
/// step size is initially set to 1 and a further function evaluation
/// determines a sum of squares function error and the function gradient. A
/// cubic function is then fitted to the old and new function values and the
/// two gradients along the search direction in order to find a better estimate
/// of the minimum. If the sum of squares error is reduced then a new search
/// direction is computed, the damping parameter lambda updated and the step
/// size reset to 1, otherwise the step size is changed depending on the
/// function gradient. This process is repeated until the function has
/// converged.
template <typename _T>
class NonLinearOptimiserBase
{
public:
  //---------------------------------------------------------------------
  /// \name   Constructors
  /// \brief  Functions to preallocate memory and register the user defined
  /// cost function with the optimiser
  //---------------------------------------------------------------------
  //@{
  NonLinearOptimiserBase(NonLinearLeastSquaresFunc<_T>& func);
  NonLinearOptimiserBase(NonLinearLeastSquaresFuncJac<_T>& funcJac);

  // Destructor
  virtual ~NonLinearOptimiserBase();
  //@}

  //---------------------------------------------------------------------
  /// \name   Solve functions
  /// \brief  Computation functions to peform non-linear optimisation
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Main computation function to peform non-linear optimisation of the
  /// unknown parameters vector with the specified user function
  ///
  /// \param  P - The initial guess for the parameter vector
  NonLinearOptimiserStatus::eType compute(const NMP::Vector<_T>& P);
  //@}

  //---------------------------------------------------------------------
  /// \name   Information functions
  /// \brief  Functions to retrieve data vectors, iterations and the
  /// current state of the optimiser
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  /// \brief  Function to retrieve the optimised parameter vector
  const NMP::Vector<_T>& getP() const;

  //---------------------------------------------------------------------
  /// \brief Number of iteration steps performed by the optimiser
  uint32_t getNumIterations() const;

  //---------------------------------------------------------------------
  /// \brief Number of function evaluations of the user function performed
  /// by the optimiser. The number of function evaluations will differ from
  /// the number of iterations when the Jacobian is computed using finite
  /// difference approximation
  uint32_t getNumFuncEvals() const;

  //---------------------------------------------------------------------
  /// \brief  Function to retrieve the current state of the optimiser. This
  /// is useful for determining if the current call to the user function
  /// is part of the finite difference approximation step to compute the
  /// Jacobian or whether it is part of the new update step
  bool isFiniteDifferencing() const;

  //---------------------------------------------------------------------
  /// \brief  Function to return the status of the optimisation
  NonLinearOptimiserStatus::eType getStatus() const;
  //@}

  //---------------------------------------------------------------------
  /// \name   Tolerance functions
  /// \brief  Functions to set the tolerance options for the optimiser
  //---------------------------------------------------------------------
  //@{

  //---------------------------------------------------------------------
  // Iteration tolerances
  uint32_t getMaxIterations() const;
  void setMaxIterations(uint32_t n);
  uint32_t getMaxFuncEvals() const;
  void setMaxFuncEvals(uint32_t n);

  //---------------------------------------------------------------------
  // Termination tolerances
  _T getTolP() const;
  void setTolP(_T tolP);
  _T getTolFunc() const;
  void setTolFunc(_T tolFunc);

  //---------------------------------------------------------------------
  // Finite difference control
  _T getFiniteDiffMinChange() const;
  void setFiniteDiffMinChange(_T finiteDiffMinChange);
  _T getFiniteDiffMaxChange() const;
  void setFiniteDiffMaxChange(_T finiteDiffMaxChange);
  //@}

protected:
  NonLinearOptimiserBase& operator=(const NonLinearOptimiserBase& rhs);

  //---------------------------------------------------------------------
  /// \brief  Function to cubicly interpolate two points and their gradients to estimate
  /// the minimum of a function along a line
  void cubicFuncMin(_T& xmin, _T& fmin, _T f2, _T f1, _T c2, _T c1, _T dx) const;

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
    NMP::JacobianBase<_T>*         J) = 0;

  //---------------------------------------------------------------------
  virtual void getJabsRowSums(
    uint32_t                     numResiduals,
    const NMP::JacobianBase<_T>* J,
    NMP::Vector<_T>&             result) = 0;

  //---------------------------------------------------------------------
  virtual void computeJtr(
    uint32_t                     numResiduals,
    const NMP::JacobianBase<_T>* J,
    const NMP::Vector<_T>&       r,
    NMP::Vector<_T>&             result) = 0;

  //---------------------------------------------------------------------
  virtual void computeJb(
    uint32_t                     numResiduals,
    const NMP::JacobianBase<_T>* J,
    const NMP::Vector<_T>&       b,
    NMP::Vector<_T>&             result) = 0;

  //---------------------------------------------------------------------
  virtual void computeJtJ(
    uint32_t                     numResiduals,
    const NMP::JacobianBase<_T>* J,
    NMP::HessianBase<_T>*        H) = 0;

  //---------------------------------------------------------------------
  virtual void augmentJtJ(
    NMP::HessianBase<_T>* H,
    _T                    lambda) = 0;

  //---------------------------------------------------------------------
  virtual void solve(
    const NMP::HessianBase<_T>* H,
    const NMP::Vector<_T>&      b,
    NMP::Vector<_T>&            searchDir) = 0;
  //@}

protected:
  //---------------------------------------------------------------------
  // Optimisation status
  NonLinearOptimiserStatus::eType m_status;

  // Least squares function
  NonLinearLeastSquaresFuncBase& m_fn;

  // Number of iterations
  uint32_t m_maxIterations;
  uint32_t m_numIterations;

  // Number of function evaluations
  uint32_t m_maxFuncEvals;
  uint32_t m_numFuncEvals;

  // Termination tolerances
  _T m_tolP;
  _T m_tolFunc;

  // Finite difference control
  _T m_finiteDiffMinChange;
  _T m_finiteDiffMaxChange;

  // Unknowns
  NMP::Vector<_T> m_P;
  NMP::Vector<_T> m_oldP;

  // Residual error vector
  NMP::Vector<_T> m_fRes;
  NMP::Vector<_T> m_fRes_;

  // Sum of squares residual error
  _T m_fResNorm;

  // Jacobian
  NMP::JacobianBase<_T>* m_J;

  // Augmented Hessian matrix: J^T * J + lambda*I
  NMP::HessianBase<_T>* m_H;

  // Right hand side of linear system: -J^T * fRes
  NMP::Vector<_T> m_b;

  // Least squares function gradient at P
  NMP::Vector<_T> m_fGrad;

  // Parameter search direction
  NMP::Vector<_T> m_searchDir;

  // Finite difference deltas
  NMP::Vector<_T> m_fdDeltas;
  NMP::Vector<_T> m_v;
  bool            m_isFiniteDifferencing;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP

#endif // NM_NONLINEAR_OPTIMISER_BASE_H
