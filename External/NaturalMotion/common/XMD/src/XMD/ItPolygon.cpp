//----------------------------------------------------------------------------------------------------------------------
/// \file ItPolygon.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Model.h"
#include "XMD/PolygonMesh.h"
#include "XMD/ItPolygon.h"
#include "XMD/FileIO.h"
#include "XM2/Plane.h"

namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
XItPolygon::XItPolygon(XPolygonMesh* poly_mesh)
  : m_Mesh(poly_mesh)
{
  Reset();
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::IsValid() const
{ 
  return m_Mesh != 0; 
}

//----------------------------------------------------------------------------------------------------------------------
void XItPolygon::Next()
{
  if(!IsValid()) 
    return;
  if(m_CurrentFaceIndex<m_Mesh->m_PolyCounts.size())
  {
    // increment current vertex index by number of vertices
    // in the previous face. 
    m_CurrentVertexIndex += m_NumVertices;

    // increment current face index
    ++m_CurrentFaceIndex;

    // now set the num of vertices in this face
    if(m_CurrentFaceIndex<m_Mesh->m_PolyCounts.size())
      m_NumVertices = m_Mesh->m_PolyCounts[m_CurrentFaceIndex];
    else 
      m_NumVertices = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XItPolygon::Previous()
{
  if(!IsValid()) 
    return;
  if(m_CurrentFaceIndex>0)
  {
    // decrement current face index
    --m_CurrentFaceIndex;

    // get num of vertices in this face
    m_NumVertices = m_Mesh->m_PolyCounts[m_CurrentFaceIndex];

    // decrement current vertex index by number of vertices
    // in the previous face. 
    m_CurrentVertexIndex -= m_NumVertices;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XItPolygon::Reset()
{
  if(!IsValid()) 
    return;

    m_CurrentVertexIndex=0;
    m_CurrentFaceIndex=0;
    m_NumVertices = m_Mesh->m_PolyCounts.size() ? m_Mesh->m_PolyCounts[m_CurrentFaceIndex] : 0;
  }

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::IsDone() const
{
  if(IsValid())
  {
    return m_CurrentFaceIndex == m_Mesh->m_PolyCounts.size();
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XItPolygon::GetFaceIndex() const
{
  return m_CurrentFaceIndex;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::SetToFace(const XU32 face_index)
{
  if(IsValid())
  {
    // make sure face index is valid.
    if(face_index<m_Mesh->m_PolyCounts.size())
    {
      // if the requested face is further on that the current
      // face, the quickest way is to iterate there from the 
      // current position
      if(face_index>GetFaceIndex())
      {
        XU32 temp = face_index-GetFaceIndex();
        for (XU32 i=0;i!=temp;++i)
          Next();
      }
      else
      {
        // in this case the requested index is before the current
        // face. 
        XU32 temp = GetFaceIndex() - face_index;

        // if it will take less work to move backwards from the current
        // position to the requested face, then do that...
        if(temp<face_index)
        {
          for (XU32 i=0;i!=temp;++i)
            Previous();
        }
        else
        {
          // else reset to the beginning and iterate forwards... 
          Reset();
          for(XU32 i=0;i!=face_index;++i)
            Next();
        }
      }
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XReal XItPolygon::GetArea() const
{
  XReal area = 0.0f;
  if(IsValid())
  {
    // for all triangles in the face
    for (XU32 i=2;i<GetNumPoints();++i)
    {
      XU32 index0 = 0;
      XU32 index1 = i-1;
      XU32 index2 = i;

      XVector3 v0,v1,v2;
      GetVertex(v0,index0);
      GetVertex(v1,index1);
      GetVertex(v2,index2);

      // get 2 edges
      XVector3 e1,e2,n;
      e1.sub(v1,v0);
      e2.sub(v1,v2);
      n.cross(e1,e2);

      // sum area from the 2 tris
      area += n.length()*0.5f;
    }
  }
  return area;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XItPolygon::GetNumPoints() const
{
  return m_NumVertices;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::IsPlanar(const XReal tolerance) const
{
  // triangles are always planar 
  if(GetNumPoints()==3 || !IsValid())
    return true;

  XVector3 p0,p1,p2;
  GetVertex(p0,0);
  GetVertex(p1,1);
  GetVertex(p2,2);

  // construct a test plane using the first 3 verts.
  XPlane test_plane;

  // now do a distance test between the remaining points and that plane
  for(XU32 i=3;i<GetNumPoints();++i)
  {
    XVector3 p;
    if(!GetVertex(p,i))
      return false;
    if(test_plane.distance(p)>tolerance)
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertex(XVector3& point,const XU32 face_vertex_index) const
{
  if(IsValid() && face_vertex_index<GetNumPoints())
  {
    XU32 index = GetVertexIndex(face_vertex_index);
    if (index<m_Mesh->GetNumPoints())
    {
      point = m_Mesh->Points()[index];
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetNormal(XVector3& normal,const XU32 face_vertex_index) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetVertexSet(XVertexSet::kNormal) : 0;
  return GetVertexData(face_vertex_index,*vset,normal);
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetTangent(XVector3& tangent,const XU32 face_vertex_index) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetVertexSet(XVertexSet::kTangent) : 0;
  return GetVertexData(face_vertex_index,*vset,tangent);
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetBiNormal(XVector3& binormal,const XU32 face_vertex_index) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetVertexSet(XVertexSet::kBiNormal) : 0;
  return GetVertexData(face_vertex_index,*vset,binormal);
}

//----------------------------------------------------------------------------------------------------------------------
/*  bool XItPolygon::GetColour(XColour3& colour,const XU32 face_vertex_index) const
{
  return GetColour(colour,face_vertex_index,0);
}*/

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetColour(XColour& colour,const XU32 face_vertex_index) const
{
  return GetColour(colour,face_vertex_index,0);
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetUvCoord(XReal& uv,const XU32 face_vertex_index) const
{
  return GetUvCoord(uv,face_vertex_index,0);
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetUvCoord(XVector2& uv,const XU32 face_vertex_index) const
{
  return GetUvCoord(uv,face_vertex_index,0);
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetUvCoord(XVector3& uv,const XU32 face_vertex_index) const
{
  return GetUvCoord(uv,face_vertex_index,0);
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetUvCoord(XVector4& uv,const XU32 face_vertex_index) const
{
  return GetUvCoord(uv,face_vertex_index,0);
}

//----------------------------------------------------------------------------------------------------------------------
/*  bool XItPolygon::GetColour(XColour3& colour,const XU32 face_vertex_index,const XU32 colour_set_index) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetColourSet(colour_set_index) : 0;
  return (vset!=0) ? GetVertexData(face_vertex_index,*vset,colour) : false;
}*/

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetColour(XColour& colour,const XU32 face_vertex_index,const XU32 colour_set_index) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetColourSet(colour_set_index) : 0;
  return (vset!=0) ? GetVertexData(face_vertex_index,*vset,colour) : false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetUvCoord(XReal& uv,const XU32 face_vertex_index,const XU32 uv_set_index) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetUvSet(uv_set_index) : 0;
  return (vset!=0) ? GetVertexData(face_vertex_index,*vset,uv) : false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetUvCoord(XVector2& uv,const XU32 face_vertex_index,const XU32 uv_set_index) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetUvSet(uv_set_index) : 0;
  return (vset!=0) ? GetVertexData(face_vertex_index,*vset,uv) : false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetUvCoord(XVector3& uv,const XU32 face_vertex_index,const XU32 uv_set_index) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetUvSet(uv_set_index) : 0;
  return (vset!=0) ? GetVertexData(face_vertex_index,*vset,uv) : false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetUvCoord(XVector4& uv,const XU32 face_vertex_index,const XU32 uv_set_index) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetUvSet(uv_set_index) : 0;
  return (vset!=0) ? GetVertexData(face_vertex_index,*vset,uv) : false;
}

//----------------------------------------------------------------------------------------------------------------------
/* 
bool XItPolygon::GetColour(XColour3& colour,const XU32 face_vertex_index,const XString& colour_set_name) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetColourSet(colour_set_name) : 0;
  return (vset!=0) ? GetVertexData(face_vertex_index,*vset,colour) : false;
}*/

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetColour(XColour& colour,const XU32 face_vertex_index,const XString& colour_set_name) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetColourSet(colour_set_name) : 0;
  return (vset!=0) ? GetVertexData(face_vertex_index,*vset,colour) : false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetUvCoord(XReal& uv,const XU32 face_vertex_index,const XString& uv_set_name) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetUvSet(uv_set_name) : 0;
  return (vset!=0) ? GetVertexData(face_vertex_index,*vset,uv) : false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetUvCoord(XVector2& uv,const XU32 face_vertex_index,const XString& uv_set_name) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetUvSet(uv_set_name) : 0;
  return (vset!=0) ? GetVertexData(face_vertex_index,*vset,uv) : false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetUvCoord(XVector3& uv,const XU32 face_vertex_index,const XString& uv_set_name) const
{
  const XVertexSet* vset = IsValid() ? m_Mesh->GetUvSet(uv_set_name) : 0;
  return (vset!=0) ? GetVertexData(face_vertex_index,*vset,uv) : false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetUvCoord(XVector4& uv,const XU32 face_vertex_index,const XString& uv_set_name) const
{
  const XVertexSet* vset = (m_Mesh!=0) ? m_Mesh->GetUvSet(uv_set_name) : 0;
  return (vset!=0) ? GetVertexData(face_vertex_index,*vset,uv) : false;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XItPolygon::GetVertexIndex(const XU32 face_vertex_index) const
{
  XIndexSet* vset = IsValid() ? m_Mesh->m_PointSet : 0;
  return vset ? GetIndex(face_vertex_index,vset) : face_vertex_index + m_CurrentVertexIndex;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XItPolygon::GetNormalIndex(const XU32 face_vertex_index) const
{
  XVertexSet* vset = IsValid() ? m_Mesh->GetVertexSet(XVertexSet::kNormal) : 0;
  return vset ? GetIndex(face_vertex_index,vset) : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XItPolygon::GetBiNormalIndex(const XU32 face_vertex_index) const
{
  XVertexSet* vset = IsValid() ? m_Mesh->GetVertexSet(XVertexSet::kBiNormal) : 0;
  return vset ? GetIndex(face_vertex_index,vset) : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XItPolygon::GetTangentIndex(const XU32 face_vertex_index) const
{
  XVertexSet* vset = IsValid() ? m_Mesh->GetVertexSet(XVertexSet::kTangent) : 0;
  return vset ? GetIndex(face_vertex_index,vset) : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XItPolygon::GetColourIndex(const XU32 face_vertex_index) const
{
  XVertexSet* vset = IsValid() ? m_Mesh->GetColourSet(0) : 0;
  return vset ? GetIndex(face_vertex_index,vset) : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XItPolygon::GetColourIndex(const XU32 face_vertex_index,const XU32 colour_set_index) const
{
  XVertexSet* vset = IsValid() ? m_Mesh->GetColourSet(colour_set_index) : 0;
  return vset ? GetIndex(face_vertex_index,vset) : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XItPolygon::GetColourIndex(const XU32 face_vertex_index,const XString& colour_set_name) const
{
  XVertexSet* vset = IsValid() ? m_Mesh->GetColourSet(colour_set_name) : 0;
  return vset ? GetIndex(face_vertex_index,vset) : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XItPolygon::GetUvCoordIndex(const XU32 face_vertex_index) const
{
  XVertexSet* vset = IsValid() ? m_Mesh->GetUvSet(0) : 0;
  return vset ? GetIndex(face_vertex_index,vset) : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XItPolygon::GetUvCoordIndex(const XU32 face_vertex_index,const XU32 uv_set_index) const
{
  XVertexSet* vset = IsValid() ? m_Mesh->GetUvSet(uv_set_index) : 0;
  return vset ? GetIndex(face_vertex_index,vset) : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XItPolygon::GetUvCoordIndex(const XU32 face_vertex_index,const XString& uv_set_name) const
{
  XVertexSet* vset = IsValid() ? m_Mesh->GetUvSet(uv_set_name) : 0;
  return vset ? GetIndex(face_vertex_index,vset) : 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XU32 vertex_set_index,XReal& data) const
{
  if(IsValid())
  {
    const XVertexSet* vertex_set = m_Mesh->GetVertexSet(vertex_set_index);
    if (vertex_set)
    {
      return GetVertexData(face_vertex_index,*vertex_set,data);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XU32 vertex_set_index,XVector2& data) const
{
  if(IsValid())
  {
    const XVertexSet* vertex_set = m_Mesh->GetVertexSet(vertex_set_index);
    if (vertex_set)
    {
      return GetVertexData(face_vertex_index,*vertex_set,data);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XU32 vertex_set_index,XVector3& data) const
{
  if(IsValid())
  {
    const XVertexSet* vertex_set = m_Mesh->GetVertexSet(vertex_set_index);
    if (vertex_set)
    {
      return GetVertexData(face_vertex_index,*vertex_set,data);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XU32 vertex_set_index,XVector4& data) const
{
  if(IsValid())
  {
    const XVertexSet* vertex_set = m_Mesh->GetVertexSet(vertex_set_index);
    if (vertex_set)
    {
      return GetVertexData(face_vertex_index,*vertex_set,data);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XU32 vertex_set_index,XColour& data) const
{
  if(IsValid())
  {
    const XVertexSet* vertex_set = m_Mesh->GetVertexSet(vertex_set_index);
    if (vertex_set)
    {
      return GetVertexData(face_vertex_index,*vertex_set,data);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XString& vertex_set_name,XReal& data) const
{
  if(IsValid())
  {
    const XVertexSet* vertex_set = m_Mesh->GetVertexSet(vertex_set_name);
    if (vertex_set)
    {
      return GetVertexData(face_vertex_index,*vertex_set,data);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XString& vertex_set_name,XVector2& data) const
{
  if(IsValid())
  {
    const XVertexSet* vertex_set = m_Mesh->GetVertexSet(vertex_set_name);
    if (vertex_set)
    {
      return GetVertexData(face_vertex_index,*vertex_set,data);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XString& vertex_set_name,XVector3& data) const
{
  if(IsValid())
  {
    const XVertexSet* vertex_set = m_Mesh->GetVertexSet(vertex_set_name);
    if (vertex_set)
    {
      return GetVertexData(face_vertex_index,*vertex_set,data);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XString& vertex_set_name,XVector4& data) const
{
  if(IsValid())
  {
    const XVertexSet* vertex_set = m_Mesh->GetVertexSet(vertex_set_name);
    if (vertex_set)
    {
      return GetVertexData(face_vertex_index,*vertex_set,data);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XString& vertex_set_name,XColour& data) const
{
  if(IsValid())
  {
    const XVertexSet* vertex_set = m_Mesh->GetVertexSet(vertex_set_name);
    if (vertex_set)
    {
      return GetVertexData(face_vertex_index,*vertex_set,data);
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XVertexSet& vertex_set,XReal& data) const
{
  if (IsValid() && face_vertex_index<GetNumPoints())
  {
    const XIndexSet* indices = vertex_set.GetIndexSet();
    if (indices)
    {
      return vertex_set.GetElement( data, (*indices)[ face_vertex_index + m_CurrentVertexIndex ] );
    }
    else
    {
      return vertex_set.GetElement( data, face_vertex_index+m_CurrentVertexIndex );
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XVertexSet& vertex_set,XVector2& data) const
{
  if (IsValid() && face_vertex_index<GetNumPoints())
  {
    const XIndexSet* indices = vertex_set.GetIndexSet();
    if (indices)
    {
      return vertex_set.GetElement( data, (*indices)[ face_vertex_index + m_CurrentVertexIndex ] );
    }
    else
    {
      return vertex_set.GetElement( data, face_vertex_index+m_CurrentVertexIndex );
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XVertexSet& vertex_set,XVector3& data) const
{
  if (IsValid() && face_vertex_index<GetNumPoints())
  {
    const XIndexSet* indices = vertex_set.GetIndexSet();
    if (indices)
    {
      return vertex_set.GetElement( data, (*indices)[ face_vertex_index + m_CurrentVertexIndex ] );
    }
    else
    {
      return vertex_set.GetElement( data, face_vertex_index+m_CurrentVertexIndex );
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XVertexSet& vertex_set,XVector4& data) const
{
  if (IsValid() && face_vertex_index<GetNumPoints())
  {
    const XIndexSet* indices = vertex_set.GetIndexSet();
    if (indices)
    {
      return vertex_set.GetElement( data, (*indices)[ face_vertex_index + m_CurrentVertexIndex ] );
    }
    else
    {
      return vertex_set.GetElement( data, face_vertex_index+m_CurrentVertexIndex );
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XItPolygon::GetVertexData(const XU32 face_vertex_index,const XVertexSet& vertex_set,XColour& data) const
{
  if (IsValid() && face_vertex_index<GetNumPoints())
  {
    const XIndexSet* indices = vertex_set.GetIndexSet();
    if (indices)
    {
      return vertex_set.GetElement( data, (*indices)[ face_vertex_index + m_CurrentVertexIndex ] );
    }
    else
    {
      return vertex_set.GetElement( data, face_vertex_index+m_CurrentVertexIndex );
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XItPolygon::GetIndex(const XU32 face_vertex_index,const XIndexSet* index_set) const
{
  if (index_set && IsValid())
  {
    return (*index_set)[ face_vertex_index + m_CurrentVertexIndex ];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XItPolygon::GetIndex(const XU32 face_vertex_index,const XVertexSet* vertex_set) const
{
  if (vertex_set && IsValid())
  {
    const XIndexSet* iset = vertex_set->GetIndexSet();
    return (iset!=0) ? GetIndex(face_vertex_index,iset) : face_vertex_index + m_CurrentVertexIndex;
  }
  return 0;
}
}
