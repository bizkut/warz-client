//----------------------------------------------------------------------------------------------------------------------
/// \file Mesh.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Mesh.h"
#include "XMD/Model.h"
#include "XMD/MeshVertexColours.h"
#include "XMD/FileIO.h"
#include "nmtl/algorithm.h"

namespace XMD 
{
NMTL_VECTOR_EXPORT(XFace,XMD_EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XTexCoord,XMD_EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
XTexCoord::XTexCoord():u(0),v(0)
{
}

//----------------------------------------------------------------------------------------------------------------------
XTexCoord::XTexCoord(const XReal a,const XReal b)
{
  u=a;
  v=b;
}

//----------------------------------------------------------------------------------------------------------------------
XTexCoord::~XTexCoord()
{
}

//----------------------------------------------------------------------------------------------------------------------
XTexCoord::XTexCoord(const XTexCoord& v_)
  : u(v_.u),v(v_.v) 
{
}

//----------------------------------------------------------------------------------------------------------------------
const XTexCoord& XTexCoord::operator=(const XTexCoord& tc) 
{
  u=tc.u;
  v=tc.v;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
bool XTexCoord::operator==(const XTexCoord& tc) const
{
  return   (XM2::float_equal(u,tc.u) && XM2::float_equal(v,tc.v));
}

//----------------------------------------------------------------------------------------------------------------------
std::istream &operator>>(std::istream& ifs,XTexCoord& v)
{
  return ifs >> v.u >> v.v;
}

//----------------------------------------------------------------------------------------------------------------------
std::ostream &operator<<(std::ostream& ofs,const XTexCoord& v)  
{
  return ofs << v.u << " " << v.v;
}

//----------------------------------------------------------------------------------------------------------------------
std::istream &operator>>(std::istream& ifs,XPointIndex& pi) 
{
  ifs >> pi.v >> pi.n; 
  for(XU32 i=0;i<pi.t.size();++i)
  {
    ifs >> pi.t[i];
  }
  return ifs;
}

//----------------------------------------------------------------------------------------------------------------------
std::ostream &operator<<(std::ostream& ofs,XPointIndex& pi)
{
  ofs << pi.v << " " << pi.n; 
  for(XU32 i=0;i<pi.t.size();++i)
  {
    ofs << " " << pi.t[i];
  }
  return ofs;
}

//----------------------------------------------------------------------------------------------------------------------
XPointIndex::XPointIndex() :  v(0),n(0),t()
{
}

//----------------------------------------------------------------------------------------------------------------------
XPointIndex::XPointIndex(const XPointIndex& v_) : v(v_.v),n(v_.n),t(v_.t)
{
}

//----------------------------------------------------------------------------------------------------------------------
const XPointIndex& XPointIndex::operator = (const XPointIndex& rhs)
{
  n=rhs.n;
  t=rhs.t;
  v=rhs.v;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
bool XPointIndex::DoData(XFileIO& io)
{
  DUMPER(PointIndex);
  IO_CHECK( io.DoData(&v) );
  DPARAM(v);
  IO_CHECK( io.DoData(&n) );
  DPARAM(n);
  IO_CHECK( io.DoDataVector(t) );
  DAPARAM(t);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XFace::ReverseWinding()
{
  XU32 id=(XU32)(m_Points.size()-1);
  for(XU32 i=0;i<m_Points.size()/2;++i)
  {
    XPointIndex temp = m_Points[i];
    m_Points[i] = m_Points[id];
    m_Points[id]=temp;
    --id;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XFace::SetSize(XU32 index_size,XU32 uv_size) 
{
  // ensure a minimum of 3 points
  if(index_size<3)
    index_size = 3;

  // create array of points
  m_Points.resize(index_size);

  XM2::vector<XPointIndex>::iterator it =m_Points.begin();
  for( ; it != m_Points.end(); ++it )
  {
    it->t.resize(uv_size);
  }
}

//----------------------------------------------------------------------------------------------------------------------
XFace::XFace() 
  : m_Points() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XFace::~XFace()
{
}

//----------------------------------------------------------------------------------------------------------------------
XFace::XFace(const XFace& f)
  : m_Points(f.m_Points) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XFace::NumPoints() const 
{
  return static_cast<XU32>(m_Points.size());
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XFace::GetVertexIndex(XU32 ind) const 
{
  // if you assert here, then an invalid index has been specified 
  XMD_ASSERT(m_Points.size()>ind);
  return m_Points[ind].v;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XFace::GetNormalIndex(XU32 ind) const 
{
  // if you assert here, then an invalid index has been specified 
  XMD_ASSERT(m_Points.size()>ind);
  return m_Points[ind].n;
}

//----------------------------------------------------------------------------------------------------------------------
XS32 XFace::GetUvIndex(XU32 ind,XU32 Set) const
{
  // if you assert here, then an invalid index has been specified 
  XMD_ASSERT(m_Points.size()>ind);
  XMD_ASSERT(m_Points[ind].t.size()>Set);
  return m_Points[ind].t[Set];
}

//----------------------------------------------------------------------------------------------------------------------
void XFace::SetVertexIndex(XU32 ind,XU32 idx) 
{
  // if you assert here, then an invalid index has been specified 
  XMD_ASSERT(m_Points.size()>ind);
  m_Points[ind].v=idx;
}

//----------------------------------------------------------------------------------------------------------------------
void XFace::SetNormalIndex(XU32 ind,XU32 idx) 
{
  // if you assert here, then an invalid index has been specified 
  XMD_ASSERT(m_Points.size()>ind);
  m_Points[ind].n=idx;
}

//----------------------------------------------------------------------------------------------------------------------
void XFace::SetUvIndex(XU32 ind,XS32 idx,XU32 Set)
{
  // if you assert here, then an invalid index has been specified 
  XMD_ASSERT(m_Points.size()>ind);

  // if you assert here, then an invalid UV set has been specified 
  XMD_ASSERT(m_Points[ind].t.size()>Set);
  m_Points[ind].t[Set]=idx;
}

//----------------------------------------------------------------------------------------------------------------------
bool XFace::Read(std::istream & ifs,XU32 num_sets)
{
  READ_CHECK("face",ifs);

  // read number of points in poly
  XS32 vcount;
  ifs >> vcount;

  // set size of the face
  SetSize(vcount,num_sets);

  // read all points in face
  XM2::vector<XPointIndex>::iterator p = m_Points.begin();
  for(;p!=m_Points.end();++p)
  {
    // read vertex & normal index
    ifs >> p->v >> p->n;

    // read uv coord indices
    for(XU32 i=0;i<num_sets;++i)
    {
      ifs >> p->t[i];
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XFace::Write(std::ostream& ofs) const
{
  ofs << "\tFACE " << static_cast<XU32>(m_Points.size()) << "\n";

  XM2::vector<XPointIndex>::const_iterator p = m_Points.begin();

  for(;p!=m_Points.end();++p) 
  {
    ofs << "\t\t" << p->v << " " << p->n;

    for(XU32 i=0;i<static_cast<XU32>(p->t.size());++i)
    {
      ofs << " " << p->t[i];
    }
    ofs << "\n";
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XFace::DeleteUVSet(XU32 idx)
{
  XM2::vector<XPointIndex>::iterator p = m_Points.begin();
  for(;p!=m_Points.end();++p) 
  {
    if(p->t.size()<=idx) return;
    p->t.erase( p->t.begin() + idx );
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XFace::DoData(XFileIO& io)
{
  DUMPER(face);
  XU16 sz = static_cast<XU16>(m_Points.size());
  IO_CHECK( io.DoData(&sz) );
  DPARAM(sz);
  if(io.IsReading())
    m_Points.resize(sz);
  XM2::vector<XPointIndex>::iterator it = m_Points.begin();
  for( ; it != m_Points.end(); ++it )
  {
    IO_CHECK( it->DoData(io) );
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XMesh::ReversePolyWindings()
{
  XFaceList::iterator it = m_Faces.begin();
  for (;it != m_Faces.end();++it)
  {
    it->ReverseWinding();
  }
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMesh::GetNumNormals() const
{
  return static_cast<XU32>( m_Normals.size() );
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMesh::GetNumFaces() const
{
  return static_cast<XU32>( m_Faces.size() );
}

//----------------------------------------------------------------------------------------------------------------------
void XMesh::SetNumFaces(const XU32 num_faces)
{
  m_Faces.resize(num_faces);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMesh::GetNumTexCoords(const XU32 uv_set) const
{
  if(uv_set>=GetNumUvSets())
  {
    return 0;
  }
  return static_cast<XU32>( m_TexCoords[uv_set].size() );
}

//----------------------------------------------------------------------------------------------------------------------
XMesh::XMesh(XModel* pmodel) 
  : XGeometry(pmodel),
  m_Faces(),
  m_TexCoords(),
  m_UvSetNames(),
  m_Normals(),
  m_Tangents(),
  m_BiNormals()
{
}

//----------------------------------------------------------------------------------------------------------------------
XMesh::~XMesh()
{
}

//----------------------------------------------------------------------------------------------------------------------
const XNormalList& XMesh::GetNormals() const 
{
  return m_Normals;
}

//----------------------------------------------------------------------------------------------------------------------
const XNormalList& XMesh::GetBiNormals() const 
{
  return m_BiNormals;
}

//----------------------------------------------------------------------------------------------------------------------
const XNormalList& XMesh::GetTangents() const 
{
  return m_Tangents;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XMesh::GetUvSetName(const XU32 uv_set) const
{
  return uv_set < m_UvSetNames.size() ? m_UvSetNames[uv_set].c_str() : 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMesh::DeleteUvSet(const XId set_to_delete)
{
  XFaceList::iterator itf = m_Faces.begin();
  for( ; itf != m_Faces.end(); ++itf )
  {
    itf->DeleteUVSet(set_to_delete);
  }

  if(m_UvSetNames.size()>set_to_delete)
    m_UvSetNames.erase( m_UvSetNames.begin() + set_to_delete );
  if(m_TexCoords.size()>set_to_delete)
    m_TexCoords.erase( m_TexCoords.begin() + set_to_delete );
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XMesh::CalculateNormals(bool keep_indices,const XU32 uv_set) 
{
  XS32 uv = uv_set;

  if(uv >= ((XS32)m_TexCoords.size()) )
    uv = ((XS32)m_TexCoords.size())-1;

  XFaceList faces = m_Faces;

  // triangulate mesh
  Triangulate();

  if(!keep_indices || (m_Normals.size()==0)) 
  {
    m_Normals.resize(m_Points.size());
    
    if(m_UvSetNames.size()) 
    {
      m_BiNormals.resize(m_Points.size());
      m_Tangents.resize(m_Points.size());
    }
  } else {

    if(m_UvSetNames.size())
    {
      m_BiNormals.resize(m_Normals.size());
      m_Tangents.resize(m_Normals.size());
    }
  }

  // zero normals
  XNormalList::iterator it = m_Normals.begin();
  for( ; it != m_Normals.end(); ++it)
  {
    it->x=it->y=it->z=0;
  }

  // zero bi-normals and tangents
  XNormalList::iterator itt = m_Tangents.begin();
  XNormalList::iterator itb = m_BiNormals.begin();
  for( ; itt != m_Tangents.end(); ++itt,++itb)
  {
    itt->x = itt->y = itt->z = 
      itb->x = itb->y = itb->z = 0;
  }

  {
    XFaceList::iterator itf = m_Faces.begin();
    for( ; itf != m_Faces.end(); ++itf )
    {
      XU32 v0 = itf->GetVertexIndex(0);
      XU32 v1 = itf->GetVertexIndex(1);
      XU32 v2 = itf->GetVertexIndex(2);

      XVector3 ab = m_Points[ v1 ] - m_Points[ v0 ] ;
      XVector3 ac = m_Points[ v2 ] - m_Points[ v0 ] ;

      XVector3 fn;
      fn.cross(ab,ac);
      fn.normalise();

      if(!keep_indices) 
      {
        itf->SetNormalIndex( 0, v0 );
        itf->SetNormalIndex( 1, v1 );
        itf->SetNormalIndex( 2, v2 );
      }

      XU32 n0 = itf->GetNormalIndex(0);
      XU32 n1 = itf->GetNormalIndex(1);
      XU32 n2 = itf->GetNormalIndex(2);

      bool normal_error = false;
      XU32 new_normal_size = 0;
      XU32 nsize = (XU32)m_Normals.size();
      if (n0>=nsize) 
      {
        normal_error = true;
        new_normal_size = n0;
      }
      if (n1>=nsize) 
      {
        normal_error = true;
        if(n1>new_normal_size) new_normal_size = n1;
      }
      if (n2>=nsize) 
      {
        normal_error = true;
        if(n2>new_normal_size) new_normal_size = n2;
      }

      if (normal_error)
      {
        m_Normals.resize(new_normal_size);
        if (m_TexCoords.size())
        {
          m_Tangents.resize(new_normal_size);
          m_BiNormals.resize(new_normal_size);
        }
      }

      m_Normals[itf->GetNormalIndex(0)] += fn;
      m_Normals[itf->GetNormalIndex(1)] += fn;
      m_Normals[itf->GetNormalIndex(2)] += fn;

      if(m_TexCoords.size()) 
      {
        const XTexCoordList& tcs = m_TexCoords[0];

        XS32 t0 = itf->GetUvIndex(0,uv);
        XS32 t1 = itf->GetUvIndex(1,uv);
        XS32 t2 = itf->GetUvIndex(2,uv);

        if(((t0>=(XS32)tcs.size()) && 
            (t1>=(XS32)tcs.size()) && 
            (t2>=(XS32)tcs.size())) ||
           (t0<0) || 
           (t1<0) || 
           (t2<0) )
        {
          continue;
        }

        const XTexCoord& uv1 = tcs[ t0 ];
        const XTexCoord& uv2 = tcs[ t1 ];
        const XTexCoord& uv3 = tcs[ t2 ];

        XVector2 uve1( uv2.u - uv1.u, uv2.v - uv1.v );
        XVector2 uve2( uv3.u - uv1.u, uv3.v - uv1.v );

        XReal r = 1.0f / (uve1.x * uve2.y - uve2.x * uve1.y);


        XVector3 sdir(  (uve2.y * ab.x - uve1.y * ac.x) * r,
                        (uve2.y * ab.y - uve1.y * ac.y) * r,
                        (uve2.y * ab.z - uve1.y * ac.z) * r );
        XVector3 tdir(  (uve2.x * ab.x - uve1.x * ac.x) * r,
                        (uve2.x * ab.y - uve1.x * ac.y) * r,
                        (uve2.x * ab.z - uve1.x * ac.z) * r );


        m_Tangents[itf->GetNormalIndex(0)] += sdir;
        m_Tangents[itf->GetNormalIndex(1)] += sdir;
        m_Tangents[itf->GetNormalIndex(2)] += sdir;

        m_BiNormals[itf->GetNormalIndex(0)] += tdir;
        m_BiNormals[itf->GetNormalIndex(1)] += tdir;
        m_BiNormals[itf->GetNormalIndex(2)] += tdir;
      }
    }
  }

  // normalise normals
  it = m_Normals.begin();
  for( ; it != m_Normals.end(); ++it )
  {
    it->normalise();
  }

  // normalise tangents and bi-normals
  it = m_Normals.begin();
  itt = m_Tangents.begin();
  itb = m_BiNormals.begin();
  for( ; itt != m_Tangents.end(); ++itt,++itb,++it )
  {
    XReal n_dot_t = it->dot(*itt);

    XVector3 nnt;
    nnt.mul(*it,n_dot_t);
    nnt.negate();
    nnt.add(*itt);
    nnt.normalise();

    // Gram-Schmidt orthogonalize
    *itt =  nnt;

    // Calculate handedness
    {
      XVector3 n_cross_t;
      n_cross_t.cross(*it,*itt);

      XReal nt_dot_tan2 = n_cross_t.dot(*itb);

      bool left_hand = (nt_dot_tan2 < 0.0f) ? true : false;

      if (!left_hand) 
      {
        itb->negate(n_cross_t);
      }
      else 
      {
        *itb = n_cross_t;
      }
    }
  }

  m_Faces = faces;

  if(!keep_indices)
  {
    XFaceList::iterator itf = m_Faces.begin();
    for( ; itf != m_Faces.end(); ++itf )
    {
      XU32 polySize = itf->NumPoints();
      for(XU32 i = 0; i < polySize; ++i)
      {
        XU32 vIdx = itf->GetVertexIndex(i);

        itf->SetNormalIndex( i, vIdx );
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMesh::GetNormals(XNormalList &norms,const XBone* bone,const XInstance* instance) const 
{
  // get parent of this object
  const XBone* parent = instance->GetParent();

  // construct transform to go from parent to world space, then from
  // world space to the new parent. When transforming normals we have
  // to ensure
  XMatrix M;
  M.quickMult(bone->GetInverse(true),parent->GetWorld(true));
  M.transpose();

  // loop through all points and transform original points
  M.transformVector(norms,m_Normals);
  norms.normalise();
}

//----------------------------------------------------------------------------------------------------------------------
void XMesh::SetNormals(const XNormalList& n)
{
  m_Normals = n;
}

//----------------------------------------------------------------------------------------------------------------------
const XTexCoordList& XMesh::GetTexCoords(XU32 uv_set) const
{
  // if you assert here, then an invalid uv set has been specified 
  XMD_ASSERT( uv_set < m_TexCoords.size() );
  return m_TexCoords[uv_set];
}

//----------------------------------------------------------------------------------------------------------------------
const XFaceList& XMesh::GetFaces() const
{
  return m_Faces;
}

//----------------------------------------------------------------------------------------------------------------------
XFaceList& XMesh::GetFaces() 
{
  return m_Faces;
}

//----------------------------------------------------------------------------------------------------------------------
void XMesh::SetTexCoords(XU32 uv_set,const XTexCoordList& uvs)
{
  // if you assert here, then an invalid uv set has been specified 
  XMD_ASSERT( uv_set < m_TexCoords.size() );
  m_TexCoords[uv_set] = uvs;
}

//----------------------------------------------------------------------------------------------------------------------
void XMesh::SetNumUvSets(XU32 count)
{
  m_TexCoords.resize(count);
  m_UvSetNames.resize(count);
  for(XU32 i=0;i!=count;++i)
  {
    m_UvSetNames[i] = "uvset";
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XMesh::SetFaces(const XFaceList& faces)
{
  m_Faces = faces;
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XMesh::GetApiType() const 
{
  return XFn::Mesh;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XMesh::GetFn(XFn::Type type) 
{
  if (type == XFn::Mesh)
    return (XMesh*)this;
  return XGeometry::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XMesh::GetFn(XFn::Type type) const
{
  if(XFn::Mesh==type)
    return (const XMesh*)this;
  return XGeometry::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
bool XMesh::NodeDeath(XId id) 
{
  return XGeometry::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XMesh::PreDelete(XIdSet& extra_nodes)
{
  XGeometry::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XMesh::WriteChunk(std::ostream& ofs)
{
  if (!XGeometry::WriteChunk(ofs))
    return false;

  ofs << "\tnormal_count " << static_cast<XU32>(m_Normals.size()) << "\n"
    << "\tnum_uv_sets " << static_cast<XU32>(m_TexCoords.size()) << "\n"
    << "\tface_count " << static_cast<XU32>(m_Faces.size()) << "\n";

  if(m_Normals.size())
  {
    ofs << "\tnormals\n";

    XNormalList::iterator it = m_Normals.begin();
    for( ; it != m_Normals.end(); ++it )
    {
      ofs << "\t\t" << *it << "\n";
    }
  }

  if(m_TexCoords.size())
  {
    XM2::vector< XTexCoordList >::iterator ittc = m_TexCoords.begin();
    XStringList::iterator itn = m_UvSetNames.begin();

    for( ; ittc != m_TexCoords.end(); ++ittc,++itn )
    {
      XTexCoordList& tlist = *ittc;

      ofs<<"\ttex_coords \"" << itn->c_str() << "\" " << static_cast<XU32>(tlist.size()) << "\n";

      XTexCoordList::iterator it_uv = tlist.begin();
      for( ; it_uv != tlist.end(); ++it_uv)
      {
        ofs << "\t\t" << it_uv->u << " " << it_uv->v << "\n";
      }
    }
  }

  XFaceList::iterator itf = m_Faces.begin();
  for( ; itf != m_Faces.end(); ++itf )
  {
    itf->Write(ofs);
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMesh::DoData(XFileIO& io)
{
  DUMPER(XMesh);

  IO_CHECK( XGeometry::DoData(io) );

  IO_CHECK( io.DoDataVector(m_Normals) );
  DAPARAM( m_Normals );

  XU16 numUvSets = static_cast<XU16>(m_TexCoords.size());
  IO_CHECK( io.DoData(&numUvSets) );
  DPARAM( numUvSets );
  
  IO_CHECK( io.DoStringVector(m_UvSetNames) );
  DAPARAM( m_UvSetNames );

  if(io.IsReading())
    m_TexCoords.resize(numUvSets);

  for(XU32 i=0;i!=numUvSets;++i) 
  {
    IO_CHECK( io.DoDataVector(m_TexCoords[i]) );
    DAPARAM( m_TexCoords[i] );
  }

  XU16 num_faces = static_cast<XU16>(m_Faces.size());
  IO_CHECK( io.DoData(&num_faces) );
  DPARAM(num_faces);
  if(io.IsReading())
    m_Faces.resize(num_faces);

  {
    DUMPER(faces);
    XFaceList::iterator it = m_Faces.begin();
    for( ; it != m_Faces.end(); ++it )
    {
      IO_CHECK( it->DoData(io) );
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XMesh::DoCopy(const XBase* rhs)
{
  const XMesh* cb = rhs->HasFn<XMesh>();
  XMD_ASSERT(cb);

  m_Normals = cb->m_Normals;
  m_TexCoords = cb->m_TexCoords;
  m_UvSetNames = cb->m_UvSetNames;
  m_Faces = cb->m_Faces;

  // if vertex colours present, duplicate those also
  XMeshVertexColours* mvc = cb->GetVertexColours();
  if (mvc)
  {
    XMeshVertexColours* new_colours = mvc->Clone()->HasFn<XMeshVertexColours>();
    new_colours->SetMesh(this);
  }

  XGeometry::DoCopy(cb);
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMesh::GetDataSize() const
{
  XU32 size = XGeometry::GetDataSize();
  size += static_cast<XU32>(m_Normals.size()) * 3*sizeof(XReal) + 4;
  
  size += sizeof(XU32);
  for(XU32 i=0;i!=m_UvSetNames.size();++i) 
  {
    size += static_cast<XU32>(m_UvSetNames[i].size()) + 2;
  }
  size += sizeof(XU16);
  for(XU32 i=0;i!=m_TexCoords.size();++i) 
  {
    size += static_cast<XU32>(m_TexCoords[i].size()) * sizeof(XTexCoord) + 4;
  }
  size += sizeof(XU16);
  for(XU32 i=0;i!=m_Faces.size();++i) 
  {
    size += 2 + m_Faces[i].NumPoints() * (12 + static_cast<XU32>(m_TexCoords.size())*4);
  }
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMesh::ReadChunk(std::istream& ifs)
{
  XU32 temp=0;

  // geometry base handles the points
  if (!XGeometry::ReadChunk(ifs))
    return false;

  // read normal count
  READ_CHECK("normal_count",ifs);
  ifs >> temp;

  m_Normals.resize(temp);

  // read num_uv_sets
  READ_CHECK("num_uv_sets",ifs);
  ifs >> temp;
  m_TexCoords.resize(temp);
  m_UvSetNames.resize(temp);

  // read face count
  READ_CHECK("face_count",ifs);
  ifs >> temp;

  // resize faces array
  m_Faces.resize(temp);

  if(m_Normals.size())
  {
    // read normals
    READ_CHECK("normals",ifs);

    XNormalList::iterator it = m_Normals.begin();
    for( ; it != m_Normals.end(); ++it )
    {
      ifs >> *it;
    }
  }

  if(m_TexCoords.size())
  {
    XM2::vector< XTexCoordList >::iterator ittc = m_TexCoords.begin();
    XStringList::iterator itn = m_UvSetNames.begin();

    for( ; ittc != m_TexCoords.end(); ++ittc,++itn )
    {
      XString buffer;
      READ_CHECK("tex_coords",ifs);

      // map name
      ifs >> buffer;
      buffer[ strlen(buffer.c_str())-1 ]='\0';

      // skip " chars
      *itn = buffer.c_str()+1;

      // get num of uv's
      XU32 num=0;
      ifs >> num;


      XTexCoordList& tlist = *ittc;

      tlist.resize(num);

      XTexCoordList::iterator it_uv = tlist.begin();
      for( ; it_uv != tlist.end(); ++it_uv)
      {
        ifs >> it_uv->u >> it_uv->v;
      }
    }
  }

  XFaceList::iterator itf = m_Faces.begin();
  for( ; itf != m_Faces.end(); ++itf )
  {
    itf->Read(ifs, static_cast<XU32>(m_TexCoords.size()));
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMesh::GetNumUvSets() const 
{
  return static_cast<XU32>(m_UvSetNames.size());
}

//----------------------------------------------------------------------------------------------------------------------
void XMesh::Triangulate() 
{
  // first determine how many extra faces are required.
  XU32 require_num_faces=GetNumFaces();
  XU32 original_num_faces=GetNumFaces();
  {
    XFaceList::const_iterator it = m_Faces.begin();
    for (;it != m_Faces.end();++it)
    {
      XU32 num = (it->NumPoints()-3);
      if(!num)
        continue;
      require_num_faces += num;
    }
  }

  if (require_num_faces == original_num_faces)
  {
    return;
  }

  // reserve some additional space within the mesh...
  m_Faces.resize(require_num_faces);

  // 
  // we need to store the additional face indices added to the mesh data
  // and how they relate to the material to face indices in the XInstance
  // structures when there are new polygons added. The face indices in 
  // the material groups then need to me modified to take account of the 
  // new polys otherwise we not see the additional polys being rendered
  // correctly. 
  // 
  typedef std::map< XU32, XIndexList > PolyMap;
  PolyMap NewPolyMappings;
  {
    XFaceList::iterator it = m_Faces.begin();
    XFaceList::iterator end = it + original_num_faces;
    XFaceList::iterator it_new = end;
    XU32 new_face_idx = original_num_faces;
    XU32 old_face_idx = 0;

    for( ; it != end; ++it,++old_face_idx ) 
    {
      if(it->NumPoints()==3)
        continue;

      XIndexList new_face_indices;

      // generate each new triangle
      for( XU32 i=3; i<it->NumPoints(); ++i,++new_face_idx,++it_new)
      {
        XFace& f = *it_new;

        f.m_Points.resize(3);

        f.m_Points[0] = it->m_Points[0];
        f.m_Points[1] = it->m_Points[i-1];
        f.m_Points[2] = it->m_Points[i];

        /// \todo this can be optimised if you store the start index + num of new polys added.
        ///       instead of dynamically allocating yet more indices. For the sake of simplicity
        ///       I'm just going to make it work in a nice simple way. Far too late at night for 
        ///       clever things to be done here.
        new_face_indices.push_back(new_face_idx);
      }

      NewPolyMappings.insert( std::make_pair(old_face_idx,new_face_indices) );

      // resize the original face so it too is a triangle...
      it->m_Points.resize(3);
    }
  }


  // skip this step if no new polygons were added
  if(NewPolyMappings.size())
  {
    // Now we need to modify all of the face indices contained within any 
    // material groups
    XInstanceList instances;
    GetObjectInstances(instances);

    XInstanceList::iterator it = instances.begin();
    for (; it != instances.end(); ++it)
    {
      XInstance* inst = *it;

      // ignore all instances with zero or one material applied
      if(!inst->HasMaterialGroups())
        continue;

      // process each group and add any additional polys generated
      for (XU32 i=0;i!=inst->GetNumMaterialGroups();++i)
      {
        XIndexList face_ids;
        inst->GetFaces(i,face_ids);
        nmtl::sort(face_ids.begin(),face_ids.end());

        // now iterate over the map to see if any of the face id's can be found
        // in this group.
        PolyMap::iterator itp = NewPolyMappings.begin();
        for (; itp != NewPolyMappings.end(); ++itp)
        {
          // if we find the poly index in the array... 
          if( face_ids.end() != nmtl::find(face_ids.begin(),face_ids.end(),itp->first) )
          {
            for (unsigned iii=0;iii!=itp->second.size();++iii)
            {
              face_ids.push_back(itp->second[iii]);
            }
            // add the rest to it
            /// \todo the following line has been replaced by above code, sure this could be nicer.
            // face_ids.insert( face_ids.end(), itp->second.begin(), itp->second.end());
          }            
        }

        inst->SetFaces(i,face_ids);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XMeshVertexColours* XMesh::GetVertexColours() const
{
  XList items;
  m_pModel->List(items,XFn::MeshVertexColours);
  if(items.size())
  {
    XList::iterator it = items.begin();
    for (;it != items.end();++it)
    {
      XMeshVertexColours* vc = (*it)->HasFn<XMeshVertexColours>();
      if (vc)
      {
        if (vc->GetMesh()==this)
        {
          return vc;
        }
      }
    }
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMesh::GetNumberOfColourSets() const
{
  const XMeshVertexColours* ptr = GetVertexColours();
  if (ptr)
  {
    return ptr->GetNumberOfColourSets();
  }
  return 0;    
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XMesh::GetColourSetName(const XU32 set_index) const
{
  const XMeshVertexColours* ptr = GetVertexColours();
  if (ptr)
  {
    return ptr->GetSetName(set_index);
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
const XColourArray& XMesh::GetColourArray(const XU32 set_index) const
{
  const XMeshVertexColours* ptr = GetVertexColours();

  // if you assert here, then no XMeshVertexColours exists on the mesh
  XMD_ASSERT(ptr);
  return ptr->GetColourArray(set_index);
}

//----------------------------------------------------------------------------------------------------------------------
const XS32Array& XMesh::GetColourIndexArray(const XU32 set_index,const XU32 face_index) const
{
  XMeshVertexColours* ptr = GetVertexColours();

  // if you assert here, then no XMeshVertexColours exists on the mesh
  XMD_ASSERT(ptr);
  return ptr->GetIndexArray(set_index,face_index);
}

//----------------------------------------------------------------------------------------------------------------------
bool XMesh::GetUvSetIndex( const XTexPlacement2D &placement, XS32 &uv_set_index ) const
{
  // Grab the mesh attributes "uv_set_ids", and "texture_placement_nodes"
  // "uv_set_ids"
  //    Array of all the uvSet ids for the mesh.
  // "texture_placement_nodes"
  //    Array of all the texture_placement_nodes ids, with respect the uvsets.
  const XMD::XAttribute *uv_set_ids_Attr             = GetAttribute ("uv_set_ids"           );
  const XMD::XAttribute *texture_placement_ids_Attr  = GetAttribute ("texture_placement_nodes");

  // If the mesh has only one set of UV's, these attributes will be NULL
  // and we need go no further.
  if (!uv_set_ids_Attr)
  {
    // Make sure that there is at least one UV set
    if( GetNumUvSets() > 0 )
    {
      uv_set_index = 0;
      return true ;
    }
    // If the mesh has no UV sets then return false
    else
    {
      return false ;
    }
  }

  // Retrieve the length of the array attribute "uv_set_ids".
  XU32 uvSetSize = uv_set_ids_Attr->NumElements();
  // Loop through the array of texture_placement node ids and find ours
  for (XU32 i=0; i!= uvSetSize; ++i)
  {
    // Grab the current tex_placement_id
    XS32 tex_placement_id;
    texture_placement_ids_Attr->Get(tex_placement_id,i);

    // If the id is the same as that of the XTexturePlacement2D passed in
    if( tex_placement_id == static_cast<XS32>(placement.GetID()) )
    {
      // Then grab the uv_set_index and return it
      uv_set_ids_Attr->Get(uv_set_index,i);
      return true;
    }
  }
  // If nothing is found then return false on error
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMesh::HasUvSet(const XChar* uv_set_name) const
{
  XStringList::const_iterator it = m_UvSetNames.begin();
  for (;it != m_UvSetNames.end();++it)
  {
    if (*it == uv_set_name)
    {
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMesh::CopyUvSet(const XId original_set,const XId to_replace_index)
{
  if( (original_set<m_UvSetNames.size()) && 
      (to_replace_index<m_UvSetNames.size()) )
  {
    // copy over tex coords
    m_TexCoords[to_replace_index] = m_TexCoords[original_set];

    // assign new XFace tex coord id's
    XFaceList::iterator it = m_Faces.begin();
    for (;it != m_Faces.end();++it)
    {
      for(XU32 i=0;i!=it->NumPoints();++i)
      {
        it->SetUvIndex(i, it->GetUvIndex(i,original_set), to_replace_index );
      }
    }

    return true;
  }
  XGlobal::GetLogger()->Logf(XBasicLogger::kError,"Unable to copy UV set on mesh %s. Invalid uv set id's sepcified (%d,%d)",
    GetName(),original_set,to_replace_index);
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMesh::CopyUvSet( const XId original_set )
{
  XU32 replace_index = (XU32)m_TexCoords.size();
  m_TexCoords.push_back(XTexCoordList());

  // resize all faces to cope with new UV sets.
  XFaceList::iterator it = m_Faces.begin();
  for (;it != m_Faces.end();++it)
  {
    for(XU32 i=0;i!=it->NumPoints();++i)
    {
      it->SetSize( it->NumPoints(), (XU32)m_TexCoords.size() );
    }
  }
  return CopyUvSet(original_set,replace_index);
}
}
