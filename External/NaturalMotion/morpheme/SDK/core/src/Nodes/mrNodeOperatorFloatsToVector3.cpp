//----------------------------------------------------------------------------------------------------------------------
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
#include "morpheme/Nodes/mrNodeOperatorFloatsToVector3.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorFloatsToVector3OutputCPUpdateVector3(
  NodeDef* node,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  AttribDataFloat* inputCPFloatX = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(0), animSet);
  AttribDataFloat* inputCPFloatY = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(1), animSet);
  AttribDataFloat* inputCPFloatZ = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(2), animSet);

  // Get the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  AttribDataVector3* outputCPVector3 = outputCPPin->getAttribData<AttribDataVector3>();

  // Perform the actual task
  outputCPVector3->m_value.set(inputCPFloatX->m_value, inputCPFloatY->m_value, inputCPFloatZ->m_value);

  return outputCPVector3;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
