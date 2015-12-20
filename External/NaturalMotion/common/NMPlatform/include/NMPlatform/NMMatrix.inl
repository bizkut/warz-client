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

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
template <typename Q>
NM_INLINE Matrix<T>::Matrix(const Matrix<Q>& other) :
  m_numRows(other.numRows()),
  m_numColumns(other.numColumns())
{
  uint32_t numElements = m_numRows * m_numColumns;
  m_elements = new T [numElements];
  const Q* otherElements = other.data();
  for (uint32_t i = 0; i < numElements; ++i)
  {
    m_elements[i] = (T)otherElements[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
template <typename Q>
NM_INLINE
void Matrix<T>::get(Q* data) const
{
  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i)
  {
    data[i] = (Q)m_elements[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
template <typename Q>
NM_INLINE
void Matrix<T>::set(const Q* data)
{
  uint32_t numElements = m_numRows * m_numColumns;
  for (uint32_t i = 0; i < numElements; ++i)
  {
    m_elements[i] = (T)data[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE Matrix<T>
operator * (T s, const Matrix<T>& mat)
{
  return mat * s;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
