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

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// Householder
//----------------------------------------------------------------------------------------------------------------------
template <>
Householder<ScalarType>::~Householder()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Householder<ScalarType>::Householder(const Matrix<ScalarType>& A) :
  m_A(A),
  m_betaL(minimum(A.numRows(), A.numColumns()), static_cast<ScalarType>(0)),
  m_betaR(minimum(A.numRows(), A.numColumns()), static_cast<ScalarType>(0)),
  m_x(maximum(A.numRows(), A.numColumns())),
  m_v(maximum(A.numRows(), A.numColumns())),
  m_w(maximum(A.numRows(), A.numColumns()))
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Householder<ScalarType>::Householder(uint32_t m, uint32_t n) :
  m_A(m, n), m_betaL(minimum(m, n), static_cast<ScalarType>(0)),
  m_betaR(minimum(m, n), static_cast<ScalarType>(0)),
  m_x(maximum(m, n)), m_v(maximum(m, n)), m_w(maximum(m, n))
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Householder<ScalarType>::house(uint32_t n, const ScalarType* x, ScalarType* v)
{
  NMP_ASSERT(n > 0);

  // Length one vector
  if (n == 1)
  {
    v[0] = 1;
    return 0;
  }

  // Compute the householder vector v = x - ||x||*e_1, where e_1 is
  // the canonical unit vector with a 1 in the first element. This
  // algorithm is based on 'Matrix Computations' - Golub and Van Loan.
  const ScalarType xn = NMP::enorm<ScalarType, ScalarType>(n, x);
  if (xn == 0)
  {
    // v = e_1, beta = 0
    for (uint32_t i = 1; i < n; ++i) v[i] = 0;
    v[0] = 1;
    return 0;
  }

  // Perform preliminary scaling to avoid algorithm overflow. Note
  // that we can scale x by any factor since this does not affect the
  // solution. i.e. if v = k*u then (v*v^T)/(v^T*v) = (u*u^T)/(u^T*u)
  for (uint32_t i = 0; i < n; ++i) v[i] = x[i] / xn;

  // sigma = (x(2)^2 + ... + x(n)^2)
  ScalarType sigma = 0;
  for (uint32_t i = 1; i < n; ++i) sigma += v[i] * v[i];

  // Check if x(2:n) already contains zeros
  if (sigma == 0)
  {
    v[0] = 1; // Normalise the vector
    return 0;
  }

  // The choice of sign in v = x +/- ||x|| * e_1 is particularly important.
  // If we choose the negative sign then P*x is a positive multiple of e_1,
  // however this is dangerous if x is also close to a positive multiple of
  // e_1 because severe cancellation would occur.
  if (v[0] <= 0)
  {
    // Normal case: v(1) = x(1) - ||x||
    v[0] -= 1;
  }
  else
  {
    // The formula v(1) = x(1) - ||x|| = (x(1)^2 - ||x||^2) / (x(1) + ||x||),
    // suggested by Parlett (1971), does not suffer from the severe cancellation
    // defect in the x(1) > 0 case.

    // v(1) = -(x(2)^2 + ... + x(n)^2) / (x(1) + ||x||)
    v[0] = -sigma / (v[0] + 1);
  }

  // Compute beta = 2 / (v^T * v) and since we are normalizing the
  // householder vector so that its first element is one then we
  // must compensate by applying the appropriate scaling to beta.
  const ScalarType v1sq = v[0] * v[0];
  const ScalarType beta = 2 * v1sq / (sigma + v1sq);

  // Normalise the Householder vector
  for (uint32_t i = 1; i < n; ++i) v[i] /= v[0];
  v[0] = 1;

  return beta;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::house(Matrix<ScalarType>& P, ScalarType beta, const ScalarType* v)
{
  NMP_ASSERT(P.isSquare());

  // P = I - beta*(v * v^T)
  P.outer(v, v);
  P *= -beta;
  P.addDiagVector(0, static_cast<ScalarType>(1));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::transformLeft(uint32_t i, uint32_t sub)
{
  uint32_t m = m_A.numRows();
  uint32_t n = m_A.numColumns();

  // Start row index of the 1 in the householder vector
  uint32_t ii = i + sub;
  NMP_ASSERT(ii < m);

  // Get the i^th column vector within matrix A
  uint32_t len = m - ii;
  if (len <= 1)
  {
    m_betaL[i] = 0;
    return;
  }
  m_A.getColumnVector(i, ii, len, &m_x[ii]);

  // Compute the householder vector
  ScalarType beta = house(len, &m_x[ii], &m_v[ii]);

  // Apply the left householder update P*A, where P = I - beta*(v*v^T)
  if (beta != 0)
  {
    // The structure of P can be exploited to compute the update
    // in 4mn flops via a matrix vector multiply and an outer product
    // update. Compute: w = beta * A^T * v, then A' = A - v * w^T

    // Update the first column
    ScalarType w_j = m_x[ii];
    for (uint32_t k = ii + 1; k < m; ++k) w_j += m_x[k] * m_v[k];
    w_j *= beta;
    m_A.element(ii, i) = m_x[ii] - w_j; // Update first element (rest are zeroed!)

    // Update the remaining columns
    for (uint32_t j = i + 1; j < n; ++j)
    {
      m_A.getColumnVector(j, ii, len, &m_x[ii]);
      w_j = m_x[ii];
      for (uint32_t k = ii + 1; k < m; ++k) w_j += m_x[k] * m_v[k];
      w_j *= beta;
      m_x[ii] -= w_j;
      for (uint32_t k = ii + 1; k < m; ++k) m_x[k] -= m_v[k] * w_j;
      m_A.setColumnVector(j, ii, len, &m_x[ii]);
    }
  }

  // Store the essential part of the householder vector back
  // in matrix A
  m_A.setColumnVector(i, ii + 1, len - 1, &m_v[ii+1]);

  // Store the beta parameter
  m_betaL[i] = beta;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::transformLeft(uint32_t sub)
{
  uint32_t m = m_A.numRows();
  uint32_t n = m_A.numColumns();

  // Compute the number of columns to eliminate
  NMP_ASSERT(m > sub);
  uint32_t c = minimum(m - sub, n);

  // Zero the unused beta parameters
  uint32_t d = minimum(m, n);
  for (uint32_t i = c; i < d; ++i) m_betaL[i] = 0;

  // Eliminate the columns below the specified sub-diagonal
  for (uint32_t i = 0; i < c; ++i) transformLeft(i, sub);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::transformRight(uint32_t i, uint32_t super)
{
  uint32_t m = m_A.numRows();
  uint32_t n = m_A.numColumns();

  // Start column index of the 1 in the householder vector
  uint32_t ii = i + super;
  NMP_ASSERT(ii < n);

  // Get the i^th row vector within matrix A
  uint32_t len = n - ii;
  if (len <= 1)
  {
    m_betaR[i] = 0;
    return;
  }
  m_A.getRowVector(i, ii, len, &m_x[ii]);

  // Compute the householder vector
  ScalarType beta = house(len, &m_x[ii], &m_v[ii]);

  // Apply the right householder update A*P, where P = I - beta*(v*v^T)
  if (beta != 0)
  {
    // The structure of P can be exploited to compute the update
    // in 4mn flops via a matrix vector multiply and an outer product
    // update. Compute: w = beta * A * v, then A' = A - w * v^T

    // Update the first row
    ScalarType w_j = m_x[ii];
    for (uint32_t k = ii + 1; k < n; ++k) w_j += m_x[k] * m_v[k];
    w_j *= beta;
    m_A.element(i, ii) = m_x[ii] - w_j; // Update first element (rest are zeroed!)

    // Update the remaining rows
    for (uint32_t j = i + 1; j < m; ++j)
    {
      m_A.getRowVector(j, ii, len, &m_x[ii]);
      w_j = m_x[ii];
      for (uint32_t k = ii + 1; k < n; ++k) w_j += m_x[k] * m_v[k];
      w_j *= beta;
      m_x[ii] -= w_j;
      for (uint32_t k = ii + 1; k < n; ++k) m_x[k] -= m_v[k] * w_j;
      m_A.setRowVector(j, ii, len, &m_x[ii]);
    }
  }

  // Store the essential part of the householder vector back
  // in matrix A
  m_A.setRowVector(i, ii + 1, len - 1, &m_v[ii+1]);

  // Store the beta parameter
  m_betaR[i] = beta;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::transformRight(uint32_t super)
{
  uint32_t m = m_A.numRows();
  uint32_t n = m_A.numColumns();

  // Compute the number of rows to eliminate
  NMP_ASSERT(n > super);
  uint32_t c = minimum(n - super, m);

  // Zero the unused beta parameters
  uint32_t d = minimum(m, n);
  for (uint32_t i = c; i < d; ++i) m_betaR[i] = 0;

  // Eliminate the rows right of the specified super-diagonal
  for (uint32_t i = 0; i < c; ++i) transformRight(i, super);
}

//----------------------------------------------------------------------------------------------------------------------
void Householder<ScalarType>::transformSym(uint32_t i, uint32_t band)
{
  // Check we are not attempting to do anything foolish,
  // i.e. like diagonalize the matrix
  NMP_ASSERT(m_A.isSquare());
  NMP_ASSERT(band > 0);

  uint32_t n = m_A.numColumns();

  // Start column index of the 1 in the householder vector
  uint32_t ii = i + band;
  NMP_ASSERT(ii < n);

  // Get the i^th row vector within matrix A
  uint32_t len = n - ii;
  if (len <= 1)
  {
    m_betaL[i] = m_betaR[i] = 0;
    return;
  }
  m_A.getRowVector(i, ii, len, &m_x[ii]);

  // Compute the householder vector
  ScalarType beta = house(len, &m_x[ii], &m_v[ii]);

  // Apply the left and right householder update A' = P*A*P,
  // where P = I - beta*(v*v^T). Since A is symmetric only update
  // the elements of the upper triangle
  if (beta != 0)
  {
    // Update the first row
    ScalarType w_j = m_x[ii];
    for (uint32_t k = ii + 1; k < n; ++k) w_j += m_x[k] * m_v[k];
    w_j *= beta;
    m_A.element(i, ii) = m_x[ii] - w_j; // Update first element (rest are zeroed!)

    // Apply the right transform to elements within the band (elements within
    // the band on the symmetric side are the same)
    for (uint32_t j = i + 1; j < ii; ++j)
    {
      m_A.getRowVector(j, ii, len, &m_x[ii]);
      w_j = m_x[ii];
      for (uint32_t k = ii + 1; k < n; ++k) w_j += m_x[k] * m_v[k];
      w_j *= beta;
      m_x[ii] -= w_j;
      for (uint32_t k = ii + 1; k < n; ++k) m_x[k] -= m_v[k] * w_j;
      m_A.setRowVector(j, ii, len, &m_x[ii]);
    }

    // Apply the symmetric transformation to the remaining square block B.
    // We only need to compute elements within upper triangle.

    // p = beta*A*v
    for (uint32_t j = ii; j < n; ++j)
    {
      m_A.getRowVector(j, ii, len, &m_x[ii]);
      ScalarType bv = m_x[ii] * m_v[ii];
      for (uint32_t k = ii + 1; k < n; ++k) bv += m_x[k] * m_v[k];
      m_w[j] = beta * bv;
    }

    // w = p - (0.5*beta * p^T*v) * v
    ScalarType pv = m_w[ii] * m_v[ii];
    for (uint32_t k = ii + 1; k < n; ++k) pv += m_w[k] * m_v[k];
    pv *= (beta / 2);
    for (uint32_t k = ii; k < n; ++k) m_w[k] -= pv * m_v[k];

    // P*A*P = A - v*w^T - w*v^T (upper triangle only)
    for (uint32_t j = ii; j < n; ++j)
    {
      m_A.getRowVector(j, j, n - j, &m_x[j]);
      m_x[j] -= (2 * m_v[j] * m_w[j]);
      for (uint32_t k = j + 1; k < n; ++k)
      {
        m_x[k] -= (m_v[j] * m_w[k] + m_v[k] * m_w[j]);
      }
      m_A.setRowVector(j, j, n - j, &m_x[j]);
    }
  }

  // Store the essential part of the householder vector back
  // in matrix A
  m_A.setRowVector(i, ii + 1, len - 1, &m_v[ii+1]);

  // Copy the upper triangle into the lower triangle
  m_A.makeSymmetric();

  // Store the beta parameters
  m_betaL[i] = m_betaR[i] = beta;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::transformSym(uint32_t band)
{
  // Compute the number of rows to eliminate
  NMP_ASSERT(m_A.numRows() > band);
  uint32_t n = m_A.numRows() - band;

  // Eliminate the rows and columns: B = V^T * A * V
  for (uint32_t i = 0; i < n; ++i) transformSym(i, band);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::left(Matrix<ScalarType>& U, uint32_t sub)
{
  // R = U^T * A = (P_n * ... * P_1) A, Note also that P_i^T = P_i
  // hence U = P_1 * ... * P_n. The leading (i-1) by (i-1) sub-matrix
  // of P_i is the identity, so it is much more efficient to compute
  // U using backward accumulation.
  uint32_t m = m_A.numRows();
  uint32_t n = m_A.numColumns();
  uint32_t Un = U.numColumns();
  NMP_ASSERT(U.numRows() == m_A.numRows() && Un > 0);

  // Matrix A has the form: [U | *] [R; 0] hence the user can request
  // the number of columns they wish to recover from U. The area marked
  // by * then forms a dead zone, which we don't need to compute
  U.set(static_cast<ScalarType>(0));
  U.setDiagVector(0, static_cast<ScalarType>(1));

  // Compute the number of columns that were eliminated
  NMP_ASSERT(m > sub);
  uint32_t rc = minimum(m, n);
  uint32_t d = minimum(rc - sub, Un - sub);

  // Backwards accumulation of U
  for (int32_t i = d - 1; i >= 0; --i)
  {
    if (m_betaL[i] != 0)
    {
      // Start row index of "essential" part of vector
      uint32_t ii = i + sub + 1;
      uint32_t iim1 = ii - 1; // index of 1 in householder vector

      // Get the essential part of the Householder vector
      uint32_t len = m - ii; // length of essential part
      m_A.getColumnVector(i, ii, len, &m_v[ii]); // v

      // U' = (I - beta(i) * v(i)*v(i)^T) * U via a matrix vector multiply
      // and an outer product update. We don't need to perform computations
      // within the dead zone

      // Update the first column
      ScalarType w_j = m_betaL[i]; // beta * [1; zeros(len, 1)]^T * [1; v(ii:m)]
      m_x[iim1] = 1 - w_j; // Update first element: U' = U - v*w^T
      for (uint32_t k = ii; k < m; ++k) m_x[k] = -m_v[k] * w_j;
      U.setColumnVector(iim1, iim1, len + 1, &m_x[iim1]);

      // Update the remaining columns
      for (uint32_t j = ii; j < Un; ++j)
      {
        // Get the essential part of U. Note: U(ii-1, ii-1:n) = [1, 0 ... 0]
        U.getColumnVector(j, ii, len, &m_x[ii]); // U(ii:m, j)

        // w(j) = beta * \sum_k=ii^m U(k, j) * v(k)
        w_j = 0;
        for (uint32_t k = ii; k < m; ++k) w_j += m_x[k] * m_v[k];
        w_j *= m_betaL[i];

        // Perform the update: U' = U - v*w^T
        m_x[iim1] = -w_j;
        for (uint32_t k = ii; k < m; ++k) m_x[k] -= m_v[k] * w_j;
        U.setColumnVector(j, iim1, len + 1, &m_x[iim1]);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Householder<ScalarType>::left(uint32_t sub)
{
  // Recover the full left orthogonal matrix U
  Matrix<ScalarType> U(m_A.numRows(), m_A.numRows());
  left(U, sub);
  return U;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::right(Matrix<ScalarType>& V, uint32_t super)
{
  // L = A*V = A * (P_1 * ... * P_m). The leading (i-1) by (i-1)
  // sub-matrix of P_i is the identity, so it is much more efficient
  // to compute V using backward accumulation.
  uint32_t m = m_A.numRows();
  uint32_t n = m_A.numColumns();
  uint32_t Vn = V.numColumns();
  NMP_ASSERT(V.numRows() == m_A.numColumns() && Vn > 0);

  // Matrix A has the form: A [V | *] = [L | 0] hence the user can request
  // the number of columns they wish to recover from V. The area marked
  // by * then forms a dead zone, which we don't need to compute
  V.set(static_cast<ScalarType>(0));
  V.setDiagVector(0, static_cast<ScalarType>(1));

  // Compute the number of rows that were eliminated
  NMP_ASSERT(n > super);
  uint32_t rc = minimum(m, n);
  uint32_t d = minimum(rc - super, Vn - super);

  // Backwards accumulation of V
  for (int32_t i = d - 1; i >= 0; --i)
  {
    if (m_betaR[i] != 0)
    {
      // Start column index of "essential" part of vector
      uint32_t ii = i + super + 1;
      uint32_t iim1 = ii - 1; // index of 1 in householder vector

      // Get the essential part of the Householder vector
      uint32_t len = n - ii; // length of essential part
      m_A.getRowVector(i, ii, len, &m_v[ii]); // v

      // V' = V * (I - beta(i) * v(i)*v(i)^T) via a matrix vector multiply
      // and an outer product update. We don't need to perform computations
      // within the dead zone

      // Update the first row
      ScalarType w_j = m_betaR[i];
      m_x[iim1] = 1 - w_j;
      for (uint32_t k = ii; k < n; ++k) m_x[k] = -m_v[k] * w_j;
      V.setColumnVector(iim1, iim1, len + 1, &m_x[iim1]);

      // Update the remaining rows
      for (uint32_t j = ii; j < Vn; ++j)
      {
        // Get the essential part of V
        V.getColumnVector(j, ii, len, &m_x[ii]);

        // w(j) = beta * \sum_k=ii^m V(k, j) * v(k)
        w_j = 0;
        for (uint32_t k = ii; k < n; ++k) w_j += m_x[k] * m_v[k];
        w_j *= m_betaR[i];

        // Perform the update: V' = V - w*v^T
        m_x[iim1] = -w_j;
        for (uint32_t k = ii; k < n; ++k) m_x[k] -= m_v[k] * w_j;
        V.setColumnVector(j, iim1, len + 1, &m_x[iim1]);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Householder<ScalarType>::right(uint32_t super)
{
  // Recover the full right orthogonal matrix V
  Matrix<ScalarType> V(m_A.numColumns(), m_A.numColumns());
  right(V, super);
  return V;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::vectorLeft(Vector<ScalarType>& v, uint32_t i, uint32_t sub) const
{
  uint32_t m = m_A.numRows();
  NMP_ASSERT(v.numElements() == m);

  // Get the essential part of the Householder vector
  uint32_t ii = i + sub + 1;
  uint32_t len = m - ii; // length of essential part
  m_A.getColumnVector(i, ii, len, &v[ii]);

  // Place zeros into the vector and add the 1
  ii--;
  for (uint32_t k = 0; k < ii; ++k) v[k] = static_cast<ScalarType>(0);
  v[ii] = static_cast<ScalarType>(1);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Householder<ScalarType>::vectorLeft(uint32_t i, uint32_t sub) const
{
  Vector<ScalarType> v(m_A.numRows());
  vectorLeft(v, i, sub);
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::vectorRight(Vector<ScalarType>& v, uint32_t i, uint32_t super) const
{
  uint32_t n = m_A.numColumns();
  NMP_ASSERT(v.numElements() == n);

  // Get the essential part of the Householder vector
  uint32_t ii = i + super + 1;
  uint32_t len = n - ii; // length of essential part
  m_A.getRowVector(i, ii, len, &v[ii]);

  // Place zeros into the vector and add the 1
  ii--;
  for (uint32_t k = 0; k < ii; ++k) v[k] = static_cast<ScalarType>(0);
  v[ii] = static_cast<ScalarType>(1);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Householder<ScalarType>::vectorRight(uint32_t i, uint32_t super) const
{
  Vector<ScalarType> v(m_A.numColumns());
  vectorRight(v, i, super);
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Householder<ScalarType>::betaLeft(uint32_t i) const
{
  return m_betaL[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Householder<ScalarType>::betaRight(uint32_t i) const
{
  return m_betaR[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::clearBetaLeft()
{
  m_betaL.set(static_cast<ScalarType>(0));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::clearBetaRight()
{
  m_betaR.set(static_cast<ScalarType>(0));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::
PLeft(Matrix<ScalarType>& P, uint32_t i, uint32_t sub)
{
  // *** This function is not optimized! ***
  NMP_ASSERT(P.isSize(m_A.numRows(), m_A.numRows()));

  // Get the i^th left Householder vector
  vectorLeft(m_v, i, sub);
  ScalarType beta = m_betaL[i];

  // Compute P = I - beta*(v * v^T)
  house(P, beta, &m_v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Householder<ScalarType>::PLeft(uint32_t i, uint32_t sub)
{
  uint32_t m = m_A.numRows();
  Matrix<ScalarType> P(m, m);
  PLeft(P, i, sub);
  return P;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Householder<ScalarType>::PRight(Matrix<ScalarType>& P, uint32_t i, uint32_t super)
{
  NMP_ASSERT(P.isSize(m_A.numColumns() , m_A.numColumns()));

  // Get the i^th left Householder vector
  vectorRight(m_v, i, super);
  ScalarType beta = m_betaR[i];

  // Compute P = I - beta*(v * v^T)
  house(P, beta, &m_v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Householder<ScalarType>::PRight(uint32_t i, uint32_t super)
{
  uint32_t n = m_A.numColumns();
  Matrix<ScalarType> P(n, n);
  PRight(P, i, super);
  return P;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& Householder<ScalarType>::getA()
{
  return m_A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& Householder<ScalarType>::getA() const
{
  return m_A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& Householder<ScalarType>::getX()
{
  return m_x;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& Householder<ScalarType>::getV()
{
  return m_v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& Householder<ScalarType>::getW()
{
  return m_w;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
