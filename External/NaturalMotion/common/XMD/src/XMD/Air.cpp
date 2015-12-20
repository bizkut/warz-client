//----------------------------------------------------------------------------------------------------------------------
/// \file Air.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Air.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XAir::XAir(XModel* pmod)
  : XField(pmod),
    m_Direction(0,1,0),
    m_Speed(1),
    m_InheritVelocity(0),
    m_Spread(0),
    m_bInheritRotation(false),
    m_bComponentOnly(false),
    m_bEnableSpread(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
XAir::~XAir()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XAir::GetApiType() const
{
  return XFn::Air;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAir::GetDataSize() const 
{
  return XField::GetDataSize() +
    3*sizeof(XReal) + 3*sizeof(XReal) + 3*sizeof(bool);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XAir::GetFn(XFn::Type type)
{
  if(XFn::Air==type)
    return (XAir*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XAir::GetFn(XFn::Type type) const
{
  if(XFn::Air==type)
    return (const XAir*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAir::NodeDeath(XId id)
{
  return XField::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XAir::PreDelete(XIdSet& extra_nodes)
{
  XField::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XAir::GetDirection() const
{
  return m_Direction;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAir::GetSpeed() const
{
  return m_Speed;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAir::GetSpread() const
{
  return m_Spread;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAir::GetSpreadEnabled() const
{
  return m_bEnableSpread;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAir::GetComponentOnly() const
{
  return m_bComponentOnly;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAir::GetInheritsRotation() const
{
  return m_bInheritRotation;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XAir::GetInheritsVelocity() const
{
  return m_InheritVelocity;
}

//----------------------------------------------------------------------------------------------------------------------
void XAir::SetDirection(const XVector3& v) 
{
  m_Direction = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XAir::SetSpeed(const XReal v)
{
  m_Speed = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XAir::SetSpread(const XReal v)
{
  m_Spread = v;
}

//----------------------------------------------------------------------------------------------------------------------
void XAir::SetSpreadEnabled(const bool b) 
{
  m_bEnableSpread = b;
}

//----------------------------------------------------------------------------------------------------------------------
void XAir::SetComponentOnly(const bool b) 
{
  m_bComponentOnly = b;
}

//----------------------------------------------------------------------------------------------------------------------
void XAir::SetInheritsRotation(const bool b) 
{
  m_bInheritRotation = b;
}

//----------------------------------------------------------------------------------------------------------------------
void XAir::SetInheritsVelocity(const XReal b) 
{
  m_InheritVelocity = b;
}

//----------------------------------------------------------------------------------------------------------------------
bool XAir::ReadChunk(std::istream& ifs)
{
  if(!XField::ReadChunk(ifs))
    return false;

  READ_CHECK("direction",ifs);
  ifs >> m_Direction;

  READ_CHECK("speed",ifs);
  ifs >> m_Speed;

  READ_CHECK("inherit_velocity",ifs);
  ifs >> m_InheritVelocity;

  READ_CHECK("inherit_rotation",ifs);
  ifs >> m_bInheritRotation;

  READ_CHECK("component_only",ifs);
  ifs >> m_bComponentOnly;

  READ_CHECK("spread",ifs);
  ifs >> m_Spread;

  READ_CHECK("enable_spread",ifs);
  ifs >> m_bEnableSpread;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAir::WriteChunk(std::ostream& ofs)
{
  if(!XField::WriteChunk(ofs))
    return false;

  ofs << "\tdirection "     << m_Direction      << "\n"
    << "\tspeed "       << m_Speed        << "\n"
    << "\tinherit_velocity " << m_InheritVelocity  << "\n"
    << "\tinherit_rotation " << m_bInheritRotation  << "\n"
    << "\tcomponent_only "   << m_bComponentOnly  << "\n"
    << "\tspread "       << m_Spread      << "\n"
    << "\tenable_spread "   << m_bEnableSpread   << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAir::DoData(XFileIO& io)
{
  DUMPER(XAir);

  CHECK_IO( XField::DoData(io) );

  CHECK_IO( io.DoData( &m_Direction ) );
  DPARAM(m_Direction);
  CHECK_IO( io.DoData( &m_Speed ) );
  DPARAM(m_Speed);
  CHECK_IO( io.DoData( &m_InheritVelocity ) );
  DPARAM(m_InheritVelocity);
  CHECK_IO( io.DoData( &m_bInheritRotation ) );
  DPARAM(m_bInheritRotation);
  CHECK_IO( io.DoData( &m_bComponentOnly ) );
  DPARAM(m_bComponentOnly);
  CHECK_IO( io.DoData( &m_Spread ) );
  DPARAM(m_Spread);
  CHECK_IO( io.DoData( &m_bEnableSpread ) );
  DPARAM(m_bEnableSpread);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XAir::DoCopy(const XBase* rhs)
{
  const XAir* cb = rhs->HasFn<XAir>();
  XMD_ASSERT(cb);
  m_Direction = cb->m_Direction;
  m_Speed = cb->m_Speed;
  m_InheritVelocity = cb->m_InheritVelocity;
  m_bInheritRotation = cb->m_bInheritRotation;
  m_bComponentOnly = cb->m_bComponentOnly;
  m_Spread = cb->m_Spread;
  m_bEnableSpread = cb->m_bEnableSpread;
  XField::DoCopy(cb);
}
}
