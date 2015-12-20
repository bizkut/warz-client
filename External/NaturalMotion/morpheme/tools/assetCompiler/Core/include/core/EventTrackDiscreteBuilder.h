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
#ifndef MR_EVENT_TRACK_DISCRETE_BUILDER_H
#define MR_EVENT_TRACK_DISCRETE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"

#include "assetProcessor/AssetProcessor.h"

#include "morpheme/mrManager.h"
#include "morpheme/mrEventTrackDiscrete.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::EventTrackDefDiscreteBuilder
/// \brief For construction of an EventTrackDefDiscrete from an EventTrackDiscrete Description.
/// \ingroup EventTrackDefAssetProcessorModule
/// \see MR::EventTrackDefDiscrete
//----------------------------------------------------------------------------------------------------------------------
class EventTrackDefDiscreteBuilder : public MR::EventTrackDefDiscrete
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    AP::AssetProcessor*                 processor,
    const ME::DiscreteEventTrackExport* eventTrackExport);

  static NMP::Memory::Format getMemoryRequirements(
    uint32_t    numEvents,
    const char* name);

  static NMP::Memory::Resource init(
    AP::AssetProcessor*                 processor,
    const ME::DiscreteEventTrackExport* eventTrackExport);

  static MR::EventTrackDefDiscrete* initEmpty(
    const NMP::Memory::Resource* desc,
    uint32_t                     numEvents,
    const char*                  name);

  // Find the requirements of an instance of this track type.
  NMP::Memory::Format getInstanceMemoryRequirements();
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_EVENT_TRACK_DISCRETE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
