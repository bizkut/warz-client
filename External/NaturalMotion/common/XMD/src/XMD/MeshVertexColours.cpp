//----------------------------------------------------------------------------------------------------------------------
/// \file MeshVertexColours.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/MeshVertexColours.h"
#include "XMD/Mesh.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
XMeshVertexColours::XMeshVertexColours(XModel* pmod)
  : XBase(pmod),m_Mesh(0),m_ColourSets()
{
}

//----------------------------------------------------------------------------------------------------------------------
XMeshVertexColours::~XMeshVertexColours(void)
{
  XM2::pod_vector<XColourSet*>::iterator it = m_ColourSets.begin();
  for (; it != m_ColourSets.end(); ++it)
  {
    delete *it;
  }
}

//----------------------------------------------------------------------------------------------------------------------
XFn::Type XMeshVertexColours::GetApiType() const
{
  return XFn::MeshVertexColours;
}

//----------------------------------------------------------------------------------------------------------------------
XBase* XMeshVertexColours::GetFn(XFn::Type type)
{
  if(XFn::MeshVertexColours==type)
    return (XMeshVertexColours*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
const XBase* XMeshVertexColours::GetFn(XFn::Type type) const
{
  if(XFn::MeshVertexColours==type)
    return (const XMeshVertexColours*)this;
  return XBase::GetFn(type);
}

//----------------------------------------------------------------------------------------------------------------------
void XMeshVertexColours::SetMesh(const XBase* mesh)
{
  // if you assert here, mesh is either NULL, or not part of the 
  // same XModel as this vertex colour chunk
  XMD_ASSERT(mesh && IsValidObj(mesh) );
  const XMesh* m = mesh->HasFn<XMesh>();

  // if you assert here, the XBase derived object specified is not a mesh
  XMD_ASSERT( m );

  if (m)
  {
    // if you assert here, the mesh already has an assigned vertex colour chunk
    XMD_ASSERT( !m->GetVertexColours() );

    m_Mesh = m->GetID();
  }
  else
    m_Mesh = 0;
}

//----------------------------------------------------------------------------------------------------------------------
XMesh* XMeshVertexColours::GetMesh() const 
{
  XBase* b = m_pModel->FindNode(m_Mesh);
  return b ? b->HasFn<XMesh>() : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMeshVertexColours::GetNumberOfColourSets() const 
{
  return static_cast<XU32>(m_ColourSets.size());
}

//----------------------------------------------------------------------------------------------------------------------
bool XMeshVertexColours::SetNumberOfColourSets(const XU32 num)
{
  int diff =  ((int)num) - (int)m_ColourSets.size();
  if (diff>0)
  {
    XU32 sz = (XU32)m_ColourSets.size();
    for (XU32 i=sz;i!=num;++i)
    {
      XColourSet* ptr = new XColourSet;
      m_ColourSets.push_back(ptr);

      ptr->m_ColourSetName = "noname";

      XMesh* m = GetMesh();
      if(m)
      {
        const XFaceList& fl = m->GetFaces();
        ptr->m_FaceIndices.resize(fl.size());

        XColorFaceIndices::iterator itf = ptr->m_FaceIndices.begin();
        for (XFaceList::const_iterator it = fl.begin();it!=fl.end();++it,++itf)
        {
          XS32Array& indices = *itf;
          indices.resize(it->NumPoints(),-1);
        }
      }
    }
  }
  else
  if(diff<0)
  {
    XU32 sz = (XU32)m_ColourSets.size();
    for (XU32 i=num;i!=sz;++i)
    {
      delete m_ColourSets[i];
    }
    m_ColourSets.resize(num);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XMeshVertexColours::GetSetName(const XU32 set_index) const 
{
  if (set_index<GetNumberOfColourSets())
  {
    const XColourSet* const cset = m_ColourSets[ set_index ];
    return cset->m_ColourSetName.c_str();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMeshVertexColours::SetSetName(const XU32 set_index,const XChar* name)
{
  if (set_index<GetNumberOfColourSets())
  {
    m_ColourSets[set_index]->m_ColourSetName = name;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const XColourArray& XMeshVertexColours::GetColourArray(const XU32 set_index) const 
{
  // if you assert here, an invalid colour set has been specified
  XMD_ASSERT(set_index<GetNumberOfColourSets());
  const XColourSet* const cset = m_ColourSets[ set_index ];
  return cset->m_Colours;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMeshVertexColours::SetColourArray(const XU32 set_index,const XColourArray& colours)
{
  if (set_index<GetNumberOfColourSets())
  {
    m_ColourSets[set_index]->m_Colours = colours;
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
const XS32Array& XMeshVertexColours::GetIndexArray(const XU32 set_index,const XU32 face_index) const 
{
  // if you assert here, an invalid colour set has been specified
  XMD_ASSERT(set_index<GetNumberOfColourSets());
  const XColourSet* const cset = m_ColourSets[ set_index ];
  return cset->m_FaceIndices[face_index];
}

//----------------------------------------------------------------------------------------------------------------------
bool XMeshVertexColours::SetIndexArray(const XU32 set_index,const XU32 face_index,const XS32Array& indices)
{
  if (set_index<GetNumberOfColourSets())
  {
    XColorFaceIndices& fi = m_ColourSets[ set_index ]->m_FaceIndices;
    if( face_index < fi.size() )
    {
      if(indices.size()==fi[face_index].size())
      {
        fi[face_index] = indices;
        return true;
      }
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMeshVertexColours::GetIndexArray(const XU32 set_index,XS32Array& indices) const
{
  if (set_index<GetNumberOfColourSets())
  {
    indices.clear();
    XColorFaceIndices& fi = m_ColourSets[ set_index ]->m_FaceIndices;
    XColorFaceIndices::const_iterator it = fi.begin();
    XColorFaceIndices::const_iterator end = fi.end();
    for (;it != end;++it)
    {
      const XS32Array& _indices = *it;
      for (XS32Array::const_iterator iti = _indices.begin();iti != _indices.end();++iti)
      {
        indices.push_back(*iti);
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMeshVertexColours::SetIndexArray(const XU32 set_index,const XS32Array& indices)
{
  if (set_index<GetNumberOfColourSets())
  {
    // run check to make sure there are the correct number of indices in the 
    // provided array
    {
      XU32 count=0;
      XColorFaceIndices& fi = m_ColourSets[ set_index ]->m_FaceIndices;
      XColorFaceIndices::const_iterator it = fi.begin();
      XColorFaceIndices::const_iterator end = fi.end();
      for (;it != end;++it)
      {
        count += (XU32)it->size();
      }
      if (count!=indices.size())
      {
        XGlobal::GetLogger()->Log(XBasicLogger::kError,
          "XMeshVertexColours :: SetIndexArray - index array provided does not match num of faces in XMesh chunk");
        return false;
      }
    }

    XU32 idx=0;
    XColorFaceIndices& fi = m_ColourSets[ set_index ]->m_FaceIndices;
    XColorFaceIndices::iterator it = fi.begin();
    XColorFaceIndices::iterator end = fi.end();
    for (;it != end;++it)
    {
      XS32Array& _indices = *it;
      for (XS32Array::iterator iti = _indices.begin();iti != _indices.end();++iti,++idx)
      {
        *iti = indices[idx];
      }
    }
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMeshVertexColours::NodeDeath(XId id)
{
  if(id == m_Mesh)
    return false;
  return XBase::NodeDeath(id);
}

//----------------------------------------------------------------------------------------------------------------------
void XMeshVertexColours::PreDelete(XIdSet& extra_nodes)
{
  XBase::PreDelete(extra_nodes);
}

//----------------------------------------------------------------------------------------------------------------------
bool XMeshVertexColours::ReadChunk(std::istream& ifs)
{
  if(!XBase::ReadChunk(ifs))
    return false;

  READ_CHECK("mesh_id",ifs);
  ifs >> m_Mesh;

  XU32 temp=0;
  READ_CHECK("num_colour_sets",ifs);
  ifs >> temp;

  m_ColourSets.resize(temp);
  XM2::pod_vector<XColourSet*>::iterator it = m_ColourSets.begin();
  for (; it != m_ColourSets.end(); ++it )
  {
    XColourSet* cset = *it = new XColourSet();
    READ_CHECK("colour_set",ifs);
    /// \todo replace with a robust string parser
    ifs >> cset->m_ColourSetName;
    cset->m_ColourSetName.erase( cset->m_ColourSetName.begin() );
    cset->m_ColourSetName.erase( cset->m_ColourSetName.end()-1 );
    ifs >> temp;
    cset->m_Colours.resize(temp);

    XColourArray::iterator itc = cset->m_Colours.begin();
    for (;itc != cset->m_Colours.end(); ++itc)
    {
      ifs >> itc->r >> itc->g >> itc->b >> itc->a;
    }

    READ_CHECK("faces",ifs);
    ifs >> temp;

    cset->m_FaceIndices.resize(temp);
    XColorFaceIndices::iterator itcf = cset->m_FaceIndices.begin();
    for (;itcf != cset->m_FaceIndices.end();++itcf)
    {
      ifs >> temp;
      XS32Array& indices = *itcf;
      indices.resize(temp);
      XS32Array::iterator itval = indices.begin();
      for (;itval != indices.end(); ++itval)
      {
        ifs >> *itval;
      }
    }
  }
  return ifs.good();
}

//----------------------------------------------------------------------------------------------------------------------
bool XMeshVertexColours::WriteChunk(std::ostream& ofs)
{
  if(!XBase::WriteChunk(ofs))
    return false;

  ofs << "\tmesh_id " << m_Mesh << "\n";
  ofs << "\tnum_colour_sets " << GetNumberOfColourSets() << "\n";

  XM2::pod_vector<XColourSet*>::iterator it = m_ColourSets.begin();
  for (; it != m_ColourSets.end(); ++it )
  {
    XColourSet* cset = *it;
    ofs << "\tcolour_set \"" << cset->m_ColourSetName.c_str() << "\" " << cset->m_Colours.size() << "\n";
    XColourArray::iterator itc= cset->m_Colours.begin();
    for (;itc != cset->m_Colours.end();++itc)
    {
      ofs << "\t\t" << itc->r << " " << itc->g << " " << itc->b << " " << itc->a << std::endl;
    }

    ofs << "\tfaces " << cset->m_FaceIndices.size() << "\n";
    XColorFaceIndices::iterator itcf = cset->m_FaceIndices.begin();
    for (;itcf != cset->m_FaceIndices.end(); ++itcf)
    {
      XS32Array& indices=*itcf;
      ofs << "\t\t" << indices.size() << " ";
      XS32Array::iterator itf = indices.begin();
      for (;itf != indices.end();++itf)
      {
        ofs << *itf << " ";
      }
      ofs<<"\n";
    }
  }

  return ofs.good();
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMeshVertexColours::XColourSet::GetDataSize() const 
{
  XU32 sz = sizeof(XU32)*2 + sizeof(XU16);
  sz += (XU32)m_ColourSetName.size();
  sz += (XU32)(m_Colours.size() * sizeof(XColour));
  sz += (XU32)(m_FaceIndices.size() * sizeof(XU32));
  XColorFaceIndices::const_iterator it= m_FaceIndices.begin();
  for (;it != m_FaceIndices.end(); ++it )
  {
    sz += (XU32)(sizeof(XS32) * it->size());
  }
  return sz;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMeshVertexColours::GetDataSize() const
{
  XU32 sz = 2*sizeof(XU32);
  XM2::pod_vector<XColourSet*>::const_iterator it= m_ColourSets.begin();
  for (;it  != m_ColourSets.end(); ++it)
  {
    sz += (*it)->GetDataSize();
  }
  return sz + XBase::GetDataSize();
}

//----------------------------------------------------------------------------------------------------------------------
bool XMeshVertexColours::DoData(XFileIO& io) 
{
  DUMPER(XMeshVertexColours);

  IO_CHECK( XBase::DoData(io) );

  IO_CHECK( io.DoData(&m_Mesh) );
  XU32 num = (XU32)m_ColourSets.size();
  IO_CHECK( io.DoData(&num) );

  if (io.IsReading())
  {
    m_ColourSets.resize(num);
    XM2::pod_vector<XColourSet*>::iterator it = m_ColourSets.begin();
    for (; it != m_ColourSets.end(); ++it )
    {
      *it = new XColourSet;
    }
  }

  XM2::pod_vector<XColourSet*>::iterator it = m_ColourSets.begin();
  for (; it != m_ColourSets.end(); ++it )
  {
    XColourSet* cset = *it;
    IO_CHECK( io.DoDataString( cset->m_ColourSetName ) );
    IO_CHECK( io.DoDataVector( cset->m_Colours ) );

    XU32 num_faces   = (XU32)cset->m_FaceIndices.size();
    IO_CHECK( io.DoData(&num_faces) );

    if (io.IsReading())
    {
      cset->m_FaceIndices.resize(num_faces);
    }

    XColorFaceIndices::iterator itcf = cset->m_FaceIndices.begin();
    for (;itcf != cset->m_FaceIndices.end(); ++itcf)
    {
      io.DoDataVector( *itcf );
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XMeshVertexColours::DoCopy(const XBase* rhs)
{
  const XMeshVertexColours* cb = rhs->HasFn<XMeshVertexColours>();
  XMD_ASSERT(cb);

  m_Mesh = cb->m_Mesh;
  m_ColourSets.resize(cb->m_ColourSets.size());

  for (XU32 i=0;i!=m_ColourSets.size();++i)
  {
    XColourSet* cset = new XColourSet;
    cset->m_ColourSetName = cb->m_ColourSets[i]->m_ColourSetName;
    cset->m_Colours = cb->m_ColourSets[i]->m_Colours;
    cset->m_FaceIndices = cb->m_ColourSets[i]->m_FaceIndices;
    m_ColourSets[i] = cset;
  }

  XBase::DoCopy(cb);
}
}
