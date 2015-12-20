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
#if (!defined(NMTL_VECTOR_ITERATOR_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_VECTOR_ITERATOR_H
#endif

//----------------------------------------------------------------------------------------------------------------------
#include "nmtl/type_utils.h"

#ifndef NMTL_TEMPLATE_EXPORT
  #define NMTL_TEMPLATE_EXPORT
  #define NMTL_KILL_TEMPLATE_EXPORT
#endif
#ifndef NMTL_NAMESPACE_OVERRIDE
  #define NMTL_NAMESPACE nmtl
#else
  #define NMTL_NAMESPACE NMTL_NAMESPACE_OVERRIDE
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMTL_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
template<typename T> class vector;
template<typename T> class pod_vector;

//----------------------------------------------------------------------------------------------------------------------
/// \class   NMTL::_Vector_const_iterator
/// \brief   defines a constant iterator type
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
class NMTL_TEMPLATE_EXPORT vector_const_iterator
{
public:
  //--------------------------------------------------------------------------------------
  // const iterator typedefs
  //--------------------------------------------------------------------------------------

  typedef T value_type;
  typedef vector_const_iterator<value_type> my_type;
#ifdef WIN32
  typedef ptrdiff_t difference_type;
#else
  typedef unsigned long difference_type;
#endif
  typedef size_t size_type;
  typedef value_type* pointer;
  typedef value_type& reference;
  typedef const value_type* const_pointer;
  typedef const value_type& const_reference;

  friend class vector<T>;
  friend class pod_vector<T>;

  //--------------------------------------------------------------------------------------
  // const iterator construction
  //--------------------------------------------------------------------------------------

  /// construct with null pointer
  NMTL_INLINE vector_const_iterator()
  {
    m_ptr = 0;
  }

  /// construct with pointer ptr
  NMTL_INLINE vector_const_iterator(pointer ptr)
  {
    m_ptr = ptr;
  }

  //--------------------------------------------------------------------------------------
  // const iterator pointer access
  //--------------------------------------------------------------------------------------

  /// return designated object
  NMTL_INLINE reference operator*() const
  {
    NMTL_ASSERT(m_ptr);
    return (*m_ptr);
  }

  /// return pointer to class object
  NMTL_INLINE pointer operator->() const
  {
    return m_ptr;
  }

  NMTL_INLINE pointer ptr() const
  {
    return m_ptr;
  }

  //--------------------------------------------------------------------------------------
  // const iterator increment / decrement operators
  //--------------------------------------------------------------------------------------

  /// pre-increment
  NMTL_INLINE my_type& operator++()
  {
    ++m_ptr;
    return (*this);
  }

  /// post-increment
  NMTL_INLINE my_type operator++(int)
  {
    my_type tmp = *this;
    ++m_ptr;
    return (tmp);
  }

  /// pre-decrement
  NMTL_INLINE my_type& operator--()
  {
    --m_ptr;
    return (*this);
  }

  /// post-decrement
  NMTL_INLINE my_type operator--(int)
  {
    my_type tmp = *this;
    --m_ptr;
    return (tmp);
  }

  //--------------------------------------------------------------------------------------
  // const iterator += / -= operators
  //--------------------------------------------------------------------------------------

  /// increment by integer
  NMTL_INLINE my_type& operator+=(difference_type offset)
  {
    m_ptr += offset;
    return (*this);
  }

  /// decrement by integer
  NMTL_INLINE my_type& operator-=(difference_type offset)
  {
    return (*this += (-offset));
  }

  //--------------------------------------------------------------------------------------
  // const iterator + / - operators
  //--------------------------------------------------------------------------------------

  /// return this + integer
  NMTL_INLINE my_type operator+(difference_type offset) const
  {
    my_type tmp = *this;
    return (tmp += offset);
  }

  /// return this - integer
  NMTL_INLINE my_type operator-(difference_type offset) const
  {
    my_type tmp = *this;
    return (tmp -= offset);
  }

  /// return difference of iterators
  NMTL_INLINE difference_type operator-(const my_type& right) const
  {
    return (m_ptr - right.m_ptr);
  }

  //--------------------------------------------------------------------------------------
  // const iterator [] operator
  //--------------------------------------------------------------------------------------

  /// subscript
  NMTL_INLINE reference operator[](difference_type offset) const
  {
    return (*(*this + offset));
  }

  //--------------------------------------------------------------------------------------
  // const iterator comparison operators
  //--------------------------------------------------------------------------------------

  /// test for iterator equality
  NMTL_INLINE bool operator==(const my_type& right) const
  {
    return (m_ptr == right.m_ptr);
  }

  /// test for iterator inequality
  NMTL_INLINE bool operator!=(const my_type& right) const
  {
    return (!(*this == right));
  }

  /// test if this < _Right
  NMTL_INLINE bool operator<(const my_type& right) const
  {
    return (m_ptr < right.m_ptr);
  }

  /// test if this > _Right
  NMTL_INLINE bool operator>(const my_type& right) const
  {
    return (right < *this);
  }

  /// test if this <= _Right
  NMTL_INLINE bool operator<=(const my_type& right) const
  {
    return (!(right < *this));
  }

  /// test if this >= _Right
  NMTL_INLINE bool operator>=(const my_type& right) const
  {
    return (!(*this < right));
  }

protected:
  /// offset of element in vector
  pointer m_ptr;
};

//----------------------------------------------------------------------------------------------------------------------
/// add offset to iterator
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<class T>
NMTL_INLINE vector_const_iterator<T> operator+(ptrdiff_t offset, vector_const_iterator<T> next)
{
  return (next += offset);
}

//----------------------------------------------------------------------------------------------------------------------
/// \class   NMTL::_Vector_iterator
/// \brief   defines the non const iterator type
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
class NMTL_TEMPLATE_EXPORT vector_iterator :
  public vector_const_iterator<T>
{
public:
  typedef T value_type;
  typedef vector_iterator<T> my_type;
  typedef vector_const_iterator<T> my_base;
  typedef typename vector_const_iterator<T>::difference_type difference_type;
  typedef typename vector_const_iterator<T>::pointer pointer;
  typedef typename vector_const_iterator<T>::reference reference;

  //--------------------------------------------------------------------------------------
  // mutable iterator ctors
  //--------------------------------------------------------------------------------------

  /// construct with null vector pointer
  NMTL_INLINE vector_iterator() :
    my_base()
  {
  }

  /// construct with pointer ptr
  NMTL_INLINE vector_iterator(pointer ptr) :
    my_base(ptr)
  {
  }

  //--------------------------------------------------------------------------------------
  // mutable iterator pointer access
  //--------------------------------------------------------------------------------------

  /// return designated object
  NMTL_INLINE reference operator*() const
  {
    return ((reference)** (my_base*)this);
  }

  /// return pointer to class object
  NMTL_INLINE pointer operator->() const
  {
    return (&** this);
  }

  /// subscript
  NMTL_INLINE reference operator[](difference_type offset) const
  {
    return (*(*this + offset));
  }

  NMTL_INLINE pointer ptr() const
  {
    return vector_const_iterator<T>::m_ptr;
  }

  //--------------------------------------------------------------------------------------
  // mutable iterator ++ / -- operators
  //--------------------------------------------------------------------------------------

  /// preincrement
  NMTL_INLINE my_type& operator++()
  {
    ++(*(my_base*)this);
    return (*this);
  }

  /// postincrement
  NMTL_INLINE my_type operator++(int)
  {
    my_type tmp = *this;
    ++*this;
    return (tmp);
  }

  /// predecrement
  NMTL_INLINE my_type& operator--()
  {
    --(*(my_base*)this);
    return (*this);
  }

  /// postdecrement
  NMTL_INLINE my_type operator--(int)
  {
    my_type tmp = *this;
    --*this;
    return (tmp);
  }

  //--------------------------------------------------------------------------------------
  // mutable iterator ++ / -= operators
  //--------------------------------------------------------------------------------------

  /// increment by integer
  NMTL_INLINE my_type& operator+=(difference_type offset)
  {
    (*(my_base*)this) += offset;
    return (*this);
  }

  /// decrement by integer
  NMTL_INLINE my_type& operator-=(difference_type offset)
  {
    return (*this += -offset);
  }

  //--------------------------------------------------------------------------------------
  // mutable iterator + / - operators
  //--------------------------------------------------------------------------------------

  /// return this + integer
  NMTL_INLINE my_type operator+(difference_type offset) const
  {
    my_type tmp = *this;
    return (tmp += offset);
  }

  /// return this - integer
  NMTL_INLINE my_type operator-(difference_type offset) const
  {
    my_type tmp = *this;
    return (tmp -= offset);
  }

  /// return difference of iterators
  NMTL_INLINE difference_type operator-(const my_base& right) const
  {
    return (*(my_base*)this - right);
  }

#ifdef __INTEL_COMPILER
  #undef value_type
  #undef my_type
  #undef my_base
  #undef difference_type
  #undef pointer
  #undef reference
#endif
};

//----------------------------------------------------------------------------------------------------------------------
/// add offset to iterator
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
NMTL_INLINE vector_iterator<T> operator+(ptrdiff_t offset, vector_iterator<T> next)
{
  return (next += offset);
}

//----------------------------------------------------------------------------------------------------------------------
/// \class   NMTL::vector_const_reverse_iterator
/// \brief   defines a const reverse iterator type
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
class NMTL_TEMPLATE_EXPORT vector_const_reverse_iterator :
  public vector_const_iterator<T>
{
public:
  typedef T value_type;
  typedef vector_const_reverse_iterator<T> my_type;
  typedef vector_const_iterator<T> my_base;

  typedef typename vector_const_iterator<T>::difference_type difference_type;
  typedef typename vector_const_iterator<T>::pointer pointer;
  typedef typename vector_const_iterator<T>::reference reference;

  NMTL_INLINE vector_const_reverse_iterator() :
    my_base()
  {
  }

  /// construct with pointer ptr
  NMTL_INLINE vector_const_reverse_iterator(my_base ptr) :
    my_base(ptr)
  {
  }

  //--------------------------------------------------------------------------------------
  // const iterator pointer access
  //--------------------------------------------------------------------------------------

  /// return designated object
  NMTL_INLINE reference operator*() const
  {
    NMTL_ASSERT(vector_const_iterator<T>::m_ptr);
    pointer tmp = vector_const_iterator<T>::m_ptr;
    return (*--tmp);
  }

  /// return pointer to class object
  NMTL_INLINE pointer operator->() const
  {
    NMTL_ASSERT(vector_const_iterator<T>::m_ptr);
    pointer tmp = vector_const_iterator<T>::m_ptr;
    return --tmp;
  }

  NMTL_INLINE pointer ptr() const
  {
    if (vector_const_iterator<T>::m_ptr)
    {
      pointer tmp = vector_const_iterator<T>::m_ptr;
      return --tmp;
    }
    return 0;
  }

  //--------------------------------------------------------------------------------------
  // const iterator increment / decrement operators
  //--------------------------------------------------------------------------------------

  /// pre-increment
  NMTL_INLINE my_type& operator++()
  {
    --vector_const_iterator<T>::m_ptr;
    return (*this);
  }

  /// post-increment
  NMTL_INLINE my_type operator++(int)
  {
    my_type tmp = *this;
    --vector_const_iterator<T>::m_ptr;
    return (tmp);
  }

  /// pre-decrement
  NMTL_INLINE my_type& operator--()
  {
    ++vector_const_iterator<T>::m_ptr;
    return (*this);
  }

  /// post-decrement
  NMTL_INLINE my_type operator--(int)
  {
    my_type tmp = *this;
    ++vector_const_iterator<T>::m_ptr;
    return (tmp);
  }

  //--------------------------------------------------------------------------------------
  // mutable iterator ++ / -= operators
  //--------------------------------------------------------------------------------------

  /// increment by integer
  NMTL_INLINE my_type& operator+=(difference_type offset)
  {
    (*(my_base*)this) += -offset;
    return (*this);
  }

  /// decrement by integer
  NMTL_INLINE my_type& operator-=(difference_type offset)
  {
    (*(my_base*)this) += offset;
    return (*this);
  }

  //--------------------------------------------------------------------------------------
  // mutable iterator + / - operators
  //--------------------------------------------------------------------------------------
  /// return this + integer
  NMTL_INLINE my_type operator+(difference_type offset) const
  {
    my_type tmp = *this;
    return (tmp += offset);
  }

  /// return this - integer
  NMTL_INLINE my_type operator-(difference_type offset) const
  {
    my_type tmp = *this;
    return (tmp -= offset);
  }

  /// return difference of iterators
  NMTL_INLINE difference_type operator-(const my_base& right) const
  {
    return (*(my_base*)this - right);
  }
};

//----------------------------------------------------------------------------------------------------------------------
/// \class   NMTL::vector_reverse_iterator
/// \brief   defines a reverse iterator type
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
class NMTL_TEMPLATE_EXPORT vector_reverse_iterator :
  public vector_const_iterator<T>
{
public:
  typedef T value_type;
  typedef vector_reverse_iterator<T> my_type;
  typedef vector_const_iterator<T> my_base;

  typedef typename vector_const_iterator<T>::difference_type difference_type;
  typedef typename vector_const_iterator<T>::pointer pointer;
  typedef typename vector_const_iterator<T>::reference reference;

  NMTL_INLINE vector_reverse_iterator() :
    my_base()
  {
  }

  /// construct with pointer ptr
  NMTL_INLINE vector_reverse_iterator(my_base ptr) :
    my_base(ptr)
  {
  }

  //--------------------------------------------------------------------------------------
  // const iterator pointer access
  //--------------------------------------------------------------------------------------

  /// return designated object
  NMTL_INLINE reference operator*() const
  {
    NMTL_ASSERT(vector_const_iterator<T>::m_ptr);
    pointer tmp = vector_const_iterator<T>::m_ptr;
    return (*--tmp);
  }

  /// return pointer to class object
  NMTL_INLINE pointer operator->() const
  {
    NMTL_ASSERT(vector_const_iterator<T>::m_ptr);
    pointer tmp = vector_const_iterator<T>::m_ptr;
    return --tmp;
  }

  NMTL_INLINE pointer ptr() const
  {
    if (vector_const_iterator<T>::m_ptr)
    {
      pointer tmp = vector_const_iterator<T>::m_ptr;
      return --tmp;
    }
    return 0;
  }

  //--------------------------------------------------------------------------------------
  // const iterator increment / decrement operators
  //--------------------------------------------------------------------------------------

  /// pre-increment
  NMTL_INLINE my_type& operator++()
  {
    --vector_const_iterator<T>::m_ptr;
    return (*this);
  }

  /// post-increment
  NMTL_INLINE my_type operator++(int)
  {
    my_type tmp = *this;
    --vector_const_iterator<T>::m_ptr;
    return (tmp);
  }

  /// pre-decrement
  NMTL_INLINE my_type& operator--()
  {
    ++vector_const_iterator<T>::m_ptr;
    return (*this);
  }

  /// post-decrement
  NMTL_INLINE my_type operator--(int)
  {
    my_type tmp = *this;
    ++vector_const_iterator<T>::m_ptr;
    return (tmp);
  }

  //--------------------------------------------------------------------------------------
  // mutable iterator ++ / -= operators
  //--------------------------------------------------------------------------------------

  /// increment by integer
  NMTL_INLINE my_type& operator+=(difference_type offset)
  {
    (*(my_base*)this) += -offset;
    return (*this);
  }

  /// decrement by integer
  NMTL_INLINE my_type& operator-=(difference_type offset)
  {
    (*(my_base*)this) += offset;
    return (*this);
  }

  //--------------------------------------------------------------------------------------
  // mutable iterator + / - operators
  //--------------------------------------------------------------------------------------

  /// return this + integer
  NMTL_INLINE my_type operator+(difference_type offset) const
  {
    my_type tmp = *this;
    return (tmp += offset);
  }

  /// return this - integer
  NMTL_INLINE my_type operator-(difference_type offset) const
  {
    my_type tmp = *this;
    return (tmp -= offset);
  }

  /// return difference of iterators
  NMTL_INLINE difference_type operator-(const my_base& right) const
  {
    return (*(my_base*)this - right);
  }
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_VECTOR_ITERATOR_H
