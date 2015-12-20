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
#ifndef MR_NODE_TRANSIT_BASE_BUILDER_H
#define MR_NODE_TRANSIT_BASE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeBuilder.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeTransitBaseBuilder
/// \brief To aid the construction of any transit nodes. Provides utilities.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeTransitBaseBuilder : public NodeDefBuilder
{
protected:
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

protected:
  NMP::Memory::Format getNodeTransitMemReqs(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor);

  MR::NodeDef* initNodeTransit(
    NMP::Memory::Resource&      memRes,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor,
    MR::AttribDataTransitDef**  transitAttribDataRef);

  NMP::Memory::Format getNodeTransitSyncEventsMemReqs(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor);

  MR::NodeDef* initNodeTransitSyncEvents(
    NMP::Memory::Resource&      memRes,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor,
    MR::AttribDataTransitSyncEventsDef**  transitAttribDataRef);

  /// \brief Gets the memory requirements for the transit to sub state init data.
  NMP::Memory::Format getTransitSubStateMemoryRequirements(const ME::NodeExport* nodeDefExport);

  /// \brief Sets the transit to sub state initialization data.
  void initTransitSubStateParams(
    NMP::Memory::Resource&      memRes,
    const ME::NodeExport*       nodeDefExport,
    MR::NodeInitDataArrayDef**  nodeInitData);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_TRANSIT_BASE_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
