//----------------------------------------------------------------------------------------------------------------------
/// \file NodeCollection.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/NodeCollection.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XNodeCollection::XNodeCollection(XModel* pModel)
  : XBase(pModel), m_Objects()
{
}

//----------------------------------------------------------------------------------------------------------------------
XNodeCollection::~XNodeCollection()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XNodeCollection::GetApiType() const 
{
  return XFn::NodeCollection;
}

//----------------------------------------------------------------------------------------------------------------------
void XNodeCollection::DoCopy(const XBase* rhs)
{
  const XNodeCollection* cb = rhs->HasFn<XNodeCollection>();
  XMD_ASSERT(cb);
  m_Objects = cb->m_Objects;
  XBase::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XNodeCollection::GetItem(const XU32 idx) const
{
  if (idx<m_Objects.size()) 
  {
    return m_pModel->FindNode(m_Objects[idx]);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XNodeCollection::GetNumItems() const 
{
  return static_cast<XU32>( m_Objects.size() );
}

//----------------------------------------------------------------------------------------------------------------------
bool XNodeCollection::IsItem(const XBase* ptr) const
{
  if (!ptr) 
    return false;

  XIndexList::const_iterator it = m_Objects.begin();
  for( ; it != m_Objects.end(); ++it )
  {
    if (*it == ptr->GetID())
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XNodeCollection::AddItem(const XBase* ptr)
{
  // if you assert here, ptr is NULL or not from this XModel
  XMD_ASSERT( (ptr) && IsValidObj(ptr) );

  if ( ptr->HasFn<XBone>() ) 
  {
    if( !IsItem(ptr) ) 
    {
      m_Objects.push_back( ptr->GetID() );
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XNodeCollection::DeleteItem(const XBase* ptr)
{
  // if you assert here, ptr is NULL or not from this XModel
  XMD_ASSERT( (ptr) && IsValidObj(ptr) );
  if (!ptr) 
    return false;

  XIndexList::iterator it = m_Objects.begin();
  for( ; it != m_Objects.end(); ++it )
  {
    if (*it == ptr->GetID())
    {
      m_Objects.erase(it);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XNodeCollection::DeleteItem(const XU32 idx)
{
  if( m_Objects.size() > idx )
  {
    m_Objects.erase(m_Objects.begin() + idx);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XNodeCollection::GetFn(XFn::Type type) 
{
  if (XFn::NodeCollection == type)
  {
    return (XNodeCollection*)this;
  }
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XNodeCollection::GetFn(XFn::Type type) const
{
  if(XFn::NodeCollection==type)
    return (const XNodeCollection*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNodeCollection::NodeDeath(XId id)
{
  XIndexList::iterator it = m_Objects.begin();
  for(;it!=m_Objects.end();++it)
  {
    if (*it == id) 
    {
      m_Objects.erase(it);
      break;
    }
  }
  return XBase::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XNodeCollection::PreDelete(XIdSet& extra_nodes)
{
  XBase::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XNodeCollection::ReadChunk(std::istream& ifs)
{
  READ_CHECK("num_items",ifs);
  XU32 num=0;
  ifs >> num;

  m_Objects.resize(num);

  XIndexList::iterator it = m_Objects.begin();
  for(;it != m_Objects.end();++it)
  {
    ifs >> *it;
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XNodeCollection::WriteChunk(std::ostream& ofs)
{
  ofs << "\tnum_items " << static_cast<XU32>(m_Objects.size()) << "\n";

  XIndexList::const_iterator it = m_Objects.begin();
  for(;it != m_Objects.end();++it)
  {
    ofs << "\t\t" << *it << "\n";
  }

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XNodeCollection::GetDataSize() const
{
  size_t sz = XBase::GetDataSize() + sizeof(XU32) + m_Objects.size() * sizeof(XU32);
  return (XU32)sz;
}

//----------------------------------------------------------------------------------------------------------------------
bool XNodeCollection::DoData(XFileIO& io) 
{
  DUMPER(XNodeCollection);

  IO_CHECK( XBase::DoData(io) );

  IO_CHECK( io.DoDataVector(m_Objects) );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XNodeCollection::GetItems(XList& items) const 
{
  items.clear();

  XIndexList::const_iterator it = m_Objects.begin();
  for(;it != m_Objects.end();++it)
  {
    XBase* base = m_pModel->FindNode((*it));
    if(base)
    {
      items.push_back(base);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XNodeCollection::SetItems(const XList& items)
{
  m_Objects.clear();

  XList::const_iterator it = items.begin();
  for(;it != items.end();++it)
  {
    if( (!IsItem(*it)) && IsValidObj(*it) ) 
    {
      m_Objects.push_back( (*it)->GetID() );
    }
  }
}
}
