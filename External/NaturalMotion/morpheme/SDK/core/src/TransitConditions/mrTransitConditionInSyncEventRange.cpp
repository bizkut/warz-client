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
#include "morpheme/TransitConditions/mrTransitConditionInSyncEventRange.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefInSyncEventRange::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefInSyncEventRange* def = (TransitConditionDefInSyncEventRange*)tcDef;
  def->m_sourceNodeSyncEventPlaybackPosAddress.endianSwap();
  def->m_sourceNodeSyncEventTrackAddress.endianSwap();
  NMP::endianSwap(def->m_eventRangeStart);
  NMP::endianSwap(def->m_eventRangeEnd);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefInSyncEventRange::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefInSyncEventRange* def = (TransitConditionDefInSyncEventRange*) tcDef;
  def->m_sourceNodeSyncEventPlaybackPosAddress.endianSwap();
  def->m_sourceNodeSyncEventTrackAddress.endianSwap();
  NMP::endianSwap(def->m_eventRangeStart);
  NMP::endianSwap(def->m_eventRangeEnd);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefInSyncEventRange::instanceGetMemoryRequirements(
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(TransitConditionInSyncEventRange), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefInSyncEventRange::instanceInit(
  TransitConditionDef*   tcDef,
  NMP::Memory::Resource& memRes,
  Network*               net,
  NodeID                 sourceNodeID)
{
  NMP_ASSERT(tcDef);
  TransitConditionInSyncEventRange* result = NULL;
  TransitConditionDefInSyncEventRange* def = static_cast <TransitConditionDefInSyncEventRange*> (tcDef);

  if (memRes.ptr)
  {
    NMP::Memory::Format memReqs = instanceGetMemoryRequirements(tcDef);
    memRes.align(memReqs);
    result = (TransitConditionInSyncEventRange*) memRes.ptr;
    memRes.increment(memReqs);

    result->condSetState(false);
    result->m_type = TRANSCOND_IN_SYNC_EVENT_RANGE_ID;
  }
  else
  {
    // Tell network to keep attrib data around after network update so that we can access it.
    NMP_ASSERT(sourceNodeID != INVALID_NODE_ID);
    AttribAddress address = *(def->getSourceNodeSyncEventPlaybackPosAttribAddress());
    net->addPostUpdateAccessAttrib(sourceNodeID, address.m_semantic, 2);
    address = *(def->getSourceNodeSyncEventTrackAttribAddress());
    net->addPostUpdateAccessAttrib(sourceNodeID, address.m_semantic, 2);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefInSyncEventRange::evaluateCondition(
  float eventRangeStart,
  float eventRangeEnd,
  bool  invertCondition,
  float previousPosition,
  float currentPosition,
  const AttribDataSyncEventTrack* syncEventTrack)
{
  float eventTrackExtent = syncEventTrack ? (float) syncEventTrack->m_syncEventTrack.getNumEvents() : 0.f;

  // positionRangeStart is assumed to be in the range [0, eventTrackExtent]
  // positionRangeEnd is always positionRangeStart - delta.
  // positionRangeEnd can be negative.
  // If it's negative, it means that the animation wrapped around last frame.
  float positionRangeStart = previousPosition;
  float positionRangeEnd = currentPosition;

  // Condition range that wraps off end of event track (eventRangeEnd < eventRangeStart).
  // We move eventRangeStart before eventRangeEnd.
  // eventRangeStart will be negative.
  eventRangeStart  = NMP::floatSelect(eventRangeEnd - eventRangeStart, eventRangeStart, eventRangeStart - eventTrackExtent);

  // Position range that wraps off end of event track (positionRangeStart < 0.f).
  // We move both eventRangeStart and eventRangeEnd backwards.
  // Both will be negative.
  eventRangeStart  = NMP::floatSelect(positionRangeStart, eventRangeStart, eventRangeStart - eventTrackExtent);
  eventRangeEnd  = NMP::floatSelect(positionRangeStart, eventRangeEnd, eventRangeEnd - eventTrackExtent);

  // We do two interval overlap tests.
  // First, in the range [eventRangeStart, eventRangeEnd].
  float maxRangeStart0 = NMP::floatSelect(eventRangeStart - positionRangeStart, eventRangeStart, positionRangeStart);
  float minRangeEnd0 = NMP::floatSelect(positionRangeEnd - eventRangeEnd, eventRangeEnd, positionRangeEnd);

  // Then, in the range [eventRangeStart + eventTrackExtent, eventRangeEnd + eventTrackExtent], but only
  // if eventRangeStart is negative.
  // This guarantees that eventRangeStart is strictly less than eventTrackExtent.
  eventRangeStart = NMP::floatSelect(eventRangeStart, eventRangeStart, eventRangeStart + eventTrackExtent);
  eventRangeEnd = NMP::floatSelect(eventRangeEnd, eventRangeEnd, eventRangeEnd + eventTrackExtent);
  float maxRangeStart1 = NMP::floatSelect(eventRangeStart - positionRangeStart, eventRangeStart, positionRangeStart);
  float minRangeEnd1 = NMP::floatSelect(positionRangeEnd - eventRangeEnd, eventRangeEnd, positionRangeEnd);

  // This should cover all possibilities.
  // The next two lines are equivalent to:
  // condition->m_satisfied = maxRangeStart0 <= minRangeEnd0 || maxRangeStart1 <= minRangeEnd1;
  // This way we only have a single float compare and no branches (on platforms with fsel).
  float inRange = NMP::floatSelect(minRangeEnd0 - maxRangeStart0, 0.0f, minRangeEnd1 - maxRangeStart1);
  bool satisfied = (inRange >= 0.0f);

  // Invert the result if necessary.
  satisfied ^= invertCondition;
  return satisfied;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefInSyncEventRange::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  NMP_ASSERT(tc && tcDef && net);
  TransitConditionInSyncEventRange* condition = (TransitConditionInSyncEventRange*) (tc);
  TransitConditionDefInSyncEventRange* conditionDef = (TransitConditionDefInSyncEventRange*) (tcDef);

  // Get source node sync event playback pos attrib data.
  AttribAddress address = *conditionDef->getSourceNodeSyncEventPlaybackPosAttribAddress();
  address.m_validFrame = net->getCurrentFrameNo() - 1;
  address.m_animSetIndex = net->getActiveAnimSetIndex();
  address.m_owningNodeID = smActiveNodeID;
  AttribDataUpdateSyncEventPlaybackPos* syncPlaybackPos = (AttribDataUpdateSyncEventPlaybackPos*) net->getAttribDataRecurseFilterNodes(address, true);

  condition->m_satisfied = false;
  if (syncPlaybackPos)
  {
    // Perform the test.
    // Get reference to the source nodes sync event track.
    address = *conditionDef->getSourceNodeSyncEventTrackAttribAddress();
    address.m_validFrame = net->getCurrentFrameNo() - 1;
    address.m_animSetIndex = net->getActiveAnimSetIndex();
    address.m_owningNodeID = smActiveNodeID;
    AttribDataSyncEventTrack* syncEventTrack = (AttribDataSyncEventTrack*) net->getAttribDataRecurseFilterNodes(address, true);

    float positionRangeEnd = syncPlaybackPos->m_absPosReal.index() + syncPlaybackPos->m_absPosReal.fraction();
    float delta = syncPlaybackPos->m_deltaPos.fraction();
    float positionRangeStart = positionRangeEnd - delta;

    // Evaluate the transition condition
    condition->m_satisfied = evaluateCondition(
      conditionDef->m_eventRangeStart,
      conditionDef->m_eventRangeEnd,
      conditionDef->m_invert,
      positionRangeStart,
      positionRangeEnd,
      syncEventTrack);
  }

  return condition->m_satisfied;
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDefInSyncEventRange::instanceReset(
  TransitCondition*    tc,
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  NMP_ASSERT(tc);
  TransitConditionInSyncEventRange* condition = static_cast <TransitConditionInSyncEventRange*> (tc);

  condition->m_satisfied = false;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
