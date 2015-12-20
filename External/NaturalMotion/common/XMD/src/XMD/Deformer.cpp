//----------------------------------------------------------------------------------------------------------------------
/// \file Deformer.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Deformer.h"
#include "XMD/Geometry.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"
#include "nmtl/algorithm.h"

namespace XMD 
{
NMTL_POD_VECTOR_EXPORT(XDeformer*,XMD_EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
XDeformer::XDeformer(XModel* pmod)
  : XBase(pmod),m_EnvelopeWeight(1.0f),m_AffectedGeometry()
{
  DUMPER(XDeformer__ctor);
}

//----------------------------------------------------------------------------------------------------------------------
XDeformer::~XDeformer()
{
  DUMPER(XDeformer__dtor);
}

//----------------------------------------------------------------------------------------------------------------------
bool XDeformer::AddAffected(const XBase* object) 
{
  DUMPER(XDeformer__AddAffected);

  // if you assert here, the object you are trying to attach to this 
  // deformer is NULL, or not from the same XModel.
  XMD_ASSERT( (object) && IsValidObj(object) );

  if(!object)
    return false;

  // If you assert here, you are attempting to deform a non XGeometry
  // derived type. That's not allowed to happen! The shape must have
  // points to deform.
  const XGeometry* geom = object->HasFn<XGeometry>();
  XMD_ASSERT(geom);

  if(geom)
  {
    XIndexList::iterator it = nmtl::find(m_AffectedGeometry.begin(),m_AffectedGeometry.end(), object->GetID() );
    if (it == m_AffectedGeometry.end()) 
    {
      m_AffectedGeometry.push_back( object->GetID() );
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XDeformer::RemoveAffected(const XU32 index)
{
  DUMPER(XDeformer__RemoveAffected);
  if (index<m_AffectedGeometry.size())
  {
    m_AffectedGeometry.erase(m_AffectedGeometry.begin()+index);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XDeformer::RemoveAffected(const XBase* object)
{
  DUMPER(XDeformer__RemoveAffected);
  if (object)
  {
    XIndexList::iterator it = nmtl::find(m_AffectedGeometry.begin(),m_AffectedGeometry.end(), object->GetID() );
    if(it != m_AffectedGeometry.end())
    {
      m_AffectedGeometry.erase(it);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XDeformer::GetApiType() const
{
  return XFn::Deformer;
}

//----------------------------------------------------------------------------------------------------------------------
bool XDeformer::DoData(XFileIO& io) 
{
  DUMPER(XDeformer);

  CHECK_IO( XBase::DoData(io) );

  CHECK_IO( io.DoDataVector(m_AffectedGeometry) );
  DAPARAM(m_AffectedGeometry);

  if (GetModel()->GetInputVersion()>=4)
  {
    CHECK_IO( io.DoData(&m_EnvelopeWeight) );
    DPARAM(m_EnvelopeWeight);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XDeformer::DoCopy(const XBase* rhs)
{
  const XDeformer* cb = rhs->HasFn<XDeformer>();
  XMD_ASSERT(cb);
  m_AffectedGeometry = cb->m_AffectedGeometry;
  m_EnvelopeWeight = cb->m_EnvelopeWeight;
  XBase::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XDeformer::GetDataSize() const
{
  XU32 size = XBase::GetDataSize();
  size += sizeof(XU32);
  size += sizeof(XReal);
  size += static_cast<XU32>(sizeof(XId)*m_AffectedGeometry.size());
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
const XIndexList& XDeformer::GetAffected() const 
{
  return m_AffectedGeometry;
}


//----------------------------------------------------------------------------------------------------------------------
XU32 XDeformer::GetNumAffected() const 
{
  return static_cast<XU32>(m_AffectedGeometry.size());
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XDeformer::GetAffected(XU32 id) 
{
  if (id < GetNumAffected() ) 
  {
    return GetModel()->FindNode(m_AffectedGeometry[id]);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XDeformer::GetEnvelopeWeight() const
{
  return m_EnvelopeWeight;
}

//----------------------------------------------------------------------------------------------------------------------
void XDeformer::SetEnvelopeWeight(const XReal envelope)
{
  m_EnvelopeWeight = envelope;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XDeformer::GetFn(XFn::Type type)
{
  if(XFn::Deformer==type)
    return (XDeformer*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XDeformer::GetFn(XFn::Type type) const
{
  if(XFn::Deformer==type)
    return (const XDeformer*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XDeformer::PreDelete(XIdSet& extra_nodes)
{
  DUMPER(XDeformer__PreDelete);
  XBase::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XDeformer::NodeDeath(XId id)
{
  DUMPER(XDeformer__NodeDeath);
  XIndexList::iterator it = m_AffectedGeometry.begin();
  // an array of affected geometry
  for( ; it != m_AffectedGeometry.end(); ++it )
  {
    if(id == *it)
    {
      m_AffectedGeometry.erase(it);
      break;
    }
  }

  if (m_AffectedGeometry.size()==0) 
  {
    return false;
  }
  return XBase::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
bool XDeformer::ReadChunk(std::istream& ifs)
{
  DUMPER(XDeformer__ReadChunk);
  READ_CHECK("affected",ifs);
  XU32 num=0;
  ifs >> num;

  m_AffectedGeometry.resize(num);
  XIndexList::iterator it = m_AffectedGeometry.begin();
  for ( ; it != m_AffectedGeometry.end(); ++it )
  {
    ifs >> *it;
  }

  if (GetModel()->GetInputVersion()>=4)
  {
    READ_CHECK("envelope_weight",ifs);
    ifs >> m_EnvelopeWeight;
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XDeformer::WriteChunk(std::ostream& ofs) 
{
  DUMPER(XDeformer__WriteChunk);

  ofs << "\taffected " << static_cast<XU32>(m_AffectedGeometry.size()) << " ";
  XIndexList::iterator it = m_AffectedGeometry.begin();
  for ( ; it != m_AffectedGeometry.end(); ++it )
  {
    ofs << *it ;
    if(it != (m_AffectedGeometry.end()-1))
      ofs << " ";
  }
  ofs << "\n";
  ofs << "\tenvelope_weight " << m_EnvelopeWeight << "\n";

  return ofs.good();
}
}
