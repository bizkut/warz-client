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
#include "morpheme/Nodes/mrNodeOperatorSmoothFloat.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrDebugMacros.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorSmoothFloatCriticallyDampFloat(
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
  AttribDataSmoothFloatOperation* stateData = stateEntry->getAttribData<AttribDataSmoothFloatOperation>();

  NodeBin* nodeBin = net->getNodeBin(nodeID);
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(outputCPPinIndex);
  AttribDataFloat* outputCPFloat = outputCPPin->getAttribData<AttribDataFloat>();

  //-------------------
  // Get the node defs.
  AttribDataSmoothFloatOperation* smoothOpDefData = node->getAttribData<AttribDataSmoothFloatOperation>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  float smoothTimeInc = smoothOpDefData->m_floatRateIncreasing;
  float smoothTimeDec = smoothOpDefData->m_floatRateDecreasing;

  // If the data we have got is not from the last update frame initialise the cached value directly to the input value.
  FrameCount currFrameNo = net->getCurrentFrameNo();
  if (outputCPPin->m_lastUpdateFrame != (currFrameNo - 1))
  {
    if(smoothOpDefData->m_useInitValOnInit)
    {
      outputCPFloat->m_value = smoothOpDefData->m_initialValueX;
    }
    else
    {
      outputCPFloat->m_value = targetVal;
    }
  }
  
  //-------------------
  // Networks update time (Global delta time).
  AttribDataUpdatePlaybackPos* netUpdateTimeAttrib = net->getOptionalAttribData<AttribDataUpdatePlaybackPos>(
    ATTRIB_SEMANTIC_UPDATE_TIME_POS, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
  if (!netUpdateTimeAttrib)
  {
    // if we cannot evaluate the current network time then do not smooth
    outputCPFloat->m_value = targetVal;
    return outputCPFloat;
  }
  NMP_ASSERT(netUpdateTimeAttrib->getType() == ATTRIB_TYPE_UPDATE_PLAYBACK_POS);

  //-------------------
  // Perform the actual task

  // Critical damping towards the target using the value and its time derivative.
  float timeDelta = 0.0f;
  if (!netUpdateTimeAttrib->m_isAbs)
  {
    timeDelta = netUpdateTimeAttrib->m_value;
  }

  //-------------------
  // The current value and what will be updated. The effective return value.
  float& currentVal = outputCPFloat->m_value;

  // Lets the cache the time to use for smoothing dependent on whether 
  // the target number is greater or smaller than the current value.
  float smoothTime = smoothTimeInc;
  if( currentVal > targetVal )// we are decreasing
    smoothTime = smoothTimeDec;

  if (smoothOpDefData->m_smoothVel) // Critically damped spring
  {
    float& currentRate = stateData->m_floatRateIncreasing;

    if (smoothTime > 0.0f)
    {
      float y = 2.0f / smoothTime;
      float x = y * timeDelta;
      float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
      float change = currentVal - targetVal;
      float temp = (currentRate + y * change) * timeDelta;
      currentRate = (currentRate - y * temp) * exp;
      currentVal = targetVal + (change + temp) * exp;
    }
    else if (timeDelta > 0.0f)
    {
      currentRate = (targetVal - currentVal) / timeDelta;
      currentVal = targetVal;
    }
  }
  else // Exponential decay to target
  {
    if (smoothTime > 0.0f)
    {
      NMP::smoothExponential(currentVal, timeDelta, targetVal, smoothTime);
    }
    else if (timeDelta > 0.0f)
    {
      currentVal = targetVal;
    }
  }

  MR_LOG_SCRATCH_PAD_FLOAT(
    net->getDispatcher()->getDebugInterface(),
    nodeID,
    "Float damping rate",
    stateData->m_floatRateIncreasing);

  return outputCPFloat;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorSmoothFloatCriticallyDampVector(
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
  AttribDataSmoothFloatOperation* stateData = stateEntry->getAttribData<AttribDataSmoothFloatOperation>();

  NodeBin* nodeBin = net->getNodeBin(nodeID);
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(outputCPPinIndex);
  AttribDataVector3* outputCPVector = outputCPPin->getAttribData<AttribDataVector3>();

  //-------------------
  // Get the node defs.
  AttribDataSmoothFloatOperation* smoothOp = node->getAttribData<AttribDataSmoothFloatOperation>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  float smoothTime = smoothOp->m_floatRateIncreasing; // DK (20/2/2013) currently the smoothed vector has no different behaviour 
                                                      // based on magnitude or direction. Indeed this part of the problem what does 
                                                      // 'increase' precisely mean for a vector.

  // If the data we have got is not from the last update frame initialise the cached value directly to the input value.
  FrameCount currFrameNo = net->getCurrentFrameNo();
  if (outputCPPin->m_lastUpdateFrame != (currFrameNo - 1))
  {
    if(smoothOp->m_useInitValOnInit)
    {
      outputCPVector->m_value.x = smoothOp->m_initialValueX;
      outputCPVector->m_value.y = smoothOp->m_initialValueY;
      outputCPVector->m_value.z = smoothOp->m_initialValueZ;
    }
    else
    {
      outputCPVector->m_value = targetVal;
    }
  }
  
  //-------------------
  // Networks update time (Global delta time).
  AttribDataUpdatePlaybackPos* netUpdateTimeAttrib = net->getOptionalAttribData<AttribDataUpdatePlaybackPos>(
    ATTRIB_SEMANTIC_UPDATE_TIME_POS, NETWORK_NODE_ID, INVALID_NODE_ID, currFrameNo);
  if (!netUpdateTimeAttrib)
  {
    // if we cannot evaluate the current network time then do not smooth
    outputCPVector->m_value = targetVal;
    return outputCPVector;
  }
  NMP_ASSERT(netUpdateTimeAttrib->getType() == ATTRIB_TYPE_UPDATE_PLAYBACK_POS);

  //-------------------
  // Perform the actual task

  // Critical damping towards the target using the value and its time derivative.
  float timeDelta = 0.0f;
  if (!netUpdateTimeAttrib->m_isAbs)
  {
    timeDelta = netUpdateTimeAttrib->m_value;
  }

  //-------------------
  NMP::Vector3& currentVal = outputCPVector->m_value;
  NMP::Vector3& currentRate = stateData->m_vectorRate;

  if (smoothOp->m_smoothVel)
  {
    if (smoothTime > 0.0f)
    {
      float y = 2.0f / smoothTime;
      float x = y * timeDelta;
      float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
      NMP::Vector3 change = currentVal - targetVal;
      NMP::Vector3 temp;
      temp.x = (currentRate.x + (y * change.x)) * timeDelta;
      temp.y = (currentRate.y + (y * change.y)) * timeDelta;
      temp.z = (currentRate.z + (y * change.z)) * timeDelta;

      currentRate = (currentRate - (y * temp)) * exp;
      currentVal = targetVal + (change + temp) * exp;
    }
    else if (timeDelta > 0.0f)
    {
      currentRate = (targetVal - currentVal) / timeDelta;
      currentVal = targetVal;
    }
  }
  else // Exponential decay to target
  {
    if (smoothTime > 0.0f)
    {
      NMP::smoothExponential(currentVal, timeDelta, targetVal, smoothTime);
    }
    else if (timeDelta > 0.0f)
    {
      currentVal = targetVal;
    }
  }
  MR_LOG_SCRATCH_PAD_VECTOR3(
    net->getDispatcher()->getDebugInterface(),
    nodeID,
    "Vector damping rate",
    currentRate);

  return outputCPVector;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorSmoothDampFloatInitInstance(NodeDef* node, Network* net)
{
  // Create state data.
  AttribDataHandle handle = AttribDataSmoothFloatOperation::create(net->getPersistentMemoryAllocator());
  AttribAddress stateAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);
  net->addAttribData(stateAddress, handle, LIFESPAN_FOREVER);

  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
  // Martin: Workaround to get the node to initialise its output to its input on
  // the first frame. Can't use -1 as this is VALID_FOREVER and so we don't
  // update.
  outputCPPin->m_lastUpdateFrame = (FrameCount) -2;

  //---------------------------
  if (node->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(node, net);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorSmoothDampVectorInitInstance(NodeDef* node, Network* net)
{
  // Create state data.
  AttribDataHandle handle = AttribDataSmoothFloatOperation::create(net->getPersistentMemoryAllocator());  
  AttribAddress stateAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);
  net->addAttribData(stateAddress, handle, LIFESPAN_FOREVER);

  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataVector3::create(net->getPersistentMemoryAllocator(), 
    NMP::Vector3::InitTypeZero);
  outputCPPin->m_lastUpdateFrame = (FrameCount) -2;

  //---------------------------
  if (node->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(node, net);
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
