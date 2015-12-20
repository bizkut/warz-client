// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
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
#ifndef NM_RING_BUFFER_H
#define NM_RING_BUFFER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::RingBuffer
/// \brief A templated FIFO style ring buffer of fixed maximum size.
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
class RingBuffer
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements(uint32_t maxEntries);
  static NM_INLINE RingBuffer* init(NMP::Memory::Resource& resource, uint32_t maxEntries);
  static NM_INLINE RingBuffer* create(NMP::MemoryAllocator* allocator, uint32_t maxEntries);

  /// \brief 
  NM_INLINE void locate();

  /// \brief 
  NM_INLINE void dislocate();

  /// \brief Re-create the internal pointers of this ring buffer if it has been moved in memory.
  NM_INLINE void relocate(void* location);
  NM_INLINE void relocate() { return relocate(this); }
  
  /// \brief Push an item onto the head of the ring buffer. Always successful even if the buffer is full.
  /// \return false if the buffer was full and we had to overwrite the tail to add this entry, true otherwise.
  NM_INLINE bool forcePushHead(T_bufferingType* toAdd);
  
  /// \brief Push an item onto the head of the ring buffer. Fails if the buffer is full.
  /// \return false if the buffer was full and the entry was not added, true otherwise.
  NM_INLINE bool pushHead(T_bufferingType* toAdd);

  /// \brief Remove an item from the tail of the ring buffer.
  /// \return true on success or false if the buffer is empty.
  NM_INLINE bool popTail();

  /// \brief Remove an item from the tail of the ring buffer.
  /// \param poppedValue A pointer to an item of the type stored in the ring buffer. Must not be NULL.
  /// \return true on success or false if the buffer is empty.
  NM_INLINE bool popTail(T_bufferingType* poppedValue);
  
  /// \brief Remove an item from the head of the ring buffer.
  /// \return true on success or false if the buffer is empty.
  NM_INLINE bool popHead();

  /// \brief Remove an item from the head of the ring buffer.
  /// \param poppedValue A pointer to an item of the type stored in the ring buffer. Must not be NULL.
  /// \return true on success or false if the buffer is empty.
  NM_INLINE bool popHead(T_bufferingType* poppedValue);

  /// \brief Get a pointer to the Nth entry from the head of the buffer.
  /// \return return false if there are not enough entries in the buffer, true otherwise
  NM_INLINE bool getNthEntryFromHead(T_bufferingType*& entryPtr, uint32_t entryIndex = 0);
  NM_INLINE bool getNthEntryFromHead(T_bufferingType& outputValue, uint32_t entryIndex = 0);
  
  /// \brief Get a pointer to the Nth entry from the tail of the buffer.
  /// \return return false if there are not enough entries in the buffer, true otherwise
  NM_INLINE bool getNthEntryFromTail(T_bufferingType*& entryPtr, uint32_t entryIndex = 0);
  NM_INLINE bool getNthEntryFromTail(T_bufferingType& outputValue, uint32_t entryIndex = 0);
  
  /// \brief Remove all active entries from the buffer.
  NM_INLINE void clear();
  
  /// \brief Test to see if the ring buffer is empty.
  /// \return True if the buffer is empty, false otherwise.
  bool isEmpty() { return m_numActiveEntries == 0; }

  /// \brief Test to see if the ring buffer is full.
  /// \return True if the buffer is full, false otherwise.
  bool isFull() { return (m_numActiveEntries == m_maxNumEntries); }

  /// \return How many entries are currently in use on the buffer.
  uint32_t getNumActiveEntries() { return m_numActiveEntries; }

  /// \return The maximum number of entries available on the buffer
  uint32_t getMaxNumEntries() { return m_maxNumEntries; }

private:

  RingBuffer() {};
  ~RingBuffer() {};

  uint32_t getNextPosition(uint32_t position) { return (position + 1) % m_maxNumEntries; }
  uint32_t getPrevPosition(uint32_t position) { return (m_maxNumEntries + position - 1) % m_maxNumEntries; }

  uint32_t         m_maxNumEntries;    ///< The maximum number of entries in the circular buffer.
  T_bufferingType* m_entriesBuffer;    ///< The storage array for the entries in the circular buffer.
  uint32_t         m_numActiveEntries; ///< The current number of entries in the circular buffer.
  uint32_t         m_headIndex;        ///< The index of the entries head.
  uint32_t         m_tailIndex;        ///< The index of the entries tail.
};

//----------------------------------------------------------------------------------------------------------------------
// RingBuffer functions.
//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
NMP::Memory::Format RingBuffer<T_bufferingType>::getMemoryRequirements(uint32_t maxEntries)
{
  NMP_ASSERT_MSG(maxEntries > 0, "Cannot create 0-entry RingBuffers");

  // Add space for the class itself.
  NMP::Memory::Format result(sizeof(RingBuffer<T_bufferingType>), NMP_VECTOR_ALIGNMENT);  // This structure is DMA-able

  // Add space for the buffer.
  result += NMP::Memory::Format(sizeof(T_bufferingType) * maxEntries, NMP_VECTOR_ALIGNMENT);
    
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
RingBuffer<T_bufferingType>* RingBuffer<T_bufferingType>::init(NMP::Memory::Resource& resource, uint32_t maxEntries)
{
  // The class itself.
  NMP::Memory::Format memReqsHdr(sizeof(RingBuffer<T_bufferingType>), NMP_VECTOR_ALIGNMENT);
  RingBuffer<T_bufferingType>* result = (RingBuffer<T_bufferingType>*) resource.alignAndIncrement(memReqsHdr);

  // The buffer.
  NMP::Memory::Format memReqsData(sizeof(T_bufferingType) * maxEntries, NMP_VECTOR_ALIGNMENT);
  result->m_entriesBuffer = (T_bufferingType*) resource.alignAndIncrement(memReqsData);
  
  result->m_maxNumEntries = maxEntries;
  result->m_headIndex = maxEntries - 1;
  result->m_tailIndex = 0;
  result->m_numActiveEntries = 0;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
RingBuffer<T_bufferingType>* RingBuffer<T_bufferingType>::create(NMP::MemoryAllocator* allocator, uint32_t maxEntries)
{   
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, getMemoryRequirements(maxEntries));
  NMP_ASSERT(resource.ptr);

  return init(resource, maxEntries);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
void RingBuffer<T_bufferingType>::locate()
{
  NMP::endianSwap(m_maxNumEntries);
  NMP::endianSwap(m_headIndex);
  NMP::endianSwap(m_tailIndex);
  NMP::endianSwap(m_numActiveEntries);

  // Buffer pointer.
  REFIX_SWAP_PTR(T_bufferingType, m_entriesBuffer);

  // Each of the buffer entries.
  uint32_t index = m_tailIndex;
  for (uint32_t i = 0; i < m_numActiveEntries; ++i)
  {
    NMP::endianSwap(m_entriesBuffer[index]);
    index = getNextPosition(index);
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
void RingBuffer<T_bufferingType>::dislocate()
{
  // Each of the buffer entries.
  uint32_t index = m_tailIndex;
  for (uint32_t i = 0; i < m_numActiveEntries; ++i)
  {
    NMP::endianSwap(m_entriesBuffer[index]);
    index = getNextPosition(index);
  }

  // Buffer pointer.
  UNFIX_SWAP_PTR(T_bufferingType, m_entriesBuffer);

  NMP::endianSwap(m_maxNumEntries);
  NMP::endianSwap(m_headIndex);
  NMP::endianSwap(m_tailIndex);
  NMP::endianSwap(m_numActiveEntries);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
void RingBuffer<T_bufferingType>::relocate(void* location)
{
  NMP_ASSERT(NMP_IS_ALIGNED(this, NMP_VECTOR_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, NMP_VECTOR_ALIGNMENT));
  Memory::Resource localResource = { location, RingBuffer<T_bufferingType>::getMemoryRequirements(m_maxNumEntries) };
  NMP::Memory::Format memReqsHdr(sizeof(RingBuffer<T_bufferingType>), NMP_VECTOR_ALIGNMENT);
  localResource.increment(memReqsHdr);
  m_entriesBuffer = (T_bufferingType*) localResource.align(NMP_VECTOR_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
void RingBuffer<T_bufferingType>::clear()
{
  m_headIndex = m_maxNumEntries - 1;
  m_tailIndex = 0;
  m_numActiveEntries = 0;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
bool RingBuffer<T_bufferingType>::forcePushHead(T_bufferingType* toAdd)
{
  m_headIndex = getNextPosition(m_headIndex);
  m_entriesBuffer[m_headIndex] = *toAdd;
  if (isFull())
  {
    // Overwrite the tail if the buffer is already full.
    m_tailIndex = getNextPosition(m_tailIndex);
    return false;
  }
  else
  {
    m_numActiveEntries++;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
bool RingBuffer<T_bufferingType>::pushHead(T_bufferingType* toAdd)
{
  // Don't add a new entry if the buffer is already full.
  if (!isFull())
  {
    m_headIndex = getNextPosition(m_headIndex);
    m_entriesBuffer[m_headIndex] = *toAdd;  
    m_numActiveEntries++;
    return true;
  }
 
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
bool RingBuffer<T_bufferingType>::popTail()
{ 
  if (!isEmpty())
  {
    m_tailIndex = getNextPosition(m_tailIndex);
    --m_numActiveEntries;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
bool RingBuffer<T_bufferingType>::popTail(T_bufferingType* poppedValue)
{ 
  if (!isEmpty())
  {
    *poppedValue = m_entriesBuffer[m_tailIndex];
    m_tailIndex = getNextPosition(m_tailIndex);
    --m_numActiveEntries;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
bool RingBuffer<T_bufferingType>::popHead()
{ 
  if (!isEmpty())
  {
    m_headIndex = getPrevPosition(m_headIndex);
    --m_numActiveEntries;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
bool RingBuffer<T_bufferingType>::popHead(T_bufferingType* poppedValue)
{ 
  // We cant pop from an empty buffer.
  if (!isEmpty())
  {
    *poppedValue = m_entriesBuffer[m_headIndex];
    m_headIndex = getPrevPosition(m_headIndex);
    --m_numActiveEntries;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
bool RingBuffer<T_bufferingType>::getNthEntryFromHead(T_bufferingType*& entryPtr, uint32_t entryIndex)
{
  if (entryIndex < m_numActiveEntries)
  {
    uint32_t index = (m_maxNumEntries + m_headIndex - entryIndex) % m_maxNumEntries;
    entryPtr = &(m_entriesBuffer[index]);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
bool RingBuffer<T_bufferingType>::getNthEntryFromHead(T_bufferingType& outputValue, uint32_t entryIndex)
{
  if (entryIndex < m_numActiveEntries)
  {
    uint32_t index = (m_maxNumEntries + m_headIndex - entryIndex) % m_maxNumEntries;
    outputValue = m_entriesBuffer[index];
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
bool RingBuffer<T_bufferingType>::getNthEntryFromTail(T_bufferingType*& entryPtr, uint32_t entryIndex)
{
  if (entryIndex < m_numActiveEntries)
  {
    uint32_t index = (m_tailIndex + entryIndex) % m_maxNumEntries;
    entryPtr = &(m_entriesBuffer[index]);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType>
bool RingBuffer<T_bufferingType>::getNthEntryFromTail(T_bufferingType& outputValue, uint32_t entryIndex)
{
  if (entryIndex < m_numActiveEntries)
  {
    uint32_t index = (m_tailIndex + entryIndex) % m_maxNumEntries;
    outputValue = m_entriesBuffer[index];
    return true;
  }
  return false;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_RING_BUFFER_H
//----------------------------------------------------------------------------------------------------------------------
