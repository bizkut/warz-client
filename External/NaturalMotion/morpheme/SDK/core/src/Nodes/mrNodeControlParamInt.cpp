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
#include "morpheme/Nodes/mrNodeControlParamInt.h"
#include "morpheme/Nodes/mrEmittedControlParamNodeUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeControlParamIntEmittedCPUpdateInt(
  NodeDef* node,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);

  // Control param attrib data is created on network init and is never deleted, so it should always be there.
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  AttribDataInt* outputCP = outputCPPin->getAttribData<AttribDataInt>();

  // Find an active input path and if there is one update it to get the result.
  AttribDataInt* inputCP = updateEmittedControlParamInputCPConnection<AttribDataInt>(node, net);

  if (!inputCP)
  {
    return outputCP;
  }

  outputCP->m_value = inputCP->m_value;

  return outputCP;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeControlParamIntInitInstance(NodeDef* node, Network* net)
{
  NMP_ASSERT(node->getNodeTypeID() == NODE_TYPE_CP_INT);

  // Control param attribute data does not exist in the Network, make sure it exists in the NetworkDef,
  // then make a new instance of it in the Network that will take priority over the Def instance.
  // Copy the def content into the new instance.
  AttribDataInt* defAttribData = node->getAttribData<AttribDataInt>(ATTRIB_SEMANTIC_CP_INT);
  
  // Create a new attribute and add it to the only output control param pin of this node.
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataInt::create(net->getPersistentMemoryAllocator(), defAttribData->m_value);
  outputCPPin->m_lastUpdateFrame = VALID_FOREVER;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
