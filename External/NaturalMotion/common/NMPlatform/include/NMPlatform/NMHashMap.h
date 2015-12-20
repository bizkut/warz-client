// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NMP_HASHMAP_H
#define NMP_HASHMAP_H

#include "NMPlatform/NMMemoryAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

///---------------------------------------------------------------------------------------------------------------------
/// \brief Given the input value, this function will return the next biggest prime number
/// in the chosen sequence. Each entry is approximately half way between the two nearest power-of-two values.
///---------------------------------------------------------------------------------------------------------------------
NM_INLINE unsigned int nextPrime(unsigned int nv)
{
  if (nv < 11) return 11;
#define NMP_HSIZE(v) else if (nv < v) return v
  NMP_HSIZE(17);
  NMP_HSIZE(53);
  NMP_HSIZE(97);
  NMP_HSIZE(193);
  NMP_HSIZE(389);
  NMP_HSIZE(769);
  NMP_HSIZE(1543);
  NMP_HSIZE(3079);
  NMP_HSIZE(6151);
  NMP_HSIZE(12289);
  NMP_HSIZE(24593);
  NMP_HSIZE(49157);
  NMP_HSIZE(98317);
  NMP_HSIZE(196613);
  NMP_HSIZE(393241);
  NMP_HSIZE(786433);
  NMP_HSIZE(1572869);
  NMP_HSIZE(3145739);
  NMP_HSIZE(6291469);
#undef NMP_HSIZE
  // We should not get here
  NMP_ASSERT_FAIL();
  return 0;
}

///---------------------------------------------------------------------------------------------------------------------
/// \brief Equality test templated stub
template <typename KeyType>
NM_INLINE bool hashKeysEqual(const KeyType& lhs, const KeyType& rhs) { return lhs == rhs; }


///---------------------------------------------------------------------------------------------------------------------
/// \brief Well-performing hash function for a 64 bit value
NM_INLINE uint32_t hashFunction(uint64_t key)
{
  key = (~key) + (key << 18);
  key = key ^ (key >> 31);
  key = key * 21;
  key = key ^ (key >> 11);
  key = key + (key << 6);
  key = key ^ (key >> 22);
  return (uint32_t) key;
}

///---------------------------------------------------------------------------------------------------------------------
/// \brief Well-performing hash function for a single u32 value
NM_INLINE uint32_t hashFunction(uint32_t key)
{
  key = (key ^ 0xe995) ^ (key >> 16);
  key += (key << 3);
  key ^= (key >> 4);
  key *= 0x27d4eb2d;
  key ^= (key >> 15);
  return key;
}

///---------------------------------------------------------------------------------------------------------------------
NM_INLINE int32_t hashFunction(int32_t key)
{
  return (int32_t)hashFunction((uint32_t)key);
}

///---------------------------------------------------------------------------------------------------------------------
/// Pointer types are cast to uint32_t, which could theoretically lead to truncation on 64 bit platforms. 
/// However, the hash_map should be able to deal with clashes resulting from such truncations.
///---------------------------------------------------------------------------------------------------------------------
template <typename KeyType>
NM_INLINE int32_t hashFunction(KeyType* key)
{
  return (int32_t)hashFunction((uint32_t)(ptrdiff_t)key);
}

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::hash_map
/// \brief An implementation of hash_map to remove code dependency on STL implementations.
/// \details An implementation of an open-addressing hashtable container with linear probing for collision resolution. It
///  offers very fast insertion, searching & deletion, with a significant performance spike if an insertion causes
///  an expansion of the table. All the core API operations are O(1) in the average case.
///
///  Memory allocation only occurs during construction or re-hashing. The table is allocated to a given
///  first-estimate size and grown dynamically when the number of entries in the table exceeds a load-factor of about
///  80%.
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
template <
  typename KeyType,  ///< key-type, ensure there is a hashFunction that can turn it into a uint32 key value
  typename ValueType ///< POD value type to store at key index
  >
class hash_map
{
public:
  typedef hash_map<KeyType, ValueType> _ThisType;

  NM_INLINE void init(unsigned int initialSize, NMP::MemoryAllocator* allocator);

  NM_INLINE hash_map(unsigned int initialSize, NMP::MemoryAllocator* allocator);
  NM_INLINE hash_map(unsigned int initialSize = 8);
  NM_INLINE ~hash_map();


  NM_INLINE static NMP::Memory::Format getMemoryRequirements();
  NM_INLINE static _ThisType* init(NMP::Memory::Resource& resource, unsigned int initialSize, NMP::MemoryAllocator* allocator);

  // copy constructors
  NM_INLINE hash_map(const _ThisType& rhs);
  NM_INLINE hash_map& operator=(const _ThisType& rhs);

  /// \brief Insert item into the table; returns false if value already exists, true if it was inserted on this call.
  NM_INLINE bool insert(const KeyType& key, const ValueType& value);
  /// \brief Passing NULL for value turns find() into exists(); returns false if key not found
  NM_INLINE bool find(const KeyType& key, ValueType* value = NULL) const;
  /// \brief Returns true if the key was found and removed
  NM_INLINE bool erase(const KeyType& key);
  /// \brief Replace the value at given key with new value; returns false if key not found
  NM_INLINE bool replace(const KeyType& key, const ValueType& value);
  /// \brief Clear the table without resizing, ready for re-use
  NM_INLINE void clear();

  /// \brief comparison operator
  NM_INLINE bool operator==(const _ThisType& rhs) const;

  /// \brief Returns number of items currently inserted in the table
  NM_INLINE unsigned int getNumUsedSlots() const { return m_numUsedSlots; }

  /// \brief Call after potential fragmentation after erase()ing to re-hash the table (without expanding it)
  ///  which will strip out unused entries, reduce probe distances, etc.
  NM_INLINE void optimize() { rehashInternal(m_hashSize); }

  /// \brief Call to manually expand the table by one level and re-hash; useful if you are about to
  ///  add a great deal of new entries to an already well populated table
  NM_INLINE void expand() { rehashInternal(m_hashSize + 1); }

  /// \brief Ensure we have a table of at least 'requestedSize' - asking for less than m_hashSize bails out early
  NM_INLINE void reserve(unsigned int requestedSize) { rehashInternal(requestedSize); }

  //--------------------------------------------------------------------------------------------------------------------
  /// \details
  /// walker classes providing limited iteration support of the hash_map values.
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
  //--------------------------------------------------------------------------------------------------------------------
  class const_value_walker
  {
    friend class hash_map;

  public:
    // Public default constructor to allow const_value_walker to be used as a member
    // variable.
    //
    const_value_walker() :
      m_initialized(false)
    {
    }

    NM_INLINE const ValueType& operator()()
    {
      // check the hash hasn't changed since we started
      NMP_ASSERT(m_hm->m_hashSize == m_hashSize);
      NMP_ASSERT(m_hm->m_insertsTillRehash == m_insertsTillRehash);

      // ensure we landed on a used slot
      NMP_ASSERT(m_hm->isSlotOccupied(m_index));

      return m_hm->m_pairs[m_index].m_value;
    }

    NM_INLINE const KeyType& key()
    {
      // check the hash hasn't changed since we started
      NMP_ASSERT(m_hm->m_hashSize == m_hashSize);
      NMP_ASSERT(m_hm->m_insertsTillRehash == m_insertsTillRehash);

      // ensure we landed on a used slot
      NMP_ASSERT(m_hm->isSlotOccupied(m_index));

      return m_hm->m_pairs[m_index].m_key;
    }

    NM_INLINE bool next()
    {
      // check the hash hasn't changed since we started
      NMP_ASSERT(m_hm->m_hashSize == m_hashSize);
      NMP_ASSERT(m_hm->m_insertsTillRehash == m_insertsTillRehash);

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

    NM_INLINE void reset()
    {
      m_index = 0;
      m_hashSize = m_hm->m_hashSize;
      m_insertsTillRehash = m_hm->m_insertsTillRehash;
      m_initialized = false;
    }

    NM_INLINE unsigned int index() const
    {
      return m_index;
    }

    NM_INLINE const_value_walker& operator = (const const_value_walker& rhs)
    {
      m_hm = rhs.m_hm;
      m_index = rhs.m_index;
      m_hashSize = rhs.m_hashSize;
      m_insertsTillRehash = rhs.m_insertsTillRehash;

      return *this;
    }

  protected:
    NM_INLINE const_value_walker(const hash_map* hm) : m_hm(hm) { reset(); }

    const hash_map*  m_hm;
    unsigned int                  m_index, m_hashSize, m_insertsTillRehash;
    bool                          m_initialized;
  };

  //--------------------------------------------------------------------------------------------------------------------
  class value_walker
  {
    friend class hash_map;

  public:
    // Public default constructor to allow value_walker to be used as a member
    // variable.
    //
    value_walker() :
      m_initialized(false)
    {
    }

    NM_INLINE ValueType& operator()()
    {
      // check the hash hasn't changed since we started
      NMP_ASSERT(m_hm->m_hashSize == m_hashSize);
      NMP_ASSERT(m_hm->m_insertsTillRehash == m_insertsTillRehash);

      // ensure we landed on a used slot
      NMP_ASSERT(m_hm->isSlotOccupied(m_index));

      return m_hm->m_pairs[m_index].m_value;
    }

    NM_INLINE KeyType& key()
    {
      // check the hash hasn't changed since we started
      NMP_ASSERT(m_hm->m_hashSize == m_hashSize);
      NMP_ASSERT(m_hm->m_insertsTillRehash == m_insertsTillRehash);

      // ensure we landed on a used slot
      NMP_ASSERT(m_hm->isSlotOccupied(m_index));

      return m_hm->m_pairs[m_index].m_key;
    }

    NM_INLINE bool next()
    {
      // check the hash hasn't changed since we started
      NMP_ASSERT(m_hm->m_hashSize == m_hashSize);
      NMP_ASSERT(m_hm->m_insertsTillRehash == m_insertsTillRehash);

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

    NM_INLINE void reset()
    {
      m_index = 0;
      m_hashSize = m_hm->m_hashSize;
      m_insertsTillRehash = m_hm->m_insertsTillRehash;
      m_initialized = false;
    }

    NM_INLINE unsigned int index() const
    {
      return m_index;
    }

    NM_INLINE value_walker& operator = (const value_walker& rhs)
    {
      m_hm = rhs.m_hm;
      m_index = rhs.m_index;
      m_hashSize = rhs.m_hashSize;
      m_insertsTillRehash = rhs.m_insertsTillRehash;
      m_initialized = rhs.m_initialized;

      return *this;
    }

  protected:

    NM_INLINE value_walker(hash_map* hm) : m_hm(hm) { reset(); }

    hash_map* m_hm;   //lint !e1725 ref
    unsigned int           m_index, m_hashSize, m_insertsTillRehash;
    bool                   m_initialized;
  };

  //--------------------------------------------------------------------------------------------------------------------
  NM_INLINE const_value_walker constWalker() const
  {
    return const_value_walker(this);
  }

  //--------------------------------------------------------------------------------------------------------------------
  NM_INLINE value_walker walker()
  {
    return value_walker(this);
  }

  //--------------------------------------------------------------------------------------------------------------------

protected:
  struct KVPair
  {
    KeyType      m_key;
    ValueType    m_value;
    unsigned int m_probeDistance;
  };

  /// common table search logic, used in find, erase, etc.
  NM_INLINE bool doFind(const KeyType& key, unsigned int& outTableIndex, KVPair** outPair) const;

  /// handles copying data from given other instance of table, used in copy ctors
  NM_INLINE void copyFrom(const _ThisType& rhs);

  /// core worker function that turns a key into a table index
  NM_INLINE void hashToTableIndex(unsigned int& result, const KeyType& key, unsigned int hashSize) const
  {
    result = hashFunction(key);
    result %= hashSize;
  }

  /// number of slots that can be occupied until the table is re-hashed;
  /// this is a factor smaller than the total table size, to try and avoid the
  /// performance penalties associated with high load factors on closed hash tables
  NM_INLINE unsigned int scaleByLoadFactor(unsigned int hashSize) const
  {
    // java hashmap has loadfactor of 0.75
    return (unsigned int)((double)hashSize * 0.8);
  }

  /// rebuild the hash table, either with a new, larger index or simply rehash the
  /// current contents. if requestedSize == m_hashSize, the function returns without doing anything.
  /// if requestedSize < m_hashSize, the table is re-indexed but not re-sized (doesn't support table shrinking)
  NM_INLINE void rehashInternal(unsigned int requestedSize);

  /// returns true if the slot at index 'i' in the table is in use
  NM_INLINE bool isSlotOccupied(unsigned int i) const
  {
    return (m_occupiedSlots[i>>5] & (1 << (i & 31))) != 0;
  }

  KVPair*       m_pairs;              // contiguous key/value storage
  unsigned int* m_occupiedSlots;      // bitfield, one bit per pair, indicating slot usage
  unsigned int  m_hashSize;           // size of current hash table
  unsigned int  m_insertsTillRehash;  // inserts until we have to rehash
  unsigned int  m_numUsedSlots;       // slots in use (tracking insert/erase/clear)

  //--------------------------------------------------------------------------------------------------------------------

private:
  NMP::MemoryAllocator* m_allocator;
  bool m_ownAllocator;
#if !defined(NM_HOST_NO_COMPILE_TIME_POD_CHECK)
  static void CompileTimeEnsurePODType()
  {
    // This will ensure key/value types are POD.
    // the compiler will complain that non-POD types have copy constructors
    // and cannot be used in unions.
    union
    {
      KeyType   kt;
      ValueType vt;
    } u;
    (void) u;
    sizeof(u);
  }
#endif // !defined(NM_HOST_NO_COMPILE_TIME_POD_CHECK)

};

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
void hash_map<KeyType, ValueType>::init(unsigned int initialSize, NMP::MemoryAllocator* allocator)
{
  m_hashSize = 0;
  m_numUsedSlots = 0;
  m_allocator = allocator;
  m_ownAllocator = false;
#if !defined(NM_HOST_NO_COMPILE_TIME_POD_CHECK)
  void (*p)() = CompileTimeEnsurePODType; sizeof(p);
  (void) p;
#endif // !defined(NM_HOST_NO_COMPILE_TIME_POD_CHECK)

  m_hashSize = nextPrime(initialSize);
  m_insertsTillRehash = scaleByLoadFactor(m_hashSize);

  m_pairs = reinterpret_cast<KVPair*>(NMPAllocatorMemAlloc(m_allocator, sizeof(KVPair) * m_hashSize, NMP_NATURAL_TYPE_ALIGNMENT));
  NMP_ASSERT(m_pairs);
  memset(m_pairs, 0, sizeof(KVPair) * m_hashSize);
  m_occupiedSlots = reinterpret_cast<unsigned int*>(NMPAllocatorMemAlloc(m_allocator, sizeof(unsigned int) * ((m_hashSize + 31) >> 5), NMP_NATURAL_TYPE_ALIGNMENT));
  NMP_ASSERT(m_occupiedSlots);
  memset(m_occupiedSlots, 0, sizeof(unsigned int) * ((m_hashSize + 31) >> 5));
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
hash_map<KeyType, ValueType>::hash_map(unsigned int initialSize, NMP::MemoryAllocator* allocator)
{
  init(initialSize, allocator);
  m_ownAllocator = false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
hash_map<KeyType, ValueType>::hash_map(unsigned int initialSize)
{
  Memory::Resource allocatorResource = 
    NMPMemoryAllocateFromFormat(HeapAllocator::getMemoryRequirements());
  HeapAllocator *allocator = HeapAllocator::init(allocatorResource);

  init(initialSize, allocator);

  m_ownAllocator = true;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
hash_map<KeyType, ValueType>::~hash_map()
{
  m_allocator->memFree(m_occupiedSlots);
  m_allocator->memFree(m_pairs);

  m_pairs = 0;
  m_occupiedSlots = 0;

  m_hashSize = 0;
  m_numUsedSlots = 0;

  if(m_ownAllocator)
  {
    NMP::Memory::memFree(m_allocator);
  }
}


//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
NMP::Memory::Format hash_map<KeyType, ValueType>::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(hash_map<KeyType, ValueType>), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
hash_map<KeyType, ValueType>* hash_map<KeyType, ValueType>::init(NMP::Memory::Resource& resource,
                                                                 unsigned int initialSize,
                                                                 NMP::MemoryAllocator* allocator)
{
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  // Make sure there's enough space in the resource for this class
  NMP_ASSERT(resource.format.size >= sizeof(hash_map<KeyType, ValueType>));
  // Construct!
  hash_map<KeyType, ValueType> *result = new(resource.ptr)hash_map<KeyType, ValueType>(initialSize, allocator);
  // Increment the resource to reflect the used-up space
  resource.increment(getMemoryRequirements());

  return result;
}


//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
hash_map<KeyType, ValueType>::hash_map(const _ThisType& rhs) :
  m_pairs(0),
  m_occupiedSlots(0)
{
  copyFrom(rhs);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
hash_map<KeyType, ValueType>& hash_map<KeyType, ValueType>::operator=(const _ThisType& rhs)
{
  copyFrom(rhs);
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
void hash_map<KeyType, ValueType>::copyFrom(const _ThisType& rhs)
{
  m_allocator = rhs.m_allocator;

  if (m_occupiedSlots)
    m_allocator->memFree(m_occupiedSlots);
  if (m_pairs)
    m_allocator->memFree(m_pairs);

  m_hashSize = rhs.m_hashSize;
  m_insertsTillRehash = rhs.m_insertsTillRehash;
  m_numUsedSlots = rhs.m_numUsedSlots;

  m_pairs = reinterpret_cast<KVPair*>(NMPAllocatorMemAlloc(m_allocator, sizeof(KVPair) * m_hashSize, NMP_NATURAL_TYPE_ALIGNMENT));
  NMP_ASSERT(m_pairs);
  memset(m_pairs, 0, sizeof(KVPair) * m_hashSize);
  memcpy(m_pairs, rhs.m_pairs, sizeof(KVPair) * m_hashSize);

  size_t occupiedSlotsSz = sizeof(unsigned int) * ((m_hashSize + 31) >> 5);
  m_occupiedSlots = reinterpret_cast<unsigned int*>(NMPAllocatorMemAlloc(m_allocator, occupiedSlotsSz, NMP_NATURAL_TYPE_ALIGNMENT));
  NMP_ASSERT(m_occupiedSlots);
  memset(m_occupiedSlots, 0, occupiedSlotsSz);
  memcpy(m_occupiedSlots, rhs.m_occupiedSlots, occupiedSlotsSz);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
bool hash_map<KeyType, ValueType>::insert(const KeyType& key, const ValueType& value)
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
    // check if the value already exists in this slot
    if (hashKeysEqual<KeyType>(m_pairs[tableIndex].m_key, key))
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
    NMP_ASSERT(probeCount < m_hashSize);
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

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
bool hash_map<KeyType, ValueType>::doFind(const KeyType& key, unsigned int& outTableIndex, KVPair** outPair) const
{
  unsigned int tableIndex, probeDist, i;
  hashToTableIndex(tableIndex, key, m_hashSize);

  probeDist = m_pairs[tableIndex].m_probeDistance;
  for (i = 0; i <= probeDist; i++)
  {
    if (isSlotOccupied(tableIndex))
    {
      if (hashKeysEqual<KeyType>(m_pairs[tableIndex].m_key, key))
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
bool hash_map<KeyType, ValueType>::find(const KeyType& key, ValueType* value) const
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
bool hash_map<KeyType, ValueType>::erase(const KeyType& key)
{
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
bool hash_map<KeyType, ValueType>::replace(const KeyType& key, const ValueType& value)
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
void hash_map<KeyType, ValueType>::clear()
{
  m_insertsTillRehash = scaleByLoadFactor(m_hashSize);

  // not strictly necessary
  memset(m_pairs, 0, sizeof(KVPair) * m_hashSize);

  size_t occupiedSlotsSz = sizeof(unsigned int) * ((m_hashSize + 31) >> 5);
  memset(m_occupiedSlots, 0, occupiedSlotsSz);
  m_numUsedSlots = 0;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename KeyType, typename ValueType>
NM_INLINE bool hash_map<KeyType, ValueType>::operator==(const _ThisType& rhs) const
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
void hash_map<KeyType, ValueType>::rehashInternal(unsigned int requestedSize)
{
  unsigned int newHashSize = (requestedSize == m_hashSize) ? m_hashSize : nextPrime(requestedSize);

  // we cannot arbitrarily shrink the table
  if (newHashSize < m_hashSize)
  {
    return;
  }

  KVPair* newPairs = reinterpret_cast<KVPair*>(NMPAllocatorMemAlloc(m_allocator, sizeof(KVPair) * newHashSize, NMP_NATURAL_TYPE_ALIGNMENT));
  NMP_ASSERT(newPairs);
  memset(newPairs, 0, sizeof(KVPair) * newHashSize);
  unsigned int* newBitfield = reinterpret_cast<unsigned int*>(NMPAllocatorMemAlloc(m_allocator, sizeof(unsigned int) * ((newHashSize + 31) >> 5), NMP_NATURAL_TYPE_ALIGNMENT));
  NMP_ASSERT(newBitfield);
  memset(newBitfield, 0, sizeof(unsigned int) * ((newHashSize + 31) >> 5));

  m_insertsTillRehash = scaleByLoadFactor(newHashSize);
  m_numUsedSlots = 0;

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

    newBitfield[tableIndex>>5] |= (1 << (tableIndex & 31));
    newPairs[tableIndex] = m_pairs[i];
  }

  m_hashSize = newHashSize;

  m_allocator->memFree(m_pairs);
  m_pairs = newPairs;

  m_allocator->memFree(m_occupiedSlots);
  m_occupiedSlots = newBitfield;
}

} // namespace NMP

#endif // NMP_HASHMAP_H
