//----------------------------------------------------------------------------------------------------------------------
/// \file AmbientLight.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/AmbientLight.h"
#include "XMD/FileIO.h"

namespace XMD 
{  
//----------------------------------------------------------------------------------------------------------------------
XAmbientLight::XAmbientLight(XModel* pmod) 
  : XLight(pmod)
{
  m_AmbientShade=0;
  m_ShadowRadius=100;
  m_CastSoftShadows=false;
}

//----------------------------------------------------------------------------------------------------------------------
XAmbientLight::~XAmbientLight()
{
}


//----------------------------------------------------------------------------------------------------------------------
XFn::Type XAmbientLight::GetApiType() const
{
  return XFn::AmbientLight;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XAmbientLight::GetFn(XFn::Type type)
{
  if(XFn::AmbientLight==type)
    return (XAmbientLight*)this;
  return XLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XAmbientLight::GetFn(XFn::Type type) const
{
  if(XFn::AmbientLight==type)
    return (const XAmbientLight*)this;
  return XLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAmbientLight::WriteChunk(std::ostream& ofs)
{
  XLight::WriteChunk(ofs);
  ofs << "\tambient_shade\t" << m_AmbientShade << "\n";
  ofs << "\tcast_soft_shadows\t" << m_CastSoftShadows << "\n";
  ofs << "\tshadow_radius\t" << m_ShadowRadius << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAmbientLight::DoData(XFileIO& io) 
{
  DUMPER(XAmbientLight);

  IO_CHECK( XLight::DoData(io) );

  IO_CHECK( io.DoData(&m_AmbientShade) );
  IO_CHECK( io.DoData(&m_CastSoftShadows) );
  IO_CHECK( io.DoData(&m_ShadowRadius) );
  DPARAM( m_AmbientShade );
  DPARAM( m_ShadowRadius );
  DPARAM( m_CastSoftShadows );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XAmbientLight::DoCopy(const XBase* rhs)
{
  const XAmbientLight* cb = rhs->HasFn<XAmbientLight>();
  XMD_ASSERT(cb);

  m_AmbientShade = cb->m_AmbientShade;
  m_CastSoftShadows = cb->m_CastSoftShadows;
  m_ShadowRadius = cb->m_ShadowRadius;

  XLight::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAmbientLight::GetDataSize() const  
{
  return sizeof(XReal)*2 + sizeof(bool) + XLight::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAmbientLight::ReadChunk(std::istream& ifs)
{
  if(!XLight::ReadChunk(ifs))
    return false;

  READ_CHECK("ambient_shade",ifs);
  ifs >> m_AmbientShade;

  READ_CHECK("cast_soft_shadows",ifs);
  ifs >> m_CastSoftShadows;

  READ_CHECK("shadow_radius",ifs);
  ifs >> m_ShadowRadius;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAmbientLight::NodeDeath(XId id)
{
  return XLight::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XAmbientLight::PreDelete(XIdSet& extra_nodes)
{
  XLight::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAmbientLight::GetAmbientShade() const
{
  return m_AmbientShade;
}

//----------------------------------------------------------------------------------------------------------------------
void XAmbientLight::SetAmbientShade(const XReal v)
{
  m_AmbientShade = v;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAmbientLight::GetShadowRadius() const
{
  return m_ShadowRadius;
}

//----------------------------------------------------------------------------------------------------------------------
void XAmbientLight::SetShadowRadius(const XReal v) 
{
  m_ShadowRadius = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAmbientLight::GetCastSoftShadows() const
{
  return m_CastSoftShadows;
}

//----------------------------------------------------------------------------------------------------------------------
void XAmbientLight::SetCastSoftShadows(const bool v) 
{
  m_CastSoftShadows=v;
}
}
