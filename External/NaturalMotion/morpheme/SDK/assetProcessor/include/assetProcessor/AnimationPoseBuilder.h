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
#ifndef MR_ANIMATION_POSE_BUILDER_H
#define MR_ANIMATION_POSE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrManager.h"
#include "assetProcessor/AssetProcessor.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnimationPoseBuilder
/// \brief For construction of animation poses.
//----------------------------------------------------------------------------------------------------------------------
class AnimationPoseBuilder
{
public:
  static void constructPose(
    NMP::Memory::Resource&            memRes,
    MR::AttribDataHandle*             netDefAttribDataArray,
    uint32_t                          NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*          netDefSemanticLookupTable,
    MR::AnimSetIndex                  animSetIndex,
    AP::AssetProcessor*               processor,
    const ME::AnimationSetExport*     animSetExport,
    const ME::AnimationPoseDefExport* sourcePose,
    MR::AttribDataSemantic            semantic);

  static NMP::Memory::Format getMemoryRequirements(
    const ME::AnimationSetExport*     animSetExport,
    const ME::AnimationPoseDefExport* pose);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIMATION_POSE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
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
#ifndef MR_ANIMATION_POSE_BUILDER_H
#define MR_ANIMATION_POSE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrManager.h"
#include "assetProcessor/AssetProcessor.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnimationPoseBuilder
/// \brief For construction of animation poses.
//----------------------------------------------------------------------------------------------------------------------
class AnimationPoseBuilder
{
public:
  static void constructPose(
    NMP::Memory::Resource&            memRes,
    MR::AttribDataHandle*             netDefAttribDataArray,
    uint32_t                          NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*          netDefSemanticLookupTable,
    MR::AnimSetIndex                  animSetIndex,
    AP::AssetProcessor*               processor,
    const ME::AnimationSetExport*     animSetExport,
    const ME::AnimationPoseDefExport* sourcePose,
    MR::AttribDataSemantic            semantic);

  static NMP::Memory::Format getMemoryRequirements(
    const ME::AnimationSetExport*     animSetExport,
    const ME::AnimationPoseDefExport* pose);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIMATION_POSE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
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
#ifndef MR_ANIMATION_POSE_BUILDER_H
#define MR_ANIMATION_POSE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrManager.h"
#include "assetProcessor/AssetProcessor.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnimationPoseBuilder
/// \brief For construction of animation poses.
//----------------------------------------------------------------------------------------------------------------------
class AnimationPoseBuilder
{
public:
  static void constructPose(
    NMP::Memory::Resource&            memRes,
    MR::AttribDataHandle*             netDefAttribDataArray,
    uint32_t                          NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*          netDefSemanticLookupTable,
    MR::AnimSetIndex                  animSetIndex,
    AP::AssetProcessor*               processor,
    const ME::AnimationSetExport*     animSetExport,
    const ME::AnimationPoseDefExport* sourcePose,
    MR::AttribDataSemantic            semantic);

  static NMP::Memory::Format getMemoryRequirements(
    const ME::AnimationSetExport*     animSetExport,
    const ME::AnimationPoseDefExport* pose);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIMATION_POSE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
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
#ifndef MR_ANIMATION_POSE_BUILDER_H
#define MR_ANIMATION_POSE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrManager.h"
#include "assetProcessor/AssetProcessor.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::AnimationPoseBuilder
/// \brief For construction of animation poses.
//----------------------------------------------------------------------------------------------------------------------
class AnimationPoseBuilder
{
public:
  static void constructPose(
    NMP::Memory::Resource&            memRes,
    MR::AttribDataHandle*             netDefAttribDataArray,
    uint32_t                          NMP_USED_FOR_ASSERTS(netDefAttribDataArraySize),
    MR::SemanticLookupTable*          netDefSemanticLookupTable,
    MR::AnimSetIndex                  animSetIndex,
    AP::AssetProcessor*               processor,
    const ME::AnimationSetExport*     animSetExport,
    const ME::AnimationPoseDefExport* sourcePose,
    MR::AttribDataSemantic            semantic);

  static NMP::Memory::Format getMemoryRequirements(
    const ME::AnimationSetExport*     animSetExport,
    const ME::AnimationPoseDefExport* pose);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIMATION_POSE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
