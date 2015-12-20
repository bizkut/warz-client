#include "XMD/Logger.h"
#include <stdarg.h>
#include <iostream>
#include <fstream>

#define LOG_BUFFER_SIZE 1024

#ifdef LINUX
#define _vsnprintf vsnprintf
#endif

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
bool fileExists(const XChar* file)
{
  DWORD ret = ::GetFileAttributesA(file);

  return (ret != (DWORD)-1) && !(ret & FILE_ATTRIBUTE_DIRECTORY);
}

//----------------------------------------------------------------------------------------------------------------------
static const XChar* lvlStr[] = { "Info : ", "Warning : ", "Error : ", "Debug : ", "StdOut : " };

//----------------------------------------------------------------------------------------------------------------------
XBasicLogger::~XBasicLogger()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XBasicLogger::Log(Level level, const XChar* msg)
{
  Log(level, XString(msg));
}

//----------------------------------------------------------------------------------------------------------------------
void XBasicLogger::Logf(Level level, const XChar* format, ...)
{
  XChar buffer[LOG_BUFFER_SIZE];
  va_list args;
  va_start(args, format);
  _vsnprintf(buffer, LOG_BUFFER_SIZE -1, format, args);
  va_end(args);
  Log(level, XString(buffer));
}

//----------------------------------------------------------------------------------------------------------------------
XBufferedLogger::XBufferedLogger()
{
  m_Messages = new XLogList;
}

//----------------------------------------------------------------------------------------------------------------------
XBufferedLogger::~XBufferedLogger()
{
  delete m_Messages;
}

//----------------------------------------------------------------------------------------------------------------------
void XBufferedLogger::FlushBuffer(XBasicLogger* outputLogger)
{
  if(outputLogger)
  {
    for(XLogList::iterator it = m_Messages->begin(); it != m_Messages->end(); ++it)
      outputLogger->Log((*it).first, (*it).second);
  }
  m_Messages->clear();
}

//----------------------------------------------------------------------------------------------------------------------
void XBufferedLogger::Log(Level level, const XString& msg)
{
  m_Messages->push_back(std::make_pair(level, msg));
}

//----------------------------------------------------------------------------------------------------------------------
XStdOutLogger::~XStdOutLogger()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XStdOutLogger::Log(Level level, const XString& msg)
{
  std::cout << lvlStr[level] << msg << std::endl;
}

//----------------------------------------------------------------------------------------------------------------------
XFileLogger::XFileLogger(const XChar* fileName)
{
  SetFileName(fileName);
}

//----------------------------------------------------------------------------------------------------------------------
XFileLogger::~XFileLogger()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XFileLogger::SetFileName(const XChar* fileName, bool clearContents)
{
  if(clearContents && fileExists(fileName))
  {
    std::ofstream file(fileName);
    file.close();
  }

  m_fileName = fileName;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XFileLogger::GetFileName() const
{
  return m_fileName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
void XFileLogger::Log(Level level, const XString& msg)
{
  //how to do error handling here? Seems not to crash for invalid filenames,,,
  std::ofstream outfile(m_fileName.c_str());//, std::ios::out | std::ios::app);
  outfile << lvlStr[level] << msg << std::endl;
  outfile.close();
}

//----------------------------------------------------------------------------------------------------------------------
XStdOutAndFileLogger::XStdOutAndFileLogger(const XChar* fileName)
{
  SetFileName(fileName);
}

//----------------------------------------------------------------------------------------------------------------------
XStdOutAndFileLogger::~XStdOutAndFileLogger()
{

}

//----------------------------------------------------------------------------------------------------------------------
void XStdOutAndFileLogger::SetFileName(const XChar* fileName, bool clearContents)
{
  if(clearContents && fileExists(fileName))
  {
    std::ofstream file(fileName);
    file.close();
  }

  m_fileName = fileName;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XStdOutAndFileLogger::GetFileName() const
{
  return m_fileName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
void XStdOutAndFileLogger::Log(Level level, const XString& msg)
{
  std::cout << lvlStr[level] << msg << std::endl;

  std::ofstream file(m_fileName.c_str(), std::ios::out | std::ios::app);

  // save output buffer of the stream
  std::streambuf* strm_buffer = std::cout.rdbuf();

  // redirect ouput into the file
  std::cout.rdbuf (file.rdbuf());

  file << lvlStr[level] << msg << std::endl;

  // restore old output buffer
  std::cout.rdbuf (strm_buffer);
}

//----------------------------------------------------------------------------------------------------------------------
XNullLogger::~XNullLogger()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XNullLogger::Log(Level level, const XString& msg)
{
  ((void)level);
  ((void)msg);
}

//----------------------------------------------------------------------------------------------------------------------
void XNullLogger::Log(Level level, const XChar* msg)
{
  ((void)level);
  ((void)msg);
}

//----------------------------------------------------------------------------------------------------------------------
void XNullLogger::Logf(Level level, const XChar* format, ...)
{
  ((void)level);
  ((void)format);
}
}
