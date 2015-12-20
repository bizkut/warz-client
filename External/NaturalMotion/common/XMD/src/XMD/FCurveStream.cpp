//----------------------------------------------------------------------------------------------------------------------
/// \file FCurveStream.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/FCurveStream.h"
#include "XMD/AnimationTake.h"
#include "XMD/Bone.h"
#include "XMD/FileIO.h"
#include "XM2/EulerAngles.h"
#include <math.h>
using namespace XM2;
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XU32 XFCurve::AddKeyFrame(const XReal time,const XReal value)
{
  XU32 index=0;
  for (;index<GetNumKeyframes();++index)
  {
    XU32 true_index = index * GetNumElementsPerKey();
    if (time < m_KeyFrameData[true_index])
    {
      break;
    }
  }

  // now index refers to the element to move back...
  {
    XU32 nframes = GetNumKeyframes();
    m_KeyFrameData.resize( m_KeyFrameData.size() + m_ElementsPerKey, 0 );

    XU32 true_start_index = index * GetNumElementsPerKey();
    XU32 true_next_index = true_start_index +  GetNumElementsPerKey();

    if(nframes>0)
    {
      XU32 end_index = GetNumElementsPerKey() * GetNumKeyframes();
      for (XU32 i=end_index-1;i>=true_next_index;++i)
      {
        XU32 ii = i - GetNumElementsPerKey();
        m_KeyFrameData[ii] = m_KeyFrameData[i];
      }
    }

    for (XU32 i=true_start_index;i<true_next_index;++i)
    {
      m_KeyFrameData[i]=0;
    }
    m_KeyFrameData[true_start_index] = time;
    m_KeyFrameData[true_start_index+1] = value;
  }

  return index;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyTime(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    return m_KeyFrameData[ m_ElementsPerKey*idx ];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyValue(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    return m_KeyFrameData[ m_ElementsPerKey * idx + 1 ];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyThreshold(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kSmoothStep)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 2 ];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyTension(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kTCB1D ||
       m_InterpolationType == XFCurve::kTCB2D ||
       m_InterpolationType == XFCurve::kCardinal1D ||
       m_InterpolationType == XFCurve::kCardinal2D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 2];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyContinuity(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kTCB1D ||
       m_InterpolationType == XFCurve::kTCB2D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 3 ];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyBias(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kTCB1D ||
       m_InterpolationType == XFCurve::kTCB2D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 4 ];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyInValue(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kBezier1D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 2];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyInValueX(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kBezier2D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 2];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyInValueY(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kBezier2D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 3];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyOutValue(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kBezier1D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 3];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyOutValueX(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kBezier2D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 4];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyOutValueY(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kBezier2D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 5];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyInTangent(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kHermite1D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 2];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyInTangentX(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kHermite2D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 2];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyInTangentY(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kHermite2D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 3];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyOutTangent(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kHermite1D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 3];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyOutTangentX(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kHermite2D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 4];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetKeyOutTangentY(const XU32 idx) const
{
  if (m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kHermite2D)
    {
      return m_KeyFrameData[ m_ElementsPerKey * idx + 5];
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyTime(const XU32 idx,const XReal t)
{
  if (m_NumberKeyframes>idx)
  {
    m_KeyFrameData[ m_ElementsPerKey * idx ] = t;

    /// \todo ckeck to see if i need to swap over the key values!
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyValue(const XU32 idx,const XReal v)
{
  if (m_NumberKeyframes>idx)
  {
    m_KeyFrameData[ m_ElementsPerKey * idx + 1 ] = v;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyTension(const XU32 idx,const XReal t)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kTCB1D ||
       m_InterpolationType == XFCurve::kTCB2D ||
       m_InterpolationType == XFCurve::kCardinal1D ||
       m_InterpolationType == XFCurve::kCardinal2D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 2 ] = t;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyThreshold(const XU32 idx,const XReal t)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kSmoothStep)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 2 ] = t;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyContinuity(const XU32 idx,const XReal c)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kTCB1D ||
       m_InterpolationType == XFCurve::kTCB2D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 3 ] = c;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyBias(const XU32 idx,const XReal b)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kTCB1D ||
       m_InterpolationType == XFCurve::kTCB2D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 4 ] = b;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyInValue(const XU32 idx,const XReal v)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kBezier1D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 2 ] = v;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyInValueX(const XU32 idx,const XReal vx)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kBezier2D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 2 ] = vx;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyInValueY(const XU32 idx,const XReal vy)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kBezier2D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 3 ] = vy;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyOutValue(const XU32 idx,const XReal v)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kBezier1D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 3 ] = v;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyOutValueX(const XU32 idx,const XReal vx)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kBezier2D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 4 ] = vx;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyOutValueY(const XU32 idx,const XReal vy)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kBezier2D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 5 ] = vy;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyInTangent(const XU32 idx,const XReal t)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kHermite1D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 2 ] = t;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyInTangentX(const XU32 idx,const XReal tx)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kHermite2D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 2 ] = tx;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyInTangentY(const XU32 idx,const XReal ty)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kHermite2D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 3 ] = ty;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyOutTangent(const XU32 idx,const XReal t)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kHermite1D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 3 ] = t;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyOutTangentX(const XU32 idx,const XReal tx)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kHermite2D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 4 ] = tx;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::SetKeyOutTangentY(const XU32 idx,const XReal ty)
{
  if(m_NumberKeyframes>idx)
  {
    if(m_InterpolationType == XFCurve::kHermite2D)
    {
      m_KeyFrameData[ m_ElementsPerKey * idx + 5 ] = ty;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XFCurve::InterpolationType XFCurve::GetInterpolationType() const
{
  return m_InterpolationType;
}

//----------------------------------------------------------------------------------------------------------------------
void XFCurve::SetInterpolationType(const InterpolationType type)
{
  /// \todo  re-assign the data stored in this FCurve.
  m_InterpolationType=type;
  switch(m_InterpolationType)
  {
  case kStep:
  case kLinear:
  case kCatmullRom1D:
  case kCatmullRom2D:
    m_ElementsPerKey = 2;
    break;

  case kCardinal1D:
  case kCardinal2D:
  case kSmoothStep:
    m_ElementsPerKey = 3;
    break;

  case kTCB1D:
  case kTCB2D:
    m_ElementsPerKey = 5;
    break;

  case kBezier1D:
  case kHermite1D:
    m_ElementsPerKey = 4;
    break;

  case kBezier2D:
  case kHermite2D:
    m_ElementsPerKey = 6;
    break;

  default:
    XMD_ASSERT(0);
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
XFCurve::InfinityType XFCurve::GetPreInfinity() const
{
  return m_PreInfinity;
}

//----------------------------------------------------------------------------------------------------------------------
XFCurve::InfinityType XFCurve::GetPostInfinity() const
{
  return m_PostInfinity;
}

//----------------------------------------------------------------------------------------------------------------------
void XFCurve::SetPreInfinity(const XFCurve::InfinityType infinity_type)
{
  m_PreInfinity = infinity_type;
}

//----------------------------------------------------------------------------------------------------------------------
void XFCurve::SetPostInfinity(const XFCurve::InfinityType infinity_type)
{
  m_PostInfinity = infinity_type;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XFCurve::GetNumKeyframes() const
{
  if(!m_ElementsPerKey)
    return 0;
  return (XU32)m_KeyFrameData.size()/m_ElementsPerKey;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XFCurve::GetNumElementsPerKey() const
{
  return m_ElementsPerKey;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetMinTime() const
{
  return (m_KeyFrameData.size()) ? m_KeyFrameData[0] : 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetMaxTime() const
{
  return (m_KeyFrameData.size()) ? m_KeyFrameData[m_KeyFrameData.size()-m_ElementsPerKey] : 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
const char* gInterpolationStrs[] =
{
  "STEP",
  "LINEAR",
  "CATMULL_ROM",
  "CARDINAL",
  "TCB",
  "BEZIER_1D",
  "BEZIER_2D",
  "HERMITE_1D",
  "HERMITE_2D",
  0
};

//----------------------------------------------------------------------------------------------------------------------
const char* gInfinityStrings[] =
{
  "kInfinityConstant",
  "kInfinityLinear",
  "kInfinityOscillate",
  "kInfinityCycle",
  "kInfinityCycleWithOffset",
  0
};

//----------------------------------------------------------------------------------------------------------------------
XU32 XFCurve::GetDataSize() const
{
  size_t s = sizeof(InterpolationType) + sizeof(InfinityType)*2 + sizeof(XS32)*2 + sizeof(XU32);
  s += sizeof(XReal)*m_KeyFrameData.size();
  return (XU32)s;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::DoData(XFileIO& io)
{
  io.DoData(&m_InterpolationType);
  io.DoData(&m_PreInfinity);
  io.DoData(&m_PostInfinity);
  io.DoData(&m_Component);
  io.DoData(&m_Index);
  if(io.IsReading())
    SetInterpolationType(m_InterpolationType);
  io.DoDataVector(m_KeyFrameData);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::Read(std::istream& ifs)
{
  READ_CHECK("ANIM_CURVE",ifs);
  READ_CHECK("{",ifs);

  XString interp;
  XU32 numKeys=0;
  XString pre;
  XString post;

  READ_CHECK("interpolation",ifs);
  ifs >> interp;

  {
    XU32 i   = (XU32)kStep;
    XU32 end = (XU32)kHermite2D;
    for (; i <= end; ++i)
    {
      if( interp == gInterpolationStrs[i] )
      {
        m_InterpolationType = (InterpolationType)i;
        break;
      }
    }
  }
  SetInterpolationType(m_InterpolationType);

  READ_CHECK("num_keys",ifs);
  ifs >> numKeys;
  READ_CHECK("pre_infinity",ifs);
  ifs >> pre;
  READ_CHECK("post_infinity",ifs);
  ifs >> post;
  READ_CHECK("component",ifs);
  ifs >> m_Component;
  READ_CHECK("index",ifs);
  ifs >> m_Index;

  // pre infinity type
  {
    XU32 i   = (XU32)kInfinityConstant;
    XU32 end = (XU32)kInfinityCycleWithOffset;
    for (; i <= end; ++i)
    {
      if( pre == gInfinityStrings[i] )
      {
        m_PreInfinity = (InfinityType)i;
        break;
      }
    }
  }

  // post infinity type
  {
    XU32 i   = (XU32)kInfinityConstant;
    XU32 end = (XU32)kInfinityCycleWithOffset;
    for (; i <= end; ++i)
    {
      if( post == gInfinityStrings[i] )
      {
        m_PostInfinity = (InfinityType)i;
        break;
      }
    }
  }

  m_KeyFrameData.resize(GetNumElementsPerKey()*numKeys);
  for (size_t i=0;i!=m_KeyFrameData.size();++i)
  {
    ifs >> m_KeyFrameData[i];
  }

  READ_CHECK("}",ifs);

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::Write(std::ostream& ofs)
{
  ofs << "\t\t\t\tANIM_CURVE " << "\n";
  ofs << "\t\t\t\t{\n";
  ofs << "\t\t\t\t\tinterpolation " << gInterpolationStrs[ m_InterpolationType ] << "\n";
  ofs << "\t\t\t\t\tnum_keys " << GetNumKeyframes() << "\n";
  ofs << "\t\t\t\t\tpre_infinity " << gInfinityStrings[ m_PreInfinity ] << "\n";
  ofs << "\t\t\t\t\tpost_infinity " << gInfinityStrings[ m_PostInfinity ] << "\n";
  ofs << "\t\t\t\t\tcomponent " << m_Component << "\n";
  ofs << "\t\t\t\t\tindex " << m_Index;

  for (size_t i=0;i!=m_KeyFrameData.size();++i)
  {
    if( (i%GetNumElementsPerKey()) == 0 )
    {
      ofs << "\n\t\t\t\t\t\t";
    }
    ofs << m_KeyFrameData[i] << " ";
  }
  ofs << "\n";
  ofs << "\t\t\t\t}\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
XFCurveStream::XFCurveStream(XAnimatedAttribute* attr) : XAnimationStream(attr)
{
  m_StreamType = XAnimationStream::kFCurveStream;
}

//----------------------------------------------------------------------------------------------------------------------
const XReal* XFCurve::GetKeyData(const XU32 idx) const
{
  return idx < GetNumKeyframes() ? &m_KeyFrameData[ idx*m_ElementsPerKey ] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XFCurveStream::GetDataSize() const
{
  XU32 s = XAnimationStream::GetDataSize();
  s+=(XU32)sizeof(XU32);

  XFCurveArray::const_iterator it = m_FCurves.begin();
  for (;it != m_FCurves.end();++it)
  {
    s += (*it)->GetDataSize();
  }
  return s;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurveStream::DoData(XFileIO& io)
{
  if(!XAnimationStream::DoData(io))
    return false;

  XU32 num = (XU32)m_FCurves.size();
  io.DoData(&num);

  if (io.IsReading())
  {
    for (XU32 i=0;i<num;++i)
    {
      m_FCurves.push_back(new XFCurve());
    }
  }

  XFCurveArray::iterator it = m_FCurves.begin();
  for (;it != m_FCurves.end();++it)
  {
    if(!(*it)->DoData(io))
      return false;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurveStream::ReadChunk(std::istream& ifs)
{
  READ_CHECK("num_fcurves",ifs);

  XU32 sz=0;
  ifs >> sz;
  m_FCurves.resize(sz,0);

  XFCurveArray::iterator it = m_FCurves.begin();
  for (;it != m_FCurves.end();++it)
  {
    *it = new XFCurve();
    if(!(*it)->Read(ifs))
      return false;
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurveStream::WriteChunk(std::ostream& ofs)
{
  ofs << "\t\t\tFCURVE_STREAM\n\t\t\t{\n";
  ofs << "\t\t\t\tnum_fcurves " << m_FCurves.size() << "\n";
  XFCurveArray::iterator it = m_FCurves.begin();
  for (;it != m_FCurves.end();++it)
  {
    if(!(*it)->Write(ofs))
      return false;
  }
  ofs << "\t\t\t}\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurveStream::GetIsAnimatedArray() const
{
  /// \todo  implement!
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurveStream::GetHasComponents() const
{
  /// \todo  implement!
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XFCurve* XFCurveStream::GetCurve()
{
  XFCurveArray::iterator it = m_FCurves.begin();
  for(;it != m_FCurves.end(); ++it )
  {
    XFCurve* crv = *it;
    XMD_ASSERT(crv);
    if( (crv->m_Component == -1) && (crv->m_Index==-1))
      return crv;
  }

  XFCurve* curve = new XFCurve();
  curve->m_Component = -1;
  curve->m_Index = -1;
  curve->m_InterpolationType = XFCurve::kBezier2D;
  curve->m_NumberKeyframes=0;
  curve->m_PostInfinity = XFCurve::kInfinityConstant;
  curve->m_PreInfinity = XFCurve::kInfinityConstant;
  curve->m_ElementsPerKey=6;
  m_FCurves.push_back(curve);

  // MORPH-21319: Validity check
  return curve;
}

//----------------------------------------------------------------------------------------------------------------------
XFCurve* XFCurveStream::GetCurve(const XU32 component)
{
  XFCurveArray::iterator it = m_FCurves.begin();
  for(;it != m_FCurves.end(); ++it )
  {
    XFCurve* crv = *it;
    XMD_ASSERT(crv);
    if( (crv->m_Component == (XS32)component) && (crv->m_Index==-1) )
      return crv;
  }

  XFCurve* curve = new XFCurve();
  curve->m_Component = component;
  curve->m_Index = -1;
  curve->m_InterpolationType = XFCurve::kBezier2D;
  curve->m_NumberKeyframes=0;
  curve->m_PostInfinity = XFCurve::kInfinityConstant;
  curve->m_PreInfinity = XFCurve::kInfinityConstant;
  curve->m_ElementsPerKey=6;
  m_FCurves.push_back(curve);

  // \todo insert validity check
  return curve;
}

//----------------------------------------------------------------------------------------------------------------------
XFCurve* XFCurveStream::GetCurve(const XU32 component,const XU32 index)
{
  XFCurveArray::iterator it = m_FCurves.begin();
  for(;it != m_FCurves.end(); ++it )
  {
    XFCurve* crv = *it;
    XMD_ASSERT(crv);
    if( (crv->m_Component == (XS32)component) && (crv->m_Index==(XS32)index) )
      return crv;
  }

  XFCurve* curve = new XFCurve();
  curve->m_Component = component;
  curve->m_Index = index;
  curve->m_InterpolationType = XFCurve::kBezier2D;
  curve->m_NumberKeyframes=0;
  curve->m_PostInfinity = XFCurve::kInfinityConstant;
  curve->m_PreInfinity = XFCurve::kInfinityConstant;
  curve->m_ElementsPerKey=6;
  m_FCurves.push_back(curve);

  /// \todo insert validity check
  return curve;
}

//----------------------------------------------------------------------------------------------------------------------
const XFCurve* XFCurveStream::GetCurve() const
{
  XFCurveArray::const_iterator it = m_FCurves.begin();
  for(;it != m_FCurves.end(); ++it )
  {
    const XFCurve* crv = *it;
    XMD_ASSERT(crv);
    if( (crv->m_Component == -1) && (crv->m_Index==-1))
      return crv;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XFCurve* XFCurveStream::GetCurve(const XU32 component) const
{
  XFCurveArray::const_iterator it = m_FCurves.begin();
  for(;it != m_FCurves.end(); ++it )
  {
    const XFCurve* crv = *it;
    XMD_ASSERT(crv);
    if( (crv->m_Component == (XS32)component) && (crv->m_Index==-1) )
      return crv;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XFCurve* XFCurveStream::GetCurve(const XU32 component,const XU32 index) const
{
  XFCurveArray::const_iterator it = m_FCurves.begin();
  for(;it != m_FCurves.end(); ++it )
  {
    const XFCurve* crv = *it;
    XMD_ASSERT(crv);
    if( (crv->m_Component == (XS32)component) && (crv->m_Index==(XS32)index) )
      return crv;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::Get1DCardinalTangent(const XReal p,const XU32 key_index0) const
{
  XReal cont = GetKeyContinuity(key_index0);
  XReal pbefore;
  XReal pafter;
  if (key_index0>0)
  {
    pbefore = GetKeyValue(key_index0-1);
  }
  else
  {
    pbefore = p;
  }
  if (key_index0<GetNumKeyframes()-1)
  {
    pafter = GetKeyValue(key_index0+1);
  }
  else
  {
    pafter = p;
  }
  return 0.5f*(1.0f-cont)*(pafter - pbefore);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::Get1DCatmullTangent(const XReal p,const XU32 key_index0) const
{
  XReal pbefore;
  XReal pafter;
  if (key_index0>0)
  {
    pbefore = GetKeyValue(key_index0-1);
  }
  else
  {
    pbefore = p;
  }
  if (key_index0<GetNumKeyframes()-1)
  {
    pafter = GetKeyValue(key_index0+1);
  }
  else
  {
    pafter = p;
  }
  return 0.5f*(pafter - pbefore);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::Get1DTCBTangent(const XReal p,const XU32 key_index0) const
{
  XReal tens = GetKeyTension(key_index0);
  XReal bias = GetKeyBias(key_index0);
  XReal cont = GetKeyContinuity(key_index0);
  XReal pbefore;
  XReal pafter;
  if (key_index0>0)
  {
    pbefore = GetKeyValue(key_index0-1);
  }
  else
  {
    pbefore = p;
  }
  if (key_index0<GetNumKeyframes()-1)
  {
    pafter = GetKeyValue(key_index0+1);
  }
  else
  {
    pafter = p;
  }
  return (1.0f-tens)*(1.0f+bias)*(1.0f-cont)*0.5f*(p-pbefore) + 
         (1.0f-tens)*(1.0f-bias)*(1.0f+cont)*0.5f*(pafter-p);
}

//----------------------------------------------------------------------------------------------------------------------
void XFCurve::FindStartAndEndKeys(const XReal t,XU32& key_start,XU32& key_end) const
{
  {
    XReal startTime = GetKeyTime(0);
    if (t<startTime)
    {
      key_end = key_start = 0;
      return;
    }
  }
  for (XU32 i=1;i<GetNumKeyframes();++i)
  {
    XReal st = GetKeyTime(i);
    if (st>t)
    {
      key_end = i;
      key_start = i-1;
      return;
    }
  }
  key_end = key_start = GetNumKeyframes()-1;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateBezier(XReal p0,XReal p1,XReal p2,XReal p3,XReal t)
{
  XReal it  = 1.0f-t;
  XReal it2 = it*it;
  XReal it3 = it*it*it;
  XReal t2 = t*t;
  XReal t3 = t2*t;
  return p0*it3 + p1*3.0f*it2*t + p2*3.0f*it*t2 + p3*t3;    
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateHermite(const XReal p0,const XReal t0,const XReal p1,const XReal t1,const XReal t)
{
  XReal _t2  =   t * t;
  XReal _t3  = _t2 * t;
  XReal _2t3 = _t3 + _t3;
  XReal _2t2 = _t2 + _t2;
  XReal _3t2 = _t2 + _2t2;

  XReal h00 =  _2t3 - _3t2 + 1.0f;
  XReal h10 =   _t3 - _2t2 + t;
  XReal h01 = -_2t3 + _3t2;
  XReal h11 =   _t3 - _t2;

  return h00 * p0 + 
         h10 * t0 + 
         h01 * p1 + 
         h11 * t1;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateStep(XReal time) const
{
  XU32 key_index0=0,key_index1=0;
  FindStartAndEndKeys(time,key_index0,key_index1);
  return GetKeyValue(key_index0);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateLinear(XReal time) const
{
  XU32 key_index0=0,key_index1=0;
  FindStartAndEndKeys(time,key_index0,key_index1);

  const XReal time0  = GetKeyTime(key_index0);
  const XReal value0 = GetKeyValue(key_index0);

  const XReal time1  = GetKeyTime(key_index1);
  const XReal value1 = GetKeyValue(key_index1);

  // determine time range
  const XReal time_range_between_keys = time1 - time0;
  if (time_range_between_keys>0.0f)
  {
    // determine the interpolation value
    const XReal interpolation_t = (time - time0) / time_range_between_keys;

    // return evaluated attribute value
    return value0 + interpolation_t * (value1 - value0);
  }

  return value0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateCatmullRom1D(XReal time) const
{
  XU32 key_index0=0,key_index1=0;
  FindStartAndEndKeys(time,key_index0,key_index1);

  // grab values for start key
  const XReal p0 = GetKeyValue(key_index0);
  const XReal time0 = GetKeyTime(key_index0);

  // grab values for end key
  const XReal p1 = GetKeyValue(key_index1);
  const XReal time1 = GetKeyTime(key_index1);

  // compute input and output tangents
  const XReal t0 = Get1DCatmullTangent(p0, key_index0);
  const XReal t1 = Get1DCatmullTangent(p1, key_index1);

  // determine time range
  const XReal time_range_between_keys = time1 - time0;
  if (time_range_between_keys>0.0f)
  {
    // determine the interpolation value
    XReal interpolation_t = (time - time0) / time_range_between_keys;

    // return evaluated attribute value
    return EvaluateHermite(p0,t0,p1,t1,interpolation_t);
  }
  return p0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateCardinal1D(XReal time) const
{
  XU32 key_index0=0,key_index1=0;
  FindStartAndEndKeys(time,key_index0,key_index1);

  // grab values for start key
  const XReal p0 = GetKeyValue(key_index0);
  const XReal time0 = GetKeyTime(key_index0);

  // grab values for end key
  const XReal p1 = GetKeyValue(key_index1);
  const XReal time1 = GetKeyTime(key_index1);

  // compute input and output tangents
  const XReal t0 = Get1DCardinalTangent(p0, key_index0);
  const XReal t1 = Get1DCardinalTangent(p1, key_index1);

  // determine time range
  const XReal time_range_between_keys = time1 - time0;
  if (time_range_between_keys>0.0f)
  {
    // determine the interpolation value
    XReal interpolation_t = (time - time0) / time_range_between_keys;

    // return evaluated attribute value
    return EvaluateHermite(p0,t0,p1,t1,interpolation_t);
  }
  return p0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateTCB1D(XReal time) const
{
  XU32 key_index0=0,key_index1=0;
  FindStartAndEndKeys(time,key_index0,key_index1);

  // grab values for start key
  const XReal p0 = GetKeyValue(key_index0);
  const XReal time0 = GetKeyTime(key_index0);

  // grab values for end key
  const XReal p1 = GetKeyValue(key_index1);
  const XReal time1 = GetKeyTime(key_index1);

  // compute input and output tangents
  const XReal t0 = Get1DTCBTangent(p0, key_index0);
  const XReal t1 = Get1DTCBTangent(p1, key_index1);

  // determine time range
  const XReal time_range_between_keys = time1 - time0;
  if (time_range_between_keys>0.0f)
  {
    // determine the interpolation value
    XReal interpolation_t = (time - time0) / time_range_between_keys;

    // return evaluated attribute value
    return EvaluateHermite(p0,t0,p1,t1,interpolation_t);
  }
  return p0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateBezier1D(XReal time) const
{
  XU32 key_index0=0,key_index1=0;
  FindStartAndEndKeys(time,key_index0,key_index1);
  if(key_index0==key_index1)
  {
    return GetKeyValue(key_index0);
  }

  XReal time0  = GetKeyTime(key_index0);
  XReal mid0   = GetKeyOutValue(key_index0);
  XReal value0 = GetKeyValue(key_index0);

  XReal time1  = GetKeyTime(key_index1);
  XReal mid1   = GetKeyInValue(key_index1);
  XReal value1 = GetKeyValue(key_index1);

  // determine time range
  const XReal time_range_between_keys = time1 - time0;
  if (time_range_between_keys>0.0f)
  {
    // determine the interpolation value
    XReal interpolation_t = (time - time0) / time_range_between_keys;

    // return evaluated attribute value
    return EvaluateBezier(value0,mid0,mid1,value1,interpolation_t);
  }
  return value0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateHermite1D(XReal time) const
{
  XU32 key_index0=0,key_index1=0;
  FindStartAndEndKeys(time,key_index0,key_index1);

  // grab values for start key
  const XReal p0 = GetKeyValue(key_index0);
  const XReal time0 = GetKeyTime(key_index0);

  // grab values for end key
  const XReal p1 = GetKeyValue(key_index1);
  const XReal time1 = GetKeyTime(key_index1);

  // calculate tangents
  const XReal t0 = GetKeyOutTangent(key_index0);
  const XReal t1 = GetKeyInTangent(key_index1);

  // determine time range
  const XReal time_range_between_keys = time1 - time0;
  if (time_range_between_keys>0.0f)
  {
    // determine the interpolation value
    XReal interpolation_t = (time - time0) / time_range_between_keys;

    // return evaluated attribute value
    return EvaluateHermite(p0,t0,p1,t1,interpolation_t);
  }
  return p0;
}

//----------------------------------------------------------------------------------------------------------------------
inline XReal bezier_calc_a(XReal x0,XReal x1,XReal x2,XReal x3)
{
  return -x0 + 3.0f*x1 - 3.0f*x2 + x3;
}

//----------------------------------------------------------------------------------------------------------------------
inline XReal bezier_calc_b(XReal x0,XReal x1,XReal x2,XReal)
{
  return  3.0f*x0 - 6.0f*x1 + 3.0f*x2;
}

//----------------------------------------------------------------------------------------------------------------------
inline XReal bezier_calc_c(XReal x0,XReal x1,XReal,XReal)
{
  return -3.0f*x0 + 3.0f*x1;
}

//----------------------------------------------------------------------------------------------------------------------
inline XReal bezier_calc_d(XReal x0,XReal,XReal,XReal)
{
  return x0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::InvertBezier(XReal x,XReal x0,XReal x1,XReal x2,XReal x3)
{
  // we are working on the assumption that x0 < x1 < x2 < x3
  XMD_ASSERT( x1>x0 );
  XMD_ASSERT( x2>x1 );
  XMD_ASSERT( x3>x2 );

  // we get accuracy issues when the values for x are small. To remedy this, just mult
  // everything by some large number. This won't affect the final t value other than
  // to make it more accurate. 
  XReal mult=1.0f;
  if ((x3-x0)<1)
  {
    mult = 1000000.0f;
  }
  else 
  if ((x3-x0)<10)
  {
    mult = 100000.0f;
  }
  else 
  if ((x3-x0)<1000)
  {
    mult = 1000.0f;
  }

  x  *= mult;
  x0 *= mult;
  x1 *= mult;
  x2 *= mult;
  x3 *= mult;

  // grab in  ax^3 + bx^2 + cx + d = 0  form
  XReal a = bezier_calc_a(x0,x1,x2,x3);
  XReal b = bezier_calc_b(x0,x1,x2,x3);
  XReal c = bezier_calc_c(x0,x1,x2,x3);
  XReal d = bezier_calc_d(x0,x1,x2,x3) - x; /// \todo why is this one -x?

  // divide through by a to get slightly simpler equation
  // 
  //   x^3 + bx^2 + cx + d = 0  
  //         -      -    -
  //         a      a    a
  //

  XReal temp = 1.0f/( XMabs(a) > 0.0f ? a : 1.0f);
  b *= temp;
  c *= temp;
  d *= temp;

  // now substitute x for  u - b
  //                           -
  //                           3
  //
  // to give the following:
  // 
  //   u^3 + Au + B = 0
  //
  // where
  //

  XReal A = c - b*b/3.0f;
  XReal B = d - b*c/3.0f + 2.0f*b*b*b/27.0f;

  // Now compute A' and B'
  //
  XReal A_dash;
  XReal B_dash;
  {
    XReal temp_1 =-B*0.5f;
    XReal temp_2 = B*B*0.25f;
    XReal temp_3 = A*A*A/27.0f;

    if ( (temp_2+temp_3) < 0 )
    {
      //
      // we have an imaginary solution since 
      //
      //   B^2  + A^3  
      //   ---    --- 
      //    4      27
      //
      // produces a negative number, which introduces i when you do the square root. 
      // Sucks, but we're forced to use an iterative method.
      return InvertBezierLong(x,x0,x1,x2,x3);
    }
    else
    {
      XReal temp_4 = XMsqrt(temp_2+temp_3);

      XReal temp_A  = temp_1 + temp_4;
      XReal temp_B  = temp_1 - temp_4;

      // determine if negative, and modify to a +ve value to stop
      // pow messing up. using these scalars to convert back to negative nums
      XReal scalarA = temp_A<0 ? -1.0f : 1.0f;
      XReal scalarB = temp_B<0 ? -1.0f : 1.0f;

      A_dash = scalarA * XMpow(scalarA*temp_A,1.0f/3.0f);
      B_dash = scalarB * XMpow(scalarB*temp_B,1.0f/3.0f);
    }
  }

  // solution for u is (there are also 2 imaginary solutions. Not sure if i need them?)
  XReal u = A_dash + B_dash;

  // now convert u back into t
  XReal t = u - b/3.0f;

  // and hopefully done... 
  return t;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::InvertBezierLong(XReal x,XReal x0,XReal x1,XReal x2,XReal x3)
{
  // run quick test if on (or past) the end points
  if ( x<(x0+0.00000001f) )
  {
    return 0.0f;
  }
  if ( x>(x3-0.00000001f) )
  {
    return 1.0f;
  }

  // possible t values
  XReal tmin = 0.0f;
  XReal tmax = 1.0f;
  XReal tmid = 0.5f;

  // x value in middle of tmin and tmax
  XReal xmid = 0;

  // number of recursive iterations until a solution is found.
  // in my tests, this tends to be around the 20 mark... 
  int num_iters = 20;
  while (num_iters)
  {
    // calc bezier value in middle of func
    xmid = EvaluateBezier(static_cast<XReal>(x0),
                          static_cast<XReal>(x1),
                          static_cast<XReal>(x2),
                          static_cast<XReal>(x3),
                          static_cast<XReal>(tmid) );

    // if found matching value, give up...
    if (XM2::float_equal(x,xmid))
    {
      break;
    }

    // if x is less than the mid point, move boundary so that
    // it's in the middle of the lower half
    if (x<xmid)
    {
      tmax = tmid;
      tmid = (tmax+tmin)*0.5f;
    }
    else
    {
      tmin = tmid;
      tmid = (tmax+tmin)*0.5f;
    }

    --num_iters;
  }
  return tmid;
}

//----------------------------------------------------------------------------------------------------------------------
inline XReal hermite_calc_a(const XReal x0,const XReal t0,const XReal x1,const XReal t1)
{
  return +2.0f*x0 - 2.0f*x1 + t0 + t1;
}

//----------------------------------------------------------------------------------------------------------------------
inline XReal hermite_calc_b(const XReal x0,const XReal t0,const XReal x1,const XReal t1)
{
  return -3.0f*x0 + 3.0f*x1 - 2.0f*t0 - t1;
}

//----------------------------------------------------------------------------------------------------------------------
inline XReal hermite_calc_c(XReal,XReal,XReal,const XReal t1)
{
  return t1;
}

//----------------------------------------------------------------------------------------------------------------------
inline XReal hermite_calc_d(const XReal x0,XReal,XReal,XReal)
{
  return x0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::InvertHermite(XReal x,XReal x0,XReal t0,XReal x1,XReal t1)
{
  // we are working on the assumption that x0 < x1 < x2 < x3
  //assert( x1>x0 );

  // we get accuracy issues when the values for x are small. To remedy this, just mult
  // everything by some large number. This won't affect the final t value other than
  // to make it more accurate. 
  XReal mult=1.0;
  if ((x1-x0)<1)
  {
    mult = 1000000.0f;
  }
  else 
  if ((x1-x0)<10)
  {
    mult = 100000.0f;
  }
  else 
  if ((x1-x0)<1000)
  {
    mult = 1000.0f;
  }

  x  *= mult;
  x0 *= mult;
  x1 *= mult;
  t0 *= mult;
  t1 *= mult;

  // grab in  ax^3 + bx^2 + cx + d = 0  form
  XReal a = hermite_calc_a(x0,t0,x1,t1);
  XReal b = hermite_calc_b(x0,t0,x1,t1);
  XReal c = hermite_calc_c(x0,t0,x1,t1);
  XReal d = hermite_calc_d(x0,t0,x1,t1) - x;

  // divide through by a to get slightly simpler equation
  // 
  //   x^3 + bx^2 + cx + d = 0  
  //         -      -    -
  //         a      a    a
  //

  XReal temp = 1.0f/( XMabs(a) > 0.0f ? a : 1.0f);
  b *= temp;
  c *= temp;
  d *= temp;

  // now substitute x for  u - b
  //                           -
  //                           3
  //
  // to give the following:
  // 
  //   u^3 + Au + B = 0
  //
  // where
  //

  XReal A = c - b*b/3.0f;
  XReal B = d - b*c/3.0f + 2.0f*b*b*b/27.0f;

  // Now compute A' and B'
  //
  XReal A_dash;
  XReal B_dash;
  {
    XReal temp_1 =-B*0.5f;
    XReal temp_2 = B*B*0.25f;
    XReal temp_3 = A*A*A/27.0f;

    if ( (temp_2+temp_3) < 0 )
    {
      //
      // we have an imaginary solution since 
      //
      //   B^2  + A^3  
      //   ---    --- 
      //    4      27
      //
      // produces a negative number, which introduces i when you sqaure root it. 
      //
      return InvertHermiteLong(x,x0,t0,x1,t1);
    }
    else
    {
      XReal temp_4 = XMsqrt(temp_2+temp_3);

      XReal temp_A  = temp_1 + temp_4;
      XReal temp_B  = temp_1 - temp_4;

      // determine if negative, and modify to a +ve value to stop
      // pow fecking up. using these scalars to convert back to negative nums
      XReal scalarA = temp_A<0 ? -1.0f : 1.0f;
      XReal scalarB = temp_B<0 ? -1.0f : 1.0f;

      A_dash = scalarA * XMpow(scalarA*temp_A,1.0f/3.0f);
      B_dash = scalarB * XMpow(scalarB*temp_B,1.0f/3.0f);
    }
  }

  // solution for u is (there are also 2 imaginary solutions. Not sure if i need them?)
  XReal u = A_dash + B_dash;

  // now convert u back into t
  XReal t = u - b/3.0f;

  // and hopefully done... 
  return t;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::InvertHermiteLong(XReal x,XReal p0,XReal t0,XReal p1,XReal t1)
{
  // run quick test if on (or past) the end points
  if ( x<(p0+0.00000001f) )
  {
    return 0.0f;
  }
  if ( x>(p1-0.00000001f) )
  {
    return 1.0f;
  }

  // possible t values
  XReal tmin = 0.0f;
  XReal tmax = 1.0f;
  XReal tmid = 0.5f;

  // x value in middle of tmin and tmax
  XReal xmid = 0;

  // number of recursive iterations until a solution is found.
  // in my tests, this tends to be around the 20 mark... 
  int num_iters = 20;
  while (num_iters)
  {
    // calc hermite value in middle of func
    xmid = EvaluateHermite( static_cast<XReal>(p0),
                            static_cast<XReal>(t0),
                            static_cast<XReal>(p1),
                            static_cast<XReal>(t1),
                            static_cast<XReal>(tmid) );

    // if found matching value, give up...
    if (XM2::float_equal(x,xmid))
    {
      break;
    }

    // if x is less than the mid point, move boundary so that
    // it's in the middle of the lower half
    if (x<xmid)
    {
      tmax = tmid;
      tmid = (tmax+tmin)*0.5f;
    }
    else
    {
      tmin = tmid;
      tmid = (tmax+tmin)*0.5f;
    }

    --num_iters;
  }
  return tmid;
}

//----------------------------------------------------------------------------------------------------------------------
void XFCurve::Get2DCardinalTangent(const XU32 key_index0,XReal& x,XReal& y) const
{
  XReal pbefore;
  XReal pafter;
  XReal tbefore;
  XReal tafter;
  const XReal t = GetKeyTime(key_index0);
  const XReal p = GetKeyValue(key_index0);
  const XReal c = GetKeyContinuity(key_index0);

  if (key_index0>0)
  {
    pbefore = GetKeyValue(key_index0-1);
    tbefore = GetKeyTime(key_index0-1);
  }
  else
  {
    pbefore = p;
    tbefore = t;
  }
  if (key_index0<GetNumKeyframes()-1)
  {
    pafter = GetKeyValue(key_index0+1);
    tafter = GetKeyTime(key_index0+1);
  }
  else
  {
    pafter = p;
    tafter = t;
  }
  y = 0.5f*(1.0f-c) * (pafter - pbefore);
  x = 0.5f*(1.0f-c) * (tafter - tbefore);
}

//----------------------------------------------------------------------------------------------------------------------
void XFCurve::Get2DCatmullTangent(const XU32 key_index0,XReal& x,XReal& y) const
{
  XReal pbefore;
  XReal pafter;
  XReal tbefore;
  XReal tafter;
  const XReal t = GetKeyTime(key_index0);
  const XReal p = GetKeyValue(key_index0);

  if (key_index0>0)
  {
    pbefore = GetKeyValue(key_index0-1);
    tbefore = GetKeyTime(key_index0-1);
  }
  else
  {
    pbefore = p;
    tbefore = t;
  }
  if (key_index0<GetNumKeyframes()-1)
  {
    pafter = GetKeyValue(key_index0+1);
    tafter = GetKeyTime(key_index0+1);
  }
  else
  {
    pafter = p;
    tafter = t;
  }
  y = 0.5f*(pafter - pbefore);
  x = 0.5f*(tafter - tbefore);
}

//----------------------------------------------------------------------------------------------------------------------
void XFCurve::Get2DTCBInTangent(const XU32 key_index0,XReal& x,XReal& y) const
{
  XReal pbefore;
  XReal pafter;
  XReal tbefore;
  XReal tafter;
  const XReal t = GetKeyTime(key_index0);
  const XReal p = GetKeyValue(key_index0);

  if (key_index0>0)
  {
    pbefore = GetKeyValue(key_index0-1);
    tbefore = GetKeyTime(key_index0-1);
  }
  else
  {
    pbefore = p;
    tbefore = t;
  }
  if (key_index0<GetNumKeyframes()-1)
  {
    pafter = GetKeyValue(key_index0+1);
    tafter = GetKeyTime(key_index0+1);
  }
  else
  {
    pafter = p;
    tafter = t;
  }

  const XReal tens = GetKeyTension(key_index0);
  const XReal cont = GetKeyContinuity(key_index0);
  const XReal bias = GetKeyBias(key_index0);

  x = (1.0f-tens)*(1.0f+bias)*(1.0f-cont)*0.5f*(t-tbefore) + 
      (1.0f-tens)*(1.0f-bias)*(1.0f+cont)*0.5f*(tafter-t);
  y = (1.0f-tens)*(1.0f+bias)*(1.0f-cont)*0.5f*(p-pbefore) + 
      (1.0f-tens)*(1.0f-bias)*(1.0f+cont)*0.5f*(pafter-p);
}

//----------------------------------------------------------------------------------------------------------------------
void XFCurve::Get2DTCBOutTangent(const XU32 key_index0,XReal& x,XReal& y) const
{
  XReal pbefore;
  XReal pafter;
  XReal tbefore;
  XReal tafter;
  const XReal t = GetKeyTime(key_index0);
  const XReal p = GetKeyValue(key_index0);

  if (key_index0>0)
  {
    pbefore = GetKeyValue(key_index0-1);
    tbefore = GetKeyTime(key_index0-1);
  }
  else
  {
    pbefore = p;
    tbefore = t;
  }
  if (key_index0<GetNumKeyframes()-1)
  {
    pafter = GetKeyValue(key_index0+1);
    tafter = GetKeyTime(key_index0+1);
  }
  else
  {
    pafter = p;
    tafter = t;
  }

  const XReal tens = GetKeyTension(key_index0);
  const XReal cont = GetKeyContinuity(key_index0);
  const XReal bias = GetKeyBias(key_index0);
  
  x = (1.0f-tens)*(1.0f+bias)*(1.0f+cont)*0.5f*(t-tbefore) + 
      (1.0f-tens)*(1.0f-bias)*(1.0f-cont)*0.5f*(tafter-t);
  y = (1.0f-tens)*(1.0f+bias)*(1.0f+cont)*0.5f*(p-pbefore) + 
      (1.0f-tens)*(1.0f-bias)*(1.0f-cont)*0.5f*(pafter-p);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateCatmullRom2D(XReal time) const
{
  XU32 start_frame,end_frame;
  FindStartAndEndKeys(time,start_frame,end_frame);

  // grab values for start key
  const XReal p0 = GetKeyValue(start_frame);
  const XReal time0 = GetKeyTime(start_frame);

  // grab values for end key
  const XReal p1 = GetKeyValue(end_frame);
  const XReal time1 = GetKeyTime(end_frame);

  // evaluate tangents
  XReal t0x,t0y,t1x,t1y;
  Get2DCatmullTangent(start_frame,t0x,t0y);
  Get2DCatmullTangent(end_frame,t1x,t1y);

  // first solve Hermite for t
  const XReal interpolating_t = InvertHermiteLong(time,time0,t0x,time1,t1x);

  // now use to calculate y
  return EvaluateHermite(p0,t0y,p1,t1y,interpolating_t);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateCardinal2D(XReal time) const
{
  XU32 start_frame,end_frame;
  FindStartAndEndKeys(time,start_frame,end_frame);

  // grab values for start key
  const XReal p0 = GetKeyValue(start_frame);
  const XReal time0 = GetKeyTime(start_frame);

  // grab values for end key
  const XReal p1 = GetKeyValue(end_frame);
  const XReal time1 = GetKeyTime(end_frame);

  // evaluate tangents
  XReal t0x,t0y,t1x,t1y;
  Get2DCardinalTangent(start_frame,t0x,t0y);
  Get2DCardinalTangent(end_frame,t1x,t1y);

  // first solve Hermite for t
  const XReal interpolating_t = InvertHermiteLong(time,time0,t0x,time1,t1x);

  // now use to calculate y
  return EvaluateHermite(p0,t0y,p1,t1y,interpolating_t);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateTCB2D(XReal time) const
{
  XU32 start_frame,end_frame;
  FindStartAndEndKeys(time,start_frame,end_frame);

  // grab values for start key
  const XReal p0 = GetKeyValue(start_frame);
  const XReal time0 = GetKeyTime(start_frame);

  // grab values for end key
  const XReal p1 = GetKeyValue(end_frame);
  const XReal time1 = GetKeyTime(end_frame);

  // evaluate tangents
  XReal t0x,t0y,t1x,t1y;
  Get2DTCBOutTangent(start_frame,t0x,t0y);
  Get2DTCBInTangent(end_frame,t1x,t1y);

  // first solve Hermite for t
  const XReal interpolating_t = InvertHermiteLong(time,time0,t0x,time1,t1x);

  // now use to calculate y
  return EvaluateHermite(p0,t0y,p1,t1y,interpolating_t);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateBezier2D(XReal time) const
{
  XU32 start_frame,end_frame;
  FindStartAndEndKeys(time,start_frame,end_frame);

  // grab values for start key
  const XReal p0  = GetKeyValue(start_frame);
  const XReal p1x = GetKeyOutValueX(start_frame);
  const XReal p1y = GetKeyOutValueY(start_frame);
  const XReal time0 = GetKeyTime(start_frame);

  if (time<=time0)
  {
    return p0;
  }

  // grab values for end key
  const XReal p3  = GetKeyValue(end_frame);
  const XReal p2x = GetKeyInValueX(end_frame);
  const XReal p2y = GetKeyInValueY(end_frame);
  const XReal time1 = GetKeyTime(end_frame);

  if (time>=time1)
  {
    return p3;
  }

  // first solve Bezier for t
  const XReal interpolating_t = (XReal)InvertBezier(time,time0,p1x,p2x,time1);

  // now use to calculate y
  return EvaluateBezier(p0,p1y,p2y,p3,interpolating_t);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateHermite2D(XReal time) const
{
  XU32 start_frame,end_frame;
  FindStartAndEndKeys(time,start_frame,end_frame);

  // grab values for start key
  const XReal p0 = GetKeyValue(start_frame);
  const XReal time0 = GetKeyTime(start_frame);

  // grab values for end key
  const XReal p1 = GetKeyValue(end_frame);
  const XReal time1 = GetKeyTime(end_frame);

  // evaluate tangents
  const XReal t0x = GetKeyOutTangentX(start_frame);
  const XReal t0y = GetKeyOutTangentY(start_frame);
  const XReal t1x = GetKeyInTangentX(end_frame);
  const XReal t1y = GetKeyInTangentY(end_frame);

  // first solve Hermite for t
  const XReal interpolating_t = InvertHermiteLong(time,time0,t0x,time1,t1x);

  // now use to calculate y
  return EvaluateHermite(p0,t0y,p1,t1y,interpolating_t);
}

//----------------------------------------------------------------------------------------------------------------------
XReal EvaluateStep(const XReal x,const XReal a,const XReal b) 
{
  if(x<a) return 0.0f;
  if(x>b) return 1.0f;
  const XReal nx = (x - a) / (b - a); 
  return nx*nx*(3.0f-2.0f*nx);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::EvaluateSmoothStep(XReal time) const
{
  XU32 start_frame,end_frame;
  FindStartAndEndKeys(time,start_frame,end_frame);
  const XReal time0 = GetKeyTime(start_frame);
  const XReal thresh0 = GetKeyThreshold(start_frame);

  if (time-thresh0>time0 || start_frame==0)
  {
    if (start_frame>=GetNumKeyframes()-2)
    {
      return GetKeyValue(GetNumKeyframes()-2);
    }
    if((end_frame+1)>=GetNumKeyframes())
    {
      return GetKeyValue(GetNumKeyframes()-1);
    }
    const XReal p0 = GetKeyValue(start_frame);
    const XReal p1 = GetKeyValue(end_frame);

    // grab values for key before current
    const XReal time1 = GetKeyTime(end_frame);
    const XReal time2 = GetKeyTime(end_frame+1);

    XReal a = time1-thresh0-time0;
    XReal b = time1+thresh0-time0;
    XReal r = time2-time0;
    a /= r;
    b /= r;

    XReal step = XMD::EvaluateStep( (time-time0)/r,a,b);
    return (1-step) * p0 + step*p1;
  }

  // return last key if run out of frames.
  if (start_frame>=GetNumKeyframes()-1)
  {
    return GetKeyValue(GetNumKeyframes()-2);;
  }

  // grab key values
  const XReal p0 = GetKeyValue(start_frame);
  const XReal p1 = GetKeyValue(end_frame);
  const XReal thresh1 = GetKeyThreshold(end_frame);

  // grab values for key before current
  const XReal time1 = GetKeyTime(start_frame-1);
  const XReal time2 = GetKeyTime(end_frame);

  XReal a = time0-thresh1-time1;
  XReal b = time0+thresh1-time1;
  XReal r = time2-time1;
  a /= r;
  b /= r;

  XReal step = XMD::EvaluateStep( (time-time1)/r,a,b);
  return (1-step) * p0 + step*p1;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetPreLinearInc() const 
{  
  XReal time = GetKeyTime(0);

  /// \todo this should probably defer this to the curves themselves, since some of them 
  ///       already know about their tangents... 
  XReal v0,v01;
  EvaluateCurveValue(v0,time);
  EvaluateCurveValue(v01,time+0.01f);
  XReal dif = -(v01 - v0);
  return dif * 100.0f;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XFCurve::GetPostLinearInc() const 
{ 
  XReal time = GetKeyTime(GetNumKeyframes()-1);

  /// \todo this should probably defer this to the curves themselves, since some of them 
  ///       already know about their tangents... 
  XReal v0,v01;
  EvaluateCurveValue(v0,time);
  EvaluateCurveValue(v01,time-0.01f);
  XReal dif = v0 - v01;
  return dif * 100.0f;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::Evaluate(XReal time,XReal& result) const
{
  XReal time_value = time;
  XReal curve_start_time = GetKeyTime(0);
  XReal curve_end_time = GetKeyTime(GetNumKeyframes()-1);

  if (time_value<curve_start_time)
  {
    XReal curve_start_val = GetKeyValue(0);
    XReal curve_end_val   = GetKeyValue(GetNumKeyframes()-1);

    XReal time_range = curve_end_time - curve_start_time;
    XReal val_range  = curve_end_val  - curve_start_val;

    // 
    switch(GetPreInfinity())
    {
    // just return first key value
    case kInfinityConstant:
      result = curve_start_val;
      return true;
    
    // step doesn't have a gradient, so will be the same as constant
    case kInfinityLinear: 
      result = curve_start_val - GetPreLinearInc()*(time-curve_start_time);
      return true;

    // cycling, just offset the time value
    case kInfinityCycle:
      while(time_value<curve_start_time)
      {
        time_value += time_range;
      }
      break;

    // cycling, offset the time and the value 
    case kInfinityCycleWithOffset:
      while(time_value<curve_start_time)
      {
        time_value += time_range;
        result -= val_range;
      }
      break;

    case kInfinityOscillate:
      {
        bool bswitch=false;
        while(time_value<curve_start_time)
        {
          time_value += time_range;
          bswitch = !bswitch;
        }

        // if on inverted cycle, modify the time value a bit.
        if (bswitch)
        {
          time_value = time_range - time_value + 2.0f*curve_start_time;
        }
      }
      break;

    default:
      XMD_ASSERT(0&&"Unknown pre-infinity curve type");
      return false;
    }
  }
  else
  if (time_value>curve_end_time)
  {
    XReal curve_start_val = GetKeyValue(0);
    XReal curve_end_val   = GetKeyValue(GetNumKeyframes()-1);

    XReal time_range = curve_end_time - curve_start_time;
    XReal val_range  = curve_end_val  - curve_start_val;

    // 
    switch(GetPostInfinity())
    {
    // just return first key value
    case kInfinityConstant:
      result = curve_end_val;
      return true;
    
    // step doesn't have a gradient, so will be the same as constant
    case kInfinityLinear: 
      result = curve_end_val + GetPostLinearInc()*(time-curve_end_time);
      return true;

    // cycling, just offset the time value
    case kInfinityCycle:
      while(time_value>curve_end_time)
      {
        time_value -= time_range;
      }
      break;

    // cycling, offset the time and the value 
    case kInfinityCycleWithOffset:
      while(time_value>curve_end_time)
      {
        time_value -= time_range;
        result += val_range;
      }
      break;

    case kInfinityOscillate:
      {
        bool bswitch=false;
        while(time_value>curve_end_time)
        {
          time_value -= time_range;
          bswitch = !bswitch;
        }

        // if on inverted cycle, modify the time value a bit.
        if (bswitch)
        {
          time_value = time_range - time_value + 2.0f*curve_start_time;
        }
      }
      break;

    default:
      XMD_ASSERT(0&&"Unknown post-infinity curve type");
      return false;
    }
  }
  return EvaluateCurveValue(result, time_value);
}

//----------------------------------------------------------------------------------------------------------------------
bool XFCurve::EvaluateCurveValue(XReal &result,const XReal time) const
{
  switch (GetInterpolationType())
  {
  case kStep:
    result += EvaluateStep(time);
    break;
  case kSmoothStep:
    result += EvaluateSmoothStep(time);
    break;
  case kLinear:
    result += EvaluateLinear(time);
    break;
  case kCatmullRom1D:
    result += EvaluateCatmullRom1D(time);
    break;
  case kCatmullRom2D:
    result += EvaluateCatmullRom2D(time);
    break;
  case kCardinal1D:
    result += EvaluateCardinal1D(time);
    break;
  case kCardinal2D:
    result += EvaluateCardinal2D(time);
    break;
  case kTCB1D:
    result += EvaluateTCB1D(time);
    break;
  case kTCB2D:
    result += EvaluateTCB2D(time);
    break;
  case kBezier1D:
    result += EvaluateBezier1D(time);
    break;
  case kBezier2D:
    result += EvaluateBezier2D(time);
    break;
  case kHermite1D:
    result += EvaluateHermite1D(time);
    break;
  case kHermite2D:
    result += EvaluateHermite2D(time);
    break;
  default:
    // custom implementation - cannot evaluate
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XFCurveStream::Evaluate(const XReal time,XReal* output) const
{
  switch (GetStreamDataType())
  {
  case kReal:
    {
      const XFCurve* curve = GetCurve();
      if (curve)
      {
        curve->Evaluate(time,*output);
      }
    }
    break;
  case kVector2:
    {
      const XFCurve* xcurve = GetCurve(0);
      if (xcurve)
      {
        xcurve->Evaluate(time,output[0]);
      }
      const XFCurve* ycurve = GetCurve(1);
      if (ycurve)
      {
        ycurve->Evaluate(time,output[1]);
      }
    }
    break;
  case kVector3:
    {
      const XFCurve* xcurve = GetCurve(0);
      if (xcurve)
      {
        xcurve->Evaluate(time,output[0]);
      }
      const XFCurve* ycurve = GetCurve(1);
      if (ycurve)
      {
        ycurve->Evaluate(time,output[1]);
      }
      const XFCurve* zcurve = GetCurve(2);
      if (zcurve)
      {
        zcurve->Evaluate(time,output[2]);
      }
    }
    break;
  case kQuaternion:
    {
      if(GetAttribute())
      {
        if(GetAttribute()->GetNode())
        {
          const XMD::XBone* bone = GetAttribute()->GetNode()->HasFn<XMD::XBone>();

          XM2::XEulerAngles angs;            
          if(bone && GetAttribute()->GetPropertyID() == XBone::kRotation)
          {
            switch(bone->GetRotationOrder())
            {
            case XBone::kXYZ:
              angs.SetOrder(XM2::XROT_XYZ);
              break;
            case XBone::kXZY:
              angs.SetOrder(XM2::XROT_XZY);
              break;
            case XBone::kYXZ:
              angs.SetOrder(XM2::XROT_YXZ);
              break;
            case XBone::kYZX:
              angs.SetOrder(XM2::XROT_YZX);
              break;
            case XBone::kZXY:
              angs.SetOrder(XM2::XROT_ZXY);
              break;
            case XBone::kZYX:
              angs.SetOrder(XM2::XROT_ZYX);
              break;
            }
          }

          // evaluate FCurves
          const XFCurve* xcurve = GetCurve(0);
          if (xcurve)
          {
            XReal x;
            xcurve->Evaluate(time,x);
            angs.SetX(x);
          }
          const XFCurve* ycurve = GetCurve(1);
          if (ycurve)
          {
            XReal y;
            ycurve->Evaluate(time,y);
            angs.SetY(y);
          }
          const XFCurve* zcurve = GetCurve(2);
          if (zcurve)
          {
            XReal z;
            zcurve->Evaluate(time,z);
            angs.SetZ(z);
          }

          // convert to quat
          XQuaternion q = angs.AsQuaternion();
          output[0] = q.x;
          output[1] = q.y;
          output[2] = q.z;
          output[3] = q.w;
        }
      }
    }
    break;
  case kVector4:
    {
      const XFCurve* xcurve = GetCurve(0);
      if (xcurve)
      {
        xcurve->Evaluate(time,output[0]);
      }
      const XFCurve* ycurve = GetCurve(1);
      if (ycurve)
      {
        ycurve->Evaluate(time,output[1]);
      }
      const XFCurve* zcurve = GetCurve(2);
      if (zcurve)
      {
        zcurve->Evaluate(time,output[2]);
      }
      const XFCurve* wcurve = GetCurve(3);
      if (wcurve)
      {
        wcurve->Evaluate(time,output[3]);
      }
    }
    break;
  case kMatrix:
    {
      // matrices don't get dumped out as F-Curves.
    }
    break;
  case kRealArray:
    {
      if(GetAttribute())
      {
        if(GetAttribute()->GetNode())
        {
          XRealArray data;
          GetAttribute()->GetNode()->GetProperty(GetAttribute()->GetPropertyID(),data);
          if(data.size())
          {
            memcpy(output,&data[0],sizeof(XReal)*data.size());
            for (XU32 i=0;i<(XU32)data.size();++i)
            {
              const XFCurve* curve = GetCurve(0,i);
              if(curve)
                curve->Evaluate(time,output[i]);
            }
          }
        }
      }
    }
    break;
  }
}
}
