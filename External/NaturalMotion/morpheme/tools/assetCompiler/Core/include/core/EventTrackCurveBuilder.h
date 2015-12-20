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
#ifndef MR_EVENT_TRACK_CURVE_BUILDER_H
#define MR_EVENT_TRACK_CURVE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMMemory.h"
#include "assetProcessor/AssetProcessor.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrEventTrackCurve.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup EventTrackDefAssetProcessorModule EventTrackDef Asset Processor.
/// \ingroup AssetProcessorModule
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::EventTrackDefCurveBuilder
/// \brief For construction of an EventTrackDefCurve from an EventTrackCurve Description.
/// \ingroup EventTrackDefAssetProcessorModule
/// \see MR::EventTrackDefCurve
//----------------------------------------------------------------------------------------------------------------------
class EventTrackDefCurveBuilder : public MR::EventTrackDefCurve
{
public:
  static NMP::Memory::Format getMemoryRequirements(
    AP::AssetProcessor*              processor,
    const ME::CurveEventTrackExport* eventTrackExport);

  static NMP::Memory::Format getMemoryRequirements(
    uint32_t    numEvents,
    const char* name);

  static NMP::Memory::Resource init(
    AP::AssetProcessor*              processor,
    const ME::CurveEventTrackExport* eventTrackExport);

  // Find the requirements of an instance of this track type.
  NMP::Memory::Format getInstanceMemoryRequirements();
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_EVENT_TRACK_CURVE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
