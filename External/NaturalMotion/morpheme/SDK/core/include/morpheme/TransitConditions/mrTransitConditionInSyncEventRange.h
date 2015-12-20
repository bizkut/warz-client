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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_TRANSIT_CONDITION_IN_SYNC_EVENT_RANGE_H
#define MR_TRANSIT_CONDITION_IN_SYNC_EVENT_RANGE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribAddress.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class TransitConditionInSyncEventRange;
class AttribDataSyncEventTrack;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefInSyncEventRange
/// \brief Transition condition definition for TransitConditionCrossedEventBoundary.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
///
/// Becomes true when the playback position lies with the specified event positions (Event index and fraction).
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefInSyncEventRange : public TransitConditionDef
{
public:
  static bool defDislocate(TransitConditionDef* tcDef);
  static bool defLocate(TransitConditionDef* tcDef);
  static NMP::Memory::Format instanceGetMemoryRequirements(TransitConditionDef* tcDef);
  static TransitCondition* instanceInit(
    TransitConditionDef*   tcDef,
    NMP::Memory::Resource& memRes,
    Network*               net,
    NodeID                 sourceNodeID);
  static bool instanceUpdate(
    TransitCondition*    tc,
    TransitConditionDef* tcDef,
    Network*             net,
    NodeID               smActiveNodeID);

  static void instanceReset(TransitCondition* tc, TransitConditionDef* tcDef);

  NM_INLINE const AttribAddress* getSourceNodeSyncEventPlaybackPosAttribAddress() const { return &m_sourceNodeSyncEventPlaybackPosAddress; }
  NM_INLINE void setSourceNodeSyncEventPlaybackPosAttribAddress(AttribAddress& address) { m_sourceNodeSyncEventPlaybackPosAddress = address; }
  NM_INLINE const AttribAddress* getSourceNodeSyncEventTrackAttribAddress() const { return &m_sourceNodeSyncEventTrackAddress; }
  NM_INLINE void setSourceNodeSyncEventTrackAttribAddress(AttribAddress& address) { m_sourceNodeSyncEventTrackAddress = address; }

  NM_INLINE float getEventRangeStart() const { return m_eventRangeStart; }
  NM_INLINE void setEventRangeStart(float eventPosition) { m_eventRangeStart = eventPosition; }
  NM_INLINE float getEventRangeEnd() const { return m_eventRangeEnd; }
  NM_INLINE void setEventRangeEnd(float eventPosition) { m_eventRangeEnd = eventPosition; }

  /// \brief Evaluate the condition against the current event position and the condition state from the last frame.
  static bool evaluateCondition(
    float eventRangeStart,                          ///< IN: Condition definition event range start.
    float eventRangeEnd,                            ///< IN: Condition definition event range end.
    bool  invertCondition,
    float previousPosition,
    float eventPosition,                            ///< IN: The current event position.
    const AttribDataSyncEventTrack* syncEventTrack  ///< IN: The current sync event track.
    );

protected:
  TransitConditionDefInSyncEventRange() { m_type = TRANSCOND_IN_SYNC_EVENT_RANGE_ID; }

  AttribAddress m_sourceNodeSyncEventPlaybackPosAddress; ///< Address of the sync event playback pos attrib of the source node of the transition.
  AttribAddress m_sourceNodeSyncEventTrackAddress;       ///< Address of the sync event track attrib of the source node of the transition.
  float         m_eventRangeStart;                       ///< Event index and fraction.
  float         m_eventRangeEnd;                         ///< Event index and fraction.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionInSyncEventRange
/// \brief Condition satisfied when source Node playback crosses over an event boundary.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
///
/// Becomes true when the specified event position is crossed (Event index and fraction).
/// Can be unset after this if it continues to be monitored.
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionInSyncEventRange : public TransitCondition
{
  friend class TransitConditionDefInSyncEventRange;

protected:
  TransitConditionInSyncEventRange();
  ~TransitConditionInSyncEventRange();
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_IN_SYNC_EVENT_RANGE_H
//----------------------------------------------------------------------------------------------------------------------
