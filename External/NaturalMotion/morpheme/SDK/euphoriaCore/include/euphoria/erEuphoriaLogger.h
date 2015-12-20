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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef EUPHORIA_LOGGER_H
#define EUPHORIA_LOGGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMBasicLogger.h"
//----------------------------------------------------------------------------------------------------------------------
namespace ER
{

// Instate these defines in order to obtain a full network update log.
//#define USING_EUPHORIA_LOGGING
//#define DISPLAY_EUPHORIA_FUNC_ENTRY_LOG
//#define DISPLAY_EUPHORIA_FUNC_EXIT_LOG

#ifdef USING_EUPHORIA_LOGGING
//----------------------------------------------------------------------------------------------------------------------
/// Logger to help track the update progress of Euphoria.
///
/// Outputs to a named file and stdout. Also handles hierarchical indentation of messages to help track the recursive
/// nature of network update.
//----------------------------------------------------------------------------------------------------------------------
class EuphoriaLogger
{
public:
  static void init(const char* filename);
  static void tidy();
  static void output(const char* format, ...);

  static int32_t increaseIndent();
  static int32_t decreaseIndent();
  static int32_t getMarginIndentSize() { return m_marginIndentSize; }
  static void setUnitIndentSize(uint32_t unitMarginIndentSize) { m_unitMarginIndentSize = (int32_t) unitMarginIndentSize; }
  static int32_t getUnitIndentSize() { return m_unitMarginIndentSize; }

private:
  EuphoriaLogger();
  ~EuphoriaLogger();

  static NMP::FileLogger  m_fileLogger;
  static NMP::BasicLogger m_basicLogger;
  static int32_t          m_unitMarginIndentSize; ///< Number of spaces in one indent.
  static int32_t          m_marginIndentSize;     ///< Current number of indentations.
  static bool             m_outputToFile;         ///< Are we outputting to a file as well as stdout.
};

//----------------------------------------------------------------------------------------------------------------------
/// For use in conjunction with EuphoriaLogger to help track current function details within the log.
///
/// Example use: Explicitly construct on the stack on entry to a function (outputs entry message),
/// on exit destructor is automatically called (outputs exit message).
//----------------------------------------------------------------------------------------------------------------------
class FunctionLogIndent
{
public:
  FunctionLogIndent(const char* funcName)
  {
    m_funcName = funcName;
  #ifdef DISPLAY_EUPHORIA_FUNC_ENTRY_LOG
    EuphoriaLogger::output("%s (enter)\n", m_funcName);
  #endif
    EuphoriaLogger::increaseIndent();
  }

  ~FunctionLogIndent()
  {
    EuphoriaLogger::decreaseIndent();
  #ifdef DISPLAY_EUPHORIA_FUNC_EXIT_LOG
    EuphoriaLogger::output("%s (exit)\n", m_funcName);
  #endif
  }

private:
  const char* m_funcName;
};

  #define EUPHORIA_LOG_INIT(fileName) { ER::EuphoriaLogger::init(fileName); }
  #define EUPHORIA_LOG_TIDY() { ER::EuphoriaLogger::tidy(); }
  #define EUPHORIA_LOG_ENTER_FUNC() ER::FunctionLogIndent _FunctionLogIndent(__FUNCTION__);
  #define EUPHORIA_LOG_INC_INDENT() { ER::EuphoriaLogger::increaseIndent(); }
  #define EUPHORIA_LOG_DEC_INDENT() { ER::EuphoriaLogger::decreaseIndent(); }
  #define EUPHORIA_LOG_LINE_DIVIDE() { ER::EuphoriaLogger::output("------------------------------------------------------------------------\n"); }
  #define EUPHORIA_LOG_SPACE_DIVIDE() { ER::EuphoriaLogger::output("                                                                       \n"); }
  #define EUPHORIA_LOG_MESSAGE(format, ...) { ER::EuphoriaLogger::output(format, ##__VA_ARGS__); }

#else // USING_EUPHORIA_LOGGING

  #define EUPHORIA_LOG_INIT(fileName) {}
  #define EUPHORIA_LOG_TIDY() {}
  #define EUPHORIA_LOG_ENTER_FUNC() {}
  #define EUPHORIA_LOG_INC_INDENT() {}
  #define EUPHORIA_LOG_DEC_INDENT() {}
  #define EUPHORIA_LOG_LINE_DIVIDE() {}
  #define EUPHORIA_LOG_SPACE_DIVIDE() {}
  #define EUPHORIA_LOG_MESSAGE(format, ...) {}

#endif // USING_EUPHORIA_LOGGING

// define profile macros for printing to console each frame, which can then be copied into a file for excel analysis
//----------------------------------------------------------------------------------------------------------------------
#define USING_PROFILE_LOG_TO_CONSOLEx

#ifdef USING_PROFILE_LOG_TO_CONSOLE
// Globals, as the macros below are being called from all sorts of places
bool getProfileToConsoleEnabled();
void setProfileToConsoleEnabled(bool enabled);

// One-liners for starting, stopping and printing profile information
  #define CREATE_TIMER_AND_START NMP::Timer timer(true); float timing = 0;
  #define START_TIMER timer.start();
  #define STOP_TIMER_AND_PRINT(tag) \
  timing = timer.stop() * 1000.0f; \
  if (ER::getProfileToConsoleEnabled()) \
  { \
    printf("tech: " tag ", %f, %f, %f, %f\n", 0.0f, timing, 0.0f, timing); \
  }
  #define RESET_TIMER_AND_PRINT(tag) \
  STOP_TIMER_AND_PRINT(tag) \
  timer.start();
#else // #ifdef PROFILE_TO_CONSOLE
  #define CREATE_TIMER_AND_START
  #define START_TIMER
  #define STOP_TIMER_AND_PRINT(tag)
  #define RESET_TIMER_AND_PRINT(tag)
#endif
} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // EUPHORIA_LOGGER_H
//----------------------------------------------------------------------------------------------------------------------
