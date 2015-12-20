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
#include "NMPlatform/NMSync.h"

#if !(defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))
  #error This NMSync.cpp file is Windows only
#endif // !(defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
bool Mutex::init()
{
  InitializeCriticalSection(&m_psMutex.m_cSection);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Mutex::term()
{
  DeleteCriticalSection(&m_psMutex.m_cSection);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Mutex::tryLock()
{
  if (TryEnterCriticalSection(&m_psMutex.m_cSection))
  {
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool Mutex::waitLock()
{
  EnterCriticalSection(&m_psMutex.m_cSection);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Mutex::unlock()
{
  LeaveCriticalSection(&m_psMutex.m_cSection);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Semaphore::init(uint16_t value)
{
  m_semaphore = CreateSemaphore(0, value, 1, 0);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Semaphore::term()
{
  CloseHandle(m_semaphore);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Semaphore::tryLock()
{
  // MORPH-21303: Win32 implementation.
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Semaphore::waitLock()
{
  WaitForSingleObject(m_semaphore, INFINITE);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Semaphore::unlock()
{
  ReleaseSemaphore(m_semaphore, 1, 0);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Event::init()
{
  m_event = CreateEvent(0, FALSE, FALSE, 0);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Event::term()
{
  CloseHandle(m_event);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Event::wait(int32_t t)
{
  if (WaitForSingleObject(m_event, t < 0 ? INFINITE : t) == WAIT_TIMEOUT)
  {
    // The time out elapsed.
    return false;
  }

  ResetEvent(m_event);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool Event::set()
{
  PulseEvent(m_event);
  return true;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
