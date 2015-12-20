// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif

#ifndef NMRU_TWOBONEIK_H
#define NMRU_TWOBONEIK_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"

#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "NMGeomUtils/NMJointLimits.h"

#include "NMIK/NMIKCommon.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief static functions for carrying out two-bone IK
///
namespace TwoBoneIK
{

//----------------------------------------------------------------------------------------------------------------------
// Constants that affect the operation of the solver

/// \brief
static const float xTwoBoneIKReachStabilityFactor = 0.99999f;

//----------------------------------------------------------------------------------------------------------------------
/// \brief TwoBoneIK solver parameters
struct Params
{
  //----------------------------------------------------------------------------------------------------------------------
  // Construction, initialisation

  /// \brief Default constructor.
  Params();

  /// \brief Calculate the hinge-to-child-bone dot product from the geometry.  Set
  /// at start-up and this will be corrected on every call.
  void initialiseHingeDriftParams(GeomUtils::PosQuat* joints);

  //----------------------------------------------------------------------------------------------------------------------

  /// \brief Axis of rotation of the middle joint
  NMP::Vector3 hingeAxis;

  /// \brief Pole vector redundancy control for the middle joint
  PoleVectorControlParams midJointRedundancyParams;

  /// \brief Pole vector control for the end effector
  PoleVectorControlParams endEffectorPoleVectorParams;

  /// \brief Solve for orientation weight for the end joint (only when using \a solveWithEndJoint()).
  float endJointOrientationWeight;

  /// \brief Contribution of swivel towards the orientation of the end effector.
  float swivelOrientationWeight;

  /// \brief Amount of swivel around the axis between root and end effector.
  float swivel;

  /// \brief Restrict the reach of the limb to this fraction of the maximum, which will
  /// prevent undesirable over-straightening.
  /// Defaults to slightly under the maximum reach to provide stability to the algorithm.
  float maximumReachFraction;

  /// \brief Prevent hinge joint from bending backwards (with reference to the start configuration).
  bool preventBendingBackwards;

  /// \brief A pseudo-limit on the hinge rotation, treat angle between bone 1 and bone 2
  /// as 'backwards' if less than this value.  This is different from applying normal
  /// limits in that it can make the algorithm 'choose' an alternate configuration.  The
  /// angle is recorded in tan-half-radians to avoid calls to trig functions at runtime.
  float zeroHingeTanHalfAngle;

  /// \brief For scaling down hinge rotation when the hinge axis is ill-conditioned
  /// (with respect to the two bones), set this between 0 and 1.
  ///
  /// Hinge rotation is scaled by the ratio of the condition number to this value, up
  /// to a maximum of 1.  So if it is 0 (default), there is no scaling and all hinge
  /// conditions are allowed without affecting hinge rotation.  If 1, only hinge axes
  /// that are precisely perpendicular to the two bones will be allowed to rotate
  /// fully.  Since this parameter results in inaccurate results, it should only be
  /// used when 2-bone IK is being used as part of an iterative IK algorithm.
  float hingeConditionLowerBound;

  /// \brief Use this to correct for drift when doing 'continuous mode' IK, i.e. never setting
  /// the geometry back to a bind pose.
  /// Numerical error will cause the child bone's orientation relative to the hinge axis to
  /// drift over time, effectively causing rotation around other axes.  Setting this value
  /// (via \a initialiseHingeDriftParams()) causes this to be corrected.
  float hingeLocusOffset;

  /// \brief Correct hinge drift using the \a hingeLocusOffset param.
  bool correctHingeDrift;

}; // end of struct TwoBoneIK::Params

//----------------------------------------------------------------------------------------------------------------------
/// \name TwoBoneIK solve functions
/// @{

/// \brief Solve - for a two joint system
/// \param hingeAxis, the rotation axis of the hinge joint in its current (moving) frame
/// \param joints Array of three PosQuat transforms.
/// \param joints[0] The root joint, parented to the same reference frame as the target.
/// \param joints[1] The hinge joint, parented to the root.
/// \param joints[2] The end effector, parented to the hinge.  This will not be modified.
void solve(
  const Params* params,
  GeomUtils::PosQuat* joints,
  const GeomUtils::PosQuat& target,
  GeomUtils::DebugDraw* debugBuffer = 0);

/// \brief Solve - for a three joint system, where orientation is also being solved for.  If the
/// orientation weight is less than 1, the position accuracy will be compromised if there is a
/// translational offset between the end effector and end joint.
/// \param joints Array of four PosQuat transforms.
/// \param joints[0] The root joint, parented to the same reference frame as the target.
/// \param joints[1] The hinge joint, parented to the root.
/// \param joints[2] The end joint, parented to the hinge.
/// \param joints[3] The end effector, parented to the end joint, to be targeted.  Will not be modified.
void solveWithEndJoint(
  const Params* params,
  GeomUtils::PosQuat* joints,
  const GeomUtils::PosQuat& target,
  GeomUtils::DebugDraw* debugBuffer = 0);

/// \brief Solve - for a three joint system, where the end joint is fixed.
/// There is a difference between this and \a solveWithEndJoint()!  Namely, there can be an offset
/// between the end joint and the end effector, and there will be no degrading of the position
/// accuracy in the solution.
/// \param joints Array of four PosQuat transforms.
/// \param joints[0] The root joint, parented to the same reference frame as the target.
/// \param joints[1] The hinge joint, parented to the root.
/// \param joints[2] The end joint, parented to the hinge.  Will not be modified
/// \param joints[3] The end effector, parented to the end joint, to be targeted.  Will not be modified.
void solveWithFixedEndJoint(
  const Params* params,
  GeomUtils::PosQuat* joints,
  const GeomUtils::PosQuat& target,
  GeomUtils::DebugDraw* debugBuffer = 0);

/// \brief Solve - for a hinge joint only, returning the two angles of rotation around the
/// hinge axis that can be used.
/// \param hingeAxis Specified in the parent frame (the moving frame of the root joint).
/// \param joints Array of three PosQuat transforms.
/// \param joints[0] The root joint, parented to the same reference frame as the target.
/// \param joints[1] The hinge joint, parented to the root.
/// \param joints[2] The end effector, parented to the hinge.  This will not be modified.
void solveHinge(
  const NMP::Vector3& hingeAxis,
  const GeomUtils::PosQuat* joints,
  const GeomUtils::PosQuat& target,
  float& angleOut1,
  float& angleOut2,
  float maximumReachFraction = xTwoBoneIKReachStabilityFactor,
  GeomUtils::DebugDraw* debugBuffer = 0);

/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \name TwoBoneIK utility functions
/// @{

/// \brief Computes the possible hinge rotation angles, used by the main solve() and by solveHinge().
bool calculateHingeRotation(
  const NMP::Vector3& hingeAxis,
  const NMP::Vector3& lowerBoneVector,
  const NMP::Vector3& upperBoneVector,
  float targetDistance,
  float maximumReachFraction,
  float& tanHalfAngleOut1,
  float& tanHalfAngleOut2,
  const float* hingeLocusOffset = 0);

/// @}

} // End of namespace TwoBoneIK

//----------------------------------------------------------------------------------------------------------------------
// Here to edit default parameters
NM_INLINE TwoBoneIK::Params::Params() :
  hingeAxis(1.0f, 0.0f, 0.0f),
  endJointOrientationWeight(0),
  swivelOrientationWeight(0),
  swivel(0),
  maximumReachFraction(xTwoBoneIKReachStabilityFactor),
  preventBendingBackwards(true),
  zeroHingeTanHalfAngle(0),
  hingeConditionLowerBound(0),
  hingeLocusOffset(0),
  correctHingeDrift(false)
{
  midJointRedundancyParams.init();
  endEffectorPoleVectorParams.init();
}

NM_INLINE void TwoBoneIK::Params::initialiseHingeDriftParams(GeomUtils::PosQuat* joints)
{
  hingeLocusOffset = joints[1].q.rotateVector(NMP::vNormalise(joints[2].t)).dot(hingeAxis);
  correctHingeDrift = true;
}

} // End of namespace NMRU

#endif // NMRU_TWOBONEIK_H
