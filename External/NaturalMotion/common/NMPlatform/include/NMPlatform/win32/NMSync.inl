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
#ifndef NMP_SYNC_INL
#define NMP_SYNC_INL

#if !(defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))
  #error Windows NMSync.inl included in non-Windows build!
#endif // !(defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))

#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN
#ifndef _WIN32_WINNT
  #define _WIN32_WINNT 0x0500
#endif // _WIN32_WINNT
#include <windows.h>

//----------------------------------------------------------------------------------------------------------------------
namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \struct NMP::PSMutex
/// \brief
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
struct PSMutex
{
  CRITICAL_SECTION    m_cSection;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::Semaphore
/// \brief
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class Semaphore
{
public:
  bool init(uint16_t value = 1);
  bool term();

  bool tryLock();
  bool waitLock();

  bool unlock();
protected:
  HANDLE m_semaphore;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::Event
/// \brief
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class Event
{
public:
  bool init();
  bool term();

  bool wait(int32_t t = -1);
  bool set();

  HANDLE get() const { return m_event; }

protected:
  HANDLE              m_event;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NMP::Thread
/// \brief
/// \ingroup NaturalMotionPlatform
//----------------------------------------------------------------------------------------------------------------------
class Thread
{
public:
  HANDLE get() { return m_thread; }

private:
  HANDLE m_thread;
};

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
#endif // NMP_SYNC_INL
//----------------------------------------------------------------------------------------------------------------------
