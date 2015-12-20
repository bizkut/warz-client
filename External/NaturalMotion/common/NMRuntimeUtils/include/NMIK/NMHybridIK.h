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

#ifndef NMRU_HYBRIDIK_H
#define NMRU_HYBRIDIK_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMFlags.h"

#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "NMGeomUtils/NMJointLimits.h"

#include "NMIK/NMIKCommon.h"
//----------------------------------------------------------------------------------------------------------------------

// NOTE  HybridIK's debug draw is INCREDIBLY inefficient.  You can switch it off in its entirety using
// NMRU_HYBRIDIK_DEBUGDRAW, or individual bits below.  It will also be switched off in the profiling config.
#ifndef NM_PROFILING
  #define NMRU_HYBRIDIK_DEBUGDRAW
#endif
#ifdef NMRU_HYBRIDIK_DEBUGDRAW
  #define NMRU_HYBRIDIK_DEBUGDRAW_EVERY_JOINT
  #define NMRU_HYBRIDIK_DEBUGDRAW_INITIAL_POSE
  #define NMRU_HYBRIDIK_DEBUGDRAW_HINGESx
  #define NMRU_HYBRIDIK_DEBUGDRAW_LIMITS
  #define NMRU_HYBRIDIK_DEBUGDRAW_TARGET
  #define NMRU_HYBRIDIK_DEBUGDRAW_POLEVECTORSx
  #define NMRU_HYBRIDIK_TWOBONESOLVE_DEBUGDRAWx
  #define NMRU_HYBRIDIK_CCDSOLVER_DEBUGDRAW_POLESx
  #define NMRU_HYBRIDIK_DEBUGDRAW_JOINT_FRAMES
  #define NMRU_HYBRIDIK_DEBUGDRAW_DIRECTION_HINTSx
  #define NMRU_HYBRIDIK_DEBUGDRAW_ONLY_ARGUMENT(_param) _param
#else
  #define NMRU_HYBRIDIK_DEBUGDRAW_ONLY_ARGUMENT(_param)
#endif

// Various tuning settings
#define DIRECT_CLAMP
#define DIRECT_CLAMP_EVEN_FOR_SWIVEL
#define LIMIT_FLIP_SOFTENING
#define TWOBONE_LIMIT_DIVERGENCE_TRADEOFF

namespace NMRU
{

// Forward declaration for parent IK solvers
namespace CharacterIK
{
class Solver;
}
namespace PolyHybridIK
{
class Solver;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Provides functions for carrying out Hybrid CCD IK
///
namespace HybridIK
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Constraint flags - define precedence between IK constraints
enum ConstraintFlags
{
  IKCONSTRAINT_NONE                   = 0,
  IKCONSTRAINT_PRIMARY_POSITION       = 1 << 0,
  IKCONSTRAINT_SECONDARY_POSITION     = 1 << 1,
  IKCONSTRAINT_PRIMARY_ORIENTATION    = 1 << 2,
  IKCONSTRAINT_SECONDARY_ORIENTATION  = 1 << 3,
  IKCONSTRAINT_TERTIARY_ORIENTATION   = 1 << 4,
  IKCONSTRAINT_PRIMARY_POLEVECTOR     = 1 << 5,
  IKCONSTRAINT_SECONDARY_POLEVECTOR   = 1 << 6,
  IKCONSTRAINT_TERTIARY_POLEVECTOR    = 1 << 7,
  IKCONSTRAINT_PRIMARY_BINDPOSE       = 1 << 8,
  IKCONSTRAINT_SECONDARY_BINDPOSE     = 1 << 9,
  IKCONSTRAINT_TERTIARY_BINDPOSE      = 1 << 10,
  IKCONSTRAINT_POSITION               = IKCONSTRAINT_PRIMARY_POSITION | IKCONSTRAINT_SECONDARY_POSITION,
  IKCONSTRAINT_ORIENTATION            = IKCONSTRAINT_PRIMARY_ORIENTATION | IKCONSTRAINT_SECONDARY_ORIENTATION | IKCONSTRAINT_TERTIARY_ORIENTATION,
  IKCONSTRAINT_POLEVECTOR             = IKCONSTRAINT_PRIMARY_POLEVECTOR | IKCONSTRAINT_SECONDARY_POLEVECTOR | IKCONSTRAINT_TERTIARY_POLEVECTOR,
  IKCONSTRAINT_BINDPOSE               = IKCONSTRAINT_PRIMARY_BINDPOSE | IKCONSTRAINT_SECONDARY_BINDPOSE | IKCONSTRAINT_TERTIARY_BINDPOSE,
  IKCONSTRAINT_PRIMARY_GEN_ORIENT     = IKCONSTRAINT_PRIMARY_ORIENTATION | IKCONSTRAINT_PRIMARY_POLEVECTOR,
  IKCONSTRAINT_SECONDARY_GEN_ORIENT   = IKCONSTRAINT_SECONDARY_ORIENTATION | IKCONSTRAINT_SECONDARY_POLEVECTOR,
  IKCONSTRAINT_TERTIARY_GEN_ORIENT    = IKCONSTRAINT_TERTIARY_ORIENTATION | IKCONSTRAINT_TERTIARY_POLEVECTOR
};

/// \brief Constraint flags as NMP::Flags
typedef NMP::Flags Constraints;

//----------------------------------------------------------------------------------------------------------------------
/// \brief A container for holding all the per-joint solver data
struct PerJointParams : public IKJointParams
{
  friend class CharacterIK::Solver;
  friend class PolyHybridIK::Solver;

  ///----------------------------------------------------------------------------------------------------------------------
  /// \brief In-place construct, allowed for friends only
private:
  PerJointParams();
public:

  /// \brief init is necessary for all objects which may be allocated without construction
  static PerJointParams* init(NMP::Memory::Resource* memDesc);
  /// \brief init is necessary for all objects which may be allocated without construction
  /// This form of init sets parameters to their defaults
  void init();

  /// \brief getMemoryRequirements is required for centralised memory allocation
  static NMP::Memory::Format getMemoryRequirements();

  //----------------------------------------------------------------------------------------------------------------------
  // Serialisation

#ifdef NM_UNIT_TESTING
  /// \brief Test-only code for saving out params in a form that can be used to populate
  /// parameters objects.  Do not ever use in production.
  bool saveCode(char prefixString[], std::ostream& stream);
#endif

  /// \brief Endian swap for deployment on a different platform.
  static void endianSwap(PerJointParams* params);

  //----------------------------------------------------------------------------------------------------------------------
  // Accessors and Modifiers

  //----------------------------------------------------------------------------------------------------------------------

  /// \brief This joint's bind pose for bind pose constraints.  Typically would be fixed.
  mutable NMP::Quat bindPose;

  /// \brief A desired orientation, the orientation towards which this joint should tend
  /// in order to keep it within a particular hemisphere (or within joint limits).
  /// To indicate that this quat is not set (and so should not be used), the w value is
  /// set to > 1.
  NMP::Quat directionHintQuat;

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Weights on different constraints, balanced against each other at each level of precedence.
  /// @{
  /// \brief A value between 0 and 1 which restricts this joint's contribution towards achieving
  /// position goals.
  float positionWeight;
  /// \brief A value between 0 and 1 which restricts this joint's contribution towards achieving
  /// end effector pole vector targets.
  float endEffectorPoleVectorWeight;
  /// \brief A value between 0 and 1 which restricts this joint's contribution towards achieving
  /// orientation targets.
  float orientationWeight;
  /// \brief A value between 0 and 1 which restricts this joint's contribution towards the
  /// bind pose.
  float bindPoseWeight;
  /// @}

  /// \brief Use a hybrid CCD/2-bone algorithm at this joint rather than just rotating it on its
  /// own.
  ///
  /// In this algorithm, each joint between the end effector and this one is treated in
  /// turn as the hinge in a 2-bone solve, where this joint is the root.  Currently, only intermediate
  /// joints marked as hinges are considered valid middle joints.
  bool doTwoBoneSolve;

  /// \brief If this is a valid joint index (non-negative and between this joint and the root of
  /// the IK chain) it marks this joint as a hinge to a two-bone system rooted by the specified
  /// joint.
  /// This can substitute the doTwoBoneSolve parameter above, specifying the hinge of the two-bone
  /// system instead of the root.  It is more efficient because it does not require additional
  /// limb traversals.
  int32_t twoBoneRootJointIndex;

  /// \brief This joint is to ignore primary constraints - use for e.g. wrists so you can turn off
  /// the primary position solve and have them concern themselves only with secondary constraints.
  bool secondaryOnly;

}; // end of struct HybridIK::PerJointParams

//----------------------------------------------------------------------------------------------------------------------
/// \brief The parameters for passing to the Hybrid IK solver functions
///
struct Params
{
  friend class CharacterIK::Solver;
  friend class PolyHybridIK::Solver;

  //----------------------------------------------------------------------------------------------------------------------
  // Construction, initialisation

  // \brief In-place construct, for friends only
private:
  Params(uint32_t numJoints);

public:

  /// \brief init is necessary for all objects which may be allocated without construction.
  static Params* init(NMP::Memory::Resource* memDesc, uint32_t numJoints);
  /// \brief init is necessary for all objects which may be allocated without construction.
  /// This form of init sets parameters to their defaults.
  void init(uint32_t numJoints);

  // \brief getMemoryRequirements is required for centralised memory allocation
  static NMP::Memory::Format getMemoryRequirements(uint32_t numJoints);

  //----------------------------------------------------------------------------------------------------------------------
  // Serialisation

#ifdef NM_UNIT_TESTING
  /// \brief Test-only code for saving out params in a form that can be used to populate
  /// parameters objects.  Do not ever use in production.
  bool saveCode(char prefixString[], std::ostream& stream, uint32_t numJoints);
#endif

  //----------------------------------------------------------------------------------------------------------------------
  // Data

  /// \brief Maximum allowable solver iterations.
  uint32_t maxIterations;

  /// \brief End effector pole vector control.
  PoleVectorControlParams endEffectorPoleVectorParams;

  /// \name Identifies which constraints to resolve, in which order of precedence.
  Constraints constraints;

  /// \brief Solve cycle starts from the root joint.
  bool reverseDirectionOfSolve;

  /// \brief Don't solve on return loop.
  bool traverseOneWayOnly;

  /// \brief How much to use direction hints, generally between 0 and 1 is about right
  float directionHintsFactor;

  /// \brief Weight for position vs other competing constraints.
  float targetPositionWeight;

  /// \brief Weight for orientation vs other competing constraints.
  float targetOrientationWeight;

  /// \brief Proportion of the bind pose to apply to each joint before constraint resolution.
  float jointBindPoseWeight;

  /// \brief When end effector is this close to target, exit early.
  float accuracy;

  /// \brief When end effector is this close to target orientation (in units that approximate to
  /// cosine-radians), exit early.
  float orientationAccuracy;

  /// \brief Array of per-joint solver parameters.
  PerJointParams* perJointParams;

}; // End of struct HybridIK::Params

//----------------------------------------------------------------------------------------------------------------------
/// \name Solver functions
/// @{

/// \brief Solve for an IK chain of specified order
void solve(
  const HybridIK::Params* params,
  GeomUtils::PosQuat* joints,
  uint32_t* jointIndices,
  uint32_t numJoints,
  const GeomUtils::PosQuat& target,
  const GeomUtils::PosQuat* globalTransform = 0,
  GeomUtils::DebugDraw* debugBuffer = 0);

/// \brief Solve for an IK chain of specified order with additional input (solutionEE) to feed the
/// solution back to the caller
void solve(
  const HybridIK::Params* params,
  GeomUtils::PosQuat* joints,
  uint32_t* jointIndices,
  uint32_t numJoints,
  const GeomUtils::PosQuat& target,
  GeomUtils::PosQuat& solutionEE,
  const GeomUtils::PosQuat* globalTransform = 0,
  GeomUtils::DebugDraw* debugBuffer = 0);

/// \brief Solve for one joint, primary, secondary and tertiary position, orientation, pole vector
/// and redundancies.
void solveHybridJoint(
  GeomUtils::PosQuat* joints,
  GeomUtils::PosQuat& localEndEffector,
  const IKSolverData& solverData,
  const Params* params);

/// \brief Solve CCD standard method for a single joint
void solveCCDJoint(
  GeomUtils::PosQuat* joints,
  const IKSolverData& solverData,
  const Params* params);

/// \brief Solve 2-bone method for a single joint, treat that joint as the root and iterate over
/// child joints for the hinge.
void solveTwoBoneJoint(
  GeomUtils::PosQuat* joints,
  GeomUtils::PosQuat& localEndEffector,
  const IKSolverData& solverData,
  const Params* params);

/// \brief Solve for the target distance from a specified root joint, this joint acting as the
/// hinge of a two-bone system.
void solveTwoBoneHinge(
  GeomUtils::PosQuat* joints,
  const IKSolverData& solverData,
  const Params* params);

/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \name Solver helper functions
/// @{

/// \brief Solve for a particular joint and progress to the next joint - helps
/// prevent duplication of code in inward and outward loops.
/// \return true if the system has been solved to sufficient accuracy.
bool solveJoint(
  const HybridIK::Params* params,
  GeomUtils::PosQuat* joints,
  uint32_t* jointIndices,
  uint32_t jointPosition,
  uint32_t iteration,
  bool isInwardLoop,
  uint32_t numJoints,
  GeomUtils::PosQuat& localEndEffector,
  const GeomUtils::PosQuat& target,
  GeomUtils::PosQuat& globalJointTM,
  const GeomUtils::PosQuat* globalTransform = 0,
  GeomUtils::DebugDraw* debugBuffer = 0);

/// \brief Solve for a single pass of position, orientation and pole vector control, each with
/// equal precedence.
void solveHybridJointPositionOrientation(
  bool isPrimary,
  GeomUtils::PosQuat* joints,
  GeomUtils::PosQuat& localEndEffector,
  const IKSolverData& solverData,
  const Params* params);

/// \brief Solve any joint for orientation and pole vector targets, either directly or around a
/// redundant swivel axis depending on settings.
void solveJointOrientations(
  bool isPrimary,
  bool redundancyOnly,
  float& weightOut,
  GeomUtils::PosQuat* joints,
  const IKSolverData& solverData,
  const Params* params);

/// \brief Apply the bind pose to this joint with the appropriate weight.
void jointApplyBindPose(
  bool isPrimary,
  bool redundancyOnly,
  GeomUtils::PosQuat* joints,
  const IKSolverData& solverData,
  const Params* params);

/// \brief Allows the rig to be clamped to within limit boundaries from the outset, before
/// the IK iterations begin.
/// This function is blind to the hierarchy, it will just clamp all the joints in the joints
/// array from index zero to index \a numJoints, if they are hard-limited.
void preClampJoints(
  const HybridIK::PerJointParams* jointParams,
  GeomUtils::PosQuat* joints,
  uint32_t numJoints);

/// \brief Centralised place to do joint weighting and clamping
void weightAndClamp(
  const HybridIK::PerJointParams& jointParams,
  const NMP::Quat& qPrev,
  NMP::Quat& qNext,
  bool overrideClampBetween);

/// \brief Calculate the global TM between the end effector and target
void errorTransform(
  const GeomUtils::PosQuat& joint,
  const GeomUtils::PosQuat& localEndEffector,
  const GeomUtils::PosQuat& localTargetFF,
  const GeomUtils::PosQuat& globalJointTM,
  GeomUtils::PosQuat* errorT);

/// \brief Distance between target and end effector position.
float positionError(
  const GeomUtils::PosQuat& localEndEffectorFF,
  const GeomUtils::PosQuat& localTargetFF);

/// \brief Distance between target and end effector orientation - units as (1 - cos-half-angle).
float orientationError(
  const GeomUtils::PosQuat& localEndEffectorFF,
  const GeomUtils::PosQuat& localTargetFF);

/// \brief Distance between target and end effector pole vector - units as (1 - 0.5*(cos-angle + 1)).
float poleVectorError(
  const GeomUtils::PosQuat& localEndEffectorFF,
  const GeomUtils::PosQuat& localTargetFF,
  const PoleVectorControlParams& poleVectorParams);

/// \brief Check to see whether the end effector is within required accuracy of target.
bool closeEnough(
  const GeomUtils::PosQuat& joint,
  const GeomUtils::PosQuat& localEndEffector,
  const GeomUtils::PosQuat& localTargetFF,
  const HybridIK::Params* params);

/// \brief For testing, returns true if the end effector has moved away from the target.
bool divergence(
  const NMP::Quat& qPrev,
  const NMP::Quat& qNext,
  const GeomUtils::PosQuat& localEndEffector,
  const GeomUtils::PosQuat& localTargetFF,
  const HybridIK::Params* params);

/// \brief Rotate, taking into account any hinge axis provided.
void forRotationToTarget(
  const HybridIK::PerJointParams& jointParams,
  const NMP::Quat& qPrev,
  NMP::Quat& qOut,
  const IKSolverData& solverData);

/// \brief Rotate, taking into account any hinge axis provided, applying 'direction hints' feature
/// as appropriate.  This is a separate function in order to provide the simpler alternative interface
/// for external use.
void forRotationToTarget(
  const HybridIK::PerJointParams& jointParams,
  const NMP::Quat& qPrev,
  NMP::Quat& qOut,
  float directionHintsFactor,
  const IKSolverData& solverData);

/// @}

} // End of namespace HybridIK

} // End of namespace NMRU

#include "NMHybridIK.inl"

#endif // NMRU_HYBRIDIK_H
