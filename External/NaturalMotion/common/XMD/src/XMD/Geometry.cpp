//----------------------------------------------------------------------------------------------------------------------
/// \file Geometry.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Geometry.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XU32 XGeometry::GetNumPoints() const 
{
  return static_cast<XU32>(m_Points.size());
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3& XGeometry::GetPoint(XU32 idx) const 
{
  return m_Points[idx];
}

//----------------------------------------------------------------------------------------------------------------------
void XGeometry::SetPoint(XU32 idx,const XReal x,const XReal y,const XReal z)
{
  m_Points[idx].set(x,y,z);
}

//----------------------------------------------------------------------------------------------------------------------
void XGeometry::SetPoint(XU32 idx,const XVector3& p)
{
  m_Points[idx] = p;
}

//----------------------------------------------------------------------------------------------------------------------
XVector3Array::const_iterator XGeometry::PointsBegin() const 
{
  return m_Points.begin(); 
}

//----------------------------------------------------------------------------------------------------------------------
XVector3Array::const_iterator XGeometry::PointsEnd() const 
{ 
  return m_Points.end(); 
}

//----------------------------------------------------------------------------------------------------------------------
XVector3Array::iterator XGeometry::PointsBegin() 
{
  return m_Points.begin(); 
}

//----------------------------------------------------------------------------------------------------------------------
XVector3Array::iterator XGeometry::PointsEnd() 
{ 
  return m_Points.end(); 
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3Array& XGeometry::Points() const 
{
  return m_Points; 
}

//----------------------------------------------------------------------------------------------------------------------
XGeometry::XGeometry(XModel* pmod)
  : XShape(pmod),m_Points()
{
  DUMPER(XGeometry__ctor);
  m_IsIntermediateObject=false;
}

//----------------------------------------------------------------------------------------------------------------------
XGeometry::~XGeometry()
{
  DUMPER(XGeometry__dtor);
}

//----------------------------------------------------------------------------------------------------------------------
bool XGeometry::GetIsIntermediateObject() const
{
  return m_IsIntermediateObject;
}

//----------------------------------------------------------------------------------------------------------------------
void XGeometry::SetIsIntermediateObject(bool flag) 
{
  m_IsIntermediateObject=flag;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XGeometry::GetApiType() const
{
  return XFn::Geometry;
}

//----------------------------------------------------------------------------------------------------------------------
bool XGeometry::DoData(XFileIO& io)
{
  DUMPER(XGeometry);

  IO_CHECK( XShape::DoData(io) );

  IO_CHECK( io.DoDataVector(m_Points) );
  DAPARAM( m_Points );

  if(io.IsWriting())
  {
    if(GetModel()->GetExportPolicy().IsEnabled(XExportPolicy::WriteDeformers))
    {
      IO_CHECK( io.DoDataVector(m_DeformerQueue) );
      DAPARAM( m_DeformerQueue );
    }
    else
    {
      XIndexList no_deformers;
      IO_CHECK( io.DoDataVector(no_deformers) );
    }
  }
  else
  {
    IO_CHECK( io.DoDataVector(m_DeformerQueue) );
    DAPARAM( m_DeformerQueue );
  }

  IO_CHECK( io.DoData(&m_IsIntermediateObject) );
  DPARAM( m_IsIntermediateObject );

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XGeometry::DoCopy(const XBase* rhs)
{
  const XGeometry* cb = rhs->HasFn<XGeometry>();
  XMD_ASSERT(cb);

  m_Points = cb->m_Points;
  m_IsIntermediateObject = cb->m_IsIntermediateObject;

  // if we are duplicating the geometry deformers... 
  if(GetModel()->GetClonePolicy().IsEnabled(XClonePolicy::DuplicateGeometryDeformers))
  {
    XDeformerList defs;
    cb->GetDeformerQueue(defs);
    XDeformerList new_defs;
    XDeformerList::iterator it = defs.begin();
    for (;it != defs.end();++it)
    {
      // clone each deformer
      XDeformer* def = (*it)->Clone()->HasFn<XDeformer>();

      // add to deformer queue
      new_defs.push_back(def);

      // make deformer affect this geom
      def->AddAffected(this);
    }

    // set the queue of deformers on the duplicated geometry
    SetDeformerQueue(new_defs);
  }

  XShape::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XGeometry::GetDataSize() const 
{
  size_t sz = XShape::GetDataSize();
  sz += sizeof(XU32) + sizeof(XReal)*3*m_Points.size(); // points
  sz += sizeof(XU32) + sizeof(XU32)*m_DeformerQueue.size(); // deformer queue
  sz += sizeof(bool); // inherits transform
  return static_cast<XU32>(sz);
}

//----------------------------------------------------------------------------------------------------------------------
bool XGeometry::GetDeformerQueue(XDeformerList& items) const
{

  DUMPER(XGeometry__GetDeformerQueue);
  // the list of deformers affecting this object
  XIndexList::const_iterator it = m_DeformerQueue.begin();

  for( ; it != m_DeformerQueue.end(); ++it )
  {
    XBase* pb = m_pModel->FindNode(*it);
    if(pb)
    {
      XDeformer* pD = pb->HasFn<XDeformer>();
      if(pD)
        items.push_back(pD);
    }
  }

  return items.size() != 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XGeometry::SetDeformerQueue(const XDeformerList& _list)
{
  DUMPER(XGeometry__SetDeformerQueue);
  m_DeformerQueue.resize(_list.size());

  XIndexList::iterator itdq = m_DeformerQueue.begin();
  XDeformerList::const_iterator it = _list.begin();
  for ( ; it != _list.end(); ++it, ++itdq ) 
  {
    // if you assert here the deformer queue contains a null deformer, 
    // or a deformer from another XModel.
    XMD_ASSERT( (*it) && IsValidObj(*it) );

    *itdq =  (*it)->GetID();
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XGeometry::GetPoints(XVector3Array &points) const
{
  DUMPER(XGeometry__GetPoints);
  points = m_Points;
}

//----------------------------------------------------------------------------------------------------------------------
void XGeometry::GetPoints(XVector3Array &points,const XBone* bone,const XInstance* instance) const
{
  DUMPER(XGeometry__GetPoints);
  points.resize(m_Points.size());

  // get parent of this object
  const XBone* parent = instance->GetParent();

  // construct transform to go from parent to world space, then from
  // world space to the new parent
  XMatrix M = bone->GetInverse(true) * parent->GetWorld(true);

  // loop through all points and transform original points
  XVector3Array::iterator it = points.begin();
  XVector3Array::const_iterator ito = m_Points.begin();
  for( ; it != points.end(); ++it,++ito )
  {
    M.transformPoint(*it,*ito);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XGeometry::GetPoints(XReal** points,XU32& num) const
{
  DUMPER(XGeometry__GetPoints);
  if(!m_Points.size())
  {
    num=0;
    *points=0;
    return;
  }
  *points = new XReal [ m_Points.size()*3 ];
  XMD_ASSERT(*points);
  num = static_cast<XU32>(m_Points.size());
  XReal *p= *points;
  XVector3Array::const_iterator it = m_Points.begin();
  for( ; it != m_Points.end(); ++it,p+=3 )
  {
    p[0] = it->x;
    p[1] = it->y;
    p[2] = it->z;
  }
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XGeometry::GetFn(XFn::Type type)
{
  if(XFn::Geometry==type)
    return (XGeometry*)this;
  return XShape::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XGeometry::GetFn(XFn::Type type) const
{
  if(XFn::Geometry==type)
    return (const XGeometry*)this;
  return XShape::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XGeometry::PreDelete(XIdSet& extra_nodes)
{
  DUMPER(XGeometry__PreDelete);
  XShape::PreDelete(extra_nodes);

  const XDeletePolicy& dp = GetModel()->GetDeletePolicy();


  if ( dp.IsEnabled(XDeletePolicy::RelatedDeformers) ) 
  {
    XDeformerList defs;
    GetDeformerQueue(defs);
    XDeformerList::iterator it = defs.begin();
    for ( ; it != defs.end(); ++it ) 
    {
      const XDeformer* def = *it;
      if (def->GetNumAffected() <= 1) 
      {
        extra_nodes.insert(def->GetID());
      }
    }
  }    
}

//----------------------------------------------------------------------------------------------------------------------
bool XGeometry::NodeDeath(XId id)
{
  DUMPER(XGeometry__NodeDeath);

  XIndexList::iterator it = m_DeformerQueue.begin();
  for( ; it != m_DeformerQueue.end(); ++it )
  {
    if(id == *it) 
    {
      m_DeformerQueue.erase(it);
      return true;
    }
  }
  return XShape::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
bool XGeometry::ReadChunk(std::istream& ifs)
{
  DUMPER(XGeometry__ReadChunk);
  if(!XShape::ReadChunk(ifs))
    return false;

  if( ApiType() == XFn::ParticleShape && 
      GetModel()->GetInputVersion() <= 3 )
  {
    //
    // version 3 had an inconsistency in the way that particle shapes were
    // written to the disk vs. all other geometry objects. That has now been
    // fixed in V4 to allow you to attach geometry deformers to particle shapes
    //
  }
  else
  {
    READ_CHECK("points",ifs);
    XS32 sz;
    ifs >> sz;
    if(sz > 0)
    {
      m_Points.resize(sz);
      XVector3Array::iterator it = m_Points.begin();
      for( ; it != m_Points.end(); ++it )
      {
        ifs >> *it;
        if( !ifs.good() ) 
        {
          XGlobal::GetLogger()->Logf(XBasicLogger::kError,"UnExpected file-IO error");
          return false;
        }
      }
    }

    {
      READ_CHECK("deformer_queue",ifs);
      ifs >> sz;
      m_DeformerQueue.resize(sz);
      XIndexList::iterator it = m_DeformerQueue.begin();
      for (; it != m_DeformerQueue.end(); ++it) 
      {
        ifs >> *it;
      }
    }
  }

  if (GetModel()->GetInputVersion()>=4)
  {
    READ_CHECK("intermediate_object",ifs);
    ifs >> m_IsIntermediateObject; 
  }

  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
void XGeometry::SetPoints(const XVector3Array &points)
{
  DUMPER(XGeometry__SetPoints);
  m_Points = points;
}

//----------------------------------------------------------------------------------------------------------------------
void XGeometry::SetPoints(const XReal* points,XU32 num)
{
  DUMPER(XGeometry__SetPoints);
  m_Points.resize(num);
  XVector3Array::iterator it = m_Points.begin();
  for(XU32 i=0; it != m_Points.end(); ++it)
  {
    it->x = points[i++];
    it->y = points[i++];
    it->z = points[i++];
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XGeometry::WriteChunk(std::ostream& ofs)
{
  DUMPER(XGeometry__WriteChunk);
  if(!XShape::WriteChunk(ofs))
    return false;

  ofs << "\tpoints " << static_cast<XU32>(m_Points.size()) << "\n";
  XVector3Array::iterator it = m_Points.begin();
  for( ; it != m_Points.end(); ++it )
  {
    ofs << "\t\t" << *it << "\n";
  }

  if(GetModel()->GetExportPolicy().IsEnabled(XExportPolicy::WriteDeformers))
  {
    ofs << "\tdeformer_queue " << static_cast<XU32>(m_DeformerQueue.size());
    XIndexList::iterator it = m_DeformerQueue.begin();
    for (; it != m_DeformerQueue.end(); ++it) 
    {
      ofs << " " << *it;
    }
    ofs << std::endl;
  }
  else
  {
    ofs << "\tdeformer_queue 0\n";
  }

  ofs << "\tintermediate_object " << m_IsIntermediateObject << "\n";

  return ofs.good();
}
}
