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
#include "morpheme/Nodes/mrNodeOperatorRampFloat.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// See NodeOperatorRampFloatBuilder::init for the array of attributes
AttribData* nodeOperatorRampFloatOutputCPUpdateFloat(
  NodeDef* node,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  //-------------------
  // Update the connected control parameters.
  AttribDataFloat* inputCPFloat = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(0), animSet);
  float rateMultiplier = inputCPFloat->m_value;

  //-------------------
  // Get the output attribute data
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  AttribDataFloat* outputCPFloat = outputCPPin->getAttribData<AttribDataFloat>();

  //-------------------
  // Node defs initial value
  AttribDataFloatArray* valuesAttrib = node->getAttribData<AttribDataFloatArray>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  //-------------------
  // Node state - the internal float value.
  AttribDataFloat* attribFloatState = net->getAttribData<AttribDataFloat>(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID());
  float& val = attribFloatState->m_value;

  // If the data we have got is not from the last update frame initialise the cached value directly
  // to the initial value. Don't actually ramp on the first update, as people will expect to see the
  // initial value on the first output.
  const uint32_t currFrameNo = net->getCurrentFrameNo();
  if (outputCPPin->m_lastUpdateFrame != (currFrameNo - 1))
  {
    val = valuesAttrib->m_values[0];
  }
  else
  {
    //-------------------
    // Perform the actual task

    // Networks update time.
    AttribDataUpdatePlaybackPos* netUpdateTimeAttrib = net->getAttribData<AttribDataUpdatePlaybackPos>(
                                                                   ATTRIB_SEMANTIC_UPDATE_TIME_POS,
                                                                   NETWORK_NODE_ID,
                                                                   INVALID_NODE_ID,
                                                                   currFrameNo,
                                                                   0);

    // update the output
    float rate = valuesAttrib->m_values[1] * rateMultiplier;
    float timeDelta = 0.0f;
    if (!netUpdateTimeAttrib->m_isAbs)
      timeDelta = netUpdateTimeAttrib->m_value;
    val += rate * timeDelta;
  }

  // Clamp the output? 
  // Connect - upon serialisation of this node will set the minimum value > maximum value 
  // if clamping is not enabled so we will check this here.
  if (valuesAttrib->m_values[2] <= valuesAttrib->m_values[3])
  {
    outputCPFloat->m_value = NMP::clampValue(val, valuesAttrib->m_values[2], valuesAttrib->m_values[3]);
  }
  else
  {
    outputCPFloat->m_value = val;
  }

  return outputCPFloat;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorRampFloatInitInstance( NodeDef* node, Network* net )
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);

  // Create state data.
  AttribDataHandle handle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
  AttribAddress stateAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);
  net->addAttribData(stateAddress, handle, LIFESPAN_FOREVER);

  // Workaround to get the node to initialise its output to its input on
  // the first frame. Can't use -1 as this is VALID_FOREVER
  outputCPPin->m_lastUpdateFrame = (FrameCount) -2;

  if(node->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(node, net);
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
