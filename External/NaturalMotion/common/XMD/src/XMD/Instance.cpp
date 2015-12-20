//----------------------------------------------------------------------------------------------------------------------
/// \file Instance.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/Instance.h"
#include "XMD/FileIO.h"
#include "XMD/Model.h"
#include "XMD/Shape.h"
#include "XMD/Geometry.h"
#include "XMD/Mesh.h"
#include "XMD/PolygonMesh.h"
#include "XMD/Bone.h"
#include "XMD/Material.h"
#include "XMD/Texture.h"
#include "nmtl/algorithm.h"

namespace XMD
{
NMTL_POD_VECTOR_EXPORT(XInstance*,XM2EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
bool XMaterialGroup::DoData(XFileIO& io)
{
  DUMPER(XMaterialGroup);

  CHECK_IO( io.DoData(&m_MaterialID) );
  DPARAM(m_MaterialID);

  CHECK_IO( io.DoDataVector(m_FaceIndices) );
  DAPARAM(m_FaceIndices);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XInstance::GetUvSetForTexture(const XTexture* texture) const
{
  if (texture)
  {
    if (m_pModel->IsValidObj(texture))
    {
      XM2::pod_vector<XUvLookup*>::const_iterator it = m_UvSets.begin();
      for (;it != m_UvSets.end();++it)
      {
        if (texture->GetID()== (*it)->m_TextureNode)
        {
          return (*it)->m_UvSet.c_str();
        }
      }
    }
  }
  return "";
}

//----------------------------------------------------------------------------------------------------------------------
bool XInstance::AssignTextureAndUvSet(const XTexture* texture,const XChar* uv_set)
{
  bool set_ok = false;
  {
    XBase* b = m_pModel->FindNode(m_Geometry);

    XMesh* m = b->HasFn<XMesh>();
    if (m&&m->HasUvSet(uv_set))
    {
      set_ok=true;
    }
    XPolygonMesh* pm = b->HasFn<XPolygonMesh>();
    if (pm&&pm->HasUvSet(uv_set))
    {
      set_ok=true;
    }
  }
  if (texture&&set_ok)
  {
    if (m_pModel->IsValidObj(texture))
    {
      XM2::pod_vector<XUvLookup*>::iterator it = m_UvSets.begin();
      for (;it != m_UvSets.end();++it)
      {
        if (texture->GetID()== (*it)->m_TextureNode)
        {
          (*it)->m_UvSet = uv_set;
          return true;
        }
      }

      XUvLookup* lu = new XUvLookup();
      lu->m_TextureNode = texture->GetID();
      lu->m_UvSet = uv_set;
      m_UvSets.push_back(lu);
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInstance::DoData(XFileIO& io)
{
  DUMPER(XInstance);

  CHECK_IO( io.DoDataString(m_Name) );
  DPARAM(m_Name);

  CHECK_IO( io.DoData(&m_Geometry) );
  DPARAM(m_Geometry);

  if(io.IsWriting())
  {
    if(m_pModel->GetExportPolicy().IsEnabled(XExportPolicy::WriteMaterials))
    {
      bool HasMaterialGroups = m_Groups.size()!=0;
      CHECK_IO( io.DoData(&HasMaterialGroups) );
      DPARAM(HasMaterialGroups);

      if(!HasMaterialGroups)
      {
        CHECK_IO( io.DoData(&m_Material) );
        DPARAM(m_Material);
      }
      else
      {
        CHECK_IO( io.DoDataVector(m_Groups) );
      }
    }
    else
    {
      bool HasMaterialGroups = 0;
      CHECK_IO( io.DoData(&HasMaterialGroups) );
      XU32 no_material = 0;
      CHECK_IO( io.DoData(&no_material) );
    }
  }
  else
  {
    bool HasMaterialGroups = m_Groups.size()!=0;
    CHECK_IO( io.DoData(&HasMaterialGroups) );
    DPARAM(HasMaterialGroups);

    if(!HasMaterialGroups)
    {
      CHECK_IO( io.DoData(&m_Material) );
      DPARAM(m_Material);
    }
    else
    {
      CHECK_IO( io.DoDataVector(m_Groups) );
    }
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XMaterialGroup::GetDataSize() const
{
    /// the ID of the material applied to these faces
    size_t size = 2*sizeof(XU32);
    size += m_FaceIndices.size() * sizeof(XU32);
    return (XU32)size;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XInstance::GetDataSize() const
{
  XU32 size = sizeof(XId);
  if(m_Groups.size())
  {
    for(XU32 i=0;i!=m_Groups.size();++i)
    {
      size += m_Groups[i]->GetDataSize();
    }
  }
  size += sizeof(XU32);
  size += 2;
  size += static_cast<XU32>(m_Name.size()) + sizeof(bool);
  return size;
}

//----------------------------------------------------------------------------------------------------------------------
const XChar* XInstance::GetName() const
{
  return m_Name.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
void XInstance::SetName(const XChar* n)
{
  m_Name = n;
}

//----------------------------------------------------------------------------------------------------------------------
XInstance::XInstance(XModel* model,const XShape* geom,const XBone* bone)
  : m_Material(0),m_Groups(),m_Geometry(0),m_Parent(0),m_UvSets(),m_pModel(0),m_Name()
{
  DUMPER(XInstance__ctor);

  // if you assert here you have attempted to create an invalid instance
  XMD_ASSERT( model && geom && bone );
  if (geom)
  {
    m_Geometry = geom->GetID();
  }
  if (model)
  {
    m_pModel = model;
  }
  if (bone)
  {
    m_Parent = bone->GetID();
  }
}

//----------------------------------------------------------------------------------------------------------------------
XInstance::XInstance(XModel* model)
  : m_Material(0),m_Groups(),m_Geometry(0),m_Parent(0),m_UvSets(),m_pModel(0),m_Name()
{
  DUMPER(XInstance__ctor);
  // if you assert here you have passed in a NULL model
  XMD_ASSERT(model);

  if (model)
  {
    m_pModel = model;
  }
  m_InstanceID = ++(m_pModel->m_InstanceIDGen);
  m_pModel->m_Instances.push_back(this);
}

//----------------------------------------------------------------------------------------------------------------------
XInstance::~XInstance()
{
  DUMPER(XInstance__dtor);

  for(XU32 i=0;i!=m_Groups.size();++i)
  {
    delete m_Groups[i];
  }
  for(XU32 i=0;i!=m_UvSets.size();++i)
  {
    delete m_UvSets[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XInstance::HasMaterial() const
{
  if(m_Groups.size())
    return true;
  if(m_Material)
    return true;
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInstance::HasMaterialGroups() const
{
  if(m_Groups.size())
    return true;
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
XMaterial* XInstance::GetMaterial() const
{
  DUMPER(XInstance__GetMaterial);
  XBase* ptr=0;

  if(m_Groups.size())
    ptr = m_pModel->FindNode(m_Groups[0]->m_MaterialID);
  else
  if(m_Material)
    ptr = m_pModel->FindNode(m_Material);
  return ptr ? ptr->HasFn<XMaterial>() : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XMaterial* XInstance::GetMaterial(const XU32 group) const
{
  DUMPER(XInstance__GetMaterial);
  XBase* ptr=0;
  if(group < m_Groups.size())
    ptr = m_pModel->FindNode(m_Groups[group]->m_MaterialID);
  else
  if(m_Material!=0)
    ptr = m_pModel->FindNode(m_Material);
  return ptr ? ptr->HasFn<XMaterial>() : 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInstance::SetMaterial(const XMaterial* mat)
{
  DUMPER(XInstance__SetMaterial);
  if(!mat)
  {
    m_Material=0;
    return true;
  }
  if(m_Groups.size())
    m_Groups[0]->m_MaterialID = mat->GetID();
  else
    m_Material = mat->GetID();
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInstance::SetMaterial(const XMaterial* mat,const XU32 group)
{
  DUMPER(XInstance__SetMaterial);
  if(!mat)
  {
    if(group<m_Groups.size())
    {
      m_Groups[group]->m_MaterialID = 0;
      return true;
    }
  }
  if(group>=m_Groups.size())
  {
    m_Groups.resize(group+1);
    m_Groups[group] = new XMaterialGroup;
  }
  m_Groups[group]->m_MaterialID= mat ? mat->GetID() : 0;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XU32 XInstance::GetNumMaterialGroups() const
{
  return static_cast<XU32>(m_Groups.size());
}

//----------------------------------------------------------------------------------------------------------------------
bool XInstance::SetNumMaterialGroups(const XU32 num)
{
  DUMPER(XInstance__SetNumMaterialGroups);
  XMaterialGroupList::iterator it = m_Groups.begin();
  for( ; m_Groups.end()!=it;++it )
  {
    delete *it;
  }

  m_Groups.resize(num);
  it = m_Groups.begin();
  for( ; m_Groups.end()!=it;++it )
  {
    *it = new XMaterialGroup;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInstance::GetFaces(const XU32 group,XIndexList& FaceIds) const
{
  DUMPER(XInstance__GetFaces);
  if (group < m_Groups.size())
  {
    XU32Array::const_iterator it = m_Groups[group]->m_FaceIndices.begin();
    for( ; it != m_Groups[group]->m_FaceIndices.end(); ++it )
      FaceIds.push_back(*it);
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XInstance::SetFaces(const XU32 group,const XIndexList& FaceIds)
{
  DUMPER(XInstance__SetFaces);
  //
  // if you assert here, you have either passed in an invalid group id, or
  // you have forgotten to call SetNumMaterialGroups()
  //
  XMD_ASSERT(group<m_Groups.size());
  m_Groups[group]->m_FaceIndices.clear();

  {
    XIndexList::const_iterator it = FaceIds.begin();
    for( ; it !=FaceIds.end(); ++it )
      m_Groups[group]->m_FaceIndices.push_back(*it);
  }
  nmtl::sort(m_Groups[group]->m_FaceIndices.begin(),m_Groups[group]->m_FaceIndices.end());

  XIndexList::const_iterator it  = FaceIds.begin();
  XIndexList::const_iterator end = FaceIds.end();


  // loop through all indices added to the material group
  for( ; it != end; ++it )
  {
    // go through each existing material group
    for(XU32 i=0;i<m_Groups.size();++i)
    {
      // ignore this group
      if(group==i)
        continue;

      // find any instances where it has been used before and remove it
      XU32Array::iterator itv = nmtl::find(m_Groups[i]->m_FaceIndices.begin(),m_Groups[i]->m_FaceIndices.end(),*it);
      if (itv != m_Groups[i]->m_FaceIndices.end())
      {
         m_Groups[i]->m_FaceIndices.erase(itv);
      }
    }
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XInstance::SetShape(const XShape* shape)
{
  DUMPER(XInstance__SetShape);
  if (!shape)
    return;
  m_Geometry = shape->GetID();
}

//----------------------------------------------------------------------------------------------------------------------
XGeometry* XInstance::GetGeometry() const
{
  DUMPER(XInstance__GetGeometry);
  if (!m_Geometry)
    return 0;
  XBase* ptr = m_pModel->FindNode(m_Geometry);
  return ptr ? ptr->HasFn<XGeometry>() : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XShape* XInstance::GetShape() const
{
  DUMPER(XInstance__GetShape);
  if (!m_Geometry)
    return 0;
  XBase* ptr = m_pModel->FindNode(m_Geometry);
  return ptr ? ptr->HasFn<XShape>() : 0;
}

//----------------------------------------------------------------------------------------------------------------------
XBone* XInstance::GetParent() const
{
  DUMPER(XInstance__GetParent);
  XBase* ptr = m_pModel->FindNode(m_Parent);
  if(!ptr)
    return 0;
  return ptr->HasFn<XBone>();
}

//----------------------------------------------------------------------------------------------------------------------
std::ostream& operator << (std::ostream& ofs,const XInstance& instance)
{
  DUMPER(XInstance__operator_file_out);
  ofs << "\t\tINSTANCE " << instance.m_Name.c_str() << " {\n";
    ofs << "\t\t\tobject_id " << instance.m_Geometry << "\n";

    ofs << "\t\t\tnum_uv_sets " << instance.m_UvSets.size() << "\n";
    XM2::pod_vector<XInstance::XUvLookup*>::const_iterator it = instance.m_UvSets.begin();
    for (;it != instance.m_UvSets.end();++it)
    {
      ofs << "\t\t\t\t" << (*it)->m_TextureNode << " " << (*it)->m_UvSet << "\n";
    }

    if(instance.m_pModel->GetExportPolicy().IsEnabled(XExportPolicy::WriteMaterials))
    {
      if(instance.m_Material!=0)
      {
        ofs << "\t\t\tmaterial " << instance.m_Material << "\n";
      }
      else
      {
        // go through each existing material group
        for(XU32 i=0;i<instance.m_Groups.size();++i)
        {
          ofs << "\t\t\tMATERIAL_GROUP {\n";

          ofs << "\t\t\t\tmaterial_id " << instance.m_Groups[i]->m_MaterialID << "\n";

          ofs << "\t\t\t\tnum_face_indices "
            << static_cast<XU32>(instance.m_Groups[i]->m_FaceIndices.size())
            << "\n\t\t\t\t\t";

          // find any instances where it has been used before and remove it
          XU32Array::iterator itv = instance.m_Groups[i]->m_FaceIndices.begin();
          for ( ; itv != instance.m_Groups[i]->m_FaceIndices.end() ; ++itv )
          {
            ofs << *itv << " ";
          }

          ofs << "\n\t\t\t}\n";
        }
      }
    }

  ofs << "\t\t}\n";

  return ofs;
}

//----------------------------------------------------------------------------------------------------------------------
std::istream& operator >> (std::istream& ifs, XInstance& instance)
{
  DUMPER(XInstance__operator_file_in);
  XString buffer;

  (void)instance;
  return ifs;
}
}

