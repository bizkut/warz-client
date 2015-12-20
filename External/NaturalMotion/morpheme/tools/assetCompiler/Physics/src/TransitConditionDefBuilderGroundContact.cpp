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
#include "TransitConditionDefBuilderGroundContact.h"
#include "physics/Nodes/mrTransitConditionGroundContact.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefBuilderGroundContact::getMemoryRequirements(
  const ME::ConditionExport*   NMP_UNUSED(condExport),
  const ME::NetworkDefExport*  NMP_UNUSED(netDefExport),
  AssetProcessor*              NMP_UNUSED(processor))
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(MR::TransitConditionDefGroundContact), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the TransitCondition from connects xml data.
MR::TransitConditionDef* TransitConditionDefBuilderGroundContact::init(
  NMP::Memory::Resource&       memRes,
  const ME::ConditionExport*   condExport,
  const ME::NetworkDefExport*  netDefExport,
  AssetProcessor*              processor,
  MR::NodeID                   NMP_UNUSED(sourceNodeID))
{
  const ME::DataBlockExport* condDataBlock = condExport->getDataBlock();

  NMP::Memory::Format memReqs = getMemoryRequirements(condExport, netDefExport, processor);
  memRes.align(memReqs);
  MR::TransitConditionDefGroundContact* result = (MR::TransitConditionDefGroundContact*) memRes.ptr;
  memRes.increment(memReqs);
  result->setType(TRANSCOND_GROUND_CONTACT_ID);

  // Initialize control param attrib address to look for.
  float triggerTime;
  condDataBlock->readFloat(triggerTime, "TriggerTime");
  result->setTriggerTime(triggerTime);
  result->setInvertFlag(false); // TODO: Implement UI.


  bool onGround;
  condDataBlock->readBool(onGround, "OnGround");
  result->setOnGround(onGround);

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
