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
#include "morpheme/Nodes/mrNodeOperatorRateOfChange.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrDebugMacros.h"
#include <float.h>
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorRateOfChangeFloat(
  NodeDef* node,
  PinIndex outputCPPinIndex,
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  //-------------------
  // Update the connected input control parameter.
  AttribDataFloat* inputCPFloat = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(0), animSet);
  float targetVal = inputCPFloat->m_value;

  //-------------------
  // Get the output attribute data and state data 
  NodeID nodeID = node->getNodeID();
  NodeBinEntry* stateEntry = net->getAttribDataNodeBinEntry(
                                        ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                                        nodeID,
                                        INVALID_NODE_ID,
                                        VALID_FRAME_ANY_FRAME);
  NMP_ASSERT(stateEntry); // State data must also already exist.
  AttribDataRateOfChangeState* stateData = stateEntry->getAttribData<AttribDataRateOfChangeState>();

  NodeBin* nodeBin = net->getNodeBin(nodeID);
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(outputCPPinIndex);
  AttribDataFloat* outputCPFloat = outputCPPin->getAttribData<AttribDataFloat>();

  // If the data we have got is not from the last update frame initialise the cached value directly to zero.
  FrameCount currFrameNo = net->getCurrentFrameNo();
  if (outputCPPin->m_lastUpdateFrame != (currFrameNo - 1))
  {
    outputCPFloat->m_value = 0.0f;
  }
  

  //-------------------
  // Networks update time (Global delta time).
  AttribDataUpdatePlaybackPos* netUpdateTimeAttrib = net->getOptionalAttribData<AttribDataUpdatePlaybackPos>(
    ATTRIB_SEMANTIC_UPDATE_TIME_POS, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
  if (!netUpdateTimeAttrib)
  {
    // if we cannot evaluate the current network time then do not get a rate of change
    outputCPFloat->m_value = 0.0f;
    return outputCPFloat;
  }
  NMP_ASSERT(netUpdateTimeAttrib->getType() == ATTRIB_TYPE_UPDATE_PLAYBACK_POS);

  //-------------------
  // Perform the actual task
  float timeDelta = 0.0f;
  if (!netUpdateTimeAttrib->m_isAbs)
  {
    timeDelta = netUpdateTimeAttrib->m_value;
  }

  outputCPFloat->m_value = 0.0f;
  if (timeDelta > FLT_MIN)
  {
    outputCPFloat->m_value = (targetVal - stateData->m_lastFloat) / timeDelta;
  }
  stateData->m_lastFloat = targetVal;

  return outputCPFloat;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorRateOfChangeVector(
  NodeDef* node,
  PinIndex outputCPPinIndex, // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  //-------------------
  // Update the connected input control parameter.
  AttribDataVector3* inputCPVector = net->updateInputCPConnection<AttribDataVector3>(node->getInputCPConnection(0), animSet);
  NMP::Vector3 targetVal = inputCPVector->m_value;

  //-------------------
  // Get the output attribute data and state data.
  NodeID nodeID = node->getNodeID();
  NodeBinEntry* stateEntry = net->getAttribDataNodeBinEntry(
                                        ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                                        nodeID,
                                        INVALID_NODE_ID,
                                        VALID_FRAME_ANY_FRAME);
  NMP_ASSERT(stateEntry); // State data must also already exist.
  AttribDataRateOfChangeState* stateData = stateEntry->getAttribData<AttribDataRateOfChangeState>();

  NodeBin* nodeBin = net->getNodeBin(nodeID);
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(outputCPPinIndex);
  AttribDataVector3* outputCPVector = outputCPPin->getAttribData<AttribDataVector3>();

  // If the data we have got is not from the last update frame initialise the cached value directly to the zero.
  FrameCount currFrameNo = net->getCurrentFrameNo();
  if (outputCPPin->m_lastUpdateFrame != (currFrameNo - 1))
  {
    outputCPVector->m_value.setToZero();
  }
  
  //-------------------
  // Networks update time (Global delta time).
  AttribDataUpdatePlaybackPos* netUpdateTimeAttrib = net->getOptionalAttribData<AttribDataUpdatePlaybackPos>(
    ATTRIB_SEMANTIC_UPDATE_TIME_POS, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
  if (!netUpdateTimeAttrib)
  {
    // if we cannot evaluate the current network time then do not set a rate of change
    outputCPVector->m_value.setToZero();
    return outputCPVector;
  }
  NMP_ASSERT(netUpdateTimeAttrib->getType() == ATTRIB_TYPE_UPDATE_PLAYBACK_POS);

  //-------------------
  // Perform the actual task
  float timeDelta = 0.0f;
  if (!netUpdateTimeAttrib->m_isAbs)
  {
    timeDelta = netUpdateTimeAttrib->m_value;
  }

  //-------------------
  NMP::Vector3& currentVal = stateData->m_lastVector;

  outputCPVector->m_value.setToZero();
  if (timeDelta > FLT_MIN)
  {
    outputCPVector->m_value = (targetVal - currentVal) / timeDelta;
  }
  currentVal = targetVal;

  return outputCPVector;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorRateOfChangeFloatInitInstance(NodeDef* node, Network* net)
{
  // Create state data.
  AttribDataHandle handle = AttribDataRateOfChangeState::create(net->getPersistentMemoryAllocator());
  AttribAddress stateAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);
  net->addAttribData(stateAddress, handle, LIFESPAN_FOREVER);

  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
  outputCPPin->m_lastUpdateFrame = 0;

  //---------------------------
  if (node->getNumReflexiveCPPins() > 0)
  {
    MR::nodeInitPinAttribDataInstance(node, net);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorRateOfChangeVectorInitInstance(NodeDef* node, Network* net)
{
  // Create state data.
  AttribDataHandle handle = AttribDataRateOfChangeState::create(net->getPersistentMemoryAllocator());  
  AttribAddress stateAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);
  net->addAttribData(stateAddress, handle, LIFESPAN_FOREVER);

  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataVector3::create(net->getPersistentMemoryAllocator(), 
    NMP::Vector3::InitTypeZero);
  outputCPPin->m_lastUpdateFrame = 0;

  //---------------------------
  if (node->getNumReflexiveCPPins() > 0)
  {
    MR::nodeInitPinAttribDataInstance(node, net);
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
