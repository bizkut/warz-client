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
#include "morpheme/Nodes/mrNodeOperatorCPCompare.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Templated for shared use by float, int and uint.
template <typename T>
NM_INLINE AttribData* nodeOperatorCPCompareOutputCPUpdateBool(
  NodeDef* nodeDef,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);

  // Get the output attribute
  NodeBin* nodeBin = net->getNodeBin(nodeDef->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  AttribDataBool* outputCPBool = outputCPPin->getAttribData<AttribDataBool>();

  // Which operation do we need to perform.
  AttribDataValueCompareOperation* compareOperator = nodeDef->getAttribData<AttribDataValueCompareOperation>(
                                                                    ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  NMP_ASSERT(nodeDef->getNumInputCPConnections() > 0);

  // Get input pin values.
  AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());
  T* inputPin0Attrib = net->updateInputCPConnection<T>(nodeDef->getInputCPConnection(0), animSet);
  T* inputPin1Attrib = net->updateInputCPConnection<T>(nodeDef->getInputCPConnection(1), animSet);

  // Update the connected control parameters and determine the results of the correct operation.
  switch (compareOperator->m_operation)
  {
    case AttribDataValueCompareOperation::OPERATION_GREATER:
      outputCPBool->m_value = (inputPin0Attrib->m_value > inputPin1Attrib->m_value);
      break;
    case AttribDataValueCompareOperation::OPERATION_LESS:
      outputCPBool->m_value = (inputPin0Attrib->m_value < inputPin1Attrib->m_value);
      break;
    case AttribDataValueCompareOperation::OPERATION_GREATER_EQUAL:
      outputCPBool->m_value = (inputPin0Attrib->m_value >= inputPin1Attrib->m_value);
      break;
    case AttribDataValueCompareOperation::OPERATION_LESS_EQUAL:
      outputCPBool->m_value = (inputPin0Attrib->m_value <= inputPin1Attrib->m_value);
      break;
    case AttribDataValueCompareOperation::OPERATION_EQUAL:
      outputCPBool->m_value = (inputPin0Attrib->m_value == inputPin1Attrib->m_value);
      break;
    case AttribDataValueCompareOperation::OPERATION_NOT_EQUAL:
      outputCPBool->m_value = (inputPin1Attrib->m_value != inputPin0Attrib->m_value);
      break;
    default:
      // Invalid operator.
      NMP_ASSERT_FAIL();
      break;
  }

  return outputCPBool;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorCPCompareFloatOutputCPUpdateBool(
  NodeDef* node,
  PinIndex outputCPPinIndex, // The output pin we have been asked to update.
  Network* net)
{
  return nodeOperatorCPCompareOutputCPUpdateBool<AttribDataFloat>(node, outputCPPinIndex, net);
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorCPCompareIntOutputCPUpdateBool(
  NodeDef* nodeDef,
  PinIndex outputCPPinIndex, // The output pin we have been asked to update.
  Network* net)
{
  return nodeOperatorCPCompareOutputCPUpdateBool<AttribDataInt>(nodeDef, outputCPPinIndex, net);
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorCPCompareUIntOutputCPUpdateBool(
  NodeDef* nodeDef,
  PinIndex outputCPPinIndex, // The output pin we have been asked to update.
  Network* net)
{
  return nodeOperatorCPCompareOutputCPUpdateBool<AttribDataUInt>(nodeDef, outputCPPinIndex, net);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
