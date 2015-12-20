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
#ifndef MR_RIGBUILDER_H
#define MR_RIGBUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"

#include "assetProcessor/AssetProcessor.h"

#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup RigAssetProcessorModule Rig Asset Processor.
/// \ingroup AssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::RigBuilder
/// \brief For construction of a Rig from a Rig Description.
/// \ingroup RigAssetProcessorModule
/// \see MR::Rig
//----------------------------------------------------------------------------------------------------------------------
class AnimRigDefBuilder : public MR::AnimRigDef
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    AP::AssetProcessor*       processor,
    const ME::RigExport*      rigExport,
    std::vector<const char*>& boneNames, ///< Out: entries are allocated in this function.
                                         ///< Caller must deallocate these names.
    uint32_t&                 tableSize  ///< Required size of string table.
  );

  static NMP::Memory::Resource init(
    AP::AssetProcessor*  processor,
    const ME::RigExport* rigExport);

  static NMP::Memory::Format getMemoryRequirements(
    const std::vector<const char*>& boneNames,
    uint32_t&                       tableSize);

  static NMP::Memory::Resource createRigDef(
    const std::vector<const char*>&           rigBoneNames, ///< IN: Conglomerated rig bone names in simple hierarchy order
    const std::vector<const MR::AnimRigDef*>& rigs          ///< IN: The set of rigs from which to compile the super set rig from
  );

  /// \brief Examine the parental bone hierarchy of both rigs to determine if the topology is identical with
  /// respect to the character root and trajectory bones. i.e. check that the bone names from the character
  /// root to the world root are the same in both rigs; also check that the bone names from the trajectory to
  /// the world root are the same in both rigs.
  ///
  /// Note: The bone indexes do not need to be identical, its the hierarchy that we are checking.
  /// We also check to see if the blend frame transforms are identical.
  /// TODO: To truly validate this we should be checking that the rig bind pose topology of these bones are the same.
  /// We will do this later.
  static bool haveIdenticalTrajectoryComponents(const AnimRigDef* rig0, const AnimRigDef* rig1);

  /// \brief Check if the supplied bone name if one of the trajectory components of this Rig.
  ///
  /// Trajectory components are those parts of the rig from the Trajectory Bone up the hierarchy and
  /// the Character Root Bone up the hierarchy.
  /// \return Index of bone if found or INVALID_BONE_INDEX if the bone is not part of the starting bones parent hierarchy.
  static uint32_t isOneOfTheTrajectoryComponents(
    const AnimRigDef* rigToSearch,
    const char*       boneNameToSearchFor);

  /// \brief
  static NMP::Memory::Format getBindPoseMemoryRequirements(uint32_t numChannels);
  
  /// \brief
  static void initBindPose(
    NMP::Memory::Resource&  memRes,
    AP::AssetProcessor*     processor,
    const ME::RigExport*    rigExport,
    AnimRigDefBuilder*      animRigDef);

private:
  /// \brief Validate if the 2 parental hierarchies from the specified bones are identical.
  ///
  /// Note: The bone indexes do not need to be identical, its the hierarchy that we are checking.
  static bool haveIdenticalParentHierarchiesFromStartingBoneIndexes(
    const AnimRigDef* rig0,
    uint32_t          rig0StartBoneIndex,
    const AnimRigDef* rig1,
    uint32_t          rig1StartBoneIndex);

  /// \brief Search up the parental hierarchy tree from the specified bone index trying to find the specified bone.
  /// \return Index of bone if found or INVALID_BONE_INDEX if the bone is not part of the starting bones parent hierarchy.
  static uint32_t searchBoneIsInParentHierarchyOfStartingBone(
    const AnimRigDef* rigToSearch,
    uint32_t          rigStartBoneIndex,
    const char*       boneNameToSearchFor);

  /// \brief Determines if the blend frame transform in both rigs is the same
  static bool haveIdenticalBlendFrameTransforms(
    const AnimRigDef* rig0,
    const AnimRigDef* rig1);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_RIGBUILDER_H
//----------------------------------------------------------------------------------------------------------------------
