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
#include "NMPlatform/NMTimer.h"

#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4996)
#endif

#if !(defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))
  #error this file is Windows only
#endif // !(defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))

#include <stdio.h>
#include <windows.h>

//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

uint64_t Timer::sm_frequency = 0;

//----------------------------------------------------------------------------------------------------------------------
uint64_t GetTimerFrequency()
{
  uint64_t result;

  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);
  result = (uint64_t)frequency.QuadPart;

  NMP_ASSERT(result != 0);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint64_t GetCurrentTimerValue()
{
  uint64_t result;

  LARGE_INTEGER frequency;
  QueryPerformanceCounter(&frequency);
  result = (uint64_t)frequency.QuadPart;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void LockThreadAffinityForTiming()
{
  DWORD_PTR processAffinityMask;
  DWORD_PTR systemAffinityMask;

  // get the mask of all available processors this process can run on.
  //
#if defined(NMP_ENABLE_ASSERTS)
  BOOL result =
#endif
    GetProcessAffinityMask(GetCurrentProcess(), &processAffinityMask, &systemAffinityMask);
  NMP_ASSERT(result);

  unsigned long firstSetBit = 0;
  // pick a bit in the process affinity mask to lock the current thread to.
  // on 64-bit platforms DWORD_PTR is uint64_t so we must use _BitScanForward64.
  //
#if defined(NMP_ENABLE_ASSERTS)
  unsigned char found =
#endif
#if defined(NM_HOST_WIN64)
    _BitScanForward64(&firstSetBit, systemAffinityMask);
#else
    _BitScanForward(&firstSetBit, systemAffinityMask);
#endif
  NMP_ASSERT(found != 0);

  // generate the mask to lock the calling thread to the first available cpu.
  //
  DWORD_PTR threadAffinityMask = 1 << firstSetBit;

  // lock the thread so all calls to QueryPerformanceCounter are on the same processor.
  //
#if defined(NMP_ENABLE_ASSERTS)
  DWORD_PTR oldThreadAffinityMask =
#endif
    SetThreadAffinityMask(GetCurrentThread(), threadAffinityMask);
  NMP_ASSERT(oldThreadAffinityMask != 0);
}

//----------------------------------------------------------------------------------------------------------------------
float Timer::getElapsedTimeWithCurrent(const uint64_t& start) const
{
  NMP_ASSERT(sm_frequency != 0);
  if (!m_enabled)
    return 0.0;
  else
  {
    uint64_t endTime = GetCurrentTimerValue();

    float result = float((endTime - start) * 1000.0 / sm_frequency);

    return result;
  }
}

//----------------------------------------------------------------------------------------------------------------------
float Timer::getTime(uint64_t elapsed) const
{
  NMP_ASSERT(sm_frequency != 0);

  return float((elapsed) * 1000.0 / sm_frequency);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMP
//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
