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
#ifndef ANIM_SOURCE_PREPROCESSOR_H
#define ANIM_SOURCE_PREPROCESSOR_H
//----------------------------------------------------------------------------------------------------------------------
#include "XMD/Model.h"
#include "XMD/AnimCycle.h"
#include "XMD/AnimationTake.h"
#include "assetProcessor/acAnimInfo.h"
#include "assetProcessor/AnimSource/AnimSourceUncompressed.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::ChannelSetTableBuilderXMD
/// \brief Builds a ChannelSetTable from the XMD animation keyframe data.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class ChannelSetTableBuilderXMD : public ChannelSetTable
{
public:
  /// \brief Function for calculating the maximum number of position or rotation keyframes
  /// within the animation.
  static uint32_t calculateMaxNumKeyFrames(
    const XMD::XModel&           xmdAnims,      ///< IN: XMD anim structure containing the take we want to process.
    const XMD::XAnimCycle&       xmdCycle,      ///< IN: The take/cycle from this animation that we are interested in.
    const std::vector<XMD::XId>& animBoneIDs    ///< IN: The bone IDs that we are interested in.
  );

  /// \brief Function for calculating the maximum number of position or rotation keyframes
  /// within the animation.
  static uint32_t calculateMaxNumKeyFrames(
    const XMD::XModel&           xmdAnims,      ///< IN: XMD anim structure containing the take we want to process.
    const XMD::XAnimationTake&   xmdCycle,      ///< IN: The take/cycle from this animation that we are interested in.
    const std::vector<XMD::XId>& animBoneIDs    ///< IN: The bone IDs that we are interested in.
  );

  /// \brief Function to build the uncompressed quaternion channel keyframe data.
  static void initChannelQuat(
    uint32_t                     channelNumFrames,
    NMP::Quat*                   channelQuat,   ///< OUT: The channel set for the uncompressed quaternion data
    const XMD::XBone*            bone,          ///< IN: XMD Bone data
    const XMD::XSampledKeys*     keys           ///< IN: XMD Keyframe samples
  );

  /// \brief Function to build the uncompressed quaternion channel keyframe data.
  static void initChannelQuat(
    uint32_t                       channelNumFrames,
    NMP::Quat*                     channelQuat, ///< OUT: The channel set for the uncompressed quaternion data
    const acAnimInfo&              animInfo,    ///< IN: information about the animation take being extracted
    const XMD::XBone*              bone,        ///< IN: XMD Bone data
    const XMD::XAnimatedAttribute* keys         ///< IN: XMD Keyframe samples
  );

  /// \brief Function to build the uncompressed position channel keyframe data.
  static void initChannelPos(
    uint32_t                     channelNumFrames,
    NMP::Vector3*                channelPos,    ///< OUT: The channel set for the uncompressed position data
    const XMD::XBone*            bone,          ///< IN: XMD Bone data
    const XMD::XSampledKeys*     keys           ///< IN: XMD Keyframe samples
  );

  /// \brief Function to build the uncompressed position channel keyframe data.
  static void initChannelPos(
    uint32_t                       channelNumFrames,
    NMP::Vector3*                  channelPos,  ///< OUT: The channel set for the uncompressed position data
    const acAnimInfo&              animInfo,    ///< IN: information about the animation take being extracted
    const XMD::XBone*              bone,        ///< IN: XMD Bone data
    const XMD::XAnimatedAttribute* keys         ///< IN: XMD Keyframe samples
  );

  /// \brief Generate a ChannelSetTransformTable from the XMD animation data.
  static ChannelSetTable* init(
    NMP::Memory::Resource        memDesc,       ///< IN: Resource description of where to build the table
    const XMD::XModel&           xmdAnims,      ///< IN: XMD anim structure containing the take we want to process
    const acAnimInfo&            animInfo,      ///< IN: Information about the anim and take that we are interested in
    const std::vector<XMD::XId>& animBoneIDs,   ///< IN: IDs of the bones from this anim that we want to process
    float                        scale,
    const MR::AnimRigDef*        rig,
    const MR::RigToAnimEntryMap* rigToAnimEntryMap
  );
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::StringTableBuilderXMD
/// \brief Builds a channel name table from a XMD model and specified bone ids.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class StringTableBuilderXMD : public NMP::OrderedStringTable
{
public:
  static uint32_t getDataLength(
    const MR::AnimRigDef*        rig,
    const MR::RigToAnimEntryMap* rigToAnimEntryMap);

  /// \brief Calculate the memory required to create a OrderedStringTable from the supplied parameters.
  static NMP::Memory::Format getMemoryRequirements(
    const MR::AnimRigDef*        rig,
    const MR::RigToAnimEntryMap* rigToAnimEntryMap);

  /// \brief Generate a channel name table from the XMD model.
  static NMP::OrderedStringTable* init(
    NMP::Memory::Resource        resource,
    const MR::AnimRigDef*        rig,
    const MR::RigToAnimEntryMap* rigToAnimEntryMap);
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnimSourceUncompressedBuilder
/// \brief Converts an XMD animation to the basic uncompressed format required for morpheme runtime.
/// \ingroup UncompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class AnimSourceUncompressedBuilderXMD : public AnimSourceUncompressedBuilder
{
public:
  /// \brief Generate a correctly formatted animation for use with morpheme runtime from an XMD take/clip.
  static AnimSourceUncompressed* init(
    const XMD::XModel&           xmdAnims,                ///< IN: XMD anim structure containing the take we want to process.
    const acAnimInfo&            animInfo,                ///< IN: Information about the anim and take that we are interested in.
    const std::vector<XMD::XId>& animBoneIDs,             ///< IN: IDs of the bones from this anim that we want to process.
    const MR::AnimRigDef*        rig,                     ///< IN: Need for calculation of trajectory info.
    const MR::RigToAnimEntryMap* rigToAnimEntryMap,       ///< IN: Need for calculation of trajectory info.
    float                        scale,                   ///< IN: The amount to scale the translations by.
    const AnimSourceUncompressedOptions& animSourceOptions  ///< IN: The options used to compile the uncompressed animation
  );
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // ANIM_SOURCE_PREPROCESSOR_H
//----------------------------------------------------------------------------------------------------------------------
