//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_NODE_EMIT_MESSAGE_ON_DISCRETE_EVENT_BUILDER_H
#define MR_NODE_EMIT_MESSAGE_ON_DISCRETE_EVENT_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeEmitMessageOnDiscreteEventBuilder
/// \brief For construction of EmitMessageOnDiscreteEvent Node.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeEmitMessageOnDiscreteEventBuilder : public NodeDefBuilder
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
  // Initialise as correct NodeDef type; fills in queuing ops table and outputCP ops table.
  void initTaskQueuingFns(
    MR::NodeDef*               nodeDef,
    NetworkDefCompilationInfo* netDefCompilationInfo,
    NMP::BasicLogger*          logger,
    uint32_t                   numMessageSlots);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_EMIT_MESSAGE_ON_DISCRETE_EVENT_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
