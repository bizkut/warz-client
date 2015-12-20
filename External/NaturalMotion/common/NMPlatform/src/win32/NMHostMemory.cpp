// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"

#if !(defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))
  #error This NMHostMemory.cpp file is Windows only
#endif // (defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{
  void* mallocWrapped(size_t size) { return _aligned_malloc(size, 16); }
  void* mallocAlignedWrapped(size_t size, size_t alignment) { return _aligned_malloc(size, alignment); }
  void* callocWrapped(size_t size)
  {
    // This has to be done as there is no _aligned_calloc for some odd reason, and we're using _alligned_free.
    // Round up the size to nearest alignment boundary so that we can zero the padding too
    size = Memory::align(size, 4);
    void* result = _aligned_malloc(size, 4);
    for (uint32_t i = 0; i < size; i++)
    {
      ((char*)result)[i] = 0;
    }
    return result;
  }
  void freeWrapped(void* ptr) { _aligned_free(ptr); }

  void memcpyWrapped(void* dst, const void* src, size_t size) { memcpy(dst, src, size); }
  void memcpy128Wrapped(void* dst, const void* src, size_t size) { memcpy(dst, src, size); }

  size_t memSizeWrapped(void* ptr) { return ptr ? _aligned_msize(ptr, 1, 0) : 0; }

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
