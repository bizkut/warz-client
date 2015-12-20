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
#include "morpheme/Nodes/mrNodeOperatorRandomFloat.h"
//----------------------------------------------------------------------------------------------------------------------
#include <stdlib.h>

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorRandomFloatOutputCPUpdateFloat(
  NodeDef* node,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);

  //-------------------
  // Get node def data.
  AttribDataRandomFloatDef* defData = node->getAttribData<AttribDataRandomFloatDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  
  //-------------------
  // Get the output attribute data and state data.
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  NodeBinEntry* stateEntry = net->getAttribDataNodeBinEntry(
                                        ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
                                        node->getNodeID(),
                                        INVALID_NODE_ID,
                                        VALID_FRAME_ANY_FRAME);

  NMP_ASSERT(stateEntry); // State data must also already exist.
  AttribDataRandomFloatOperation* stateData = stateEntry->getAttribData<AttribDataRandomFloatOperation>();

  AttribDataFloat* outputCPFloat = outputCPPin->getAttribData<AttribDataFloat>();

  //-------------------
  // account for duration - get current network time
  const uint32_t currFrameNo = net->getCurrentFrameNo();
  AttribDataUpdatePlaybackPos* netUpdateTimeAttrib = net->getAttribData<AttribDataUpdatePlaybackPos>(
                                                                 ATTRIB_SEMANTIC_UPDATE_TIME_POS,
                                                                 NETWORK_NODE_ID,
                                                                 INVALID_NODE_ID,
                                                                 currFrameNo,
                                                                 0);

  //-------------------
  // update the output
  const float duration = defData->m_duration;

  bool hasDurationElapsed = false;

  if (netUpdateTimeAttrib->m_isAbs)
  {
    // check to see if time between stored time and this frame's time matches duration
    const float elapsedTime = netUpdateTimeAttrib->m_value - stateData->m_lastFrameTime;

    if (elapsedTime >= duration)
    {
      hasDurationElapsed = true;
      stateData->m_lastFrameTime = netUpdateTimeAttrib->m_value;
    }
  }
  else
  {
    // treat node specific data as elapsed time and accumulate to check duration
    stateData->m_lastFrameTime += netUpdateTimeAttrib->m_value;

    if (stateData->m_lastFrameTime >= duration)
    {
      hasDurationElapsed = true;
      stateData->m_lastFrameTime = 0.0f;
    }
  }

  if (hasDurationElapsed)
  {
    // grab the min and max parameters
    const float min = defData->m_min;
    const float max = defData->m_max;

#ifdef NMP_ENABLE_ASSERTS
    const float range = max - min;
    const float epsilon = 1e-4f;
#endif

    NMP_ASSERT(range > epsilon);

    outputCPFloat->m_value = stateData->m_generator.genFloat(min, max);

    NMP_ASSERT(outputCPFloat->m_value >= min && outputCPFloat->m_value <= max);
  }

  return outputCPFloat;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorRandomFloatInitInstance(NodeDef* node, Network* net)
{
  //-------------------
  // Get node def data.
  AttribDataRandomFloatDef* defData = node->getAttribData<AttribDataRandomFloatDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  //-------------------
  // Create the output attribute data and state data if they don't exist already.
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);

  // Generate a random seed or use the one provided by the user
  uint32_t seed = defData->m_seed == 0 ? (uint32_t)rand() : defData->m_seed;
  
  // Create state data.
  AttribDataHandle handle = AttribDataRandomFloatOperation::create(net->getPersistentMemoryAllocator(), seed);
  AttribDataRandomFloatOperation* stateData = (AttribDataRandomFloatOperation*)(handle.m_attribData);  
  AttribAddress stateAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);
  net->addAttribData(stateAddress, handle, LIFESPAN_FOREVER);

  // Create output CP data.
  const float initialValue = stateData->m_generator.genFloat(defData->m_min, defData->m_max);
  outputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), initialValue);
  outputCPPin->m_lastUpdateFrame = 0;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
