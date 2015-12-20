//----------------------------------------------------------------------------------------------------------------------
/// \file   Logger.h
/// \author Robert Bateman    mailto:robthebloke@hotmail.com
/// \brief  defines an interface for a plugin file translator type
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
#include <list>
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XBasicLogger
/// \brief logging service.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XBasicLogger
{
public:

  /// \brief Log Levels
  /// The available log levels, used to indicate the type of information in the log message.
  enum Level
  {
    kInfo,    ///< Informational messages
    kWarning, ///< Warning messages
    kError,   ///< Error messages
    kDebug,   ///< Debugging messages
    kStdOut   ///< Messages sent to stdout
  };

  /// \brief dtor
  virtual ~XBasicLogger();

  /// \brief Logs a message
  /// \param level - the log level of the message
  /// \param msg - the message
  virtual void Log(Level level, const XString& msg) = 0;

  /// \brief Logs a message
  /// \param level - the log level of the message
  /// \param msg - the message
  virtual void Log(Level level, const XChar* msg);

  /// \brief Logs a printf style formatted message
  /// \param level - the log level of the message
  /// \param msg - the format of the message
  virtual void Logf(Level level, const XChar* format, ...);
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XBufferedLogger
/// \brief A logger that stores messages before passing to another logger.
///        The %XBufferedLogger is a logger that stores its log messages and then
///        flushes them to another logger on demand. This is useful if logging needs
///        to be performed before an application logger is available, or if a series
///        of log messages only need to be logged if a condition is later satisfied 
///        (ie for error traceback).
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XBufferedLogger 
  : public XBasicLogger
{
public:

  /// \brief ctor
  XBufferedLogger();

  /// \brief dtor
  virtual ~XBufferedLogger();

  //-----------------------------------------------------------------------------
  /// \brief Flushes all the currently buffered messages to another logger
  /// \param outputLogger - the logger that receives the flushed messages
  ///
  void FlushBuffer(XBasicLogger* outputLogger);
  
  //-----------------------------------------------------------------------------
  /// \brief Logs a message
  /// \param level - the log level of the message
  /// \param msg - the message
  ///
  void Log(Level level, const XString& msg);

protected:

  /// \brief a list of log levels and strings
  typedef std::list<std::pair<XBasicLogger::Level,XString> > XLogList;

  /// \brief the list of buffered messages
  XLogList* m_Messages;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XStdOutLogger
/// \brief A logger that uses std::cout
///        Implements BasicLogger, providing output to std::cout.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XStdOutLogger 
  : public XBasicLogger
{
public:

  /// \brief dtor
  virtual ~XStdOutLogger();

  /// \param level - the log level of the message
  /// \param msg - the message
  void Log(Level level, const XString& msg);
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XFileLogger
/// \brief A logger that outputs to a file
///        Implements BasicLogger, providing output to std::cout.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XFileLogger 
  : public XBasicLogger
{
public:
  /// \brief ctor
  /// \param fileName - the name of the log file
  XFileLogger(const XChar* fileName = "log.txt");

  /// \brief dtor
  virtual ~XFileLogger();

  /// \brief sets the name of the log file
  /// \param fileName - the name of the log file
  /// \param clearContents - clear the existing contents of the logfile
  void SetFileName(const XChar* fileName, bool clearContents = true);

  /// \brief gets the name of the log file
  /// \return the name of the log file
  const XChar* GetFileName() const;

  /// \brief Logs a message
  /// \param level - the log level of the message
  /// \param msg - the message
  void Log(Level level, const XString& msg);

private:
  std::string m_fileName;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief A logger that uses std::cout and outputs to a file
///        Implements BasicLogger, providing output to std::cout and redirects it to a specified file.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XStdOutAndFileLogger : public XBasicLogger
{
public:
  /// \brief ctor
  /// \param fileName - the name of the log file
  XStdOutAndFileLogger(const XChar* fileName = "log.txt");

  /// \brief dtor
  virtual ~XStdOutAndFileLogger();

  /// \brief sets the name of the log file
  /// \param fileName - the name of the log file
  /// \param clearContents - clear the existing contents of the logfile
  void SetFileName(const XChar* fileName, bool clearContents = false);

  /// \brief gets the name of the log file
  /// \return the name of the log file
  const XChar* GetFileName() const;

  /// \param level - the log level of the message
  /// \param msg - the message
  void Log(Level level, const XString& msg);

private:
  std::string m_fileName;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XNullLogger
/// \brief A logger that does nothing. Useful when profiling.
///        Implements XBasicLogger, but does nothing. Useful when profiling.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XNullLogger
  : public XBasicLogger
{
public:

  /// \brief dtor
  virtual ~XNullLogger();

  /// \brief Logs a message
  /// \param level - the log level of the message
  /// \param msg - the message
  void Log(Level level, const XString& msg);
  
  /// \brief Logs a message
  /// \param level - the log level of the message
  /// \param msg - the message
  void Log(Level level, const XChar* msg);

  /// \brief Logs a message
  /// \param level - the log level of the message
  /// \param format - the format of the message
  void Logf(Level level, const XChar* format, ...);
};
}
