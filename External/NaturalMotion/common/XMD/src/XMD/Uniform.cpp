//----------------------------------------------------------------------------------------------------------------------
/// \file Uniform.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Uniform.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XUniform::XUniform(XModel* pmod) 
  : XField(pmod),
  m_Direction(0,0,0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XUniform::~XUniform()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XUniform::GetApiType() const
{
  return XFn::Uniform;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XUniform::GetFn(XFn::Type type)
{
  if(XFn::Uniform==type)
    return (XUniform*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XUniform::GetFn(XFn::Type type) const
{
  if(XFn::Uniform==type)
    return (const XUniform*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XUniform::GetDirection() const
{
  return m_Direction;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XUniform::GetDataSize() const
{
  return XField::GetDataSize() + 3*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
void XUniform::SetDirection(const XVector3& v) 
{
  m_Direction = v;
}

//----------------------------------------------------------------------------------------------------------------------
bool XUniform::WriteChunk(std::ostream& ofs)
{
  if(!XField::WriteChunk(ofs))
    return false;

  ofs << "\tdirection " << m_Direction << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XUniform::DoData(XFileIO& io)
{
  DUMPER(XUniform);

  CHECK_IO( XField::DoData(io) );

  CHECK_IO( io.DoData( &m_Direction ) );
  DPARAM(m_Direction);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XUniform::DoCopy(const XBase* rhs)
{
  const XUniform* cb = rhs->HasFn<XUniform>();
  XMD_ASSERT(cb);
  m_Direction = cb->m_Direction;
  XField::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XUniform::ReadChunk(std::istream& ifs)
{
  if(!XField::ReadChunk(ifs))
    return false;

  READ_CHECK("direction",ifs);
  ifs >> m_Direction;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XUniform::NodeDeath(XId id)
{
  return XField::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XUniform::PreDelete(XIdSet& extra_nodes)
{
  XField::PreDelete(extra_nodes);
}
}

