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
#if !(defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))
  #error This file is Windows only
#endif // !(defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64))

#include <xmmintrin.h>
#include <emmintrin.h>

namespace NMP
{
  namespace vpu
  {
    typedef __m128 vector4_t;
  }
}