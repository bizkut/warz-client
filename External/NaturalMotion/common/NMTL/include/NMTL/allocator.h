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
#ifndef NMTL_ALLOCATOR_H
#define NMTL_ALLOCATOR_H

//----------------------------------------------------------------------------------------------------------------------
#include "nmtl/defs.h"

//----------------------------------------------------------------------------------------------------------------------
namespace nmtl
{

//----------------------------------------------------------------------------------------------------------------------
/// \class  NMTL::allocator
/// \brief  defines an interface that allows you to provide your own custom memory
///         allocators. inherit from this interface and override the alloc and free
///         methods. There is no need to worry about memory alignment, that's all taken
///         care of internally within this class.
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
class allocator
{
public:
  /// \brief  dtor
  virtual ~allocator() {}

  /// \brief  override to allocate the requested amount of memory
  /// \param  num_bytes - the number of bytes to allocate
  virtual void* alloc(size_t num_bytes) = 0;

  /// \brief  override to allocate the requested amount of memory
  /// \param  num_bytes - the number of bytes to allocate
  virtual void* calloc(size_t num_bytes) = 0;

  /// \brief  override to free the memory
  /// \param  ptr - the memory block to free
  virtual void free(void* ptr) = 0;
};

/// \brief  sets the internal memory allocator used by NMTL
/// \param  _allocator - the memory allocator
NMTL_EXPORT void NMTLSetDefaultAllocator(allocator* _allocator);
NMTL_EXPORT allocator* NMTLGetDefaultAllocator();
NMTL_EXPORT void NMTLDeleteDefaultAllocator();
NMTL_EXPORT void* NMTLAlloc(size_t sz);
NMTL_EXPORT void* NMTLCalloc(size_t sz);
NMTL_EXPORT void NMTLFree(void* mem);

//----------------------------------------------------------------------------------------------------------------------
} // namespace nmtl

#ifdef __INTEL_COMPILER
  #pragma warning(disable: 588)
namespace nmtl
{
extern NMTL_EXPORT allocator* g_defaultAllocator;
}
  #define NMTL_DEFAULT_ALLOCATOR nmtl::g_defaultAllocator
#elif defined(_MSC_VER)
  #define NMTL_DEFAULT_ALLOCATOR nmtl::NMTLGetDefaultAllocator()
#elif defined(__GNUC__)
  #define NMTL_DEFAULT_ALLOCATOR nmtl::NMTLGetDefaultAllocator()
#endif

#endif // NMTL_ALLOCATOR_H
