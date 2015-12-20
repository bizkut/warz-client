//----------------------------------------------------------------------------------------------------------------------
/// \file   SkinPruner.cpp
/// \date   11-9-2004
/// \author Rob Bateman [http://robthebloke.org]
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------

#include <algorithm>
#include "XMU/SkinPruner.h"
#include "XMD/Geometry.h"
#include "XMD/Bone.h"
#include "XMD/SkinCluster.h"
#include "XMD/JointCluster.h"
#include "nmtl/algorithm.h"

using namespace XMD;

namespace XMU 
{
NMTL_POD_VECTOR_EXPORT(XMUSkinWeight,XM2EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XMUVertexWeight,XM2EXPORT,XM2);
NMTL_POD_VECTOR_EXPORT(XMUJointCluster,XM2EXPORT,XM2);
//----------------------------------------------------------------------------------------------------------------------
XMUSkinWeight::XMUSkinWeight()
  : Weight(0), JointIDX(0xFFFF) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XMUSkinWeight::XMUSkinWeight( XMD::XId joint,XReal w) 
  :Weight(w),JointIDX(joint) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XMUSkinWeight::~XMUSkinWeight() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XMUSkinWeight::XMUSkinWeight(const XMUSkinWeight& sw)
  : Weight(sw.Weight),JointIDX(sw.JointIDX)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XMUSkinWeight::operator<(const XMUSkinWeight& sw) const 
{
  return Weight < sw.Weight;
}

//----------------------------------------------------------------------------------------------------------------------
XMUVertexWeight::XMUVertexWeight()
  : Weight(0), VertexIDX(0) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XMUVertexWeight::XMUVertexWeight(XU32 idx,XReal w) 
  : Weight(w),VertexIDX(idx) 
{
}

//----------------------------------------------------------------------------------------------------------------------
XMUVertexWeight::~XMUVertexWeight() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XMUVertexWeight::XMUVertexWeight(const XMUVertexWeight& sw)
  : Weight(sw.Weight),VertexIDX(sw.VertexIDX)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XMUVertexWeight::operator<(const XMUVertexWeight& sw) const 
{
  return VertexIDX < sw.VertexIDX;
}

//----------------------------------------------------------------------------------------------------------------------
XMUJointCluster::XMUJointCluster() 
  : m_JointIDX(0xFFFF),m_VertexWeights()
{
}

//----------------------------------------------------------------------------------------------------------------------
XMUJointCluster::~XMUJointCluster() 
{
}

//----------------------------------------------------------------------------------------------------------------------
XMUJointCluster::XMUJointCluster(const XMUJointCluster& sw)
  : m_JointIDX(sw.m_JointIDX),m_VertexWeights(sw.m_VertexWeights)
{
}

//----------------------------------------------------------------------------------------------------------------------
bool XMUJointCluster::IsMember(XU32 idx,XReal &w) 
{
  XMUVertexWeightList::iterator it = m_VertexWeights.begin();
  for( ; it != m_VertexWeights.end(); ++it )
  {
    if(it->VertexIDX == idx)
    {
      w = it->Weight;
      return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XMUJointCluster::operator<(const XMUJointCluster& sw) const
{
  return m_JointIDX < sw.m_JointIDX;
}

//----------------------------------------------------------------------------------------------------------------------
static void XMUShrinkWeights(XMUSkinWeightList& weights,XU16 num) 
{
  // put smallest weights first
  nmtl::sort(weights.begin(),weights.end());

  // loop till we have enough weights
  while(num < weights.size())
  {
    // remove least significant weight
    XReal w_to_remove = weights.begin()->Weight;
    XReal w_minus_remove = 1.0f-w_to_remove;

    // loop through remaining weights
    XMUSkinWeightList::iterator it = weights.begin();
    for( ; it != weights.end(); ++it )
    {
      // add the removed weight as a proportion of the remaining weights
      XReal w_to_add = (it->Weight/w_minus_remove) * w_to_remove;
      it->Weight += w_to_add;
    }

    // remove the least important weight
    weights.erase(weights.begin());
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void XMUNormalizeWeights(XMUSkinWeightList& weights)
{
  //Also make sure that the weights are normalized
  XReal sumWeights = 0.0f;
  XMUSkinWeightList::iterator it = weights.begin();
  for(; it !=  weights.end();++it)
  {
    sumWeights += it->Weight;
  }

  if(sumWeights< 1.0f)
  {
    XReal propWeightDiffToAdd = (1.0f-sumWeights)/(XReal)weights.size();
    XMUSkinWeightList::iterator it = weights.begin();
    for(; it !=  weights.end();++it)
    {
      it->Weight += propWeightDiffToAdd;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
XSkinPruner::XSkinPruner(const XMD::XGeometry* geom,XMD::XU32 maxWeights) 
{
  // get deformers affecting the geometry object
  XMD::XDeformerList defs;
  geom->GetDeformerQueue(defs);

  // resize the weights array
  m_Weights.resize(geom->GetNumPoints());

  {
    // do any skin clusters first...
    XDeformerList::iterator it = defs.begin();
    for( ; it != defs.end(); ++it )
    {
      XBase* pb = (*it);
      // get skin interface
      XSkinCluster* ISkin = pb->HasFn< XSkinCluster >();
      if(ISkin) 
      {
        // construct influence array
        for(XMD::XU32 i=0;i!=ISkin->GetNumInfluences();++i)
        {
          XBone* pInfluence = ISkin->GetInfluence(i);
          if(pInfluence)
            m_Influences.push_back( pInfluence->GetID() );
          else
            m_Influences.push_back(0);
        }

        // copy over skinning info
        const XM2::vector<XSkinnedVertex>& verts = ISkin->VertexData();
        XM2::vector<XSkinnedVertex>::const_iterator itv = verts.begin();
        XM2::vector<XMUSkinWeightList>::iterator itw = m_Weights.begin();

        // copy all skin weights
        for( ; itv != verts.end(); ++itv,++itw ) 
        {
          XSkinnedVertex::const_iterator itsv = itv->begin();
          for( ; itsv != itv->end(); ++itsv )
          {
            itw->push_back( XMUSkinWeight(itsv->m_JointID,itsv->w) );
          }
        }

        break;
      }
    }
  }

  // add in any attached joint clusters
  {
    // find each joint cluster
    XDeformerList::iterator it = defs.begin();
    for( ; it != defs.end(); ++it )
    {
      XBase* pb = (*it);
      // get cluster
      XJointCluster* ICluster = pb->HasFn<XJointCluster>();
      if(ICluster)
      {
        // get influence bone
        XBone* bone = ICluster-> GetInfluenceJoint();
        XMD::XU32 idx=0;
        XMD::XIndexList::iterator iti = m_Influences.begin();

        // loop through influences to find influence index
        for( ; iti != m_Influences.end(); ++iti,++idx )
        {
          if(bone->GetID() == *iti)
            break;
        }
        if(iti == m_Influences.end())
        {
          m_Influences.push_back( bone->GetID() );
        }

        XIndexList VertIndices;
        XMD::XRealArray Weights;

        // get vertex indices and vertex weights from cluster
        ICluster-> GetIndices(VertIndices);
        ICluster-> GetWeights(Weights);

        // loop through all weights and indices and add into skin cluster
        XIndexList::iterator itvi = VertIndices.begin();
        XMD::XRealArray::iterator itw = Weights.begin();
        for( ; itvi != VertIndices.end(); ++itvi, ++itw )
        {
          m_Weights[ *itvi ].push_back( XMUSkinWeight(idx, *itw) );
        }
      }
    }
  }

  // loop through the weights for each vertex and shrink the number of
  // skin weights per vertex to the requested amount. Also normalise the 
  // weights so that the sum of the weight is 1.0f.
  //
  XM2::vector<XMUSkinWeightList>::iterator it = m_Weights.begin();
  for( ; it != m_Weights.end(); ++it )
  {
    XMUShrinkWeights(*it,(XMD::XU16)maxWeights);
    XMUNormalizeWeights(*it);
  }

  // remove any redundant joints
  PruneUnusedJoints();
}

//----------------------------------------------------------------------------------------------------------------------
const bool XSkinPruner::IsRigid() const
{
  // loop through each vertex
  XM2::vector<XMUSkinWeightList>::const_iterator it = m_Weights.begin();
  for( ; it != m_Weights.end(); ++it )
  {
    // loop through skin weights for each joint that has an influence on the vertex.
    if( it->size() != 1 )
      return false;
  }
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void XSkinPruner::Get(XMUJointClusterList& clusters) const 
{
  // resize the number of returned clusters.
  clusters.resize(m_Influences.size());

  // allocate clusters
  XMD::XIndexList::const_iterator iti = m_Influences.begin();
  XMUJointClusterList::iterator itc = clusters.begin();
  for( ; itc != clusters.end(); ++itc,++iti) 
  {
    *itc = new XMUJointCluster;
    XMD_ASSERT( *itc );

    (*itc)->m_JointIDX = *iti;
  }

  XMD::XU32 vidx=0;

  // loop through each vertex
  XM2::vector<XMUSkinWeightList>::const_iterator it = m_Weights.begin();
  for( ; it != m_Weights.end(); ++it,++vidx )
  {
    // loop through skin weights for each joint that has an influence on the vertex.
    XMUSkinWeightList::const_iterator itw = it->begin();
    for( ; itw != it->end(); ++itw )
    {
      XMD_ASSERT(clusters.size() > itw->JointIDX);
      // if the joint index matches the requested one...
      clusters[itw->JointIDX]->m_VertexWeights.push_back(XMUVertexWeight(vidx,itw->Weight));
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XSkinPruner::Get(XId jointIndex,XMUJointCluster& cluster) const
{
  XMD::XU32 idx=0;
  cluster.m_JointIDX=0;

  {
    XIndexList::const_iterator it = m_Influences.begin();
    // hunt for the influence id and an index
    for(; it != m_Influences.end();++it )
    {
      if( *it == jointIndex )
        goto sorted;
      ++idx;
    }
  }
  return;
  sorted: ;

  XMD::XU32 vidx=0;

  cluster.m_JointIDX = jointIndex;

  {
    // loop through each vertex
    XM2::vector<XMUSkinWeightList>::const_iterator it = m_Weights.begin();
    for( ; it != m_Weights.end(); ++it,++vidx )
    {
      // loop through skin weights for each joint that has an influence on the vertex.
      XMUSkinWeightList::const_iterator itw = it->begin();
      for( ; itw != it->end(); ++itw ) 
      {
        // if the joint index matches the requested one...
        if(itw->JointIDX == idx) 
        {
          cluster.m_VertexWeights.push_back(XMUVertexWeight(vidx,itw->Weight));
        }
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void XSkinPruner::GetWeights(XMD::XU32 vertexIndex,XMUSkinWeightList& weights) const 
{
  if(vertexIndex<m_Weights.size())
  {
    weights = m_Weights[vertexIndex];
  }
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU32 XSkinPruner::GetNumInfluences() const 
{
  return static_cast<XMD::XU32>(m_Influences.size());
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XU32 XSkinPruner::GetNumVertices() const 
{
  return static_cast<XMD::XU32>(m_Weights.size());
}

//----------------------------------------------------------------------------------------------------------------------
void XSkinPruner::PruneUnusedJoints()
{
  for(XMD::XU32 i=0;i!=m_Influences.size(); )
  {
    if( !RemoveBone(i) )
    {
      ++i;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool XSkinPruner::HasWeights(XMD::XU32 idx) const 
{
  // loop through each vertex
  XM2::vector<XMUSkinWeightList>::const_iterator it = m_Weights.begin();
  for( ; it != m_Weights.end(); ++it )
  {
    // loop through skin weights for each joint that has an influence on the vertex.
    XMUSkinWeightList::const_iterator itw = it->begin();
    for( ; itw != it->end(); ++itw )
    {
      // if the joint index matches the requested one...
      if( idx == itw->JointIDX )
        return true;
    }
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool XSkinPruner::RemoveBone(XMD::XU32 idx)  
{
  if( (idx>=m_Influences.size()) || HasWeights(idx) )
    return false;

  // loop through each vertex
  XM2::vector<XMUSkinWeightList>::iterator it = m_Weights.begin();
  for( ; it != m_Weights.end(); ++it )
  {
    // loop through skin weights for each joint that has an influence on the vertex.
    XMUSkinWeightList::iterator itw = it->begin();
    for( ; itw != it->end(); ++itw )
    {
      // if the joint index matches the requested one...
      if( idx <= itw->JointIDX )
        --itw->JointIDX;
    }
  }
  m_Influences.erase(m_Influences.begin()+idx);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
XMD::XId XSkinPruner::GetInfluence(XMD::XU32 idx) const
{
  if(m_Influences.size()>idx)
    return m_Influences[idx];
  return 0;
}
}

