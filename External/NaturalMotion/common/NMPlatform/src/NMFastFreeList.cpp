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
#include "NMPlatform/NMFastFreeList.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// FastFreeList functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format FastFreeList::getMemoryRequirements(
  NMP::Memory::Format NMP_UNUSED(entryFormat),
  uint32_t            NMP_UNUSED(entriesPerChunk))
{
  NMP::Memory::Format result(sizeof(FastFreeList), NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::FastFreeList* FastFreeList::init(
  NMP::Memory::Resource resource,
  NMP::Memory::Format   entryFormat,
  uint32_t              entriesPerChunk,
  NMP::MemoryAllocator* allocator)
{
  NMP::FastFreeList* result = (NMP::FastFreeList*)resource.ptr;
  result->m_entryFormat = entryFormat;
  result->m_entriesPerChunk = entriesPerChunk;

  result->m_chunkFormat = NMP::Memory::Format(sizeof(FreeListChunk), NMP_NATURAL_TYPE_ALIGNMENT);

  result->m_chunkFormat += NMP::Memory::Format(
                             result->m_entriesPerChunk * NMP::Memory::align(sizeof(void*), NMP_NATURAL_TYPE_ALIGNMENT),
                             NMP_NATURAL_TYPE_ALIGNMENT);

  result->m_chunkFormat += NMP::Memory::Format(
                             result->m_entriesPerChunk * NMP::Memory::align(result->m_entryFormat.size, result->m_entryFormat.alignment),
                             result->m_entryFormat.alignment);

  result->m_actualCapacity = 0;

  result->m_chunks = NULL;

  // Fixup any initial chunks
  resource.increment(NMP::Memory::Format(sizeof(FastFreeList), NMP_VECTOR_ALIGNMENT));

  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
FastFreeList::FreeListChunk* FastFreeList::addChunk(NMP::Memory::Resource resource)
{
  // Create a new freelist chunk.
  NMP_ASSERT((resource.format.size >= m_chunkFormat.size) && (resource.format.alignment >= m_chunkFormat.alignment));

  FreeListChunk* newChunk = (FreeListChunk*)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(FreeListChunk), NMP_NATURAL_TYPE_ALIGNMENT));

  // Init the chunks free entry array.
  NMP::Memory::Format freeEntriesFmt(
    m_entriesPerChunk * NMP::Memory::align(sizeof(void*), NMP_NATURAL_TYPE_ALIGNMENT),
    NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(freeEntriesFmt);
  newChunk->freeEntries = (void**) resource.ptr;
  newChunk->numFreeEntries = m_entriesPerChunk;

#ifdef NMP_FREELIST_MEM_LOGGING
  newChunk->minFreeEntries = m_entriesPerChunk;
  newChunk->touches = 0;
  newChunk->numEmpties = 0;
#endif

  resource.increment(freeEntriesFmt);

  // Init the chunks entries buffer;
  resource.align(m_entryFormat);
  newChunk->m_buffer = resource.ptr;
  newChunk->m_bufferEnd = (char*)newChunk->m_buffer +
                          (NMP::Memory::align(m_entryFormat.size, m_entryFormat.alignment) * m_entriesPerChunk);
  newChunk->m_next = NULL;

  // Init each free entry element.
  for (uint32_t i = 0; i < m_entriesPerChunk; ++i)
  {
    newChunk->freeEntries[i] =
      (char*)newChunk->m_buffer + (NMP::Memory::align(m_entryFormat.size, m_entryFormat.alignment) * i);
  }

  // Add the chunk to the end of the list of chunks.
  if (m_chunks == 0)
  {
    m_chunks = newChunk;
  }
  else
  {
    FreeListChunk* existingChunk = m_chunks;
    while (existingChunk->m_next != NULL)
      existingChunk = existingChunk->m_next;
    existingChunk->m_next = newChunk;
  }

  m_actualCapacity += m_entriesPerChunk;

  return newChunk;
}

//----------------------------------------------------------------------------------------------------------------------
FastFreeList::FreeListChunk* FastFreeList::addChunk()
{
  // Create a new freelist chunk
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(m_allocator, m_chunkFormat);
  NMP_ASSERT(resource.ptr);
  return addChunk(resource);
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NMP_FREELIST_MEM_LOGGING
void* FastFreeList::allocateEntry(FreeListChunk** stress)
#else
void* FastFreeList::allocateEntry()
#endif
{
  FreeListChunk* chunk = m_chunks;

  while (chunk)
  {
    if (chunk->numFreeEntries != 0)
    {
      uint32_t lc = chunk->numFreeEntries--;
#ifdef NMP_FREELIST_MEM_LOGGING
      if (chunk->numFreeEntries < chunk->minFreeEntries)
      {
        chunk->minFreeEntries = chunk->numFreeEntries;
        *stress = chunk;
      }
#endif
      return chunk->freeEntries[--lc];
    }
    chunk = chunk->m_next;
  }

  chunk = addChunk();
  NMP_ASSERT(chunk);

  // Allocate the entry.
  NMP_ASSERT(chunk->numFreeEntries > 0);
  uint32_t lc = chunk->numFreeEntries--;

#ifdef NMP_FREELIST_MEM_LOGGING
  if (chunk->numFreeEntries < chunk->minFreeEntries)
  {
    chunk->minFreeEntries = chunk->numFreeEntries;
    *stress = chunk;
  }
#endif

  return chunk->freeEntries[--lc];
}

//----------------------------------------------------------------------------------------------------------------------
bool FastFreeList::deallocateEntry(void* ptr)
{
  // Find the chunk this entry came from
  NMP_ASSERT(m_chunks);
  FreeListChunk* chunk = m_chunks;

  while (chunk)
  {
    // was the pointer in this block?
    if ((ptr >= chunk->m_buffer) && (ptr < chunk->m_bufferEnd))
    {
      // Yes it was. Deallocate it.
      NMP_ASSERT(chunk->numFreeEntries < m_entriesPerChunk);
      chunk->freeEntries[chunk->numFreeEntries] = ptr;
      ++chunk->numFreeEntries;

#ifdef NMP_FREELIST_MEM_LOGGING
      ++chunk->touches;
      if (chunk->numFreeEntries == m_entriesPerChunk)
      {
        chunk->numEmpties++;
      }
#endif

      return true;
    }
    else
    {
      chunk = chunk->m_next;
    }
  }

  NMP_DEBUG_MSG("Reached the end of freelist chain while trying to free %p", ptr);
  NMP_ASSERT_FAIL();

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void FastFreeList::freeChunk(FastFreeList::FreeListChunk* chunk)
{
  if (chunk->m_next)
  {
    freeChunk(chunk->m_next);
  }

  m_allocator->memFree(chunk);
  chunk = NULL;

  return;
}

//----------------------------------------------------------------------------------------------------------------------
bool FastFreeList::freeChunks()
{
  if (m_chunks)
  {
    freeChunk(m_chunks);
  }
  m_chunks = NULL;

  return true;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
