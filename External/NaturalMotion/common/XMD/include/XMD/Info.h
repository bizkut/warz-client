//----------------------------------------------------------------------------------------------------------------------
/// \file   Info.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines methods and classes to determine information 
///         about the exported file. This info includes the local machine on
///         which the file was created, the date and time it was exported, the
///         user logged on at the time, the file format version, the original
///         maya/xsi/max file etc.
/// 
///         The XMD::XTimeStamp class can be used to compare the exported
///         file to the original file to see if it needs to be re-exported etc.
///
///         The main reason i added this stuff was to allow for version 
///         control and incremental graphics builds (ie, only re-export the files
///         if they have changed etc). It also allows for me to identify the 
///         machine and user who exported them. (In reality since this is likely
///         to be done by an automated graphics build, this may always end up 
///         being the same....).
///
///         Eventually i'd like to add a simple info node into maya that 
///         allows users to add custom notes into the maya scene that can be 
///         written out into this node....
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XFileIO;
class XMD_EXPORT XBasicLogger;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// the version identifier for the file format. 
/// The major revision of the file format is denoted by the numberical value
/// \code
///      XMD_0x###A  -  alpha
///      XMD_0x###B  -  beta
///      XMD_0x###R  -  final
/// \endcode
/// The global constant XFV_CURRENT denotes the current revision of the file format
//----------------------------------------------------------------------------------------------------------------------
enum XFileVersion
{
  XMD_UNKNOWN  =  0x0000,

  // version 001
  XMD_0x001A  =  0x0010,
  XMD_0x001B  =  0x0011,
  XMD_0x001F  =  0x0012,

  // version 002
  XMD_0x002A  =  0x0020,
  XMD_0x002B  =  0x0021,
  XMD_0x002F  =  0x0022,
  
  // version 003
  XMD_0x003A  =  0x0030,
  XMD_0x003B  =  0x0031,
  XMD_0x003F  =  0x0032,
  
  // version 004
  XMD_0x004A  =  0x0040,
  XMD_0x004B  =  0x0041,
  XMD_0x004F  =  0x0042,
  
  // version 005
  XMD_0x005A  =  0x0050,
  XMD_0x005B  =  0x0051,
  XMD_0x005F  =  0x0052,
  
  // version 006
  XMD_0x006A  =  0x0060,
  XMD_0x006B  =  0x0061,
  XMD_0x006F  =  0x0062,
  
  // version 007
  XMD_0x007A  =  0x0070,
  XMD_0x007B  =  0x0071,
  XMD_0x007F  =  0x0072,
  
  // version 008
  XMD_0x008A  =  0x0080,
  XMD_0x008B  =  0x0081,
  XMD_0x008F  =  0x0082,
  
  // version 009
  XMD_0x009A  =  0x0090,
  XMD_0x009B  =  0x0091,
  XMD_0x009F  =  0x0092,

  XMD_LAST,
  XFV_MAX=0xFFFF
};
/// \brief  The current file version of XMD
const XFileVersion XFV_CURRENT = XMD_0x003A;

//----------------------------------------------------------------------------------------------------------------------
enum UpAxis
{
  XMD_YUp,
  XMD_ZUp,
  XMD_InvalidUpAxis,
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XTimeStamp
/// \brief structure to hold the date when the file was created
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XTimeStamp
{
  /// \brief  ctor
  /// \note    this will be initialised with the current time
  XTimeStamp();

  /// \brief  ctor
  /// \param  str - the time string
  /// \note    this will be initialised to the time of the specified file
  XTimeStamp(const XString& str);

  /// \brief  copy ctor
  /// \param  ts - the time stamp to copy
  XTimeStamp(const XTimeStamp& ts);

  /// \brief  ctor
  /// \param  ifs - the input file to read the time from
  /// \note    this will read the time from the file.
  XTimeStamp(std::istream& ifs);

  /// 1 to 31
  XU32 Day;
  /// 1 to 12
  XU32 Month;
  /// the year as an int (ie, 2004)
  XU32 Year;
  /// 0 to 23
  XU32 Hour;
  /// 0 to 59
  XU32 Minute;
  /// 0 to 59
  XU32 Sec;

  /// \brief  equivalence operator
  /// \param  d - the time stamp to compare to this
  /// \return true if they are the same
  bool operator==(const XTimeStamp& d) const;

  /// \brief  not equal operator
  /// \param  d - the time stamp to compare to this
  /// \return true if they are the different
  bool operator!=(const XTimeStamp& d) const;

  /// \brief  less than operator. compares d to this and returns true 
  ///         if this time came before d
  /// \param  d - the time stamp to compare to this
  /// \return true if this came before d
  bool operator <(const XTimeStamp& d) const;

  /// \brief  greater than operator. compares d to this and returns true 
  ///         if this time came after d
  /// \param  d - the time stamp to compare to this
  /// \return true if this came after d
  bool operator >(const XTimeStamp& d) const;

  /// \brief  stream insertion operator
  /// \param  ofs - the output file
  /// \param  ts - the time stamp to write
  /// \return the output stream
  friend std::ostream& operator << (std::ostream& ofs, const XTimeStamp& ts );
};

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XInfo
/// \brief This class is used to hold information about the new inherited nodes added into the
///        file when exported from Maya. The info structures will appear within the first part 
///        of the file so that the custom chunk headers can be recognised and appropriately parsed.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XInfo
{
public:
#ifndef DOXYGEN_PROCESSING

  /// ctor
  XInfo();

  /// copy ctor
  XInfo(const XInfo&);

  /// copy ctor
  bool DoData(XFileIO& io);

  /// \brief  returns the total amount of memory (in bytes) needed to
  ///         write this node
  /// \return the total size (in bytes) of the node data
  XU32 GetDataSize() const;

  /// \brief  This function reads the file info from the file.
  /// \param  ifs  -  the input file stream
  /// \return true if OK
  bool Read(std::istream& ifs);

  /// \brief  This function writes the file info to the file.
  /// \param  ofs  -  the input file stream
  /// \return true if OK
  bool Write(std::ostream& ofs);
#endif

  /// \brief  This function returns the name of the files chunk header
  /// \return the chunk header string
  const XFileVersion& GetVersion() const;

  /// \brief  This function returns the base type of this node
  /// \return the type identifier
  const XTimeStamp& GetTimeStamp() const;

  /// \brief  This function returns the name of the software that originally exported the code.
  ///         This may be maya, xsi, max etc.... just a way of handling the 
  /// \return the chunk header string
  const XString& GetSoftware() const;

  /// \brief  This function returns the name of the files chunk header
  /// \return the chunk header string
  const XString& GetOriginalFile() const;

  /// \brief  This function returns the name of the files chunk header
  /// \return the chunk header string
  const XString& GetUser() const;

  /// \brief  This function returns the name of the files chunk header
  /// \return the chunk header string
  const XString& GetHost() const;

  /// \brief  This function returns the name of the files chunk header
  /// \return the chunk header string
  const XString& GetTextureFile() const;

  /// \brief  This function returns the Up Axis
  /// \return the Up Axis
  const UpAxis GetUpAxis() const;

  /// \brief  This function sets the software string of the info structure
  /// \param  str  - the software string name
  void SetSoftware(const XChar* str);

  /// \brief  This function sets the name of the original file. This is 
  ///         useful when writing an exporter for a package so that you
  ///         can tag the original filename in the XMD file for asset
  ///         tracking - i.e. "Human.max", "Horse.mb" etc.
  /// \param  str  -  the name of the original file
  void SetOriginalFile(const XChar* str);

  /// \brief  This function sets the Up axis of the info structure
  /// \param  axis  - the up axis
  void SetUpAxis(UpAxis axis);

private:
  /// the version of the file format
  XFileVersion m_Version;
  /// the chunk header within the file
  XString m_OriginalFile;
  /// the software from which the file was exported
  XString m_Software;
  /// the software from which the file was exported
  XString m_TextureFile;
  /// the name of the logged in user who created the file
  XString m_User;
  /// the host machine
  XString m_Host;
  /// the time stamp from when the file was created
  XTimeStamp m_TimeStamp;
  /// the up axis
  UpAxis m_UpAxis;
};
}
