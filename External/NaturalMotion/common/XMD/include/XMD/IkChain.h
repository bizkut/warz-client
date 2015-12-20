//----------------------------------------------------------------------------------------------------------------------
/// \file   IkChain.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the input/output method for IkChain
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Base.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XIkChain
/// \brief Holds information about display layers within the scene.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XIkChain
  : public XBase
{
  XMD_CHUNK(XIkChain);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kChain          = XBase::kLast+1; // XList
  static const XU32 kHandle         = XBase::kLast+2; // XBase*
  static const XU32 kPriority       = XBase::kLast+3; // XS32
  static const XU32 kMaxIterations  = XBase::kLast+4; // XU32
  static const XU32 kWeight         = XBase::kLast+5; // XReal
  static const XU32 kPoWeight       = XBase::kLast+6; // XReal
  static const XU32 kTolerance      = XBase::kLast+7; // XReal
  static const XU32 kStickiness     = XBase::kLast+8; // XReal
  static const XU32 kPoleVector     = XBase::kLast+9; // XVector3
  static const XU32 kPoleConstraint = XBase::kLast+10;// XBase*
  static const XU32 kLast           = kPoleConstraint;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   IK Chain Params
  //@{

  /// \brief  returns the number of bones making up the IK chain
  /// \return the number of bones
  XU32 GetNumBonesInChain() const;

  /// \brief  returns a pointer to the requested bone within the IK chain
  /// \param  idx  -  the index into the IK chain
  /// \return a pointer to the bone
  XBone* GetBoneInChain(XU32 idx) const;

  /// \brief  Adds the specified bone into the IK chain
  /// \param  bone  -  the new bone to add into the chain
  /// \return true if OK
  bool AddBoneToChain(const XBone* bone);

  /// \brief  sets the bones to use for the IK chain.
  /// \param  bones - the bones starting at the IK root and moving all
  ///         the way down to the end effector of the IK chain. 
  bool SetBonesInChain(const XBoneList& bones);

  /// \brief  sets the bones to use for the IK chain.
  /// \param  ik_root - the root bone of the IK chain
  /// \param  ik_effector - the ik effector at the end of the IK chain
  bool SetBonesInChain(const XBone* ik_root, const XBone* ik_effector);

  /// \brief  returns all the bones in use within the IK chain.
  /// \param  bones - the returned bones starting at the IK root and 
  ///         moving all the way down to the end effector.
  void GetBonesInChain(XBoneList& bones) const;

  /// \brief  removes the specified bone from the IK chain.
  /// \param  bone - the bone to remove
  /// \return true if OK
  bool RemoveBone(const XBone* bone);

  /// \brief  allows you to retrieve the transform used as the IK handle
  /// \return the IK handle
  XBone* GetHandle() const;

  /// \brief  returns the pole vector value for the IK chain
  /// \return the pole vector direction
  const XVector3& GetPoleVector() const;

  /// \brief  returns the bone of a pole vector constraint.
  /// \return  the pole vector constraint (or NULL if not used)
  XBone* GetPoleConstraint() const;

  /// \brief  returns the priority for this IK chain. this is only used
  ///         when the IK chain cannot be properly evaluated.
  /// \return the priority value
  XS32 GetPriority() const;

  /// \brief  returns the max number of iterations before giving up on
  ///         a solution.
  /// \return The max num of iterations
  XU32 GetMaxIterations() const;

  /// \brief  returns the weighting used in the IK calculation
  /// \return The weight
  XReal GetWeight() const;

  /// \brief  returns the PO weighting used in the IK calculation
  /// \return The PO weight
  XReal GetPoWeight() const;

  /// \brief  returns the tolerance used in the IK calculation
  /// \return The tolerance
  XReal GetTolerance() const;

  /// \brief  returns the stickiness used in the IK calculation
  /// \return The stickiness
  XReal GetStickiness() const;

  /// \brief  allows you to set the transform used as the IK handle
  /// \param  handle - the IK handle
  bool SetHandle(const XBase* handle);

  /// \brief  returns the pole vector value for the IK chain
  /// \param  pole_vector - the pole vector direction
  void SetPoleVector(const XVector3& pole_vector);

  /// \brief  returns the bone of a pole vector constraint.
  /// \param  transform - the pole vector constraint
  bool SetPoleConstraint(const XBase* transform);

  /// \brief  sets the priority for this IK chain. this is only used
  ///         when the IK chain cannot be properly evaluated.
  /// \param  priority - the new priority value
  void SetPriority(XS32 priority);

  /// \brief  sets the max number of iterations before giving up on
  ///         a solution.
  /// \param  num_iterations - The max num of iterations
  void SetMaxIterations(XU32 num_iterations);

  /// \brief  returns the weighting used in the IK calculation
  /// \param  weight - the weight
  void SetWeight(XReal weight);

  /// \brief  returns the PO weighting used in the IK calculation
  /// \param  po_weight - The PO weight
  void SetPoWeight(XReal po_weight);

  /// \brief  returns the tolerance used in the IK calculation
  /// \param  tolerance - the tolerance for the IK chain
  void SetTolerance(XReal tolerance);

  /// \brief  returns the stickiness used in the IK calculation
  /// \param  stickiness - The new stickiness value for the IK chain
  void SetStickiness(XReal stickiness);

  //@}

  // internal node data
private:
#ifndef DOXYGEN_PROCESSING
  XId m_Handle;
  XS32 m_Priority;
  XU32 m_MaxIterations;
  XReal m_Weight;
  XReal m_PoWeight;
  XReal m_Tolerance;
  XReal m_Stickiness;
  XVector3 m_PoleVector;
  XId m_PoleConstraint;
  XIndexList m_IkChain;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::IkChain, XIkChain, "ikChain", "IK_CHAIN");
#endif
}
