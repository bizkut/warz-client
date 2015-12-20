//----------------------------------------------------------------------------------------------------------------------
///  \file    AimConstraint.cpp
/// \note    (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/AimConstraint.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XAimConstraint::XAimConstraint(XModel* pmod) 
  : XConstraint(pmod)
{
  m_Aim.set(1.0f,0.0f,0.0f);
  m_Up.set(0,1.0f,0);
  m_Offset.set(0,0,0);
}

//----------------------------------------------------------------------------------------------------------------------
XAimConstraint::~XAimConstraint()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XAimConstraint::GetApiType() const
{
  return XFn::AimConstraint;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XAimConstraint::GetFn(XFn::Type type)
{
  if(XFn::AimConstraint==type)
    return (XAimConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XAimConstraint::GetFn(XFn::Type type) const
{
  if(XFn::AimConstraint==type)
    return (const XAimConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAimConstraint::NodeDeath(XId id)
{
  return XConstraint::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XAimConstraint::PreDelete(XIdSet& extra_nodes)
{
  XConstraint::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XAimConstraint::ReadChunk(std::istream& ifs)
{  
  if (!XConstraint::ReadChunk(ifs)) 
    return false;

  READ_CHECK("aim_vector",ifs);
  ifs >> m_Aim;
  READ_CHECK("up_vector",ifs);
  ifs >> m_Up;
  READ_CHECK("offset",ifs);
  ifs >> m_Offset;
  
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAimConstraint::WriteChunk(std::ostream& ofs) 
{
  if (!XConstraint::WriteChunk(ofs))
    return false;

  ofs << "\taim_vector " << m_Aim << "\n";
  ofs << "\tup_vector " << m_Up << "\n";
  ofs << "\toffset " << m_Offset << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XAimConstraint::DoData(XFileIO& io)
{
  DUMPER(XAimConstraint);

  IO_CHECK( XConstraint::DoData(io) );

  io.DoData(&m_Aim);
  io.DoData(&m_Up);
  io.DoData(&m_Offset);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XAimConstraint::GetDataSize() const
{
  return 9*sizeof(XReal) + XConstraint::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XAimConstraint::GetAimVector() const 
{ 
  return m_Aim; 
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XAimConstraint::GetUpVector() const 
{
  return m_Up;
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XAimConstraint::GetOffset() const
{ 
  return m_Offset; 
}

//----------------------------------------------------------------------------------------------------------------------
void XAimConstraint::SetAimVector(const XVector3& v)
{ 
  m_Aim=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XAimConstraint::SetUpVector(const XVector3& v)
{ 
  m_Up=v;
}

//----------------------------------------------------------------------------------------------------------------------
void XAimConstraint::SetOffset(const XVector3& v)
{
  m_Offset=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XAimConstraint::DoCopy(const XBase* rhs)
{
  const XAimConstraint* cb = rhs->HasFn<XAimConstraint>();
  XMD_ASSERT(cb);
  m_Aim = cb->m_Aim;
  m_Up = cb->m_Up;
  m_Offset = cb->m_Offset;
  XConstraint::DoCopy(cb);
}
}
