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
#if (!defined(NMTL_HASHMAP_PACKED_CHAIN_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_HASHMAP_PACKED_CHAIN_H
#endif

//----------------------------------------------------------------------------------------------------------------------
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
template <unsigned int TableSize, unsigned int DiscardLimit, typename KeyType, typename ValueType>
class NMTL_TEMPLATE_EXPORT hashmap_packed_chain;

// enable this define to have the table collect usage statistics
// note: this shouldn't be left on, it is provided for tuning and performance analysis
// #define NMTL_HASHMAP_PACKED_CHAIN_STATS

#ifndef NMTL_HAS_MSVC_INTRINSICS

// used when finding highest-bit-set when BSF/BSR unavailable
static const unsigned long MultiplyDeBruijnBitPosition[32] =
{
  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
};

#endif

#ifdef _MSC_VER
  #pragma warning(push)
// 4324: don't whine about padding when stored types require declspec(align())
  #pragma warning(disable:4324)
#endif // _MSC_VER

//----------------------------------------------------------------------------------------------------------------------
/// \details
///  An implementation of a direct-chaining hashtable container with a focus on reducing memory thrashing & improving
///  cache locality of stored items.
///
///  Instead of one-key/value-pair-per-entry, each hash bucket consists of an HChunk, storing up to 32 key/value pairs.
///  With a well-chosen table size this can reduce the amount of memory allocation/freeing dramatically. D$ locality
///  is also improved, with keys being packed together in the chunk.
///
///  Also, instead of directly freeing HChunks when there are no more slots in use, HChunks are pushed onto a
///  'dismissal stack' - the HChunk allocator can then pull items off this stack instead of allocating a new chunk,
///  encouraging memory re-use under heavy insert/erase load.
///
/// Usage:
///
///  + Intended use is mainly as a int-int style-lookup or similar (int-void, hashed-string-void, etc)
///  + Performance is biased towards insertion; Best usage in scenarios where high load of both insert / (find|erase).
///
///  + Does not support multiple value entries with the same key, but does not check for this (for performance reasons)
///      Use find(key) to test for existence before insert()ing if you are not sure that your key set is unique.
///
/// Template Parameters:
///
/// TableSize :    Size of the hash-table used to store entries; larger values means less collisions, although
///                  too high a value will mean lots of sparse storage (eg. Chunks with 1 value in, wasting the 31 other slots' worth)
///
///                  NOTE: TableSize must be a power-of-2. This is checked in the ctor.
///
/// DiscardLimit : How many chunks can sit in the dismissal list before we start just deleting them outright.
///                  A 'good' value will depend on the table usage and how much erase()ing you do! 256-512 for heavy use, 8-64 for less
///
/// Performance Notes:
///  + insertion is fast and remains fast no matter how crowded the table gets.
///  + retrieval is generally quick but can be degraded by too many collisions (value fragmentation); tuning TableSize
///      to the usage scenario is recommended to keep searching/erasing fast. If necessary, turn on the stats define to
///      examine how the table is performing - you want to be keeping collisions down if possible.
///  + memory usage under real-world load is ~1/4 that of std::map<>
///
/// KeyType and ValueType should be self-expanitory.
/// Whatever you use for KeyType, you will need a hashFunction<>() to hash it into an unsigned int. see hashfunc.h.
///
/// Type Notes:
///  + KeyType and ValueType must be POD; there is a compile-time check for this. the ideal use for this container
///      is a fast lookup map, eg from pointer-to-integer (or vice versa), string-to-pointer (where string is hashed to a UID), etc.
///  + Aligned types, such as __m128, can be used as either Key or Value. HChunk will inherit the alignment.
//----------------------------------------------------------------------------------------------------------------------
template <unsigned int TableSize, unsigned int DiscardLimit, typename KeyType, typename ValueType>
class NMTL_TEMPLATE_EXPORT hashmap_packed_chain
{
public:
  class const_value_walker;
  class value_walker;

  NMTL_INLINE hashmap_packed_chain(nmtl::allocator* allocator_ = NMTL_DEFAULT_ALLOCATOR);
  NMTL_INLINE ~hashmap_packed_chain();

  NMTL_INLINE void insert(KeyType key, const ValueType& value);
  NMTL_INLINE bool find(KeyType key, ValueType* value = NULL) const;  ///< passing NULL for value turns find() into exists()
  NMTL_INLINE bool erase(KeyType key);                                ///< returns true if the key was found and removed

  // call to purge the whole table, effectively resetting it;
  // note that chunks will still be pushed to the dismissal list, so
  // if you really want all the memory nuked, call dismissUnusedChunks() afterwards.
  // see the dtor for example.
  NMTL_INLINE void deleteAll();

  // call to flush dismissal list and reclaim memory
  NMTL_INLINE void dismissUnusedChunks();

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
  NMTL_INLINE const_value_walker constWalker() const { return const_value_walker(this); }
  NMTL_INLINE value_walker walker() { return value_walker(this); }

protected:
  struct HChunk
  {
    unsigned int    m_usage;    ///< bitfield that tracks usage of key/value slots

    // these are unpacked as we usually look at m_keys for the most part
    // so it's more cache efficient to have them sequential
    KeyType         m_keys[32];
    ValueType       m_values[32];

    // linkage
    HChunk*         m_prev;
    HChunk*         m_next;

    /// \brief find the highest/lowest bits set in m_usage
    ///        note that on versions without BSR/BSF ops, we revert to setting 'low' to zero
    ///        and using a bitswizzling trick to find 'high'
    NMTL_INLINE void getUsageBitRange(unsigned long& low, unsigned long& high)
    {
#ifdef NMTL_HAS_MSVC_INTRINSICS

      // 0000 1001 0100 0000
      // BSF->       <---BSR
      _BitScanForward(&low, m_usage);
      _BitScanReverse(&high, m_usage);

#else // NMTL_HAS_MSVC_INTRINSICS
      low = 0;

      high = m_usage;
      high |= high >> 1;
      high |= high >> 2;
      high |= high >> 4;
      high |= high >> 8;
      high |= high >> 16;
      high = (high >> 1) + 1;

      high = MultiplyDeBruijnBitPosition[(high * 0x077CB531UL) >> 27];
#endif // NMTL_HAS_MSVC_INTRINSICS
    }

    NMTL_INLINE void insert(KeyType key, const ValueType& value)
    {
#ifdef NMTL_HAS_MSVC_INTRINSICS
      // invert m_usage, we are searching for first un-set bit we can find
      unsigned long i;
      _BitScanForward(&i, static_cast<unsigned long>(~m_usage));
#else // NMTL_HAS_MSVC_INTRINSICS
      for (unsigned int i = 0; i < 32; i++)
      {
        if ((m_usage & (1 << i)) == 0)
        {
#endif // NMTL_HAS_MSVC_INTRINSICS

          m_keys[i] = key;
          m_values[i] = value;
          m_usage |= (1 << i);

#ifndef NMTL_HAS_MSVC_INTRINSICS
          return;
        }
      }
      NMTL_ASSERT(0);
#endif
    }
  };

  // delete a chunk - or rather, push it onto the dismissal stack
  // should there be space.
  NMTL_INLINE void deleteChunk(HChunk* chunk)
  {
    if (m_discardIndex < DiscardLimit)
    {
      chunk->m_usage = 0;

      // don't delete, push onto the 'discard' stack for dismissal
      // or re-use
      m_toDiscard[m_discardIndex] = chunk;
      m_discardIndex ++;
    }
    else
    {
#ifdef NMTL_HASHMAP_PACKED_CHAIN_STATS
      ++ m_dbgFrees;
      m_dbgMemoryFootprint -= sizeof(HChunk);
#endif // NMTL_HASHMAP_PACKED_CHAIN_STATS

      m_allocator->free(chunk);
    }
  }

  // pop a spare Chunk from the dismissal stack if available, or
  // create a fresh one if not
  NMTL_INLINE HChunk* createChunk()
  {
    if (m_discardIndex > 0)
    {
#ifdef NMTL_HASHMAP_PACKED_CHAIN_STATS
      ++ m_dbgReusedDismissedChunk;
#endif // NMTL_HASHMAP_PACKED_CHAIN_STATS

      -- m_discardIndex;
      
      HChunk* reused = m_toDiscard[m_discardIndex];
      assert(reused->m_usage == 0);
      reused->m_prev = 
        reused->m_next = 0;

      return reused;
    }
    else
    {
#ifdef NMTL_HASHMAP_PACKED_CHAIN_STATS
      ++ m_dbgAllocations;
      m_dbgMemoryFootprint += sizeof(HChunk);
#endif // NMTL_HASHMAP_PACKED_CHAIN_STATS

      HChunk* chk = reinterpret_cast<HChunk*>(m_allocator->alloc(sizeof(HChunk)));
      chk->m_usage = 0;
      chk->m_prev =
        chk->m_next = 0;

      return chk;
    }
  }

  // hash table storage
  HChunk*                 m_buckets[TableSize];

  // instead of just blithely free'ing Chunks, we push them onto the
  // dismissal stack. this is checked before any nmtl-malloc's are done, to see
  // if we can avoid memory allocation by re-use of old, empty Chunks.
  HChunk*                 m_toDiscard[DiscardLimit];
  unsigned int            m_discardIndex;

#ifdef NMTL_HASHMAP_PACKED_CHAIN_STATS
public:
  unsigned int            m_dbgInserts;               // number of calls to insert()
  unsigned int            m_dbgErases;                //    "         "     erase()
  unsigned int            m_dbgAllocations;           // number of new Chunk objects nmtl-malloc'd
  unsigned int            m_dbgFrees;                 // number of Chunks nmtl-free'd
  unsigned int            m_dbgCollisions;            // number of times a Chunk is chained to another (eg. collided with a filled Chunk)
  unsigned int            m_dbgAddedRoot;             // number of times a new root Chunk is added
  unsigned int            m_dbgRemovedRoot;           // number of times an empty root chunk was dismissed
  unsigned int            m_dbgReusedDismissedChunk;  // number of times the dismissal stack was used to avoid allocation
  unsigned int            m_dbgMemoryFootprint;       // amount of memory allocated/free'd, in bytes
#endif // NMTL_HASHMAP_PACKED_CHAIN_STATS

private:
  hashmap_packed_chain(const hashmap_packed_chain& rhs);  //lint !e1704 private ctor
  hashmap_packed_chain& operator=(const hashmap_packed_chain& rhs);   //lint !e1704 private ctor

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

  nmtl::allocator* m_allocator;

// functions to access internal state during unit-testing
#ifdef NMTL_UNIT_TESTING
public:
  NMTL_INLINE size_t getChunkSize() const { return sizeof(HChunk); }
  NMTL_INLINE size_t getChunkAlignment() const { return NMTL_ALIGNOF(HChunk); }
  NMTL_INLINE unsigned int getDiscardIndex() const { return m_discardIndex; }
#endif // NMTL_UNIT_TESTING
};

//----------------------------------------------------------------------------------------------------------------------
template <unsigned int TableSize, unsigned int DiscardLimit, typename KeyType, typename ValueType>
hashmap_packed_chain<TableSize, DiscardLimit, KeyType, ValueType>::hashmap_packed_chain(nmtl::allocator* allocator_) :
#ifdef NMTL_HASHMAP_PACKED_CHAIN_STATS
  m_dbgInserts(0),
  m_dbgErases(0),
  m_dbgAllocations(0),
  m_dbgFrees(0),
  m_dbgCollisions(0),
  m_dbgAddedRoot(0),
  m_dbgRemovedRoot(0),
  m_dbgReusedDismissedChunk(0),
  m_dbgMemoryFootprint(0),
#endif // NMTL_HASHMAP_PACKED_CHAIN_STATS
  m_discardIndex(0)
{
  m_allocator = allocator_;
  assert((TableSize != 0) && (!(TableSize & (TableSize - 1))));    // ensure pow-2
  void (*p)() = CompileTimeEnsurePODType; sizeof(p);                      // ensure POD
  (void)*p;

  memset(m_buckets, 0, sizeof(HChunk*) * TableSize);
}

//----------------------------------------------------------------------------------------------------------------------
template <unsigned int TableSize, unsigned int DiscardLimit, typename KeyType, typename ValueType>
hashmap_packed_chain<TableSize, DiscardLimit, KeyType, ValueType>::~hashmap_packed_chain()
{
  deleteAll();
  dismissUnusedChunks();
}

//----------------------------------------------------------------------------------------------------------------------
template <unsigned int TableSize, unsigned int DiscardLimit, typename KeyType, typename ValueType>
void hashmap_packed_chain<TableSize, DiscardLimit, KeyType, ValueType>::deleteAll()
{
  for (unsigned int i = 0; i < TableSize; i++)
  {
    HChunk* chunk = m_buckets[i];

#ifdef NMTL_HASHMAP_PACKED_CHAIN_STATS
    if (chunk)
      ++ m_dbgRemovedRoot;
#endif // NMTL_HASHMAP_PACKED_CHAIN_STATS

    while (chunk)
    {
      HChunk* nextChunk = chunk->m_next;
      deleteChunk(chunk);
      chunk = nextChunk;
    }

    m_buckets[i] = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <unsigned int TableSize, unsigned int DiscardLimit, typename KeyType, typename ValueType>
void hashmap_packed_chain<TableSize, DiscardLimit, KeyType, ValueType>::insert(KeyType key, const ValueType& value)
{
  unsigned int bkIndex = hfn::hashFunction(key);
  bkIndex &= (TableSize - 1);

  HChunk* chunk = m_buckets[bkIndex];

#ifdef NMTL_HASHMAP_PACKED_CHAIN_STATS
  ++ m_dbgInserts;
#endif // NMTL_HASHMAP_PACKED_CHAIN_STATS

  if (chunk == 0)
  {
#ifdef NMTL_HASHMAP_PACKED_CHAIN_STATS
    ++ m_dbgAddedRoot;
#endif // NMTL_HASHMAP_PACKED_CHAIN_STATS
    chunk = (m_buckets[bkIndex] = createChunk());
  }
  else
  {
    while (chunk->m_usage == 0xFFFFFFFF)
    {
      if (chunk->m_next)
      {
        chunk = chunk->m_next;
      }
      else
      {
#ifdef NMTL_HASHMAP_PACKED_CHAIN_STATS
        ++ m_dbgCollisions;
#endif // NMTL_HASHMAP_PACKED_CHAIN_STATS

        HChunk* newChunk = createChunk();

        // push new chunk to the start of the chain where
        // it can be used immediately
        newChunk->m_next = m_buckets[bkIndex];
        m_buckets[bkIndex]->m_prev = newChunk;

        m_buckets[bkIndex] = newChunk;

        chunk = newChunk;
        break;
      }
    }
  }

  chunk->insert(key, value);
}

//----------------------------------------------------------------------------------------------------------------------
template <unsigned int TableSize, unsigned int DiscardLimit, typename KeyType, typename ValueType>
bool hashmap_packed_chain<TableSize, DiscardLimit, KeyType, ValueType>::find(KeyType key, ValueType* value) const
{
  unsigned int bkIndex = hfn::hashFunction(key);
  bkIndex &= (TableSize - 1);

  HChunk* chunk = m_buckets[bkIndex];

  while (chunk && chunk->m_usage != 0)
  {
    unsigned long i = 0, highBit = 31;
    if (chunk->m_usage != 0xFFFFFFFF)
      chunk->getUsageBitRange(i, highBit);

    for (; i <= highBit; ++i)
    {
      if ((chunk->m_usage & (1 << i)))
      {
        if (hfn::hashKeysEqual<KeyType>(chunk->m_keys[i], key))
        {
          if (value != NULL)
            (*value) = chunk->m_values[i];

          return true;
        }
      }
    }

    chunk = chunk->m_next;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <unsigned int TableSize, unsigned int DiscardLimit, typename KeyType, typename ValueType>
bool hashmap_packed_chain<TableSize, DiscardLimit, KeyType, ValueType>::erase(KeyType key)
{
  unsigned int bkIndex = hfn::hashFunction(key);
  bkIndex &= (TableSize - 1);

  HChunk* chunk = m_buckets[bkIndex];

#ifdef NMTL_HASHMAP_PACKED_CHAIN_STATS
  ++ m_dbgErases;
#endif // NMTL_HASHMAP_PACKED_CHAIN_STATS

  while (chunk && chunk->m_usage != 0)
  {
    unsigned long i = 0, highBit = 31;
    if (chunk->m_usage != 0xFFFFFFFF)
      chunk->getUsageBitRange(i, highBit);

    for (; i <= highBit; ++i)
    {
      if ((chunk->m_usage & (1 << i)))
      {
        if (hfn::hashKeysEqual<KeyType>(chunk->m_keys[i], key))
        {
          chunk->m_usage &= ~(1 << i);

          // is the chunk unused now?
          if (chunk->m_usage == 0)
          {
            // unwire it from the bucket chain
            HChunk* prev = chunk->m_prev;
            HChunk* next = chunk->m_next;

            if (next)
              next->m_prev = prev;

            // handle case where we could be the root chunk
            if (prev)
              prev->m_next = next;
            else
            {
#ifdef NMTL_HASHMAP_PACKED_CHAIN_STATS
              ++ m_dbgRemovedRoot;
#endif // NMTL_HASHMAP_PACKED_CHAIN_STATS
              m_buckets[bkIndex] = next;
            }

            deleteChunk(chunk);
          }

          return true;
        }
      }
    }

    chunk = chunk->m_next;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <unsigned int TableSize, unsigned int DiscardLimit, typename KeyType, typename ValueType>
void hashmap_packed_chain<TableSize, DiscardLimit, KeyType, ValueType>::dismissUnusedChunks()
{
  for (unsigned int i = 0; i < m_discardIndex; i++)
  {
    m_allocator->free(m_toDiscard[i]);
  }

#ifdef NMTL_HASHMAP_PACKED_CHAIN_STATS
  m_dbgMemoryFootprint -= sizeof(HChunk) * m_discardIndex;
#endif // NMTL_HASHMAP_PACKED_CHAIN_STATS

  m_discardIndex = 0;
}

//----------------------------------------------------------------------------------------------------------------------
template <unsigned int TableSize, unsigned int DiscardLimit, typename KeyType, typename ValueType>
class NMTL_TEMPLATE_EXPORT hashmap_packed_chain<TableSize, DiscardLimit, KeyType, ValueType>::const_value_walker
{
  friend class hashmap_packed_chain;

public:
  NMTL_INLINE const ValueType& operator()()
  {
    return m_curChunk->m_values[m_foundOffset];
  }

  NMTL_INLINE const KeyType& key()
  {
    return m_curChunk->m_keys[m_foundOffset];
  }

  NMTL_INLINE bool next()
  {
    while (m_index <= TableSize)
    {
      // find next usable chunk
      while (m_curChunk == 0)
      {
        m_curChunk = m_hm->m_buckets[m_index];
        m_foundOffset = m_offset = 0;

        m_index ++;
        if (m_index > TableSize)
          return false;
      }

      // find slot within chunk chain
      while (m_curChunk != 0)
      {
        for (; m_offset < 32; m_offset++)
        {
          if ((m_curChunk->m_usage & (1 << m_offset)) != 0)
          {
            m_foundOffset = m_offset;
            m_offset ++;
            return true;
          }
        }

        m_curChunk = m_curChunk->m_next;
        m_foundOffset = m_offset = 0;
      }
    }

    return false;
  }

  NMTL_INLINE void reset()
  {
    m_index = 0;
    m_foundOffset = m_offset = 0;
    m_curChunk = 0;
    m_initialized = false;
  }

  NMTL_INLINE unsigned int index() const
  {
    return m_index;
  }

protected:
  NMTL_INLINE const_value_walker(const hashmap_packed_chain* hm) : m_hm(hm) { reset(); }

  const hashmap_packed_chain*   m_hm;
  const HChunk*                 m_curChunk;
  unsigned int                  m_index, m_foundOffset, m_offset;
  bool                          m_initialized;

private:
  const_value_walker();   //lint !e1704 private ctor
};

//----------------------------------------------------------------------------------------------------------------------
template <unsigned int TableSize, unsigned int DiscardLimit, typename KeyType, typename ValueType>
class NMTL_TEMPLATE_EXPORT hashmap_packed_chain<TableSize, DiscardLimit, KeyType, ValueType>::value_walker
{
  friend class hashmap_packed_chain;
public:

  NMTL_INLINE ValueType& operator()()
  {
    return m_curChunk->m_values[m_foundOffset];
  }

  NMTL_INLINE KeyType& key()
  {
    return m_curChunk->m_keys[m_foundOffset];
  }

  NMTL_INLINE bool next()
  {
    while (m_index <= TableSize)
    {
      // find next usable chunk
      while (m_curChunk == 0)
      {
        m_curChunk = m_hm->m_buckets[m_index];
        m_foundOffset = m_offset = 0;

        m_index ++;
        if (m_index > TableSize)
          return false;
      }

      // find slot within chunk chain
      while (m_curChunk != 0)
      {
        for (; m_offset < 32; m_offset++)
        {
          if ((m_curChunk->m_usage & (1 << m_offset)) != 0)
          {
            m_foundOffset = m_offset;
            m_offset ++;
            return true;
          }
        }

        m_curChunk = m_curChunk->m_next;
        m_foundOffset = m_offset = 0;
      }
    }

    return false;
  }

  NMTL_INLINE void reset()
  {
    m_index = 0;
    m_foundOffset = m_offset = 0;
    m_curChunk = 0;
    m_initialized = false;
  }

  NMTL_INLINE unsigned int index() const
  {
    return m_index;
  }

protected:
  NMTL_INLINE value_walker(hashmap_packed_chain* hm) : m_hm(hm) { reset(); }

  hashmap_packed_chain* m_hm;
  HChunk*               m_curChunk;
  unsigned int          m_index, m_foundOffset, m_offset;
  bool                  m_initialized;

private:
  value_walker();   //lint !e1704 private ctor
};

#ifdef _MSC_VER
  #pragma warning(pop) // pop disable of 4324
#endif // _MSC_VER

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif
#endif // NMTL_HASHMAP_PACKED_CHAIN_H
