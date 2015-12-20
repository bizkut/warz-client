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
#include "NMPlatform/NMLoggingMemoryAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::BasicLogger  LoggingMemoryAllocator::sm_basicLogger;

//----------------------------------------------------------------------------------------------------------------------
void* LoggingMemoryAllocator::memAlloc(size_t size, uint32_t alignment NMP_MEMORY_TRACKING_DECL)
{
  void* result = NMP::Memory::memAllocAligned(size, alignment NMP_MEMORY_TRACKING_PASS_THROUGH);
  size_t allocSize = NMP::Memory::memSize(result);
  logInternalAlloc(allocSize);
  logExternalAlloc(allocSize);

  NMP_ASSERT(result);

  // Find a format that's the same as this alloc (if there is one)
  uint32_t index = 0;
  while (index < m_numRecords)
  {
    if (m_currentAllocCount[index].format.size == size &&
        m_currentAllocCount[index].format.alignment == alignment)
    {
      break;
    }
    index++;
  }

  // Create a new record if we didn't find a matching one
  if (index == m_numRecords)
  {
    m_currentAllocCount[index].count = 0;
    m_currentAllocCount[index].format.size = size;
    m_currentAllocCount[index].format.alignment = alignment;

    m_totalAllocCount[index].count = 0;
    m_totalAllocCount[index].format = m_currentAllocCount[index].format;

    m_maxAllocCount[index].count = 0;
    m_maxAllocCount[index].format = m_currentAllocCount[index].format;

    m_numRecords++;
  }

  // Add one to the current count for this allocation's format
  m_currentAllocCount[index].count++;
  // Add one to the total count for this allocation's format
  m_totalAllocCount[index].count++;

  // See if we have broken the max ever.
  if (m_currentAllocCount[index].count > m_maxAllocCount[index].count)
  {
    m_maxAllocCount[index].count = m_currentAllocCount[index].count;
  }

  // Log this allocation in the list so we can remove it correctly later.
  LoggedAllocation loggedAllocation;
  loggedAllocation.allocPtr = result;
  loggedAllocation.formatEntryIndex = index;
#ifdef NMP_MEMORY_LOGGING
  loggedAllocation.file = file;
  loggedAllocation.line = line;
  loggedAllocation.function = func;
#endif //NMP_MEMORY_LOGGING

  m_currentAllocations->push_back(loggedAllocation);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void LoggingMemoryAllocator::memFree(void* ptr)
{
  size_t size = NMP::Memory::memSize(ptr);
  logExternalFree(size);
  logInternalFree(size);
  NMP::Memory::memFree(ptr);

  NMP::VectorContainer<LoggedAllocation>::iterator iter = m_currentAllocations->begin();

  uint32_t index = 0;

  while (iter != m_currentAllocations->end())
  {
    if ((*iter).allocPtr == ptr)
    {
      index = (*iter).formatEntryIndex;
      m_currentAllocations->erase(iter);
      break;
    }
    iter++;
  }

  m_currentAllocCount[index].count--;

  return;
}

//----------------------------------------------------------------------------------------------------------------------
bool LoggingMemoryAllocator::term()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void LoggingMemoryAllocator::printTotalAllocations()
{
  sm_basicLogger.output("----------------------------------------------\n"
                        "NMP::LoggingMemoryAllocator total allocations:\n"
                        "----------------------------------------------\n"
                        "     SIZE, ALIGNMENT,    COUNT\n");
  for (uint32_t i = 0; i < m_numRecords; i++)
  {
    sm_basicLogger.output("%9i, %9i, %8i\n",
                          m_totalAllocCount[i].format.size, m_totalAllocCount[i].format.alignment, m_totalAllocCount[i].count);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void LoggingMemoryAllocator::printPeakAllocations()
{
  uint32_t totalBytes = 0;
  sm_basicLogger.output("---------------------------------------------\n"
                        "NMP::LoggingMemoryAllocator peak allocations:\n"
                        "---------------------------------------------\n"
                        "     SIZE, ALIGNMENT,    COUNT\n");
  for (uint32_t i = 0; i < m_numRecords; i++)
  {
    sm_basicLogger.output("%9i, %9i, %8i\n",
                          m_maxAllocCount[i].format.size, m_maxAllocCount[i].format.alignment, m_maxAllocCount[i].count);
    totalBytes += (uint32_t)NMP::Memory::align((uint32_t)m_maxAllocCount[i].format.size, m_maxAllocCount[i].format.alignment) * m_maxAllocCount[i].count;
  }
  sm_basicLogger.output("---------------------------------------------\n"
                        "Peak memory used: %i bytes\n"
                        "---------------------------------------------\n", totalBytes);

}

//----------------------------------------------------------------------------------------------------------------------
void LoggingMemoryAllocator::printCurrentAllocations()
{
  uint32_t totalBytes = 0;
  sm_basicLogger.output("------------------------------------------------------\n"
                        "NMP::LoggingMemoryAllocator current allocation counts:\n"
                        "------------------------------------------------------\n"
                        "     SIZE, ALIGNMENT,    COUNT\n");
  for (uint32_t i = 0; i < m_numRecords; i++)
  {
    sm_basicLogger.output("%9i, %9i, %8i\n",
                          m_currentAllocCount[i].format.size, m_currentAllocCount[i].format.alignment, m_currentAllocCount[i].count);
    totalBytes += (uint32_t)NMP::Memory::align((uint32_t)m_currentAllocCount[i].format.size, m_currentAllocCount[i].format.alignment) * m_currentAllocCount[i].count;
  }
  sm_basicLogger.output("---------------------------------------------\n"
                        "Total memory used: %i bytes\n"
                        "---------------------------------------------\n", totalBytes);

#if defined NMP_MEMORY_LOGGING
  sm_basicLogger.output("------------------------------------------------\n"
                        "NMP::LoggingMemoryAllocator current allocations:\n"
                        "------------------------------------------------\n"
                        "      ADDR,     SIZE,  ALIGNMENT,                                 FUNCTION, FILE AND LINE\n");

  NMP::VectorContainer<LoggedAllocation>::iterator iter = m_currentAllocations->begin();
  while (iter != m_currentAllocations->end())
  {
    sm_basicLogger.output("%10x, %9i, %9i, %40s, %s, %i\n",
                          (*iter).allocPtr,
                          m_currentAllocCount[(*iter).formatEntryIndex].format.size,
                          m_currentAllocCount[(*iter).formatEntryIndex].format.alignment,
                          (*iter).function,
                          (*iter).file,
                          (*iter).line);
    iter++;
  }
#endif //NMP_MEMORY_LOGGING
}

//----------------------------------------------------------------------------------------------------------------------
void LoggingMemoryAllocator::calculatePeakAndCurrentUsageTotals(uint32_t& peakBytes, uint32_t& curBytes)
{
  peakBytes = 0;
  curBytes = 0;

  // do the same calculations as done in the print*() fns above, but just return the summed totals
  for (uint32_t i = 0; i < m_numRecords; i++)
  {
    peakBytes += (uint32_t)NMP::Memory::align((uint32_t)m_maxAllocCount[i].format.size, m_maxAllocCount[i].format.alignment) * m_maxAllocCount[i].count;
    curBytes += (uint32_t)NMP::Memory::align((uint32_t)m_currentAllocCount[i].format.size, m_currentAllocCount[i].format.alignment) * m_currentAllocCount[i].count;
  }
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
