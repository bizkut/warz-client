//----------------------------------------------------------------------------------------------------------------------
/// \file Drag.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Drag.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XDrag::XDrag(XModel* pmod) 
  : XField(pmod),m_Direction(0,1,0),m_bUseDirection(false)
{
}

//----------------------------------------------------------------------------------------------------------------------
XDrag::~XDrag()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XDrag::GetApiType() const
{
  return XFn::Drag;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XDrag::GetDataSize() const 
{
  return XField::GetDataSize() + 3*sizeof(XReal) + sizeof(bool);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XDrag::GetFn(XFn::Type type)
{
  if(XFn::Drag==type)
    return (XDrag*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XDrag::GetFn(XFn::Type type) const
{
  if(XFn::Drag==type)
    return (const XDrag*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XDrag::NodeDeath(XId id)
{
  return XField::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XDrag::PreDelete(XIdSet& extra_nodes)
{
  XField::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XDrag::GetDirection() const
{
  return m_Direction;
}

//----------------------------------------------------------------------------------------------------------------------
void XDrag::SetDirection(const XVector3& v)
{
  m_Direction = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XDrag::GetIsDirectionUsed() const
{
  return m_bUseDirection;
}

//----------------------------------------------------------------------------------------------------------------------
void XDrag::SetIsDirectionUsed(const bool v)
{
  m_bUseDirection = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XDrag::ReadChunk(std::istream& ifs)
{
  if(!XField::ReadChunk(ifs))
    return false;

  READ_CHECK("direction",ifs);
  ifs >> m_Direction;

  READ_CHECK("use_direction",ifs);
  ifs >> m_bUseDirection ;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XDrag::WriteChunk(std::ostream& ofs)
{
  if(!XField::WriteChunk(ofs))
    return false;

  // the camera's aspect ratio
  ofs << "\tdirection " << m_Direction << "\n";
  ofs << "\tuse_direction " << m_bUseDirection << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XDrag::DoData(XFileIO& io) 
{
  DUMPER(XDrag);

  CHECK_IO( XField::DoData(io) );

  CHECK_IO( io.DoData( &m_Direction ) );
  DPARAM(m_Direction);
  
  CHECK_IO( io.DoData( &m_bUseDirection ) );
  DPARAM(m_bUseDirection);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XDrag::DoCopy(const XBase* rhs)
{
  const XDrag* cb = rhs->HasFn<XDrag>();
  XMD_ASSERT(cb);
  m_Direction = cb->m_Direction;
  m_bUseDirection = cb->m_bUseDirection;
  XField::DoCopy(cb);
}
}
