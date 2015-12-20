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
#ifndef MR_NODE_ANIM_WITH_EVENTS_BUILDER_H
#define MR_NODE_ANIM_WITH_EVENTS_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeAnimWithEventsBuilder
/// \brief For construction of NodeAnimWithEvents.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeAnimWithEventsBuilder : public NodeDefBuilder
{
public:
  virtual bool hasSemanticLookupTable() NM_OVERRIDE;

  virtual MR::SemanticLookupTable* initSemanticLookupTable(
    NMP::Memory::Resource&      memRes) NM_OVERRIDE;

  virtual void getNodeDefInputConnections(
    std::vector<MR::NodeID>&    childNodeIDs,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) NM_OVERRIDE;

  virtual void preInit(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) NM_OVERRIDE;

  virtual NMP::Memory::Format getNodeDefMemoryRequirements(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) NM_OVERRIDE;

  virtual MR::NodeDef* init(
    NMP::Memory::Resource&      memRes,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) NM_OVERRIDE;

  enum ClipRangeModes
  {
    kMarkerRange = 1,
    kEntireRange,
    kCustomRange,
    kInvalidClipRangeMode = 0xFFFFFFFF
  };

private:
  // Initialise as correct NodeDef type; fills in queuing ops table and outputCP ops table.
  void initTaskQueuingFns(
    MR::NodeDef*                nodeDef,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    NMP::BasicLogger*           logger,
    bool                        preComputeSyncEventTracks);

  /// If anim node is non-looping the clip section must start on a sync event boundary.
  /// i.e. cannot include any trailing event section at the section start.
  /// This function trims the clip start to the next nearest sync event start if necessary.
  void trimClipSectionWhenNotLooping(
    bool                         looping,
    float&                       clipStartFraction,
    float&                       clipEndFraction,
    const MR::EventTrackDefBase* eventTrack);

  /// Find out the event track counts from the source XML.
  void calculateEventTrackArraySizes(
    uint32_t&                   numDiscreteEventTracks,
    uint32_t&                   numDurationEventTracks,
    uint32_t&                   numCurveEventTracks,
    uint32_t&                   totalNumEventTracks,
    AssetProcessor*             processor,
    ME::AnimationEntryExport*   animEntry);

  /// Create and init the anim source nodes event track assets.
  void initEventTrackAttribs(
    AssetProcessor*                 processor,
    const ME::NetworkDefExport*     netDefExport,
    ME::AnimationEntryExport*       animEntry,
    MR::NodeDef*                    nodeDef,
    NMP::Memory::Resource&          memRes,
    bool                            loopVal,
    float&                          clipStartFraction,
    float&                          clipEndFraction,
    MR::AnimSetIndex                animSetIndex,
    MR::AttribDataSourceAnim*       sourceAnim,
    MR::EventTrackDefDiscrete**     syncTracksSourceDiscreteTrack);

  ///
  static const MR::EventTrackDefBase* createDefaultDiscreteEventTrack(AssetProcessor* processor, const ME::NetworkDefExport* netDefExport);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_ANIM_WITH_EVENTS_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
