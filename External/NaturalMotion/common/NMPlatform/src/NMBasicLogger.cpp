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
#include "NMPlatform/NMBasicLogger.h"
#include "NMPlatform/NMString.h"

#include "NMBasicLogger.inl"

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

#define LOG_BUFFER_SIZE 1024 * 16

// NullLogger singleton.
NullLogger NullLogger::sm_nullLogger;

//----------------------------------------------------------------------------------------------------------------------
// NMP::LogColour
// Log text output colour.
//----------------------------------------------------------------------------------------------------------------------
#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
static uint32_t g_logColourLookup[NUM_LOG_COLOURS] =
{
  FOREGROUND_RED,
  FOREGROUND_GREEN,
  FOREGROUND_BLUE,
  FOREGROUND_GREEN | LOG_COLOUR_RED,
  FOREGROUND_BLUE | FOREGROUND_GREEN,
  FOREGROUND_BLUE | FOREGROUND_RED,
  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
  FOREGROUND_RED | FOREGROUND_INTENSITY,
  FOREGROUND_GREEN | FOREGROUND_INTENSITY,
  FOREGROUND_BLUE | FOREGROUND_INTENSITY,
  FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_INTENSITY,
  FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
  FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY,
  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
};
#endif

//----------------------------------------------------------------------------------------------------------------------
// BasicLogger functions.
//----------------------------------------------------------------------------------------------------------------------
BasicLogger::BasicLogger(OutputType outputType)
{
  m_outputType = outputType;
  m_appendCarriageReturn = false;
}

//----------------------------------------------------------------------------------------------------------------------
BasicLogger::~BasicLogger()
{
}

//----------------------------------------------------------------------------------------------------------------------
void BasicLogger::output(const char* format, ...)
{
  va_list argList;
  va_start(argList, format);

  voutput(format, argList);

  va_end(argList);
}

//----------------------------------------------------------------------------------------------------------------------
void BasicLogger::output(NMP::LogColour colour, uint32_t tabSpaces, const char* format, ...)
{
  va_list argList;
  va_start(argList, format);

  voutput(colour, tabSpaces, format, argList);

  va_end(argList);
}

//----------------------------------------------------------------------------------------------------------------------
void BasicLogger::voutput(const char* format, va_list argList)
{
  voutput(LOG_COLOUR_WHITE, 0, format, argList);
}

//----------------------------------------------------------------------------------------------------------------------
void BasicLogger::voutput(NMP::LogColour colour, uint32_t tabSpaces, const char* format, va_list argList)
{
#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsoleHandle, (WORD) g_logColourLookup[colour]);
#else
  (void)colour;
#endif

  uint32_t spaceCount = tabSpaces;

  if (format)
  {
#ifdef NM_HOST_USES_SECURE_STRING_FNS
    char cStringBuff[LOG_BUFFER_SIZE];
    _snprintf_s(cStringBuff, LOG_BUFFER_SIZE, LOG_BUFFER_SIZE, "%*s", spaceCount, "");
    vsprintf_s(cStringBuff + spaceCount, LOG_BUFFER_SIZE - spaceCount, format, argList);
    if (m_appendCarriageReturn)
      strcat_s(cStringBuff, LOG_BUFFER_SIZE, "\n");
  #if defined(_MSC_VER)
    OutputDebugStringA(cStringBuff);
  #else
    printf_s(cStringBuff);
  #endif
#else // NM_HOST_USES_SECURE_STRING_FNS
    printf("%*s", spaceCount, "");
    vprintf(format, argList);
    if (m_appendCarriageReturn)
      printf("\n");
#endif // NM_HOST_USES_SECURE_STRING_FNS
  }

#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  SetConsoleTextAttribute(hConsoleHandle, (WORD) g_logColourLookup[LOG_COLOUR_WHITE]);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void BasicLogger::output(const wchar_t* format, ...)
{
  va_list argList;
  va_start(argList, format);

  voutput(format, argList);

  va_end(argList);
}

//----------------------------------------------------------------------------------------------------------------------
void BasicLogger::output(NMP::LogColour colour, uint32_t tabSpaces, const wchar_t* format, ...)
{
  va_list argList;
  va_start(argList, format);

  voutput(colour, tabSpaces, format, argList);

  va_end(argList);
}

//----------------------------------------------------------------------------------------------------------------------
void BasicLogger::voutput(const wchar_t* format, va_list argList)
{
  voutput(LOG_COLOUR_WHITE, 0, format, argList);
}

//----------------------------------------------------------------------------------------------------------------------
void BasicLogger::voutput(NMP::LogColour colour, uint32_t tabSpaces, const wchar_t* format, va_list argList)
{
#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  HANDLE hConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  SetConsoleTextAttribute(hConsoleHandle, (WORD) g_logColourLookup[colour]);
#else
  (void)colour;
#endif

  wchar_t wStringBuff[LOG_BUFFER_SIZE];
  uint32_t spaceCount = tabSpaces;

  if (format)
  {
    switch (m_outputType)
    {
    case OUTPUT_ASCII:
    {
      char    cStringBuff[LOG_BUFFER_SIZE];
      wchar_t wStringBuff1[LOG_BUFFER_SIZE];
#ifdef NM_HOST_USES_SECURE_STRING_FNS
      _snwprintf_s(wStringBuff, LOG_BUFFER_SIZE - 1, _TRUNCATE, L"%*s", spaceCount, L"");
      _vsnwprintf_s(wStringBuff + spaceCount, LOG_BUFFER_SIZE - 1 - spaceCount, _TRUNCATE, format, argList);
      if (m_appendCarriageReturn)
      {
        _snwprintf_s(wStringBuff1, LOG_BUFFER_SIZE - 1, _TRUNCATE, L"%s\r\n", wStringBuff);
        NMString::wCharToChar(cStringBuff, wStringBuff1);
      }
      else
      {
        NMString::wCharToChar(cStringBuff, wStringBuff);
      }
  #if defined(_MSC_VER)
      OutputDebugStringA(cStringBuff);
  #else
      printf_s(cStringBuff);
  #endif
#else // NM_HOST_USES_SECURE_STRING_FNS
      swprintf(wStringBuff, LOG_BUFFER_SIZE - 1, L"%*ls", spaceCount, L"");
      swprintf(wStringBuff + spaceCount, LOG_BUFFER_SIZE - 1 - spaceCount, format, argList);
      if (m_appendCarriageReturn)
      {
        swprintf(wStringBuff1, LOG_BUFFER_SIZE - 1, L"%ls\r\n", wStringBuff);
        NMString::wCharToChar(cStringBuff, wStringBuff1);
      }
      else
      {
        NMString::wCharToChar(cStringBuff, wStringBuff);
      }
      printf("%s", cStringBuff);
#endif // NM_HOST_USES_SECURE_STRING_FNS
      break;
    }
    case OUTPUT_UNICODE:
    {
#ifdef NM_HOST_USES_SECURE_STRING_FNS
      wchar_t wStringBuff1[LOG_BUFFER_SIZE];
      wprintf_s(L"%*s", spaceCount, L"");
      _vsnwprintf_s(wStringBuff, LOG_BUFFER_SIZE - 1, _TRUNCATE, format, argList);
      if (m_appendCarriageReturn)
      {
        _snwprintf_s(wStringBuff1, LOG_BUFFER_SIZE - 1, _TRUNCATE, L"%s\r\n", wStringBuff);
        OutputDebugStringW(wStringBuff1);
        wprintf_s(wStringBuff);
      }
      else
      {
        wprintf_s(wStringBuff);
        OutputDebugStringW(wStringBuff);
      }
#else // NM_HOST_USES_SECURE_STRING_FNS
      swprintf(wStringBuff, LOG_BUFFER_SIZE - 1, L"%*ls", spaceCount, L"");
      swprintf(wStringBuff + spaceCount, LOG_BUFFER_SIZE - 1 - spaceCount, format, argList);
      if (m_appendCarriageReturn)
        wprintf(L"%ls\r\n", wStringBuff);
      else
        wprintf(wStringBuff);
#endif // NM_HOST_USES_SECURE_STRING_FNS
      break;
    }
    default:
    {
      NMP_DEBUG_MSG("Invalid output type set on logger.");
      NMP_ASSERT_FAIL();
      break;
    }
    }
  }

#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  SetConsoleTextAttribute(hConsoleHandle, (WORD) g_logColourLookup[LOG_COLOUR_WHITE]);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
// FileLogger functions.
//----------------------------------------------------------------------------------------------------------------------
FileLogger::FileLogger()
{
  m_initialised = false;
  m_appendCarriageReturn = false;
}

//----------------------------------------------------------------------------------------------------------------------
FileLogger::FileLogger(const char* filename, OutputType outputType)
{
  m_initialised = false;
  init(filename, outputType);
}

//----------------------------------------------------------------------------------------------------------------------
FileLogger::~FileLogger()
{
  if (m_fileHandle.isOpen())
    m_fileHandle.close();
}

//----------------------------------------------------------------------------------------------------------------------
void FileLogger::output(const char* format, ...)
{
  if (!m_fileHandle.isOpen())
    return;

  va_list argList;
  va_start(argList, format);

  voutput(format, argList);

  va_end(argList);
}

//----------------------------------------------------------------------------------------------------------------------
void FileLogger::output(NMP::LogColour colour, uint32_t tabSpaces, const char* format, ...)
{
  if (!m_fileHandle.isOpen())
    return;

  va_list argList;
  va_start(argList, format);

  voutput(colour, tabSpaces, format, argList);

  va_end(argList);
}

//----------------------------------------------------------------------------------------------------------------------
void FileLogger::voutput(const char* format, va_list argList)
{
  voutput(LOG_COLOUR_WHITE, 0, format, argList);
}

//----------------------------------------------------------------------------------------------------------------------
void FileLogger::voutput(NMP::LogColour colour, uint32_t tabSpaces, const char* format, va_list argList)
{
  (void)colour;
  uint32_t spaceCount = tabSpaces;
  char cStringBuff[LOG_BUFFER_SIZE];
  NMP_ASSERT(m_initialised);

  if (format)
  {
#ifdef NM_HOST_USES_SECURE_STRING_FNS
    _snprintf_s(cStringBuff, LOG_BUFFER_SIZE, LOG_BUFFER_SIZE, "%*s", spaceCount, "");
    vsprintf_s(cStringBuff + spaceCount, (LOG_BUFFER_SIZE - spaceCount) - 1, format, argList);
    if (m_appendCarriageReturn)
      strcat_s(cStringBuff, 1, "\n");
#else // NM_HOST_USES_SECURE_STRING_FNS
    sprintf(cStringBuff, "%*s", spaceCount, "");
    vsnprintf(cStringBuff + spaceCount, (LOG_BUFFER_SIZE - spaceCount) - 1, format, argList);
    if (m_appendCarriageReturn)
      strncat(cStringBuff, "\n", 1);
#endif // NM_HOST_USES_SECURE_STRING_FNS
    uint64_t iSize = (uint64_t)strlen(cStringBuff);
    uint64_t iWritten = m_fileHandle.write(cStringBuff, strlen(cStringBuff));
    if (iWritten != iSize)
    {
      NMP_DEBUG_MSG("Log file open for write failed");
      NMP_ASSERT_FAIL();
    }
    m_fileHandle.flush();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void FileLogger::output(const wchar_t* format, ...)
{
  va_list argList;
  va_start(argList, format);

  voutput(format, argList);

  va_end(argList);
}

//----------------------------------------------------------------------------------------------------------------------
void FileLogger::output(NMP::LogColour colour, uint32_t tabSpaces, const wchar_t* format, ...)
{
  va_list argList;
  va_start(argList, format);

  voutput(colour, tabSpaces, format, argList);

  va_end(argList);
}

//----------------------------------------------------------------------------------------------------------------------
void FileLogger::voutput(const wchar_t* format, va_list argList)
{
  voutput(LOG_COLOUR_WHITE, 0, format, argList);
}

//----------------------------------------------------------------------------------------------------------------------
void FileLogger::voutput(NMP::LogColour colour, uint32_t tabSpaces, const wchar_t* format, va_list argList)
{
  (void)colour;
  uint32_t spaceCount = tabSpaces;
  wchar_t wStringBuff[LOG_BUFFER_SIZE];
  wchar_t wStringBuff1[LOG_BUFFER_SIZE];

  if (format)
  {
#ifdef NM_HOST_USES_SECURE_STRING_FNS
    if (m_appendCarriageReturn)
    {
      _snwprintf_s(wStringBuff, LOG_BUFFER_SIZE - 1, _TRUNCATE, L"%*s", spaceCount, L"");
      _vsnwprintf_s(wStringBuff + spaceCount, LOG_BUFFER_SIZE - 1 - spaceCount, _TRUNCATE, format, argList);
      _snwprintf_s(wStringBuff1, LOG_BUFFER_SIZE - 1, _TRUNCATE, L"%s\r\n", wStringBuff);
    }
    else
    {
      _snwprintf_s(wStringBuff1, LOG_BUFFER_SIZE - 1, _TRUNCATE, L"%*s", spaceCount, L"");
      _vsnwprintf_s(wStringBuff1 + spaceCount, LOG_BUFFER_SIZE - 1 - spaceCount, _TRUNCATE, format, argList);
    }
#else // NM_HOST_USES_SECURE_STRING_FNS
    if (m_appendCarriageReturn)
    {
      swprintf(wStringBuff, LOG_BUFFER_SIZE - 1, L"%*ls", spaceCount, L"");
      swprintf(wStringBuff + spaceCount, LOG_BUFFER_SIZE - 1 - spaceCount, format, argList);
      swprintf(wStringBuff1, LOG_BUFFER_SIZE - 1, L"%ls\r\n", wStringBuff);
    }
    else
    {
      swprintf(wStringBuff1, LOG_BUFFER_SIZE - 1, L"%*ls", spaceCount, L"");
      swprintf(wStringBuff1 + spaceCount, LOG_BUFFER_SIZE - 1 - spaceCount, format, argList);
    }
#endif // NM_HOST_USES_SECURE_STRING_FNS

    switch (m_outputType)
    {
    case OUTPUT_ASCII:
    {
      char cStringBuff[LOG_BUFFER_SIZE];
      NMString::wCharToChar(cStringBuff, wStringBuff1);
      m_fileHandle.write(cStringBuff, strlen(cStringBuff));
      break;
    }
    case OUTPUT_UNICODE:
    {
      m_fileHandle.write(wStringBuff1, wcslen(wStringBuff1) * sizeof(wchar_t));
      break;
    }
    default:
    {
      NMP_DEBUG_MSG("Invalid output type set on logger.");
      NMP_ASSERT_FAIL();
      break;
    }
    }
  }

  m_fileHandle.flush();
}

//----------------------------------------------------------------------------------------------------------------------
bool FileLogger::init(const char* filename, OutputType outputType)
{
  NMP_ASSERT(!m_initialised && filename);

  AppendFileOutputDirToPath(m_filename, filename);

  if (m_fileHandle.create(m_filename))
  {
    NMP_ASSERT(m_fileHandle.isOpen());
    setOutputType(outputType);
    m_initialised = true;
    return true;
  }

  NMP_DEBUG_MSG("Creation of log file failed:%s", m_filename);
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void FileLogger::tidy()
{
  m_initialised = false;
  m_filename[0] = '\0';
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------

