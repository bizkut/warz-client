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
// BandDiagSolverLU
//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverLU<ScalarType>::~BandDiagSolverLU()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverLU<ScalarType>::BandDiagSolverLU(const BandDiagMatrix<ScalarType>& A, const Matrix<ScalarType>& B) :
  m_L(A.numSub(), A.numColumns() - 1), m_U(A.numRows(), A.bandWidth()),
  m_Pk(A.numRows()),
  m_X(B.numRows(), B.numColumns()),
  m_p(A.numRows()), m_u(A.bandWidth()), m_v(A.bandWidth())
{
  NMP_ASSERT(A.numRows() == A.numColumns());
  NMP_ASSERT(B.numRows() == A.numColumns());
  if (decompose(A) == BandDiagSolverLUStatus::Success) solve(B);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverLU<ScalarType>::BandDiagSolverLU(const BandDiagMatrix<ScalarType>& A, const Vector<ScalarType>& b) :
  m_L(A.numSub(), A.numColumns() - 1), m_U(A.numRows(), A.bandWidth()),
  m_Pk(A.numRows()), m_X(b.numElements(), 1),
  m_p(A.numRows()), m_u(A.bandWidth()), m_v(A.bandWidth())
{
  NMP_ASSERT(A.numRows() == A.numColumns());
  NMP_ASSERT(b.numElements() == A.numColumns());
  if (decompose(A) == BandDiagSolverLUStatus::Success)
  {
    m_p = b;
    solve(m_p);
    m_X.setColumnVector(0, m_p);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverLU<ScalarType>::BandDiagSolverLU(uint32_t An, uint32_t sub, uint32_t super, uint32_t Bc) :
  m_L(sub, An - 1), m_U(An, sub + super + 1), m_X(An, Bc), m_Pk(An),
  m_p(An), m_u(sub + super + 1), m_v(sub + super + 1)
{
  NMP_ASSERT(An > 1 && Bc > 0);
  NMP_ASSERT(sub > 0 && super > 0);
  m_status = BandDiagSolverLUStatus::BadParams;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverLU<ScalarType>::BandDiagSolverLU(const BandDiagMatrix<ScalarType>& A) :
  m_L(A.numSub(), A.numColumns() - 1), m_U(A.numRows(), A.bandWidth()),
  m_Pk(A.numRows()), m_X(A.numRows(), A.numRows()),
  m_p(A.numRows()), m_u(A.bandWidth()), m_v(A.bandWidth())
{
  NMP_ASSERT(A.numRows() == A.numColumns());
  invert(A);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverLU<ScalarType>::BandDiagSolverLU(uint32_t An, uint32_t sub, uint32_t super) :
  m_L(sub, An - 1), m_U(An, sub + super + 1), m_X(An, An), m_Pk(An),
  m_p(An), m_u(sub + super + 1), m_v(sub + super + 1)
{
  NMP_ASSERT(An > 0 && sub > 0 && super > 0);
  m_status = BandDiagSolverLUStatus::BadParams;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagSolverLU<ScalarType>::flattenRows(const BandDiagMatrix<ScalarType>& B)
{
  uint32_t n = B.numRows();
  uint32_t w = B.bandWidth();
  uint32_t sub = B.numSub();
  NMP_ASSERT(n == B.numColumns() && n == m_U.numRows());
  NMP_ASSERT(sub == m_L.numRows());
  NMP_ASSERT(w == m_U.numColumns());

  uint32_t i;
  for (i = 0; i <= sub; ++i)
  {
    B.getRowVector(i, 0, w, &m_v[0]);
    m_U.setRowVector(i, m_v);
  }
  for (; i < n; ++i)
  {
    B.getRowVector(i, i - sub, w, &m_v[0]);
    m_U.setRowVector(i, m_v);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverLUStatus::eType BandDiagSolverLU<ScalarType>::decompose(const BandDiagMatrix<ScalarType>& A)
{
  NMP_ASSERT(A.isSize(m_U.numRows(), m_U.numRows()));
  flattenRows(A);
  m_status = doDecomposition();
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverLUStatus::eType BandDiagSolverLU<ScalarType>::invert(const BandDiagMatrix<ScalarType>& A)
{
  // First decompose A into LU form
  decompose(A);
  if (m_status != BandDiagSolverLUStatus::Success) return m_status;

  // Invert by columns
  for (uint32_t i = 0; i < m_U.numRows(); ++i)
  {
    m_p.set(static_cast<ScalarType>(0));
    m_p[i] = static_cast<ScalarType>(1);
    solve(m_p);
    m_X.setColumnVector(i, m_p);
  }

  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverLUStatus::eType BandDiagSolverLU<ScalarType>::solve(
  const BandDiagMatrix<ScalarType>& A,
  const Matrix<ScalarType>&         B)
{
  if (decompose(A) == BandDiagSolverLUStatus::Success) solve(B);
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagSolverLU<ScalarType>::solve(const Matrix<ScalarType>& B)
{
  NMP_ASSERT(m_X.isSize(B.numRows(), B.numColumns()));
  NMP_ASSERT(m_status == BandDiagSolverLUStatus::Success);

  // Solve by columns
  for (uint32_t i = 0; i < B.numColumns(); ++i)
  {
    B.getColumnVector(i, &m_p[0]);
    solve(m_p);
    m_X.setColumnVector(i, m_p);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// (L U) x = b
template <>
void BandDiagSolverLU<ScalarType>::solve(Vector<ScalarType>& b)
{
  NMP_ASSERT(b.numElements() == m_U.numRows());
  NMP_ASSERT(m_status == BandDiagSolverLUStatus::Success);

  // Since we have decomposed a row-wise permutation of A i.e.
  // (M_{n-1} P_{n-1} ... M_1 P_1) A = U, where M_i are the
  // Gauss transforms and P_i are the permutation matrices, then
  // we can solve the linear system A x = b by solving
  // U x = (M_{n-1} P_{n-1} ... M_1 P_1) b.
  uint32_t n = m_U.numRows() - 1;
  uint32_t len, ext = n - m_L.numRows(); // row window extent

  // Compute the transformed vector: (M_{n-1} P_{n-1} ... M_1 P_1) b
  for (uint32_t i = 0; i < n; ++i)
  {
    // P_i
    uint32_t pi = (uint32_t)m_Pk[i]; // pivot index
    if ((uint32_t)m_Pk[pi] != i)
    {
      ScalarType y = b[pi]; // element in b moved to i
      b[pi] = b[i]; b[i] = y; // swap with element currently at i
    }

    // if b_i is zero then the Gauss transform has no effect
    if (b[i] == static_cast<ScalarType>(0)) continue;

    // Adjust the length of the band segment (last sub rows)
    len = m_L.numRows();
    if (i > ext) len -= (i - ext);

    // M_i = I - v*(e_i)^{ T }
    m_L.getColumnVector(i, 0, len, &m_v[0]);
    for (uint32_t k = 0; k < len; ++k) b[(i+1)+k] -= m_v[k] * b[i];
  }
  // Last permutation is identity so don't bother

  // Back substitution step: U x = y
  len = 1;
  ext = n - m_U.numColumns() + 1;
  for (int32_t i = n; i >= 0; --i)
  {
    // Get the current row of U (>= i)
    m_U.getRowVector(i, 0, len, &m_v[0]);

    ScalarType x = b[i];
    for (uint32_t k = 1; k < len; ++k) x -= m_v[k] * b[i+k];
    b[i] = x / m_v[0]; // Set solution x inplace back in b

    // Adjust the length of the band segment (last sub+super rows)
    if ((uint32_t)i > ext) len++;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Perform LU decomposition using Gauss transforms and partial pivoting
template <>
BandDiagSolverLUStatus::eType BandDiagSolverLU<ScalarType>::doDecomposition()
{
  uint32_t k, len = m_L.numRows() + 1; // sub + diag
  uint32_t w = m_U.numColumns(); // band width
  uint32_t n = m_U.numRows(); // rows
  uint32_t ext = n - len; // absolute sub band extent
  uint32_t ext2 = n - w; // absolute sub+super band extent

  // Iterate over the rows
  for (k = 0; k < n - 1; ++k)
  {
    // Adjust the length of the band segment (last sub rows)
    if (k > ext) len--;

    // Get the k^th column vector (>= diagonal) of size len
    m_U.getColumnVector(0, k, len, &m_v[0]);

    // Interchange permutation matrices can be used within the decomposition
    // to ensure that no multiplier in the Gauss vector v is greater than one
    // in absolute value. v = ( a_{k+1,k}/a_{k,k}, ... , a_{n-1,k}/a_{k,k} )^{T}
    // This ensures that no large entries appear in the computed triangular
    // factors L and U, and is achieved by placing the largest element in the
    // k^th column (>= diagonal) onto the diagonal by interchanging rows.

    // Find the largest absolute element value within the vector
    uint32_t pi = 0; // pivot index (relative to diagonal)
    ScalarType pv = abs(m_v[0]);
    for (uint32_t i = 1; i < len; ++i)
    {
      ScalarType val = abs(m_v[i]);
      if (val > pv)
      {
        pv = val;
        pi = i;
      }
    }

    // Check for matrix singularity
    if (pv == static_cast<ScalarType>(0)) return BandDiagSolverLUStatus::Singular;

    // Do we need to perform the row interchange
    pv = m_v[pi]; // pivot value that ends up on the diagonal
    if (pi != 0)
    {
      m_v[pi] = m_v[0]; // move current diagonal to the pivot's position
      pi += k; // absolute index of pivot position

      // Interchange rows. Note this exchange can cause fill in elements to
      // appear in the super < x < super + sub band within the band diagonal
      // matrix. Hence the reason for storing U in the storage area of A.
      m_U.swapRows(pi, k);

      m_Pk[k] = (int32_t)pi; // record the permutation p(k)
    }
    else
    {
      m_Pk[k] = (int32_t)k; // record the permutation p(k)
    }

    // Adjust the width of the sub+super band vector
    // Zero the invalid segment of the row vector (may contain values from exchanges)
    if (k > ext2)
    {
      w--;
      m_U.setRowVector(k, w, m_U.numColumns() - w, static_cast<ScalarType>(0));
    }

    // Apply the Gauss transform M = I - v*(e_k)^{T}
    // where I is the identity matrix and e_k is the k^th canonical column vector
    // Since we know this zeros the k^th column apply it to the > k^th columns
    ScalarType s = static_cast<ScalarType>(1) / pv;
    m_U.getRowVector(k, 0, w, &m_u[0]); // C(k,:)
    for (uint32_t i = 1; i < len; ++i)
    {
      // Gauss vector: v = ( a_{k+1,k}/a_{k,k}, ... , a_{n-1,k}/a_{k,k} )^{T}
      m_v[i] *= s;

      // Eliminate the k^th column and store the result of the gauss transform
      // back in U shifted one element to the left. Note this elimination strategy
      // places the diagonal elements of the banded U matrix in the first column
      // after len transform and shifts.
      uint32_t ki = k + i;
      m_U.getRowVector(ki, 0, w, &m_p[0]); // C(k+i,:)
      for (uint32_t j = 1; j < w; ++j) m_p[j-1] = m_p[j] - m_v[i] * m_u[j]; // C' = C - v*C(k,:)
      m_p[w-1] = static_cast<ScalarType>(0);
      m_U.setRowVector(ki, 0, w, &m_p[0]);
    }

    // Store the banded Gauss vector in matrix L
    // Since L = M_1^{-1} M_2^{-1} ... M_{n-1}^{-1} then by happy coincidence
    // L = I + \sum_{k=1}^{n-1} v(k) * (e_k)^{T} i.e. the set of Gauss vectors
    for (uint32_t i = len; i <= m_L.numRows(); ++i) m_v[i] = static_cast<ScalarType>(0);
    m_L.setColumnVector(k, 0, m_L.numRows(), &m_v[1]);
  }

  // Zero the invalid segment of the last row vector (may contain values from exchanges)
  m_U.setRowVector(k, 1, m_U.numColumns() - 1, static_cast<ScalarType>(0));
  m_Pk[k] = (int32_t)k; // Record the last identity permutation

  return BandDiagSolverLUStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType BandDiagSolverLU<ScalarType>::determinant() const
{
  // Avoid overflowing the fpu by keeping track of the sign and the
  // logarithm of the absolute values of the diagonal elements in U
  uint32_t n = m_U.numRows();
  ScalarType sum = static_cast<ScalarType>(0);
  int32_t sign = 1;

  for (uint32_t i = 0; i < n; ++i)
  {
    // Keep track of the number of row exchanges
    if ((uint32_t)m_Pk[i] != i) sign = -sign;

    // Keep track of the sign of each element. The diagonal elements
    // correspond to the first column of the U matrix
    ScalarType e = m_U.element(i, 0);
    if (e < 0)
    {
      sign = -sign;
      e = -e;
    }
    sum += log(e); // Update the sum of the logarithms
  }

  return sign * exp(sum);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& BandDiagSolverLU<ScalarType>::getX() const
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& BandDiagSolverLU<ScalarType>::getX()
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverLUStatus::eType BandDiagSolverLU<ScalarType>::getStatus() const
{
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
