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
// SVD
//----------------------------------------------------------------------------------------------------------------------
template <>
SVD<ScalarType>::~SVD()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SVD<ScalarType>::SVD(const Matrix<ScalarType>& A, bool wantU, bool wantV) :
  m_Bidiag(A.numRows(), A.numColumns()),
  m_Givens(A.numColumns()),
  m_U(A.numRows(), A.numColumns()),
  m_V(A.numColumns(), A.numColumns())
{
  m_status = SVDStatus::BadParams;
  setRTol(NMSVD_RTOL);
  setMaxIts(30);
  compute(A, wantU, wantV);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SVD<ScalarType>::SVD(uint32_t m, uint32_t n) :
  m_Bidiag(m, n), m_Givens(n), m_U(m, n), m_V(n, n)
{
  m_status = SVDStatus::BadParams;
  setRTol(NMSVD_RTOL);
  setMaxIts(30);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SVDStatus::eType SVD<ScalarType>::compute(const Matrix<ScalarType>& A, bool wantU, bool wantV)
{
  // Reduction to bi-diagonal form
  m_Bidiag.compute(A);

  // Get the temporary matrix for Ud computation
  Matrix<ScalarType>& Ud = m_Bidiag.getQ();
  Matrix<ScalarType>* pUd = 0;
  Matrix<ScalarType>* pV = 0;

  m_status = SVDStatus::Success;

  // Update U
  if (wantU)
  {
    m_Bidiag.getU(m_U); // thin
    Ud.setIdentity();
    pUd = &Ud;
  }

  // Update V
  if (wantV)
  {
    m_Bidiag.getV(m_V);
    pV = &m_V;
  }

  // Set the Givens processor
  m_Givens.setT(m_Bidiag.getB());

  // Diagonalize
  if (!m_Givens.diagonalizeB(m_maxIts, pUd, pV))
  {
    m_status = SVDStatus::MaxIts;
  }

  // Perform final U' = U * Ud multiplication
  if (wantU)
  {
    // Get a temporary matrix for the U * Ud multiplication
    Matrix<ScalarType> Ut = m_Bidiag.getB();
    Ut.swap(m_U);
    m_U.setFromMultiplication(Ut, Ud);
  }

  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t SVD<ScalarType>::rank() const
{
  const Vector<ScalarType>& D = m_Givens.getD();
  uint32_t n = D.numElements();

  // Singular values are ordered
  uint32_t i;
  for (i = 0; i < n; ++i)
  {
    if (D[i] < m_rTol) break;
  }

  return i;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType SVD<ScalarType>::norm() const
{
  const Vector<ScalarType>& D = m_Givens.getD();
  return D[0];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType SVD<ScalarType>::cond() const
{
  const Vector<ScalarType>& D = m_Givens.getD();
  uint32_t n = D.numElements();
  NMP_ASSERT(n > 0);
  return D[0] / D[n-1];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType SVD<ScalarType>::rcond() const
{
  const Vector<ScalarType>& D = m_Givens.getD();
  uint32_t n = D.numElements();
  NMP_ASSERT(n > 0);
  return D[n-1] / D[0];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::zeroAbsolute(ScalarType tol)
{
  Vector<ScalarType>& D = m_Givens.getD();
  uint32_t n = D.numElements();

  // Singular values are ordered
  uint32_t i;
  for (i = 0; i < n; ++i)
  {
    if (D[i] < tol) break;
  }

  for (; i < n; ++i) D[i] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::zeroRelative(ScalarType tol)
{
  Vector<ScalarType>& D = m_Givens.getD();
  uint32_t n = D.numElements();
  NMP_ASSERT(n > 0);

  // Update tolerance
  tol = tol * D[0];

  // Singular values are ordered
  uint32_t i;
  for (i = 0; i < n; ++i)
  {
    if (D[i] < tol) break;
  }

  for (; i < n; ++i) D[i] = 0;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::zeroAbsolute()
{
  zeroAbsolute(m_rTol);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::nullVector(Vector<ScalarType>& v) const
{
  const Vector<ScalarType>& D = m_Givens.getD();
  uint32_t n = D.numElements();
  NMP_ASSERT(n > 0);

  m_V.getColumnVector(n - 1, v);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> SVD<ScalarType>::nullVector() const
{
  const Vector<ScalarType>& D = m_Givens.getD();
  uint32_t n = D.numElements();
  NMP_ASSERT(n > 0);

  Vector<ScalarType> v(n);
  m_V.getColumnVector(n - 1, v);

  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::
recompose(Matrix<ScalarType>& A)
{
  // Get a temporary matrix for the D * V^T computation
  Matrix<ScalarType>& DVt = m_Bidiag.getQ();
  m_V.getTranspose(DVt);

  // D * V^T
  Vector<ScalarType>& D = m_Givens.getD();
  DVt.multiplyDiag(&D[0]);

  // U * D * V^T
  A.setFromMultiplication(m_U, DVt);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> SVD<ScalarType>::recompose()
{
  Matrix<ScalarType> M(m_U.numRows(), m_U.numColumns());
  recompose(M);
  return M;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::recomposeUVt(Matrix<ScalarType>& A)
{
  // Get a temporary matrix for the D * V^T computation
  Matrix<ScalarType>& Vt = m_Bidiag.getQ();
  m_V.getTranspose(Vt);

  // U * V^T
  A.setFromMultiplication(m_U, Vt);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> SVD<ScalarType>::recomposeUVt()
{
  Matrix<ScalarType> M(m_U.numRows(), m_U.numColumns());
  recomposeUVt(M);
  return M;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::solve(Matrix<ScalarType>& X, const Matrix<ScalarType>& B)
{
  // Number of RHS equations
  uint32_t c = B.numColumns();

  // Get a temporary vector for x
  Householder<ScalarType>& Hr = m_Bidiag.getTransformHr();
  Vector<ScalarType>& x = Hr.getX(); // n - vector

  // Get a temporary vector for b
  Householder<ScalarType>& H = m_Bidiag.getTransformH();
  Vector<ScalarType>& v = H.getV(); // m - vector

  // Solve by columns
  for (uint32_t i = 0; i < c; ++i)
  {
    B.getColumnVector(i, v);
    solve(x, v);
    X.setColumnVector(i, x);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::solve(Vector<ScalarType>& x, const Vector<ScalarType>& b)
{
  uint32_t m = m_U.numRows();
  uint32_t n = m_U.numColumns();
  NMP_ASSERT(m > 0);

  // Get a temporary vector for U(:, i)
  Householder<ScalarType>& H = m_Bidiag.getTransformH();
  Vector<ScalarType>& u = H.getX(); // m - vector

  // Get a temporary vector for the U^T * b computation
  Householder<ScalarType>& Hr = m_Bidiag.getTransformHr();
  Vector<ScalarType>& y = Hr.getW(); // n - vector

  // y = D^+ * (U^T * b)
  Vector<ScalarType>& D = m_Givens.getD();
  uint32_t i;
  for (i = 0; i < n; ++i)
  {
    // Check if D_ii is zero within rank tolerance
    if (D[i] < m_rTol) break;

    // U^T * b
    m_U.getColumnVector(i, u);

    ScalarType w = u[0] * b[0];
    for (uint32_t k = 1; k < m; ++k) w += u[k] * b[k];
    y[i] = w / D[i];
  }

  // Zero remaining elements of y
  for (; i < n; ++i) y[i] = 0;

  // Compute x = V y
  m_V.preMultiply(&x[0], &y[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::pseudoInverse(Matrix<ScalarType>& M)
{
  uint32_t n = m_U.numColumns();
  NMP_ASSERT(M.isSize(n, m_U.numRows()));

  // Get a temporary vector for v
  Householder<ScalarType>& Hr = m_Bidiag.getTransformHr();
  Vector<ScalarType>& v = Hr.getV(); // n - vector

  // Get a temporary vector for u
  Householder<ScalarType>& H = m_Bidiag.getTransformH();
  Vector<ScalarType>& u = H.getX(); // m - vector

  // A^+ = V * D^+ * U^T (outer product update version)
  M.set(static_cast<ScalarType>(0));
  Vector<ScalarType>& D = m_Givens.getD();
  for (uint32_t i = 0; i < n; ++i)
  {
    if (D[i] < m_rTol) break;
    m_V.getColumnVector(i, v);
    m_U.getColumnVector(i, u);
    v /= D[i];
    M.outerUpdate(&v[0], &u[0]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> SVD<ScalarType>::pseudoInverse()
{
  Matrix<ScalarType> M(m_U.numColumns(), m_U.numRows());
  pseudoInverse(M);
  return M;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& SVD<ScalarType>::getA()
{
  Householder<ScalarType>& H = m_Bidiag.getTransformH();
  return H.getA();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& SVD<ScalarType>::getU() const
{
  return m_U;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& SVD<ScalarType>::getU()
{
  return m_U;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<ScalarType>& SVD<ScalarType>::getD() const
{
  return m_Givens.getD();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& SVD<ScalarType>::getD()
{
  return m_Givens.getD();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& SVD<ScalarType>::getV() const
{
  return m_V;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& SVD<ScalarType>::getV()
{
  return m_V;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> SVD<ScalarType>::getV(uint32_t i) const
{
  uint32_t n = m_V.numColumns();
  Vector<ScalarType> v(n);
  getV(i, v);
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::getV(uint32_t i, Vector<ScalarType>& v) const
{
  m_V.getColumnVector(i, v);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType SVD<ScalarType>::getETol() const
{
  return m_Givens.getETol();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::setETol(ScalarType tol)
{
  m_Givens.setETol(tol);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType SVD<ScalarType>::getRTol() const
{
  return m_rTol;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::setRTol(ScalarType tol)
{
  m_rTol = tol;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t SVD<ScalarType>::getMaxIts() const
{
  return m_maxIts;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void SVD<ScalarType>::setMaxIts(uint32_t its)
{
  m_maxIts = its;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
SVDStatus::eType SVD<ScalarType>::getStatus() const
{
  return m_status;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool SVD<ScalarType>::fail() const
{
  return m_status != SVDStatus::Success;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
