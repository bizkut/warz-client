//----------------------------------------------------------------------------------------------------------------------
/// \file EnvmapBaseTexture.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/EnvmapBaseTexture.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"
#include <math.h>

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XEnvmapBaseTexture::XEnvmapBaseTexture(XModel* pmod)
  : XTexture(pmod) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XEnvmapBaseTexture::~XEnvmapBaseTexture() 
{
}
//----------------------------------------------------------------------------------------------------------------------
bool XEnvmapBaseTexture::WriteChunk(std::ostream& ofs) 
{
  return XTexture::WriteChunk(ofs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XEnvmapBaseTexture::ReadChunk(std::istream& ifs) 
{
  return XTexture::ReadChunk(ifs);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XEnvmapBaseTexture::GetApiType() const 
{
  return XFn::EnvMap;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XEnvmapBaseTexture::GetFn(XFn::Type type)
{
  if(type==EnvMap) 
  {
    return (XEnvmapBaseTexture*)this;
  }
  return XTexture::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XEnvmapBaseTexture::GetFn(XFn::Type type) const
{
  if(XFn::EnvMap==type)
    return (const XEnvmapBaseTexture*)this;
  return XTexture::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XEnvmapBaseTexture::DoData(XFileIO& io)
{
  DUMPER(XEnvmapBaseTexture);

  IO_CHECK( XTexture::DoData(io) );

  IO_CHECK( io.DoData(&m_Placement) );
  DPARAM( m_Placement );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XEnvmapBaseTexture::DoCopy(const XBase* rhs)
{
  const XEnvmapBaseTexture* cb = rhs->HasFn<XEnvmapBaseTexture>();
  XMD_ASSERT(cb);

  m_Placement = cb->m_Placement;

  XTexture::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XEnvmapBaseTexture::GetDataSize() const
{
  return XTexture::GetDataSize() + sizeof(XU32) ;
}

//----------------------------------------------------------------------------------------------------------------------
void XEnvmapBaseTexture::PreDelete(XIdSet& extra_nodes)
{
  XTexture::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XEnvmapBaseTexture::NodeDeath(XId id) 
{
  if(m_Placement==id) 
  {
    m_Placement=0;
    return true;
  }
  return XTexture::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement3D* XEnvmapBaseTexture::GetPlacement3D() const
{
  XBase* ptr = m_pModel->FindNode( m_Placement );
  if(ptr)
    return ptr->HasFn<XTexPlacement3D>();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XEnvmapBaseTexture::SetPlacement3D(const XBase* ptr)
{
  if(!ptr)
  {
    m_Placement = 0;
    return true;
  }

  // if you assert here, the 3D texture placement node does not belong to
  // the same XModel as this environment map node
  XMD_ASSERT(IsValidObj(ptr));

  if(ptr->HasFn<XTexPlacement3D>())
  {
    m_Placement = ptr->GetID();
    return true;
  }

  // if you assert here, you have not provided 3D texture placement node as 
  // the argument to this function
  XMD_ASSERT(0);

  return false;
}
}
