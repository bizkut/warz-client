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
# pragma once
#endif
#ifndef MR_RIG_RETARGET_MAPPING_BUILDER_H
#define MR_RIG_RETARGET_MAPPING_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetprocessor/NodeBuilder.h"
#include <morpheme/mrRigRetargetMapping.h>

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::RigRetargetMappingBuilder
/// \brief For construction of rig retarget mapping AttribData.
//----------------------------------------------------------------------------------------------------------------------
class RigRetargetMappingBuilder
{

public:

  /// \brief Initializes the (pre-allocated) rig retarget mapping attribute with the data from the rig.
  static void init(
      MR::AttribDataRigRetargetMapping* mapping,
      const ME::RigExport* rigExport,
      const ME::AnimationSetExport* animSetExport,
      const std::map<std::string, int32_t>& retargetMap);

  /// \brief Retrieves and records rig joint limits to the retarget mapping attribute data
  static void initJointLimits(
    MR::AttribDataRigRetargetMapping* mapping,
    const ME::RigExport* rigExport);

};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_RIG_RETARGET_MAPPING_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
