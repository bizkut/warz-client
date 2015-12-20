//----------------------------------------------------------------------------------------------------------------------
/// \file PointConstraint.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/PoleVectorConstraint.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XPoleVectorConstraint::XPoleVectorConstraint(XModel* pmod) 
  : XPointConstraint(pmod)
{
  m_PivotSpace.identity();
}

//----------------------------------------------------------------------------------------------------------------------
XPoleVectorConstraint::~XPoleVectorConstraint()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XPoleVectorConstraint::GetApiType() const
{
  return XFn::PoleVectorConstraint;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XPoleVectorConstraint::GetFn(XFn::Type type)
{
  if(XFn::PoleVectorConstraint==type)
    return (XPoleVectorConstraint*)this;
  return XPointConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XPoleVectorConstraint::GetFn(XFn::Type type) const
{
  if(XFn::PoleVectorConstraint==type)
    return (const XPoleVectorConstraint*)this;
  return XPointConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoleVectorConstraint::NodeDeath(XId id)
{
  return XPointConstraint::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XPoleVectorConstraint::PreDelete(XIdSet& extra_nodes)
{
  XPointConstraint::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoleVectorConstraint::ReadChunk(std::istream& ifs)
{  
  if (!XPointConstraint::ReadChunk(ifs)) 
    return false;

  READ_CHECK("pivot_space",ifs);
  ifs >> m_PivotSpace;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoleVectorConstraint::WriteChunk(std::ostream& ofs) 
{
  if (!XPointConstraint::WriteChunk(ofs))
    return false;

  ofs << "\tpivot_space\n"
      << "\t\t" << m_PivotSpace.data[ 0] << " " << m_PivotSpace.data[ 1] << " " << m_PivotSpace.data[ 2] << " " << m_PivotSpace.data[ 3] << "\n"
      << "\t\t" << m_PivotSpace.data[ 4] << " " << m_PivotSpace.data[ 5] << " " << m_PivotSpace.data[ 6] << " " << m_PivotSpace.data[ 7] << "\n"
      << "\t\t" << m_PivotSpace.data[ 8] << " " << m_PivotSpace.data[ 9] << " " << m_PivotSpace.data[10] << " " << m_PivotSpace.data[11] << "\n"
      << "\t\t" << m_PivotSpace.data[12] << " " << m_PivotSpace.data[13] << " " << m_PivotSpace.data[14] << " " << m_PivotSpace.data[15] << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XPoleVectorConstraint::DoData(XFileIO& io) 
{
  DUMPER(XPoleVectorConstraint);

  IO_CHECK( XPointConstraint::DoData(io) );

  io.DoData(&m_PivotSpace);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XPoleVectorConstraint::GetDataSize() const 
{
  return sizeof(XMatrix) + XPointConstraint::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
const XMatrix& XPoleVectorConstraint::GetPivotSpace() const
{
  return m_PivotSpace; 
}

//----------------------------------------------------------------------------------------------------------------------
void XPoleVectorConstraint::SetPivotSpace(const XMatrix& v) 
{
  m_PivotSpace=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XPoleVectorConstraint::DoCopy(const XBase* rhs)
{
  const XPoleVectorConstraint* cb = rhs->HasFn<XPoleVectorConstraint>();
  XMD_ASSERT(cb);
  m_PivotSpace = cb->m_PivotSpace;
  XPointConstraint::DoCopy(cb);
}
}
