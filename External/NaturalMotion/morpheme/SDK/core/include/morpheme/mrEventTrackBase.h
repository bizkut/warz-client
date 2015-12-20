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
#ifndef MR_EVENT_TRACK_H
#define MR_EVENT_TRACK_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

#define INVALID_EVENT_INDEX 0xFFFFFFFF

//----------------------------------------------------------------------------------------------------------------------
/// \defgroup EventTrackModule Event Tracks.
///
/// Classes and functions handling several different types of event track data.
/// Event track definition data is usually associated with a source animation and each event usually marks a
/// significant point in that animation.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::EventTrackDefBase
/// \brief Base class for all Event Track types.
/// \ingroup EventTrackModule
//----------------------------------------------------------------------------------------------------------------------
class EventTrackDefBase
{
public:
  bool locate();
  bool dislocate();

  /// \brief Event count access.
  uint32_t getNumEvents() const { return m_numEvents; }
  void setNumEvents(uint32_t numEvents) { m_numEvents = numEvents; }

  /// \brief Event type access.
  uint32_t getType() const { return m_type; }
  void setType(uint32_t type) { m_type = type; }

  /// \brief Access to track name.  Note you can't set the track name once the event sequence is created.
  const char* getTrackName() const { return m_name; }

  /// \brief Access to user specified data associated with this track.
  uint32_t getUserData() const { return m_userData; }
  void setUserData(uint32_t userData) { m_userData = userData; }

  /// \brief Access to the track ID.
  uint32_t getTrackID() const { return m_trackID; }
  void setTrackID(uint32_t trackID) { m_trackID = trackID; }

protected:
  EventTrackDefBase() {};
  ~EventTrackDefBase() {};

  uint32_t  m_numEvents;  ///< How many events are recorded in this track.
  uint32_t  m_type;       ///< What type of event track is this. See EventType enum.
  char*     m_name;       ///< Event track name.
  uint32_t  m_userData;   ///< Allows this track to be associated with arbitrary user data.
  uint32_t  m_trackID;    ///< The ID of this track.
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_EVENT_TRACK_H
//----------------------------------------------------------------------------------------------------------------------
