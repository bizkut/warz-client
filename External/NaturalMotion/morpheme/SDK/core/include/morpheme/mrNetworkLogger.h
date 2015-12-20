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
#ifndef MR_NETWORK_UPDATE_LOGGER_H
#define MR_NETWORK_UPDATE_LOGGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPrioritiesLogger.h"
#include "NMPlatform/NMMemory.h" // check for NMP_MEMORY_LOGGING
#include "NMPlatform/NMProfiler.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Instate these defines in order to obtain a full network update log.
#if defined(NM_PRIORITIES_LOGGING) && (defined(NMP_ENABLE_ASSERTS) || defined(NMP_MEMORY_LOGGING))
  #define MR_NETWORK_LOGGING
#endif // NMP_ENABLE_ASSERTS || NMP_MEMORY_LOGGING
#define x_PHYSICS_LOGGING


//----------------------------------------------------------------------------------------------------------------------
/// \def MR_USED_FOR_NETWORK_LOGGING
/// \brief Indicates that arguments are only used when MR_NETWORK_LOGGING is enabled.
///
/// Prevents compiler warnings when MR_NETWORK_LOGGING is not enabled.
#if defined(MR_NETWORK_LOGGING)
  #define MR_USED_FOR_NETWORK_LOGGING(x) x
#else
  #define MR_USED_FOR_NETWORK_LOGGING(x) NMP_UNUSED(x)
#endif

//----------------------------------------------------------------------------------------------------------------------
/// Logging of network profiling output.
#if defined(NM_PROFILING)

  extern NMP::PrioritiesLogger g_ProfilingLogger;
  
  #define PROFILING_LOG_INIT(fileName) NM_LOG_INIT(MR::g_ProfilingLogger, fileName, true);
  #define PROFILING_LOG_TIDY() NM_LOG_TIDY(MR::g_ProfilingLogger);

    #define LOG_PROFILING_MESSAGE(format, ...) MR::g_ProfilingLogger.output(format, ##__VA_ARGS__);

#else // defined(NM_PROFILING)

  #define PROFILING_LOG_INIT(fileName)
  #define PROFILING_LOG_TIDY()

    #define LOG_PROFILING_MESSAGE(format, ...)

#endif // defined(NM_PROFILING)

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_NETWORK_LOGGING)

  extern NMP::PrioritiesLogger g_NetworkLogger;

  #define NET_LOG_INIT(fileName) NM_LOG_INIT(MR::g_NetworkLogger, fileName, true);
  #define NET_LOG_TIDY() NM_LOG_TIDY(MR::g_NetworkLogger);
  #define NET_LOG_ENTER_FUNC() NM_LOG_ENTER_FUNC(MR::g_NetworkLogger);
  #define NET_LOG_LINE_DIVIDE(priority) NM_LOG_LINE_DIVIDE(MR::g_NetworkLogger, priority);
  #define NET_LOG_SPACE_DIVIDE(priority) NM_LOG_SPACE_DIVIDE(MR::g_NetworkLogger, priority);
  #define NET_LOG_ADD_PRIORITY_RANGE(lower, upper) NM_LOG_ADD_PRIORITY_RANGE(MR::g_NetworkLogger, lower, upper);

  #define NET_LOG_CONDITIONAL_MESSAGE(exp, priority, format, ...) NM_LOG_CONDITIONAL_MESSAGE(MR::g_NetworkLogger, exp, priority, format, ##__VA_ARGS__);
  #define NET_LOG_MESSAGE(priority, format, ...) NM_LOG_MESSAGE(MR::g_NetworkLogger, priority, format, ##__VA_ARGS__);
  #define NET_LOG_MESSAGE_COLOUR(colour, priority, format, ...) NM_LOG_MESSAGE_COLOUR(MR::g_NetworkLogger, colour, priority, format, ##__VA_ARGS__);
  #define NET_LOG_MESSAGE_INDENT(indent, priority, format, ...) NM_LOG_MESSAGE_INDENT(MR::g_NetworkLogger, indent, priority, format, ##__VA_ARGS__);
  #define NET_LOG_MESSAGE_COLOUR_INDENT(colour, indent, priority, format, ...) NM_LOG_MESSAGE_COLOUR_INDENT(MR::g_NetworkLogger, colour, indent, priority, format, ##__VA_ARGS__);
  #define NET_LOG_ERROR_MESSAGE(format, ...) NM_LOG_ERROR_MESSAGE(MR::g_NetworkLogger, format, ##__VA_ARGS__)
  #define NET_LOG_ASSERT_MESSAGE(exp, format, ...) NM_LOG_ASSERT_MESSAGE(MR::g_NetworkLogger, exp, format, ##__VA_ARGS__);

#else // defined(MR_NETWORK_LOGGING)

  #define NET_LOG_INIT(fileName)
  #define NET_LOG_TIDY()
  #define NET_LOG_ENTER_FUNC()
  #define NET_LOG_LINE_DIVIDE(priority)
  #define NET_LOG_SPACE_DIVIDE(priority)
  #define NET_LOG_ADD_PRIORITY_RANGE(lower, upper)
  
  #define NET_LOG_CONDITIONAL_MESSAGE(exp, priority, format, ...)
  #define NET_LOG_MESSAGE(priority, format, ...)
  #define NET_LOG_MESSAGE_COLOUR(colour, priority, format, ...)
  #define NET_LOG_MESSAGE_INDENT(indent, priority, format, ...)
  #define NET_LOG_MESSAGE_COLOUR_INDENT(colour, indent, priority, format, ...)
  #define NET_LOG_ERROR_MESSAGE(format, ...)
  #define NET_LOG_ASSERT_MESSAGE(exp, ...)


#endif // defined(MR_NETWORK_LOGGING)

//-----------------------------
#if defined(PHYSICS_LOGGING)

  #define PHYSICS_LOG_ENTER_FUNC() { NET_LOG_ENTER_FUNC() }
  #define PHYSICS_LOG_INC_INDENT() { NET_LOG_INC_INDENT() }
  #define PHYSICS_LOG_DEC_INDENT() { NET_LOG_DEC_INDENT() }
  #define PHYSICS_LOG_LINE_DIVIDE() { NET_LOG_LINE_DIVIDE() }
  #define PHYSICS_LOG_SPACE_DIVIDE() { NET_LOG_SPACE_DIVIDE() }
  #define PHYSICS_LOG_MESSAGE(format, ...) { NET_LOG_MESSAGE(format, ...) }

#else // defined(PHYSICS_LOGGING)

  #define PHYSICS_LOG_ENTER_FUNC()
  #define PHYSICS_LOG_INC_INDENT()
  #define PHYSICS_LOG_DEC_INDENT()
  #define PHYSICS_LOG_LINE_DIVIDE()
  #define PHYSICS_LOG_SPACE_DIVIDE()
  #define PHYSICS_LOG_MESSAGE(format, ...)

#endif // defined(PHYSICS_LOGGING)

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NETWORK_UPDATE_LOGGER_H
//----------------------------------------------------------------------------------------------------------------------
