//----------------------------------------------------------------------------------------------------------------------
/// \file CollisionObject.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/CollisionBase.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XCollisionObject::XCollisionObject(XModel* pmod) 
  : XShape(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XCollisionObject::~XCollisionObject()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XCollisionObject::GetApiType() const
{
  return XFn::CollisionObject;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XCollisionObject::GetDataSize() const 
{
  return XShape::GetDataSize() + 
    2*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XCollisionObject::GetFn(XFn::Type type)
{
  if(XFn::CollisionObject==type)
    return (XCollisionObject*)this;
  return XShape::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XCollisionObject::GetFn(XFn::Type type) const
{
  if(XFn::CollisionObject==type)
    return (const XCollisionObject*)this;
  return XShape::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionObject::NodeDeath(XId id)
{
  return XShape::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionObject::PreDelete(XIdSet& extra_nodes)
{
  XShape::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionObject::ReadChunk(std::istream& ifs)
{
  if( !XShape::ReadChunk(ifs) )
    return false;

  READ_CHECK("gravity_scale",ifs);
  ifs >> m_GravityScale;
  READ_CHECK("mass",ifs);
  ifs >> m_Mass;
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionObject::WriteChunk(std::ostream& ofs)
{
  if( !XShape::WriteChunk(ofs) )
    return false;
  ofs << "\tgravity_scale " << m_GravityScale << "\n";
  ofs << "\tmass " << m_Mass << "\n";
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionObject::DoData(XFileIO& io)
{
  DUMPER(XCollisionObject);

  IO_CHECK( XShape::DoData(io) );

  io.DoData( &m_GravityScale );
  io.DoData( &m_Mass );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionObject::DoCopy(const XBase* rhs)
{
  const XCollisionObject* co = rhs->HasFn<XCollisionObject>();
  XMD_ASSERT(co);
  m_Mass = co->m_Mass;
  m_GravityScale = co->m_GravityScale;
  XShape::DoCopy(co);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCollisionObject::GetGravityScale() const
{
  return m_GravityScale;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCollisionObject::GetMass() const
{
  return m_Mass;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionObject::SetGravityScale(const XReal v)
{
  m_GravityScale = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionObject::SetMass(const XReal v)
{
  m_Mass = v;
}
}
