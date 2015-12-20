// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef NM_TIMER_H
#define NM_TIMER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

uint64_t GetTimerFrequency();
uint64_t GetCurrentTimerValue();

/// \brief Locks the current thread to one processor on multicore systems.
/// For timing on pc platforms timers must be locked to one thread to produce accurate timings
/// see http://msdn.microsoft.com/en-us/library/ee417693(v=vs.85).aspx for more details.
/// \note On all platforms other than NM_HOST_WIN32/64 this function does nothing.
void LockThreadAffinityForTiming();

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::Timer
/// \brief
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class Timer
{
public:
  Timer(bool enabled = true);
  ~Timer();

  void  enable(bool enable);
  void  start();
  float stop(); ///< Returns elapsed time in ms from Timer::start.
  void  reset();
  float getTotalTime();

  float stop(const char* outputString); ///< Returns elapsed time in ms from Timer::start and prints it using NMP_MSG
  void currentTime(uint64_t& output);
  bool isEnabled() const;
  void printTotalSectionTime(const char* outputString);

  float getElapsedTimeWithCurrent(const uint64_t& start) const;

  float getElapsedTime(const uint64_t start, const uint64_t end) const;

  float getTime(uint64_t elapsed) const;

private:
  float    m_totalSectionTime;
  uint64_t m_sectionStartTime;
  uint64_t m_sectionEndTime;
  bool     m_enabled;

  static uint64_t sm_frequency;
};

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Timer::Timer(bool enabled)
  : m_enabled(enabled)
{
  if (sm_frequency == 0)
  {
    sm_frequency = GetTimerFrequency();
  }

  if (m_enabled)
  {
    start();
    m_totalSectionTime = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE Timer::~Timer()
{
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Timer::enable(bool enable)
{
  if (enable && !m_enabled)
  {
    m_totalSectionTime = 0;
  }
  m_enabled = enable;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Timer::start()
{
  if (m_enabled)
    m_sectionStartTime = GetCurrentTimerValue();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Timer::stop()
{
  float elapsedTime = 0.0f;
  if (m_enabled)
  {
    m_sectionEndTime = GetCurrentTimerValue();
    elapsedTime = getElapsedTime(m_sectionStartTime, m_sectionEndTime);
    m_totalSectionTime += elapsedTime;
  }
  return elapsedTime;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Timer::stop(const char* outputString)
{
  float elapsedTime = 0.0f;
  if (m_enabled)
  {
    m_sectionEndTime = GetCurrentTimerValue();
    elapsedTime = getElapsedTime(m_sectionStartTime, m_sectionEndTime);

    NMP_MSG("%s took %f ms\n", outputString, elapsedTime);

    m_totalSectionTime += elapsedTime;
  }
  return elapsedTime;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Timer::printTotalSectionTime(const char* outputString)
{
  if (m_enabled)
  {
    NMP_MSG("Total for %s: %f ms\n", outputString, m_totalSectionTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Timer::reset()
{
  if (m_enabled)
  {
    m_sectionEndTime = 0;
    m_totalSectionTime = 0;
    m_sectionStartTime = 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void Timer::currentTime(uint64_t& output)
{
  output = GetCurrentTimerValue();
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Timer::getTotalTime()
{
  return m_totalSectionTime;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool Timer::isEnabled() const
{
  return m_enabled;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float Timer::getElapsedTime(const uint64_t start, const uint64_t end) const
{
  NMP_ASSERT(sm_frequency != 0);
  uint64_t elapsed = (end - start);
  float result = getTime(elapsed);
  return result;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_TIMER_H
//----------------------------------------------------------------------------------------------------------------------
