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

#ifndef ScalarType
  #error This file should not be compiled directly
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// CSplineSmoother
//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmoother<ScalarType>::~CSplineSmoother()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmoother<ScalarType>::CSplineSmoother(uint32_t dims, ScalarType tol) :
  m_curve(4, dims), m_tol(tol)
{
  // Initialize default values
  init();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmoother<ScalarType>::CSplineSmoother(
  const Vector<ScalarType>& x, const Matrix<ScalarType>& Y,
  const Vector<ScalarType>& w, ScalarType tol) :
  m_curve(4, Y.numColumns()), m_tol(tol)
{
  // Initialize default values
  init();

  // Compute the cubic smoothing spline
  m_status = doSolve(x, Y, w);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmoother<ScalarType>::CSplineSmoother(const Vector<ScalarType>& x, const Matrix<ScalarType>& Y, ScalarType tol) :
  m_curve(4, Y.numColumns()), m_tol(tol)
{
  // Initialize default values
  init();

  // Compute the cubic smoothing spline
  solve(x, Y);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmoother<ScalarType>::CSplineSmoother(const Matrix<ScalarType>& Y, ScalarType tol) :
  m_curve(4, Y.numColumns()), m_tol(tol)
{
  // Initialize default values
  init();

  // Compute the cubic smoothing spline
  solve(Y);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CSplineSmoother<ScalarType>::init()
{
  m_epsFac = static_cast<ScalarType>(1e-2); // 100th of specified tolerance
  m_maxIts = 15; // Maximum number of iterations
  m_rho = static_cast<ScalarType>(0); // Smoothing parameter
  m_Ep = static_cast<ScalarType>(0); // Error functional
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CSplineSmoother<ScalarType>::weights(const Vector<ScalarType>& x, Vector<ScalarType>& w) const
{
  NMP_ASSERT(w.numElements() == x.numElements());
  NMP_ASSERT(w.numElements() >= 2);

  uint32_t n = w.numElements();
  uint32_t ext = n - 1;

  // Interior weights
  for (uint32_t i = 1; i < ext; ++i)
    w[i] = (x[i+1] - x[i-1]) / static_cast<ScalarType>(2);

  // First and last weights
  w[0] = (x[1] - x[0]) / static_cast<ScalarType>(2);
  w[ext] = (x[ext] - x[ext-1]) / static_cast<ScalarType>(2);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmootherStatus::eType CSplineSmoother<ScalarType>::solve(
  const Vector<ScalarType>& x, const Matrix<ScalarType>& Y, const Vector<ScalarType>& w)
{
  // Compute the cubic smoothing spline
  m_status = doSolve(x, Y, w);
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmootherStatus::eType CSplineSmoother<ScalarType>::solve(
  const Vector<ScalarType>& x, const Matrix<ScalarType>& Y)
{
  // Create a default fitting weight vector
  uint32_t n = Y.numRows();
  Vector<ScalarType> w(n);
  weights(x, w); // Composite Trapezoid approximation

  // Compute the cubic smoothing spline
  m_status = doSolve(x, Y, w);
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmootherStatus::eType CSplineSmoother<ScalarType>::solve(const Matrix<ScalarType>& Y)
{
  // Create a default set of uniform parameter samples x = [0:n-1]
  uint32_t n = Y.numRows();
  Vector<ScalarType> x(n);
  x.linear(static_cast<ScalarType>(0), static_cast<ScalarType>(n - 1));

  // Create a default fitting weight vector
  Vector<ScalarType> w(n);
  weights(x, w); // Composite Trapezoid approximation

  // Compute the cubic smoothing spline
  m_status = doSolve(x, Y, w);
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
// Main cubic smoothing spline fitting algorithm
template <>
CSplineSmootherStatus::eType CSplineSmoother<ScalarType>::doSolve(
  const Vector<ScalarType>& x, const Matrix<ScalarType>& Y,
  const Vector<ScalarType>& w)
{
  NMP_ASSERT(m_tol >= 0 && m_epsFac > 0);

  // Set up the minimizer termination conditions
  ScalarType eps = m_epsFac * m_tol; // termination tolerance

  // Clear the smoothing parameter
  m_rho = static_cast<ScalarType>(0);
  m_Ep = static_cast<ScalarType>(0);

  // Basic dimensional information
  uint32_t dims = Y.numColumns();
  uint32_t n = Y.numRows();
  if (n < 1 || n != x.numElements() || n != w.numElements())
    return CSplineSmootherStatus::BadParams;

  // Check if we must fit to one point. i.e. a constant
  if (n == 1) return doCurve1(x, Y);

  // Check if the smoothing spline f(t) is the basic interpolating polynomial
  // i.e. fit a linear spline to the two data points
  if (n == 2) return doCurve2(x, Y);

  // Check if the fitting tolerance is zero i.e. we want to fit the
  // interpolating polynomial to the data.
  if (m_tol == 0) return doInterpolate(x, Y);

  // The minimum number of points required to compute a cubic smoothing spline is
  // four. If we have three points then we fit a quadratic curve to the data.
  if (n == 3) return doCurve3(x, Y);

  // Compute the parameter difference vector and its reciprocal
  Vector<ScalarType> dx(n - 1), odx(n - 1);
  for (uint32_t i = 0; i < n - 1; ++i)
  {
    dx[i] = x[i+1] - x[i];
    if (dx[i] <= 0) return CSplineSmootherStatus::Multiplicity;
    odx[i] = 1 / dx[i];
  }

  // Set up the linear system for solving the B-spline coefficients of the
  // 2nd derivative of the smoothing spline: D^(2)f(t). We make use of the
  // sparsity of the system by only storing the required diagonals of the
  // Gramian A and normalized divided difference matrices Ct.
  Vector<ScalarType> p(n - 2);
  for (uint32_t i = 0; i < n - 2; ++i) p[i] = 2 * (dx[i+1] + dx[i]);

  // A is the Gramian of B_{ j, x }
  ScalarType fac = static_cast<ScalarType>(1) / static_cast<ScalarType>(6);
  BandDiagMatrix<ScalarType> A(n - 2, n - 2, 1, 1);
  BandDiagMatrix<ScalarType> pA(n - 2, n - 2, 1, 1);
  A.setDiagVector(-1, &dx[1]);
  A.setDiagVector(0, &p[0]);
  A.setDiagVector(1, &dx[1]);
  A *= fac; // normalized coefficients

  // C^T is the matrix whose j^th row contains the weights for the
  // normalized 2^nd divided difference (x_{j+2} - x_j) * [x_j,...,x_{j+2}]f
  for (uint32_t i = 0; i < n - 2; ++i) p[i] = -(odx[i] + odx[i+1]);
  BandDiagMatrix<ScalarType> Ct(n - 2, n, 0, 2);
  Ct.setDiagVector(0, &odx[0]);
  Ct.setDiagVector(1, &p[0]);
  Ct.setDiagVector(2, &odx[1]);

  // C^T * Y
  Matrix<ScalarType> Cty(n - 2, dims);
  Ct.multiply(Cty, Y);

  // W^{-1} * C
  Vector<ScalarType> iw(n);
  for (uint32_t i = 0; i < n; ++i) iw[i] = 1.0f / w[i];
  BandDiagMatrix<ScalarType> C(n, n - 2, 2, 0);
  BandDiagMatrix<ScalarType> WiC(n, n - 2, 2, 0);
  Ct.getTranspose(C);
  WiC = C;
  WiC.multiplyDiag(&iw[0]);

  // C^T * W^{-1} * C
  BandDiagMatrix<ScalarType> CtWiC(n - 2, n - 2, 2, 2);
  BandDiagMatrix<ScalarType> CtWiC_pA(n - 2, n - 2, 2, 2);
  CtWiC.setFromMultiplicationSym(Ct, WiC);

  // Solve: (C^T * W^{-1} * C) u = C^T * Y
  BandDiagSolverCholesky<ScalarType> solver(CtWiC, Cty);
  BandDiagSolverCholeskyStatus::eType solverStatus = solver.getStatus();
  if (solverStatus != BandDiagSolverCholeskyStatus::Success)
  {
    if (solverStatus == BandDiagSolverCholeskyStatus::NotPosDef)
      return CSplineSmootherStatus::NotPosDef;
    return CSplineSmootherStatus::Singular;
  }

  // C * u
  const Matrix<ScalarType>& u = solver.getX();
  Matrix<ScalarType> ymfp(n, dims);
  C.multiply(ymfp, u); // C * u

  // (C * u)^T
  Matrix<ScalarType> utCt(dims, n);
  ymfp.getTranspose(utCt);

  // W^{-1} * C * u
  ymfp.multiplyDiag(&iw[0]);

  // u^T * (C^T * W^{-1} * C) * u
  Matrix<ScalarType> utCtWiCu(dims, dims);
  utCtWiCu.setFromMultiplication(utCt, ymfp);

  // Since y - fp = W^{-1} C u then E(p) = || y - f(p,x) ||^2
  // E(p) = u^T C^T W^{-1} C u
  m_Ep = utCtWiCu.trace();

  // Check if the data error functional || y - f(p,x) ||^2 is within
  // the fitting tolerance
  CSplineSmootherStatus::eType status = CSplineSmootherStatus::Success;
  if (m_Ep < m_tol)
  {
    // Set the fitted curve f(t) as a linear B-spline with a single span
    BSplineCurve<ScalarType> sp2(2, dims, 2);
    sp2.knotsOpenUniform(x[0], x[n-1]);

    // Zero the coefficients
    Matrix<ScalarType>& coefs = sp2.coefs();
    coefs.set(static_cast<ScalarType>(0));

    // Swap contents with the fitted curve
    m_curve.swap(sp2);
  }
  else
  {
    //
    // Initial Newton method update to the smoothing parameter
    //

    // Compute the error functional: g(p) = 1 / E(p)^{1/2} - 1 / (tol)^{1/2}
    // which is concave upward and becomes ever more linear with growing p.
    ScalarType oost = static_cast<ScalarType>(1) / sqrt(m_tol);
    ScalarType sEp = sqrt(m_Ep);
    ScalarType g0 = static_cast<ScalarType>(1) / sEp - oost; // g(0)

    // Use Newtons method at p=0 to obtain a better estimate of the smoothing
    // parameter p+ = p - g(p) / g'(p). This requires the derivative g'(p) with
    // respect to the parameters u: g'(p) = -(1/2)E(p)^{-3/2} DE(p), with
    // DE(p) = 2 u^T C^T W^-1 C Du and since -A u = (C^T W^-1 C + p A) Du
    // then DE(p) = -2 u^T A u
    Matrix<ScalarType> ut(dims, n - 2);
    Matrix<ScalarType> Au(n - 2, dims);
    Matrix<ScalarType> utAu(dims, dims);
    A.multiply(Au, u);
    u.getTranspose(ut);
    utAu.setFromMultiplication(ut, Au);
    ScalarType DEp = utAu.trace(); // u^T A u

    // Thus g'(p) = E(p)^{-3/2} u^T A u
    // Newton iteration: p+ = p - g(p) / g'(p) with initial p=0
    ScalarType dp = -g0 * (m_Ep * sEp) / DEp; // p and also delta change in parameter p

    // Absolute difference between Ep and the target tolerance
    ScalarType xEp_old = abs(m_Ep - m_tol);

    // To continue performing newton iteration we need to compute g'(p), which
    // requires computing a set of linear equations. To avoid this we can use the secant
    // method instead. This finds the root based only on the current and last estimates
    // of p and g(p)
    uint32_t xits = 0;
    ScalarType rho_old = m_rho;
    ScalarType Ep_old = m_Ep;

    //
    // Iteration loop for the smoothing parameter p that minimizes the
    // residual error functional so that Ep --> tol
    //
    uint32_t its;
    for (its = 1; its < m_maxIts; ++its)
    {
      // Update smoothing parameter p
      m_rho += dp;

      // Check it is positive (poor secant update!)
      if (m_rho <= 0)
      {
        m_rho = rho_old;
        status = CSplineSmootherStatus::Divergent;
        break;
      }

      // C^T * W^{-1} * C + p*A
      pA = A; pA *= m_rho;
      CtWiC_pA = CtWiC;
      CtWiC_pA += pA;

      // Solve: (C^T * W^{-1} * C + p*A) u = C^T * Y
      solver.solve(CtWiC_pA, Cty);
      solverStatus = solver.getStatus();
      if (solverStatus != BandDiagSolverCholeskyStatus::Success)
      {
        if (solverStatus == BandDiagSolverCholeskyStatus::NotPosDef)
          return CSplineSmootherStatus::NotPosDef;
        return CSplineSmootherStatus::Singular;
      }

      // C * u
      const Matrix<ScalarType>& u = solver.getX();
      C.multiply(ymfp, u); // C * u

      // (C * u)^T
      ymfp.getTranspose(utCt);

      // W^{-1} * C * u
      ymfp.multiplyDiag(&iw[0]);

      // u^T * (C^T * W^{-1} * C) * u
      utCtWiCu.setFromMultiplication(utCt, ymfp);

      // Ep = || y - fp ||^2
      m_Ep = utCtWiCu.trace();

      // Check conditions for terminating on the target tolerance
      ScalarType xEp = abs(m_Ep - m_tol);
      if (xEp < eps) break;

      // Check if xEp diverged from the last value. this can happen in the
      // presence of roundoff error when solving the linear system A x = b
      if (xEp > xEp_old)
      {
        // Restore the old smoothing parameter and error functional
        m_rho = rho_old;
        m_Ep = Ep_old;

        // Check if the error is still diverging
        if (xits > 2) {
          status = CSplineSmootherStatus::Divergent;
          break;
        }

        // The error functional: g(p) = 1 / Ep^{1/2} - 1 / (tol)^{1/2} failed
        // to give us a better estimate of p by using the secant method so
        // lets just cut down the step size
        dp = dp / 2;
        xits++;
        continue;
      }

      // Update the old parameter values
      xEp_old = xEp;
      rho_old = m_rho;
      Ep_old = m_Ep;
      xits = 0;

      //
      // Secant method update to the smoothing parameter
      //

      // Compute the error functional: g(p) = 1 / Ep^{1/2} - 1 / (tol)^{1/2}
      sEp = sqrt(m_Ep);
      ScalarType gp = static_cast<ScalarType>(1) / sEp - oost; // g(p)

      // Compute secant: pn, pc, pl are next, current and last p
      // pn = pc - (pc - pl)/(g(pc) - g(pl)) * g(pc)
      // i.e. dpc = (pn - pc) = - (dp(pl) / dg(pl)) * g(pc)

      // Check if update puts p = \infty i.e. interpolation
      ScalarType dg = g0 - gp; // -( g(pc) - g(pl) )

      //if(dg == static_cast<ScalarType>(0)) return doInterpolate(x, Y);
      if (dg == static_cast<ScalarType>(0)) break; // Do this for now instead

      // Compute updates
      dp = dp * gp / dg; // update delta change in p
      g0 = gp; // update last gp error functional
    }

    // Check for termination on max iterations. But fail gracefully by
    // returning the B-spline curve with the last computed smoothing parameter
    if (its == m_maxIts) {
      status = CSplineSmootherStatus::MaxIterations;
    }

    // Recover the spline coefficients c: i.e. c/p = u
    Matrix<ScalarType>& c = solver.getX();
    c *= m_rho;

    // Set the second order B-spline curve D^(2)f(t)
    BSplineCurve<ScalarType> f2t(2, dims, c.numRows());
    Vector<ScalarType>& knots = f2t.knots();
    Matrix<ScalarType>& coefs = f2t.coefs();
    knots = x;
    coefs.setTranspose(c);

    // Integrate the second order spline D^(2)f(t) twice in order to recover
    // the required fourth order spline f(t) consistent with answer
    BSplineCurve<ScalarType>* f3t = (BSplineCurve<ScalarType>* )f2t.integral();
    NMP_ASSERT(f3t != NULL);
    BSplineCurve<ScalarType>* f4t = (BSplineCurve<ScalarType>* )f3t->integral();
    NMP_ASSERT(f4t != NULL);

    // Swap the contents of the spline
    m_curve.swap(*f4t);

    // clean up the created integral curves
    delete f4t;
    delete f3t;
  }

  //
  // Recover the actual coefficients of the smoothing spline
  //

  // f(p,x) = Y - W^-1 C u,  hence Y - f(p,x) = W^-1 C u
  // Evaluate the values of the computed smoothing spline f(p,x)
  Matrix<ScalarType> values(Y);
  values -= ymfp;

  // Having integrated D^(2)f(t) twice, the spline f(t) differs from the true answer
  // f(p,x) by a polynomial of order 2. This polynomial is computable from the residual
  // values - f(x). We fit the residual data to a linear spline with a single span.
  Vector<ScalarType>& knots = m_curve.knots();
  Matrix<ScalarType>& coefs = m_curve.coefs();
  ScalarType dknots = knots.end(0) - knots[0];

  // Solve the n by 2 linear system B P = V in the least squares sense by
  // computing B^T B P = B^T V
  Matrix<ScalarType> BtB(2, 2, static_cast<ScalarType>(0));
  Matrix<ScalarType> BtV(2, dims, static_cast<ScalarType>(0));
  Vector<ScalarType> va(dims);
  Vector<ScalarType> vb(dims);
  uint32_t intv = 0;
  for (uint32_t i = 0; i < n; ++i)
  {
    // values - f(x)
    values.getRowVector(i, vb);
    m_curve.evaluate(x[i], intv, va, true);
    vb -= va;

    // Compute the basis functions of the linear spline
    // C(t) = B_{1,2}(t)*P_1 + B_{2,2}(t)*P_2
    // where B_{1,2}(t) = 1-t, B_{2,2}(t) = t
    ScalarType t = (x[i] - knots[0]) / dknots;
    ScalarType omt = static_cast<ScalarType>(1) - t;

    // Update B^T B
    BtB.element(0, 0) += omt * omt;
    BtB.element(0, 1) += omt * t;
    BtB.element(1, 1) += t * t;

    // Update B^T V
    for (uint32_t j = 0; j < dims; ++j)
    {
      BtV.element(0, j) += omt * vb[j];
      BtV.element(1, j) += t * vb[j];
    }
  }

  // Fill in the symmetric lower triangle of the matrix
  BtB.makeSymmetric();

  // Compute X = (B^T B)^-1 B^T V
  if (!Inverse2x2(BtB, BtB)) return CSplineSmootherStatus::Singular;
  Matrix<ScalarType> P(2, dims);
  P.setFromMultiplication(BtB, BtV);

  // Our computed spline f(t) may have different numbers of control points depending
  // on whether || y - f(p,x) ||^2 < Etol when p = 0.
  // i.e. when p = 0 we fit the data to a straight line (2 control points), and
  // N control points otherwise.
  if (m_curve.order() == 2)
  {
    // Set the smoothing spline control points
    coefs.setTranspose(P);

  }
  else
  {
    // Compute the values at the Greville points of a straight line. We know
    // these to be the B-coefficients of the straight line w.r.t. knots.
    uint32_t nP = knots.numElements() - m_curve.order();
    Vector<ScalarType> avknots(nP);
    ScalarType fac = static_cast<ScalarType>(1) / (m_curve.order() - 1);
    for (uint32_t i = 0; i < nP; ++i)
    { // Average of the order-1 consecutive knots
      avknots[i] = (knots[i+1] + knots[i+2] + knots[i+3]) * fac;
    }
    // Preserve the start and end knots
    avknots[0] = knots[0];
    avknots.end(0) = knots.end(0);

    // Update the set of smoothing spline control points
    for (uint32_t i = 0; i < nP; ++i)
    {
      // C(t) = B_{1,2}(t)*P_1 + B_{2,2}(t)*P_2
      // where B_{1,2}(t) = 1-t, B_{2,2}(t) = t
      ScalarType t = (avknots[i] - knots[0]) / dknots;
      ScalarType omt = static_cast<ScalarType>(1) - t;
      P.getRowVector(0, va);
      P.getRowVector(1, vb);
      va *= omt;
      vb *= t;
      va += vb;

      // Update the control point
      coefs.getColumnVector(i, vb);
      vb += va;
      coefs.setColumnVector(i, vb);
    }
  }

  // Return the status of the solve
  return status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmootherStatus::eType CSplineSmoother<ScalarType>::doCurve1(
  const Vector<ScalarType>& x, const Matrix<ScalarType>& Y)
{
  uint32_t dims = Y.numColumns();

  BSplineCurve<ScalarType> sp1(1, dims, 1);
  Vector<ScalarType>& knots = sp1.knots();
  knots[0] = x[0];
  knots[1] = x[0] + 1; // Extreme knots must be different
  Matrix<ScalarType>& coefs = sp1.coefs();
  coefs.setTranspose(Y);
  m_curve.swap(sp1); // Swap with fitted curve

  return CSplineSmootherStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmootherStatus::eType CSplineSmoother<ScalarType>::doCurve2(
  const Vector<ScalarType>& x, const Matrix<ScalarType>& Y)
{
  uint32_t dims = Y.numColumns();

  BSplineCurve<ScalarType> sp2(2, dims, 2);
  sp2.knotsOpenUniform(x[0], x[1]);
  Matrix<ScalarType>& coefs = sp2.coefs();
  coefs.setTranspose(Y);
  m_curve.swap(sp2); // Swap with fitted curve

  return CSplineSmootherStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmootherStatus::eType CSplineSmoother<ScalarType>::doCurve3(
  const Vector<ScalarType>& x, const Matrix<ScalarType>& Y)
{
  BSplineSolver<ScalarType> solver(3, 1, Y, x);
  BSplineSolverStatus::eType status = solver.getStatus();
  if (status != BSplineSolverStatus::Success)
  {
    if (status == BSplineSolverStatus::Singular) return CSplineSmootherStatus::Singular;
    return CSplineSmootherStatus::BadParams;
  }

  BSplineCurve<ScalarType>& sp3 = solver.getCurve();
  m_curve.swap(sp3); // Swap with fitted curve

  return CSplineSmootherStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmootherStatus::eType CSplineSmoother<ScalarType>::doInterpolate(
  const Vector<ScalarType>& x, const Matrix<ScalarType>& Y)
{
  // Compute the cubic interpolating polynomial to the data
  CSplineInterpolator<ScalarType> interp(Y, x);

  // Check the status of the solve
  SolverTriDiagStatus::eType status = interp.getStatus();
  if (status != SolverTriDiagStatus::Success)
  {
    // Multiplicity of data
    if (status == SolverTriDiagStatus::BadParams) return CSplineSmootherStatus::Multiplicity;

    // Singular system
    return CSplineSmootherStatus::Singular;
  }

  // For this codepath, m_tol == 0, this is not currently supported and requires
  // conversion to the B-form of the cubic interpolating polynomial.
  NMP_ASSERT_FAIL();

  // Set the smoothing parameter to infinity.
  m_rho = std::numeric_limits<ScalarType>::infinity();

  // Set the error functional.
  m_Ep = static_cast<ScalarType>(0);

  return CSplineSmootherStatus::BadParams;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const BSplineCurve<ScalarType>& CSplineSmoother<ScalarType>::getCurve() const
{
  return m_curve;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineCurve<ScalarType>& CSplineSmoother<ScalarType>::getCurve()
{
  return m_curve;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType CSplineSmoother<ScalarType>::getSmoothingParam() const
{
  return m_rho;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType CSplineSmoother<ScalarType>::getError() const
{
  return m_Ep;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType CSplineSmoother<ScalarType>::getTol() const
{
  return m_tol;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CSplineSmoother<ScalarType>::setTol(ScalarType tol)
{
  m_tol = tol;
  NMP_ASSERT(m_tol >= 0);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType CSplineSmoother<ScalarType>::getEpsFac() const
{
  return m_epsFac;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CSplineSmoother<ScalarType>::setEpsFac(ScalarType fac)
{
  m_epsFac = fac;
  NMP_ASSERT(m_epsFac > 0);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t CSplineSmoother<ScalarType>::getMaxIterations() const
{
  return m_maxIts;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CSplineSmoother<ScalarType>::setMaxIterations(uint32_t n)
{
  m_maxIts = n;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineSmootherStatus::eType CSplineSmoother<ScalarType>::getStatus() const
{
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool CSplineSmoother<ScalarType>::fail() const
{
  if (m_status > CSplineSmootherStatus::MaxIterations) return true;
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
