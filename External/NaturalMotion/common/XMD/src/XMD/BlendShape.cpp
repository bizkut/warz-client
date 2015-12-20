//----------------------------------------------------------------------------------------------------------------------
/// \file BlendShape.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/BlendShape.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XGeometry* XBlendShape::GetTarget(const XU32 target) const
{
  if(target < m_BlendTargets.size() ) 
  {
    XBase* p = m_pModel->FindNode(m_BlendTargets[target]->m_GeometryID);
    if(p) 
    {
      return p->HasFn<XGeometry>();
    }

    p = m_pModel->FindNode(m_BlendTargets[target]->m_TargetName);
    if(p) 
    {
      return p->HasFn<XGeometry>();
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBlendShape::XBlendShape(XModel* pmod) 
  : XDeformer(pmod),m_BlendTargets()
{
}

//----------------------------------------------------------------------------------------------------------------------
XBlendShape::~XBlendShape()
{
  XBlendTargetArray::iterator it = m_BlendTargets.begin();
  for (;it != m_BlendTargets.end();++it)
  {
    delete *it;
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XBlendShape::GetNumTargets() const 
{
  return static_cast<XU32>(m_BlendTargets.size());
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XBlendShape::GetApiType() const
{
  return XFn::BlendShape;
}

//----------------------------------------------------------------------------------------------------------------------
XGeometry* XBlendShape::GetBase() const 
{
  if(!m_AffectedGeometry.size())
    return 0;
  XBase* ptr = m_pModel->FindNode(m_AffectedGeometry[0]);
  if (ptr)
  {
    return ptr->HasFn<XGeometry>();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlendShape::SetBase(const XBase* geom) 
{
  if (geom && geom->HasFn<XGeometry>() && IsValidObj(geom))
  {
    m_AffectedGeometry.resize(1);
    m_AffectedGeometry[0] = geom->GetID();
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlendShape::AddTarget(const XVector3Array& points,const XString& name) 
{
  XBlendTargetArray::iterator it = m_BlendTargets.begin();
  for( ; it != m_BlendTargets.end(); ++it )
  {
    if ((*it)->m_TargetName == name) 
    {
      return false;
    }
  }
  XBlendTarget* bt = new XBlendTarget;
  bt->m_GeometryID=0;
  bt->m_Offsets = points;
  bt->m_Weight = 0.0f;
  bt->m_TargetName = name;
  m_BlendTargets.push_back(bt);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlendShape::AddTarget(const XGeometry* geometry,const XString& name) 
{
  XBlendTargetArray::iterator it = m_BlendTargets.begin();
  for( ; it != m_BlendTargets.end(); ++it )
  {
    if ((*it)->m_TargetName == name) 
    {
      return false;
    }
  }

  if(!geometry)
    return false;

  XVector3Array points;
  geometry->GetPoints(points);

  XBlendTarget* bt = new XBlendTarget;
  bt->m_GeometryID = geometry->GetID();
  bt->m_Offsets = points;
  bt->m_Weight = 0.0f;
  bt->m_TargetName = name;
  m_BlendTargets.push_back(bt);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlendShape::DeleteTarget(const XU32 target)
{
  if(target<m_BlendTargets.size())
  {
    delete m_BlendTargets[target];
    m_BlendTargets.erase( m_BlendTargets.begin() + target );
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlendShape::GetTarget(const XU32 target,XVector3Array& points) const
{
  if(target<m_BlendTargets.size()) 
  {
    points = m_BlendTargets[target]->m_Offsets;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlendShape::SetTarget(const XU32 target, const XVector3Array& points)
{
  if(target<m_BlendTargets.size()) 
  {
    m_BlendTargets[target]->m_Offsets = points;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlendShape::GetTargetName(const XU32 target,XString& name) const
{
  if(target<m_BlendTargets.size())
  {
    name = m_BlendTargets[target]->m_TargetName;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XBlendShape::GetFn(XFn::Type type)
{
  if(XFn::BlendShape==type)
    return (XBlendShape*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XBlendShape::GetFn(XFn::Type type) const
{
  if(XFn::BlendShape==type)
    return (const XBlendShape*)this;
  return XDeformer::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlendShape::NodeDeath(XId id)
{
  return XDeformer::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XBlendShape::PreDelete(XIdSet& extra_nodes)
{
  XDeformer::PreDelete(extra_nodes);
  const XDeletePolicy& dp = GetModel()->GetDeletePolicy();

  if ( dp.IsEnabled(XDeletePolicy::RelatedDeformerInputs) ) 
  {
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlendShape::ReadChunk(std::istream& ifs)
{
  XString buffer;

  if (!XDeformer::ReadChunk(ifs)) 
    return false;

  READ_CHECK("num_shapes",ifs);
  XU32 ns=0;
  ifs >> ns;

  m_BlendTargets.resize(ns);
  for(XU32 i=0;i<ns;++i)
  {
    XBlendTarget* bt = new XBlendTarget;
    m_BlendTargets[i] = bt;
    ifs >> bt->m_TargetName;

    if(GetModel()->GetInputVersion()>=4) 
    {
      ifs >> bt->m_GeometryID;
      ifs >> bt->m_Weight;
    }

    READ_CHECK("points",ifs);
    XS32 np;
    ifs >> np;

    bt->m_Offsets.resize(np);
    XVector3Array::iterator it = bt->m_Offsets.begin();
    for( ; it != bt->m_Offsets.end(); ++it )
    {  
      ifs >> *it;
    }
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlendShape::WriteChunk(std::ostream& ofs) 
{
  if (!XDeformer::WriteChunk(ofs))
    return false;

  ofs << "\tnum_shapes " << static_cast<XU32>(m_BlendTargets.size()) << "\n";

  XBlendTargetArray::iterator it = m_BlendTargets.begin();
  for(;it != m_BlendTargets.end();++it)
  {
    XBlendTarget* bt = *it;

    ofs << "\t" << bt->m_TargetName.c_str() << " " << bt->m_GeometryID << " " << bt->m_Weight << "\n";
    ofs << "\t\tpoints " << static_cast<XU32>(bt->m_Offsets.size()) << "\n";


    XVector3Array::const_iterator it = bt->m_Offsets.begin();
    for( ; it != bt->m_Offsets.end(); ++it )
    {  
      ofs << "\t\t" << *it << "\n";
    }
  }

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XBlendShape::DoData(XFileIO& io) 
{
  DUMPER(XBlendShape);

  IO_CHECK( XDeformer::DoData(io) );

  // changing the format to match the ascii data layout
  if(GetModel()->GetInputVersion()>=4)
  {
    XU32 num_blend_shapes = static_cast<XU32>(m_BlendTargets.size());
    IO_CHECK( io.DoData(&num_blend_shapes) );
    m_BlendTargets.resize(num_blend_shapes);

    XBlendTargetArray::iterator it = m_BlendTargets.begin();
    for(;it != m_BlendTargets.end();++it)
    {
      if(io.IsReading())
      {
        *it = new XBlendTarget;
      }
      IO_CHECK( io.DoDataString( (*it)->m_TargetName ) );
      IO_CHECK( io.DoData( &(*it)->m_GeometryID ) );
      IO_CHECK( io.DoData( &(*it)->m_Weight ) );
      IO_CHECK( io.DoDataVector( (*it)->m_Offsets ) );
    }
  }
  else
  {
    XStringList names;
    if(!io.IsReading())
    {
      XBlendTargetArray::iterator it = m_BlendTargets.begin();
      for(;it != m_BlendTargets.end();++it)
      {
        names.push_back((*it)->m_TargetName);
      }
    }
    IO_CHECK( io.DoStringVector(names) );
    DAPARAM( names );

    XU32 numShapes = static_cast<XU32>(m_BlendTargets.size());

    IO_CHECK( io.DoData(&numShapes) );
    DPARAM( numShapes );

    m_BlendTargets.resize(numShapes);
    if(io.IsReading())
    {
      XBlendTargetArray::iterator it = m_BlendTargets.begin();
      XStringList::iterator itn = names.begin();
      for(;it != m_BlendTargets.end();++it,++itn)
      {
        *it = new XBlendTarget;
        (*it)->m_GeometryID = 0;
        (*it)->m_TargetName = *itn;
      }
    }
    XBlendTargetArray::iterator it = m_BlendTargets.begin();
    for( ; it != m_BlendTargets.end(); ++it ) 
    {
      IO_CHECK( io.DoDataVector( (*it)->m_Offsets ) );
      DAPARAM( (*it)->m_Offsets );
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XBlendShape::DoCopy(const XBase* rhs)
{
  const XBlendShape* cb = rhs->HasFn<XBlendShape>();
  XMD_ASSERT(cb);

  m_BlendTargets.resize( cb->m_BlendTargets.size() );
  XBlendTargetArray::const_iterator itb = cb->m_BlendTargets.begin();
  XBlendTargetArray::iterator it = m_BlendTargets.begin();
  for( ; it != m_BlendTargets.end(); ++it, ++itb ) 
  {
    (*it)->m_GeometryID = (*itb)->m_GeometryID;
    (*it)->m_TargetName = (*itb)->m_TargetName;
    (*it)->m_Offsets = (*itb)->m_Offsets;
    (*it)->m_Weight = (*itb)->m_Weight;
  }

  XDeformer::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XBlendShape::GetDataSize() const
{
  XU32 sz = static_cast<XU32>(sizeof(XU32));
  {
    XBlendTargetArray::const_iterator it = m_BlendTargets.begin();
    for( ; it != m_BlendTargets.end(); ++it ) 
    {
      sz += static_cast<XU32>(2 + (*it)->m_TargetName.size());
      sz += static_cast<XU32>( sizeof(XId) );
      sz += static_cast<XU32>( sizeof(XReal) );
      sz += static_cast<XU32>(4 + (*it)->m_Offsets.size() * 3*sizeof(XReal) );
    }
  }
  return sz + XDeformer::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
XReal XBlendShape::GetWeight(const XU32 target_num) const
{
  if (target_num < GetNumTargets())
  {
    return m_BlendTargets[target_num]->m_Weight;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XBlendShape::SetWeight(const XU32 target_num,const XReal weight)
{
  if (target_num < GetNumTargets())
  {
    m_BlendTargets[target_num]->m_Weight = weight;
  }
}
}

