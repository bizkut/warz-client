//----------------------------------------------------------------------------------------------------------------------
/// \file TexPlacement3D.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/TexPlacement3D.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XTexPlacement3D::XTexPlacement3D(XModel* pmod)
  : XBone(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement3D::~XTexPlacement3D()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XTexPlacement3D::GetApiType() const
{
  return XFn::TexturePlacement3D;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XTexPlacement3D::GetDataSize() const 
{
  return XBone::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XTexPlacement3D::GetFn(XFn::Type type)
{
  if(XFn::TexturePlacement3D==type)
    return (XTexPlacement3D*)this;
  return XBone::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XTexPlacement3D::GetFn(XFn::Type type) const
{
  if(XFn::TexturePlacement3D==type)
    return (const XTexPlacement3D*)this;
  return XBone::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement3D::NodeDeath(XId id)
{
  return XBone::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement3D::PreDelete(XIdSet& extra_nodes)
{
  XBone::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement3D::ReadChunk(std::istream& ifs)
{
  return XBone::ReadChunk(ifs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement3D::WriteChunk(std::ostream& ofs)
{
  return XBone::WriteChunk(ofs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexPlacement3D::DoData(XFileIO& io)
{
  DUMPER(XTexPlacement3D);

  CHECK_IO( XBone::DoData(io) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexPlacement3D::DoCopy(const XBase* rhs)
{
  const XTexPlacement3D* cb = rhs->HasFn<XTexPlacement3D>();
  XMD_ASSERT(cb);
  XBone::DoCopy(cb);
}
}
