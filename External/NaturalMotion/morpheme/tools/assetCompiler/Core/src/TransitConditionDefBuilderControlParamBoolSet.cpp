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
#include "TransitConditionDefBuilderControlParamBoolSet.h"
#include "morpheme/TransitConditions/mrTransitConditionControlParamBoolSet.h"
#include "assetProcessor/NodeBuilderUtils.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefBuilderControlParamBoolSet::getMemoryRequirements(
  const ME::ConditionExport*   NMP_UNUSED(condExport),
  const ME::NetworkDefExport*  NMP_UNUSED(netDefExport),
  AssetProcessor*              NMP_UNUSED(processor))
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(MR::TransitConditionDefControlParamBoolSet), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the TransitCondition from connects xml data.
MR::TransitConditionDef* TransitConditionDefBuilderControlParamBoolSet::init(
  NMP::Memory::Resource&       memRes,
  const ME::ConditionExport*   condExport,
  const ME::NetworkDefExport*  netDefExport,
  AssetProcessor*              processor,
  MR::NodeID                   NMP_UNUSED(sourceNodeID))
{
  const ME::DataBlockExport* condDataBlock = condExport->getDataBlock();

  NMP::Memory::Format memReqs = getMemoryRequirements(condExport, netDefExport, processor);
  memRes.align(memReqs);
  MR::TransitConditionDefControlParamBoolSet* result = (MR::TransitConditionDefControlParamBoolSet*) memRes.ptr;
  memRes.increment(memReqs);
  result->setType(TRANSCOND_CONTROL_PARAM_BOOL_SET_ID);

  // Initialise input control param connection to look for.
  MR::NodeID cpNodeID = MR::INVALID_NODE_ID;
  readNodeID(condDataBlock, "RuntimeNodeID", cpNodeID);
  MR::PinIndex cpPinIndex = MR::CONTROL_PARAMETER_NODE_PIN_0; // Only connects to control parameters [MORPH-9125]
  result->setCPConnection(cpNodeID, cpPinIndex);

  bool testValue;
  condDataBlock->readBool(testValue, "TestValue");
  result->setTestValue(testValue);
 
  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
