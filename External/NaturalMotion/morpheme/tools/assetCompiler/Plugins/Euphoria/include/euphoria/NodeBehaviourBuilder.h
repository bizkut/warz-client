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
#ifndef MR_NODE_BEHAVIOUR_BUILDER_H
#define MR_NODE_BEHAVIOUR_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
#include "assetProcessor/NodeBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{
class AttribDataBehaviourParameters;
}

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
/// \class AP::NodeBehaviourBuilder
/// \brief For construction of behaviour nodes.
/// \ingroup
//----------------------------------------------------------------------------------------------------------------------
class NodeBehaviourBuilder : public NodeDefBuilder
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
  // Initialise as correct NodeDef type; fills in queuing ops table and immediate ops table.
  void initTaskQueuingFns(
    MR::NodeDef*                nodeDef,
    NetworkDefCompilationInfo*  netDefCompilationInfo,
    ER::AttribDataBehaviourParameters* behaviourParameters,
    NMP::BasicLogger*           logger,
    bool                        emitMessages,
    bool                        hasAnimationInputConnection);
};

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_BEHAVIOUR_BUILDER_H
//----------------------------------------------------------------------------------------------------------------------
