//----------------------------------------------------------------------------------------------------------------------
/// \file   SkinCluster.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  This file defines the XAnimCycle class. basicaly this class holds a 
///         set of animation cycles for each bone in the hierarchy. 
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Deformer.h"
#include "XMD/Bone.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
// forward declarations
class XMD_EXPORT XBone;
class XMD_EXPORT XGeometry;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XSkinWeight
/// \brief the skin weight holds a weight an a joint ID for the influence
///        joint on a specific vertex
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XSkinWeight
{
  /// \brief  ctor
  XSkinWeight();

  /// \brief  copy ctor
  /// \param  lw - the weight to copy
  XSkinWeight(const XSkinWeight& lw);

  /// \brief  ctor
  /// \param  f - the skin weight
  /// \param  idx - the joint index
  XSkinWeight(XReal f, XU32 idx);

  /// the vertex weight
  XReal w;

  /// the vertex IDX
  XU32 m_JointID;

  /// \brief  stream insertion operator
  /// \param  ofs - the output file stream to write to
  /// \param  sw - the skin weight to write
  /// \return the output stream
  friend std::ostream& operator<<(std::ostream& ofs, const XSkinWeight& sw);
};

/// \brief  an array of skin weights assigned to a specific vertex.
///         The XSkinCluster node will store an array of these, one for
///         each vertex.
typedef XM2::pod_vector<XSkinWeight> XSkinnedVertex;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XClusterWeight
/// \brief the skin weight holds a weight an a joint ID for the influence
///        joint on a specific vertex
//----------------------------------------------------------------------------------------------------------------------
struct XMD_EXPORT XClusterWeight
{
  /// \brief  ctor
  XClusterWeight();

  /// \brief  copy ctor
  XClusterWeight(const XClusterWeight& lw);

  /// \brief  ctor
  /// \param  f - the skin weight
  /// \param  idx - the joint index
  XClusterWeight(XReal f, XU32 idx);

  /// the vertex weight
  XReal w;

  /// the vertex IDX
  XU32 mVertexIndex;
};
/// \brief  an array of skin weights assigned to a specific vertex.
///         The XSkinCluster node will store an array of these, one for
///         each vertex.
typedef XM2::pod_vector<XClusterWeight> XClusterWeightList;

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XSkinCluster
/// \brief The skin cluster holds skinning information that applies a 
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XSkinCluster 
  : public XDeformer
{
  XMD_CHUNK(XSkinCluster);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kInfluences      = XDeformer::kLast+1; // XList
  static const XU32 kSkinnedGeometry = XDeformer::kLast+2; // XBase*
  static const XU32 kVertexIndices   = XDeformer::kLast+3; // XU32Array
  static const XU32 kWeights         = XDeformer::kLast+4; // XRealArray
  static const XU32 kJointIndices    = XDeformer::kLast+5; // XU32Array
  static const XU32 kLast            = kJointIndices;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Influence Joints
  /// \brief  Sets/Gets the influence joints used for the Skin deformation
  //@{

  /// \brief  This function returns the number of joint influences
  /// \return the number of influence joints
  XU32 GetNumInfluences() const;

  /// \brief  This function returns a pointer to the requested influence
  /// \param  idx - the index into the stored influences.
  /// \return returns the influence bone ID of the specified bone index.
  XBone* GetInfluence(XU32 idx) const;

  /// \brief  adds an influence bone to this skin cluster
  /// \param  bone  - the bone to add as an influence
  /// \return the index that refers to the influence joint. This index should be used as
  ///         the joint_idx parameter of the AddWeight function. 
  XU32 AddInfluence(const XBone* bone);

  /// \brief  sets the array of influences for the skin cluster deformer.
  ///         If the skin cluster has a set of skin weight already, then
  ///         it will attempt to re-assign the skinned joint'ids in each
  ///         skin weight. If the previous influence joint does not appear
  ///         in the new influence array, it will simply assign the joint
  ///         index 0. 
  /// \param  bones  - the array of influence objects
  void SetInfluences(const XBoneList& bones);

  /// \brief  this function retrieves all of the influence objects that
  ///         drive this deformer. 
  /// \param  bones  - the returned list of influences.
  void GetInfluences(XBoneList& bones) const;

  /// \brief  removes the specified influence bone from the skin cluster.
  /// \param  bone - the bone to remove.
  /// \return true if OK
  bool RemoveInfluence(const XBone* bone);

  /// \brief  removes the specified influence bone from the skin cluster.
  /// \param  bone - the bone to remove.
  /// \return true if OK
  bool RemoveInfluence(const XU32 bone_index);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Vertex & Vertex Weight Data
  /// \brief  Sets/Gets the vertex indices and their weights
  //@{

  /// \brief  A quick check func to ensure that all weights are actually 
  ///         valid (i.e. they all sum to 1.0f).
  /// \param  invalid_vtx_weight  - the indices of the vertices that are 
  ///         not valid
  /// \return true if valid, false otherwise
  bool AreWeightsValid(XIndexList& invalid_vtx_weight) const;

  /// \brief  Forces the weights on every vert to add up to 1.
  /// \param  invalid_vtx_weight - the indices of the vertices that are 
  ///         not valid, this is obtained from AreWeightsValid();
  void NormaliseWeights(const XIndexList& invalid_vtx_weight);

  /// \brief Adds a new skin weight into the cluster.
  /// \param  control_point_index - the index of the vertx/control point
  /// \param  joint_idx - the index (in the internal influence array) of the 
  ///         influence joint. Note, this should be the ID returned by AddInfluence(),
  ///         not the bone ID!
  /// \param  w - the weight for that influence.
  /// \return true if OK
  bool AddWeight(XU32 control_point_index, XU32 joint_idx, XReal w);

  /// \brief  This function returns the skinned vertex data
  /// \return the skin weights
  const XM2::vector<XSkinnedVertex>& VertexData() const;

  /// \brief  a utility function to construct and return an array of all 
  ///         skin weights that are used by the specified joint index. 
  /// \param  joint_idx - the joint index
  /// \param  returned - the returned array of vertices and weights that 
  ///         are influenced by the given joint index. 
  bool GetClusterWeights(XU32 joint_idx, XClusterWeightList& returned) const;


  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Skinned Geometry
  /// \brief  Sets/Gets the geometry that is influenced by the skin 
  ///         deformation
  //@{

  /// \brief  This function returns a pointer to the affected geometry
  /// \return the affected geometry object
  /// 
  XGeometry* GetAffected() const;

  /// \brief  sets the geometry to be deformed by this skin cluster
  /// \param  geom - the geometry to deform
  /// \return true if OK
  bool SetAffected(const XGeometry* geom);

  /// \brief  get the skin weights for each vertex
  /// \param  skin_weights - the list of skin weights
  void GetSkinWeights(XM2::vector<XSkinnedVertex>& skin_weights) const;

  /// \brief  set the skin weights for each vertex
  /// \param  skin_weights - the new list of skin weights
  void SetSkinWeights(const XM2::vector<XSkinnedVertex>& skin_weights);

  //@}

private:
#ifndef DOXYGEN_PROCESSING

  /// all of the influences for the skin cluster
  XIndexList m_Influences;

  /// the skin weights for each vertex
  XM2::vector<XSkinnedVertex> m_SkinWeights;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::SkinCluster, XSkinCluster, "skinCluster", "SKIN");
#endif
}
