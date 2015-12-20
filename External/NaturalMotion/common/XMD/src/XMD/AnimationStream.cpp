//----------------------------------------------------------------------------------------------------------------------
/// \file AnimationStream.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XM2/Colour.h"
#include "XM2/Vector2.h"
#include "XM2/Vector3.h"
#include "XM2/Vector4.h"
#include "XM2/Quaternion.h"
#include "XM2/Matrix34.h"

#include "XMD/AnimationStream.h"
#include "XMD/PoseStream.h"
#include "XMD/FCurveStream.h"
#include "XMD/SampledStream.h"
#include "XMD/AnimationTake.h"
#include "XMD/FileIO.h"

namespace XMD
{
NMTL_POD_VECTOR_EXPORT(XAnimationStream*,XMD_EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
XAnimationStream::XAnimationStream(XAnimatedAttribute* parent)
{
  m_ParentAttr = parent;
  m_StreamDataType = (StreamDataType)parent->GetStreamDataType();
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimationStream::GetNumArrayElements() const
{
  XMD_ASSERT(m_ParentAttr);
  if(m_ParentAttr)
  {
    XBase* node = m_ParentAttr->GetNode();

    switch (node->GetPropertyType(m_ParentAttr->GetPropertyID()))
    {
    case XPropertyType::kNodeArray:
      {
        XList val;
        node->GetProperty(m_ParentAttr->GetPropertyID(),val);
        return (XU32)val.size();
      }
      break;
    case XPropertyType::kRealArray:
      {
        XRealArray val;
        node->GetProperty(m_ParentAttr->GetPropertyID(),val);
        return (XU32)val.size();
      }
      break;
    case XPropertyType::kUnsignedArray:
      {
        XU32Array val;
        node->GetProperty(m_ParentAttr->GetPropertyID(),val);
        return (XU32)val.size();
      }
      break;
    case XPropertyType::kVector3Array:
      {
        XVector3Array val;
        node->GetProperty(m_ParentAttr->GetPropertyID(),val);
        return (XU32)val.size();
      }
      break;
    default:
      break;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XPoseStream* XAnimationStream::AsPoseStream()
{
  if (m_StreamType == kPoseStream)
  {
    return static_cast<XPoseStream*>(this);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XSampledStream* XAnimationStream::AsSampledStream()
{
  if (m_StreamType == kSampledStream)
  {
    return static_cast<XSampledStream*>(this);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XFCurveStream* XAnimationStream::AsFCurveStream()
{
  if (m_StreamType == kFCurveStream)
  {
    return static_cast<XFCurveStream*>(this);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XPoseStream* XAnimationStream::AsPoseStream() const
{
  if (m_StreamType == kPoseStream)
  {
    return static_cast<const XPoseStream*>(this);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XSampledStream* XAnimationStream::AsSampledStream() const
{
  if (m_StreamType == kSampledStream)
  {
    return static_cast<const XSampledStream*>(this);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XFCurveStream* XAnimationStream::AsFCurveStream() const
{
  if (m_StreamType == kFCurveStream)
  {
    return static_cast<const XFCurveStream*>(this);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationStream::Evaluate(const XReal time,XReal& output) const
{
  Evaluate(time,&output);
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationStream::Evaluate(const XReal time,XVector2& output) const
{
  Evaluate(time,output.data);
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationStream::Evaluate(const XReal time,XVector3& output) const
{
  Evaluate(time,output.data);
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationStream::Evaluate(const XReal time,XVector4& output) const
{
  Evaluate(time,output.data);
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationStream::Evaluate(const XReal time,XQuaternion& output) const
{
  Evaluate(time,output.data);
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationStream::Evaluate(const XReal time,XMatrix& output) const
{
  Evaluate(time,output.data);
}

//----------------------------------------------------------------------------------------------------------------------
void XAnimationStream::Evaluate(const XReal time,XColour& output) const
{
  Evaluate(time,output.data);
}

//----------------------------------------------------------------------------------------------------------------------
XAnimationStream::StreamDataType XAnimationStream::GetStreamDataType() const
{
  return m_StreamDataType;
}

//----------------------------------------------------------------------------------------------------------------------
XAnimationStream::StreamType XAnimationStream::GetStreamType() const
{
  return m_StreamType;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimationStream::GetMinTime() const
{
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAnimationStream::GetMaxTime() const
{
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XId XAnimationStream::GetNodeID() const
{
  return m_ParentAttr->GetNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAnimationStream::GetPropertyID() const
{
  return m_ParentAttr->GetPropertyID();
}

//----------------------------------------------------------------------------------------------------------------------
XM2::XU32 XAnimationStream::GetDataSize() const
{
  return sizeof(StreamType)+sizeof(StreamDataType);
}

//----------------------------------------------------------------------------------------------------------------------
XAnimatedAttribute* XAnimationStream::GetAttribute()
{
  return m_ParentAttr;
}

//------------------------------------------------------------------- XAnimationStream :: ~GetStreamTypeSize
XU32 XAnimationStream::GetStreamTypeNumElements(StreamDataType type)
{
  switch(type)
  {
  case kReal:
  case kRealArray:
    return 1;

  case kVector2:
    return 2;

  case kVector3:
    return 3;

  case kVector4:
  case kQuaternion:
    return 4;

  case kMatrix:
    return 16;

  default: // unsupported stream data type
    XMD_ASSERT(0);
    return 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
const XAnimatedAttribute* XAnimationStream::GetAttribute() const
{
  return m_ParentAttr;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimationStream::DoData(XFileIO&)
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimationStream::ReadChunk(std::istream&)
{
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAnimationStream::WriteChunk(std::ostream&)
{
  return false;
}
}
