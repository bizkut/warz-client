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
// Bidiagonalizer
//----------------------------------------------------------------------------------------------------------------------
template <>
Bidiagonalizer<ScalarType>::~Bidiagonalizer()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Bidiagonalizer<ScalarType>::Bidiagonalizer(const Matrix<ScalarType>& A) :
  m_H(A.numRows(), A.numColumns()),
  m_Hr(A.numColumns(), A.numColumns()),
  m_Q(A.numColumns(), A.numColumns()),
  m_method(R_BiDiag)
{
  NMP_ASSERT(A.numRows() >= A.numColumns());

  // Decompose matrix A to bidiagonal form
  compute(A);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Bidiagonalizer<ScalarType>::Bidiagonalizer(uint32_t m, uint32_t n) :
  m_H(m, n), m_Hr(n, n), m_Q(n, n), m_method(R_BiDiag)
{
  NMP_ASSERT(m >= n);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Bidiagonalizer<ScalarType>::compute(const Matrix<ScalarType>& A)
{
  // Size information
  uint32_t m = A.numRows();
  uint32_t n = A.numColumns();
  NMP_ASSERT(m >= n);

  // Set the Householder manipulation matrix
  Matrix<ScalarType>& M = m_H.getA();
  M = A;

  // Determine the best reduction method to use: R_BiDiag is more
  // efficient whenever m >= 5n/3
  if (3 * m >= 5 * n)
  {
    m_method = R_BiDiag;

    // Reduction to upper triangular form
    m_H.transformLeft(0);

    // Bi-diagonalization
    Matrix<ScalarType>& Mr = m_Hr.getA();
    M.getUpper(Mr);

    for (uint32_t i = 0; i < n - 2; ++i)
    {
      m_Hr.transformRight(i, 1);
      m_Hr.transformLeft(i + 1, 0);
    }
  }
  else
  {

    // Perform sequential reduction on both sides
    m_method = BiDiag;
    uint32_t k;
    for (k = 0; k < n - 2; ++k)
    {
      m_H.transformLeft(k, 0);
      m_H.transformRight(k, 1);
    }

    // Perform the reduction on the remaining left sides
    for (; k < n; ++k) m_H.transformLeft(k, 0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Bidiagonalizer<ScalarType>::getB(Matrix<ScalarType>& B)
{
  if (m_method == R_BiDiag)
  {
    const Matrix<ScalarType>& A = m_Hr.getA();
    A.getBandDiag(B, 0, 1);
  }
  else
  {
    const Matrix<ScalarType>& A = m_H.getA();
    A.getBandDiag(B, 0, 1);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Bidiagonalizer<ScalarType>::getB()
{
  const Matrix<ScalarType>& A = m_H.getA();
  Matrix<ScalarType> B(A.numRows(), A.numColumns());
  getB(B);
  return B;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Bidiagonalizer<ScalarType>::getU(Matrix<ScalarType>& U)
{
  // Compute the left orthogonal transform
  m_H.left(U, 0);

  if (m_method == R_BiDiag)
  {
    // Number of columns in U (could be thin - m by n!)
    const Matrix<ScalarType>& A = m_H.getA();
    uint32_t m = A.numRows();
    uint32_t n = A.numColumns();
    NMP_ASSERT(U.numColumns() >= n); // Dead zone check!

    // Compute the left orthogonal transform for Hr
    m_Hr.left(m_Q, 0);

    // Get some workspace vectors
    Vector<ScalarType>& u = m_H.getX();
    Vector<ScalarType>& x = m_Hr.getX();
    Vector<ScalarType>& v = m_Hr.getV();

    // Update U: U' = U * diag(Ur, I_{m-n})
    uint32_t nm1 = n - 1;
    for (uint32_t j = 0; j < m; ++j)
    {
      // Note that Ur = diag(I_{1}, Z_{n-1})
      U.getRowVector(j, 1, nm1, &x[0]);
      for (uint32_t i = 1; i < n; ++i)
      {
        m_Q.getColumnVector(i, 1, nm1, &v[0]);
        ScalarType dot = x[0] * v[0];
        for (uint32_t k = 1; k < nm1; ++k) dot += x[k] * v[k];
        u[i] = dot;
      }
      U.setRowVector(j, 1, nm1, &u[1]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Bidiagonalizer<ScalarType>::getU()
{
  const Matrix<ScalarType>& A = m_H.getA();
  Matrix<ScalarType> U(A.numRows(), A.numRows());
  getU(U);
  return U;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Bidiagonalizer<ScalarType>::getV(Matrix<ScalarType>& V)
{
  if (m_method == R_BiDiag)
  {
    m_Hr.right(V, 1);
  }
  else
  {
    m_H.right(V, 1);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Bidiagonalizer<ScalarType>::getV()
{
  const Matrix<ScalarType>& A = m_H.getA();
  Matrix<ScalarType> V(A.numColumns(), A.numColumns());
  getV(V);
  return V;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& Bidiagonalizer<ScalarType>::getQ() const
{
  return m_Q;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& Bidiagonalizer<ScalarType>::getQ()
{
  return m_Q;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Householder<ScalarType>& Bidiagonalizer<ScalarType>::getTransformH()
{
  return m_H;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Householder<ScalarType>& Bidiagonalizer<ScalarType>::getTransformHr()
{
  return m_Hr;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
