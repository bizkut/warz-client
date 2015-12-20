//----------------------------------------------------------------------------------------------------------------------
/// \file DirectionalLight.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/DirectionalLight.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XDirectionalLight::XDirectionalLight(XModel* pmod)
  : XExtendedLight(pmod),m_ShadowAngle(0.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XDirectionalLight::~XDirectionalLight()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XDirectionalLight::GetApiType() const
{
  return XFn::DirectionalLight;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XDirectionalLight::GetFn(XFn::Type type)
{
  if(XFn::DirectionalLight==type)
    return (XDirectionalLight*)this;
  return XExtendedLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XDirectionalLight::GetFn(XFn::Type type) const
{
  if(XFn::DirectionalLight==type)
    return (const XDirectionalLight*)this;
  return XExtendedLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XDirectionalLight::WriteChunk(std::ostream& ofs)
{
  if(!XExtendedLight::WriteChunk(ofs))
    return false;
  ofs << "\tshadow_angle\t" << m_ShadowAngle << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XDirectionalLight::DoData(XFileIO& io)
{
  DUMPER(XDirectionalLight);

  IO_CHECK( XExtendedLight::DoData(io) );

  IO_CHECK( io.DoData(&m_ShadowAngle) );
  DPARAM( m_ShadowAngle );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XDirectionalLight::DoCopy(const XBase* rhs)
{
  const XDirectionalLight* cb = rhs->HasFn<XDirectionalLight>();
  XMD_ASSERT(cb);
  m_ShadowAngle = cb->m_ShadowAngle;
  XExtendedLight::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XDirectionalLight::GetDataSize() const 
{
  return sizeof(XReal) + XExtendedLight::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XDirectionalLight::ReadChunk(std::istream& ifs)
{
  if(!XExtendedLight::ReadChunk(ifs))
    return false;

  READ_CHECK("shadow_angle",ifs);
  ifs >> m_ShadowAngle;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XDirectionalLight::NodeDeath(XId id)
{
  return XExtendedLight::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XDirectionalLight::PreDelete(XIdSet& extra_nodes)
{
  XExtendedLight::PreDelete(extra_nodes);
}


//----------------------------------------------------------------------------------------------------------------------
XReal XDirectionalLight::GetShadowAngle() const 
{
  return m_ShadowAngle;
}

//----------------------------------------------------------------------------------------------------------------------
void XDirectionalLight::SetShadowAngle(const XReal v)
{
  m_ShadowAngle = v;
}
}
