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
NM_INLINE Vector<T>::Vector(const Vector<Q>& other) :
  m_numElements(other.numElements())
{
  m_elements = new T [m_numElements];
  for (uint32_t i = 0; i < m_numElements; ++i)
  {
    m_elements[i] = (T)other.element(i);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
template <typename Q>
NM_INLINE void Vector<T>::get(Q* data) const
{
  for (uint32_t i = 0; i < m_numElements; ++i)
  {
    data[i] = (Q)m_elements[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
template <typename Q>
NM_INLINE void Vector<T>::set(const Q* data)
{
  for (uint32_t i = 0; i < m_numElements; ++i)
  {
    m_elements[i] = (T)data[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE Vector<T> operator * (T s, const Vector<T>& v)
{
  return v * s;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE T Dot(const Vector<T>& v1, const Vector<T>& v2)
{
  return v1.dot(v2);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
