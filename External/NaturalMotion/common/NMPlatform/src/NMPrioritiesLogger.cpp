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
#include "NMPlatform/NMPrioritiesLogger.h"
#include <stdarg.h>
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
PrioritiesLogger::PrioritiesLogger()
{
  m_outputToFile = false;
  m_outputToBasic = true;
  m_numPriorityRanges = 0;
}

//----------------------------------------------------------------------------------------------------------------------
PrioritiesLogger::PrioritiesLogger(const char* filename, bool outputToBasic)
{
  init(filename, outputToBasic);
}

//----------------------------------------------------------------------------------------------------------------------
PrioritiesLogger::~PrioritiesLogger()
{
  tidy();
}

//----------------------------------------------------------------------------------------------------------------------
void PrioritiesLogger::init(const char* filename, bool outputToBasic)
{
  NMP_ASSERT(!m_fileLogger.isInitialised());
  if (filename && m_fileLogger.init(filename))
    m_outputToFile = true;
  else
    m_outputToFile = false;

  m_outputToBasic = outputToBasic;
  NMP_ASSERT_MSG(m_outputToBasic || m_outputToFile, "Initialising a logger with no form of output");

  m_numPriorityRanges = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void PrioritiesLogger::tidy()
{
  if (m_fileLogger.isInitialised())
  {
    m_fileLogger.tidy();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PrioritiesLogger::output(const char* format, ...)
{
  va_list argList;
  va_start(argList, format);

  if (m_outputToBasic)
    m_basicLogger.voutput(format, argList);
  if (m_outputToFile)
    m_fileLogger.voutput(format, argList);

  va_end(argList);
}

//----------------------------------------------------------------------------------------------------------------------
void PrioritiesLogger::output(NMP::LogColour colour, uint32_t tabSpaces, const char* format, ...)
{
  va_list argList;
  va_start(argList, format);

  if (m_outputToBasic)
    m_basicLogger.voutput(colour, tabSpaces, format, argList);
  if (m_outputToFile)
    m_fileLogger.voutput(colour, tabSpaces, format, argList);

  va_end(argList);
}

//----------------------------------------------------------------------------------------------------------------------
void PrioritiesLogger::outputWithPriority(LogPriority priority, const char* format, ...)
{
  va_list argList;
  va_start(argList, format);

  if (prioritiesSatisfied(priority))
  {
    if (m_outputToBasic)
      m_basicLogger.voutput(format, argList);
    if (m_outputToFile)
      m_fileLogger.voutput(format, argList);
  }
  va_end(argList);
}

//----------------------------------------------------------------------------------------------------------------------
void PrioritiesLogger::outputWithPriority(NMP::LogColour colour, uint32_t tabSpaces, LogPriority priority, const char* format, ...)
{
  va_list argList;
  va_start(argList, format);

  if (prioritiesSatisfied(priority))
  {
    if (m_outputToBasic)
      m_basicLogger.voutput(colour, tabSpaces, format, argList);
    if (m_outputToFile)
      m_fileLogger.voutput(colour, tabSpaces, format, argList);
  }
  va_end(argList);
}

//----------------------------------------------------------------------------------------------------------------------
void PrioritiesLogger::setPriorityRanges(
  uint32_t     numRanges,
  LogPriority* lowerPriorityLevels,
  LogPriority* upperPriorityLevels)
{
  NMP_ASSERT(lowerPriorityLevels && upperPriorityLevels);
  NMP_ASSERT(numRanges < MAX_NUM_PRIORITY_RANGES);
  m_numPriorityRanges = numRanges;
  for (uint32_t i = 0; i < numRanges; ++i)
  {
    m_priorityRangeUppers[i] = upperPriorityLevels[i];
    m_priorityRangeLowers[i] = lowerPriorityLevels[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PrioritiesLogger::addPriorityRange(
  LogPriority lowerPriorityLevel,
  LogPriority upperPriorityLevel)
{
  NMP_ASSERT(m_numPriorityRanges < MAX_NUM_PRIORITY_RANGES);
  m_priorityRangeUppers[m_numPriorityRanges] = upperPriorityLevel;
  m_priorityRangeLowers[m_numPriorityRanges] = lowerPriorityLevel;
  ++m_numPriorityRanges;
}

//----------------------------------------------------------------------------------------------------------------------
LogPriority PrioritiesLogger::getUpperPriorityLevel(uint32_t priorityRangeIndex)
{
  NMP_ASSERT(priorityRangeIndex < MAX_NUM_PRIORITY_RANGES);
  return m_priorityRangeUppers[priorityRangeIndex];
}

//----------------------------------------------------------------------------------------------------------------------
LogPriority PrioritiesLogger::getLowerPriorityLevel(uint32_t priorityRangeIndex)
{
  NMP_ASSERT(priorityRangeIndex < MAX_NUM_PRIORITY_RANGES);
  return m_priorityRangeLowers[priorityRangeIndex];
}

//----------------------------------------------------------------------------------------------------------------------
bool PrioritiesLogger::inPriorityRange(LogPriority testPriority)
{
  for (uint32_t i = 0; i < m_numPriorityRanges; ++i)
  {
    if ((testPriority >= getLowerPriorityLevel(i)) && (testPriority <= getUpperPriorityLevel(i)))
      return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool PrioritiesLogger::prioritiesSatisfied(LogPriority testPriority)
{
  if (inPriorityRange(testPriority) || testPriority == LOG_PRIORITY_ALWAYS)
    return true;
  return false;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------

