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
#ifndef NMP_MEMORY_H
#define NMP_MEMORY_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include <stddef.h>

#if defined(NM_COMPILER_MSVC) || defined(NM_COMPILER_INTEL)
  #include <memory.h>
  #include <malloc.h>
  #include <limits.h>
  #include <new.h>
#endif
//----------------------------------------------------------------------------------------------------------------------

/// Enables the logging of all memory allocations that occur through NMP::Memory.
/// Memory logging is not supported on SPU due to the memory overhead involved.
#ifndef NM_HOST_CELL_SPU
  #define NMP_MEMORY_LOGGINGx
  #ifdef NMP_MEMORY_LOGGING
    #define NMP_MEMORY_LOGGING_HIGH_VERBOSITYx // Outputs a debug message for every allocation and free.
  #endif // NMP_MEMORY_LOGGING
#endif // NM_HOST_CELL_SPU

#define NMP_MEMORY_STATSx

/// Utilities for converting pointers to offsets and visa versa.
/// Generally used when wanting to store assets, containing pointers, to file
/// or when wanting to relocate an asset in memory.

/// \def UNFIX_PTR
/// Turn a pointer into a byte offset from a class instances base memory location.
#define UNFIX_PTR(type, ptr) {NMP_ASSERT(ptr != NULL); ptr = (type*)((ptrdiff_t) ptr - (ptrdiff_t)this);}

/// \def REFIX_PTR
/// Turn a byte offset from a class instances base memory location into a pointer.
#define REFIX_PTR(type, ptr) {ptr = (type*)((ptrdiff_t) ptr + (ptrdiff_t)this);}

/// \def UNFIX_PTR_RELATIVE
/// Turn a pointer into a byte offset from a specified base memory location.
#define UNFIX_PTR_RELATIVE(type, ptr, base) {NMP_ASSERT(ptr != NULL); ptr = (type*)((ptrdiff_t) ptr - (ptrdiff_t)base);}

/// \def REFIX_PTR_RELATIVE
/// Turn a byte offset from specified base memory location into a pointer.
#define REFIX_PTR_RELATIVE(type, ptr, base) {ptr = (type*)((ptrdiff_t) ptr + (ptrdiff_t)base);}

/// \def UNFIX_SWAP_PTR
/// Unfix a pointer to an offset, then endian swap it.
#define UNFIX_SWAP_PTR(type, ptr) {UNFIX_PTR(type, ptr); NMP::endianSwap(ptr);}

/// \def REFIX_SWAP_PTR
/// Endian swap a pointer offset, then re-fix it.
#define REFIX_SWAP_PTR(type, ptr) {NMP::endianSwap(ptr); REFIX_PTR(type, ptr);}

/// \def UNFIX_SWAP_PTR_RELATIVE
/// Unfix a pointer to an offset from specified base memory location, then endian swap it.
#define UNFIX_SWAP_PTR_RELATIVE(type, ptr, base) {UNFIX_PTR_RELATIVE(type, ptr, base); NMP::endianSwap(ptr);}

/// \def REFIX_SWAP_PTR_RELATIVE
/// Endian swap a pointer offset from specified base memory location, then re-fix it.
#define REFIX_SWAP_PTR_RELATIVE(type, ptr, base) {NMP::endianSwap(ptr); REFIX_PTR_RELATIVE(type, ptr, base);}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NMP_MEMORY_LOGGING
  #define NMP_MEMORY_TRACKING_ARGS , __FILE__, __LINE__, __FUNCTION__
  #define NMP_MEMORY_TRACKING_DECL , const char *file, uint32_t line, const char *func
  #define NMP_MEMORY_TRACKING_PASS_THROUGH , file, line, func

  #define NMP_MEMORY_LOGGING_CONFIG_NAME loggingC
  #define NMP_MEMORY_LOGGING_PASS_THROUGH , NMP_MEMORY_LOGGING_CONFIG_NAME
  #define NMP_MEMORY_LOGGING_DECL , const NMP::Memory::MemoryLoggingConfig* NMP_MEMORY_LOGGING_CONFIG_NAME

#else
  #define NMP_MEMORY_TRACKING_ARGS
  #define NMP_MEMORY_TRACKING_DECL
  #define NMP_MEMORY_TRACKING_PASS_THROUGH

  #define NMP_MEMORY_LOGGING_DECL
  #define NMP_MEMORY_LOGGING_PASS_THROUGH
#endif

/// Logged memory defines. Use these defines in place of directly calling NMP::Memory::allocateFromFormat etc, and
/// you will be able to detect memory leaks and get a readout of where the allocation was made.

/// Direct NMP::Memory allocation macros
///
#define NMPMemoryAlloc(size) NMP::Memory::memAlloc(size NMP_MEMORY_TRACKING_ARGS)
#define NMPMemoryAllocAligned(size, alignment) NMP::Memory::memAllocAligned(size, alignment NMP_MEMORY_TRACKING_ARGS)
#define NMPMemoryAllocateFromFormat(format) NMP::Memory::allocateFromFormat(format NMP_MEMORY_TRACKING_ARGS)
#define NMPMemoryCalloc(size) NMP::Memory::memCalloc(size NMP_MEMORY_TRACKING_ARGS)

/// Indirect NMP::MemoryAllocator memory allocation macros
///
/// use NMPAllocatorMemAlloc(NMP::MemoryAllocator*, size_t, size_t);
#define NMPAllocatorMemAlloc(allocator, size, alignment) (allocator)->memAlloc(size, alignment NMP_MEMORY_TRACKING_ARGS)

/// use NMPAllocatorAllocateFromFormat(NMP::MemoryAllocator*, NMP::Memory::Format)
#define NMPAllocatorAllocateFromFormat(allocator, format) (allocator)->allocateFromFormat(format NMP_MEMORY_TRACKING_ARGS)

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
// Endian swapping functions
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE void forceEndianSwap(T& data)
{
  uint32_t size = sizeof(T);

  // Early out for byte-sized types
  if (size == 1)
  {
    return;
  }
  if (size == 2)
  {
    uint16_t* dataPtr = (uint16_t*)&data;
    *dataPtr = (((*dataPtr << 8) & 0xff00) | ((*dataPtr >> 8) & 0x00ff));
    return;
  }
  if (size == 4)
  {
    uint32_t* dataPtr = (uint32_t*)&data;

    *dataPtr =
      ((*dataPtr << 24) & 0xff000000) |
      ((*dataPtr <<  8) & 0x00ff0000) |
      ((*dataPtr >>  8) & 0x0000ff00) |
      ((*dataPtr >> 24) & 0x000000ff);
    return;
  }

  // The case for larger types may be platform specific
  NMP_ASSERT((size % 4) == 0);

  uint32_t* words = (uint32_t*)&data;
  for (uint32_t i = 0 ; i < (size / sizeof(uint32_t)) ; ++i)
  {
    words[i] =
      ((words[i] << 24) & 0xff000000) |
      ((words[i] <<  8) & 0x00ff0000) |
      ((words[i] >>  8) & 0x0000ff00) |
      ((words[i] >> 24) & 0x000000ff);
  }
  return;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE void forceEndianSwapArray(T* data, uint32_t numElements)
{
  for (uint32_t i = 0 ; i < numElements ; ++i)
  {
    forceEndianSwap(data[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Endian swapping when building objects for other platforms
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE void endianSwap(T& data)
{
#if defined(NM_ENDIAN_SWAP)
  forceEndianSwap(data);
#else
  (void) data;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE void endianSwapArray(T* data, uint32_t numElements)
{
#if defined(NM_ENDIAN_SWAP)
  forceEndianSwapArray(data, numElements);
#else
  (void) data;
  (void) numElements;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// Endian swapping for passing objects over network connections. All network traffic is big endian.
//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE void netEndianSwap(T& data)
{
#if NM_HOST_BIGENDIAN == 0
  forceEndianSwap(data);
#else
  (void) data;
#endif // NM_HOST_BIGENDIAN == 0
}

//----------------------------------------------------------------------------------------------------------------------
template <typename T>
NM_INLINE void netEndianSwapArray(T* data, uint32_t numElements)
{
#if NM_HOST_BIGENDIAN == 0
  forceEndianSwapArray(data, numElements);
#else
  (void) data;
  (void) numElements;
#endif // NM_HOST_BIGENDIAN == 0
}

//----------------------------------------------------------------------------------------------------------------------
// Endian swapping data of known type size but unknown type.
//----------------------------------------------------------------------------------------------------------------------
void forceEndianSwap(void* data, uint32_t size);
void endianSwap(void* data, uint32_t size);
void forceEndianSwapArray(void* data, uint32_t numElements, uint32_t elementSize);
void endianSwapArray(void* data, uint32_t numElements, uint32_t elementSize);

//----------------------------------------------------------------------------------------------------------------------
/// \namespace Memory
/// \brief Memory configuration Options.
///
/// %Allows use of custom allocators in the appropriate classes.
//----------------------------------------------------------------------------------------------------------------------

namespace Memory
{

extern bool initialised;

extern size_t totalBytes;

//----------------------------------------------------------------------------------------------------------------------
/// \struct NMP::Memory::Stats
/// \brief Structure to describe memory allocation statistics.
//----------------------------------------------------------------------------------------------------------------------
class Stats
{
public:
  NM_INLINE size_t getAllocedBytes() const
  {
#ifdef NMP_MEMORY_STATS
    return m_allocedBytes;
#else
    return 0;
#endif // NMP_MEMORY_STATS
  }

  NM_INLINE size_t getAllocCount() const
  {
#ifdef NMP_MEMORY_STATS
    return m_allocCount;
#else
    return 0;
#endif // NMP_MEMORY_STATS
  }

  NM_INLINE size_t getFreedBytes() const
  {
#ifdef NMP_MEMORY_STATS
    return m_freedBytes;
#else
    return 0;
#endif // NMP_MEMORY_STATS
  }

  NM_INLINE size_t getFreeCount() const
  {
#ifdef NMP_MEMORY_STATS
    return m_freeCount;
#else
    return 0;
#endif // NMP_MEMORY_STATS
  }

  Stats()
  {
    reset();
  }

  NM_INLINE void logAlloc(size_t size)
  {
#ifdef NMP_MEMORY_STATS
    m_allocedBytes += size;
    ++ m_allocCount;
#else
    (void)size;
#endif // NMP_MEMORY_STATS
  }

  NM_INLINE void logFree(size_t size)
  {
#ifdef NMP_MEMORY_STATS
    m_freedBytes += size;
    ++ m_freeCount;
#else
    (void)size;
#endif // NMP_MEMORY_STATS
  }

  NM_INLINE void reset()
  {
#ifdef NMP_MEMORY_STATS
    m_allocedBytes = m_allocCount = m_freedBytes = m_freeCount = 0;
#endif // NMP_MEMORY_STATS
  }

private:
#ifdef NMP_MEMORY_STATS
  size_t m_allocedBytes;
  size_t m_allocCount;
  size_t m_freedBytes;
  size_t m_freeCount;
#endif // NMP_MEMORY_STATS
};

//----------------------------------------------------------------------------------------------------------------------
/// \struct NMP::Format
/// \brief Structure to describe a block of memory's format.
//----------------------------------------------------------------------------------------------------------------------
struct Format
{
  explicit Format() : size(0), alignment(NMP_NATURAL_TYPE_ALIGNMENT) {}
  explicit Format(size_t s, size_t a = NMP_NATURAL_TYPE_ALIGNMENT) : size(s), alignment(a) {}

  NM_FORCEINLINE void set(size_t s, size_t a) { size = s; alignment = a; }

  /// Increment format size to be aligned to the alignment value.
  NM_FORCEINLINE void align();

  /// Check if this format matches the other.
  bool operator == (const Format& other) const;

  /// Expand this Format by the size specified in other, taking into account alignment.
  void operator += (const Format& other);

  /// Expand this Format to be large enough to hold n times the amount of data, aligned correctly.
  void operator *= (const uint32_t n);

  /// Expand this Format to be large enough to hold n times the amount of data, aligned correctly.
  Format operator * (const uint32_t n) const;

  size_t size;
  size_t alignment;
};

/// Structure to describe a block of memory.
struct Resource
{
  /// Increment the pointer by the given amount.
  NM_FORCEINLINE void increment(size_t size);

  /// Decrement the pointer by the given amount.
  NM_FORCEINLINE void decrement(size_t size);

  /// Increment the pointer by the given amount. The alignment in the format argument is used to check that the
  /// resource was appropriately aligned *before* is was incremented.
  NM_FORCEINLINE void increment(const Format& incrementFormat);

  /// Increment the resource to the alignment requirements of the supplied Format.
  ///  In debug, this also checks that there is enough space for an object of the given format.
  /// \return The newly aligned Resource pointer.
  NM_FORCEINLINE void* align(const Format& alignFormat);

  /// Increment the resource to be aligned to a specific value.
  /// \return The newly aligned Resource pointer.
  NM_FORCEINLINE void* align(const size_t alignment);

  /// Util for aligning to the format, storing the pointer and then incrementing,
  /// returning the unstepped pointer. Useful when used in conjuntion with placement new.
  NM_FORCEINLINE void* alignAndIncrement(const Format& format);
  
  /// Test if a pointer is within the resource block
  NM_FORCEINLINE bool contains(void* ptr) const;

  void*  ptr;
  Format format;
};

/// Note that this function only works with alignments that are powers of 2
NM_FORCEINLINE ptrdiff_t align(ptrdiff_t value, size_t alignment);
NM_FORCEINLINE ptrdiff_t align(void* value, size_t alignment);

NM_FORCEINLINE void* increment(void* ptr, size_t size);
NM_FORCEINLINE void* alignAndIncrement(void*& ptr, const Format& format);

typedef void*(*NMMemoryAllocator)(size_t mSize);
typedef void*(*NMMemoryAlignedAllocator)(size_t mSize, size_t alignment);
typedef void(*NMMemoryDeallocator)(void* mPtr);
typedef void(*NMMemoryMemcpy)(void* dst, const void* src, size_t size);
typedef size_t(*NMMemorySize)(void* dst);

struct Config
{
  NMMemoryAllocator        allocator;
  NMMemoryAlignedAllocator alignedAllocator;
  NMMemoryAllocator        callocator;
  NMMemoryDeallocator      deallocator;
  NMMemoryMemcpy           memcopy;
  NMMemoryMemcpy           memcopy128;
  NMMemorySize             memSize;
};

extern Memory::Config config;

#ifdef NMP_MEMORY_LOGGING

enum NMP_MEM_STATE
{
  NMP_MEM_STATE_NOT_YET_ALLOCATED = 10,
  NMP_MEM_STATE_ALLOCATED,
  NMP_MEM_STATE_FREED
};

struct MemoryLoggingConfig
{
  void** loggedPtrs;
  size_t* loggedSizes;
  NMP_MEM_STATE* loggedAllocated;
  uint32_t* loggedLine;
  const char** loggedFile;
  const char** loggedFunc;
  uint32_t maxAllocations;

  /// Wipe the log of allocated/freed memory.  One use of this might be to call
  /// it at the beginning of a game's main loop, after all set-up has been performed
  /// to keep track of memory usage during runtime.
  void clearLog()
  {
    for (uint32_t i = 0; i < maxAllocations; i++)
    {
      loggedPtrs[i] = NULL;
      loggedSizes[i] = 0;
      loggedAllocated[i] = NMP_MEM_STATE_NOT_YET_ALLOCATED;
      loggedLine[i] = 0;
      loggedFile[i] = NULL;
      loggedFunc[i] = NULL;
    }
  }
};

extern Memory::MemoryLoggingConfig memLoggingconfig;

#endif

/// A call to init() is needed to initialise the system.  A second call to
/// it will create a problem if and only if the second given configuration
/// differs from the first.  Note that the version of init that does not
/// take a Config parameter uses the internal default.
#ifdef NMP_MEMORY_LOGGING
void init(MemoryLoggingConfig memCfg);
void init(Config c, MemoryLoggingConfig memCfg);
#endif

void init();
void init(Config c);

/// Shutdown should be called after all memory has been freed.  When memory
/// logging has been compiled in, a list of all allocations remaining (leaks)
/// will be printed out.
void shutdown();

/// Has the memory system been initialised yet?
NM_FORCEINLINE bool isInitialised() { return initialised; }

/// Allocate memory based on the requirements passed in.
/// Remember to always include NMP_MEMORY_TRACKING_ARGS in the parameters list so that NMP_MEMORY_LOGGING builds will work.
NM_FORCEINLINE Resource allocateFromFormat(const Format& format NMP_MEMORY_TRACKING_DECL);

/// Allocate memory.
/// Remember to always include NMP_MEMORY_TRACKING_ARGS in the parameters list so that NMP_MEMORY_LOGGING builds will work.
NM_FORCEINLINE void* memAlloc(size_t size NMP_MEMORY_TRACKING_DECL);

/// Allocate memory with specified alignment.
/// Remember to always include NMP_MEMORY_TRACKING_ARGS in the parameters list so that NMP_MEMORY_LOGGING builds will work.
NM_FORCEINLINE void* memAllocAligned(size_t size, size_t alignment NMP_MEMORY_TRACKING_DECL);

/// Allocate memory, clearing it to zero.
NM_FORCEINLINE void* memCalloc(size_t size NMP_MEMORY_TRACKING_DECL);

/// Free an allocated chunk of memory.
NM_FORCEINLINE void memFree(void* ptr);

/// Returns the size of a chunk of memory
NM_FORCEINLINE size_t memSize(void* ptr);

NM_FORCEINLINE void memcpy(void* dst, const void* src, size_t size);
NM_FORCEINLINE void memcpy128(void* dst, const void* src, size_t size);

#ifdef NMP_MEMORY_LOGGING
/// Returns the total memory allocated through NMP::Memory.
NM_FORCEINLINE size_t totalAllocatedBytes() { return totalBytes; }

/// Print out a list of all memory that is currently allocated.  The printout
/// contains: base address, size in bytes and the source file, function and line
/// number which performed the allocation.
void printAllocations();

/// Internal use only.
void logAllocation(void* result, size_t size NMP_MEMORY_TRACKING_DECL);
void memFreeLogging(void* ptr);

#endif

} // namespace Memory

//----------------------------------------------------------------------------------------------------------------------
// Memory::Format inline functions
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Memory::Format::operator == (const Format& other) const
{
  NMP_ASSERT(other.alignment > 0u);
  return (size == other.size) && (alignment == other.alignment);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Memory::Format::operator += (const Format& other)
{
  // Alignments of zero are not valid
  NMP_ASSERT(alignment != 0);
  NMP_ASSERT(other.alignment != 0);

  if (other.alignment > alignment)
  {
    alignment = other.alignment;
  }
  size = Memory::align(size, other.alignment);
  size += other.size;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Memory::Format::operator *= (const uint32_t n)
{
  // Alignments of zero are not valid
  NMP_ASSERT(alignment != 0);

  if (n == 1)
    return;

  size = Memory::align(size, alignment);  // Pad each element
  size *= n;                              // Calculate the array size required
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Memory::Format Memory::Format::operator * (const uint32_t n) const
{
  // Alignments of zero are not valid
  NMP_ASSERT(alignment != 0);

  return NMP::Memory::Format(n == 1 ? size : (Memory::align(size, alignment) * n), alignment);
}

//----------------------------------------------------------------------------------------------------------------------
/// Increment format size to be aligned to the alignment value.
NM_FORCEINLINE void Memory::Format::align()
{
  NMP_ASSERT(alignment > 0u);
  size = Memory::align(size, alignment);
}

//----------------------------------------------------------------------------------------------------------------------
// Memory::Resource inline functions.
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Memory::Resource::increment(size_t size)
{
  NMP_ASSERT(size <= format.size);
#ifdef NM_COMPILER_INTEL
  uint8_t* cptr = (uint8_t*)ptr;
  cptr += size;
  ptr = cptr;
#else
  ptr = (void*)(((size_t)ptr) + size);
#endif
  format.size -= (uint32_t)size;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Memory::Resource::decrement(size_t size)
{
#ifdef NM_COMPILER_INTEL
  uint8_t* cptr = (uint8_t*)ptr;
  cptr -= size;
  ptr = cptr;
#else
  ptr = (void*)(((size_t)ptr) - size);
#endif
  format.size += (uint32_t)size;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Memory::Resource::increment(const Format& incrementFormat)
{
  // Alignments of zero are not valid
  NMP_ASSERT(incrementFormat.alignment != 0);

  NMP_ASSERT(NMP_IS_ALIGNED(ptr, incrementFormat.alignment)); // Check that we are already appropriately aligned
  increment(incrementFormat.size); // Increment by the appropriate amount
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void* Memory::Resource::align(const Format& alignFormat)
{
  // Alignments of zero are not valid
  NMP_ASSERT(alignFormat.alignment != 0);

  size_t alignedAddr = Memory::align(ptr, alignFormat.alignment);
#ifdef NM_COMPILER_INTEL
  const uint8_t* cptr = (uint8_t*)ptr;
  const uint8_t* nil = 0;
  const ptrdiff_t offset = (cptr - nil);
  increment(alignedAddr - offset); // Increment by the appropriate amount, checking for space
#else
  increment(alignedAddr - (size_t)ptr); // Increment by the appropriate amount, checking for space
#endif
  NMP_ASSERT_MSG(
    alignFormat.size <= format.size,
    "align for format size is %u, remaining size is %u",
    alignFormat.size,
    format.size); // Check that 'format' will fit in the remaining resource

  return ptr;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void* Memory::Resource::align(const size_t alignment)
{
  // Alignments of zero are not valid
  NMP_ASSERT(alignment != 0);

  size_t alignedAddr = Memory::align(ptr, (uint32_t)alignment);
#ifdef NM_COMPILER_INTEL
  const uint8_t* cptr = (uint8_t*)ptr;
  const uint8_t* nil = 0;
  const ptrdiff_t offset = (cptr - nil);
  increment(alignedAddr - offset); // Increment by the appropriate amount, checking for space
#else
  increment(alignedAddr - (size_t)ptr); // Increment by the appropriate amount, checking for space
#endif

  return ptr;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void* Memory::Resource::alignAndIncrement(const Format& formatToIncrement)
{
  // Alignments of zero are not valid
  NMP_ASSERT(formatToIncrement.alignment != 0);

  align(formatToIncrement);
  void* ptr_ = ptr;
  increment(formatToIncrement);
  return ptr_;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE bool Memory::Resource::contains(void* data) const
{
  if ((size_t)data >= (size_t)ptr && (size_t)data < ((size_t)ptr + format.size))
    return true;
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
// Memory functions.
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE ptrdiff_t Memory::align(ptrdiff_t value, size_t alignment)
{
  // Alignments of zero are not valid
  NMP_ASSERT(alignment != 0);

  const ptrdiff_t align_ = (alignment - 1);
  return (value + align_) & ~align_;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE ptrdiff_t Memory::align(void* value, size_t alignment)
{
  // Check alignment is non-zero and is a power-of-two
  NMP_ASSERT(alignment != 0 && (alignment & (alignment - 1)) == 0);
#ifdef NM_COMPILER_INTEL
  const uint8_t* cptr = (const uint8_t*)value;
  const uint8_t* nil  = 0;
  return ((cptr - nil) + (alignment - 1)) & ~(ptrdiff_t)(alignment - 1);
#else
  return ((ptrdiff_t)value + (alignment - 1)) & ~(ptrdiff_t)(alignment - 1);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void* Memory::increment(void* ptr, size_t size)
{
#ifdef NM_COMPILER_INTEL
  uint8_t* cptr = (uint8_t*)ptr;
  cptr += size;
  ptr = cptr;
#else
  ptr = (void*)(((size_t)ptr) + size);
#endif

  return ptr;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void* Memory::alignAndIncrement(void*& ptr, const Format& format)
{
  ptr = (void*) NMP::Memory::align(ptr, format.alignment);
  void* result = ptr;
  ptr = NMP::Memory::increment(ptr, format.size);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE Memory::Resource Memory::allocateFromFormat(const Format& format NMP_MEMORY_TRACKING_DECL)
{
  // Alignments of zero are not valid
  NMP_ASSERT(format.alignment != 0);

  Memory::Resource result;
  result.ptr = Memory::memAllocAligned(format.size, format.alignment NMP_MEMORY_TRACKING_PASS_THROUGH);
  NMP_ASSERT(result.ptr);
  result.format = format;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void* Memory::memAlloc(size_t size NMP_MEMORY_TRACKING_DECL)
{
  NMP_ASSERT(initialised); // The memory system must have been initialised.
  NMP_ASSERT(config.allocator);

  void* result = config.allocator(size);
#ifdef NMP_MEMORY_LOGGING
  NMP_ASSERT(config.memSize);
  logAllocation(result, size, file, line, func);

  // We do it this way so that if an implementation decides to return always 0 from
  // memSize, we get consistent results.
  totalBytes += config.memSize(result);
#endif // NMP_MEMORY_LOGGING

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void* Memory::memAllocAligned(size_t size, size_t alignment NMP_MEMORY_TRACKING_DECL)
{
  NMP_ASSERT(initialised); // The memory system must have been initialised.
  NMP_ASSERT(config.alignedAllocator);

  void* result = config.alignedAllocator(size, alignment);
#ifdef NMP_MEMORY_LOGGING
  NMP_ASSERT(config.memSize);
  logAllocation(result, size, file, line, func);  
  
  // We do it this way so that if an implementation decides to return always 0 from
  // memSize, we get consistent results.
  totalBytes += config.memSize(result);
#endif // NMP_MEMORY_LOGGING

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void* Memory::memCalloc(size_t size NMP_MEMORY_TRACKING_DECL)
{
  NMP_ASSERT(initialised); // The memory system must have been initialised.
  NMP_ASSERT(config.callocator);

  void* result = config.callocator(size);
#ifdef NMP_MEMORY_LOGGING
  NMP_ASSERT(config.memSize);
  logAllocation(result, size, file, line, func);

  // We do it this way so that if an implementation decides to return always 0 from
  // memSize, we get consistent results.
  totalBytes += config.memSize(result);
#endif // NMP_MEMORY_LOGGING

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Memory::memFree(void* ptr)
{
  NMP_ASSERT(initialised); // The memory system must have been initialised.
  NMP_ASSERT(config.deallocator);
#ifdef NMP_MEMORY_LOGGING
  NMP_ASSERT(config.memSize);
  memFreeLogging(ptr);

  size_t size = config.memSize(ptr);
  totalBytes -= size;
#endif // NMP_MEMORY_LOGGING

  config.deallocator(ptr);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Memory::memcpy(void* dst, const void* src, size_t size)
{
  NMP_ASSERT(initialised); // The memory system must have been initialised.
  NMP_ASSERT(config.memcopy);
  config.memcopy(dst, src, size);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void Memory::memcpy128(void* dst, const void* src, size_t size)
{
  NMP_ASSERT(initialised); // The memory system must have been initialised.
  NMP_ASSERT(config.memcopy128);
  config.memcopy128(dst, src, size);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE size_t Memory::memSize(void* ptr)
{
  NMP_ASSERT(initialised); // The memory system must have been initialised.
  NMP_ASSERT(config.memSize);
  return config.memSize(ptr);
}

//----------------------------------------------------------------------------------------------------------------------
// Host specific malloc wrappers (these are implemented in NMHostMemory.cpp)
//----------------------------------------------------------------------------------------------------------------------
void* mallocWrapped(size_t size);
void* mallocAlignedWrapped(size_t size, size_t alignment);
void* callocWrapped(size_t size);
void freeWrapped(void* ptr);
void memcpyWrapped(void* dst, const void* src, size_t size);
void memcpy128Wrapped(void* dst, const void* src, size_t size);
size_t memSizeWrapped(void* ptr);

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_MEMORY_H
//----------------------------------------------------------------------------------------------------------------------
