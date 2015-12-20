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
#include "morpheme/Nodes/mrNodeOperatorBoolean.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorBooleanOutputCPUpdateBool(
  NodeDef* node,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);

  // Get the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  AttribDataBool* outputCPBool = outputCPPin->getAttribData<AttribDataBool>();

  // Which operation do we need to perform.
  AttribDataBooleanOperation* boolOperator = node->getAttribData<AttribDataBooleanOperation>(
                                                                    ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Update the connected control parameters and determine the results of the correct operation.
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  switch (boolOperator->m_operation)
  {
    case AttribDataBooleanOperation::OPERATION_AND:
      outputCPBool->m_value = true;
      for (PinIndex i = 0; i < node->getNumInputCPConnections(); ++i)
      {
        outputCPBool->m_value &= net->updateInputCPConnection<AttribDataBool>(node->getInputCPConnection(i), animSet)->m_value;
      }
      break;
    case AttribDataBooleanOperation::OPERATION_OR:
      outputCPBool->m_value = false;
      for (PinIndex i = 0; i < node->getNumInputCPConnections(); ++i)
      {
        outputCPBool->m_value |= net->updateInputCPConnection<AttribDataBool>(node->getInputCPConnection(i), animSet)->m_value;
      }
      break;
    case AttribDataBooleanOperation::OPERATION_XOR:
      outputCPBool->m_value = false;
      for (PinIndex i = 0; i < node->getNumInputCPConnections(); ++i)
      {
        outputCPBool->m_value ^= net->updateInputCPConnection<AttribDataBool>(node->getInputCPConnection(i), animSet)->m_value;
      }
      break;
    default:
      // Invalid operator.
      NMP_ASSERT_FAIL();
      break;
  }
  
  // Invert the result if asked to.
  if (boolOperator->m_invertResult)
  {
     outputCPBool->m_value = !outputCPBool->m_value;
  }

  return outputCPBool;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
