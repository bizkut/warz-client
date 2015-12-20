//----------------------------------------------------------------------------------------------------------------------
/// \file NormalConstraint.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/NormalConstraint.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XNormalConstraint::XNormalConstraint(XModel* pmod) 
  : XConstraint(pmod)
{
  m_Aim.set(1.0f,0,0);
  m_Up.set(0.0f,1.0f,0);
}

//----------------------------------------------------------------------------------------------------------------------
XNormalConstraint::~XNormalConstraint(void)
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XNormalConstraint::GetApiType() const
{
  return XFn::NormalConstraint;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XNormalConstraint::GetFn(XFn::Type type)
{
  if(XFn::NormalConstraint==type)
    return (XNormalConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XNormalConstraint::GetFn(XFn::Type type) const
{
  if(XFn::NormalConstraint==type)
    return (const XNormalConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNormalConstraint::NodeDeath(XId id)
{
  return XConstraint::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XNormalConstraint::PreDelete(XIdSet& extra_nodes)
{
  XConstraint::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNormalConstraint::ReadChunk(std::istream& ifs)
{  
  if (!XConstraint::ReadChunk(ifs)) 
    return false;

  READ_CHECK("aim_vector",ifs);
  ifs >> m_Aim;
  READ_CHECK("up_vector",ifs);
  ifs >> m_Up;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNormalConstraint::WriteChunk(std::ostream& ofs) 
{
  if (!XConstraint::WriteChunk(ofs))
    return false;

  ofs << "\taim_vector " << m_Aim << "\n";
  ofs << "\tup_vector " << m_Up << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNormalConstraint::DoData(XFileIO& io)
{
  DUMPER(XNormalConstraint);

  IO_CHECK( XConstraint::DoData(io) );

  io.DoData(&m_Aim);
  io.DoData(&m_Up);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XNormalConstraint::GetDataSize() const 
{
  return 6*sizeof(XReal) +  XConstraint::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XNormalConstraint::GetAimVector() const 
{
  return m_Aim; 
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XNormalConstraint::GetUpVector() const 
{
  return m_Up; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNormalConstraint::SetAimVector(const XVector3& v) 
{
  m_Aim=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNormalConstraint::SetUpVector(const XVector3& v) 
{
  m_Up=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XNormalConstraint::DoCopy(const XBase* rhs)
{
  const XNormalConstraint* cb = rhs->HasFn<XNormalConstraint>();
  XMD_ASSERT(cb);
  m_Aim = cb->m_Aim;
  m_Up = cb->m_Up;
  XConstraint::DoCopy(cb);
}
}
