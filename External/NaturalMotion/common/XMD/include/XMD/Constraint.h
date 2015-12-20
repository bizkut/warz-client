//----------------------------------------------------------------------------------------------------------------------
/// \file   Constraint.h
/// \author Rob Bateman [http://robthebloke.org]
/// \brief  Defines the base class for all contraints
/// \note   (C) Copyright 2003-2005 Robert Bateman. All rights reserved.
//----------------------------------------------------------------------------------------------------------------------
#pragma once
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Bone.h"
//----------------------------------------------------------------------------------------------------------------------
namespace XMD
{
//----------------------------------------------------------------------------------------------------------------------
/// \class XMD::XConstraint
/// \brief This class holds the data for a transform constraint on a
///        joint hierarchy. Typically constraints are used to drive bones
///        for look-at, or other transform constraint types.
//----------------------------------------------------------------------------------------------------------------------
class XMD_EXPORT XConstraint
  : public XBone
{
  XMD_ABSTRACT_CHUNK(XConstraint);
public:

  //--------------------------------------------------------------------------------------------------------------------
  // property ID's
  static const XU32 kWeights     = XBone::kLast+1; // XRealArray
  static const XU32 kTargets     = XBone::kLast+2; // XList
  static const XU32 kConstrained = XBone::kLast+3; // XList
  static const XU32 kLast        = kConstrained;
  //--------------------------------------------------------------------------------------------------------------------

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Constraint Manipulation functions
  /// \brief  Functions to set which transform node is constrained by 
  ///         this XConstraint object
  //@{

  /// \brief  returns the constrained object
  /// \return the constrained object
  XBase* GetConstrained() const;

  /// \brief  base the object to be constrained
  /// \param  base  - the constrained object
  bool SetConstrained(const XBase* base);

  //@}

  //--------------------------------------------------------------------------------------------------------------------
  /// \name   Target Manipulation functions
  /// \brief  Functions to set any target objects and associated weights
  ///         for the constraint to use.
  //@{

  /// \brief  returns direct reference to the constraining objects
  /// \return the internal target array
  /// \note   slightly cheaper for read only access
  const XIndexList& Targets() const;

  /// \brief  returns direct reference to the weights for each target
  /// \return the internal weights array
  /// \note   slightly cheaper for read only access
  const XRealArray& Weights() const;

  /// \brief  returns the number of target objects
  /// \return the num of target objects
  XU32 GetNumTargets() const;

  /// \brief  returns the requested target
  /// \param  target_index  - the target to retrieve
  /// \return the targets transform
  XBase* GetTarget(XU32 target_index) const;

  /// \brief  returns the requested target
  /// \param  target_index  - the target to retrieve
  /// \return the targets weight
  XReal GetTargetWeight(XU32 target_index) const;

  /// \brief  adds a new target into the constraint
  /// \param  bone  - the transform node to use as the target
  /// \param  weight  - the initial weight for the constraint
  /// \return true if OK
  bool AddTarget(const XBone* bone, XReal weight);

  /// \brief  removes a target from the constraint
  /// \param  target_index  - the transform node to remove from the constraint
  /// \return true if OK
  bool RemoveTarget(XU32 target_index);

  /// \brief  returns the requested target
  /// \param  target_index  - the target to retrieve
  /// \param  weight        - the new weight for the target
  /// \return true if OK
  bool SetTargetWeight(XU32 target_index, XReal weight);

  /// \brief  returns a list of the targets used by this constraint.
  /// \param  targets - the returned array of bone targets for the constraint 
  void GetTargets(XBoneList& targets) const;

  /// \brief  sets the targets to be used by this constraint.
  /// \param  targets - the new array of bone targets for the constraint
  bool SetTargets(const XBoneList& targets);

  /// \brief  sets the targets to be used by this constraint.
  /// \param  targets - the new array of bone targets for the constraint
  /// \param  weights - the new array of weight values for each target
  bool SetTargets(const XBoneList& targets, const XRealArray& weights);

  /// \brief  sets the weights to be used by this constraint.
  /// \param  weights - the new array of weight values for each target
  bool SetWeights(const XRealArray& weights);

  //@}

// internal node data
private:
#ifndef DOXYGEN_PROCESSING
  /// the constrained object
  XId m_Constrained;
  /// the incomming constraining objects
  XIndexList m_Targets;
  /// the blend weight names
  XStringList m_AttrNames;
  /// the weights for each target
  XRealArray m_Weights;
#endif
};
#ifndef DOXYGEN_PROCESSING
DECLARE_API_TYPE(XFn::Constraint, XConstraint, "constraint");
#endif
}
