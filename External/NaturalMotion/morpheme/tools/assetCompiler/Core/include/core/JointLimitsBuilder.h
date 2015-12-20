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
#ifndef MR_JOINT_LIMITS_BUILDER_H
#define MR_JOINT_LIMITS_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "assetProcessor/NodeBuilder.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrManager.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
/// \class AP::JointLimitsBuilder
/// \brief For construction of kinematic joint limits AttribData.
//----------------------------------------------------------------------------------------------------------------------
class JointLimitsBuilder
{
public:
  /// \brief Initializes the (pre-allocated) joint limits attribute with the data from the rig.
  static void init(
    MR::AttribDataJointLimits* limitsAttrib,
    const ME::RigExport* rigExport);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_JOINT_LIMITS_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
