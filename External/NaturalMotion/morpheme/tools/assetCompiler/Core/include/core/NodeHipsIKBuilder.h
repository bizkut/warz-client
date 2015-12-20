// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_NODE_HIPS_I_K_BUILDER_H
#define MR_NODE_HIPS_I_K_BUILDER_H

//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeBuilder.h"

//----------------------------------------------------------------------------------------------------------------------
namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeHipsIKBuilder
/// \brief For construction of HipsIK Node.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeHipsIKBuilder : public NodeDefBuilder
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
    uint32_t&                  numberOfInputControlParams,
    uint8_t&                   numberOfOutputControlParams);

  void initTaskQueuingFns(
    MR::NodeDef*                nodeDef,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    NMP::BasicLogger*           logger);
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_HIPS_I_K_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
