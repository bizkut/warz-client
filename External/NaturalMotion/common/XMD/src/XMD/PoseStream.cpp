//----------------------------------------------------------------------------------------------------------------------
/// \file PoseStream.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XM2/Colour.h"
#include "XM2/Vector2.h"
#include "XM2/Vector3.h"
#include "XM2/Vector4.h"
#include "XM2/Quaternion.h"
#include "XM2/Matrix34.h"
#include "XMD/PoseStream.h"
#include "XMD/AnimationTake.h"
#include "XMD/FileIO.h"

namespace XMD 
{

//-------------------------------------------------------------------  XPoseStream :: Evaluate
// 
XPoseStream::XPoseStream(XAnimatedAttribute* attr,XBase* node, StreamDataType dataType) 
  : XAnimationStream(attr),m_PoseData(0),m_NumElements(0)
{
  switch(dataType)
  {
  case XAnimationStream::kReal:
    m_PoseData = new XReal[1];
    memset(m_PoseData,0,sizeof(XReal));
    break;
  case XAnimationStream::kVector2:
    m_PoseData = new XReal[2];
    memset(m_PoseData,0,2*sizeof(XReal));
    break;
  case XAnimationStream::kVector3:
    m_PoseData = new XReal[3];
    memset(m_PoseData,0,3*sizeof(XReal));
    break;
  case XAnimationStream::kVector4:
    m_PoseData = new XReal[4];
    memset(m_PoseData,0,4*sizeof(XReal));
    break;
  case XAnimationStream::kQuaternion:
    m_PoseData = new XReal[4];
    memset(m_PoseData,0,4*sizeof(XReal));
    break;
  case XAnimationStream::kMatrix:
    m_PoseData = new XReal[16];
    memset(m_PoseData,0,16*sizeof(XReal));
    m_PoseData[0] = m_PoseData[5] = m_PoseData[8] = m_PoseData[15] = 1.0f;
    break;
  case XAnimationStream::kRealArray:
    {
      XRealArray vals;
      node->GetProperty(attr->GetPropertyID(),vals);
      m_NumElements = (XU32)vals.size();
      m_PoseData = new XReal[m_NumElements];
      memset(m_PoseData,0,sizeof(XReal)*m_NumElements);
    }
    break;

  default:
    break;
  }
  m_StreamType = XAnimationStream::kPoseStream;
}

//----------------------------------------------------------------------------------------------------------------------
XPoseStream::~XPoseStream()
{
  delete [] m_PoseData;
}

//----------------------------------------------------------------------------------------------------------------------
XQuaternion XPoseStream::AsQuaternion() const
{
  return XQuaternion( m_PoseData[0], m_PoseData[1], m_PoseData[2], m_PoseData[3] );
}

//----------------------------------------------------------------------------------------------------------------------
XVector2 XPoseStream::AsVector2() const
{
  return XVector2( m_PoseData[0], m_PoseData[1] );
}

//----------------------------------------------------------------------------------------------------------------------
XVector3 XPoseStream::AsVector3() const
{
  return XVector3( m_PoseData[0], m_PoseData[1], m_PoseData[2] );
}

//----------------------------------------------------------------------------------------------------------------------
XVector4 XPoseStream::AsVector4() const
{
  return XVector4( m_PoseData[0], m_PoseData[1], m_PoseData[2], m_PoseData[3] );
}

//----------------------------------------------------------------------------------------------------------------------
XColour XPoseStream::AsColour3() const
{
  return XColour( m_PoseData[0], m_PoseData[1], m_PoseData[2] );
}

//----------------------------------------------------------------------------------------------------------------------
XColour XPoseStream::AsColour4() const
{
  return XColour( m_PoseData[0], m_PoseData[1], m_PoseData[2], m_PoseData[3] );
}

//----------------------------------------------------------------------------------------------------------------------
XReal XPoseStream::AsFloat() const
{
  return m_PoseData[0];
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XPoseStream::AsInteger() const
{
  return (XS32)m_PoseData[0];
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoseStream::AsBoolean() const
{
  return m_PoseData[0] > 0.5f;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoseStream::Set(const XQuaternion& value)
{
  m_PoseData[0] = value.x;
  m_PoseData[1] = value.y;
  m_PoseData[2] = value.z;
  m_PoseData[3] = value.w;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoseStream::Set(const XVector2& value)
{
  m_PoseData[0] = value.x;
  m_PoseData[1] = value.y;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoseStream::Set(const XVector3& value)
{
  m_PoseData[0] = value.x;
  m_PoseData[1] = value.y;
  m_PoseData[2] = value.z;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoseStream::Set(const XVector4& value)
{
  m_PoseData[0] = value.x;
  m_PoseData[1] = value.y;
  m_PoseData[2] = value.z;
  m_PoseData[3] = value.w;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoseStream::Set(const XColour& value)
{
  m_PoseData[0] = value.r;
  m_PoseData[1] = value.g;
  m_PoseData[2] = value.b;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoseStream::Set(const XReal& value)
{
  m_PoseData[0] = value;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoseStream::Set(const XS32& value)
{
  m_PoseData[0] = (XReal)value;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoseStream::Set(const bool& value)
{
  m_PoseData[0] = value ? 1.0f : 0.0f;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XPoseStream::GetDataSize() const
{
  return GetPoseDataSize() + XAnimationStream::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
void XPoseStream::Evaluate(const XReal time,XReal* output) const
{
  // if you have asserted here the animation stream is currently invalid
  XMD_ASSERT(m_PoseData);

  // if you have asserted here, the output memory location is invalid 
  XMD_ASSERT(output);

  // time is irellevent in a pose key. There's only one value.... 
  (void)time;

  // copy pose to the output
  memcpy(output,m_PoseData, GetPoseDataSize());
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoseStream::DoData(XFileIO& io)
{
  if (!XAnimationStream::DoData(io))
    return false;
  IO_CHECK( io.DoData(m_PoseData,GetPoseDataSize()/sizeof(XReal)) );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoseStream::ReadChunk(std::istream& ifs)
{
  XU32 n_elements = GetPoseDataSize() / sizeof(XReal);
  if(m_NumElements)
  {
    size_t k=0;
    for (size_t i=0;i<m_NumElements;++i)
    {
      std::string s;
      for(XU32 j=0;j!=n_elements;++j,++k)
      {
        ifs >> s >> m_PoseData[k];
      }
    }
  }
  else
  {
    for(XU32 j=0;j!=n_elements;++j)
    {
      ifs >> m_PoseData[j];
    }
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoseStream::WriteChunk(std::ostream& ofs)
{
  ofs << "\t\t\tPOSE_KEY\n";
  ofs << "\t\t\t{\n";
  XU32 n_elements = GetPoseDataSize() / sizeof(XReal);
  if(m_NumElements)
  {
    XU32 k=0;
    for(XU32 i=0;i!=m_NumElements;++i)
    {
      ofs << "\t\t\t\t[" << i << "]";
      for(XU32 j=0;j!=n_elements;++j,++k)
      {
        ofs << " " << m_PoseData[k];
      }
      ofs << "\n";
    }
  }
  else
  {
    ofs << "\t\t\t\t";
    for(XU32 j=0;j!=n_elements;++j)
    {
      ofs << m_PoseData[j] << " " ;
    }
    ofs << "\n";
  }
  ofs << "\t\t\t}\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU32 XPoseStream::GetPoseDataSize() const
{
  // all streams are of type float, so PoseDataSize = StreamTypeNumElements() * sizeof(float)
  return GetStreamTypeNumElements(GetStreamDataType()) * sizeof(float);
}
}
