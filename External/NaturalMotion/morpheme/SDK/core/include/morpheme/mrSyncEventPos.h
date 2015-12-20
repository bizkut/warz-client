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
#ifndef MR_SYNC_EVENT_POS_H
#define MR_SYNC_EVENT_POS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::SyncEventPos
/// \ingroup CoreModule
///
/// This wraps an event. The integer part  gives the index of the event in
/// the track, the float part gives the fraction between the events
//----------------------------------------------------------------------------------------------------------------------
class SyncEventPos
{
public:
  NM_FORCEINLINE SyncEventPos(float evt = 0) { set(evt); }
  NM_FORCEINLINE SyncEventPos(uint32_t idx, float frac) : m_eventIndex(idx), m_eventFraction(frac) {}

  NM_FORCEINLINE void set(uint32_t idx, float frac);
  NM_FORCEINLINE uint32_t index() const { return m_eventIndex; }
  NM_FORCEINLINE float fraction() const { return m_eventFraction; }
  NM_FORCEINLINE void setIndex(uint32_t idx) { m_eventIndex = idx; }
  NM_FORCEINLINE void setFraction(float frac);
  NM_FORCEINLINE void addIndex(uint32_t idx) { m_eventIndex += idx; }
  NM_FORCEINLINE void addFraction(float frac);
  NM_FORCEINLINE void set(float event);
  NM_FORCEINLINE float get() const;

  NM_INLINE void dislocate();
  NM_INLINE void locate();

protected:
  uint32_t m_eventIndex;
  float    m_eventFraction;
};

//----------------------------------------------------------------------------------------------------------------------
// Event functions.
//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void SyncEventPos::setFraction(float frac)
{
  m_eventFraction = frac;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void SyncEventPos::set(uint32_t idx, float frac)
{
  NMP_ASSERT(frac >= 0.0f && frac <= 1.0f);
  m_eventIndex = idx;
  m_eventFraction = frac;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void SyncEventPos::set(float evt)
{
  NMP_ASSERT(evt >= 0.0f);
  m_eventIndex = (uint32_t) evt;
  m_eventFraction = evt - ((float) m_eventIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void SyncEventPos::addFraction(float frac)
{
  m_eventFraction += frac;
  uint32_t numWholeEvents = (uint32_t) m_eventFraction;
  m_eventIndex += numWholeEvents;
  m_eventFraction -= ((float) numWholeEvents);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE float SyncEventPos::get() const
{
  return ((float) m_eventIndex) + m_eventFraction;
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void SyncEventPos::dislocate()
{
  NMP::endianSwap(m_eventIndex);
  NMP::endianSwap(m_eventFraction);
}

//----------------------------------------------------------------------------------------------------------------------
NM_FORCEINLINE void SyncEventPos::locate()
{
  NMP::endianSwap(m_eventIndex);
  NMP::endianSwap(m_eventFraction);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_SYNC_EVENT_POS_H
//----------------------------------------------------------------------------------------------------------------------
