//----------------------------------------------------------------------------------------------------------------------
/// \file Camera.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Camera.h"
#include "XMD/FileIO.h"
#include "XMD/NodeProperties.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XCamera::XCamera(XModel* pmod) 
  : XShape(pmod),
  m_Aspect(1.5f),
  m_Near(0.1f),
  m_Far(1000.0f),
  m_HorizAspect(0.950022f),
  m_VertAspect(0.660595f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XCamera::~XCamera()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XCamera::GetApiType() const
{
  return XFn::Camera;
}

//----------------------------------------------------------------------------------------------------------------------
void XCamera::DoCopy(const XBase* rhs)
{
  const XCamera* cb = rhs->HasFn<XCamera>();
  XMD_ASSERT(cb);

  m_Aspect = cb->m_Aspect;
  m_Near = cb->m_Near;
  m_Far = cb->m_Far;
  m_HorizAspect = cb->m_HorizAspect;
  m_VertAspect = cb->m_VertAspect;

  XShape::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCamera::DoData(XFileIO& io) 
{
  DUMPER(XCamera);

  IO_CHECK( XShape::DoData(io) );

  IO_CHECK( io.DoData(&m_Aspect) );
  DPARAM( m_Aspect );

  IO_CHECK( io.DoData(&m_Near) );
  DPARAM( m_Near );

  IO_CHECK( io.DoData(&m_Far) );
  DPARAM( m_Far );

  IO_CHECK( io.DoData(&m_HorizAspect) );
  DPARAM( m_HorizAspect );

  IO_CHECK( io.DoData(&m_VertAspect) );
  DPARAM( m_VertAspect );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCamera::GetAspect() const
{
  return m_Aspect;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XCamera::GetDataSize() const 
{
  XU32 size = XShape::GetDataSize();
  size += 5 * sizeof(XReal);
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCamera::GetFar() const
{
  return m_Far;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCamera::GetHorizontalAspect() const
{
  return m_HorizAspect;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCamera::GetNear() const
{
  return m_Near;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCamera::GetVerticalAspect() const
{
  return m_VertAspect;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XCamera::GetFn(XFn::Type type)
{
  if(XFn::Camera ==type)
    return (XCamera*)this;
  return XShape::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XCamera::GetFn(XFn::Type type) const
{
  if(XFn::Camera==type)
    return (const XCamera*)this;
  return XShape::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCamera::NodeDeath(XId id)
{
  return XShape::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XCamera::PreDelete(XIdSet& extra_nodes)
{
  XShape::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCamera::ReadChunk(std::istream& ifs)
{
  READ_CHECK("aspect",ifs);
  ifs >> m_Aspect;

  READ_CHECK("near",ifs);
  ifs >> m_Near;

  READ_CHECK("far",ifs);
  ifs >> m_Far;

  READ_CHECK("horiz_aspect",ifs);
  ifs >> m_HorizAspect;

  READ_CHECK("vert_aspect",ifs);
  ifs >> m_VertAspect;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
void XCamera::SetAspect(const XReal v)
{
  m_Aspect = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XCamera::SetFar(const XReal v)
{
  m_Far = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XCamera::SetHorizontalAspect(const XReal v)
{
  m_HorizAspect = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XCamera::SetNear(const XReal v)
{
  m_Near = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XCamera::SetVerticalAspect(const XReal v) 
{
  m_VertAspect = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCamera::WriteChunk(std::ostream& ofs)
{
  // the camera's aspect ratio
  ofs << "\taspect " << m_Aspect << "\n";
  ofs << "\tnear " << m_Near << "\n";
  ofs << "\tfar " << m_Far << "\n";
  ofs << "\thoriz_aspect " << m_HorizAspect << "\n";
  ofs << "\tvert_aspect " << m_VertAspect << "\n";

  return ofs.good();
}
}
