//----------------------------------------------------------------------------------------------------------------------
/// \file VolumeSphere.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/VolumeSphere.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XVolumeSphere::XVolumeSphere(XModel* pmod) 
  : XVolumeObject(pmod),m_Radius(1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XVolumeSphere::~XVolumeSphere()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XVolumeSphere::GetApiType() const
{
  return XFn::VolumeSphere;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XVolumeSphere::GetDataSize() const 
{
  return XVolumeObject::GetDataSize() + sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XVolumeSphere::GetFn(XFn::Type type)
{
  if(XFn::VolumeSphere==type)
    return (XVolumeSphere*)this;
  return XVolumeObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XVolumeSphere::GetFn(XFn::Type type) const
{
  if(XFn::VolumeSphere==type)
    return (const XVolumeSphere*)this;
  return XVolumeObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeSphere::NodeDeath(XId id)
{
  return XVolumeObject::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeSphere::PreDelete(XIdSet& extra_nodes)
{
  XVolumeObject::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeSphere::GetRadius() const
{
  return m_Radius;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeSphere::SetRadius(const XReal v)
{    
  m_Radius = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeSphere::ReadChunk(std::istream& ifs)
{
  READ_CHECK("radius",ifs);
  ifs >> m_Radius;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeSphere::WriteChunk(std::ostream& ofs)
{
  // the camera's aspect ratio
  ofs << "\tradius " << m_Radius << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeSphere::DoData(XFileIO& io)
{
  DUMPER(XVolumeSphere);
  
  IO_CHECK( XVolumeObject::DoData(io) );

  IO_CHECK( io.DoData(&m_Radius) );
  DPARAM( m_Radius );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeSphere::DoCopy(const XBase* rhs)
{
  const XVolumeSphere* cb = rhs->HasFn<XVolumeSphere>();
  XMD_ASSERT(cb);
  m_Radius = cb->m_Radius;
  XVolumeObject::DoCopy(cb);
}
}
