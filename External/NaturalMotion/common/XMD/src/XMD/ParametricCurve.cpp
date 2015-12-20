//----------------------------------------------------------------------------------------------------------------------
/// \file ParametricCurve.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/ParametricCurve.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XParametricCurve::XParametricCurve(XModel* pmod)
  : XParametricBase(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XParametricCurve::~XParametricCurve()
{
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XParametricCurve::GetCurveType() const
{
  return m_CurveType;
}

//----------------------------------------------------------------------------------------------------------------------
void XParametricCurve::SetCurveType(const XU32 type)
{
  if (type<3)
  {
    m_CurveType = type;
  }
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XParametricCurve::GetApiType() const 
{
  return XFn::ParametricCurve;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XParametricCurve::GetFn(XFn::Type type) 
{
  if (type == XFn::ParametricCurve)
    return (XParametricCurve*)this;
  return XParametricBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XParametricCurve::GetFn(XFn::Type type) const
{
  if(XFn::ParametricCurve==type)
    return (const XParametricCurve*)this;
  return XParametricBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XParametricCurve::WriteChunk(std::ostream& ofs) 
{
  if(!XParametricBase::WriteChunk(ofs))
    return false;

  ofs << "\tcurve_type " << m_CurveType << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XParametricCurve::ReadChunk(std::istream& ifs)
{
  if(!XParametricBase::ReadChunk(ifs))
    return false;

  if(GetModel()->GetInputVersion()>=4)
  {
    READ_CHECK("curve_type",ifs);
    ifs >> m_CurveType;
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
void XParametricCurve::PreDelete(XIdSet& extra_nodes)
{
  XParametricBase::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XParametricCurve::NodeDeath(XId id)
{
  return XParametricBase::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
bool XParametricCurve::DoData(XFileIO& io)
{
  DUMPER(XParametricCurve);

  CHECK_IO( XParametricBase::DoData(io) );

  if (GetModel()->GetInputVersion()>=4)
  {
    CHECK_IO( io.DoData(&m_CurveType,1) );
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XParametricCurve::DoCopy(const XBase* rhs)
{
  const XParametricCurve* cb = rhs->HasFn<XParametricCurve>();
  XMD_ASSERT(cb);
  XParametricBase::DoCopy(cb);

  m_CurveType = cb->m_CurveType;
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU32 XParametricCurve::GetDataSize() const
{
  return sizeof(XU32) + XParametricBase::GetDataSize();
}
}
