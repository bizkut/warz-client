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
// BSplineSolver
//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineSolver<ScalarType>::BSplineSolver(
  uint32_t order, const Vector<ScalarType>& knots,
  const Matrix<ScalarType>& DmY, const Vector<int32_t>& m,
  const Vector<ScalarType>& x, const Vector<ScalarType>& w) :
//
  m_curve(order, DmY.numColumns(), knots)
{
  // Do the main solve to compute the B-Spline
  solve(DmY, m, x, w);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineSolver<ScalarType>::BSplineSolver(
  uint32_t order, const Vector<ScalarType>& knots,
  const Matrix<ScalarType>& Y, const Vector<ScalarType>& x) :
//
  m_curve(order, Y.numColumns(), knots)
{
  // Do the solve to compute the B-Spline
  solve(Y, x);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineSolver<ScalarType>::BSplineSolver(
  uint32_t order, uint32_t spans,
  const Matrix<ScalarType>& DmY, const Vector<int32_t>& m,
  const Vector<ScalarType>& x, const Vector<ScalarType>& w) :
//
  m_curve(order, DmY.numColumns(), spans, x)
{
  // Do the main solve to compute the B-Spline
  solve(DmY, m, x, w);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineSolver<ScalarType>::BSplineSolver(
  uint32_t order, uint32_t spans,
  const Matrix<ScalarType>& Y, const Vector<ScalarType>& x) :
//
  m_curve(order, Y.numColumns(), spans, x)
{
  // Do the solve to compute the B-Spline
  solve(Y, x);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineSolver<ScalarType>::BSplineSolver(uint32_t order, uint32_t dims, const Vector<ScalarType>& knots) :
//
  m_curve(order, dims, knots)
{
  m_status = BSplineSolverStatus::BadParams;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineSolver<ScalarType>::BSplineSolver(uint32_t order, uint32_t dims, uint32_t spans, const Vector<ScalarType>& x) :
//
  m_curve(order, dims, spans, x)
{
  m_status = BSplineSolverStatus::BadParams;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineSolver<ScalarType>::~BSplineSolver()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineSolverStatus::eType BSplineSolver<ScalarType>::solve(const Matrix<ScalarType>& Y, const Vector<ScalarType>& x)
{
  uint32_t n = Y.numRows();
  NMP_ASSERT(n >= 2);

  // m^th derivatives vector
  Vector<int32_t> m(n, 0);

  // Create a unit weight vector
  Vector<ScalarType> w(n, static_cast<ScalarType>(1));

  // Do the main solve to compute the B-spline curve
  solve(Y, m, x, w);

  // Return the status of the solve
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineSolverStatus::eType BSplineSolver<ScalarType>::solve(
  const Matrix<ScalarType>& DmY, const Vector<int32_t>& m,
  const Vector<ScalarType>& x, const Vector<ScalarType>& w)
{
  // Get system info
  uint32_t n = DmY.numRows();
  uint32_t dims = DmY.numColumns();
  uint32_t order = m_curve.order();
  NMP_ASSERT(n >= 2);
  NMP_ASSERT(order > 0);
  NMP_ASSERT(x.numElements() == n &&
             w.numElements() == n && m.numElements() == n);

  // Get the B-Spline knot vector
  Vector<ScalarType>& knots = m_curve.knots();

  // Get the B-Spline coefficient matrix
  Matrix<ScalarType>& coefs = m_curve.coefs();
  uint32_t cpts = coefs.numColumns();

  // Interval vector
  Vector<int32_t> intvs(n);

  // Set the status as invalid interval
  m_status = BSplineSolverStatus::InvalidInterval;

  // Compute the set of intervals that t lie within
  if (!m_curve.interval(x, intvs)) return m_status;

  // Check if the basis functions are valid on the leftmost knots
  NMP_ASSERT((uint32_t)intvs[0] >= (order - 1));
  uint32_t i1 = (uint32_t)intvs[0] - (order - 1);
  ScalarType val = knots[i1]; // knot multiplicity at start
  if (val != knots[0]) return m_status;

  // Check if the basis functions are valid on the rightmost knots
  uint32_t i2 = (uint32_t)intvs[n-1];
  NMP_ASSERT(i2 < knots.numElements() - 1);
  val = knots[i2+1]; // knot multiplicity at end
  if (val != knots.end(0) || i2 < i1) return m_status;

  // Make a first pass over the interval vector to find the number
  // of different blocks
  uint32_t blocks = 1;
  for (uint32_t i = 1, ii = 0; i < n; ++i)
  {
    if (intvs[i] != intvs[ii])
    {
      // Check that the intervals are strictly increasing
      if (intvs[i] < intvs[ii]) return m_status;

      // Check if the interval increases by more than the curve order.
      // i.e. there exists a band of basis intervals that no sample data
      // lies within, thus causing a singular system.
      if ((uint32_t)(intvs[i] - intvs[ii]) > order) return m_status;

      // Update info
      blocks++;
      ii = i;
    }
  }

  // Preallocate the solver memory
  SolverQR<ScalarType> solver(n, order, dims, blocks, cpts);

  // Get the block size and offset vectors
  Vector<int32_t>& sizes = solver.getSizes();
  Vector<int32_t>& offsets = solver.getOffsets();

  // Get the fixed width block matrix A
  Matrix<ScalarType>& A = solver.getA();

  // Get the right hand side matrix B
  Matrix<ScalarType>& B = solver.getB();

  // Vector for the non-vanishing basis functions
  Vector<ScalarType> p(order);
  Vector<ScalarType> q(dims);

  // Make a second pass over the interval vector to fill in the size,
  // offset, right hand side matrix B and collocation matrix A.
  sizes[0] = 0; offsets[0] = 0;
  uint32_t indx = 0;
  for (uint32_t i = 0, ii = 0; i < n; ++i)
  {
    // Check for a change in the sparse block segment
    if (intvs[i] > intvs[ii])
    {
      indx++; ii = i;
      sizes[indx] = 0;
      offsets[indx] = intvs[i] - (order - 1);
    }

    // Compute the basis function derivatives
    Matrix<ScalarType> DNik(order, m[i] + 1);
    m_curve.basisDerivatives(intvs[i], x[i], DNik);

    // Get the column vector corresponding to the m^th derivative
    DNik.getColumnVector(m[i], p);

    // Set the coefficients within the collocation matrix
    A.setRowVector(i, p);

    // Set the corresponding elements of the right hand side matrix
    DmY.getRowVector(i, q);
    B.setRowVector(i, q);

    // Update block segment size
    sizes[indx]++;
  }

  // We minimize the residual: sum_{ i } w(i) || Y(i, :) - C(x(j)) ||^2,
  // therefore we need to compute the square root of the weight factors
  // before multiplying both left and right hand sides of the system.
  Vector<ScalarType> sqrtw(w);
  sqrtw.squareroot();

  // Multiply the collocation matrix by the weight vector
  A.multiplyColumns(&sqrtw[0]);

  // Multiply the right hand side matrix by the weight vector
  B.multiplyColumns(&sqrtw[0]);

  // Set the failure status
  m_status = BSplineSolverStatus::Singular;

  // Solve the sparse linear system A X = B
  SolverQRStatus::eType statusQR;
  statusQR = solver.solve(A, B, sizes, offsets);
  if (statusQR != SolverQRStatus::Success) return m_status;

  // Get the solution matrix
  const Matrix<ScalarType>& P = solver.getX();

  // Set the B-Spline control point coefficients
  coefs.setTranspose(P);

  // Set the solver success
  m_status = BSplineSolverStatus::Success;

  // return solver status
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineSolverStatus::eType BSplineSolver<ScalarType>::getStatus() const
{
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool BSplineSolver<ScalarType>::fail() const
{
  return m_status != BSplineSolverStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const BSplineCurve<ScalarType>& BSplineSolver<ScalarType>::getCurve() const
{
  return m_curve;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BSplineCurve<ScalarType>& BSplineSolver<ScalarType>::getCurve()
{
  return m_curve;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
