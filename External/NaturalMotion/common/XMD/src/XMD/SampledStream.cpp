//----------------------------------------------------------------------------------------------------------------------
/// \file SampledStream.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/SampledStream.h"
#include "XM2/Colour.h"
#include "XM2/Vector2.h"
#include "XM2/Vector3.h"
#include "XM2/Vector4.h"
#include "XM2/Quaternion.h"
#include "XM2/Matrix34.h"
#include "XMD/FileIO.h"
#include "XMD/AnimationTake.h"
#include <sstream>

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XSampledStream::XSampledStream(XAnimatedAttribute* attr, const XU32 elementSize, const XU32 numFrames)
  : XAnimationStream(attr)
{
  m_ElementsPerSample = elementSize;
  m_StreamType = XAnimationStream::kSampledStream;
  m_Samples.resize(elementSize * numFrames);
}

//----------------------------------------------------------------------------------------------------------------------
XSampledStream::XSampledStream(XAnimatedAttribute* attr) 
  : XAnimationStream(attr),m_ElementsPerSample(0), m_Samples()
{    
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XSampledStream::GetDataSize() const
{
  size_t s = XAnimationStream::GetDataSize();
  s += sizeof(XU32);
  s += m_Samples.size()*sizeof(XReal);
  return (XU32)s;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::DoData(XFileIO& io)
{
  if(io.DoDataVector(m_Samples) == 0)
  {
    return false;
  }

  XAnimationTake* animTake = m_ParentAttr->GetAnimatedNode()->GetTake();
  animTake->SetNumFrames(static_cast<XM2::XU32>(m_Samples.size()) / m_ElementsPerSample);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::ReadChunk(std::istream& ifs)
{
  unsigned index = 0;
  // Different exporters write different sample counts based upon whether they rounded up or down the frame count. 
  // We don't know how many samples we are going to read so read until a } occurs
  std::string nextString;
  std::streamoff lastPos = ifs.tellg();
  ifs >> nextString;
  float floatVal;
  while(nextString != "}")
  {
    std::istringstream nextStringStream(nextString);
    nextStringStream >> floatVal;
    m_Samples[index++] = floatVal;
    XMD_ASSERT(index <= m_Samples.size());

    lastPos = ifs.tellg();
    ifs >> nextString;
  }

  // Move back before the close brackets
  ifs.seekg(lastPos);

  // Check that complete samples were read
  if(m_Samples.size() % m_ElementsPerSample != 0)
  {
    return false;
  }

  XAnimationTake* animTake = m_ParentAttr->GetAnimatedNode()->GetTake();
  animTake->SetNumFrames(static_cast<XM2::XU32>(m_Samples.size()) / m_ElementsPerSample);

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::WriteChunk(std::ostream& ofs)
{
  ofs << "\t\t\tSAMPLED_KEYS\n";
  ofs << "\t\t\t{\n";
  for(XU32 i=0;i<m_Samples.size();++i)
  {
    if((i%m_ElementsPerSample)==0)
      ofs << "\n\t\t\t\t";
    ofs << m_Samples[i] << " ";
  }
  ofs << "\n\t\t\t}\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XSampledStream::GetNumElemtents() const
{
  return static_cast<XU32>(m_Samples.size()/m_ElementsPerSample);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XSampledStream::GetElemtentSize() const
{
  return m_ElementsPerSample;
}

//----------------------------------------------------------------------------------------------------------------------
XQuaternion XSampledStream::AsQuaternion(const XU32 index) const
{
  const XU32 _index = index * m_ElementsPerSample;
  const bool _check = _index<m_Samples.size();
  XMD_ASSERT(_check);
  if(!_check)
    return XQuaternion();

  // make sure we actually have 4 samples per element
  XMD_ASSERT(m_ElementsPerSample==4);
  return XQuaternion( m_Samples[_index], m_Samples[_index+1], m_Samples[_index+2], m_Samples[_index+3] );
}

//----------------------------------------------------------------------------------------------------------------------
XVector2 XSampledStream::AsVector2(const XU32 index) const
{
  const XU32 _index = index * m_ElementsPerSample;
  const bool _check = _index<m_Samples.size();
  XMD_ASSERT(_check);
  if(!_check)
    return XVector2();    

  // make sure we actually have 2 samples per element
  XMD_ASSERT(m_ElementsPerSample==2);
  return XVector2( m_Samples[_index], m_Samples[_index+1] );
}

//-------------------------------------------------------------------  XSampledStream :: AsVector3
// 
XVector3 XSampledStream::AsVector3(const XU32 index) const
{
  const XU32 _index = index * m_ElementsPerSample;
  const bool _check = _index<m_Samples.size();
  XMD_ASSERT(_check);
  if(!_check)
    return XVector3();    

  // make sure we actually have 3 samples per element
  XMD_ASSERT(m_ElementsPerSample==3);
  return XVector3( m_Samples[_index], m_Samples[_index+1], m_Samples[_index+2] );
}

//----------------------------------------------------------------------------------------------------------------------
XVector4 XSampledStream::AsVector4(const XU32 index) const
{
  const XU32 _index = index * m_ElementsPerSample;
  const bool _check = _index<m_Samples.size();
  XMD_ASSERT(_check);
  if(!_check)
    return XVector4();    

  // make sure we actually have 4 samples per element
  XMD_ASSERT(m_ElementsPerSample==4);
  return XVector4( m_Samples[_index], m_Samples[_index+1], m_Samples[_index+2], m_Samples[_index+3] );
}

//----------------------------------------------------------------------------------------------------------------------
XColour XSampledStream::AsColour(const XU32 index) const
{
  const XU32 _index = index * m_ElementsPerSample;
  const bool _check = _index<m_Samples.size();
  XMD_ASSERT(_check);
  if(!_check)
    return XColour();

  // make sure we actually have 3 samples per element
  XMD_ASSERT(m_ElementsPerSample==3);
  return XColour( m_Samples[_index], m_Samples[_index+1], m_Samples[_index+2] );
}

//----------------------------------------------------------------------------------------------------------------------
const XReal* XSampledStream::AsRawData(const XU32 index) const
{
  const XU32 _index = index * m_ElementsPerSample;
  XMD_ASSERT(_index < m_Samples.size());
  return &(m_Samples[_index]);
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::AsBoolean(const XU32 index) const
{
  // make sure we actually have 1 sample per element. Otherwise it is
  // assumed that there is a mistake of some form. 
  XMD_ASSERT(m_ElementsPerSample==1);

  // ensure we have a valid index
  XMD_ASSERT(index<m_Samples.size());

  // I'll define true as greater than 0.5f
  return m_Samples[ index ] >= 0.5f;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XSampledStream::AsInteger(const XU32 index) const
{
  // make sure we actually have 1 sample per element. Otherwise it is
  // assumed that there is a mistake of some form. 
  XMD_ASSERT(m_ElementsPerSample==1);

  // ensure we have a valid index
  XMD_ASSERT(index<m_Samples.size());

  // I'll define true as greater than 0.5f
  return static_cast<XS32>(m_Samples[ index ]);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XSampledStream::AsReal(const XU32 index) const
{
  // It is assumed that if you are grabbing data as a float, it should
  // propbably have a single element per sample.
  XMD_ASSERT(m_ElementsPerSample==1);

  // ensure we have a valid index
  XMD_ASSERT(index<m_Samples.size());

  return m_Samples[ index ];
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::SetSample(const XU32 index,const XQuaternion& value) 
{
  XMD_ASSERT(m_ElementsPerSample==4);
  if(m_ElementsPerSample!=4)
    return false;

  const XU32 _index = index * m_ElementsPerSample;

  // ensure we have a valid index
  XMD_ASSERT((_index+3)<m_Samples.size());

  m_Samples[_index+0] = value.x;
  m_Samples[_index+1] = value.y;
  m_Samples[_index+2] = value.z;
  m_Samples[_index+3] = value.w;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::SetSample(const XU32 index,const XVector2& value) 
{
  XMD_ASSERT(m_ElementsPerSample==2);
  if(m_ElementsPerSample!=2)
    return false;

  const XU32 _index = index * m_ElementsPerSample;

  // ensure we have a valid index
  XMD_ASSERT((_index+1)<m_Samples.size());

  m_Samples[_index+0] = value.x;
  m_Samples[_index+1] = value.y;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::SetSample(const XU32 index,const XVector3& value) 
{
  XMD_ASSERT(m_ElementsPerSample==3);
  if(m_ElementsPerSample!=3)
    return false;

  const XU32 _index = index * m_ElementsPerSample;

  // ensure we have a valid index
  XMD_ASSERT((_index+2)<m_Samples.size());

  m_Samples[_index+0] = value.x;
  m_Samples[_index+1] = value.y;
  m_Samples[_index+2] = value.z;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::SetSample(const XU32 index,const XVector4& value) 
{
  XMD_ASSERT(m_ElementsPerSample==4);
  if(m_ElementsPerSample!=4)
    return false;

  const XU32 _index = index * m_ElementsPerSample;

  // ensure we have a valid index
  XMD_ASSERT((_index+3)<m_Samples.size());

  m_Samples[_index+0] = value.x;
  m_Samples[_index+1] = value.y;
  m_Samples[_index+2] = value.z;
  m_Samples[_index+3] = value.w;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::SetSample(const XU32 index,const XColour& value) 
{
  XMD_ASSERT(m_ElementsPerSample==3);

  const XU32 _index = index * m_ElementsPerSample;

  // ensure we have a valid index
  XMD_ASSERT((_index+2)<m_Samples.size());

  m_Samples[_index+0] = value.r;
  m_Samples[_index+1] = value.g;
  m_Samples[_index+2] = value.b;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::SetSample(const XU32 index,const XReal* raw_data) 
{
  const XU32 _index = index * m_ElementsPerSample;

  // ensure we have a valid index
  XMD_ASSERT((_index+m_ElementsPerSample-1)<m_Samples.size());

  for(XU32 i=0;i<m_ElementsPerSample;++i)
    m_Samples[_index+i] = raw_data[i];
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::SetSample(const XU32 index,const bool value) 
{
  XMD_ASSERT(m_ElementsPerSample==1);

  // ensure we have a valid index
  XMD_ASSERT(index<m_Samples.size());

  m_Samples[index] = value ? 1.0f : 0.0f;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::SetSample(const XU32 index,const XS32 value) 
{
  XMD_ASSERT(m_ElementsPerSample==1);

  // ensure we have a valid index
  XMD_ASSERT(index<m_Samples.size());

  m_Samples[index] = (XReal)value;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSampledStream::SetSample(const XU32 index,const XReal value) 
{
  XMD_ASSERT(m_ElementsPerSample==1);

  // ensure we have a valid index
  XMD_ASSERT(index<m_Samples.size());

  m_Samples[index] = value;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XSampledStream::Evaluate(const XReal time,XReal* output) const
{
  // XAnimationTake* take = m_ParentAttr->m_Node->m_Take;
  for (XU32 i=0;i!=m_Samples.size();i+=m_ElementsPerSample)
  {
    //if (1) /// \todo  make this search for correct frames.
    {
      const XReal time_start=time;
      const XReal time_end=time+1.0f; /// \todo FIX!
      const XU32 start_idx = i * m_ElementsPerSample;
      const XU32 end_idx   = start_idx + m_ElementsPerSample;
      const XReal t = (time - time_start)/(time_start-time_end);
      switch(GetStreamDataType())
      {
      case kReal:
        {
          output[0] = m_Samples[start_idx+0] + t*( m_Samples[end_idx+0]-m_Samples[start_idx+0] );
        }
        break;

      case kVector2:
        {
          output[0] = m_Samples[start_idx+0] + t*( m_Samples[end_idx+0]-m_Samples[start_idx+0] );
          output[1] = m_Samples[start_idx+1] + t*( m_Samples[end_idx+1]-m_Samples[start_idx+1] );
        }
        break;

      case kVector3:
        {
          output[0] = m_Samples[start_idx+0] + t*( m_Samples[end_idx+0]-m_Samples[start_idx+0] );
          output[1] = m_Samples[start_idx+1] + t*( m_Samples[end_idx+1]-m_Samples[start_idx+1] );
          output[2] = m_Samples[start_idx+2] + t*( m_Samples[end_idx+2]-m_Samples[start_idx+2] );
        }
        break;

      case kVector4:
        {
          output[0] = m_Samples[start_idx+0] + t*( m_Samples[end_idx+0]-m_Samples[start_idx+0] );
          output[1] = m_Samples[start_idx+1] + t*( m_Samples[end_idx+1]-m_Samples[start_idx+1] );
          output[2] = m_Samples[start_idx+2] + t*( m_Samples[end_idx+2]-m_Samples[start_idx+2] );
          output[3] = m_Samples[start_idx+3] + t*( m_Samples[end_idx+3]-m_Samples[start_idx+3] );
        }
        break;

      case kQuaternion:
        {
          XQuaternion a(m_Samples[start_idx+0],m_Samples[start_idx+1],m_Samples[start_idx+2],m_Samples[start_idx+3]);
          XQuaternion b(m_Samples[end_idx+0],m_Samples[end_idx+1],m_Samples[end_idx+2],m_Samples[end_idx+3]);
          XQuaternion r;
          r.slerp(a,b,t);
          memcpy(output,r.data,sizeof(XReal)*4);
        }
        break;

      default:
        XMD_ASSERT(0);
        break;
      }
    }
  }
}
}
