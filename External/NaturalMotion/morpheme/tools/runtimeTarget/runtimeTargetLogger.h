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
#ifndef NM_RTT_RUNTIMETARGETLOGGER_H
#define NM_RTT_RUNTIMETARGETLOGGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPrioritiesLogger.h"
//----------------------------------------------------------------------------------------------------------------------

/// Using the RTT_MESSAGE_PRIORITY pre-compiler macro for referencing the runtime target logger message priority makes
/// it easier to change to another (shared) logger instance.
#define RTT_MESSAGE_PRIORITY RuntimeTargetLogger::MESSAGE_PRIORITY

/// Using the RTT_LOGGER pre-compiler macro for referencing the runtime target logger makes it easier to change to
/// another (shared) logger instance.
#define RTT_LOGGER RuntimeTargetLogger::logger

//----------------------------------------------------------------------------------------------------------------------
/// \class RuntimeTargetLogger
/// \brief Holds an instance of a priorities logger to be used by the runtime target.
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class RuntimeTargetLogger
{
public:

  /// Priority to be used for runtime target general messages (as opposed to error messages which have their priority
  /// assigned by the logging system directly).
  /// \see RTT_MESSAGE_PRIORITY
  static const NMP::LogPriority MESSAGE_PRIORITY = 130;

  /// Logger instance for runtime target use.
  /// \see RTT_LOGGER
  static NMP::PrioritiesLogger logger;
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_RTT_RUNTIMETARGETLOGGER_H
//----------------------------------------------------------------------------------------------------------------------
