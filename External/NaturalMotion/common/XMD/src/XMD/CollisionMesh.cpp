//----------------------------------------------------------------------------------------------------------------------
/// \file CollisionMesh.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/CollisionMesh.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XCollisionMesh::XCollisionMesh(XModel* pmod) 
  : XCollisionObject(pmod),m_Indices(),m_Vertices()
{
}

//----------------------------------------------------------------------------------------------------------------------
XCollisionMesh::~XCollisionMesh()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XCollisionMesh::GetApiType() const
{
  return XFn::CollisionMesh;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XCollisionMesh::GetDataSize() const 
{
  size_t sz = XCollisionObject::GetDataSize() + 
              sizeof(XId)*(2+m_Indices.size()) +
              3*sizeof(XReal)*(m_Vertices.size());
  return (XU32)sz;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XCollisionMesh::GetFn(XFn::Type type)
{
  if(XFn::CollisionMesh==type)
    return (XCollisionMesh*)this;
  return XCollisionObject::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XCollisionMesh::GetFn(XFn::Type type) const
{
  if(XFn::CollisionMesh==type)
    return (const XCollisionMesh*)this;
  return XCollisionMesh::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionMesh::NodeDeath(XId id)
{
  return XCollisionObject::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionMesh::PreDelete(XIdSet& extra_nodes)
{
  XCollisionObject::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionMesh::ReadChunk(std::istream& ifs)
{
  READ_CHECK("num_points",ifs);
  XS32 NumVertices;
  ifs >> NumVertices;
  m_Vertices.resize(NumVertices);

  READ_CHECK("num_indices",ifs);
  XS32 NumIndices;
  ifs >> NumIndices;
  m_Indices.resize(NumIndices);

  READ_CHECK("points",ifs);
  {
    XVector3Array::iterator it = m_Vertices.begin();
    for(;it != m_Vertices.end();++it)
    {
      if (!ifs.good()) 
      {
        XGlobal::GetLogger()->Log(XBasicLogger::kError,"XMD::XCollisionMesh::ReadChunk >> unexpected File-IO error whilst reading vertex data");
        return false;
      }
      ifs >> *it;
    }
  }

  READ_CHECK("indices",ifs);
  {
    XIndexList::iterator it = m_Indices.begin();
    for(;it != m_Indices.end();++it)
    {
      if (!ifs.good()) 
      {
        XGlobal::GetLogger()->Log(XBasicLogger::kError,"XMD::XCollisionMesh::ReadChunk >> unexpected File-IO error whilst reading indices");
        return false;
      }
      ifs >> *it;
    }
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionMesh::WriteChunk(std::ostream& ofs)
{
  // the camera's aspect ratio
  ofs << "\tnum_points " << static_cast<XU32>(m_Vertices.size()) << "\n";
  ofs << "\tnum_indices " << static_cast<XU32>(m_Indices.size()) << "\n";

  ofs << "\tpoints\n";
  {
    XVector3Array::iterator it = m_Vertices.begin();
    for( ; it != m_Vertices.end(); ++it )
    {
      ofs << "\t\t" << *it << "\n";
    }
  }

  ofs << "\tindices\n";
  {
    XIndexList::iterator it = m_Indices.begin();
    for( ; it != m_Indices.end(); ++it )
    {
      ofs << "\t\t" << *it << "\n";
    }
  }

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XCollisionMesh::DoData(XFileIO& io) 
{
  DUMPER(XCollisionMesh);
  
  IO_CHECK( XCollisionObject::DoData(io) );
  IO_CHECK( io.DoDataVector(m_Vertices) );
  IO_CHECK( io.DoDataVector(m_Indices) );
  DAPARAM( m_Indices );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionMesh::GetIndices(XIndexList& point_inds) const
{
  point_inds = m_Indices;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionMesh::SetIndices(const XIndexList& point_inds)
{
  m_Indices = point_inds;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionMesh::GetVertices(XVector3Array& vertices) const
{
  vertices = m_Vertices;
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionMesh::SetVertices(const XVector3Array& vertices)
{
  m_Vertices = vertices;
}

//----------------------------------------------------------------------------------------------------------------------
const XIndexList& XCollisionMesh::Indices() const 
{
  return m_Indices; 
}

//----------------------------------------------------------------------------------------------------------------------
const XVector3Array& XCollisionMesh::Vertices() const 
{ 
  return m_Vertices; 
}

//----------------------------------------------------------------------------------------------------------------------
void XCollisionMesh::DoCopy(const XBase* rhs)
{
  const XCollisionMesh* cb = rhs->HasFn<XCollisionMesh>();
  XMD_ASSERT(cb);
  m_Vertices = cb->m_Vertices;
  m_Indices = cb->m_Indices;
  XCollisionObject::DoCopy(cb);
}
}
