//----------------------------------------------------------------------------------------------------------------------
/// \file BezierCurve.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//-------------------------------------------------------------------------------

#include "XMD/BezierCurve.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XBezierCurve::XBezierCurve(XModel* pmod) 
  : XParametricCurve(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XBezierCurve::~XBezierCurve()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XBezierCurve::GetApiType() const
{
  return XFn::BezierCurve;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XBezierCurve::GetDataSize() const 
{
  return XParametricCurve::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XBezierCurve::GetFn(XFn::Type type)
{
  if(XFn::BezierCurve==type)
    return (XBezierCurve*)this;
  return XParametricCurve::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XBezierCurve::GetFn(XFn::Type type) const
{
  if(XFn::BezierCurve==type)
    return (const XBezierCurve*)this;
  return XParametricCurve::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBezierCurve::NodeDeath(XId id)
{
  return XParametricCurve::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XBezierCurve::PreDelete(XIdSet& extra_nodes)
{
  XParametricCurve::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBezierCurve::ReadChunk(std::istream& ifs)
{
  return XParametricCurve::ReadChunk(ifs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBezierCurve::WriteChunk(std::ostream& ofs)
{
  return XParametricCurve::WriteChunk(ofs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBezierCurve::DoData(XFileIO& io) 
{
  DUMPER(XBezierCurve);

  CHECK_IO( XParametricCurve::DoData(io) );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XBezierCurve::DoCopy(const XBase* rhs)
{
  const XBezierCurve* cb = rhs->HasFn<XBezierCurve>();
  XMD_ASSERT(cb);
  XParametricCurve::DoCopy(cb);
}
}
