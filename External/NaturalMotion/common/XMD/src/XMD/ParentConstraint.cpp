//----------------------------------------------------------------------------------------------------------------------
/// \file ParentConstraint.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/ParentConstraint.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XParentConstraint::XParentConstraint(XModel* pmod) 
  : XConstraint(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XParentConstraint::~XParentConstraint()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XParentConstraint::GetApiType() const
{
  return XFn::ParentConstraint;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XParentConstraint::GetFn(XFn::Type type)
{
  if(XFn::ParentConstraint==type)
    return (XParentConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XParentConstraint::GetFn(XFn::Type type) const
{
  if(XFn::ParentConstraint==type)
    return (const XParentConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XParentConstraint::NodeDeath(XId id)
{
  return XConstraint::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XParentConstraint::PreDelete(XIdSet& extra_nodes)
{
  XConstraint::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XParentConstraint::ReadChunk(std::istream& ifs)
{  
  if (!XConstraint::ReadChunk(ifs)) 
    return false;
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XParentConstraint::WriteChunk(std::ostream& ofs) 
{
  if (!XConstraint::WriteChunk(ofs))
    return false;
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XParentConstraint::DoData(XFileIO& io)
{
  DUMPER(XParentConstraint);

  IO_CHECK( XConstraint::DoData(io) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XParentConstraint::GetDataSize() const 
{
  return XConstraint::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
void XParentConstraint::DoCopy(const XBase* rhs)
{
  const XParentConstraint* cb = rhs->HasFn<XParentConstraint>();
  XMD_ASSERT(cb);
  XConstraint::DoCopy(cb);
}
}
