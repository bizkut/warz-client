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
#ifndef MR_MIRRORED_ANIM_MAPPING_BUILDER_H
#define MR_MIRRORED_ANIM_MAPPING_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "assetProcessor/NodeBuilder.h"
#include "morpheme/mrMirroredAnimMapping.h"
#include "morpheme/mrManager.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
/// \class AP::MirroredAnimMappingBuilder
/// \brief For construction of mirrored animation mapping AttribData.
//----------------------------------------------------------------------------------------------------------------------
class MirroredAnimMappingBuilder
{
public:
  /// \brief  initializes the (pre-allocated) mirrored anim mapping attribute with the data from the rig.
  ///         This builds up the rotation offsets and bone indices of the mapped bones that need to be mirrored.
  static void init(
    MR::AttribDataMirroredAnimMapping* mapping,
    const ME::RigExport*               rigExport,
    const ME::AnimationSetExport*      animSetExport);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_RIGBUILDER_H
//----------------------------------------------------------------------------------------------------------------------
