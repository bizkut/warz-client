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
// SolverCholesky
//----------------------------------------------------------------------------------------------------------------------
template <>
SolverCholesky<ScalarType>::~SolverCholesky()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverCholesky<ScalarType>::SolverCholesky(const Matrix<ScalarType>& A, const Matrix<ScalarType>& B) :
  m_L(A.numRows(), A.numColumns()), m_X(B.numRows(), B.numColumns()),
  m_p(A.numRows()), m_v(A.numRows())
{
  NMP_ASSERT(m_L.numRows() == m_L.numColumns());
  NMP_ASSERT(B.numRows() == m_L.numColumns());
  if (decompose(A) == SolverCholeskyStatus::Success) solve(B);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverCholesky<ScalarType>::SolverCholesky(const Matrix<ScalarType>& A, const Vector<ScalarType>& b) :
  m_L(A.numRows(), A.numColumns()), m_X(b.numElements(), 1),
  m_p(A.numRows()), m_v(A.numRows())
{
  NMP_ASSERT(m_L.numRows() == m_L.numColumns());
  NMP_ASSERT(b.numElements() == m_L.numColumns());
  if (decompose(A) == SolverCholeskyStatus::Success)
  {
    m_p = b;
    solve(m_p);
    m_X.setColumnVector(0, m_p);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverCholesky<ScalarType>::SolverCholesky(uint32_t An, uint32_t Bc) :
  m_L(An, An), m_X(An, Bc), m_p(An), m_v(An)
{
  NMP_ASSERT(An > 0 && Bc > 0);
  m_status = SolverCholeskyStatus::BadParams;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverCholesky<ScalarType>::SolverCholesky(const Matrix<ScalarType>& A) :
  m_L(A.numRows(), A.numColumns()), m_X(A.numRows(), A.numColumns()),
  m_p(A.numRows()), m_v(A.numRows())
{
  NMP_ASSERT(m_L.numRows() == m_L.numColumns());
  invert(A);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverCholesky<ScalarType>::SolverCholesky(uint32_t An) :
  m_L(An, An), m_X(An, An), m_p(An), m_v(An)
{
  NMP_ASSERT(An > 0);
  m_status = SolverCholeskyStatus::BadParams;
}

//----------------------------------------------------------------------------------------------------------------------
// Perform Cholesky decomposition
template <>
SolverCholeskyStatus::eType SolverCholesky<ScalarType>::decompose(const Matrix<ScalarType>& A)
{
  setLower(A);
  m_status = doDecomposition();
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverCholeskyStatus::eType SolverCholesky<ScalarType>::invert(const Matrix<ScalarType>& A)
{
  // First decompose A into L L^T form
  if (decompose(A) != SolverCholeskyStatus::Success) return m_status;

  // Invert by columns
  for (uint32_t i = 0; i < m_L.numColumns(); ++i)
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
SolverCholeskyStatus::eType SolverCholesky<ScalarType>::solve(
  const Matrix<ScalarType>& A,
  const Matrix<ScalarType>& B)
{
  if (decompose(A) == SolverCholeskyStatus::Success)
  {
    solve(B);
  }
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverCholeskyStatus::eType SolverCholesky<ScalarType>::solve(
  const Matrix<ScalarType>& A,
  const Vector<ScalarType>& b)
{
  if (decompose(A) == SolverCholeskyStatus::Success)
  {
    m_p = b;
    solve(m_p);
    m_X.setColumnVector(0, m_p);
  }
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SolverCholesky<ScalarType>::solve(const Matrix<ScalarType>& B)
{
  NMP_ASSERT(m_X.isSize(B.numRows(), B.numColumns()));
  NMP_ASSERT(m_status == SolverCholeskyStatus::Success);

  // Solve by columns
  for (uint32_t i = 0; i < B.numColumns(); ++i)
  {
    B.getColumnVector(i, &m_p[0]);
    solve(m_p);
    m_X.setColumnVector(i, m_p);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// (L L^T) x = b
template <>
void SolverCholesky<ScalarType>::solve(Vector<ScalarType>& b)
{
  NMP_ASSERT(b.numElements() == m_L.numRows());
  NMP_ASSERT(m_status == SolverCholeskyStatus::Success);

  // Forward substitution step: L y = b
  uint32_t n = m_L.numRows();
  for (uint32_t i = 0; i < n; ++i)
  {
    ScalarType y = b[i];

    // Get the current row of L
    m_L.getRowVector(i, 0, i + 1, &m_v[0]);

    // Substitute columns < i into the right hand side
    for (uint32_t k = 0; k < i; ++k) y -= m_v[k] * b[k];
    b[i] = y / m_v[i]; // Set y inplace back in b
  }

  // Back substitution step: L^T x = y
  for (int32_t i = n - 1; i >= 0; --i)
  {
    // Get the current column of L
    m_L.getColumnVector(i, i, n - i, &m_v[i]);

    ScalarType x = b[i];
    for (uint32_t k = (uint32_t)i + 1; k < n; ++k) x -= m_v[k] * b[k];
    b[i] = x / m_v[i]; // Set solution x inplace back in b
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SolverCholesky<ScalarType>::setLower(const Matrix<ScalarType>& A)
{
  NMP_ASSERT(m_L.isSize(A.numRows(), A.numColumns()));

  uint32_t n = A.numColumns();
  for (uint32_t i = 0; i < n; ++i)
  {
    uint32_t k = i + 1;
    A.getColumnVector(i, 0, k, &m_v[0]);
    for (; k < n; ++k) m_v[k] = static_cast<ScalarType>(0);
    m_L.setRowVector(i, m_v);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Main Cholesky decomposition routine that does not allocate memory
template <>
SolverCholeskyStatus::eType SolverCholesky<ScalarType>::doDecomposition()
{
  // Iterate down the rows of the lower triangle L
  uint32_t n = m_L.numRows();
  for (uint32_t i = 0; i < n; ++i)
  {
    // Get the i^th row of L. This starts off containing the elements of
    // the lower triangle of A which are replaced by the elements of L.
    m_L.getRowVector(i, 0, i + 1, &m_v[0]);

    // Iterate across the columns of the lower triangle L
    for (uint32_t j = 0; j < i; ++j)
    {
      // Get the j^th row of L whose elements have previously been computed
      m_L.getRowVector(j, 0, j + 1, &m_p[0]);

      // Compute the sum: A_{i,j} - \sum_{k=0}^{j-1} L_{i,k} * L_{j,k}
      ScalarType sum = m_v[j];
      for (int32_t k = j - 1; k >= 0; --k) sum -= m_v[k] * m_p[k];

      // L_ij = (A_ij - \sum_{k=0}^{j-1} L_ik * L_jk ) / L_jj
      m_v[j] = sum / m_p[j];
    }

    // Diagonal elements: L_ii = (A_ii - \sum_{k=0}^{i-1} L_ik^2 )^(1/2)
    ScalarType sum = m_v[i];
    for (int32_t k = i - 1; k >= 0; --k) sum -= m_v[k] * m_v[k];

    // Test if matrix is positive definite
    if (sum <= static_cast<ScalarType>(0)) return SolverCholeskyStatus::NotPosDef;

    // Set the computed row in L
    m_v[i] = sqrt(sum);
    m_L.setRowVector(i, 0, i + 1, &m_v[0]);
  }

  return SolverCholeskyStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType SolverCholesky<ScalarType>::determinant()
{
  // Avoid overflowing the fpu by keeping track of the logarithm
  // of the diagonal elements
  uint32_t n = m_L.numRows();
  m_L.getDiagVector(0, &m_p[0]);
  ScalarType sum = static_cast<ScalarType>(0);
  for (uint32_t i = 0; i < n; ++i) sum += log(m_p[i]);
  return exp(sum);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& SolverCholesky<ScalarType>::getL() const
{
  return m_L;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& SolverCholesky<ScalarType>::getL()
{
  return m_L;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& SolverCholesky<ScalarType>::getX() const
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& SolverCholesky<ScalarType>::getX()
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SolverCholesky<ScalarType>::getX(Vector<ScalarType>& x) const
{
  m_X.getColumnVector(0, x);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverCholeskyStatus::eType SolverCholesky<ScalarType>::getStatus() const
{
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool SolverCholesky<ScalarType>::fail() const
{
  return m_status != SolverCholeskyStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
