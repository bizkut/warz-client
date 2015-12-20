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
/// \type ThreadId
/// \ingroup NaturalMotionPlatformAtomic
typedef uint32_t ThreadId;

//----------------------------------------------------------------------------------------------------------------------
/// \func NMP::getCurrentThreadId
/// \ingroup NaturalMotionPlatformAtomic
NM_FORCEINLINE ThreadId getCurrentThreadId()
{
  return GetCurrentThreadId();
}

} // namespace NMP
