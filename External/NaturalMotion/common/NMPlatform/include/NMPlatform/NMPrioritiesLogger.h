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
#ifndef NMP_PRIORITIES_LOGGER_H
#define NMP_PRIORITIES_LOGGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMBasicLogger.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

// Priorities logging excluded from SPU builds.
#ifndef NM_HOST_CELL_SPU
  #define NM_PRIORITIES_LOGGING
#endif //NM_HOST_CELL_SPU

// Instate this define in order to activate function indentation logging.
#define x_DISPLAY_FUNCTION_INDENT_LOG

/// \brief
typedef int32_t LogPriority;

const LogPriority LOG_MAX_PRIORITY = 10000000;
const LogPriority LOG_MIN_PRIORITY = -10000000;
const LogPriority LOG_PRIORITY_ALWAYS = 0xFFFFFFF;

const uint32_t MAX_NUM_PRIORITY_RANGES = 128;

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::PrioritiesLogger
/// \brief Logger with priority ranges that can optionally output to stdout and to file.
///
/// Outputs to a named file, stdout, and debugger. Also handles hierarchical indentation.
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class PrioritiesLogger
{
public:
  PrioritiesLogger();
  PrioritiesLogger(const char* filename, bool outputToBasic);
  ~PrioritiesLogger();

  void init(const char* filename, bool outputToBasic = true);
  void tidy();
  void output(const char* format, ...);                                   ///< Output with no priority checking.
  void output(NMP::LogColour colour, uint32_t tabSpaces, const char* format, ...);                                   ///< Output with no priority checking.
  void outputWithPriority(LogPriority priority, const char* format, ...); ///< Only outputs if priority is satisfied.
  void outputWithPriority(NMP::LogColour colour, uint32_t tabSpaces, LogPriority priority, const char* format, ...); ///< Only outputs if priority is satisfied.

  void setPriorityRanges(
    uint32_t     numRanges,
    LogPriority* lowerPriorityLevels,
    LogPriority* upperPriorityLevels);
  void addPriorityRange(
    LogPriority lowerPriorityLevel,
    LogPriority upperPriorityLevel);
  bool inPriorityRange(LogPriority testPriority);

  LogPriority getUpperPriorityLevel(uint32_t priorityRangeIndex);
  LogPriority getLowerPriorityLevel(uint32_t priorityRangeIndex);

  bool prioritiesSatisfied(LogPriority testPriority);

private:
  NMP::FileLogger  m_fileLogger;
  NMP::BasicLogger m_basicLogger;
  bool             m_outputToFile;                                 ///< Are we outputting to a file.
  bool             m_outputToBasic;                                ///< Are we outputting to stdout.
  uint32_t         m_numPriorityRanges;                            ///<
  LogPriority      m_priorityRangeUppers[MAX_NUM_PRIORITY_RANGES]; ///< Only messages within these priority ranges are output.
  LogPriority      m_priorityRangeLowers[MAX_NUM_PRIORITY_RANGES]; ///<
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::FunctionEntryIndentLog
/// \brief For use in conjunction with PrioritiesLogger to help track current function details within the log.
///
/// Example use: Explicitly construct on the stack on entry to a function (outputs entry message),
/// on exit destructor is automatically called (outputs exit message).
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class FunctionEntryIndentLog
{
public:
  FunctionEntryIndentLog(const char* indentString, PrioritiesLogger& logger)
  {
    m_logger = &logger;
    m_indentString = indentString;
#if defined(DISPLAY_FUNCTION_INDENT_LOG)
    logger->output("%s (enter)\n", m_indentString);
#endif
  }

  ~FunctionEntryIndentLog()
  {
#if defined(DISPLAY_FUNCTION_INDENT_LOG)
    m_logger->output("%s (exit)\n", m_indentString);
#endif
  }

private:
  const char*       m_indentString;
  PrioritiesLogger* m_logger;
};

//----------------------------------------------------------------------------------------------------------------------
#if defined(NM_PRIORITIES_LOGGING)

  #define NM_LOG_INIT(logger, fileName, outputToBasic) { logger.init(fileName, outputToBasic); }
  #define NM_LOG_TIDY(logger) { logger.tidy(); }
  #define NM_LOG_ENTER_FUNC(logger) NMP::FunctionEntryIndentLog _functionLogIndent(__FUNCTION__, logger);
  #define NM_LOG_LINE_DIVIDE(logger, priority) \
    logger.outputWithPriority(priority, "----------------------------------------------------------------------\n");
  #define NM_LOG_SPACE_DIVIDE(logger, priority) \
    logger.outputWithPriority(priority, "                                                                      \n");
  #define NM_LOG_ADD_PRIORITY_RANGE(logger, lower, upper) { logger.addPriorityRange(lower, upper); }

  #ifdef NMP_ENABLE_ASSERTS
    #define NM_LOG_ASSERT_MESSAGE(logger, exp, ...) { if (!(exp)) { NM_LOG_ERROR_MESSAGE(logger, __VA_ARGS__); NM_BREAK(); } }
  #else  // NM_DEBUG
    #define NM_LOG_ASSERT_MESSAGE(logger, exp, ...) { if (!(exp)) { NM_LOG_ERROR_MESSAGE(logger, __VA_ARGS__); } }
  #endif // NM_DEBUG

  #define NM_LOG_CONDITIONAL_MESSAGE(logger, exp, priority, format, ...)  \
      {                                                                   \
        if ((exp) && logger.prioritiesSatisfied(priority))              \
          logger.output(format, ##__VA_ARGS__);                        \
      }
  #define NM_LOG_MESSAGE(logger, priority, format, ...) \
      {                                                 \
        if (logger.prioritiesSatisfied(priority))     \
          logger.output(format, ##__VA_ARGS__);      \
      }
  #define NM_LOG_MESSAGE_COLOUR(logger, colour, priority, format, ...) \
      {                                                 \
        if (logger.prioritiesSatisfied(priority))     \
          logger.output(colour, 0, format, ##__VA_ARGS__);      \
      }
  #define NM_LOG_MESSAGE_INDENT(logger, indent, priority, format, ...) \
      {                                                 \
        if (logger.prioritiesSatisfied(priority))     \
          logger.output(NMP::LOG_COLOUR_WHITE, indent, format, ##__VA_ARGS__);      \
      }
  #define NM_LOG_MESSAGE_COLOUR_INDENT(logger, colour, indent, priority, format, ...) \
      {                                                 \
        if (logger.prioritiesSatisfied(priority))     \
          logger.output(colour, indent, format, ##__VA_ARGS__);      \
      }
  #define NM_LOG_ERROR_MESSAGE(logger, format, ...)                        \
      NM_LOG_MESSAGE_COLOUR(logger, NMP::LOG_COLOUR_BRIGHT_RED, NMP::LOG_PRIORITY_ALWAYS, format, ##__VA_ARGS__);

#else // defined(NM_PRIORITIES_LOGGING)

  #define NM_LOG_INIT(logger, fileName)
  #define NM_LOG_TIDY(logger)
  #define NM_LOG_ENTER_FUNC(logger)
  #define NM_LOG_INC_INDENT(logger)
  #define NM_LOG_DEC_INDENT(logger)
  #define NM_LOG_LINE_DIVIDE(logger, priority)
  #define NM_LOG_SPACE_DIVIDE(logger, priority)
  #define NM_LOG_COLOUR(logger, colour)
  #define NM_LOG_ADD_PRIORITY_RANGE(logger, lower, upper)
  #define NM_LOG_CONDITIONAL_MESSAGE(logger, exp, priority, format, ...)
  #define NM_LOG_MESSAGE(logger, priority, format, ...)
  #define NM_LOG_ERROR_MESSAGE(logger, format, ...)
  #define NM_LOG_ASSERT_MESSAGE(logger, exp, ...)

#endif // defined(NM_PRIORITIES_LOGGING)

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_PRIORITIES_LOGGER_H
//----------------------------------------------------------------------------------------------------------------------
