//----------------------------------------------------------------------------------------------------------------------
/// \file PointConstraint.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/PointConstraint.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XPointConstraint::XPointConstraint(XModel* pmod) 
  : XConstraint(pmod)
{
  m_Offset.set(0,0,0);
}

//----------------------------------------------------------------------------------------------------------------------
XPointConstraint::~XPointConstraint()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XPointConstraint::GetApiType() const
{
  return XFn::PointConstraint;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XPointConstraint::GetFn(XFn::Type type)
{
  if(XFn::PointConstraint==type)
    return (XPointConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XPointConstraint::GetFn(XFn::Type type) const
{
  if(XFn::PointConstraint==type)
    return (const XPointConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XPointConstraint::NodeDeath(XId id)
{
  return XConstraint::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XPointConstraint::PreDelete(XIdSet& extra_nodes)
{
  XConstraint::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XPointConstraint::ReadChunk(std::istream& ifs)
{  
  if (!XConstraint::ReadChunk(ifs)) 
    return false;

  READ_CHECK("offset",ifs);
  ifs >> m_Offset;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XPointConstraint::WriteChunk(std::ostream& ofs) 
{
  if (!XConstraint::WriteChunk(ofs))
    return false;

  ofs << "\toffset " << m_Offset << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XPointConstraint::DoData(XFileIO& io) 
{
  DUMPER(XPointConstraint);

  IO_CHECK( XConstraint::DoData(io) );

  io.DoData(&m_Offset);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XPointConstraint::GetDataSize() const 
{
  return 3*sizeof(XReal) + XConstraint::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XPointConstraint::GetOffset() const
{
  return m_Offset; 
}

//----------------------------------------------------------------------------------------------------------------------
void XPointConstraint::SetOffset(const XVector3& v) 
{
  m_Offset=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XPointConstraint::DoCopy(const XBase* rhs)
{
  const XPointConstraint* cb = rhs->HasFn<XPointConstraint>();
  XMD_ASSERT(cb);
  m_Offset = cb->m_Offset;
  XConstraint::DoCopy(cb);
}
}
