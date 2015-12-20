//----------------------------------------------------------------------------------------------------------------------
/// \file TangentConstraint.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/TangentConstraint.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XTangentConstraint::XTangentConstraint(XModel* pmod) 
  : XConstraint(pmod)
{
  m_Aim.set(1.0f,0,0);
  m_Up.set(0.0f,1.0f,0);
}

//----------------------------------------------------------------------------------------------------------------------
XTangentConstraint::~XTangentConstraint()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XTangentConstraint::GetApiType() const
{
  return XFn::TangentConstraint;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XTangentConstraint::GetFn(XFn::Type type)
{
  if(XFn::TangentConstraint==type)
    return (XTangentConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XTangentConstraint::GetFn(XFn::Type type) const
{
  if(XFn::TangentConstraint==type)
    return (const XTangentConstraint*)this;
  return XConstraint::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTangentConstraint::NodeDeath(XId id)
{
  return XBone::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XTangentConstraint::PreDelete(XIdSet& extra_nodes)
{
  XConstraint::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTangentConstraint::ReadChunk(std::istream& ifs)
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
bool XTangentConstraint::WriteChunk(std::ostream& ofs) 
{
  if (!XConstraint::WriteChunk(ofs))
    return false;

  ofs << "\taim_vector " << m_Aim << "\n";
  ofs << "\tup_vector " << m_Up << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XTangentConstraint::DoData(XFileIO& io) 
{
  DUMPER(XTangentConstraint);

  IO_CHECK( XConstraint::DoData(io) );

  io.DoData(&m_Aim);
  io.DoData(&m_Up);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XTangentConstraint::GetDataSize() const
{
  return 2*3*sizeof(XReal) + XConstraint::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XTangentConstraint::GetAimVector() const 
{
  return m_Aim; 
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XTangentConstraint::GetUpVector() const 
{
  return m_Up; 
}

//----------------------------------------------------------------------------------------------------------------------
void XTangentConstraint::SetAimVector(const XVector3& v) 
{
  m_Aim=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XTangentConstraint::SetUpVector(const XVector3& v) 
{
  m_Up=v; 
}

//----------------------------------------------------------------------------------------------------------------------
void XTangentConstraint::DoCopy(const XBase* rhs)
{
  const XTangentConstraint* cb = rhs->HasFn<XTangentConstraint>();
  XMD_ASSERT(cb);
  m_Aim = cb->m_Aim;
  m_Up = cb->m_Up;
  XConstraint::DoCopy(cb);
}
}
