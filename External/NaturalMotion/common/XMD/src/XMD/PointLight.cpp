//----------------------------------------------------------------------------------------------------------------------
/// \file PointLight.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/PointLight.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XPointLight::XPointLight(XModel* pmod) 
  : XExtendedLight(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XPointLight::~XPointLight()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XPointLight::GetApiType() const
{
  return XFn::PointLight;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XPointLight::GetFn(XFn::Type type)
{
  if(XFn::PointLight==type)
    return (XPointLight*)this;
  return XExtendedLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XPointLight::GetFn(XFn::Type type) const
{
  if(XFn::PointLight==type)
    return (const XPointLight*)this;
  return XExtendedLight::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XPointLight::DoCopy(const XBase* rhs)
{
  const XPointLight* cb = rhs->HasFn<XPointLight>();
  XMD_ASSERT(cb);
  XExtendedLight::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XPointLight::WriteChunk(std::ostream& ofs) 
{
  if(!XExtendedLight::WriteChunk(ofs))
    return false;
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XPointLight::DoData(XFileIO& io)
{
  DUMPER(XPointLight);

  IO_CHECK( XExtendedLight::DoData(io) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XPointLight::GetDataSize() const 
{
  return XExtendedLight::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XPointLight::ReadChunk(std::istream& ifs)
{
  if(!XExtendedLight::ReadChunk(ifs))
    return false;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XPointLight::NodeDeath(XId id)
{
  return XExtendedLight::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XPointLight::PreDelete(XIdSet& extra_nodes)
{
  XExtendedLight::PreDelete(extra_nodes);
}
}
