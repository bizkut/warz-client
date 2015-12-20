// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

// This file assumes that ScalarType is set to an appropriate type
// before this file is included from another .cpp file.
#ifndef ScalarType
  #error This file should not be compiled directly
#endif

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// Apply _op to each element of _src_a and _src_b and put the result
// into the corresponding element of _dst.  Does not do any checking
// of sizes.
#define NM_VECTOR_OPERATE(_dest, _src_a, _src_b, _op)                         \
  for (uint32_t i = 0 ; i < (_dest).m_numElements ; ++i)                          \
  {                                                                             \
    (_dest).m_elements[i] = (_src_a).m_elements[i] _op (_src_b).m_elements[i]; \
  }

// Apply _op to each element of _src_a and _scalar and put the result
// into the corresponding element of _dst.  Does not do any checking
// of sizes.
#define NM_VECTOR_OPERATE_SCALAR(_dest , _src_a , _scalar , _op)              \
  for (uint32_t i = 0 ; i < (_src_a).m_numElements ; ++i)                         \
  {                                                                             \
    (_dest).m_elements[i] = (_src_a).m_elements[i] _op _scalar;           \
  }

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::set(const ScalarType* data)
{
  // MORPH-21302: Replace with fast memcpy for relevant platforms
  NMP_ASSERT(data != 0);
  for (uint32_t i = 0; i < m_numElements; ++i) m_elements[i] = data[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::set(ScalarType val)
{
  for (uint32_t i = 0; i < m_numElements; ++i) m_elements[i] = val;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>::~Vector()
{
  if (m_elements) delete [] m_elements;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>::Vector() :
  m_numElements(0), m_elements(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>::Vector(uint32_t elements) :
  m_numElements(elements), m_elements(NULL)
{
  if (elements > 0) m_elements = new ScalarType [m_numElements];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>::Vector(uint32_t elements, ScalarType value) :
  m_numElements(elements), m_elements(NULL)
{
  if (elements > 0) {
    m_elements = new ScalarType [m_numElements];
    set(value);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>::Vector(uint32_t elements, const ScalarType* data) :
  m_numElements(elements), m_elements(NULL)
{
  if (elements > 0) {
    m_elements = new ScalarType [m_numElements];
    set(data);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>::Vector(const Vector<ScalarType>& copy) :
  m_numElements(copy.m_numElements), m_elements(NULL)
{
  if (m_numElements > 0) {
    m_elements = new ScalarType [m_numElements];
    set(copy.m_elements);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const Vector<ScalarType>& Vector<ScalarType>::operator = (const Vector& copy)
{
  NMP_ASSERT(m_numElements == copy.m_numElements);
  if (m_elements) set(copy.m_elements);
  return (*this);
}

//----------------------------------------------------------------------------------------------------------------------
// Basic operations
template <>
Vector<ScalarType> Vector<ScalarType>:: operator + (const Vector<ScalarType>& vec) const
{
  NMP_ASSERT(m_numElements == vec.m_numElements);

  Vector<ScalarType> result(m_numElements);
  NM_VECTOR_OPERATE(result, *this, vec, +);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Vector<ScalarType>::operator - (const Vector<ScalarType>& v) const
{
  NMP_ASSERT(m_numElements == v.m_numElements);

  Vector result(m_numElements);
  NM_VECTOR_OPERATE(result, *this, v, -);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Vector<ScalarType>::operator * (ScalarType f) const
{
  Vector result(m_numElements);
  NM_VECTOR_OPERATE_SCALAR(result, *this, f, *);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Vector<ScalarType>::operator / (ScalarType f) const
{
  Vector result(m_numElements);
  NM_VECTOR_OPERATE_SCALAR(result, *this, f, /);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::operator += (const Vector<ScalarType>& v)
{
  NMP_ASSERT(m_numElements == v.m_numElements);

  NM_VECTOR_OPERATE(*this, *this, v, +);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::operator -= (const Vector<ScalarType>& v)
{
  NMP_ASSERT(m_numElements == v.m_numElements);

  NM_VECTOR_OPERATE(*this, *this, v, -);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::operator += (ScalarType f)
{
  NM_VECTOR_OPERATE_SCALAR(*this, *this, f, +);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::operator -= (ScalarType f)
{
  NM_VECTOR_OPERATE_SCALAR(*this, *this, f, -);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::operator *= (ScalarType f)
{
  NM_VECTOR_OPERATE_SCALAR(*this, *this, f, *);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::operator /= (ScalarType f)
{
  NM_VECTOR_OPERATE_SCALAR(*this, *this, f, /);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Vector<ScalarType>::operator - () const
{
  Vector result(m_numElements);
  for (uint32_t i = 0 ; i < m_numElements ; ++i)
  {
    result.m_elements[i] = - m_elements[i];
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Vector<ScalarType>::operator == (const Vector<ScalarType>& v) const
{
  if (m_numElements != v.m_numElements) return false;

  for (uint32_t i = 0 ; i < m_numElements ; ++i)
  {
    if (m_elements[i] != v.m_elements[i])
    {
      return false;
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Vector<ScalarType>::operator != (const Vector<ScalarType>& v) const
{
  return !(*this == v);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType& Vector<ScalarType>::element(uint32_t index)
{
  NMP_ASSERT(m_elements != 0);
  return m_elements[index];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const ScalarType& Vector<ScalarType>::element(uint32_t index) const
{
  NMP_ASSERT(m_elements != 0);
  return m_elements[index];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType& Vector<ScalarType>::end(uint32_t index)
{
  NMP_ASSERT(m_elements != 0);
  return m_elements[(m_numElements-1) - index];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const ScalarType& Vector<ScalarType>::end(uint32_t index) const
{
  NMP_ASSERT(m_elements != 0);
  return m_elements[(m_numElements-1) - index];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType* Vector<ScalarType>::data()
{
  return m_elements;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const ScalarType* Vector<ScalarType>::data() const
{
  return m_elements;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
uint32_t Vector<ScalarType>::numElements() const
{
  return m_numElements;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Vector<ScalarType>::isSize(uint32_t n) const
{
  return m_numElements == n;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
bool Vector<ScalarType>::isEmpty() const
{
  return m_elements == NULL;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::swap(Vector<ScalarType>& v)
{
  uint32_t n = m_numElements;
  m_numElements = v.numElements();
  v.m_numElements = n;

  ScalarType* data = m_elements;
  m_elements = v.m_elements;
  v.m_elements = data;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType& Vector<ScalarType>::operator [] (uint32_t index)
{
  NMP_ASSERT(m_elements != 0);
  return m_elements[index];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
const ScalarType& Vector<ScalarType>::operator [] (uint32_t index) const
{
  NMP_ASSERT(m_elements != 0);
  return m_elements[index];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Vector<ScalarType>::magnitudeSquared() const
{
  NMP_ASSERT(m_elements != 0);
  ScalarType magSquared = 0;
  for (uint32_t i = 0 ; i < m_numElements ; ++i)
  {
    ScalarType cVal = m_elements[i];
    magSquared += (cVal * cVal);
  }
  return magSquared;
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef FloatSpecific
template <>
ScalarType Vector<ScalarType>::magnitude() const
{
  return sqrt(magnitudeSquared());
}
#endif

#ifdef IntegerSpecific
// Specialization of functions for integer type
template <>
int32_t Vector<int32_t>::magnitude() const
{
  return (int32_t)sqrt((float)magnitudeSquared());
}
#endif // IntegerSpecific

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::normalise()
{
  NMP_ASSERT(m_elements != 0);
  ScalarType mag = magnitude();
  (*this) /= mag;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::cumulativeSum()
{
  for (uint32_t i = 1; i < m_numElements; ++i) m_elements[i] += m_elements[i-1];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::linear(ScalarType start, ScalarType end)
{
  NMP_ASSERT(m_numElements > 0);
  if (m_numElements == 1) { m_elements[0] = end; return; }

  // Set the internal points
  uint32_t ext = m_numElements - 1;
  ScalarType m = (end - start) / ext;
  for (uint32_t i = 1; i < ext; ++i) m_elements[i] = m * i + start;

  // Set the first and last points explicitly in order to avoid
  // potential problems with roundoff error
  m_elements[0] = start;
  m_elements[ext] = end;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType> Vector<ScalarType>::diff() const
{
  NMP_ASSERT(m_numElements >= 2);
  uint32_t ext = m_numElements - 1;
  Vector<ScalarType> v(ext);
  for (uint32_t i = 0; i < ext; ++i) v[i] = m_elements[i+1] - m_elements[i];
  return v;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::diff(Vector<ScalarType>& v) const
{
  NMP_ASSERT(m_numElements >= 2);
  uint32_t ext = m_numElements - 1;
  NMP_ASSERT(v.numElements() == ext);
  for (uint32_t i = 0; i < ext; ++i) v[i] = m_elements[i+1] - m_elements[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& Vector<ScalarType>::setFromMultiplication(const Matrix<ScalarType>& m, const Vector<ScalarType>& v)
{
  NMP_ASSERT(this->numElements() == m.numRows() &&
             m.numColumns() == v.numElements());

  for (uint32_t i = 0 ; i < m.numRows() ; ++i)
  {
    m_elements[i] = 0;
    for (uint32_t j = 0 ; j < m.numColumns() ; ++j)
      m_elements[i] += m.element(i, j) * v[j];
  }

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& Vector<ScalarType>::setFromMultiplication(ScalarType coeff, const Vector& v)
{
  NMP_ASSERT(numElements() == v.numElements());

  NM_VECTOR_OPERATE_SCALAR(*this, v, coeff, *)

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& Vector<ScalarType>::setFromMultiplication(ScalarType coeff)
{
  for (uint32_t i = 0 ; i < m_numElements ; ++i)
    element(i) = coeff * element(i);

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& Vector<ScalarType>::setFromAdd(const Vector& v1, const Vector& v2)
{
  NMP_ASSERT(numElements() == v1.numElements());
  NMP_ASSERT(numElements() == v2.numElements());

  NM_VECTOR_OPERATE(*this, v1, v2, +)

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
Vector<ScalarType>& Vector<ScalarType>::setFromDiff(const Vector& v1, const Vector& v2)
{
  NMP_ASSERT(numElements() == v1.numElements());
  NMP_ASSERT(numElements() == v2.numElements());

  NM_VECTOR_OPERATE(*this, v1, v2, -)

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::reverse()
{
  if (m_numElements < 2) return;

  uint32_t i = 0;
  uint32_t j = m_numElements - 1;
  for (; i < j; ++i, --j)
  {
    ScalarType temp = m_elements[i];
    m_elements[i] = m_elements[j];
    m_elements[j] = temp;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// a rather pants exchange sort...
template <>
void Vector<ScalarType>::sort()
{
  for (uint32_t i = 0; i < m_numElements; ++i)
  {
    for (uint32_t j = i + 1; j < m_numElements; ++j)
    {
      if (m_elements[i] < m_elements[j])
      {
        ScalarType temp = m_elements[i];
        m_elements[i] = m_elements[j];
        m_elements[j] = temp;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Vector<ScalarType>::dot(const Vector<ScalarType>& v) const
{
  NMP_ASSERT(m_numElements == v.m_numElements);
  ScalarType sum = (ScalarType)0;
  for (uint32_t i = 0 ; i < m_numElements ; ++i)
  {
    sum += m_elements[i] * v.m_elements[i];
  }
  return sum;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::get(ScalarType* data) const
{
  // MORPH-21302: Replace with fast memcpy for relevant platforms
  NMP_ASSERT(data != 0);
  for (uint32_t i = 0; i < m_numElements; ++i) data[i] = m_elements[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::getSubVector(uint32_t indx, uint32_t n, ScalarType* data) const
{
  NMP_ASSERT(n + indx <= m_numElements);
  const ScalarType* ptr = &m_elements[indx];
  for (uint32_t i = 0; i < n; ++i) data[i] = ptr[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::getSubVector(uint32_t indx, Vector<ScalarType>& v) const
{
  getSubVector(indx, v.m_numElements, v.m_elements);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::setSubVector(uint32_t indx, uint32_t n, const ScalarType* data)
{
  NMP_ASSERT(n + indx <= m_numElements);
  ScalarType* ptr = &m_elements[indx];
  for (uint32_t i = 0; i < n; ++i) ptr[i] = data[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::setSubVector(uint32_t indx, uint32_t n, ScalarType s)
{
  NMP_ASSERT(n + indx <= m_numElements);
  ScalarType* ptr = &m_elements[indx];
  for (uint32_t i = 0; i < n; ++i) ptr[i] = s;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::setSubVector(uint32_t indx, const Vector<ScalarType>& v)
{
  setSubVector(indx, v.m_numElements, v.m_elements);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::setSubVector(uint32_t indx, ScalarType s)
{
  for (uint32_t i = indx; i < m_numElements; ++i) m_elements[i] = s;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::absolute()
{
#if defined FloatSpecific
  for (uint32_t i = 0; i < m_numElements; ++i) m_elements[i] = fabs(m_elements[i]);
#else
  for (uint32_t i = 0; i < m_numElements; ++i) m_elements[i] = (int32_t)abs((float)m_elements[i]);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::zeroSmall(ScalarType v)
{
  for (uint32_t i = 0; i < m_numElements; ++i)
  {
#if defined FloatSpecific
    if (fabs(m_elements[i]) < v) m_elements[i] = 0;
#else
    if ((int32_t)abs((float)m_elements[i]) < v) m_elements[i] = 0;
#endif
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::multiply(const Vector<ScalarType>& v)
{
  NMP_ASSERT(m_numElements == v.m_numElements);
  for (uint32_t i = 0; i < m_numElements; ++i) m_elements[i] *= v.m_elements[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
void Vector<ScalarType>::square()
{
  for (uint32_t i = 0; i < m_numElements; ++i) m_elements[i] *= m_elements[i];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Vector<ScalarType>::maximum(uint32_t& indx) const
{
  indx = 0;
  for (uint32_t i = 1; i < m_numElements; ++i)
  {
    if (m_elements[i] > m_elements[indx]) indx = i;
  }
  return m_elements[indx];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Vector<ScalarType>::minimum(uint32_t& indx) const
{
  indx = 0;
  for (uint32_t i = 1; i < m_numElements; ++i)
  {
    if (m_elements[i] < m_elements[indx]) indx = i;
  }
  return m_elements[indx];
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Vector<ScalarType>::maximum() const
{
  uint32_t indx;
  return maximum(indx);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Vector<ScalarType>::minimum() const
{
  uint32_t indx;
  return minimum(indx);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Vector<ScalarType>::sum() const
{
  ScalarType sum = static_cast<ScalarType>(0);
  for (uint32_t i = 0; i < m_numElements; ++i) sum += m_elements[i];
  return sum;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Vector<ScalarType>::mean() const
{
  NMP_ASSERT(m_numElements > 0);
  return sum() / m_numElements;
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Vector<ScalarType>::median() const
{
  NMP_ASSERT(m_numElements > 0);
  Vector<ScalarType> v(*this);
  v.sort();

  // Check for an odd number of elements
  uint32_t i = m_numElements >> 1;
  if (m_numElements & 0x01) return v[i];

  // Even number of elements
  return (v[i-1] + v[i]) / static_cast<ScalarType>(2);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
ScalarType Vector<ScalarType>::var() const
{
  ScalarType sum = static_cast<ScalarType>(0);
  if (m_numElements < 2) return sum;

  ScalarType xbar = mean();
  for (uint32_t i = 0; i < m_numElements; ++i)
  {
    ScalarType dx = m_elements[i] - xbar;
    sum += dx * dx;
  }

  // Return the unbiased estimator for the population variance
  return sum / (m_numElements - 1);
}

//----------------------------------------------------------------------------------------------------------------------
template <>
int32_t Vector<ScalarType>::interval(ScalarType x, uint32_t& i, bool incend) const
{
  // Before the first element
  NMP_ASSERT(m_numElements >= 2);
  if (x < m_elements[0]) { i = 0; return -1; }

  uint32_t low = 0;
  uint32_t high = m_numElements - 1;

  if (incend)
  {
    // Beyond the last element
    if (x > m_elements[high]) { i = high; return 1; }

    // Find the first non repeated element before the end value
    for (--high; high > 1; --high) if (m_elements[high] != m_elements[high+1]) break;

    // Check if x lies within the last interval
    if (x >= m_elements[high] && x <= m_elements[high+1]) { i = high; return 0; }
  } else {
    // Beyond the last element
    if (x >= m_elements[high]) { i = high; return 1; }
  }

  // Perform a binary search to find the required interval
  i = (low + high) >> 1;
  while (x < m_elements[i] || x >= m_elements[i+1])
  {
    if (x < m_elements[i]) high = i; else low = i;
    i = (low + high) >> 1;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
// Function templates that are specific to float types
#ifdef FloatSpecific
template <>
bool Vector<ScalarType>::isZero(ScalarType threshold) const
{
  for (uint32_t i = 0 ; i < m_numElements ; ++i)
  {
    if (abs(m_elements[i]) > threshold) return false;
  }
  return true;
}

template <>
bool Vector<ScalarType>::isEqual(const Vector& v, ScalarType threshold) const
{
  NMP_ASSERT(v.m_numElements == m_numElements);
  for (uint32_t i = 0 ; i < m_numElements ; ++i)
  {
    if (fabs(m_elements[i] - v.m_elements[i]) > threshold) return false;
  }
  return true;
}

template <>
void Vector<ScalarType>::squareroot()
{
  for (uint32_t i = 0; i < m_numElements; ++i) m_elements[i] = sqrt(m_elements[i]);
}

template <>
ScalarType Vector<ScalarType>::sd() const
{
  return sqrt(var());
}
#endif // FloatSpecific

#undef NM_VECTOR_OPERATE
#undef NM_VECTOR_OPERATE_SCALAR

} // namespace NMP

