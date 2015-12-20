//----------------------------------------------------------------------------------------------------------------------
/// \file VolumeAxis.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/VolumeAxis.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XVolumeAxis::XVolumeAxis(XModel* pmod) 
  : XField(pmod),
    m_Direction(1.0f,0.0f,0.0f),
    m_SpeedAlongAxis(0),
    m_SpeedAroundAxis(0),
    m_SpeedAwayFromAxis(0),
    m_SpeedAwayFromCenter(0),
    m_TurbulenceFrequency(),
    m_TurbulenceOffset(),
    m_Turbulence(0),
    m_TurbulenceSpeed(0),
    m_DetailTurbulence(0),
    m_InvertAttenuation(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
XVolumeAxis::~XVolumeAxis()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XVolumeAxis::GetApiType() const
{
  return XFn::VolumeAxis;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XVolumeAxis::GetDataSize() const 
{
  return XField::GetDataSize() + 3*3*sizeof(XReal)+ 8*sizeof(XReal) + sizeof(bool);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XVolumeAxis::GetFn(XFn::Type type)
{
  if(XFn::VolumeAxis==type)
    return (XVolumeAxis*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XVolumeAxis::GetFn(XFn::Type type) const
{
  if(XFn::VolumeAxis==type)
    return (const XVolumeAxis*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XVolumeAxis::GetDirection() const 
{
  return m_Direction;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeAxis::GetDirectionalSpeed() const 
{
  return m_DirectionalSpeed;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeAxis::GetInvertAttenuation() const 
{
  return m_InvertAttenuation;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeAxis::GetSpeedAlongAxis() const
{
  return m_SpeedAlongAxis;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeAxis::GetSpeedAroundAxis() const
{
  return m_SpeedAroundAxis;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeAxis::GetSpeedAwayFromAxis() const
{
  return m_SpeedAwayFromAxis;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeAxis::GetSpeedAwayFromCenter() const 
{
  return m_SpeedAwayFromCenter;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XVolumeAxis::GetTurbulenceFrequency() const 
{
  return m_TurbulenceFrequency;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XVolumeAxis::GetTurbulenceOffset() const
{
  return m_TurbulenceOffset;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeAxis::GetTurbulence() const
{
  return m_Turbulence;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeAxis::GetTurbulenceSpeed() const 
{
  return m_TurbulenceSpeed;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeAxis::GetDetailTurbulence() const 
{
  return m_DetailTurbulence;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::SetDirection(const XVector3& v) 
{
  m_Direction = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::SetDirectionalSpeed(const XReal v)
{
  m_DirectionalSpeed = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::SetInvertAttenuation(const bool v) 
{
  m_InvertAttenuation = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::SetSpeedAlongAxis(const XReal v) 
{
  m_SpeedAlongAxis = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::SetSpeedAroundAxis(const XReal v) 
{
  m_SpeedAroundAxis = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::SetSpeedAwayFromAxis(const XReal v)
{
  m_SpeedAwayFromAxis = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::SetSpeedAwayFromCenter(const XReal v)
{
  m_SpeedAwayFromCenter = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::SetTurbulenceFrequency(const XVector3& v) 
{
  m_TurbulenceFrequency = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::SetTurbulenceOffset(const XVector3& v)
{
  m_TurbulenceOffset = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::SetTurbulence(const XReal v)
{
  m_Turbulence = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::SetTurbulenceSpeed(const XReal v)
{
  m_TurbulenceSpeed = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::SetDetailTurbulence(const XReal v) 
{
  m_DetailTurbulence = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeAxis::WriteChunk(std::ostream& ofs)
{
  if(!XField::WriteChunk(ofs))
    return false;

  // write volume axis attributes
  ofs << "\tdirection " << m_Direction << "\n";
  ofs << "\tdirectional_speed " << m_DirectionalSpeed << "\n";
  ofs << "\tinvert_attenuation " << m_InvertAttenuation << "\n";
  ofs << "\tspeed_along_axis " << m_SpeedAlongAxis << "\n";
  ofs << "\tspeed_around_axis " << m_SpeedAroundAxis << "\n";
  ofs << "\tspeed_away_from_axis " << m_SpeedAwayFromAxis << "\n";
  ofs << "\tspeed_away_from_center " << m_SpeedAwayFromCenter << "\n";
  ofs << "\tturbulence " << m_Turbulence << "\n";
  ofs << "\tturbulence_speed " << m_TurbulenceSpeed << "\n";
  ofs << "\tturbulence_frequency " << m_TurbulenceFrequency << "\n";
  ofs << "\tturbulence_offset " << m_TurbulenceOffset << "\n";
  ofs << "\tdetail_turbulence " << m_DetailTurbulence << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeAxis::DoData(XFileIO& io) 
{
  DUMPER(XVolumeAxis);

  CHECK_IO( XField::DoData(io) );

  CHECK_IO( io.DoData( &m_Direction ) );
  DPARAM(m_Direction);

  CHECK_IO( io.DoData( &m_DirectionalSpeed ) );
  DPARAM(m_DirectionalSpeed);

  CHECK_IO( io.DoData( &m_InvertAttenuation ) );
  DPARAM(m_InvertAttenuation);

  CHECK_IO( io.DoData( &m_SpeedAlongAxis ) );
  DPARAM(m_SpeedAlongAxis);

  CHECK_IO( io.DoData( &m_SpeedAroundAxis ) );
  DPARAM(m_SpeedAroundAxis);

  CHECK_IO( io.DoData( &m_SpeedAwayFromAxis ) );
  DPARAM(m_SpeedAwayFromAxis);

  CHECK_IO( io.DoData( &m_SpeedAwayFromCenter ) );
  DPARAM(m_SpeedAwayFromCenter);

  CHECK_IO( io.DoData( &m_Turbulence ) );
  DPARAM(m_Turbulence);

  CHECK_IO( io.DoData( &m_TurbulenceSpeed ) );
  DPARAM(m_TurbulenceSpeed);

  CHECK_IO( io.DoData( &m_TurbulenceFrequency ) );
  DPARAM(m_TurbulenceFrequency);

  CHECK_IO( io.DoData( &m_TurbulenceOffset ) );
  DPARAM(m_TurbulenceOffset);

  CHECK_IO( io.DoData( &m_DetailTurbulence ) );
  DPARAM(m_DetailTurbulence);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::DoCopy(const XBase* rhs)
{
  const XVolumeAxis* cb = rhs->HasFn<XVolumeAxis>();
  XMD_ASSERT(cb);
  m_Direction = cb->m_Direction;
  m_DirectionalSpeed = cb->m_DirectionalSpeed;
  m_InvertAttenuation = cb->m_InvertAttenuation;
  m_SpeedAlongAxis = cb->m_SpeedAlongAxis;
  m_SpeedAroundAxis = cb->m_SpeedAroundAxis;
  m_SpeedAwayFromAxis = cb->m_SpeedAwayFromAxis;
  m_SpeedAwayFromCenter = cb->m_SpeedAwayFromCenter;
  m_Turbulence = cb->m_Turbulence;
  m_TurbulenceSpeed = cb->m_TurbulenceSpeed;
  m_TurbulenceFrequency = cb->m_TurbulenceFrequency;
  m_TurbulenceOffset = cb->m_TurbulenceOffset;
  m_DetailTurbulence = cb->m_DetailTurbulence;
  XField::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeAxis::ReadChunk(std::istream& ifs)
{
  if(!XField::ReadChunk(ifs))
    return false;

  READ_CHECK("direction",ifs);
  ifs >> m_Direction;

  READ_CHECK("directional_speed",ifs);
  ifs >> m_DirectionalSpeed;

  READ_CHECK("invert_attenuation",ifs);
  ifs >> m_InvertAttenuation;

  READ_CHECK("speed_along_axis",ifs);
  ifs >> m_SpeedAlongAxis;

  READ_CHECK("speed_around_axis",ifs);
  ifs >> m_SpeedAroundAxis;

  READ_CHECK("speed_away_from_axis",ifs);
  ifs >> m_SpeedAwayFromAxis;

  READ_CHECK("speed_away_from_center",ifs);
  ifs >> m_SpeedAwayFromCenter;

  READ_CHECK("turbulence",ifs);
  ifs >> m_Turbulence;

  READ_CHECK("turbulence_speed",ifs);
  ifs >> m_TurbulenceSpeed;

  READ_CHECK("turbulence_frequency",ifs);
  ifs >> m_TurbulenceFrequency;

  READ_CHECK("turbulence_offset",ifs);
  ifs >> m_TurbulenceOffset;

  READ_CHECK("detail_turbulence",ifs);
  ifs >> m_DetailTurbulence;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeAxis::NodeDeath(XId id)
{
  return XField::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeAxis::PreDelete(XIdSet& extra_nodes)
{
  XField::PreDelete(extra_nodes);
}
}
