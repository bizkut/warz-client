//----------------------------------------------------------------------------------------------------------------------
/// \file Image.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Image.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"
#include <math.h>

namespace XMD 
{
#pragma pack(push,1)
//----------------------------------------------------------------------------------------------------------------------
/// \brief  This struct is used as a 2byte identifier just before an image
/// \note sizeof(ImageHeader) == 2
///
struct XImageHeader
{
  XU16 m_ID;
  /// the width of the image as the power of two
  XU8 m_WidthPower:4;
  /// the height of the image as a power of two
  XU8 m_HeightPower:4;

  /// the bits per pixel
  /// 0 = 8bit
  /// 1 = 16bit
  /// 2 = 24bit
  /// 3 = 32bit
  ///
  XU8 m_Bpp:2;

  /// 6bits for a rainy day...
  XU8 m_Unused:6;

  XImageHeader()
    : m_WidthPower(0),
    m_HeightPower(0),
    m_Bpp(0),
    m_Unused(0)
  {
  }
};
#pragma pack(pop)

//----------------------------------------------------------------------------------------------------------------------
void SkipTextures(FILE* fp)
{
  XU16 s=0;
  fread(&s,sizeof(XU16),1,fp);

  for(XU32 i=0;i!=s;++i) 
  {
    // read the image header
    XImageHeader header;
    fread(&header,sizeof(XImageHeader),1,fp);

    // skip maya name
    XU16 n=0;
    fread(&n,sizeof(XU16),1,fp);
    fseek(fp,n,SEEK_CUR);

    // skip filename
    fread(&n,sizeof(XU16),1,fp);
    fseek(fp,n,SEEK_CUR);

    // skip image data
    const XReal two = 2.0f;
    fseek(fp, static_cast<XU32>(powf(two,(XS32)((XReal)header.m_WidthPower) * powf(two,(XReal)header.m_HeightPower) * (header.m_Bpp+1))), SEEK_CUR );
  }
}


//----------------------------------------------------------------------------------------------------------------------
XImage::XImage()
  :m_MayaName(),m_FilePath(),m_ID(0), m_Width(0),m_Height(0),m_Depth(0),m_Pixels(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XImage::~XImage()
{
  delete [] m_Pixels;
  m_Pixels=0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XImage::Read(FILE* fp)
{
  XImageHeader img;
  bool error = false;
  if(fread(&img,sizeof(XImageHeader),1,fp) != 0)
  {
    m_ID = img.m_ID;

    XU16 s = 0;
    XChar *buffer=0;

    if(fread(&s,2,1,fp) != 0)
    {
      if(s)
      {
        buffer = new XChar[s+1];
        if(fread(buffer,1,s,fp) == 0)
        {
          error = true;
        }
      }
      else
      {
        error = true;
      }
      if(!error)
      {
        buffer[s] = '\0';
        m_FilePath = buffer;

        if(fread(&s,2,1,fp) != 0)
        {
          if(s)
          {
            delete [] buffer;
            buffer = new XChar[s+1];
            if(fread(buffer,1,s,fp) == 0)
            {
              error = true;
            }
          }
          else
          {
            error = true;
          }
          buffer[s] = '\0';
          m_MayaName = buffer;

          m_Width = static_cast<XU32>(pow(2.0f,img.m_WidthPower));
          m_Height = static_cast<XU32>(pow(2.0f,img.m_HeightPower));
          m_Depth  = img.m_Bpp + 1;

          if( img.m_WidthPower!=0 &&
            img.m_HeightPower!=0 &&
            img.m_Bpp != 0) 
          {
            m_Pixels = new XU8[ m_Width*m_Height*m_Depth ];
            if(fread(m_Pixels,1,m_Width*m_Height*m_Depth,fp) != 0)
            {
              delete [] buffer;
              return true;
            }
          }
        }
      }
    }
    delete [] buffer;
  }
  return true;
}


//----------------------------------------------------------------------------------------------------------------------
XU8 LogIt(XU16 v)
{
  switch(v) 
  {
  case 1:
    return 0;
  case 2:
    return 1;
  case 4:
    return 2;
  case 8:
    return 3;
  case 16:
    return 4;
  case 32:
    return 5;
  case 64:
    return 6;
  case 128:
    return 7;
  case 256:
    return 8;
  case 512:
    return 9;
  case 1024:
    return 10;
  case 2048:
    return 11;
  case 4096:
    return 12;
  case 8192:
    return 13;
  case 16384:
    return 14;
  case 32768:
    return 15;
  default:
    break;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XImage::Write(FILE* fp) 
{
  XImageHeader img;
  img.m_WidthPower = LogIt((XU16)m_Width);
  img.m_HeightPower = LogIt((XU16)m_Height);
  img.m_Bpp = m_Depth-1;
  img.m_ID = (XU16)m_ID;

  fwrite(&img,sizeof(XImageHeader),1,fp);

  XU16 s = static_cast<XU16>(m_FilePath.length());
  fwrite(&s,2,1,fp);
  if(s)
    fwrite(m_FilePath.c_str(),1,s,fp);

  s = static_cast<XU16>(m_MayaName.length());
  fwrite(&s,2,1,fp);
  if(s)
    fwrite(m_MayaName.c_str(),1,s,fp);

  if( img.m_WidthPower!=0 &&
      img.m_HeightPower!=0 &&
      img.m_Bpp != 0) 
  {
    fwrite(m_Pixels,1,m_Width*m_Height*m_Depth,fp);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XImage::GetFileName() const  
{
  return m_FilePath.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XImage::GetMayaName() const
{
  return m_MayaName.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
XU8* XImage::Pixels() const 
{
  return m_Pixels;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XImage::Width() const 
{
  return m_Width;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XImage::Height() const
{
  return m_Height;
}

//----------------------------------------------------------------------------------------------------------------------
XU8 XImage::Depth() const 
{
  return (XU8)m_Depth;
}

//----------------------------------------------------------------------------------------------------------------------
bool XImage::SetSize( const XU32 w,const XU32 h,const XU32 d/*=3*/ )
{
  m_Width = w;
  m_Height = h;
  m_Depth = d;
  delete [] m_Pixels;
  m_Pixels = new XU8[ m_Width * m_Height * m_Depth ];
  return true;
}
}
