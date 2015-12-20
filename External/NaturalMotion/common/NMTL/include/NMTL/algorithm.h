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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef NMTL_ALGORITHM_H
#define NMTL_ALGORITHM_H

//----------------------------------------------------------------------------------------------------------------------
#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
namespace nmtl
{

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type>
inline bool binary_search(iter_type<value_type> start, iter_type<value_type> end, const value_type& value)
{
  typename iter_type<value_type>::pointer s = start.ptr();
  typename iter_type<value_type>::pointer e = end.ptr();
  if (s && e)
  {
    return std::binary_search(s, e, value);
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// Vector sort operations
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<typename value_type>
inline void sort(value_type* start, value_type* end)
{
  std::sort(start, end);
}

//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type>
inline void sort(iter_type<value_type> start, iter_type<value_type> end)
{
  typename iter_type<value_type>::pointer s = start.ptr();
  typename iter_type<value_type>::pointer e = end.ptr();
  if (s && e)
  {
    std::sort(s, e);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type, typename Predicate>
inline void sort(iter_type<value_type> start, iter_type<value_type> end, Predicate predicate)
{
  value_type* s = start.ptr();
  typename iter_type<value_type>::pointer e = end.ptr();
  if (s && e)
  {
    std::sort(s, e, predicate);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template<typename value_type>
inline void stable_sort(value_type* start, value_type* end)
{
  std::stable_sort(start, end);
}

//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type>
inline void stable_sort(iter_type<value_type> start, iter_type<value_type> end)
{
  typename iter_type<value_type>::pointer s = start.ptr();
  typename iter_type<value_type>::pointer e = end.ptr();
  if (s && e)
  {
    std::stable_sort(s, e);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type, typename Predicate>
inline void stable_sort(iter_type<value_type> start, iter_type<value_type> end, Predicate predicate)
{
  value_type* s = start.ptr();
  typename iter_type<value_type>::pointer e = end.ptr();
  if (s && e)
  {
    std::stable_sort(s, e, predicate);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type>
inline void fill(iter_type<value_type> start, iter_type<value_type> end, const value_type& val)
{
  value_type* s = start.ptr();
  value_type* e = end.ptr();
  if (s && e)
  {
    std::fill(s, e, val);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template<typename iter_type, typename value_type>
inline iter_type find(iter_type start, iter_type end, const value_type& val)
{
  NMTL_ASSERT(start <= end);
  for (; start != end; ++start)
  {
    if (val == *start)
    {
      break;
    }
  }
  return start;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  uses a simple binary search to locate the variable. The pod_vector *must* be sorted using the
///         < operator before calling this function (since binary searches require ordered data sets).
template<typename iter_type, typename value_type>
inline iter_type fast_find(iter_type _start, iter_type _finish, const value_type& val)
{
  iter_type start = _start;
  iter_type finish = _finish;
  ptrdiff_t inc = 100;
  while (start != finish && inc != 0)
  {
    ptrdiff_t inc = (finish - start) / 2;
    iter_type mid    = start + inc;
    const value_type& midValue = *mid;

    if (midValue == val)
    {
      return mid;
    }
    else if (val < midValue)
    {
      finish = mid;
    }
    else
    {
      start = mid;
    }
  }
  return finish;
}

//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type>
inline iter_type<value_type> remove(iter_type<value_type> start, iter_type<value_type> end, const value_type& val)
{
  value_type* s = start.ptr();
  value_type* e = end.ptr();
  if (s && e)
  {
    return std::remove(s, e, val);
  }
  return end;
}

//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type>
inline void reverse(iter_type<value_type> start, iter_type<value_type> end)
{
  value_type* s = start.ptr();
  value_type* e = end.ptr();
  if (s && e)
  {
    std::reverse(s, e);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type, typename Predicate>
inline iter_type<value_type> remove_if(iter_type<value_type> start, iter_type<value_type> end, Predicate predicate)
{
  value_type* s = start.ptr();
  value_type* e = end.ptr();
  if (s && e)
  {
    return std::remove_if(s, e, predicate);
  }
  return end;
}

//----------------------------------------------------------------------------------------------------------------------
/// Vector heap operations
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<typename value_type>
inline void push_heap(value_type* start, value_type* end)
{
  std::push_heap(start, end);
}

//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type>
inline void push_heap(iter_type<value_type> start, iter_type<value_type> end)
{
  if (start != end)
  {
    std::push_heap(&(*start), &(*end));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type, typename _Pr>
inline void push_heap(iter_type<value_type> start, iter_type<value_type> end, _Pr pred)
{
  if (start != end)
  {
    std::push_heap(&(*start), &(*end), pred);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template<typename value_type>
inline void pop_heap(value_type* start, value_type* end)
{
  std::pop_heap(start, end);
}

//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type>
inline void pop_heap(iter_type<value_type> start, iter_type<value_type> end)
{
  if (start != end)
  {
    std::pop_heap(&(*start), &(*end));
  }
}

//----------------------------------------------------------------------------------------------------------------------
template<template<class> class iter_type, typename value_type, typename _Pr>
inline void pop_heap(iter_type<value_type> start, iter_type<value_type> end, _Pr pred)
{
  if (start != end)
  {
    std::pop_heap(&(*start), &(*end), pred);
  }
}
//----------------------------------------------------------------------------------------------------------------------
/// \brief  Returns true if the the value is in the vector
template <typename T, template<class> class vector_t>
bool vectorContains(const vector_t<T>& v, const T& value)
{
  if (v.empty())
    return false;
  return nmtl::find(v.begin(), v.end(), value) != v.end();
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Inserts the given item at the given index
template <typename T, template<class> class vector_t>
bool vectorInsertAtIndex(vector_t<T>& v, const T& value, size_t index)
{
  if (index > v.size())
    return false;

  v.insert(v.begin() + index, value);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Removes the item at the given index from the vector
template <typename T, template<class> class vector_t>
bool vectorEraseAt(vector_t<T>& v, size_t index)
{
  if (index < v.size())
  {
    v.erase(v.begin() + index);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Removes the first occurrence of value from vector
template <typename T, template<class> class vector_t>
bool vectorEraseFirst(vector_t<T>& v, const T& value)
{
  typename vector_t<T>::iterator it = nmtl::find(v.begin(), v.end(), value);

  if (it != v.end())
  {
    v.erase(it);
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Removes all occurrences of value from vector
template <typename T, template<class> class vector_t>
bool vectorEraseAll(vector_t<T>& v, const T& value)
{
  typename vector_t<T>::iterator it = nmtl::remove(v.begin(), v.end(), value);

  if (it != v.end())
  {
    v.erase(it, v.end());
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Returns the integer index of the item or -1 if not found
template <typename T, template<class> class vector_t>
int vectorGetItemIndex(const vector_t<T>& v, const T& value)
{
  int index = 0;
  typename vector_t<T>::const_iterator it = v.begin();

  for (; it != v.end(); ++it, ++index)
  {
    if (*it == value)
    {
      return index;
    }
  }

  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Returns the integer index of the item or -1 if not found
template <typename T, template<class> class vector_t>
int vectorGetItemIndex(const vector_t<T>& v, const T& value, const T& tolerance)
{
  int index = 0;
  typename vector_t<T>::const_iterator it = v.begin();

  for (; it != v.end(); ++it, ++index)
  {
    if (fabs(*it - value) < tolerance)
    {
      return index;
    }
  }

  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Appends value to vector but only if value is not already in vector.
///         Passing 'force' in removes all occurrences of value and then appends it.
template <typename T, template<class> class vector_t>
bool vectorPushBackUnique(vector_t<T>& v, const T& value, bool force = false)
{
  typename vector_t<T>::iterator it = nmtl::find(v.begin(), v.end(), value);

  if (it == v.end())
  {
    v.push_back(value);
    return true;
  }
  else if (force)
  {
    v.erase(nmtl::remove(v.begin(), v.end(), value), v.end());
    v.push_back(value);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Inserts value at start of vector
template <typename T, template<class> class vector_t>
void vectorPushFront(vector_t<T>& v, const T& value)
{
  if (v.empty())
  {
    v.push_back(value);
  }
  else
  {
    v.insert(v.begin(), value);
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Removes first element from the vector if one exists
template <typename T, template<class> class vector_t>
bool vectorPopFront(vector_t<T>& v)
{
  if (!v.empty())
  {
    v.erase(v.begin(), v.begin() + 1);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Clears vector calling delete on each element
template <typename T, template<class> class vector_t>
void vectorClearAndDeleteValues(vector_t<T>& v)
{
  for (typename vector_t<T>::iterator it = v.begin(); it != v.end(); ++it)
    delete *it;

  v.clear();
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace nmtl

#endif // NMTL_ALGORITHM_H
