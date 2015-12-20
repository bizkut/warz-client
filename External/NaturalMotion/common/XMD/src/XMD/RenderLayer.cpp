//----------------------------------------------------------------------------------------------------------------------
/// \file RenderLayer.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Model.h"
#include "XMD/RenderLayer.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XRenderLayer::XRenderLayer(XModel* pModel)
  : XNodeCollection(pModel)
{
}

//----------------------------------------------------------------------------------------------------------------------
XRenderLayer::~XRenderLayer()
{
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU32 XRenderLayer::GetDataSize() const
{
  return XNodeCollection::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XRenderLayer::ReadChunk(std::istream& ifs)
{
  return XNodeCollection::ReadChunk(ifs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XRenderLayer::WriteChunk(std::ostream& ofs)
{
  return XNodeCollection::WriteChunk(ofs);
}

//----------------------------------------------------------------------------------------------------------------------
void XRenderLayer::PreDelete(XIdSet& extra_nodes)
{
  XNodeCollection::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XRenderLayer::NodeDeath(XId id) 
{
  return XNodeCollection::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
bool XRenderLayer::DoData(XFileIO& io)
{
  return XNodeCollection::DoData(io);
}

//----------------------------------------------------------------------------------------------------------------------
void XRenderLayer::DoCopy(const XBase* rhs)
{
  return XNodeCollection::DoCopy(rhs);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XRenderLayer::GetApiType() const 
{
  return XFn::RenderLayer;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XRenderLayer::GetFn(XFn::Type type) 
{
  if(XFn::RenderLayer == type) 
  {
    return (XRenderLayer*)this;
  }
  return XNodeCollection::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XRenderLayer::GetFn(XFn::Type type) const
{
  if(XFn::RenderLayer==type)
    return (const XRenderLayer*)this;
  return XNodeCollection::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XRenderLayer::GetItems(XBoneList& items) const
{
  items.clear();

  XIndexList::const_iterator it = m_Objects.begin();
  for(;it != m_Objects.end();++it)
  {
    XBase* base = m_pModel->FindNode((*it));
    if(base)
    {
      XBone* bone = base->HasFn<XBone>();
      if(bone)
        items.push_back( bone );
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XRenderLayer::SetItems(const XBoneList& items)
{
  m_Objects.clear();

  XBoneList::const_iterator it = items.begin();
  for(;it != items.end();++it)
  {
    if(!IsItem((*it))) 
    {
      m_Objects.push_back( (*it)->GetID() );
    }
  }
}
}
