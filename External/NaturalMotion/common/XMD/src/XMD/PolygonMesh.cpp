//----------------------------------------------------------------------------------------------------------------------
/// \file PolygonMesh.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Model.h"
#include "XMD/PolygonMesh.h"
#include "XMD/BlendShape.h"
#include "XMD/Cluster.h"
#include "XMD/JointCluster.h"
#include "XMD/SkinCluster.h"
#include "XMD/FileIO.h"
#include "XMD/ItPolygon.h"
#include "nmtl/algorithm.h"
#include <map>


namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
const XChar* XIndexSet::GetName() const
{
  return m_Name.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
bool XIndexSet::SetName(const XChar* name)
{
  XMD_ASSERT(m_Mesh);

  // ignore if name is the same
  if (m_Name==name)
    return true;

  // ignore if we have an identically named set
  if(m_Mesh->GetIndexSet(name))
    return false;

  m_Name = name;


  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XS32& XIndexSet::operator[](const XU32 index)
{
  static XS32 nullValue=0;
  bool test = index < GetSize();
  XMD_ASSERT(test);
  return test ? m_Indices[index] : nullValue;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XIndexSet::operator[](const XU32 index) const
{
  bool test = index < GetSize();
  XMD_ASSERT(test);
  return test ? m_Indices[index] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XS32& XIndexSet::GetIndex(const XU32 index)
{
  static XS32 nullValue=0;
  bool test = index < GetSize();
  XMD_ASSERT(test);
  return test ? m_Indices[index] : nullValue;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XIndexSet::GetIndex(const XU32 index) const
{
  bool test = index < GetSize();
  XMD_ASSERT(test);
  return test ? m_Indices[index] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XIndexSet::SetData(const XS32Array& array)
{
  if(m_Indices.size()==array.size())
  {
    m_Indices = array;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XIndexSet::GetSize() const
{
  return static_cast<XU32>( m_Indices.size() );
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XIndexSet::GetNumVertexSets() const
{
  XMD_ASSERT(m_Mesh);
  XU32 count=0;
  for (XU32 i=0;i!=m_Mesh->GetNumVertexSets();++i)
  {
    const XVertexSet* vset = m_Mesh->GetVertexSet(i);
    if (vset && (vset->GetIndexSet()==this))
    {
      ++count;
    }
  }
  return count;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet* XIndexSet::GetVertexSet(const XU32 idx)
{
  XMD_ASSERT(m_Mesh);
  XU32 count=0;
  for (XU32 i=0;i!=m_Mesh->GetNumVertexSets();++i)
  {
    XVertexSet* vset = m_Mesh->GetVertexSet(i);
    if (vset && (vset->GetIndexSet()==this))
    {
      if (count==idx)
      {
        return vset;
      }
      ++count;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVertexSet* XIndexSet::GetVertexSet(const XU32 idx) const
{
  XMD_ASSERT(m_Mesh);
  XU32 count=0;
  for (XU32 i=0;i!=m_Mesh->GetNumVertexSets();++i)
  {
    const XVertexSet* vset = m_Mesh->GetVertexSet(i);
    if (vset && (vset->GetIndexSet()==this))
    {
      if (count==idx)
      {
        return vset;
      }
      ++count;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XIndexSet::XIndexSet(XPolygonMesh* mesh,const XChar* name,const XU32 size)
  : m_Name(name), m_Indices(), m_Mesh(mesh)
{
  m_Indices.resize(size,0);
}

//----------------------------------------------------------------------------------------------------------------------
XIndexSet::XIndexSet(XPolygonMesh* mesh,const XU32 size)
  : m_Name(), m_Indices(), m_Mesh(mesh)
{
  m_Indices.resize(size);
}

//----------------------------------------------------------------------------------------------------------------------
bool XIndexSet::DoData(XFileIO& io)
{
  DUMPER(XIndexSet__DoData);
  IO_CHECK( io.DoDataString(m_Name) );
  DPARAM(m_Name);
  IO_CHECK( io.DoDataVector(m_Indices) );
  DAPARAM(m_Indices);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XIndexSet::GetDataSize() const
{
  XU32 sz = sizeof(XU16) + sizeof(XU32);
  sz += (XU32)m_Name.size();
  sz += (XU32)m_Indices.size() * sizeof(XU32);
  return sz;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet::XVertexSet(XPolygonMesh* mesh,const XChar* name,const XU32 elem_size,const XU32 num_elems)
  : m_Name(name),m_VertexData(),m_Usage(kNone),m_Mesh(mesh)
{
  m_VertexData.resize(elem_size * num_elems);
  m_ElementSize = elem_size;
  m_NumItems = num_elems;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet::XVertexSet(XPolygonMesh* mesh)
  : m_Name(),m_VertexData(),m_Usage(kNone),m_Mesh(mesh)
{
  m_ElementSize = 0;
  m_NumItems = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void XVertexSet::SetUsage(const XVertexSet::VertexSemantic semantic)
{
  m_Usage = semantic;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet::VertexSemantic XVertexSet::GetUsage() const
{
  return m_Usage;
}

//----------------------------------------------------------------------------------------------------------------------
const XString& XVertexSet::GetName() const
{
  return m_Name;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetName(const XString& name)
{
  if (m_Mesh->GetVertexSet(name))
  {
    return false;
  }
  m_Name=name;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XVertexSet::GetNumElements() const
{
  return m_NumItems;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XVertexSet::GetElementSize() const
{
  return m_ElementSize;
}

//----------------------------------------------------------------------------------------------------------------------
XIndexSet* XVertexSet::CreateIndexSet()
{
  XMD_ASSERT(m_Mesh);
  if(!m_Mesh)
    return 0;
  XIndexSet* indices = m_Mesh->CreateIndexSet(GetName());
  SetIndexSet(indices);
  return indices;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetIndexSet(XIndexSet* indices)
{
  XMD_ASSERT(m_Mesh);
  if(!m_Mesh)
    return false;
  if(!m_Mesh->IsSetValid(indices))
    return false;
  m_IndexSetName = indices ? indices->GetName() : "";
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XIndexSet* XVertexSet::GetIndexSet()
{
  XMD_ASSERT(m_Mesh);
  if(!m_Mesh)
    return 0;
  return m_Mesh->GetIndexSet(m_IndexSetName);
}

//----------------------------------------------------------------------------------------------------------------------
const XIndexSet* XVertexSet::GetIndexSet() const
{
  XMD_ASSERT(m_Mesh);
  if(!m_Mesh)
    return 0;
  return m_Mesh->GetIndexSet(m_IndexSetName);
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetData(const XReal* data,const XU32 num_elements,const XU32 element_size)
{
  m_ElementSize = element_size;
  m_VertexData.resize( m_ElementSize * num_elements );
  XRealArray::iterator it = m_VertexData.begin();
  for (;it != m_VertexData.end(); ++it,++data)
  {
    *it = *data;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetData(const XRealArray& data)
{
  m_ElementSize=1;
  m_VertexData = data;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetData(const XVector2Array& data)
{
  m_ElementSize = 2;
  m_VertexData.resize(m_ElementSize*data.size());

  XVector2Array::const_iterator itd = data.begin();
  XRealArray::iterator it = m_VertexData.begin();
  for (;itd != data.end();++itd)
  {
    *it = itd->x; ++it;
    *it = itd->y; ++it;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetData(const XVector3Array& data)
{
  m_ElementSize = 3;
  m_VertexData.resize(m_ElementSize*data.size());

  XVector3Array::const_iterator itd = data.begin();
  XRealArray::iterator it = m_VertexData.begin();
  for (;itd != data.end();++itd)
  {
    *it = itd->x; ++it;
    *it = itd->y; ++it;
    *it = itd->z; ++it;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetData(const XVector4Array& data)
{
  m_ElementSize = 4;
  m_VertexData.resize(m_ElementSize*data.size());

  XVector4Array::const_iterator itd = data.begin();
  XRealArray::iterator it = m_VertexData.begin();
  for (;itd != data.end();++itd)
  {
    *it = itd->x; ++it;
    *it = itd->y; ++it;
    *it = itd->z; ++it;
    *it = itd->w; ++it;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetData(const XColourArray& data)
{
  m_ElementSize = 4;
  m_VertexData.resize(m_ElementSize*data.size());

  XColourArray::const_iterator itd = data.begin();
  XRealArray::iterator it = m_VertexData.begin();
  for (;itd != data.end();++itd)
  {
    *it = itd->r; ++it;
    *it = itd->g; ++it;
    *it = itd->b; ++it;
    *it = itd->a; ++it;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::GetData(XReal* data) const
{
  XRealArray::const_iterator it = m_VertexData.begin();
  for (;it != m_VertexData.end();++it,++data)
  {
    *data = *it;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::GetData(XRealArray& data) const
{
  if(m_ElementSize==1) 
  {
    data = m_VertexData;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::GetData(XVector2Array& data) const
{
  if (m_ElementSize==2)
  {
    XMD_ASSERT((m_VertexData.size()%2)==0);
    data.resize(GetNumElements());
    XVector2Array::iterator ita = data.begin();
    XRealArray::const_iterator it = m_VertexData.begin();
    for (;it != m_VertexData.end();++ita)
    {
      ita->x = *it; ++it;
      ita->y = *it; ++it;
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::GetData(XVector3Array& data) const
{
  if (m_ElementSize==3)
  {
    XMD_ASSERT((m_VertexData.size()%3)==0);
    data.resize(GetNumElements());
    XVector3Array::iterator ita = data.begin();
    XRealArray::const_iterator it = m_VertexData.begin();
    for (;it != m_VertexData.end();++ita)
    {
      ita->x = *it; ++it;
      ita->y = *it; ++it;
      ita->z = *it; ++it;
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::GetData(XVector4Array& data) const
{
  if (m_ElementSize==4)
  {
    XMD_ASSERT((m_VertexData.size()%4)==0);
    data.resize(GetNumElements());
    XVector4Array::iterator ita = data.begin();
    XRealArray::const_iterator it = m_VertexData.begin();
    for (;it != m_VertexData.end();++ita)
    {
      ita->x = *it; ++it;
      ita->y = *it; ++it;
      ita->z = *it; ++it;
      ita->w = *it; ++it;
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::GetData(XColourArray& data) const
{
  if (m_ElementSize==4)
  {
    XMD_ASSERT((m_VertexData.size()%4)==0);
    data.resize(GetNumElements());
    XColourArray::iterator ita = data.begin();
    XRealArray::const_iterator it = m_VertexData.begin();
    for (;it != m_VertexData.end();++ita)
    {
      ita->r = *it; ++it;
      ita->g = *it; ++it;
      ita->b = *it; ++it;
      ita->a = *it; ++it;
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::GetElement(XReal& data,const XU32 index) const
{
  if ((index<GetNumElements()) && (m_ElementSize == 1))
  {
    data = m_VertexData[index];
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::GetElement(XVector2& data,const XU32 index) const
{
  if ((index<GetNumElements()) && (m_ElementSize == 2))
  {
    data.x = m_VertexData[2*index];
    data.y = m_VertexData[2*index+1];
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::GetElement(XVector3& data,const XU32 index) const
{
  if ((index<GetNumElements()) && (m_ElementSize == 3))
  {
    data.x = m_VertexData[3*index];
    data.y = m_VertexData[3*index+1];
    data.z = m_VertexData[3*index+2];
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::GetElement(XVector4& data,const XU32 index) const
{
  if ((index<GetNumElements()) && (m_ElementSize == 4))
  {
    data.x = m_VertexData[4*index];
    data.y = m_VertexData[4*index+1];
    data.z = m_VertexData[4*index+2];
    data.w = m_VertexData[4*index+3];
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::GetElement(XColour& data,const XU32 index) const
{
  if ((index<GetNumElements()) && (m_ElementSize == 4))
  {
    data.r = m_VertexData[4*index];
    data.g = m_VertexData[4*index+1];
    data.b = m_VertexData[4*index+2];
    data.a = m_VertexData[4*index+3];
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetElement(const XReal& data,const XU32 index) 
{
  if (index<GetNumElements() && m_ElementSize==1)
  {
    m_VertexData[index] = data;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetElement(const XVector2& data,const XU32 index) 
{
  if ((index<GetNumElements()) && (m_ElementSize==2))
  {
    m_VertexData[2*index] = data.x;
    m_VertexData[2*index+1] = data.y;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetElement(const XVector3& data,const XU32 index) 
{
  if ((index<GetNumElements()) && (m_ElementSize==3))
  {
    m_VertexData[3*index] = data.x;
    m_VertexData[3*index+1] = data.y;
    m_VertexData[3*index+2] = data.z;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetElement(const XVector4& data,const XU32 index) 
{
  if ((index<GetNumElements()) && (m_ElementSize==4))
  {
    m_VertexData[4*index] = data.x;
    m_VertexData[4*index+1] = data.y;
    m_VertexData[4*index+2] = data.z;
    m_VertexData[4*index+3] = data.w;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::SetElement(const XColour& data,const XU32 index) 
{
  if ((index<GetNumElements()) && (m_ElementSize==4))
  {
    m_VertexData[4*index] = data.r;
    m_VertexData[4*index+1] = data.g;
    m_VertexData[4*index+2] = data.b;
    m_VertexData[4*index+3] = data.a;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XVertexSet::DoData(XFileIO& io)
{
  DUMPER(XVertexSet__DoData);
  IO_CHECK( io.DoDataString(m_Name) );
  DPARAM(m_Name);
  IO_CHECK( io.DoData(&m_NumItems) );
  DPARAM(m_NumItems);
  IO_CHECK( io.DoData(&m_ElementSize) );
  DPARAM(m_ElementSize);
  IO_CHECK( io.DoData(&m_Usage) );
  DPARAM(m_Usage);
  IO_CHECK( io.DoDataString(m_IndexSetName) );
  DPARAM(m_IndexSetName);
  IO_CHECK( io.DoDataVector(m_VertexData) );
  DAPARAM(m_VertexData);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XVertexSet::GetDataSize() const
{
  XU32 sz = 2*sizeof(XU16) + 
            3*sizeof(XU32) + 
            sizeof(VertexSemantic);
  sz += (XU32)m_Name.size();
  sz += (XU32)m_IndexSetName.size() * sizeof(XS8);
  sz += (XU32)m_VertexData.size() * sizeof(XReal);
  return sz;
}

//----------------------------------------------------------------------------------------------------------------------
XPolygonMesh::XPolygonMesh(XModel* model) 
  : XGeometry(model),m_PolyCounts(),m_NumRequiredIndices(0),
    m_VertexSets(),m_IndexSets(),m_PointSet(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XPolygonMesh::~XPolygonMesh()
{
  Clear();
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XPolygonMesh::GetApiType() const
{
  return XFn::PolygonMesh;
}

//----------------------------------------------------------------------------------------------------------------------
void XPolygonMesh::ReversePolyWindings()
{
  // just need to modify each index set
  for(XIndexSetArray::iterator it = m_IndexSets.begin(); it != m_IndexSets.end(); ++it)
  {
    XIndexSet* iset = *it;

    unsigned offset = 0;
    XU32Array::iterator iti = m_PolyCounts.begin();
    for (;iti != m_PolyCounts.end(); ++iti)
    {
      std::reverse( iset->m_Indices.buffer()+offset, iset->m_Indices.buffer()+offset+(*iti) );

      offset+=(*iti);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XPolygonMesh::DoCopy(const XBase* rhs)
{
  XGeometry::DoCopy(rhs);

  const XPolygonMesh* orig = rhs->HasFn<XPolygonMesh>();
  XMD_ASSERT(orig);

  for(XIndexSetArray::const_iterator it = orig->m_IndexSets.begin(); it != orig->m_IndexSets.end(); ++it)
  {
    XIndexSet* copy_set = CreateIndexSet((*it)->m_Name);
    copy_set->m_Indices = (*it)->m_Indices;
  }

  for(XVertexSetArray::const_iterator it = orig->m_VertexSets.begin(); it != orig->m_VertexSets.end(); ++it)
  {
    XVertexSet* copy_set = CreateVertexSet((*it)->m_Name,
                                           (*it)->m_NumItems,
                                           (*it)->m_ElementSize,
                                           (*it)->m_Usage);
    copy_set->m_VertexData = (*it)->m_VertexData;
    copy_set->m_IndexSetName = (*it)->m_IndexSetName;
  }

  const XIndexSet* orig_points_set = orig->GetPointsIndexSet();
  m_PointSet = GetIndexSet(orig_points_set->GetName());

  m_PolyCounts = orig->m_PolyCounts;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XPolygonMesh::GetFn(XFn::Type type)
{
  if (type == XFn::PolygonMesh)
    return (XPolygonMesh*)this;
  return XGeometry::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XPolygonMesh::GetFn(XFn::Type type) const
{
  if (type == XFn::PolygonMesh)
    return (const XPolygonMesh*)this;
  return XGeometry::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::NodeDeath(XId id)
{
  return XGeometry::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XPolygonMesh::PreDelete(XIdSet& nodes)
{
  XGeometry::PreDelete(nodes);
}

//----------------------------------------------------------------------------------------------------------------------
void do_read_string(std::istream& ifs,XString& name)
{
  XChar c=(XChar)ifs.get();
  while( c == ' ' || c == '\t' )
    c=(XChar)ifs.get();
  XMD_ASSERT(c=='\"');
  c=(XChar)ifs.get();// skip "
  while( c != '\"' && !ifs.eof())
  {
    name += c;
    c=(XChar)ifs.get();
  }
  XMD_ASSERT(c=='\"');
}

//----------------------------------------------------------------------------------------------------------------------
void do_read_usage(std::istream& ifs, XVertexSet::VertexSemantic& semantic)
{
  std::string str;
  ifs >> str;
  if(str == "kNone")
  {
    semantic = XVertexSet::kNone;
  }
  else if (str == "kVertex")
  {
    semantic = XVertexSet::kVertex;
  }
  else if (str == "kFogCoord")
  {
    semantic = XVertexSet::kFogCoord;
  }
  else if (str == "kTexCoord")
  {
    semantic = XVertexSet::kTexCoord;
  }
  else if (str == "kColour")
  {
    semantic = XVertexSet::kColour;
  }
  else if (str == "kNormal")
  {
    semantic = XVertexSet::kNormal;
  }
  else if (str == "kBiNormal" || str == "kBiTangent")
  {
    semantic = XVertexSet::kBiNormal;
  }
  else if (str == "kTangent")
  {
    semantic = XVertexSet::kTangent;
  }
  else if (str == "kTBN")
  {
    semantic = XVertexSet::kTBN;
  }
}

//----------------------------------------------------------------------------------------------------------------------
std::string usage_as_string(const XVertexSet::VertexSemantic& semantic)
{
  switch(semantic)
  {
    default:
    case XVertexSet::kNone:
      return "kNone";
    case XVertexSet::kVertex:
      return "kVertex";
    case XVertexSet::kFogCoord:
      return "kFogCoord";
    case XVertexSet::kTexCoord:
      return "kTexCoord";
    case XVertexSet::kColour:
      return "kColour";
    case XVertexSet::kNormal:
      return "kNormal";
    case XVertexSet::kTangent:
      return "kTangent";
    case XVertexSet::kBiNormal:
      return "kBiNormal";
    case XVertexSet::kTBN:
      return "kTBN";
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XPolygonMesh::GetNumFaces() const
{
  return static_cast<XU32>(m_PolyCounts.size());
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::ReadChunk(std::istream& ifs)
{
  if (!XGeometry::ReadChunk(ifs))
    return false;

  READ_CHECK("point_index_set",ifs);
  XString point_index_set;
  do_read_string(ifs,point_index_set);

  // poly count data
  {
    READ_CHECK("POLY_COUNTS",ifs);
    XU32 num_polys;
    ifs >> num_polys;
    m_NumRequiredIndices=0;

    READ_CHECK("{",ifs);
    m_PolyCounts.resize(num_polys);
    XU32Array::iterator it = m_PolyCounts.begin();
    for (;it != m_PolyCounts.end();++it)
    {
      ifs >> *it;
      if (*it < 3)
      {
        XGlobal::GetLogger()->Log(XBasicLogger::kError,
          "Poly count data contains a poly with less than 3 sides!");
        return false;
      }
      m_NumRequiredIndices += *it;
    }
    READ_CHECK("}",ifs);
  }

  // index sets
  {
    XU32 num_index_sets;
    READ_CHECK("num_index_sets",ifs);
    ifs >> num_index_sets;

    for (XU32 i=0;i!=num_index_sets;++i)
    {
      XIndexSet* ind_set = new XIndexSet(this,m_NumRequiredIndices);
      READ_CHECK("INDEX_SET",ifs);
      do_read_string(ifs,ind_set->m_Name);
      READ_CHECK("{",ifs);

      if (ind_set->m_Name == point_index_set)
        m_PointSet = ind_set;

      for (XU32 i=0;i<m_NumRequiredIndices;++i)
      {
        ifs >> (*ind_set)[i];
      }
      m_IndexSets.push_back(ind_set);

      READ_CHECK("}",ifs);
    }
  }

  // vertex sets
  {
    XU32 num_vertex_sets;
    READ_CHECK("num_vertex_sets",ifs);
    ifs >> num_vertex_sets;

    for (XU32 i=0;i!=num_vertex_sets;++i)
    {
      XVertexSet* v_set = new XVertexSet(this);
      m_VertexSets.push_back(v_set);
      READ_CHECK("VERTEX_SET",ifs);
      do_read_string(ifs,v_set->m_Name);
      READ_CHECK("{",ifs);
      READ_CHECK("num_elements",ifs);
      ifs >> v_set->m_NumItems;
      READ_CHECK("element_size",ifs);
      ifs >> v_set->m_ElementSize;
      READ_CHECK("usage",ifs);
      do_read_usage(ifs, v_set->m_Usage);

      READ_CHECK("index_set",ifs);
      do_read_string(ifs,v_set->m_IndexSetName);
      v_set->m_VertexData.resize( v_set->m_NumItems * v_set->m_ElementSize );

      READ_CHECK("elements",ifs);
      XRealArray::iterator it = v_set->m_VertexData.begin();
      for (;it != v_set->m_VertexData.end();++it)
      {
        ifs >> *it;
      }

      READ_CHECK("}",ifs);
    }
  }
  return ifs.good();  
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::WriteChunk(std::ostream& ofs)
{
  if (!XGeometry::WriteChunk(ofs))
    return false;

  ofs << "\tpoint_index_set ";
  if (m_PointSet)
    ofs << "\"" << m_PointSet->GetName() << "\"\n";
  else
    ofs << "\"none\"\n";

  ofs << "\tPOLY_COUNTS " << m_PolyCounts.size() << "\n";
  ofs << "\t{\n";
  {
    XU32Array::const_iterator it = m_PolyCounts.begin();
    for (;it != m_PolyCounts.end(); ++it)
    {
      ofs << "\t\t" << *it << "\n";
    }
  }
  ofs << "\t}\n";

  ofs << "\tnum_index_sets " << m_IndexSets.size() << "\n";
  {
    XIndexSetArray::const_iterator it = m_IndexSets.begin();
    for (;it != m_IndexSets.end(); ++it)
    {
      const XIndexSet& is = *(*it);
      ofs << "\tINDEX_SET \"" << is.GetName() << "\"\n\t{\n";

      // write the indices out nicely so that they match the polycounts
      XU32 idx=0;
      XU32Array::const_iterator itpc = m_PolyCounts.begin();
      for (;itpc != m_PolyCounts.end(); ++itpc)
      {
        ofs << "\t\t";
        for (XU32 j=0;j<*itpc;++j,++idx)
        {
          ofs << is[idx] << " ";
        }
        ofs << "\n";
      }
      ofs << "\t}\n";
    }
  }

  ofs << "\tnum_vertex_sets " << m_VertexSets.size() << "\n";
  {
    XVertexSetArray::const_iterator it = m_VertexSets.begin();
    for (;it != m_VertexSets.end(); ++it)
    {
      const XVertexSet& vs = **it;
      ofs << "\tVERTEX_SET \"" << vs.GetName() << "\"\n\t{\n";
      ofs << "\t\tnum_elements " << vs.GetNumElements() << "\n";
      ofs << "\t\telement_size " << vs.GetElementSize() << "\n";
      ofs << "\t\tusage " << usage_as_string(vs.GetUsage()) << "\n";
      ofs << "\t\tindex_set \"" << vs.m_IndexSetName << "\"\n";
      ofs << "\t\telements\n";

      XRealArray::const_iterator itv = vs.m_VertexData.begin();
      for (XU32 j=0;j<vs.GetNumElements();++j)
      {
        ofs << "\t\t\t";
        for (XU32 k=0;k<vs.GetElementSize();++k,++itv)
        {
          ofs << *itv << " ";
        }
        ofs << "\n";
      }
      ofs << "\t}\n";
    }
  }
  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::HasUvSet(const XChar* uv_set_name) const
{
  return GetUvSet(uv_set_name)!=0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::DoData(XFileIO& io)
{
  DUMPER(XPolygonMesh__DoData);
  if(!XGeometry::DoData(io))
    return false;

  XString point_set_name;
  if (m_PointSet)
    point_set_name = m_PointSet->GetName();
  else
    point_set_name = "";

  IO_CHECK(io.DoDataString(point_set_name));
  DPARAM(point_set_name);

  IO_CHECK(io.DoDataVector(m_PolyCounts));
  DAPARAM(m_PolyCounts);

  XU32 num_index_sets = (XU32)m_IndexSets.size();

  IO_CHECK( io.DoData(&num_index_sets) );
  DPARAM(num_index_sets);

  if (io.IsReading())
  {
    for (XU32 i=0;i!=num_index_sets;++i)
    {
      m_IndexSets.push_back(new XIndexSet(this,0));
    }
  }

  {
    XIndexSetArray::iterator it = m_IndexSets.begin();
    for (;it != m_IndexSets.end();++it)
    {
      IO_CHECK( (*it)->DoData(io) );
    }
  }

  XU32 num_vertex_sets = (XU32)m_VertexSets.size();

  IO_CHECK( io.DoData(&num_vertex_sets) );
  DPARAM(num_vertex_sets);
  if (io.IsReading())
  {
    for (XU32 i=0;i!=num_vertex_sets;++i)
    {
      m_VertexSets.push_back(new XVertexSet(this));
    }
  }

  {
    XVertexSetArray::iterator it = m_VertexSets.begin();
    for (;it != m_VertexSets.end();++it)
    {
      IO_CHECK( (*it)->DoData(io) );
    }
  }

  // re-connect the point index set
  if (io.IsReading())
  {
    XIndexSetArray::iterator it = m_IndexSets.begin();
    for (;it != m_IndexSets.end();++it)
    {
      if (point_set_name == (*it)->GetName())
      {
        m_PointSet = *it;
        break;
      }
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XPolygonMesh::GetDataSize() const
{
  XString point_set_name;
  if (m_PointSet)
    point_set_name = m_PointSet->GetName();
  else
    point_set_name = "";

  XU32 sz = XGeometry::GetDataSize();
  sz += 3 * sizeof(XU32);
  sz += sizeof(XU16);
  sz += (XU32)point_set_name.size();
  sz += (XU32)(m_PolyCounts.size()*sizeof(XU32));

  {
    XIndexSetArray::const_iterator it = m_IndexSets.begin();
    for (;it != m_IndexSets.end();++it)
    {
      sz += (*it)->GetDataSize();
    }
  }

  {
    XVertexSetArray::const_iterator it = m_VertexSets.begin();
    for (;it != m_VertexSets.end();++it)
    {
      sz += (*it)->GetDataSize();
    }
  }
  return sz;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::SetPolyCounts(const XU32Array& poly_counts,const XS32Array& vertex_indices)
{
  XU32 total_vertex_count=0;
  {
    XU32Array::const_iterator it = poly_counts.begin();
    for (;it != poly_counts.end();++it)
    {
      if (*it < 3)
      {
        XGlobal::GetLogger()->Log(XBasicLogger::kError,
            "Cannot create a polygon with less than 3 vertices");
        return false;
      }
      total_vertex_count += *it;
    }

    if (total_vertex_count != vertex_indices.size())
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError,
          "The number of indices specified by the poly_counts array does "
          "not match the number provided in the vertex_indices array");
      return false;
    }

  }

  // ok to create the poly data... 
  Clear();

  m_NumRequiredIndices = total_vertex_count;
  m_PolyCounts = poly_counts;
  XIndexSet* indices = CreateIndexSet("points");
  indices->m_Indices = vertex_indices;
  m_PointSet = indices;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XPolygonMesh::GetPolyCounts(XU32Array& poly_counts)
{
  poly_counts = m_PolyCounts;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet* XPolygonMesh::CreateVertexSet(const XString& name,
                                          const XU32 num_vertices,
                                          const XU32 num_elements,
                                          const XVertexSet::VertexSemantic usage)
{
  XVertexSet* verts = new XVertexSet(this,name.c_str(),num_elements,num_vertices);
  verts->SetUsage(usage);
  m_VertexSets.push_back(verts);
  return verts;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::IsSetValid(const XVertexSet* vset)
{
  XVertexSet* temp = (XVertexSet*)vset;
  return m_VertexSets.end() != nmtl::find(m_VertexSets.begin(),m_VertexSets.end(),temp);
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet* XPolygonMesh::GetVertexSet(const XVertexSet::VertexSemantic& usage)
{
  XVertexSetArray::iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (usage == (*it)->GetUsage())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVertexSet* XPolygonMesh::GetVertexSet(const XVertexSet::VertexSemantic& usage) const
{
  XVertexSetArray::const_iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (usage == (*it)->GetUsage())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XPolygonMesh::GetNumVertexSets() const
{
  return static_cast<XU32>(m_VertexSets.size());
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet* XPolygonMesh::GetVertexSet(const XU32 index)
{
  return (index<GetNumVertexSets()) ? m_VertexSets[index] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVertexSet* XPolygonMesh::GetVertexSet(const XU32 index) const
{
  return (index<GetNumVertexSets()) ? m_VertexSets[index] : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet* XPolygonMesh::GetVertexSet(const XString& name)
{
  XVertexSetArray::iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (name == (*it)->GetName())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVertexSet* XPolygonMesh::GetVertexSet(const XString& name) const
{
  XVertexSetArray::const_iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (name == (*it)->GetName())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::RemoveVertexSet(XVertexSet* set)
{
  XMD_ASSERT(set);
  if(!set)
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "XVertexSet is NULL");
    return false;
  }
  XVertexSetArray::iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (set == *it)
    {
      XVertexSet* vertexSet = *it;
      m_VertexSets.erase(it);

      XIndexSet* indexSet = vertexSet->GetIndexSet();
      if(indexSet && indexSet->GetNumVertexSets() == 0)
      {
        RemoveIndexSet(indexSet);
      }
      delete vertexSet;
      set = 0;

      return true;
    }
  }
  XGlobal::GetLogger()->Logf(XBasicLogger::kError, "XVertexSet \"%s\" does not belong to XPolygonMesh \"%s\"", set->GetName().c_str(), GetName());
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::RemoveVertexSet(const XU32 index)
{
  if(index < GetNumVertexSets())
  {
    XVertexSet* vertexSet = m_VertexSets[index];
    m_VertexSets.erase(m_VertexSets.begin() + index);

    XIndexSet* indexSet = vertexSet->GetIndexSet();
    if(indexSet && indexSet->GetNumVertexSets() == 0)
    {
      RemoveIndexSet(indexSet);
    }
    delete vertexSet;

    return true;
  }
  XGlobal::GetLogger()->Logf(XBasicLogger::kError, "XVertexSet %d not found", index);
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::RemoveVertexSet(const XString& name)
{
  XVertexSetArray::iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (name == (*it)->GetName())
    {
      XVertexSet* vertexSet = *it;
      m_VertexSets.erase(it);

      XIndexSet* indexSet = vertexSet->GetIndexSet();
      if(indexSet && indexSet->GetNumVertexSets() == 0)
      {
        RemoveIndexSet(indexSet);
      }
      delete vertexSet;

      return true;
    }
  }
  XGlobal::GetLogger()->Logf(XBasicLogger::kError, "XVertexSet \"%s\" not found", name.c_str());
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet* XPolygonMesh::GetNormals()
{
  XVertexSetArray::iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (XVertexSet::kNormal == (*it)->GetUsage())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVertexSet* XPolygonMesh::GetNormals() const
{
  XVertexSetArray::const_iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (XVertexSet::kNormal == (*it)->GetUsage())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet* XPolygonMesh::GetTangents()
{
  XVertexSetArray::iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (XVertexSet::kTangent == (*it)->GetUsage())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVertexSet* XPolygonMesh::GetTangents() const
{
  XVertexSetArray::const_iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (XVertexSet::kTangent == (*it)->GetUsage())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet* XPolygonMesh::GetBiNormals()
{
  XVertexSetArray::iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (XVertexSet::kBiNormal == (*it)->GetUsage())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVertexSet* XPolygonMesh::GetBiNormals() const
{
  XVertexSetArray::const_iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (XVertexSet::kBiNormal == (*it)->GetUsage())
    {
      return *it;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XPolygonMesh::GetNumUvSet() const
{
  XU32 num_sets=0;
  XVertexSetArray::const_iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (XVertexSet::kTexCoord == (*it)->GetUsage())
    {
      ++num_sets;
    }
  }
  return num_sets;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet* XPolygonMesh::GetUvSet(const XU32 index)
{
  XU32 num_sets=0;
  XVertexSetArray::iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (XVertexSet::kTexCoord == (*it)->GetUsage())
    {
      if(index == num_sets)
        return *it;
      ++num_sets;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVertexSet* XPolygonMesh::GetUvSet(const XU32 index) const
{
  XU32 num_sets=0;
  XVertexSetArray::const_iterator  it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (XVertexSet::kTexCoord == (*it)->GetUsage())
    {
      if(index == num_sets)
        return *it;
      ++num_sets;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet* XPolygonMesh::GetUvSet(const XString& name)
{
  return GetVertexSet(name);
}

//----------------------------------------------------------------------------------------------------------------------
const XVertexSet* XPolygonMesh::GetUvSet(const XString& name) const
{
  return GetVertexSet(name);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XPolygonMesh::GetNumColourSet() const
{
  XU32 num_sets=0;
  XVertexSetArray::const_iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (XVertexSet::kColour == (*it)->GetUsage())
    {
      ++num_sets;
    }
  }
  return num_sets;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet* XPolygonMesh::GetColourSet(const XU32 index)
{
  XU32 num_sets=0;
  XVertexSetArray::iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (XVertexSet::kColour == (*it)->GetUsage())
    {
      if(index == num_sets)
        return *it;
      ++num_sets;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XVertexSet* XPolygonMesh::GetColourSet(const XU32 index) const
{
  XU32 num_sets=0;
  XVertexSetArray::const_iterator it = m_VertexSets.begin();
  for (;it != m_VertexSets.end(); ++it)
  {
    if (XVertexSet::kColour == (*it)->GetUsage())
    {
      if(index == num_sets)
        return *it;
      ++num_sets;
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XVertexSet* XPolygonMesh::GetColourSet(const XString& name)
{
  return GetVertexSet(name);
}

//----------------------------------------------------------------------------------------------------------------------
const XVertexSet* XPolygonMesh::GetColourSet(const XString& name) const
{
  return GetVertexSet(name);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XPolygonMesh::GetNumIndexSets() const
{
  return static_cast<XU32>(m_IndexSets.size());
}

//----------------------------------------------------------------------------------------------------------------------
XIndexSet* XPolygonMesh::CreateIndexSet(const XString& name)
{
  // make sure the name is not taken
  XIndexSet* indices = GetIndexSet(name);
  if(!indices)
  {
    indices = new XIndexSet(this,name.c_str(),m_NumRequiredIndices);
    m_IndexSets.push_back(indices);
  }
  return indices;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::IsSetValid(const XIndexSet* vset) const
{
  XIndexSetArray::const_iterator it = m_IndexSets.begin();
  for (;it != m_IndexSets.end(); ++it)
  {
    if (*it == vset)
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XIndexSet* XPolygonMesh::GetPointsIndexSet()
{
  return m_PointSet;
}

//----------------------------------------------------------------------------------------------------------------------
const XIndexSet* XPolygonMesh::GetPointsIndexSet() const
{
  return m_PointSet;
}

//----------------------------------------------------------------------------------------------------------------------
XIndexSet* XPolygonMesh::GetIndexSet(const XU32 index)
{
  if (index<GetNumIndexSets())
  {
    return m_IndexSets[index];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XIndexSet* XPolygonMesh::GetIndexSet(const XU32 index) const
{
  if (index<GetNumIndexSets())
  {
    return m_IndexSets[index];
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XIndexSet* XPolygonMesh::GetIndexSet(const XString& name)
{
  XIndexSetArray::iterator it = m_IndexSets.begin();
  for (;it != m_IndexSets.end(); ++it)
  {
    if (name == (*it)->GetName())
      return (*it);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XIndexSet* XPolygonMesh::GetIndexSet(const XString& name) const
{
  XIndexSetArray::const_iterator it = m_IndexSets.begin();
  for (;it != m_IndexSets.end(); ++it)
  {
    if (name == (*it)->GetName())
      return (*it);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::RemoveIndexSet(XIndexSet* set)
{
  XMD_ASSERT(set);
  if(!set)
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kError, "XIndexSet is NULL");
    return false;
  }
  XIndexSetArray::iterator it = m_IndexSets.begin();
  for (;it != m_IndexSets.end(); ++it)
  {
    if (set == *it)
    {
      XIndexSet* indexSet = *it;
      m_IndexSets.erase(it);

      // invalidate the associated VertexSet's IndexSets
      XU32 numVertexSets = indexSet->GetNumVertexSets();
      for(XU32 i = 0; i != numVertexSets; ++i)
      {
        XVertexSet* vertexSet = indexSet->GetVertexSet(i);
        vertexSet->SetIndexSet(0);
      }

      delete indexSet;
      set = 0;        

      return true;
    }
  }
  XGlobal::GetLogger()->Logf(XBasicLogger::kError, "XIndexSet \"%s\" does not belong to XPolygonMesh \"%s\"", set->GetName(), GetName());
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::RemoveIndexSet(const XU32 index)
{
  if(index < m_IndexSets.size())
  {
    XIndexSet* indexSet = m_IndexSets[index];
    m_IndexSets.erase(m_IndexSets.begin() + index);

    // invalidate the associated VertexSet's IndexSets
    XU32 numVertexSets = indexSet->GetNumVertexSets();
    for(XU32 i = 0; i != numVertexSets; ++i)
    {
      XVertexSet* vertexSet = indexSet->GetVertexSet(i);
      vertexSet->SetIndexSet(0);
    }

    delete indexSet;

    return true;
  }
  XGlobal::GetLogger()->Logf(XBasicLogger::kError, "XIndexSet %d not found", index);
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::RemoveIndexSet(const XString& name)
{
  XIndexSetArray::iterator it = m_IndexSets.begin();
  for (;it != m_IndexSets.end(); ++it)
  {
    if (name == (*it)->GetName())
    {
      XIndexSet* indexSet = *it;
      m_IndexSets.erase(it);

      // invalidate the associated VertexSet's IndexSets
      XU32 numVertexSets = indexSet->GetNumVertexSets();
      for(XU32 i = 0; i != numVertexSets; ++i)
      {
        XVertexSet* vertexSet = indexSet->GetVertexSet(i);
        vertexSet->SetIndexSet(0);
      }
      
      delete indexSet;

      return true;
    }
  }
  XGlobal::GetLogger()->Logf(XBasicLogger::kError, "XIndexSet \"%s\" not found", name.c_str());
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void XPolygonMesh::Clear()
{
  {
    XIndexSetArray::iterator it = m_IndexSets.begin();
    for (;it != m_IndexSets.end(); ++it)
    {
      delete *it;
    }
    m_IndexSets.clear();
  }
  {
    XVertexSetArray::iterator it = m_VertexSets.begin();
    for (;it != m_VertexSets.end(); ++it)
    {
      delete *it;
    }
    m_VertexSets.clear();
  }

  SetPoints(XVector3Array());
  m_PointSet=0;
  m_NumRequiredIndices=0;
  m_PolyCounts.clear();
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::Triangulate()
{
  bool isTriangluated = true;
  XU32 numTriangles = 0;
  // calculate the number of triangles
  for(XU32Array::const_iterator it = m_PolyCounts.begin(); it != m_PolyCounts.end(); ++it)
  {
    numTriangles += (*it) - 2;
    if(isTriangluated && *it != 3)
      isTriangluated = false;
  }

  if(isTriangluated)
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kWarning, "XPolygonMesh \"%s\" already triangulated", GetName());
    return false;
  }

  // make a map for the old face to new face mapping for material groups
  typedef std::map<XU32, XIndexList> FaceMap;
  FaceMap newFaceMappings;

  // create a temp array for the new indices
  XS32Array newIndices;
  newIndices.resize(numTriangles * 3);

  // do one IndexSet triangulation first and calculate the new face indices for
  // the instance material groups
  XIndexSetArray::iterator itSet = m_IndexSets.begin();
  if(itSet != m_IndexSets.end())
  {
    XIndexSet* indexSet = *itSet;

    // the index into the new index set
    XU32 index = 0;
    // the offset into the old index set
    XU32 offset = 0;

    XU32 oldFaceIndex = 0, newFaceIndex = 0;
    for(XU32Array::const_iterator count = m_PolyCounts.begin(); count != m_PolyCounts.end(); ++count, ++oldFaceIndex)
    {
      XIndexList newFaces;
      newFaces.resize(*count - 2);
      for(XU32 i = 0; i != *count - 2; ++i, ++newFaceIndex)
      {
        newIndices[index++] = (*indexSet)[offset];
        newIndices[index++] = (*indexSet)[offset + i + 1];
        newIndices[index++] = (*indexSet)[offset + i + 2];

        newFaces[i] = newFaceIndex;
      }
      newFaceMappings.insert(std::make_pair(oldFaceIndex, newFaces));

      // offset the index into the index array by the poly count of this poly
      offset += *count;
    }

    // update the first index sets indices
    indexSet->m_Indices = newIndices;

    // do the rest of the index sets
    for(++itSet; itSet != m_IndexSets.end(); ++itSet)
    {
      indexSet = *itSet;

      // the index into the new index set
      index = 0;
      // the offset into the old index set
      offset = 0;
      for(XU32Array::const_iterator count = m_PolyCounts.begin(); count != m_PolyCounts.end(); ++count)
      {
        for(XU32 i = 0; i != *count - 2; ++i)
        {
          newIndices[index++] = (*indexSet)[offset];
          newIndices[index++] = (*indexSet)[offset + i + 1];
          newIndices[index++] = (*indexSet)[offset + i + 2];
        }
        // offset the index into the index array by the poly count of this poly
        offset += *count;
      }

      // update the index sets indices
      indexSet->m_Indices = newIndices;
    }
  }

  // Now we need to modify all of the face indices contained within any 
  // material groups
  XInstanceList instances;
  GetObjectInstances(instances);
  for(XInstanceList::iterator it = instances.begin(); it != instances.end(); ++it)
  {
    XInstance* inst = *it;

    // ignore all instances with zero or one material applied
    if(!inst->HasMaterialGroups())
      continue;

    // have to store the new indices so they are updated after all new indices are calculated otherwise
    // SetFaces incorrectly removes some old indices before they can be used to map the new indices
    XM2::vector<XIndexList> new_group_face_ids;
    new_group_face_ids.resize(inst->GetNumMaterialGroups());
    // process each group and add any additional face indices generated
    for(XU32 i = 0; i != inst->GetNumMaterialGroups(); ++i)
    {
      XIndexList old_face_ids;
      inst->GetFaces(i, old_face_ids);

      // now for every old face id find it in the map and add them to the new face id array
      for(XIndexList::const_iterator index = old_face_ids.begin(); index != old_face_ids.end(); ++index)
      {
        FaceMap::const_iterator mapping = newFaceMappings.find(*index);
        if(mapping != newFaceMappings.end())
        {
          new_group_face_ids[i].insert(new_group_face_ids[i].end(), mapping->second.begin(), mapping->second.begin() + mapping->second.size());
        }
      }
    }

    for(XU32 i = 0; i != inst->GetNumMaterialGroups(); ++i)
    {
      inst->SetFaces(i, new_group_face_ids[i]);
    }
  }

  m_PolyCounts.resize(numTriangles);
  //m_PolyCounts.assign(numTriangles, 3);
  for (size_t i=0;i<m_PolyCounts.size();++i)
  {
    m_PolyCounts[i] = 3;
  }
  m_NumRequiredIndices = numTriangles * 3;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::Compress()
{
  // copy the array as new index sets will possibly be added within the loop
  // invalidating the index set array
  XIndexSetArray temp_index_set_array = m_IndexSets;

  // make sure every vertex set has a unique index set
  for(XIndexSetArray::const_iterator it = temp_index_set_array.begin(); it != temp_index_set_array.end(); ++it)
  {
    // the minimum number of vertex sets allowed for this index set
    // has to be 0 if it's the point's index set
    XU32 min_num_sets = ((*it) == m_PointSet) ? 0 : 1;
    
    XU32 num_vertex_sets = (*it)->GetNumVertexSets();
    // for each vertex set belonging to this index set
    // assign a new index set until this index set has
    // the minimum number of vertex sets
    for(;num_vertex_sets > min_num_sets; --num_vertex_sets)
    {
      XVertexSet* vertex_set = (*it)->GetVertexSet(min_num_sets);
      XIndexSet* index_set = vertex_set->CreateIndexSet();
      index_set->m_Indices = (*it)->m_Indices;
    }
  }

  // update the points array
  XVector3Array new_points;
  new_points.reserve(m_Points.size());

  XS32Array indices_map;
  indices_map.resize(m_Points.size());
  XS32 old_index = 0, new_index = 0;

  // loop through the old points array checking if there is a duplicate in the new points array
  // if there isn't then add the point to the new points array
  for(XVector3Array::iterator old_point = m_Points.begin(); old_point != m_Points.end(); ++old_point, ++old_index)
  {
    bool unique = true;
    XU32 found_index = 0;
    // check this old point with the existing new points
    for(XVector3Array::iterator new_point = new_points.begin(); new_point != new_points.end(); ++new_point, ++found_index)
    {
      if(*old_point == *new_point)
      {
        unique = false;
        break;
      }
    }
    // if it's a new point add it to the points array otherwise
    // just store the index into the new point array
    if(unique)
    {
      new_points.push_back(*old_point);
      indices_map[old_index] = new_index++;
    }
    else
    {
      indices_map[old_index] = found_index;
    }
  }

  // if the points have been compressed
  if(new_points.size() != m_Points.size())
  {
    // should maybe check deformers for compression as well
    XDeformerList deformers;
    GetDeformerQueue(deformers);

    for(XDeformerList::iterator it = deformers.begin(); it != deformers.end(); ++it)
    {
      switch((*it)->ApiType())
      {
        case XFn::BlendShape:
        {
          XBlendShape* blend_shape = (*it)->HasFn<XBlendShape>();
          XMD_ASSERT(blend_shape);

          XVector3Array new_target_points;
          new_target_points.resize(new_points.size());
          for(XU32 i = 0; i != blend_shape->GetNumTargets(); ++i)
          {
            XGeometry* target = blend_shape->GetTarget(i);
            XVector3Array old_target_points;
            // if there's still target geometry
            if(target)
            {
              target->GetPoints(old_target_points);
            }
            else
            {
              blend_shape->GetTarget(i, old_target_points);
            }
            
            for(XS32Array::iterator index = m_PointSet->m_Indices.begin(); index != m_PointSet->m_Indices.end(); ++index)
            {
              new_target_points[indices_map[*index]] = old_target_points[*index];
            }
            blend_shape->SetTarget(i, new_target_points);
          }
          break;  
        }
        case XFn::Cluster:
        {
          XCluster* cluster = (*it)->HasFn<XCluster>();
          XMD_ASSERT(cluster);
          XIndexList indices;
          XRealArray weights;
          cluster->GetIndices(indices);
          cluster->GetWeights(weights);

          for(XIndexList::iterator index = indices.begin(); index != indices.end(); ++index)
          {
            *index = indices_map[*index];
          }

          // remove duplicate indices
          for(XU32 i = 0; i < indices.size() - 1; ++i)
          {
            for(XU32 j = i + 1; j < indices.size();)
            {
              if(indices[i] == indices[j])
              {
                indices.erase(indices.begin() + j);
                weights.erase(weights.begin() + j);
              }
              else
              {
                ++j;
              }
            }
          }

          cluster->SetIndices(indices);
          cluster->SetWeights(weights);
          break;  
        }
        case XFn::JointCluster:
        {
          XJointCluster* joint_cluster = (*it)->HasFn<XJointCluster>();
          XMD_ASSERT(joint_cluster);

          XIndexList new_indices;
          XRealArray weights;

          joint_cluster->GetWeights(weights);

          // make a new index list
          XIndexList::const_iterator index = joint_cluster->Indices().begin();
          for(; index != joint_cluster->Indices().end(); ++index)
          {
            new_indices.push_back(indices_map[*index]);
          }
          
          // remove duplicate indices
          for(XU32 i = 0; i < new_indices.size() - 1; ++i)
          {
            for(XU32 j = i + 1; j < new_indices.size();)
            {
              if(new_indices[i] == new_indices[j])
              {
                new_indices.erase(new_indices.begin() + j);
                weights.erase(weights.begin() + j);
              }
              else
              {
                ++j;
              }
            }
          }

          joint_cluster->SetIndices(new_indices);
          joint_cluster->SetWeights(weights);

          break;  
        }
        case XFn::SkinCluster:
        {
          XSkinCluster* skin_cluster = (*it)->HasFn<XSkinCluster>();
          XMD_ASSERT(skin_cluster);

          // update the skin cluster skin weights
          XM2::vector<XSkinnedVertex> new_skin_weights,old_skin_weights;
          new_skin_weights.resize(new_points.size());
          skin_cluster->GetSkinWeights(old_skin_weights);
          for(XS32Array::iterator index = m_PointSet->m_Indices.begin(); index != m_PointSet->m_Indices.end(); ++index)
          {
            new_skin_weights[indices_map[*index]] = old_skin_weights[*index];
          }
          skin_cluster->SetSkinWeights(new_skin_weights);

          break;   
        }
        default:
          break;
      }
    }

    // update the points
    m_Points = new_points;

    // update the index set indices
    for(XS32Array::iterator it = m_PointSet->m_Indices.begin(); it != m_PointSet->m_Indices.end(); ++it)
    {
      *it = indices_map[*it];
    }
  } // end of if(new_points.size() != m_Points.size())

  // loop through all the vertex sets
  for(XVertexSetArray::iterator vertex_set = m_VertexSets.begin(); vertex_set != m_VertexSets.end(); ++vertex_set)
  {
    XRealArray new_vertices;
    new_vertices.reserve((*vertex_set)->m_VertexData.size());
    
    indices_map.resize((*vertex_set)->m_VertexData.size());
    old_index = 0; new_index = 0;

    // loop through the old vertex set array checking if there is a duplicate in the new vertex set array
    // if there isn't then add the vertex to the new vertex set array
    for(XRealArray::iterator old_data = (*vertex_set)->m_VertexData.begin(); old_data != (*vertex_set)->m_VertexData.end(); ++old_index)
    {
      bool element_unique = true;
      XU32 found_index = 0;

      // look for the old element in the new set
      for(XRealArray::iterator new_data = new_vertices.begin(); new_data != new_vertices.end(); ++found_index)
      {
        
        bool components_equal = true;
        XRealArray::iterator old_comp_it = old_data;
        // as the data is stored as an array of reals we must loop through each component as well
        for(XU32 i = 0; i != (*vertex_set)->m_ElementSize; ++i, ++old_comp_it, ++new_data)
        {
          // check each component
          if(*old_comp_it != *new_data)
          {
            components_equal = false;
            new_data += (*vertex_set)->m_ElementSize - i;
            break;
          }
        }
        if(components_equal)
        {
          element_unique = false;
          break;
        }
      }
      // if it wasn't found add the new element
      if(element_unique)
      {
        for(XU32 i = 0; i != (*vertex_set)->m_ElementSize; ++i, ++old_data)
        {
          new_vertices.push_back(*old_data);
        }
        indices_map[old_index] = new_index++;
      }
      else
      {
        // else just store the index it was found at
        indices_map[old_index] = found_index;
        // increment the old_data iterator
        old_data += (*vertex_set)->m_ElementSize;
      }
    }

    (*vertex_set)->m_VertexData = new_vertices;
    (*vertex_set)->m_NumItems = (XU32)new_vertices.size() / (*vertex_set)->m_ElementSize;

    // update the index set indices
    XIndexSet* index_set = (*vertex_set)->GetIndexSet();
    for(XS32Array::iterator it = index_set->m_Indices.begin(); it != index_set->m_Indices.end(); ++it)
    {
      *it = indices_map[*it];
    }
  }

  // check for duplicate index arrays
  for(XU32 i = 0; i < m_IndexSets.size() - 1; ++i)
  {
    XIndexSet* index_set = m_IndexSets[i];

    // loop through all the index sets that haven't been checked so far
    for(XU32 j = i + 1; j < m_IndexSets.size();)
    {
      bool is_same = true;

      XMD_ASSERT(index_set->m_Indices.size() == m_IndexSets[j]->m_Indices.size());
      if(index_set->m_Indices.size() != m_IndexSets[j]->m_Indices.size())
      {
        XGlobal::GetLogger()->Log(XBasicLogger::kError, "index_set->m_Indices.size() != m_IndexSets[j]->m_Indices.size()");
        return false;
      }

      // loop through all the indices
      XS32Array::iterator first = index_set->m_Indices.begin();
      XS32Array::iterator second = m_IndexSets[j]->m_Indices.begin();
      for(; first != index_set->m_Indices.end(); ++first, ++second)
      {
        if(*first != *second)
        {
          is_same = false;
          break;
        }
      }

      // if the indices are the same
      if(is_same)
      {
        // change the VertexSet to use the first duplicate IndexSet
        XVertexSet* vertex_set = m_IndexSets[j]->GetVertexSet(0);
        XMD_ASSERT(vertex_set);
        if(vertex_set)
        {
          vertex_set->SetIndexSet(index_set);
        }
        // erase the other duplicate
        delete m_IndexSets[j];
        m_IndexSets.erase(m_IndexSets.begin() + j);
      }
      else
      {
        ++j;
      }
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPolygonMesh::ExpandToVertexArray(bool use_indices)
{
  XIndexSet* index_set = m_PointSet;

  // the new flattened points 
  XVector3Array new_points;
  // the mapping of the new indices to old indices
  XS32Array points_mapping;
  // the mapping of the old indices to new indices
  XM2::vector<XIndexList> index_mapping;

  new_points.resize(index_set->m_Indices.size());
  index_mapping.resize(m_Points.size());
  points_mapping.resize(index_set->m_Indices.size());

  // flatten the points list
  XU32 count = 0;
  XVector3Array::iterator point = new_points.begin();
  XS32Array::iterator old_index = points_mapping.begin();
  XS32Array::iterator index = index_set->m_Indices.begin();
  for(; index != index_set->m_Indices.end(); ++index, ++point, ++old_index, ++count)
  {
    *point = m_Points[*index];
    *old_index = *index;

    index_mapping[*index].push_back(count);
  }
  // so the deformers aren't recalculated needlessly
  bool points_changed = (m_Points.size() != new_points.size()); 
  m_Points = new_points;

  for(XVertexSetArray::iterator it = m_VertexSets.begin(); it != m_VertexSets.end(); ++it)
  {
    XVertexSet* vertex_set = *it;
    index_set = vertex_set->GetIndexSet();

    vertex_set->m_NumItems = (XU32)index_set->m_Indices.size();
    XRealArray new_vertex_data;
    new_vertex_data.resize(index_set->m_Indices.size() * vertex_set->m_ElementSize);

    XRealArray::iterator new_data = new_vertex_data.begin();
    index = index_set->m_Indices.begin();
    for(; index != index_set->m_Indices.end(); ++index)
    {
      for(XU32 i = 0; i != vertex_set->m_ElementSize; ++i, ++new_data)
      {
        *new_data = vertex_set->m_VertexData[*index * vertex_set->m_ElementSize + i];
      }
    }
    vertex_set->SetIndexSet(m_PointSet);
    vertex_set->m_VertexData = new_vertex_data;
  }

  // remove every index set apart from the point index set
  for(XIndexSetArray::iterator it = m_IndexSets.begin(); it != m_IndexSets.end(); ++it)
  {
    if(*it != m_PointSet)
    {
      delete *it;
    }
  }
  m_IndexSets.clear();
  m_IndexSets.push_back(m_PointSet);

  // update the point index set's indices
  count = 0;
  for(index = m_PointSet->m_Indices.begin(); index != m_PointSet->m_Indices.end(); ++index)
  {
    *index = count++;
  }

  // if we're using indices then remove any duplicate verts
  if(use_indices)
  { 
    bool same;

    // a list of indices that have been removed so the index into IndexSet
    // can be correctly calculated, always in order of size
    XIndexList removed_indices;
    for(XS32 i = 0; i < (XS32)m_Points.size() - 1; ++i)
    {
      for(XS32 j = i + 1; j < (XS32)m_Points.size();)
      {
        same = true;
        // check if points are the same
        if(m_Points[i] == m_Points[j])
        {
          // check if all the vertex sets elements are the same
          for(XVertexSetArray::iterator it = m_VertexSets.begin(); it != m_VertexSets.end(); ++it)
          {
            XVertexSet* vertex_set = *it;

            for(XU32 k = 0; k != vertex_set->m_ElementSize; ++k)
            {
              // if they're different then set same to false and goto different
              if( !XM2::float_equal(vertex_set->m_VertexData[i * vertex_set->m_ElementSize + k],
                  vertex_set->m_VertexData[j * vertex_set->m_ElementSize + k]))
              {
                same = false;
                goto different;
              }
            }
          }
        }
        else
        {
          same = false;
        } 

different:
        // if the points were the same remove that vertex and change the index and decrement all
        // indices after by 1
        if(same)
        {
          points_changed = true;
          // remove this index from the point
          m_Points.erase(m_Points.begin() + j);
          // remove this index from the points mapping
          points_mapping.erase(points_mapping.begin() + j);

          // update the index map
          for(XM2::vector<XIndexList>::iterator it = index_mapping.begin(); it != index_mapping.end(); ++it)
          {
            for(XU32 k = 0; k < it->size();)
            {
              // if it's the same remove it from the list
              if ((*it)[k] == (XU32)j)
              {
                it->erase(it->begin() + k);
              }
              else
              {
                // if it's greater than the removed index then decrement it
                if ((*it)[k] > (XU32)j)
                {
                  (*it)[k] = (*it)[k] - 1;
                }
                ++k;
              }
            }
          }
          
          // update the vertex sets
          for(XVertexSetArray::iterator it = m_VertexSets.begin(); it != m_VertexSets.end(); ++it)
          {
            XVertexSet* vertex_set = *it;

            // remove each element from the vertex data
            XU32 offset = j * vertex_set->m_ElementSize;
            for(XU32 k = 0; k != vertex_set->m_ElementSize; ++k)
            {
              vertex_set->m_VertexData.erase(vertex_set->m_VertexData.begin() + offset);
            }
            --vertex_set->m_NumItems;
          }

          // workout the actual IndexSet index
          XU32 removed_index = j;
          XIndexList::iterator rem = removed_indices.begin();
          for(;rem != removed_indices.end(); ++rem)
          {
            if(*rem <= removed_index)
            {
              ++removed_index;
            }
            else
            {
              break;
            }
          }
          // add this index to the list of removed indices
          // preserving the order
          removed_indices.insert(rem, removed_index);

          // set the actual index to index value of the first of the duplicates
          index = m_PointSet->m_Indices.begin() + removed_index;
          *index = i;
          // decrement all indices greater than the index removed by one
          for(index = m_PointSet->m_Indices.begin(); index != m_PointSet->m_Indices.end(); ++index)
          {
            if(*index > j)
            {
              *index -= 1;
            }
          }
        }
        else
        {
          // if they weren't the same then increment i
          ++j;
        }
      }
    }
  }

  // if the points list has changed size
  if(points_changed)
  {
    /// \todo should maybe check deformers for compression as well
    XDeformerList deformers;
    GetDeformerQueue(deformers);

    for(XDeformerList::iterator it = deformers.begin(); it != deformers.end(); ++it)
    {
      switch((*it)->ApiType())
      {
        case XFn::BlendShape:
        {
          XBlendShape* blend_shape = (*it)->HasFn<XBlendShape>();
          XMD_ASSERT(blend_shape);

          XVector3Array new_target_points;
          new_target_points.resize(m_Points.size());
          for(XU32 i = 0; i != blend_shape->GetNumTargets(); ++i)
          {
            XGeometry* target = blend_shape->GetTarget(i);
            XVector3Array old_target_points;
            // if there's still target geometry
            if(target)
            {
              target->GetPoints(old_target_points);
            }
            else
            {
              blend_shape->GetTarget(i, old_target_points);
            }
            
            point = new_target_points.begin();
            for(old_index = points_mapping.begin(); old_index != points_mapping.end(); ++old_index, ++point)
            {
              *point = old_target_points[*old_index];
            }
            blend_shape->SetTarget(i, new_target_points);
          }
          break;  
        }
        case XFn::Cluster:
        {
          XCluster* cluster = (*it)->HasFn<XCluster>();
          XMD_ASSERT(cluster);

          XIndexList indices, new_indices;
          cluster->GetIndices(indices);

          XRealArray old_weights, new_weights;
          cluster->GetWeights(old_weights);

          XRealArray::const_iterator weight = old_weights.begin();
          for(XIndexList::const_iterator it = indices.begin(); it != indices.end(); ++it, ++weight)
          {
            XIndexList::const_iterator new_index = index_mapping[*it].begin();
            for(; new_index != index_mapping[*it].end(); ++new_index)
            {
              new_indices.push_back(*new_index);
              new_weights.push_back(*weight);
            }
          }

          cluster->SetIndices(new_indices);
          cluster->SetWeights(new_weights);
          break;  
        }
        case XFn::JointCluster:
        {
          XJointCluster* joint_cluster = (*it)->HasFn<XJointCluster>();
          XMD_ASSERT(joint_cluster);

          XIndexList new_indices;
          XRealArray weights, new_weights;

          joint_cluster->GetWeights(weights);

          // make a new index list
          XIndexList::const_iterator index = joint_cluster->Indices().begin();
          XRealArray::const_iterator weight = weights.begin();
          for(; index != joint_cluster->Indices().end(); ++index, ++weight)
          {
            for(XIndexList::const_iterator new_index = index_mapping[*index].begin(); new_index != index_mapping[*index].end(); ++new_index)
            {
              new_indices.push_back(*new_index);
              new_weights.push_back(*weight);
            }
          }

          joint_cluster->SetIndices(new_indices);
          joint_cluster->SetWeights(new_weights);

          break;  
        }
        case XFn::SkinCluster:
        {
          XSkinCluster* skin_cluster = (*it)->HasFn<XSkinCluster>();
          XMD_ASSERT(skin_cluster);

          // update the skin cluster skin weights
          XM2::vector<XSkinnedVertex> new_skin_weights,old_skin_weights;
          new_skin_weights.resize(m_Points.size());
          XM2::vector<XSkinnedVertex>::iterator new_skin_weight = new_skin_weights.begin();
          skin_cluster->GetSkinWeights(old_skin_weights);

          old_index = points_mapping.begin();
          for(; new_skin_weight != new_skin_weights.end(); ++new_skin_weight, ++old_index)
          {
            *new_skin_weight = old_skin_weights[*old_index];
          }
          skin_cluster->SetSkinWeights(new_skin_weights);

          break;   
        }
        default:
          break;
      }
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XPolygonMesh::DeleteDegenerateTriangles(const XReal tolerance)
{
  XItPolygon it(this);

  while(!it.IsDone())
  {
    if(it.GetArea() < tolerance)
    {
      XU32 face_index = it.GetFaceIndex();

      for(XIndexSetArray::iterator set = m_IndexSets.begin(); set != m_IndexSets.end(); ++set)
      {
        XS32Array::iterator first = (*set)->m_Indices.begin() + it.m_CurrentVertexIndex;
        XS32Array::iterator last = first + m_PolyCounts[face_index];
        
        (*set)->m_Indices.erase(first, last);
      }
      m_PolyCounts.erase(m_PolyCounts.begin() + face_index);
    }
    else
    {
      it.Next();
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XPolygonMesh::CalculateNormals(bool calculate_tangents, bool calculate_bi_normals, XString* uvset_name)
{
  XVertexSet* normalSet = GetVertexSet(XVertexSet::kNormal);
  if(normalSet == 0)
  {
    normalSet = CreateVertexSet("normals", static_cast<XU32>(m_Points.size()), 3, XVertexSet::kNormal);
    if(normalSet == 0)
    {
      XGlobal::GetLogger()->Log(XBasicLogger::kError, "XPolygonMesh::CalculateNormals : could not create XVertexSet \"normals\"");
      return;
    }
    normalSet->SetIndexSet(m_PointSet);
  }
  XIndexSet* normalIndexSet = normalSet->GetIndexSet();
  if(normalIndexSet == 0)
  {
    XGlobal::GetLogger()->Logf(XBasicLogger::kError, "XPolygonMesh::CalculateNormals : XVertexSet \"%s\" has no XIndexSet", normalSet->GetName().c_str());
    return;
  }
  XVector3Array normals;
  normals.resize(normalSet->GetNumElements());

  XU32 offset = 0;
  for(XU32Array::const_iterator count = m_PolyCounts.begin(); count != m_PolyCounts.end(); ++count)
  {
    // loop through each vertex of the face
    for(XU32 i = 0; i != *count; ++i)
    {
      XU32 index1 = offset + i;
      XVector3 p1 = m_Points[(*m_PointSet)[index1]];

      XU32 index2 = offset + ((i + 1) % *count);
      XVector3 p2 = m_Points[(*m_PointSet)[index2]];

      XU32 index3 = offset + ((i + *count - 1) % *count);
      XVector3 p3 = m_Points[(*m_PointSet)[index3]];

      XVector3 v1 = p2 - p1;
      XVector3 v2 = p3 - p1;

      // not normalising would be quicker but would add bias of the normal
      // towards larger faces
      XVector3 normal;
      normal.cross(v1,v2);
      normal.normalise();

      normals[(*normalIndexSet)[offset + i]] += normal;
    }

    offset += *count;
  }

  for(XVector3Array::iterator it = normals.begin(); it != normals.end(); ++it)
  {
    it->normalise();
  }

  normalSet->SetData(normals);

  XVertexSet* uvSet = 0;
  if(uvset_name)
  {
    uvSet = GetVertexSet(*uvset_name);
  }
  if(uvSet == 0)
  {
    uvSet = GetVertexSet(XVertexSet::kTexCoord);
  }

  // if we have no uvs return
  if((calculate_tangents || calculate_bi_normals) && uvSet == 0)
  {
    XGlobal::GetLogger()->Log(XBasicLogger::kWarning, "XPolygonMesh::CalculateNormals : no uv sets found, tangents and binormals not calculated");
    return;
  }
  
  XVector2Array uvs;
  XIndexSet* uvIndexSet=0;

  if(uvSet)
  {
    uvSet->GetData(uvs);
    uvIndexSet = uvSet->GetIndexSet();
    if(uvIndexSet == 0)
    {
      XGlobal::GetLogger()->Logf(XBasicLogger::kError, "XPolygonMesh::CalculateNormals : XVertexSet \"%s\" has no XIndexSet", uvSet->GetName().c_str());
      return;
    }
  }

  if(calculate_tangents && calculate_bi_normals && uvSet && uvIndexSet)
  {
    XVertexSet* tangentSet = GetVertexSet(XVertexSet::kTangent);
    if(tangentSet == 0)
    {
      tangentSet = CreateVertexSet("tangents", normalSet->GetNumElements(), 3, XVertexSet::kTangent);
      if(tangentSet == 0)
      {
        XGlobal::GetLogger()->Log(XBasicLogger::kError, "XPolygonMesh::CalculateNormals : could not create XVertexSet \"tangents\"");
        return;
      }
      tangentSet->SetIndexSet(normalIndexSet);
    }

    XVertexSet* bitangentSet = GetVertexSet(XVertexSet::kBiTangent);
    if(bitangentSet == 0)
    {
      bitangentSet = CreateVertexSet("bitangents", normalSet->GetNumElements(), 3, XVertexSet::kBiTangent);
      if(bitangentSet == 0)
      {
        XGlobal::GetLogger()->Log(XBasicLogger::kError, "XPolygonMesh::CalculateNormals : could not create XVertexSet \"bitangents\"");
        return;
      }
      bitangentSet->SetIndexSet(normalIndexSet);
    }

    XVector3Array tan1,tan2;
    tan1.resize(tangentSet->GetNumElements());
    tan2.resize(tangentSet->GetNumElements());

    XU32 offset = 0;
    for(XU32Array::const_iterator count = m_PolyCounts.begin(); count != m_PolyCounts.end(); ++count)
    {
      // loop through each vertex of the face
      for(XU32 i = 0; i != *count; ++i)
      {
        XU32 index1 = offset + i;
        XU32 index2 = offset + ((i + 1) % *count);
        XU32 index3 = offset + ((i + *count - 1) % *count);

        XVector3 v1 = m_Points[(*m_PointSet)[index1]];
        XVector3 v2 = m_Points[(*m_PointSet)[index2]];
        XVector3 v3 = m_Points[(*m_PointSet)[index3]];

        XVector2 w1 = uvs[(*uvIndexSet)[index1]];
        XVector2 w2 = uvs[(*uvIndexSet)[index2]];
        XVector2 w3 = uvs[(*uvIndexSet)[index3]];

        XReal x1 = v2.x - v1.x;
        XReal x2 = v3.x - v1.x;
        XReal y1 = v2.y - v1.y;
        XReal y2 = v3.y - v1.y;
        XReal z1 = v2.z - v1.z;
        XReal z2 = v3.z - v1.z;

        XReal s1 = w2.x - w1.x;
        XReal s2 = w3.x - w1.x;
        XReal t1 = w2.y - w1.y;
        XReal t2 = w3.y - w1.y;

        XReal r = 1.0f / (s1 * t2 - s2 * t1);
        XVector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
        XVector3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);
        
        tan1[(*normalIndexSet)[index1]] += sdir;
        tan1[(*normalIndexSet)[index2]] += sdir;
        tan1[(*normalIndexSet)[index3]] += sdir;

        tan2[(*normalIndexSet)[index1]] += tdir;
        tan2[(*normalIndexSet)[index2]] += tdir;
        tan2[(*normalIndexSet)[index3]] += tdir;
      }

      offset += *count;
    }

    XVector3Array tangents,bitangents;
    tangents.resize(tangentSet->GetNumElements());
    bitangents.resize(bitangentSet->GetNumElements());

    XVector3Array::const_iterator normal = normals.begin();
    XVector3Array::const_iterator t = tan1.begin();
    XVector3Array::const_iterator t2 = tan2.begin();
    XVector3Array::iterator tangent = tangents.begin();
    XVector3Array::iterator bitangent = bitangents.begin();
    for(; bitangent != bitangents.end(); ++normal, ++t, ++t2, ++tangent, ++bitangent)
    {
      XReal _dp = normal->dot(*t);
      tangent->sub(*t,*normal);
      tangent->mul(_dp);
      tangent->normalise();
     
      bitangent->cross(*normal , *tangent);

      XVector3 temp;
      temp.cross(*normal,*t);

      // dot( cross(normal, t), t2 )
      if(temp.dot(*t2) < 0.0f)
      {
        bitangent->negate();
      }
    }

    tangentSet->SetData(tangents);
    bitangentSet->SetData(bitangents);
  }
  else if(calculate_tangents && uvSet)
  {
    XVertexSet* tangentSet = GetVertexSet(XVertexSet::kTangent);
    if(tangentSet == 0)
    {
      tangentSet = CreateVertexSet("tangents", normalSet->GetNumElements(), 3, XVertexSet::kTangent);
      if(tangentSet == 0)
      {
        XGlobal::GetLogger()->Log(XBasicLogger::kError, "XPolygonMesh::CalculateNormals : could not create XVertexSet \"tangents\"");
        return;
      }
      tangentSet->SetIndexSet(normalIndexSet);
    }

    XVector3Array tan1;
    tan1.resize(tangentSet->GetNumElements());

    XU32 offset = 0;
    for(XU32Array::const_iterator count = m_PolyCounts.begin(); count != m_PolyCounts.end(); ++count)
    {
      // loop through each vertex of the face
      for(XU32 i = 0; i != *count; ++i)
      {
        XU32 index1 = offset + i;
        XU32 index2 = offset + ((i + 1) % *count);
        XU32 index3 = offset + ((i + *count - 1) % *count);

        XVector3 v1 = m_Points[(*m_PointSet)[index1]];
        XVector3 v2 = m_Points[(*m_PointSet)[index2]];
        XVector3 v3 = m_Points[(*m_PointSet)[index3]];

        XVector2 w1 = uvs[(*uvIndexSet)[index1]];
        XVector2 w2 = uvs[(*uvIndexSet)[index2]];
        XVector2 w3 = uvs[(*uvIndexSet)[index3]];

        XReal x1 = v2.x - v1.x;
        XReal x2 = v3.x - v1.x;
        XReal y1 = v2.y - v1.y;
        XReal y2 = v3.y - v1.y;
        XReal z1 = v2.z - v1.z;
        XReal z2 = v3.z - v1.z;

        XReal s1 = w2.x - w1.x;
        XReal s2 = w3.x - w1.x;
        XReal t1 = w2.y - w1.y;
        XReal t2 = w3.y - w1.y;

        XReal r = 1.0f / (s1 * t2 - s2 * t1);
        XVector3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);

        tan1[(*normalIndexSet)[index1]] += sdir;
        tan1[(*normalIndexSet)[index2]] += sdir;
        tan1[(*normalIndexSet)[index3]] += sdir;
      }

      offset += *count;
    }

    XVector3Array tangents;
    tangents.resize(tangentSet->GetNumElements());

    XVector3Array::const_iterator normal = normals.begin();
    XVector3Array::const_iterator t = tan1.begin();
    XVector3Array::iterator tangent = tangents.begin();
    for(; tangent != tangents.end(); ++normal, ++t, ++tangent)
    {
      XReal dp = normal->dot(*t);
      tangent->sub(*t,*normal);
      tangent->mul(dp);
      tangent->normalise();
    }

    tangentSet->SetData(tangents);
  }
  else if(calculate_bi_normals)
  {
    /// \todo implement calculating only bitangents
  }
}
}
