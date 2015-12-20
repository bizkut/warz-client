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
#if (!defined(NMTL_HASHMAP_DENSE_DYNAMIC_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_HASHMAP_DENSE_DYNAMIC_H
#endif

#include "nmtl/defs.h"
#include "nmtl/allocator.h"
#ifndef NMTL_NO_SENTRY
  #include "nmtl/hashfunc.h"
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

//----------------------------------------------------------------------------------------------------------------------
// Forward declarations
template <typename KeyType, typename ValueType>
class NMTL_TEMPLATE_EXPORT hashmap_dense_dynamic;

#ifdef _MSC_VER
  #pragma warning(push)
  // 4127: temporarily disable warning about comparisons with constants (when comparing with default template arguments)
  // 4324: don't whine about padding when stored types require declspec(align())
  #pragma warning(disable:4127 4324)
#endif // _MSC_VER

//----------------------------------------------------------------------------------------------------------------------
/// \details
///  An implementation of an open-addressing hashtable container with linear probing for collision resolution. It
///  offers very fast insertion, searching & deletion, with a significant performance spike if an insertion causes
///  an expansion of the table. All the core API operations are O(1) in the average case.
///
///  Memory allocation only occurs during construction or re-hashing. The table is allocated to a given
///  first-estimate size and grown dynamically when the number of entries in the table exceeds a load-factor of about 80%.
//----------------------------------------------------------------------------------------------------------------------
template <
  typename KeyType,  ///< key-type, ensure there is a hashFunction that can turn it into a uint32 key value
  typename ValueType ///< POD value type to store at key index
  >
class NMTL_TEMPLATE_EXPORT hashmap_dense_dynamic
{
public:
  typedef hashmap_dense_dynamic<KeyType, ValueType> _ThisType;

  NMTL_INLINE hashmap_dense_dynamic(unsigned int initialSize, nmtl::allocator* allocator_ = NMTL_DEFAULT_ALLOCATOR);
  NMTL_INLINE ~hashmap_dense_dynamic();

  // copy ctors
  NMTL_INLINE hashmap_dense_dynamic(const _ThisType& rhs);
  NMTL_INLINE hashmap_dense_dynamic& operator=(const _ThisType& rhs);

  // core API
  NMTL_INLINE bool insert(const KeyType& key, const ValueType& value);        ///< insert item into the table; returns true if value already inserted
  NMTL_INLINE bool find(const KeyType& key, ValueType* value = NULL) const;   ///< passing NULL for value turns find() into exists(); returns false if key not found
  NMTL_INLINE bool erase(const KeyType& key);                                 ///< returns true if the key was found and removed
  NMTL_INLINE bool replace(const KeyType& key, const ValueType& value);       ///< replace the value at given key with new value; returns false if key not found
  NMTL_INLINE void clear();                                                   ///< clear the table without resizing, ready for re-use

  // comparison
  NMTL_INLINE bool operator==(const _ThisType& rhs) const;

  // returns number of items currently inserted in the table
  NMTL_INLINE unsigned int getNumUsedSlots() const { return m_numUsedSlots; }

  // call after potential fragmentation after erase()ing to re-hash the table (without expanding it)
  // which will strip out unused entries, reduce probe distances, etc.
  NMTL_INLINE void optimize() { rehashInternal(m_hashSize); }

  // call to manually expand the table by one level and re-hash; useful if you are about to
  // add a great deal of new entries to an already well populated table
  NMTL_INLINE void expand() { rehashInternal(m_hashSize + 1); }

  // ensure we have a table of at least 'requestedSize' - asking for less than m_hashSize bails out early
  NMTL_INLINE void reserve(unsigned int requestedSize) { rehashInternal(requestedSize); }

  //-----------------------------------------------------------------
  /// \details
  /// walker classes providing limited iteration support of the hashmap values.
  /// designed for use thus:
  ///
  /// MyMap::const_value_walker myMapValues = myMap.constWalker();
  /// while (myMapValues.next())
  /// {
  ///    MyMapValueType* value = myMapValues();
  /// }
  ///
  /// call reset() to begin iteration anew. will assert if the hashmap changes during
  /// iteration. no support for reverse-iteration, etc.
  //-----------------------------------------------------------------
  class NMTL_TEMPLATE_EXPORT const_value_walker
  {
    friend class hashmap_dense_dynamic;

  public:
    // Public default constructor to allow const_value_walker to be used as a member
    // variable.
    //
    const_value_walker() :
      m_initialized(false)
    {
    }

    NMTL_INLINE const ValueType& operator()()
    {
      // check the hash hasn't changed since we started
      NMTL_ASSERT(m_hm->m_hashSize == m_hashSize);
      NMTL_ASSERT(m_hm->m_insertsTillRehash == m_insertsTillRehash);

      // ensure we landed on a used slot
      NMTL_ASSERT(m_hm->isSlotOccupied(m_index));

      return m_hm->m_pairs[m_index].m_value;
    }

    NMTL_INLINE const KeyType& key()
    {
      // check the hash hasn't changed since we started
      NMTL_ASSERT(m_hm->m_hashSize == m_hashSize);
      NMTL_ASSERT(m_hm->m_insertsTillRehash == m_insertsTillRehash);

      // ensure we landed on a used slot
      NMTL_ASSERT(m_hm->isSlotOccupied(m_index));

      return m_hm->m_pairs[m_index].m_key;
    }

    NMTL_INLINE bool next()
    {
      // check the hash hasn't changed since we started
      NMTL_ASSERT(m_hm->m_hashSize == m_hashSize);
      NMTL_ASSERT(m_hm->m_insertsTillRehash == m_insertsTillRehash);

      if (m_initialized)
        m_index ++;
      else
        m_initialized = true;

      while (m_index < m_hashSize)
      {
        // if we can see that the current 32 items are all unused, jump the lot
        // this gives a huge performance boost for large, sparsely populated tables
        if (m_hm->m_occupiedSlots[m_index>>5] == 0)
          m_index += 31;
        else if (m_hm->isSlotOccupied(m_index))
          return true;

        m_index ++;
      }

      return false;
    }

    NMTL_INLINE void reset()
    {
      m_index = 0;
      m_hashSize = m_hm->m_hashSize;
      m_insertsTillRehash = m_hm->m_insertsTillRehash;
      m_initialized = false;
    }

    NMTL_INLINE unsigned int index() const
    {
      return m_index;
    }

    NMTL_INLINE const_value_walker& operator = (const const_value_walker& rhs)
    {
      m_hm = rhs.m_hm;
      m_index = rhs.m_index;
      m_hashSize = rhs.m_hashSize;
      m_insertsTillRehash = rhs.m_insertsTillRehash;

      return *this;
    }

  protected:
    NMTL_INLINE const_value_walker(const hashmap_dense_dynamic* hm) : m_hm(hm) { reset(); }

    const hashmap_dense_dynamic*  m_hm;
    unsigned int                  m_index, m_hashSize, m_insertsTillRehash;
    bool                          m_initialized;
  };

  //-----------------------------------------------------------------
  class NMTL_TEMPLATE_EXPORT value_walker
  {
    friend class hashmap_dense_dynamic;

  public:
    // Public default constructor to allow value_walker to be used as a member
    // variable.
    //
    value_walker() :
      m_initialized(false)
    {
    }

    NMTL_INLINE ValueType& operator()()
    {
      // check the hash hasn't changed since we started
      NMTL_ASSERT(m_hm->m_hashSize == m_hashSize);
      NMTL_ASSERT(m_hm->m_insertsTillRehash == m_insertsTillRehash);

      // ensure we landed on a used slot
      NMTL_ASSERT(m_hm->isSlotOccupied(m_index));

      return m_hm->m_pairs[m_index].m_value;
    }

    NMTL_INLINE KeyType& key()
    {
      // check the hash hasn't changed since we started
      NMTL_ASSERT(m_hm->m_hashSize == m_hashSize);
      NMTL_ASSERT(m_hm->m_insertsTillRehash == m_insertsTillRehash);

      // ensure we landed on a used slot
      NMTL_ASSERT(m_hm->isSlotOccupied(m_index));

      return m_hm->m_pairs[m_index].m_key;
    }

    NMTL_INLINE bool next()
    {
      // check the hash hasn't changed since we started
      NMTL_ASSERT(m_hm->m_hashSize == m_hashSize);
      NMTL_ASSERT(m_hm->m_insertsTillRehash == m_insertsTillRehash);

      if (m_initialized)
        m_index ++;
      else
        m_initialized = true;

      while (m_index < m_hashSize)
      {
        // if we can see that the current 32 items are all unused, jump the lot
        // this gives a huge performance boost for large, sparsely populated tables
        if (m_hm->m_occupiedSlots[m_index>>5] == 0)
          m_index += 31;
        else if (m_hm->isSlotOccupied(m_index))
          return true;

        m_index ++;
      }

      return false;
    }

    NMTL_INLINE void reset()
    {
      m_index = 0;
      m_hashSize = m_hm->m_hashSize;
      m_insertsTillRehash = m_hm->m_insertsTillRehash;
      m_initialized = false;
    }

    NMTL_INLINE unsigned int index() const
    {
      return m_index;
    }

    NMTL_INLINE value_walker& operator = (const value_walker& rhs)
    {
      m_hm = rhs.m_hm;
      m_index = rhs.m_index;
      m_hashSize = rhs.m_hashSize;
      m_insertsTillRehash = rhs.m_insertsTillRehash;

      return *this;
    }

  protected:

    NMTL_INLINE value_walker(hashmap_dense_dynamic* hm) : m_hm(hm) { reset(); }

    hashmap_dense_dynamic* m_hm;   //lint !e1725 ref
    unsigned int           m_index, m_hashSize, m_insertsTillRehash;
    bool                   m_initialized;
  };

  NMTL_INLINE const_value_walker constWalker() const
  {
    return const_value_walker(this);
  }

  NMTL_INLINE value_walker walker()
  {
    return value_walker(this);
  }

  //-----------------------------------------------------------------

protected:
  struct KVPair
  {
    KeyType      m_key;
    ValueType    m_value;
    unsigned int m_probeDistance;
  };

  /// common table search logic, used in find, erase, etc.
  NMTL_INLINE bool doFind(const KeyType& key, unsigned int& outTableIndex, KVPair** outPair) const;

  /// handles copying data from given other instance of table, used in copy ctors
  NMTL_INLINE void copyFrom(const _ThisType& rhs);

  /// core worker function that turns a key into a table index
  NMTL_INLINE void hashToTableIndex(unsigned int& result, const KeyType& key, unsigned int hashSize) const
  {
    result = hfn::hashFunction(key);
    result %= hashSize;
  }

  /// number of slots that can be occupied until the table is re-hashed;
  /// this is a factor smaller than the total table size, to try and avoid the
  /// performance penalties associated with high load factors on closed hash tables
  NMTL_INLINE unsigned int scaleByLoadFactor(unsigned int hashSize) const
  {
    // java hashmap has loadfactor of 0.75
    return (unsigned int)((double)hashSize * 0.8);
  }

  /// rebuild the hash table, either with a new, larger index or simply rehash the
  /// current contents. if requestedSize == m_hashSize, the function returns without doing anything.
  /// if requestedSize < m_hashSize, the table is re-indexed but not re-sized (doesn't support table shrinking)
  NMTL_INLINE void rehashInternal(unsigned int requestedSize);

  /// returns true if the slot at index 'i' in the table is in use
  NMTL_INLINE bool isSlotOccupied(unsigned int i) const
  {
    return (m_occupiedSlots[i>>5] & (1 << (i & 31))) != 0;
  }

  KVPair*       m_pairs;              // contiguous key/value storage
  unsigned int* m_occupiedSlots;      // bitfield, one bit per pair, indicating slot usage
  unsigned int  m_hashSize;           // size of current hash table
  unsigned int  m_insertsTillRehash;  // inserts until we have to rehash
  unsigned int  m_numUsedSlots;       // slots in use (tracking insert/erase/clear)

#ifdef NMTL_DENSE_DYNAMIC_HASH_STATS
public:
  unsigned int  m_dbgInserts;         // number of calls to insert()
  unsigned int  m_dbgInsertClash;
  unsigned int  m_dbgErases;          //    "         "     erase()
  unsigned int  m_dbgRehashes;
  unsigned int  m_dbgMaxIter;
#endif // NMTL_DENSE_DYNAMIC_HASH_STATS

  //-----------------------------------------------------------------

private:
  nmtl::allocator* m_allocator;

  static void CompileTimeEnsurePODType()
  {
    // ensure key/value types are POD
    // the compiler will complain that non-POD types have copy constructors
    // and cannot be used in unions.
    union
    {
      KeyType   kt;
      ValueType vt;
    } u;
    sizeof(u);
    (void)u;
  }

// functions to access internal state during unit-testing
#ifdef NMTL_UNIT_TESTING
public:
  NMTL_INLINE unsigned int getNumberOfUnusedEntries() const
  {
    unsigned int i, numUnused = 0;
    for (i = 0; i < m_hashSize; i++)
    {
      if (!isSlotOccupied(i))
      {
        numUnused ++;
      }
    }
    return numUnused;
  }
#endif // NMTL_UNIT_TESTING
};

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
hashmap_dense_dynamic<KeyType, ValueType>::hashmap_dense_dynamic(unsigned int initialSize, nmtl::allocator* allocator_) :
#ifdef NMTL_DENSE_DYNAMIC_HASH_STATS
  m_dbgInserts(0),
  m_dbgInsertClash(0),
  m_dbgErases(0),
  m_dbgRehashes(0),
  m_dbgMaxIter(0),
#endif // NMTL_DENSE_DYNAMIC_HASH_STATS
  m_hashSize(0),
  m_numUsedSlots(0)
{
  m_allocator = allocator_;
  void (*p)() = CompileTimeEnsurePODType; sizeof(p);
  (void)*p;

  m_hashSize = nextPrime(initialSize);
  m_insertsTillRehash = scaleByLoadFactor(m_hashSize);

  m_pairs = reinterpret_cast<KVPair*>(m_allocator->calloc(sizeof(KVPair) * m_hashSize));
  m_occupiedSlots = reinterpret_cast<unsigned int*>(m_allocator->calloc(sizeof(unsigned int) * ((m_hashSize + 31) >> 5)));
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
hashmap_dense_dynamic<KeyType, ValueType>::~hashmap_dense_dynamic()
{
  m_allocator->free(m_occupiedSlots);
  m_allocator->free(m_pairs);

  m_pairs = 0;
  m_occupiedSlots = 0;

  m_hashSize = 0;
  m_numUsedSlots = 0;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
hashmap_dense_dynamic<KeyType, ValueType>::hashmap_dense_dynamic(const _ThisType& rhs) :
  m_pairs(0),
  m_occupiedSlots(0)
{
  copyFrom(rhs);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
hashmap_dense_dynamic<KeyType, ValueType>& hashmap_dense_dynamic<KeyType, ValueType>::operator=(const _ThisType& rhs)
{
  copyFrom(rhs);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
void hashmap_dense_dynamic<KeyType, ValueType>::copyFrom(const _ThisType& rhs)
{
  m_allocator = rhs.m_allocator;

  if (m_occupiedSlots)
    m_allocator->free(m_occupiedSlots);
  if (m_pairs)
    m_allocator->free(m_pairs);

  m_hashSize = rhs.m_hashSize;
  m_insertsTillRehash = rhs.m_insertsTillRehash;
  m_numUsedSlots = rhs.m_numUsedSlots;

  m_pairs = reinterpret_cast<KVPair*>(m_allocator->calloc(sizeof(KVPair) * m_hashSize));
  memcpy(m_pairs, rhs.m_pairs, sizeof(KVPair) * m_hashSize);

  size_t occupiedSlotsSz = sizeof(unsigned int) * ((m_hashSize + 31) >> 5);
  m_occupiedSlots = reinterpret_cast<unsigned int*>(m_allocator->calloc(occupiedSlotsSz));
  memcpy(m_occupiedSlots, rhs.m_occupiedSlots, occupiedSlotsSz);

#ifdef NMTL_DENSE_DYNAMIC_HASH_STATS
  m_dbgInserts = rhs.m_dbgInserts;
  m_dbgInsertClash = rhs.m_dbgInsertClash;
  m_dbgErases = rhs.m_dbgErases;
  m_dbgRehashes = rhs.m_dbgRehashes;
  m_dbgMaxIter = rhs.m_dbgMaxIter;
#endif // NMTL_DENSE_DYNAMIC_HASH_STATS
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
bool hashmap_dense_dynamic<KeyType, ValueType>::insert(const KeyType& key, const ValueType& value)
{
  if (m_insertsTillRehash == 0)
    expand();

  unsigned int tableIndex, probeCount = 0;

  hashToTableIndex(tableIndex, key, m_hashSize);

  // store first-hit slot where the probeDistance will be stored
  KVPair* chunk = &m_pairs[tableIndex];

  // linear probe search, walk until we find an empty slot
  while (isSlotOccupied(tableIndex))
  {
#ifdef NMTL_DENSE_DYNAMIC_HASH_STATS
    m_dbgInsertClash ++;
#endif // NMTL_DENSE_DYNAMIC_HASH_STATS

    // check if the value already exists in this slot
    if (hfn::hashKeysEqual<KeyType>(m_pairs[tableIndex].m_key, key))
    {
      return false;
    }

    // next slot, wrap-around at the end
    tableIndex ++;
    if (tableIndex >= m_hashSize)
      tableIndex = 0;

    // keep note on probe distance
    probeCount ++;

    // should never happen, the table will be expanded
    // automatically before completely full
    NMTL_ASSERT(probeCount < m_hashSize);
  }

  m_insertsTillRehash --;
  m_numUsedSlots ++;

  // mark slot as occupied
  m_occupiedSlots[tableIndex>>5] |= (1 << (tableIndex & 31));

  // store maximum probe distance for all keys that hit the original slot
  if (probeCount > chunk->m_probeDistance)
    chunk->m_probeDistance = probeCount;

  // store data into slot
  chunk = &m_pairs[tableIndex];
  chunk->m_key = key;
  chunk->m_value = value;

#ifdef NMTL_DENSE_DYNAMIC_HASH_STATS
  m_dbgMaxIter = (probeCount > m_dbgMaxIter) ? probeCount : m_dbgMaxIter;
  m_dbgInserts ++;
#endif // NMTL_DENSE_DYNAMIC_HASH_STATS

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
bool hashmap_dense_dynamic<KeyType, ValueType>::doFind(const KeyType& key, unsigned int& outTableIndex, KVPair** outPair) const
{
  unsigned int tableIndex, probeDist, i;
  hashToTableIndex(tableIndex, key, m_hashSize);

  probeDist = m_pairs[tableIndex].m_probeDistance;
  for (i = 0; i <= probeDist; i++)
  {
    if (isSlotOccupied(tableIndex))
    {
      if (hfn::hashKeysEqual<KeyType>(m_pairs[tableIndex].m_key, key))
      {
        *outPair = &m_pairs[tableIndex];
        outTableIndex = tableIndex;
        return true;
      }
    }

    tableIndex ++;
    if (tableIndex >= m_hashSize)
      tableIndex = 0;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
bool hashmap_dense_dynamic<KeyType, ValueType>::find(const KeyType& key, ValueType* value) const
{
  unsigned int tableIndex;
  KVPair* tPair;
  if (doFind(key, tableIndex, &tPair))
  {
    if (value != NULL)
      *value = tPair->m_value;

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
bool hashmap_dense_dynamic<KeyType, ValueType>::erase(const KeyType& key)
{
#ifdef NMTL_DENSE_DYNAMIC_HASH_STATS
  m_dbgErases ++;
#endif // NMTL_DENSE_DYNAMIC_HASH_STATS

  unsigned int tableIndex;
  KVPair* tPair;
  if (doFind(key, tableIndex, &tPair))
  {
    m_occupiedSlots[tableIndex>>5] &= ~(1 << (tableIndex & 31));
    m_numUsedSlots --;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
bool hashmap_dense_dynamic<KeyType, ValueType>::replace(const KeyType& key, const ValueType& value)
{
  unsigned int tableIndex;
  KVPair* tPair;
  if (doFind(key, tableIndex, &tPair))
  {
    tPair->m_value = value;
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
void hashmap_dense_dynamic<KeyType, ValueType>::clear()
{
  m_insertsTillRehash = scaleByLoadFactor(m_hashSize);

  // not strictly necessary
  memset(m_pairs, 0, sizeof(KVPair) * m_hashSize);

  size_t occupiedSlotsSz = sizeof(unsigned int) * ((m_hashSize + 31) >> 5);
  memset(m_occupiedSlots, 0, occupiedSlotsSz);
  m_numUsedSlots = 0;

#ifdef NMTL_DENSE_DYNAMIC_HASH_STATS
  m_dbgInserts = 0;
  m_dbgInsertClash = 0;
  m_dbgErases = 0;
  m_dbgRehashes = 0;
  m_dbgMaxIter = 0;
#endif // NMTL_DENSE_DYNAMIC_HASH_STATS
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
NMTL_INLINE bool hashmap_dense_dynamic<KeyType, ValueType>::operator==(const _ThisType& rhs) const
{
  if (m_hashSize != rhs.m_hashSize)
  {
    return false;
  }

  if (m_numUsedSlots != rhs.m_numUsedSlots)
  {
    return false;
  }

  return memcmp(m_pairs, rhs.m_pairs, sizeof(KVPair) * m_hashSize) == 0;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
void hashmap_dense_dynamic<KeyType, ValueType>::rehashInternal(unsigned int requestedSize)
{
  unsigned int newHashSize = (requestedSize == m_hashSize) ? m_hashSize : nextPrime(requestedSize);

  // we cannot arbitrarily shrink the table
  if (newHashSize < m_hashSize)
  {
    return;
  }

  KVPair* newPairs = reinterpret_cast<KVPair*>(m_allocator->calloc(sizeof(KVPair) * newHashSize));
  unsigned int* newBitfield = reinterpret_cast<unsigned int*>(m_allocator->calloc(sizeof(unsigned int) * ((newHashSize + 31) >> 5)));

  m_insertsTillRehash = scaleByLoadFactor(newHashSize);
  m_numUsedSlots = 0;

#ifdef NMTL_DENSE_DYNAMIC_HASH_STATS
  m_dbgMaxIter = 0;
#endif // NMTL_DENSE_DYNAMIC_HASH_STATS

  unsigned int tableIndex, probeCount;

  for (unsigned int i = 0; i < m_hashSize; i++)
  {
    if (!isSlotOccupied(i))
      continue;

    // hash existing entry with new hash table size to find it's new position
    hashToTableIndex(tableIndex, m_pairs[i].m_key, newHashSize);

    // store first-hit slot where the probeDistance will be stored
    probeCount = 0;
    KVPair* originalSlot = &newPairs[tableIndex];

    while ((newBitfield[tableIndex>>5] & (1 << (tableIndex & 31))) != 0)
    {
      tableIndex ++;
      if (tableIndex >= newHashSize)
        tableIndex = 0;

      probeCount ++;
    }

    m_insertsTillRehash --;
    m_numUsedSlots ++;

    if (probeCount > originalSlot->m_probeDistance)
      originalSlot->m_probeDistance = probeCount;

#ifdef NMTL_DENSE_DYNAMIC_HASH_STATS
    m_dbgMaxIter = (probeCount > m_dbgMaxIter) ? probeCount : m_dbgMaxIter;
#endif // NMTL_DENSE_DYNAMIC_HASH_STATS

    newBitfield[tableIndex>>5] |= (1 << (tableIndex & 31));
    newPairs[tableIndex] = m_pairs[i];
  }

  m_hashSize = newHashSize;

  m_allocator->free(m_pairs);
  m_pairs = newPairs;

  m_allocator->free(m_occupiedSlots);
  m_occupiedSlots = newBitfield;

#ifdef NMTL_DENSE_DYNAMIC_HASH_STATS
  m_dbgRehashes ++;
#endif // NMTL_DENSE_DYNAMIC_HASH_STATS
}

#ifdef _MSC_VER
  #pragma warning(pop)
#endif // _MSC_VER

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_HASHMAP_DENSE_DYNAMIC_H
