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
#ifndef NMP_LOGGING_MEMORY_ALLOCATOR_H
#define NMP_LOGGING_MEMORY_ALLOCATOR_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMVectorContainer.h"
#include "NMPlatform/NMBasicLogger.h"

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class LoggingMemoryAllocator
/// \brief This memory allocator allocates directly from the system heap using malloc/free.  It also logs the total,
///  peak and current memory allocation requests for every NMP::Format that have been passed in.  It can be used to
///  gather metrics in a debugging environment.
///  NOTE: this allocator is slow and should not be used in production builds.
//----------------------------------------------------------------------------------------------------------------------
class LoggingMemoryAllocator : public MemoryAllocator
{
public:
  void* memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL) NM_OVERRIDE;
  void memFree(void* ptr) NM_OVERRIDE;
  bool term() NM_OVERRIDE;

  static NM_INLINE NMP::Memory::Format getMemoryRequirements();
  static NM_INLINE LoggingMemoryAllocator* init(NMP::Memory::Resource& resource);

  void printTotalAllocations();
  void printPeakAllocations();
  void printCurrentAllocations();

  void calculatePeakAndCurrentUsageTotals(uint32_t& peakBytes, uint32_t& curBytes);

private:

  static const uint32_t LOGGING_ALLOC_MAX_RECORDS = 1024;
  static const uint32_t LOGGING_ALLOC_MAX_ALLOCS = 1024 * 256;
  struct LoggedFormatAllocCount
  {
    NMP::Memory::Format format;
    uint32_t count;
  };

  struct LoggedAllocation
  {
    void* allocPtr;
    uint32_t formatEntryIndex;
#ifdef NMP_MEMORY_LOGGING
    const char* file;
    uint32_t line;
    const char* function;
#endif //NMP_MEMORY_LOGGING
  };

  LoggedFormatAllocCount m_currentAllocCount[LOGGING_ALLOC_MAX_RECORDS];
  LoggedFormatAllocCount m_maxAllocCount[LOGGING_ALLOC_MAX_RECORDS];
  LoggedFormatAllocCount m_totalAllocCount[LOGGING_ALLOC_MAX_RECORDS];

  NMP::VectorContainer<LoggedAllocation>* m_currentAllocations;

  uint32_t m_numRecords;

  static NMP::BasicLogger sm_basicLogger;
};

//----------------------------------------------------------------------------------------------------------------------
// HeapAllocator functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format LoggingMemoryAllocator::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(LoggingMemoryAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  result += NMP::VectorContainer<LoggedAllocation>::getMemoryRequirements(LOGGING_ALLOC_MAX_ALLOCS);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
LoggingMemoryAllocator* LoggingMemoryAllocator::init(NMP::Memory::Resource& resource)
{
  NMP::Memory::Format format(sizeof(LoggingMemoryAllocator), NMP_NATURAL_TYPE_ALIGNMENT);

  resource.align(format);
  // Set up the vtable by calling in place constructor
  LoggingMemoryAllocator* result = new(resource.ptr) LoggingMemoryAllocator();
  resource.increment(format);

  result->initStats();
  result->m_currentAllocations = NMP::VectorContainer<LoggedAllocation>::init(resource, LOGGING_ALLOC_MAX_ALLOCS);

  result->m_numRecords = 0;

  return result;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_LOGGING_MEMORY_ALLOCATOR_H
//----------------------------------------------------------------------------------------------------------------------
