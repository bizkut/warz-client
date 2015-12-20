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
#include <stdio.h>
#include "NMPlatform/NMMemory.h"

#ifdef NMP_MEMORY_LOGGING
  #include "NMPlatform/NMSync.h"
#endif

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
void forceEndianSwap(void* data, uint32_t size)
{
  NMP_ASSERT(data && size > 0);

  // Early out for byte-sized types
  if (size == 1)
  {
    return;
  }
  if (size == 2)
  {
    uint16_t* dataPtr = (uint16_t*) data;
    *dataPtr = (((*dataPtr << 8) & 0xff00) | ((*dataPtr >> 8) & 0x00ff));
    return;
  }
  if (size == 4)
  {
    uint32_t* dataPtr = (uint32_t*) data;

    *dataPtr =
      ((*dataPtr << 24) & 0xff000000) |
      ((*dataPtr <<  8) & 0x00ff0000) |
      ((*dataPtr >>  8) & 0x0000ff00) |
      ((*dataPtr >> 24) & 0x000000ff);
    return;
  }

  // The case for larger types may be platform specific
  NMP_ASSERT((size % 4) == 0);

  uint32_t* words = (uint32_t*) data;
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
void forceEndianSwapArray(void* data, uint32_t numElements, uint32_t elementSize)
{
  uint8_t* currentElement = (uint8_t*) data;

  for (uint32_t i = 0 ; i < numElements ; ++i)
  {
    forceEndianSwap((void*) currentElement, elementSize);
    currentElement += elementSize;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Endian swapping when building objects for other platforms
//----------------------------------------------------------------------------------------------------------------------
void endianSwap(void* data, uint32_t size)
{
#if defined(NM_ENDIAN_SWAP)
  forceEndianSwap(data, size);
#else
  (void) data;
  (void) size;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void endianSwapArray(void* data, uint32_t numElements, uint32_t elementSize)
{
#if defined(NM_ENDIAN_SWAP)
  forceEndianSwapArray(data, numElements, elementSize);
#else
  (void) data;
  (void) numElements;
  (void) elementSize;
#endif
}

namespace Memory
{

// The configuration of the NM Memory system contains function pointers for all memory management functions.
Memory::Config config = { NULL, NULL, NULL, NULL, NULL, NULL, NULL };

// Has the memory system been initialised.
bool initialised = false;

// Total allocated bytes.
size_t totalBytes = 0;

#ifdef NMP_MEMORY_LOGGING

// A mutex to serialize the access to logAllocation and memFreeLogging
NMP::Mutex MemoryLoggingMutex;

// The memory logging configuration of the NM Memory system contains static arrays.
Memory::MemoryLoggingConfig memLoggingconfig = { NULL, NULL, NULL, NULL, NULL, NULL, 0 };

// The default memory logger used if one isn't passed to Memory::init(Config c, MemoryLoggingConfig memConfig) or
// Memory::init(MemoryLoggingConfig loggingCfg)
struct DefaultMemoryLogger
{
  static const uint32_t maxAllocations = 1024 * 64;

  void* loggedPtrs[maxAllocations];
  size_t loggedSizes[maxAllocations];
  NMP::Memory::NMP_MEM_STATE loggedAllocated[maxAllocations];
  uint32_t loggedLine[maxAllocations];
  const char* loggedFile[maxAllocations];
  const char* loggedFunc[maxAllocations];
} defaultMemoryLogger;

//----------------------------------------------------------------------------------------------------------------------
void printAllocations()
{
  NMP_ASSERT(initialised); // The memory system must have been initialised.
  NMP_DEBUG_MSG("Current allocations: \n");
  uint32_t allocated = 0;
  uint32_t allocatedThenFreed = 0;
  uint32_t notYetAllocated = 0;
  for (uint32_t i = 0; i < memLoggingconfig.maxAllocations; i++)
  {
    NMP_MEM_STATE k = memLoggingconfig.loggedAllocated[i];
    switch (k)
    {
    case NMP_MEM_STATE_ALLOCATED:
      NMP_DEBUG_MSG("%p: %10i bytes, from function %30s, line %5i in file %s \n",
                    memLoggingconfig.loggedPtrs[i], memLoggingconfig.loggedSizes[i], 
                    memLoggingconfig.loggedFunc[i], memLoggingconfig.loggedLine[i], memLoggingconfig.loggedFile[i]);
      allocated++;
      break;
    case NMP_MEM_STATE_FREED:
      allocatedThenFreed++;
      break;
    case NMP_MEM_STATE_NOT_YET_ALLOCATED:
      notYetAllocated++;
      break;
    default:
      NMP_ASSERT_FAIL(); // NMP memory log corrupted
      break;
    }
  }

  NMP_DEBUG_MSG("MAX_ALLOCS = %i\n", memLoggingconfig.maxAllocations);
  NMP_DEBUG_MSG("allocated = %i\n", allocated);
  NMP_DEBUG_MSG("allocatedThenFreed = %i\n", allocatedThenFreed);
  NMP_DEBUG_MSG("notYetAllocated = %i\n", notYetAllocated);
  NMP_ASSERT(allocated + allocatedThenFreed + notYetAllocated == memLoggingconfig.maxAllocations); // verify accounting is valid
}

//----------------------------------------------------------------------------------------------------------------------
void init(MemoryLoggingConfig loggingCfg)
{
  Config cfg = { mallocWrapped, mallocAlignedWrapped, callocWrapped, freeWrapped, memcpyWrapped, memcpy128Wrapped, memSizeWrapped };
  loggingCfg.clearLog();
  init(cfg, loggingCfg);
}

//----------------------------------------------------------------------------------------------------------------------
void init(Config c, MemoryLoggingConfig memConfig)
{
  NMP_ASSERT(c.allocator);
  NMP_ASSERT(c.alignedAllocator);
  NMP_ASSERT(c.callocator);
  NMP_ASSERT(c.deallocator);
  NMP_ASSERT(c.memcopy);
  NMP_ASSERT(c.memcopy128);
  NMP_ASSERT(c.memSize);

  if (memLoggingconfig.loggedAllocated == NULL)
  {
    memLoggingconfig.loggedAllocated = memConfig.loggedAllocated;
    memLoggingconfig.loggedFile = memConfig.loggedFile;
    memLoggingconfig.loggedFunc = memConfig.loggedFunc;
    memLoggingconfig.loggedLine = memConfig.loggedLine;
    memLoggingconfig.loggedPtrs = memConfig.loggedPtrs;
    memLoggingconfig.loggedSizes = memConfig.loggedSizes;
    memLoggingconfig.maxAllocations = memConfig.maxAllocations;
  }
  MemoryLoggingMutex.init();

  config = c;
  initialised = true;
}
#endif // NMP_MEMORY_LOGGING

//----------------------------------------------------------------------------------------------------------------------
void init()
{
  Config c = { mallocWrapped, mallocAlignedWrapped, callocWrapped, freeWrapped, memcpyWrapped, memcpy128Wrapped, memSizeWrapped };
  init(c);
}

//----------------------------------------------------------------------------------------------------------------------
void init(Config c)
{
  NMP_ASSERT(!initialised);  // The memory system should only be initialised once.
  NMP_ASSERT(c.allocator);
  NMP_ASSERT(c.alignedAllocator);
  NMP_ASSERT(c.callocator);
  NMP_ASSERT(c.deallocator);
  NMP_ASSERT(c.memcopy);
  NMP_ASSERT(c.memcopy128);
  NMP_ASSERT(c.memSize);

#ifdef NMP_MEMORY_LOGGING
  if (memLoggingconfig.loggedAllocated == NULL)
  {
    memLoggingconfig.loggedAllocated = defaultMemoryLogger.loggedAllocated;
    memLoggingconfig.loggedFile = defaultMemoryLogger.loggedFile;
    memLoggingconfig.loggedFunc = defaultMemoryLogger.loggedFunc;
    memLoggingconfig.loggedLine = defaultMemoryLogger.loggedLine;
    memLoggingconfig.loggedPtrs = defaultMemoryLogger.loggedPtrs;
    memLoggingconfig.loggedSizes = defaultMemoryLogger.loggedSizes;
    memLoggingconfig.maxAllocations = defaultMemoryLogger.maxAllocations;
    // We clear the log on first startup if the defaultMemoryLogger is being used.
    memLoggingconfig.clearLog();
  }
  MemoryLoggingMutex.init();
#endif

  config = c;
  initialised = true;
}

//----------------------------------------------------------------------------------------------------------------------
void shutdown()
{
  NMP_ASSERT(initialised); // The memory system must have been initialised.
#ifdef NMP_MEMORY_LOGGING
  if (memLoggingconfig.loggedAllocated == defaultMemoryLogger.loggedAllocated)
  {
    // NB: If you are running with a debugger trying to track down leaks, it is helpful to add an assert BEFORE this
    //  call to printAllocations so you can spew the leaks into the debugger.
    printAllocations();

    memLoggingconfig.clearLog();
  }
#endif // NMP_MEMORY_LOGGING

  initialised = false;
  config.allocator = NULL;
  config.callocator = NULL;
  config.alignedAllocator = NULL;
  config.deallocator = NULL;
  config.memcopy = NULL;
  config.memcopy128 = NULL;
  config.memSize = NULL;
  return;
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NMP_MEMORY_LOGGING
void logAllocation(void* result, size_t size NMP_MEMORY_TRACKING_DECL)
{
  MemoryLoggingMutex.waitLock();

  if (result != NULL)
  {
    int slot = -1;

    // log this allocation
    uint32_t i = 0;
    for (i = 0; i < memLoggingconfig.maxAllocations; i++)
    {
      if ((slot == -1) && (memLoggingconfig.loggedAllocated[i] == NMP_MEM_STATE_NOT_YET_ALLOCATED))
        slot = i;
      if (memLoggingconfig.loggedPtrs[i] == result)
      { // Then the system memory allocator has returned an address we already have in our list
        // which means either the memory was previously freed (and so has been reallocated) or that
        // the memory list is inconsistent with that of the system (which would probably be a bug in NMMemory).
        // In this case we choose this as our slot, overriding anything else
        NMP_ASSERT(memLoggingconfig.loggedAllocated[i] == NMP_MEM_STATE_FREED);
        slot = i;
        break;
      }
    }
    if (slot != -1)
    {
      // We've found a slot to store this allocation
      memLoggingconfig.loggedAllocated[slot] = NMP_MEM_STATE_ALLOCATED;
      memLoggingconfig.loggedPtrs[slot] = result;
      memLoggingconfig.loggedSizes[slot] = size;
      memLoggingconfig.loggedFile[slot] = file;
      memLoggingconfig.loggedLine[slot] = line;
      memLoggingconfig.loggedFunc[slot] = func;
#ifdef NMP_MEMORY_LOGGING_HIGH_VERBOSITY
      NMP_DEBUG_MSG("logAllocation: %p: %10i bytes, from function %30s, line %5i in file %s \n",
        memLoggingconfig.loggedPtrs[slot], memLoggingconfig.loggedSizes[slot], memLoggingconfig.loggedFunc[slot], 
        memLoggingconfig.loggedLine[slot], memLoggingconfig.loggedFile[slot]);
#endif
    }
    else
    { // ... else we didn't find anywhere to put this allocation.  One way of addressing this would be to
      // increase the value of memLoggingconfig.maxAllocations.  However, if you are running out of logging space then 
      // that probably indicates either that you have a runtime leak, or that you are just doing too much allocating and
      // freeing.
      NMP_ASSERT_FAIL();
    }
  }

  MemoryLoggingMutex.unlock();
}

//----------------------------------------------------------------------------------------------------------------------
void memFreeLogging(void* ptr)
{
  MemoryLoggingMutex.waitLock();

  // Strictly speaking, it's ok to free a null pointer but we won't log that so just early out.
  if (ptr != NULL)
  {
    NMP_USED_FOR_ASSERTS(bool found = false;)
    for (uint32_t i = 0; i < memLoggingconfig.maxAllocations; i++)
    {
      if (memLoggingconfig.loggedPtrs[i] == ptr)
      {
  #ifdef NMP_MEMORY_LOGGING_HIGH_VERBOSITY
        NMP_DEBUG_MSG("memFree(%p): %10i bytes, from function %30s, line %5i in file %s\n",
                      memLoggingconfig.loggedPtrs[i], memLoggingconfig.loggedSizes[i], memLoggingconfig.loggedFunc[i], 
                      memLoggingconfig.loggedLine[i], memLoggingconfig.loggedFile[i]);
  #endif
        // Found the allocation
        if (memLoggingconfig.loggedAllocated[i] == NMP_MEM_STATE_ALLOCATED)
        { // ... then the memory address is currently allocated, which is what we'd expect
          memLoggingconfig.loggedAllocated[i] = NMP_MEM_STATE_FREED;
          NMP_USED_FOR_ASSERTS(found = true;)
          break;
        }
        else
        { // ... else the memory address has either never been allocated or has already been freed.
          // Either way indicates there is a bug somewhere.
          NMP_DEBUG_MSG("memFree(%p): %10i bytes, from function %30s, line %5i in file %s ; loggedAllocated = %s\n",
                        memLoggingconfig.loggedPtrs[i], memLoggingconfig.loggedSizes[i], memLoggingconfig.loggedFunc[i], 
                        memLoggingconfig.loggedLine[i], memLoggingconfig.loggedFile[i],
                        (memLoggingconfig.loggedAllocated[i] == NMP_MEM_STATE_FREED) ? "Already freed" : "Never allocated");
          NMP_ASSERT_FAIL(); // Memory manager state inconsistent!
        }
      }
    } // for (...)
    NMP_ASSERT(found);
  }

  MemoryLoggingMutex.unlock();
}

#endif // NMP_MEMORY_LOGGING

} // namespace Memory

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
