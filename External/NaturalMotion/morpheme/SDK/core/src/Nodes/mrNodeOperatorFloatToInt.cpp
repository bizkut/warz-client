//----------------------------------------------------------------------------------------------------------------------
// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/Nodes/mrNodeOperatorFloatToInt.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorFloatToIntRoundToNearestOutputCPUpdateInt(
  NodeDef* node,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  AttribDataFloat* inputCPFloat = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(0), animSet);
  
  // Get the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  AttribDataInt* outputCPInt = outputCPPin->getAttribData<AttribDataInt>();

  // Perform the actual task
  // MORPH-19893. Investigate optimisation.
  outputCPInt->m_value = static_cast<int32_t>( inputCPFloat->m_value + 0.5f );
  return outputCPInt;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorFloatToIntFloorOutputCPUpdateInt(
  NodeDef* node,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  AttribDataFloat* inputCPFloat = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(0), animSet);

  // Get the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  AttribDataInt* outputCPInt = outputCPPin->getAttribData<AttribDataInt>();

  // Perform the actual task
  // MORPH-19893. Investigate optimisation.
  outputCPInt->m_value = static_cast<int32_t>( inputCPFloat->m_value );
  return outputCPInt;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorFloatToIntCeilingOutputCPUpdateInt(
  NodeDef* node,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  AttribDataFloat* inputCPFloat = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(0), animSet);

  // Get the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  AttribDataInt* outputCPInt = outputCPPin->getAttribData<AttribDataInt>();

  // Perform the actual task
  // MORPH-19893. Investigate optimisation.
  outputCPInt->m_value = static_cast<int32_t>( NMP::nmceil( inputCPFloat->m_value ) );
  return outputCPInt;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
