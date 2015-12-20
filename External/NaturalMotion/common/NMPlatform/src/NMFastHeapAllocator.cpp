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
#include <stdio.h>
#include "NMPlatform/NMFastHeapAllocator.h"
#ifdef NM_HOST_CELL_SPU
  #include "NMPlatform/ps3/NMSPUDefines.h"
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// SPUFastHeapAllocator functions
//----------------------------------------------------------------------------------------------------------------------
#if defined(NM_HOST_CELL_SPU)
void* SPUFastHeapAllocator::memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL)
{
#ifdef NMP_MEMORY_LOGGING
  (void)file;
  (void)line;
  (void)func;
#endif // NMP_MEMORY_LOGGING
  
  // Check that there's space left for this allocation. 
  // If not then we fail.
  // It does not return NULL, as a standard memory allocator, for the following reasons:
  //  1. We always write runtime code that is dependent on allocation success. We do not write alternate paths to deal with failure.
  //  2. Returning a NULL requires a success condition on to every memory allocation even in release.
  //  3. We could be catching all allocation failures with an NMP_ASSERT, but on SPU we do not have space to add an NMP_SPU_VERBOSE_ASSERT_MSG after every request.
  //     This was causing very cryptic bugs that were taking a long time to trace on SPU.
  NMP_SPU_VERBOSE_ASSERT_MSG(((char*)(NMP::Memory::align(m_currentPtr, alignment) + size) <= m_memoryBlock + m_size), "SPUFastHeapAllocator::memAlloc FAIL.\n");

  m_currentPtr = (char*)NMP::Memory::align(m_currentPtr, alignment);

  void* result = m_currentPtr;
  m_currentPtr += size;

#ifdef NMP_MEMORY_LOGGING
  // Find a format that's the same as this alloc (if there is one)
  uint32_t index = 0;
  while (index < m_numRecords)
  {
    if (m_currentAllocCount[index].format.size == size &&
        m_currentAllocCount[index].format.alignment == alignment)
    {
      break;
    }
    index++;
  }

  // Create a new record if we didn't find a matching one
  if (index == m_numRecords)
  {
    m_currentAllocCount[index].count = 0;
    m_currentAllocCount[index].format.size = size;
    m_currentAllocCount[index].format.alignment = alignment;

    m_numRecords++;
  }

  // Add one to the current count for this allocation's format
  m_currentAllocCount[index].count++;

  // Log this allocation in the list so we can remove it correctly later.
  LoggedAllocation loggedAllocation;
  loggedAllocation.allocPtr = result;
  loggedAllocation.formatEntryIndex = index;
  loggedAllocation.file = file;
  loggedAllocation.line = line;
  loggedAllocation.function = func;

  m_currentAllocations->push_back(loggedAllocation);
#endif // NMP_MEMORY_LOGGING

  NMP_SPU_VERBOSE_ASSERT_MSG(
    (((result) >= _end) &&                              // Overrunning code before the start of heap memory.
     (((uint32_t)result) < getSpuTopOfStack()) &&       // Allocated over the stack.
     (((uint32_t)result + size) < getSpuTopOfStack())), // Allocated over the stack.
    "FastHeapAllocator:memAlloc fail3. Stack %x, alloc %p, size %zu\n", getSpuTopOfStack(), result, size);
  return result;
}
#endif // defined(NM_HOST_CELL_SPU)

//----------------------------------------------------------------------------------------------------------------------
// FastHeapAllocator functions
//----------------------------------------------------------------------------------------------------------------------
bool FastHeapAllocator::term()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void* FastHeapAllocator::memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL)
{
  // You are not allowed to allocate from a FastHeapAllocator while it has an active child
  NMP_ASSERT(m_childAllocator == NULL);

#ifdef NMP_MEMORY_LOGGING
  (void)file;
  (void)line;
  (void)func;
#endif // NMP_MEMORY_LOGGING

  char* alignedAddr = (char*)NMP::Memory::align(m_currentPtr, alignment);

  // Check that there's space left for this allocation. 
  // If not then we fail.
  // It does not return NULL, as a standard memory allocator, for the following reasons:
  //  1. We always write runtime code that is dependent on allocation success. We do not write alternate paths to deal with failure.
  //  2. Returning a NULL requires a success condition on to every memory allocation even in release.
  //  3. Avoids us having to catch all allocation failures with an NMP_ASSERT.
  NMP_ASSERT_MSG((alignedAddr + size) <= m_memoryEnd, "FastHeapAllocator::memAlloc FAIL.\n");

  logExternalAlloc(alignedAddr + size - m_currentPtr);
  void* result = alignedAddr;
  m_currentPtr = alignedAddr + size;

#ifdef NMP_MEMORY_LOGGING
  // Find a format that's the same as this alloc (if there is one)
  uint32_t index = 0;
  while (index < m_numRecords)
  {
    if (m_currentAllocCount[index].format.size == size &&
      m_currentAllocCount[index].format.alignment == alignment)
    {
      break;
    }
    index++;
  }

  // Create a new record if we didn't find a matching one
  if (index == m_numRecords)
  {
    NMP_ASSERT_MSG(m_numRecords < LOGGING_ALLOC_MAX_RECORDS, "No space to log memory allocation!");
    m_currentAllocCount[index].count = 0;
    m_currentAllocCount[index].format.size = size;
    m_currentAllocCount[index].format.alignment = alignment;

    m_numRecords++;
  }

  // Add one to the current count for this allocation's format
  m_currentAllocCount[index].count++;

  // Log this allocation in the list so we can remove it correctly later.
  LoggedAllocation loggedAllocation;
  loggedAllocation.allocPtr = result;
  loggedAllocation.formatEntryIndex = index;
  loggedAllocation.file = file;
  loggedAllocation.line = line;
  loggedAllocation.function = func;

  m_currentAllocations->push_back(loggedAllocation);
#endif // NMP_MEMORY_LOGGING

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void FastHeapAllocator::memFree(void* NMP_UNUSED(ptr))
{
  // This does nothing in the FastHeapAllocator
  return;
}

//----------------------------------------------------------------------------------------------------------------------
FastHeapAllocator* FastHeapAllocator::init(NMP::Memory::Resource& resource, uint32_t size, uint32_t alignment)
{
  NMP::Memory::Format format(sizeof(FastHeapAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  // Set up the vtable by calling in place constructor
  FastHeapAllocator* result = new(resource.ptr) FastHeapAllocator();
  resource.increment(format);

  format.size = size;
  format.alignment = alignment;

  resource.align(format);
  result->initStats();
  result->m_memoryBlock = (char*)resource.ptr;
  result->m_currentPtr = result->m_memoryBlock;
  result->m_memoryEnd = result->m_memoryBlock + size;
  result->m_size = size;
  result->m_parentAllocator = NULL;
  result->m_childAllocator = NULL;
  resource.increment(format);

#ifdef NMP_MEMORY_LOGGING
  result->m_currentAllocations = NMP::VectorContainer<LoggedAllocation>::init(resource, LOGGING_ALLOC_MAX_ALLOCS);
  result->m_numRecords = 0;
  result->m_maxAllocated = 0;
#endif // NMP_MEMORY_LOGGING

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
FastHeapAllocator* FastHeapAllocator::initChildAllocator(NMP::Memory::Resource& resource, uint32_t size, uint32_t alignment)
{
  NMP::Memory::Format format(sizeof(FastHeapAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  // Set up the vtable by calling in place constructor
  FastHeapAllocator* result = new(resource.ptr) FastHeapAllocator();
  resource.increment(format);

  format.size = size;
  format.alignment = alignment;

  resource.align(format);
  result->initStats();
  result->m_memoryBlock = (char*)resource.ptr;
  result->m_currentPtr = result->m_memoryBlock;
  result->m_memoryEnd = result->m_memoryBlock + size;
  result->m_size = size;
  result->m_parentAllocator = NULL;
  result->m_childAllocator = NULL;
  resource.increment(format);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
TempMemoryAllocator *FastHeapAllocator::createChildAllocator()
{
  // You cannot add a child allocator if there's already one there.
  NMP_ASSERT(m_childAllocator == NULL);

  // We create a new temp memory allocator within this one.  We want it to be as large as possible, so we need to work
  //  out how much memory we have available.
  size_t bytesLeft = (size_t)(m_memoryEnd - NMP::Memory::align(m_currentPtr, NMP_VECTOR_ALIGNMENT));

  // Remove the space required for the allocator itself
  size_t dataSize = bytesLeft - NMP::Memory::align(sizeof(FastHeapAllocator), NMP_VECTOR_ALIGNMENT);

  // Create the new allocator
  NMP::Memory::Format allocatorFormat = getChildAllocatorMemoryRequirements((uint32_t)dataSize, NMP_VECTOR_ALIGNMENT);
  char* currentPtr = getCurrentPtr();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(this, allocatorFormat);
  NMP_ASSERT(resource.ptr);
  FastHeapAllocator *result = initChildAllocator(resource, (uint32_t)dataSize, NMP_VECTOR_ALIGNMENT);

#ifdef NMP_MEMORY_LOGGING
  // We copy the parent allocator's allocation log into the child, as it's wasteful to create a new one.
  result->m_currentAllocations = m_currentAllocations;
  result->m_numRecords = 0;
  result->m_maxAllocated = 0;
#endif // NMP_MEMORY_LOGGING

  m_childAllocator = result;
  result->m_parentAllocator = this;
  // We restore this allocator's pointer to what it was before the child allocator was made, so that when we roll it
  // back, we will have the memory ready to use.
  m_currentPtr = currentPtr; 

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void FastHeapAllocator::destroyChildAllocator(TempMemoryAllocator *NMP_USED_FOR_ASSERTS(childAllocator))
{
  // Check it's our child allocator
  NMP_ASSERT(childAllocator == m_childAllocator);
  NMP_ASSERT(((FastHeapAllocator*)childAllocator)->m_parentAllocator == this);

  // Remove it
  m_childAllocator = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t FastHeapAllocator::reset()
{
  // You cannot reset an allocator which has an active child.
  NMP_ASSERT(m_childAllocator == NULL);
  size_t size = getUsedBytes();
  logExternalFree(size);
  NMP_ASSERT((getTotalExternalStats().getAllocedBytes() - getTotalExternalStats().getFreedBytes()) == 0);
  uint32_t result = (uint32_t)(m_currentPtr - m_memoryBlock);
  m_currentPtr = m_memoryBlock;

#ifdef NM_DEBUG
  static const uint32_t FILL_PATTERN = 0xCDCDCDCD;
  memset(m_memoryBlock, FILL_PATTERN, m_size);
#endif

#ifdef NMP_MEMORY_LOGGING
  m_numRecords = 0;
  NMP::VectorContainer<LoggedAllocation>::iterator iter = m_currentAllocations->begin();

  while (iter != m_currentAllocations->end())
  {
    iter = m_currentAllocations->erase(iter);
  }

  if (result > m_maxAllocated)
    m_maxAllocated = result;
#endif // NMP_MEMORY_LOGGING

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
size_t FastHeapAllocator::getUsedBytes()
{
  return (uint32_t)(m_currentPtr - m_memoryBlock);
}

//----------------------------------------------------------------------------------------------------------------------
void FastHeapAllocator::printAllocations()
{
#ifdef NMP_MEMORY_LOGGING
  const static uint32_t bufferSize = 1024 * 10;
  static char           output[bufferSize];
  char*                 buffer = output;
  uint32_t              bytesWritten = 0;

  bytesWritten += NMP_SPRINTF(buffer, bufferSize - bytesWritten, "\n");
  NMP_ASSERT(bytesWritten < bufferSize);
  buffer = &(output[bytesWritten]);

  bytesWritten += NMP_SPRINTF(
                    buffer,
                    bufferSize - bytesWritten,
                    "-------------------------------------------------------\n"
                    "NMP::FastHeapMemoryAllocator Maximum Bytes Allocated:  \n"
                    "-------------------------------------------------------\n"
                    "%i\n",
                    m_maxAllocated);
  NMP_ASSERT(bytesWritten < bufferSize);
  buffer = &(output[bytesWritten]);

  bytesWritten += NMP_SPRINTF(
                    buffer,
                    bufferSize - bytesWritten,
                    "-------------------------------------------------------\n"
                    "NMP::FastHeapMemoryAllocator current allocation counts:\n"
                    "-------------------------------------------------------\n"
                    "     SIZE, ALIGNMENT,    COUNT\n");
  NMP_ASSERT(bytesWritten < bufferSize);
  buffer = &(output[bytesWritten]);

  for (uint32_t i = 0; i < m_numRecords; i++)
  {
    bytesWritten += NMP_SPRINTF(
                      buffer,
                      bufferSize - bytesWritten,
                      "%9i, %9i, %8i\n",
                      m_currentAllocCount[i].format.size,
                      m_currentAllocCount[i].format.alignment,
                      m_currentAllocCount[i].count);
    NMP_ASSERT(bytesWritten < bufferSize);
    buffer = &(output[bytesWritten]);
  }

  bytesWritten += NMP_SPRINTF(
                    buffer,
                    bufferSize - bytesWritten,
                    "-------------------------------------------------\n"
                    "NMP::FastHeapMemoryAllocator current allocations:\n"
                    "-------------------------------------------------\n"
                    "      ADDR,     SIZE,  ALIGNMENT,                                 FUNCTION, FILE AND LINE\n");
  NMP_ASSERT(bytesWritten < bufferSize);
  buffer = &(output[bytesWritten]);

  NMP::VectorContainer<LoggedAllocation>::iterator iter = m_currentAllocations->begin();
  while (iter != m_currentAllocations->end())
  {
    // First see if we have enough space to avoid overrunning the buffer
    static char tempBuffer[1024];
    uint32_t numBytes = NMP_SPRINTF(
                      tempBuffer,
                      1024,
                      "%10p, %9i, %9i, %40s, %s, %i\n",
                      (*iter).allocPtr,
                      m_currentAllocCount[(*iter).formatEntryIndex].format.size,
                      m_currentAllocCount[(*iter).formatEntryIndex].format.alignment,
                      (*iter).function,
                      (*iter).file,
                      (*iter).line);

    if (numBytes + bytesWritten > bufferSize)
    {
      // We will overrun the buffer so exit
      NMP_DEBUG_MSG("Warning: FastHeapAllocator overran output buffer.");
      break;
    }

    bytesWritten += NMP_SPRINTF(
                      buffer,
                      bufferSize - bytesWritten,
                      "%s", tempBuffer);
    buffer = &(output[bytesWritten]);

    iter++;
  }

  NMP_DEBUG_MSG(output);
#endif //NMP_MEMORY_LOGGING
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
