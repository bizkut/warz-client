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
#ifndef NM_VECTOR_CONTAINER_H
#define NM_VECTOR_CONTAINER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMStaticFreeList.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::VectorContainer
/// \brief A vector class.  This class has a max size at creation and uses a freelist to store the links.
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
class VectorContainer
{
  typedef T value_type;

  //-------------------------------
  struct Link
  {
    Link* m_next;
    Link* m_prev;
    T    m_data;

    Link(): m_next(NULL), m_prev(NULL) {}
  };

public:

  //-------------------------------
  //
  class iterator
  {
  public:
    iterator(Link* link) { m_link = link; }

    bool operator!=(const iterator& other) const
    {
      return other.m_link != m_link;
    }

    bool operator==(const iterator& other) const
    {
      return other.m_link == m_link;
    }

    const T& operator*() const
    {
      return m_link->m_data;
    }

    T& operator*()
    {
      return m_link->m_data;
    }

    iterator& operator++()
    {
      // Preincrement version
      m_link = m_link->m_next;
      return (*this);
    }

    iterator operator++(int)
    {
      // Postincrement version
      iterator temp = m_link;
      m_link = m_link->m_next;
      return temp;
    }

    Link* m_link;
  };

  //-------------------------------
  static NM_INLINE VectorContainer* create(uint32_t maxEntries, NMP::MemoryAllocator* allocator);

  static NM_INLINE NMP::Memory::Format getMemoryRequirements(uint32_t maxEntries);
  static NM_INLINE VectorContainer* init(NMP::Memory::Resource& resource, uint32_t maxEntries);

  NM_INLINE bool push_back(const T& data);

  NM_INLINE iterator begin();
  NM_INLINE iterator end();
  NM_INLINE const iterator begin() const;
  NM_INLINE const iterator end() const;

  NM_INLINE bool empty() const;
  NM_INLINE uint32_t size() const;
  NM_INLINE bool isFreeListEmpty() const;

  NM_INLINE iterator insert(const iterator& position, const T& data);
  NM_INLINE iterator erase(const iterator& position);

  NM_INLINE void relocateLink(Link* link, int32_t offset);
  NM_INLINE void relocate();

  NM_INLINE void pop_back();

  NM_INLINE T& back();

  iterator find(const T& data);
  const iterator find(const T& data) const;

  void clear();

private:
  VectorContainer();
  ~VectorContainer();

  Link*                m_head; ///< Points to the first entry. NULL if the vector is empty
  Link*                m_tail; ///< Points to the last entry. NULL if the vector is empty
  NMP::StaticFreeList* m_freelist;
};

//----------------------------------------------------------------------------------------------------------------------
// VectorContainer functions.
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
VectorContainer<T>* VectorContainer<T>::create(
  uint32_t              maxEntries,
  NMP::MemoryAllocator* allocator)
{
  NMP_ASSERT(maxEntries > 0);
  NMP_ASSERT(allocator);
  NMP::Memory::Format format = getMemoryRequirements(maxEntries);

  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, format);

  // Check the allocation worked
  NMP_ASSERT(resource.ptr);

  VectorContainer<T>* result = init(resource, maxEntries);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NMP::Memory::Format VectorContainer<T>::getMemoryRequirements(uint32_t maxEntries)
{
  // You can't create a vector of 0 entries.
  NMP_ASSERT_MSG(maxEntries, "0-length VectorContainer is not allowed");

  NMP::Memory::Format result(sizeof(VectorContainer<T>), NMP_VECTOR_ALIGNMENT);

  // Reserve space for the freelist that will be used to store entries
  result += NMP::StaticFreeList::getMemoryRequirements(
              NMP::Memory::Format(sizeof(Link), NMP_NATURAL_TYPE_ALIGNMENT),
              maxEntries);

  // Align to 16 bytes to allow the structure to be DMAd
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
VectorContainer<T>* VectorContainer<T>::init(
  NMP::Memory::Resource& resource,
  uint32_t               maxEntries)
{
  NMP_ASSERT_MSG(maxEntries, "0-length VectorContainer is not allowed");
  NMP_ASSERT_MSG(resource.ptr, "NULL resource passed into VectorContainer::init");
  NMP_ASSERT_MSG(resource.format.size, "0-sized resource passed into VectorContainer::init");
  NMP_ASSERT_MSG(resource.format.alignment, "0-aligned resource passed into VectorContainer::init");

  resource.align(getMemoryRequirements(maxEntries));
  VectorContainer<T>* result = (VectorContainer<T>*)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(VectorContainer<T>), NMP_VECTOR_ALIGNMENT));

  result->m_freelist = NMP::StaticFreeList::init(
                         resource,
                         NMP::Memory::Format(sizeof(Link), NMP_NATURAL_TYPE_ALIGNMENT),
                         maxEntries);

  result->m_head = NULL;
  result->m_tail = NULL;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
bool VectorContainer<T>::push_back(const T& data)
{
  // Allocate a new entry
  Link* newLink = (Link*)m_freelist->allocateEntry();
  new (&(newLink->m_data)) T;

  // It's possible that this will fail so we should return a fail in this case
  if (!newLink)
  {
    return false;
  }

  newLink->m_next = NULL;
  newLink->m_data = data;

  if (m_tail)
  {
    // If the vector is not empty, link the tail to this new entry
    m_tail->m_next = newLink;
    newLink->m_prev = m_tail;
  }
  else
  {
    // If the vector is empty, this is now the head
    m_head = newLink;
    newLink->m_prev = NULL;
  }

  m_tail = newLink;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename VectorContainer<T>::iterator VectorContainer<T>::begin()
{
  return m_head;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename VectorContainer<T>::iterator VectorContainer<T>::end()
{
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
const typename VectorContainer<T>::iterator VectorContainer<T>::begin() const
{
  return m_head;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
const typename VectorContainer<T>::iterator VectorContainer<T>::end() const
{
  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename VectorContainer<T>::iterator VectorContainer<T>::insert(
  const typename VectorContainer<T>::iterator& position, const T& data)
{
  // Allocate a new entry
  Link* result = (Link*)m_freelist->allocateEntry();
  if (!result)
  {
    return NULL;
  }
  result->m_data = data;

  //------------------------
  // Insert at the end
  if (position == end())
  {
    result->m_next = NULL;
    if (m_tail)
    {
      // If the vector is not empty, link the tail to this new entry
      m_tail->m_next = result;
      result->m_prev = m_tail;
    }
    else
    {
      // If the vector is empty, this is now the head
      m_head = result;
      result->m_prev = NULL;
    }

    m_tail = result;
  }
  else
  {
    // Update the new entry links
    result->m_prev = position.m_link->m_prev;
    result->m_next = position.m_link;

    // Fixup the links back
    if (position.m_link->m_prev)
    {
      position.m_link->m_prev->m_next = result;
    }
    else
    {
      // This is now the head
      m_head = result;
    }

    // Fixup the links forward
    position.m_link->m_prev = result;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename VectorContainer<T>::iterator VectorContainer<T>::erase(
  const typename VectorContainer<T>::iterator& iterToErase)
{
  typename VectorContainer<T>::Link* result = iterToErase.m_link->m_next;

  // Fixup the links back
  if (iterToErase.m_link->m_prev)
  {
    iterToErase.m_link->m_prev->m_next = iterToErase.m_link->m_next;
  }
  else
  {
    m_head = iterToErase.m_link->m_next;
  }

  // Fixup the links forward
  if (iterToErase.m_link->m_next)
  {
    iterToErase.m_link->m_next->m_prev = iterToErase.m_link->m_prev;
  }
  else
  {
    m_tail = iterToErase.m_link->m_prev;
  }

  iterToErase.m_link->m_data.~value_type();
  m_freelist->deallocateEntry(iterToErase.m_link);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void VectorContainer<T>::clear()
{
  typename VectorContainer<T>::Link* linkToErase = m_head;

  for (; linkToErase; linkToErase = linkToErase->m_next)
  {
    linkToErase->m_data.~value_type();
    m_freelist->deallocateEntry(linkToErase);
  }

  m_head = NULL;
  m_tail = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void VectorContainer<T>::pop_back()
{
  if (m_tail)
  {
    typename VectorContainer<T>::iterator iterToErase(m_tail);
    erase(iterToErase);
  }

  return;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
T& VectorContainer<T>::back()
{
  NMP_ASSERT_MSG(m_tail, "NMP::VectorContainer::back called on an empty container");

  return m_tail->m_data;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
bool VectorContainer<T>::empty() const
{
  return m_head == NULL;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
uint32_t VectorContainer<T>::size() const
{
  uint32_t result = 0;

  typename VectorContainer<T>::iterator iter = begin();
  while (iter != end())
  {
    result++;
    iter++;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
bool VectorContainer<T>::isFreeListEmpty() const
{
  return m_freelist->isEmpty();
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void VectorContainer<T>::relocateLink(Link* link, int32_t offset)
{
  while (link)
  {
    if (link->m_prev)
      link->m_prev = (Link*)((char*)link->m_prev + offset);

    if (link->m_next)
      link->m_next = (Link*)((char*)link->m_next + offset);

    link = link->m_next;
  }

  return;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
void VectorContainer<T>::relocate()
{
  // Calculate the offset from the old location
  NMP::StaticFreeList* newFreelistLocation = (NMP::StaticFreeList*)(this + 1);
  newFreelistLocation = (NMP::StaticFreeList*)NMP::Memory::align(newFreelistLocation, NMP_NATURAL_TYPE_ALIGNMENT);

  int32_t offset = (int32_t)((char*)newFreelistLocation - (char*)m_freelist);

  m_freelist = (NMP::StaticFreeList*)((char*)m_freelist + offset);
  m_freelist->relocate();

  // Relocate all the pointers to entries
  if (m_head)
    m_head = (Link*)((char*)m_head + offset);
  if (m_tail)
    m_tail = (Link*)((char*)m_tail + offset);

  if (m_head)
    relocateLink(m_head, offset);

  return;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
typename VectorContainer<T>::iterator VectorContainer<T>::find(const T& data)
{
  typename VectorContainer<T>::iterator iter = begin();
  while (iter != end())
  {
    if (*iter == data)
    {
      break;
    }
    ++iter;
  }

  return iter;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
const typename VectorContainer<T>::iterator VectorContainer<T>::find(const T& data) const
{
  typename VectorContainer<T>::iterator iter = begin();
  while (iter != end())
  {
    if (*iter == data)
    {
      break;
    }
    ++iter;
  }

  return iter;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_VECTOR_CONTAINER_H
//----------------------------------------------------------------------------------------------------------------------
