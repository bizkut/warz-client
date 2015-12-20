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
#if (!defined(NMTL_BIDIR_PTRMAP_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_BIDIR_PTRMAP_H
#endif

//----------------------------------------------------------------------------------------------------------------------
#include "nmtl/defs.h"

#ifndef NMTL_NO_SENTRY
  #include "nmtl/hashmap_dense_dynamic.h"
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
template <typename KeyType, typename ValueType>
class NMTL_TEMPLATE_EXPORT bidirectional_ptrmap;

/// \brief bidirectional lookup table specifically for doing [ptr <-> ptr] searches; uses a pair of dense_dynamic
///        hashmaps and may provide leaner memory and performance than NMutils' TwoWayMapHelper, which uses std::map
///        The API emulates TwoWayMapHelper so it can act as a drop-in replacement (barring the iterator access, sadly)
template <typename KeyType, typename ValueType>
class NMTL_TEMPLATE_EXPORT bidirectional_ptrmap
{
public:
  typedef typename nmtl::hashmap_dense_dynamic<KeyType, ValueType>::const_value_walker    const_value_walker;

  NMTL_INLINE bidirectional_ptrmap(unsigned int defaultSize, nmtl::allocator* allocator_ = NMTL_DEFAULT_ALLOCATOR) :
    m_allocator(allocator_),
    m_keyValueHash(defaultSize, allocator_),
    m_valueKeyHash(defaultSize, allocator_)
  {
#ifdef NMTL_HAS_TR1
    NMTL_STATIC_ASSERT(std::tr1::is_pointer<KeyType>::value);
    NMTL_STATIC_ASSERT(std::tr1::is_pointer<ValueType>::value);
#endif // NMTL_HAS_TR1
  }

  NMTL_INLINE bidirectional_ptrmap(const bidirectional_ptrmap& rhs) :
    m_allocator(rhs.m_allocator),
    m_keyValueHash(rhs.m_keyValueHash),
    m_valueKeyHash(rhs.m_valueKeyHash)
  {
  }

  NMTL_INLINE bool insertPair(KeyType key, ValueType value)
  {
    if (m_keyValueHash.insert(key, value))
    {
      bool secondInsert = m_valueKeyHash.insert(value, key);
      NMTL_ASSERT(secondInsert);
      return secondInsert;
    }
    return false;
  }

  NMTL_INLINE KeyType getFirst(ValueType value) const
  {
    KeyType key = 0;
    m_valueKeyHash.find(value, &key);

    return key;
  }

  NMTL_INLINE ValueType getSecond(KeyType key) const
  {
    ValueType value = 0;
    m_keyValueHash.find(key, &value);

    return value;
  }

  NMTL_INLINE bool erase(KeyType key)
  {
    ValueType value = 0;
    if (m_keyValueHash.find(key, &value))
    {
      bool erasedFirst = m_keyValueHash.erase(key);
      bool erasedSecond = m_valueKeyHash.erase(value);

      NMTL_ASSERT(erasedFirst && erasedSecond);
      return (erasedFirst && erasedSecond);
    }
    return false;
  }

  NMTL_INLINE void clear()
  {
    m_keyValueHash.clear();
    m_valueKeyHash.clear();
  }

  NMTL_INLINE bool empty()
  {
    return ((m_keyValueHash.getNumUsedSlots() == 0) && (m_valueKeyHash.getNumUsedSlots() == 0));
  }

  // check dense_dynamic hash comments for usage
  NMTL_INLINE const_value_walker constWalker() const
  {
    return m_keyValueHash.constWalker();
  }

protected:
  typedef typename nmtl::hashmap_dense_dynamic<KeyType, ValueType>  HashKV;
  typedef typename nmtl::hashmap_dense_dynamic<ValueType, KeyType>  HashVK;

  nmtl::allocator* m_allocator;

  HashKV    m_keyValueHash;
  HashVK    m_valueKeyHash;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_BIDIR_PTRMAP_H
