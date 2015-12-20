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

#ifndef NMRU_POINTIK_H
#define NMRU_POINTIK_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"

#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "NMGeomUtils/NMJointLimits.h"

#include "NMIK/NMIKCommon.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

// Forward declaration
namespace CharacterIK
{
class Solver;
}
namespace PolyHybridIK
{
class Solver;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Provides functions for carrying out Pointing IK, otherwise known as Head Look IK
///
namespace PointIK
{

//----------------------------------------------------------------------------------------------------------------------
// Constants

/// \name These values controls how close the solve needs to be to a singularity in the 'KeepUpright'
/// constraint before the effect is reduced in order to avoid instability.
/// @{
const float xKeepUprightConditionRangeMin = 0.00f;
const float xKeepUprightConditionRangeMax = 0.04f;
/// @}

/// \brief Determines the required accuracy for early exit, as a fraction of the remaining rotation.
/// If set too low, numerical errors will mean that the algorithm thinks it never converges and
/// will keeping looping and iterating until the maximum iterations are reached.
const float xAccuracy = 1e-4f;

//----------------------------------------------------------------------------------------------------------------------
/// \brief The parameters for passing to the PointIK solver functions
///
struct Params
{
  //----------------------------------------------------------------------------------------------------------------------
  /// \name Construction, initialisation
  /// @{
  Params(uint32_t numJointsInChain);

  /// \brief init is necessary for all objects which may be allocated without construction.
  static Params* init(NMP::Memory::Resource* memDesc, uint32_t numJointsInChain);

  /// \brief This version of init sets attributes to their defaults.
  void init(uint32_t numJointsInChain);

  /// \brief getMemoryRequirements is required for centralised memory allocation.
  static NMP::Memory::Format getMemoryRequirements(uint32_t numJointsInChain);

  /// \brief Set joint weights based on a single bias parameter which specifies the slope of
  /// a linear bias curve.  Takes into account disabled joints.
  /// \param fixedEndEffector  Ignore the last joint in the chain when doing the calculation.
  void setLinearBiasCurve(uint32_t numJointsInChain, float bias, bool fixedEndEffector = true);

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Serialisation
  /// @{

  /// \brief Prepare this object for deployment an unknown platform.
  static void dislocate(Params* target, uint32_t numJoints);

  /// \brief Deploy this object on this platform.
  static void locate(Params* target, uint32_t numJoints);

  /// \brief Reset pointers after memory transfer.
  static void relocate(Params* target, void* location, uint32_t numJoints);
  static void relocate(Params* target, uint32_t numJoints) { relocate(target, target, numJoints); }

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  // Data

  /// \brief Direction in frame of end effector which must pass through target.
  NMP::Vector3 pointingVector;

  /// \brief World up axis - used to implement the 'keep upright' feature.
  NMP::Vector3 worldUpAxis;

  /// \brief Value between -1 and 1, specifies the distribution of motion along the IK chain.
  float bias;

  /// \brief Instead of minimising end effector translation, minimise overall joint rotation.  This is the default.
  bool minimiseRotation;

  /// \brief Minimise tilt around the pointing vector, relative to the world up.
  bool keepUpright;

  /// \brief Array of per-joint solver parameters.
  IKJointParams* perJointParams;

}; // End of struct PointIK::Params

//----------------------------------------------------------------------------------------------------------------------
/// \name Solver functions
/// @{

/// \brief Solve an IK chain.
void solve(
  const PointIK::Params* params,
  GeomUtils::PosQuat* joints,
  uint32_t* jointIndices,
  uint32_t numJoints,
  const GeomUtils::PosQuat& target,
  const GeomUtils::PosQuat* globalTransform = 0,
  GeomUtils::DebugDraw* debugBuffer = 0);

/// \brief Solve for a single joint.
/// \return The actual weight 'used', the fraction of the way towards the target orientation moved.
float solveAimJoint(
  const PointIK::Params* params,
  GeomUtils::PosQuat* joints,
  const IKSolverData& solverData,
  float biasWeight,
  const NMP::Vector3* tiltReferenceVector = 0);

/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \name Solver helper functions
/// @{

/// \brief Solve for a particular joint and progress to the next joint - helps
/// prevent duplication of code in inward and outward loops.
/// \return true if the system has been solved to sufficient accuracy.
bool solveJoint(
  const PointIK::Params* params,
  GeomUtils::PosQuat* joints,
  uint32_t* jointIndices,
  uint32_t jointPosition,
  bool isInwardLoop,
  float& weightRemaining,
  float& additionalWeight,
  uint32_t numJoints,
  GeomUtils::PosQuat& localEndEffector,
  const GeomUtils::PosQuat& target,
  const NMP::Vector3& tiltReferenceVector,
  GeomUtils::PosQuat& globalJointTM,
  GeomUtils::DebugDraw* debugBuffer = 0);

/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \name Utility functions
/// @{

/// \brief Return the standard joint bias weight based on a bias factor
float calcBiasWeight(uint32_t jointPosition, uint32_t numJointsInChain, float bias);

/// \brief Test whether the solution has been reached to within a certain accuracy
bool closeEnough(
  const GeomUtils::PosQuat& joint,
  const GeomUtils::PosQuat& localEndEffector,
  const GeomUtils::PosQuat& localTargetFF,
  const NMP::Vector3& pointingVector,
  float accuracy);

/// @}

} // End of namespace PointIK

} // End of namespace NMRU

#include "NMPointIK.inl"

#endif // NMRU_POINTIK_H
