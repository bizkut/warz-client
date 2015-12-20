//----------------------------------------------------------------------------------------------------------------------
/// \file SkinCluster.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include <stdlib.h>
#include "XMD/SkinCluster.h"
#include "XMD/Bone.h"
#include "XMD/Geometry.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"
#include "nmtl/algorithm.h"

namespace XMD
{
  NMTL_POD_VECTOR_EXPORT(XSkinWeight,XMD_EXPORT,XM2);
  NMTL_VECTOR_EXPORT(XSkinnedVertex,XMD_EXPORT,XM2);
  NMTL_POD_VECTOR_EXPORT(XClusterWeight,XMD_EXPORT,XM2);
  //----------------------------------------------------------------------------------------------------------------------
  XSkinWeight::XSkinWeight() 
    : w(0),m_JointID(0) 
  {
  }

  //----------------------------------------------------------------------------------------------------------------------
  XSkinWeight::XSkinWeight(const XSkinWeight& lw)
    : w(lw.w),m_JointID(lw.m_JointID) 
  {
  }

  //----------------------------------------------------------------------------------------------------------------------
  XSkinWeight::XSkinWeight(const XReal f,const XU32 idx)
    : w(f),m_JointID(idx)
  {
  }

  //----------------------------------------------------------------------------------------------------------------------
  XClusterWeight::XClusterWeight() 
    : w(0),mVertexIndex(0) 
  {
  }

  //----------------------------------------------------------------------------------------------------------------------
  XClusterWeight::XClusterWeight(const XClusterWeight& lw)
    : w(lw.w),mVertexIndex(lw.mVertexIndex) 
  {
  }

  //----------------------------------------------------------------------------------------------------------------------
  XClusterWeight::XClusterWeight(const XReal f,const XU32 idx)
    : w(f),mVertexIndex(idx) 
  {
  }

  //----------------------------------------------------------------------------------------------------------------------
  XSkinCluster::~XSkinCluster()
  {
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XSkinCluster::AreWeightsValid(XIndexList& invalid_vtx_weight) const
  {

    XM2::vector<XSkinnedVertex>::const_iterator it = m_SkinWeights.begin();
    for( XU32 idx=0; it != m_SkinWeights.end(); ++it,++idx )
    {
      XReal w=0.0f;
      for( XSkinnedVertex::const_iterator itv = it->begin(); itv != it->end(); ++itv)
      {
        const XSkinWeight& sw= *itv;
        w+= sw.w;
      }
      if (w<0.99999f||w>1.00001f)
      {
        invalid_vtx_weight.push_back(idx);
      }      
    }

    return invalid_vtx_weight.size()==0;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XSkinCluster::NormaliseWeights(const XIndexList& invalid_vtx_weight)
  {
    // loop through all invalid vertices
    for(XIndexList::const_iterator itIndices = invalid_vtx_weight.begin(); itIndices != invalid_vtx_weight.end(); ++itIndices)
    {
      XSkinnedVertex& skinVert = m_SkinWeights[(*itIndices)];

      // skip the vert if its empty
      if(!skinVert.size())
        continue;

      XReal total = 0.0f;
      // calculate the total weight for this skin
      for( XSkinnedVertex::const_iterator itv = skinVert.begin(); itv != skinVert.end(); ++itv)
      {
        total += itv->w;
      }

      const XReal fixValue = 1.0f / total;
      for(XSkinnedVertex::iterator itv = skinVert.begin(); itv != skinVert.end(); ++itv)
      {
        itv->w *= fixValue;
      }
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XSkinCluster::SetAffected(const XGeometry* geom) 
  {
    // if you assert here, the geom is NULL or from another XModel
    XMD_ASSERT( geom && IsValidObj(geom) );
    if(!geom)
      return false;
    XIndexList::iterator it = nmtl::find(m_AffectedGeometry.begin(),m_AffectedGeometry.end(), geom->GetID() );
    if (it != m_AffectedGeometry.end())
    {
      XGlobal::GetLogger()->Logf(XBasicLogger::kWarning,"XSkinCluster::SetAffected: adding geometry that is already affected");
      return false;
    }
    m_AffectedGeometry.push_back(geom->GetID());
    m_SkinWeights.resize(geom->GetNumPoints());
    return true;    
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XSkinCluster::GetClusterWeights(const XU32 joint_idx, XClusterWeightList& returned) const 
  {
    XM2::vector<XSkinnedVertex>::const_iterator it = m_SkinWeights.begin();
    for( XU32 idx=0; it != m_SkinWeights.end(); ++it,++idx )
    {
      for( XSkinnedVertex::const_iterator itv = it->begin(); itv != it->end(); ++itv)
      {
        if(joint_idx==itv->m_JointID)
        {
          returned.push_back( XClusterWeight(itv->w,idx) );
        }
      }
    }
    return returned.size()>0;
  }

  //----------------------------------------------------------------------------------------------------------------------
  XU32 XSkinCluster::AddInfluence(const XBone* bone)
  {
    // if you assert here, the bone is NULL or from another XModel
    XMD_ASSERT( (bone) && IsValidObj(bone) );

    if(!bone)
      return false;

    XIndexList::iterator it = m_Influences.begin();
    for ( XU32 i=0 ; it != m_Influences.end(); ++it,++i) 
    {
      if (*it == bone->GetID()) 
      {
        return i;
      }
    }
    m_Influences.push_back( bone->GetID() );
    return (XU32)m_Influences.size()-1;
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XSkinCluster::AddWeight(const XU32 control_point_index,const XU32 joint_idx,const XReal w)
  {
    // if invalid joint IDX
    if (m_Influences.size()<=joint_idx) 
    {
      return false;
    }
    if (control_point_index<m_SkinWeights.size()) 
    {
      XSkinnedVertex& sv = m_SkinWeights[ control_point_index ];
      XSkinnedVertex::iterator itv = sv.begin();
      for (;itv != sv.end(); ++itv) 
      {
        if (itv->m_JointID == joint_idx) 
        {
          itv->w = w;
          return true;
        }
      }
      XSkinWeight sw;
      sw.m_JointID = joint_idx;
      sw.w = w;
      sv.push_back(sw);
      return true;
    }
    return false;
  }

  //----------------------------------------------------------------------------------------------------------------------
  XU32 XSkinCluster::GetNumInfluences() const 
  {
    return static_cast<XU32>(m_Influences.size());
  }

  //----------------------------------------------------------------------------------------------------------------------
  XBone* XSkinCluster::GetInfluence(const XU32 idx) const
  {
    if(idx<GetNumInfluences()) 
    {
      XBase* p= m_pModel->FindNode(m_Influences[idx]);
      if(p)
        return p->HasFn<XBone>();
    }
    return 0;
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XSkinCluster::RemoveInfluence(const XBone* bone)
  {
    if( bone && IsValidObj(bone) )
    {
      for (XU32 i=0;i!=m_Influences.size();++i)
      {
        if (m_Influences[i]==bone->GetID())
        {
          return RemoveInfluence(i);
        }
      }
    }
    return false;
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XSkinCluster::RemoveInfluence(const XU32 bone_index)
  {
    if(bone_index<GetNumInfluences()) 
    {
      // remove all skin weights for this bone
      XM2::vector<XSkinnedVertex>::iterator it = m_SkinWeights.begin();
      for (;it != m_SkinWeights.end();++it)
      {
        XSkinnedVertex::iterator itv = it->begin();
        for (;itv != it->end(); ++itv)
        {
          if(itv->m_JointID == bone_index)
          {
            it->erase(itv);
            break;
          }
        }
      }

      // remove the bone from the influence array
      m_Influences.erase( m_Influences.begin() + bone_index );
      return true;
    }
    return false;
  }

  //----------------------------------------------------------------------------------------------------------------------
  XGeometry* XSkinCluster::GetAffected() const
  {
    if(m_AffectedGeometry.size()) 
    {
      XBase* p= m_pModel->FindNode(m_AffectedGeometry[0]);
      if(p)
        return p->HasFn<XGeometry>();
    }
    return 0;
  }

  //----------------------------------------------------------------------------------------------------------------------
  const XM2::vector<XSkinnedVertex>& XSkinCluster::VertexData() const 
  {
    return m_SkinWeights;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XSkinCluster::GetSkinWeights(XM2::vector<XSkinnedVertex>& skin_weights) const
  {
    skin_weights = m_SkinWeights;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XSkinCluster::SetSkinWeights(const XM2::vector<XSkinnedVertex>& skin_weights)
  {
    m_SkinWeights = skin_weights;
  }

  //----------------------------------------------------------------------------------------------------------------------
  XSkinCluster::XSkinCluster(XModel* M)
    : XDeformer(M),
      m_Influences(),
      m_SkinWeights()
  {
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XSkinCluster::WriteChunk(std::ostream& ofs)
  {
    if (!XDeformer::WriteChunk(ofs)) 
      return false;

    ofs << "\tnum_influences " << static_cast<XU32>(m_Influences.size()) << "\n";
    {
      XIndexList::iterator it = m_Influences.begin();
      ofs << "\t\t";
      for( ; it != m_Influences.end(); ++it )
      {
        ofs << *it << " ";
      }
      ofs << "\n";
    }

    ofs << "\tnum_points " << static_cast<XU32>(m_SkinWeights.size()) << "\n";
    ofs << "\tweights\n";

    {

      XM2::vector<XSkinnedVertex>::iterator it = m_SkinWeights.begin();
      for( ; it != m_SkinWeights.end(); ++it )
      {
        ofs << "\t\t" << static_cast<XU32>(it->size()) << "\t";

        for( XSkinnedVertex::iterator itv = it->begin(); itv != it->end(); ++itv)
        {
          ofs << " " << itv->m_JointID << " " << itv->w ;
        }
        ofs << "\n";
      }
    }
    return ofs.good();
  }

  //----------------------------------------------------------------------------------------------------------------------
  std::ostream& operator<<(std::ostream& ofs,const XSkinWeight& sw) 
  {
    return ofs << "{" << sw.m_JointID << "," << sw.w << "}";
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XSkinCluster::DoData(XFileIO& io)
  {
    DUMPER(XSkinCluster);

    IO_CHECK( XDeformer::DoData(io) );

    IO_CHECK( io.DoDataVector(m_Influences) );
    DAPARAM( m_Influences );

    XU32 size = static_cast<XU32>(m_SkinWeights.size());
    IO_CHECK( io.DoData(&size) );

    if( io.IsReading() )
      m_SkinWeights.resize(size);

    XM2::vector<XSkinnedVertex>::iterator it = m_SkinWeights.begin();
    for( ; it != m_SkinWeights.end(); ++it ) 
    {
      IO_CHECK( io.DoDataVector(*it) );
    }

    return true;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XSkinCluster::DoCopy(const XBase* rhs)
  {
    const XSkinCluster* cb = rhs->HasFn<XSkinCluster>();
    XMD_ASSERT(cb);
    m_Influences = cb->m_Influences;
    m_SkinWeights = cb->m_SkinWeights;
    XDeformer::DoCopy(cb);
  }

  //----------------------------------------------------------------------------------------------------------------------
  XU32 XSkinCluster::GetDataSize() const 
  {
    size_t sz = 8;
    XM2::vector<XSkinnedVertex>::const_iterator it = m_SkinWeights.begin();
    for( ; it != m_SkinWeights.end(); ++it ) 
    {
      sz += (4 + sizeof(XSkinWeight) * static_cast<XU32>(it->size()));
    }
    sz += XDeformer::GetDataSize() +
          sizeof(XId)*m_Influences.size();
    return (XU32)sz;
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XSkinCluster::ReadChunk(std::istream& ifs)
  {
    if (!XDeformer::ReadChunk(ifs))
      return false;

    XU32 ni=0,temp;

    READ_CHECK("num_influences",ifs);
    ifs >> ni;
    m_Influences.resize(ni);
    XIndexList::iterator it = m_Influences.begin();

    for(; it != m_Influences.end(); ++it) 
    {
      ifs >> *it;
    }

    READ_CHECK("num_points",ifs);
    ifs >> temp;

    m_SkinWeights.resize(temp);
    READ_CHECK("weights",ifs);

    XM2::vector<XSkinnedVertex>::iterator itw = m_SkinWeights.begin();
    for( ; itw != m_SkinWeights.end(); ++itw )
    {
      ifs >> temp;
      itw->resize(temp);

      for( XSkinnedVertex::iterator itv = itw->begin(); itv != itw->end(); ++itv)
      {
        ifs >> itv->m_JointID >> itv->w;
      }
    }

    XIndexList list;
    if(!AreWeightsValid(list))
    {
      NormaliseWeights(list);
    }

    return true;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XSkinCluster::PreDelete(XIdSet& extra_nodes)
  {
    XDeformer::PreDelete(extra_nodes);
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XSkinCluster::NodeDeath(XId id)
  {
    XIndexList::iterator it = m_Influences.begin();
    for( ; it != m_Influences.end(); ++it )
    {
      if( *it == id )
      {
        m_Influences.erase(it);
        break;
      }
    }

    return XDeformer::NodeDeath(id);
  }

  //----------------------------------------------------------------------------------------------------------------------
  XFn::Type XSkinCluster::GetApiType() const 
  {
    return XFn::SkinCluster;
  }

  //----------------------------------------------------------------------------------------------------------------------
  XBase* XSkinCluster::GetFn(XFn::Type type)
  {
    if (type == XFn::SkinCluster)
      return (XSkinCluster*)this;
    return XDeformer::GetFn(type);
  }

  //----------------------------------------------------------------------------------------------------------------------
  const XBase* XSkinCluster::GetFn(XFn::Type type) const
  {
    if(XFn::SkinCluster==type)
      return (const XSkinCluster*)this;
    return XDeformer::GetFn(type);
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XSkinCluster::SetInfluences( const XBoneList& bones )
  {
    XIndexList indices = m_Influences;
    m_Influences.clear();
    {
      XBoneList::const_iterator it = bones.begin();
      for (;it != bones.end(); ++it)
      {
        if((*it))
          m_Influences.push_back((*it)->GetID());
      }
    }

    XIndexList::iterator it = indices.begin();
    for (;it != indices.end();++it)
    {
      XU32 index = *it;
      XU32 i=0;
      for (i=0;i!=m_Influences.size();++i)
      {
        if(m_Influences[i] == index)
        {
          *it = i;
          break;
        }
      }
      if (i==m_Influences.size())
      {
        *it=0;
      }
    }

    {
      XM2::vector<XSkinnedVertex>::iterator it = m_SkinWeights.begin();
      for (;it != m_SkinWeights.end(); ++it)
      {
        XSkinnedVertex::iterator sv = it->begin();
        for (;sv != it->end();++sv)
        {
          XSkinWeight& sw = *sv;
          if( sw.m_JointID < indices.size() )
          {
            sw.m_JointID = indices[ sw.m_JointID ];
          }
          else
          {
            sw.m_JointID = 0;
          }
        }
      }
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XSkinCluster::GetInfluences( XBoneList& bones ) const 
  {
    bones.resize(0);
    XIndexList::const_iterator it = m_Influences.begin();
    for( ; it != m_Influences.end(); ++it )
    {
      XBase* ptr = m_pModel->FindNode(*it);
      if(ptr && ptr->HasFn<XBone>())
        bones.push_back( ptr->HasFn<XBone>() );
    }
  }
}
