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
// Specialises template implementations for the type ScalarType.
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
#define NM_BANDMATRIX_CHECK_DIMENSIONS(_mat_a, _mat_b)                        \
  NMP_ASSERT((_mat_a).m_numRows == (_mat_b).m_numRows);                     \
  NMP_ASSERT((_mat_a).m_numColumns == (_mat_b).m_numColumns);               \
  NMP_ASSERT((_mat_a).m_numSub == (_mat_b).m_numSub);                       \
  NMP_ASSERT((_mat_a).m_numSuper == (_mat_b).m_numSuper)

//----------------------------------------------------------------------------------------------------------------------
// BandDiagMatrix
//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType>::~BandDiagMatrix()
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType>::BandDiagMatrix() :
  m_numRows(0), m_numColumns(0),
  m_numSub(0), m_numSuper(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType>::BandDiagMatrix(uint32_t rows, uint32_t cols, uint32_t sub, uint32_t super) :
  m_data(NMP::minimum(rows, cols), sub + super + 1),
  m_numRows(rows), m_numColumns(cols),
  m_numSub(sub), m_numSuper(super)
{
  if (!m_data.isEmpty())
  {
    NMP_ASSERT(m_numSub < m_numRows);
    NMP_ASSERT(m_numSuper < m_numColumns);
    m_data.set(static_cast<ScalarType>(0));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType>::BandDiagMatrix(const Matrix<ScalarType>& A, uint32_t sub, uint32_t super) :
  m_data(NMP::minimum(A.numRows(), A.numColumns()), sub + super + 1),
  m_numRows(A.numRows()), m_numColumns(A.numColumns()),
  m_numSub(sub), m_numSuper(super)
{
  if (!m_data.isEmpty())
  {
    NMP_ASSERT(m_numSub < m_numRows);
    NMP_ASSERT(m_numSuper < m_numColumns);
    m_data.set(static_cast<ScalarType>(0));
    set(A);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType>::BandDiagMatrix(const BandDiagMatrix& copy) :
  m_data(copy.m_data),
  m_numRows(copy.m_numRows), m_numColumns(copy.m_numColumns),
  m_numSub(copy.m_numSub), m_numSuper(copy.m_numSuper)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const BandDiagMatrix<ScalarType>& BandDiagMatrix<ScalarType>::operator = (const BandDiagMatrix<ScalarType>& copy)
{
  NM_BANDMATRIX_CHECK_DIMENSIONS(*this, copy);
  if (&copy == this) return *this;
  m_data = copy.m_data;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> BandDiagMatrix<ScalarType>::operator * (const Matrix<ScalarType>& v) const
{
  Matrix<ScalarType> result(m_numRows, v.numColumns());
  multiply(result, v);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType> BandDiagMatrix<ScalarType>::operator * (const BandDiagMatrix& v) const
{
  uint32_t sub = m_numSub + v.m_numSub;
  uint32_t super = m_numSuper + v.m_numSuper;
  BandDiagMatrix<ScalarType> result(m_numRows, v.m_numColumns, sub, super);
  result.setFromMultiplication(*this, v);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> BandDiagMatrix<ScalarType>::operator * (const Vector<ScalarType> v) const
{
  NMP_ASSERT(v.numElements() == m_numColumns);
  Vector<ScalarType> result(m_numRows);
  multiply(&result[0], &v[0]);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType> BandDiagMatrix<ScalarType>::operator + (const BandDiagMatrix& v) const
{
  BandDiagMatrix A(*this);
  A += v;
  return A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType> BandDiagMatrix<ScalarType>::operator - (const BandDiagMatrix& v) const
{
  BandDiagMatrix A(*this);
  A -= v;
  return A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType> BandDiagMatrix<ScalarType>::operator * (ScalarType s) const
{
  BandDiagMatrix A(*this);
  A *= s;
  return A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType> BandDiagMatrix<ScalarType>::operator / (ScalarType s) const
{
  BandDiagMatrix A(*this);
  A /= s;
  return A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::operator += (const BandDiagMatrix<ScalarType>& v)
{
  NMP_ASSERT(m_numRows == v.m_numRows && m_numColumns == v.m_numColumns);
  NMP_ASSERT(m_numSub >= v.m_numSub && m_numSuper >= v.m_numSuper);

  uint32_t r1, c1, r2, c2, s;
  for (int32_t i = -(int32_t)v.m_numSub; i <= (int32_t)v.m_numSuper; ++i)
  {
    getDiag(i, r1, c1, s);
    v.getDiag(i, r2, c2, s);
    const ScalarType* src = &v.m_data.element(r2, c2);
    ScalarType* dst = &m_data.element(r1, c1);
    for (uint32_t j = 0; j < s; ++j) dst[j] += src[j];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::operator -= (const BandDiagMatrix<ScalarType>& v)
{
  NMP_ASSERT(m_numRows == v.m_numRows && m_numColumns == v.m_numColumns);
  NMP_ASSERT(m_numSub >= v.m_numSub && m_numSuper >= v.m_numSuper);

  uint32_t r1, c1, r2, c2, s;
  for (int32_t i = -(int32_t)v.m_numSub; i <= (int32_t)v.m_numSuper; ++i)
  {
    getDiag(i, r1, c1, s);
    v.getDiag(i, r2, c2, s);
    const ScalarType* src = &v.m_data.element(r2, c2);
    ScalarType* dst = &m_data.element(r1, c1);
    for (uint32_t j = 0; j < s; ++j) dst[j] -= src[j];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::operator += (ScalarType v)
{
  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    ScalarType* dst = &m_data.element(r, c);
    for (uint32_t j = 0; j < s; ++j) dst[j] += v;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::operator -= (ScalarType v)
{
  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    ScalarType* dst = &m_data.element(r, c);
    for (uint32_t j = 0; j < s; ++j) dst[j] -= v;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::operator *= (ScalarType v)
{
  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    ScalarType* dst = &m_data.element(r, c);
    for (uint32_t j = 0; j < s; ++j) dst[j] *= v;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::operator /= (ScalarType v)
{
  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    ScalarType* dst = &m_data.element(r, c);
    for (uint32_t j = 0; j < s; ++j) dst[j] /= v;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setFromMultiplication(int32_t i, const BandDiagMatrix& A, const BandDiagMatrix& B)
{
  // Get the row, column and size parameters of the corresponding
  // diagonal within the storage matrix
  uint32_t rs, cs, ss;
  getDiag(i, rs, cs, ss);
  ScalarType* pM = &m_data.element(rs, cs);

  // Get the corresponding row and column indices of the diagonal within
  // the actual matrix
  uint32_t Mr, Mc;
  getDiagIndex(i, Mr, Mc);

  // Iterate down the current diagonal
  for (uint32_t k = 0; k < ss; ++k, ++Mr, ++Mc)
  {
    // Get the row, column and increment offset parameters corresponding
    // to row Mr within the storage matrix A.m_data
    uint32_t Acm, Asm;
    int32_t incA = A.getRow(Mr, rs, cs, Acm, Asm);
    if (incA == 0)
    {
      pM[k] = static_cast<ScalarType>(0);
      continue;
    }
    const ScalarType* pA = &A.m_data.element(rs, cs);

    // Get the row, column and increment offset parameters corresponding
    // to column Mc within the storage matrix B.m_data
    uint32_t Brm, Bsm;
    int32_t incB = B.getColumn(Mc, rs, cs, Brm, Bsm);
    if (incB == 0)
    {
      pM[k] = static_cast<ScalarType>(0);
      continue;
    }
    const ScalarType* pB = &B.m_data.element(rs, cs);

    // Clip to the first valid start segment
    if (Acm > Brm)
    {
      uint32_t d = Acm - Brm;
      pB = &pB[incB * (int32_t)d];
      Bsm -= d;
    }
    else if (Acm < Brm)
    {
      uint32_t d = Brm - Acm;
      pA = &pA[incA * (int32_t)d];
      Acm = Brm;
      Asm -= d;
    }

    // Clip to the minimum segment size
    if (Asm > Bsm) Asm = Bsm;

    // Compute the data summation loop
    pM[k] = static_cast<ScalarType>(0);
    for (uint32_t kk = 0; kk < Asm; ++kk)
    {
      pM[k] += (*pA) * (*pB);
      pA = &pA[incA];
      pB = &pB[incB];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setFromMultiplication(const BandDiagMatrix& A, const BandDiagMatrix& B)
{
  NMP_ASSERT(m_numRows == A.m_numRows && m_numColumns == B.m_numColumns);
  NMP_ASSERT(A.m_numColumns == B.m_numRows);
  NMP_ASSERT(m_numSub == A.m_numSub + B.m_numSub);
  NMP_ASSERT(m_numSuper == A.m_numSuper + B.m_numSuper);

  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    setFromMultiplication(i, A, B);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setFromMultiplicationSym(const BandDiagMatrix& A, const BandDiagMatrix& B)
{
  NMP_ASSERT(m_numRows == A.m_numRows && m_numColumns == m_numRows);
  NMP_ASSERT(A.m_numColumns == B.m_numRows);
  NMP_ASSERT(m_numSuper == A.m_numSuper + B.m_numSuper);
  NMP_ASSERT(m_numSub == m_numSuper);

  for (int32_t i = 0; i <= (int32_t)m_numSuper; ++i)
  {
    setFromMultiplication(i, A, B);
  }
  makeSymmetric();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::multiplyDiag(const ScalarType* v)
{
  for (uint32_t i = 0; i < m_numRows; ++i) multiplyRow(i, v[i]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::multiply(Matrix<ScalarType>& v, const Matrix<ScalarType>& u) const
{
  NMP_ASSERT(u.numRows() == m_numColumns);
  NMP_ASSERT(v.numRows() == m_numRows && v.numColumns() == u.numColumns());
  for (uint32_t i = 0; i < u.numColumns(); ++i)
  {
    multiply(&v.element(0, i), &u.element(0, i));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::multiply(ScalarType* v, const ScalarType* u) const
{
  const ScalarType* ptr;
  uint32_t rs, cs, cm, sm;
  int32_t inc;

  for (uint32_t j = 0; j < m_numRows; ++j)
  {
    // Get the row, column and increment offset parameters corresponding
    // to row r within the storage matrix m_data
    inc = getRow(j, rs, cs, cm, sm);

    // Check for zero row size
    if (sm == 0)
    {
      v[j] = static_cast<ScalarType>(0);
      continue;
    }
    else
    {
      // loop unroll with first multiplication
      ptr = &m_data.element(rs, cs);
      v[j] = (*ptr) * u[cm];
    }

    for (uint32_t k = 1; k < sm; ++k)
    {
      ptr = &ptr[inc]; // update storage data pointer
      v[j] += ((*ptr) * u[cm+k]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::postMultiply(Matrix<ScalarType>& v, const Matrix<ScalarType>& u) const
{
  NMP_ASSERT(u.numColumns() == m_numRows);
  NMP_ASSERT(v.numRows() == u.numRows() && v.numColumns() == m_numColumns);

  Vector<ScalarType> p(u.numColumns());
  Vector<ScalarType> q(m_numColumns);

  for (uint32_t i = 0; i < u.numRows(); ++i)
  {
    u.getRowVector(i, &p[0]);
    postMultiply(&q[0], &p[0]);
    v.setRowVector(i, q);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::postMultiply(ScalarType* v, const ScalarType* u) const
{
  const ScalarType* ptr;
  uint32_t rs, cs, rm, sm;
  int32_t inc;

  for (uint32_t j = 0; j < m_numColumns; ++j)
  {
    // Get the row, column and increment offset parameters corresponding
    // to column r within the storage matrix m_data
    inc = getColumn(j, rs, cs, rm, sm);

    // Check for zero column size
    if (sm == 0)
    {
      v[j] = static_cast<ScalarType>(0);
      continue;
    }
    else
    {
      // loop unroll with first multiplication
      ptr = &m_data.element(rs, cs);
      v[j] = (*ptr) * u[rm];
    }

    for (uint32_t k = 1; k < sm; ++k)
    {
      ptr = &ptr[inc]; // update storage data pointer
      v[j] += ((*ptr) * u[rm+k]);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::greater(const BandDiagMatrix& v)
{
  NM_BANDMATRIX_CHECK_DIMENSIONS(*this, v);
  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    const ScalarType* src = &v.m_data.element(r, c);
    ScalarType* dst = &m_data.element(r, c);
    for (uint32_t j = 0; j < s; ++j)
    {
      if (dst[j] > src[j]) dst[j] = static_cast<ScalarType>(1);
      else dst[j] = static_cast<ScalarType>(0);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::greater(ScalarType v)
{
  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    ScalarType* dst = &m_data.element(r, c);
    for (uint32_t j = 0; j < s; ++j)
    {
      if (dst[j] > v) dst[j] = static_cast<ScalarType>(1);
      else dst[j] = static_cast<ScalarType>(0);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::less(const BandDiagMatrix& v)
{
  NM_BANDMATRIX_CHECK_DIMENSIONS(*this, v);
  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    const ScalarType* src = &v.m_data.element(r, c);
    ScalarType* dst = &m_data.element(r, c);
    for (uint32_t j = 0; j < s; ++j)
    {
      if (dst[j] < src[j]) dst[j] = static_cast<ScalarType>(1);
      else dst[j] = static_cast<ScalarType>(0);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::less(ScalarType v)
{
  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    ScalarType* dst = &m_data.element(r, c);
    for (uint32_t j = 0; j < s; ++j)
    {
      if (dst[j] < v) dst[j] = static_cast<ScalarType>(1);
      else dst[j] = static_cast<ScalarType>(0);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::absolute()
{
  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    ScalarType* ptr = &m_data.element(r, c);
    for (uint32_t j = 0; j < s; ++j) ptr[j] = abs(ptr[j]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::multiply(const BandDiagMatrix& v)
{
  NMP_ASSERT(m_numRows == v.m_numRows && m_numColumns == v.m_numColumns);

  // Find intersecting region
  uint32_t sub = m_numSub;
  if (v.m_numSub < m_numSub) sub = v.m_numSub;

  uint32_t super = m_numSuper;
  if (v.m_numSuper < m_numSuper) sub = v.m_numSuper;

  // Element-wise multiply
  uint32_t r1, c1, r2, c2, s;
  for (int32_t i = -(int32_t)sub; i <= (int32_t)super; ++i)
  {
    getDiag(i, r1, c1, s);
    v.getDiag(i, r2, c2, s);
    const ScalarType* src = &v.m_data.element(r2, c2);
    ScalarType* dst = &m_data.element(r1, c1);
    for (uint32_t j = 0; j < s; ++j) dst[j] *= src[j];
  }

  // Zero diagonals outside of intersecting sub region
  for (uint32_t i = sub + 1; i <= m_numSub; ++i)
    setDiagVector(-(int32_t)i, static_cast<ScalarType>(0));

  // Zero diagonals outside of intersecting super region
  for (uint32_t i = super + 1; i <= m_numSuper; ++i)
    setDiagVector(i, static_cast<ScalarType>(0));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType BandDiagMatrix<ScalarType>::element(uint32_t rowIndex, uint32_t colIndex) const
{
  NMP_ASSERT(rowIndex < m_numRows);
  NMP_ASSERT(colIndex < m_numColumns);
  uint32_t r, c, s;
  int32_t i = (int32_t)colIndex - (int32_t)rowIndex;
  getDiag(i, r, c, s);
  if (s == 0) return static_cast<ScalarType>(0);

  if (i >= 0) return m_data.element(r + rowIndex, c);
  return m_data.element(r + colIndex, c);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::element(uint32_t rowIndex, uint32_t colIndex, ScalarType val)
{
  NMP_ASSERT(rowIndex < m_numRows);
  NMP_ASSERT(colIndex < m_numColumns);
  uint32_t r, c, s;
  int32_t i = (int32_t)colIndex - (int32_t)rowIndex;
  getDiag(i, r, c, s);

  NMP_ASSERT(s > 0);
  if (s == 0) return;

  if (i >= 0) m_data.element(r + rowIndex, c) = val;
  else m_data.element(r + colIndex, c) = val;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool BandDiagMatrix<ScalarType>::operator == (const BandDiagMatrix& m) const
{
  if (m.m_numRows != m_numRows || m.m_numColumns != m_numColumns) return false;
  if (m.m_numSub != m_numSub || m.m_numSuper != m_numSuper) return false;
  return m.m_data == m_data;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool BandDiagMatrix<ScalarType>::operator != (const BandDiagMatrix& m) const
{
  return !((*this) == m);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BandDiagMatrix<ScalarType>::numRows() const
{
  return m_numRows;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BandDiagMatrix<ScalarType>::numColumns() const
{
  return m_numColumns;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool BandDiagMatrix<ScalarType>::isSize(uint32_t r, uint32_t c) const
{
  return m_numRows == r && m_numColumns == c;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool BandDiagMatrix<ScalarType>::isEmpty() const
{
  return m_data.isEmpty();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BandDiagMatrix<ScalarType>::numSub() const
{
  return m_numSub;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BandDiagMatrix<ScalarType>::numSuper() const
{
  return m_numSuper;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BandDiagMatrix<ScalarType>::bandWidth() const
{
  return m_data.numColumns();
}

//----------------------------------------------------------------------------------------------------------------------
// Should we allow this
template <>
const Matrix<ScalarType>& BandDiagMatrix<ScalarType>::data() const
{
  return m_data;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType BandDiagMatrix<ScalarType>::trace() const
{
  NMP_ASSERT(m_numRows == m_numColumns);
  if (m_numRows == 0) return static_cast<ScalarType>(0);

  const ScalarType* ptr = &m_data.element(0, m_numSub);
  ScalarType sum = ptr[0];
  for (uint32_t i = 1; i < m_numRows; ++i) sum += ptr[i];
  return sum;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> BandDiagMatrix<ScalarType>::full() const
{
  Matrix<ScalarType> A(m_numRows, m_numColumns);
  get(A);
  return A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::get(Matrix<ScalarType>& A) const
{
  NMP_ASSERT(A.numRows() == m_numRows && A.numColumns() == m_numColumns);
  A.set(static_cast<ScalarType>(0));

  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    A.setDiagVector(i, &m_data.element(r, c));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::set(const Matrix<ScalarType>& A)
{
  NMP_ASSERT(A.numRows() == m_numRows && A.numColumns() == m_numColumns);

  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    A.getDiagVector(i, &m_data.element(r, c));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::swap(BandDiagMatrix<ScalarType>& A)
{
  uint32_t temp;
  temp = m_numRows; m_numRows = A.m_numRows; A.m_numRows = temp;
  temp = m_numColumns; m_numColumns = A.m_numColumns; A.m_numColumns = temp;
  temp = m_numSub; m_numSub = A.m_numSub; A.m_numSub = temp;
  temp = m_numSuper; m_numSuper = A.m_numSuper; A.m_numSuper = temp;

  m_data.swap(A.m_data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::set(ScalarType val)
{
  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    m_data.setColumnVector(c, r, s, val);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setIdentity()
{
  m_data.set(static_cast<ScalarType>(0));
  m_data.setColumnVector(m_numSub, static_cast<ScalarType>(1));
}

//----------------------------------------------------------------------------------------------------------------------
template <>
int32_t BandDiagMatrix<ScalarType>::getRow(uint32_t i, uint32_t& rs, uint32_t& cs, uint32_t& cm, uint32_t& sm) const
{
  NMP_ASSERT(i < m_numRows);

  // Set defaults
  rs = cs = cm = sm = 0;

  // Compute the x intercept at y for the sub limit
  uint32_t x[2];
  if (i > m_numSub)
  {
    x[0] = i - m_numSub;
    if (x[0] >= m_numColumns) return 0;
  }
  else
  {
    x[0] = 0;
    cs = m_numSub - i;
  }

  // Compute the x intercept at y for the super limit
  x[1] = i + m_numSuper;
  if (x[1] >= m_numColumns) x[1] = m_numColumns - 1;

  // Set the cm and sm parameters
  cm = x[0];
  sm = x[1] - x[0] + 1;

  // Compute the corresponding row index within the storage data
  if (m_numRows >= m_numColumns)
  {
    rs = x[0];
    return (int32_t)m_data.numRows() + 1; // down + right
  }
  else
  {
    rs = i;
    return (int32_t)m_data.numRows(); // right
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
int32_t BandDiagMatrix<ScalarType>::getColumn(uint32_t i, uint32_t& rs, uint32_t& cs, uint32_t& rm, uint32_t& sm) const
{
  NMP_ASSERT(i < m_numColumns);

  // Set defaults
  rs = cs = rm = sm = 0;

  // Compute the y intercept at x for the super limit
  uint32_t y[2];
  if (i > m_numSuper)
  {
    y[0] = i - m_numSuper;
    if (y[0] >= m_numRows) return 0;
    cs = m_numSuper + m_numSub;
  }
  else
  {
    y[0] = 0;
    cs = m_numSub + i;
  }

  // Compute the y intercept at x for the sub limit
  y[1] = i + m_numSub;
  if (y[1] >= m_numRows) y[1] = m_numRows - 1;

  // Set the rm and sm parameters
  rm = y[0];
  sm = y[1] - y[0] + 1;

  // Compute the corresponding row index within the storage data
  if (m_numRows >= m_numColumns)
  {
    rs = i;
    return -(int32_t)m_data.numRows(); // left
  }
  else
  {
    rs = y[0];
    return -(int32_t)m_data.numRows() + 1; // down + left
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> BandDiagMatrix<ScalarType>::getRowVector(uint32_t i) const
{
  Vector<ScalarType> v(m_numColumns);
  getRowVector(i, 0, m_numColumns, &v[0]);
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::getRowVector(uint32_t i, ScalarType* data) const
{
  getRowVector(i, 0, m_numColumns, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::getRowVector(uint32_t r, uint32_t c, uint32_t cn, ScalarType* data) const
{
  NMP_ASSERT(data);
  if (cn == 0) return;

  // Get the row, column and increment offset parameters corresponding
  // to row r within the storage matrix m_data
  uint32_t rs, cs, cm, sm;
  int32_t inc = getRow(r, rs, cs, cm, sm);

  // Get a pointer to the storage data
  const ScalarType* ptr = &m_data.element(rs, cs);
  uint32_t i = 0; // output vector index

  // extents of data vectors
  uint32_t cext = c + cn - 1;
  uint32_t csext = cm + sm - 1;

  // start column less than the band column
  if (c < cm)
  {
    // end column also less than the band column
    if (cext < cm)
    {
      for (; i < cn; ++i) data[i] = static_cast<ScalarType>(0);
      return;
    }
    // Fill in any leading zeros in the output vector
    for (uint32_t j = c; j < cm; ++j, ++i) data[i] = static_cast<ScalarType>(0);
    c = cm; // clip c
  }
  else if (c > cm) // start column greater than the band column
  {
    // start column also greater than the band end column
    if (c > csext)
    {
      for (; i < cn; ++i) data[i] = static_cast<ScalarType>(0);
      return;
    }
    // Skip any elements in the storage data
    ptr = &ptr[inc * (int32_t)(c-cm)];
  }

  // end column less than the band end column
  if (cext < csext)
  {
    for (uint32_t j = c; j <= cext; ++j, ++i)
    {
      data[i] = *ptr;
      ptr = &ptr[inc];
    }
  }
  else
  {
    for (uint32_t j = c; j <= csext; ++j, ++i)
    {
      data[i] = *ptr;
      ptr = &ptr[inc];
    }
    for (; i < cn; ++i) data[i] = static_cast<ScalarType>(0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> BandDiagMatrix<ScalarType>::getColumnVector(uint32_t i) const
{
  Vector<ScalarType> v(m_numRows);
  getColumnVector(i, 0, m_numRows, &v[0]);
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::getColumnVector(uint32_t i, ScalarType* data) const
{
  getColumnVector(i, 0, m_numRows, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::getColumnVector(uint32_t c, uint32_t r, uint32_t rn, ScalarType* data) const
{
  NMP_ASSERT(data);
  if (rn == 0) return;

  // Get the row, column and increment offset parameters corresponding
  // to column c within the storage matrix m_data
  uint32_t rs, cs, rm, sm;
  int32_t inc = getColumn(c, rs, cs, rm, sm);

  // Get a pointer to the storage data
  const ScalarType* ptr = &m_data.element(rs, cs);
  uint32_t i = 0; // output vector index

  // extents of data vectors
  uint32_t rext = r + rn - 1;
  uint32_t rsext = rm + sm - 1;

  // start row less than the band row
  if (r < rm)
  {
    // end row also less than the band row
    if (rext < rm)
    {
      for (; i < rn; ++i) data[i] = static_cast<ScalarType>(0);
      return;
    }
    // Fill in any leading zeros in the output vector
    for (uint32_t j = r; j < rm; ++j, ++i) data[i] = static_cast<ScalarType>(0);
    r = rm; // clip r
  }
  else if (r > rm) // start row greater than the band row
  {
    // start row also greater than the band end row
    if (r > rsext)
    {
      for (; i < rn; ++i) data[i] = static_cast<ScalarType>(0);
      return;
    }
    // Skip any elements in the storage data
    ptr = &ptr[inc * (int32_t)(r-rm)];
  }

  // end row less than the band end row
  if (rext < rsext)
  {
    for (uint32_t j = r; j <= rext; ++j, ++i)
    {
      data[i] = *ptr;
      ptr = &ptr[inc];
    }
  }
  else
  {
    for (uint32_t j = r; j <= rsext; ++j, ++i)
    {
      data[i] = *ptr;
      ptr = &ptr[inc];
    }
    for (; i < rn; ++i) data[i] = static_cast<ScalarType>(0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setRowVector(uint32_t i, ScalarType val)
{
  // Get the row, column and increment offset parameters corresponding
  // to row i within the storage matrix m_data
  uint32_t rs, cs, cm, sm;
  int32_t inc = getRow(i, rs, cs, cm, sm);

  // Get a pointer to the storage data and set the values
  ScalarType* ptr = &m_data.element(rs, cs);
  for (uint32_t j = 0; j < sm; ++j)
  {
    *ptr = val;
    ptr = &ptr[inc];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setRowVector(uint32_t i, const Vector<ScalarType>& v)
{
  NMP_ASSERT(v.numElements() == m_numColumns);
  setRowVector(i, 0, m_numColumns, &v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setRowVector(uint32_t i, const ScalarType* data)
{
  setRowVector(i, 0, m_numColumns, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setRowVector(uint32_t r, uint32_t c, uint32_t cn, const ScalarType* data)
{
  NMP_ASSERT(c + cn <= m_numColumns);
  NMP_ASSERT(data);
  if (cn == 0) return;

  // Get the row, column and increment offset parameters corresponding
  // to row r within the storage matrix m_data
  uint32_t rs, cs, cm, sm;
  int32_t inc = getRow(r, rs, cs, cm, sm);

  // Get a pointer to the storage data
  ScalarType* ptr = &m_data.element(rs, cs);
  uint32_t i = 0; // input vector index

  // extents of data vectors
  uint32_t cext = c + cn - 1;
  uint32_t csext = cm + sm - 1;

  // start column less than the band column
  if (c < cm)
  {
    // end column also less than the band column
    if (cext < cm) return;
    i = cm - c; c = cm; // Clip
  }
  else if (c > cm) // start column greater than the band column
  {
    // start column also greater than the band end column
    if (c > csext) return;
    ptr = &ptr[inc * (int32_t)(c-cm)]; // Skip any elements in storage
  }

  // end column less than the band end column
  uint32_t ext;
  if (cext < csext) ext = cext; else ext = csext;
  for (uint32_t j = c; j <= ext; ++j, ++i)
  {
    *ptr = data[i];
    ptr = &ptr[inc];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setRowVector(uint32_t r, uint32_t c, uint32_t cn, ScalarType val)
{
  NMP_ASSERT(c + cn <= m_numColumns);
  if (cn == 0) return;

  // Get the row, column and increment offset parameters corresponding
  // to row r within the storage matrix m_data
  uint32_t rs, cs, cm, sm;
  int32_t inc = getRow(r, rs, cs, cm, sm);

  // Get a pointer to the storage data
  ScalarType* ptr = &m_data.element(rs, cs);

  // extents of data vectors
  uint32_t cext = c + cn - 1;
  uint32_t csext = cm + sm - 1;

  // start column less than the band column
  if (c < cm)
  {
    // end column also less than the band column
    if (cext < cm) return;
    c = cm; // Clip
  }
  else if (c > cm) // start column greater than the band column
  {
    // start column also greater than the band end column
    if (c > csext) return;
    ptr = &ptr[inc * (int32_t)(c-cm)]; // Skip any elements in storage
  }

  // end column less than the band end column
  uint32_t ext;
  if (cext < csext) ext = cext; else ext = csext;
  for (uint32_t j = c; j <= ext; ++j)
  {
    *ptr = val;
    ptr = &ptr[inc];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setColumnVector(uint32_t i, ScalarType val)
{
  // Get the row, column and increment offset parameters corresponding
  // to column i within the storage matrix m_data
  uint32_t rs, cs, rm, sm;
  int32_t inc = getColumn(i, rs, cs, rm, sm);

  // Get a pointer to the storage data and set the values
  ScalarType* ptr = &m_data.element(rs, cs);
  for (uint32_t j = 0; j < sm; ++j)
  {
    *ptr = val;
    ptr = &ptr[inc];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setColumnVector(uint32_t i, const Vector<ScalarType>& v)
{
  NMP_ASSERT(v.numElements() == m_numRows);
  setColumnVector(i, 0, m_numRows, &v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setColumnVector(uint32_t i, const ScalarType* data)
{
  setColumnVector(i, 0, m_numRows, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setColumnVector(uint32_t c, uint32_t r, uint32_t rn, const ScalarType* data)
{
  NMP_ASSERT(r + rn <= m_numRows);
  NMP_ASSERT(data);
  if (rn == 0) return;

  // Get the row, column and increment offset parameters corresponding
  // to column c within the storage matrix m_data
  uint32_t rs, cs, rm, sm;
  int32_t inc = getColumn(c, rs, cs, rm, sm);

  // Get a pointer to the storage data
  ScalarType* ptr = &m_data.element(rs, cs);
  uint32_t i = 0; // input vector index

  // extents of data vectors
  uint32_t rext = r + rn - 1;
  uint32_t rsext = rm + sm - 1;

  // start row less than the band row
  if (r < rm)
  {
    // end row also less than the band row
    if (rext < rm) return;
    i = rm - r; r = rm; // Clip
  }
  else if (r > rm) // start row greater than the band row
  {
    // start row also greater than the band end row
    if (r > rsext) return;
    ptr = &ptr[inc * (int32_t)(r-rm)]; // Skip any elements in storage
  }

  // end row less than the band end row
  uint32_t ext;
  if (rext < rsext) ext = rext; else ext = rsext;
  for (uint32_t j = r; j <= ext; ++j, ++i)
  {
    *ptr = data[i];
    ptr = &ptr[inc];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setColumnVector(uint32_t c, uint32_t r, uint32_t rn, ScalarType val)
{
  NMP_ASSERT(r + rn <= m_numRows);
  if (rn == 0) return;

  // Get the row, column and increment offset parameters corresponding
  // to column c within the storage matrix m_data
  uint32_t rs, cs, rm, sm;
  int32_t inc = getColumn(c, rs, cs, rm, sm);

  // Get a pointer to the storage data
  ScalarType* ptr = &m_data.element(rs, cs);

  // extents of data vectors
  uint32_t rext = r + rn - 1;
  uint32_t rsext = rm + sm - 1;

  // start row less than the band row
  if (r < rm)
  {
    // end row also less than the band row
    if (rext < rm) return;
    r = rm; // Clip
  }
  else if (r > rm) // start row greater than the band row
  {
    // start row also greater than the band end row
    if (r > rsext) return;
    ptr = &ptr[inc * (int32_t)(r-rm)]; // Skip any elements in storage
  }

  // end row less than the band end row
  uint32_t ext;
  if (rext < rsext) ext = rext; else ext = rsext;
  for (uint32_t j = r; j <= ext; ++j)
  {
    *ptr = val;
    ptr = &ptr[inc];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::multiplyRow(uint32_t i, ScalarType s)
{
  // Get the row, column and increment offset parameters corresponding
  // to row i within the storage matrix m_data
  uint32_t rs, cs, cm, sm;
  int32_t inc = getRow(i, rs, cs, cm, sm);

  // Get a pointer to the storage data and set the values
  ScalarType* ptr = &m_data.element(rs, cs);
  for (uint32_t j = 0; j < sm; ++j)
  {
    (*ptr) *= s;
    ptr = &ptr[inc];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::multiplyColumn(uint32_t i, ScalarType s)
{
  // Get the row, column and increment offset parameters corresponding
  // to column i within the storage matrix m_data
  uint32_t rs, cs, rm, sm;
  int32_t inc = getColumn(i, rs, cs, rm, sm);

  // Get a pointer to the storage data and multiply the values
  ScalarType* ptr = &m_data.element(rs, cs);
  for (uint32_t j = 0; j < sm; ++j)
  {
    (*ptr) *= s;
    ptr = &ptr[inc];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::getDiag(int32_t i, uint32_t& r, uint32_t& c, uint32_t& s) const
{
  uint32_t absi;
  r = c = s = 0;

  // if r >= c then the super diagonals are stored in the lower columns of m_data,
  // and the sub diagonals are stored in the upper columns of m_data.
  if (m_numRows >= m_numColumns)
  {
    if (i > 0)
    {
      absi = (uint32_t)i;
      if (absi > m_numSuper) return; // Check within matrix
      s = m_numColumns - absi;
      r = absi;
      c = m_numSub + absi;
    }
    else
    {
      absi = (uint32_t) - i;
      if (absi > m_numSub) return; // Check within matrix
      if (absi <= (m_numRows - m_numColumns))
        s = m_numColumns; // Check within mid bound
      else
        s = m_numRows - absi;
      c = m_numSub - absi;
    }
  }
  else
  {
    // if r < c then the super diagonals are stored in the upper columns of m_data,
    // and the sub diagonals are stored in the lower columns of m_data.
    if (i < 0)
    {
      absi = (uint32_t) - i;
      if (absi > m_numSub) return; // Check within matrix
      s = m_numRows - absi;
      c = m_numSub - absi;
      r = absi;
    }
    else
    {
      absi = (uint32_t)i;
      if (absi > m_numSuper) return; // Check within matrix
      if (absi <= (m_numColumns - m_numRows))
        s = m_numRows; // Check within mid bound
      else
        s = m_numColumns - absi;
      c = m_numSub + absi;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t BandDiagMatrix<ScalarType>::getDiagLength(int32_t i) const
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  return s;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
int32_t BandDiagMatrix<ScalarType>::getDiagIndex(uint32_t r, uint32_t c) const
{
  return (int32_t)c - (int32_t)r;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::getDiagIndex(int32_t i, uint32_t& r, uint32_t& c) const
{
  if (i >= 0)
  {
    r = 0; c = (uint32_t)i;
  }
  else
  {
    r = (uint32_t) - i;
    c = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const ScalarType& BandDiagMatrix<ScalarType>::getDiagElement(int32_t i, uint32_t k) const
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  NMP_ASSERT(s > 0);
  return m_data.element(r + k, c);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType& BandDiagMatrix<ScalarType>::getDiagElement(int32_t i, uint32_t k)
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  NMP_ASSERT(s > 0);
  return m_data.element(r + k, c);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::getDiagVector(int32_t i, ScalarType* data) const
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  m_data.getColumnVector(c, r, s, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::getDiagVector(uint32_t rowIndex, uint32_t colIndex, uint32_t dn, ScalarType* data) const
{
  NMP_ASSERT(rowIndex < m_numRows);
  NMP_ASSERT(colIndex < m_numColumns);
  if (dn == 0) return;

  uint32_t r, c, s;
  int32_t i = (int32_t)colIndex - (int32_t)rowIndex;
  getDiag(i, r, c, s);
  if (s == 0) return;

  if (i >= 0) m_data.getColumnVector(c, r + rowIndex, dn, data);
  else m_data.getColumnVector(c, r + colIndex, dn, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> BandDiagMatrix<ScalarType>::getDiagVector(int32_t i) const
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  NMP_ASSERT(s > 0);

  Vector<ScalarType> v(s);
  m_data.getColumnVector(c, r, s, &v.element(0));
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setDiagVector(int32_t i, ScalarType val)
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  if (s == 0) return;
  m_data.setColumnVector(c, r, s, val);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setDiagVector(int32_t i, const ScalarType* data)
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  m_data.setColumnVector(c, r, s, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setDiagVector(uint32_t rowIndex, uint32_t colIndex, uint32_t dn, const ScalarType* data)
{
  NMP_ASSERT(rowIndex < m_numRows);
  NMP_ASSERT(colIndex < m_numColumns);
  if (dn == 0) return;

  uint32_t r, c, s;
  int32_t i = (int32_t)colIndex - (int32_t)rowIndex;
  getDiag(i, r, c, s);
  if (s == 0) return;

  if (i >= 0) m_data.setColumnVector(c, r + rowIndex, dn, data);
  else m_data.setColumnVector(c, r + colIndex, dn, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::addDiagVector(int32_t i, ScalarType val)
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  if (s == 0) return;
  m_data.addColumn(c, r, s, val);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType BandDiagMatrix<ScalarType>::norm() const
{
  return m_data.norm();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::transpose()
{
  uint32_t ext = m_data.numColumns() - 1;
  uint32_t p = m_data.numColumns() >> 1;

  // First swap the columns (this also reverses the storage format!)
  for (uint32_t i = 0; i < p; ++i) m_data.swapColumns(i, ext - i);

  // If we have a square matrix then we must preserve the storage format
  // by shifting the column elements
  if (m_numRows == m_numColumns)
  {
    for (uint32_t j = 0; j < m_numSuper; ++j)
      m_data.shiftColumnElements(j, (int32_t)j - (int32_t)m_numSuper, static_cast<ScalarType>(0));

    for (uint32_t j = 0; j < m_numSub; ++j)
      m_data.shiftColumnElements(ext - j, (int32_t)(m_numSub - j), static_cast<ScalarType>(0));
  }

  // Swap the sub and super parameters
  uint32_t temp = m_numSub; m_numSub = m_numSuper; m_numSuper = temp;

  // Swap the row and column parameters
  temp = m_numRows; m_numRows = m_numColumns; m_numColumns = temp;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::getTranspose(BandDiagMatrix<ScalarType>& mat) const
{
  NMP_ASSERT(mat.m_numRows == m_numColumns && mat.m_numColumns == m_numRows);
  NMP_ASSERT(mat.m_numSub == m_numSuper && mat.m_numSuper == m_numSub);

  uint32_t ext = m_data.numColumns() - 1;
  uint32_t p = m_data.numColumns() >> 1;

  // First swap the columns (this also reverses the storage format!)
  mat.m_data = m_data;
  for (uint32_t i = 0; i < p; ++i) mat.m_data.swapColumns(i, ext - i);

  // If we have a square matrix then we must preserve the storage format
  // by shifting the column elements
  if (m_numRows == m_numColumns)
  {
    for (uint32_t j = 0; j < m_numSuper; ++j)
      mat.m_data.shiftColumnElements(j, (int32_t)j - (int32_t)m_numSuper, static_cast<ScalarType>(0));

    for (uint32_t j = 0; j < m_numSub; ++j)
      mat.m_data.shiftColumnElements(ext - j, (int32_t)(m_numSuper - j), static_cast<ScalarType>(0));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType> BandDiagMatrix<ScalarType>::getTranspose() const
{
  BandDiagMatrix A(*this);
  A.transpose();
  return A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setTranspose(const BandDiagMatrix<ScalarType>& mat)
{
  BandDiagMatrix<ScalarType> A(mat);
  A.transpose();
  (*this) = A;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::getUpper(BandDiagMatrix<ScalarType>& mat) const
{
  NMP_ASSERT(mat.isSize(m_numRows, m_numColumns));
  NMP_ASSERT(mat.m_numSuper == m_numSuper && mat.m_numSub == 0);

  uint32_t r, c, s;
  for (uint32_t i = 0; i <= m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    mat.setDiagVector(i, &m_data.element(r, c));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::getLower(BandDiagMatrix<ScalarType>& mat) const
{
  NMP_ASSERT(mat.isSize(m_numRows, m_numColumns));
  NMP_ASSERT(mat.m_numSub == m_numSub && mat.m_numSuper == 0);

  uint32_t r, c, s;
  for (uint32_t i = 0; i < m_numSub; ++i)
  {
    getDiag(-(int32_t)i, r, c, s);
    mat.setDiagVector(-(int32_t)i, &m_data.element(r, c));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType> BandDiagMatrix<ScalarType>::getUpper() const
{
  BandDiagMatrix<ScalarType> mat(m_numRows, m_numColumns, 0, m_numSuper);
  getUpper(mat);
  return mat;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
BandDiagMatrix<ScalarType> BandDiagMatrix<ScalarType>::getLower() const
{
  BandDiagMatrix<ScalarType> mat(m_numRows, m_numColumns, m_numSub, 0);
  getLower(mat);
  return mat;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setUpper(const BandDiagMatrix<ScalarType>& mat)
{
  NMP_ASSERT(mat.isSize(m_numRows, m_numColumns));
  NMP_ASSERT(mat.m_numSuper == m_numSuper);

  uint32_t r, c, s;
  for (uint32_t i = 0; i <= m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    mat.getDiagVector(i, &m_data.element(r, c));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setLower(const BandDiagMatrix<ScalarType>& mat)
{
  NMP_ASSERT(mat.isSize(m_numRows, m_numColumns));
  NMP_ASSERT(mat.m_numSub == m_numSub);

  uint32_t r, c, s;
  for (uint32_t i = 0; i < m_numSub; ++i)
  {
    getDiag(-(int32_t)i, r, c, s);
    mat.getDiagVector(-(int32_t)i, &m_data.element(r, c));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setUpper(ScalarType s)
{
  for (uint32_t i = 0; i <= m_numSuper; ++i) setDiagVector(i, s);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::setLower(ScalarType s)
{
  for (uint32_t i = 0; i <= m_numSub; ++i) setDiagVector(-(int32_t)i, s);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void BandDiagMatrix<ScalarType>::makeSymmetric()
{
  NMP_ASSERT(m_numSub == m_numSuper);
  NMP_ASSERT(m_numRows == m_numColumns);

  for (uint32_t i = 1; i <= m_numSuper; ++i)
  {
    uint32_t r, c, s;
    getDiag(i, r, c, s);
    const ScalarType* src = &m_data.element(r, c);

    getDiag(-(int32_t)i, r, c, s);
    ScalarType* dst = &m_data.element(r, c);

    for (uint32_t k = 0; k < s; ++k) dst[k] = src[k];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <> bool
BandDiagMatrix<ScalarType>::isZero(ScalarType threshold) const
{
  uint32_t r, c, s;
  for (int32_t i = -(int32_t)m_numSub; i <= (int32_t)m_numSuper; ++i)
  {
    getDiag(i, r, c, s);
    const ScalarType* ptr = &m_data.element(r, c);
    for (uint32_t j = 0; j < s; ++j)
    {
      if (abs(ptr[j]) > threshold)
        return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool BandDiagMatrix<ScalarType>::isEqual(const BandDiagMatrix<ScalarType>& mat, ScalarType threshold) const
{
  if (!isSize(mat.numRows(), mat.numColumns()))
    return false;

  BandDiagMatrix<ScalarType> D(*this);
  D -= mat;
  return D.isZero(threshold);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
std::ostream& operator<<(std::ostream& os, const BandDiagMatrix<ScalarType>& mat)
{
  int32_t sub = (int32_t)mat.numSub();
  int32_t super = (int32_t)mat.numSuper();
  os << "dim : " << mat.numRows() << " " << mat.numColumns() << " " << std::endl;
  os << "sub : " << sub << ", super : " << super << std::endl;

  const NMP::Matrix<ScalarType>& D = mat.data();
  os << std::endl << "Data:" << std::endl;
  os << D << std::endl;

  //////////////////////////////////////////////////////////
  // Debug stuff
  os << std::endl << "Diags:" << std::endl;
  sub = -sub;
  uint32_t n = mat.getDiagLength(0);
  NMP::Vector<ScalarType> v(n);

  for (int32_t i = sub; i <= super; ++i)
  {
    uint32_t s = mat.getDiagLength(i);
    mat.getDiagVector(i, &v.element(0));
    for (uint32_t k = 0; k < s; ++k) os << v.element(k) << " ";
    os << std::endl;
  }

  os << std::endl << "Matrix.full:" << std::endl;
  NMP::Matrix<ScalarType> A = mat.full();
  os << A << std::endl;
  //////////////////////////////////////////////////////////

  return os;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
