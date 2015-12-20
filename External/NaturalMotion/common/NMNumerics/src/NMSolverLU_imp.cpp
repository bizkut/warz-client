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
// SolverLU
//----------------------------------------------------------------------------------------------------------------------
template <>
SolverLU<ScalarType>::~SolverLU()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverLU<ScalarType>::SolverLU(const Matrix<ScalarType>& A, const Matrix<ScalarType>& B) :
  m_A(A.numRows(), A.numColumns()), m_X(B.numRows(), B.numColumns()),
  m_Pk(A.numRows()), m_p(A.numRows()), m_v(A.numRows())
{
  NMP_ASSERT(m_A.numRows() == m_A.numColumns());
  NMP_ASSERT(B.numRows() == m_A.numColumns());
  if (decompose(A) == SolverLUStatus::Success) solve(B);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverLU<ScalarType>::SolverLU(const Matrix<ScalarType>& A, const Vector<ScalarType>& b) :
  m_A(A.numRows(), A.numColumns()), m_X(b.numElements(), 1),
  m_Pk(A.numRows()), m_p(A.numRows()), m_v(A.numRows())
{
  NMP_ASSERT(m_A.numRows() == m_A.numColumns());
  NMP_ASSERT(b.numElements() == m_A.numColumns());
  if (decompose(A) == SolverLUStatus::Success)
  {
    m_p = b;
    solve(m_p);
    m_X.setColumnVector(0, m_p);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverLU<ScalarType>::SolverLU(uint32_t An, uint32_t Bc) :
  m_A(An, An), m_X(An, Bc), m_Pk(An), m_p(An), m_v(An)
{
  NMP_ASSERT(An > 0 && Bc > 0);
  m_status = SolverLUStatus::BadParams;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverLU<ScalarType>::SolverLU(const Matrix<ScalarType>& A) :
  m_A(A.numRows(), A.numColumns()), m_X(A.numRows(), A.numColumns()),
  m_Pk(A.numRows()), m_p(A.numRows()), m_v(A.numRows())
{
  NMP_ASSERT(m_A.numRows() == m_A.numColumns());
  invert(A);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverLU<ScalarType>::SolverLU(uint32_t An) :
  m_A(An, An), m_X(An, An), m_Pk(An), m_p(An), m_v(An)
{
  NMP_ASSERT(An > 0);
  m_status = SolverLUStatus::BadParams;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverLUStatus::eType SolverLU<ScalarType>::decompose(const Matrix<ScalarType>& A)
{
  NMP_ASSERT(A.isSize(m_A.numRows(), m_A.numColumns()));
  m_A = A;
  m_status = doDecomposition();
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverLUStatus::eType SolverLU<ScalarType>::invert(const Matrix<ScalarType>& A)
{
  // First decompose A into LU form
  decompose(A);
  if (m_status != SolverLUStatus::Success) return m_status;

  // Invert by columns
  for (uint32_t i = 0; i < m_A.numColumns(); ++i)
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
void SolverLU<ScalarType>::solve(const Matrix<ScalarType>& B)
{
  NMP_ASSERT(m_X.isSize(B.numRows(), B.numColumns()));
  NMP_ASSERT(m_status == SolverLUStatus::Success);

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
void SolverLU<ScalarType>::solve(Vector<ScalarType>& b)
{
  NMP_ASSERT(b.numElements() == m_A.numRows());
  NMP_ASSERT(m_status == SolverLUStatus::Success);

  // Since we have decomposed a row-wise permutation of A i.e.
  // L U = P A, where P is a permutation matrix that swaps the
  // rows of A then we must solve the linear system P A x = P b
  uint32_t n = m_A.numRows();

  // If b (after permuting) contains zeros in the upper part of the
  // vector then the corresponding columns in L have no effect. bn
  // records the number of consecutive zero elements in the vector
  uint32_t bn = 0;

  // Forward substitution step: L y = P b
  // We apply each permutation M_i of P = M_{n-1} M_{n-2} ... M_1
  // in lock-step with each row of L and b
  for (uint32_t i = 0; i < n; ++i)
  {
    uint32_t pi = (uint32_t)m_Pk[i]; // pivot index
    ScalarType y = b[pi]; // element in b moved to i
    b[pi] = b[i]; // swap with element currently at i

    // Get the current row of L (< i)
    m_A.getRowVector(i, 0, i, &m_v[0]);

    // Check if we are still searching for the first non-zero element in b
    if (bn > 0)
    {
      for (uint32_t k = bn - 1; k < i; ++k) y -= m_v[k] * b[k];
    }
    else if (y != static_cast<ScalarType>(0))
    {
      bn = i + 1;
    }

    b[i] = y; // Set y inplace back in b
  }

  // Back substitution step: U x = y
  for (int32_t i = n - 1; i >= 0; --i)
  {
    // Get the current row of U (>= i)
    m_A.getRowVector(i, i, n - i, &m_v[i]);

    ScalarType x = b[i];
    for (uint32_t k = (uint32_t)i + 1; k < n; ++k) x -= m_v[k] * b[k];
    b[i] = x / m_v[i]; // Set solution x inplace back in b
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Perform LU decomposition using Gauss transforms and partial pivoting
// as described in algorithm 3.4.1 in "Matrix Computations" [Golub & Van Loan].
// Decompose A (well a row-wise permutation of A):
// |  1                  | | b_00  b_01  b_02  b_03 |
// | a_10   1            | |       b_11  b_12  b_13 |
// | a_20  a_21   1      | |             b_22  b_23 |
// | a_30  a_31  a_32  1 | |                   b_33 |
template <>
SolverLUStatus::eType SolverLU<ScalarType>::doDecomposition()
{
  // Iterate over the rows
  uint32_t k, n = m_A.numRows();
  for (k = 0; k < n - 1; ++k)
  {
    // Get the k^th column vector (>= diagonal)
    uint32_t m = n - k; // length of array
    m_A.getColumnVector(k, k, m, &m_v[0]);

    // Interchange permutation matrices can be used within the decomposition
    // to ensure that no multiplier in the Gauss vector v is greater than one
    // in absolute value. v = ( a_{k+1,k}/a_{k,k}, ... , a_{n-1,k}/a_{k,k} )^{T}
    // This ensures that no large entries appear in the computed triangular
    // factors L and U, and is achieved by placing the largest element in the
    // k^th column (>= diagonal) onto the diagonal by interchanging rows.

    // Find the largest absolute element value within the vector
    uint32_t pi = 0; // pivot index (relative to diagonal)
    ScalarType pv = abs(m_v[0]);
    for (uint32_t i = 1; i < m; ++i)
    {
      ScalarType val = abs(m_v[i]);
      if (val > pv)
      {
        pv = val;
        pi = i;
      }
    }

    // Check for matrix singularity
    if (pv == static_cast<ScalarType>(0)) return SolverLUStatus::Singular;

    // Do we need to perform the row interchange
    pv = m_v[pi]; // pivot value that ends up on the diagonal
    if (pi != 0)
    {
      m_v[pi] = m_v[0]; // move current diagonal to the pivot's position
      pi += k; // absolute index of pivot position
      m_A.swapRows(pi, k); // interchange rows (entire row: as in Golub)
      m_Pk[k] = (int32_t)pi; // record the permutation p(k)
    }
    else
    {
      m_Pk[k] = (int32_t)k; // record the permutation p(k)
    }

    // Gauss vector: v = ( a_{k+1,k}/a_{k,k}, ... , a_{n-1,k}/a_{k,k} )^{T}
    ScalarType s = static_cast<ScalarType>(1) / pv;
    for (uint32_t i = 1; i < m; ++i) m_v[i] *= s;

    // Apply the Gauss transform M = I - v*(e_k)^{T}
    // where I is the identity matrix and e_k is the k^th canonical column vector
    // Since we know this zeros the k^th column apply it to the > k^th columns
    m_A.gaussTransform(k, &m_v[1], k + 1, m - 1);

    // Store the Gauss vector in the zeroed elements below the diagonal.
    // Since L = M_1^{-1} M_2^{-1} ... M_{n-1}^{-1} then by happy coincidence
    // L = I + \sum_{k=1}^{n-1} v(k) * (e_k)^{T} i.e. the set of Gauss vectors
    m_A.setColumnVector(k, k + 1, m - 1, &m_v[1]);
  }
  m_Pk[k] = (int32_t)k; // Record the last identity permutation

  return SolverLUStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType SolverLU<ScalarType>::determinant() const
{
  // Avoid overflowing the fpu by keeping track of the sign and the
  // logarithm of the absolute values of the diagonal elements
  uint32_t n = m_A.numRows();
  ScalarType sum = static_cast<ScalarType>(0);
  int32_t sign = 1;

  for (uint32_t i = 0; i < n; ++i)
  {
    // Keep track of the number of row exchanges
    if ((uint32_t)m_Pk[i] != i) sign = -sign;

    // Keep track of the sign of each element
    ScalarType e = m_A.element(i, i);
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
const Matrix<ScalarType>& SolverLU<ScalarType>::getX() const
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& SolverLU<ScalarType>::getX()
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverLUStatus::eType SolverLU<ScalarType>::getStatus() const
{
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool SolverLU<ScalarType>::fail() const
{
  return m_status != SolverLUStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& SolverLU<ScalarType>::getA()
{
  return m_A;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
