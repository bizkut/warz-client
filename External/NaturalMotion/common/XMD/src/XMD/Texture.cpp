//----------------------------------------------------------------------------------------------------------------------
/// \file Texture.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Texture.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"
#include <math.h>

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XTexture::XTexture(XModel* pm)
  : XShadingNode(pm),m_PlacementIDs()
{
}

//----------------------------------------------------------------------------------------------------------------------
XTexture::~XTexture()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XTexture::GetApiType() const 
{
  return XFn::Texture;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XTexture::GetNumPlacements() const
{
  return static_cast<XU32>(m_PlacementIDs.size());
}

//----------------------------------------------------------------------------------------------------------------------
XTexPlacement2D* XTexture::GetPlacement(const XU32 idx) const
{
  if(idx>=m_PlacementIDs.size())
    return 0;
  XBase* ptr = m_pModel->FindNode(m_PlacementIDs[idx]);
  if(!ptr)
    return 0;
  return ptr->HasFn<XTexPlacement2D>();
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexture::SetPlacement(const XBase* base,const XU32 idx) 
{
  // if you assert here you have provided an invalid index for the texture placement
  XMD_ASSERT(idx < m_PlacementIDs.size());
  if(idx>=m_PlacementIDs.size())
    return false;

  if (!base)
  {
    m_PlacementIDs[idx] = 0;
    return true;
  }

  // if you assert here, base is NULL or not from this XModel
  XMD_ASSERT( IsValidObj(base) );
  if(base->HasFn<XTexPlacement2D>()) 
  {
    m_PlacementIDs[idx] = base->GetID();
    return true;
  }

  // if you assert here, base is not a texture placement node
  XMD_ASSERT(0);
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XTexture::GetFn(XFn::Type type) 
{
  if(type == XFn::Texture)
  {
    return (XTexture*)this;
  }
  return XShadingNode::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexture::WriteChunk(std::ostream& ofs) 
{
  return XShadingNode::WriteChunk(ofs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexture::ReadChunk(std::istream& ifs) 
{
  return XShadingNode::ReadChunk(ifs);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XTexture::GetFn(XFn::Type type) const
{
  if(XFn::Texture==type)
    return (const XTexture*)this;
  return XShadingNode::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexture::DoData(XFileIO& io)
{
  DUMPER(XTexture);

  IO_CHECK( XShadingNode::DoData(io) );

  IO_CHECK( io.DoDataVector(m_PlacementIDs) );
  DAPARAM( m_PlacementIDs );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexture::DoCopy(const XBase* rhs)
{
  const XTexture* cb = rhs->HasFn<XTexture>();
  XMD_ASSERT(cb);
  if(cb)
  {
    m_PlacementIDs = cb->m_PlacementIDs;

    XShadingNode::DoCopy(cb);
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XTexture::GetDataSize() const
{
  size_t sz = XShadingNode::GetDataSize() + sizeof(XU32) + sizeof(XId) * m_PlacementIDs.size();
  return (XU32)sz;
}

//----------------------------------------------------------------------------------------------------------------------
void XTexture::PreDelete(XIdSet& extra_nodes)
{
  XShadingNode::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexture::NodeDeath(XId id) 
{
  XIndexList::iterator it = m_PlacementIDs.begin();
  for( ; it != m_PlacementIDs.end(); ++it )
  {
    if(*it == id)
    {
      *it = 0;
      break;
    }
  }
  return XShadingNode::NodeDeath(id);
}
}

