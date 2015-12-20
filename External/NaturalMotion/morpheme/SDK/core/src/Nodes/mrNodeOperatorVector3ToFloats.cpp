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
#include "morpheme/Nodes/mrNodeOperatorVector3ToFloats.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorVector3ToFloatsOutputCPUpdate(
    NodeDef* node,
    PinIndex outputCPPinIndex, // The output pin we have been asked to update.
    Network* net)
{
  NMP_ASSERT(outputCPPinIndex < NODE_OPERATOR_VECTOR3_TO_FLOATS_OUT_CP_PINID_COUNT);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  AttribDataVector3* inputCPVector3 = net->updateInputCPConnection<AttribDataVector3>(node->getInputCPConnection(0), animSet);

  // Get the output attributes,
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPinX = nodeBin->getOutputCPPin(NODE_OPERATOR_VECTOR3_TO_FLOATS_OUT_CP_PINID_X);
  OutputCPPin *outputCPPinY = nodeBin->getOutputCPPin(NODE_OPERATOR_VECTOR3_TO_FLOATS_OUT_CP_PINID_Y);
  OutputCPPin *outputCPPinZ = nodeBin->getOutputCPPin(NODE_OPERATOR_VECTOR3_TO_FLOATS_OUT_CP_PINID_Z);

  AttribDataFloat* xOutput = outputCPPinX->getAttribData<AttribDataFloat>();
  AttribDataFloat* yOutput = outputCPPinY->getAttribData<AttribDataFloat>();
  AttribDataFloat* zOutput = outputCPPinZ->getAttribData<AttribDataFloat>();

  // Perform the actual task
  xOutput->m_value = inputCPVector3->m_value.x;
  yOutput->m_value = inputCPVector3->m_value.y;
  zOutput->m_value = inputCPVector3->m_value.z;

  // The requested output pin will automatically be flagged as having been updated this frame,
  // but we need to make sure that all pins we have updated are correctly flagged.
  outputCPPinX->m_lastUpdateFrame = net->getCurrentFrameNo();
  outputCPPinY->m_lastUpdateFrame = net->getCurrentFrameNo();
  outputCPPinZ->m_lastUpdateFrame = net->getCurrentFrameNo();
  
  // Return the value of the requested output cp pin.
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorVector3ToFloatsInitInstance(NodeDef* node, Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPinX = nodeBin->getOutputCPPin(NODE_OPERATOR_VECTOR3_TO_FLOATS_OUT_CP_PINID_X);
  OutputCPPin *outputCPPinY = nodeBin->getOutputCPPin(NODE_OPERATOR_VECTOR3_TO_FLOATS_OUT_CP_PINID_Y);
  OutputCPPin *outputCPPinZ = nodeBin->getOutputCPPin(NODE_OPERATOR_VECTOR3_TO_FLOATS_OUT_CP_PINID_Z);
  outputCPPinX->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
  outputCPPinY->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
  outputCPPinZ->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
  if(node->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(node, net);
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
