//----------------------------------------------------------------------------------------------------------------------
/// \file GeometryConstraint.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/GeometryConstraint.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XGeometryConstraint::XGeometryConstraint(XModel* pmod) 
  : XConstraint(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XGeometryConstraint::~XGeometryConstraint()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XGeometryConstraint::GetApiType() const
{
  return XFn::GeometryConstraint;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XGeometryConstraint::GetFn(XFn::Type type)
{
  if(XFn::GeometryConstraint==type)
    return (XGeometryConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XGeometryConstraint::GetFn(XFn::Type type) const
{
  if(XFn::GeometryConstraint==type)
    return (const XGeometryConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XGeometryConstraint::NodeDeath(XId id)
{
  return XConstraint::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XGeometryConstraint::PreDelete(XIdSet& extra_nodes)
{
  XConstraint::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XGeometryConstraint::ReadChunk(std::istream& ifs)
{  
  if (!XConstraint::ReadChunk(ifs)) 
    return false;
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XGeometryConstraint::WriteChunk(std::ostream& ofs) 
{
  if (!XConstraint::WriteChunk(ofs))
    return false;
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XGeometryConstraint::DoData(XFileIO& io) 
{
  DUMPER(XGeometryConstraint);

  IO_CHECK( XConstraint::DoData(io) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XGeometryConstraint::GetDataSize() const 
{
  return XConstraint::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
void XGeometryConstraint::DoCopy(const XBase* rhs)
{
  const XGeometryConstraint* cb = rhs->HasFn<XGeometryConstraint>();
  XMD_ASSERT(cb);
  XConstraint::DoCopy(cb);
}
}
