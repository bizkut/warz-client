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
#include "TransitConditionDefBuilderInDurationEvent.h"
#include "morpheme/TransitConditions/mrTransitConditionInDurationEvent.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefBuilderInDurationEvent::getMemoryRequirements(
  const ME::ConditionExport*  NMP_UNUSED(condExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(MR::TransitConditionDefInDurationEvent), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the TransitCondition from connects xml data.
MR::TransitConditionDef* TransitConditionDefBuilderInDurationEvent::init(
  NMP::Memory::Resource&      memRes,
  const ME::ConditionExport*  condExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor,
  MR::NodeID                  sourceNodeID)
{
  const ME::DataBlockExport* condDataBlock = condExport->getDataBlock();

  NMP::Memory::Format memReqs = getMemoryRequirements(condExport, netDefExport, processor);
  memRes.align(memReqs);
  MR::TransitConditionDefInDurationEvent* result = (MR::TransitConditionDefInDurationEvent*) memRes.ptr;
  memRes.increment(memReqs);
  result->setType(TRANSCOND_IN_DURATION_EVENT_ID);

  // Initialise sampled events buffer attrib address to look for.
  MR::AttribAddress sourceNodeSampledEventsBufferAttribAddress(MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, sourceNodeID, MR::INVALID_NODE_ID, MR::VALID_FRAME_ANY_FRAME, MR::ANIMATION_SET_ANY);
  result->setSourceNodeSampledEventsAttribAddress(sourceNodeSampledEventsBufferAttribAddress);

  bool useEventTrackUserData;
  condDataBlock->readBool(useEventTrackUserData, "UseEventTrackUserTypeID");

  int32_t eventTrackUserData;
  condDataBlock->readInt(eventTrackUserData, "EventTrackUserTypeID");
  if (!useEventTrackUserData)
    eventTrackUserData = MR::USER_DATA_UNSPECIFIED;
  result->setEventTrackUserData(eventTrackUserData);

  bool useEventUserData;
  condDataBlock->readBool(useEventUserData, "UseEventUserTypeID");

  int32_t eventUserData;
  condDataBlock->readInt(eventUserData, "EventUserTypeID");
  if (!useEventUserData)
    eventUserData = MR::USER_DATA_UNSPECIFIED;
  result->setEventUserData(eventUserData);

  bool onNotSet = false;
  condDataBlock->readBool(onNotSet, "OnNotSet");
  result->setInvertFlag(onNotSet);

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
