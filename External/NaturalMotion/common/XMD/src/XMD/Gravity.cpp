//----------------------------------------------------------------------------------------------------------------------
/// \file Gravity.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Gravity.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XGravity::XGravity(XModel* pmod) 
  : XField(pmod),m_Direction(0,-1,0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XGravity::~XGravity()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XGravity::GetApiType() const
{
  return XFn::Gravity;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XGravity::GetDataSize() const
{
  return XField::GetDataSize() + 3*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XGravity::GetFn(XFn::Type type)
{
  if(XFn::Gravity==type)
    return (XGravity*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XGravity::GetFn(XFn::Type type) const
{
  if(XFn::Gravity==type)
    return (const XGravity*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XGravity::NodeDeath(XId id)
{
  return XField::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XGravity::PreDelete(XIdSet& extra_nodes)
{
  XField::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XGravity::GetDirection() const
{
  return m_Direction;
}

//----------------------------------------------------------------------------------------------------------------------
void XGravity::SetDirection(const XVector3& v) 
{
  m_Direction = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XGravity::ReadChunk(std::istream& ifs)
{
  if(!XField::ReadChunk(ifs))
    return false;

  READ_CHECK("direction",ifs);
  ifs >> m_Direction ;
  
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XGravity::WriteChunk(std::ostream& ofs)
{
  if(!XField::WriteChunk(ofs))
    return false;

  ofs << "\tdirection " << m_Direction << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XGravity::DoData(XFileIO& io) 
{
  DUMPER(XGravity);

  CHECK_IO( XField::DoData(io) );

  CHECK_IO( io.DoData( &m_Direction ) );
  DPARAM(m_Direction);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XGravity::DoCopy(const XBase* rhs)
{
  const XGravity* cb = rhs->HasFn<XGravity>();
  XMD_ASSERT(cb);
  m_Direction = cb->m_Direction;
  XField::DoCopy(cb);
}
}
