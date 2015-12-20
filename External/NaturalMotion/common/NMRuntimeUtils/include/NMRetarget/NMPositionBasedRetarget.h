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

#ifndef NMRU_POSITIONBASEDRETARGET_H
#define NMRU_POSITIONBASEDRETARGET_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBuffer.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "NMGeomUtils/NMJointLimits.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Functions and data for retargeting poses between a point-cloud rig (worldspace positions only)
/// and a normal rig.
namespace PositionBasedRetarget
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief A structure holding the parameters that describe how a particular source joint in the
/// rig will retarget
struct SourceJointParams
{

  //----------------------------------------------------------------------------------------------------------------------
  // Construction and initialisation

  /// \brief init is necessary for all objects which may be allocated without construction
  /// This form of init does not set any parameters to their defaults
  static SourceJointParams* init(NMP::Memory::Resource* memDesc);
  /// \brief Initialise contents to default values
  void setDefaults();

  /// \brief getMemoryRequirements is required for centralised memory allocation
  static NMP::Memory::Format getMemoryRequirements();

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Serialisation
  /// @{

  /// \brief Prepare this object for another platform.
  void endianSwap();

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief Defines how motion being calculated for this joint's parent is spread between the two.
  float sourceJointBias;

  /// \brief Defines how sibling joints (parented to the same joint) each affected motion of the parent.
  float sourceJointPriority;

  /// \brief Defines how much confidence there is in this joint's position data, effectively a weight factor.
  float sourceJointConfidence;

}; // end of struct PositionBasedRetarget::SourceJointParams

//----------------------------------------------------------------------------------------------------------------------
/// \brief A structure holding the parameters that describe how a particular target joint in the
/// rig will retarget
struct TargetJointParams
{

  //----------------------------------------------------------------------------------------------------------------------
  // Construction and initialisation

  /// \brief init is necessary for all objects which may be allocated without construction
  /// This form of init does not set any parameters to their defaults
  static TargetJointParams* init(NMP::Memory::Resource* memDesc);
  /// \brief Initialise contents to default values
  void setDefaults();

  /// \brief getMemoryRequirements is required for centralised memory allocation
  static NMP::Memory::Format getMemoryRequirements();

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Serialisation
  /// @{

  /// \brief Prepare this object for another platform.
  void endianSwap();

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief Defines the local reference frame for this joint.
  GeomUtils::PosQuat offsetTransform;

  /// \brief Whether or not the translation component of the joint geometry should be retargeted
  bool isRigid;

  /// \brief The weight adjusts how much rotation this joint should have relative to its start position
  float rotationWeight;

  /// \brief The weight adjusts how much translation this joint should have relative to its start position
  float positionWeight;

  /// \brief Whether or not to apply joint limits at this joint.
  bool isLimited;

  /// \brief Identifies hinge limits (at the moment hinge specification is not supported within JointLimit::Params)
  bool isHinge;

  /// \brief Specification of joint limits for this joint
  JointLimits::Params limits;

}; // end of struct PositionBasedRetarget::TargetJointParams

//----------------------------------------------------------------------------------------------------------------------
/// \brief A structure holding the parameters that describe how a source rig will retarget - currently just
/// the per-joint parameters.
struct SourceParams
{

  //----------------------------------------------------------------------------------------------------------------------
  // Construction and initialisation

  /// \brief init is necessary for all objects which may be allocated without construction
  /// This form of init does not set any parameters to their defaults
  static SourceParams* init(NMP::Memory::Resource* memDesc, uint32_t numJoints);
  /// \brief Initialise contents to default values
  void setDefaults(uint32_t numJoints);

  /// \brief getMemoryRequirements is required for centralised memory allocation
  static NMP::Memory::Format getMemoryRequirements(uint32_t numJoints);

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Serialisation
  /// @{

  /// \brief Prepare this object for deployment an unknown platform.
  void dislocate(uint32_t numJoints);

  /// \brief Deploy this object on this platform
  void locate(uint32_t numJoints);

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief The parameters that define how each joint retargets
  SourceJointParams* jointParams;

}; // end of struct PositionBasedRetarget::SourceParams

//----------------------------------------------------------------------------------------------------------------------
/// \brief A structure holding the parameters that describe how a target rig will retarget - a joint
/// index list into the source rig, the parameters per-joint etc.
struct TargetParams
{

  //----------------------------------------------------------------------------------------------------------------------
  // Types

  /// \brief Special settings for passing out transform data that helps understand what is going on in the
  /// retargeting process, or whether problems are due to the source data or the retargeter.
  enum DebugMode
  {
    kRetargetDebuggingOff,
    kRetargetDebuggingPassThrough,
    kRetargetDebuggingShowOffsets,
    kRetargetDebuggingInvalid
  };

  //----------------------------------------------------------------------------------------------------------------------
  // Construction and initialisation

  /// \brief init is necessary for all objects which may be allocated without construction
  /// This form of init does not set any parameters to their defaults
  static TargetParams* init(NMP::Memory::Resource* memDesc, uint32_t numJoints);
  /// \brief Initialise contents to default values
  void setDefaults(uint32_t numJoints);

  /// \brief getMemoryRequirements is required for centralised memory allocation
  static NMP::Memory::Format getMemoryRequirements(uint32_t numJoints);

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Serialisation
  /// @{

  /// \brief Prepare this object for deployment an unknown platform.
  void dislocate(uint32_t numJoints);

  /// \brief Deploy this object on this platform
  void locate(uint32_t numJoints);

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief An overall scale factor for bringing the source rig to the same scale as the target
  float rigScale;

  /// \brief Applies retargeting with some debug settings which allow some settings to be visualised more easily.
  DebugMode debugMode;

  /// \brief Maps joint indices in this rig to a source rig.
  int32_t* targetToSourceMap;

  /// \brief The parameters that define how each joint retargets
  TargetJointParams* jointParams;

}; // end of struct PositionBasedRetarget::TargetParams

//----------------------------------------------------------------------------------------------------------------------
/// \brief Contains PositionBasedRetarget solve methods, and working memory and variables that have to
/// be initialised.
class Solver
{
public:

  //----------------------------------------------------------------------------------------------------------------------
  // Construction, initialisation

  /// \brief init is necessary for all objects which may be allocated without construction
  static Solver* init(
    NMP::Memory::Resource* memDesc,
    uint32_t numSourceJoints,
    uint32_t numTargetJoints);

  // \brief getMemoryRequirements is required for centralised memory allocation
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t numSourceJoints,
    uint32_t numTargetJoints);

  /// \brief Deep copy of contents to another object.
  void copy(Solver* dest);

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Serialisation
  /// @{

  /// \brief Prepare this object for deployment an unknown platform.
  void dislocate();

  /// \brief Deploy this object on this platform
  void locate();

  /// \brief Recalculate internal pointers for this object.
  void relocate();

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Retargeting functions
  /// @{

  /// \brief Recover translations and positions on a target rig based on worldspace position
  /// data from the source.
  /// \param scaleMultiplier  Allows the rig scale mapping to be adjusted at runtime.
  /// \param ignoreInputJoints  Only output joints affected by retarget and mark additional channels as unused.
  void retarget(
    const SourceParams* sourceParams,
    const NMP::DataBuffer* sourceTransforms,
    const TargetParams* targetParams,
    NMP::DataBuffer* targetTransforms,
    const int32_t* targetHierarchy,
    const NMRU::GeomUtils::PosQuat& rootTransform,
    float scaleMultiplier = 1.0f,
    bool ignoreInputJoints = false,
    bool calculateOffsets = false);

  /// \brief Does the same as a retarget except that offsets are applied for the retarget but then
  /// not removed at the end.  This can be used to calculate offsets by retargeting between two
  /// equivalent poses.
  void calculateOffsets(
    const SourceParams* sourceParams,
    const NMP::DataBuffer* sourceTransforms,
    const TargetParams* targetParams,
    NMP::DataBuffer* targetTransforms,
    const int32_t* targetHierarchy,
    const NMRU::GeomUtils::PosQuat& rootTransform,
    float scaleMultiplier = 1.0f);

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief Prepare solver for a new target of rigs to retarget to - must be called whenever
  /// the rigs change.
  /// \return \a false if the mapping fails validation.  This will happen when your mapping results in an
  /// incompatibility between your target and source hierarchies, or if your target rig is not in
  /// strict root-first order (parent joints have a lower channel index than their children).
  bool prepare(
    const TargetParams* targetParams,
    uint32_t numTargetJoints,
    const int32_t* targetHierarchy);

  /// \brief Version of \a prepare which just takes a target-to-source map instead of the whole
  /// target params.
  /// \return \a false if the mapping fails validation.  This will happen when your mapping results in an
  /// incompatibility between your target and source hierarchies, or if your target rig is not in
  /// strict root-first order (parent joints have a lower channel index than their children).
  bool prepare(
    const int32_t* targetToSourceMap,
    uint32_t numTargetJoints,
    const int32_t* targetHierarchy);

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Utilities
  /// @{

  /// \brief Retrieves the estimated rig scale for the target rig, which can be used to get a better match
  /// between rigs, by copying directly into the TargetParams->rigScale.
  float getEstimatedRigScale() const;

  /// \brief Checks through a mapping and makes sure it is compatible with the hierarchy provided.
  static bool validateMapping(
    const int32_t* targetToSourceMap,
    uint32_t numTargetJoints,
    const int32_t* targetHierarchy);

  /// @}

protected:

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Maps and hierarchies
  /// @{

  /// \brief The retargeting system can calculate its own estimate of the rig scale, which can be used for
  /// setting up your target rig or just allow you to adapt on the fly to changes in scale.
  float m_estimatedTargetRigScale;

  /// \brief The source rig is fixed so its size cannot change.
  uint32_t m_numSourceJoints;

  /// \brief The number of joints in the largest possible target rig.
  uint32_t m_numTargetJoints;

  /// \brief Allows copies by recording the memory requirements of this object.
  NMP::Memory::Format m_memoryRequirements;

  /// \brief Identifies the target joint for each source joint, or -1 if unmapped
  int32_t* m_sourceToTargetMap;

  /// \brief Stores the hierarchy of mapped source joints, which varies depending on the target's mapping
  int32_t* m_mappedHierarchy;

  /// \brief Stores a reverse hierarchy, an array of mapped child joints per source joint, or NULL
  /// for no children
  int32_t** m_reverseHierarchy;

  /// \brief The number of mapped children per source joint, or -1 for unmapped
  int32_t* m_numMappedChildren;

  /// \brief The reverse hierarchy data as a block of indices
  int32_t* m_reverseHierarchyData;

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Working memory
  /// @{

  /// \brief Temporary storage for localspace target transforms input to the solver
  NMP::DataBuffer* m_inputTransforms;

  /// \brief Temporary storage for worldspace transforms of the input target rig
  NMP::DataBuffer* m_worldTransforms;

  /// \brief Temporary storage for the confidence in the orientation of each target joint based on the confidence for
  /// the two source joints on which it is based.
  float* m_targetRotationConfidence;

  /// \brief Temporary storage for the confidence in the position of each target joint based on the confidence for
  /// the two source joints on which it is based.
  float* m_targetPositionConfidence;

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Utilities
  /// @{

  /// \brief Accumulates transforms between \a jointIndex and \referenceJointIndex (inclusive of \a jointIndex and
  /// exclusive of \a referenceJointIndex) and fills in the accumulated result, compounded with the \a baseTransform
  void fillTransforms(
    const int32_t* hierarchy,
    const NMP::DataBuffer* transformBuffer,
    uint32_t jointIndex,
    int32_t referenceJointIndex,
    const NMRU::GeomUtils::PosQuat& baseTransform);

  /// \brief Much faster approximation for vector slerp - does a lerp but rescales the magnitude to what the
  /// slerp would give, i.e. a linear change in magnitude as well as an approximately linear change in direction.
  NMP::Vector3 vectorLerpDirection(
    const NMP::Vector3& sourceVec0,
    const NMP::Vector3& sourceVec1,
    float alpha);

  /// @}

}; // end of class PositionBasedRetarget::Solver

} // end of namespace PositionBasedRetarget

} // end of namespace NMRU

#include "NMPositionBasedRetarget.inl"

#endif // NMRU_POSITIONBASEDRETARGET_H
