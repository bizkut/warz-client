//----------------------------------------------------------------------------------------------------------------------
/// \file CollisionCone.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/CollisionCone.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XCollisionCone::XCollisionCone(XModel* pmod) 
  : XCollisionObject(pmod),m_Angle(0.785398f),m_Cap(1.0f)
{
}

//----------------------------------------------------------------------------------------------------------------------
XCollisionCone::~XCollisionCone()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XCollisionCone::GetApiType() const
{
  return XFn::CollisionCone;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XCollisionCone::GetDataSize() const 
{
  return XCollisionObject::GetDataSize() + 2*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XCollisionCone::GetFn(XFn::Type type)
{
  if(XFn::CollisionCone==type)
    return (XCollisionCone*)this;
  return XCollisionObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XCollisionCone::GetFn(XFn::Type type) const
{
  if(XFn::CollisionCone==type)
    return (const XCollisionCone*)this;
  return XCollisionObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionCone::NodeDeath(XId id)
{
  return XCollisionObject::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionCone::PreDelete(XIdSet& extra_nodes)
{
  XCollisionObject::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCollisionCone::GetAngle() const
{
  return m_Angle;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XCollisionCone::GetCap() const
{
  return m_Cap;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionCone::SetAngle(const XReal v)
{
  m_Angle  = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionCone::SetCap(const XReal v)
{
  m_Cap = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionCone::ReadChunk(std::istream& ifs)
{
  READ_CHECK("angle",ifs);
  ifs >> m_Angle;

  READ_CHECK("cap",ifs);
  ifs >> m_Cap;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionCone::WriteChunk(std::ostream& ofs)
{
  // the camera's aspect ratio
  ofs << "\tangle " << m_Angle << "\n";
  ofs << "\tcap " << m_Cap << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionCone::DoData(XFileIO& io)
{
  DUMPER(XCollisionCone);

  IO_CHECK( XCollisionObject::DoData(io) );

  IO_CHECK( io.DoData(&m_Angle) );
  DPARAM( m_Angle );

  IO_CHECK( io.DoData(&m_Cap) );
  DPARAM( m_Cap );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionCone::DoCopy(const XBase* rhs)
{
  const XCollisionCone* cb = rhs->HasFn<XCollisionCone>();
  XMD_ASSERT(cb);
  m_Angle = cb->m_Angle;
  m_Cap = cb->m_Cap;
  XCollisionObject::DoCopy(cb);
}
}
