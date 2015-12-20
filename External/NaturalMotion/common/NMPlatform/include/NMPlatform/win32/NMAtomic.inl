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
#ifndef WIN32_LEAN_AND_MEAN
  #define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
  #define NOMINMAX
#endif
#include <windows.h>
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

//----------------------------------------------------------------------------------------------------------------------
/// \func NMP::atomicAssign32
/// \ingroup NaturalMotionPlatformAtomic
/// \brief Atomically performs:
/// uint32_t oldvalue = i;
/// i = j;
/// return oldvalue;
NM_FORCEINLINE uint32_t atomicAssign32(uint32_t& i, uint32_t j)
{
  return InterlockedExchange((LONG*)&i, (LONG)j);
}

//----------------------------------------------------------------------------------------------------------------------
/// \func NMP::atomicIncrement32
/// \ingroup NaturalMotionPlatformAtomic
/// \brief Atomically performs:
/// return ++i;
NM_FORCEINLINE uint32_t atomicIncrement32(uint32_t& i)
{
  return (uint32_t)InterlockedIncrement((LONG*)&i);
}

//----------------------------------------------------------------------------------------------------------------------
/// \func NMP::atomicDecrement32
/// \ingroup NaturalMotionPlatformAtomic
/// \brief Atomically performs:
/// return --i;
NM_FORCEINLINE uint32_t atomicDecrement32(uint32_t& i)
{
  return (uint32_t)InterlockedDecrement((LONG*)&i);
}

//----------------------------------------------------------------------------------------------------------------------
/// \func NMP::atomicAdd32
/// \ingroup NaturalMotionPlatformAtomic
/// \brief Atomically performs:
/// return (i += j);
NM_FORCEINLINE uint32_t atomicAdd32(uint32_t& i, uint32_t j)
{
  return InterlockedExchangeAdd((LONG*)&i, (LONG)j) + j;
}

//----------------------------------------------------------------------------------------------------------------------
/// \func NMP::atomicSub32
/// \ingroup NaturalMotionPlatformAtomic
/// \brief Atomically performs:
/// return (i -= j);
NM_FORCEINLINE uint32_t atomicSub32(uint32_t& i, uint32_t j)
{
  return InterlockedExchangeAdd((LONG*)&i, -(LONG)j) - j;
}

//----------------------------------------------------------------------------------------------------------------------
/// \func NMP::atomicCompareAndSwapPtr
/// \ingroup NaturalMotionPlatformAtomic
/// \brief Atomically performs:
/// MemoryBarrier();
/// void *oldValue = dest;
/// if (dest == comp)
/// {
///   dest = newValue;
/// }
/// MemoryBarrier();
/// return oldValue;
/// \remark The size of the arguments depends on the platform.
NM_FORCEINLINE void* atomicCompareAndSwapPtr(void *&dest, void* comp, void* newValue)
{
  /// On Windows, InterlockedXXX functions are full memory barriers.
  /// Read Bruce Dawson's "Lockless Programming Considerations for Xbox 360 and Microsoft Windows"
  /// for details.
  return InterlockedCompareExchangePointer(&dest, newValue, comp);
}

} // namespace NMP
