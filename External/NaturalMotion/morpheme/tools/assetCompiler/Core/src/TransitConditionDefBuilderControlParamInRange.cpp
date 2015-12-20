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
#include "TransitConditionDefBuilderControlParamInRange.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamFloatInRange.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamIntInRange.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamUIntInRange.h"
#include "assetProcessor/NodeBuilderUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefBuilderControlParamInRange::getMemoryRequirements(
  const ME::ConditionExport*   condExport,
  const ME::NetworkDefExport*  NMP_UNUSED(netDefExport),
  AssetProcessor*              NMP_UNUSED(processor))
{
  const ME::DataBlockExport* condDataBlock = condExport->getDataBlock();

  std::string dataType;
  condDataBlock->readString(dataType, "DataType");
  if(dataType == "float")
  {
    return NMP::Memory::Format(NMP::Memory::align(sizeof(MR::TransitConditionDefControlParamFloatInRange), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else if(dataType == "int")
  {
    return NMP::Memory::Format(NMP::Memory::align(sizeof(MR::TransitConditionDefControlParamIntInRange), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
  }
  else if(dataType == "uint")
  {
    return NMP::Memory::Format(NMP::Memory::align(sizeof(MR::TransitConditionDefControlParamUIntInRange), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
  }

  NMP_VERIFY_MSG(dataType == "float" || dataType == "int" || dataType == "uint", "Invalid data type '%s' specified for condition.", dataType.c_str());
  return NMP::Memory::Format();

}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the TransitCondition from connects xml data.
MR::TransitConditionDef* TransitConditionDefBuilderControlParamInRange::init(
  NMP::Memory::Resource&       memRes,
  const ME::ConditionExport*   condExport,
  const ME::NetworkDefExport*  netDefExport,
  AssetProcessor*              processor,
  MR::NodeID                   NMP_UNUSED(sourceNodeID))
{
  const ME::DataBlockExport* condDataBlock = condExport->getDataBlock();

  NMP::Memory::Format memReqs = getMemoryRequirements(condExport, netDefExport, processor);
  memRes.align(memReqs);

  // Initialise control param connection to look for.
  MR::NodeID cpNodeID = MR::INVALID_NODE_ID;
  readNodeID(condDataBlock, "RuntimeNodeID", cpNodeID);
  MR::PinIndex cpPinIndex = MR::CONTROL_PARAMETER_NODE_PIN_0; // Only connects to control parameters [MORPH-9125]

  bool notInRange;
  condDataBlock->readBool(notInRange, "NotInRange");

  std::string dataType;
  condDataBlock->readString(dataType, "DataType");
  if(dataType == "float")
  {
    MR::TransitConditionDefControlParamFloatInRange* result = (MR::TransitConditionDefControlParamFloatInRange*) memRes.ptr;
    memRes.increment(memReqs);
    result->setType(TRANSCOND_CONTROL_PARAM_FLOAT_IN_RANGE_ID);

    result->setCPConnection(cpNodeID, cpPinIndex);

    float testValue;
    condDataBlock->readFloat(testValue, "LowerTestValue");
    result->setLowerTestValue(testValue);
    condDataBlock->readFloat(testValue, "UpperTestValue");
    result->setUpperTestValue(testValue);

    result->setInvertFlag(notInRange);
    return result;
  }
  else if(dataType == "int")
  {
    MR::TransitConditionDefControlParamIntInRange* result = (MR::TransitConditionDefControlParamIntInRange*) memRes.ptr;
    memRes.increment(memReqs);
    result->setType(TRANSCOND_CONTROL_PARAM_INT_IN_RANGE_ID);

    result->setCPConnection(cpNodeID, cpPinIndex);

    int32_t testValue;
    condDataBlock->readInt(testValue, "LowerTestValue");
    result->setLowerTestValue(testValue);
    condDataBlock->readInt(testValue, "UpperTestValue");
    result->setUpperTestValue(testValue);

    result->setInvertFlag(notInRange);
    return result;
  }
  else if(dataType == "uint")
  {
    MR::TransitConditionDefControlParamUIntInRange* result = (MR::TransitConditionDefControlParamUIntInRange*) memRes.ptr;
    memRes.increment(memReqs);
    result->setType(TRANSCOND_CONTROL_PARAM_UINT_IN_RANGE_ID);

    result->setCPConnection(cpNodeID, cpPinIndex);

    uint32_t testValue;
    condDataBlock->readUInt(testValue, "LowerTestValue");
    result->setLowerTestValue(testValue);
    condDataBlock->readUInt(testValue, "UpperTestValue");
    result->setUpperTestValue(testValue);

    result->setInvertFlag(notInRange);
    return result;
  }

  return 0;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
