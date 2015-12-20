// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "euphoria/erEuphoriaLogger.h"
#include <stdarg.h>
#include <iostream>
//----------------------------------------------------------------------------------------------------------------------

// Disable warnings about unused formal parameters
#ifdef NM_COMPILER_MSVC
  #pragma warning (disable : 4100)
#endif

namespace ER
{

#ifdef USING_EUPHORIA_LOGGING

  #define LOG_BUFFER_SIZE 1024

NMP::FileLogger   EuphoriaLogger::m_fileLogger;
NMP::BasicLogger  EuphoriaLogger::m_basicLogger;
int32_t           EuphoriaLogger::m_unitMarginIndentSize = 2;      // Number of spaces in one indent.
int32_t           EuphoriaLogger::m_marginIndentSize     = 0;      // Current number of indentations.
bool              EuphoriaLogger::m_outputToFile         = false;  // Are we outputting to a file as well as stdout.

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaLogger::init(const char* filename)
{
  NMP_ASSERT(!m_fileLogger.isInitialised());
  if (filename && m_fileLogger.init(filename))
  {
    m_outputToFile = true;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaLogger::tidy()
{
  if (m_fileLogger.isInitialised())
  {
    m_fileLogger.tidy();
  }
}

//----------------------------------------------------------------------------------------------------------------------
int32_t EuphoriaLogger::increaseIndent()
{
  m_marginIndentSize += m_unitMarginIndentSize;
  return m_marginIndentSize;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t EuphoriaLogger::decreaseIndent()
{
  m_marginIndentSize -= m_unitMarginIndentSize;
  if (m_marginIndentSize < 0)
    m_marginIndentSize = 0;
  return m_marginIndentSize;
}

//----------------------------------------------------------------------------------------------------------------------
void EuphoriaLogger::output(const char* format, ...)
{
  char    cStringBuff[LOG_BUFFER_SIZE];
  int32_t i;

  va_list argList;
  va_start(argList, format);

  NMP_ASSERT(m_marginIndentSize < LOG_BUFFER_SIZE)

  for (i = 0; i < m_marginIndentSize; ++i)
    cStringBuff[i] = ' ';
  cStringBuff[m_marginIndentSize] = '\0';

  NMP_STRNCAT_S(cStringBuff, LOG_BUFFER_SIZE, format);

  m_basicLogger.voutput(cStringBuff, argList);
  if (m_outputToFile)
    m_fileLogger.voutput(cStringBuff, argList);

  va_end(argList);
}

#endif // USING_EUPHORIA_LOGGING

// Globals for printing profile information to console each frame
//----------------------------------------------------------------------------------------------------------------------
#ifdef USING_PROFILE_LOG_TO_CONSOLE
bool g_profile_to_console_enabled = false;
bool getProfileToConsoleEnabled() { return g_profile_to_console_enabled; };
void setProfileToConsoleEnabled(bool enable) { g_profile_to_console_enabled = enable; };
#endif

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
