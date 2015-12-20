// HDD @ NM 16-03-07

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

#include "NMTinyFastHeap.h"

#ifdef _MSC_VER
#pragma warning(disable:4100)
#endif

namespace NM
{

TiXmlFastHeap*                  gTiXmlFastHeap = 0;

// define this to make the heap manager spew out the number of allocs, footprint, heap stats, etc on dtor / reset
#if 0
#define TIXML_LOG_ON_DTOR_AND_RESET
#endif

TiXmlFastHeap::TiXmlFastHeap(size_t initialSize, size_t reallocSize) :
  m_currentHeap(0),
  m_heapWalkIdx(0),
  m_allocCount(0),
  m_deallocCount(0),
  m_subHeapSize(reallocSize),
  m_totalAllocSize(0)
{
  m_heapList = new HeapVector;
  m_heapList->reserve(64);
  newSubHeap(initialSize);
}

TiXmlFastHeap::~TiXmlFastHeap()
{
#ifdef TIXML_LOG_ON_DTOR_AND_RESET
  logStats("dtor");
#endif // TIXML_LOG_ON_DTOR_AND_RESET

  for (HeapVector::iterator it = m_heapList->begin(); it != m_heapList->end(); ++it)
  {
    #ifdef __linux__
    free((*it)->m_heap);
    #else
    VirtualFree((*it)->m_heap, 0, MEM_RELEASE);
    #endif
    delete(*it);
  }
  delete m_heapList;
}

void TiXmlFastHeap::logStats(const char* logTag)
{
  const int dbgBufSize = 256;
  char dbg[dbgBufSize];

  _snprintf(dbg, dbgBufSize, "TiXmlFastHeap: %s [  alloc: %u = %u KB  |  dealloc: %u  ]\n", logTag, m_allocCount, m_totalAllocSize / 1024, m_deallocCount);
  OutputDebugStringA(dbg);

  _snprintf(dbg, dbgBufSize, "  /-- %i heaps :\n", m_heapList->size());
  OutputDebugStringA(dbg);

  for (HeapVector::iterator it = m_heapList->begin(); it != m_heapList->end(); ++it)
  {
    _snprintf(dbg, dbgBufSize, "  + heap sz: %u  max: %u \n", (*it)->m_heapUse, (*it)->m_heapSize);
    OutputDebugStringA(dbg);
  }
}

void TiXmlFastHeap::resetForReuse()
{
#ifdef TIXML_LOG_ON_DTOR_AND_RESET
  logStats("reset");
#endif // TIXML_LOG_ON_DTOR_AND_RESET

  if (m_heapList->size() > 0)
  {
    for (HeapVector::iterator it = m_heapList->begin(); it != m_heapList->end(); ++it)
    {
      (*it)->m_heapUse = 0;
    }
    m_currentHeap = m_heapList->at(0);
  }
  else
    m_currentHeap = 0;

  m_allocCount = 0;
  m_deallocCount = 0;
  m_totalAllocSize = 0;
  m_heapWalkIdx = 0;
}

void TiXmlFastHeap::newSubHeap(size_t sz)
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
  #ifdef __linux__
  h->m_heap = (unsigned char*)malloc(sz);
  #else
  h->m_heap = (unsigned char*)VirtualAlloc(NULL, sz, MEM_COMMIT, PAGE_READWRITE);
  #endif
  h->m_heapSize = sz;
  h->m_heapUse = 0;

  m_heapList->push_back(h);
  m_currentHeap = h;
  m_heapWalkIdx ++;
}

void* TiXmlFastHeap::alloc(size_t size)
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

      // we could up m_subHeapSize to 'size' here...
    }
  }

  // return memory block to use
  unsigned char* ptr = m_currentHeap->m_heap + m_currentHeap->m_heapUse;
  m_currentHeap->m_heapUse += size;

  // update class-local stats
  m_allocCount ++;
  m_totalAllocSize += size;

  // pop back to a heap recorded above
  if (pushedHeap)
  {
    m_currentHeap = pushedHeap;
    m_heapWalkIdx = pushedWalkIdx;
  }

  return (void*)ptr;
}

void TiXmlFastHeap::dealloc(void* ptr)
{
  m_deallocCount ++;
}

} // namespace NM
