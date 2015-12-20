//----------------------------------------------------------------------------------------------------------------------
/// \file   Bone.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the class to handle a Bone transform within the lib
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XM2/Vector3.h"
#include "XM2/Vector4.h"
#include "XM2/Quaternion.h"
#include "XM2/Transform.h"
#include "XM2/Matrix34.h"
#include "XMD/Base.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
#ifndef DOXYGEN_PROCESSING
class XMD_EXPORT XInstance;
typedef XM2::pod_vector<XInstance*> XInstanceList;
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XBone
/// \brief XBone is the main interface for a transform within the XMD scene data.
//---------------------------------------------------------------------------------------------------------------------- 
class XMD_EXPORT XBone
  : public XBase
{
  friend class XMD_EXPORT XJoint;
  XMD_CHUNK(XBone);
public:

  /// \brief  defines the euler angle rotation order
  enum RotationOrder
  {
    kXYZ,
    kXZY,
    kYXZ,
    kYZX,
    kZXY,
    kZYX
  };

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kRotation              = XBase::kLast+1; // XQuaternion
  static const XU32 kScale                 = XBase::kLast+2; // XVector3
  static const XU32 kTranslation           = XBase::kLast+3; // XVector3
  static const XU32 kVisibility            = XBase::kLast+4; // XBool
  static const XU32 kJointOrient           = XBase::kLast+5; // XQuaternion
  static const XU32 kRotationOrient        = XBase::kLast+6; // XQuaternion
  static const XU32 kRotatePivot           = XBase::kLast+7; // XVector3
  static const XU32 kScalePivot            = XBase::kLast+8; // XVector3
  static const XU32 kBoundingBoxMin        = XBase::kLast+9; // XVector3
  static const XU32 kBoundingBoxMax        = XBase::kLast+10;// XVector3
  static const XU32 kLocal                 = XBase::kLast+11;// XMatrix
  static const XU32 kWorld                 = XBase::kLast+12;// XMatrix
  static const XU32 kInverse               = XBase::kLast+13;// XMatrix
  static const XU32 kInverseTranspose      = XBase::kLast+14;// XMatrix
  static const XU32 kChildren              = XBase::kLast+15;// XList
  static const XU32 kParent                = XBase::kLast+16;// XBase*
  static const XU32 kShearing              = XBase::kLast+17;// XVector3
  static const XU32 kInheritsTransform     = XBase::kLast+18;// bool
  static const XU32 kRotationOrder         = XBase::kLast+19;// XU32
  static const XU32 kHasMinTranslateXLimit = XBase::kLast+20;// bool
  static const XU32 kHasMinTranslateYLimit = XBase::kLast+21;// bool
  static const XU32 kHasMinTranslateZLimit = XBase::kLast+22;// bool
  static const XU32 kHasMinScaleXLimit     = XBase::kLast+23;// bool
  static const XU32 kHasMinScaleYLimit     = XBase::kLast+24;// bool
  static const XU32 kHasMinScaleZLimit     = XBase::kLast+25;// bool
  static const XU32 kHasMinRotateXLimit    = XBase::kLast+26;// bool
  static const XU32 kHasMinRotateYLimit    = XBase::kLast+27;// bool
  static const XU32 kHasMinRotateZLimit    = XBase::kLast+28;// bool
  static const XU32 kHasMaxTranslateXLimit = XBase::kLast+29;// bool
  static const XU32 kHasMaxTranslateYLimit = XBase::kLast+30;// bool
  static const XU32 kHasMaxTranslateZLimit = XBase::kLast+31;// bool
  static const XU32 kHasMaxScaleXLimit     = XBase::kLast+32;// bool
  static const XU32 kHasMaxScaleYLimit     = XBase::kLast+33;// bool
  static const XU32 kHasMaxScaleZLimit     = XBase::kLast+34;// bool
  static const XU32 kHasMaxRotateXLimit    = XBase::kLast+35;// bool
  static const XU32 kHasMaxRotateYLimit    = XBase::kLast+36;// bool
  static const XU32 kHasMaxRotateZLimit    = XBase::kLast+37;// bool
  static const XU32 kMinTranslateXLimit    = XBase::kLast+38;// XReal
  static const XU32 kMinTranslateYLimit    = XBase::kLast+39;// XReal
  static const XU32 kMinTranslateZLimit    = XBase::kLast+40;// XReal
  static const XU32 kMinScaleXLimit        = XBase::kLast+41;// XReal
  static const XU32 kMinScaleYLimit        = XBase::kLast+42;// XReal
  static const XU32 kMinScaleZLimit        = XBase::kLast+43;// XReal
  static const XU32 kMinRotateXLimit       = XBase::kLast+44;// XReal
  static const XU32 kMinRotateYLimit       = XBase::kLast+45;// XReal
  static const XU32 kMinRotateZLimit       = XBase::kLast+46;// XReal
  static const XU32 kMaxTranslateXLimit    = XBase::kLast+47;// XReal
  static const XU32 kMaxTranslateYLimit    = XBase::kLast+48;// XReal
  static const XU32 kMaxTranslateZLimit    = XBase::kLast+49;// XReal
  static const XU32 kMaxScaleXLimit        = XBase::kLast+50;// XReal
  static const XU32 kMaxScaleYLimit        = XBase::kLast+51;// XReal
  static const XU32 kMaxScaleZLimit        = XBase::kLast+52;// XReal
  static const XU32 kMaxRotateXLimit       = XBase::kLast+53;// XReal
  static const XU32 kMaxRotateYLimit       = XBase::kLast+54;// XReal
  static const XU32 kMaxRotateZLimit       = XBase::kLast+55;// XReal
  static const XU32 kLast                  = kMaxRotateZLimit;
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  An array of bones
  typedef XM2::pod_vector<XBone*> XBoneList;

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Visibility Flags
  /// \brief  Sets/Gets whether the transform and it's children are visible or not.
  //@{

  /// \brief  queries the visibility flag on this transform node.
  /// \return true if the transform (and children) are visible.
  bool IsVisible() const;

  /// \brief  sets the visibility flag on this transform
  /// \param  v - true for visible, false for hidden
  void SetVisible(bool v);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Parenting information
  /// \brief  Sets/Gets information about child transforms and geometry
  ///         instances that use this transform
  //@{

  /// \brief  returns a list of all object instances parented under this bone.
  /// \param  _list - the returned list of instances
  /// \return true if any found
  bool GetObjectInstances(XInstanceList& _list) const;

  /// \brief  returns a list of all object instances parented under this bone.
  /// \param  _list - the returned list of instances
  /// \return true if any found
  bool GetChildren(XBoneList& _list) const;

  /// \brief  returns a list of all object instances parented under this bone.
  /// \param  _list - the returned list of instances
  /// \return true if any found
  bool GetParents(XBoneList& _list) const;

  /// \brief  returns the parent of this joint
  /// \return returns the parent node
  XBone* GetParent() const;

  /// \brief  Deletes the specified instance from the Bone
  /// \param  instance  - the instance ID to delete
  /// \return true if OK
  bool DeleteInstance(const XId instance);

  /// \brief  Adds the specified bone as a child of this bone
  /// \param  kid - the bone to make as a child to this one
  bool AddChild(XBone* kid);

  /// \brief  removes the specified transform from being a child of this bone. 
  /// \param  kid - the child node to remove from this transform;
  /// \return true if successful
  bool RemoveChild(XBone* kid);

  /// \brief  Adds a shape as an instance under this bone (ie, this bone)
  /// \param  kid - the instance to add to this bone
  void AddInstance(const XInstance* kid);

  /// \brief  Adds the specified bone as a parent of this bone
  /// \param  ptr - the bone to make as a parent to this one
  void AddParent(const XBone* ptr);

  /// \brief  sets all children of this bone
  /// \param  children - the new children of this bone
  /// \return true if OK
  bool SetChildren(XBoneList& children);

  /// \brief  recurses down the currently set children, and returns true
  ///         if the specified node is a child of this node (or one of 
  ///         its children). 
  /// \param  kid - the child to test
  /// \return true if kid is a child (or child of a child) of this bone.
  /// \note   this is primarily used internally to ensure that at no point
  ///         do you get an infinite recursion of the child hierarchy.
  ///         parenting of nodes should always be acyclic!
  bool IsChildNode(const XBone* kid) const;

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Transform information
  /// \brief  Sets/Gets information about the rotation/scale etc
  //@{

  /// \brief  sets whether the parent transform is inherited or not
  /// \param  val - true if the transform is inherited
  void SetInheritsTransform(bool val);

  /// \brief  gets whether the parent transform is inherited or not
  /// \return true if the transform is inherited
  bool GetInheritsTransform() const;

  /// \brief  returns the world space matrix for the joint.
  void SetWorld(const XMatrix& mat);

  /// \brief  returns the world space matrix for the joint.
  const XMatrix& GetWorld(bool bind_pose = false) const;

  /// \brief  returns the local space matrix for the joint
  const XMatrix& GetLocal(bool bind_pose = false) const;

  /// \brief  returns the world inverse space matrix for the joint
  const XMatrix& GetInverse(bool bind_pose = false) const;

  /// \brief  returns the world space matrix for the joint. This inverse
  ///         transpose matrix is used to transform normals.
  const XMatrix& GetInverseTranspose(bool bind_pose = false) const;

  /// \brief  Returns the joint orient of the node
  /// \return the joint orient
  const XQuaternion& GetJointOrient() const;

  /// \brief  Returns the rotation orient of the node
  /// \return the rotation orient
  const XQuaternion& GetRotationOrient() const;

  /// \brief  Returns the rotation of the node (as a quaternion)
  /// \param  bind_pose - if true, returns bind pose rotation, otherwise current
  /// \return the rotation as a quaternion
  const XQuaternion& GetRotation(bool bind_pose = false) const;

  /// \brief  Returns the translation of the node (as a vector)
  /// \param  bind_pose - if true, returns bind pose translation, otherwise current
  /// \return the translation as a vector
  const XVector3& GetTranslation(bool bind_pose = false) const;

  /// \brief  Returns the scale of the node (as a vector)
  /// \param  bind_pose - if true, returns bind pose scale, otherwise current
  /// \return the scale as a vector
  const XVector3& GetScale(bool bind_pose = false) const;

  /// \brief  returns the shearing amount for the transform matrices.
  /// \return the transforms shearing values
  const XVector3& GetShearing() const;

  /// \brief  Returns the rotation pivot for this bone
  /// \return the pivot offset as a vector
  const XVector3& GetRotatePivot() const;

  /// \brief  Returns the scale pivot for this bone
  /// \return the pivot offset as a vector
  const XVector3& GetScalePivot() const;

  /// \brief  Returns the rotation pivot translate value for this bone
  /// \return the translate value offset as a vector
  const XVector3& GetRotatePivotTranslate() const;

  /// \brief  Returns the scale pivot translate value for this bone
  /// \return the pivot translate value as a vector
  const XVector3& GetScalePivotTranslate() const;

  /// \brief  Returns the minimum boundign box extents of the node (as a vector)
  /// \param  bind_pose - if true, returns bind pose bounds, otherwise current
  /// \return the min bounding extent as a vector
  const XVector3& GetBoundingBoxMin(bool bind_pose = false) const;

  /// \brief  Returns the maximum boundign box extents of the node (as a vector)
  /// \param  bind_pose - if true, returns bind pose bounds, otherwise current
  /// \return the max bounding extent as a vector
  const XVector3& GetBoundingBoxMax(bool bind_pose = false) const;

  /// \brief  Sets the joint orientation for this bone
  /// \param  r - the joint orient
  void  SetJointOrient(const XQuaternion& r);

  /// \brief  Sets the rotation orientation for this bone
  /// \param  r - the rotation orient
  void SetRotationOrient(const XQuaternion& r);

  /// \brief  Sets the joint rotation for this bone
  /// \param  r - the rotation
  void SetRotation(const XQuaternion& r);

  /// \brief  Sets the joint translation for this bone
  /// \param  v - the translation as a vector
  void SetTranslation(const XVector3& v);

  /// \brief  Sets the joint scale for this bone
  /// \param  v - the scale as a vector
  void SetScale(const XVector3& v);

  /// \brief  Sets the rotation pivot for this bone
  /// \param  v - the pivot offset as a vector
  void SetRotatePivot(const XVector3& v);

  /// \brief  Sets the scale pivot for this bone
  /// \param  v - the pivot offset as a vector
  void SetScalePivot(const XVector3& v);

  /// \brief  Sets the rotation pivot translate value for this bone
  /// \param  v - the pivot translate value  as a vector
  void SetRotatePivotTranslate(const XVector3& v);

  /// \brief  Sets the scale pivot translate value for this bone
  /// \param  v - the pivot translate value as a vector
  void SetScalePivotTranslate(const XVector3& v);

  /// \brief  Sets the nodes shearing amount.
  /// \param  shear - the new shearing values
  void SetShearing(const XVector3& shear);

  /// \brief  Sets the nodes shearing amount.
  /// \param  x - the new shearing x value
  /// \param  y - the new shearing y value
  /// \param  z - the new shearing z value
  void SetShearing(XReal x, XReal y, XReal z);

  /// \brief  Allows you to remove the rotation and joint orients contained
  ///         on this bone.
  void RemoveOrients();

  /// \brief  updates the current bind pose from the current transform
  void UpdateBindPose();

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Transform Limits
  /// \brief  Sets/Gets information about the rotation, scale and translation
  ///         limits.
  //@{

  /// \brief  the type of limit
  enum Limit 
  {
    kScaleMaxX,
    kScaleMinX,
    kScaleMaxY,
    kScaleMinY,
    kScaleMaxZ,
    kScaleMinZ,

    kRotateMaxX,
    kRotateMinX,
    kRotateMaxY,
    kRotateMinY,
    kRotateMaxZ,
    kRotateMinZ,

    kTranslateMaxX,
    kTranslateMinX,
    kTranslateMaxY,
    kTranslateMinY,
    kTranslateMaxZ,
    kTranslateMinZ,
  };

  /// \brief  returns true if the specified limit has a max or min value. 
  /// \param  limit_type - the limit to query
  /// \return true if limited.
  bool HasLimit(Limit limit_type) const;

  /// \brief  returns the value of the specified limit. 
  /// \param  limit_type - the limit to query
  /// \return the value of the limit. This may be 0x7fffffff if there is 
  ///         no max limit,  or 0xffffffff if there is no min limit. 
  XReal GetLimitValue(Limit limit_type) const;

  /// \brief  sets the limit value
  /// \param  limit_type - the limit to set
  /// \param  limit - the limit value.
  void SetLimit(Limit limit_type, XReal limit);

  /// \brief  disables the specified limit
  /// \param  limit_type - the limit to disable
  void DisableLimit(Limit limit_type);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Euler Rotation Order
  /// \brief  The XBone and derived classes use Quaternions throughout,
  ///         so this data is used only when combining a set of Euler
  ///         animation curves to create a quaternion rotation. 
  //@{

  /// \brief  returns the rotation order for this transform
  /// \return the rotation order
  RotationOrder GetRotationOrder() const;

  /// \brief  sets the rotation order for this transform
  /// \param  ro - the rotation order
  void SetRotationOrder(RotationOrder ro);

  //@}

protected:
#ifndef DOXYGEN_PROCESSING
  virtual void Evaluate(const XMatrix& ParentX, bool recurse = true);
  void KillOrients();
  void KillScale();
  void FreezeTransforms(bool local_is_LH = false);
  void SetIdentity();
  bool UnParent();
  bool Parent(XBone* new_parent);
  XMatrix RecursiveBuild(XBone* parent);
  XMatrix RecursiveBindBuild(XBone* parent);
  void BuildMatrices();
  void BuildBindMatrices();
  void SetToBindPose();

protected:
  /// parent ID
  XId m_Parent;
  /// the id number of the child bones
  XIndexList m_ChildBones;
  /// the id numbers of geometry objects instanced under this bone
  XIndexList m_Instances;
  /// the minimum bounding box extents
  XVector3 m_BoundingBoxMin;
  /// the maximum bounding box extents
  XVector3 m_BoundingBoxMax;
  /// the offset from transform pivot to scale pivot
  XVector3 m_ScalePivot;
  /// the offset from transform pivot to rotate pivot
  XVector3 m_RotatePivot;
  /// scale pivot compensation
  XVector3 m_ScalePivotTranslate;
  /// rotate pivot compensation
  XVector3 m_RotatePivotTranslate;
  /// the shearing amount
  XVector3 m_Shearing;
  /// the translation limits. 0x7fffffff indicates +INF, 0xffffffff indicates 
  /// -INF limits. 
  XReal m_TranslateLimits[6];
  /// the rotation limits. 0x7fffffff indicates +INF, 0xffffffff indicates 
  /// -INF limits. 
  XReal m_RotateLimits[6];
  /// the scale limits. 0x7fffffff indicates +INF, 0xffffffff indicates 
  /// -INF limits. 
  XReal m_ScaleLimits[6];
  /// used to determine the euler rotation order when applying euler anim
  /// curves to the transforms rotation
  RotationOrder m_RotationOrder;
  /// the joint orientation. This is used when the animators create the skeleton.
  /// it basically changes the orientation of the joints local space coord system
  /// so that the z axis goes down the bone.
  XQuaternion m_JointOrient;
  /// a post rotation effect
  XQuaternion m_RotationOrient;
  /// the current local space transform
  XTransform m_Transform;
  /// the local space bind pose transform
  XTransform m_BindTransform;
  /// the current world space matrix
  XMatrix m_World;
  /// the current local space matrix
  XMatrix m_Local;
  /// the current world inverse matrix
  XMatrix m_Inverse;
  /// the current world inverse transpose matrix
  XMatrix m_InverseTranspose;
  /// the current world space matrix
  XMatrix m_BindWorld;
  /// the current local space matrix
  XMatrix m_BindLocal;
  /// the current world inverse matrix
  XMatrix m_BindInverse;
  /// the current world inverse transpose matrix
  XMatrix m_BindInverseTranspose;
  /// is the transform (and it's children) visible?
  bool m_Visible;
  /// true if the parent transform affects this XBone. If false, the transform
  /// basically ignores it's parent xform value. 
  bool m_InheritsTransform;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Bone, XBone, "bone", "BONE");
#endif

/// an array of bones
typedef XBone::XBoneList XBoneList;


//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XJoint
/// \brief ultimately no difference between a bone and a joint, just
///        used to differentiate between transform and ikJoint types
///        in maya.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XJoint :
  public XBone
{
  XMD_CHUNK(XJoint);
public:
  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kSegmentScaleCompensate = XBone::kLast+1; // bool
  static const XU32 kLast                   = kSegmentScaleCompensate;
  //--------------------------------------------------------------------------------------------------------------------

  /// \brief  returns the segment scale compensate flag
  /// \return true if the scale of this joints parent is compensated for
  bool GetSegmentScaleCompensate() const;

  /// \brief  sets the segment scale compensate flag on the node
  /// \param  val - the segment scale compensate value
  void SetSegmentScaleCompensate(bool val);

private:
#ifndef DOXYGEN_PROCESSING
  virtual void Evaluate(const XMatrix& ParentX, bool recurse = true);
  bool m_SegmentScaleCompensate;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::Joint, XJoint, "joint", "JOINT");
#endif
}
