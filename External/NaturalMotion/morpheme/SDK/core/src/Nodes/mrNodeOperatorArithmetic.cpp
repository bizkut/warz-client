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
#include "morpheme/Nodes/mrNodeOperatorArithmetic.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
static bool updateArithmeticOperator(
  NodeDef*         node,
  AttribDataFloat* input0,
  AttribDataFloat* input1,
  AttribDataFloat* output)
{
  AttribDataArithmeticOperation* floatOperator = node->getAttribData<AttribDataArithmeticOperation>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  
  float f0 = input0->m_value;
  float f1 = input1->m_value;

  switch (floatOperator->m_operation)
  {
    case AttribDataArithmeticOperation::OPERATION_MULTIPLY:
      output->m_value = f0 * f1;
      break;
    case AttribDataArithmeticOperation::OPERATION_ADD:
      output->m_value = f0 + f1;
      break;
    case AttribDataArithmeticOperation::OPERATION_DIVIDE:
#ifdef NMP_ENABLE_ASSERTS
      if (f1 == 0)
        NMP_DEBUG_MSG("OperatorArithmetic node: division by zero has been avoided.\n");
#endif
      output->m_value = (f1!=0) ? (f0 / f1) : (0.0f);
      break;
    case AttribDataArithmeticOperation::OPERATION_SUBTRACT:
      output->m_value = f0 - f1;
      break;
    case AttribDataArithmeticOperation::OPERATION_MIN:
      output->m_value = (f0 > f1) ? (f1) : (f0);
      break;
    case AttribDataArithmeticOperation::OPERATION_MAX:
      output->m_value = (f0 > f1) ? (f0) : (f1);
      break;
    default:
      NMP_ASSERT_FAIL(); // Invalid operation
      break;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorArithmeticOutputCPUpdateFloat(
  NodeDef* node,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  AttribDataFloat* inputCPFloat0 = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(0), animSet);
  AttribDataFloat* inputCPFloat1 = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(1), animSet);

  // Get the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  AttribDataFloat* outputCPFloat = outputCPPin->getAttribData<AttribDataFloat>();

  // Perform the actual task
  updateArithmeticOperator(node, inputCPFloat0, inputCPFloat1, outputCPFloat);

  return outputCPFloat;
}

//----------------------------------------------------------------------------------------------------------------------
static bool updateArithmeticVectorOperator(
  NodeDef* node,
  AttribDataVector3* input0,
  AttribDataVector3* input1,
  AttribDataVector3* output)
{
  AttribDataArithmeticOperation* floatOperator = node->getAttribData<AttribDataArithmeticOperation>(
    ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  NMP::Vector3 f0 = input0->m_value;
  NMP::Vector3 f1 = input1->m_value;

  switch (floatOperator->m_operation)
  {
    case AttribDataArithmeticOperation::OPERATION_MULTIPLY:
      output->m_value = f0;
      output->m_value.multiplyElements(f1);
      break;
    case AttribDataArithmeticOperation::OPERATION_ADD:
      output->m_value = f0 + f1;
      break;
    case AttribDataArithmeticOperation::OPERATION_DIVIDE:
#ifdef NMP_ENABLE_ASSERTS
      if (f1.x == 0 || f1.y == 0 || f1.z == 0)
        NMP_DEBUG_MSG("Operator2Vector3 node: division by zero has been avoided.\n");
#endif
      output->m_value.x = f1.x==0 ? 0 : f0.x/f1.x;
      output->m_value.y = f1.y==0 ? 0 : f0.y/f1.y;
      output->m_value.z = f1.z==0 ? 0 : f0.z/f1.z;
      break;
    case AttribDataArithmeticOperation::OPERATION_SUBTRACT:
      output->m_value = f0 - f1;
      break;
    case AttribDataArithmeticOperation::OPERATION_MIN:
      output->m_value.set(NMP::minimum(f0.x, f1.x), NMP::minimum(f0.y, f1.y), NMP::minimum(f0.z, f1.z));
      break;
    case AttribDataArithmeticOperation::OPERATION_MAX:
      output->m_value.set(NMP::maximum(f0.x, f1.x), NMP::maximum(f0.y, f1.y), NMP::maximum(f0.z, f1.z));
      break;
    default:
      NMP_ASSERT_FAIL(); // Invalid operation
      break;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeOperatorArithmeticOutputCPUpdateVector(
  NodeDef* node,
  PinIndex NMP_USED_FOR_ASSERTS(outputCPPinIndex), // The output pin we have been asked to update.
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex == 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  AttribDataVector3* inputCPVector0 = net->updateInputCPConnection<AttribDataVector3>(node->getInputCPConnection(0), animSet);
  AttribDataVector3* inputCPVector1 = net->updateInputCPConnection<AttribDataVector3>(node->getInputCPConnection(1), animSet);

  // Get the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  AttribDataVector3* outputCPVector = outputCPPin->getAttribData<AttribDataVector3>();

  // Perform the actual task
  updateArithmeticVectorOperator(node, inputCPVector0, inputCPVector1, outputCPVector);

  return outputCPVector;
}


} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
