//----------------------------------------------------------------------------------------------------------------------
/// \file     OrientConstraint.cpp
/// \note    (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/OrientConstraint.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XOrientConstraint::XOrientConstraint(XModel* pmod) 
  : XConstraint(pmod)
{
  m_Offset.set(0,0,0);
}

//----------------------------------------------------------------------------------------------------------------------
XOrientConstraint::~XOrientConstraint()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XOrientConstraint::GetApiType() const
{
  return XFn::OrientConstraint;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XOrientConstraint::GetFn(XFn::Type type)
{
  if(XFn::OrientConstraint==type)
    return (XOrientConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XOrientConstraint::GetFn(XFn::Type type) const
{
  if(XFn::OrientConstraint==type)
    return (const XOrientConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XOrientConstraint::NodeDeath(XId id)
{
  return XConstraint::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XOrientConstraint::PreDelete(XIdSet& extra_nodes)
{
  XConstraint::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XOrientConstraint::ReadChunk(std::istream& ifs)
{  
  if (!XConstraint::ReadChunk(ifs)) 
    return false;

  READ_CHECK("offset",ifs);
  ifs >> m_Offset;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XOrientConstraint::WriteChunk(std::ostream& ofs) 
{
  if (!XConstraint::WriteChunk(ofs))
    return false;

  ofs << "\toffset " << m_Offset << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XOrientConstraint::DoData(XFileIO& io) 
{
  DUMPER(XOrientConstraint);

  IO_CHECK( XConstraint::DoData(io) );

  io.DoData(&m_Offset);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XOrientConstraint::GetDataSize() const 
{
  return 3*sizeof(XReal) + XConstraint::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XOrientConstraint::GetOffset() const 
{ 
  return m_Offset; 
}
//----------------------------------------------------------------------------------------------------------------------
void XOrientConstraint::SetOffset(const XVector3& v) 
{ 
  m_Offset=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XOrientConstraint::DoCopy(const XBase* rhs)
{
  const XOrientConstraint* cb = rhs->HasFn<XOrientConstraint>();
  XMD_ASSERT(cb);
  m_Offset = cb->m_Offset;
  XConstraint::DoCopy(cb);
}
}
