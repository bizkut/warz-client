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
#ifndef TRAJECTORY_SOURCE_COMPRESSOR_ASA_H
#define TRAJECTORY_SOURCE_COMPRESSOR_ASA_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/AnimSource/AnimSourceCompressorASA.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
class AnimSourceCompressorASA;

//----------------------------------------------------------------------------------------------------------------------
// TrajectorySourceASABuilder
//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceASABuilder : public MR::TrajectorySourceASA
{
public:
  /// \brief Function to compute the required memory for the trajectory source
  static NMP::Memory::Format getMemoryRequirements(
    const AnimSourceUncompressed* inputAnim);

  /// \brief Function to build the trajectory source within the specified memory buffer
  static void init(
    const AnimSourceUncompressed* inputAnim,
    MR::TrajectorySourceASA*      trajectorySource,
    const NMP::Memory::Format&    memoryReqs);
};

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::TrajectorySourceCompressorASA
/// \brief This class is responsible for compiling a trajectory source in the ASA format.
/// \ingroup CompressedAnimationAssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------
class TrajectorySourceCompressorASA
{
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // TRAJECTORY_SOURCE_COMPRESSOR_ASA_H
//----------------------------------------------------------------------------------------------------------------------
