//----------------------------------------------------------------------------------------------------------------------
/// \file BezierPatch.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/BezierPatch.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XBezierPatch::XBezierPatch(XModel* pmod) 
  : XParametricSurface(pmod)
{
  m_NumCvsU = m_NumCvsV = 4;
}

//----------------------------------------------------------------------------------------------------------------------
XBezierPatch::~XBezierPatch()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XBezierPatch::GetApiType() const
{
  return XFn::BezierPatch;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XBezierPatch::GetDataSize() const 
{
  return XParametricSurface::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XBezierPatch::GetFn(XFn::Type type)
{
  if(XFn::BezierPatch==type)
    return (XBezierPatch*)this;
  return XParametricSurface::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XBezierPatch::GetFn(XFn::Type type) const
{
  if(XFn::BezierPatch==type)
    return (const XBezierPatch*)this;
  return XParametricSurface::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBezierPatch::NodeDeath(XId id)
{
  return XParametricSurface::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XBezierPatch::PreDelete(XIdSet& extra_nodes)
{
  XParametricSurface::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBezierPatch::ReadChunk(std::istream& ifs)
{
  return XParametricSurface::ReadChunk(ifs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBezierPatch::WriteChunk(std::ostream& ofs)
{
  return XParametricSurface::WriteChunk(ofs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBezierPatch::DoData(XFileIO& io)
{
  DUMPER(XBezierPatch);

  CHECK_IO( XParametricSurface::DoData(io) );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XBezierPatch::DoCopy(const XBase* rhs)
{
  const XBezierPatch* cb = rhs->HasFn<XBezierPatch>();
  XMD_ASSERT(cb);
  XParametricSurface::DoCopy(cb);
}
}
