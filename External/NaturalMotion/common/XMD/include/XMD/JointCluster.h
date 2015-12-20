//----------------------------------------------------------------------------------------------------------------------
/// \file   JointCluster.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a joint cluster
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Deformer.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XJointCluster
/// \brief This class holds the data for a Joint cluster deformer. For a rigidly skinned mesh, there will be 
///        as many of these objects as there are weighted bones. 
/// \note  If you are using XMD in order to obtain game friendly animation data, It is strongly recommended that 
///        you use the XMU/SkinPruner class since it allows conversions between rigid and soft skinning,
///        and has a host of useful optimisations methods.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XJointCluster
  : public XDeformer
{
  XMD_CHUNK(XJointCluster);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kWeights        = XDeformer::kLast+1; // XRealArray
  static const XU32 kIndices        = XDeformer::kLast+2; // XU32Array
  static const XU32 kInfluenceJoint = XDeformer::kLast+3; // XBase*
  static const XU32 kLast           = kInfluenceJoint;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Influence Joint Functions
  /// \brief  Allows you to set or get the Joint that drives the joint
  ///         cluster deformation
  //@{

  /// \brief  returns a pointer to the joint used by the cluster.
  /// \return the joint pointer
  XBone* GetInfluenceJoint() const;

  /// \brief  This function sets the joint used by the cluster
  /// \param  ptr - the pointer to the joint node
  /// \return true if OK
  bool SetInfluenceJoint(const XBase* ptr);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Vertex Weight Functions
  /// \brief  Allows you to set or get the vertex weights associated 
  ///         with the vertices that are to be deformed by the cluster.
  //@{

  /// \brief  returns the weight for the specified geometry point index
  /// \param  idx  -  the point index
  /// \return the weight for that point
  XReal GetWeight(const XU32 idx) const;

  /// \brief  returns a direct reference to the internal weights array.
  ///         Use this for speed (if thats an issue)
  /// \return the weight array
  const XRealArray& Weights() const;

  /// \brief  retrieves the joint cluster weights for the vertices of the 
  ///         deformed geometry. These weights will match the indices
  ///         returned by the GetIndices() and Indices() functions.
  /// \param  outputData  -  the output weight array
  void GetWeights(XRealArray& outputData) const;

  /// \brief  sets the joint cluster weights for the vertices of the 
  ///         deformed geometry. These weights are used to
  ///         assign weightings for the set of vertex indicies that will
  ///         be deformed by the cluster. The number of weights should 
  ///         therefore match the number of indices set in the SetIndices() 
  ///         function.
  /// \param  newData  -  the new weight array
  void SetWeights(const XRealArray& newData);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   JointCluster Member Functions
  /// \brief  Allows you to set or get which vertices are influenced 
  ///         by this joint cluster
  //@{

  /// \brief  If the point index is part of the cluster, then this will 
  ///         simply edit the weight. If not part of the cluster, 
  ///         it will add a new point to the joint cluster
  /// \param  idx  -  the point index.
  /// \param  w  -  the weight
  void AddPoint(XU32 idx, XReal w = 1.0f);

  /// \brief  This removes the specified point ID from the cluster
  /// \param  idx  -  the point index to remove from the cluster
  void RemovePoint(XU32 idx);

  /// \brief  retrieves the joint cluster indices for the vertices of the 
  ///         deformed geometry. These indices refer to the weights 
  ///         returned by the GetWeights() and Weights() functions.
  /// \param  outputData  -  the output indices array
  void GetIndices(XIndexList& outputData) const;

  /// \brief  sets the joint cluster indices for the vertices of the 
  ///         deformed geometry. These indices are the vertices that will
  ///         be deformed on the affected geometry. The indices array 
  ///         should match the number of weights set in the SetWeights()
  ///         function. 
  /// \param  newData  -  the new index array
  void SetIndices(const XIndexList& newData);

  /// \brief  returns a direct reference to the internal index array
  /// \return const reference to the internal index array data
  const XIndexList& Indices() const;

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING
  /// the id number of the bone
  XId m_Bone;
  /// a list of point indices
  XIndexList m_PointIndices;
  /// a list of weights
  XRealArray m_Weights;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::JointCluster, XJointCluster, "jointCluster", "JOINT_CLUSTER");
#endif
/// \brief  An array of joint cluster deformers
typedef XM2::pod_vector<XJointCluster*> XJointClusterList;
}
