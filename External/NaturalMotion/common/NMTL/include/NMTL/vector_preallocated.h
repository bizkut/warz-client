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
#if (!defined(NMTL_VECTOR_PREALLOCATED_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_VECTOR_PREALLOCATED_H
#endif

//----------------------------------------------------------------------------------------------------------------------
#include <functional>
#include <algorithm>
#include <stddef.h>
#ifndef NMTL_NO_SENTRY
  #include "nmtl/vector_iterators.h"
#endif

#include "nmtl/container_destroy_method.h"

#ifndef NMTL_TEMPLATE_EXPORT
  #define NMTL_TEMPLATE_EXPORT
  #define NMTL_KILL_TEMPLATE_EXPORT
#endif

#ifndef NMTL_NAMESPACE_OVERRIDE
  #define NMTL_NAMESPACE nmtl
#else
  #define NMTL_NAMESPACE NMTL_NAMESPACE_OVERRIDE
#endif

#ifndef NMTL_INDEX
  #define NMTL_INDEX
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace NMTL_NAMESPACE
{


//----------------------------------------------------------------------------------------------------------------------
/// \class   NMTL::vector_preallocated
/// \brief   defines a templated array container, which contains a preallocated array of type T, this allows
///          the standard case to not use any dynamically allocated memory, expanding into the heap as required.
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<typename T, size_t PREALLOC>
class NMTL_TEMPLATE_EXPORT vector_preallocated
{
public:
  //--------------------------------------------------------------------------------------
  // vector typedefs
  //--------------------------------------------------------------------------------------

  typedef vector_preallocated<T, PREALLOC> my_type;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef T* pointer;
  typedef const T* const_pointer;
  typedef T& reference;
  typedef const T& const_reference;
  typedef T value_type;
  typedef vector_iterator<T> iterator;
  typedef vector_const_iterator<T> const_iterator;
  typedef vector_reverse_iterator<T> reverse_iterator;
  typedef vector_const_reverse_iterator<T> const_reverse_iterator;

  //--------------------------------------------------------------------------------------
  // vector constructors
  //--------------------------------------------------------------------------------------

  /// \brief  construct empty vector
  NMTL_INLINE vector_preallocated(nmtl::allocator* allocator_ = NMTL_DEFAULT_ALLOCATOR) :
    m_first(m_preallocated), m_last(m_preallocated + PREALLOC), m_end(m_preallocated), m_allocator(allocator_)
  {
  }

  /// \brief  construct empty vector
  /// \param  count - num to allocate
  /// \param  val - default value for new elements
  NMTL_INLINE vector_preallocated(
    size_type count, const T& val = T(),
    nmtl::allocator* allocator_ = NMTL_DEFAULT_ALLOCATOR) :
    m_first(m_preallocated), m_last(m_preallocated + PREALLOC), m_end(m_preallocated), m_allocator(allocator_)
  {
    resize(count, val);
  }

  /// \brief  copy ctor
  /// \param  right - the value to copy
  NMTL_INLINE vector_preallocated(const my_type& right) :
    m_first(m_preallocated), m_last(m_preallocated + PREALLOC), m_end(m_preallocated), m_allocator(right.m_allocator)
  {
    reserve(right.size());
    m_end = m_first + right.size();
    copy(right.begin(), right.end(), begin());
  }

  /// \brief  construct vector from iterator range in another vector
  /// \param  start - start iterator in copy range
  /// \param  finish - end iterator in copy range
  NMTL_INLINE vector_preallocated(
    const_iterator start, const_iterator finish,
    nmtl::allocator* allocator_ = NMTL_DEFAULT_ALLOCATOR) :
    m_first(m_preallocated), m_last(m_preallocated + PREALLOC), m_end(m_preallocated), m_allocator(allocator_)
  {
    assign(start, finish);
  }

  /// \brief  dtor
  NMTL_INLINE ~vector_preallocated()
  {
    // destroy the object
    if (m_first)
    {
      destroy(begin(), end());

      if(m_first != m_preallocated)
      {
        m_allocator->free(m_first);
      }
    }
  }

  //--------------------------------------------------------------------------------------
  // vector assignment
  //--------------------------------------------------------------------------------------

  /// \brief  assignment operator
  /// \param  right - the vector<> to copy
  /// \return reference to this
  NMTL_INLINE my_type& operator=(const my_type& right)
  {
    // assign right
    if (this != &right)
    {
      clear();
      resize(right.size());
      copy(right.begin(), right.end(), begin());
    }
    return (*this);
  }

  /// \brief  assign empty vector
  /// \param  count - num to allocate
  /// \param  val - default value for new elements
  NMTL_INLINE void assign(size_type count, const T& val)
  {
    resize(count, val);
  }

  /// \brief  assign vector from iterator range in another vector
  /// \param  start - start iterator in copy range
  /// \param  finish - end iterator in copy range
  NMTL_INLINE void assign(const_iterator start, const_iterator finish)
  {
    reserve(finish - start);
    m_end = m_first + (finish - start);
    copy(start, finish, begin());
  }

  /// \brief  swaps two vectors - if they have the same allocator then it is
  /// efficient, if not it copies via a temporary variable
  NMTL_INLINE void swap(my_type& rhs)
  { // exchange contents with rhs
    if (this == &rhs)
    {
      // same object, do nothing
    }
    else if (m_allocator == rhs.m_allocator &&
      m_first != m_preallocated &&
      rhs.m_first != rhs.m_preallocated)
    { // same allocator, swap control information
      std::swap(m_first, rhs.m_first);
      std::swap(m_last, rhs.m_last);
      std::swap(m_end, rhs.m_end);
    }
    else
    { // different allocator, do multiple assigns
      my_type temp = *this;
      *this = rhs;
      rhs = temp;
    }
  }

  /// \brief  reserves the specified amount of memory
  /// \param  count - the number of elements to reserve in the array
  NMTL_INLINE void reserve(size_type count)
  {
    size_t old_size = (m_first == 0 ? 0 : m_end - m_first);
    if (capacity() < count)
    {
      size_t old_capacity = capacity();
      size_t new_capacity = old_capacity + (old_capacity >> 1);
      if (new_capacity < count)
      {
        new_capacity = count;
      }
      pointer temp = (pointer)m_allocator->alloc(sizeof(value_type) * new_capacity);
      if (temp && m_first)
      {
        for (size_t s = 0; s < old_size; ++s)
        {
          // re-assign using copy ctor.
          pointer temp_plus_s = temp + s;
          create(temp_plus_s, m_first[s]);
          // call dtor as we destroy previous array
          destroy(m_first + s);
        }
      }

      if (m_first && m_first != m_preallocated)
      {
        m_allocator->free(m_first);
      }

      m_first = temp;
      m_last = m_first + new_capacity;
      m_end = m_first + old_size;
    }
    else if (count == 0 && m_first)
    {
      destroy(begin(), end());

      if (m_first != m_preallocated)
      {
        m_allocator->free(m_first);
      }

      m_first = m_end = m_preallocated;
      m_last = m_preallocated + PREALLOC;
    }
  }

  /// \brief  return current length of allocated storage
  /// \return the capacity of the vector
  NMTL_INLINE size_type capacity() const
  {
    return (m_first == 0 ? 0 : m_last - m_first);
  }

  /// \brief   return iterator for beginning of mutable sequence
  /// \return  returns an iterator to the start of the data
  NMTL_INLINE iterator begin()
  {
    return (iterator(m_first));
  }

  /// \brief   return iterator for beginning of non-mutable sequence
  /// \return  returns an iterator to the start of the data
  NMTL_INLINE const_iterator begin() const
  {
    return (const_iterator(m_first));
  }

  /// \brief   return iterator for the end of mutable sequence
  /// \return  returns an iterator to the end of the data
  NMTL_INLINE iterator end()
  {
    return (iterator(m_end));
  }

  /// \brief   return iterator for the end of non-mutable sequence
  /// \return  returns an iterator to the end of the data
  NMTL_INLINE const_iterator end() const
  {
    return (const_iterator(m_end));
  }

  /// \brief   return reverse iterator for beginning of mutable sequence
  /// \return  returns a reverse iterator to the start of the data
  NMTL_INLINE reverse_iterator rbegin()
  {
    return (reverse_iterator(m_end));
  }

  /// \brief   return reverse iterator for beginning of non-mutable sequence
  /// \return  returns a reverse iterator to the start of the data
  NMTL_INLINE const_reverse_iterator rbegin() const
  {
    return (const_reverse_iterator(m_end));
  }

  /// \brief   return reverse iterator for the end of mutable sequence
  /// \return  returns a reverse iterator to the end of the data
  NMTL_INLINE reverse_iterator rend()
  {
    return (reverse_iterator(m_first));
  }

  /// \brief   return reverse iterator for the end of non-mutable sequence
  /// \return  returns a reverse iterator to the end of the data
  NMTL_INLINE const_reverse_iterator rend() const
  {
    return (const_reverse_iterator(m_first));
  }

  /// \brief  resizes the array to the specified size
  /// \param  new_size - the new size of the array
  NMTL_INLINE void resize(size_type new_size)
  {
    size_type old_size = size();

    if (old_size == new_size)
    {
      return;
    }

    if (new_size > old_size)
    {
      reserve(new_size);
      create(m_first + old_size, m_first + new_size);
    }
    else if (new_size < old_size)
    {
      destroy(m_first + new_size, m_first + old_size);
    }

    m_end = m_first + new_size;
  }

  /// \brief  resizes the array to the specified size
  /// \param  new_size - the new size of the array
  /// \param  val - the value to insert into any new elements added
  NMTL_INLINE void resize(size_type new_size, const T& val)
  {
    size_type old_size = size();

    if (old_size == new_size)
    {
      return;
    }

    if (new_size > old_size)
    {
      reserve(new_size);
      create(m_first + old_size, m_first + new_size, val);
    }
    else if (new_size < old_size)
    {
      destroy(m_first + new_size, m_first + old_size);
    }

    m_end = m_first + new_size;
  }

  /// \brief  returns length of sequence
  /// \return the size of the array
  NMTL_INLINE size_type size() const
  {
    return (m_first == 0 ? 0 : m_end - m_first);
  }

  /// \brief  returns true if the array is currently empty
  /// \return true if the array is empty
  NMTL_INLINE bool empty() const
  {
    return (size() == 0);
  }

  /// \brief  returns a reference to the specified element in the array
  /// \param  pos - the index of the element to return
  NMTL_INLINE const_reference at(NMTL_INDEX size_type pos) const
  {
    NMTL_ASSERT(pos < size());
    return (*(begin() + pos));
  }

  /// \brief  returns a reference to the specified element in the array
  /// \param  pos - the index of the element to return
  NMTL_INLINE reference at(NMTL_INDEX size_type pos)
  {
    NMTL_ASSERT(pos < size());
    return (*(begin() + pos));
  }

  /// \brief  subscript non-mutable sequence
  /// \param  pos - the index of the element to return
  NMTL_INLINE const_reference operator[](size_type pos) const
  {
    NMTL_ASSERT(pos < size());
    return (*(m_first + pos));
  }

  /// \brief  subscript mutable sequence
  /// \param  pos - the index of the element to return
  NMTL_INLINE reference operator[](size_type pos)
  {
    NMTL_ASSERT(pos < size());
    return (*(m_first + pos));
  }

  /// \brief  return first element of mutable sequence
  NMTL_INLINE reference front()
  {
    return (*begin());
  }

  /// \brief  return first element of non-mutable sequence
  NMTL_INLINE const_reference front() const
  {
    return (*begin());
  }

  /// \brief  return last element of mutable sequence
  NMTL_INLINE reference back()
  {
    return (*(end() - 1));
  }

  /// \brief  return last element of non-mutable sequence
  NMTL_INLINE const_reference back() const
  {
    return (*(end() - 1));
  }

  /// \brief  insert element at end
  /// \param  val - the value of the new element
  NMTL_INLINE void push_back(const T& val)
  {
    insert(end(), val);
  }

  /// \brief  erase element at end
  NMTL_INLINE void pop_back()
  {
    if (!empty())
    {
      erase(end() - 1);
    }
  }

  /// \brief  insert element at the front
  /// \param  val - the value of the new element
  NMTL_INLINE void push_front(const T& val)
  {
    insert(begin(), val);
  }

  /// \brief  erase an element from the end
  NMTL_INLINE void pop_front()
  {
    if (!empty())
    {
      erase(begin());
    }
  }

  /// \brief  inserts a new element into the array at the specified location
  /// \param  location - the location to insert the new element
  /// \param  val - the value to insert
  /// \return an iterator to the newly added element
  NMTL_INLINE iterator insert(iterator location, const T& val)
  {
    return insert(location, 1, val);
  }

  /// \brief  inserts a range of element into the array at the specified location
  /// \param  location - the location to insert the new element
  /// \param  start - start iterator in copy range
  /// \param  finish - end iterator in copy range
  /// \return an iterator to end of the newly added element
  NMTL_INLINE iterator insert(iterator location, const_iterator start, const_iterator finish)
  {
    ptrdiff_t w0 = location - m_first;
    ptrdiff_t w1 = m_end - m_first;
    ptrdiff_t w2 = finish - start;

    // add 1 to array size
    if ((size() + w2) >= capacity())
    {
      reserve(size() + w2);
    }

    location = m_first + w0;
    iterator location2 = location + w2;

    // move elements after _Where back 1 in array
    if (location < end())
    {
      copy(location, m_end, location2);
      m_end += w2;
    }

    // (need to use placement new & the copy constructor for this)
    for (ptrdiff_t i = 0; i != w2; ++i)
    {
      create((location + i).ptr(), start[i]);
    }

    m_end = m_first + w1 + w2;
    return location;
  }

  /// \brief  inserts a number of new elements into the array at the specified location
  /// \param  location - the location to insert the new element
  /// \param  count - the number of elements to insert
  /// \param  val - the value to insert
  /// \return an iterator to the newly added elements
  NMTL_INLINE iterator insert(iterator location, size_type count, const T& val)
  {
    ptrdiff_t w0 = location - m_first;
    ptrdiff_t w1 = m_end - m_first;

    // add 1 to array size
    if ((size() + count) >= capacity())
    {
      reserve(size() + count);
    }

    location = m_first + w0;
    iterator _Where2 = location + count;

    // move elements after _Where back 1 in array
    if (location < end())
    {
      copy(location, m_end, _Where2);
    }

    // (need to use placement new & the copy constructor for this)
    for (size_type i = 0; i != count; ++i)
    {
      create((location + i).ptr(), val);
    }

    m_end = m_first + w1 + count;

    return location;
  }

  /// \brief  erases the element at the specified location
  /// \param  location - the element to erase
  /// \return the position of the deleted memory
  NMTL_INLINE iterator erase(iterator location)
  {
    NMTL_ASSERT((location >= m_first) && (location < m_end));
    return erase(location, location + 1);
  }

  /// \brief  erases a sequence of elements at the specified location
  /// \param  first - the start of the sequence to delete
  /// \param  last - the end of the sequence to delete
  /// \return the position of the deleted memory
  NMTL_INLINE iterator erase(iterator first, iterator last)
  {
    // check iterator range
    NMTL_ASSERT((first >= m_first) && (first <= m_end));
    NMTL_ASSERT((last >= m_first) && (last <= m_end));

    // first call destructors on the elements we are going to erase
    destroy(first, last);

    // now call copy constructors on any elements we are going to move about in memory
    if (last < m_end)
    {
      copy(last, m_end, first);
    }

    m_end -= (last - first);

    //
    // now reset end pointer. I'm going to be lazy and not actually
    // free any memory unless resize() is explicitly called. This
    // should help with performance a little bit for very dynamic arrays
    // (eg particle systems).
    //
    return first;
  }

  /// \brief  copies a sequence of elements between first and last into dst
  /// \param  first - the start of the sequence of elements to copy
  /// \param  last - the end of the sequence to copy
  /// \param  dst - where to copy the data to
  NMTL_INLINE void copy(const_iterator first, const_iterator last, iterator dst)
  {
    NMTL_ASSERT(first <= last);

    iterator dst_end = dst + (last - first);
    if (((first < dst_end) && (last > dst_end)))
    {
      for (; first != last; ++first, ++dst)
      {
        create(dst.ptr(), (*first));
      }
    }
    else
    {
      if (((first < dst) && (last > dst)))
      {
        for (; dst < dst_end; --dst_end)
        {
          --last;
          iterator dst = dst_end - 1;
          create(dst.ptr(), (*last));
        }
      }
      else
      {
        for (; first != last; ++first, ++dst)
        {
          create(dst.ptr(), (*first));
        }
      }
    }
  }

  /// \brief  clears (and destructs) all elements in the array, but does not alter capacity
  NMTL_INLINE void clear()
  {
    if (m_first)
    {
      destroy(begin(), end());
    }
    m_end = m_first;
  }

  NMTL_INLINE pointer buffer()
  {
    return m_first;
  }

  NMTL_INLINE const_pointer buffer() const
  {
    return m_first;
  }

protected:
  /// manually calls the destructor on ptr
  NMTL_INLINE void destroy(iterator ptr)
  {
    nmtl::__destroy_method<T, iterator, nmtl::is_class<T>::result>::destroy(ptr);
  }

  /// manually calls the destructor on all objects between ptr and ptrend
  NMTL_INLINE void destroy(iterator ptr, iterator ptrend)
  {
    for (; ptr != ptrend; ++ptr)
    {
      nmtl::__destroy_method<T, iterator, nmtl::is_class<T>::result>::destroy(ptr);
    }
  }

  /// performs a placement new on the memory at ptr
  NMTL_INLINE void create(T* ptr)
  {
    void* vptr = ptr;
    ::new (vptr) T;
  }

  /// performs a placement new on the objects between ptr and ptrend
  NMTL_INLINE void create(T* ptr, T* ptrend)
  {
    for (; ptr != ptrend; ++ptr)
    {
      void* vptr = ptr;
      ::new (vptr) T;
    }
  }

  /// performs a placement new on ptr, this time calling the copy ctor
  NMTL_INLINE void create(T* ptr, const T& rhs)
  {
    void* vptr = ptr;
    ::new (vptr) T(rhs);
  }

  /// performs a placement new on the objects between ptr and ptrend,
  /// this time calling the copy ctor
  NMTL_INLINE void create(T* ptr, T* ptrend, const T& rhs)
  {
    for (; ptr != ptrend; ++ptr)
    {
      void* vptr = ptr;
      ::new (vptr) T(rhs);
    }
  }

protected:

  /// pointer to beginning of array
  pointer m_first;

  /// pointer to current end of sequence
  pointer m_last;

  /// pointer to end of array
  pointer m_end;

  T m_preallocated[PREALLOC];

  /// the internal allocator
  nmtl::allocator* m_allocator;
};

//----------------------------------------------------------------------------------------------------------------------
template<typename value_type, size_t SIZE_A, size_t SIZE_B>
NMTL_INLINE bool operator ==(const vector_preallocated<value_type, SIZE_A>& lhs, const vector_preallocated<value_type, SIZE_B>& rhs)
{
  if (lhs.size() != rhs.size())
  {
    return false;
  }

  typename vector_preallocated<value_type, SIZE_A>::const_iterator ita = lhs.begin();
  typename vector_preallocated<value_type, SIZE_B>::const_iterator itb = rhs.begin();
  for (; ita != lhs.end(); ++ita, ++itb)
  {
    if (!(*ita == *itb))
    {
      return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename value_type, size_t SIZE_A, size_t SIZE_B>
NMTL_INLINE bool operator !=(const vector_preallocated<value_type, SIZE_A>& lhs, const vector_preallocated<value_type, SIZE_B>& rhs)
{
  return !(lhs == rhs);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_VECTOR_PREALLOCATED_H
