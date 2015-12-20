//----------------------------------------------------------------------------------------------------------------------
/// \file HermiteCurve.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/HermiteCurve.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XHermiteCurve::XHermiteCurve(XModel* pmod) 
  : XParametricCurve(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XHermiteCurve::~XHermiteCurve(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XHermiteCurve::GetApiType() const
{
  return XFn::HermiteCurve;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XHermiteCurve::GetDataSize() const 
{
  return XParametricCurve::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XHermiteCurve::GetFn(XFn::Type type)
{
  if(XFn::HermiteCurve==type)
    return (XHermiteCurve*)this;
  return XParametricCurve::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XHermiteCurve::GetFn(XFn::Type type) const
{
  if(XFn::HermiteCurve==type)
    return (const XHermiteCurve*)this;
  return XParametricCurve::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XHermiteCurve::PreDelete(XIdSet& extra_nodes)
{
  XParametricCurve::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XHermiteCurve::NodeDeath(XId id)
{
  return XParametricCurve::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
bool XHermiteCurve::ReadChunk(std::istream& ifs)
{
  return XParametricCurve::ReadChunk(ifs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XHermiteCurve::WriteChunk(std::ostream& ofs)
{
  return XParametricCurve::WriteChunk(ofs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XHermiteCurve::DoData(XFileIO& io)
{
  DUMPER(XHermiteCurve);

  CHECK_IO( XParametricCurve::DoData(io) );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XHermiteCurve::DoCopy(const XBase* rhs)
{
  const XHermiteCurve* cb = rhs->HasFn<XHermiteCurve>();
  XMD_ASSERT(cb);
  XParametricCurve::DoCopy(cb);
}
}
