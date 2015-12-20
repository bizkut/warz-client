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
// CSplineInterpolator
//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineInterpolator<ScalarType>::CSplineInterpolator(const Matrix<ScalarType>& X, const Vector<ScalarType>& t) :
//
  SolverTriDiag<ScalarType>(X.numRows(), X.numColumns()),
  m_startV(X.numColumns()), m_endV(X.numColumns()),
  m_dt(X.numRows() - 1), m_divdif(X.numRows() - 1, X.numColumns()),
  m_curve(4, X.numColumns(), X.numRows()),
  m_temp(X.numRows()), m_q(X.numColumns())
{
  // Set the start and end condition vectors
  m_startC = CSplineBoundaryType::SecondDerivative;
  m_endC = CSplineBoundaryType::SecondDerivative;
  m_startV.set(static_cast<ScalarType>(0));
  m_endV.set(static_cast<ScalarType>(0));

  // Compute the interpolating polynomial
  solve(X, t);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineInterpolator<ScalarType>::CSplineInterpolator(const Matrix<ScalarType>& X) :
//
  SolverTriDiag<ScalarType>(X.numRows(), X.numColumns()),
  m_startV(X.numColumns()), m_endV(X.numColumns()),
  m_dt(X.numRows() - 1), m_divdif(X.numRows() - 1, X.numColumns()),
  m_curve(4, X.numColumns(), X.numRows()),
  m_temp(X.numRows()), m_q(X.numColumns())
{
  // Set the sample vector t = [0:n-1]
  ScalarType t0 = static_cast<ScalarType>(0);
  ScalarType tn = (ScalarType)(X.numRows() - 1);
  m_temp.linear(t0, tn);

  // Set the start and end condition vectors
  m_startC = CSplineBoundaryType::SecondDerivative;
  m_endC = CSplineBoundaryType::SecondDerivative;
  m_startV.set(static_cast<ScalarType>(0));
  m_endV.set(static_cast<ScalarType>(0));

  // Compute the interpolating polynomial
  solve(X, m_temp);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineInterpolator<ScalarType>::CSplineInterpolator(
  const Matrix<ScalarType>& X, const Vector<ScalarType>& t,
  CSplineBoundaryType::eType startC, const Vector<ScalarType>& startV,
  CSplineBoundaryType::eType endC, const Vector<ScalarType>& endV) :
//
  SolverTriDiag<ScalarType>(X.numRows(), X.numColumns()),
  m_startV(X.numColumns()), m_endV(X.numColumns()),
  m_dt(X.numRows() - 1), m_divdif(X.numRows() - 1, X.numColumns()),
  m_curve(4, X.numColumns(), X.numRows()),
  m_temp(X.numRows()), m_q(X.numColumns())
{
  // Set the start and end condition vectors
  m_startC = startC;
  m_endC = endC;
  m_startV = startV;
  m_endV = endV;

  // Compute the interpolating polynomial
  solve(X, t);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineInterpolator<ScalarType>::CSplineInterpolator(uint32_t n, uint32_t dims) :
//
  SolverTriDiag<ScalarType>(n, dims),
  m_startV(dims), m_endV(dims),
  m_dt(n - 1), m_divdif(n - 1, dims),
  m_curve(4, dims, n),
  m_temp(n), m_q(dims)
{
  m_startC = CSplineBoundaryType::SecondDerivative;
  m_endC = CSplineBoundaryType::SecondDerivative;
  m_startV.set(static_cast<ScalarType>(0));
  m_endV.set(static_cast<ScalarType>(0));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineInterpolator<ScalarType>::~CSplineInterpolator()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverTriDiagStatus::eType CSplineInterpolator<ScalarType>::solve(
  const Matrix<ScalarType>& X, const Vector<ScalarType>& t)
{
  NMP_ASSERT(X.numRows() == t.numElements());

  // We do not support NotAKnot conditions currently.
  NMP_ASSERT(m_startC != CSplineBoundaryType::NotAKnot);
  NMP_ASSERT(m_endC != CSplineBoundaryType::NotAKnot);

  // Info on system
  uint32_t n = t.numElements();
  if (n < 2) return SolverTriDiagStatus::BadParams;
  uint32_t nm2 = n - 2;

  // Compute the sample vector delta differences
  for (uint32_t i = 0; i <= nm2; ++i)
  {
    m_dt[i] = t[i+1] - t[i];
    if (m_dt[i] <= 0) return SolverTriDiagStatus::BadParams;
  }

  // Compute the vector(s) of first order divided differences [t_i, t_{ i+1 }]g
  for (uint32_t i = 0; i <= nm2; ++i) m_temp[i] = static_cast<ScalarType>(1) / m_dt[i];
  X.diffRows(m_divdif);
  m_divdif.multiplyColumns(&m_temp[0]);

  // Set the tri-diagonal design matrix interior section corresponding to
  // the unknowns [s_2:s_{ n-1 }]
  BandDiagMatrix<ScalarType>& A = SolverTriDiag::getA();

  // Sub diagonal of A: dt[2:n-1]
  A.setDiagVector(1, 0, nm2, &m_dt[1]);

  // Main diagonal of A: 2*(dt[2:n-1] + dt[1:n-2])
  for (uint32_t i = 0; i < nm2; ++i) m_temp[i] = 2 * (m_dt[i+1] + m_dt[i]);
  A.setDiagVector(1, 1, nm2, &m_temp[0]);

  // Super diagonal of A: dt[1:n-2]
  A.setDiagVector(1, 2, nm2, &m_dt[0]);

  // Set the corresponding right hand side matrix
  // 3 * (dt_{i-1} * [t_i, t_{i+1}]g + dt_i * [t_{i-1}, t_i]g)
  Matrix<ScalarType>& B = SolverTriDiag::getB();
  for (uint32_t i = 1; i <= nm2; ++i)
  {
    m_divdif.getRowVector(i, m_v); // [t_i, t_{i+1}]g
    m_divdif.getRowVector(i - 1, m_p); // [t_{i-1}, t_i]g
    m_v *= m_dt[i - 1]; // dt_{i-1} * [t_i, t_{i+1}]g
    m_p *= m_dt[i]; // dt_i * [t_{i-1}, t_i]g
    m_v += m_p;
    m_v *= static_cast<ScalarType>(3);
    B.setRowVector(i, m_v);
  }

  // At this point the top and bottom rows of the design and right
  // hand side matrices corresponding to s_1 and s_n are zero. We
  // must set the required values corresponding to the end conditions.
  ScalarType conds[2];

  // Start boundary conditions
  if (m_startC == CSplineBoundaryType::FirstDerivative)
  {
    // s_1 = g'(t_1)
    conds[0] = static_cast<ScalarType>(1);
    conds[1] = static_cast<ScalarType>(0);
    m_v = m_startV;
  }
  else
  { // eSecondDerivative
    // 2*s_1 + s_2 = 3*[t_1, t_2]g - dt_1 * g''(t_1) / 2
    conds[0] = static_cast<ScalarType>(2);
    conds[1] = static_cast<ScalarType>(1);

    m_divdif.getRowVector(0, m_v); // [t_1, t_2]g
    m_v *= static_cast<ScalarType>(3);
    m_p = m_startV; // g''(t_1)
    m_p *= (m_dt[0] / static_cast<ScalarType>(2)); // dt_1 * g''(t_1) / 2
    m_v -= m_p; // 3*[t_1, t_2]g - dt_1 * g''(t_1) / 2
  }
  A.setRowVector(0, 0, 2, conds);
  B.setRowVector(0, m_v);

  // End boundary conditions
  if (m_endC == CSplineBoundaryType::FirstDerivative)
  {
    // s_n = g'(t_n)
    conds[0] = static_cast<ScalarType>(0);
    conds[1] = static_cast<ScalarType>(1);
    m_v = m_endV;
  }
  else
  { // eSecondDerivative
    // s_{n-1} + 2*s_n = 3*[t_{n-1}, t_n]g + dt_{n-1} * g''(t_n) / 2
    conds[0] = static_cast<ScalarType>(1);
    conds[1] = static_cast<ScalarType>(2);

    m_divdif.getRowVector(nm2, m_v); // [t_{n-1}, t_n]g
    m_v *= static_cast<ScalarType>(3);
    m_p = m_endV; // g''(t_n)
    m_p *= (m_dt[nm2] / static_cast<ScalarType>(2)); // dt_{n-1} * g''(t_n) / 2
    m_v += m_p; // 3*[t_{n-1}, t_n]g + dt_{n-1} * g''(t_n) / 2
  }
  A.setRowVector(n - 1, nm2, 2, conds);
  B.setRowVector(n - 1, m_v);

  // Solve the tri-diagonal system of equations
  m_status = SolverTriDiag::doSolve();
  if (m_status != SolverTriDiagStatus::Success) return m_status;

  // Iterate over the number of curve spans
  for (uint32_t i = 0; i <= nm2; ++i)
  {
    // c_{1,i} = g(t_i)
    X.getRowVector(i, m_v);
    m_curve.setCoef(0, i, m_v);

    // c_{2,i} = s_i
    m_X.getRowVector(i, m_p); // s_i
    m_curve.setCoef(1, i, m_p);

    // c_{4,i} = ( s_i + s_{i+1} - 2*[t_i, t_{i+1}]g ) / (dt_i)^2
    m_X.getRowVector(i + 1, m_v); // s_{i+1}
    m_v += m_p; // s_i + s_{i+1}
    m_divdif.getRowVector(i, m_p); // [t_i, t_{i+1}]g
    m_p *= static_cast<ScalarType>(2); // 2*[t_i, t_{i+1}]g
    m_v -= m_p; // s_i + s_{i+1} - 2*[t_i, t_{i+1}]g
    m_v /= m_dt[i];
    m_q = m_v; // c_{4,i} * dt_i needed for c_{3,i}
    m_v /= m_dt[i]; // Numerically stable / (dt_i)^2
    m_curve.setCoef(3, i, m_v);

    // c_{3,i} = ([t_i, t_{i+1}]g - s_i) / dt_i - c_{4,i} * dt_i
    m_divdif.getRowVector(i, m_v); // [t_i, t_{i+1}]g
    m_X.getRowVector(i, m_p); // s_i
    m_v -= m_p; // [t_i, t_{i+1}]g - s_i
    m_v /= m_dt[i];
    m_v -= m_q;
    m_curve.setCoef(2, i, m_v);
  }

  // Set the break-site vector
  Vector<ScalarType>& knots = m_curve.knots();
  knots = t;

  // return the status of the solve
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineBoundaryType::eType CSplineInterpolator<ScalarType>::getStartC() const
{
  return m_startC;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CSplineInterpolator<ScalarType>::setStartC(CSplineBoundaryType::eType startC)
{
  m_startC = startC;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
CSplineBoundaryType::eType CSplineInterpolator<ScalarType>::getEndC() const
{
  return m_endC;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CSplineInterpolator<ScalarType>::setEndC(CSplineBoundaryType::eType endC)
{
  m_endC = endC;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<ScalarType>& CSplineInterpolator<ScalarType>::getStartV() const
{
  return m_startV;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& CSplineInterpolator<ScalarType>::getStartV()
{
  return m_startV;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CSplineInterpolator<ScalarType>::setStartV(const Vector<ScalarType>& v)
{
  m_startV = v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<ScalarType>& CSplineInterpolator<ScalarType>::getEndV() const
{
  return m_endV;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& CSplineInterpolator<ScalarType>::getEndV()
{
  return m_endV;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void CSplineInterpolator<ScalarType>::setEndV(const Vector<ScalarType>& v)
{
  m_endV = v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverTriDiagStatus::eType CSplineInterpolator<ScalarType>::getStatus() const
{
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool CSplineInterpolator<ScalarType>::fail() const
{
  return m_status != SolverTriDiagStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const PPolyCurve<ScalarType>& CSplineInterpolator<ScalarType>::getCurve() const
{
  return m_curve;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
PPolyCurve<ScalarType>& CSplineInterpolator<ScalarType>::getCurve()
{
  return m_curve;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
