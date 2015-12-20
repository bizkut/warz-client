//----------------------------------------------------------------------------------------------------------------------
/// \file   FileIO.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the class XFileIO which is used for all reading and writing of binary
///         files. In theory (though not in practice) it allows you to write a single function
///         to read and write the file. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdio.h>
#include "XMD/XFn.h"
#include "XMD/Logger.h"
#include "XMD/NodeTypeRegister.h"
#include "XM2/Vector3.h"
#include "XM2/Transform.h"
//----------------------------------------------------------------------------------------------------------------------
#define XMD_ENABLE_DEBUG_LOGGING 0
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{

#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XModel;

extern "C" 
{
  typedef size_t (*RW_FUNC) (void*,size_t,size_t,FILE*);
}

extern bool __CheckStr(const XChar*,std::istream&,XMD::XString& got);

#if XMD_ENABLE_DEBUG_LOGGING
struct Dumper
{
  // ctor always requires a text string. For example, "fred" would produce
  // the debug output,
  //
  //  <fred>
  //    ...
  //  </fred>
  //
  Dumper(const XChar* str);

  // dtor closes the </flag> chunk
  ~Dumper();

  // the flag name
  XString m_str;

  // the number of levels in the Dumper
  static XU32 num;
  static std::ofstream ofs;
};

// macro to create a dumper
#define NEW_LOG(X) { Dumper::ofs.close(); Dumper::ofs.open( (X) ); }
#define DUMPER__(X,L) Dumper D___##X##L ( #X );
#define DUMPER_(X,L) Dumper DMPR__##X ( #X )
#define DUMPER(X) DUMPER_(X,__LINE__);
#define DTABS                           \
  { for(XU32 __i=0;__i!=Dumper::num;++__i)              \
    Dumper::ofs << "\t"; }
// macro to output param on line
#define DPARAM(X) {                                 \
  DTABS                                     \
  Dumper::ofs << "<" << #X << ">\t" << (X) << "\t</" << #X << ">" << std::endl;   \
}

#define DAPARAM(X) {                  \
  DTABS                       \
  Dumper::ofs << "<" << #X << ">\n" ;         \
  for(XU32 __i=0;__i!= (X) .size(); __i++) {      \
    DTABS Dumper::ofs << "\t" << (X)[__i] << std::endl;   \
  }                           \
  DTABS                       \
  Dumper::ofs << "</" << #X << ">\n" ;        \
}

#define CHECK_IO(X) \
  if(!(X)) {                                \
    Dumper::ofs << "IO Failed for " << #X << "\n";            \
    Dumper::ofs << "line: " << __LINE__ << "\nfile: " << __FILE__ <<std::endl; \
    return false;                             \
  }
#define IO_CHECK(X)  \
  if(!(X)) {                                \
    Dumper::ofs << "IO Failed for " << #X << "\n";            \
    Dumper::ofs << "line: " << __LINE__ << "\nfile: " << __FILE__ <<std::endl; \
    return false;                             \
  }
#define READ_CHECK(FLAG,IFS) \
  { \
    XMD::XString _itmp; \
    if( !__CheckStr( FLAG , IFS, _itmp ) ) { \
      XMD::XGlobal::GetLogger()->Logf(XBasicLogger::kError, "Expected %s, line %d, file %s got %s", \
            FLAG, __LINE__, __FILE__, _itmp); \
      return false; \
    } \
  }
#define SREAD_CHECK(FLAG,IFS) \
  { \
    XMD::XString _itmp; \
    if( !__CheckStr( FLAG , IFS, _itmp ) ) { \
    XMD::XGlobal::GetLogger()->Logf(XBasicLogger::kError, "Expected %s, line %d, file %s got %s", \
            FLAG, __LINE__, __FILE__, _itmp); \
      return (IFS); \
    } \
  }
#else
#define DTABS ;
#define DPARAM(X) ;
#define DUMPER(X) ;
#define DAPARAM(X) ;
#define NEW_LOG(X) ;
#define CHECK_IO(X)   \
  if( !(X) ) {    \
    return false; \
  }
#define IO_CHECK(X)   \
  if( !(X) ) {    \
    return false;   \
  }
#define READ_CHECK(FLAG,IFS) \
  { \
    XMD::XString _itmp; \
    if( !__CheckStr( FLAG , IFS, _itmp ) ) { \
      XGlobal::GetLogger()->Logf(XBasicLogger::kError, "Expected %s got %s [%s:%d]", FLAG, _itmp.c_str(),__FILE__,__LINE__); \
      return false; \
    } \
  }
#define SREAD_CHECK(FLAG,IFS) \
  { \
    XMD::XString _itmp; \
    if( !__CheckStr( FLAG , IFS, _itmp ) ) { \
      XGlobal::GetLogger()->Logf(XBasicLogger::kError, "Expected %s got %s [%s:%d]", FLAG, _itmp.c_str(),__FILE__,__LINE__ ); \
      return (IFS); \
    } \
  }

#endif
#endif// DOXYGEN_PROCESSING

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XFileIO
/// \brief wraps binary IO operations. 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XFileIO 
{
public:

  /// \brief  ctor
  /// \param  file  - the filename
  /// \param  reading - if true, the file will read, else it will write
  XFileIO(const XChar* file, bool reading=true);

  /// \brief  ctor
  /// \param  file  - the filename
  /// \param  reading - if true, the file will read, else it will write
  XFileIO(FILE* file, bool reading=true);

  ~XFileIO();

  /// \brief  reads/writes a vector of objects to/from the file
  /// \param  data  - the array to output
  /// \return the number of bytes written
  XU32 DoStringVector(XStringList& data);

  /// \brief  reads or writes the specified data amount
  /// \param  ptr - pointer to the data
  /// \param  num - number of elements to write
  /// \return the number of elements written
  /// \note   returns 0 if the number of elements read or written is different
  ///         to the expected value num
  template<typename T>
  XU32 DoData(T* ptr, XU32 num = 1)
  {
    XMD_ASSERT(m_io && "Require the read/write function to be specified before DoData gets called");
    XMD_ASSERT(m_fp && "File pointer not valid");

    XU32 ret = static_cast<XU32>( m_io(ptr,sizeof(T),num,m_fp) );
    XMD_ASSERT(ret == num);
    if(ret != num)
    {
      XGlobal::GetLogger()->Logf(XBasicLogger::kError, "XFileIO::DoData : expected to read/write %d elements got %d", num, ret);
      ret=0;
    }
    return ret;
  }
  /// \brief  reads or writes the specified data amount
  /// \param  ptr - pointer to the data
  /// \param  num - number of elements to write
  /// \return the number of elements written
  /// \note   returns 0 if the number of elements read or written is different
  ///         to the expected value num
  template<>
  XU32 DoData<XM2::XVector3>(XM2::XVector3* ptr, XU32 num)
  {
    XMD_ASSERT(m_io && "Require the read/write function to be specified before DoData gets called");
    XMD_ASSERT(m_fp && "File pointer not valid");

    XU32 ret=0;
    XM2::XVector3* end = ptr + num;
    for (;ptr != end;++ptr)
    {
      XU32 temp = static_cast<XU32>( m_io(ptr,sizeof(XReal)*3,1,m_fp) );
      if(temp==0)
      {
        XGlobal::GetLogger()->Logf(XBasicLogger::kError, "XFileIO::DoData : expected to read/write %d elements got %d", num, ret);
      }
      ret += temp;
    }
    return ret;
  }
  /// \brief  reads or writes the specified data amount
  /// \param  ptr - pointer to the data
  /// \param  num - number of elements to write
  /// \return the number of elements written
  /// \note   returns 0 if the number of elements read or written is different
  ///         to the expected value num
  template<>
  XU32 DoData<XM2::XTransform>(XM2::XTransform* ptr, XU32 num)
  {
    XMD_ASSERT(m_io && "Require the read/write function to be specified before DoData gets called");
    XMD_ASSERT(m_fp && "File pointer not valid");

    XU32 ret=0;
    XM2::XTransform* end = ptr + num;
    for (;ptr != end;++ptr)
    {
      XU32 temp = static_cast<XU32>( m_io(&ptr->rotate,sizeof(XQuaternion),1,m_fp) );
      // only deal with the x, y, z of translate and scale. This allows XMD files to be used with
      // either XM or XM2. sizeof(XM::Vector3) != sizeof(XM2::Vector3)
      temp += static_cast<XU32>( m_io(&ptr->translate,sizeof(XReal)*3,1,m_fp) );
      temp += static_cast<XU32>( m_io(&ptr->scale,sizeof(XReal)*3,1,m_fp) );
      if(temp!=3)
      {
        XGlobal::GetLogger()->Logf(XBasicLogger::kError, "XFileIO::DoData : expected to read/write %d elements got %d", num, ret);
      }
      ret += temp;
    }
    return ret;
  }
  /// \brief  reads or writes the specified data amount
  /// \param  ptr - pointer to the data
  /// \param  num - number of elements to write
  /// \return the number of elements written
  /// \note   returns 0 if the number of elements read or written is different
  ///         to the expected value num
  XU32 DoData(XM2::XVector3* ptr)
  {
    return DoData(ptr,1);
  }
  /// \brief  reads or writes the specified data amount
  /// \param  ptr - pointer to the data
  /// \param  num - number of elements to write
  /// \return the number of elements written
  /// \note   returns 0 if the number of elements read or written is different
  ///         to the expected value num
  XU32 DoData(XM2::XTransform* ptr)
  {
    return DoData(ptr,1);
  }

#ifndef DOXYGEN_PROCESSING

  template<typename T>
  struct TPtrTest
  {
  private:
    struct B {T dummy[2];};
    static T _IsPtr(void*);
    static B _IsPtr(...);
    static T MakeT();
  public:
    enum { IsPtr = (sizeof(T)==sizeof(_IsPtr( MakeT() ))) };
  };


  template< bool IsPtr, typename Tunused = void  >
  struct DoDataVectorInternal 
  {
    template <typename TVector>
    static XU32 DoB(XFileIO& io, TVector& data, XModel* model)
    {
      (void)model;
      XU32 ret=0;
      XU32 sz;
      if( io.IsWriting() )
        sz = static_cast<XU32>( data.size() );

      ret += io.DoData(&sz);

      if( !io.IsReading() )
      {
        typename TVector::iterator it = data.begin();
        for( ; it != data.end(); ++it ) {
          typename TVector::value_type val = *it;
          XU32 temp = io.DoData( &val );
          if(temp==0)
          {
            return 0;
          }
          ret += temp;
        }
      }
      else
      {
        data.resize(sz);
        typename TVector::iterator it = data.begin();
        for( ; it != data.end(); ++it ) {
          typename TVector::value_type val;
          XU32 temp = io.DoData( &val );
          if(temp==0)
          {
            return 0;
          }
          ret += temp;
          *it = val;
        }
      }
      return ret;
    }
    template <typename TVector>
    static XU32 Do(XFileIO& io, TVector& data)
    {
      return DoB(io,data,0);
    }
  };

  template<typename Tunused>
  struct DoDataVectorInternal<true,Tunused> 
  {
  private:
    template <typename TVector,typename T>
    static XU32 Do_(XFileIO& io, TVector& data, XModel* model, T* dummy)
    {
      XU32 ret=0;
      XU32 sz;
      if( io.IsWriting() )
        sz = static_cast<XU32>( data.size() );

      ret += io.DoData(&sz);

      if( io.IsReading() )
        data.resize(sz);

      typename TVector::iterator it = data.begin();
      for( ; it != data.end(); ++it ) {
        if( io.IsReading() ) {
          (*it) = new T(model);
        }
        ret += (*it)->DoData( io );
      }

      return ret;
    }
    template <typename TVector,typename T>
    static XU32 Dob_(XFileIO& io, TVector& data, T* dummy)
    {
      (void)dummy;
      XU32 ret=0;
      XU32 sz;
      if( io.IsWriting() )
        sz = static_cast<XU32>( data.size() );

      ret += io.DoData(&sz);

      if( io.IsReading() )
        data.resize(sz);

      typename TVector::iterator it = data.begin();
      for( ; it != data.end(); ++it ) {
        if( io.IsReading() ) {
          (*it) = new T;
        }
        ret += (*it)->DoData( io );
      }

      return ret;
    }
  public:
    template <typename TVector>
    static XU32 Do(XFileIO& io, TVector& data)
    {
      typename TVector::value_type ptr=0;
      return Dob_(io,data,ptr);
    }
    template <typename TVector>
    static XU32 DoB(XFileIO& io, TVector& data, XModel* model)
    {
      typename TVector::value_type ptr=0;
      return Do_(io,data,model,ptr);
    }
  };
  #endif
  /// \brief  reads/writes a vector of objects to/from the file
  /// \param  data  - the array to output
  /// \return the number of bytes written
  template< typename TVector >
  XU32 DoDataVector(TVector& data) 
  {
    return DoDataVectorInternal< TPtrTest< typename TVector::value_type >::IsPtr >::Do(*this,data);
  }

  /// \brief  reads/writes a vector of objects to/from the file
  /// \param  data  - the array to output
  /// \param  model - the model pointer
  /// \return the number of bytes written
  template< typename TVector >
  XU32 DoDataVectorB(TVector& data, XModel* model = 0)
  {
    return DoDataVectorInternal< TPtrTest< typename TVector::value_type >::IsPtr >::DoB(*this,data,model);
  }
  /// \brief  reads/writes a string to/from the file
  /// \param  data  - the array to output
  /// \return the number of bytes written
  XU32 DoDataString(XString& data);

  /// \brief  returns true if this object is a reader
  /// \return true if reading
  bool IsReading() const;

  /// \brief  returns true if this object is a writer
  /// \return true if writing
  bool IsWriting() const;

  /// \brief  returns true if this object is a writer
  /// \return true if writing
  bool Valid() const;

  /// \brief  returns the current file position in bytes
  /// \return the number of bytes read or written
  XU32 CurrPos() const;

  /// \brief  skips the specified number of bytes in the file
  /// \param  num - the number of bytes to skip
  void Skip(XU32 num);

  /// \brief  seeks to specified position in the file (from the beginning)
  /// \param  pos - the position to seek to
  void Seek(XU32 pos);

  /// \brief  returns a reference to the internal FILE pointer
  /// \return the internal file pointer
  FILE* GetPtr();
private:
#ifndef DOXYGEN_PROCESSING
  /// fread or fwrite
  RW_FUNC m_io;
  /// the file pointer
  FILE* m_fp;
  /// flag for read/write
  bool m_IsReading;
  /// flag for read/write
  bool m_Owner;
#endif
};
}
