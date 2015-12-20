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
#include "morpheme/Nodes/mrNodeOperatorFunction.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
static bool updateFunctionOperator(
  NodeDef*         node,
  AttribDataFloat* input,
  AttribDataFloat* output)
{
  AttribDataFunctionOperation* floatOperator = node->getAttribData<AttribDataFunctionOperation>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  
  switch (floatOperator->m_operation)
  {
  case AttribDataFunctionOperation::OPERATION_SIN:
    output->m_value = sinf(input->m_value);
    break;
  case AttribDataFunctionOperation::OPERATION_COS:
    output->m_value = cosf(input->m_value);
    break;
  case AttribDataFunctionOperation::OPERATION_TAN:
    output->m_value = tanf(input->m_value);
    break;
  case AttribDataFunctionOperation::OPERATION_EXP:
    output->m_value = expf(input->m_value);
    break;
  case AttribDataFunctionOperation::OPERATION_LOG:
    output->m_value = logf(input->m_value);
    break;
  case AttribDataFunctionOperation::OPERATION_SQRT:
    output->m_value = sqrtf(input->m_value);
    break;
  case AttribDataFunctionOperation::OPERATION_ABS:
    output->m_value = fabsf(input->m_value);
    break;
  case AttribDataFunctionOperation::OPERATION_ASIN:
    output->m_value = asinf(NMP::clampValue(input->m_value, -1.0f, 1.0f));
    break;
  case AttribDataFunctionOperation::OPERATION_ACOS:
    output->m_value = acosf(NMP::clampValue(input->m_value, -1.0f, 1.0f));
    break;
  default:
    NMP_ASSERT_FAIL(); // Invalid operation
    break;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorFunctionOutputCPUpdateFloat(
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
  AttribDataFloat* outputCPFloat = outputCPPin->getAttribData<AttribDataFloat>();

  // Perform the actual task
  updateFunctionOperator(node, inputCPFloat, outputCPFloat);

  return outputCPFloat;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
