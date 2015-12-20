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
#ifndef NM_MAP_CONTAINER_H
#define NM_MAP_CONTAINER_H

#include "NMPlatform/NMVectorContainer.h"

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::MapContainer
/// \brief A map that is wraps a vector container - it is built to be straight forward and not being complicated
///        complicated (so no hashing etc, just a key->value map, no sorting)
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename V>
class MapContainer
{

private:

  //-------------------------------
  struct Data
  {
    K key;
    V value;

    Data() {}
    Data(const K& inKey, const V& inValue) : key(inKey), value(inValue) { };
    Data(const Data& o) : key(o.key), value(o.value) { };
    bool operator<(const Data& o) const { return key < o.key; };
  };

  //-------------------------------
  typedef VectorContainer<Data> TStorageVector;
  TStorageVector* m_Storage;

  MapContainer();
  ~MapContainer();

public:

  //-------------------------------
  //
  class iterator
  {
  public:
    iterator(const iterator& o): m_iteStorage(o.m_iteStorage) { };

    bool operator!=(const iterator& other) const
    {
      return  !(*this == other);
    }

    bool operator==(const iterator& other) const
    {
      return m_iteStorage == other.m_iteStorage
             || (m_iteStorage.m_link != NULL
                 && other.m_iteStorage.m_link != NULL
                 && other.operator * ().key == m_iteStorage.operator * ().key);
    }

    const Data& operator*() const
    {
      return m_iteStorage.operator * ();
    }

    Data& operator*()
    {
      return m_iteStorage.operator * ();
    }

    iterator& operator++()
    {
      // Preincrement version
      m_iteStorage++;
      return (*this);
    }

    iterator operator++(int)
    {
      // Postincrement version
      return m_iteStorage++;
    }

    friend class MapContainer<K, V>;
  protected:
    iterator(const typename TStorageVector::iterator& o): m_iteStorage(o) { };
    typename TStorageVector::iterator m_iteStorage;
  };

  //-------------------------------
  static NM_INLINE MapContainer* create(uint32_t maxEntries, NMP::MemoryAllocator* allocator);

  static NM_INLINE NMP::Memory::Format getMemoryRequirements(uint32_t maxEntries);
  static NM_INLINE MapContainer* init(NMP::Memory::Resource& resource, uint32_t maxEntries);

  NM_INLINE void insert(const K& key, const V& value);

  NM_INLINE iterator begin();
  NM_INLINE iterator end();
  NM_INLINE iterator find(const K& key);

  NM_INLINE bool empty();
  NM_INLINE uint32_t size();

  NM_INLINE iterator erase(const iterator& iterToErase);

  NM_INLINE void relocate();
};

//----------------------------------------------------------------------------------------------------------------------
// VectorContainer functions.
//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename V>
MapContainer<K, V>* MapContainer<K, V>::create(
  uint32_t              maxEntries,
  NMP::MemoryAllocator* allocator)
{
  NMP::Memory::Format format = getMemoryRequirements(maxEntries);

  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, format);

  // Check the allocation worked
  NMP_ASSERT(resource.ptr);

  MapContainer<K, V>* result = init(resource, maxEntries);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename V>
NMP::Memory::Format MapContainer<K, V>::getMemoryRequirements(uint32_t maxEntries)
{
  NMP_ASSERT_MSG(maxEntries, "0-length MapContainer is not allowed");

  NMP::Memory::Format result(sizeof(MapContainer<K, V>), NMP_VECTOR_ALIGNMENT);

  // gets DMA-aligned internally
  result += NMP::VectorContainer<Data>::getMemoryRequirements(maxEntries);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename V>
MapContainer<K, V>* MapContainer<K, V>::init(
  NMP::Memory::Resource& resource,
  uint32_t               maxEntries)
{
  NMP_ASSERT_MSG(maxEntries, "0-length MapContainer is not allowed");
  NMP_ASSERT_MSG(resource.ptr, "NULL resource passed into MapContainer::init");
  NMP_ASSERT_MSG(resource.format.size, "0-sized resource passed into MapContainer::init");
  NMP_ASSERT_MSG(resource.format.alignment, "0-aligned resource passed into MapContainer::init");

  resource.align(getMemoryRequirements(maxEntries));
  MapContainer<K, V>* result = (MapContainer<K, V>*)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(MapContainer<K, V>), NMP_VECTOR_ALIGNMENT));
  result->m_Storage = TStorageVector::init(resource, maxEntries);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename V>
void MapContainer<K, V>::insert(const K& key, const V& value)
{
  iterator check = find(key);
  if (check != end())
  {
    check.operator *().value = value;
    return;
  }

#ifdef NMP_ENABLE_ASSERTS
  bool retVal =
#endif
    m_Storage->push_back(Data(key, value));
  NMP_ASSERT_MSG(retVal, "Unable to insert new value - storage limit reached!");
}

//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename V>
typename MapContainer<K, V>::iterator MapContainer<K, V>::begin()
{
  return iterator(m_Storage->begin());
}

//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename V>
typename MapContainer<K, V>::iterator MapContainer<K, V>::end()
{
  return iterator(NULL);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename V>
typename MapContainer<K, V>::iterator NMP::MapContainer<K, V>::find(const K& key)
{
  for (typename TStorageVector::iterator iteStorage = m_Storage->begin();
       iteStorage != m_Storage->end();
       ++iteStorage)
  {
    if ((*iteStorage).key == key)
      return iterator(iteStorage);
  }
  return m_Storage->end();
}

//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename V>
typename MapContainer<K, V>::iterator MapContainer<K, V>::erase(
  const typename MapContainer<K, V>::iterator& iterToErase)
{
  return m_Storage->erase(iterToErase.m_iteStorage);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename V>
bool MapContainer<K, V>::empty()
{
  return m_Storage->empty();
}

//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename V>
uint32_t MapContainer<K, V>::size()
{
  return m_Storage->size();
}

//----------------------------------------------------------------------------------------------------------------------
template <typename K, typename V>
void MapContainer<K, V>::relocate()
{
  REFIX_PTR(TStorageVector, m_Storage);
  m_Storage->relocate();

  return;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_MAP_CONTAINER_H
//----------------------------------------------------------------------------------------------------------------------
