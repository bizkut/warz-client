//----------------------------------------------------------------------------------------------------------------------
/// \file   JiggleDeformer.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines a jiggle deformation
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Deformer.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD 
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XJiggleDeformer
/// \brief This class holds the data for a jiggle deformer
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XJiggleDeformer
  : public XDeformer
{
  XMD_CHUNK(XJiggleDeformer);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kMotionMultiplier = XDeformer::kLast+1; // XReal
  static const XU32 kForceAlongNormal = XDeformer::kLast+2; // XReal
  static const XU32 kForceOnTangent   = XDeformer::kLast+3; // XReal
  static const XU32 kStiffness        = XDeformer::kLast+4; // XReal
  static const XU32 kDamping          = XDeformer::kLast+5; // XReal
  static const XU32 kJiggleWeight     = XDeformer::kLast+6; // XReal
  static const XU32 kDirectionBias    = XDeformer::kLast+7; // XReal
  static const XU32 kPointIndices     = XDeformer::kLast+8; // XU32Array
  static const XU32 kIgnoreTransform  = XDeformer::kLast+9; // bool
  static const XU32 kEnable           = XDeformer::kLast+10;// bool
  static const XU32 kLast             = kEnable;
  //--------------------------------------------------------------------------------------------------------------------

  /// \name   Jiggle Deformer Functions
  //@{

  /// \brief  returns the indices of the points on the affected surface
  ///         that are jiggled.
  /// \return reference to the internal index array
  const XIndexList& GetPointIndices() const;

  /// \brief  returns the force along the normal
  /// \return the force along the normal
  XReal GetForceAlongNormal() const;

  /// \brief  returns the force along the tangent
  /// \return the force along the tangent
  XReal GetForceOnTangent() const;

  /// \brief  returns the motion multiplier
  /// \return the motion multiplier
  XReal GetMotionMultiplier() const;

  /// \brief  returns the stiffness of the deformation
  /// \return the stiffness of the deformation
  XReal GetStiffness() const;

  /// \brief  returns the damping applied to the motion
  /// \return the damping applied to the motion
  XReal GetDamping() const;

  /// \brief  returns the jiggle deformer weight
  /// \return the jiggle deformer weight
  XReal GetJiggleWeight() const;

  /// \brief  returns the direction bias
  /// \return the direction bias
  XReal GetDirectionBias() const;

  /// \brief  queries if the deformer is enabled
  /// \return true if enabled, false otherwise
  bool GetEnable() const;

  /// \brief  a flag to indicate that the transform should be ignored
  ///         when computing the deformation
  /// \return true if transform should be ignored
  bool GetIgnoreTransform() const;

  /// \brief  sets the indices of the points (on the affected geometry)
  ///         that will be jiggled.
  /// \param  indices - the point index array
  void SetPointIndices(const XIndexList& indices);

  /// \brief  sets the force along the normal value
  /// \param  v - the new force along the normal value
  void SetForceAlongNormal(XReal v);

  /// \brief  sets the force along the tangent value
  /// \param  v - the new force along the tangent value
  void SetForceOnTangent(XReal v);

  /// \brief  sets the motion multiplier
  /// \param  v - the new motion multiplier amount
  void SetMotionMultiplier(XReal v);

  /// \brief  sets the stiffness value
  /// \param  v - the new stiffness value
  void SetStiffness(XReal v);

  /// \brief  sets the damping amount
  /// \param  v - the new damping amount
  void SetDamping(XReal v);

  /// \brief  sets the jiggle weight
  /// \param  v - the new jiggle weight
  void SetJiggleWeight(XReal v);

  /// \brief  sets the direction bias of the deformer
  /// \param  v - the new direction bias of the deformer
  void SetDirectionBias(XReal v);

  /// \brief  sets the ignore transform flag
  /// \param  v - true if we ignore the transform
  void SetIgnoreTransform(bool v);

  /// \brief  sets the enable
  /// \param  v - the value of the enable flag
  void SetEnable(bool v);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING

  XS32 enable;
  bool ignore_transform;
  XReal force_along_normal;
  XReal force_on_tangent;
  XReal motion_multiplier;
  XReal stiffness;
  XReal damping;
  XReal jiggle_weight;
  XReal direction_bias;
  XIndexList point_indices;

#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_CHUNK_TYPE(XFn::JiggleDeformer, XJiggleDeformer, "jiggleDeformer", "JIGGLE_DEFORMER");
#endif
}
