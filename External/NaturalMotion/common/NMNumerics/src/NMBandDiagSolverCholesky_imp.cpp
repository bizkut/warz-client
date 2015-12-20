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
// BandDiagSolverCholesky
//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverCholesky<ScalarType>::~BandDiagSolverCholesky()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverCholesky<ScalarType>::BandDiagSolverCholesky(
  const BandDiagMatrix<ScalarType>& A, const Matrix<ScalarType>& B) :
  m_L(A.numRows(), A.numColumns(), A.numSuper(), 0), // Lower triangle band
  m_X(B.numRows(), B.numColumns()),
  m_p(A.numRows()),
  m_u(1 + A.numSuper()),
  m_v(1 + A.numSuper())
{
  NMP_ASSERT(A.numRows() == A.numColumns());
  NMP_ASSERT(B.numRows() == A.numColumns());
  if (decompose(A) == BandDiagSolverCholeskyStatus::Success)
  {
    solve(B);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverCholesky<ScalarType>::BandDiagSolverCholesky(
  const BandDiagMatrix<ScalarType>& A, const Vector<ScalarType>& b) :
  m_L(A.numRows(), A.numColumns(), A.numSuper(), 0), // Lower triangle band
  m_X(b.numElements(), 1),
  m_p(A.numRows()),
  m_u(1 + A.numSuper()),
  m_v(1 + A.numSuper())
{
  NMP_ASSERT(A.numRows() == A.numColumns());
  NMP_ASSERT(b.numElements() == A.numColumns());
  if (decompose(A) == BandDiagSolverCholeskyStatus::Success)
  {
    m_p = b;
    solve(m_p);
    m_X.setColumnVector(0, m_p);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverCholesky<ScalarType>::BandDiagSolverCholesky(uint32_t An, uint32_t sub, uint32_t Bc) :
  m_L(An, An, sub, 0), // Lower triangle band
  m_X(An, Bc),
  m_p(An),
  m_u(1 + sub),
  m_v(1 + sub)
{
  NMP_ASSERT(An > 1 && Bc > 0);
  NMP_ASSERT(sub > 0);
  m_status = BandDiagSolverCholeskyStatus::BadParams;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverCholesky<ScalarType>::BandDiagSolverCholesky(const BandDiagMatrix<ScalarType>& A) :
  m_L(A.numRows(), A.numColumns(), A.numSuper(), 0), // Lower triangle band
  m_X(A.numRows(), A.numRows()),
  m_p(A.numRows()),
  m_u(1 + A.numSuper()),
  m_v(1 + A.numSuper())
{
  NMP_ASSERT(A.numRows() == A.numColumns());
  invert(A);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverCholesky<ScalarType>::BandDiagSolverCholesky(uint32_t An, uint32_t sub) :
  m_L(An, An, sub, 0), // Lower triangle band
  m_X(An, An),
  m_p(An),
  m_u(1 + sub),
  m_v(1 + sub)
{
  NMP_ASSERT(An > 0);
  m_status = BandDiagSolverCholeskyStatus::BadParams;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverCholeskyStatus::eType BandDiagSolverCholesky<ScalarType>::decompose(const BandDiagMatrix<ScalarType>& A)
{
  setLower(A); // Set the lower triangle of L with the upper triangle of A
  m_status = doDecomposition();
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverCholeskyStatus::eType BandDiagSolverCholesky<ScalarType>::invert(const BandDiagMatrix<ScalarType>& A)
{
  // First decompose A into L L^T form
  if (decompose(A) != BandDiagSolverCholeskyStatus::Success) return m_status;

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
BandDiagSolverCholeskyStatus::eType BandDiagSolverCholesky<ScalarType>::solve(
  const BandDiagMatrix<ScalarType>& A,
  const Matrix<ScalarType>&         B)
{
  if (decompose(A) == BandDiagSolverCholeskyStatus::Success)
  {
    solve(B);
  }
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverCholeskyStatus::eType BandDiagSolverCholesky<ScalarType>::solve(
  const BandDiagMatrix<ScalarType>& A,
  const Vector<ScalarType>&         b)
{
  if (decompose(A) == BandDiagSolverCholeskyStatus::Success)
  {
    m_p = b;
    solve(m_p);
    m_X.setColumnVector(0, m_p);
  }
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagSolverCholesky<ScalarType>::solve(const Matrix<ScalarType>& B)
{
  NMP_ASSERT(m_X.isSize(B.numRows(), B.numColumns()));
  NMP_ASSERT(m_status == BandDiagSolverCholeskyStatus::Success);

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
void BandDiagSolverCholesky<ScalarType>::solve(Vector<ScalarType>& b)
{
  NMP_ASSERT(b.numElements() == m_L.numRows());
  NMP_ASSERT(m_status == BandDiagSolverCholeskyStatus::Success);

  uint32_t n = m_L.numRows();
  uint32_t sub = m_L.numSub();
  uint32_t w = sub + 1;
  uint32_t indx = n - w;

  // Forward substitution step: L y = b
  for (uint32_t i = 0; i < n; ++i)
  {
    // Get the current column of L
    if (i > indx) w--;
    m_L.getColumnVector(i, i, w, &m_v[0]);

    // Eliminate the current diagonal element
    ScalarType y = b[i] / m_v[0];
    b[i] = y; // Set y inplace back in b

    // Substitute remaining column elements into the right hand side
    for (uint32_t k = 1; k < w; ++k) b[i+k] -= y * m_v[k];
  }

  // Back substitution step: L^T x = y
  w = sub;
  for (int32_t i = n - 1; i >= 0; --i)
  {
    // Get the row column of L
    if (i < (int32_t)sub) w--;
    m_L.getRowVector(i, i - w, w + 1, &m_v[0]);

    // Eliminate the current diagonal element
    ScalarType x = b[i] / m_v[w];
    b[i] = x; // Set x inplace back in b

    // Substitute remaining column elements into the right hand side
    for (uint32_t k = 1; k <= w; ++k) b[i-k] -= x * m_v[w-k];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagSolverCholesky<ScalarType>::setLower(const BandDiagMatrix<ScalarType>& A)
{
  NMP_ASSERT(m_L.isSize(A.numRows(), A.numColumns()));
  NMP_ASSERT(m_L.numSub() == A.numSuper());

  uint32_t n = A.numSuper();
  for (uint32_t i = 0; i <= n; ++i)
  {
    A.getDiagVector(i, &m_p[0]);
    m_L.setDiagVector(-(int32_t)i, &m_p[0]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Main Band Diagonal Cholesky decomposition routine that does not allocate memory
template <>
BandDiagSolverCholeskyStatus::eType BandDiagSolverCholesky<ScalarType>::doDecomposition()
{
  uint32_t n = m_L.numRows();
  uint32_t sub = m_L.numSub();

  //
  // Iterate down the first sub rows of the lower triangle L
  //
  for (uint32_t i = 0; i < sub; ++i)
  {
    // Get the i^th row of L
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
    if (sum <= static_cast<ScalarType>(0)) return BandDiagSolverCholeskyStatus::NotPosDef;

    // Set the computed row in L
    m_v[i] = sqrt(sum);
    m_L.setRowVector(i, 0, i + 1, &m_v[0]);
  }

  //
  // Iterate down the remaining rows of the lower triangle L
  //
  for (uint32_t i = sub; i < n; ++i)
  {
    // Get the i^th row of L. This starts off containing the elements of
    // the lower triangle of A which are replaced by the elements of L.
    uint32_t ims = i - sub;
    m_L.getRowVector(i, ims, sub + 1, &m_v[0]);

    // Iterate across the columns of the lower triangle L
    for (uint32_t j = 0; j < sub; ++j)
    {
      // Get the j^th row of L whose elements have previously been computed
      m_L.getRowVector(j + ims, ims, j + 1, &m_p[0]);

      // Compute the sum: A_{i,j} - \sum_{k=0}^{j-1} L_{i,k} * L_{j,k}
      ScalarType sum = m_v[j];
      for (int32_t k = j - 1; k >= 0; --k) sum -= m_v[k] * m_p[k];

      // L_ij = (A_ij - \sum_{k=0}^{j-1} L_ik * L_jk ) / L_jj
      m_v[j] = sum / m_p[j];
    }

    // Diagonal elements: L_ii = (A_ii - \sum_{k=0}^{i-1} L_ik^2 )^(1/2)
    ScalarType sum = m_v[sub];
    for (int32_t k = sub - 1; k >= 0; --k) sum -= m_v[k] * m_v[k];

    // Test if matrix is positive definite
    if (sum <= static_cast<ScalarType>(0)) return BandDiagSolverCholeskyStatus::NotPosDef;

    // Set the computed row in L
    m_v[sub] = sqrt(sum);
    m_L.setRowVector(i, ims, sub + 1, &m_v[0]);
  }

  return BandDiagSolverCholeskyStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType BandDiagSolverCholesky<ScalarType>::determinant()
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
const BandDiagMatrix<ScalarType>& BandDiagSolverCholesky<ScalarType>::getL() const
{
  return m_L;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType>& BandDiagSolverCholesky<ScalarType>::getL()
{
  return m_L;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& BandDiagSolverCholesky<ScalarType>::getX() const
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& BandDiagSolverCholesky<ScalarType>::getX()
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagSolverCholesky<ScalarType>::getX(Vector<ScalarType>& x) const
{
  m_X.getColumnVector(0, x);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagSolverCholeskyStatus::eType BandDiagSolverCholesky<ScalarType>::getStatus() const
{
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool BandDiagSolverCholesky<ScalarType>::fail() const
{
  return m_status != BandDiagSolverCholeskyStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
