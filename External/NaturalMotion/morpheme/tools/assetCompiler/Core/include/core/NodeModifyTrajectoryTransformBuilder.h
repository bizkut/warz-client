//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_NODE_MODIFY_TRAJECTORY_TRANSFORM_BUILDER_H
#define MR_NODE_MODIFY_TRAJECTORY_TRANSFORM_BUILDER_H

//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeBuilder.h"

//----------------------------------------------------------------------------------------------------------------------
namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeModifyTrajectoryTransformBuilder
/// \brief For construction of SetTransorm Node.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeModifyTrajectoryTransformBuilder : public NodeDefBuilder
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

  virtual NMP::Memory::Format getNodeDefMemoryRequirements(
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    const ME::NodeExport*       nodeDefExport,
    const ME::NetworkDefExport* netDefExport,
    AssetProcessor*             processor) NM_OVERRIDE;

  virtual void preInit(
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
  void calculateNodeDefMetrics(
    const ME::DataBlockExport* nodeDefDatablock,
    uint32_t&                  numberOfChildren,
    uint32_t&                  numberOfControlParams,
    uint8_t& numberOfOutputControlParams);

  void initTaskQueuingFns(
    MR::NodeDef*               nodeDef,
    NetworkDefCompilationInfo* netDefCompilationInfo,
    NMP::BasicLogger*          logger);
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_MODIFY_TRAJECTORY_TRANSFORM_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
