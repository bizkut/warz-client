//----------------------------------------------------------------------------------------------------------------------
/// \file DisplayLayer.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/DisplayLayer.h"
#include "XMD/Model.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XDisplayLayer::XDisplayLayer(XModel* pModel)
  : XNodeCollection(pModel)
{
}

//----------------------------------------------------------------------------------------------------------------------
XDisplayLayer::~XDisplayLayer()
{
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU32 XDisplayLayer::GetDataSize() const
{
  return XNodeCollection::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XDisplayLayer::ReadChunk(std::istream& ifs)
{
  return XNodeCollection::ReadChunk(ifs);
}

//----------------------------------------------------------------------------------------------------------------------
bool XDisplayLayer::WriteChunk(std::ostream& ofs)
{
  return XNodeCollection::WriteChunk(ofs);
}

//----------------------------------------------------------------------------------------------------------------------
void XDisplayLayer::PreDelete(XIdSet& extra_nodes)
{
  XNodeCollection::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XDisplayLayer::NodeDeath(XId id) 
{
  return XNodeCollection::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
bool XDisplayLayer::DoData(XFileIO& io)
{
  return XNodeCollection::DoData(io);
}

//----------------------------------------------------------------------------------------------------------------------
void XDisplayLayer::DoCopy(const XBase* rhs)
{
  return XNodeCollection::DoCopy(rhs);
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XDisplayLayer::GetApiType() const 
{
  return XFn::DisplayLayer;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XDisplayLayer::GetFn(XFn::Type type) 
{
  if (XFn::DisplayLayer == type)
  {
    return (XDisplayLayer*)this;
  }
  return XNodeCollection::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XDisplayLayer::GetFn(XFn::Type type) const
{
  if(XFn::DisplayLayer==type)
    return (const XDisplayLayer*)this;
  return XNodeCollection::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XDisplayLayer::GetItems(XBoneList& items) const 
{
  items.clear();

  XIndexList::const_iterator it = m_Objects.begin();
  for( ; it != m_Objects.end(); ++it )
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
void XDisplayLayer::SetItems(const XBoneList& items)
{
  m_Objects.clear();

  XBoneList::const_iterator it = items.begin();
  for( ; it != items.end(); ++it )
  {
    if( !IsItem((*it)) ) 
    {
      m_Objects.push_back( (*it)->GetID() );
    }
  }
}
}
