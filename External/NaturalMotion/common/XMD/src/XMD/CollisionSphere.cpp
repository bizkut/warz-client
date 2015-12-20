//----------------------------------------------------------------------------------------------------------------------
/// \file CollisionSphere.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/CollisionSphere.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XCollisionSphere::XCollisionSphere(XModel* pmod) 
  : XCollisionObject(pmod),m_Radius(1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XCollisionSphere::~XCollisionSphere()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XCollisionSphere::GetApiType() const
{
  return XFn::CollisionSphere;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XCollisionSphere::GetDataSize() const 
{
  return XCollisionObject::GetDataSize() + sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XCollisionSphere::GetFn(XFn::Type type)
{
  if(XFn::CollisionSphere==type)
    return (XCollisionSphere*)this;
  return XCollisionObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XCollisionSphere::GetFn(XFn::Type type) const
{
  if(XFn::CollisionSphere==type)
    return (const XCollisionSphere*)this;
  return XCollisionObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionSphere::NodeDeath(XId id)
{
  return XCollisionObject::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionSphere::PreDelete(XIdSet& extra_nodes)
{
  XCollisionObject::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCollisionSphere::GetRadius() const
{
  return m_Radius;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionSphere::SetRadius(const XReal v)
{    
  m_Radius = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionSphere::ReadChunk(std::istream& ifs)
{
  READ_CHECK("radius",ifs);
  ifs >> m_Radius;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionSphere::WriteChunk(std::ostream& ofs)
{
  ofs << "\tradius " << m_Radius << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionSphere::DoData(XFileIO& io)
{
  DUMPER(XCollisionSphere);
  
  IO_CHECK( XCollisionObject::DoData(io) );

  IO_CHECK( io.DoData(&m_Radius) );
  DPARAM( m_Radius );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionSphere::DoCopy(const XBase* rhs)
{
  const XCollisionSphere* cb = rhs->HasFn<XCollisionSphere>();
  XMD_ASSERT(cb);
  m_Radius = cb->m_Radius;
  XCollisionObject::DoCopy(cb);
}
}
