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
#include "assetProcessor/TransitDeadBlendDefBuilder.h"
#include "assetProcessor/NodeBuilderUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool autoAddDeadBlendDef(ME::DataBlockExport* datablock, MR::NodeID sourceNodeID)
{  
  MR::NodeID destNodeID;
  readNodeID(datablock, "SourceNodeID", sourceNodeID, true);
  readNodeID(datablock, "DestNodeID", destNodeID, true);

  bool deadblendBreakoutToSource = false;
  datablock->readBool(deadblendBreakoutToSource, "DeadblendBreakoutToSource");

  //     has check box ticked          explicit self-transition        wildcard self-transition
  return deadblendBreakoutToSource || (sourceNodeID == destNodeID) || (sourceNodeID == MR::INVALID_NODE_ID);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format getAttribDataDeadBlendDefMemoryRequirements(
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP::Memory::Format result(0, NMP_VECTOR_ALIGNMENT);

  TransitDeadBlendDefBuilder* deadBlendBuilder = processor->getTransitDeadBlendDefBuilder(TRANSDEADBLEND_DEFAULT_ID);
  if (!deadBlendBuilder)
  {
    processor->outputDeadBlendError(TRANSDEADBLEND_DEFAULT_ID, "Cannot find dead blend builder.");
  }
  NMP_VERIFY(deadBlendBuilder);
  result += deadBlendBuilder->getNodeDefMemoryRequirements(nodeDefExport, netDefExport, processor);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribData* initAttribDataDeadBlendDef(
  NMP::Memory::Resource&      memRes,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  TransitDeadBlendDefBuilder* deadBlendBuilder = processor->getTransitDeadBlendDefBuilder(TRANSDEADBLEND_DEFAULT_ID);
  if (!deadBlendBuilder)
  {
    processor->outputDeadBlendError(TRANSDEADBLEND_DEFAULT_ID, "Cannot find dead blend builder.");
  }
  NMP_VERIFY(deadBlendBuilder);
  return deadBlendBuilder->initAttribData(memRes, nodeDefExport, netDefExport, processor);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
