//----------------------------------------------------------------------------------------------------------------------
/// \file HermitePatch.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/HermitePatch.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XHermitePatch::XHermitePatch(XModel* pmod) 
  : XParametricSurface(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XHermitePatch::~XHermitePatch(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XHermitePatch::GetApiType() const
{
  return XFn::HermitePatch;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XHermitePatch::GetDataSize() const 
{
  return XParametricSurface::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XHermitePatch::GetFn(XFn::Type type)
{
  if(XFn::HermitePatch==type)
    return (XHermitePatch*)this;
  return XParametricSurface::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XHermitePatch::GetFn(XFn::Type type) const
{
  if(XFn::HermitePatch==type)
    return (const XHermitePatch*)this;
  return XParametricSurface::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XHermitePatch::NodeDeath(XId id)
{
  return XParametricSurface::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XHermitePatch::PreDelete(XIdSet& extra_nodes)
{
  XParametricSurface::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XHermitePatch::ReadChunk(std::istream& ifs)
{
  return XParametricSurface::ReadChunk(ifs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XHermitePatch::WriteChunk(std::ostream& ofs)
{
  return XParametricSurface::WriteChunk(ofs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XHermitePatch::DoData(XFileIO& io)
{
  DUMPER(XHermitePatch);

  CHECK_IO( XParametricSurface::DoData(io) );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XHermitePatch::DoCopy(const XBase* rhs)
{
  const XHermitePatch* cb = rhs->HasFn<XHermitePatch>();
  XMD_ASSERT(cb);
  XParametricSurface::DoCopy(cb);
}
}
