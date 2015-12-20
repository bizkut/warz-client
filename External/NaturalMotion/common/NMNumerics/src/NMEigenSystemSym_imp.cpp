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
// EigenSystemSym
//----------------------------------------------------------------------------------------------------------------------
template <>
EigenSystemSym<ScalarType>::~EigenSystemSym()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
EigenSystemSym<ScalarType>::EigenSystemSym(const Matrix<ScalarType>& A, bool wantV) :
  m_House(A.numRows(), A.numRows()),
  m_Givens(A.numRows()), m_V(A.numRows(), A.numRows())
{
  NMP_ASSERT(A.isSquare());
  m_status = EigenSystemStatus::BadParams;
  setMaxIts(30);
  setRTol(NMEIGSYM_RTOL);
  compute(A, wantV);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
EigenSystemSym<ScalarType>::EigenSystemSym(uint32_t n) :
  m_House(n, n), m_Givens(n), m_V(n, n)
{
  m_status = EigenSystemStatus::BadParams;
  setMaxIts(30);
  setRTol(NMEIGSYM_RTOL);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
EigenSystemStatus::eType EigenSystemSym<ScalarType>::compute(const Matrix<ScalarType>& A, bool wantV)
{
  // Set the Householder matrix
  Matrix<ScalarType>& M = m_House.getA();
  M = A;

  m_status = EigenSystemStatus::Success;

  // Reduction of matrix M to tri-diagonal form
  m_House.transformSym(1);

  // Set the Givens tri-diagonal matrix
  m_Givens.setT(M);

  // Diagonalize
  Matrix<ScalarType>* pV = 0;
  if (wantV)
  {
    m_House.right(m_V, 1);
    pV = &m_V;
  }

  if (!m_Givens.diagonalizeT(m_maxIts, pV))
  {
    m_status = EigenSystemStatus::MaxIts;
  }

  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void EigenSystemSym<ScalarType>::solve(Vector<ScalarType>& x, const Vector<ScalarType>& b)
{
  // Solve: V * D * V^T x = b
  uint32_t n = m_V.numColumns();
  NMP_ASSERT(b.numElements() == n);
  NMP_ASSERT(x.numElements() == n);
  NMP_ASSERT(n > 0);

  // Get a temporary vector for v
  Vector<ScalarType>& u = m_House.getV(); // n - vector
  Vector<ScalarType>& y = m_House.getX(); // n - vector

  // u = V^T * b
  m_V.preMultiplyTranspose(n, u.data(), b.data());

  // Solve: D y = u
  Vector<ScalarType>& D = m_Givens.getD();
  for (uint32_t i = 0; i < n; ++i)
  {
    if (D[i] < m_rTol)
    {
      y[i] = 0;
    }
    else
    {
      y[i] = u[i] / D[i];
    }
  }

  // Compute x = V * y
  m_V.preMultiply(x.data(), y.data());
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void EigenSystemSym<ScalarType>::pseudoInverse(Matrix<ScalarType>& M)
{
  uint32_t n = m_V.numColumns();
  NMP_ASSERT(M.isSize(n, n));

  // Get a temporary vector for v
  Vector<ScalarType>& u = m_House.getV(); // n - vector
  Vector<ScalarType>& v = m_House.getX(); // n - vector

  // A^+ = V * D^+ * V^T (outer product update version)
  M.set(static_cast<ScalarType>(0));
  Vector<ScalarType>& D = m_Givens.getD();
  for (uint32_t i = 0; i < n; ++i)
  {
    if (D[i] < m_rTol) continue;
    m_V.getColumnVector(i, u);
    m_V.getColumnVector(i, v);
    v /= D[i];
    M.outerUpdate(&v[0], &u[0]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> EigenSystemSym<ScalarType>::pseudoInverse()
{
  Matrix<ScalarType> M(m_V.numRows(), m_V.numColumns());
  pseudoInverse(M);
  return M;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t EigenSystemSym<ScalarType>::rank() const
{
  const Vector<ScalarType>& D = m_Givens.getD();
  uint32_t n = D.numElements();

  // Iterate over the eigen values (note these are unsorted)
  uint32_t r = n;
  for (uint32_t i = 0; i < n; ++i)
  {
    if (D[i] < m_rTol) --r;
  }
  return r;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType EigenSystemSym<ScalarType>::norm() const
{
  const Vector<ScalarType>& D = m_Givens.getD();
  uint32_t n = D.numElements();
  NMP_ASSERT(n > 0);

  // Iterate over the eigen values (note these are unsorted)
  // For a real symmetric matrix A, all Eigen-values should be positive
  ScalarType maxD = D[0];
  NMP_ASSERT(D[0] >= 0);
  for (uint32_t i = 1; i < n; ++i)
  {
    NMP_ASSERT(D[i] >= 0);
    if (D[i] > maxD) maxD = D[i];
  }
  return sqrt(maxD);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType EigenSystemSym<ScalarType>::cond() const
{
  const Vector<ScalarType>& D = m_Givens.getD();
  uint32_t n = D.numElements();
  NMP_ASSERT(n > 0);

  // Iterate over the eigen values (note these are unsorted)
  // For a real symmetric matrix A, all Eigen-values should be positive
  ScalarType minD = D[0];
  ScalarType maxD = D[0];
  NMP_ASSERT(D[0] >= 0);
  for (uint32_t i = 1; i < n; ++i)
  {
    NMP_ASSERT(D[i] >= 0);
    if (D[i] < minD)
    {
      minD = D[i];
    }
    else if (D[i] > maxD)
    {
      maxD = D[i];
    }
  }
  return sqrt(maxD) / sqrt(minD);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void EigenSystemSym<ScalarType>::sort()
{
  Vector<ScalarType>& D = m_Givens.getD();
  uint32_t n = D.numElements();

  // Get a temporary matrix for V
  Matrix<ScalarType>& A = m_House.getA();
  A = m_V;

  // Get a temporary vector for D and V
  Vector<ScalarType>& v = m_House.getV(); // n - vector
  Vector<ScalarType>& d = m_House.getX(); // n - vector
  d = D;

  // Set the initial sort index order
  // For a real symmetric matrix A, all Eigen-values should be positive
  Vector<int32_t> indx(n);
  for (uint32_t i = 0; i < n; ++i)
  {
    NMP_ASSERT(D[i] >= -1e-7f);
    indx[i] = (int32_t)i;
  }

  // Exchange sort
  for (uint32_t i = 0; i < n - 1; ++i)
  {
    for (uint32_t j = i + 1; j < n; ++j)
    {
      int32_t t = indx[j];
      if (D[t] > D[ indx[i] ])
      {
        indx[j] = indx[i];
        indx[i] = t;
      }
    }
  }

  // Exchange the values in D and V
  for (uint32_t i = 0; i < n; ++i)
  {
    uint32_t k = indx[i];
    if (k != i)
    {
      // D
      D[i] = d[k];

      // V
      A.getColumnVector(k, v);
      m_V.setColumnVector(i, v);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<ScalarType>& EigenSystemSym<ScalarType>::getD() const
{
  return m_Givens.getD();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& EigenSystemSym<ScalarType>::getD()
{
  return m_Givens.getD();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& EigenSystemSym<ScalarType>::getV() const
{
  return m_V;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& EigenSystemSym<ScalarType>::getV()
{
  return m_V;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType EigenSystemSym<ScalarType>::getETol() const
{
  return m_Givens.getETol();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void EigenSystemSym<ScalarType>::setETol(ScalarType tol)
{
  m_Givens.setETol(tol);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType EigenSystemSym<ScalarType>::getRTol() const
{
  return m_rTol;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void EigenSystemSym<ScalarType>::setRTol(ScalarType tol)
{
  m_rTol = tol;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t EigenSystemSym<ScalarType>::getMaxIts() const
{
  return m_maxIts;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void EigenSystemSym<ScalarType>::setMaxIts(uint32_t its)
{
  m_maxIts = its;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
EigenSystemStatus::eType EigenSystemSym<ScalarType>::getStatus() const
{
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool EigenSystemSym<ScalarType>::fail() const
{
  return m_status != EigenSystemStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
