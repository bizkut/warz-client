//----------------------------------------------------------------------------------------------------------------------
/// \file Info.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Info.h"
#include "XMD/Base.h"
#include "XMD/Logger.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <time.h>
#include <ctype.h>

#ifndef WIN32
# include <dirent.h>
# include <unistd.h>
# include <sys/stat.h>
# define SSCANF(X,Y,Z) sscanf(X,Y,Z)
#else
# ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
# endif
# include <windows.h>
# define SSCANF(X,Y,Z) sscanf_s(X,Y,Z)
#endif

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
std::ostream& operator << (std::ostream& ofs,const XTimeStamp& ts )
{
  ofs << ts.Day << "/" << ts.Month << "/" << ts.Year << "\t";
  ofs << ts.Hour << ":" << ts.Minute << ":" << ts.Sec;
  return ofs;
}

//----------------------------------------------------------------------------------------------------------------------
XTimeStamp::XTimeStamp() 
{
  #ifdef WIN32
  {
    SYSTEMTIME time_info;
    GetLocalTime(&time_info);

    Day    = time_info.wDay;
    Month  = time_info.wMonth;
    Year   = time_info.wYear;
    Hour   = time_info.wHour;
    Minute = time_info.wMinute;
    Sec    = time_info.wSecond;
  }
  #else
  {
    time_t tt;
    time(&tt);
    const tm* thetime = localtime(&tt);

    Year   = thetime->tm_year + 1900;
    Month  = thetime->tm_mon +1;
    Day    = thetime->tm_mday;
    Minute = thetime->tm_min;
    Hour   = thetime->tm_hour;
    Sec    = thetime->tm_sec;
  }
  #endif
}

//----------------------------------------------------------------------------------------------------------------------
XTimeStamp::XTimeStamp(const XString& file)
{
  // force call to default ctor
  *this = XTimeStamp();

  #ifdef WIN32

    HANDLE _file = CreateFileA( file.c_str(),
                  READ_CONTROL,
                  0,
                  NULL,
                  OPEN_EXISTING,
                  FILE_FLAG_BACKUP_SEMANTICS,
                  NULL );

    if(!_file)
      return;

    FILETIME create;
    FILETIME access;
    FILETIME modify;
    SYSTEMTIME systime;

    if( GetFileTime( _file, &create, &access, &modify ) )
    {
      // convert to system time
      FileTimeToSystemTime(&modify,&systime);
      Year   = systime.wYear;
      Month  = systime.wMonth;
      Day    = systime.wDay;
      Minute = systime.wMinute;
      Hour   = systime.wHour;
      Sec    = systime.wSecond;
    }

    CloseHandle(_file);


  #else

    struct stat stat_p;

    // stat the file
    stat(file.c_str(), &stat_p);

    const tm* thetime = gmtime(&stat_p.st_ctime);

    Year   = thetime->tm_year + 1900;
    Month  = thetime->tm_mon +1;
    Day    = thetime->tm_mday;
    Minute = thetime->tm_min;
    Hour   = thetime->tm_hour;
    Sec    = thetime->tm_sec;

  #endif
}

//----------------------------------------------------------------------------------------------------------------------
XTimeStamp::XTimeStamp(const XTimeStamp& ts)
  : Day(ts.Day), 
    Month(ts.Month),
    Year(ts.Year),  
    Hour(ts.Hour), 
    Minute(ts.Minute),
    Sec(ts.Sec) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XTimeStamp::XTimeStamp(std::istream& ifs)
{
  XString buffer;
  ifs >> buffer ;
  XMD_ASSERT( buffer == "date" );

  ifs >> buffer;
  sscanf(buffer.c_str(),"%d/%d/%d",&Day,&Month,&Year);

  ifs >> buffer;
  XMD_ASSERT( buffer == "time" );
  ifs >> buffer;

  const size_t stringLength = buffer.size();
  for(size_t i = 0; i < stringLength; ++i) 
  {
    if(buffer[i] == '.' || buffer[i] == ':')
      buffer[i] = ' ';
  }
  sscanf(buffer.c_str(),"%d%d%d",&Hour,&Minute,&Sec);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTimeStamp::operator==(const XTimeStamp& d) const 
{
  return Year == d.Year  &&
    Month  == d.Month  &&
    Day    == d.Day    &&
    Hour   == d.Hour   &&
    Minute == d.Minute &&
    Sec    == d.Sec;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTimeStamp::operator!=(const XTimeStamp& d) const 
{
  return !((*this) == d);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTimeStamp::operator<(const XTimeStamp& d) const
{
  if(Year<d.Year)     return true;
  if(Month<d.Month)   return true;
  if(Day<d.Day)       return true;
  if(Hour<d.Hour)     return true;
  if(Minute<d.Minute) return true;
  if(Sec<d.Sec)       return true;
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTimeStamp::operator > (const XTimeStamp& d) const
{
  if(Year>d.Year)     return true;
  if(Month>d.Month)   return true;
  if(Day>d.Day)       return true;
  if(Hour>d.Hour)     return true;
  if(Minute>d.Minute) return true;
  if(Sec>d.Sec)       return true;
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInfo::DoData(XFileIO& io) 
{
  DUMPER(XInfo__DoData);

  XChunkHeader header;
  if(io.IsWriting()) 
  {
    header.Type = (XU16) (XFn::Info);
    header.HasExtraAttrs = 0;
    header.Reserved = 0;
    header.ByteSize = GetDataSize() + sizeof(XChunkHeader);
    CHECK_IO( io.DoData(&header) );
    DPARAM( header.Type );
    DPARAM( (XS32)header.HasExtraAttrs );
    DPARAM( header.ByteSize );
  }

  // the version of the file format
  IO_CHECK( io.DoData(&m_Version) );
  DPARAM(m_Version);

  // the chunk header within the file
  IO_CHECK( io.DoDataString(m_OriginalFile) );
  DPARAM(m_OriginalFile);

  // the software from which the file was exported
  IO_CHECK( io.DoDataString(m_Software) );
  DPARAM(m_Software);

  // the name of the logged in user who created the file
  IO_CHECK( io.DoDataString(m_User) );
  DPARAM(m_User);

  // the host machine
  IO_CHECK( io.DoDataString(m_Host) );
  DPARAM(m_Host);

  // the time stamp from when the file was created
  IO_CHECK( io.DoData(&m_TimeStamp) );
  DPARAM(m_TimeStamp);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XInfo::GetDataSize() const
{
  XU32 size = static_cast<XU32>(
    m_OriginalFile.size() + m_Software.size() + m_User.size() + m_Host.size()
    );
  size += 4*sizeof(XU16);
  size += sizeof(XTimeStamp);
  size += sizeof(XFileVersion);
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
XInfo::XInfo() : m_Version(XFV_CURRENT), m_OriginalFile("unknown"), 
  m_Software("XMD_SDK"),m_TextureFile(),m_User(),m_Host(),m_TimeStamp(),m_UpAxis(XMD_InvalidUpAxis)
{
}

//----------------------------------------------------------------------------------------------------------------------
XInfo::XInfo(const XInfo& i) : m_Version(i.m_Version), 
  m_OriginalFile(i.m_OriginalFile), m_Software(i.m_Software),
  m_TextureFile(i.m_TextureFile),m_User(i.m_User),m_Host(i.m_Host),
  m_TimeStamp(i.m_TimeStamp),m_UpAxis(XMD_InvalidUpAxis)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool ReadStr(std::istream& ifs,XString& name) 
{
  // 
  XChar c = (XChar)ifs.get();
  while( c == ' ' || c == '\t' || c == '\n' || c == '\r' ) 
  {
    c = (XChar)ifs.get();
    if(!ifs.good())
      return false;
  }

  if(c=='\"')
  {
    // skip '\"' character
    c = (XChar)ifs.get();

    // read to next "
    while( c != '\"' ) 
    {
      name += c;
      c = (XChar)ifs.get();
      if(!ifs.good())
        return false;
    }
  }
  else
  {
    name = c;
    while( c != ' ' && c != '\t' && c != '\n' && c != '\r' ) 
    {
      name += c;
      c = (XChar)ifs.get();
      if(!ifs.good())
        return false;
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInfo::Read(std::istream& ifs)
{
  DUMPER(XInfo__Read);
  XString buffer;

  m_Software = "";
  m_OriginalFile = "";

  ifs >> buffer;

  READ_CHECK("user",ifs);
  if(!ReadStr(ifs,m_User))
    return false;

  READ_CHECK("host",ifs);
  if(!ReadStr(ifs,m_Host))
    return false;

  READ_CHECK("texture_file",ifs);
  if(!ReadStr(ifs,m_TextureFile))
    return false;

  READ_CHECK("format_version",ifs);
  ifs >> buffer;

  if(strncmp("XMD_",buffer.c_str(),4)!=0) 
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError,"Incorrect format");
    return false;
  }

  XS32 version = (buffer[4]-'0') * 100 + (buffer[5]-'0') * 10 + (buffer[6]-'0');
  if (version>XMD_VERSION_NUMBER)
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError,"This version of XMD is not supported by this version of the SDK");
    return false;
  }

  READ_CHECK("send_bug_reports_to",ifs);
  ifs >> buffer;

  READ_CHECK("application",ifs);
  if(!ReadStr(ifs,m_Software))
    return false;

  READ_CHECK("original_file",ifs);
  if(!ReadStr(ifs,m_OriginalFile))
    return false;

  m_TimeStamp = XTimeStamp(ifs);
  XMD::XString _itmp;

  if (__CheckStr("up_axis",ifs, _itmp))
  {
    int axis = 1;
    ifs >> axis;
    m_UpAxis = static_cast<UpAxis>(axis);
    READ_CHECK("}",ifs);
  }
  else
  {
    if (strcmp ("}",_itmp.c_str()) != 0)
    {
      return false;
    }
  }
  

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XInfo::Write(std::ostream& ofs)
{
  DUMPER(XInfo__Write);
  tm thetime;
  #ifdef WIN32
    SYSTEMTIME time_info;
    GetLocalTime(&time_info);
    thetime.tm_mday = time_info.wDay;
    thetime.tm_mon  = time_info.wMonth;
    thetime.tm_year = time_info.wYear;
    thetime.tm_hour = time_info.wHour;
    thetime.tm_min  = time_info.wMinute;
    thetime.tm_sec  = time_info.wSecond;
  #else
    time_t tt;
    time(&tt);
    const tm* ptime = localtime(&tt);
    memcpy(&thetime,ptime,sizeof(tm));
  #endif
  
  ofs << "INFO info\n{\n";
  
  #ifdef WIN32
    XChar _buffer[128];
    DWORD buff_size=128;
    GetUserNameA(_buffer,&buff_size);
    if(_buffer[0])
      ofs << "\tuser \"" << _buffer << "\"\n";
    else
      ofs << "\tuser \"unknown\"\n";

    DWORD _sz=128;
    GetComputerNameA(_buffer,&_sz);
    if(_buffer[0])
      ofs << "\thost \"" << _buffer << "\"\n";
    else
      ofs << "\thost \"unknown\"\n";
  #else
    XChar _buffer[128];
    gethostname(_buffer,128);

    ofs << "\tuser \"" << getlogin() << "\"" << std::endl;
    ofs << "\thost \"" << _buffer  << "\"" << std::endl;

  #endif

  ofs << "\ttexture_file \"";
  if(m_TextureFile.size())
    ofs << m_TextureFile;
  else 
    ofs << "none";
  ofs << "\"\n";
  ofs << "\tformat_version " XMD_VERSION_ASCII "\n\tsend_bug_reports_to support@naturalmotion.com\n";
  ofs << "\tapplication \"" << m_Software << "\"\n";
  ofs << "\toriginal_file \"" << m_OriginalFile.c_str() << "\"\n";
  ofs << "\tdate " << thetime.tm_mday << "/" << (thetime.tm_mon+1) << "/" << (1900+thetime.tm_year) << "\n";
  ofs << "\ttime " << thetime.tm_hour << "." << thetime.tm_min << ":" << thetime.tm_sec << "\n";
  if(m_UpAxis != XMD_InvalidUpAxis)
  {
    ofs << "\tup_axis " << m_UpAxis << "\n";
  }
  ofs << "}\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
const XFileVersion& XInfo::GetVersion() const
{
  return m_Version;
}

//----------------------------------------------------------------------------------------------------------------------
const XTimeStamp& XInfo::GetTimeStamp() const
{
  return m_TimeStamp;
}

//----------------------------------------------------------------------------------------------------------------------
const XString& XInfo::GetSoftware() const
{
  return m_Software;
}

//----------------------------------------------------------------------------------------------------------------------
const XString& XInfo::GetOriginalFile() const
{
  return m_OriginalFile;
}

//----------------------------------------------------------------------------------------------------------------------
const XString& XInfo::GetUser() const
{
  return m_User;
}

//----------------------------------------------------------------------------------------------------------------------
const XString& XInfo::GetHost() const 
{
  return m_Host;
}

//----------------------------------------------------------------------------------------------------------------------
const XString& XInfo::GetTextureFile() const 
{
  return m_TextureFile;
}

//----------------------------------------------------------------------------------------------------------------------
const UpAxis XInfo::GetUpAxis() const
{
  return m_UpAxis;
}

//----------------------------------------------------------------------------------------------------------------------
void XInfo::SetSoftware(const XChar* str)
{
  m_Software = str;
}

//----------------------------------------------------------------------------------------------------------------------
void XInfo::SetOriginalFile(const XChar* str)
{
  m_OriginalFile = str;
}

//----------------------------------------------------------------------------------------------------------------------
void XInfo::SetUpAxis(UpAxis axis)
{
  m_UpAxis = axis;
}
}
