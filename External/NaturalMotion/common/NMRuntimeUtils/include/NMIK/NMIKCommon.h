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

#ifndef NMRU_NMIKCOMMON_H
#define NMRU_NMIKCOMMON_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMProfiler.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "NMGeomUtils/NMJointLimits.h"

//----------------------------------------------------------------------------------------------------------------------

/// \name Macros for optimised functionality of NMIK
/// @{

/// Adds an additional clamp per joint before the solve, to help deal with passing in poses that violate
/// joint limits.  Shouldn't be necessary but might help convergence or issues with locking.
#define NMIK_ENABLE_PRECLAMPINGx

/// Runtime-specific optimisations

/// NMIK_OPTIMISE makes some specific assumptions about the way IK is used which for the time being are valid.
/// In future more flexibility may be required, but for now it's best to keep things as lean as possible.
#ifndef NMIK_OPTIMISE
  #define NMIK_OPTIMISE
#endif
#ifdef NMIK_OPTIMISE

  /// Identifies uses where the joint limit offset frame is the identity (the child frame is the limited frame)
  #define NMIK_JOINT_LIMITS_HAVE_IDENTITY_OFFSET_FRAME

  /// Strips out all the code which deals with tertiary constraints, in the case they're not used
  #define NMIK_NO_TERTIARIES

#endif // NMIK_OPTIMISE

/// @}

/// \name Tuning settings for all solvers
/// @{

#define SMOOTH_LIMITSx

/// @}

//----------------------------------------------------------------------------------------------------------------------
/// Profiling
#if defined(PROFILE)
  #if defined(NM_HOST_X360)
    #define NMIK_PROFILE_BEGIN(ev) \
    PIXBeginNamedEvent(0, ev); \
    NM_BEGIN_PROFILING(ev)
    #define NMIK_PROFILE_END() \
    PIXEndNamedEvent(); \
    NM_END_PROFILING()
  #elif defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
    #define NMIK_PROFILE_BEGIN(ev) NM_BEGIN_PROFILING(ev)
    #define NMIK_PROFILE_END() NM_END_PROFILING()
  #endif
#else
  #define NMIK_PROFILE_BEGIN(ev) {}
  #define NMIK_PROFILE_END() {}
#endif

//----------------------------------------------------------------------------------------------------------------------
// Other preprocessor macros

#ifdef NM_DEBUG
  #define NMIK_DEBUG_ONLY_PARAM(p) p
#else
  #define NMIK_DEBUG_ONLY_PARAM(p)
#endif

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
/// \name Shared IK solver constants
/// @{
/// \brief Limit-tightening factor to ensure clamped orientations are inside limits
/// even after numerical manipulation.
const float xClampTighten = 1.0f;
/// \brief Smoothness of limit boundaries - smoothing brings boundary inwards but improves
/// convergence and smoothness
const float xLimitSmoothness = 0.1f;
/// \brief Drop-off around hinge rotations and swivels, to prevent large rotations in poorly
/// conditioned situations
const float xHingeSoftness = 0.2f;
/// \brief Drop-off around 180 degree rotations, to prevent flipping of joints between rotating
/// in one direction and rotating in the other
const float xFlipSoftness = 0.2f;
/// @}

//----------------------------------------------------------------------------------------------------------------------
/// \brief A container for holding typical per-joint solver data for an IK solver
struct IKJointParams
{

  ///----------------------------------------------------------------------------------------------------------------------
  /// \brief Prevent default construction
protected:
  IKJointParams();

public:

  /// \brief init is necessary for all objects which may be allocated without construction
  static IKJointParams* init(NMP::Memory::Resource* memDesc);

  /// \brief Put sensible default values into the object
  void setDefaults();

  /// \brief getMemoryRequirements is required for centralised memory allocation
  static NMP::Memory::Format getMemoryRequirements();

  //----------------------------------------------------------------------------------------------------------------------
  // Serialisation

  /// \brief Endian swap for deployment on a different platform.
  static void endianSwap(IKJointParams* params);

  //----------------------------------------------------------------------------------------------------------------------
  // Accessors and Modifiers

  /// \brief Based on the hinge limit parameters, choose a hinge axis.
  void setHingeParametersFromLimits(
    const NMP::Vector3& parentBone,
    const NMP::Vector3& childBone);

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Hinge joints
  /// @{

  /// \brief Axis of rotation of this joint - rotation off this axis will not be allowed.
  NMP::Vector3 hingeAxis;

  /// \brief This joint is or is not a hinge.
  bool isHinge;

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Weights
  /// @{

  /// \brief A value between 0 and 1 which restricts overall motion at this joint.
  float weight;

  /// \brief A value between 0 and 1 which restricts this joint's contribution towards tertiary
  /// goals via redundant degrees of freedom.
  float redundancyControlWeight;

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Joint  limits
  /// @{

  /// \brief This joint's limits are defined.
  bool isLimited;
  /// \brief Clamp this joint's motion to limits provided.
  bool applyHardLimits;
  /// \brief Weight the motion of this joint based on limits.
  bool applySoftLimits;
  /// \brief A positive value which defines how easily a joint can approach the limit boundary, where
  /// zero means no repulsion (joint will always move onto boundary).
  float hardLimitRepulsionFactor;
  /// \brief A positive value which defines how easily a joint will move in relation to the limit
  /// boundary.  The higher the value, the softer the limit and the less influence the limit
  /// has on joint motion.  Good values are around 5 to 10.
  float softLimitSoftness;
  /// \brief Specification of joint limits for this joint.
  JointLimits::Params limits;

  /// @}

  /// \brief Joint enable flag, has the same effect as setting the joint weight to zero except
  /// that the original weight setting is not lost.  Can be used to identify fixed joints or
  /// transforms (such as end effectors) in the rig.
  bool enabled;

}; // end of struct IKJointParams

///----------------------------------------------------------------------------------------------------------------------
/// \brief A container for generic pole vector control parameters
struct PoleVectorControlParams
{
  ///----------------------------------------------------------------------------------------------------------------------
  /// \brief Construct.
  PoleVectorControlParams();

  /// \brief init is necessary for all objects which may be allocated without construction.
  static PoleVectorControlParams* init(NMP::Memory::Resource* memDesc);
  void init();

  /// \brief getMemoryRequirements is required for centralised memory allocation.
  static NMP::Memory::Format getMemoryRequirements();

  ///----------------------------------------------------------------------------------------------------------------------
  // Serialisation

#ifdef NM_UNIT_TESTING
  /// \brief Test-only code for saving out params in a form that can be used to populate
  /// parameters objects.  Do not ever use in production.
  bool saveCode(char prefixString[], std::ostream& stream);
#endif

  /// \brief Endian swap for deployment on a different platform.
  static void endianSwap(PoleVectorControlParams* params);

  ///----------------------------------------------------------------------------------------------------------------------
  // Parameters

  /// \brief The vector, fixed to the parent frame at this node, to be brought as close as
  /// possible to the reference pole vector by using redundant joint rotation.
  NMP::Vector3 controlPoleVector;

  /// \brief The vector, fixed to the root frame (or another node if specified) that determines
  /// where you would like the control pole vector to 'point'.
  NMP::Vector3 referencePoleVector;

  /// \brief Whether or not to apply redundancy control.
  bool active;

  /// \brief How much weight to give redundancy control at this node
  float weight;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Parameters encapsulating tertiary constraints
///
struct TertiaryConstraints
{
  //----------------------------------------------------------------------------------------------------------------------
  // Construction, initialisation
private:
  TertiaryConstraints();  // Prevents default construction

public:

  /// \brief init is necessary for all objects which may be allocated without construction.
  static TertiaryConstraints* init(
    NMP::Memory::Resource* memDesc,
    uint32_t numPoleVectorControllers,
    uint32_t numRotations = 0);
  /// \brief init is necessary for all objects which may be allocated without construction.
  /// This form of init sets parameters to their defaults.
  void init();

  // \brief getMemoryRequirements is required for centralised memory allocation
  static NMP::Memory::Format getMemoryRequirements(uint32_t numPoleVectorControllers, uint32_t numRotations = 0);

  //----------------------------------------------------------------------------------------------------------------------
  // Data

  /// \brief Array of pole vector controllers
  PoleVectorControlParams* poleVectorControllers;

  /// \brief Array of quat constraints
  NMP::Quat* quatConstraints;

  /// \brief Array of joint indices identifying which constraints apply to which joints, aids memory
  /// usage when doing multiple end effector IK
  uint32_t* poleVectorJoints;

  /// \brief Array of joint indices identifying which constraints apply to which joints, aids memory
  /// usage when doing multiple end effector IK
  uint32_t* quatJoints;

  /// \brief Size of array of pole vector control parameters
  uint32_t maxNumPoleVectorControllers;

  /// \brief Size of array of quat constraints
  uint32_t maxNumRotations;

  /// \brief Number of used pole vector controllers
  uint32_t numPoleVectorControllers;

  /// \brief Number of used quat constraints
  uint32_t numRotations;

}; // end of struct TertiaryConstraints

//----------------------------------------------------------------------------------------------------------------------
/// \brief Persistent (const) data for passing between solver utilities, to package common arguments together.
///
struct IKSolverData
{
  /// \brief The end effector frame in the moving frame of the current joint.
  GeomUtils::PosQuat localEndEffector;

  /// \brief The target transform in the fixed frame of the current joint.
  GeomUtils::PosQuat localTarget;

  /// \brief The fixed frame of the current joint relative to the global frame.
  GeomUtils::PosQuat globalJointTM;

  /// \brief The original joint orientation from the start of the solve.
  NMP::Quat solveStartQuat;

  /// \brief The parameters for tertiary constraints being used for this solve.
  const TertiaryConstraints* tertiaries;

  /// \brief The current position on the joint chain, starting from 0 at the root.
  uint32_t jointPosition;

  /// \brief The map from joint positions to indices into the hierarchy.
  const uint32_t* jointIndices;

  /// \brief The number of joints in this chain.
  uint32_t numJoints;

  /// \brief The debug draw buffer (allowed to be modified even if object is const).
  mutable GeomUtils::DebugDraw* debugBuffer;

}; // End of struct IKSolverData

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \name Shared IK utilities
/// @{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Solver utility for tertiary constraints
void solveJointTertiaries(
  float& weightOut,
  const IKJointParams* jointParams,
  GeomUtils::PosQuat* joints,
  const IKSolverData& solverData);

/// \brief Check for joint violation, also taking into account whether joint actually has limits
char isViolated(const IKJointParams& limitParams, const NMP::Quat& q, float factor = xClampTighten);

/// @}

} // end of namespace NMRU

#include "NMIKCommon.inl"

#endif // NMRU_NMIKCOMMON_H
