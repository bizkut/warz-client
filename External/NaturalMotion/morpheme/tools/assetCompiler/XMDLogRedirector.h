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
#ifndef AP_XMDLOGREDIRECTOR_H
#define AP_XMDLOGREDIRECTOR_H

#include "XMD/Logger.h"

//----------------------------------------------------------------------------------------------------------------------
/// \namespace AP
/// \brief Morpheme Asset Processor.
/// \see AssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/** \class AP::AssetProcessor
    \brief Redirects the output from XMD to the asset compiler's logs
    \ingroup AssetProcessorModule
*/
//----------------------------------------------------------------------------------------------------------------------
  class XMDLogRedirector : public XMD::XBasicLogger
{
public:
  /// \brief ctor
  /// \param fileName - the name of the log file
  XMDLogRedirector(NMP::BasicLogger *errorLogger, NMP::BasicLogger *messageLogger = 0, bool verbose = false);

  /// \param level - the log level of the message
  /// \param msg - the message
  void Log(XMD::XBasicLogger::Level level, const XMD::XString& msg);

private:
  NMP::BasicLogger *m_errorLogger;
  NMP::BasicLogger *m_messageLogger;
  bool m_verbose;
};

//----------------------------------------------------------------------------------------------------------------------
XMDLogRedirector::XMDLogRedirector(NMP::BasicLogger *errorLogger, NMP::BasicLogger *messageLogger, bool verbose)
: m_errorLogger(errorLogger), m_messageLogger(messageLogger), m_verbose(verbose)
{
  // We must at least have an error logger
  NMP_VERIFY(m_errorLogger);
}

//----------------------------------------------------------------------------------------------------------------------
void XMDLogRedirector::Log(XMD::XBasicLogger::Level level, const XMD::XString& msg)
{
  NMP::BasicLogger *logger = 0;

  // Select which logger to output to
  if (level == XMD::XBasicLogger::kError)
  {
    logger = m_errorLogger;
  }
  else if (level == XMD::XBasicLogger::kWarning)
  {
    logger = m_messageLogger;
  }
  else if (m_verbose)
  {
    logger = m_messageLogger;
  }

  // Now output the message
  if (logger)
  {
    static const char* lvlStr[] = { 
      "Info", 
      "Warning", 
      "Error", 
      "Debug", 
      "StdOut" };
    logger->output("XMD %s : %s\n", lvlStr[level], msg.c_str());
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // AP_XMDLOGREDIRECTOR_H
//----------------------------------------------------------------------------------------------------------------------