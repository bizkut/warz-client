//----------------------------------------------------------------------------------------------------------------------
/// \file VolumeLight.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/VolumeLight.h"
#include "XMD/FileIO.h"
#include "XMD/Logger.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
bool XRampAttribute::WriteChunk(std::ostream& ofs)
{
  ofs << "{\n";
  ofs << "\t\ttype " << (m_IsColour?"colour":"curve") << "\n";
  ofs << "\t\tcount " << m_NumEntries << "\n";
  for(XU32 i=0;i!=m_NumEntries;++i)
  {
    ofs << "\t\t" << i << " " << m_Positions[i] << " ";

    if(m_IsColour) 
    {
      ofs  << m_ColourEntries[i].r << " "
        << m_ColourEntries[i].g << " "
        << m_ColourEntries[i].b << " "
        << m_ColourEntries[i].a << " ";
    }
    else
    {
      ofs  << m_CurveEntries[i] << " ";
    }
    switch(m_Interps[i])
    {
    case 1:
      ofs << "linear\n";
      break;
    case 2:
      ofs << "smooth\n";
      break;
    case 3:
      ofs << "spline\n";
      break;
    default:
      ofs << "none\n";
      break;
    }
  }
  ofs << "\t}\n";
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampAttribute::ReadChunk(std::istream& ifs)
{
  READ_CHECK("{",ifs);
  READ_CHECK("type",ifs);
  XString temp_;
  ifs >> temp_;
  if( temp_[0] == 'c' &&
    temp_[1] == 'u' &&
    temp_[2] == 'r')
    m_IsColour = false;
  else
    m_IsColour = true;

  READ_CHECK("count",ifs);
  XS32 num;
  ifs >> num;

  if(!m_IsColour)
  {
    for(XS32 i=0;i!=num;++i) 
    {
      XReal pos,val;
      XS32 idx,interp;
      XString buffer;
      ifs >> idx >> pos >> val >> buffer;
      if(buffer=="none")
        interp = 0;
      else
      if(buffer=="linear")
        interp = 1;
      else
      if(buffer=="smooth")
        interp = 2;
      else
      if(buffer=="spline")
        interp = 3;
      else 
      {
        XGlobal::GetLogger()->Logf(XBasicLogger::kError, "expected \"none\", \"linear\", \"smooth\" or \"spline\"\tgot: \"%s\"", buffer.c_str());
        return false;
      }

      AddEntry(pos,val,interp);
    }
  }
  else
  {
    for(XS32 i=0;i!=num;++i) 
    {
      XReal pos;
      XColour val;
      XS32 idx,interp;
      XString buffer;
      ifs >> idx >> pos >> val.r >> val.g >> val.b >> val.a >> buffer;
      if(buffer=="none")
        interp = 0;
      else
      if(buffer=="linear")
        interp = 1;
      else
      if(buffer=="smooth")
        interp = 2;
      else
      if(buffer=="spline")
        interp = 3;
      else
      {
        XGlobal::GetLogger()->Logf(XBasicLogger::kError,
          "expected \"none\", \"linear\", \"smooth\" or \"spline\" got: \"%s\"",
          buffer.c_str());
          return false;
      }

      AddEntry(pos,val,interp);
    }
  }


  READ_CHECK("}",ifs);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const XRampAttribute& XRampAttribute::operator = (const XRampAttribute& rhs)
{
  if(m_IsColour)
    delete [] m_ColourEntries;
  else
    delete [] m_CurveEntries;

  m_IsColour = rhs.m_IsColour;
  m_NumEntries = rhs.m_NumEntries;

  delete [] m_Interps;
  delete [] m_Positions;


  if(m_IsColour)
    m_ColourEntries = new XColour[m_NumEntries];
  else
    m_CurveEntries = new XReal[m_NumEntries];

  m_Interps = new XS32[m_NumEntries];
  m_Positions = new XReal[m_NumEntries];

  // copy data
  if (m_NumEntries)
  {
    if(m_IsColour)
      memcpy(m_ColourEntries,rhs.m_ColourEntries,sizeof(XColour)*m_NumEntries);
    else
      memcpy(m_CurveEntries,rhs.m_CurveEntries,sizeof(XReal)*m_NumEntries);

    memcpy(m_Interps,rhs.m_Interps,sizeof(XS32)*m_NumEntries);
    memcpy(m_Positions,rhs.m_Positions,sizeof(XReal)*m_NumEntries);
  }

  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampAttribute::DoData(XFileIO& io) 
{

  IO_CHECK( io.DoData(&m_IsColour) );
  IO_CHECK( io.DoData(&m_NumEntries) );

  // if reading, allocate the correct data sizes
  if(io.IsReading())
  {
    if(m_IsColour)
      m_ColourEntries = new XColour[m_NumEntries];
    else
      m_CurveEntries = new XReal[m_NumEntries];

    m_Interps   = new XS32[m_NumEntries];
    m_Positions = new XReal[m_NumEntries];
  }

  if(m_IsColour)
  {
    IO_CHECK( io.DoData(m_ColourEntries,m_NumEntries) );
  }
  else
  {
    IO_CHECK( io.DoData(m_CurveEntries,m_NumEntries) );
  }

  IO_CHECK( io.DoData(m_Interps,m_NumEntries) );
  IO_CHECK( io.DoData(m_Positions,m_NumEntries) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XRampAttribute::GetDataSize() const 
{

  XU32 sz=sizeof(bool) + sizeof(XU32);
  if(m_IsColour)
    sz += (sizeof(XReal)*5 + sizeof(XS32))*m_NumEntries;
  else
    sz += (sizeof(XReal)*2 + sizeof(XS32))*m_NumEntries;

  return sz;
}

//----------------------------------------------------------------------------------------------------------------------
XRampAttribute::XRampAttribute(bool colour) 
{
  m_IsColour=colour;
  m_NumEntries=0;
  m_ColourEntries=0;
  m_Positions=0;
  m_Interps=0;
}

//----------------------------------------------------------------------------------------------------------------------
XRampAttribute::XRampAttribute(const XReal start,const XReal end,const XU32 num) 
{
  m_IsColour=false;
  m_NumEntries=num;
  m_ColourEntries=0;
  m_Interps=0;
  m_Positions=0;
  if(num==1) 
  {
    m_CurveEntries = new XReal[1];
    m_CurveEntries[0] = start;
      m_Interps[0] = 0;
      m_Positions[0] = 0;
  }
  else
  if(num==0) 
  {
  }
  else
  {
    XReal tot = end - start;
    XReal f = start;
    XReal inc = tot/(num-1);
    XReal incv = 1.0f/(num-1);
    m_CurveEntries = new XReal[num];
    for(XU32 i=0;i!=num;++i,f+=inc)
    {
      m_CurveEntries[i] = f;
      m_Interps[i] = 1;
      m_Positions[i] = incv*i;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XRampAttribute::XRampAttribute(const XColour& start,const XColour& end,const XU32 num) 
{
  m_IsColour=true;
  m_NumEntries=num;
  m_ColourEntries=0;
  m_Interps=0;
  m_Positions=0;
  if(num==1)
  {
    m_ColourEntries = new XColour[1];
    m_ColourEntries[0] = start;
  }
  else
  if(num==0)
  {
  }
  else
  {
    XColour tot = end - start;
    XColour f = start;
    XColour inc = tot/static_cast<XReal>(num-1);
    XReal incv = 1.0f/(num-1);
    m_ColourEntries = new XColour[num];
    for(XU32 i=0;i!=num;++i,f+=inc)
    {
      m_ColourEntries[i] = f;
      m_Interps[i] = 1;
      m_Positions[i] = incv*i;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XRampAttribute::~XRampAttribute()
{
  if(IsColour())
  {
    delete [] m_ColourEntries;
  }
  else
  {
    delete [] m_CurveEntries;
  }
  delete [] m_Interps;
  delete [] m_Positions;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampAttribute::IsColour() const 
{
  return m_IsColour;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampAttribute::IsCurve() const 
{
  return !m_IsColour;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XRampAttribute::GetNumEntries() const 
{
  return m_NumEntries;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampAttribute::GetEntry(const XU32 index,XReal& pos,XReal& val,XS32& interp) const 
{
  if(index<m_NumEntries)
  {
    pos = m_Positions[index];
    val = m_CurveEntries[index];
    interp = m_Interps[index];
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampAttribute::GetEntry(const XU32 index,XReal& pos,XColour& val,XS32& interp) const 
{
  if(index<m_NumEntries) 
  {
    pos = m_Positions[index];
    val = m_ColourEntries[index];
    interp = m_Interps[index];
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampAttribute::AddEntry(const XReal pos,const XColour& val,const XS32 interp) 
{
  if (!IsColour())
  {
    return false;
  }

  XReal* pNewPositions     = new XReal[m_NumEntries+1];
  XS32*   pNewInterps       = new XS32[m_NumEntries+1];
  XColour* pNewColourEntries = new XColour[m_NumEntries+1];

  bool ignore=false;
  XS32 j=0;
  for(XU32 i=0;i!=m_NumEntries;++j) 
  {
    if(!ignore)
    {
      if(m_Positions[i] > pos)
      {
        pNewPositions[j] = pos;
        pNewColourEntries[j] = val;
        pNewInterps[j] = interp;
        ignore=true;
      }
      else 
      {
        pNewPositions[j]     = m_Positions[i];
        pNewColourEntries[j] = m_ColourEntries[i];
        pNewInterps[j]       = m_Interps[i];
        ++i;
      }
    }
    else 
    {
      pNewPositions[j]     = m_Positions[i];
      pNewColourEntries[j] = m_ColourEntries[i];
      pNewInterps[j]       = m_Interps[i];
      ++i;
    }
  }

  if(!ignore)
  {
    pNewPositions[m_NumEntries] = pos;
    pNewColourEntries[m_NumEntries] = val;
    pNewInterps[m_NumEntries] = interp;
  }

  delete [] m_Positions;
  delete [] m_ColourEntries;
  delete [] m_Interps;

  m_Positions = pNewPositions;
  m_ColourEntries = pNewColourEntries;
  m_Interps = pNewInterps;

  ++m_NumEntries;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XRampAttribute::AddEntry(const XReal pos,const XReal val,const XS32 interp)
{

  if (IsColour())
  {
    return false;
  }

  XReal* pNewPositions     = new XReal[m_NumEntries+1];
  XS32*   pNewInterps      = new XS32  [m_NumEntries+1];
  XReal* pNewCurveEntries  = new XReal[m_NumEntries+1];

  bool ignore=false;
  XS32 j=0;
  for(XU32 i=0;i!=m_NumEntries;++j) 
  {
    if(!ignore)
    {
      if(m_Positions[i] > pos)
      {
        pNewPositions[j] = pos;
        pNewCurveEntries[j] = val;
        pNewInterps[j] = interp;
        ignore=true;
      }
      else 
      {
        pNewPositions[j]     = m_Positions[i];
        pNewCurveEntries[j] = m_CurveEntries[i];
        pNewInterps[j]       = m_Interps[i];
        ++i;
      }
    }
    else
    {
      pNewPositions[j]     = m_Positions[i];
      pNewCurveEntries[j] = m_CurveEntries[i];
      pNewInterps[j]       = m_Interps[i];
      ++i;
    }
  }

  if(!ignore) 
  {
    pNewPositions[m_NumEntries] = pos;
    pNewCurveEntries[m_NumEntries] = val;
    pNewInterps[m_NumEntries] = interp;
  }

  delete [] m_Positions;
  delete [] m_CurveEntries;
  delete [] m_Interps;

  m_Positions = pNewPositions;
  m_CurveEntries = pNewCurveEntries;
  m_Interps = pNewInterps;

  ++m_NumEntries;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XVolumeLight::XVolumeLight(XModel* pmod) 
  : XExtendedLight(pmod),m_ColorRamp(true),m_PenumbraRamp(false)
{
  m_Arc = 360;
  m_EndRadius = 0;
  XColour black(0.0f,0.0f,0.0f,1.0f);
  XColour white(1.0f,1.0f,1.0f,1.0f);
  m_ColorRamp.AddEntry(0.0f,black,0);
  m_ColorRamp.AddEntry(1.0f,white,0);
  m_PenumbraRamp.AddEntry(0.0f,0.0f,0);
  m_PenumbraRamp.AddEntry(1.0f,1.0f,0);
}

//----------------------------------------------------------------------------------------------------------------------
XVolumeLight::~XVolumeLight() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XVolumeLight::GetApiType() const
{
  return XFn::VolumeLight;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XVolumeLight::GetFn(XFn::Type type)
{
  if(XFn::VolumeLight==type)
    return (XVolumeLight*)this;
  return XExtendedLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XVolumeLight::GetFn(XFn::Type type) const
{
  if(XFn::VolumeLight==type)
    return (const XVolumeLight*)this;
  return XExtendedLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeLight::DoCopy(const XBase* rhs)
{
  const XVolumeLight* cb = rhs->HasFn<XVolumeLight>();
  XMD_ASSERT(cb);

  m_Arc = cb->m_Arc;
  m_EndRadius = cb->m_EndRadius;
  m_ColorRamp = cb->m_ColorRamp;
  m_PenumbraRamp = cb->m_PenumbraRamp;

  XExtendedLight::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeLight::WriteChunk(std::ostream& ofs)
{
  if( !XExtendedLight::WriteChunk(ofs) )
    return false;

  ofs << "\tarc\t" << m_Arc << "\n";
  ofs << "\tcone_end_radius\t" << m_EndRadius << "\n";

  ofs << "\tcolor_ramp ";
  if(!m_ColorRamp.WriteChunk(ofs))
    return false;

  ofs << "\tpenumbra_ramp ";
  if(!m_PenumbraRamp.WriteChunk(ofs))
    return false;

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeLight::DoData(XFileIO& io)
{
  DUMPER(XVolumeLight);

  IO_CHECK( XExtendedLight::DoData(io) );

  IO_CHECK( io.DoData(&m_Arc) );
  DPARAM( m_Arc );

  IO_CHECK( io.DoData(&m_EndRadius) );
  DPARAM( m_EndRadius );

  IO_CHECK( m_ColorRamp.DoData(io) );
  IO_CHECK( m_PenumbraRamp.DoData(io) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XVolumeLight::GetDataSize() const 
{
  return sizeof(XReal)*2 +
    m_ColorRamp.GetDataSize() +
    m_PenumbraRamp.GetDataSize() +
    XExtendedLight::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeLight::ReadChunk(std::istream& ifs)
{
  if( !XExtendedLight::ReadChunk(ifs) )
    return false;

  READ_CHECK("arc",ifs);
  ifs >> m_Arc;

  READ_CHECK("cone_end_radius",ifs);
  ifs >> m_EndRadius;

  READ_CHECK("color_ramp",ifs);
  if(!m_ColorRamp.ReadChunk(ifs))
    return false;

  READ_CHECK("penumbra_ramp",ifs);
  if(!m_PenumbraRamp.ReadChunk(ifs))
    return false;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeLight::NodeDeath(XId id)
{
  return XExtendedLight::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeLight::PreDelete(XIdSet& extra_nodes)
{
  XExtendedLight::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeLight::GetArc() const
{
  return m_Arc;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeLight::GetConeEndRadius() const
{
  return m_EndRadius;
}

//----------------------------------------------------------------------------------------------------------------------
const XRampAttribute& XVolumeLight::ColourRamp() const
{
  return m_ColorRamp;
}

//----------------------------------------------------------------------------------------------------------------------
const XRampAttribute& XVolumeLight::PenumbraRamp() const
{
  return m_PenumbraRamp;
}

//----------------------------------------------------------------------------------------------------------------------
XRampAttribute& XVolumeLight::ColourRamp()
{
  return m_ColorRamp;
}

//----------------------------------------------------------------------------------------------------------------------
XRampAttribute& XVolumeLight::PenumbraRamp()
{
  return m_PenumbraRamp;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeLight::SetArc(const XReal value) 
{
  m_Arc = value;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeLight::SetConeEndRadius(const XReal value) 
{
  m_EndRadius = value;
}
}
