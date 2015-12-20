//----------------------------------------------------------------------------------------------------------------------
/// \file VolumeCylinder.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/VolumeCylinder.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XVolumeCylinder::XVolumeCylinder(XModel* pmod) 
  : XVolumeObject(pmod),m_Radius(1.0f),m_Height(1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XVolumeCylinder::~XVolumeCylinder()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XVolumeCylinder::GetApiType() const
{
  return XFn::VolumeCylinder;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XVolumeCylinder::GetDataSize() const 
{
  return XVolumeObject::GetDataSize() + 2*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XVolumeCylinder::GetFn(XFn::Type type)
{
  if(XFn::VolumeCylinder==type)
    return (XVolumeCylinder*)this;
  return XVolumeObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XVolumeCylinder::GetFn(XFn::Type type) const
{
  if(XFn::VolumeCylinder==type)
    return (const XVolumeCylinder*)this;
  return XVolumeObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeCylinder::NodeDeath(XId id)
{
  return XVolumeObject::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeCylinder::PreDelete(XIdSet& extra_nodes)
{
  XVolumeObject::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeCylinder::GetRadius() const
{
  return m_Radius;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XVolumeCylinder::GetHeight() const
{
  return m_Height;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeCylinder::SetRadius(const XReal v)
{
  m_Radius  = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeCylinder::SetHeight(const XReal v)
{
  m_Height = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeCylinder::ReadChunk(std::istream& ifs)
{
  READ_CHECK("radius",ifs);
  ifs >> m_Radius;

  READ_CHECK("height",ifs);
  ifs >> m_Height;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeCylinder::WriteChunk(std::ostream& ofs)
{
  // the camera's aspect ratio
  ofs << "\tradius " << m_Radius << "\n";
  ofs << "\theight " << m_Height << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeCylinder::DoData(XFileIO& io) 
{
  DUMPER(XVolumeCylinder);

  IO_CHECK( XVolumeObject::DoData(io) );

  IO_CHECK( io.DoData(&m_Radius) );
  DPARAM( m_Radius );

  IO_CHECK( io.DoData(&m_Height) );
  DPARAM( m_Height );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeCylinder::DoCopy(const XBase* rhs)
{
  const XVolumeCylinder* cb = rhs->HasFn<XVolumeCylinder>();
  XMD_ASSERT(cb);
  m_Radius = cb->m_Radius;
  m_Height = cb->m_Height;
  XVolumeObject::DoCopy(cb);
}
}
