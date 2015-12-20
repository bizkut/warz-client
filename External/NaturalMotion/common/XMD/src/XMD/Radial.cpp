//----------------------------------------------------------------------------------------------------------------------
/// \file Radial.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Radial.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XReal XRadial::GetRadialType() const
{
  return m_RadialType;
}

//----------------------------------------------------------------------------------------------------------------------
void XRadial::SetRadialType(const XReal t)
{
  m_RadialType = t;
}

//----------------------------------------------------------------------------------------------------------------------
XRadial::XRadial(XModel* pmod) 
  : XField(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XRadial::~XRadial()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XRadial::GetApiType() const
{
  return XFn::Radial;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XRadial::GetDataSize() const 
{
  return XField::GetDataSize() + sizeof(XReal) ;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XRadial::GetFn(XFn::Type type)
{
  if(XFn::Radial==type)
    return (XRadial*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XRadial::GetFn(XFn::Type type) const
{
  if(XFn::Radial==type)
    return (const XRadial*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XRadial::WriteChunk(std::ostream& ofs)
{
  if(!XField::WriteChunk(ofs))
    return false;

  ofs << "\tradial_type " << m_RadialType << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XRadial::DoData(XFileIO& io)
{
  DUMPER(XRadial);

  CHECK_IO( XField::DoData(io) );

  CHECK_IO( io.DoData( &m_RadialType ) );
  DPARAM(m_RadialType);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XRadial::DoCopy(const XBase* rhs)
{
  const XRadial* cb = rhs->HasFn<XRadial>();
  XMD_ASSERT(cb);
  m_RadialType = cb->m_RadialType;
  XField::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XRadial::ReadChunk(std::istream& ifs)
{
  if(!XField::ReadChunk(ifs))
    return false;

  READ_CHECK("radial_type",ifs);
  ifs >> m_RadialType;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XRadial::NodeDeath(XId id)
{
  return XField::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XRadial::PreDelete(XIdSet& extra_nodes)
{
  XField::PreDelete(extra_nodes);
}
}

