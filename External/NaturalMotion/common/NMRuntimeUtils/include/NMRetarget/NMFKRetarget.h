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

#ifndef NMRU_FKRETARGET_H
#define NMRU_FKRETARGET_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMathUtils.h"
#include "NMGeomUtils/NMGeomUtils.h"
#include "NMPlatform/NMvpu.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMFlags.h"
#include "NMPlatform/NMBuffer.h"
#include "NMPlatform/NMHierarchy.h"
#include "NMPlatform/NMMathPlatform.h"
#include "NMPlatform/NMFastHeapAllocator.h"
#include "NMGeomUtils/NMJointLimitsMP.h"
#include "NMGeomUtils/NMJointLimits.h"
#include "NMIK/NMIKCommon.h"
//----------------------------------------------------------------------------------------------------------------------

#define NMRU_FKRETARGET_VPU_OPTIMISE
#define NMRU_FKRETARGET_VPU_N_TO_M

namespace NMRU
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Functions and data for joint-to-joint retargeting poses between two rigs.
namespace FKRetarget
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Mirroring flags - define mirroring of rotation or translation around each axis
enum MirroringFlags
{
  RETARGETMIRROR_NONE             = 0,
  RETARGETMIRROR_ROTATION_X       = 1 << 0,
  RETARGETMIRROR_ROTATION_Y       = 1 << 1,
  RETARGETMIRROR_ROTATION_Z       = 1 << 2,
  RETARGETMIRROR_TRANSLATION_X    = 1 << 3,
  RETARGETMIRROR_TRANSLATION_Y    = 1 << 4,
  RETARGETMIRROR_TRANSLATION_Z    = 1 << 5
};

/// \brief Mirroring flags as NMP::Flags
typedef NMP::Flags Mirroring;

//----------------------------------------------------------------------------------------------------------------------
/// \brief A structure holding the parameters that describe how a particular joint in the
/// rig will retarget
struct PerJointParams
{

  //----------------------------------------------------------------------------------------------------------------------
  // Construction and initialisation

  /// \brief init is necessary for all objects which may be allocated without construction.
  /// init does not set any parameters to their defaults.
  static PerJointParams* init(NMP::Memory::Resource* memDesc);

  /// \brief Initialise contents to default values.
  void setDefaults();

  /// \brief getMemoryRequirements is required for centralised memory allocation.
  static NMP::Memory::Format getMemoryRequirements();

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Serialisation
  /// @{

  /// \brief Prepare this object for another platform.
  void endianSwap();

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief Defines the offset of this joint from the template or standard joint transform.
  /// Note that if this is changed you need to call Solver::prepare() before doing retargeting.
  GeomUtils::PosQuat offsetTransform;

  /// \brief Defines the local reference frame for this joint.
  /// Note that if this is changed you need to call Solver::prepare() before doing retargeting.
  GeomUtils::PosQuat referenceTransform;

  /// \brief Specification of joint limits for this joint
  JointLimits::Params limits;

  /// \brief The weight adjusts how much rotation this joint should have relative to its input orientation
  float rotationWeight;

  /// \brief The weight adjusts how much translation this joint should have relative to its input position
  float positionWeight;

  /// \brief Defines how motion is distributed along a sequence.  0 means evenly spread, -1 means biased
  /// towards doing the motion at the root, 1 means biased towards the end joint.
  /// The bias parameter is taken from that for the end joint in a sequence and ignored for the other joints.
  float bias;

  /// \brief Invert rotation and translation around each axis.
  Mirroring mirrorFlags;

  /// \brief Whether or not the translation component of the joint geometry should be retargeted.
  bool isRigid;

  /// \brief Whether or not to apply joint limits at this joint.
  bool isLimited;

  /// \brief For joints that are part of a sequence, take any motion of this joint that has been clamped
  /// off and redistribute to other joints in the sequence.
  bool redistributeClampedMotion;

}; // end of struct FKRetarget::PerJointParams

//----------------------------------------------------------------------------------------------------------------------
/// \brief A structure holding the parameters that describe how a rig will retarget - a joint
/// index list into an intermediate rig, weights etc.
struct Params
{

  //----------------------------------------------------------------------------------------------------------------------
  // Construction and initialisation

  /// \brief init is necessary for all objects which may be allocated without construction.
  /// init does not set any parameters to their defaults.
  static Params* init(NMP::Memory::Resource* memDesc, uint32_t numJoints);

  /// \brief Initialise contents to default values.
  void setDefaults(uint32_t numJoints);

  /// \brief getMemoryRequirements is required for centralised memory allocation.
  static NMP::Memory::Format getMemoryRequirements(uint32_t numJoints);

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Serialisation
  /// @{

  /// \brief Reformat this object for this platform.
  void locate(uint32_t numJoints);

  /// \brief Reformat this object for another platform.
  void dislocate(uint32_t numJoints);

  /// \brief Recalculate pointers to trailing objects on a new platform.
  void relocate(void* location, uint32_t numJoints);
  void relocate(uint32_t numJoints) { relocate(this, numJoints); }

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief An overall scale factor for normalising this rig's size.
  float rigScale;

  /// \brief Maps joint indices in this rig to an intermediate rig.  -1 means this joint is not mapped.
  int32_t* indexMap;

  /// \brief Maps joint indices in this rig to their reference roots.
  /// If the reference joint is this joint, motion is relative to the input frame of this joint, if the
  /// parent, motion is relative to the parent's reference frame, and the same for other joints, which
  /// must be in the direct ancestry of this joint.  -1 means the joint is referenced to the world.
  int32_t* jointReferenceIndex;

  /// \brief The parameters that define how each joint retargets.
  PerJointParams* perJointParams;

}; // end of struct FKRetarget::Params

//----------------------------------------------------------------------------------------------------------------------
/// \brief Contains FKRetarget solve methods, and working memory and variables that have to
/// be initialised.
class Solver
{

  //----------------------------------------------------------------------------------------------------------------------
  // Construction, initialisation

public:

  //----------------------------------------------------------------------------------------------------------------------
  // Types

  /// \brief Info needed to determine memory requirements
  struct StorageStats
  {
    uint32_t intermediateRigSize;
    uint32_t largestRigSize;
    uint32_t mostMappedJoints;
    uint32_t longestSequence;
    uint32_t mostMultiplexPackages; ///< \brief Set to 0 if unknown, and init will use the maximum possible
  };

  //----------------------------------------------------------------------------------------------------------------------
  // Construction, initialisation

  /// \brief init is necessary for all objects which may be allocated without construction.
  static Solver* init(
    NMP::Memory::Resource* memDesc,
    const StorageStats& storageStats);

  /// \brief getMemoryRequirements is required for centralised memory allocation.
  static NMP::Memory::Format getMemoryRequirements(
    const StorageStats& storageStats);

  /// \brief Deep copy of contents to another object.
  void copy(Solver* dest);

  //----------------------------------------------------------------------------------------------------------------------
  // Serialisation

  /// \brief Reformat this object for this platform.
  void locate();

  /// \brief Reformat this object for another platform.
  void dislocate();

  /// \brief Recalculate internal pointers for this object.
  void relocate(void* location);

  /// \brief Recalculate internal pointers for this object.
  void relocate() { relocate(this); }

  /// \brief Get instance memory requirements
  NMP::Memory::Format getInstanceMemoryRequirements() { return m_memoryRequirements; }

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Retargeting functions
  /// @{

  /// \brief One-to-one transfer of transforms between rigs, on the VPU.
  void retarget1to1(
    const Params* sourceParams,
    const NMP::DataBuffer* sourceTransforms,
    const Params* targetParams,
    NMP::DataBuffer* targetTransforms);

  /// \brief N-to-M transfer of transforms between rigs, also needs rig hierarchies.
#ifdef NMRU_FKRETARGET_VPU_N_TO_M
  void retarget(
    const NMP::DataBuffer* sourceTransforms,
    NMP::DataBuffer* targetTransforms,
    NMP::TempMemoryAllocator* tempAllocator);
#else
  void retarget(
    const Params* sourceParams,
    const NMP::DataBuffer* sourceTransforms,
    const NMP::Hierarchy* sourceHierarchy,
    const Params* targetParams,
    NMP::DataBuffer* targetTransforms,
    const NMP::Hierarchy* targetHierarchy);
#endif

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Accessors
  /// @{

  /// \brief Get the size of the arrays of source multiplex packages
  uint32_t getNumSourcePackages();

  /// \brief Get the size of the arrays of target multiplex packages
  uint32_t getNumTargetPackages();

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief Prepare solver for a new pair of rigs to retarget to and from - must be called
  /// whenever the rigs change.
  void prepare(
    const Params* sourceParams,
    uint32_t numSourceJoints,
    const NMP::Hierarchy* sourceHierarchy,
    const Params* targetParams,
    uint32_t numTargetJoints,
    const NMP::Hierarchy* targetHierarchy);

  /// \brief Prepare solver's multiplex-packaged copies of retargeting parameters for a new pair
  /// of rigs to be mapped - must be called whenever the rigs change.
  void prepareMP(
    const Params* sourceParams,
    const NMP::Hierarchy* sourceHierarchy,
    const Params* targetParams,
    const NMP::Hierarchy* targetHierarchy);

  /// \brief Overwrite the solver's multiplex-packaged offset transforms only, leaving everything
  /// else the same.
  /// This method is quite specific to a particular requirement to be able to update the target
  /// offsets on the fly, and may well be modified or expanded to become more general.
  void overwriteTargetOffsetsMP(
    const NMP::DataBuffer* offsetTransforms,
    const NMP::DataBuffer* referenceTransforms);

  /// \brief Overwrite one of the solver's multiplex-packaged offset transforms, leaving everything
  /// else the same.
  /// This method is quite specific to a particular requirement to be able to update the target
  /// offsets on the fly, and may well be modified or expanded to become more general.
  void overwriteTargetOffsetMP(
    int32_t jointIndex,
    const NMP::PosQuat& offsetTransforms,
    const NMP::PosQuat& referenceTransforms);

  /// \brief Overwrite the solvers target scale
  /// This method is quite specific to a particular requirement to be able to update the target
  /// offsets on the fly, and may well be modified or expanded to become more general.
  void overwriteTargetScale(float scale);

protected:

  //----------------------------------------------------------------------------------------------------------------------
  /// \brief Container for intermediate rig per-joint data, changes with each change of
  /// source or target, and reset with prepare().
  struct IntermediateJointData
  {
    int32_t sourceIndex; ///< \brief Map to end joint in source sequence
    int32_t targetIndex; ///< \brief Map to end joint in target sequence
    uint32_t sourceSequenceLength;
    uint32_t targetSequenceLength;

    /// \brief This joint is a root for one or more referenced joints, i.e. transforms will
    /// be accumulated up to this joint but no further.
    bool isRootJoint;
  };

  /// \brief Container for index maps between multiplexed geometry arrays and channel indices.
  struct MPIndexMap
  {
    /// \name Per-joint hierarchy and mapping info
    /// @{

    /// \brief The channel index of this joint.
    int32_t jointIndex[4];

    /// \brief The channel index of this joint's parent, or a dummy channel with identity TMs.
    int32_t jointParent[4];

    /// \brief The channel index of the joint to which this joint maps in the other rig.  The actual
    /// transform is a blend of jointPartnerBlendFrom and jointPartnerBlendTo.
    int32_t jointPartnerBlendFrom[4];

    /// \brief The channel index of the joint to which this joint maps in the other rig.  The actual
    /// transform is a blend of jointPartnerBlendFrom and jointPartnerBlendTo.
    int32_t jointPartnerBlendTo[4];

    /// \brief The blend weights with which to blend this package when retargeting (for sequences)
    float blendWeights[4];

    /// @}

    /// \name Mixtures of per-joint parameters from source and target, packaged
    /// @{

    /// \brief The rotation weights to apply to this package when retargeting
    float rotationWeights[4];

    /// \brief The position weights to apply to this package when retargeting
    float positionWeights[4];

    /// @}

    /// \name Per-joint flags
    /// @{

    /// \brief Flags used to identify unused or unmapped entries in each package.
    bool valid[4];

    /// \brief Flags used to identify package entries with no parent.
    bool parentValid[4];

    /// \brief Flags used to identify package with a partner blend from index that is invalid.
    bool blendFromValid[4];

    /// @}

    /// \name Package sequence hierarchy information
    /// @{

    /// \brief The package index for the child package in this sequence, or -1 to indicate this
    /// is the end package in the sequence.
    int32_t childPackage;

    /// \brief The package index for the parent package in this sequence, or -1 to indicate this
    /// is the root package in the sequence.
    int32_t parentPackage;

    /// \brief The package index of the end package for this sequence.
    int32_t endPackage;

    /// \brief The package index of the root package for this sequence.
    int32_t rootPackage;

    /// @}

    /// \name Package flags
    /// @{

    /// \brief Identifies the packages which have only 1-to-1 mappings and so the same package index between
    /// source and target
    bool hasDirectMapping;

    /// \brief Mark this package as having all unit weights so no weighting needs to be done.
    bool allUnitRotationWeights;

    /// \brief Mark this package as having all unit weights so no weighting needs to be done.
    bool allUnitPositionWeights;

    /// @}
  };

  /// \brief Container for VPU multiplexed transforms.
  struct MPTransform
  {
    /// \brief Translation
    NMP::vpu::Vector3MP t;
    /// \brief Rotation
    NMP::vpu::QuatMP q;
  };

  /// \brief Container for VPU multiplexed geometrical parameters.
  struct MPGeometry
  {
    /// \brief Joint offset transforms.
    MPTransform offset;

    /// \brief Joint reference transforms.
    MPTransform reference;

    /// \brief Parent reference transforms.
    MPTransform parentReference;

    /// \brief Pre-packed valid mask (packaged version of the valid flags in the mapping)
    vector4 validMask;
  };

  /// \brief Container to keep mirroring parameters tidy
  struct MPMirrorMasks
  {
    vector4 mirrorXRotation;
    vector4 mirrorYRotation;
    vector4 mirrorZRotation;
    vector4 mirrorXTranslation;
    vector4 mirrorYTranslation;
    vector4 mirrorZTranslation;
  };

  /// \brief Container for VPU multiplexed parameters for other purposes.
  struct MPParams
  {
    /// \name Joint limits (vectors)
    /// @{

    /// \brief The target joint limits for this package.
    JointLimitsMP::Params limits;

    /// \brief Says whether or not to apply limits to the members of this package.
    vector4 applyLimitsMask;

    /// @}

    /// \name Mirroring masks - allows simple and direct application of mirroring.
    /// @{

    MPMirrorMasks sourceMirrorMasks;
    MPMirrorMasks targetMirrorMasks;

    /// @}

    /// \brief Whether or not to redistribute clamped-off motion for this package.
    vector4 redistributeMask;

    /// \brief For target rigs, whether or not the bone translation is retargeted.
    vector4 isRigidMask;

    /// \brief Flag for saying whether there are any limits in this package, for early outs.
    bool isLimited;

    /// \brief Flag for saying whether we're redistributing motion in this package, for early outs.
    bool doRedistribution;
  };

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Variables, scratch memory etc
  /// @{

  /// \brief Local store of the source rig scale as a vector
  vector4 m_sourceInverseRigScale;

  /// \brief Local store of the target rig scale as a vector
  vector4 m_targetRigScale;

  /// \brief Set on initialisation, can only be calculated from all possible combinations
  /// of source and target rig.
  uint32_t m_intermediateRigSize;

  /// \brief Set on initialisation, gives the length of the working data transform buffers.
  uint32_t m_largestRigSize;

  /// \brief Allows copies by recording the memory requirements of this object.
  NMP::Memory::Format m_memoryRequirements;

  /// \brief Records how long our arrays of multiplex-packaged data need to be to process the
  /// most demanding rig expected to use this solver.
  uint32_t m_mpMaxLength;

  /// \brief Records how long our arrays of multiplex-packaged data need to be to process
  /// the source rig.
  uint32_t m_mpSourceLength;

  /// \brief Records how long our arrays of multiplex-packaged data need to be to process
  /// the target rig.
  uint32_t m_mpTargetLength;

  /// \brief Marks which source package has the first non 1-to-1 map, allowing us to skip them
  /// when processing n-to-m mappings without using conditionals.
  uint32_t m_mpFirstNToMPackageInSource;

  /// \brief Marks which target package has the first non 1-to-1 map, allowing us to skip them
  /// when processing n-to-m mappings without using conditionals.
  uint32_t m_mpFirstNToMPackageInTarget;

  /// \brief Tells us when we can skip all transform weighting because all the weights are 1.
  bool m_applyJointWeights;

  /// \brief Intermediate rig index map, maps back to source rig.
  IntermediateJointData* m_intermediateRigData;

  /// \brief Array of multiplexed mappings for the source rig
  MPIndexMap* m_mpSourceMappings;

  /// \brief Array of multiplexed mappings for the target rig
  MPIndexMap* m_mpTargetMappings;

  /// \brief Array of multiplexed source geometrical parameters
  MPGeometry* m_mpSourceGeom;

  /// \brief Array of multiplexed target geometrical parameters
  MPGeometry* m_mpTargetGeom;

  /// \brief Array of multiplexed parameters for other purposes
  MPParams* m_mpParams;

  /// @}

  //----------------------------------------------------------------------------------------------------------------------
  /// \name Utilities
  /// @{

  /// \brief Convert the source geometry into the source intermediate rig geometry
  void retargetSource(const NMP::DataBuffer* sourceTransforms, MPTransform* mpSourceTransforms);

  /// \brief For sequences of joints, accumulate the source transforms
  void accumulateSource(MPTransform* mpSourceTransforms);

  /// \brief For sequences of joints, unpack accumulated source transforms into local storage
  void unpackSource(NMP::DataBuffer* workingGeom, MPTransform* mpSourceTransforms);

  /// \brief For sequence of joints, calculate the target transforms by interpolating
  /// the source.
  void interpolateSequences(
    const NMP::DataBuffer* sourceTransforms,
    NMP::DataBuffer* targetTransforms,
    NMP::DataBuffer* workingGeom,
    MPTransform* mpSourceTransforms,
    MPTransform* mpTargetTransforms);

  /// \brief For sequence of joint, unaccumulate the target transforms
  void unaccumulateTarget(NMP::DataBuffer* targetTransforms, MPTransform* mpTargetTransforms);

  /// \brief Convert the target geometry into the target intermediate rig geometry
  void retargetTarget(
    NMP::DataBuffer* targetTransforms,
    MPTransform* mpSourceTransforms,
    MPTransform* mpTargetTransforms);

  /// \brief Set-up for VPU package index maps and sequence data
  void prepareMappings(
    const Params* params,
    const NMP::Hierarchy* hierarchy,
    const Params* otherParams,
    const NMP::Hierarchy* otherHierarchy,
    bool isSource);

  /// \brief Set-up for VPU packaged geometrical parameters
  void prepareGeom(
    const Params* params,
    bool isSource);

  /// \brief Set-up for VPU packaged 'other' parameters
  void prepareParams(const Params* sourceParams, const Params* targetParams);

  /// \brief Utility to centralise some serialisation code.
  void endianSwapMPData();

  /// \brief Calculate number of packages from storage statistics.
  static uint32_t calculateMaxNumPackages(const StorageStats& storageStats);

  /// @}

}; // end of class FKRetarget::Solver

//----------------------------------------------------------------------------------------------------------------------
// Utilities for the FKRetarget namespace

/// \brief Fills a whole transform buffer with worldspace data.  Offline only!
void accumulateTransforms(
  const NMP::DataBuffer* transformBuffer,
  NMP::DataBuffer* worldBuffer,
  const NMP::Hierarchy* hierarchy);

/// \brief Fills a whole transform buffer with worldspace data for the rig after application
/// of offsets.  Offline only!
void accumulateOffsetTransforms(
  const Params* params,
  const NMP::DataBuffer* transformBuffer,
  NMP::DataBuffer* worldBuffer,
  const NMP::Hierarchy* hierarchy);

/// \brief Fills a single transform with data in the frame of the given referenceJointIndex.
/// If referenceJointIndex is negative, returns the worldspace transform.
void accumulateTransform(
  NMRU::GeomUtils::PosQuat& transform,
  const NMP::Hierarchy* hierarchy,
  const NMP::DataBuffer* transformBuffer,
  uint32_t jointIndex,
  int32_t referenceJointIndex);

/// \brief Fills a single transform with data in the frame of the given referenceJointIndex,
/// with application of offsets.  If referenceJointIndex is negative, returns the worldspace
/// offset transform.
void accumulateOffsetTransform(
  const Params* params,
  NMRU::GeomUtils::PosQuat& transform,
  const NMP::Hierarchy* hierarchy,
  const NMP::DataBuffer* transformBuffer,
  uint32_t jointIndex,
  int32_t referenceJointIndex);

/// \brief Fills in unknown reference transforms based on putting the bone directions down the
/// y-axis, and fitting z to forwards or y to up where appropriate.  Offline only, especially
/// if worldTransforms not provided.
/// \param worldTransforms  If provided, is used for accumulating transforms more efficiently.
void approximateReferenceTransforms(
  Params* params,
  const NMP::DataBuffer* transforms,
  const NMP::Hierarchy* hierarchy,
  NMP::DataBuffer* worldTransforms = 0);

} // end of namespace FKRetarget

} // end of namespace NMRU

#include "NMFKRetarget.inl"

#endif // NMRU_FKRETARGET_H
