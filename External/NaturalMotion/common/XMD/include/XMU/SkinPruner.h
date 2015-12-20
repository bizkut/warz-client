//----------------------------------------------------------------------------------------------------------------------
/// \file   SkinPruner.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file impliments a utility, XSkinPruner to reduce the data size of skin and joint clusters. By simply 
///         providing a pointer to a geometry object, the utility class XSkinPruner will construct itself from
///         and skin or joint clusters attached. 
///         The data set constructed basically merges joint and skin clusters so that you can make use of a single 
///         generalised interface to read back both forms of skinning. The data can then be queried as joint or skin
///         clusters.
///         The data set can be reduced so that a maximum number of weights per vertex can be en-forced. The data 
///         set can be optimised by requesting that rigid vertices (ie, has only 1 influence weight of
///         1) be removed from the skin cluster as a set of joint clusters.
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/XFn.h"
//----------------------------------------------------------------------------------------------------------------------
#ifndef DOXYGEN_PROCESSING
namespace XMD
{
  class XMD_EXPORT XGeometry;
}
#endif
namespace XMU
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUSkinWeight
/// \brief a structure to hold a joint index and a weighting value.
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XMUSkinWeight
{
  /// \brief  ctor
  XMUSkinWeight();

  /// \brief  ctor
  /// \param  joint - the index of the joint
  /// \param  w - the weight of the joint
  XMUSkinWeight(XMD::XId joint, XM2::XReal w);

  /// \brief  dtor
  ~XMUSkinWeight();

  /// \brief  copy ctor
  /// \param  sw - the skin weight to copy
  XMUSkinWeight(const XMUSkinWeight& sw);

  /// \brief  for std::sort
  /// \param  sw - the skin weight to compare
  /// \return true if this weight is less than vw
  bool operator<(const XMUSkinWeight& sw) const;

  /// the weight of the influence
  XM2::XReal Weight;

  /// the influence joint ID
  XMD::XId JointIDX;
};
/// typedef a list of skin clusters
typedef XM2::pod_vector<XMUSkinWeight> XMUSkinWeightList;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUVertexWeight 
/// \brief a structure to hold a joint index and a weighting value.
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XMUVertexWeight 
{
  /// \brief  ctor
  XMUVertexWeight();

  /// \brief  ctor
  /// \param  idx - the vertex index
  /// \param  w - the skin weight
  XMUVertexWeight(XM2::XU32 idx, XM2::XReal w);

  /// \brief  dtor
  ~XMUVertexWeight();

  /// \brief  copy ctor
  /// \param  vw - the vertex weight to copy
  XMUVertexWeight(const XMUVertexWeight& vw);

  /// \brief  for XM2::sort
  /// \param  vw - the vertex weight to compare
  /// \return true if this weight is less than vw
  bool operator<(const XMUVertexWeight& vw) const;

  /// the weight of the influence
  XM2::XReal Weight;

  /// the vertex index under influence
  XM2::XU32 VertexIDX;
};
/// typedef a list of skin clusters
typedef XM2::pod_vector<XMUVertexWeight> XMUVertexWeightList;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XMUJointCluster
/// \brief a structure to hold a joint index and the vertex weights for the vertices affected by this joint
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XMUJointCluster
{
  /// \brief  ctor
  XMUJointCluster();

  /// \brief  dtor
  ~XMUJointCluster();

  /// \brief  copy ctor
  /// \param  sw - the joint cluster to copy
  XMUJointCluster(const XMUJointCluster& sw);

  /// \brief  for std::set to organise the joint clusters
  ///         via the joint idx.
  /// \param  sw - the joint cluster to compare
  /// \return true if this joint idx is less than sw's.
  bool operator<(const XMUJointCluster& sw) const;

  /// \brief  returns true if the specified skin weight is a member of this joint cluster.
  /// \param  idx - the vertex index
  /// \param  w - it's weight
  /// \return true if a member
  bool IsMember(XM2::XU32 idx, XM2::XReal &w);

  /// the joint ID for the cluster
  XMD::XId m_JointIDX;

  /// the list of vertex weights assigned to this joint
  XMUVertexWeightList m_VertexWeights;
};
/// typedef a list of skin clusters
typedef XM2::pod_vector<XMUJointCluster*> XMUJointClusterList;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMU::XSkinPruner 
/// \brief This class is used to reduce the data required for a skin cluster. It also allows for conversions between 
///        skin clusters and joint clusters (they are essentially the same, just a different representation).
///        When constructing and instance of the class, simply pass in a pointer to the geometry object on which 
///        skinning is applied. This class will then construct a skin cluster from any attached joint or skin clusters.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XSkinPruner 
{
public:

  /// \brief  constructs the skin pruner from joint clusters assigned to the geometry object.
  /// \param  geom - the geometry to extract the attached joint clusters
  /// \param  maxWeights - the max weights allowed per vertex
  XSkinPruner(const XMD::XGeometry* geom, XM2::XU32 maxWeights = 4);

  /// \brief  dtor
  virtual ~XSkinPruner() {}

  /// \brief  returns the skin cluster data as a set of joint clusters. Each cluster will hold all weights for 
  ///         vertices affected by a specific joint. i.e., the weights are sorted by the joint indices.
  /// \param  clusters - the returned array of joint clusters
  /// \note   This is a conversion process so may incur a bit of overhead :p
  void Get(XMUJointClusterList& clusters) const;

  /// \brief  returns the skin cluster data for a specific joint. ie, a joint cluster
  /// \param  jointIndex - the index of the joint to query
  /// \param  cluster - the returned joint cluster
  void Get(XMD::XId jointIndex, XMUJointCluster& cluster) const;

  /// \brief  returns the skin cluster data for a specific vertex. ie, the weighting
  ///         values and which joints are used as the influences.
  /// \param  vertexIndex - the index of the vert to query
  /// \param  weights - the returned list of vertex weights.
  /// \note   This is a conversion process so may incur a bit of overhead :p
  void GetWeights(XM2::XU32 vertexIndex, XMUSkinWeightList& weights) const;

  /// \brief  this function returns the number of influence joints used by the skin cluster
  /// \return the number of influences
  XMD::XU32 GetNumInfluences() const;

  /// \brief  returns the influence ID for the specified joint index.
  /// \param  idx  -  the index to query
  /// \return the node index
  /// \note   this is needed because once the pruner has occurred, the number of influences may have been reduced. 
  ///         (ie, the removal of small weights has removed all influences from a fairly un-important joint)
  XMD::XId GetInfluence(XM2::XU32 idx) const;

  /// \brief  this function returns the number of vertices influenced by the skin cluster
  /// \return the number of vertices
  XMD::XU32 GetNumVertices() const;

  /// \brief  returns true if all weights are 1, thus can be skinned rigidly....
  /// \return true if OK
  const bool IsRigid() const;

private:

  /// \brief  An internal function to prune any unused joints from the skin cluster
  void PruneUnusedJoints();

  /// \brief  A check func to see if a given bone ID is actually used
  bool HasWeights(XM2::XU32) const;

  /// \brief  A check func to see if a given bone ID is actually used
  bool RemoveBone(XM2::XU32);

  /// an array of joint influences
  XMD::XIndexList m_Influences;

  /// the skin weight info.
  XM2::vector<XMUSkinWeightList> m_Weights;

};
};
