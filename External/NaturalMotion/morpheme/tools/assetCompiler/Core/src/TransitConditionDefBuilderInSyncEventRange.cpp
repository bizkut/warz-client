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
#include "TransitConditionDefBuilderInSyncEventRange.h"
#include "morpheme/TransitConditions/mrTransitConditionInSyncEventRange.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefBuilderInSyncEventRange::getMemoryRequirements(
  const ME::ConditionExport*   NMP_UNUSED(condExport),
  const ME::NetworkDefExport*  NMP_UNUSED(netDefExport),
  AssetProcessor*              NMP_UNUSED(processor))
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(MR::TransitConditionDefInSyncEventRange), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the TransitCondition from connects xml data.
MR::TransitConditionDef* TransitConditionDefBuilderInSyncEventRange::init(
  NMP::Memory::Resource&       memRes,
  const ME::ConditionExport*   condExport,
  const ME::NetworkDefExport*  netDefExport,
  AssetProcessor*              processor,
  MR::NodeID                   sourceNodeID)
{
  const ME::DataBlockExport* condDataBlock = condExport->getDataBlock();

  NMP::Memory::Format memReqs = getMemoryRequirements(condExport, netDefExport, processor);
  memRes.align(memReqs);
  MR::TransitConditionDefInSyncEventRange* result = (MR::TransitConditionDefInSyncEventRange*) memRes.ptr;
  memRes.increment(memReqs);

  NMP_VERIFY(
    (condExport->getTypeID() == TRANSCOND_IN_SYNC_EVENT_RANGE_ID) ||
    (condExport->getTypeID() == TRANSCOND_CROSSED_SYNC_EVENT_BOUNDARY_ID) ||
    (condExport->getTypeID() == TRANSCOND_IN_SYNC_EVENT_SEGMENT_ID));
  result->setType(TRANSCOND_IN_SYNC_EVENT_RANGE_ID);

  // Initialise event pos attrib address to look for.
  MR::AttribAddress attribAddress(MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, sourceNodeID, MR::INVALID_NODE_ID, MR::VALID_FRAME_ANY_FRAME, MR::ANIMATION_SET_ANY);
  result->setSourceNodeSyncEventPlaybackPosAttribAddress(attribAddress);

  // Initialise sync event track attrib address to look for.
  attribAddress.m_semantic = MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK;
  result->setSourceNodeSyncEventTrackAttribAddress(attribAddress);

  float eventRangeStart = 0.0f;
  float eventRangeEnd = 0.0f;
  uint32_t eventIndex = 0;
  switch (condExport->getTypeID())
  {
    case TRANSCOND_IN_SYNC_EVENT_RANGE_ID:
      condDataBlock->readFloat(eventRangeStart, "EventRangeStart");
      condDataBlock->readFloat(eventRangeEnd, "EventRangeEnd");
      break;

    case TRANSCOND_CROSSED_SYNC_EVENT_BOUNDARY_ID:
      condDataBlock->readFloat(eventRangeStart, "EventPosition");
      eventRangeEnd = eventRangeStart;
      break;

    case TRANSCOND_IN_SYNC_EVENT_SEGMENT_ID:
      condDataBlock->readUInt(eventIndex, "EventIndex");
      eventRangeStart = (float) eventIndex;
      eventRangeEnd = eventRangeStart + 1.0f;
      break;

    default:
      NMP_VERIFY_FAIL("Unknown transit condition type");
      break;
  }

  result->setEventRangeStart(eventRangeStart);
  result->setEventRangeEnd(eventRangeEnd);
  result->setInvertFlag(false); // TODO: Add UI for this.

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
