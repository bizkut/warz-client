//----------------------------------------------------------------------------------------------------------------------
/// \file AreaLight.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/AreaLight.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XAreaLight::XAreaLight(XModel* pmod) 
  : XExtendedLight(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XAreaLight::~XAreaLight()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XAreaLight::GetApiType() const
{
  return XFn::AreaLight;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XAreaLight::GetFn(XFn::Type type)
{
  if(XFn::AreaLight==type)
    return (XAreaLight*)this;
  return XExtendedLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XAreaLight::GetFn(XFn::Type type) const
{
  if(XFn::AreaLight==type)
    return (const XAreaLight*)this;
  return XExtendedLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAreaLight::WriteChunk(std::ostream& ofs) 
{
  return XExtendedLight::WriteChunk(ofs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAreaLight::DoData(XFileIO& io) 
{
  DUMPER(XAreaLight);

  IO_CHECK( XExtendedLight::DoData(io) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XAreaLight::DoCopy(const XBase* rhs)
{
  const XAreaLight* cb = rhs->HasFn<XAreaLight>();
  XMD_ASSERT(cb);
  XExtendedLight::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAreaLight::GetDataSize() const  
{
  return XExtendedLight::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAreaLight::ReadChunk(std::istream& ifs)
{
  return XExtendedLight::ReadChunk(ifs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAreaLight::NodeDeath(XId id)
{
  return XExtendedLight::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XAreaLight::PreDelete(XIdSet& extra_nodes)
{
  XExtendedLight::PreDelete(extra_nodes);
}
}
