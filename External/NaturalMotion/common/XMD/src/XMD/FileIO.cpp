//----------------------------------------------------------------------------------------------------------------------
/// \file     FileIO.cpp
/// \note    (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/FileIO.h"

namespace XMD 
{
#if XMD_ENABLE_DEBUG_LOGGING
  Dumper::Dumper(const XChar* str) 
  {

    // save str name
    m_str = str;

    // output correct indentation
    if(ofs.is_open())
      for(XU32 i=0;i!=num;++i)
        ofs << "\t";

    if(ofs.is_open())
      // write xml start
      ofs << "<" << str << ">\n";

    // increase num of tabs
    ++num;
  }

  // dtor closes the </flag> chunk
  Dumper::~Dumper() 
  {
    --num;

    if(ofs.is_open())
      // write correct number of tabs
      for(XU32 i=0;i!=num;++i)
        ofs << "\t";

    if(ofs.is_open())
      // write closing html tags... </flag>
      ofs << "</" << m_str.c_str() << ">" << std::endl;
  }
  // set static count to zero
  XU32 Dumper::num = 0;

  // output log file
  /// \todo  use an EAT LOGGER
  std::ofstream Dumper::ofs("C:/log.txt");
#endif

//----------------------------------------------------------------------------------------------------------------------
bool __CheckStr(const XChar* str,std::istream& ifs,XString& buffer)
{
  ifs >> buffer;
  for(XU32 i=0;i!=buffer.size();++i)
  {
    if( isalpha(buffer[i]) )
      buffer[i] = (XChar)tolower(buffer[i]);
  }
  XMD::XString buff2 = str;
  for(XU32 i=0;i!=buff2.size();++i)
  {
    if( isalpha(buff2[i]) )
      buff2[i] = (XChar)tolower(buff2[i]);
  }
  if( buff2 == buffer) 
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XFileIO::Seek(XU32 pos)
{
  fseek(m_fp,pos,SEEK_SET);
}

//----------------------------------------------------------------------------------------------------------------------
FILE* XFileIO::GetPtr()
{
  return m_fp;
}

//----------------------------------------------------------------------------------------------------------------------
XFileIO::XFileIO(const XChar* file,bool reading)
{
  m_IsReading = reading;
  m_Owner=true;
  if(m_IsReading) 
  {
    m_fp = fopen(file,"rb");
    m_io = (RW_FUNC)fread;
  }
  else 
  {
    m_fp = fopen(file,"wb");
    m_io = (RW_FUNC)fwrite;
  }
}

//----------------------------------------------------------------------------------------------------------------------
XFileIO::XFileIO(FILE* file,bool reading)
{
  m_IsReading = reading;
  m_Owner=false;
  m_fp = file;
  m_io = m_IsReading ? ((RW_FUNC)fread) : ((RW_FUNC)fwrite);
}

//----------------------------------------------------------------------------------------------------------------------
XFileIO::~XFileIO()
{
  if(m_fp&&m_Owner)
    fclose(m_fp);
  m_fp = 0;
  m_io = 0;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XFileIO::CurrPos() const
{
  if(!m_fp)
    return 0;
  return static_cast<XU32>(ftell(m_fp));
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XFileIO::DoDataString(XString& data) 
{
  XU32 ret=0;
  XU16 sz=0;
  if( IsWriting() )
    sz = static_cast<XU16>( data.size() );

  ret += DoData(&sz);
  IO_CHECK( ret );

  if( IsReading() )
    data.resize(sz);

  for(XU32 i=0;i!=sz;++i) 
  {
    XU32 temp = DoData(&data[i]);
    if(temp==0)
    {
      return 0;
    }
    ret += temp;
  }
  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFileIO::Valid() const 
{
  bool res = m_fp!=0;
  if(IsReading())
    res = res && !feof(m_fp);
  return res;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XFileIO::DoStringVector(XStringList& data)
{
  XU32 ret=0;
  XU32 sz;
  if( IsWriting() )
    sz = static_cast<XU32>( data.size() );

  ret += DoData(&sz);
  XMD_ASSERT(ret);
  if(ret==0)
  {
    return 0;
  }

  if(IsReading())
    data.resize(sz);

  XStringList::iterator it = data.begin();
  for( ; it != data.end(); ++it ) 
  {
    XU32 temp = DoDataString( *it );
    if(temp==0)
    {
      return 0;
    }
    ret += temp;
  }
  return ret;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFileIO::IsReading() const
{
  return m_IsReading;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFileIO::IsWriting() const
{
  return !IsReading();
}

//----------------------------------------------------------------------------------------------------------------------
void XFileIO::Skip(XU32 num)
{
  fseek(m_fp,num,SEEK_CUR);
}
}
