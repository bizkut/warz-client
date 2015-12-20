//----------------------------------------------------------------------------------------------------------------------
/// \file ObjectSet.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Model.h"
#include "XMD/ObjectSet.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XObjectSet::XObjectSet(XModel* pModel)
  : XNodeCollection(pModel), m_Annotation()
{
}

//----------------------------------------------------------------------------------------------------------------------
XObjectSet::~XObjectSet()
{
}

//----------------------------------------------------------------------------------------------------------------------
void XObjectSet::PreDelete(XIdSet& extra_nodes)
{
  XNodeCollection::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XObjectSet::NodeDeath(XId id) 
{
  return XNodeCollection::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XObjectSet::GetAnnotation() const 
{
  return m_Annotation.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
void XObjectSet::SetAnnotation(const XChar* name)
{
  m_Annotation=name;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XObjectSet::GetApiType() const 
{
  return XFn::ObjectSet;
}

//----------------------------------------------------------------------------------------------------------------------
void XObjectSet::DoCopy(const XBase* rhs)
{
  const XObjectSet* cb = rhs->HasFn<XObjectSet>();
  XMD_ASSERT(cb);
  m_Annotation = cb->m_Annotation;
  XNodeCollection::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XObjectSet::GetFn(XFn::Type type) 
{
  if(XFn::ObjectSet == type)
  {
    return (XObjectSet*)this;
  }
  return XNodeCollection::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XObjectSet::GetFn(XFn::Type type) const
{
  if(XFn::ObjectSet==type)
    return (const XObjectSet*)this;
  return XNodeCollection::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XObjectSet::ReadChunk(std::istream& ifs) 
{
  if(!XNodeCollection::ReadChunk(ifs))
    return false;

  READ_CHECK("annotation",ifs);
  ifs >> m_Annotation;
  /// \todo can't deal with spaces!
  if (m_Annotation[0]=='\"') 
  {
    m_Annotation.erase(m_Annotation.begin());
  }
  if (m_Annotation[m_Annotation.size()-1]=='\"') 
  {
    m_Annotation.erase(m_Annotation.end()-1);
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XObjectSet::WriteChunk(std::ostream& ofs)
{
  if(!XNodeCollection::WriteChunk(ofs))
    return false;

  ofs << "\tannotation \"" << m_Annotation << "\"\n";

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XObjectSet::GetDataSize() const 
{
  size_t sz = XNodeCollection::GetDataSize() + 2 + m_Annotation.size();
  return (XU32)sz;
}

//----------------------------------------------------------------------------------------------------------------------
bool XObjectSet::DoData(XFileIO& io) 
{
  DUMPER(XObjectSet);

  IO_CHECK( XNodeCollection::DoData(io) );

  IO_CHECK( io.DoDataString(m_Annotation) );
  DPARAM( m_Annotation );

  return true;
}
}
