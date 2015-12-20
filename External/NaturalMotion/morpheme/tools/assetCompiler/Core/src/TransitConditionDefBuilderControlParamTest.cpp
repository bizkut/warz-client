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
#include "TransitConditionDefBuilderControlParamTest.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamFloatGreater.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamFloatLess.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamIntGreater.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamIntLess.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamUIntGreater.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamUIntLess.h"
#include "assetProcessor/NodeBuilderUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefBuilderControlParamTest::getMemoryRequirements(
  const ME::ConditionExport*   condExport,
  const ME::NetworkDefExport*  NMP_UNUSED(netDefExport),
  AssetProcessor*              NMP_UNUSED(processor))
{
  // We are using this builder to construct less than and greater than.
  // Make sure they are still the same size at least.
  NM_ASSERT_COMPILE_TIME(sizeof(MR::TransitConditionDefControlParamFloatGreater) == sizeof(MR::TransitConditionDefControlParamFloatLess));
  NM_ASSERT_COMPILE_TIME(sizeof(MR::TransitConditionDefControlParamIntGreater) == sizeof(MR::TransitConditionDefControlParamIntLess));
  NM_ASSERT_COMPILE_TIME(sizeof(MR::TransitConditionDefControlParamUIntGreater) == sizeof(MR::TransitConditionDefControlParamUIntLess));

  const ME::DataBlockExport* condDataBlock = condExport->getDataBlock();

  std::string dataType;
  condDataBlock->readString(dataType, "DataType");
  if(dataType == "float")
  {
    return NMP::Memory::Format(NMP::Memory::align(sizeof(MR::TransitConditionDefControlParamFloatGreater), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else if(dataType == "int")
  {
    return NMP::Memory::Format(NMP::Memory::align(sizeof(MR::TransitConditionDefControlParamIntGreater), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else if(dataType == "uint")
  {
    return NMP::Memory::Format(NMP::Memory::align(sizeof(MR::TransitConditionDefControlParamUIntGreater), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
  }

  NMP_VERIFY_MSG(dataType == "float" || dataType == "int" || dataType == "uint", "Invalid data type '%s' specified for condition.", dataType.c_str());
  return NMP::Memory::Format();
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the TransitCondition from connects xml data.
MR::TransitConditionDef* TransitConditionDefBuilderControlParamTest::init(
  NMP::Memory::Resource&       memRes,
  const ME::ConditionExport*   condExport,
  const ME::NetworkDefExport*  netDefExport,
  AssetProcessor*              processor,
  MR::NodeID                   NMP_UNUSED(sourceNodeID))
{
  const ME::DataBlockExport* condDataBlock = condExport->getDataBlock();

  NMP::Memory::Format memReqs = getMemoryRequirements(condExport, netDefExport, processor);
  memRes.align(memReqs);
  bool lessThan;
  condDataBlock->readBool(lessThan, "LessThanOperation");

  // Initialise input control param connection to look for.
  MR::NodeID cpNodeID = MR::INVALID_NODE_ID;
  readNodeID(condDataBlock, "RuntimeNodeID", cpNodeID);
  MR::PinIndex cpPinIndex = MR::CONTROL_PARAMETER_NODE_PIN_0; // Only connects to control parameters [MORPH-9125]
  
  bool orEqual;
  condDataBlock->readBool(orEqual, "OrEqual");

  std::string dataType;
  condDataBlock->readString(dataType, "DataType");
  if(dataType == "float")
  {
    float testValue;
    condDataBlock->readFloat(testValue, "TestValue");

    if (lessThan)
    {
      MR::TransitConditionDefControlParamFloatLess* result = (MR::TransitConditionDefControlParamFloatLess*) memRes.ptr;
      memRes.increment(memReqs);
      result->setType(TRANSCOND_CONTROL_PARAM_FLOAT_LESS_ID);
      result->setCPConnection(cpNodeID, cpPinIndex);
      result->setTestValue(testValue);
      result->setOrEqual(orEqual);
      result->setInvertFlag(false);
      return result;
    }
    else
    {
      MR::TransitConditionDefControlParamFloatGreater* result = (MR::TransitConditionDefControlParamFloatGreater*) memRes.ptr;
      memRes.increment(memReqs);
      result->setType(TRANSCOND_CONTROL_PARAM_FLOAT_GREATER_ID);
      result->setCPConnection(cpNodeID, cpPinIndex);
      result->setTestValue(testValue);
      result->setOrEqual(orEqual);
      result->setInvertFlag(false);
      return result;
    }
  }
  else if(dataType == "int")
  {
    int32_t testValue;
    condDataBlock->readInt(testValue, "TestValue");

    if (lessThan)
    {
      MR::TransitConditionDefControlParamIntLess* result = (MR::TransitConditionDefControlParamIntLess*) memRes.ptr;
      memRes.increment(memReqs);
      result->setType(TRANSCOND_CONTROL_PARAM_INT_LESS_ID);
      result->setCPConnection(cpNodeID, cpPinIndex);
      result->setTestValue(testValue);
      result->setOrEqual(orEqual);
      result->setInvertFlag(false);
      return result;
    }
    else
    {
      MR::TransitConditionDefControlParamIntGreater* result = (MR::TransitConditionDefControlParamIntGreater*) memRes.ptr;
      memRes.increment(memReqs);
      result->setType(TRANSCOND_CONTROL_PARAM_INT_GREATER_ID);
      result->setCPConnection(cpNodeID, cpPinIndex);
      result->setTestValue(testValue);
      result->setOrEqual(orEqual);
      result->setInvertFlag(false);
      return result;
    }
  }
  else if(dataType == "uint")
  {
    uint32_t testValue;
    condDataBlock->readUInt(testValue, "TestValue");

    if (lessThan)
    {
      MR::TransitConditionDefControlParamUIntLess* result = (MR::TransitConditionDefControlParamUIntLess*) memRes.ptr;
      memRes.increment(memReqs);
      result->setType(TRANSCOND_CONTROL_PARAM_UINT_LESS_ID);
      result->setCPConnection(cpNodeID, cpPinIndex);
      result->setTestValue(testValue);
      result->setOrEqual(orEqual);
      result->setInvertFlag(false);
      return result;
    }
    else
    {
      MR::TransitConditionDefControlParamUIntGreater* result = (MR::TransitConditionDefControlParamUIntGreater*) memRes.ptr;
      memRes.increment(memReqs);
      result->setType(TRANSCOND_CONTROL_PARAM_UINT_GREATER_ID);
      result->setCPConnection(cpNodeID, cpPinIndex);
      result->setTestValue(testValue);
      result->setOrEqual(orEqual);
      result->setInvertFlag(false);
      return result;
    }
  }

  return 0;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
