//----------------------------------------------------------------------------------------------------------------------
/// \file CollisionCapsule.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/CollisionCapsule.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XCollisionCapsule::XCollisionCapsule(XModel* pmod) 
  : XCollisionObject(pmod),m_Radius(1.0f),m_Length(1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XCollisionCapsule::~XCollisionCapsule()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XCollisionCapsule::GetApiType() const
{
  return XFn::CollisionCapsule;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XCollisionCapsule::GetDataSize() const 
{
  return XCollisionObject::GetDataSize() + 2*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XCollisionCapsule::GetFn(XFn::Type type)
{
  if(XFn::CollisionCapsule==type)
    return (XCollisionCapsule*)this;
  return XCollisionObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XCollisionCapsule::GetFn(XFn::Type type) const
{
  if(XFn::CollisionCapsule==type)
    return (const XCollisionCapsule*)this;
  return XCollisionObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionCapsule::NodeDeath(XId id)
{
  return XCollisionObject::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionCapsule::PreDelete(XIdSet& extra_nodes)
{
  XCollisionObject::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCollisionCapsule::GetRadius() const
{
  return m_Radius;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionCapsule::SetRadius(const XReal v)
{    
  m_Radius = v;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCollisionCapsule::GetLength() const
{
  return m_Length;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionCapsule::SetLength(const XReal v)
{    
  m_Length = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionCapsule::ReadChunk(std::istream& ifs)
{
  READ_CHECK("radius",ifs);
  ifs >> m_Radius;

  READ_CHECK("length",ifs);
  ifs >> m_Length;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionCapsule::WriteChunk(std::ostream& ofs)
{
  ofs << "\tradius " << m_Radius << "\n";
  ofs << "\tlength" << m_Length << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionCapsule::DoData(XFileIO& io) 
{
  DUMPER(XCollisionCapsule);
  
  IO_CHECK( XCollisionObject::DoData(io) );

  IO_CHECK( io.DoData(&m_Radius) );
  DPARAM( m_Radius );

  IO_CHECK( io.DoData(&m_Length) );
  DPARAM( m_Length);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionCapsule::DoCopy(const XBase* rhs)
{
  const XCollisionCapsule* cb = rhs->HasFn<XCollisionCapsule>();
  XMD_ASSERT(cb);
  m_Radius = cb->m_Radius;
  m_Length = cb->m_Length;
  XCollisionObject::DoCopy(cb);
}
}
