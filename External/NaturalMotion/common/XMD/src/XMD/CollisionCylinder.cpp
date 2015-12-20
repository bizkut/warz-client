//----------------------------------------------------------------------------------------------------------------------
/// \file CollisionCylinder.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/CollisionCylinder.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XCollisionCylinder::XCollisionCylinder(XModel* pmod) 
  : XCollisionObject(pmod),m_Radius(1.0f),m_Length(1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XCollisionCylinder::~XCollisionCylinder()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XCollisionCylinder::GetApiType() const
{
  return XFn::CollisionCylinder;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XCollisionCylinder::GetDataSize() const
{
  return XCollisionObject::GetDataSize() + 2*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XCollisionCylinder::GetFn(XFn::Type type)
{
  if(XFn::CollisionCylinder==type)
    return (XCollisionCylinder*)this;
  return XCollisionObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XCollisionCylinder::GetFn(XFn::Type type) const
{
  if(XFn::CollisionCylinder==type)
    return (const XCollisionCylinder*)this;
  return XCollisionObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionCylinder::NodeDeath(XId id)
{
  return XCollisionObject::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionCylinder::PreDelete(XIdSet& extra_nodes)
{
  XCollisionObject::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCollisionCylinder::GetRadius() const
{
  return m_Radius;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionCylinder::SetRadius(const XReal v)
{    
  m_Radius = v;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCollisionCylinder::GetLength() const
{
  return m_Length;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionCylinder::SetLength(const XReal v)
{    
  m_Length = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionCylinder::ReadChunk(std::istream& ifs)
{
  READ_CHECK("radius",ifs);
  ifs >> m_Radius;

  READ_CHECK("length",ifs);
  ifs >> m_Length;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionCylinder::WriteChunk(std::ostream& ofs)
{
  // the camera's aspect ratio
  ofs << "\tradius " << m_Radius << "\n";
  ofs << "\tlength" << m_Length << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionCylinder::DoData(XFileIO& io) 
{
  DUMPER(XCollisionCylinder);
  
  IO_CHECK( XCollisionObject::DoData(io) );

  IO_CHECK( io.DoData(&m_Radius) );
  DPARAM( m_Radius );

  IO_CHECK( io.DoData(&m_Length) );
  DPARAM( m_Length);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionCylinder::DoCopy(const XBase* rhs)
{
  const XCollisionCylinder* cb = rhs->HasFn<XCollisionCylinder>();
  XMD_ASSERT(cb);
  m_Radius = cb->m_Radius;
  m_Length = cb->m_Length;
  XCollisionObject::DoCopy(cb);
}
}
