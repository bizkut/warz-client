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
#include "nmtl/defs.h"
#include "nmtl/algorithm.h"
#include "nmtl/allocator.h"
#include "nmtl/deque.h"
#include "nmtl/fixed_free_list.h"
#include "nmtl/hashfunc.h"
#include "nmtl/hashmap_packed_chain.h"
#include "nmtl/hashmap_dense_dynamic.h"
#include "nmtl/bidirectional_ptrmap.h"
#include "nmtl/intrusive_double_list.h"
#include "nmtl/intrusive_single_list.h"
#include "nmtl/list.h"
#include "nmtl/pod_stack.h"
#include "nmtl/pod_vector.h"
#include "nmtl/set.h"
#include "nmtl/stack.h"
#include "nmtl/string.h"
#include "nmtl/type_utils.h"
#include "nmtl/unique_vector.h"
#include "nmtl/vector.h"
#include "nmtl/vector_iterators.h"

#include <malloc.h>

//----------------------------------------------------------------------------------------------------------------------
namespace nmtl
{

#if !NMTL_NO_DEFAULT_ALLOCATOR
// on MSVC, use the built-in aligned allocators
  #ifdef _MSC_VER

//----------------------------------------------------------------------------------------------------------------------
/// \brief provide base allocator using aligned_malloc on Windows
class NMTLCRTAllocator :
  public allocator
{
public:

  void* calloc(size_t num_bytes)
  {
    void* mem = ::_aligned_malloc(num_bytes, 16);
    memset(mem, 0, num_bytes);
    return mem;
  }
  void* alloc(size_t num_bytes)
  {
    return ::_aligned_malloc(num_bytes, 16);
  }
  void free(void* ptr)
  {
    ::_aligned_free(ptr);
  }
};

  #else // ... otherwise supply our own

//----------------------------------------------------------------------------------------------------------------------
/// \brief  provides a base level implementation using malloc/free
class NMTLCRTAllocator :
  public allocator
{
public:

  void* calloc(size_t num_bytes)
  {
    void* mem = alloc(num_bytes);
    memset(mem, 0, num_bytes);
    return mem;
  }
  void* alloc(size_t num_bytes)
  {
    unsigned char* _ptr = (unsigned char*)::malloc(num_bytes + 16);
    unsigned char* _zero = 0;
    ptrdiff_t address = _ptr - _zero;
    ptrdiff_t offset = 16U - (address & 15U);
    _ptr += offset;
    _ptr[-1] = (unsigned char)offset;
    return _ptr;
  }
  void free(void* ptr)
  {
    if (ptr)
    {
      unsigned char* _ptr = (unsigned char*)ptr;
      ::free(_ptr - _ptr[-1]);
    }
  }
};

  #endif // _MSC_VER

#endif

NMTLCRTAllocator* createDefaultAllocator()
{
  static NMTLCRTAllocator* ptr = 0;
  return (ptr ? ptr : ptr = new NMTLCRTAllocator());
}

//----------------------------------------------------------------------------------------------------------------------
/// global allocator
NMTL_EXPORT allocator* g_defaultAllocator = createDefaultAllocator();

//----------------------------------------------------------------------------------------------------------------------
NMTL_EXPORT void NMTLSetDefaultAllocator(allocator* _allocator)
{
  NMTL_ASSERT(_allocator);
  g_defaultAllocator = _allocator;
}
//----------------------------------------------------------------------------------------------------------------------
NMTL_EXPORT allocator* NMTLGetDefaultAllocator()
{
#if !NMTL_NO_DEFAULT_ALLOCATOR
  // On-demand allocation to avoid relying on static initialization order.
  if (g_defaultAllocator == 0)
    g_defaultAllocator = createDefaultAllocator();
#else
  NMTL_ASSERT(g_defaultAllocator);
#endif

  return g_defaultAllocator;
}
//----------------------------------------------------------------------------------------------------------------------
NMTL_EXPORT void NMTLDeleteDefaultAllocator()
{
  delete g_defaultAllocator;
  g_defaultAllocator  = 0;
}
//----------------------------------------------------------------------------------------------------------------------
NMTL_EXPORT void* NMTLAlloc(const size_t sz)
{
  NMTL_ASSERT(g_defaultAllocator);
  return g_defaultAllocator->alloc(sz);
}

//----------------------------------------------------------------------------------------------------------------------
NMTL_EXPORT void* NMTLCalloc(const size_t sz)
{
  NMTL_ASSERT(g_defaultAllocator);
  return g_defaultAllocator->calloc(sz);
}

//----------------------------------------------------------------------------------------------------------------------
NMTL_EXPORT void NMTLFree(void* mem)
{
  NMTL_ASSERT(g_defaultAllocator);
  return g_defaultAllocator->free(mem);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace nmtl
