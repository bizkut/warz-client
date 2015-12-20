//----------------------------------------------------------------------------------------------------------------------
/// \file JointCluster.cpp
/// \note (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include "XMD/JointCluster.h"
#include "XMD/Model.h"
#include "XMD/FileIO.h"

namespace XMD
{
  //----------------------------------------------------------------------------------------------------------------------
  XJointCluster::XJointCluster(XModel* pmod) 
    : XDeformer(pmod),
    m_Bone(0),
    m_PointIndices(),
    m_Weights()
  {
  }

  //----------------------------------------------------------------------------------------------------------------------
  XJointCluster::~XJointCluster(void)
  {
  }

  //----------------------------------------------------------------------------------------------------------------------
  XBone* XJointCluster::GetInfluenceJoint() const
  {
    XBase* ptr = m_pModel->FindNode(m_Bone);
    if (ptr) 
    {
      return ptr->HasFn< XBone >();
    }
    return 0;
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XJointCluster::SetInfluenceJoint(const XBase* p)
  {
    if (!p)
    {
      m_Bone = 0;
      return true;
    }

    // if you assert here, p is NULL or not from this XModel
    XMD_ASSERT( IsValidObj(p) );
    if ( IsValidObj(p) && p->HasFn< XBone >() )
    {
      m_Bone = p->GetID();
      return true;
    }
    
    // if you assert here, p is not an XBone derived class
    XMD_ASSERT(0);
    return false;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XJointCluster::AddPoint(const XU32 idx,XReal w)
  {
    XIndexList::iterator it = m_PointIndices.begin();
    XRealArray::iterator itw = m_Weights.begin();

    for( ; it != m_PointIndices.end(); ++it, ++itw )
    {
      if (*it == idx) 
      {
        *itw = w;
        return;
      }
    }

    m_PointIndices.push_back(idx);
    m_Weights.push_back(w);
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XJointCluster::RemovePoint(const XU32 idx)
  {
    XIndexList::iterator it = m_PointIndices.begin();
    XRealArray::iterator itw = m_Weights.begin();

    for( ; it != m_PointIndices.end(); ++it, ++itw )
    {
      if (*it == idx) 
      {
        m_PointIndices.erase(it);
        m_Weights.erase(itw);
        return;
      }
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  XReal XJointCluster::GetWeight(const XU32 idx) const
  {
    XIndexList::const_iterator it = m_PointIndices.begin();
    XRealArray::const_iterator itw = m_Weights.begin();

    for( ; it != m_PointIndices.end(); ++it, ++itw )
    {
      if (*it == idx) 
      {
        return *itw;
      }
    }
    return 0;
  }

  //----------------------------------------------------------------------------------------------------------------------
  XFn::Type XJointCluster::GetApiType() const
  {
    return XFn::JointCluster;
  }

  //----------------------------------------------------------------------------------------------------------------------
  XBase* XJointCluster::GetFn(XFn::Type type)
  {
    if(XFn::JointCluster==type)
      return (XJointCluster*)this;
    return XDeformer::GetFn(type);
  }

  //----------------------------------------------------------------------------------------------------------------------
  const XBase* XJointCluster::GetFn(XFn::Type type) const
  {
    if(XFn::JointCluster==type)
      return (const XJointCluster*)this;
    return XDeformer::GetFn(type);
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XJointCluster::NodeDeath(XId id)
  {
    return XDeformer::NodeDeath(id);
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XJointCluster::PreDelete(XIdSet& extra_nodes)
  {
    XDeformer::PreDelete(extra_nodes);

    const XDeletePolicy& dp = GetModel()->GetDeletePolicy();

    if ( dp.IsEnabled(XDeletePolicy::RelatedDeformerInputs) ) 
    {
    }
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XJointCluster::ReadChunk(std::istream& ifs)
  {
    if (!XDeformer::ReadChunk(ifs)) 
      return false;

    READ_CHECK("joint_id",ifs);
    ifs >> m_Bone;


    XU32 nw=0;
    READ_CHECK("num_weights",ifs);
    ifs >> nw;

    m_PointIndices.resize(nw); 
    m_Weights.resize(nw);

    XIndexList::iterator iti = m_PointIndices.begin();
    XRealArray::iterator itw = m_Weights.begin();
    for( ; iti != m_PointIndices.end(); ++iti,++itw )
    {
      ifs >> *iti >> *itw;
    }
    return ifs.good();
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XJointCluster::WriteChunk(std::ostream& ofs)
  {
    if (!XDeformer::WriteChunk(ofs)) 
      return false;

    ofs << "\tjoint_id " << m_Bone << "\n";
    ofs << "\t\tnum_weights " << static_cast<XU32>(m_PointIndices.size()) << "\n"; 

    XIndexList::iterator iti = m_PointIndices.begin();
    XRealArray::iterator itw = m_Weights.begin();
    for( ; iti != m_PointIndices.end(); ++iti,++itw )
    {
      ofs << "\t\t\t" << *iti << " " << *itw << std::endl;
    }
    return ofs.good();
  }

  //----------------------------------------------------------------------------------------------------------------------
  bool XJointCluster::DoData(XFileIO& io)
  {
    DUMPER(XJointCluster);

    IO_CHECK( XDeformer::DoData(io) );

    IO_CHECK( io.DoData(&m_Bone) );
    DPARAM( m_Bone );

    IO_CHECK( io.DoDataVector(m_PointIndices) );
    DAPARAM( m_PointIndices );

    IO_CHECK( io.DoDataVector(m_Weights) );
    DAPARAM( m_Weights );

    return true;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XJointCluster::DoCopy(const XBase* rhs)
  {
    const XJointCluster* cb = rhs->HasFn<XJointCluster>();
    XMD_ASSERT(cb);
    m_Bone = cb->m_Bone;
    m_PointIndices = cb->m_PointIndices;
    m_Weights = cb->m_Weights;
    XDeformer::DoCopy(cb);
  }

  //----------------------------------------------------------------------------------------------------------------------
  XU32 XJointCluster::GetDataSize() const 
  {
    XU32 sz = (XU32)(sizeof(XId)+8 + m_Weights.size()*sizeof(XReal) +
      sizeof(XId)*m_PointIndices.size());
    return sz + XDeformer::GetDataSize();
  }

  //----------------------------------------------------------------------------------------------------------------------
  const XRealArray& XJointCluster::Weights() const
  {
    return m_Weights;
  }

  //----------------------------------------------------------------------------------------------------------------------
  const XIndexList& XJointCluster::Indices() const 
  {
    return m_PointIndices;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XJointCluster::GetWeights(XRealArray& outputData) const 
  {
    outputData.resize( m_Weights.size() );
    XRealArray::const_iterator it = m_Weights.begin();
    XRealArray::iterator it_out = outputData.begin();
    for( ; it != m_Weights.end(); ++it, ++it_out )
      *it_out = *it;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XJointCluster::SetWeights(const XRealArray& newData)
  {
    m_Weights.resize( newData.size() );
    XRealArray::iterator it = m_Weights.begin();
    XRealArray::const_iterator it_out = newData.begin();
    for( ; it != m_Weights.end(); ++it, ++it_out )
      *it = *it_out;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XJointCluster::GetIndices(XIndexList& outputData) const 
  {
    outputData.resize( m_PointIndices.size() );
    XIndexList::const_iterator it = m_PointIndices.begin();
    XIndexList::iterator it_out = outputData.begin();
    for( ; it != m_PointIndices.end(); ++it, ++it_out )
      *it_out = *it;
  }

  //----------------------------------------------------------------------------------------------------------------------
  void XJointCluster::SetIndices(const XIndexList& newData)
  {
    m_PointIndices.resize( newData.size() );
    XIndexList::iterator it = m_PointIndices.begin();
    XIndexList::const_iterator it_out = newData.begin();
    for( ; it != m_PointIndices.end(); ++it, ++it_out )
      *it = *it_out;
  }
}
