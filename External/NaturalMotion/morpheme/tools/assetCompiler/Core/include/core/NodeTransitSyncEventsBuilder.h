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
#ifndef MR_NODE_TRANSIT_SYNC_EVENTS_BUILDER_H
#define MR_NODE_TRANSIT_SYNC_EVENTS_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeTransitBaseBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeTransitSyncEventsBuilder
/// \brief For construction of NodeTransitSyncEvents.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeTransitSyncEventsBuilder : public NodeTransitBaseBuilder
{
public:
  virtual bool hasSemanticLookupTable() NM_OVERRIDE;

  virtual MR::SemanticLookupTable* initSemanticLookupTable(
    NMP::Memory::Resource&      memRes) NM_OVERRIDE;

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

private:
  // Initialise as correct NodeDef type; fills in queuing ops table and outputCP ops table.
  void initTaskQueuingFns(
    MR::NodeDef*                nodeDef,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    MR::AttribDataTransitSyncEventsDef* transitAttribData,
    const ME::DataBlockExport*  nodeDefDatablock,
    NMP::BasicLogger*           logger);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_TRANSIT_SYNC_EVENTS_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
