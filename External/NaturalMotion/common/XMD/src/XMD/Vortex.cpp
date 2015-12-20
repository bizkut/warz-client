//----------------------------------------------------------------------------------------------------------------------
/// \file Vortex.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Vortex.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XVortex::XVortex(XModel* pmod) 
  : XField(pmod)
{
}

//----------------------------------------------------------------------------------------------------------------------
XVortex::~XVortex()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XVortex::GetApiType() const
{
  return XFn::Vortex;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XVortex::GetDataSize() const 
{
  return XField::GetDataSize() + 3*sizeof(XReal);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XVortex::GetFn(XFn::Type type)
{
  if(XFn::Vortex==type)
    return (XVortex*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XVortex::GetFn(XFn::Type type) const
{
  if(XFn::Vortex==type)
    return (const XVortex*)this;
  return XField::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVortex::WriteChunk(std::ostream& ofs)
{
  if(!XField::WriteChunk(ofs))
    return false;

  /// the camera's aspect ratio
  ofs << "\taxis " << m_Axis << "\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVortex::DoData(XFileIO& io)
{
  DUMPER(XVortex);

  CHECK_IO( XField::DoData(io) );

  CHECK_IO( io.DoData( &m_Axis ) );
  DPARAM(m_Axis);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XVortex::DoCopy(const XBase* rhs)
{
  const XVortex* cb = rhs->HasFn<XVortex>();
  XMD_ASSERT(cb);
  m_Axis = cb->m_Axis;
  XField::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVortex::ReadChunk(std::istream& ifs)
{
  if(!XField::ReadChunk(ifs))
    return false;

  READ_CHECK("axis",ifs);
  ifs >> m_Axis;

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XVortex::NodeDeath(XId id)
{
  return XField::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XVortex::GetAxis() const 
{
  return m_Axis;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVortex::SetAxis(const XVector3& val)  
{
  m_Axis = val;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XVortex::PreDelete(XIdSet& extra_nodes)
{
  XField::PreDelete(extra_nodes);
}
}
