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
#include "morpheme/Nodes/mrNodeOperatorVector3Normalise.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorVector3NormaliseOutputCPUpdate(
  NodeDef* node,
  PinIndex outputCPPinIndex, // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex < 1);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  // First CP.
  AttribDataVector3* inputCPVector3 = net->updateInputCPConnection<AttribDataVector3>(node->getInputCPConnection(0), animSet);

  // Get the output attributes.
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPinResult = nodeBin->getOutputCPPin(0);
  AttribDataVector3* resultOutput = outputCPPinResult->getAttribData<AttribDataVector3>();
  
  // Perform the actual task
  resultOutput->m_value = NMP::vNormalise(inputCPVector3->m_value);

  // The requested output pin will automatically be flagged as having been updated this frame,
  // but we need to make sure that all pins we have updated are correctly flagged.
  outputCPPinResult->m_lastUpdateFrame = net->getCurrentFrameNo();
    
  // Return the value of the requested output cp pin.
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
