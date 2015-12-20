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
#ifndef MR_NODE_FEATHER_BLEND_2_BUILDER_H
#define MR_NODE_FEATHER_BLEND_2_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeBuilder.h"
#include "assetProcessor/BlendNodeBuilderUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeFeatherBlend2Builder
/// \brief For construction of NodeFeatherBlend2.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeFeatherBlend2Builder : public NodeDefBuilder
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

private:
  void initTaskQueuingFnsUpdateConnections(
    MR::NodeDef*                nodeDef,
    NodeTimeStretchModes        timeStretchMode,
    bool                        isBlendWeightConnected,
    NMP::BasicLogger*           logger);

  void initTaskQueuingFnsEvents(
    MR::NodeDef*                nodeDef,
    NodeTimeStretchModes        timeStretchMode,
    NodePassThroughModes        passThroughMode,
    NodeSampledEventBlendModes  eventBlendMode,
    MR::QueueAttrTaskFn*        taskQueuingFns,
    NMP::BasicLogger*           logger);

  void initTaskQueuingFnsTransforms(
    MR::NodeDef*                nodeDef,
    NodeBlendModes              blendMode,
    MR::QueueAttrTaskFn*        taskQueuingFns,
    NMP::BasicLogger*           logger);

  void initTaskQueuingFnsTrajectoryDelta(
    MR::NodeDef*                nodeDef,
    NodeBlendModes              blendMode,
    bool                        slerpDeltaTrajectoryTranslation,
    MR::QueueAttrTaskFn*        taskQueuingFns,
    NMP::BasicLogger*           logger);

  void initTaskQueuingFnsTrajectoryDeltaAndTransforms(
    MR::NodeDef*                nodeDef,
    NodeBlendModes              blendMode,
    bool                        slerpDeltaTrajectoryTranslation,
    MR::QueueAttrTaskFn*        taskQueuingFns,
    NMP::BasicLogger*           logger);

  // Initialise as correct NodeDef type; fills in queuing ops table and outputCP ops table.
  void initTaskQueuingFns(
    MR::NodeDef*                nodeDef,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    NodeTimeStretchModes        timeStretchMode,
    NodePassThroughModes        passThroughMode,
    NodeBlendModes              blendMode,
    NodeSampledEventBlendModes  eventBlendMode,
    bool                        slerpDeltaTrajectoryTranslation,
    bool                        isBlendWeightConnected,
    NMP::BasicLogger*           logger);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_FEATHER_BLEND_2_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
