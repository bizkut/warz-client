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
#include "TransitConditionDefBuilderSKDeviation.h"
#include "physics/Nodes/mrTransitConditionSKDeviation.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefBuilderSKDeviation::getMemoryRequirements(
  const ME::ConditionExport*   NMP_UNUSED(condExport),
  const ME::NetworkDefExport*  NMP_UNUSED(netDefExport),
  AssetProcessor*              NMP_UNUSED(processor))
{
  return NMP::Memory::Format(
           NMP::Memory::align(sizeof(MR::TransitConditionDefSKDeviation), NMP_NATURAL_TYPE_ALIGNMENT),
           NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the TransitCondition from connects xml data.
MR::TransitConditionDef* TransitConditionDefBuilderSKDeviation::init(
  NMP::Memory::Resource&       memRes,
  const ME::ConditionExport*   condExport,
  const ME::NetworkDefExport*  netDefExport,
  AssetProcessor*              processor,
  MR::NodeID                   NMP_UNUSED(sourceNodeID))
{
  const ME::DataBlockExport* condDataBlock = condExport->getDataBlock();

  NMP::Memory::Format memReqs = getMemoryRequirements(condExport, netDefExport, processor);
  memRes.align(memReqs);
  MR::TransitConditionDefSKDeviation* result = (MR::TransitConditionDefSKDeviation*) memRes.ptr;
  memRes.increment(memReqs);
  result->setType(TRANSCOND_SK_DEVIATION_ID);
  result->setInvertFlag(false); // TODO: Implement UI.

  // Initialize control param attrib address to look for.
  float deviationTime;
  condDataBlock->readFloat(deviationTime, "DeviationTime");
  result->setDeviationTime(deviationTime);

  float deviationTriggerAmount;
  condDataBlock->readFloat(deviationTriggerAmount, "DeviationTriggerAmount");
  result->setDeviationTriggerAmount(deviationTriggerAmount);

  bool triggerWhenDeviationIsExceeded;
  condDataBlock->readBool(triggerWhenDeviationIsExceeded, "TriggerWhenDeviationIsExceeded");
  result->setTriggerWhenDeviationIsExceeded(triggerWhenDeviationIsExceeded);

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
