//----------------------------------------------------------------------------------------------------------------------
/// \file     ScaleConstraint.cpp
/// \note    (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/ScaleConstraint.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XScaleConstraint::XScaleConstraint(XModel* pmod) 
  : XConstraint(pmod)
{
  m_Offset.set(0,0,0);
}

//----------------------------------------------------------------------------------------------------------------------
XScaleConstraint::~XScaleConstraint()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XScaleConstraint::GetApiType() const
{
  return XFn::ScaleConstraint;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XScaleConstraint::GetFn(XFn::Type type)
{
  if(XFn::ScaleConstraint==type)
    return (XScaleConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XScaleConstraint::GetFn(XFn::Type type) const
{
  if(XFn::ScaleConstraint==type)
    return (const XScaleConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XScaleConstraint::NodeDeath(XId id)
{
  return XBone::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XScaleConstraint::PreDelete(XIdSet& extra_nodes)
{
  XConstraint::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XScaleConstraint::ReadChunk(std::istream& ifs)
{  
  if (!XConstraint::ReadChunk(ifs)) 
    return false;

  READ_CHECK("offset",ifs);
  ifs >> m_Offset;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XScaleConstraint::WriteChunk(std::ostream& ofs) 
{
  if (!XConstraint::WriteChunk(ofs))
    return false;

  ofs << "\toffset " << m_Offset << std::endl;

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XScaleConstraint::DoData(XFileIO& io) 
{
  DUMPER(XScaleConstraint);

  IO_CHECK( XConstraint::DoData(io) );

  IO_CHECK( io.DoData(&m_Offset) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XScaleConstraint::GetDataSize() const 
{
  return 3*sizeof(XReal) + XConstraint::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XScaleConstraint::GetOffset() const 
{
  return m_Offset; 
}

//----------------------------------------------------------------------------------------------------------------------
void XScaleConstraint::SetOffset(const XVector3& v) 
{
  m_Offset=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XScaleConstraint::DoCopy(const XBase* rhs)
{
  const XScaleConstraint* cb = rhs->HasFn<XScaleConstraint>();
  XMD_ASSERT(cb);
  m_Offset = cb->m_Offset;
  XConstraint::DoCopy(cb);
}
}
