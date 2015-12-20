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

#include "MDFPrecompiled.h"
#include "ParserMemory.h"

ParserMemory                 *ActiveMemoryManager::g_inst = 0;
NMTLParserMemoryAllocator     ActiveMemoryManager::g_nmtlInst;

//----------------------------------------------------------------------------------------------------------------------
ParserMemory::ParserMemory(size_t initialSize, size_t reallocSize) :
  m_currentHeap(0),
  m_heapWalkIdx(0),
#ifdef NM_PARSEMEM_LOGSTAT
  m_allocCount(0),
#endif // NM_PARSEMEM_LOGSTAT
  m_subHeapSize(reallocSize),
  m_totalAllocSize(0)
{
  m_heapList = new HeapVector;
  m_heapList->reserve(64);
  newSubHeap(initialSize);
}

  //----------------------------------------------------------------------------------------------------------------------
ParserMemory::~ParserMemory()
{
  logStats();

  for (HeapVector::iterator it = m_heapList->begin(); it != m_heapList->end(); ++it)
  {
    VirtualFree((*it)->m_heap, 0, MEM_RELEASE);
    delete(*it);
  }
  delete m_heapList;
}

//----------------------------------------------------------------------------------------------------------------------
void ParserMemory::logStats()
{
#ifdef NM_PARSEMEM_LOGSTAT

  const int dbgBufSize = 256;
  char dbg[dbgBufSize];

  NMP_SPRINTF(dbg, dbgBufSize, "ParserMemory: reset/dtor [  alloc: %u = %u KB  ]\n", m_allocCount, m_totalAllocSize / 1024);
  OutputDebugStringA(dbg);

  NMP_SPRINTF(dbg, dbgBufSize, "  /-- %i heaps :\n", m_heapList->size());
  OutputDebugStringA(dbg);

  for (HeapVector::iterator it = m_heapList->begin(); it != m_heapList->end(); ++it)
  {
    NMP_SPRINTF(dbg, dbgBufSize, "  + heap sz: %u  max: %u \n", (*it)->m_heapUse, (*it)->m_heapSize);
    OutputDebugStringA(dbg);
  }

#endif // NM_PARSEMEM_LOGSTAT
}

//----------------------------------------------------------------------------------------------------------------------
void ParserMemory::resetForReuse()
{
  logStats();

  if (m_heapList->size() > 0)
  {
    for (HeapVector::iterator it = m_heapList->begin(); it != m_heapList->end(); ++it)
    {
      Heap* thisHeap = (*it);

      thisHeap->m_heapUse = 0;
#ifdef _DEBUG
      memset(thisHeap->m_heap, 0xCC, thisHeap->m_heapSize);
#endif // _DEBUG
    }
    m_currentHeap = m_heapList->at(0);
  }
  else
    m_currentHeap = 0;

#ifdef NM_PARSEMEM_LOGSTAT
  m_allocCount = 0;
#endif // NM_PARSEMEM_LOGSTAT
  m_totalAllocSize = 0;
  m_heapWalkIdx = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void ParserMemory::newSubHeap(size_t sz)
{
  Heap *tmpHeap = 0;
  for (unsigned int i=m_heapWalkIdx; i<m_heapList->size(); i++)
  {
    tmpHeap = (*m_heapList)[i];
    
    if (tmpHeap->m_heapUse + sz <= tmpHeap->m_heapSize)
    {
      m_currentHeap = tmpHeap;
      m_heapWalkIdx = i;
      return;
    }
  }

  Heap *h = new Heap;
  h->m_heap = (unsigned char*)VirtualAlloc(NULL, sz, MEM_COMMIT, PAGE_READWRITE);
  h->m_heapSize = sz;
  h->m_heapUse = 0;
#ifdef _DEBUG
  memset(h->m_heap, 0xCC, h->m_heapSize);
#endif // _DEBUG

  m_heapList->push_back(h);
  m_currentHeap = h;
  m_heapWalkIdx ++;
}

//----------------------------------------------------------------------------------------------------------------------
void* ParserMemory::alloc(size_t size)
{
  // optionally we can store a heap to pop back to using
  // after the function has returned a pointer; this is used
  // if the size requested is > the sub-heap size (see comments below)
  Heap *pushedHeap = 0;
  unsigned int pushedWalkIdx = 0;

  // new heap required?
  if ( m_currentHeap == 0 || 
      (m_currentHeap->m_heapUse + size) > m_currentHeap->m_heapSize)
  {
    // ensure the next sub-heap can contain the allocation
    if (size <= m_subHeapSize)
      newSubHeap(m_subHeapSize);
    else
    {
      // otherwise build a heap to contain this size of allocation,
      // do the work of returning the pointer, then rewind back to the 
      // other heap to use it up
      pushedHeap = m_currentHeap;
      pushedWalkIdx = m_heapWalkIdx;
      newSubHeap(size);
    }
  }

  // return memory block to use
  unsigned char* ptr = m_currentHeap->m_heap + m_currentHeap->m_heapUse;
  m_currentHeap->m_heapUse += size;

  // update class-local stats
#ifdef NM_PARSEMEM_LOGSTAT
  m_allocCount ++;
#endif // NM_PARSEMEM_LOGSTAT
  m_totalAllocSize += size;

  // pop back to a heap recorded above
  if (pushedHeap)
  {
    m_currentHeap = pushedHeap;
    m_heapWalkIdx = pushedWalkIdx;
  }

  return (void*)ptr;
}
