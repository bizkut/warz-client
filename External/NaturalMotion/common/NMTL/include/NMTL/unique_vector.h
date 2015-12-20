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
#if (!defined(NMTL_UNIQUE_VECTOR_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_UNIQUE_VECTOR_H
#endif

//----------------------------------------------------------------------------------------------------------------------
#include "nmtl/defs.h"

#ifndef NMTL_NO_SENTRY
  #include "nmtl/hashmap_dense_dynamic.h"
  #include "nmtl/pod_vector.h"
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

//----------------------------------------------------------------------------------------------------------------------
namespace NMTL_NAMESPACE
{
// Forward declarations
template <typename T>
class NMTL_TEMPLATE_EXPORT unique_vector;

/// \brief A vector array that discards duplicate entries; avoids doing a linear search
//         by using a hash table to find out if an entry has already been added
template <typename T>
class NMTL_TEMPLATE_EXPORT unique_vector
{
public:
  typedef typename nmtl::pod_vector<T>::const_iterator        const_iterator;
  typedef typename nmtl::pod_vector<T>::iterator              iterator;

  NMTL_INLINE unique_vector(unsigned int defaultSize, nmtl::allocator* allocator_ = NMTL_DEFAULT_ALLOCATOR) :
    m_allocator(allocator_),
    m_hash(defaultSize, allocator_),
    m_vector(allocator_)
  {
    m_vector.reserve(defaultSize);
  }

  NMTL_INLINE unique_vector(const unique_vector& rhs) :
    m_allocator(rhs.m_allocator),
    m_hash(rhs.m_hash),
    m_vector(m_allocator)
  {
    // clone
    m_hash = rhs.m_hash;
    m_vector.assign(rhs.m_vector.begin(), rhs.m_vector.end());
  }

  NMTL_INLINE bool add(const T& item)
  {
    if (contains(item))
      return false;

    m_vector.push_back(item);
    m_hash.insert(item, true);

    return true;
  }

  NMTL_INLINE bool remove(const T& item)
  {
    if (m_hash.erase(item))
    {
      iterator it = begin();
      iterator itEnd = end();
      for (; it != itEnd; ++it)
      {
        if ((*it) == item)
        {
          m_vector.erase(it);
          return true;
        }
      }
      NMTL_ASSERT(0); // erased from the hash table, not found in the vector? fail.
    }
    return false;
  }

  NMTL_INLINE bool contains(const T& item) const
  {
    if (m_hash.find(item))
      return true;

    return false;
  }

  NMTL_INLINE void clear()
  {
    m_hash.clear();
    m_vector.clear();
  }

  NMTL_INLINE size_t size() const
  {
    return m_vector.size();
  }

  NMTL_INLINE const_iterator const_begin() const { return m_vector.begin(); }
  NMTL_INLINE const_iterator const_end() const { return m_vector.end(); }
  NMTL_INLINE iterator begin() { return m_vector.begin(); }
  NMTL_INLINE iterator end() { return m_vector.end(); }

protected:

  typedef typename nmtl::hashmap_dense_dynamic<T, bool>  ItemHash;
  typedef typename nmtl::pod_vector<T>                   ItemVector;

  nmtl::allocator* m_allocator;

  ItemHash         m_hash;
  ItemVector       m_vector;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_UNIQUE_VECTOR_H
