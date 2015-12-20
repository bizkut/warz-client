//----------------------------------------------------------------------------------------------------------------------
/// \file VolumeObject.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/VolumeObject.h"
#include "XMD/FileIO.h"
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XVolumeObject::XVolumeObject(XModel* pmod) 
  : XShape(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XVolumeObject::~XVolumeObject()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XVolumeObject::GetApiType() const
{
  return XFn::VolumeObject;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XVolumeObject::GetDataSize() const 
{
  return XShape::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XVolumeObject::GetFn(XFn::Type type)
{
  if(XFn::VolumeObject==type)
    return (XVolumeObject*)this;
  return XShape::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XVolumeObject::GetFn(XFn::Type type) const
{
  if(XFn::VolumeObject==type)
    return (const XVolumeObject*)this;
  return XShape::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeObject::NodeDeath(XId id)
{
  return XShape::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeObject::PreDelete(XIdSet& extra_nodes)
{
  XShape::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeObject::ReadChunk(std::istream& ifs)
{
  return XShape::ReadChunk(ifs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeObject::WriteChunk(std::ostream& ofs)
{
  return XShape::WriteChunk(ofs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVolumeObject::DoData(XFileIO& io)
{
  DUMPER(XVolumeObject);

  IO_CHECK( XShape::DoData(io) );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XVolumeObject::DoCopy(const XBase* rhs)
{
  const XVolumeObject* cb = rhs->HasFn<XVolumeObject>();
  XMD_ASSERT(cb);
  XShape::DoCopy(cb);
}
}
