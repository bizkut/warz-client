#pragma once
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

#include "nmtl/pod_vector.h"
#include "nmtl/allocator.h"
#include <malloc.h>

// define this to make the heap manager spew out the number of allocs, footprint, heap stats, etc on dtor
#define NM_PARSEMEM_LOGSTAT

#define MDF_HEAPCHECK    { _ASSERT(_heapchk() == _HEAPOK); _ASSERT(_CrtCheckMemory()); }

//----------------------------------------------------------------------------------------------------------------------
// NMTL allocator to target ParserMemory
class NMTLParserMemoryAllocator : public nmtl::allocator
{
public:
  inline void* calloc(size_t num_bytes);
  inline void* alloc(size_t num_bytes);
  inline void free(void*);
};

//----------------------------------------------------------------------------------------------------------------------
/**
 * Custom memory manager for the parser.
 * it can only allocate, deallocates do nothing. once finished with, all heaps are simply discarded.
 * it creates a heap of 'heapSize' bytes and allocates directly from that
 * pool without adding headers/footers to the returned pointers - once full, it allocates
 * new pools of at least 'subHeapSize' bytes. 
 *
 */
class ParserMemory
{
public:

  ParserMemory(size_t heapSize, size_t subHeapSize);
  ~ParserMemory();

  void* alloc   (size_t size);

  template <typename T>
  inline T* alloc() { return (T*)alloc(sizeof(T)); }

  template <typename T>
  inline T* allocArray(size_t size) { return (T*)alloc(sizeof(T) * size); }

  void  resetForReuse();

protected:

  void logStats();

  // create a new heap and assign to m_currentHeap
  void newSubHeap(size_t sz);

  // heap structure used to store everything we need to know about
  // a single storage block
  struct Heap
  {
    unsigned char*  m_heap;
    size_t          m_heapUse,
                    m_heapSize;
    unsigned int    m_pad;
  };
  typedef nmtl::pod_vector<Heap*> HeapVector;


  HeapVector*       m_heapList;         ///< list of used heaps
  Heap*             m_currentHeap;      ///< heap being used presently
  unsigned int      m_heapWalkIdx;      ///< 

#ifdef NM_PARSEMEM_LOGSTAT
  unsigned int      m_allocCount;       ///< number of alloc calls
#endif // NM_PARSEMEM_LOGSTAT

  size_t            m_subHeapSize,      ///< size of a new sub-heap, set in ctor
                    m_totalAllocSize;   ///< total memory usage footprint
};

//----------------------------------------------------------------------------------------------------------------------
// all memory allocation is routed through this instance pointer
// we support a single manager for simplicity and completeness
struct ActiveMemoryManager abstract
{
  // returns previously set instance
  inline static ParserMemory* set(ParserMemory* newInst) { ParserMemory* prev = g_inst; g_inst = newInst; return prev; }

  inline static ParserMemory* get() { return g_inst; }
  inline static NMTLParserMemoryAllocator* getForNMTL() { return &g_nmtlInst; }

private:
  ActiveMemoryManager();

  static ParserMemory                 *g_inst;
  static NMTLParserMemoryAllocator     g_nmtlInst;
};

//----------------------------------------------------------------------------------------------------------------------
struct ActiveMemoryManagerAutoScoped
{
  ActiveMemoryManagerAutoScoped(ParserMemory* setActive) :
    m_current(setActive)
  {
    m_previous = ActiveMemoryManager::set(m_current);
  }

  ~ActiveMemoryManagerAutoScoped()
  {
    ActiveMemoryManager::set(m_previous);
  }

private:
  ParserMemory   *m_previous,
                 *m_current;
};


//----------------------------------------------------------------------------------------------------------------------
#define parserMemoryAlloc(_type, _var) \
  assert(ActiveMemoryManager::get()); \
  _type *_var = ActiveMemoryManager::get()->alloc<_type>(); \
  new (_var) _type();

#define parserMemoryAllocDirect(_type, _var) \
  assert(ActiveMemoryManager::get()); \
  _var = ActiveMemoryManager::get()->alloc<_type>(); \
  new (_var) _type();

#define parserMemoryAllocArgs(_type, _var, ...) \
  assert(ActiveMemoryManager::get()); \
  _type *_var = ActiveMemoryManager::get()->alloc<_type>(); \
  new (_var) _type(__VA_ARGS__);

#define parserMemoryAllocDirectArgs(_type, _var, ...) \
  assert(ActiveMemoryManager::get()); \
  _var = ActiveMemoryManager::get()->alloc<_type>(); \
  new (_var) _type(__VA_ARGS__);


//----------------------------------------------------------------------------------------------------------------------
void* NMTLParserMemoryAllocator::calloc(size_t num_bytes)
{
  void* mem = ActiveMemoryManager::get()->alloc(num_bytes);
  memset(mem, 0, num_bytes);
  return mem;
}

//----------------------------------------------------------------------------------------------------------------------
void* NMTLParserMemoryAllocator::alloc(size_t num_bytes)
{
  unsigned char* _ptr = (unsigned char*)ActiveMemoryManager::get()->alloc(num_bytes+16);
  unsigned char* _zero =0;
  ptrdiff_t address = _ptr-_zero;
  ptrdiff_t offset = 16U - (address&15U);
  _ptr += offset;
  _ptr[-1] = (unsigned char)offset;
  return _ptr;
}

//----------------------------------------------------------------------------------------------------------------------
void NMTLParserMemoryAllocator::free(void*) 
{
}
