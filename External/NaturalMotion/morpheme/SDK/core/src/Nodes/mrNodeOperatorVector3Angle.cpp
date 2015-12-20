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
#include "morpheme/Nodes/mrNodeOperatorVector3Angle.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorVector3AngleOutputCPUpdate(
  NodeDef* node,
  PinIndex outputCPPinIndex, // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex < 1);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  // First CP.
  AttribDataVector3* inputCPVector3A = net->updateInputCPConnection<AttribDataVector3>(node->getInputCPConnection(0), animSet);
  // Second CP.
  AttribDataVector3* inputCPVector3B = net->updateInputCPConnection<AttribDataVector3>(node->getInputCPConnection(1), animSet);

  // Get the output attributes.
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPinResult = nodeBin->getOutputCPPin(0);
  AttribDataFloat* resultOutput = outputCPPinResult->getAttribData<AttribDataFloat>();

  //-------------------
  // Get the axis index from the attribute data
  AttribDataVector3* angleOp =
    node->getAttribData<AttribDataVector3>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  NMP::Vector3 axis = angleOp->m_value;
  NMP::Vector3 vecA = inputCPVector3A->m_value;
  NMP::Vector3 vecB = inputCPVector3B->m_value;
  NMP::Vector3 cross = NMP::vCross(vecA, vecB);
  float scaledSinAngle = 0.0f;
  float scaledCosAngle = 1.0f;

  if (axis.x==0.0 && axis.y==0.0 && axis.z==0.0) // Angle between any two vectors, must be positive if no axis specified
  {
    scaledSinAngle = cross.magnitude();
  }
  else  // We get the angle on a particular axis, which gives us a signed angle which is helpful
  {
    scaledSinAngle = cross.dot(axis);
    // Flatten the vectors to the plane of the axis
    vecA -= axis*vecA.dot(axis);
    vecB -= axis*vecB.dot(axis);
  }
  scaledCosAngle =   NMP::vDot(vecA, vecB);
  resultOutput->m_value = NMP::radiansToDegrees(atan2f(scaledSinAngle, scaledCosAngle));

  // The requested output pin will automatically be flagged as having been updated this frame,
  // but we need to make sure that all pins we have updated are correctly flagged.
  outputCPPinResult->m_lastUpdateFrame = net->getCurrentFrameNo();
    
  // Return the value of the requested output cp pin.
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
