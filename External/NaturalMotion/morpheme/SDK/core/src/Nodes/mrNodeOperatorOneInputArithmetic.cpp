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
#include "morpheme/Nodes/mrNodeOperatorOneInputArithmetic.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
static bool updateOneInputArithmeticOperator(
  NodeDef*         node,
  AttribDataFloat* input,
  AttribDataFloat* output)
{
  AttribDataArithmeticOperation* floatOperation = node->getAttribData<AttribDataArithmeticOperation>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  float constValue = floatOperation->m_constValue;
  
  switch (floatOperation->m_operation)
  {
    case AttribDataArithmeticOperation::OPERATION_MULTIPLY:
      output->m_value = input->m_value * constValue;
      break;
    case AttribDataArithmeticOperation::OPERATION_MULTIPLYADD:
      // This special case makes use of the first vector component in order to * then add a different value
      output->m_value = (input->m_value * constValue) + floatOperation->m_constVector.x; 
      break;
    case AttribDataArithmeticOperation::OPERATION_ADD:
      output->m_value = input->m_value + constValue;
      break;
    case AttribDataArithmeticOperation::OPERATION_DIVIDE:
      output->m_value = input->m_value / constValue;
      break;
    case AttribDataArithmeticOperation::OPERATION_SUBTRACT:
      output->m_value = input->m_value - constValue;
      break;
    case AttribDataArithmeticOperation::OPERATION_MIN:
      output->m_value = NMP::minimum(input->m_value, constValue);
      break;
    case AttribDataArithmeticOperation::OPERATION_MAX:
      output->m_value = NMP::maximum(input->m_value, constValue);
      break;
    default:
      NMP_ASSERT_FAIL(); // Invalid operation
      break;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorOneInputArithmeticOutputCPUpdateFloat(
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
  updateOneInputArithmeticOperator(node, inputCPFloat, outputCPFloat);

  return outputCPFloat;
}



//----------------------------------------------------------------------------------------------------------------------
static bool updateOneInputArithmeticOperator(
                                NodeDef*         node,
                                AttribDataVector3* input,
                                AttribDataVector3* output)
{
  AttribDataArithmeticOperation* floatOperation = node->getAttribData<AttribDataArithmeticOperation>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  NMP::Vector3 constValue = floatOperation->m_constVector;

  switch (floatOperation->m_operation)
  {
    case AttribDataArithmeticOperation::OPERATION_MULTIPLY:
      output->m_value = input->m_value;
      output->m_value.multiplyElements(constValue);
      break;
    case AttribDataArithmeticOperation::OPERATION_MULTIPLYADD:
      output->m_value = (input->m_value * floatOperation->m_constValue) + constValue;
      break;
    case AttribDataArithmeticOperation::OPERATION_ADD:
      output->m_value = input->m_value + constValue;
      break;
    case AttribDataArithmeticOperation::OPERATION_DIVIDE:
#ifdef NMP_ENABLE_ASSERTS
      if (constValue.x == 0 || constValue.y == 0 || constValue.z == 0)
        NMP_DEBUG_MSG("Operator2Const node: division by zero has been avoided.\n");
#endif
      output->m_value.x = constValue.x==0 ? 0 : input->m_value.x/constValue.x;
      output->m_value.y = constValue.y==0 ? 0 : input->m_value.y/constValue.y;
      output->m_value.z = constValue.z==0 ? 0 : input->m_value.z/constValue.z;
      break;
    case AttribDataArithmeticOperation::OPERATION_SUBTRACT:
      output->m_value = input->m_value - constValue;
      break;
    case AttribDataArithmeticOperation::OPERATION_MIN:
      output->m_value.set(
        NMP::minimum(input->m_value.x, constValue.x),
        NMP::minimum(input->m_value.y, constValue.y),
        NMP::minimum(input->m_value.z, constValue.z));
      break;
    case AttribDataArithmeticOperation::OPERATION_MAX:
      output->m_value.set(
        NMP::maximum(input->m_value.x, constValue.x),
        NMP::maximum(input->m_value.y, constValue.y),
        NMP::maximum(input->m_value.z, constValue.z));
      break;
    default:
      NMP_ASSERT_FAIL(); // Invalid operation
      break;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorOneInputArithmeticOutputCPUpdateVector(
  NodeDef* node,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  AttribDataVector3* inputCPVector = net->updateInputCPConnection<AttribDataVector3>(node->getInputCPConnection(0), animSet);

  // Get the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  AttribDataVector3* outputCPVector = outputCPPin->getAttribData<AttribDataVector3>();

  // Perform the actual task
  updateOneInputArithmeticOperator(node, inputCPVector, outputCPVector);

  return outputCPVector;
}
} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
