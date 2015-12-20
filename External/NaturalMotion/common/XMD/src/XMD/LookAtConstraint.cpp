//----------------------------------------------------------------------------------------------------------------------
/// \file LookAtConstraint.cpp
/// \note (C) Copyright 2003-2009 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/LookAtConstraint.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XLookAtConstraint::XLookAtConstraint(XModel* pmod) 
  : XAimConstraint(pmod)
{
  m_DistanceBetween=0;
  m_Twist=0;
}

//----------------------------------------------------------------------------------------------------------------------
XLookAtConstraint::~XLookAtConstraint()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XLookAtConstraint::GetApiType() const
{
  return XFn::LookAtConstraint;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XLookAtConstraint::GetFn(XFn::Type type)
{
  if(XFn::LookAtConstraint==type)
    return (XLookAtConstraint*)this;
  return XAimConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XLookAtConstraint::GetFn(XFn::Type type) const
{
  if(XFn::LookAtConstraint==type)
    return (const XLookAtConstraint*)this;
  return XAimConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XLookAtConstraint::NodeDeath(XId id)
{
  return XAimConstraint::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XLookAtConstraint::PreDelete(XIdSet& extra_nodes)
{
  XAimConstraint::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XLookAtConstraint::ReadChunk(std::istream& ifs)
{  
  if (!XAimConstraint::ReadChunk(ifs)) 
    return false;

  READ_CHECK("distance_between",ifs);
  ifs >> m_DistanceBetween;
  READ_CHECK("twist",ifs);
  ifs >> m_Twist;
  
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XLookAtConstraint::WriteChunk(std::ostream& ofs) 
{
  if (!XAimConstraint::WriteChunk(ofs))
    return false;

  ofs << "\tdistance_between " << m_DistanceBetween << "\n";
  ofs << "\ttwist " << m_Twist << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XLookAtConstraint::DoData(XFileIO& io)
{
  DUMPER(XLookAtConstraint);

  IO_CHECK( XAimConstraint::DoData(io) );

  io.DoData(&m_DistanceBetween);
  io.DoData(&m_Twist);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XLookAtConstraint::GetDataSize() const
{
  return 2*sizeof(XReal) + XAimConstraint::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XReal XLookAtConstraint::GetDistanceBetween() const 
{ 
  return m_DistanceBetween; 
}

//----------------------------------------------------------------------------------------------------------------------
XReal XLookAtConstraint::GetTwist() const 
{
  return m_Twist;
}

//----------------------------------------------------------------------------------------------------------------------
void XLookAtConstraint::SetDistanceBetween(XReal v)
{ 
  m_DistanceBetween=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XLookAtConstraint::SetTwist(XReal v)
{ 
  m_Twist=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XLookAtConstraint::DoCopy(const XBase* rhs)
{
  const XLookAtConstraint* cb = rhs->HasFn<XLookAtConstraint>();
  XMD_ASSERT(cb);
  m_DistanceBetween = cb->m_DistanceBetween;
  m_Twist = cb->m_Twist;
  XAimConstraint::DoCopy(cb);
}
}
