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
// Specializes template implementations for the type ScalarType.
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// JacobianBase
//----------------------------------------------------------------------------------------------------------------------
template <>
JacobianBase<ScalarType>::JacobianBase(uint32_t numRows, uint32_t numColumns) : m_numRows(numRows), m_numColumns(numColumns)
{
  NMP_ASSERT(m_numRows > 0);
  NMP_ASSERT(m_numColumns > 0);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
JacobianBase<ScalarType>::~JacobianBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
// HessianBase
//----------------------------------------------------------------------------------------------------------------------
template <>
HessianBase<ScalarType>::HessianBase(uint32_t size) : m_size(size)
{
  NMP_ASSERT(m_size > 0);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
HessianBase<ScalarType>::~HessianBase()
{
}

//----------------------------------------------------------------------------------------------------------------------
// NonLinearLeastSquaresFunc
//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearLeastSquaresFunc<ScalarType>::NonLinearLeastSquaresFunc(uint32_t numUnknowns, uint32_t maxNumResiduals)
  : NonLinearLeastSquaresFuncBase(NonLinearOptimiserFuncType::Func, numUnknowns, maxNumResiduals)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearLeastSquaresFunc<ScalarType>::~NonLinearLeastSquaresFunc()
{
}

//----------------------------------------------------------------------------------------------------------------------
// NonLinearLeastSquaresFuncJac
//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearLeastSquaresFuncJac<ScalarType>::NonLinearLeastSquaresFuncJac(uint32_t numUnknowns, uint32_t maxNumResiduals)
  : NonLinearLeastSquaresFuncBase(NonLinearOptimiserFuncType::FuncJac, numUnknowns, maxNumResiduals)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearLeastSquaresFuncJac<ScalarType>::~NonLinearLeastSquaresFuncJac()
{
}

//----------------------------------------------------------------------------------------------------------------------
// NonLinearOptimiserBase
//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearOptimiserBase<ScalarType>::NonLinearOptimiserBase(NonLinearLeastSquaresFunc<ScalarType>& func) :
  m_status(NonLinearOptimiserStatus::Invalid),
  m_fn(func),
  m_numIterations(0), m_numFuncEvals(0),
  m_P(func.getNumUnknowns()),
  m_oldP(func.getNumUnknowns()),
  m_fRes(func.getMaxNumResiduals()),
  m_fRes_(func.getMaxNumResiduals()),
  m_fResNorm(0),
  m_J(0),
  m_H(0),
  m_b(func.getNumUnknowns()),
  m_fGrad(func.getNumUnknowns()),
  m_searchDir(func.getNumUnknowns()),
  m_fdDeltas(func.getNumUnknowns()),
  m_v(func.getNumUnknowns()),
  m_maxIterations(100 * func.getNumUnknowns()),
  m_maxFuncEvals(300),
  m_tolP(NMNONLINEAR_TOLP),
  m_tolFunc(NMNONLINEAR_TOLFUNC),
  m_finiteDiffMinChange(NMNONLINEAR_FDMIN),
  m_finiteDiffMaxChange(NMNONLINEAR_FDMAX),
  m_isFiniteDifferencing(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearOptimiserBase<ScalarType>::NonLinearOptimiserBase(NonLinearLeastSquaresFuncJac<ScalarType>& funcJac) :
  m_status(NonLinearOptimiserStatus::Invalid),
  m_fn(funcJac),
  m_numIterations(0), m_numFuncEvals(0),
  m_P(funcJac.getNumUnknowns()),
  m_oldP(funcJac.getNumUnknowns()),
  m_fRes(funcJac.getMaxNumResiduals()),
  m_fRes_(funcJac.getMaxNumResiduals()),
  m_fResNorm(0),
  m_J(0),
  m_H(0),
  m_b(funcJac.getNumUnknowns()),
  m_fGrad(funcJac.getNumUnknowns()),
  m_searchDir(funcJac.getNumUnknowns()),
  m_fdDeltas(funcJac.getNumUnknowns()),
  m_v(funcJac.getNumUnknowns()),
  m_maxIterations(100 * funcJac.getNumUnknowns()),
  m_maxFuncEvals(300),
  m_tolP(NMNONLINEAR_TOLP),
  m_tolFunc(NMNONLINEAR_TOLFUNC),
  m_finiteDiffMinChange(NMNONLINEAR_FDMIN),
  m_finiteDiffMaxChange(NMNONLINEAR_FDMAX),
  m_isFiniteDifferencing(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearOptimiserBase<ScalarType>::~NonLinearOptimiserBase()
{
  if (m_J) delete m_J;
  if (m_H) delete m_H;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearOptimiserStatus::eType NonLinearOptimiserBase<ScalarType>::compute(const NMP::Vector<ScalarType>& P)
{
  NMP_ASSERT(m_J);
  NMP_ASSERT(m_H);

  // Reset the iteration information
  m_status = NonLinearOptimiserStatus::Invalid;
  m_numIterations = 0;
  m_numFuncEvals = 0;
  ScalarType curStepSize, lambda, fResNormInit, fResNormEst, fGradSD, fGradSDNew;
  bool fnStatus;
  uint32_t numResiduals = m_fn.getNumResiduals();

  //---------------------------------------------------------------------
  // Initialisation (To avoid compiler warnings)
  m_fResNorm = static_cast<ScalarType>(0);
  curStepSize = static_cast<ScalarType>(1);
  lambda = NMNONLINEAR_LAMBDA_INIT;
  fResNormInit = static_cast<ScalarType>(0);
  fResNormEst = static_cast<ScalarType>(0);
  fGradSD = static_cast<ScalarType>(0);
  fGradSDNew = static_cast<ScalarType>(0);

  //---------------------------------------------------------------------
  // Check if the values in the parameter vector are finite
  //
  uint32_t numUnknowns = P.numElements();
  NMP_ASSERT(numUnknowns == m_fn.getNumUnknowns());
#ifdef _DEBUG
  for (uint32_t i = 0; i < numUnknowns; ++i)
  {
    if (P[i] != P[i]) return m_status; // Test for NaNs
  }
#endif
  m_P = P;

  //---------------------------------------------------------------------
  // Compute the initial function residuals and Jacobian
  //
  m_isFiniteDifferencing = false;
  if (m_fn.getFuncType() == NonLinearOptimiserFuncType::Func)
  {
    // Jacobian will be updated when it is finite-differenced
    NonLinearLeastSquaresFunc<ScalarType>& lsqFn = (NonLinearLeastSquaresFunc<ScalarType>&)m_fn;
    fnStatus = lsqFn.func(*this, m_P, m_fRes);

    // Set the initial finite difference deltas
    ScalarType fdEps = NMNONLINEAR_FDEPS;
    for (uint32_t i = 0; i < numUnknowns; ++i)
    {
      m_fdDeltas[i] = fdEps * (static_cast<ScalarType>(1) + abs(m_P[i]));
    }

  }
  else
  {
    NonLinearLeastSquaresFuncJac<ScalarType>& lsqFn = (NonLinearLeastSquaresFuncJac<ScalarType>&)m_fn;
    fnStatus = lsqFn.func(*this, m_P, m_fRes, m_J);
  }
  m_numFuncEvals++;

  // Check for user termination
  if (!fnStatus)
  {
    m_status = NonLinearOptimiserStatus::TerminatedByUser;
    return m_status;
  }

  //---------------------------------------------------------------------
  // Main iteration loop
  //
  uint32_t fsmStatus = 0; // Init search direction
  while (true)
  {
    // Increment the iteration count
    m_numIterations++;

    //---------------------------------------------------------------------
    // Compute the Jacobian by finite difference
    //
    if (m_fn.getFuncType() == NonLinearOptimiserFuncType::Func)
    {
      m_isFiniteDifferencing = true;
      NonLinearLeastSquaresFunc<ScalarType>& lsqFn = (NonLinearLeastSquaresFunc<ScalarType>&)m_fn;

      // Clamp the finite difference deltas to the min and max difference range
      for (uint32_t i = 0; i < numUnknowns; ++i)
      {
        if (m_fdDeltas[i] < m_finiteDiffMinChange)
        {
          m_fdDeltas[i] = m_finiteDiffMinChange;
        }
        else if (m_fdDeltas[i] > m_finiteDiffMaxChange)
        {
          m_fdDeltas[i] = m_finiteDiffMaxChange;
        }
      }

      // Compute the columns of the Jacobian by finite difference
      fnStatus = computeJByFiniteDifference(m_P, m_fdDeltas, m_fRes, lsqFn, m_J);

      // Check for user termination
      if (!fnStatus)
      {
        m_status = NonLinearOptimiserStatus::TerminatedByUser;
        return m_status;
      }

      // Try to set difference to finiteDiffMinChange for the next finite
      // difference iteration step
      getJabsRowSums(numResiduals, m_J, m_v);
      ScalarType minChangeSum = numUnknowns * m_finiteDiffMinChange;
      for (uint32_t i = 0; i < numResiduals; ++i)
      {
        // Compute the sum of absolute values
        if (m_v[i] > 0)
        {
          m_fdDeltas[i] = minChangeSum / m_v[i];
        }
        else
        {
          m_fdDeltas[i] = m_finiteDiffMinChange;
        }
      }

      m_isFiniteDifferencing = false;
    }

    //---------------------------------------------------------------------
    // Least squares function gradient at P
    //
    computeJtr(numResiduals, m_J, m_fRes, m_b); // m_b = J^T * r
    for (uint32_t i = 0; i < numUnknowns; ++i)
    {
      m_fGrad[i] = static_cast<ScalarType>(2) * m_b[i];
    }

    // Compute the sum of squares residual error. Note that numResiduals can be
    // different to the preallocated size of the residuals vector maxNumResiduals
    m_fResNorm = static_cast<ScalarType>(0);
    for (uint32_t i = 0; i < numResiduals; ++i)
    {
      m_fResNorm += (m_fRes[i] * m_fRes[i]);
    }

    //---------------------------------------------------------------------
    // Initialization of Search Direction
    //
    if (fsmStatus == 0)
    {
      // Initialise the optimisation parameters
      m_oldP = m_P;
      curStepSize = static_cast<ScalarType>(1);
      fResNormInit = m_fResNorm;

      // Set the initial lambda value
      lambda = NMNONLINEAR_LAMBDA_INIT;

      // Compute the augmented Hessian matrix: J^T * J + lambda * I
      computeJtJ(numResiduals, m_J, m_H);
      augmentJtJ(m_H, lambda);

      // Compute the right hand side of the system -J^T * fRes
      // Note J^T * r has already been computed
      for (uint32_t i = 0; i < numUnknowns; ++i)
      {
        m_b[i] = -m_b[i];
      }

      // Compute the parameter update vector that minimises the
      // least squares function. This is the initial search direction
      solve(m_H, m_b, m_searchDir);

      // Update the parameter vector P
      for (uint32_t i = 0; i < numUnknowns; ++i)
      {
        m_P[i] = m_oldP[i] + curStepSize * m_searchDir[i];
      }

      // Store the initial component of the least squares function
      // gradient in the search direction
      fGradSD = m_fGrad.dot(m_searchDir);

      // Estimate the updated sum of squares residual error
      computeJb(numResiduals, m_J, m_searchDir, m_fRes_);
      m_fRes_ += m_fRes; // Estimate of new residuals (quadratic approx)
      fResNormEst = static_cast<ScalarType>(0);
      for (uint32_t i = 0; i < numResiduals; ++i)
      {
        fResNormEst += (m_fRes_[i] * m_fRes_[i]);
      }
      fsmStatus = 1;
    }
    else
    {
      //---------------------------------------------------------------------
      // Direction update
      //

      // Compute the component of the least squares function gradient in the
      // search direction. This should be negative if the downhill function
      // gradient has the same direction (sign) as the search direction
      fGradSDNew = m_fGrad.dot(m_searchDir);
      if (fGradSDNew > 0 && m_fResNorm > fResNormInit)
      {
        // The new sum of squares cost function is bigger than the last and
        // the function gradient also has the same direction (sign)
        // as the search direction. Fit a cubic to the current and initial
        // function cost values and the function gradients to obtain the
        // step that should take us directly to the minimum. Update the
        // current step size by a factor of this computed step
        ScalarType newStepSize, fResNormBest;
        cubicFuncMin(newStepSize, fResNormBest, m_fResNorm, fResNormInit, fGradSDNew, fGradSD, curStepSize);
        curStepSize = static_cast<ScalarType>(0.9) * newStepSize;
      }
      else
      {
        // Check for a reduction in the sum of squares cost function
        if (m_fResNorm < fResNormInit)
        {
          // The new sum of squares cost function is less than the old cost
          // so we can update the search direction and reset the initial
          // conditions to the current values

          // Fit a cubic to the current and initial function cost values and
          // the function gradients to obtain the step that should take us
          // directly to the minimum
          ScalarType newStepSize, fResNormBest;
          cubicFuncMin(newStepSize, fResNormBest, m_fResNorm, fResNormInit, fGradSDNew, fGradSD, curStepSize);
          if (fResNormBest > m_fResNorm)
          {
            fResNormBest = static_cast<ScalarType>(0.9) * m_fResNorm;
          }
          curStepSize = abs(newStepSize) + std::numeric_limits<ScalarType>::epsilon();

          // Update the lambda parameter
          if (fResNormBest < fResNormEst)
          {
            // Unbias the Hessian by decreasing lambda so that it becomes less
            // diagonally dominant, i.e. the computed search direction tends
            // more to the direction that moves the variables directly to the
            // quadratic function approximation minimum
            lambda = lambda / (static_cast<ScalarType>(1) + curStepSize);
          }
          else
          {
            // Bias the Hessian by increasing lambda so that it becomes more
            // diagonally dominant, i.e. the computed search direction tends
            // more to the steepest gradient direction
            lambda = lambda + (fResNormBest - fResNormEst) / curStepSize;
          }

          // Compute the augmented Hessian matrix: J^T * J + lambda * I
          computeJtJ(numResiduals, m_J, m_H);
          augmentJtJ(m_H, lambda);

          // Compute the right hand side of the system -J^T * fRes
          // Note J^T * r has already been computed
          for (uint32_t i = 0; i < numUnknowns; ++i)
          {
            m_b[i] = -m_b[i];
          }

          // Compute the parameter update vector that minimises the
          // least squares function. This is the new search direction
          solve(m_H, m_b, m_searchDir);

          // Reset the initial step size for the new search direction
          curStepSize = static_cast<ScalarType>(1);

          // Update the estimated sum of squares residual error
          computeJb(numResiduals, m_J, m_searchDir, m_fRes_);
          m_fRes_ += m_fRes; // Estimate of new residuals (quadratic approx)
          fResNormEst = static_cast<ScalarType>(0);
          for (uint32_t i = 0; i < numResiduals; ++i)
          {
            fResNormEst += (m_fRes_[i] * m_fRes_[i]);
          }

          // Component of the function gradient in the search direction
          fGradSDNew = m_fGrad.dot(m_searchDir);

          // Save the initial conditions
          m_oldP = m_P;
          fResNormInit = m_fResNorm;
          fGradSD = fGradSDNew;
        }
        else
        {
          // No reduction in sum of squares function error. Check if the step update
          // failed to change the sum of squares function error at all
          if (m_fResNorm == fResNormInit)
          {
            m_status = NonLinearOptimiserStatus::DidNotConverge;
            break;
          }
          else
          {
            // Halve the step length to avoid bouncing between the sides
            // of the valley
            curStepSize *= static_cast<ScalarType>(0.5);
          }
        }
      }

      // Update the parameter variables using the current search direction
      // and step size
      for (uint32_t i = 0; i < numUnknowns; ++i)
      {
        m_P[i] = m_oldP[i] + curStepSize * m_searchDir[i];
      }
    }

    //---------------------------------------------------------------------
    // Check termination conditions
    //

    // Termination on change in parameter vector. Test the maximum of the
    // absolute values in the search direction is smaller than tolP
    ScalarType termPval = abs(m_searchDir[0]);
    for (uint32_t i = 1; i < numUnknowns; ++i)
    {
      ScalarType t = abs(m_searchDir[i]);
      if (t > termPval) termPval = t;
    }
    if (termPval < m_tolP)
    {
      m_status = NonLinearOptimiserStatus::Converged;
      break;
    }
    else
    {
      // Gradient in the search direction is less than tolFun and the
      // maximum component of function gradient is less than 10*(tolFun+tolP)
      ScalarType a = m_fGrad[0] * m_searchDir[0];
      ScalarType b = abs(m_fGrad[0]);
      for (uint32_t i = 1; i < numUnknowns; ++i)
      {
        a += (m_fGrad[i] * m_searchDir[i]); // Gradient in search direction
        ScalarType t = abs(m_fGrad[i]);
        if (t > b) b = t;
      }
      if (abs(a) < m_tolFunc && b < 10 * (m_tolFunc + m_tolP))
      {
        m_status = NonLinearOptimiserStatus::Converged;
        break;
      }
      else
      {
        // Maximum number of function evaluations or iterations exceeded
        if (m_numFuncEvals > m_maxFuncEvals || m_numIterations > m_maxIterations)
        {
          m_status = NonLinearOptimiserStatus::TerminatedOnMaxIts;
          break;
        }
      }
    }

    //---------------------------------------------------------------------
    // Compute the new Jacobian and residuals using the updated parameters
    //
    if (m_fn.getFuncType() == NonLinearOptimiserFuncType::Func)
    {
      // Jacobian will be updated when it is finite-differenced
      NonLinearLeastSquaresFunc<ScalarType>& lsqFn = (NonLinearLeastSquaresFunc<ScalarType>&)m_fn;
      fnStatus = lsqFn.func(*this, m_P, m_fRes);
    }
    else
    {
      NonLinearLeastSquaresFuncJac<ScalarType>& lsqFn = (NonLinearLeastSquaresFuncJac<ScalarType>&)m_fn;
      fnStatus = lsqFn.func(*this, m_P, m_fRes, m_J);
    }
    m_numFuncEvals++;

    // Check for user termination
    if (!fnStatus)
    {
      m_status = NonLinearOptimiserStatus::TerminatedByUser;
      return m_status;
    }
  }

  //---------------------------------------------------------------------
  // Set the output parameters
  m_P = m_oldP;
  m_fResNorm = fResNormInit;

  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearOptimiserBase<ScalarType>& NonLinearOptimiserBase<ScalarType>::operator=(const NonLinearOptimiserBase<ScalarType>& rhs)
{
  (void)rhs;
  NMP_ASSERT_FAIL();
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBase<ScalarType>::cubicFuncMin(
  ScalarType& xmin, ScalarType& fmin,
  ScalarType f2, ScalarType f1, ScalarType c2, ScalarType c1, ScalarType dx) const
{
  // Find the coefficients of: f(x) = a*x^3 + b*x^2 + c*x + d
  // Trivially c = c1 and d = f1 at x=0. a and b are recovered by solving
  // the simultaneous equations at x = dx
  ScalarType dx2 = dx * dx;
  ScalarType dx3 = dx * dx2;
  ScalarType a = (c1 * dx + c2 * dx - 2 * f2 + 2 * f1) / dx3;
  ScalarType b = (3 * f2 - 3 * f1 - 2 * c1 * dx - c2 * dx) / dx2;

  // Find the root of df/dx = 3*a*x^2 + 2*b*x + c that is the *minimum*
  // of the cubic function, i.e. has positive d^2f/dx^2
  ScalarType a3 = 3 * a;
  ScalarType disc = b * b - a3 * c1;
  if (a == 0)
  {
    // df/dx is a linear function
    xmin = -c1 / (2 * b);
  }
  else if (disc <= 0)
  {
    // df/dx has complex roots. Use the real component of the roots.
    xmin = -b / a3;
  }
  else
  {
    // Compute the two roots stabally
    ScalarType q, x1, x2;
    if (b >= 0)
    {
      q = -(b + sqrt(disc));
    }
    else
    {
      q = -(b - sqrt(disc));
    }
    x1 = q / a3;
    x2 = c1 / q;

    // Determine the root corresponding to the minimum
    if (a3 * x1 > a3 * x2)
    {
      xmin = x1;
    }
    else
    {
      xmin = x2;
    }
  }

  // Compute the minimum of the cubic function
  fmin = ((a * xmin + b) * xmin + c1) * xmin + f1;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const NMP::Vector<ScalarType>& NonLinearOptimiserBase<ScalarType>::getP() const
{
  return m_P;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t NonLinearOptimiserBase<ScalarType>::getNumIterations() const
{
  return m_numIterations;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t NonLinearOptimiserBase<ScalarType>::getNumFuncEvals() const
{
  return m_numFuncEvals;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool NonLinearOptimiserBase<ScalarType>::isFiniteDifferencing() const
{
  return m_isFiniteDifferencing;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
NonLinearOptimiserStatus::eType NonLinearOptimiserBase<ScalarType>::getStatus() const
{
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t NonLinearOptimiserBase<ScalarType>::getMaxIterations() const
{
  return m_maxIterations;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBase<ScalarType>::setMaxIterations(uint32_t n)
{
  m_maxIterations = n;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t NonLinearOptimiserBase<ScalarType>::getMaxFuncEvals() const
{
  return m_maxFuncEvals;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBase<ScalarType>::setMaxFuncEvals(uint32_t n)
{
  m_maxFuncEvals = n;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType NonLinearOptimiserBase<ScalarType>::getTolP() const
{
  return m_tolP;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBase<ScalarType>::setTolP(ScalarType tolP)
{
  m_tolP = tolP;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType NonLinearOptimiserBase<ScalarType>::getTolFunc() const
{
  return m_tolFunc;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBase<ScalarType>::setTolFunc(ScalarType tolFunc)
{
  m_tolFunc = tolFunc;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType NonLinearOptimiserBase<ScalarType>::getFiniteDiffMinChange() const
{
  return m_finiteDiffMinChange;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBase<ScalarType>::
setFiniteDiffMinChange(ScalarType finiteDiffMinChange)
{
  m_finiteDiffMinChange = finiteDiffMinChange;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType NonLinearOptimiserBase<ScalarType>::getFiniteDiffMaxChange() const
{
  return m_finiteDiffMaxChange;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void NonLinearOptimiserBase<ScalarType>::setFiniteDiffMaxChange(ScalarType finiteDiffMaxChange)
{
  m_finiteDiffMaxChange = finiteDiffMaxChange;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
