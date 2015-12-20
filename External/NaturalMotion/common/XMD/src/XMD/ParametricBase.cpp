//----------------------------------------------------------------------------------------------------------------------
/// \file ParametricBase.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/ParametricBase.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XParametricBase::XParametricBase(XModel* pmod) 
  : XGeometry(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XParametricBase::~XParametricBase(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU32 XParametricBase::GetDataSize() const
{
  return XGeometry::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XParametricBase::GetApiType() const
{
  return XFn::ParametricBase;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XParametricBase::GetFn(XFn::Type type)
{
  if(XFn::ParametricBase==type)
    return (XParametricBase*)this;
  return XGeometry::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XParametricBase::GetFn(XFn::Type type) const
{
  if(XFn::ParametricBase==type)
    return (const XParametricBase*)this;
  return XGeometry::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XParametricBase::WriteChunk(std::ostream& ofs) 
{
  if(!XGeometry::WriteChunk(ofs))
    return false;

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XParametricBase::ReadChunk(std::istream& ifs)
{
  if(!XGeometry::ReadChunk(ifs))
    return false;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
void XParametricBase::PreDelete(XIdSet& extra_nodes)
{
  XGeometry::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XParametricBase::NodeDeath(XId id)
{
  return XGeometry::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
bool XParametricBase::DoData(XFileIO& io)
{
  DUMPER(XParametricBase);

  CHECK_IO( XGeometry::DoData(io) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XParametricBase::DoCopy(const XBase* rhs)
{
  const XParametricBase* cb = rhs->HasFn<XParametricBase>();
  XMD_ASSERT(cb);
  XGeometry::DoCopy(cb);
}
}
