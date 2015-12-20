// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

// Specialises template implementations for the type ScalarType.
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
#define NM_MATRIX_CHECK_DIMENSIONS(_mat_a, _mat_b)                            \
  NMP_ASSERT((_mat_a).m_numRows == (_mat_b).m_numRows);                     \
  NMP_ASSERT((_mat_a).m_numColumns == (_mat_b).m_numColumns)

#define NM_MATRIX_CHECK_DIMENSIONS_MULTIPLY(_mat_a, _mat_b)                   \
  NMP_ASSERT((_mat_a).m_numColumns == (_mat_b).m_numRows)

#define NM_MATRIX_OPERATE(_dest, _src_a, _src_b, _op)                         \
  uint32_t numElements = m_numRows * m_numColumns;                                  \
  for (uint32_t i = 0 ; i < numElements ; ++i)                                      \
  {                                                                             \
    (_dest).m_elements[i] = (_src_a).m_elements[i] _op (_src_b).m_elements[i]; \
  }

#define NM_MATRIX_OPERATE_SCALAR(_dest, _src_a, _scalar, _op)                 \
  uint32_t numElements = m_numRows * m_numColumns;                                  \
  for (uint32_t i = 0 ; i < numElements ; ++i)                                      \
  {                                                                             \
    (_dest).m_elements[i] = (_src_a).m_elements[i] _op (_scalar);       \
  }

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::set(const ScalarType* data)
{
  NMP_ASSERT(data != NULL);
  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i) m_elements[i] = data[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::set(const Matrix34& src)
{
  // Matrix34 is actually 4x3, with each row stored in order.
  m_elements[0]   = (ScalarType)src.r[0].f[0];
  m_elements[1]   = (ScalarType)src.r[0].f[1];
  m_elements[2]   = (ScalarType)src.r[0].f[2];
  m_elements[3]   = (ScalarType)0.0;
  m_elements[4]   = (ScalarType)src.r[1].f[0];
  m_elements[5]   = (ScalarType)src.r[1].f[1];
  m_elements[6]   = (ScalarType)src.r[1].f[2];
  m_elements[7]   = (ScalarType)0.0;
  m_elements[8]   = (ScalarType)src.r[2].f[0];
  m_elements[9]   = (ScalarType)src.r[2].f[1];
  m_elements[10]  = (ScalarType)src.r[2].f[2];
  m_elements[11]  = (ScalarType)0.0;
  m_elements[12]  = (ScalarType)src.r[3].f[0];
  m_elements[13]  = (ScalarType)src.r[3].f[1];
  m_elements[14]  = (ScalarType)src.r[3].f[2];
  m_elements[15]  = (ScalarType)1.0;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::set(ScalarType val)
{
  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i) m_elements[i] = val;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>::~Matrix()
{
  if (m_elements) delete [] m_elements;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>::Matrix() :
  m_numRows(0), m_numColumns(0), m_elements(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>::Matrix(uint32_t rows, uint32_t cols) :
  m_numRows(rows), m_numColumns(cols), m_elements(NULL)
{
  uint32_t n = m_numRows * m_numColumns;
  if (n > 0) m_elements = new ScalarType [n];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>::Matrix(uint32_t rows, uint32_t cols, ScalarType value) :
  m_numRows(rows), m_numColumns(cols), m_elements(NULL)
{
  uint32_t n = m_numRows * m_numColumns;
  if (n > 0)
  {
    m_elements = new ScalarType [n];
    set(value);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>::Matrix(uint32_t rows, uint32_t cols, const ScalarType* data) :
  m_numRows(rows), m_numColumns(cols), m_elements(NULL)
{
  uint32_t n = m_numRows * m_numColumns;
  if (n > 0)
  {
    m_elements = new ScalarType [n];
    set(data);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>::Matrix(const Matrix& copy) :
  m_numRows(copy.m_numRows),
  m_numColumns(copy.m_numColumns),
  m_elements(NULL)
{
  uint32_t n = m_numRows * m_numColumns;
  if (n > 0)
  {
    m_elements = new ScalarType [n];
    set(copy.m_elements);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Matrix<ScalarType>& Matrix<ScalarType>::operator = (const Matrix& copy)
{
  NM_MATRIX_CHECK_DIMENSIONS(*this, copy);
  if (m_elements) set(copy.m_elements);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>::Matrix(const Matrix34& matrix) :
  m_numRows(4),
  m_numColumns(4)
{
  m_elements = new ScalarType [m_numRows * m_numColumns];

  // Currently, Matrix34 is actually 4x3, with each row stored in order.
  m_elements[0]   = (ScalarType)matrix.r[0].f[0];
  m_elements[1]   = (ScalarType)matrix.r[0].f[1];
  m_elements[2]   = (ScalarType)matrix.r[0].f[2];
  m_elements[3]   = (ScalarType)0.0;
  m_elements[4]   = (ScalarType)matrix.r[1].f[0];
  m_elements[5]   = (ScalarType)matrix.r[1].f[1];
  m_elements[6]   = (ScalarType)matrix.r[1].f[2];
  m_elements[7]   = (ScalarType)0.0;
  m_elements[8]   = (ScalarType)matrix.r[2].f[0];
  m_elements[9]   = (ScalarType)matrix.r[2].f[1];
  m_elements[10]  = (ScalarType)matrix.r[2].f[2];
  m_elements[11]  = (ScalarType)0.0;
  m_elements[12]  = (ScalarType)matrix.r[3].f[0];
  m_elements[13]  = (ScalarType)matrix.r[3].f[1];
  m_elements[14]  = (ScalarType)matrix.r[3].f[2];
  m_elements[15]  = (ScalarType)1.0;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t Matrix<ScalarType>::numRows() const
{
  return m_numRows;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t Matrix<ScalarType>::numColumns() const
{
  return m_numColumns;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Matrix<ScalarType>::isSize(uint32_t r, uint32_t c) const
{
  return m_numRows == r && m_numColumns == c;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType& Matrix<ScalarType>::element(uint32_t rowIndex, uint32_t colIndex)
{
  NMP_ASSERT(rowIndex < m_numRows);
  NMP_ASSERT(colIndex < m_numColumns);
  return m_elements[(colIndex * m_numRows) + rowIndex];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const ScalarType& Matrix<ScalarType>::element(uint32_t rowIndex, uint32_t colIndex) const
{
  NMP_ASSERT(rowIndex < m_numRows);
  NMP_ASSERT(colIndex < m_numColumns);
  return m_elements[(colIndex * m_numRows) + rowIndex];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType& Matrix<ScalarType>::element(uint32_t index)
{
  NMP_ASSERT(index < m_numRows * m_numColumns);
  return m_elements[index];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const ScalarType& Matrix<ScalarType>::element(uint32_t index) const
{
  NMP_ASSERT(index < m_numRows * m_numColumns);
  return m_elements[index];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType* Matrix<ScalarType>::data()
{
  return m_elements;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const ScalarType* Matrix<ScalarType>::data() const
{
  return m_elements;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::makeSymmetric()
{
  NMP_ASSERT(m_numRows == m_numColumns);
  for (uint32_t i = 0; i < m_numRows - 1; ++i)
  {
    ScalarType* pA = &element(i, i);
    ScalarType* pB = pA;
    uint32_t n = m_numRows - i;
    for (uint32_t k = 1; k < n; ++k)
    {
      pA += m_numRows;
      pB[k] = *pA;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& Matrix<ScalarType>::setFromMultiplication(const Matrix<ScalarType>& A, const Matrix<ScalarType>& B)
{
  NM_MATRIX_CHECK_DIMENSIONS_MULTIPLY(A, B);
  NMP_ASSERT(m_numRows == A.m_numRows && m_numColumns == B.m_numColumns);

  for (uint32_t i = 0; i < m_numColumns; ++i)
  {
    ScalarType* dst = &element(0, i);
    const ScalarType* srcB = &B.element(0, i);
    for (uint32_t j = 0; j < m_numRows; ++j)
    {
      const ScalarType* srcA = &A.element(j, 0);
      ScalarType v = (*srcA) * srcB[0];
      for (uint32_t k = 1; k < A.m_numColumns; ++k)
      {
        srcA += A.m_numRows; // Update pointer
        v += (*srcA) * srcB[k];
      }
      dst[j] = v; // Set value
    }
  }

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& Matrix<ScalarType>::setFromMultiplicationAtA(uint32_t n, const Matrix<ScalarType>& A)
{
  NMP_ASSERT(A.m_numColumns == m_numColumns && m_numRows == m_numColumns);
  NMP_ASSERT(n > 0);

  for (uint32_t i = 0; i < m_numColumns; ++i)
  {
    ScalarType* dst = &element(0, i);
    const ScalarType* srcA = &A.element(0, i);
    for (uint32_t j = 0; j <= i; ++j)
    {
      const ScalarType* srcAt = &A.element(0, j);
      ScalarType v = srcAt[0] * srcA[0];
      for (uint32_t k = 1; k < n; ++k)
      {
        v += srcAt[k] * srcA[k];
      }
      dst[j] = v;
    }
  }
  makeSymmetric();
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& Matrix<ScalarType>::setFromMultiplication(ScalarType coeff, const Matrix& A)
{
  NMP_ASSERT(m_numRows == A.m_numRows && m_numColumns == A.m_numColumns);

  for (uint32_t i = 0; i < m_numRows; ++i)
  {
    for (uint32_t j = 0; j < m_numColumns; ++j)
      element(i, j) = coeff * A.element(i, j);
  }

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& Matrix<ScalarType>::setFromDiff(const Matrix<ScalarType>& A, const Matrix<ScalarType>& B)
{
  NMP_ASSERT(numRows() == A.numRows() && numColumns() == A.numColumns());
  NMP_ASSERT(numRows() == B.numRows() && numColumns() == B.numColumns());

  for (uint32_t i = 0; i < numRows(); ++i)
  {
    for (uint32_t j = 0; j < numColumns(); ++j)
      element(i, j) = A.element(i, j) - B.element(i, j);
  }

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType>& Matrix<ScalarType>::setFromAdd(const Matrix<ScalarType>& A, const Matrix<ScalarType>& B)
{
  NMP_ASSERT(numRows() == A.numRows() && numColumns() == A.numColumns());
  NMP_ASSERT(numRows() == B.numRows() && numColumns() == B.numColumns());

  for (uint32_t i = 0; i < numRows(); ++i)
  {
    for (uint32_t j = 0; j < numColumns(); ++j)
      element(i, j) = A.element(i, j) + B.element(i, j);
  }

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::operator + (const Matrix& v) const
{
  NM_MATRIX_CHECK_DIMENSIONS(*this, v);
  Matrix result(m_numRows, m_numColumns);
  NM_MATRIX_OPERATE(result, *this, v, +);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::operator - (const Matrix& v) const
{
  NM_MATRIX_CHECK_DIMENSIONS(*this, v);
  Matrix result(m_numRows, m_numColumns);
  NM_MATRIX_OPERATE(result, *this, v, -);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::operator * (const Matrix& v) const
{
  NM_MATRIX_CHECK_DIMENSIONS_MULTIPLY(*this, v);
  Matrix result(m_numRows, v.m_numColumns);
  for (uint32_t column = 0 ; column < v.m_numColumns ; ++column)
  {
    // find beginning of column
    const ScalarType* srcRowStart = &v.element(0, column);

    for (uint32_t row = 0 ; row < m_numRows ; ++row)
    {
      // result[row, col] = sum_{ i=0 }^{ m_numColumns } (this[row, i] * v[i, col])
      ScalarType sum = 0;
      for (uint32_t i = 0 ; i < m_numColumns ; ++i)
      {
        sum += element(row, i) * srcRowStart[i];
      }
      result.element(row, column) = sum;
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Matrix<ScalarType>::operator * (const Vector<ScalarType> v) const
{
  NMP_ASSERT(v.numElements() == m_numColumns);
  Vector<ScalarType> result(m_numRows);
  for (uint32_t elIdx = 0 ; elIdx < m_numRows ; ++elIdx)
  {
    // result[elIdx] = sum_{ i=0 }^{ m_numColumns } (this[elIdx, i] * v[i])
    ScalarType sum = 0;
    for (uint32_t i = 0 ; i < m_numColumns ; ++i)
    {
      sum += element(elIdx, i) * v[i];
    }
    result[elIdx] = sum;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::operator * (ScalarType s) const
{
  Matrix<ScalarType> result(m_numRows, m_numColumns);
  NM_MATRIX_OPERATE_SCALAR(result, *this, s, *);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::operator / (ScalarType s) const
{
  Matrix<ScalarType> result(m_numRows, m_numColumns);
  NM_MATRIX_OPERATE_SCALAR(result, *this, s, /);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::operator += (const Matrix& v)
{
  NM_MATRIX_CHECK_DIMENSIONS(*this, v);
  NM_MATRIX_OPERATE(*this, *this, v, +);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::operator -= (const Matrix& v)
{
  NM_MATRIX_CHECK_DIMENSIONS(*this, v);
  NM_MATRIX_OPERATE(*this, *this, v, -);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::operator *= (const Matrix& v)
{
  Matrix<ScalarType> A(*this);
  setFromMultiplication(A, v);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::operator += (ScalarType s)
{
  NM_MATRIX_OPERATE_SCALAR(*this, *this, s, +);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::operator -= (ScalarType s)
{
  NM_MATRIX_OPERATE_SCALAR(*this, *this, s, -);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::operator *= (ScalarType s)
{
  NM_MATRIX_OPERATE_SCALAR(*this, *this, s, *);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::operator /= (ScalarType s)
{
  NM_MATRIX_OPERATE_SCALAR(*this, *this, s, /);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Matrix<ScalarType>::operator == (const Matrix& m) const
{
  if (!isSize(m.m_numRows, m.m_numColumns)) return false;

  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0 ; i < numElements ; ++i)
  {
    if (m_elements[i] != m.m_elements[i]) return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Matrix<ScalarType>::operator != (const Matrix& m) const
{
  return !((*this) == m);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::less(const Matrix& v)
{
  NM_MATRIX_CHECK_DIMENSIONS(*this, v);

  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i)
  {
    if (m_elements[i] < v.m_elements[i]) m_elements[i] = static_cast<ScalarType>(1);
    else m_elements[i] = static_cast<ScalarType>(0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::less(ScalarType s)
{
  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i)
  {
    if (m_elements[i] < s) m_elements[i] = static_cast<ScalarType>(1);
    else m_elements[i] = static_cast<ScalarType>(0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::greater(const Matrix& v)
{
  NM_MATRIX_CHECK_DIMENSIONS(*this, v);

  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i)
  {
    if (m_elements[i] > v.m_elements[i]) m_elements[i] = static_cast<ScalarType>(1);
    else m_elements[i] = static_cast<ScalarType>(0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::greater(ScalarType s)
{
  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i)
  {
    if (m_elements[i] > s) m_elements[i] = static_cast<ScalarType>(1);
    else m_elements[i] = static_cast<ScalarType>(0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::clamp(ScalarType valMin, ScalarType valMax)
{
  NMP_ASSERT(valMin <= valMax);
  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i)
  {
    m_elements[i] = NMP::clampValue(m_elements[i], valMin, valMax);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::absolute()
{
  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i)
  {
    if (m_elements[i] < 0)
      m_elements[i] = -m_elements[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::zeroSmall(ScalarType v)
{
  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i)
  {
    ScalarType absElement;
    (m_elements[i] < 0) ? absElement = -m_elements[i] : absElement = m_elements[i];
    if (absElement < v) m_elements[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::multiply(const Matrix& v)
{
  NM_MATRIX_CHECK_DIMENSIONS(*this, v);
  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i) m_elements[i] *= v.m_elements[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::multiplyColumns(const ScalarType* v)
{
  for (uint32_t i = 0; i < m_numColumns; ++i)
  {
    ScalarType* ptr = &element(0, i);
    for (uint32_t k = 0; k < m_numRows; ++k) ptr[k] *= v[k];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::multiplyRows(const ScalarType* v)
{
  for (uint32_t i = 0; i < m_numColumns; ++i)
  {
    ScalarType* ptr = &element(0, i);
    for (uint32_t k = 0; k < m_numRows; ++k) ptr[k] *= v[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t Matrix<ScalarType>::numElements() const
{
  return m_numRows * m_numColumns;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Matrix<ScalarType>::isEmpty() const
{
  return m_elements == NULL;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Matrix<ScalarType>::isSquare() const
{
  return m_numRows == m_numColumns;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Matrix<ScalarType>::trace() const
{
  if (!m_elements) return 0;
  NM_MATRIX_CHECK_DIMENSIONS_MULTIPLY(*this, *this);
  const ScalarType* ptr = &element(0, 0);

  uint32_t inc = m_numRows + 1;
  ScalarType tr = *ptr;
  for (uint32_t i = 1; i < m_numRows; ++i)
  {
    ptr += inc;
    tr += *ptr;
  }
  return tr;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Matrix<ScalarType>::maximum() const
{
  uint32_t n = m_numRows * m_numColumns;
  uint32_t indx = 0;
  for (uint32_t i = 1; i < n; ++i)
  {
    if (m_elements[i] > m_elements[indx]) indx = i;
  }
  return m_elements[indx];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Matrix<ScalarType>::minimum() const
{
  uint32_t n = m_numRows * m_numColumns;
  uint32_t indx = 0;
  for (uint32_t i = 1; i < n; ++i)
  {
    if (m_elements[i] < m_elements[indx]) indx = i;
  }
  return m_elements[indx];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setIdentity()
{
  NM_MATRIX_CHECK_DIMENSIONS_MULTIPLY(*this, *this);
  set((ScalarType)0);
  uint32_t inc = m_numRows + 1;
  ScalarType* ptr = m_elements;
  for (uint32_t i = 0; i < m_numColumns; ++i, ptr += inc) * ptr = (ScalarType)1;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Matrix<ScalarType>::inverse(const Matrix<ScalarType>& A, Matrix<ScalarType>& tmpMat, ScalarType epsilon)
{
  NMP_ASSERT(A.numRows()      == A.numColumns());
  NMP_ASSERT(tmpMat.numRows() == tmpMat.numColumns());
  NMP_ASSERT(numRows()        == numColumns());
  NMP_ASSERT(numRows()        == A.numColumns());
  NMP_ASSERT(numRows()        == tmpMat.numColumns());

  ScalarType pivot, mult;
  tmpMat = A; // Copy of A into tmpMat to avoid modifying A while pivoting
  this->setIdentity();

  // Calculus
  for (unsigned int i = 0; i < numRows(); i++)
  {
    // If pivot is null, the matrix is not invertible
    pivot = tmpMat.element(i, i);
    ScalarType pivot_absVal = (pivot < 0) ? -pivot : pivot;
    if (pivot_absVal < epsilon)
      return false;

    // Division of the row by the pivot
    for (unsigned int j = 0; j < numColumns(); j++)
    {
      tmpMat.element(i, j) = tmpMat.element(i, j) / pivot;
      element(i, j) = element(i, j) / pivot;
    }
    // Subtracting rows
    for (unsigned int k = 0; k < numRows(); k++)
    {
      if (k != i)
      {
        mult = tmpMat.element(k, i);
        for (unsigned int l = 0; l < numColumns(); l++)
        {
          tmpMat.element(k, l) = tmpMat.element(k, l) - mult * tmpMat.element(i, l);
          element(k, l) = element(k, l) - mult * element(i, l);
        }
      }
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::cumulativeRowSum()
{
  for (uint32_t c = 0; c < m_numColumns; ++c)
  {
    ScalarType* ptr = &element(0, c);
    for (uint32_t i = 1; i < m_numRows; ++i) ptr[i] += ptr[i-1];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::cumulativeColumnSum()
{
  for (uint32_t r = 0; r < m_numRows; ++r)
  {
    ScalarType* ptr = &element(r, 0);
    ScalarType s = *ptr;
    for (uint32_t i = 1; i < m_numColumns; ++i)
    {
      ptr += m_numRows;
      *ptr += s;
      s = *ptr;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::diffRows(Matrix<ScalarType>& mat) const
{
  NMP_ASSERT(m_numRows >= 2);
  uint32_t ext = m_numRows - 1;
  NMP_ASSERT(mat.m_numRows == ext);

  for (uint32_t i = 0; i < m_numColumns; ++i)
  {
    const ScalarType* src = &element(0, i);
    ScalarType* dst = &mat.element(0, i);
    for (uint32_t k = 0; k < ext; ++k) dst[k] = src[k+1] - src[k];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::diffRows() const
{
  NMP_ASSERT(m_numRows >= 2);
  Matrix<ScalarType> mat(m_numRows - 1, m_numColumns);
  diffRows(mat);
  return mat;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::diffColumns(Matrix<ScalarType>& mat) const
{
  NMP_ASSERT(m_numColumns >= 2);
  uint32_t ext = m_numColumns - 1;
  NMP_ASSERT(mat.m_numColumns == ext);

  for (uint32_t i = 0; i < m_numRows; ++i)
  {
    const ScalarType* src = &element(i, 0);
    const ScalarType* src1 = src + m_numRows;
    ScalarType* dst = &mat.element(i, 0);
    for (uint32_t k = 0; k < ext; ++k)
    {
      *dst = *src1 - *src;
      src = src1; src1 += m_numRows;
      dst += m_numRows;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::diffColumns() const
{
  NMP_ASSERT(m_numColumns >= 2);
  Matrix<ScalarType> mat(m_numRows, m_numColumns - 1);
  diffColumns(mat);
  return mat;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::get(ScalarType* data) const
{
  NMP_ASSERT(data);
  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0 ; i < numElements ; ++i)
  {
    data[i] = m_elements[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::swap(Matrix<ScalarType>& A)
{
  uint32_t temp;
  temp = m_numRows; m_numRows = A.m_numRows; A.m_numRows = temp;
  temp = m_numColumns; m_numColumns = A.m_numColumns; A.m_numColumns = temp;

  ScalarType* data = m_elements;
  m_elements = A.m_elements; A.m_elements = data;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::getMatrixWithoutRow(uint32_t rowIndex) const
{
  Matrix result(m_numRows - 1, m_numColumns);
  for (uint32_t col = 0 ; col < m_numColumns ; ++col)
  {
    ScalarType* dest = &result.element(0, col);
    const ScalarType* src = &element(0, col);

    for (uint32_t row = 0 ; row < m_numRows ; ++row)
    {
      if (row != rowIndex)
      {
        *dest++ = src[row];
      }
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::getMatrixWithoutColumn(uint32_t colIndex) const
{
  Matrix result(m_numRows, m_numColumns - 1);
  for (uint32_t row = 0 ; row < m_numRows ; ++row)
  {
    uint32_t destColIndex = 0;
    for (uint32_t col = 0 ; col < m_numColumns ; ++col)
    {
      if (col != colIndex)
      {
        result.element(row, destColIndex++) = element(row, col);
      }
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Matrix<ScalarType>::determinant() const
{
  NM_MATRIX_CHECK_DIMENSIONS_MULTIPLY(*this, *this);
  NMP_ASSERT(m_numColumns > 1);

  if (m_numColumns == 2)
  {
    return ((m_elements[0] * m_elements[3]) - (m_elements[1] * m_elements[2]));
  }

  Matrix lowerMatrix = getMatrixWithoutRow(0);

  ScalarType determinant = (ScalarType)0;
  ScalarType sign = (ScalarType)1;
  for (uint32_t colIndex = 0 ; colIndex < m_numColumns ; ++colIndex)
  {
    Matrix subMatrix = lowerMatrix.getMatrixWithoutColumn(colIndex);
    determinant += sign * element(0, colIndex) * subMatrix.determinant();

    sign = -sign;
  }

  return determinant;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::getSubMatrix(
  uint32_t startRow, uint32_t startColumn,
  uint32_t rows, uint32_t columns) const
{
  NMP_ASSERT(startRow + rows <= m_numRows);
  NMP_ASSERT(startColumn + columns <= m_numColumns);

  Matrix subRegion(rows, columns);
  for (uint32_t col = 0 ; col < columns ; ++col)
  {
    const ScalarType* srcColumn = &element(startRow, startColumn + col);
    ScalarType* destColumn = &subRegion.element(0, col);

    for (uint32_t row = 0 ; row < rows ; ++row)
    {
      destColumn[row] = srcColumn[row];
    }
  }
  return subRegion;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getSubMatrix(
  uint32_t startRow, uint32_t startColumn,
  uint32_t rows, uint32_t columns, ScalarType* data) const
{
  NMP_ASSERT(startRow + rows <= m_numRows);
  NMP_ASSERT(startColumn + columns <= m_numColumns);

  for (uint32_t col = 0; col < columns; ++col)
  {
    const ScalarType* srcColumn = &element(startRow, startColumn + col);
    for (uint32_t row = 0; row < rows; ++row)
    {
      *data = srcColumn[row];
      data++;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getSubMatrix(
  uint32_t            startRow,
  uint32_t            startColumn,
  Matrix<ScalarType>& subMatrix) const
{
  getSubMatrix(startRow, startColumn, subMatrix.m_numRows,
               subMatrix.m_numColumns, subMatrix.m_elements);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setSubMatrix(
  uint32_t      startRow,
  uint32_t      startColumn,
  const Matrix& subMatrix)
{
  NMP_ASSERT(startRow + subMatrix.m_numRows <= m_numRows);
  NMP_ASSERT(startColumn + subMatrix.m_numColumns <= m_numColumns);

  uint32_t numColumns = subMatrix.numColumns();
  uint32_t numRows = subMatrix.numRows();
  for (uint32_t col = 0 ; col < numColumns ; ++col)
  {
    const ScalarType* srcColumn = &subMatrix.element(0, col);
    ScalarType* destColumn = &element(startRow, col + startColumn);

    for (uint32_t row = 0 ; row < numRows ; ++row)
    {
      destColumn[row] = srcColumn[row];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setSubMatrix(
  uint32_t srcStartRow, uint32_t srcStartColumn,
  uint32_t dstStartRow, uint32_t dstStartColumn,
  uint32_t rows, uint32_t columns, const Matrix& subMatrix)
{
  NMP_ASSERT(srcStartRow + rows <= subMatrix.m_numRows);
  NMP_ASSERT(srcStartColumn + columns <= subMatrix.m_numColumns);
  NMP_ASSERT(dstStartRow + rows <= m_numRows);
  NMP_ASSERT(dstStartColumn + columns <= m_numColumns);

  for (uint32_t col = 0 ; col < columns ; ++col)
  {
    const ScalarType* srcColumn = &subMatrix.element(srcStartRow, col + srcStartColumn);
    ScalarType* destColumn = &element(dstStartRow, col + dstStartColumn);

    for (uint32_t row = 0 ; row < rows ; ++row)
    {
      destColumn[row] = srcColumn[row];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setSubMatrix(
  uint32_t startRow, uint32_t startColumn,
  uint32_t rows, uint32_t columns, const ScalarType* data)
{
  NMP_ASSERT(startRow + rows <= m_numRows);
  NMP_ASSERT(startColumn + columns <= m_numColumns);

  for (uint32_t col = 0 ; col < columns ; ++col)
  {
    ScalarType* destColumn = &element(startRow, col + startColumn);

    for (uint32_t row = 0 ; row < rows ; ++row)
    {
      destColumn[row] = *data;
      data++;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setSubMatrix(
  uint32_t startRow, uint32_t startColumn,
  uint32_t rows, uint32_t columns, ScalarType s)
{
  NMP_ASSERT(startRow + rows <= m_numRows);
  NMP_ASSERT(startColumn + columns <= m_numColumns);

  for (uint32_t col = 0 ; col < columns ; ++col)
  {
    ScalarType* destColumn = &element(startRow, col + startColumn);
    for (uint32_t row = 0 ; row < rows ; ++row) destColumn[row] = s;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getRowVector(uint32_t r, uint32_t c, uint32_t cn, ScalarType* data) const
{
  NMP_ASSERT(c + cn <= m_numColumns);
  NMP_ASSERT(data);
  const ScalarType* ptr = &element(r, c);
  for (uint32_t j = 0; j < cn; ++j)
  {
    data[j] = *ptr;
    ptr += m_numRows;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Matrix<ScalarType>::getRowVector(uint32_t i) const
{
  Vector<ScalarType> v(m_numColumns);
  getRowVector(i, 0, m_numColumns, &v[0]);
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getRowVector(uint32_t i, Vector<ScalarType>& v) const
{
  getRowVector(i, 0, m_numColumns, &v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getRowVector(uint32_t i, ScalarType* data) const
{
  getRowVector(i, 0, m_numColumns, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getColumnVector(uint32_t c, uint32_t r, uint32_t rn, ScalarType* data) const
{
  NMP_ASSERT(r + rn <= m_numRows);
  NMP_ASSERT(data);
  const ScalarType* ptr = &element(r, c);
  for (uint32_t j = 0; j < rn; ++j) data[j] = ptr[j];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Matrix<ScalarType>::getColumnVector(uint32_t i) const
{
  Vector<ScalarType> v(m_numRows);
  getColumnVector(i, 0, m_numRows, &v[0]);
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getColumnVector(uint32_t i, Vector<ScalarType>& v) const
{
  getColumnVector(i, 0, m_numRows, &v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getColumnVector(uint32_t i, ScalarType* data) const
{
  getColumnVector(i, 0, m_numRows, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setRowVector(uint32_t r, uint32_t c, uint32_t cn, ScalarType val)
{
  ScalarType* ptr = &element(r, c);
  for (uint32_t j = 0; j < cn; ++j)
  {
    *ptr = val;
    ptr += m_numRows;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setRowVector(uint32_t r, uint32_t c, uint32_t cn, const ScalarType* data)
{
  NMP_ASSERT(data);
  ScalarType* ptr = &element(r, c);
  for (uint32_t j = 0; j < cn; ++j)
  {
    *ptr = data[j];
    ptr += m_numRows;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setRowVector(uint32_t i, ScalarType val)
{
  setRowVector(i, 0, m_numColumns, val);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setRowVector(uint32_t i, const Vector<ScalarType>& v)
{
  NMP_ASSERT(v.numElements() == m_numColumns);
  setRowVector(i, 0, m_numColumns, &v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setRowVector(uint32_t i, const ScalarType* data)
{
  setRowVector(i, 0, m_numColumns, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setColumnVector(uint32_t c, uint32_t r, uint32_t rn, ScalarType val)
{
  ScalarType* ptr = &element(r, c);
  for (uint32_t j = 0; j < rn; ++j) ptr[j] = val;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setColumnVector(uint32_t c, uint32_t r, uint32_t rn, const ScalarType* data)
{
  NMP_ASSERT(data);
  ScalarType* ptr = &element(r, c);
  for (uint32_t j = 0; j < rn; ++j) ptr[j] = data[j];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setColumnVector(uint32_t i, const Vector<ScalarType>& v)
{
  NMP_ASSERT(v.numElements() == m_numRows);
  setColumnVector(i, 0, m_numRows, &v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setColumnVector(uint32_t i, ScalarType val)
{
  setColumnVector(i, 0, m_numRows, val);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setColumnVector(uint32_t i, const ScalarType* data)
{
  setColumnVector(i, 0, m_numRows, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::multiplyRow(uint32_t r, ScalarType s)
{
  ScalarType* ptr = &element(r, 0);
  for (uint32_t k = 0; k < m_numColumns; ++k)
  {
    (*ptr) *= s;
    ptr += m_numRows;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::multiplyRow(uint32_t r, uint32_t c, uint32_t cn, ScalarType s)
{
  ScalarType* ptr = &element(r, c);
  for (uint32_t k = 0; k < cn; ++k)
  {
    (*ptr) *= s;
    ptr += m_numRows;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::multiplyColumn(uint32_t c, ScalarType s)
{
  ScalarType* ptr = &element(0, c);
  for (uint32_t j = 0; j < m_numRows; ++j) ptr[j] *= s;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::multiplyColumn(uint32_t c, uint32_t r, uint32_t rn, ScalarType s)
{
  ScalarType* ptr = &element(r, c);
  for (uint32_t k = 0; k < rn; ++k) ptr[k] *= s;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::addRow(uint32_t r, ScalarType s)
{
  ScalarType* ptr = &element(r, 0);
  for (uint32_t k = 0; k < m_numColumns; ++k)
  {
    (*ptr) += s;
    ptr += m_numRows;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::addRow(uint32_t r, uint32_t c, uint32_t cn, ScalarType s)
{
  ScalarType* ptr = &element(r, c);
  for (uint32_t k = 0; k < cn; ++k)
  {
    (*ptr) += s;
    ptr += m_numRows;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::addColumn(uint32_t c, ScalarType s)
{
  ScalarType* ptr = &element(0, c);
  for (uint32_t j = 0; j < m_numRows; ++j) ptr[j] += s;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::addColumn(uint32_t c, uint32_t r, uint32_t rn, ScalarType s)
{
  ScalarType* ptr = &element(r, c);
  for (uint32_t k = 0; k < rn; ++k) ptr[k] += s;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
ScalarType Matrix<ScalarType>::getRowSum(uint32_t r) const
{
  NMP_ASSERT(r < m_numRows);
  const ScalarType* ptr = &element(r, 0);
  ScalarType val = 0;
  for (uint32_t k = 0; k < m_numColumns; ++k)
  {
    ptr += m_numRows;
    val += *ptr;
  }
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getColumnVectorSum(ScalarType* data) const
{
  for (uint32_t i = 0; i < m_numRows; ++i) data[i] = getRowSum(i);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getColumnVectorSum(Vector<ScalarType>& v) const
{
  getColumnVectorSum(&v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Matrix<ScalarType>::getColumnVectorSum() const
{
  Vector<ScalarType> v(m_numRows);
  getColumnVectorSum(&v[0]);
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template<>
ScalarType Matrix<ScalarType>::getColumnSum(uint32_t c) const
{
  NMP_ASSERT(c < m_numColumns);
  const ScalarType* ptr = &element(0, c);
  ScalarType val = 0;
  for (uint32_t k = 0; k < m_numRows; ++k) val += ptr[k];
  return val;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getRowVectorSum(ScalarType* data) const
{
  for (uint32_t i = 0; i < m_numColumns; ++i) data[i] = getColumnSum(i);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getRowVectorSum(Vector<ScalarType>& v) const
{
  getRowVectorSum(&v[0]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Matrix<ScalarType>::getRowVectorSum() const
{
  Vector<ScalarType> v(m_numColumns);
  getRowVectorSum(&v[0]);
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getDiag(int32_t i, uint32_t& r, uint32_t& c, uint32_t& s) const
{
  r = c = s = 0;
  if (m_numRows >= m_numColumns)
  {
    if (i > 0)
    {
      c = (uint32_t)i;
      if (c >= m_numColumns) return; // Check within matrix
      s = m_numColumns - c;
    } else {
      r = (uint32_t) - i;
      if (r >= m_numRows) return; // Check within matrix
      if (r <= (m_numRows - m_numColumns)) s = m_numColumns; // Check within mid bound
      else s = m_numRows - r;
    }
  }
  else
  {
    if (i < 0) {
      r = (uint32_t) - i;
      if (r >= m_numRows) return; // Check within matrix
      s = m_numRows - r;
    } else {
      c = (uint32_t)i;
      if (c >= m_numColumns) return; // Check within matrix
      if (c <= (m_numColumns - m_numRows)) s = m_numRows; // Check within mid bound
      else s = m_numColumns - c;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t Matrix<ScalarType>::getDiagLength(int32_t i) const
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  return s;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getDiagVector(uint32_t r, uint32_t c, uint32_t dn, ScalarType* data) const
{
  if (dn == 0) return;
  const ScalarType* ptr = &element(r, c);
  data[0] = *ptr;
  uint32_t inc = m_numRows + 1;
  for (uint32_t j = 1; j < dn; ++j)
  {
    ptr += inc;
    data[j] = *ptr;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Matrix<ScalarType>::getDiagVector(int32_t i) const
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  NMP_ASSERT(s > 0);

  Vector<ScalarType> v(s);
  getDiagVector(r, c, s, &v[0]);
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getDiagVector(int32_t i, ScalarType* data) const
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  getDiagVector(r, c, s, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setDiagVector(int32_t i, ScalarType val)
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  if (s == 0) return;

  ScalarType* ptr = &element(r, c);
  *ptr = val;
  uint32_t inc = m_numRows + 1;
  for (uint32_t j = 1; j < s; ++j)
  {
    ptr += inc;
    *ptr = val;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setDiagVector(uint32_t r, uint32_t c, uint32_t dn, const ScalarType* data)
{
  if (dn == 0) return;
  ScalarType* ptr = &element(r, c);
  *ptr = data[0];
  uint32_t inc = m_numRows + 1;
  for (uint32_t j = 1; j < dn; ++j)
  {
    ptr += inc;
    *ptr = data[j];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setDiagVector(int32_t i, const ScalarType* data)
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  setDiagVector(r, c, s, data);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::addDiagVector(int32_t i, ScalarType val)
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  if (s == 0) return;

  ScalarType* ptr = &element(r, c);
  *ptr += val;
  uint32_t inc = m_numRows + 1;
  for (uint32_t j = 1; j < s; ++j)
  {
    ptr += inc;
    *ptr += val;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::multiplyDiagVector(int32_t i, ScalarType val)
{
  uint32_t r, c, s;
  getDiag(i, r, c, s);
  if (s == 0) return;

  ScalarType* ptr = &element(r, c);
  *ptr *= val;
  uint32_t inc = m_numRows + 1;
  for (uint32_t j = 1; j < s; ++j)
  {
    ptr += inc;
    *ptr *= val;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::shiftRows(int32_t shift, uint32_t c, uint32_t cn, ScalarType fill)
{
  ScalarType* dst;
  uint32_t s;

  // Check for no shift
  if (shift == 0) return;

  // Shift according to the direction
  if (shift > 0)
  {
    // Clamp the shift count
    s = (uint32_t)shift;
    if (s > m_numRows) s = m_numRows;

    for (uint32_t i = 0; i < cn; ++i)
    {
      // Stuff on the bottom gets shifted out so move the elements
      // from bottom to top
      dst = &element(0, c + i);
      for (uint32_t j = m_numRows - 1; j >= s; --j) dst[j] = dst[j-s];

      // Now fill in the blanks
      for (uint32_t j = 0; j < s; ++j) dst[j] = fill;
    }
  } else {
    // Clamp the shift count
    s = (uint32_t)(-shift);
    if (s > m_numRows) s = m_numRows;

    for (uint32_t i = 0; i < cn; ++i)
    {
      // Stuff on the top gets shifted out so move the elements
      // from top to bottom
      dst = &element(0, c + i);
      for (uint32_t j = 0; j < m_numRows - s; ++j) dst[j] = dst[j+s];

      // Now fill in the blanks
      for (uint32_t j = m_numRows - s; j < m_numRows; ++j) dst[j] = fill;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::shiftRows(int32_t shift, ScalarType fill)
{
  shiftRows(shift, 0, m_numColumns, fill);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::shiftColumns(int32_t shift, uint32_t r, uint32_t rn, ScalarType fill)
{
  ScalarType* src, *dst;
  uint32_t s;

  // Check for no shift
  if (shift == 0) return;

  // Shift according to the direction
  if (shift > 0)
  {
    // Clamp the shift count
    s = (uint32_t)shift;
    if (s > m_numColumns) s = m_numColumns;

    // Stuff on the right gets shifted out so move the elements
    // from right to left
    for (uint32_t i = m_numColumns - 1; i >= s; --i)
    {
      dst = &element(r, i);
      src = &element(r, i - s);
      for (uint32_t j = 0; j < rn; ++j) dst[j] = src[j];
    }

    // Now fill in the blanks
    dst = &element(r, 0);
    for (uint32_t i = 0; i < s; ++i)
    {
      for (uint32_t j = 0; j < rn; ++j) dst[j] = fill;
      dst += m_numRows;
    }

  } else {
    // Clamp the shift count
    s = (uint32_t)(-shift);
    if (s > m_numColumns) s = m_numColumns;

    // Stuff on the left gets shifted out so move the elements
    // from left to right
    for (uint32_t i = 0; i < m_numColumns - s; ++i)
    {
      dst = &element(r, i);
      src = &element(r, i + s);
      for (uint32_t j = 0; j < rn; ++j) dst[j] = src[j];
    }

    // Now fill in the blanks
    dst = &element(r, m_numColumns - s);
    for (uint32_t i = 0; i < s; ++i)
    {
      for (uint32_t j = 0; j < rn; ++j) dst[j] = fill;
      dst += m_numRows;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::shiftColumns(int32_t shift, ScalarType fill)
{
  shiftColumns(shift, 0, m_numRows, fill);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::shiftRowElements(uint32_t r, int32_t shift, ScalarType fill)
{
  NMP_ASSERT(r < m_numRows);

  // Check for no shift
  if (shift == 0) return;

  // Shift according to the direction
  if (shift > 0)
  {
    uint32_t ext = m_numColumns - 1;
    ScalarType* dst = &element(r, ext);

    // Clamp the shift count
    uint32_t j, s = (uint32_t)shift;
    if (s < m_numColumns)
    {
      ScalarType* src = &element(r, ext - s);
      for (j = ext; j >= s; --j, src -= m_numRows, dst -= m_numRows) * dst = *src;
    }
    else
    {
      s = m_numColumns;
    }
    for (j = 0; j < s; ++j, dst -= m_numRows) // because j is unsigned
    {
      *dst = fill;
    }
  }
  else
  {

    ScalarType* dst = &element(r, 0);
    uint32_t j = 0; // zero j here

    // Clamp the shift count
    uint32_t s = (uint32_t)(-shift);
    if (s < m_numColumns)
    {
      ScalarType* src = &element(r, s);
      uint32_t ext = m_numColumns - s;
      for (; j < ext; ++j, src += m_numRows, dst += m_numRows) * dst = *src;
    } else {
      s = m_numColumns;
    }
    for (; j < m_numColumns; ++j, dst += m_numRows) * dst = fill;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::shiftColumnElements(uint32_t c, int32_t shift, ScalarType fill)
{
  NMP_ASSERT(c < m_numColumns);

  // Check for no shift
  if (shift == 0) return;

  ScalarType* ptr = &element(0, c);

  // Shift according to the direction
  if (shift > 0)
  {
    // Clamp the shift count
    uint32_t j, s = (uint32_t)shift;
    if (s > m_numRows) s = m_numRows;

    for (j = m_numRows - 1; j >= s; --j) ptr[j] = ptr[j-s];
    for (j = 0; j < s; ++j) ptr[j] = fill; // because j is unsigned
  } else {

    // Clamp the shift count
    uint32_t s = (uint32_t)(-shift);
    if (s > m_numRows) s = m_numRows;

    uint32_t j, ext = m_numRows - s;
    for (j = 0; j < ext; ++j) ptr[j] = ptr[j+s];
    for (; j < m_numRows; ++j) ptr[j] = fill;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::swapRows(uint32_t r1, uint32_t r2, uint32_t c, uint32_t cn)
{
  if (r1 == r2) return;
  ScalarType* ptr1 = &element(r1, c);
  ScalarType* ptr2 = &element(r2, c);
  for (uint32_t j = 0; j < cn; ++j)
  {
    ScalarType temp;
    temp = *ptr1; *ptr1 = *ptr2; *ptr2 = temp;
    ptr1 += m_numRows; ptr2 += m_numRows;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::swapRows(uint32_t r1, uint32_t r2)
{
  swapRows(r1, r2, 0, m_numColumns);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::swapColumns(uint32_t c1, uint32_t c2, uint32_t r, uint32_t rn)
{
  if (c1 == c2) return;
  ScalarType* ptr1 = &element(r, c1);
  ScalarType* ptr2 = &element(r, c2);
  for (uint32_t j = 0; j < rn; ++j)
  {
    ScalarType temp;
    temp = ptr1[j]; ptr1[j] = ptr2[j]; ptr2[j] = temp;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::swapColumns(uint32_t c1, uint32_t c2)
{
  swapColumns(c1, c2, 0, m_numRows);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::transpose()
{
  NMP_ASSERT(m_numRows == m_numColumns);
  for (uint32_t i = 0; i < m_numRows - 1; ++i)
  {
    ScalarType* pA = &element(i, i);
    ScalarType* pB = pA;
    uint32_t n = m_numRows - i;
    for (uint32_t k = 1; k < n; ++k)
    {
      pA += m_numRows;
      ScalarType t = pB[k];
      pB[k] = *pA; *pA = t;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getTranspose(Matrix& mat) const
{
  NMP_ASSERT(mat.m_numRows == m_numColumns && mat.m_numColumns == m_numRows);
  NMP_ASSERT(this != &mat);
  for (uint32_t j = 0; j < m_numRows; ++j)
  {
    const ScalarType* pA = &element(j, 0);
    ScalarType* pB = &mat.element(0, j);
    pB[0] = *pA;
    for (uint32_t i = 1; i < m_numColumns; ++i)
    {
      pA += m_numRows;
      pB[i] = *pA;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::getTranspose() const
{
  Matrix result(m_numColumns, m_numRows);
  getTranspose(result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setTranspose(const Matrix& mat)
{
  NMP_ASSERT(mat.m_numRows == m_numColumns && mat.m_numColumns == m_numRows);
  NMP_ASSERT(this != &mat);
  for (uint32_t j = 0; j < m_numRows; ++j)
  {
    ScalarType* pA = &element(j, 0);
    const ScalarType* pB = &mat.element(0, j);
    *pA = pB[0];
    for (uint32_t i = 1; i < m_numColumns; ++i)
    {
      pA += m_numRows;
      *pA = pB[i];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::multiplyDiag(const ScalarType* v)
{
  for (uint32_t i = 0; i < m_numRows; ++i) multiplyRow(i, v[i]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::postMultiplyDiag(const ScalarType* v)
{
  for (uint32_t i = 0; i < m_numColumns; ++i) multiplyColumn(i, v[i]);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::preMultiply(uint32_t n, ScalarType* v, const ScalarType* u) const
{
  for (uint32_t i = 0; i < n; ++i)
  {
    const ScalarType* ptr = &element(i, 0);
    ScalarType val = (*ptr) * u[0];
    for (uint32_t k = 1; k < m_numColumns; ++k)
    {
      ptr += m_numRows;
      val += (*ptr) * u[k];
    }
    v[i] = val;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::preMultiplyTranspose(uint32_t n, ScalarType* v, const ScalarType* u) const
{
  NMP_ASSERT(v != 0);
  NMP_ASSERT(u != 0);
  NMP_ASSERT(n > 0);
  for (uint32_t i = 0; i < m_numColumns; ++i)
  {
    const ScalarType* ptr = &element(0, i);
    v[i] = u[0] * ptr[0];
    for (uint32_t k = 1; k < n; ++k) v[i] += (ptr[k] * u[k]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::outer(const ScalarType* u, const ScalarType* v)
{
  for (uint32_t i = 0; i < m_numColumns; ++i)
  {
    ScalarType* ptr = &element(0, i);
    for (uint32_t j = 0; j < m_numRows; ++j)
    {
      ptr[j] = u[j] * v[i];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::outerUpdate(const ScalarType* u, const ScalarType* v)
{
  for (uint32_t i = 0; i < m_numColumns; ++i)
  {
    ScalarType* ptr = &element(0, i);
    for (uint32_t j = 0; j < m_numRows; ++j)
    {
      ptr[j] += u[j] * v[i];
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getUpper(Matrix& mat) const
{
  // Number of rows can differ
  NMP_ASSERT(mat.m_numColumns == m_numColumns);
  uint32_t m = mat.m_numRows;
  uint32_t i, rc = NMP::minimum(m, m_numColumns);

  // Get the upper triangle of the smallest square
  for (i = 0; i < rc; ++i)
  {
    const ScalarType* pA = &element(0, i);
    ScalarType* pB = &mat.element(0, i);
    uint32_t k;
    for (k = 0; k <= i; ++k) pB[k] = pA[k];
    for (; k < m; ++k) pB[k] = static_cast<ScalarType>(0);
  }

  // Get the full right block for n>m
  for (; i < m_numColumns; ++i)
  {
    const ScalarType* pA = &element(0, i);
    ScalarType* pB = &mat.element(0, i);
    for (uint32_t k = 0; k < m; ++k) pB[k] = pA[k];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getLower(Matrix& mat) const
{
  // Number of columns can differ
  NMP_ASSERT(mat.m_numRows == m_numRows);
  uint32_t n = mat.m_numColumns;
  uint32_t i, rc = NMP::minimum(m_numRows, n);

  // Get the lower triangle of the smallest square
  for (i = 0; i < rc; ++i)
  {
    const ScalarType* pA = &element(0, i);
    ScalarType* pB = &mat.element(0, i);
    uint32_t k;
    for (k = 0; k < i; ++k) pB[k] = static_cast<ScalarType>(0);
    for (; k < m_numRows; ++k) pB[k] = pA[k];
  }

  // Zero the full right block for n>m
  for (; i < n; ++i)
  {
    ScalarType* pB = &mat.element(0, i);
    for (uint32_t k = 0; k < m_numRows; ++k) pB[k] = static_cast<ScalarType>(0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::getUpper() const
{
  uint32_t m = NMP::minimum(m_numRows, m_numColumns);
  Matrix<ScalarType> result(m, m_numColumns);
  getUpper(result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Matrix<ScalarType>::getLower() const
{
  uint32_t n = NMP::minimum(m_numRows, m_numColumns);
  Matrix<ScalarType> result(m_numRows, n);
  getLower(result);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setUpper(const Matrix& mat)
{
  // Number of rows can differ
  NMP_ASSERT(mat.m_numColumns == m_numColumns);
  uint32_t m = NMP::minimum(m_numRows, mat.m_numRows);
  uint32_t i, rc = NMP::minimum(m, m_numColumns);

  // Set the upper triangle of the smallest square
  for (i = 0; i < rc; ++i)
  {
    ScalarType* pA = &element(0, i);
    const ScalarType* pB = &mat.element(0, i);
    uint32_t k;
    for (k = 0; k <= i; ++k) pA[k] = pB[k];
  }

  // Set the full right block for n>m
  for (; i < m_numColumns; ++i)
  {
    ScalarType* pA = &element(0, i);
    const ScalarType* pB = &mat.element(0, i);
    for (uint32_t k = 0; k < m; ++k) pA[k] = pB[k];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setUpper(ScalarType s)
{
  uint32_t i, rc = NMP::minimum(m_numRows, m_numColumns);

  // Set the upper triangle of the smallest square
  for (i = 0; i < rc; ++i)
  {
    ScalarType* pA = &element(0, i);
    for (uint32_t k = 0; k <= i; ++k) pA[k] = s;
  }

  // Set the full right block for n>m
  for (; i < m_numColumns; ++i)
  {
    ScalarType* pA = &element(0, i);
    for (uint32_t k = 0; k < m_numRows; ++k) pA[k] = s;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setLower(const Matrix& mat)
{
  // Number of columns can differ
  NMP_ASSERT(mat.m_numRows == m_numRows);
  uint32_t n = mat.m_numColumns;
  uint32_t i, rc = NMP::minimum(m_numRows, n);

  // Set the lower triangle of the smallest square
  for (i = 0; i < rc; ++i)
  {
    ScalarType* pA = &element(0, i);
    const ScalarType* pB = &mat.element(0, i);
    for (uint32_t k = i; k < m_numRows; ++k) pA[k] = pB[k];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setLower(ScalarType s)
{
  uint32_t i, rc = NMP::minimum(m_numRows, m_numColumns);

  // Set the lower triangle of the smallest square
  for (i = 0; i < rc; ++i)
  {
    ScalarType* pA = &element(0, i);
    for (uint32_t k = i; k < m_numRows; ++k) pA[k] = s;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::getBandDiag(Matrix& mat, uint32_t sub, uint32_t super) const
{
  // Matrix can be a different size
  mat.set(static_cast<ScalarType>(0));

  uint32_t r, c, s;
  for (int32_t i = -(int32_t)sub; i <= (int32_t)super; ++i)
  {
    getDiag(i, r, c, s);
    NMP_ASSERT(s > 0);

    const ScalarType* pA = &element(r, c);
    ScalarType* pB = &mat.element(r, c);
    *pB = *pA;
    uint32_t incA = m_numRows + 1;
    uint32_t incB = mat.m_numRows + 1;
    for (uint32_t j = 1; j < s; ++j)
    {
      pA += incA; pB += incB;
      *pB = *pA;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setBandDiag(const Matrix& mat, uint32_t sub, uint32_t super)
{
  // Matrix can be a different size
  uint32_t r, c, s;
  for (int32_t i = -(int32_t)sub; i <= (int32_t)super; ++i)
  {
    getDiag(i, r, c, s);
    NMP_ASSERT(s > 0);

    ScalarType* pA = &element(r, c);
    const ScalarType* pB = &mat.element(r, c);
    *pA = *pB;
    uint32_t incA = m_numRows + 1;
    uint32_t incB = mat.m_numRows + 1;
    for (uint32_t j = 1; j < s; ++j)
    {
      pA += incA; pB += incB;
      *pA = *pB;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::setFromMultiplicationSym(const Matrix<ScalarType>& A, const Matrix<ScalarType>& B)
{
  NM_MATRIX_CHECK_DIMENSIONS_MULTIPLY(A, B);
  NMP_ASSERT(m_numRows == A.m_numRows);

  for (uint32_t i = 0; i < m_numColumns; ++i)
  {
    ScalarType* dst = &element(0, i);
    const ScalarType* srcB = &B.element(0, i);
    for (uint32_t j = 0; j <= i; ++j)
    {
      const ScalarType* srcA = &A.element(j, 0);
      ScalarType v = (*srcA) * srcB[0];
      for (uint32_t k = 1; k < A.m_numColumns; ++k)
      {
        srcA += A.m_numRows; // Update pointer
        v += (*srcA) * srcB[k];
      }
      dst[j] = v; // Set value
    }
  }
  makeSymmetric();
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> operator* (const Vector<ScalarType>& v, const Matrix<ScalarType>& mat)
{
  NMP_ASSERT(v.numElements() == mat.numRows());
  Vector<ScalarType> result(mat.numColumns());
  for (uint32_t elIdx = 0 ; elIdx < mat.numColumns() ; ++elIdx)
  {
    // result[elIdx] = sum_{ i=0 }^{ m_numRows } (m[i, elIdx] * v[i])
    ScalarType sum = 0;
    for (uint32_t i = 0 ; i < mat.numRows() ; ++i)
    {
      sum += mat.element(i, elIdx) * v[i];
    }
    result[elIdx] = sum;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// "inverse" for 4*4 tm (transform matrix)
template <>
Matrix<ScalarType> Matrix<ScalarType>::tmInverse() const
{
  NMP_ASSERT((numRows() == 4) && (numColumns() == 4));

  Matrix<ScalarType> rotTranspose(3, 3);
  rotTranspose = getSubMatrix(0, 0, 3, 3);
  rotTranspose.transpose();

  Matrix<ScalarType> matPos(3, 1);
  matPos = getSubMatrix(0, 3, 3, 1);
  Matrix<ScalarType> invPos(3, 1);
  invPos = rotTranspose * matPos;

  Matrix<ScalarType> inverse(4, 4);
  inverse.setSubMatrix(0, 0, rotTranspose);
  inverse.element(0, 3) = -invPos.element(0, 0);
  inverse.element(1, 3) = -invPos.element(1, 0);
  inverse.element(2, 3) = -invPos.element(2, 0);
  inverse.element(3, 0) = (ScalarType)0.0;
  inverse.element(3, 1) = (ScalarType)0.0;
  inverse.element(3, 2) = (ScalarType)0.0;
  inverse.element(3, 3) = (ScalarType)1.0;

  return inverse;
}

//----------------------------------------------------------------------------------------------------------------------
// "inverse" for 3*3 rm (rotation matrix)
template <>
Matrix<ScalarType> Matrix<ScalarType>::rmInverse() const
{
  NMP_ASSERT((numRows() == 3) && (numColumns() == 3));

  Matrix<ScalarType> rotTranspose(3, 3);
  rotTranspose = getSubMatrix(0, 0, 3, 3);
  rotTranspose.transpose();

  return rotTranspose;
}

//----------------------------------------------------------------------------------------------------------------------
// "inverse" for 4*4 tm (transform matrix)
template <>
void Matrix<ScalarType>::tmSetFromInverse(
  const Matrix<ScalarType>&src,
  Matrix<ScalarType>&tempRot,
  Matrix<ScalarType>&tempPos0,
  Matrix<ScalarType>&tempPos1)
{
  NMP_ASSERT((numRows() == 4) && (numColumns() == 4));

  src.getSubMatrix(0, 0, tempRot);
  tempRot.transpose();

  src.getSubMatrix(0, 3, tempPos0);
  tempPos1.setFromMultiplication (tempRot, tempPos0);

  setSubMatrix(0, 0, tempRot);
  element(0, 3) = -tempPos1.element(0, 0);
  element(1, 3) = -tempPos1.element(1, 0);
  element(2, 3) = -tempPos1.element(2, 0);
  element(3, 0) = (ScalarType)0.0;
  element(3, 1) = (ScalarType)0.0;
  element(3, 2) = (ScalarType)0.0;
  element(3, 3) = (ScalarType)1.0;

  return;
}

//----------------------------------------------------------------------------------------------------------------------
// "symmetric" part of a square matrix
template <>
Matrix<ScalarType> Matrix<ScalarType>::sym() const
{
  NMP_ASSERT(numRows() == numColumns());

  Matrix<ScalarType> matTranspose(*this);
  Matrix<ScalarType> sum(numRows(), numColumns());
  Matrix<ScalarType> res(numRows(), numColumns());

  matTranspose.transpose();
  sum = *this + matTranspose;
  res = static_cast<ScalarType>(0.5) * sum;

  return res;
}

//----------------------------------------------------------------------------------------------------------------------
// "asymmetric" part of a square matrix
template <>
Matrix<ScalarType> Matrix<ScalarType>::asym() const
{
  NMP_ASSERT(numRows() == numColumns());

  Matrix<ScalarType> matTranspose(*this);
  Matrix<ScalarType> diff(numRows(), numColumns());
  Matrix<ScalarType> res(numRows(), numColumns());

  matTranspose.transpose();
  diff = *this - matTranspose;
  res = static_cast<ScalarType>(0.5) * diff;

  return res;
}

//----------------------------------------------------------------------------------------------------------------------
// "symmetric" part of a square matrix
template <>
void Matrix<ScalarType>::sym(const Matrix<ScalarType>& src)
{
  NMP_ASSERT(src.numRows() == src.numColumns());
  NMP_ASSERT(numRows() == src.numRows());
  NMP_ASSERT(numColumns() == src.numColumns());

  Matrix<ScalarType> matTranspose(src);
  Matrix<ScalarType> sum(numRows(), numColumns());
  Matrix<ScalarType> res(numRows(), numColumns());

  matTranspose.transpose();
  sum = src + matTranspose;
  *this = static_cast<ScalarType>(0.5) * sum;
}

//----------------------------------------------------------------------------------------------------------------------
// "asymmetric" part of a square matrix
template <>
void Matrix<ScalarType>::asym(const Matrix<ScalarType>& src)
{
  NMP_ASSERT(src.numRows() == src.numColumns());
  NMP_ASSERT(numRows() == src.numRows());
  NMP_ASSERT(numColumns() == src.numColumns());

  Matrix<ScalarType> matTranspose(src);
  Matrix<ScalarType> diff(numRows(), numColumns());
  Matrix<ScalarType> res(numRows(), numColumns());

  matTranspose.transpose();
  diff.setFromDiff(src, matTranspose);
  this->setFromMultiplication(static_cast<ScalarType>(0.5), diff);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Matrix<ScalarType>::gaussTransform(uint32_t k, const ScalarType* v, uint32_t c, uint32_t cn)
{
  const ScalarType* Ck = &element(k, c);
  ScalarType* ptr = &element(k + 1, c);
  uint32_t n = m_numRows - k - 1;
  for (uint32_t i = 0; i < cn; ++i)
  {
    ScalarType Cki = *Ck;
    for (uint32_t j = 0; j < n; ++j) ptr[j] -= v[j] * Cki;
    Ck += m_numRows;
    ptr += m_numRows;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Matrix<ScalarType>::norm() const
{
  return (ScalarType)NMP::enorm<ScalarType, NormType>(m_numRows * m_numColumns, m_elements);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Matrix<ScalarType>::isZero(ScalarType threshold) const
{
  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i)
  {
    if (abs((NormType)m_elements[i]) > threshold) return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Matrix<ScalarType>::isEqual(const Matrix<ScalarType>& mat, ScalarType threshold) const
{
  // Should this assert?
  if (!isSize(mat.numRows(), mat.numColumns())) return false;

  Matrix<ScalarType> D(*this);
  D -= mat;
  return D.isZero(threshold);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Inverse2x2(const Matrix<ScalarType>& A, Matrix<ScalarType>& iA)
{
  NMP_ASSERT(A.numRows() == 2 && A.numColumns() == 2);
  NMP_ASSERT(iA.isSize(A.numRows(), A.numColumns()));
  ScalarType a = A.element(0, 0);
  ScalarType b = A.element(0, 1);
  ScalarType c = A.element(1, 0);
  ScalarType d = A.element(1, 1);
  ScalarType det = a * d - b * c;
  if (det == static_cast<ScalarType>(0)) return false;

  det = static_cast<ScalarType>(1) / det;
  iA.element(0, 0) = d * det;
  iA.element(0, 1) = -b * det;
  iA.element(1, 0) = -c * det;
  iA.element(1, 1) = a * det;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> Inverse2x2(const Matrix<ScalarType>& A)
{
  Matrix<ScalarType> iA(2, 2);
  if (!Inverse2x2(A, iA)) iA.set(static_cast<ScalarType>(0));
  return iA;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Matrix<ScalarType> PseudoInverse(const Matrix<ScalarType>& A)
{
  Matrix<ScalarType> At = A.getTranspose();
  return Inverse2x2(At * A) * At;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
