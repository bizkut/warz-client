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
#if (!defined(NMTL_SET_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_SET_H
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifndef NMTL_NO_SENTRY
  #include "nmtl/vector.h"
#endif

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
/// \class   NMTL::set
/// \brief   stores a set of values in an ordered set.
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename P = std::less<K> >
class NMTL_TEMPLATE_EXPORT set :
  protected vector<K>
{
public:

  typedef set<K, P> my_type;

  typedef K key_type;
  typedef P key_compare;
  typedef typename vector<K>::size_type size_type;
  typedef typename vector<K>::difference_type difference_type;
  typedef typename vector<K>::pointer pointer;
  typedef typename vector<K>::const_pointer const_pointer;
  typedef typename vector<K>::reference reference;
  typedef typename vector<K>::const_reference const_reference;
  typedef typename vector<K>::iterator iterator;
  typedef typename vector<K>::const_iterator const_iterator;
  typedef typename vector<K>::value_type value_type;

  /// \brief  construct empty set from defaults
  NMTL_INLINE set(nmtl::allocator* allocator_ = NMTL_DEFAULT_ALLOCATOR) : vector<key_type>(allocator_)
  {
  }

  /// \brief  erase element at _Where
  /// \param  _Where - the item to erase
  NMTL_INLINE void erase(iterator location)
  {
    vector<key_type>::erase(location);
  }

  /// \brief  erase element at _Where
  /// \param  _Keyval - the item to erase
  ///
  NMTL_INLINE size_type erase(const key_type& key_val)
  {
    iterator it = std::find(begin(), end(), key_val);
    if (it != end())
    {
      vector<key_type>::erase(it);
      return 1;
    }
    return 0;
  }

  /// \brief  finds the specified element
  /// \todo   implement this as a binary search (since the set is ordered). std::find does a linear search.
  NMTL_INLINE iterator find(const key_type& key_val)
  {
    return std::find(this->m_first, this->m_last, key_val);
  }

  /// \brief  finds the specified element
  /// \todo   implement this as a binary search (since the set is ordered). std::find does a linear search.
  NMTL_INLINE const_iterator find(const key_type& key_val) const
  {
    return std::find(this->m_first, this->m_last, key_val);
  }

  /// \brief  erases the elements between _First and _Last
  /// \param  _First - first element in sequence to erase
  /// \param  _Last - last element in sequence to erase
  NMTL_INLINE void erase(iterator first, iterator last)
  {
    vector<key_type>::erase(first, last);
  }

  /// \brief  inserts the specified value into the set
  /// \param  _Keyval -
  NMTL_INLINE size_type insert(const key_type& key_val)
  {
    iterator it = std::find(this->m_first, this->m_end, key_val);
    if (it == end())
    {
      vector<key_type>::push_back(key_val);
      std::sort(this->m_first, this->m_end, key_compare());
    }
    return size();
  }

  /// \brief   return iterator for beginning of mutable sequence
  /// \return  returns an iterator to the start of the data
  NMTL_INLINE iterator begin()
  {
    return vector<key_type>::begin();
  }

  /// \brief   return iterator for beginning of non-mutable sequence
  /// \return  returns an iterator to the start of the data
  NMTL_INLINE const_iterator begin() const
  {
    return vector<key_type>::begin();
  }

  /// \brief   return iterator for the end of mutable sequence
  /// \return  returns an iterator to the end of the data
  NMTL_INLINE iterator end()
  {
    return vector<key_type>::end();
  }

  /// \brief   return iterator for the end of non-mutable sequence
  /// \return  returns an iterator to the end of the data
  NMTL_INLINE const_iterator end() const
  {
    return vector<key_type>::end();
  }

  /// \brief  returns length of sequence
  /// \return the size of the array
  NMTL_INLINE size_type size() const
  {
    return vector<key_type>::size();
  }

  /// \brief  returns true if the array is currently empty
  /// \return true if the array is empty
  NMTL_INLINE bool empty() const
  {
    return vector<key_type>::empty();
  }

  /// \brief clears set
  NMTL_INLINE void clear()
  {
    vector<key_type>::clear();
  }

  /// \brief  returns a reference to the specified element in the array
  /// \param  _Pos - the index of the element to return
  /// \return the array element requested
  NMTL_INLINE const_reference at(NMTL_INDEX size_type pos) const
  {
    return vector<key_type>::at(pos);
  }

  /// \brief  returns a reference to the specified element in the array
  /// \param  _Pos - the index of the element to return
  /// \return the array element requested
  NMTL_INLINE reference at(NMTL_INDEX size_type pos)
  {
    return vector<key_type>::at(pos);
  }

  /// \brief  subscript non-mutable sequence
  /// \param  _Pos - the array element to access
  /// \return the array element requested
  ///
  NMTL_INLINE const_reference operator[](NMTL_INDEX size_type pos) const
  {
    return *(begin() + pos);
  }

  /// \brief  subscript mutable sequence
  /// \param  _Pos - the array element to access
  /// \return the array element requested
  NMTL_INLINE reference operator[](NMTL_INDEX size_type pos)
  {
    return *(begin() + pos);
  }

  /// \brief  return first element of mutable sequence
  NMTL_INLINE reference front()
  {
    return vector<key_type>::front();
  }

  /// \brief  return first element of non-mutable sequence
  NMTL_INLINE const_reference front() const
  {
    return vector<key_type>::front();
  }

  /// \brief  return last element of mutable sequence
  NMTL_INLINE reference back()
  {
    return vector<key_type>::back();
  }

  /// \brief  return last element of non-mutable sequence
  NMTL_INLINE const_reference back() const
  {
    return vector<key_type>::back();
  }
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_SET_H
