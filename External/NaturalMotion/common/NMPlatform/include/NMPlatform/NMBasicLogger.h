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
#ifndef NMP_BASIC_LOGGER_H
#define NMP_BASIC_LOGGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMFile.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \enum  NMP::LogColour
/// \brief Log text output colour.
//----------------------------------------------------------------------------------------------------------------------
enum LogColour
{
  LOG_COLOUR_RED    = 0,
  LOG_COLOUR_GREEN,
  LOG_COLOUR_BLUE,
  LOG_COLOUR_YELLOW,
  LOG_COLOUR_CYAN,
  LOG_COLOUR_BROWN,
  LOG_COLOUR_WHITE,
  LOG_COLOUR_BRIGHT_RED,
  LOG_COLOUR_BRIGHT_GREEN,
  LOG_COLOUR_BRIGHT_BLUE,
  LOG_COLOUR_BRIGHT_YELLOW,
  LOG_COLOUR_BRIGHT_CYAN,
  LOG_COLOUR_BRIGHT_BROWN,
  LOG_COLOUR_BRIGHT_WHITE,
  NUM_LOG_COLOURS
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::BasicLogger
/// \brief Base class for a logging service. Writes to stdout.
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class BasicLogger
{
public:

  /// \enum OutputType
  /// \brief
  /// \ingroup
  enum OutputType
  {
    OUTPUT_INVALID,
    OUTPUT_ASCII,
    OUTPUT_UNICODE
  };

  BasicLogger(OutputType outputType = OUTPUT_ASCII);
  virtual ~BasicLogger();

  virtual void output(const char* format, ...);
  virtual void output(NMP::LogColour colour, uint32_t tabSpaces, const char* format, ...);
  virtual void output(const wchar_t* format, ...);
  virtual void output(NMP::LogColour colour, uint32_t tabSpaces, const wchar_t* format, ...);
  virtual void voutput(const char* format, va_list argList);
  virtual void voutput(NMP::LogColour colour, uint32_t tabSpaces, const char* format, va_list argList);
  virtual void voutput(const wchar_t* format, va_list argList);
  virtual void voutput(NMP::LogColour colour, uint32_t tabSpaces, const wchar_t* format, va_list argList);

  void setOutputType(OutputType outputType) { m_outputType = outputType; }
  void setAppendingCarriageReturns(bool set) { m_appendCarriageReturn = set; }
  bool getAppendingCarriageReturns() { return m_appendCarriageReturn; }

protected:
  OutputType  m_outputType;           ///<
  bool        m_appendCarriageReturn; ///< On to the end of each log message.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::NullLogger
/// \brief A logger that does nothing.
/// \ingroup NaturalMotionPlatform
///
/// Implements BasicLogger, but does nothing. Useful when profiling.
//----------------------------------------------------------------------------------------------------------------------
class NullLogger : public BasicLogger
{
public:
  virtual void output(const char* format, ...) { ((void) format); }
  virtual void output(NMP::LogColour colour, uint32_t tabSpaces, const char* format, ...) { (void)colour; (void) tabSpaces; ((void) format); }
  virtual void output(const wchar_t* format, ...) { ((void) format); }
  virtual void output(NMP::LogColour colour, uint32_t tabSpaces, const wchar_t* format, ...) { (void)colour; (void) tabSpaces; ((void) format); }
  virtual void voutput(const char* format, va_list argList) { ((void) format); ((void) argList); }
  virtual void voutput(NMP::LogColour colour, uint32_t tabSpaces, const char* format, va_list argList) { (void)colour; (void) tabSpaces; ((void) format); ((void) argList); }
  virtual void voutput(const wchar_t* format, va_list argList) { ((void) format); ((void) argList); }
  virtual void voutput(NMP::LogColour colour, uint32_t tabSpaces, const wchar_t* format, va_list argList) { (void)colour; (void) tabSpaces; ((void) format); ((void) argList); }

  static NullLogger sm_nullLogger;

private:
  NullLogger() {}
  ~NullLogger() {}
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::FileLogger
/// \brief A logger that outputs to a named file.
/// \ingroup NaturalMotionPlatform
///
/// Implements BasicLogger, providing output to a named file.
//----------------------------------------------------------------------------------------------------------------------
class FileLogger : public BasicLogger
{
public:
  FileLogger();
  FileLogger(const char* filename, OutputType outputType = OUTPUT_ASCII);
  ~FileLogger();

  virtual void output(const char* format, ...);
  virtual void output(NMP::LogColour colour, uint32_t tabSpaces, const char* format, ...);
  virtual void output(const wchar_t* format, ...);
  virtual void output(NMP::LogColour colour, uint32_t tabSpaces, const wchar_t* format, ...);
  virtual void voutput(const char* format, va_list argList);
  virtual void voutput(NMP::LogColour colour, uint32_t tabSpaces, const char* format, va_list argList);
  virtual void voutput(const wchar_t* format, va_list argList);
  virtual void voutput(NMP::LogColour colour, uint32_t tabSpaces, const wchar_t* format, va_list argList);

  virtual bool init(const char* filename, OutputType outputType = OUTPUT_ASCII);
  virtual void tidy();
  bool isInitialised() const { return m_initialised; }

  void AppendFileOutputDirToPath(char* out, const char* in);
private:
  bool                  m_initialised;
  static const uint32_t MAX_LOG_FILENAME_LENGTH = 256;
  char                  m_filename[MAX_LOG_FILENAME_LENGTH];
  NMFile                m_fileHandle;
};

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_BASIC_LOGGER_H
//----------------------------------------------------------------------------------------------------------------------
