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
// SolverTriDiag
//----------------------------------------------------------------------------------------------------------------------
template <>
SolverTriDiag<ScalarType>::~SolverTriDiag()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverTriDiag<ScalarType>::SolverTriDiag(const BandDiagMatrix<ScalarType>& A, const Matrix<ScalarType>& B) :
  m_A(A), m_B(B), m_X(B.numRows(), B.numColumns()),
  m_v(B.numColumns()), m_p(B.numColumns())
{
  NMP_ASSERT(A.numRows() == A.numColumns());
  NMP_ASSERT(A.numSub() == 1 && A.numSuper() == 1);
  NMP_ASSERT(B.numRows() == A.numColumns());

  // Compute the tri-diagonal linear system
  m_status = doSolve();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverTriDiag<ScalarType>::SolverTriDiag(
  const Vector<ScalarType>& Asub, const Vector<ScalarType>& Adiag,
  const Vector<ScalarType>& Asuper, const Matrix<ScalarType>& B) :
  m_A(Adiag.numElements(), Adiag.numElements(), 1, 1),
  m_B(B), m_X(B.numRows(), B.numColumns()),
  m_v(B.numColumns()), m_p(B.numColumns())
{
  NMP_ASSERT(Asub.numElements() == Adiag.numElements() - 1);
  NMP_ASSERT(Asuper.numElements() == Adiag.numElements() - 1);
  NMP_ASSERT(m_B.numRows() == m_A.numColumns());

  // Set the tri diagonal matrix
  m_A.setDiagVector(-1, &Asub[0]);
  m_A.setDiagVector(0, &Adiag[0]);
  m_A.setDiagVector(1, &Asuper[0]);

  // Compute the tri-diagonal linear system
  m_status = doSolve();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverTriDiag<ScalarType>::SolverTriDiag(uint32_t An, uint32_t Bc) :
  m_A(An, An, 1, 1), m_B(An, Bc), m_X(An, Bc),
  m_v(Bc), m_p(Bc)
{
  // Initialize defaults
  m_status = SolverTriDiagStatus::BadParams;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverTriDiagStatus::eType SolverTriDiag<ScalarType>::solve(
  const BandDiagMatrix<ScalarType>& A, const Matrix<ScalarType>& B)
{
  // Compute the tri-diagonal linear system
  m_A = A;
  m_B = B;
  m_status = doSolve();
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverTriDiagStatus::eType SolverTriDiag<ScalarType>::solve(
  const Vector<ScalarType>& Asub, const Vector<ScalarType>& Adiag,
  const Vector<ScalarType>& Asuper, const Matrix<ScalarType>& B)
{
  // Compute the tri-diagonal linear system
  m_A.setDiagVector(-1, &Asub[0]);
  m_A.setDiagVector(0, &Adiag[0]);
  m_A.setDiagVector(1, &Asuper[0]);
  m_B = B;
  m_status = doSolve();
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
// Perform the tri-diagonal linear solve. Perform LU elimination without
// pivoting on the tri-diagonals. Note in practice most tri-diagonal systems are
// diagonally dominant, thus guaranteeing a stable solution without the need for pivoting.
template <>
SolverTriDiagStatus::eType SolverTriDiag<ScalarType>::doSolve()
{
  ScalarType bi;
  uint32_t An = m_A.numRows();
  uint32_t Bc = m_B.numColumns();
  NMP_ASSERT(An > 1);

  // Get the pointers to the diagonal vectors
  ScalarType* pSub = &m_A.getDiagElement(-1, 0); // [a_1, a_2, ...]
  ScalarType* pDiag = &m_A.getDiagElement(0, 0); // [b_0, b_1, ...]
  ScalarType* pSuper = &m_A.getDiagElement(1, 0); // [c_0, c_1, ...]

  // Check if we can trivially eliminate one of the variables (when b_0 = 0)
  bool flag = false;
  if (pDiag[0] == static_cast<ScalarType>(0))
  {
    // Check for singular matrix (c_0 = 0 or a_1 = 0)
    if (pSuper[0] == static_cast<ScalarType>(0) ||
        pSub[0] == static_cast<ScalarType>(0))
      return SolverTriDiagStatus::Singular;

    // X_1 = B_0 / c_0
    m_B.getRowVector(0, &m_v[0]); // B_0
    for (uint32_t k = 0; k < Bc; ++k) m_p[k] = m_v[k] / pSuper[0];
    m_X.setRowVector(1, m_p); // store X_1

    // Adjust the right hand side with the terms from the left hand side
    // B'_1 = B_1 - b_1*X_1
    m_B.getRowVector(1, &m_v[0]); // B_1
    for (uint32_t k = 0; k < Bc; ++k) m_v[k] -= pDiag[1] * m_p[k];
    m_B.setRowVector(1, m_v); // store B'_1

    // B'_2 = B_2 - a_2*X_1
    m_B.getRowVector(2, &m_v[0]); // B_2
    for (uint32_t k = 0; k < Bc; ++k) m_v[k] -= pSub[1] * m_p[k];
    m_B.setRowVector(2, m_v); // store B'_2

    // Form a set of N-1 equations by substituting the second column into the
    // right hand side then drop the first row (since it contains all zeros)
    pSuper++; // [c_1, c_2, ... ]
    pDiag++; pDiag[0] = pSub[0]; // [a_1, b_2, b_3, ...]
    pSub++; pSub[0] = 0; // [0, a_3, a_4, ...]
    An--; // N-1 equations

    // Drop the first row on the right hand side by shifting elements up by one
    m_B.shiftRows(-1, static_cast<ScalarType>(0));

    // Set the reduced system flag
    flag = true;
  }

  // First solution Y_0 = B_0 / b_0 doesn't need a_i
  m_B.getRowVector(0, &m_v[0]); // B_0
  for (uint32_t k = 0; k < Bc; ++k) m_p[k] = m_v[k] / pDiag[0];
  m_B.setRowVector(0, m_p); // store Y_i inplace back in B
  bi = pDiag[0]; // b_i

  // Forward substitution: L Y = B
  // (L U) X = B  i.e. L (U X) = B such that L Y = B
  for (uint32_t i = 1; i < An; ++i)
  {
    // We don't need to compute the very last c_i, so perform loop unrolling
    // here to compute c_{i-1} = A_{i-1,i} / b_{i-1}
    pSuper[i-1] /= bi;

    // Now compute the new b_i = A_{i,i} - a_i*c_{i-1}
    bi = pDiag[i] - pSub[i-1] * pSuper[i-1];

    // Check for a singular matrix
    if (bi == static_cast<ScalarType>(0)) return SolverTriDiagStatus::Singular;

    // Compute the solution Y_i = ( Bi - a_i*Y_{i-1} ) / b_i
    m_B.getRowVector(i, &m_v[0]); // Note that m_p holds Y_{ i-1 }
    for (uint32_t k = 0; k < Bc; ++k) m_p[k] = (m_v[k] - pSub[i-1] * m_p[k]) / bi;
    m_B.setRowVector(i, m_p); // store Y_i inplace back in B
  }

  // Back substitution: U X = Y
  if (flag)
  {
    // Back substitution for the reduced system
    m_B.getRowVector(An-1, &m_v[0]); // X_{n-1} = Y_{n-1}
    m_X.setRowVector(An, m_v);
    for (int32_t i = An - 2; i >= 1; --i)
    {
      // Compute the solution X_i = Y_i - c_i*X_{i+1}
      m_B.getRowVector(i, &m_p[0]); // Y_i
      for (uint32_t k = 0; k < Bc; ++k) m_v[k] = m_p[k] - pSuper[i] * m_v[k];
      m_X.setRowVector(i + 1, m_v); // store X_i
    }

    // We've already computed X_1 so handle X_0 separately
    m_B.getRowVector(0, &m_p[0]); // Y_0
    for (uint32_t k = 0; k < Bc; ++k) m_v[k] = m_p[k] - pSuper[0] * m_v[k];
    m_X.setRowVector(0, m_v); // store X_0
  }
  else
  {
    // Back substitution for the full system
    m_B.getRowVector(An - 1, &m_v[0]); // X_{n-1} = Y_{n-1}
    m_X.setRowVector(An - 1, m_v);
    for (int32_t i = An - 2; i >= 0; --i)
    {
      // Compute the solution X_i = Y_i - c_i*X_{i+1}
      m_B.getRowVector(i, &m_p[0]); // Y_i
      for (uint32_t k = 0; k < Bc; ++k) m_v[k] = m_p[k] - pSuper[i] * m_v[k];
      m_X.setRowVector(i, m_v); // store X_i
    }
  }

  return SolverTriDiagStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const BandDiagMatrix<ScalarType>& SolverTriDiag<ScalarType>::getA() const
{
  return m_A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType>& SolverTriDiag<ScalarType>::getA()
{
  return m_A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& SolverTriDiag<ScalarType>::getB() const
{
  return m_B;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& SolverTriDiag<ScalarType>::getB()
{
  return m_B;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& SolverTriDiag<ScalarType>::getX() const
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& SolverTriDiag<ScalarType>::getX()
{
  return m_X;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SolverTriDiagStatus::eType SolverTriDiag<ScalarType>::getStatus() const
{
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool SolverTriDiag<ScalarType>::fail() const
{
  return m_status != SolverTriDiagStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
